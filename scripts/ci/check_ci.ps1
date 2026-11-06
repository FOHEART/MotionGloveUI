<#
check_ci.ps1 - Query / wait for GitHub Actions CI/CD results.

Usage:
  scripts\ci\check_ci.bat                     list recent workflow runs
  scripts\ci\check_ci.bat -Watch              wait for the latest run to finish
  scripts\ci\check_ci.bat -Watch -Workflow cmake-multi-platform.yml
  scripts\ci\check_ci.bat -RunId 42160538216  failed-log summary of one run
  scripts\ci\check_ci.bat -RunId <id> -Watch  wait for that specific run

Auth / gh path / repo resolution: see ci_common.ps1.
#>

param(
    [string]$Repo = '',
    [string]$Workflow = '',
    [int]$Limit = 6,
    [string]$RunId = '',
    [switch]$Watch
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'ci_common.ps1')

$Repo = Resolve-Repo $Repo
Set-GhToken
$gh = Resolve-GhCli

if (-not [string]::IsNullOrEmpty($RunId))
{
    if ($Watch)
    {
        & $gh run watch $RunId --repo $Repo --exit-status --interval 30
        $code = $LASTEXITCODE
        Write-Host ($(if ($code -eq 0) { "[OK] run $RunId success" } else { "[FAIL] run $RunId failed" }))
        exit $code
    }
    & $gh run view $RunId --repo $Repo --log-failed 2>&1 | Select-Object -First 60
    exit $LASTEXITCODE
}

$listArgs = @('run', 'list', '--repo', $Repo, '--limit', "$Limit")
if (-not [string]::IsNullOrEmpty($Workflow)) { $listArgs += @('--workflow', $Workflow) }

if ($Watch)
{
    $latest = & $gh @listArgs --json databaseId --jq '.[0].databaseId'
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrEmpty($latest)) { Write-Host '[ERROR] No run to wait for.'; exit 1 }
    Write-Host "[INFO] Watching run $latest ..."
    & $gh run watch $latest --repo $Repo --exit-status --interval 30
    $code = $LASTEXITCODE
    Write-Host ($(if ($code -eq 0) { "[OK] run $latest success" } else { "[FAIL] run $latest failed" }))
    & $gh @listArgs
    exit $code
}

& $gh @listArgs
exit $LASTEXITCODE
