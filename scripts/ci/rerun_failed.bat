@echo off
REM rerun_failed.bat - re-run only the failed jobs of a GitHub Actions run.
REM Examples:
REM   scripts\ci\rerun_failed.bat                 latest run
REM   scripts\ci\rerun_failed.bat -RunId 123456   given run
REM   scripts\ci\rerun_failed.bat -DryRun         print what would happen
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0rerun_failed.ps1" %*
exit /b %ERRORLEVEL%
