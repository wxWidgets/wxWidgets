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
