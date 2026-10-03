# Plan 108: Close component parity gaps and version template dependencies

- Status: IN PROGRESS: concrete component failures and test-oracle corrections
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P2
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101; 105; 106; host changes affecting each family

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

### Required WinUI presentation (user clarification, 2026-10-03)

"Ported" now explicitly requires the WinUI/Fluent design for every component,
including data tables, wxGrid, wxDataViewCtrl/List/Tree, TreeList and Ribbon.
A generic GDI control merely hosted by WinUI, a Fluent-painted border, or a
standalone prototype with a separate model does not satisfy this requirement.
Keep compatibility tests for those existing implementations, but do not count
them as completed WinUI presentation. Preserve the public wx model, APIs,
events and custom renderer/art-provider contracts while changing presentation.

This clarification supersedes the historical V0 "do not rewrite in XAML" and
"post-beta" scope exclusions in components/10 and components/11. It does not
erase their risks or turn their old generic-runtime evidence into native-peer
evidence. Third-party component libraries are explicitly allowed by the user.

Dependency candidates checked against upstream sources on 2026-10-03:

- [Community Toolkit Labs Ribbon](https://github.com/CommunityToolkit/Labs-Windows/blob/main/components/Ribbon/samples/Ribbon.md)
  provides responsive command groups, but is still marked experimental.
- [Toolkit DataTable](https://github.com/CommunityToolkit/Labs-Windows/blob/main/components/DataTable/samples/DataTable.md)
  provides coordinated columns on ListView/TreeView. It is not a complete
  wxGrid replacement: built-in cell selection/editing, row headers, sorting and
  grid lines are explicitly absent. Its reuse must include those wx contracts.
- [XamlToolkit C++/WinRT port](https://github.com/lgztx96/CommunityToolkit.WinUI)
  includes Ribbon and DataTable under MIT. Its current source uses v145/C++23;
  the current wx build uses v143. WinRT binary consumption or a narrowly scoped
  compatible source integration must be proved before adoption. The published
  Labs Native package series is alpha, not production qualification.
- [WinUI.TableView](https://github.com/w-ahmad/WinUI.TableView) is an MIT,
  C#/.NET candidate with Fluent cells and richer table behavior. Direct
  consumption by this C++ backend is not established; do not introduce an
  implicit managed runtime dependency or call NuGet availability integration.

#### Dependency feasibility: native Toolkit package inspection

Read-only inspection on 2026-10-03 downloaded and decoded the exact NuGet
`XamlToolkit.Labs.WinUI.Native` **0.8.0-alpha** archive and its six Toolkit
dependencies in memory. No package was installed or loaded, no build ran,
and this is **not runtime PASS, adoption or production qualification**.

Primary sources: [versioned package](https://www.nuget.org/packages/XamlToolkit.Labs.WinUI.Native/0.8.0-alpha),
[exact Labs archive](https://api.nuget.org/v3-flatcontainer/xamltoolkit.labs.winui.native/0.8.0-alpha/xamltoolkit.labs.winui.native.0.8.0-alpha.nupkg),
and [source tree inspected at 5f0c861a46d2fd93ae2f6e9aba1ce6d4e8d1e2da](https://github.com/lgztx96/CommunityToolkit.WinUI/tree/5f0c861a46d2fd93ae2f6e9aba1ce6d4e8d1e2da).
The package nuspec does not record a source commit; do not assume that this
inspected source tree reproduces the published archive byte-for-byte.

- The Labs nuspec declares MIT and `Microsoft.WindowsAppSDK.WinUI` minimum
  `1.8.260224000`. The current wx pins are AppSDK runtime `1.8.260710003`,
  WinUI `1.8.260709004` and Foundation `1.8.260709000`; the WinUI pin satisfies
  the declared minimum, but compatibility still needs the activation gate.
- Labs contains native DLLs for x86/x64/ARM64, a 70656-byte `.winmd`, and a
  72944-byte `.pri`. The x64 DLL is 17209344 bytes. Its inspected PE export
  table includes `DllGetActivationFactory` and `DllCanUnloadNow`; its normal
  import table contains Win32/COM/UCRT, with no CLR/MSVCP/VCRUNTIME import
  observed. Consuming the WinRT ABI is the bounded candidate, not compiling
  the source tree's v145/C++23/module-based projects with our v143 toolchain.
- The actual package `.winmd` exposes `XamlToolkit.Labs.WinUI.Ribbon`,
  `RibbonGroup`, `RibbonCollapsibleGroup`, `RibbonPanel`, `DataTable`,
  `DataColumn`, `DataRow`, and `XamlToolkit.Labs.WinUI.XamlMetaDataProvider`.
  DataTable offers column-spacing/layout primitives, DataColumn offers
  resizing and desired width, and Ribbon offers an Items collection plus
  group/layout primitives. This is not a complete Grid/DataView or wxRibbon
  API implementation; the wx adapters and missing semantics remain required.
- The native MSBuild targets reference the `.winmd` and copy DLL/PRI payloads;
  the archive supplies neither generated C++ headers nor a standalone
  activation manifest. CMake consumption must generate projections, deploy
  payloads, establish unpackaged activation, and aggregate Toolkit metadata
  providers with the existing `XamlControlsXamlMetaDataProvider`. The current
  `wxWinUIApplication` forwards metadata queries only to the latter.
- The dependency closure includes all seven Toolkit packages below, each at
  `0.8.0-alpha`, plus **Microsoft.Graphics.Win2D 1.4.0** through Controls.
  Labs and Controls carry PRI files. The existing fingerprinted projection
  and resource-deployment contracts must include all required metadata,
  DLLs/resources and notices; copying only the Labs DLL is not qualification.

SHA256 values computed over the downloaded `.nupkg` bytes (not DLL hashes):

| Package, version 0.8.0-alpha | SHA256 |
|---|---|
| XamlToolkit.Labs.WinUI.Native | `A6B345F647D99931E93FBD20A555697D939642499BDA88D128FE797F69086ED2` |
| XamlToolkit.WinUI.Native | `76A6D0A2ABCB7D9F44511ECD0E62C82EFB25FC6BC079732FB11DA6AA2D9B49DF` |
| XamlToolkit.WinUI.Converters.Native | `D4593EC6564A66FBCF3654730B85A92AFE7AE395C3ED398AC7AE1EDE016CC7F5` |
| XamlToolkit.WinUI.Controls.Native | `82D0D3B8034FDDE4F3036B71C4F6F9DC3DB1D218C31F030BF8389BC7C93C52DE` |
| XamlToolkit.WinUI.Helpers.Native | `2A0CED054FF4337872963D0A6E5CEE568808CA53A9E9C1C148EE2FF166B185F3` |
| XamlToolkit.WinUI.Animations.Native | `FE3F7299E55CEBF07368FAEAE7C52F7580F7FC0EC99FECD9CD18F63A57B47AC8` |
| XamlToolkit.WinUI.Interactivity.Native | `3733AC07DDFF57EE76A569AED76F9803BD2DAAF0301EDF8897C349B97CFD8C65` |

#### Isolated native-consumption probe

`wxWINUI3_BUILD_TOOLKIT_PROBE` is OFF by default. Explicitly enabling it in
an existing MSVC WinUI test build adds only the excluded-from-all
`wx_winui_toolkit_activation_probe` target and its CTest entry. Its projection,
merged PRI, native DLLs and metadata are separate from wxcore and ordinary
sample/test payloads. It never installs an activation hook or registers COM
classes globally. No physical input is injected.

The probe loads DLL factories and Toolkit metadata providers, creates actual
Ribbon and table peers, checks their property contracts and Loaded geometry,
checks native control templates, and retains the existing XAML/DispatcherQueue
shutdown oracle. This is dependency feasibility, not the public wxRibbon or
wxGrid/wxDataView adapters and not production qualification.

The transitive Win2D 1.4.0 archive SHA256 is
`E13866088B3C50205AAE36E9E4CB3C23B21D11C4B4590DDCBEAAFA9EA2C79F48`.
Its nuspec is copied separately and is not relabelled with the Toolkit MIT
declaration. The official Win2D source provides an
[MIT license](https://github.com/microsoft/Win2D/blob/winappsdk/main/LICENSE.txt),
but package redistribution/provenance must still be reviewed before shipping.

In the existing shared build, configure with
`-DwxWINUI3_BUILD_TOOLKIT_PROBE=ON`, build only
`--target wx_winui_toolkit_activation_probe --config Release`, and run the
result in `winui/toolkit-probe/bin/Release` with
`--toolkit-activation-probe`. Prefer the existing private-desktop runner.
No experimental package is required by a default build or installed consumer.

2026-10-03 local result, x64/MSVC Release:

- Configure and the isolated target build pass with `/warnaserror`; all eight
  DLLs load and both Toolkit metadata providers activate.
- Ribbon, RibbonGroup and RibbonCollapsibleGroup pass native Loaded,
  finite/nonzero geometry and real-template checks (600x110, 130x100 and
  160x100 respectively).
- DataTable and DataRow are absent from both metadata lookup overloads. Their
  exact winmd projects an ordinary `IActivationFactory` constructor, so the
  probe separately exercises direct DLL activation for only these two Panel
  types, verifies their runtime class and interface, and reports
  `metadata_complete=false`. It does not call the original metadata gate PASS.
  DataColumn still uses the strict metadata/template path. The resulting table,
  two columns and row all load with nonzero dimensions and real column templates.
- **The consumption gate FAILS (exit 22):** `DataTable.ColumnSpacing(8)` reads
  back **1**, not 8, in the actual pinned binary. The two 220-pixel columns yield
  a 441-pixel row. The property oracle is retained; a successful presentation
  is not used to hide the failed contract. Dispatcher/XAML shutdown passes.
- Eight build-system self-tests pass, including five negative alpha-opt-in
  cases and an archive-verified synthetic positive. The normal smoke rebuilds
  with `/warnaserror`; smoke + Supported V0 remain 2/2 PASS in both shared and
  static builds. The static/default-OFF executable rejects the probe flag with
  exit 22 before initialization, as intended.

Logs in the shared build root: `audit108-toolkit-configure2-20261003.log`,
`audit108-toolkit-build3-20261003.log`,
`audit108-toolkit-activation3-20261003.log`,
`audit108-toolkit-selftests-all-20261003.log`, and
`audit108-toolkit-normal-gates-20261003.log`. Earlier strict metadata failures
remain in `audit108-toolkit-activation-20261003.log` and
`audit108-toolkit-activation2-20261003.log`. Static/default-OFF evidence:
`audit108-toolkit-off-configure-20261003.log`,
`audit108-toolkit-off-smoke-build-20261003.log`,
`audit108-toolkit-off-normal-gates-20261003.log`, and
`audit108-toolkit-off-refusal-20261003.log`.
Probe executable SHA256:
`0406E50D71C9FACBC183A835253947AD43C821CDB223744283B3A2C65AF6DF99`.
The ordinary shared library and runtime-smoke directories contain no Toolkit
files and retain the same PRI hash
`3BADA227F5AE4C0039A9E580DABB1703B2AF70F906CC2D3D8D2A7E78ABF07C91`.
The probe's distinct merged PRI is
`EA6D4F450415C3C66727AF89E375E69BC2992BCEC5FEE99B8305D1F3B31C3929`.

Decision: native consumption/presentation is feasible, but this alpha package
is **not accepted unchanged for production DataTable use**. Correct and rebuild
the third-party implementation with reproducible source provenance, or choose
a different native table implementation; do not bypass the property test.
Ribbon still needs public wx API integration and interaction qualification.

Implementation sequence after the current ToolBar lot:

1. Select and pin a reproducible native component boundary (version, source,
   hashes, notices, runtime payload and CMake/installed-consumer contract).
   Compare actual required functionality before importing a whole toolkit.
2. Connect the public wxRibbon command model to real WinUI peers, beginning
   with ButtonBar commands and then Bar/Page/Panel, Gallery, Toolbar,
   responsive/minimized states and keyboard navigation. The existing R2 test
   prototype is exploratory evidence only, not public wxRibbon implementation.
   Keep the actual `wxRibbonButtonBar::m_buttons` as the model; route native
   invocation through the same public event semantics as the generic path.
   Its host boundary must be private production code exported from wxcore to
   wxribbon, not direct slot registration copied from the test prototype.
3. Build a private native tabular surface with separate Grid and DataView
   adapters. The existing wx data model remains authoritative. First cover
   virtualized text rows/columns, model changes, selection and edit commit/
   cancel; then typed/custom renderers, DataView trees, Grid spans/frozen panes,
   column manipulation, clipboard/OLE and UIA Grid/Table semantics.
4. For each family, require public-API tests that also observe actual native
   peers, plus visual light/dark/high-contrast/DPI/RTL checks. Keep the existing
   semantic suites. A demonstration is not full API or production sign-off.

These are open implementation requirements, not completed work or a promise
that one third-party widget covers the entire wx API.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

The audits distinguish implementation from qualification and report concrete SpinButton focus divergence plus template coupling and incomplete owner-draw/style/metric contracts. Feature percentages are estimates, not acceptance criteria.

## Scope

src/winui control implementations and headers; generic/MSW fallback integration only where demonstrated; tests/controls; private template contract registry

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Work family-by-family: buttons/toggle/check/radio/static/feedback; text/search; choice/combo/list/checklist; gauge/slider/scroll/spin; date/time/calendar; notebook/tree/books; menus/toolbar/status/tooltips; generic/advanced fallbacks.
2. For every family test common API, programmatic silence/user event order, best-size/font/DPI, disabled ancestors, focus/UIA, RTL/theme/HC, retemplate/virtualization and reentrant destruction.
3. Fix SpinButton Tab/focus parity first. Cover bitmap margins/states, partial markup, owner-draw expectations, rich paragraph/bullet limitations, tree line/full-row styles, toolbar rows/wrapping and tooltip delay behaviour explicitly.
4. Centralize pinned template-part contracts for Search/Combo/List/Slider/Time/Calendar/Toolbar. Missing parts must produce a controlled supported fallback or precise failure, not heuristic accidental success.
5. Test generic composites separately from native peers: panels/scrolled/splitter, header/list/DataView/grid, combo/editable/rearrange/property/filesystem, richtext/STC/HTML, WebView/media/GL/ActiveX, AUI/Ribbon/MDI/native adoption, printing/shell. Classify spatial overlap restrictions precisely instead of declaring every native surface impossible.

## Verification and test plan

Add named family regression tags, run each focused test_gui filter plus unchanged Supported V0. Build minimal/widgets/dataview/showcase/dialogs/winuispike as their affected targets allow. Template-version tests must create actual peers after Loaded and cover replacement/missing-part fixtures; physical matrices remain separate.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Every family in the audit matrix has feature-level disposition and tests; implemented and physically qualified are distinct; no feature silently removed to turn a checklist green.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

No claim of native Ribbon, native titlebar or fully supported advanced surface based only on successful generic compilation. Hardware-dependent claims remain pending their gate. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Concrete failures captured during plan 106

The unchanged pre-migration shared/static binaries expose six failing cases. Preserve them while moving test accessors; do not call baseline equivalence component qualification.

- Two owner-drawn CheckListBox activation/focus cases reject `ActivatePeerCheck()` before `SetFocus()`. `GetItemRect()` only establishes the outer ListViewItem, not the CheckBox's arranged/hit-testable area. Inspect actual CheckBox geometry: the pinned theme's minimum dimensions exceed the compact margin requested by the projection. This is a hypothesis to measure, not yet a confirmed fix.
- TreeCtrl selection/expansion still requires its own diagnosis. The two historical DatePicker/composite focus failures from `c0f1d55a06` no longer reproduce on the current host baseline: the exact six-case predecessor prefix passes **201 assertions / 6 cases**, and both former targets pass in fresh full shared and static range replays. No product or fixture change is attributed without a current reproducer; physical focus qualification remains separate.
- TextCtrl's read-only peer mutation case reads a RichEditBox immediately after a real TOM write, before the asynchronous TextChanged callback restores the model value. Check the same bounded native-event wait used by adjacent tests, retaining model/peer/event assertions. No production repair is yet demonstrated.
- The restored shared range run adds a Calendar recycling/coalescing budget failure: `GetWeekRefreshRunCount() <= beforeNavigation + 6` reports **11 <= 10** at `winuirangedate.cpp:3693`. This was not in the initial baseline; classify its cause before attributing it to intermittence or the API relocation. The unchanged limit remains enforced. Log: `audit106-family5-restored-range.log` in the shared build tree.

Baseline logs and exact counts are recorded in plan 106. The isolated focus run is `audit106-family5-focus-isolated.log` with its passive input trace in the shared build tree. All use `WX_UI_TESTS=0`; none supplies physical input evidence.

## Read-only text and Calendar observation corrections

- TextCtrl's unchanged production RichEditBox rolls back a read-only peer write through its asynchronous TextChanged callback. The fixture now waits for that real callback, retaining the immediate model check and the original final peer checks. Four added assertions require bounded completion, the unchanged model value, read-only state and no wx text event. No synthetic notification or production setter substitutes for the callback.
- Calendar's `GetWeekNumber()` was not a passive observer: it forced rendering and called `RefreshWeekNumbers()`. Calling it repeatedly inside the reverse-navigation wait manufactured the refresh requests whose count the case was measuring. A non-installed, test-only `ReadProjectedWeekNumber()` now reads already projected rows after validating the live owner/peer; the existing active getter keeps its rendering/refresh/lifetime checks and delegates only its final lookup. The measurement uses the passive getter and seven added assertions check observation purity and capture navigation diagnostics. The original `beforeNavigation + 6` limit is unchanged.
- Shared and static Release `/warnaserror` builds pass. Each full text group passes **859 assertions / 28 cases**; each full 70-case range group passes **3007/3009 assertions, 68/70 cases**, with only the two previously recorded DatePicker focus failures. Calendar coalescing now passes within both broader groups, not only in isolation. Logs: `audit108-text-calendar-{build,text,range-runtime}.log` shared; `audit108-text-calendar-{build,text,range}.log` static. Smoke/Supported V0 also pass 2/2 in each linkage (`audit108-text-calendar-ctest.log`); a passing observation fix does not close the remaining component work. The fixes are published in `c0f1d55a06` and require their own remote CI confirmation.

## SpinButton focus and tab parity

- The standalone WinUI `wxSpinButton` now matches wxMSW's focus contract: `AcceptsFocus()` is false, neither native `RepeatButton` is a tab stop, and pointer interaction cannot transfer keyboard focus to either arrow. The obsolete preferred-focus publication was removed while the existing post-`SetContent()` generation/owner revalidation remains intact.
- The non-installed range test-access surface observes both arrows' `IsTabStop` and `AllowFocusOnInteraction` values. The native UIA topology test retains the Spinner root, Increment/Decrement identities and invocation checks while adding five focus-contract assertions.
- Before the change, `[winui-spin-model][spinbutton]` passed **45 assertions / 4 cases** in each linkage. After it, shared and static pass **50 assertions / 4 cases**; Release `/warnaserror` builds of `test_gui` and `minimal` pass, and runtime-smoke plus Supported V0 pass **2/2** in each linkage. Runs used private desktops with `WX_UI_TESTS=0`; they are native automation evidence, not physical keyboard sign-off.
- The override uses the existing virtual contract and adds no data member or new virtual slot. Shipping behavior changes only the intended focus policy; UIA invocation, event ordering, range normalization and reentrancy/lifetime guards are unchanged. Remote CI remains pending.

## Earlier range follow-up

Fresh full shared/static range replays confirm both historical DatePicker focus targets now pass on the current host baseline. The static replay instead exposes the separate Calendar coalescing budget at **15 <= 10**, finishing **3019/3020 assertions, 69/70 cases**. Keep that Calendar defect open; do not reclassify the former focus failures or claim physical focus qualification.

## Activation, compact CheckListBox and Tree focus (2026-10-03)

Source baseline: `ce00e7be04`; qualified source set:
`b05210bdd527372e1063ac12635f072dbe192247`.

- `d2b2138459` fixes an actual host activation race. Both DatePicker focus
  cases reproduce in the passive full range sequence (45/47 cases), despite
  passing together in isolation (71 assertions / 2 cases). The input trace
  shows native focus transfer activating the TLW, whose implicit last-child
  restoration supersedes the explicit requested child. Only this implicit
  restoration is now suppressed during the exact live, current bridge focus
  transfer. Application activation handlers and newer explicit requests still
  win. The new real-HWND regression covers both paths, real FocusManager state
  and a later ordinary activation; it is included in Supported V0 CI.
- `317b51c29a` removes the standalone CheckBox theme's 120x32 DIP minimum from
  compact list peers. Measurements show a requested 23x17 DIP owner-draw margin
  previously arranged at 120x32, and at 23x17 after correction. The initial
  failures also queried hit-testing before the native item entrance transition
  had settled. A private, non-installed observer now lets the fixtures wait
  for the actual CheckBox hit target, without focusing, toggling or injecting
  input. Added assertions check compact geometry and observer silence; all
  original focus, pointer-target and exactly-once event checks are retained.
  Diagnostic prints and the exploratory fixed delay are removed.
- `b05210bdd5` corrects the Tree fixture, not Tree production code. The
  list-to-tree sequence reproduced its initial `FocusPeerItem()` failure even
  after the host fix. It had never acquired wx/native control focus after
  preceding temporary frames. The fixture now deliberately activates a
  competing host, proves that precondition, and calls `multi.SetFocus()` once.
  The three strict item-focus/FocusManager checks and selection/key checks are
  unchanged. No logical-focus substitute or success retry is introduced.

Both existing WinUI Release build trees pass `test_gui` and `minimal` with
`/warnaserror`. Ordinary wxMSW shared Debug `wxcore` also compiles with
`/warnaserror`; no ordinary-MSW runtime behavior is changed. Both WinUI caches
remain `wxBUILD_TESTS=ALL`. All GUI results below use the existing private
desktop runner and `WX_UI_TESTS=0`; these are local native-automation results,
not physical input, external UIA, remote CI or soak qualification.

| Filter / gate | Shared | Static |
|---|---|---|
| New activation case + two owner-drawn CheckListBox cases | 95 assertions / 3 cases PASS | 95 / 3 PASS |
| All 21 exact `HostState`/`HostLifecycle` names containing `Focus` | 427 / 21 PASS | 427 / 21 PASS |
| `[winui-listmodel]~[physical],[winui-treemodel]~[physical]` | 392296 / 36 PASS | 392296 / 36 PASS |
| `[winui-range-date]~[physical]` | 2226 / 47 PASS | 2226 / 47 PASS |
| Historical extended range/date/spin/XRC/UIA filter below | 3020 / 70 PASS | 3020 / 70 PASS |
| CTest runtime smoke + Supported V0 (including new activation case) | 2/2 PASS | 2/2 PASS |

Exact extended filter:
`[winui-range-date]~[physical]~[gauge],[winui-range-events][slider],[winui-spin-model]~[spinbutton],XRC::WinUI 008c range and date controls,wxWinUI UIA RangeValue tracks the public numeric contract`.

Evidence in each existing build root:
`audit108-final-build-20261003.log`,
`audit108-focus-checklist-targeted-20261003.log`,
`audit108-host-focus-exact-20261003.log`,
`audit108-list-tree-qualified-20261003.log`,
`audit108-range-final-20261003.log`,
`audit108-full-range-qualified-20261003.log`, and
`audit108-runtime-gates-final-20261003.log`.
Failing sequence evidence remains in the shared
`audit108-list-tree-final-20261003.log` and
`audit108-list-tree-trace-run-20261003.log`, with passive native focus traces
in `audit108-list-tree-focus-trace-20261003.log` (shared) and
`audit108-range-focus-20261003.log` (static). Empty-filter exploratory runs are
not counted as successful tests.

Final binary SHA256 values:

| Artifact | SHA256 |
|---|---|
| Shared `test_gui.exe` | `384C360EAA50DCA11002ACD7023A6B5F5CB9E02EC436945AACFA37AE27F7FDC3` |
| Static `test_gui.exe` | `340BB6C3EE892B97E78E1A166A8A357376D18530D728FFC00595299F9D7B5384` |
| Shared `minimal.exe` | `EFCDA6900A6DC6790ABED768A705FBC84CFD5C5460A105318042F3AE4C846A03` |
| Static `minimal.exe` | `97B5D2E021D7F8BF7803494CA46B71A59610C6FEC30446B3998E0B83C51B47DC` |

Calendar's historical coalescing budget failure does not reproduce in either
current extended sequence. No speculative scheduler change is made, its
unchanged budget remains enforced, and the historical defect is not called
causally repaired. Other component families, physical qualification and
current-HEAD remote CI remain open.

### ToolBar rows: local shared/static qualification (2026-10-03)

Source: `24f4ee2a22f05782622dc03df498a573c124aba3`.
`SetRows()` now projects the public model into the retained native XAML Grid,
instead of inheriting the common no-op. Construction, overflow and best size
share one layout calculation. This includes ragged rows, vertical columns,
per-row RTL, embedded controls and hidden multiline flexible spacers.
Inherited `SetMaxRowsCols()` limits are accepted by a successful `Realize()`;
an impossible pinned-control/overflow capacity is rejected transactionally.
Published limits remain attached to their root/generation, so a failed Loaded
transaction cannot later rearrange the old root using rejected limits.
Rollback and direction changes publish arranged geometry before returning.

The original 23 cases pass unchanged. Five new public-API/native-peer cases
exercise row transitions, best/actual sizes, hit-testing, vertical/RTL and
inherited caps, embedded controls, overflow identity/state, failed Loaded
publication, nested mutation and destruction. The size fixture uses a plain
panel to avoid a frame's single-child auto-sizing and clears its explicit
construction minimum; geometry oracles are not relaxed.

Both existing Release trees compile `test_gui`, `toolbar` and `minimal` with
`/warnaserror`. Private-desktop results (`WX_UI_TESTS=0`), per linkage:

- `[winui-toolbar][rows]`: 595 assertions / 5 cases PASS.
- `[winui-toolbar]~[physical]`: 1696 assertions / 28 cases PASS.
- CTest runtime smoke and Supported V0: 2/2 PASS.

Evidence in each build root: `audit108-toolbar-rows-build3-20261003.log`,
`audit108-toolbar-rows-targeted3-20261003.log`,
`audit108-toolbar-rows-full3-20261003.log`, and
`audit108-toolbar-rows-gates-20261003.log`. Earlier build and failing-test
logs are retained; the initial const UIElement/FrameworkElement compile error,
RTL arrangement failure and rollback geometry leak are not hidden.

| Artifact | SHA256 |
|---|---|
| Shared `test_gui.exe` | `65C76A139190C479460D137AACA180D53170E1C678F6515C005913463AC30F50` |
| Static `test_gui.exe` | `B95FBD351A6DF5E6E894A14CE5AF21A623B3B468079DD188152360EC0CA7C0B5` |
| Shared `toolbar.exe` | `EE0A41BD4954A38CD5E03D18CECE7A7D3FF444FE0673D94F355D206FD7DD1638` |
| Static `toolbar.exe` | `8E817DC996BE155B5F785BDC788A9E3CC4A5155C8F7D537C37F8B56468CD63B9` |
| Shared `minimal.exe` | `EC210D148A2D64EBC15B33846DA4CA9C8934CF19C6EE7CBC752C576B2BF82D75` |
| Static `minimal.exe` | `D5AC8354D67506C3DF33A3E9D228114EF9430EBCC30D9A61B3AE76982B63AF2F` |

This closes the bounded row-layout gap locally, not all ToolBar contracts or
physical/remote qualification. Next: the opt-in native Toolkit activation and
resource proof described above, followed by real public Ribbon and tabular
adapters. Tooltip timing and the remaining family matrix are still open.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
