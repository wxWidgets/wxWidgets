# wxWinUI V0 manual qualification checklist

This checklist is the human gate for the V0 profile. Automated test success is
necessary but does not replace these observations. Record `PASS`, `FAIL`,
`BLOCKED` or `N/A (excluded)` for every row; attach the exact component/page,
steps and screenshot/log path to any non-PASS result.

Do not run this checklist while another person is using the desktop: it
deliberately moves focus, changes display/theme settings and uses physical
pointer/keyboard input.

## Test record

| Field | Value |
|---|---|
| Date/operator | |
| Source revision + dirty diff identifier | |
| Build directory/configuration | |
| Windows edition/build | |
| Windows App Runtime version | |
| MSVC / Windows SDK / CMake | |
| Architecture/linkage/identity | `x64 / shared / unpackaged` |
| GPU/driver | |
| Displays, scale and arrangement | |
| Keyboard layouts / IME | |
| Accessibility Insights version | |
| Narrator version/voice | |

## Application smoke

Launch each executable from its own deployed output directory. Confirm there
is no resource warning, hang, unexpected console/error dialog or dependency on
another sample's files.

| Application | Launch/close | Main rendering | Input/focus | Notes/evidence |
|---|---:|---:|---:|---|
| `minimal.exe` | | | | |
| `widgets.exe` | | | | |
| `showcase.exe` | | | | |
| `dataview.exe` | | | | |
| `dialogs.exe` | | | | |

## Supported component pages

For every page/class marked Supported in
`plans/winui3-v0/component-matrix.md`, exercise the following contract. List
the page/class and result rather than recording one aggregate “looks OK”.

| Component/page | Render + best size | Enable/show | Pointer/wheel/capture | Keyboard/Tab/mnemonic | Tooltip/cursor | Scroll/Z/clip | Result/evidence |
|---|---:|---:|---:|---:|---:|---:|---|
| | | | | | | | |

Also verify each supported mutable property twice (initial value and hot
change), the disabled path, destruction while an event handler is active and
default-construction followed by `Create()` where the API permits it.

## Menus and accelerators

| Scenario | Expected | Result/evidence |
|---|---|---|
| Frame menu accelerator (for example Ctrl+T) | One command, no focus dependency | |
| Alt mnemonic opens menu | Correct top menu and highlight events | |
| Arrow/Enter/Escape navigation | Native ordering, one close event | |
| Disabled/check/radio item | State and command suppression agree | |
| Menu rebuilt while callback runs | No stale command or crash | |

## Drag and drop

Use a dedicated scratch file and reversible text payload.

| Scenario | Expected | Result/evidence |
|---|---|---|
| Text drag over WinUI-backed control | enter/over/drop routed to wx target once | |
| File drag from Explorer | matching files, coordinates and effects | |
| Move between two sibling slots | leave then enter, no duplicate session | |
| Destroy/reparent target during drag | safe cancellation/re-resolution | |
| Internal Tree drag | Tree events only; no accidental OLE duplicate | |
| Native/generic child under bridge | original native target remains authoritative | |

## Dialogues and transient UI

Test both the default Window presentation and explicit Overlay mode where the
matrix marks Overlay Experimental/Supported.

| Scenario | Expected | Result/evidence |
|---|---|---|
| Message/Text/Password dialogs | initial focus, default button, Enter/Escape | |
| Font/Find/Colour dialogs | no crop after template realization | |
| Parent minimize/restore and Alt-Tab | owner/grouping and modality preserved | |
| Dialog opens nested dialog | correct owner, focus restored in order | |
| Parent/dialog destroyed during callback | no late callback or orphan | |
| Modeless dialog active during parent action | enabled/owner state restored | |

## Theme, contrast, RTL and DPI

Perform changes while the samples remain open; do not treat a restart as the
hot-change gate.

| Scenario | Expected | Result/evidence |
|---|---|---|
| Light → Dark → Light | theme resources update, no duplicate handlers | |
| High Contrast on/off | system colours and visible focus preserved | |
| LTR → RTL → LTR | content mirrors, physical slot coordinates remain correct | |
| 100 → 150 → 200% display move | no double scaling or stale bitmap bundle | |
| Return to original display | original size/selection/focus restored | |
| Menus/Tree/Notebook/Toolbar bitmaps | resolution changes without model rebuild | |

## Multi-top-level and reparent

Create two frames, ideally on displays with different scale factors.

| Scenario | Expected | Result/evidence |
|---|---|---|
| Reparent one supported control A → B → A, 100 cycles | one slot/peer, state retained | |
| Reparent focused control | logical/native/XAML focus follows once | |
| Reparent control with tooltip/drop target | authority migrates without stale callback | |
| Destroy source immediately after migration | destination control survives | |
| Destroy destination during migration callback | safe rollback/no leak | |
| Open owned dialog from each frame | correct owner and task grouping | |

## Accessibility Insights and Narrator

Use the expected topology in `docs/winui/accessibility-topology.md`.

| Scenario | Expected | Result/evidence |
|---|---|---|
| Mixed frame tree snapshot | one logical fragment per control, no HWND shell duplicate | |
| Dynamic labels/help/state | Name/HelpText/state update without recreating control | |
| Invoke/Toggle/Selection/Value/RangeValue | action occurs once; disabled action refused | |
| Tab and Narrator scan order | agrees with wx logical order | |
| Menu, dialog, list and tree announcements | role/name/state/value are meaningful | |
| Reparent/destroy with retained provider | old provider reports unavailable | |

## Freeze and soak

Run only opt-in harness modes documented by `samples/winuispike/README.md`;
normal applications must have no render-ring/histogram instrumentation.

| Scenario | Duration/budget | Result/evidence |
|---|---|---|
| Repeated pointer enter/leave/move/capture | 15 min, no freeze/storm | |
| Scroll/splitter/notebook overlap | 15 min, measured latency within ADR 0003 | |
| Dialog/menu/transient churn | 15 min, no orphan/focus loss | |
| Mixed widgets/showcase use | total soak at least 60 min | |

## Sign-off

- [ ] All Supported rows are PASS.
- [ ] Every `N/A` maps to an explicit Experimental/Fallback/Excluded matrix row.
- [ ] Every failure has a reproducible issue and owning plan.
- [ ] Accessibility Insights snapshot is attached.
- [ ] Narrator pass is signed by the human operator.
- [ ] OS/runtime/display metadata above is complete.
- [ ] No instrumentation remained enabled after the run.

Operator/signature:

Date:

Qualification verdict:
