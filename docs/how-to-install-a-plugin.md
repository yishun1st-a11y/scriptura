# How to Install a Plugin

Scriptura supports installing and managing plugins only through the built-in **Plugin Manager**. Manual plugin installation is not supported.

## 1. Plugin Manager (GUI)

### Opening the Plugin Manager

- Menu: **Plugins → Manage Plugins...**

### Installing a Plugin

1. Click **Install Plugin...**
2. Select the plugin directory (the folder containing `plugin.json`)
3. Confirm the installation
4. Restart Scriptura to load the new plugin

### Removing a Plugin

1. Select the installed plugin from the list
2. Click **Remove Plugin**
3. Confirm the removal

If the plugin is currently loaded, restart Scriptura for the removal to take effect.

### Using the GUI

- **Refresh** — Rescan installed plugins
- **Details** — Click or double-click a plugin to view its metadata (ID, version, author, dependencies, permissions)

## 2. Plugin Package Structure

A Scriptura plugin package is a directory containing:

```
myplugin/
├── plugin.json      # Metadata (required)
├── myplugin.dll     # Compiled plugin library (required)
└── resources/       # Optional static assets
    └── icons/
```

## 3. Plugin Installation Path

Scriptura installs plugins through the GUI to a single managed plugins directory:

- **Linux**: `~/.local/share/scriptura/plugins/`

## 4. Platform-Specific Library Names

| Platform | Output Name | Full Path |
|----------|-------------|-----------|
| Linux | `libmyplugin.so` | `<pluginPath>/myplugin/libmyplugin.so` |
| macOS | `libmyplugin.dylib` | `<pluginPath>/myplugin/libmyplugin.dylib` |
| Windows | `myplugin.dll` | `<pluginPath>/myplugin/myplugin.dll`

The `library` field in `plugin.json` should match the expected file name for the target platform.

## 5. Verifying Installation

After restarting Scriptura, open **Plugins → Manage Plugins...** and confirm the plugin appears in the list. If it does not appear, check that:

- `plugin.json` is present and valid
- The plugin library file exists in the plugin directory
- The plugin was installed using the Plugin Manager

## 6. Common Issues

### Missing dependencies

Ensure `dependencies` in `plugin.json` matches loaded plugin IDs exactly.

### Architecture mismatch

Plugin libraries must match the application's architecture (x86_64 vs arm64) and Qt version.

### Missing metadata

Each plugin directory must contain exactly one `plugin.json` at its root.

### Wrong permissions

Do not run Scriptura as root to install plugins. Install them through the GUI instead.

## 7. Uninstalling

Use the Plugin Manager to remove plugins.
