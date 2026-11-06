<#
rerun_failed.ps1 - Re-run only the failed jobs of a GitHub Actions run.

Usage:
  scripts\ci\rerun_failed.bat                 re-run failed jobs of the latest run
  scripts\ci\rerun_failed.bat -RunId 123456   re-run failed jobs of a given run
  scripts\ci\rerun_failed.bat -DryRun         only print what would happen

Notes:
  - Works only for runs in a terminal state; a successful run is reported and
    left untouched.
  - Needs write access: the credential must have the workflow/repo scope.
#>

param(
    [string]$Repo = '',
    [string]$RunId = '',
    [switch]$DryRun
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

$info = (& $gh run view $RunId --repo $Repo --json status,conclusion) | ConvertFrom-Json
Write-Host ("[INFO] run {0}: status={1} conclusion={2}" -f $RunId, $info.status, $info.conclusion)

if ($info.status -ne 'completed')
{
    Write-Host '[ERROR] Run is still in progress; wait for it to finish first.'
    exit 1
}
if ($info.conclusion -eq 'success')
{
    Write-Host '[OK] Run already succeeded; nothing to re-run.'
    exit 0
}

if ($DryRun)
{
    Write-Host "[DRY] would run: gh run rerun $RunId --failed --repo $Repo"
    exit 0
}

& $gh run rerun $RunId --repo $Repo --failed
exit $LASTEXITCODE
