# wxWidgets → WinUI 3 port — progress

Status of the experimental `wxUSE_WINUI3` backend (`-DwxBUILD_TOOLKIT=winui`,
toolkit `__WXWINUI__`), an *overlay* port where every wx control keeps its real
Win32 `HWND` and is rendered by a WinUI 3 XAML island composited on top.

**Goal:** a drop-in replacement for wxMSW — same public API, same `wx*` styles,
same events and behaviour — only the rendering changes to modern WinUI 3
controls. The benchmark is `widgets.exe`: every parameter/checkbox on a control's
page must work the same as it did with the native Win32 control.

Percentages below measure **API/parameter integration** (not just "does it
render"). They are rough, hand-assessed estimates meant to prioritise work.

Legend: `█` done · `░` remaining

---

## Infrastructure

| Area | Progress | Notes |
|------|----------|-------|
| XAML island hosting (`wxWinUIControlHost`) | `█████████░` 90% | one island per control, focus hand-off, resize, z-order |
| Theming (light/dark, follow system) | `█████████░` 90% | `wxApp::SetAppearance` wired |
| Mica backdrop (auto on all TLWs) | `████████░░` 85% | transparent-erase technique; per-control opacity edge cases |
| Keyboard / tab navigation into islands | `████████░░` 80% | works; some focus edge cases remain |
| Per-window cursor (`SetCursor`) | `███████░░░` 70% | maps stock cursors to `ProtectedCursor` |
| Global busy cursor (`wxBusyCursor`) | `░░░░░░░░░░` 0% | **deliberately deferred** (XAML re-asserts its own cursor) |
| Tooltips (`wxWinUISetToolTip`) | `███████░░░` 70% | shared helper; adopted by button, more controls to wire |

---

## Buttons

| Control | Progress | Notes |
|---------|----------|-------|
| `wxButton` | `██████████` 95% | **reference implementation** — label, bitmaps + position/margins, alignment, markup, auth (UAC shield), default (Accent), `wxBU_EXACTFIT`, tooltip, font sizing |
| `wxBitmapButton` | `████████░░` 80% | rides on `wxButton`; per-state bitmaps wired |
| `wxToggleButton` / `wxBitmapToggleButton` | `███████░░░` 70% | renders; review states/params |
| `wxCommandLinkButton` | `████████░░` 80% | note line + chevron/shield; icon/crop just fixed |

---

## Static / display controls

| Control | Progress | Notes |
|---------|----------|-------|
| `wxStaticText` | `████████░░` 85% | multi-line + markup + mnemonics + alignment/ellipsize |
| `wxStaticBitmap` | `████████░░` 80% | mask→alpha transparency |
| `wxStaticBox` | `███████░░░` 75% | label masking over frame line |
| `wxStaticLine` | `█████████░` 90% | |
| `wxGauge` | `████████░░` 80% | determinate/indeterminate |
| `wxActivityIndicator` | `████████░░` 85% | |
| `wxHyperlinkCtrl` | `████████░░` 85% | |
| `wxInfoBar` | `████████░░` 80% | WinUI `InfoBar` |

---

## Input controls

| Control | Progress | Notes |
|---------|----------|-------|
| `wxCheckBox` | `███████░░░` 75% | 2-state + 3-state to review |
| `wxRadioButton` | `███████░░░` 75% | grouping to verify |
| `wxRadioBox` | `███████░░░` 70% | layout/dimensions params to verify |
| `wxChoice` | `███████░░░` 75% | per-item bitmap hook |
| `wxComboBox` | `███████░░░` 70% | editable + dropdown params |
| `wxBitmapComboBox` | `███████░░░` 70% | |
| `wxListBox` | `███████░░░` 75% | WinUI `ListView`; multi-sel/styles to verify |
| `wxCheckListBox` | `███████░░░` 70% | |
| `wxTextCtrl` | `███████░░░` 70% | single/multi-line; rich/styles partial |
| `wxSearchCtrl` | `███████░░░` 75% | `AutoSuggestBox` |
| `wxSpinButton` | `████████░░` 80% | horizontal WinUI style |
| `wxSpinCtrl` / `wxSpinCtrlDouble` | `███████░░░` 75% | `NumberBox` |
| `wxSlider` | `███████░░░` 75% | ticks/labels params to verify |
| `wxScrollBar` | `███████░░░` 75% | |

---

## Pickers / date-time

| Control | Progress | Notes |
|---------|----------|-------|
| `wxCalendarCtrl` | `███████░░░` 70% | `CalendarView` |
| `wxDatePickerCtrl` | `███████░░░` 75% | |
| `wxTimePickerCtrl` | `███████░░░` 75% | |
| `wxColourPickerCtrl` | `███████░░░` 70% | flyout `ColorPicker` |
| `wxFilePicker` / `DirPicker` / `FontPicker` | `░░░░░░░░░░` 0% | not started |

---

## Composite / container

| Control | Progress | Notes |
|---------|----------|-------|
| `wxNotebook` | `████████░░` 80% | `TabView`; page scrollbars handled via bridge height limit |
| `wxTreeCtrl` | `███████░░░` 75% | `TreeView`; selection-loop crash fixed, image transparency |
| `wxListbook`/`Choicebook`/`Toolbook`/`Treebook` | `███░░░░░░░` 30% | rely on base notebook/tree; SelectorBar idea pending |
| `wxListCtrl` / `wxDataViewCtrl` / `wxGrid` | `░░░░░░░░░░` 0% | still generic/native |
| `wxToolBar` | `░░░░░░░░░░` 0% | not started |
| `wxHeaderCtrl` | `░░░░░░░░░░` 0% | idea: WinUI `TableView` |

---

## Window chrome

| Area | Progress | Notes |
|------|----------|-------|
| `wxMenuBar` (frame menu) | `████████░░` 85% | WinUI `MenuBar`; check/radio items, dynamic rebuild, status-bar help |
| `wxStatusBar` | `████████░░` 80% | **new** WinUI island: fields + inset dividers, dynamic menu help, on Mica |
| `wxMenu` (popup) | `█████░░░░░` 50% | flyout building shared with menubar; standalone popup to verify |

---

## Dialogs (parallel effort — in tree, WIP)

| Dialog | Progress | Notes |
|--------|----------|-------|
| `wxMessageDialog` | `██████░░░░` 60% | `ContentDialog`-based, in progress |
| `wxTextEntryDialog` | `██████░░░░` 60% | in progress |
| `wxFileDialog` / `wxDirDialog` / others | `░░░░░░░░░░` 0% | not started |

---

## Not yet migrated (still native MSW / generic)

`wxToolBar`, `wxListCtrl`, `wxDataViewCtrl`, `wxGrid`, `wxHeaderCtrl`,
`wxFileCtrl` + file/dir/font pickers, `wxOwnerDrawnComboBox` / `wxComboCtrl`,
`wxEditableListBox` (intentionally left as-is), `wxCollapsiblePane`,
`wxBannerWindow`, `wxRearrangeCtrl`, `wxAnimationCtrl`, `wxPropertyGrid`,
`wxRichTextCtrl`, `wxStyledTextCtrl`, `wxWebView`, `wxMediaCtrl`, AUI, Ribbon.

---

## Next priority

Bring the already-rendering controls up to the **button's level of API
integration** — i.e. make every parameter on their `widgets.exe` page behave
exactly like the Win32 version. Suggested order (most-used first): checkbox,
radio button/box, choice/combobox, slider, gauge, text ctrl, then the pickers.

---

## Update — 2026-07-20 implementation pass

Landed since the review plan (untested — needs a full build + widgets.exe
pass):

- **Hygiene**: `wxUSE_WINUI3` documented in `setup.h` + defaulted/validated in
  `chkconf.h`; debug logging gated behind `wxUSE_WINUI3_DEBUG_LOG` (default 0);
  duplicated dialog helpers factored into `private.h`
  (`wxWinUIRemoveMnemonics`, `wxWinUISetDialogText`, shared
  `wxWinUIChoiceImpl`); dead code removed (radiobox ternary, statbar
  `RefreshAllFields`); menubar radio-group name collision fixed.
- **wxComboBox**: rewritten on top of wxChoice — editable text via inner
  TextBox (`TextProperty` callback), correct `wxEVT_COMBOBOX` + `wxEVT_TEXT`
  ordering, `wxEVT_TEXT_ENTER`, clipboard/undo forwarding, `Popup/Dismiss`,
  style handling.
- **wxSlider**: full `wxEVT_SCROLL_*` family (THUMBTRACK, THUMBRELEASE,
  CHANGED), line/page size mapped to Small/LargeChange.
- **wxTextCtrl**: real caret/selection tracking (SelectionChanged),
  `wxEVT_TEXT_MAXLEN`, `DoPositionToCoords`.
- **Quick wins**: spinctrl digits formatting + `wxEVT_TEXT`; search ctrl
  `wxEVT_SEARCH_CANCEL`; statline theme divider brush; timectrl locale
  12/24h; hyperlink custom colours; bitmap toggle button content; notebook
  tab icons; treectrl icons refresh on `SetImageList`.
- **Dialogs**: wxMessageDialog/wxTextEntryDialog made app-modal
  (`wxWindowDisabler`); **new wxColourDialog** (ContentDialog + ColorPicker,
  `wxEVT_COLOUR_CHANGED`, alpha support) replacing the Win32 common dialog.
- **wxTreeCtrl**: real hit-test/bounding rect from realized containers,
  right-click (`ITEM_RIGHT_CLICK`/`ITEM_MENU`), `wxEVT_TREE_KEY_DOWN`,
  in-place label editing (`EditLabel` + BEGIN/END events).
- **Popup menus**: `wxWindow::PopupMenu` now shows a WinUI `MenuFlyout`
  (transient island + nested loop, TrackPopupMenu-like deferred command
  dispatch, MENU_OPEN/CLOSE) with Win32 fallback.
- **Small gaps**: gauge `wxGA_VERTICAL`; datepicker `wxDP_ALLOWNONE`;
  calendar first-day-of-week + `DOUBLECLICKED`; listbox
  `GetTopItem`/`GetCountPerPage`/`EnsureVisible`; colour picker button
  `wxCLRP_SHOW_LABEL`.

Still open (next priorities): toolbar → CommandBar, list/table family
(`wxListCtrl`/`wxHeaderCtrl`/`wxDataViewCtrl`), font/find-replace dialogs,
search ctrl menu, tree drag & drop, scrollbar skinning, UIA accessibility,
`WM_DPICHANGED`, RTL, CI for the winui toolkit.

---

## Update — 2026-07-20 (2): phases 1 & 2 wrap-up

- **Builds again**: `wxcore` (toolkit winui, Debug) compiles; the
  too-strict `wxUSE_WINUI3`/`__WXWINUI__` consistency check in `chkconf.h`
  was removed (non-GUI libs get the platform defines only).
- **wxWinUIDialogIsland** (`private.h`/`ctrlhost.cpp`): shared helper for
  the ContentDialog dialogs (island setup, theming, app-modality, nested
  loop); wxMessageDialog/wxTextEntryDialog/wxColourDialog now use it.
- **wxTextEntryDialog** now creates a real (hidden) dialog HWND so it
  behaves like a normal wxDialog for the application.
- **wxFontDialog / wxFindReplaceDialog**: dispatched to the generic
  dialogs under the winui toolkit — they are rendered with the
  WinUI-ported controls, unlike the dated Win32 common dialogs
  (`src/msw/fontdlg.cpp`/`fdrepdlg.cpp` compiled out).
- **wxTreeCtrl drag & drop**: BEGIN_DRAG (must be Allow()ed, otherwise the
  WinUI drag is cancelled) and END_DRAG with the drop target from a real
  hit-test; WinUI auto-reordering stays off, the app performs the move.

### Phase 1 decisions

- `wxTE_RICH`/`wxTE_RICH2`: accepted but rendered with the plain TextBox
  (no RichEditBox backend for now) — behaviour equals a non-rich control.
- `wxSearchCtrl::SetMenu()`: the menu is stored but no drop-down UI is
  shown (AutoSuggestBox has no menu affordance); apps can use PopupMenu().
- Slider `wxSL_LABELS`/`wxSL_MIN_MAX_LABELS`: not available on the WinUI
  Slider (no built-in labels); ticks are supported via SetTickFreq.
- wxSingleChoice/MultiChoice/NumberEntry/Progress/About/Tip/BusyInfo: the
  wx generic dialogs are used as-is — they already render with the
  WinUI-ported controls, so no dedicated ContentDialog port is needed.

---

## Update — 2026-07-20 (3): phases 3 & 4

- **wxToolBar → CommandBar**: AppBarButton/AppBarToggleButton/
  AppBarSeparator built from the tools in Realize(); labels per
  wxTB_TEXT/wxTB_HORZ_TEXT, icons from the bitmap bundles, tooltips from
  the short help, radio-group handling, vetoable wxEVT_TOOL.  Vertical
  bars use a StackPanel of the same elements (CommandBar is
  horizontal-only).  Control tools not supported yet.  Unblocks
  wxToolbook.
- **Fluent wxRendererNative** (`src/winui/renderer.cpp`, installed from
  `wxWinUI3Initialize()`): flat hover-tinted headers with hairline
  divider, rounded selection pills, rounded focus outline, chevron tree
  expanders.  This themes the whole generic family at once — the
  pragmatic answer for phase 4.
- **wxRichToolTip → TeachingTip**: transient island + light-dismiss
  TeachingTip with dispatcher-timer timeout and self-owning lifetime;
  generic balloon as fallback.

### Explicit decisions

- **Scrollbars of wxScrolledWindow** stay native for now: bridging
  SetScrollInfo to scrollbar islands (or rehosting in a ScrollViewer)
  risks destabilising every scrolling window; revisit once the rest of
  the port has soaked.
- **wxListCtrl/wxDataViewCtrl/wxGrid remain the generic
  implementations**, now Fluent-themed by the renderer above.  A real
  ListView/ItemsView-backed wxListCtrl (virtualisation, report headers)
  and a virtualised wxDataViewCtrl are the next big chunks of work and
  need iterative on-screen testing, not a blind port.
- wxListbook (NavigationView), wxAuiNotebook (TabView),
  wxEditableListBox and wxFileCtrl: future work; their generic versions
  already render with the ported controls and themed renderer.

## Update — 2026-07-20 (4): RE-ARCHITECTURE — one island per top-level window

The per-control-island model above is **retired**. Every symptom it produced
(input synthesis per control, focus ping-pong, first-render sizing hacks,
Mica marking saga, dialog airspace, z-order fights, N fragmented UIA trees)
came from the same structural choice, so the hosting layer was rebuilt around
a single island per top-level window. This section is the architecture
reference; the older sections describe the historical model.

### Components

- **`wxWinUITopLevelHost`** (`include/wx/winui/private/tlwhost.h`,
  `src/winui/tlwhost.cpp`) — one per TLW, created lazily by the first hosted
  control. Owns the single `DesktopWindowXamlSource` (bridge covering the TLW
  client area, kept at the TOP of the child z-order — the composition band
  renders relative to sibling GDI in HWND z-order, so top is the only
  position where XAML is visible), a transparent root `Canvas`, the slot
  table, the geometry synchronizer, the input router and the focus arbiter.
- **Slots** — each hosted control keeps its real `HWND` (full wx/Win32
  compat: `GetHandle`, subclassing, sizers, wxDC for wx-drawn windows) and
  contributes one XAML element wrapped in a cursor-capable `Grid` container
  placed on the root canvas at the control's TLW-client position (DIPs),
  with pinned size, ancestor-chain `Clip` and wx-paint-order `ZIndex`.
- **Geometry sync** — coalesced flush (`CallAfter`) driven by
  `wxEVT_MOVE/SIZE/SHOW/DESTROY` on the slotted windows **and on their whole
  ancestor chains** (a child gets no event when an ancestor moves/hides);
  handles TLW resize, DPI change, freeze/thaw, HWND recreation and
  cross-TLW reparenting (slot migration).
- **`wxWinUIControlHost`** (`src/winui/private.h`, `src/winui/ctrlhost.cpp`)
  is now a thin proxy over the TLW host preserving the historical 7-method
  contract (`Initialize/SetContent/ForceRender/MeasureContent/
  SetBridgeHeightLimit/IsOk/Close`), so the ~30 control implementations were
  untouched by the migration. `MeasureContent` became a plain unconstrained
  `Measure` (elements are no longer size-pinned; their slot containers are).

### Input (deterministic island-first routing)

Hit-test pass-through (`WM_NCHITTEST` → `HTTRANSPARENT`,
`WS_EX_TRANSPARENT`, `ContentIsland::IsHitTestVisibleWhenTransparent`) is
**not reliable** under the pointer pipeline — measured empirically: the
input is dropped, not forwarded. The retained design assumes nothing:

- the root canvas has a `Transparent` background, so the island receives
  ALL the input;
- slot containers synthesize the wx mouse/focus events for their windows
  (`AddHandler(handledEventsToo=true)` — XAML controls mark pointer events
  handled) with a per-TLW focus arbiter (`FindFocus` resolution via the
  `DoFindFocus` hook in `src/msw/window.cpp`, Tab on
  `wxWinUI3ProcessTabNavigation` re-implemented over the arbiter, run
  BEFORE `ContentPreTranslateMessage` in `src/msw/evtloop.cpp`);
- where NO XAML content is hit, the root router forwards **real native
  messages** to the deepest wx window under the point: client and
  non-client alike, wheel included (so `DefWindowProc` bubbling scrolls the
  pages), button DOWNs sent synchronously so a target starting a native
  tracking loop (`SetCapture`) takes over the real input stream — which is
  what makes splitter/sash and selection drags native; `WM_MOUSELEAVE` is
  synthesized on hover-target changes, and the target's cursor is mirrored
  onto the island root (`ProtectedCursor`);
- the native scrollbar bands of the wx children are **cut out of the bridge
  window region** (`SetWindowRgn`): there the island does not exist at all
  and scrollbar tracking (thumb drag, arrow auto-repeat) is fully native.

### Transient surfaces

Popup menus (`wxWinUIPopupMenu`), `wxRichToolTip` (TeachingTip, anchored on
the target's slot) and the in-window dialogs (`ContentDialog`, now the
DEFAULT presentation — `WX_WINUI_DIALOG_WINDOW=1` restores the classic
separate window) all live on the shared island: one `XamlRoot`, correct
full-client dimming, and the open-popup rule makes the island capture all
input while one is up. Menu commands are deferred past the flyout close.
The transient `wxWinUIDialogIsland` and the per-control machinery
(`gs_winuiHosts`, per-island Mica, `HWND_BOTTOM` pinning, bridge height
surgery) are deleted.

### Painting

Unchanged in principle: DWM Mica + the `WM_ERASEBKGND` black fill for
windows carrying the `wxWinUIBackdropTransparent` prop — which is now also
**inherited on first erase from the ancestor chain**, so lazily created
windows (notebook pages...) can never show up with the stock light
background again. First-render sizing: content `Loaded` triggers ONE
globally-coalesced relayout pass per burst, which also sends a size event
to every scrolling window so their virtual sizes pick up the loaded
dimensions.

## Update — 2026-07-21: phase 7 hardening + dialog-bug forensics

### Dialog freeze/crash — established facts (hunt paused, not fixed)

Cross-referencing the dialog paths of widgets vs showcase pinned the bug
family down considerably:

- **"Set font" is NOT the native ChooseFont.** In this build wxFontDialog
  resolves to the GENERIC `wxGenericFontDialog` — a real wx modal dialog
  (class `#32770`, wx `choice`/`button` children) whose controls are hosted,
  so it gets its **own island** like any TLW. The freeze cluster is exactly
  "wx modal dialog with its own island" (generic font dialog, wxWizard).
  `wxFindReplaceDialog` is the native modeless one — no island, never
  freezes. The presenter dialogs (message box / text entry / colour as
  `ContentDialog` overlays on the frame island) never freeze either.
- **The freeze is not a hung pump and not a capture leak.** In the frozen
  state the process answers `WM_NULL`, `GetGUIThreadInfo` shows capture=0,
  the dialog is the active window and keyboard focus sits on the dialog
  island's `InputSiteWindowClass`. Real injected clicks on the dialog's
  Cancel are simply swallowed: input dies between the dialog's island and
  its slots.
- **A prior dialog in the session is required.** On a fresh process,
  WM_COMMAND-opening the font dialog and clicking Cancel works, repeatedly.
  After opening + Escape-closing a ContentDialog overlay first (colour),
  the NEXT font dialog is input-dead. This matches both user protocols:
  widgets' open/close/open, and showcase's immediate freeze (their click
  arrives through the island, after other dialogs/flyouts have run).
- **The delayed crash after closing a presenter dialog reproduces in
  showcase** with plain colour/font cycles — the process died spontaneously
  ~1 min after the dialogs closed, twice. It is NOT specific to "set
  tooltip"; that menu item is merely one way to run a presenter dialog.
- Mental model fixed: **wx pending events (CallAfter) DO run during native
  modal loops** — `wxIdleWakeUpModule`'s WH_GETMESSAGE hook processes them
  on every pumped message, whoever owns the loop. Deferred-exit lambdas can
  therefore interleave with the next modal dialog.
- Resume plan: run showcase under cdb from the start, replay the
  colour→font sequence (`freeze_sequence.ps1` in the session scratchpad),
  dump all stacks at the frozen state; then idle 2 min after closing
  dialogs to catch the delayed crash with a real stack.

### Phase 7 progress

- **Central UIA naming**: `wxWinUIApplyAutomationName` (tlwhost.cpp) sets
  `AutomationProperties.Name` from the wx label (mnemonics stripped, falls
  back to a meaningful window name) on every slot (re)registration, only
  when the element has no name of its own.
- **Default-name filter**: wx's library-wide default window names
  ("panel", "choice", "treeCtrl", "control"...) are never used as UIA
  names — a screen reader announcing a class label is worse than silence.
  Verified externally: the menubar and the pages tree no longer announce
  "control"/"treeCtrl", and the unnamed slot wrapper drops out of the UIA
  tree entirely.
- **wxTreeCtrl item names**: every generated `TreeViewItem` used to
  announce "Microsoft.UI.Xaml.Controls.TreeViewNode". The inner
  TreeViewList's `ContainerContentChanging` now names each
  realized/recycled container from the item text; the control template is
  force-applied at creation so the hook lands before the first
  realization, and `UpdatePeerItem` renames the live container when an
  item's text changes. Verified with an external UIA client: items
  announce 'Native', 'Generic', 'Pickers'...
- **Bridge region cuts now use `GetScrollBarInfo`** instead of style bits +
  `SM_CXVSCROLL` guessing: exact physical rect per bar, correct for RTL
  layouts (left-side vertical bars), correct under any per-monitor DPI, and
  currently-hidden bars no longer punch dead holes over the island.
  Verified live: widgets' bridge region is 4 rects with a 17px hole exactly
  over the one visible scrollbar; the log textctrl (WS_VSCROLL set, bar
  hidden) no longer cuts.
- **RTL — full geometry fix** (validated against the Win32 build of the
  dataview sample, "File > Layout direction"): `ScreenToClient` /
  `ClientToScreen` on a `WS_EX_LAYOUTRTL` window flip the X axis while the
  island Canvas stays physically LTR, which misplaced every slot (buttons
  overlapped and overflowed the frame). Every island<->screen conversion
  now goes through `GetClientScreenRect()` (physical client rect,
  normalized left<right): slot rects, ancestor clips, bridge region cuts,
  hit-test, `ScreenToRootDIP` and the root router's pointer forwarding.
  Slot containers also re-sync `FlowDirection` on every geometry sync, so
  a dynamic `SetLayoutDirection` (the sample toggles it at runtime)
  mirrors the hosted content — the log text control aligns right exactly
  like the native build. Input verified in RTL on both paths: row click
  through the native router (selection follows) and XAML Button click
  (Select ninth symphony moves the selection). Remaining cosmetic deltas
  are per-component items: the custom header's info icon sits on the
  wrong side of its label, and the dark-theme selected row hides its
  text (also true in LTR).
- **Backdrop priming re-evaluated and kept**: per-window one-shot resize
  nudge (prop-tracked); still required — DWM only composes Mica after a
  real resize step.

### Window-presentation dialogs: Mica RESOLVED (2026-07-21)

Two root causes, both structural:

1. **Late top-level windows never got the backdrop at all.**
   `wxWinUIApplyWindowBackdrop` ran from the app-theme broadcast (startup)
   and from `wxTLW::Show` — but the Show-time call runs AFTER the first
   paint, so dialogs opened with the stock light background ("the white
   resolves itself once you resize").  The host now applies the backdrop
   at island creation too (before Show), so any TLW hosting WinUI content
   is themed before it is ever painted.
2. **The "DWM never composes the backdrop on owned windows" conclusion
   was WRONG** — re-verified on the spike's F7 modal dialog: with the
   transparent marking and the per-window resize-nudge priming (which the
   original experiment predated), the material composes fine on owned
   top-levels.  The owned exclusion is removed: dialogs get the same Mica
   pipeline as main frames.  This also kills the mismatched black control
   patches, which were the transparent-erase children compositing over a
   fallback-solid dialog surface.

Verified on the spike (screenshots): default build now renders the modal
dialog with composed backdrop, accent-colored default button, normal
Fluent TextBox — identical to the main frame.  Left to confirm by hand on
widgets' generic font dialog / showcase.

### Input black box (freeze diagnosis tooling)

Set `WX_WINUI_INPUT_LOG=<file>` (or use `build-winui/run-instrumented.bat
[app]`) to trace the island input pipeline: root pointer stream (sampled
moves), capture-lost/canceled transitions, activation, every native
forward with its hit-test code, modal-loop hits.  When a freeze happens,
run `build-winui/collect-freeze.bat`: it dumps all thread stacks (cdb),
per-thread GUI state (focus/capture/moveSize) and copies the logs into
`build-winui/freeze-report/<timestamp>/`.  The spike gained modal
scenarios: F7 wx modal dialog (own island), F8 wxMessageBox (presenter
overlay), F9 native MessageBoxW.

### Update — 2026-07-21 (afternoon): modal-loop starvation SOLVED, dialogs
### are frame-class windows, cursor persistence, owned-TLW cost diagnosed

**Native modal loops starved the islands — fixed with the canonical
GetMessage hook.**  `ContentPreTranslateMessage` was only called from our
own event loop (`wxGUIEventLoop::ProcessMessage`), so any message loop we
do not control — the SC_MOVE/SC_SIZE loop while the user drags or resizes
a window, `MessageBoxW`, native menus — pumped the thread without ever
feeding the islands.  Visible damage: interactive resize left stale-pixel
onion-skin trails and ghost XAML, hosted dialogs went ~1 Hz, native
message boxes hovered at ~1 Hz.  Fix (`winui.cpp`): a thread-wide
`WH_GETMESSAGE` hook pre-translates every retrieved message — the
documented island contract for nested loops.  The hook deliberately skips
Tab so the wx tab-order logic keeps running first;
`wxGUIEventLoop::ProcessMessage` now pre-translates only that Tab (a
message must reach `ContentPreTranslateMessage` exactly once).
User-confirmed: main-window interactive resize finally smooth; measured:
native MessageBox hover went from ~2 visible updates in 2.6s to 57/57
distinct frames.

**wxDialog no longer uses the Windows dialog manager under WinUI**
(`toplevel.cpp`).  Dialogs were real `#32770` windows created by
`CreateDialogIndirect` with `DefDlgProc` in the wndproc chain; measured
against an identical frame-class window, every step of an interactive
move cost 6–80 ms in `WM_WINDOWPOSCHANGED` (against 0.2 ms) plus a
`WM_GETICON` storm.  wx uses none of the dialog-manager services
(navigation, default button, Enter/Escape are wx-side; the island handles
Tab/focus), so under WinUI the `wxTOPLEVEL_EX_DIALOG` branch now creates
a regular window of the wx class with the exact same styles (WS_POPUP,
WS_EX_DLGMODALFRAME instead of DS_MODALFRAME, RTL) and the same owner
(`MSWGetParent()` reproduces `GetParentForModalDialog()`).  Verified:
modal F7 and modeless F10 open/render/close correctly (class wxWindowNR,
owner set).  Hand-test still wanted: Enter/Escape/default button/initial
focus in the real samples' dialogs.

**Owned top-level windows make interactive move/size expensive —
diagnosed, clean fix still open.**  With the two fixes above, dialog
move/resize is only janky under compositor load (a game was running):
the thread's XAML tick collapses 165→~45 Hz while dragging an OWNED
window, and returns to full rate the instant `GWLP_HWNDPARENT` is
stripped (A/B/A verified live, user-confirmed "B works much better"; an
identical but UNOWNED second frame never collapses).  The dialog's own
Mica, its bridge region, the caret, the window class and the modal loop
were all ablated and are innocent.  A `WM_ENTERSIZEMOVE` owner-detach
mitigation was prototyped and REJECTED (dirty; does not cover hover) and
has been reverted.  Next lead: under real load, discriminate ownership
alone vs ownership×(owner backdrop) — `owner_mica_ab.ps1` strips the
OWNER's Mica while keeping ownership; needs to run while the machine is
actually loaded (local ring-based load generators were far too weak).

**Cursor no longer resets to arrow while the pointer rests** (two
mechanisms, `tlwhost.cpp`): (1) the island windows' CLASS cursor is
nulled — USER32 re-applies a class cursor directly (no WM_SETCURSOR)
whenever any window repositions under a resting pointer, wiping the
element cursor (I-beam only held while the mouse moved); (2) the
bridge subclass swallows WM_SETCURSOR arriving while the pointer has not
actually moved from the last island pointer event (±2 px — DIP
round-trip).  Residual, spike-only: a scene that RE-LAYOUTS continuously
under a resting pointer (the wanderer badge) still cycles the cursor via
XAML's own pointer replay; stop the wanderer (F4) and the cursor holds.
Real apps do not re-layout at rest.

**Spike tooling for all of the above** (`samples/winuispike`):
- WM_APP remote control, no focus/keyboard needed (the user's game can
  cover the screen): `WM_APP+1` toggle modeless dialog, `WM_APP+2`
  toggle second frame, `WM_APP+3` open the modal F7.
- F10 modeless-dialog and F11 second-frame scenarios (the pair that
  isolated ownership from class/modality).
- Render probe: an always-active ProgressRing + a
  `CompositionTarget.Rendering` ticks/s counter, dumped every second to
  `%TEMP%\spike-ticks.txt` — THE metric that quantified every collapse.
- Queue probe: WH_GETMESSAGE histogram (msg id × count × last target
  class) and a WH_CALLWNDPROC/RET sent-message cost meter (total/max ms
  per message id) in the same file.

### Update — 2026-07-21 (evening): owned-window drag lag FIXED
### (active-window owner detach), freeze family FIXED (phantom-replay
### breaker)

**Owned-window interactive lag — resolved.**  The final ablation matrix
(spike bench, reproducible without any external load): dragging an owned
island window runs the thread's XAML pump at 36-86 ticks/s whether or not
either window has Mica and WHOEVER the owner is (even a bare invisible
window of another process); only the unowned window is fluid (148-167).
So the ownership link itself is the per-step tax.  Fix (`window.cpp`,
WM_ACTIVATE, WinUI-only): the link is only ever needed while the window
is INACTIVE (that is when the parent could be raised above it), so the
owner is stashed in a window prop and cleared on activation, and restored
on deactivation.  Bench validation: detached while active, drag 139-165
ticks/s (from 36-86), boundary hover 163-165 (from 110-143), owner
restored on deactivation and re-stripped on reactivation.  Covers modal
and modeless dialogs uniformly, no per-case machinery.

### Update — 2026-07-21 (late): presenter delayed crash FIXED, owner-detach
### backdrop regression FIXED

**The "set tooltip" delayed crash is dead.**  Captured under cdb: an
access violation inside `~wxTextEntryDialog` invoked from the sample's
OnSetTooltip -- the WinUI wxTextEntryDialog's non-default constructor only
called Create(), which never initialized `m_validator` (nor m_textctrl /
m_maxLength / m_forceUpper), so the destructor deleted a garbage pointer:
an instant crash when unlucky, a silent heap corruption crashing seconds
later when not (hence "delayed", "random", and "absent with
wxMessageDialog" which has no such member).  Fix: default member
initializers in the class declaration (`wx/winui/textdlg.h`), covering
every present and future constructor.  The reproduction that crashed on
cycle 2 now survives 3 open/close cycles + soak.  The other winui headers
carry only uninitialized SCALARS (no owned-pointer deletes): to be
NSDMI-hardened in the per-component pass.

**Owner-detach regression on the backdrop, fixed the same hour it
shipped.**  The active-window owner detach (WM_ACTIVATE) changes
GWLP_HWNDPARENT, which makes DWM rebuild the frame and silently DROP the
composed backdrop: every owned dialog painted an opaque background
permanently (until a manual resize re-primed it).  Fix: after each owner
change (detach and reattach), clear the primed flag and re-run
`wxWinUIPrimeBackdrop`.  Verified: widgets' Choose Font renders dark/Mica
from the very first frame.

### Currently known issues
- Textareas often cropped in length (win32 controls were smaller; account
  for the taller WinUI metrics) — per-component pass.  Related: generic
  dialogs sized before the WinUI templates realize can crop controls
  (taller WinUI metrics); candidate fix is a grow-only re-fit of
  sizer-managed dialogs in the post-load relayout pass.
- Generic font dialog: label/groupbox strips and the preview area render
  with light-theme backgrounds over the dark Mica — per-component theming
  pass.
- The multiline TextBox clear (×) button renders enormous — per-component
  pass.
- Input-dead freeze family: FIXED (initial user confirmation).  The black
  box finally captured the mechanism end to end: a press whose implicit
  island capture gets canceled (every synchronously forwarded DOWN did
  that — `fwd DOWN` immediately followed by `POINTER-CANCELED` +
  `CAPTURE-LOST` in the log) which then ENDS OUTSIDE the window (press
  near a border, fast swipe out, release elsewhere) leaves the input site
  replaying one stale PointerMoved at a fixed out-of-bounds position
  (`root MOVE px=(-36,165) pressed=0`) tens of thousands of times per
  second.  The thread is NOT hung (it pumps at 100%, animations keep
  running) but the storm eats all input; the taskbar-preview recovery
  worked because the system menu sends WM_CANCELMODE.  Three-layer fix in
  `tlwhost.cpp`:
  (1) a run of identical-position moves — impossible from real hardware,
  the queue coalesces them — short-circuits all per-event work past 8
  repeats, starving the feedback loop before it amplifies (in practice
  this alone kills the storm: post-fix logs show it never reaches the
  detector);
  (2) at 256 repeats the same WM_CANCELMODE the taskbar recovery used is
  posted to the island's input windows automatically, re-issued
  rate-limited while the storm persists (defense in depth);
  (3) presses forwarded into system-modal loops (caption/border hit
  codes) or into native tracking loops (the send outlasted the physical
  press) proactively cancel the island's in-flight interaction, since the
  matching release can never arrive.
- Spike-only cosmetic: continuously animating slots under a resting
  pointer cycle the mouse cursor (XAML pointer replay), see above.
