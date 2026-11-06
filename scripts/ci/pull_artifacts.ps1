<#
pull_artifacts.ps1 - Download workflow-run artifacts (test screenshots etc.).

Usage:
  scripts\ci\pull_artifacts.bat                          latest run, all artifacts
  scripts\ci\pull_artifacts.bat -RunId 123456            given run, all artifacts
  scripts\ci\pull_artifacts.bat -Name ui-artifacts-*     only matching artifacts
  scripts\ci\pull_artifacts.bat -Dest <dir>              custom output directory

Default destination: <repo-root>/out/ci-artifacts. The directory is wiped on
each call because gh run download refuses to overwrite existing files.
#>

param(
    [string]$Repo = '',
    [string]$RunId = '',
    [string]$Name = '',
    [string]$Dest = ''
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'ci_common.ps1')

$Repo = Resolve-Repo $Repo
Set-GhToken
$gh = Resolve-GhCli

if ([string]::IsNullOrEmpty($RunId))
{
    $RunId = & $gh run list --repo $Repo --limit 1 --json databaseId --jq '.[0].databaseId'
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrEmpty($RunId)) { Write-Host '[ERROR] No runs found.'; exit 1 }
}

if ([string]::IsNullOrEmpty($Dest))
{
    $Dest = Join-Path (Resolve-RepoRoot) 'out/ci-artifacts'
}

if (Test-Path -LiteralPath $Dest)
{
    Write-Host "[INFO] Cleaning existing $Dest"
    Remove-Item -LiteralPath $Dest -Recurse -Force
}

$dlArgs = @('run', 'download', $RunId, '--repo', $Repo, '--dir', $Dest)
if (-not [string]::IsNullOrEmpty($Name)) { $dlArgs += @('--name', $Name) }

Write-Host "[INFO] Downloading artifacts of run $RunId into $Dest"
& $gh @dlArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Get-ChildItem -LiteralPath $Dest -Directory | ForEach-Object { Write-Host ("  {0}" -f $_.Name) }
exit 0
