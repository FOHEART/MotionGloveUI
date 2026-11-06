@echo off
REM pull_artifacts.bat - download workflow-run artifacts (test screenshots etc.).
REM Examples:
REM   scripts\ci\pull_artifacts.bat                        latest run, all artifacts
REM   scripts\ci\pull_artifacts.bat -RunId 123456          given run
REM   scripts\ci\pull_artifacts.bat -Name ui-artifacts-*   only matching artifacts
REM   scripts\ci\pull_artifacts.bat -Dest D:\tmp\art       custom output directory
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0pull_artifacts.ps1" %*
exit /b %ERRORLEVEL%
