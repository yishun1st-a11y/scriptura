# Scriptura Test Checklist

> **Scope:** Every feature, menu action, button, shortcut, panel, plugin, and expected behavior.
> **OS columns:** Tick `[x]` if working, `[ ]` if not. Use the paired ✗ column only for known regressions on that OS.

---

## 1. Application Lifecycle

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---------|---------|-----------|---|---|
| 1.1 | Launch `scriptura` | Starts without crash; shows Welcome screen | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | |
| 1.2 | Launch with `--help` / invalid arg | Prints usage or handles gracefully | won't repo | |         |         |           | | |
| 1.3 | Window resize | Resizes editor, sidebar, and bottom panel proportionally | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | |
| 1.4 | Window maximize | All panels fill screen; state restored on reopen | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | |
| 1.5 | Window close (Ctrl+W / X button) | Prompts to save modified files; exits cleanly | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | Ctrl+W wired in mainwindow.cpp |
| 1.6 | New Window (`menu_terminal -> New Window`) | Spawns second independent Scriptura window | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | Fixed: now spawns real Scriptura process |
| 1.7 | Clone Window (`menu_terminal -> Clone Window`) | Spawns window with same project + open files | [x] | [ ] | [ ]     | [ ]     | [x]       | [ ] | Fixed: passes `--project` + open file paths |

---

## 2. Menu Bar — File

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---------|--------|---|---|---|---|---|
| 2.1 | Open Project (`Ctrl+O`) | Opens folder picker; loads directory tree into sidebar | [x]     | [ ]    | [ ] | [ ] | [ ] | [ ] | |
| 2.2 | Open File | Opens file picker; opens file in new editor tab | [x]     | []     | [ ] | [ ] | [ ] | [ ] | |
| 2.3 | Save (`Ctrl+S`) | Saves current file; clears modified indicator | [x]     | [ ]    | [ ] | [ ] | [ ] | [ ] | |
| 2.4 | Save As (`Ctrl+Shift+S`) | Opens save dialog; writes to new path; updates tab title | [x]     | [ ]    | [ ] | [ ] | [ ] | [ ] | |
| 2.5 | Add File/Directory | Context menu in file tree; creates file or folder | [x]     | [ ]    | [ ] | [ ] | [ ] | [ ] | |
| 2.6 | Delete File/Directory | Deletes selected path; refreshes tree; confirmation dialog | [x]     | [ ]    | [ ] | [ ] | [ ] | [ ] | |

---

## 3. Menu Bar — Edit

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 3.1 | Cut (`Ctrl+X`) | Removes selection to clipboard | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.2 | Copy (`Ctrl+C`) | Copies selection to clipboard | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.3 | Paste (`Ctrl+V`) | Inserts clipboard content at cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.4 | Undo (`Ctrl+Z`) | Reverts last edit | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.5 | Redo (`Ctrl+Y`) | Re-applies undone edit | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 4. Menu Bar — Terminal

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 4.1 | New Window | Opens new empty Scriptura window | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 4.2 | Clone Window | Opens new window duplicating current session | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 5. Menu Bar — Git

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 5.1 | Commit | Opens commit dialog; runs `git commit` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 5.2 | Push | Runs `git push`; shows output in Git panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 5.3 | Pull (`on_action_git_pull_triggered`) | Wired in `mainwindow.ui`; auto-connected via `connectSlotsByName` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Fixed: now shows Git panel |
| 5.4 | Fetch (`on_action_git_fetch_triggered`) | Wired in `mainwindow.ui`; auto-connected via `connectSlotsByName` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Fixed: now shows Git panel |

---

## 6. Menu Bar — Search

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 6.1 | Find (`Ctrl+F`) | Shows Find bar; highlights matches; Find Next/Prev work | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: case-sensitivity flag was inverted |
| 6.2 | Replace (`Ctrl+H`) | Shows Replace bar; replace / replace-all work | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: case-sensitivity flag was inverted |
| 6.3 | Project Search (`Ctrl+Shift+F`) | Shows Project Search panel; searches files; opens result | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 6.4 | Command Palette (`Ctrl+Shift+P`) | Shows palette; filters commands; executes on Enter | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 7. Menu Bar — Code (LSP)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 7.1 | Format Document (`Ctrl+Shift+I`) | Formats via LSP; replaces document text or range | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: formattingReceived now applied to editor |
| 7.2 | Show Symbols | Displays document symbols list | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: documentSymbolReceived shows symbol list |
| 7.3 | Go to Definition (`F12`) | Jumps cursor to symbol definition | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: definitionReceived now jumps to location |

---

## 8. Menu Bar — Debug

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 8.1 | Run / Debug (`F5`) | Shows Run Dialog; launches DAP session | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.2 | Stop Debugging (`Shift+F5`) | Stops debuggee; cleans up breakpoints | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.3 | Toggle Breakpoint (`F9`) | Adds/removes breakpoint on current line | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.4 | Continue (`Ctrl+F5`) | Resumes execution; debugger stops on next breakpoint | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.5 | Step Over (`F10`) | Steps over current line; stops at next | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.6 | Step Into (`F11`) | Steps into function call | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.7 | Step Out (`Shift+F11`) | Steps out of current function | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.8 | Conditional Breakpoint | Breakpoint dialog accepts condition; only hits when true | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Requires DAP server support | Fixed: conditional breakpoint dialog + DAP conditions |
| 8.9 | Attach to Process | Run Dialog "Attach" mode lists PIDs; attaches debugger | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Platform-dependent PID listing |
| 8.10 | Debug Console | Evaluates expressions in Debug panel; shows result | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 9. Menu Bar — Tools

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 9.1 | HTTP Client | Opens HTTP tab in bottom panel; send requests | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 9.2 | SQLite Viewer | Opens Database tab; browse tables and run queries | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 9.3 | AI Completions (toggle) | Enables/disables inline ghost text completions | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 10. Menu Bar — Settings

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 10.1 | About | Shows version and info dialog | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: About now shows version |
| 10.2 | Editor Settings | Opens Settings tab with editor controls | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.3 | Theme | Opens Theme tab; preview updates live | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.4 | License | Shows license text | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.5 | Check for Updates | Fetches latest version; shows notification if available | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 11. Menu Bar — Plugins

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 11.1 | Manage Plugins | Opens Plugin Manager dialog with Installed + Registry tabs | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 12. Top Toolbar Buttons

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 12.1 | Sidebar Toggle (☰) | Uses SVG icon; collapses/expands left file tree drawer | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.2 | Go Up (↑) | Uses SVG icon; navigates file tree to parent directory | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.3 | Find Replace Bar | Visible when Find/Replace activated; input functional | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 12.4 | Theme Button (☼) | Uses SVG icon; opens theme selector | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.5 | Settings Button (⚙) | Uses SVG icon; opens editor settings tab | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |

---

## 13. Sidebar Buttons (Icon Bar)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 13.1 | File Tree (☸) | SVG icon; toggles file tree visibility | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.2 | Todo (⚑) | SVG icon; switches main view to Todo panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.3 | Terminal (>_) | SVG icon; switches main view to Terminal panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.4 | Problems (⚠) | SVG icon; opens bottom Problems panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.5 | Git (⎇) | SVG icon; opens bottom Git panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |

---

## 14. Top Tab Bar

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
 | 14.1 | Welcome tab | Shows on startup with app icon, title card, primary buttons + shortcuts | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated card layout |
 | 14.2 | Open Project button (Welcome) | Opens project picker | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 14.3 | New File button (Welcome) | Opens new file dialog | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 14.4 | Recent Projects list (Welcome) | Click opens project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 14.5 | Editor file tabs | Each open file shows tab with close button | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 14.6 | Tab close button | Closes editor tab; restores previous editor state | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | 20×20 SVG with hover |
| 14.7 | Theme Settings tab | Color family, mode, high-contrast, preview | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.8 | Editor Settings tab | Font, tab width, etc. | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.9 | Keyboard Shortcuts tab | Displays shortcut reference | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.10 | Updates tab | Update checker UI | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 15. Editor Tab Widget

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 15.1 | Open file via tree | File opens in new tab; content loads | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.2 | Switch tabs | Editor content switches to selected file | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.3 | Modified indicator | Asterisk or dot appears on unsaved tab | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: modified asterisk on top tab bar |
| 15.4 | Tab tooltip | Shows full file path on hover | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: tab tooltip shows full path |
| 15.5 | Tab close (middle-click) | Closes tab | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: middle-click closes tab |

---

## 16. Bottom Panel Tabs

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 16.1 | Problems tab | Shows LSP diagnostics list; filterable | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.2 | Git tab | Shows git output, diff, branches, merge | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.3 | Search Results tab | Shows project search results | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.4 | Debug tab | Shows stack, scopes, variables, watches | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.5 | HTTP tab | Shows HTTP Client panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.6 | Database tab | Shows SQLite Viewer panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 17. Editor Surface

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 17.1 | Syntax highlighting | Language-specific colors for C/C++, Python, Java, JS, TS, Rust, Go, Shell, HTML, CSS, Script, plain text | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.2 | Line numbers | Gutter shows line numbers; updates on scroll/edit | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.3 | Indent guides | Vertical guides at tab stops | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.4 | Current line highlight | Current line highlighted on focus | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.5 | Breakpoint gutter | Red dot on breakpoint line | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.6 | Debug execution line | Current debug line highlighted distinctively | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.7 | Inlay hints | Parameter names / type hints rendered inline | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.8 | Ghost text (AI) | Translucent suggested text after cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.9 | Diagnostic underlines | Red squiggly / highlight on LSP errors | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.10 | Hover diagnostic tooltip | Shows error message on mouse hover | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: diagnostic tooltip on hover |
| 17.11 | Multi-cursor (Alt+Click) | Adds secondary cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.12 | Column selection (Alt+Shift+Click) | Rectangular selection | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.13 | Minimap | Overview strip rendered right of editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.14 | Breadcrumb | Shows file path / symbol path above editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.15 | Split horizontal | Editor splits left/right | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.16 | Split vertical | Editor splits top/bottom | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.17 | Close split | Returns to single editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.18 | Accept ghost text (Tab/Enter) | Inserts AI suggestion into document | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.19 | Dismiss ghost text (Escape) | Removes AI suggestion without inserting | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.20 | Typing clears ghost text | Any typed character invalidates pending suggestion | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.21 | Scroll with ghost text | Ghost text stays anchored to cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.22 | Cursor position status | Status bar shows line:column | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.23 | Tab width | Tab stops match configured width (default 4) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.24 | Word wrap | Toggle word wrap wraps long lines | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.25 | Auto-save | Modified files saved on timer or focus change | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 18. LSP Features

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 18.1 | LSP server start | Starts automatically on recognized file type | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.2 | LSP diagnostics | Errors/warnings appear in Problems panel | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.3 | Completion | Ctrl+Space or auto-trigger shows suggestion list | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: Ctrl+Space completion popup |
| 18.4 | Hover | Mouse hover shows documentation popup | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: hover tooltip via LSP |
| 18.5 | Go to Definition | Jumps to definition; opens file if needed | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: definition jump wired |
| 18.6 | Find References | Lists all references in project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.7 | Rename | In-place rename across project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.8 | Document Symbols | Tree of symbols in current file | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: document symbols wired |
| 18.9 | Workspace Symbols | Search across project symbols | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.10 | Formatting | Formats entire document | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: formatting wired |
| 18.11 | Signature Help | Shows parameter hints during function call | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.12 | Declaration | Jumps to declaration site | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: declaration jump wired |
| 18.13 | Type Definition | Jumps to type definition | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: typeDefinition jump wired |
| 18.14 | Implementation | Jumps to implementation | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: implementation jump wired |
| 18.15 | Code Actions (quick-fix) | Ctrl+. shows fix bar; clicking fixes diagnostic | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 19. Debugger (DAP)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 19.1 | Run/Debug dialog | Lists saved configurations; Add/Delete works | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: Run dialog Add/Delete/Edit |
| 19.2 | Launch configuration | Starts debuggee; stops on first line or breakpoint | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.3 | Stop Debugging | Terminates debuggee; clears debug state | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.4 | Continue | Resumes until next breakpoint | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.5 | Step Over | Steps one line; enters functions if configured | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.6 | Step Into | Steps into function | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.7 | Step Out | Steps out to caller | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.8 | Breakpoint set (F9) | Visual indicator in gutter; DAP `setBreakpoints` sent | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.9 | Conditional breakpoint | Entered condition sent to DAP server | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.10 | Stack trace panel | Shows call frames on stop | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.11 | Scopes panel | Shows local/global/closure variables | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: scopes requested; variables populated |
| 19.12 | Variables panel | Expandable variables with types/values | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: variables tree populated |
| 19.13 | Watches panel | Add watch expressions; values update | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: watches evaluate via DAP |
| 19.14 | Debug Console evaluate | Evaluates expression in current frame | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: debug console evaluate via DAP |

---

## 20. AI Inline Completions

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 20.1 | Enable toggle (Tools menu) | Checked state enables/disables inline completions | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.2 | Ghost text render | Translucent text appears after cursor after debounce | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: AI editor signals connected |
| 20.3 | Accept (Tab/Enter) | Ghost text inserted into document | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.4 | Dismiss (Escape) | Ghost text removed without insertion | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.5 | Typing clears ghost | Any printable character clears pending suggestion | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: typing clears ghost |
| 20.6 | Cursor move clears ghost | Clicking elsewhere invalidates suggestion | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: cursor move clears ghost |
| 20.7 | Ollama backend | `POST` to endpoint returns completion in ghost text | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Requires Ollama running | Fixed: Ollama /api/chat response parsed |
 | 20.8 | OpenAI-compatible backend | `POST /v1/chat/completions` returns completion; authenticates with `ai/apiKey` Bearer token if set | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.9 | Debounce | Requests throttled to configured ms (default 400) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 21. HTTP Client Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 21.1 | Method selector | GET/POST/PUT/DELETE/PATCH/HEAD/OPTIONS | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.2 | URL input | Type URL; supports `Ctrl+Enter` or Send button | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.3 | Send button | Dispatches request; shows response | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.4 | Environment dropdown | Selects env preset; `{{VAR}}` substitution applied to URL/body on send | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Implemented | Fixed: env dropdown switches presets |
| 21.5 | Auth — None | No auth headers sent | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.6 | Auth — Basic | `Authorization: Basic <base64>` header sent | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.7 | Auth — Bearer | `Authorization: Bearer <token>` header sent | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.8 | Request body editor | Sends JSON body when method supports payload | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.9 | Timeout spinner | Request aborts after selected ms | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.10 | Response Headers tab | Lists raw response headers | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.11 | Response Body tab | JSON bodies rendered as key/value table or raw text | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.12 | Copy header value (double-click) | Copies header value to clipboard | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.13 | HEAD request body | Body tab shows "No body for HEAD request" | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.14 | Status label | Shows HTTP status code + elapsed ms | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: elapsed time measured |

---

## 22. CodeAction UI

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 22.1 | Ctrl+. shortcut | Shows/hides quick-fix bar for current diagnostics | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.2 | Quick-fix bar rendering | Shows count label + action buttons | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.3 | Preferred action highlight | Preferred fix shown with highlighted style | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.4 | Click action button | Emits actionTriggered; hides bar (wiring to apply fix is TBD) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 23. SQLite Viewer Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 23.1 | Open SQLite button | File picker filters `.sqlite`, `.db`, `.sqlite3` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.2 | Table list | Lists tables + views from `sqlite_master` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.3 | Double-click table | Loads rows into data grid | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.4 | Ad-hoc SQL query | Enter SELECT query; press Enter; shows result grid | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.5 | Refresh button | Re-reads schema / re-runs query | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.6 | Error display | SQL errors shown in status label | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.7 | NULL values | NULLs rendered as text "NULL" | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.8 | Connection cleanup | DB closed when panel destroyed or new DB opened | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 24. Plugin Registry & Auto-Update

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 24.1 | Registry URL | Configurable via Updates settings panel; persisted with QSettings | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | Implemented |
| 24.2 | Check for updates | `GET` manifest JSON; emits `registryUpdated` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.3 | Weekly timer | `checkForUpdates()` called on timer (mock in code) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: weekly update timer starts |
| 24.4 | Install plugin from registry | Downloads payload; emits `pluginDownloaded` | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.5 | Upgrade detection | `upgradeAvailable()` compares versions | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.6 | Plugin Manager — Installed tab | Lists installed plugins + state | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.7 | Plugin Manager — Install Plugin | Installs from directory | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.8 | Plugin Manager — Remove Plugin | Deletes plugin files | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.9 | Plugin Manager — Disable/Enable | Toggles plugin state without deletion | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.10 | Plugin hot-reload | Reload updates plugin without full restart | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 25. Git Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 25.1 | Git output tab | Shows command stdout/stderr | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.2 | Commit button | Commits staged changes with message | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.3 | Push button | Pushes to remote | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.4 | Pull button | Pulls from remote | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.5 | Fetch button | Fetches remote refs | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.6 | Stage / Unstage | Changes staging area in working tree view | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: stage/unstage runs git |
| 25.7 | Diff tab | Shows unified diff of selected file | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: diff tab populated |
| 25.8 | Branches tab | Lists local + remote branches | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: branches list + checkout/create/delete |
| 25.9 | Merge tab | Three-way merge view (ours/theirs/result) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: merge tab three-way view |

---

## 26. Terminal Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 26.1 | Shell start | Launches default shell (`bash`, `zsh`, `powershell`, etc.) | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.2 | Command input | Typed commands execute in shell; output rendered | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.3 | Command history | Up/Down arrows cycle previous commands | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.4 | Clear button | Clears terminal output | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.5 | Working directory | Updates on `cd`; synchronized with project root | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.6 | ANSI color codes | Colors rendered in output | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.7 | Shell exit | Terminal shows exit code; can restart | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 27. Problem Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 27.1 | Problems list | Shows diagnostics grouped by file | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.2 | Filter — All | Shows errors, warnings, info | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.3 | Filter — Errors | Shows only errors | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.4 | Filter — Warnings | Shows only warnings | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.5 | Filter — Info | Shows only information diagnostics | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.6 | Activate problem | Double-click jumps to line in editor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.7 | Close button | Clears problem list view | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.8 | Count label | Shows number of problems in current filter | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 28. Todo Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 28.1 | Todo items list | Displays parsed TODO comments from open document | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: TODO comments parsed |
| 28.2 | Click todo item | Jumps to corresponding line in editor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: click jumps to line |

---

## 29. Workspace / State

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 29.1 | Recent projects | Persisted across restarts; click to reopen | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.2 | Recent files | Persisted per project; reopenable | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: recent files persisted |
| 29.3 | Window geometry | Restores size + position on launch | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.4 | Panel layout | Restores sidebar + bottom panel visibility | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: bottom panel visibility persisted |
| 29.5 | Settings persistence | Theme, editor settings, shortcuts saved | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 30. Keyboard Shortcuts Reference

| # | Shortcut | Feature | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 30.1 | `Ctrl+K` | Keyboard Shortcuts dialog | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.2 | `Ctrl+F` | Find | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.3 | `Ctrl+H` | Replace | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.4 | `Ctrl+G` | Find Next | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.5 | `Ctrl+Shift+G` | Find Previous | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.6 | `Ctrl+Shift+F` | Project Search | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.7 | `Ctrl+Shift+P` | Command Palette | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.8 | `Ctrl+Shift+I` | Format Document | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.9 | `F12` | Go to Definition | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.10 | `F5` | Run / Debug | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.11 | `Shift+F5` | Stop Debugging | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.12 | `F9` | Toggle Breakpoint | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.13 | `F10` | Step Over | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.14 | `F11` | Step Into | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.15 | `Shift+F11` | Step Out | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.16 | `Ctrl+F5` | Continue | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.17 | `Ctrl+.` | CodeAction Quick-Fix | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: Ctrl+. quick-fix (see 22.1) |
| 30.18 | `Tab` | Accept AI ghost text | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.19 | `Escape` | Dismiss ghost text / clear multi-cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.20 | `Alt+Click` | Add cursor | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.21 | `Alt+Shift+Click` | Column selection | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.22 | `Ctrl+O` | Open Project | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: Ctrl+O opens project; Open File is Ctrl+Shift+O |
| 30.23 | `Ctrl+S` | Save | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.24 | `Ctrl+Shift+S` | Save As | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.25 | `Ctrl+X` | Cut | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.26 | `Ctrl+C` | Copy | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.27 | `Ctrl+V` | Paste | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.28 | `Ctrl+Z` | Undo | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.29 | `Ctrl+Y` | Redo | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 31. Dialogs

| # | Dialog / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 31.1 | Open Project dialog | Native folder picker; returns selected path | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.2 | Open File dialog | Native file picker; filters by extension | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.3 | Save As dialog | Native save picker; default extension per language | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.4 | Add File/Directory dialog | Creates file/folder at selected tree path | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.5 | Delete confirmation | Asks confirmation; deletes recursively for dirs | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.6 | About dialog | Shows app name, version, license link | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: About shows version |
| 31.7 | License dialog | Shows MIT license text in scrollable view | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.8 | Run/Debug dialog | Dropdown of saved configs; fields for program, args, cwd, request type | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.9 | Plugin Manager dialog | Installed list + details + install/remove/toggle | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.10 | Theme preview | Immediate preview of selected family/mode/contrast | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 32. Cross-Cutting Concerns

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 32.1 | Localization / `tr()` | Strings wrapped in `tr()` for translation | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 32.2 | Theme — Light | Light palette applied to all widgets; layered QSS with hover states | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Modern QSS |
 | 32.3 | Theme — Dark | Dark palette applied to all widgets; layered QSS with hover states | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Modern QSS |
 | 32.4 | Theme — High Contrast | High-contrast accent colors applied; SVG icons used throughout | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Icon refresh |
| 32.5 | Permission prompts | Network, file, process permissions gated with user approval | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | | Fixed: permission approval dialog |
| 32.6 | Crash handler | Restarts previous session or clears state safely | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 32.7 | Auto-update | Detects newer version; downloads installer | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 32.8 | Updater settings tab | Configures update channel or disable | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 33. Plugin Features (Declared Metadata)

| Plugin ID | Declared Feature | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| `com.scriptura.aiinline` | `network.access` | HTTP POST to completion endpoint | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| `com.scriptura.httpclient` | `network.access` | HTTP requests sent | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| `com.scriptura.codeaction` | (none) | Uses host LSP only | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| `com.scriptura.sqliteviewer` | `file.read` | Opens `.sqlite` files | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| `com.scriptura.registry` | `network.access` | Downloads registry manifest + plugin archive | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| `com.scriptura.git` (builtin) | `VCSIntegration` | Git commands executed; output displayed | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 34. Closed Gaps

| # | Item | Description | Resolution |
|---|---|---|---|
| 34.1 | Git Pull / Fetch menu | Slots existed in `mainwindow.h` but not wired in UI | Fixed: `action_git_pull` and `action_git_fetch` added to `mainwindow.ui` Git menu; auto-connected via `connectSlotsByName` |
| 34.2 | Environment variable substitution | HTTP Client env dropdown was placeholder only | Fixed: `HttpClientPanel::substituteEnvVars()` applies `{{VAR}}` substitution to URL and body on send; defaults seeded with `BASE_URL`, `API_KEY`, `USER_ID` |
| 34.3 | Plugin Registry manifest | URL was hardcoded to placeholder GitHub path | Fixed: `registryUrl` persisted in QSettings under `plugin/registryUrl`; editable from Updates settings panel |
| 34.4 | AI provider auth | OpenAI-style auth parsing was missing | Fixed: Bearer token read from `ai/apiKey` QSettings key; `Authorization: Bearer <token>` header sent with every `AiInlineCompletion` request |
| 34.5 | New/Clone Window | Slots opened an external terminal instead of a Scriptura window | Fixed: `on_action_new_window_triggered` spawns a new `scriptura` process; `on_action_clone_window_triggered` passes `--project <dir>` plus open file paths so the new window clones project + files |
| 34.6 | Ctrl+W window close | Shortcut was not wired | Fixed: `Ctrl+W` shortcut added in `MainWindow` ctor, triggers `QWidget::close` (same path as X button) |
