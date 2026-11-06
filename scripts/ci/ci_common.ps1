<#
ci_common.ps1 - Shared helpers for every script in scripts\ci.
Dot-source it at the top of a script:

    . (Join-Path $PSScriptRoot 'ci_common.ps1')

Functions:
  Resolve-GhCli       -> full path to gh.exe (GH_CLI env > local default > PATH)
  Set-GhToken         -> ensures GH_TOKEN is set (env > credential helper)
  Resolve-Repo <value>-> "owner/repo" (arg > GH_REPO_DEFAULT env > origin URL)
  Resolve-RepoRoot    -> work tree root of the current directory

Design notes:
  - Keep every file in this folder pure ASCII. Windows PowerShell 5.1 and cmd
    read BOM-less files as ANSI, so non-ASCII text breaks parsing.
  - Exit codes must propagate: scripts are meant to be chained in CI or shells.
#>

function Resolve-GhCli
{
    $gh = $env:GH_CLI
    if ([string]::IsNullOrEmpty($gh)) { $gh = 'D:\autotools\gh_windows_amd64\bin\gh.exe' }
    if (-not (Test-Path -LiteralPath $gh))
    {
        $cmd = Get-Command gh.exe -ErrorAction SilentlyContinue
        if ($cmd) { $gh = $cmd.Source }
        else { Write-Host "[ERROR] gh.exe not found: $gh (set GH_CLI to its full path)"; exit 2 }
    }
    return $gh
}

function Set-GhToken
{
    if ([string]::IsNullOrEmpty($env:GH_TOKEN) -and [string]::IsNullOrEmpty($env:GITHUB_TOKEN))
    {
        $tempRoot = $env:TEMP
        if (-not $tempRoot) { $tempRoot = $env:TMPDIR }
        if (-not $tempRoot) { $tempRoot = [System.IO.Path]::GetTempPath() }
        $credInput = Join-Path $tempRoot 'mgui_ci_cred_in.txt'
        [IO.File]::WriteAllText($credInput, "protocol=https`nhost=github.com`n`n")
        $credOut = cmd /c "git credential fill < `"$credInput`""
        Remove-Item -LiteralPath $credInput -Force -ErrorAction SilentlyContinue
        $tok = ($credOut | Where-Object { $_ -like 'password=*' }) -replace '^password=', ''
        if ([string]::IsNullOrEmpty($tok))
        {
            Write-Host '[ERROR] No GitHub token available: run a push once (stores the credential), or set GH_TOKEN.'
            exit 2
        }
        $env:GH_TOKEN = $tok
    }
}

function Resolve-Repo
{
    param([string]$Repo)
    if ([string]::IsNullOrEmpty($Repo)) { $Repo = $env:GH_REPO_DEFAULT }
    if ([string]::IsNullOrEmpty($Repo))
    {
        $url = git remote get-url origin 2>$null
        if ($url -match 'github\.com[:/](.+?)(\.git)?$') { $Repo = $Matches[1] }
        if ([string]::IsNullOrEmpty($Repo))
        {
            Write-Host '[ERROR] Cannot resolve owner/repo: origin is not github.com (set GH_REPO_DEFAULT or pass -Repo).'
            exit 2
        }
    }
    return $Repo
}

function Resolve-RepoRoot
{
    $root = git rev-parse --show-toplevel 2>$null
    if ([string]::IsNullOrEmpty($root))
    {
        Write-Host '[ERROR] not inside a git work tree'
        exit 2
    }
    return $root
}
