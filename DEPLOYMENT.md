# Deployment Guide

This guide explains how to build and deploy Scriptura with all Qt dependencies bundled correctly for Windows, Linux, and macOS.

## Prerequisites

### All Platforms
- CMake 3.16 or higher
- Qt 5 or Qt 6 (with Widgets, Network, and LinguistTools components)
- C++17 compatible compiler

### Windows
- Visual Studio or MinGW compiler
- Qt installed with `windeployqt` tool
- PowerShell (for ZIP creation)

### Linux
- GCC or Clang compiler
- Qt development packages
- Optional: `linuxdeployqt` for AppImage creation
  - Download from: https://github.com/probonopd/linuxdeployqt

### macOS
- Xcode command line tools
- Qt installed with `macdeployqt` tool
- Xcode command line tools (for `hdiutil`)

## Quick Start

### Windows
```batch
deploy-windows.bat
```

### Linux
```bash
./deploy-linux.sh
```

### macOS
```bash
./deploy-macos.sh
```

## Manual Build and Deploy

### 1. Build the Project

#### Debug Build
```bash
# Linux/macOS
./build.sh

# Windows
build.bat
```

#### Release Build
```bash
# Linux/macOS
mkdir -p cmake-build-release
cd cmake-build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Windows (Visual Studio)
mkdir cmake-build-release
cd cmake-build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Windows (MinGW)
mkdir cmake-build-release
cd cmake-build-release
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j$(nproc)
```

### 2. Deploy Qt Dependencies

#### Windows - Using windeployqt
```batch
# Navigate to build directory
cd cmake-build-release

# Run windeployqt
windeployqt --release scriptura.exe

# This will copy all required Qt DLLs to the same directory as the executable
```

**What windeployqt does:**
- Copies Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll, Qt5Network.dll (or Qt6 equivalents)
- Copies required system DLLs (MSVC runtime, etc.)
- Copies Qt plugins (platforms, styles, etc.)
- Copies Qt translations

#### Linux - Manual Deployment
```bash
# Find Qt library paths
QT_LIB_DIR=$(qmake -query QT_INSTALL_LIBS)
QT_PLUGIN_DIR=$(qmake -query QT_INSTALL_PLUGINS)

# Create deployment directory
mkdir -p deploy
cp cmake-build-release/scriptura deploy/

# Copy Qt libraries
cd deploy
cp -P "$QT_LIB_DIR"/libQt6Core.so* .
cp -P "$QT_LIB_DIR"/libQt6Widgets.so* .
cp -P "$QT_LIB_DIR"/libQt6Network.so* .
cp -P "$QT_LIB_DIR"/libQt6Gui.so* .

# Copy platform plugins
mkdir -p platforms
cp -r "$QT_PLUGIN_DIR/platforms/libqxcb.so" platforms/
```

**Alternative: Using linuxdeployqt**
```bash
cd deploy
linuxdeployqt scriptura -appimage
```

#### macOS - Using macdeployqt
```bash
# Navigate to build directory
cd cmake-build-release

# Run macdeployqt on the app bundle
macdeployqt Scriptura.app -always-overwrite

# This will:
# - Copy Qt frameworks into the app bundle
# - Fix library paths
# - Add required plugins
```

### 3. Verify Deployment

#### Windows
```batch
# Test the executable
deploy\scriptura.exe

# Check for missing DLLs with Dependency Walker or
# use the built-in Windows tool:
ldd scriptura.exe
```

#### Linux
```bash
# Test the executable
cd deploy
LD_LIBRARY_PATH=. ./scriptura

# Check for missing libraries
ldd scriptura | grep "not found"
```

#### macOS
```bash
# Test the app
open deploy/Scriptura.app

# Check for missing frameworks
otool -L deploy/Scriptura.app/Contents/MacOS/scriptura
```

## Directory Structure After Deployment

### Windows
```
deploy/
├── scriptura.exe
├── Qt6Core.dll
├── Qt6Gui.dll
├── Qt6Widgets.dll
├── Qt6Network.dll
├── platforms/
│   └── qwindows.dll
├── styles/
│   └── qwindowsvistastyle.dll
└── icon.png
```

### Linux
```
deploy/
├── scriptura
├── libQt6Core.so.6
├── libQt6Widgets.so.6
├── libQt6Network.so.6
├── libQt6Gui.so.6
├── platforms/
│   └── libqxcb.so
├── AppRun (if using AppImage)
└── icon.png
```

### macOS
```
Scriptura.app/
├── Contents/
│   ├── MacOS/
│   │   └── scriptura
│   ├── Frameworks/
│   │   ├── QtCore.framework
│   │   ├── QtGui.framework
│   │   ├── QtWidgets.framework
│   │   └── QtNetwork.framework
│   ├── PlugIns/
│   │   └── platforms/
│   │       └── libqcocoa.dylib
│   └── Resources/
│       └── icon.png
```

## Common Issues and Solutions

### Windows
**Issue: "MSVCP140.dll not found"**
- Solution: Install Visual C++ Redistributable or use windeployqt with `--no-compiler-runtime` flag

**Issue: "This application failed to start because no Qt platform plugin could be initialized"**
- Solution: Ensure `platforms/qwindows.dll` is in the correct location

### Linux
**Issue: "error while loading shared libraries: libQt6Core.so.6: cannot open shared object file"**
- Solution: Set `LD_LIBRARY_PATH` or use `patchelf` to set RPATH

**Issue: "Could not load the Qt platform plugin "xcb""**
- Solution: Install `libxcb-xinerama0` or copy the xcb plugin

### macOS
**Issue: "dyld: Library not loaded: @rpath/QtCore.framework/Versions/A/QtCore"**
- Solution: Run `macdeployqt` to fix library paths

**Issue: App bundle won't open**
- Solution: Check Gatekeeper settings or codesign the application

## Automated Deployment Scripts

This project includes automated deployment scripts:

- **`deploy-windows.bat`** - Builds and deploys for Windows
- **`deploy-linux.sh`** - Builds and deploys for Linux
- **`deploy-macos.sh`** - Builds and deploys for macOS

These scripts handle:
1. Building the project in Release mode
2. Running the appropriate Qt deployment tool
3. Copying additional resources
4. Creating distribution archives

## Creating Installers

### Windows (NSIS or Inno Setup)
Create an installer script to package the deployment directory:
```nsis
; Example NSIS script
OutFile "Scriptura-Setup.exe"
InstallDir "$PROGRAMFILES\Scriptura"
Page directory
Page instfiles

Section "Main"
  SetOutPath "$INSTDIR"
  File /r "deploy\*"
  CreateShortCut "$DESKTOP\Scriptura.lnk" "$INSTDIR\scriptura.exe"
SectionEnd
```

### Linux (AppImage)
Use linuxdeployqt to create an AppImage:
```bash
linuxdeployqt scriptura -appimage
```

### macOS (DMG)
The `deploy-macos.sh` script automatically creates a DMG using `hdiutil`.

## Additional Resources

- [Qt Deployment Documentation](https://doc.qt.io/qt-6/deployment.html)
- [windeployqt Manual](https://doc.qt.io/qt-6/windows-deployment.html)
- [linuxdeployqt](https://github.com/probonopd/linuxdeployqt)
- [macdeployqt](https://doc.qt.io/qt-6/macos-deployment.html)

## Troubleshooting

If you encounter issues:

1. **Check Qt version compatibility**: Ensure you're using the same Qt version for building and deployment
2. **Verify plugin paths**: Qt plugins must be in the correct subdirectory (e.g., `platforms/`)
3. **Check library dependencies**: Use `ldd` (Linux), `otool` (macOS), or Dependency Walker (Windows)
4. **Clean build**: Sometimes a clean rebuild helps: `rm -rf cmake-build-* && ./build.sh`

For project-specific issues, check the main [README.md](README.md).