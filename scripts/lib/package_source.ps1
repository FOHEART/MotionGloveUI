<#
package_source.ps1 - Build the distributable source ZIP for MotionGloveUI.

Called by scripts\package.bat on Windows (powershell) and on POSIX systems
(pwsh). Keep this file ASCII only: Windows PowerShell 5.1 reads BOM-less
files as ANSI, so non-ASCII comments or strings can break parsing.

Layout rules (see the build/deploy skill notes):
  - Generated artifacts only go to out/, never into the source tree.
  - Internal helpers, CI helpers, WSL helpers and pull/release tools are not
    shipped to customers.
  - Join-Path relative fragments use '/' so the paths work on Linux too.
  - Temp directory: TEMP -> TMPDIR -> GetTempPath().
#>
param(
    [string]$OutputDir = '',
    [string]$Version = ''
)

$ErrorActionPreference = 'Stop'

function Get-Full([string]$rel) {
    return [System.IO.Path]::GetFullPath((Join-Path $repoRoot $rel))
}

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if ([string]::IsNullOrEmpty($OutputDir)) { $OutputDir = Join-Path $repoRoot 'out/pack' }

# Single source of truth for the version: Doxyfile PROJECT_NUMBER.
if ([string]::IsNullOrEmpty($Version)) {
    $doxy = Join-Path $repoRoot 'Doxyfile'
    $hit = Select-String -LiteralPath $doxy -Pattern '^\s*PROJECT_NUMBER\s*=\s*"([^"]+)"' |
        Select-Object -First 1
    if ($hit) { $Version = $hit.Matches[0].Groups[1].Value } else { $Version = '1.0.0' }
}
$name = "MotionGloveUI_v$Version"

$tempRoot = $env:TEMP
if (-not $tempRoot) { $tempRoot = $env:TMPDIR }
if (-not $tempRoot) { $tempRoot = [System.IO.Path]::GetTempPath() }
if (-not $tempRoot) { throw 'Cannot determine temp directory (TEMP/TMPDIR are empty)' }
$stageParent = Join-Path $tempRoot "mgui_pack_$PID"
$stageDir = Join-Path $stageParent $name
if (Test-Path -LiteralPath $stageParent) { Remove-Item -LiteralPath $stageParent -Recurse -Force }
New-Item -ItemType Directory -Path $stageDir -Force | Out-Null
if (-not (Test-Path -LiteralPath $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# Absolute directory exclusions. Match by prefix (equal to, or under), never
# by bare directory name: a same-named business directory must not be skipped.
$excludeAbsolute = @(
    'out', 'lib', '.git', '.github', '.vscode', '.codebuddy',
    'docs/html', 'docs/doxygen', 'scripts/ci', 'scripts/wsl'
) | ForEach-Object { Get-Full $_ }

$excludeDirNames = @('.git', '.github', '.vscode', '.codebuddy', '_deps', 'CMakeFiles')
$excludeFileNames = @(
    'AGENTS.md', '.gitattributes', '.gitignore', '.gitmodules', '.git',
    'CMakeUserPresets.json', 'git_pull_latest.bat', 'release.bat',
    'doxygen_warnings.log'
)
$excludeFilePatterns = @('*.zip', '*.7z', '*.log', '*.user', '*.bak', '*.tmp')

function Test-ExcludedDir([string]$fullPath) {
    foreach ($x in $excludeAbsolute) {
        if ($fullPath -eq $x -or $fullPath.StartsWith($x + [System.IO.Path]::DirectorySeparatorChar)) {
            return $true
        }
    }
    return $false
}

function Copy-TreeWithExcludes {
    foreach ($item in Get-ChildItem -LiteralPath $repoRoot -Recurse -Force) {
        $full = $item.FullName
        if (Test-ExcludedDir $full) { continue }
        $rel = $full.Substring($repoRoot.Length).TrimStart('\', '/')
        $segs = $rel -split '[\\/]'
        $skip = $false
        for ($i = 0; $i -lt $segs.Count - 1; $i++) {
            if ($excludeDirNames -contains $segs[$i]) { $skip = $true; break }
        }
        if ($skip) { continue }
        if ($item.PSIsContainer) {
            if ($excludeDirNames -contains $item.Name) { continue }
            New-Item -ItemType Directory -Path (Join-Path $stageDir $rel) -Force | Out-Null
            continue
        }
        if ($excludeFileNames -contains $item.Name) { continue }
        foreach ($p in $excludeFilePatterns) {
            if ($item.Name -like $p) { $skip = $true; break }
        }
        if ($skip) { continue }
        $dest = Join-Path $stageDir $rel
        $destDir = Split-Path -Parent $dest
        if (-not (Test-Path -LiteralPath $destDir)) {
            New-Item -ItemType Directory -Path $destDir -Force | Out-Null
        }
        Copy-Item -LiteralPath $full -Destination $dest -Force
    }
}

$robocopy = Get-Command robocopy -ErrorAction SilentlyContinue
if ($robocopy) {
    $robArgs = @($repoRoot, $stageDir, '/E', '/NFL', '/NDL', '/NJH', '/NJS', '/NP')
    $robArgs += '/XD'
    $robArgs += $excludeAbsolute
    $robArgs += '/XF'
    $robArgs += $excludeFileNames
    $robArgs += $excludeFilePatterns
    & robocopy @robArgs | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "Copy failed (robocopy exit code $LASTEXITCODE)" }
} else {
    Write-Host '  robocopy not found (non-Windows), using PowerShell copy'
    Copy-TreeWithExcludes
}

# Leak check: a customer package must not contain repository tooling.
$badNames = Get-ChildItem -LiteralPath $stageDir -Recurse -Force |
    Where-Object { $_.Name -like '.git*' -or $_.Name -like '*git*' }
if ($badNames) {
    throw ('Repository-only file leaked into the package: ' + ($badNames[0].FullName))
}
$scriptExt = @('.bat', '.cmd', '.ps1', '.sh')
$scriptFiles = Get-ChildItem -LiteralPath $stageDir -Recurse -File |
    Where-Object { $scriptExt -contains $_.Extension.ToLowerInvariant() }
foreach ($s in $scriptFiles) {
    # 'gi[t]' instead of a literal 'git ': the check itself must not match its own source.
    $leak = Select-String -LiteralPath $s.FullName -Pattern 'gi[t]\s+(fetch|pull|clone|push|remote|checkout|submodule)' -ErrorAction SilentlyContinue
    if ($leak) { throw ("Internal tooling leaked into the package: " + $s.FullName) }
}
$readme = Join-Path $stageDir 'README.md'
if (Test-Path -LiteralPath $readme) {
    $readmeLeak = Select-String -LiteralPath $readme -Pattern 'gi[t] (clone|pull|tag)|submodule|\.github/'
    if ($readmeLeak) { throw 'README contains internal-only sections' }
}

$zipPath = Join-Path $OutputDir "$name.zip"
if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }

$isWindowsPs = (-not $PSVersionTable.Platform) -or ($PSVersionTable.Platform -eq 'Win32NT')
$sevenZip = $null
if ($isWindowsPs) {
    $candidate = Get-Command 7z.exe -ErrorAction SilentlyContinue
    if ($candidate) { $sevenZip = $candidate.Source }
    if (-not $sevenZip -and (Test-Path -LiteralPath 'D:\autotools\7z\7za.exe')) {
        $sevenZip = 'D:\autotools\7z\7za.exe'
    }
    if (-not $sevenZip -and $env:ProgramFiles) {
        $fallback = Join-Path $env:ProgramFiles '7-Zip/7z.exe'
        if (Test-Path -LiteralPath $fallback) { $sevenZip = $fallback }
    }
} else {
    $candidate = Get-Command 7z -ErrorAction SilentlyContinue
    if ($candidate) { $sevenZip = $candidate.Source }
}

if ($sevenZip) {
    Push-Location $stageParent
    try {
        & $sevenZip a -tzip $zipPath $name | Out-Null
        if ($LASTEXITCODE -ne 0) { throw "Compression failed (7z exit code $LASTEXITCODE)" }
    } finally {
        Pop-Location
    }
} else {
    Compress-Archive -Path $stageDir -DestinationPath $zipPath -Force
}

Remove-Item -LiteralPath $stageParent -Recurse -Force
Write-Host "[OK] $zipPath"
exit 0
