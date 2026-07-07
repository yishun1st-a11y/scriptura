@echo off
setlocal enabledelayedexpansion

REM Windows packaging script for Scriptura
REM This script creates a complete Windows installer with all dependencies bundled
REM Usage: package-windows.bat [Debug^|Release]

set BUILD_TYPE=%~1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release
set BUILD_DIR=cmake-build-%BUILD_TYPE%
set DEPLOY_DIR=deploy\%BUILD_TYPE%
set PACKAGE_DIR=package

echo === Packaging Scriptura for Windows (%BUILD_TYPE%) ===
echo.

REM Check if deploy-windows.bat has been run
if not exist "%DEPLOY_DIR%\scriptura.exe" (
    echo ERROR: Deployment directory not found. Please run deploy-windows.bat first.
    pause
    exit /b 1
)

REM Check if NSIS is available
where makensis >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: NSIS (makensis) not found in PATH
    echo Please install NSIS from https://nsis.sourceforge.io/
    pause
    exit /b 1
)

REM Create package directory
echo Creating package directory...
if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"

REM Copy deployment files to package directory
echo Copying deployment files...
xcopy /E /I /Y "%DEPLOY_DIR%\*" "%PACKAGE_DIR%\"

REM Copy icon if available
if exist "icon.ico" (
    echo Copying icon...
    copy /Y "icon.ico" "%PACKAGE_DIR%\"
)

REM Generate version from git tag if available
for /f "tokens=*" %%a in ('git describe --tags --always 2^>nul') do set VERSION=%%a
if "%VERSION%"=="" set VERSION=0.0.0-dev

REM Update NSIS script with version
echo Updating NSIS script with version: %VERSION%
powershell -Command "(Get-Content 'scriptura.nsi') -replace '0\.0\.0-dev', '%VERSION%' | Set-Content 'scriptura-temp.nsi'"

REM Build the installer
echo.
echo Building NSIS installer...
makensis scriptura-temp.nsi

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: NSIS installer build failed
    del scriptura-temp.nsi
    pause
    exit /b 1
)

REM Clean up temp file
del scriptura-temp.nsi

echo.
echo === Packaging complete ===
echo Installer: Scriptura-Setup.exe
echo.
echo The installer is ready for distribution.
echo.
pause
