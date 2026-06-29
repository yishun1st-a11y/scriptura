# Scriptura

![Scriptura Icon](icon.png)

[![Storage](https://img.shields.io/github/repo-size/jason1015-coder/scriptura)](https://github.com/jason1015-coder/scriptura)
[![Stars](https://img.shields.io/github/stars/jason1015-coder/scriptura)](https://github.com/jason1015-coder/scriptura/stargazers)
[![Forks](https://img.shields.io/github/forks/jason1015-coder/scriptura)](https://github.com/jason1015-coder/scriptura/network/members)

## Star History

<a href="https://www.star-history.com/?repos=jason1015-coder%2Fscriptura&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=jason1015-coder/scriptura&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=jason1015-coder/scriptura&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=jason1015-coder/scriptura&type=date&legend=top-left" />
 </picture>
</a>

A simple Qt-based text editor with project file browsing.

## Features

- **Project-based workflow**: Open a project directory to browse files
- **File tree sidebar**: Navigate project structure with clickable folders
- **Directory navigation**: "Go Up" button to navigate to parent directories
- **Tabbed editing**: Multiple files open in tabs
- **Status bar**: Shows cursor position (line/column)
- **Edit operations**: Cut, Copy, Paste, Undo, Redo
- **File management**: Create files and directories within projects
- **Delete files/directories**: Remove files or directories from the project
- **Save As**: Save files with a different name
- **Theme support**: Multiple light/dark themes available

## Building

```bash
./build.sh
```

Or manually:
```bash
cmake -B build/Desktop-Debug -S .
cmake --build build/Desktop-Debug
```

## Running

```bash
./run.sh
```

Or directly:
```bash
./build/Desktop-Debug/scriptura
```

## Requirements

- Qt 5 or Qt 6 (with Widgets module)
- CMake 3.16+
- C++17 compiler

## CI/CD

Automated builds are available for Linux, macOS, and Windows via GitHub Actions. See `.github/workflows/build.yml` for the workflow configuration.

## License

MIT License - see LICENSE file
