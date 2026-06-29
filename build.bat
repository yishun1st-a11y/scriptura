@echo off
setlocal enabledelayedexpansion

REM Windows build script for Scriptura
REM Usage: build.bat [Debug^|Release] [--deploy]

set BUILD_TYPE=%~1
set DEPLOY=false

REM Check for --deploy flag
if "%2"=="--deploy" set DEPLOY=true
if "%BUILD_TYPE%"=="--deploy" (
    set BUILD_TYPE=Debug
    set DEPLOY=true
)

if "%BUILD_TYPE%"=="" set BUILD_TYPE=Debug
set BUILD_DIR=cmake-build-%BUILD_TYPE%

echo === Building Scriptura (%BUILD_TYPE%) ===
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    pause
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    cmake -B "%BUILD_DIR%" -S . -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
)

REM Build the project
echo Building project...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE% -j

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build complete. Run with: %BUILD_DIR%\scriptura.exe

REM Deploy if requested
if "%DEPLOY%"=="true" (
    echo.
    echo === Deploying with Qt dependencies ===
    echo.
    call deploy-windows.bat
) else (
    echo.
    echo To deploy with Qt dependencies, run:
    echo   build.bat %BUILD_TYPE% --deploy
    echo.
    echo Or use the deploy script directly:
    echo   deploy-windows.bat
    echo.
    pause
)
