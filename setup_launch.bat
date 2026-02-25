@echo off
setlocal

:: Resolve repo root relative to this script's location
set "REPO_DIR=%~dp0"
set "VS_DIR=%REPO_DIR%.vs"
set "LAUNCH_FILE=%VS_DIR%\launch.vs.json"

:: Accept path as argument or prompt
if "%~1"=="" (
    set /p "SNIRF_PATH=Enter full path to .snirf file: "
) else (
    set "SNIRF_PATH=%~1"
)

:: Escape backslashes for JSON
set "ESCAPED=%SNIRF_PATH:\=\\%"

:: Create .vs directory if it doesn't exist
if not exist "%VS_DIR%" mkdir "%VS_DIR%"

:: Write launch.vs.json
(
echo {
echo   "version": "0.2.1",
echo   "defaults": {},
echo   "configurations": [
echo     {
echo       "type": "default",
echo       "project": "CMakeLists.txt",
echo       "projectTarget": "snirf_dev.exe",
echo       "name": "snirf_dev.exe",
echo       "args": [ "%ESCAPED%" ]
echo     }
echo   ]
echo }
) > "%LAUNCH_FILE%"

echo.
echo Created: %LAUNCH_FILE%
echo   args: %SNIRF_PATH%
endlocal
