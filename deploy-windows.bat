@echo off
setlocal enabledelayedexpansion

REM Windows deployment script for Scriptura
REM This script builds the project and bundles all Qt DLLs using windeployqt

set BUILD_DIR=build
set DEPLOY_DIR=deploy

echo === Scriptura Windows Deployment ===
echo.

REM Check if windeployqt is available, try to locate it if not in PATH
where windeployqt >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo windeployqt not found in PATH, attempting to locate it...
    REM Try QT_BIN_DIR environment variable (set by install-qt-action)
    if defined QT_BIN_DIR (
        if exist "%QT_BIN_DIR%\windeployqt.exe" (
            echo Found windeployqt at: %QT_BIN_DIR%\windeployqt.exe
            set PATH=%QT_BIN_DIR%;%PATH%
            goto windeployqt_found
        )
    )
    REM Try Qt6_DIR environment variable (set by install-qt-action)
    if defined Qt6_DIR (
        for /f "delims=" %%i in ('dir /b /s "%Qt6_DIR%\..\..\bin\windeployqt.exe" 2^>nul') do set QT_BIN_DIR=%%i
        if defined QT_BIN_DIR (
            for %%a in ("%QT_BIN_DIR%") do set QT_BIN_DIR=%%~dpa
            echo Found windeployqt at: %QT_BIN_DIR%
            set PATH=%QT_BIN_DIR%;%PATH%
            goto windeployqt_found
        )
    )
    REM Try to find Qt bin directory using qt6-qmake
    where qt6-qmake >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        for /f "delims=" %%i in ('qt6-qmake -query QT_INSTALL_BINS') do set QT_BIN_DIR=%%i
        if exist "%QT_BIN_DIR%\windeployqt.exe" (
            echo Found windeployqt at: %QT_BIN_DIR%\windeployqt.exe
            set PATH=%QT_BIN_DIR%;%PATH%
            goto windeployqt_found
        )
    )
    REM Try qmake as fallback
    where qmake >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        for /f "delims=" %%i in ('qmake -query QT_INSTALL_BINS') do set QT_BIN_DIR=%%i
        if exist "%QT_BIN_DIR%\windeployqt.exe" (
            echo Found windeployqt at: %QT_BIN_DIR%\windeployqt.exe
            set PATH=%QT_BIN_DIR%;%PATH%
            goto windeployqt_found
        )
    )
    REM Final check
    where windeployqt >nul 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: windeployqt not found
        echo Please ensure Qt is installed and windeployqt is in your PATH
        exit /b 1
    )
)
:windeployqt_found

REM Determine executable path (handles both single-config and multi-config generators)
set EXE_PATH=
if exist "%BUILD_DIR%\Release\scriptura.exe" (
    set EXE_PATH=%BUILD_DIR%\Release\scriptura.exe
) else if exist "%BUILD_DIR%\scriptura.exe" (
    set EXE_PATH=%BUILD_DIR%\scriptura.exe
) else (
    echo ERROR: scriptura.exe not found in build directory
    echo Build directory contents:
    dir "%BUILD_DIR%"
    exit /b 1
)

echo Found executable: %EXE_PATH%

REM Create deployment directory
echo.
echo Creating deployment directory...
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%"

REM Copy the executable
echo Copying executable...
copy /Y "%EXE_PATH%" "%DEPLOY_DIR%\"

REM Run windeployqt to bundle Qt DLLs
echo.
echo Deploying Qt dependencies...
windeployqt --release --dir "%DEPLOY_DIR%" "%EXE_PATH%"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: windeployqt failed
    exit /b 1
)

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
echo Executable and dependencies are in: %DEPLOY_DIR%\
echo Distribution archive: scriptura-windows.zip
echo.
