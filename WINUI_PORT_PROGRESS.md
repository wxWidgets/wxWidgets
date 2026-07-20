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
