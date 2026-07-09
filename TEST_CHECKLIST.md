# Scriptura Test Checklist

> **Scope:** Every feature, menu action, button, shortcut, panel, plugin, and expected behavior.
> **OS columns:** Tick `[x]` if working, `[ ]` if not. Use the paired ✗ column only for known regressions on that OS.

---

## 1. Application Lifecycle

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 1.1 | Launch `scriptura` | Starts without crash; shows Welcome screen | [x] | [ ] | [ ] | [ ] | [x] | [ ] | |
| 1.2 | Launch with `--help` / invalid arg | Prints usage or handles gracefully | won't repo | | | | | | |
| 1.3 | Window resize | Resizes editor, sidebar, and bottom panel proportionally | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 1.4 | Window maximize | All panels fill screen; state restored on reopen | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 1.5 | Window close (Ctrl+W / X button) | Prompts to save modified files; exits cleanly | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 1.6 | New Window (`menu_terminal -> New Window`) | Spawns second independent Scriptura window | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 1.7 | Clone Window (`menu_terminal -> Clone Window`) | Spawns window with same project + open files | [x] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 2. Menu Bar — File

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---------|---|---|---|---|---|---|
| 2.1 | Open Project (`Ctrl+O`) | Opens folder picker; loads directory tree into sidebar | [x]     | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 2.2 | Open File | Opens file picker; opens file in new editor tab | [ ]     | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 2.3 | Save (`Ctrl+S`) | Saves current file; clears modified indicator | [ ]     | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 2.4 | Save As (`Ctrl+Shift+S`) | Opens save dialog; writes to new path; updates tab title | [ ]     | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 2.5 | Add File/Directory | Context menu in file tree; creates file or folder | [ ]     | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 2.6 | Delete File/Directory | Deletes selected path; refreshes tree; confirmation dialog | [ ]     | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 3. Menu Bar — Edit

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 3.1 | Cut (`Ctrl+X`) | Removes selection to clipboard | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.2 | Copy (`Ctrl+C`) | Copies selection to clipboard | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.3 | Paste (`Ctrl+V`) | Inserts clipboard content at cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.4 | Undo (`Ctrl+Z`) | Reverts last edit | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 3.5 | Redo (`Ctrl+Y`) | Re-applies undone edit | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 4. Menu Bar — Terminal

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 4.1 | New Window | Opens new empty Scriptura window | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 4.2 | Clone Window | Opens new window duplicating current session | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 5. Menu Bar — Git

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 5.1 | Commit | Opens commit dialog; runs `git commit` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 5.2 | Push | Runs `git push`; shows output in Git panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 5.3 | Pull (`on_action_git_pull_triggered`) | Wired in `mainwindow.ui`; auto-connected via `connectSlotsByName` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Fixed |
| 5.4 | Fetch (`on_action_git_fetch_triggered`) | Wired in `mainwindow.ui`; auto-connected via `connectSlotsByName` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Fixed |

---

## 6. Menu Bar — Search

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 6.1 | Find (`Ctrl+F`) | Shows Find bar; highlights matches; Find Next/Prev work | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 6.2 | Replace (`Ctrl+H`) | Shows Replace bar; replace / replace-all work | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 6.3 | Project Search (`Ctrl+Shift+F`) | Shows Project Search panel; searches files; opens result | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 6.4 | Command Palette (`Ctrl+Shift+P`) | Shows palette; filters commands; executes on Enter | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 7. Menu Bar — Code (LSP)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 7.1 | Format Document (`Ctrl+Shift+I`) | Formats via LSP; replaces document text or range | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 7.2 | Show Symbols | Displays document symbols list | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 7.3 | Go to Definition (`F12`) | Jumps cursor to symbol definition | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 8. Menu Bar — Debug

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 8.1 | Run / Debug (`F5`) | Shows Run Dialog; launches DAP session | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.2 | Stop Debugging (`Shift+F5`) | Stops debuggee; cleans up breakpoints | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.3 | Toggle Breakpoint (`F9`) | Adds/removes breakpoint on current line | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.4 | Continue (`Ctrl+F5`) | Resumes execution; debugger stops on next breakpoint | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.5 | Step Over (`F10`) | Steps over current line; stops at next | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.6 | Step Into (`F11`) | Steps into function call | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.7 | Step Out (`Shift+F11`) | Steps out of current function | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 8.8 | Conditional Breakpoint | Breakpoint dialog accepts condition; only hits when true | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Requires DAP server support |
| 8.9 | Attach to Process | Run Dialog "Attach" mode lists PIDs; attaches debugger | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Platform-dependent PID listing |
| 8.10 | Debug Console | Evaluates expressions in Debug panel; shows result | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 9. Menu Bar — Tools

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 9.1 | HTTP Client | Opens HTTP tab in bottom panel; send requests | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 9.2 | SQLite Viewer | Opens Database tab; browse tables and run queries | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 9.3 | AI Completions (toggle) | Enables/disables inline ghost text completions | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 10. Menu Bar — Settings

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 10.1 | About | Shows version and info dialog | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.2 | Editor Settings | Opens Settings tab with editor controls | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.3 | Theme | Opens Theme tab; preview updates live | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.4 | License | Shows license text | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 10.5 | Check for Updates | Fetches latest version; shows notification if available | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 11. Menu Bar — Plugins

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 11.1 | Manage Plugins | Opens Plugin Manager dialog with Installed + Registry tabs | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 12. Top Toolbar Buttons

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 12.1 | Sidebar Toggle (☰) | Uses SVG icon; collapses/expands left file tree drawer | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.2 | Go Up (↑) | Uses SVG icon; navigates file tree to parent directory | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.3 | Find Replace Bar | Visible when Find/Replace activated; input functional | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 12.4 | Theme Button (☼) | Uses SVG icon; opens theme selector | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 12.5 | Settings Button (⚙) | Uses SVG icon; opens editor settings tab | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |

---

## 13. Sidebar Buttons (Icon Bar)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 13.1 | File Tree (☸) | SVG icon; toggles file tree visibility | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.2 | Todo (⚑) | SVG icon; switches main view to Todo panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.3 | Terminal (>_) | SVG icon; switches main view to Terminal panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.4 | Problems (⚠) | SVG icon; opens bottom Problems panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |
| 13.5 | Git (⎇) | SVG icon; opens bottom Git panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Updated to icon |

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
| 14.7 | Theme Settings tab | Color family, mode, high-contrast, preview | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.8 | Editor Settings tab | Font, tab width, etc. | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.9 | Keyboard Shortcuts tab | Displays shortcut reference | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 14.10 | Updates tab | Update checker UI | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 15. Editor Tab Widget

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 15.1 | Open file via tree | File opens in new tab; content loads | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.2 | Switch tabs | Editor content switches to selected file | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.3 | Modified indicator | Asterisk or dot appears on unsaved tab | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.4 | Tab tooltip | Shows full file path on hover | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 15.5 | Tab close (middle-click) | Closes tab | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 16. Bottom Panel Tabs

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 16.1 | Problems tab | Shows LSP diagnostics list; filterable | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.2 | Git tab | Shows git output, diff, branches, merge | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.3 | Search Results tab | Shows project search results | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.4 | Debug tab | Shows stack, scopes, variables, watches | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.5 | HTTP tab | Shows HTTP Client panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 16.6 | Database tab | Shows SQLite Viewer panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 17. Editor Surface

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 17.1 | Syntax highlighting | Language-specific colors for C/C++, Python, Java, JS, TS, Rust, Go, Shell, HTML, CSS, Script, plain text | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.2 | Line numbers | Gutter shows line numbers; updates on scroll/edit | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.3 | Indent guides | Vertical guides at tab stops | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.4 | Current line highlight | Current line highlighted on focus | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.5 | Breakpoint gutter | Red dot on breakpoint line | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.6 | Debug execution line | Current debug line highlighted distinctively | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.7 | Inlay hints | Parameter names / type hints rendered inline | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.8 | Ghost text (AI) | Translucent suggested text after cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.9 | Diagnostic underlines | Red squiggly / highlight on LSP errors | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.10 | Hover diagnostic tooltip | Shows error message on mouse hover | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.11 | Multi-cursor (Alt+Click) | Adds secondary cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.12 | Column selection (Alt+Shift+Click) | Rectangular selection | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.13 | Minimap | Overview strip rendered right of editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.14 | Breadcrumb | Shows file path / symbol path above editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.15 | Split horizontal | Editor splits left/right | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.16 | Split vertical | Editor splits top/bottom | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.17 | Close split | Returns to single editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.18 | Accept ghost text (Tab/Enter) | Inserts AI suggestion into document | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.19 | Dismiss ghost text (Escape) | Removes AI suggestion without inserting | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.20 | Typing clears ghost text | Any typed character invalidates pending suggestion | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.21 | Scroll with ghost text | Ghost text stays anchored to cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.22 | Cursor position status | Status bar shows line:column | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.23 | Tab width | Tab stops match configured width (default 4) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.24 | Word wrap | Toggle word wrap wraps long lines | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 17.25 | Auto-save | Modified files saved on timer or focus change | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 18. LSP Features

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 18.1 | LSP server start | Starts automatically on recognized file type | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.2 | LSP diagnostics | Errors/warnings appear in Problems panel | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.3 | Completion | Ctrl+Space or auto-trigger shows suggestion list | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.4 | Hover | Mouse hover shows documentation popup | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.5 | Go to Definition | Jumps to definition; opens file if needed | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.6 | Find References | Lists all references in project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.7 | Rename | In-place rename across project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.8 | Document Symbols | Tree of symbols in current file | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.9 | Workspace Symbols | Search across project symbols | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.10 | Formatting | Formats entire document | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.11 | Signature Help | Shows parameter hints during function call | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.12 | Declaration | Jumps to declaration site | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.13 | Type Definition | Jumps to type definition | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.14 | Implementation | Jumps to implementation | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 18.15 | Code Actions (quick-fix) | Ctrl+. shows fix bar; clicking fixes diagnostic | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 19. Debugger (DAP)

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 19.1 | Run/Debug dialog | Lists saved configurations; Add/Delete works | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.2 | Launch configuration | Starts debuggee; stops on first line or breakpoint | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.3 | Stop Debugging | Terminates debuggee; clears debug state | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.4 | Continue | Resumes until next breakpoint | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.5 | Step Over | Steps one line; enters functions if configured | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.6 | Step Into | Steps into function | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.7 | Step Out | Steps out to caller | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.8 | Breakpoint set (F9) | Visual indicator in gutter; DAP `setBreakpoints` sent | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.9 | Conditional breakpoint | Entered condition sent to DAP server | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.10 | Stack trace panel | Shows call frames on stop | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.11 | Scopes panel | Shows local/global/closure variables | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.12 | Variables panel | Expandable variables with types/values | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.13 | Watches panel | Add watch expressions; values update | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 19.14 | Debug Console evaluate | Evaluates expression in current frame | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 20. AI Inline Completions

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 20.1 | Enable toggle (Tools menu) | Checked state enables/disables inline completions | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.2 | Ghost text render | Translucent text appears after cursor after debounce | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.3 | Accept (Tab/Enter) | Ghost text inserted into document | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.4 | Dismiss (Escape) | Ghost text removed without insertion | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.5 | Typing clears ghost | Any printable character clears pending suggestion | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.6 | Cursor move clears ghost | Clicking elsewhere invalidates suggestion | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.7 | Ollama backend | `POST` to endpoint returns completion in ghost text | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Requires Ollama running |
 | 20.8 | OpenAI-compatible backend | `POST /v1/chat/completions` returns completion; authenticates with `ai/apiKey` Bearer token if set | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 20.9 | Debounce | Requests throttled to configured ms (default 400) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 21. HTTP Client Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 21.1 | Method selector | GET/POST/PUT/DELETE/PATCH/HEAD/OPTIONS | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.2 | URL input | Type URL; supports `Ctrl+Enter` or Send button | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.3 | Send button | Dispatches request; shows response | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.4 | Environment dropdown | Selects env preset; `{{VAR}}` substitution applied to URL/body on send | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Implemented |
| 21.5 | Auth — None | No auth headers sent | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.6 | Auth — Basic | `Authorization: Basic <base64>` header sent | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.7 | Auth — Bearer | `Authorization: Bearer <token>` header sent | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.8 | Request body editor | Sends JSON body when method supports payload | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.9 | Timeout spinner | Request aborts after selected ms | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.10 | Response Headers tab | Lists raw response headers | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.11 | Response Body tab | JSON bodies rendered as key/value table or raw text | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.12 | Copy header value (double-click) | Copies header value to clipboard | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.13 | HEAD request body | Body tab shows "No body for HEAD request" | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 21.14 | Status label | Shows HTTP status code + elapsed ms | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 22. CodeAction UI

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 22.1 | Ctrl+. shortcut | Shows/hides quick-fix bar for current diagnostics | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.2 | Quick-fix bar rendering | Shows count label + action buttons | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.3 | Preferred action highlight | Preferred fix shown with highlighted style | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 22.4 | Click action button | Emits actionTriggered; hides bar (wiring to apply fix is TBD) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 23. SQLite Viewer Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 23.1 | Open SQLite button | File picker filters `.sqlite`, `.db`, `.sqlite3` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.2 | Table list | Lists tables + views from `sqlite_master` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.3 | Double-click table | Loads rows into data grid | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.4 | Ad-hoc SQL query | Enter SELECT query; press Enter; shows result grid | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.5 | Refresh button | Re-reads schema / re-runs query | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.6 | Error display | SQL errors shown in status label | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.7 | NULL values | NULLs rendered as text "NULL" | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 23.8 | Connection cleanup | DB closed when panel destroyed or new DB opened | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 24. Plugin Registry & Auto-Update

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 24.1 | Registry URL | Configurable via Updates settings panel; persisted with QSettings | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Implemented |
| 24.2 | Check for updates | `GET` manifest JSON; emits `registryUpdated` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.3 | Weekly timer | `checkForUpdates()` called on timer (mock in code) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.4 | Install plugin from registry | Downloads payload; emits `pluginDownloaded` | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.5 | Upgrade detection | `upgradeAvailable()` compares versions | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.6 | Plugin Manager — Installed tab | Lists installed plugins + state | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.7 | Plugin Manager — Install Plugin | Installs from directory | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.8 | Plugin Manager — Remove Plugin | Deletes plugin files | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.9 | Plugin Manager — Disable/Enable | Toggles plugin state without deletion | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 24.10 | Plugin hot-reload | Reload updates plugin without full restart | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 25. Git Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 25.1 | Git output tab | Shows command stdout/stderr | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.2 | Commit button | Commits staged changes with message | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.3 | Push button | Pushes to remote | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.4 | Pull button | Pulls from remote | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.5 | Fetch button | Fetches remote refs | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.6 | Stage / Unstage | Changes staging area in working tree view | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.7 | Diff tab | Shows unified diff of selected file | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.8 | Branches tab | Lists local + remote branches | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 25.9 | Merge tab | Three-way merge view (ours/theirs/result) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 26. Terminal Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 26.1 | Shell start | Launches default shell (`bash`, `zsh`, `powershell`, etc.) | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.2 | Command input | Typed commands execute in shell; output rendered | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.3 | Command history | Up/Down arrows cycle previous commands | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.4 | Clear button | Clears terminal output | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.5 | Working directory | Updates on `cd`; synchronized with project root | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.6 | ANSI color codes | Colors rendered in output | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 26.7 | Shell exit | Terminal shows exit code; can restart | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 27. Problem Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 27.1 | Problems list | Shows diagnostics grouped by file | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.2 | Filter — All | Shows errors, warnings, info | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.3 | Filter — Errors | Shows only errors | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.4 | Filter — Warnings | Shows only warnings | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.5 | Filter — Info | Shows only information diagnostics | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.6 | Activate problem | Double-click jumps to line in editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.7 | Close button | Clears problem list view | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 27.8 | Count label | Shows number of problems in current filter | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 28. Todo Panel

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 28.1 | Todo items list | Displays parsed TODO comments from open document | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 28.2 | Click todo item | Jumps to corresponding line in editor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 29. Workspace / State

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 29.1 | Recent projects | Persisted across restarts; click to reopen | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.2 | Recent files | Persisted per project; reopenable | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.3 | Window geometry | Restores size + position on launch | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.4 | Panel layout | Restores sidebar + bottom panel visibility | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 29.5 | Settings persistence | Theme, editor settings, shortcuts saved | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 30. Keyboard Shortcuts Reference

| # | Shortcut | Feature | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 30.1 | `Ctrl+K` | Keyboard Shortcuts dialog | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.2 | `Ctrl+F` | Find | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.3 | `Ctrl+H` | Replace | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.4 | `Ctrl+G` | Find Next | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.5 | `Ctrl+Shift+G` | Find Previous | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.6 | `Ctrl+Shift+F` | Project Search | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.7 | `Ctrl+Shift+P` | Command Palette | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.8 | `Ctrl+Shift+I` | Format Document | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.9 | `F12` | Go to Definition | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.10 | `F5` | Run / Debug | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.11 | `Shift+F5` | Stop Debugging | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.12 | `F9` | Toggle Breakpoint | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.13 | `F10` | Step Over | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.14 | `F11` | Step Into | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.15 | `Shift+F11` | Step Out | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.16 | `Ctrl+F5` | Continue | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.17 | `Ctrl+.` | CodeAction Quick-Fix | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.18 | `Tab` | Accept AI ghost text | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.19 | `Escape` | Dismiss ghost text / clear multi-cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.20 | `Alt+Click` | Add cursor | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.21 | `Alt+Shift+Click` | Column selection | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.22 | `Ctrl+O` | Open Project | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.23 | `Ctrl+S` | Save | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.24 | `Ctrl+Shift+S` | Save As | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.25 | `Ctrl+X` | Cut | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.26 | `Ctrl+C` | Copy | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.27 | `Ctrl+V` | Paste | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.28 | `Ctrl+Z` | Undo | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 30.29 | `Ctrl+Y` | Redo | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 31. Dialogs

| # | Dialog / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 31.1 | Open Project dialog | Native folder picker; returns selected path | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.2 | Open File dialog | Native file picker; filters by extension | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.3 | Save As dialog | Native save picker; default extension per language | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.4 | Add File/Directory dialog | Creates file/folder at selected tree path | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.5 | Delete confirmation | Asks confirmation; deletes recursively for dirs | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.6 | About dialog | Shows app name, version, license link | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.7 | License dialog | Shows MIT license text in scrollable view | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.8 | Run/Debug dialog | Dropdown of saved configs; fields for program, args, cwd, request type | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.9 | Plugin Manager dialog | Installed list + details + install/remove/toggle | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 31.10 | Theme preview | Immediate preview of selected family/mode/contrast | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

---

## 32. Cross-Cutting Concerns

| # | Feature / Control | Expected Behavior | Linux ✓ | Linux ✗ | macOS ✓ | macOS ✗ | Windows ✓ | Windows ✗ | Notes |
|---|---|---|---|---|---|---|---|---|---|
| 32.1 | Localization / `tr()` | Strings wrapped in `tr()` for translation | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
 | 32.2 | Theme — Light | Light palette applied to all widgets; layered QSS with hover states | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Modern QSS |
 | 32.3 | Theme — Dark | Dark palette applied to all widgets; layered QSS with hover states | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Modern QSS |
 | 32.4 | Theme — High Contrast | High-contrast accent colors applied; SVG icons used throughout | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Icon refresh |
| 32.5 | Permission prompts | Network, file, process permissions gated with user approval | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 32.6 | Crash handler | Restarts previous session or clears state safely | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 32.7 | Auto-update | Detects newer version; downloads installer | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |
| 32.8 | Updater settings tab | Configures update channel or disable | [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | |

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
