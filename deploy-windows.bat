@echo off
setlocal EnableDelayedExpansion

REM Windows deployment script for Scriptura
REM This script builds the project and packages the executable

set BUILD_DIR=build
set DEPLOY_DIR=deploy

echo === Scriptura Windows Deployment ===
echo.

REM Determine executable path (handles both single-config and multi-config generators)
set EXE_PATH=
if exist "!BUILD_DIR!\Release\scriptura.exe" (
    set "EXE_PATH=!BUILD_DIR!\Release\scriptura.exe"
) else if exist "!BUILD_DIR!\scriptura.exe" (
    set "EXE_PATH=!BUILD_DIR!\scriptura.exe"
) else (
    echo ERROR: scriptura.exe not found in build directory
    echo Build directory contents:
    dir "!BUILD_DIR!"
    exit /b 1
)

echo Found executable: !EXE_PATH!

REM Create deployment directory
echo.
echo Creating deployment directory...
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%"

REM Copy the executable
echo Copying executable...
copy /Y "!EXE_PATH!" "!DEPLOY_DIR!\"

REM Copy additional resources
echo.
echo Copying additional resources...
if exist "icon.png" copy /Y "icon.png" "%DEPLOY_DIR%\"
if exist "scriptura.desktop" copy /Y "scriptura.desktop" "%DEPLOY_DIR%\"
if exist "README.md" copy /Y "README.md" "%DEPLOY_DIR%\"

REM Create a ZIP archive
echo.
echo Creating ZIP archive...
powershell -Command "Compress-Archive -Path '%DEPLOY_DIR%\*' -DestinationPath 'scriptura-windows.zip' -Force"

echo.
echo === Deployment Complete ===
echo Executable is in: %DEPLOY_DIR%\
echo Distribution archive: scriptura-windows.zip
echo.