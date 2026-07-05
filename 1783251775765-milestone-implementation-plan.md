# Scriptura Missing Features — Milestone Implementation Plan

## Assumptions & Decisions

| Decision | Rationale |
|----------|-----------|
| **Editor engine**: Keep `QPlainTextEdit` for code; add `PreviewWidget` (Markdown/image) that replaces the editor stack content when a non-code file is opened. | Preserves existing highlighting + LSP integration; avoids a full Scintilla rewrite. Image/Markdown preview becomes the “second engine.” |
| **Project search**: Use `ripgrep` (`rg`) when available; fall back to a Qt `QDirIterator` + `QTextStream` scanner. | `rg` is fast; fallback ensures no hard dependency. |
| **Debugger**: Built-in DAP client in core (`MainWindow` + `CodeEditor` breakpoint overlay), not a plugin. | Requires deep editor integration (gutter icons) and lifecycle hooks that don’t fit the current plugin API. PluginContext can still expose debug signals. |
| **Git viewer**: Rewrite bottom Git panel into a tabbed widget with diff / staging / history / merge views. | Reuses the existing `bottomPanelStack` without duplicating panel logic. |
| **Workspace files**: JSON files (`.scriptura-workspace`) rather than VS Code `.code-workspace`. | Avoids namespace collisions; still supports multi-root folder arrays. |

---

## Milestone 1 — Editor Power Editing

Goal: Multi-cursor editing, column selection, full Find/Replace, and project-wide search.

### 1.1 Multi-Cursor & Column Selection

**New classes / files**
- `multi-cursor.h/.cpp` — `MultiCursorManager`
  - Owns a list of current `QTextCursor` objects.
  - Methods: `addCursor(pos)`, `removeLastCursor()`, `clear()`, `cursors()`, `setCursors(list)`.
  - Emits `cursorsChanged()`.
- `columnselection.h/.cpp` — `ColumnSelection`
  - Utilities: `calcColumnRanges(QTextCursor, QPoint, QPoint)` returning `QRectF`-based selection blocks.

**Modified files**
- `codeeditor.h`:
  - Add `MultiCursorManager* m_multiCursor;`
  - Add `bool m_columnSelectionMode = false;`
  - Add `void mousePressEvent(QMouseEvent*)`, `mouseMoveEvent(QMouseEvent*)`, `keyPressEvent(QKeyEvent*)` overrides.
  - Inline `paintEvent` already draws `drawIndentGuides`; no change there.
- `codeeditor.cpp`:
  - `mousePressEvent`: default `QPlainTextEdit::mousePressEvent`; if `Alt` is pressed, add cursor at click (column select if `Alt+Shift`).
  - `mouseMoveEvent`: same, but add/update cursors to paint extra selections.
  - `keyPressEvent`: when multi-cursor exists, forward typed chars/deletes to all cursors (by saving each cursor’s position, bulk edit, then restoring). Use `QTextEdit::ExtraSelection` for visual extra selections.
  - `void setExtraSelections` must include both current-line highlight and multi-cursor selections.

### 1.2 Find / Replace

**New classes / files**
- `findreplace.h/.cpp` — `FindReplaceBar`
  - QWidget with `QLineEdit find`, `QLineEdit replace`, checkboxes (`CaseSensitive`, `WholeWord`, `Regex`), Prev/Next/Replace/ReplaceAll buttons.
  - Methods: `findIn(const QString&, bool forward)`, `replaceOne()`, `replaceAll()`.
  - Uses `QTextDocument::find(QRegularExpression)` for regex-aware matching.
  - Signals: `replaceAllComplete(int count)`.

**Modified files**
- `mainwindow.h`:
  - Add `FindReplaceBar* findReplaceBar;`
  - Replace single-line search bar with toggle between “Find” and “Find+Replace”.
- `mainwindow.cpp`:
  - Add `findReplaceBar` to `topToolbarLayout`.
  - Toggle via `Ctrl+F` (find) and `Ctrl+H` (find+replace).
  - Wire `Ctrl+G` / `Shift+Ctrl+G` to next/prev.

**Behavior**
- `find()` tracks a `QTextCursor` per open tab for wrap-around search.
- Match highlights use `QTextEdit::ExtraSelection` (light-blue background).
- Replace uses `QTextDocument::find()` again and `QTextCursor::insertText()`.

### 1.3 Project-Wide Search

**New classes / files**
- `projectsearch.h/.cpp` — `ProjectSearchPanel`
  - Reuses `QPlainTextEdit` inside a `QDockWidget` or bottom tab.
  - Methods:
    - `search(const QString& term, const QString& rootPath)`
    - `clearResults()`
  - Run `rg` or Qt scanner:
    - If `rg` found: `QProcess::startDetached("rg", {"--no-heading", "--line-number", "--with-filename", term, rootPath})`.
    - Capture stdout, parse `<file>:<line>:<col>:<text>`.
    - If `rg` missing: `QDirIterator` with `QTextStream` per file (limit to text, skip binary).

**Modified files**
- `bottomPanelTabs`: add “Search Results” tab.
- Add action `Ctrl+Shift+F` → show panel, focus line edit.

### 1.4 Command Palette

**New classes / files**
- `commandpalette.h/.cpp` — `CommandPalette`
  - `QLineEdit` with `QListView` (`QStringListModel`) and fuzzy match filter.
  - `QHash<QString, Command>` registry where:
    - `struct Command { QString id, label, shortcut; std::function<void()> action; };`
- `mainwindow.h`: expose `void registerCommand(const Command&);`
- `mainwindow.cpp`:
  - Register core commands at startup: Open Project, Save, Find, Replace, Format, Git Commit, Git Push, etc.
  - Open palette via `Ctrl+Shift+P`; invoke selected command action.

---

## Milestone 2 — LSP Completeness

Goal: Fill the remaining LSP surface and UI state.

**Modified files**
- `lspclient.h/.cpp`:
  - Add methods:
    - `void documentSymbol(const QString& uri)`
    - `void workspaceSymbol(const QString& query)`
    - `void formatting(const QString& uri, const QJsonObject& options = {})`
    - `void rangeFormatting(const QString& uri, const LspClient::Range& range)`
    - `void signatureHelp(const QString& uri, const LspClient::Position& pos)`
    - `void declaration(const QString& uri, const LspClient::Position& pos)`
    - `void typeDefinition(const QString& uri, const LspClient::Position& pos)`
    - `void implementation(const QString& uri, const LspClient::Position& pos)`
  - Extend `initialize()` `capabilities` to advertise:
    - `textDocument/documentSymbol`
    - `textDocument/formatting`, `textDocument/rangeFormatting`
    - `textDocument/signatureHelp`
    - `textDocument/declaration`, `textDocument/typeDefinition`, `textDocument/implementation`
    - `workspace/symbol`
    - `experimental/inlayHint` (when supported)
  - New signals: `documentSymbolReceived(...)`, `formattingReceived(const QJsonArray&)`, etc.

- `codeeditor.h/.cpp`:
  - Add `void setInlayHints(const QList<LspClient::InlayHint>& hints);` (uses `QTextEdit::ExtraSelection` with a custom `QTextCharFormat` and a draw callback, or overlay widget).
  - Add breakpoint rendering: `void setBreakpoints(const QList<int>& lines);`
  - Reuse existing `lineNumberAreaPaintEvent` for breakpoint icons.
  - Add diagnostics hover tooltip: when hover on squiggly line, show diagnostic message via `QToolTip::showText()`.

- `mainwindow.cpp`:
  - Wire `Ctrl+Shift+I` → formatting.
  - Wire document symbol command to show a “Symbols” panel (reuse `ProblemPanel` widget or new `OutlinePanel`).
  - Wire signature help to `QToolTip` near cursor on `(` when requested by LSP.
  - Auto-import: expose LSP `codeAction/resolve` result; if resolution returns `addImport`, apply edits.

### 2.1 LSP Initialization Improvements

- Store `m_currentUri` per tab in addition to `currentFile`.
- Initialize once per project (`initialize(rootUri, langId)`); open files send `didOpen`.
- On tab close, `didClose` and keep server alive for other tabs.

---

## Milestone 3 — Git Workflow

Goal: Replace the raw text-output Git panel with structured views.

**New classes / files**
- `gitdiffwidget.h/.cpp` — `GitDiffWidget`
  - `QTextEdit` with syntax-highlighted diff colors.
  - Methods: `setDiff(const QString& diff);`
- `gitbranchwidget.h/.cpp` — `GitBranchWidget`
  - `QTreeView` listing local/remote branches, buttons: Create, Checkout, Delete, Merge.
- `gitmergewidget.h/.cpp` — `GitMergeWidget`
  - Three-pane merge conflict editor using `QSplitter`.
  - Buttons: Accept Ours, Accept Theirs, Accept Both.

**Modified files**
- `gitpanel.h/.cpp`:
  - Replace `m_outputEdit` with `QTabWidget` containing:
    - Diff view
    - Staging view (file list with check/uncheck + stage/unstage buttons)
    - Branch view
    - History view
    - Merge view (disabled by default; shown when merge conflicts detected)
  - Signals: `stageRequested(path)`, `unstageRequested(path)`, `commitRequested()`, `pushRequested()`, `pullRequested()`.

- `mainwindow.cpp`:
  - `on_action_git_commit_triggered()`: show staging view pre-commit; include file list + message input.
  - Add `Git Push`, `Git Pull`, `Git Fetch` with expanded output.
  - Detect `CONFLICT` in git output and show merge view.

---

## Milestone 4 — Debugger Integration (DAP)

Goal: Debug tab / side panel with breakpoints, call stack, watches, variables.

**New classes / files**
- `dapclient.h/.cpp` — `DapClient`
  - Owns `QProcess` like `LspClient`.
  - Key messages: `initialize`, `launch`, `setBreakpoints`, `configurationDone`, `stackTrace`, `scopes`, `variables`, `continue`, `next`, `stepIn`, `stepOut`, `pause`, `disconnect`.
  - Structs: `Breakpoint { int line; bool verified; }`, `StackFrame { int id; QString name; int line; }`, `Scope`, `Variable`.
  - Signals: `breakpointUpdated()`, `stopped()`, `continued()`, `stackTraceReceived()`, `variablesReceived()`.

- `debugpanel.h/.cpp` — `DebugPanel`
  - `QTreeView` for Call Stack, Watches, Variables (tabs).
  - `QLineEdit` watch input.
- `debuggergutter.h/.cpp` — `DebuggerGutter`
  - Paints breakpoint icons in the line-number area.
- `debugconfiguration.h/.cpp` — `DebugConfiguration`
  - JSON table: `name, type, request, program, args, cwd`.
- `rundialog.h/.cpp` — `RunDialog`
  - Run and Debug dropdown for active configurations.

**Modified files**
- `mainwindow.h`:
  - Add `DapClient* dapClient;`
  - Add `DebugPanel* debugPanel;`
  - Add `void startDebug(const QString& configName);`
- `codeeditor.h`:
  - Add `void setBreakpointLine(int line, bool enabled);`
  - Add `QList<int> m_breakpointLines;`
- `mainwindow.cpp`:
  - Add Run/Debug buttons next to Git/Push in toolbar.
  - When stopped: show debug panel, highlight current line.
  - Key bindings: `F5` continue, `F10` next, `F11` step in, `Shift+F11` step out, `F9` toggle breakpoint.

---

## Milestone 5 — Workspace & Productivity

Goal: Multi-root workspaces, session restore, recent files, tasks/terminal runner.

**New classes / files**
- `workspace.h/.cpp` — `Workspace`
  - Methods: `load(const QString& path)`, `save()`, `folders()`, `settings()`.
  - File format: `.scriptura-workspace` JSON with:
    ```json
    {
      "folders": [{ "path": "/home/user/proj" }],
      "settings": { "tabWidth": 4, "theme": "Dark/Blue" },
      "recentFiles": ["/a/b.cpp", "/c/d.py"]
    }
    ```
- `taskrunner.h/.cpp` — `TaskRunner`
  - JSON or hand-editable tasks similar to VS Code `tasks.json`.
  - Methods: `runTask(const QString& name)`, `stopTask()`.
  - Reuses `TerminalPanel` for output.

**Modified files**
- `mainwindow.h`:
  - Add `Workspace* m_workspace;`
  - Add `QStringList recentFiles;`
  - Add `void openRecentFile(const QString& path);`
- `mainwindow.cpp`:
  - `showWelcomeScreen()`: show recent files + recent projects.
  - On startup: load last workspace JSON if present; open listed folders; restore session file list.
  - Auto-save session to workspace on `closeEvent` and on a 30s interval.

---

## Milestone 6 — UI/UX Polish

Goal: Minimap, split view, breadcrumbs, distraction-free mode, file icons.

**New classes / files**
- `minimap.h/.cpp` — `Minimap`
  - Subclass `QWidget`.
  - Paints a compressed 1:1 pixel view of the document (or every `N` lines).
  - Highlights visible region.
  - Clicking/jumping maps viewport position back to `CodeEditor`.
- `splitmanager.h/.cpp` — `SplitManager`
  - Owns `QList<QPair<CodeEditor*, QSplitter*>>`.
  - Tracks active editor per split.
- `breadcrumb.h/.cpp` — `Breadcrumb`
  - Shows file path / symbol path based on cursor position.

**Modified files**
- `mainwindow.ui`:
  - Add minimap widget beside each editor.
  - Add split buttons to toolbar.
- `mainwindow.cpp`:
  - Toggle minimap visibility in settings.
  - Add `F11` Zen mode: full-screen `QMainWindow::showFullScreen`, hide all side panels.
  - Integrate `SplitManager` with tab events.

---

## Open Questions & Risks

1. **QPlainTextEdit multi-cursor typing**: Qt does not natively support multi-cursor editing in `QPlainTextEdit`. Budget time for a custom `keyPressEvent` that iterates all cursors and bulk-inserts text.
2. **Inlay hints**: No Qt primitive for inline drawing without subclassing `QPlainTextEdit` and handling `paintEvent`. Plan 2–3 days for layout math.
3. **DAP threading**: Like `LspClient`, `DapClient` should queue requests and never emit signals from the `QProcess` thread directly without marshaling to the main thread.
4. **Windows/macOS ripgrep fallback**: Windows `rg.exe` and macOS `brew install ripgrep` are assumed. Without them, fallback to Qt traversal.
5. **Python/Conda envs**: LSP paths are hardcoded to `/usr/bin/clangd` etc. Should switch to `envPath` resolution or `settings.json` per language before shipping.

---

## Sequence

| Order | Milestone | Depends on |
|-------|-----------|------------|
| 1 | Editor Power Editing | Naught |
| 2 | LSP Completeness | Milestone 1 (multi-cursor helps editing; find/replace helps symbols) |
| 3 | Git Workflow | Naught (parallelizable) |
| 4 | Debugger Integration | LSP (nice-to-have for variable inspection) |
| 5 | Workspace & Productivity | M1 + M3 (session restore when open files + git state) |
| 6 | UI/UX Polish | M1 (minimap + split need multi-cursor) |

Milestones 1 and 3 can run in parallel.
