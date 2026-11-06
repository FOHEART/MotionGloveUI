@echo off
REM check_ci.bat - query / wait for GitHub Actions CI/CD results.
REM Implementation lives in check_ci.ps1 (same folder). Examples:
REM   scripts\ci\check_ci.bat                    list recent runs
REM   scripts\ci\check_ci.bat -Watch             wait for the latest run to finish
REM   scripts\ci\check_ci.bat -RunId 123456      failed-log summary of one run
REM   scripts\ci\check_ci.bat -Watch -Workflow cmake-multi-platform.yml
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0check_ci.ps1" %*
exit /b %ERRORLEVEL%
