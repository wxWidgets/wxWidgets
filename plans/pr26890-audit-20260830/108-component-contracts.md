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

Implementation sequence (user clarified that native container design is
required, not just WinUI command buttons):

1. Select and pin a reproducible native component boundary (version, source,
   hashes, notices, runtime payload and CMake/installed-consumer contract).
   Compare actual required functionality before importing a whole toolkit.
2. Integrate actual Toolkit containers before extending the custom command
   adapters: one native wxRibbonBar root, a WinUI page selector, a Toolkit
   `Ribbon` per wxRibbonPage, and `RibbonGroup`/`RibbonCollapsibleGroup` per
   wxRibbonPanel. Toolkit has no page/tab API; its `RibbonPanel` is an internal
   layout panel, not the wx panel semantic equivalent. Compose ButtonBar roots
   into `Group.Content`, not independent slots overlaid above the group. This
   requires a private composite mounting boundary preserving logical wx owners,
   callback generations, rollback, native geometry and reparent/destruction.
   Keep the wx command model authoritative and preserve veto/silent-setter
   semantics. Collapsed state is not flyout-open state; ShowExpanded/HideExpanded
   need a versioned template adapter or upstream API. Then complete ToolBar,
   Gallery, responsive states and keyboard navigation. Deferred ToolBar drafts
   are saved, inactive and unqualified; they do not count as native presentation.
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
physical/remote qualification. The subsequent opt-in Toolkit activation proof
is recorded above, and the first public native Ribbon command sublot below.
Tabular adapters, tooltip timing and the remaining family matrix are still open.

### Public Ribbon ButtonBar native projection (2026-10-03)

The actual `wxRibbonButtonBar::m_buttons` now drives native command peers in
its production shared-host slot. Normal/dropdown/hybrid primaries use
`AppBarButton`; toggles use `AppBarToggleButton` and native Checked/Unchecked
notifications; dropdown/hybrid carets use a native Button. The public model
and stable per-item keys remain authoritative, including duplicate command
IDs and reuse of an ID after deletion. Native callbacks hold weak owner and
generation/revision identities, not raw layout-instance pointers across
application handlers. Setters and `UpdateWindowUI` are silent projections.

The existing common adaptive layout still selects/group sizes, but all three
size classes are measured from native command peers. Public item rectangles
and menu anchors use the existing visual-coordinate adapter. No generic
ButtonBar background/button painting is layered over the XAML peers. The
source-private ControlHost and conversion helpers are shared by wxcore and
wxribbon; their header is not installed. CMake configures wxribbon with the
already pinned C++/WinRT projection. No Toolkit package or managed runtime
is added to shipping libraries or samples.

Native content callbacks also exposed an existing host scheduling defect:
`SyncSlot()` immediately requeued an in-progress content transaction. If its
Content callback entered `wxYield()`, that queued flush requeued itself and
prevented the suspended transaction from finishing. The transaction's existing
commit/rollback boundary already schedules the required catch-up. In-progress
sync now leaves that scheduling to the finish boundary, without a new timer,
hook or bypass. This causal host correction is part of the command-integration
lot, and needs the existing Content/Flush regressions in addition to Ribbon.

The host fix is committed separately as `61e3cc7246`. In both existing Release
trees, all 19 exact `HostState`/`HostLifecycle` cases whose names contain
`Content` or `Flush` pass (279 assertions per linkage), including the extended
`ContentSwapFlushSeesOnlyPublishedModel` case which now pumps `wxYield()` at
the native setter boundary. Evidence: `audit108-ribbon-content-host-exact-20261003.log`
in each build root. The earlier wildcard selector matched no cases and is not
qualification. The failing Ribbon nested-loop run is retained as
`audit108-ribbon-native-targeted3-20261003.log` (240-second timeout); its passive
UI-thread stack is `audit108-ribbon-native-reentry-stack-20261003.log` in the
shared build root. The subsequent transaction case completes; separate Ribbon
qualification is recorded below.

ButtonBar layout publication is transactional too: candidate native sizes,
common adaptive layouts, active layout, offsets and best/minimum sizes remain
unpublished until the host accepts their matching root. A native Content
callback can fail the swap or resize/yield without leaking rejected geometry.
Viewport changes are separate from command-model revisions and receive a
bounded catch-up after publication.

Cross-TLW reparenting also exposed an incorrect caller rollback: a resize
callback can reach `SetContent()` while host migration temporarily refuses
registration. The host already preserves the previous publication on failure;
unconditionally trying to restore it again could fail and close a valid peer.
Rollback now restores native content only when this operation's candidate is
actually the host's current publication. The original refusal remains a
failure, and the explicit `Realize()` after migration succeeds normally.
The causal trace is retained in the shared build root as
`audit108-ribbon-reparent-trace-20261003.log`; temporary diagnostics are removed
from the final sources. No test oracle or timeout was relaxed.

This is the first public native command sublot, **not complete native Ribbon
parity**. Bar/Page/Panel/ToolBar/Gallery presentation, physical keyboard and
menu loops, full DPI/RTL/theme/font behavior, tooltip timing/disabled-help
behavior and custom art-provider integration remain open. In particular,
custom `DrawButtonBarButton()` drawing is no longer used for these native
commands: calling `SetArtProvider()` still affects the generic parent shells,
but this lot does not establish a native equivalent of every custom drawing
contract. Do not describe that limitation as completed compatibility.

#### Local qualification of the native ButtonBar sublot

Source: `aca65d6f15d69845c65b3818516735239ff3450a`, following the separately
qualified host fix `61e3cc7246`. No Toolkit binary dependency is adopted.
Both existing WinUI Release build trees compile `test_gui`, `ribbondemo`,
`showcase` and `xrcdemo` with `/warnaserror`. Ordinary wxMSW Debug `wxribbon`
also compiles with `/warnaserror`; its presentation remains unchanged.

Private-desktop results (`WX_UI_TESTS=0`), per linkage:

- Native command group: 352 assertions / 5 cases PASS. Covers actual native
  peers and UIA events, silent setters/UpdateUI, menu routing/anchors, atomic
  layout rollback, nested resize/yield, retired providers, ID reuse,
  reentrant destruction and cross-TLW reparenting.
- Full `[winui-ribbon]~[physical]` group: 3874 assertions / 17 cases PASS,
  retaining the original public-contract and 100-cycle stress cases.
- Runtime smoke and Supported V0: 2/2 PASS.
- New `wx_winui_ribbon_native` CTest executes the five native cases with a
  bounded private-desktop runner. Both local CTest runs pass; the Windows
  shared/static CI workflow now selects this gate separately from Supported
  V0 and archives its log/XML. Its remote execution is still pending.

Build logs: shared `audit108-ribbon-native-build8-20261003.log`, static
`audit108-ribbon-native-build7-20261003.log`, and ordinary MSW
`audit108-ribbon-native-build4-20261003.log`, each in its existing build root.
Both WinUI roots contain `audit108-ribbon-native-ctest-20261003.log`,
`audit108-ribbon-native-full-final-20261003.log` and
`audit108-ribbon-native-gates-final-20261003.log`. Earlier compile failures,
reparent failures and the nested-loop timeout are retained, not overwritten.

Final binary SHA256 values:

| Artifact | SHA256 |
|---|---|
| Shared `test_gui.exe` | `07564A5A16306EAB787925F61B27B6E946698761E9CAC8CBD589971F4CE6FCA4` |
| Static `test_gui.exe` | `8E381DA232F71C7737AD80BAB93B675977697ECFE1FB2A6495CD9B24EE645E6F` |
| Shared `ribbondemo.exe` | `49E3CBB3BF40BF2079E8C86E0B7C73E861C91871FEC86F957C9F0FABC75D0C44` |
| Static `ribbondemo.exe` | `188E429C5CE91E82C4BF6D1AC3FA119A7E030FBC6FBCDC865D152FB29C67C41D` |
| Shared `showcase.exe` | `38F2671BE4B9C1AA443807ABA896306CECE625E732BC4133FC1690704FEFEE7C` |
| Static `showcase.exe` | `9247B895CF27C9956BD49395978266F22B4A0666B84D2324E2DC13A505A069E8` |
| Shared `xrcdemo.exe` | `FD849361CDF55AF057E172A44344C2EB6AAABA4ED8F68FEF5EF914FB4A0DE953` |
| Static `xrcdemo.exe` | `14CFEA7E32319CB4EBB83107085DF9D7C321E3645599BEA2FAD69018F7A2DAB5` |
| Shared Ribbon DLL | `6DA4FF47C4AD7B53D9DE9284589D01026F71511435846E66F256E62DDDFFB92A` |
| Static Ribbon library | `A1E80FBA161CBD19632F76C7C7920F493FAA6FAA92997E1356ED29972CB16D62` |

Rebuilt demos are under `F:\wxwinui-pr26890-feedback-build\lib\vc_x64_dll`
and `F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`. Their successful
build is not a human visual sign-off. Physical input, external UIA, full native
Ribbon presentation and the remaining component matrix are still open.

### User-observed mixed rendering on HDR displays (2026-10-03)

The user reports that non-WinUI content fails to render on HDR screens in the
rebuilt `ribbondemo.exe`, while native WinUI content remains visible. The
attached screenshot also shows incomplete/clipped Ribbon presentation. This
is a real outstanding user report, not an established causal HDR diagnosis:
compare the same binary on SDR/HDR and inspect the shared island/GDI
composition boundary before selecting a fix. Do not dismiss it merely because
the private-desktop semantic tests pass. No display setting has been changed.

The user meant the existing `showcase.exe` gallery, which has now also been
opened on request; its Ribbon is on the Advanced page. Do not overwrite shared
DLLs while either user-operated demo remains running.

The subsequent Showcase photographs broaden this report to RichText,
StyledText, HTML and other classic surfaces. The current investigation is
cross-component, not a Ribbon-only rendering patch. The full-client DWM frame,
inherited black GDI background erasure and full-client XAML bridge are the
candidate shared boundary. The old code comment claiming reliable HDR/SDR
rendering has been removed; it was not qualification evidence.

A process-scoped `winui.backdrop` system option now selects the existing solid
fallback transaction when set to 0. It retires transparent markers before
requesting removal of DWM material/frame extension, preserves application
colours and light/dark policy, and cannot override High Contrast. It does not
change HDR, display settings, native control selection or default behaviour.
Showcase's **View > Request Mica backdrop** checkbox allows a same-window A/B
on the Advanced page. The title/log report the backdrop marker state, not a
pixel-level proof of DWM success. An off-mode improvement would isolate the
backdrop protocol, not by itself prove an alpha-only cause; persistent failure
still requires checking shared-island clipping/airspace. Automated policy
tests are not visual HDR sign-off. Compilation and new regression results are
recorded below only after execution.

The user completed that A/B on the HDR display: classic components reappear
with the checkbox off. This isolates the backdrop/composition protocol, but
does not identify a particular DWM internal implementation. The user explicitly
requires keeping Mica. No automatic HDR/Advanced Color material-disable policy
has been implemented; the checkbox is diagnostic, not the proposed resolution.

The next candidate repairs GDI output alpha at the shared wxPaintDC cache
boundary, after nested buffered-DC blits have finished. It is restricted to
background-owning (`wxBG_STYLE_PAINT`) GDI content under material; native XAML
slots and the material splitter sash are excluded. Empty paints must not
publish an opaque rectangle, black content must stay black/opaque, and partial
invalidations and RTL must keep their coordinates. This preserves the DWM
material and its frame extension. HWND-native controls not using wxPaintDC
and renderers bypassing GDI bounds need separate qualification; this candidate
is not a claim that every composition contract is closed.

The Mica-preserving candidate is now built in the existing **shared** tree.
`wxPaintDCInfoOur` uses a top-down UxTheme buffer for the qualifying GDI
surface, flushes GDI before touching its bits, and makes the drawn bounds opaque
without changing their RGB. Empty handlers do not publish a buffer. Mirrored
window update coordinates are kept distinct from physical bitmap coordinates;
the cached UxTheme bitmap may be larger than the current partial repaint.
The non-installed observer checks the buffer before publication. The native
comparison uses the same direct/buffered callback with backdrop markers
temporarily removed, so an existing wxMSW RTL bitmap rounding difference is
measured rather than hidden with a new coordinate adjustment.

Executed qualification (2026-10-03):

- Shared `test_gui` and `showcase`, Release `/warnaserror`: PASS;
  final source build log `audit108-mica-gdi-alpha-build14-20261003.log`
  and Showcase target log `audit108-mica-gdi-alpha-build12-20261003.log`.
- `[opaque-gdi],[shell-theme],[winui-ribbon][native-buttonbar]` on a private
  desktop with `WX_UI_TESTS=0`: **841 assertions / 21 cases PASS** in
  `audit108-mica-gdi-alpha-qualified-tests-20261003.log`.
  This includes 210 assertions for black/red RGB and alpha, direct/nested
  buffered painting, cached DC reuse, empty paints, partial updates, RTL and
  unchanged material markers/native-slot/splitter exclusions.
- Ordinary wxMSW `wxcore`, Debug `/warnaserror`: PASS in
  `audit108-mica-gdi-alpha-msw-qualified-20261003.log`.
- No physical input, monitor settings, Mica default, or HDR policy was changed.
  No automatic opaque-shell fallback was added. The static A/B binary has not
  been rebuilt with this candidate (the static test executable has, as recorded
  below).

Candidate for the user's **Mica-on HDR** check:
`F:\wxwinui-pr26890-feedback-build\lib\vc_x64_dll\showcase.exe`.
SHA-256: `A501CA0F1C8C5D8CD59BE6DBF41798B03410DE526CA4EF42FF342A6A5BCB5126`;
its rebuilt core DLL is
`C9705107B0E8841D6DDC5B5B92A3E9F6227DF06A7D02E0044C16EBAF5E776BCB`.
**User HDR verification (2026-10-04):** using this candidate with Mica still
requested, the user confirms that Advanced-page classic components remain
visible and that the HDR-specific defect is fixed. The EXE and core DLL hashes
above were rechecked unchanged. This is the user's observation on their
display, not a claim of universal HDR qualification.

The same check identifies a separate, pre-existing defect: moving/resizing
the window can intermittently erase classic content even on SDR, and a
refresh restores it. This repaint/invalidation defect remains open and is
being addressed without disabling Mica or altering the HDR policy.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

## Unconnected Toolkit runtime helper and script tests (2026-10-03)

`build/cmake/winui-toolkit-runtime.cmake` now provides explicit preparation,
target configuration and executable deployment functions for the same seven
Toolkit 0.8.0-alpha packages and Win2D 1.4.0 pinned above. It is **not connected
to the shipping build**. Including it alone performs no package acquisition,
projection generation, deployment or target modification. Preparation is
cache-only and rejects missing or damaged extractions; it does not download
or repair them. It reuses the existing extraction verifier, projection worker,
PRI worker and runtime-deployment ownership/locking mechanisms.

The explicit preparation arguments are `ACCEPT_EXPERIMENTAL_PACKAGES` and
`ACCEPT_WIN2D_PACKAGE_LICENSE`. The latter is required by the pinned Win2D
package's actual `requireLicenseAcceptance=true` metadata, not imposed on the
Toolkit MIT packages. Original nuspecs and a factual declaration inventory are
included in the proposed payload. Additional local notices are optional and
must match their supplied SHA256 when specified. These checks and notices are
**not proof of complete redistribution compliance**.

The accompanying `tests/winui/selftest/toolkit-runtime.cmake` was executed with:

```powershell
cmake -DWINUI_CMAKE=F:/wxwinui-pr26890-feedback/build/cmake/winui.cmake -DTOOLKIT_RUNTIME_CMAKE=F:/wxwinui-pr26890-feedback/build/cmake/winui-toolkit-runtime.cmake -DWORK_DIR=F:/wxwinui-pr26890-feedback-build-static/winui/selftest/toolkit-runtime -DSAFE_ROOT=F:/wxwinui-pr26890-feedback-build-static/winui/selftest -P F:/wxwinui-pr26890-feedback/tests/winui/selftest/toolkit-runtime.cmake
```

Result: **22 cases PASS** (19 expected rejections and three positive groups),
approximately 1.3 seconds. Coverage includes inactive/repeated inclusion,
valid cache reuse, missing cache, extraction/archive corruption without repair,
RID/WinUI/PRI requirements, unknown/incomplete options, package-license
declarations and Win2D acceptance, optional-notice hashes, unsafe paths,
conflicting preparation, missing targets and reuse of an already-prepared
context. The latter uses a seeded context: it does not establish successful
first-time creation of real CMake targets.

No compiler, build, download, MakePri/C++/WinRT generation, graphical test or
physical input was used by these script tests. They do **not** prove a real
final payload copy, installation/export support, Toolkit activation from the
production application, or composition of wxRibbon Bar/Page/Panel containers.
The helper's target wiring, merged-PRI deployment and real Ribbon mounting
still require separate integration evidence; this result does not close the
native Ribbon presentation requirement.

## Application-owned component metadata foundation (2026-10-03)

The real `wxWinUIApplication` now delegates its two `GetXamlType` overloads and
`GetXmlnsDefinitions` to the source-private `src/winui/xamlmetadata.h` registry.
The ordinary WinUI provider remains first. Component registration publishes a
complete immutable provider snapshot, rejects null batches without partial
publication, deduplicates repeated providers and namespace declarations, and
preserves a lookup's snapshot during reentrant registration. It does not replace
the application or install a global activation hook.

The application owns the providers. The runtime retains only a weak registry
reference and its application identity; registration is rejected outside the
running owner thread or for an application not owned by this implementation.
The existing application retirement/quarantine boundary therefore also owns
the registry. This is not a public plugin API. A future native component adapter
must retain its implementing modules for the full XAML epoch.

Static Release `/warnaserror` build and native tests: **LOCAL PASS**.

- `audit108-component-metadata-build3-20261003.log` in the existing static build
  records the final `test_gui` build. Earlier compile/test failures are retained:
  the WinRT test implementation cannot be `final`, and the fixture now uses
  component-owned TabView metadata rather than intrinsic TextBlock metadata.
- `audit108-metadata-mica-ribbon-tests-20261003.log`: **883 assertions / 24 cases**
  pass for `[winui-component-metadata],[opaque-gdi],[shell-theme],`
  `[winui-ribbon][native-buttonbar]`, with `WX_UI_TESTS=0` on a private desktop.
  The component metadata subset is **42 assertions / 3 cases**, including
  actual application resolution and wrong-thread rejection. The opaque-GDI
  subset remains **210 assertions / 1 case**.
- Dedicated bounded CTests `wx_winui_component_metadata`, `wx_winui_opaque_gdi`
  and `wx_winui_selftest_toolkit_runtime` pass in
  `audit108-component-contract-ctest-20261003.log`. The MSW workflow now selects
  these gates for each WinUI linkage; no remote run is claimed.
- Fresh installation into the existing
  `F:\wxwinui-pr26890-audit104-install-static` and rebuilt external consumer
  in `F:\wxwinui-pr26890-audit104-consumer-static` pass the full runtime smoke
  in 44.32 seconds (`audit108-component-metadata-runtime-20261003.log`). This
  covers repeated epochs, MTA/owner-thread policy, clean startup recovery,
  renderer ownership, quarantine faults and a deliberately wrong fault-mode
  oracle. Source and installed static core hashes match:
  `0DDAF43BA16CAB7F4539A7C06CBB7257157EA91C285EE9CC9BDD2A0AEF782908`.
- Static `test_gui.exe` SHA256:
  `A1264509ED105173927ADDBD19B4719B18F71F2F16D7DE3F20E6E40B309552EC`.
  The user's shared Showcase and its core DLL above were not replaced.

The new provider registry is **not yet qualified in the shared build** (kept
intact for the user's HDR test). The runtime smoke checks existing lifecycle
invariants after this change, not teardown of an activated Toolkit component.
The runtime helper is still unconnected, and no real Toolkit Bar/Page/Panel
mounting, dependency redistribution acceptance, DataTable qualification or
physical HDR acceptance is implied by this foundation.

## Nested native component ownership foundation (2026-10-04)

The source-private `wxWinUIControlHost::SetContentInParent()` now mounts a
child slot in a validated `ContentControl` belonging to its live parent slot.
It rejects foreign, occupied and stale mounts, and keeps the ordinary content
publication/Loaded transaction. Native measure/arrange owns the child rectangle;
the host observes transformed native geometry instead of positioning it on the
top-level Canvas. Focus and routed pointer ownership resolve the innermost live
slot, with the previous fast path retained when no composite slots exist.

Logical removal and physical removal remain distinct. A parent retirement gate
waits for actual descendant disconnect receipts, including descendants already
logically retired. It does not replace a child's popup completion callback or
treat logical removal as a physical receipt. Whole-host shutdown transfers the
graph and each original popup gate to the existing XAML retirement boundary;
it does not wait cyclically for wrappers already handed to that boundary.

This is a restricted foundation, not the completed Ribbon adapter. Replacing a
parent's content while it still has mounted descendants is rejected. Reparenting
a mounted composite subtree across top-level hosts is also rejected before HWND
mutation; cohort migration remains to be implemented. No shipping control uses
this new mounting path yet. The Toolkit runtime helper and native Ribbon group
adapters remain unconnected.

The new native fixtures cover innermost focus, content replacement, mount
rejection, parent/child teardown including an unsatisfied popup gate, whole-host
shutdown, native arrangement, resize, collapse/reflow and empty explicit clips.
The pure retirement-gate fixture separately covers completion and shutdown
ordering. A bounded `wx_winui_composite` CTest now runs these contracts in both
WinUI linkage jobs. No remote CI result is claimed.

## Classic repaint after bridge erase (2026-10-04)

The user's HDR success above exposed a separate repaint defect after moving or
resizing. The shared island's background erase can overwrite classic HWND
pixels without invalidating those HWNDs. After a successful erase the host now
invalidates only intersecting visible classic client regions, clipped through
their ancestors. Island descendants, native XAML slot owners, hidden controls,
foreign HWNDs and deleting windows are excluded. For complex clips the repair
is a bounded rectangle with wholly clipped-out clients rejected; it is not an
exact arbitrary-region decomposition.

This schedules ordinary asynchronous `WM_PAINT` without requesting another
background erase, synchronous painting, a periodic refresh or whole-tree
invalidation. The WinUI-specific wxMSW backdrop erase also leaves
`wxBG_STYLE_PAINT` and transparent clients' existing pixels untouched, matching
their paint ownership. Mica, its frame extension and HDR policy are unchanged.

`winuibridgepaint.cpp` exercises the real native window/bridge message paths:
paint-owned RGB/alpha survive erase, only the intersecting classic update region
is scheduled, native/hidden/outside clients are excluded, repaint is deferred,
and an empty clip does nothing, for both LTR and RTL layouts. This deterministic
coverage does not itself constitute physical move/resize sign-off.

**User check of the rebuilt shared Showcase (2026-10-04):** the classic
components now remain visible while moving/resizing with Mica enabled. The user
still observes blinking during refresh. The persistent disappearance is fixed
in this observation, but repaint flicker remains open; it must not be described
as complete visual qualification. The shared candidate tested here is
`showcase.exe` SHA256
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`, with core DLL
`6E6ABCF64AE8F817848B4C444E695311D7F817760AE18E57F42E92C272081949`.

Executed checks for this candidate:

- Shared `test_gui` and `showcase`, Release `/warnaserror`: PASS in
  `audit108-composite-repaint-build2-20261004.log`.
- Shared aggregate metadata, composite ownership/retirement, opaque GDI,
  shell theme and native Ribbon buttonbar tests: **1471 assertions / 41 cases
  PASS**, `audit108-composite-repaint-tests1-20261004.log`, on a private desktop
  with `WX_UI_TESTS=0`.
- The four component CTests (metadata, opaque GDI, composite and Toolkit script
  selftest) pass for shared and static linkage in
  `audit108-composite-contract-ctest-20261004.log` (shared) and
  `audit108-composite-contract-ctest1-20261004.log` (static).
- Static Release `/warnaserror`: PASS in
  `audit108-composite-repaint-build4-20261004.log`. A larger static aggregate
  still intermittently fails the new composite focus fixture's replacement
  `IsLoaded` wait, despite its isolated/component-gate pass. The failing logs
  are preserved and this inconsistency is under investigation; the static
  aggregate is **not claimed green**.
- Ordinary wxMSW core Debug `/warnaserror`: PASS in
  `audit108-repaint-msw-20261004.log` in the existing MSW audit build.
- Fresh static installation and rebuilt external consumer pass the runtime
  smoke in **53.71 seconds**, `audit108-composite-runtime-20261004.log`. Source
  and installed core hashes both equal
  `03BD27D91F00902A0814DFBA5D1B1C64C752AE4AA04ED4E7CCCC898516591927`.
  This exercises existing runtime fault/epoch contracts, not teardown of an
  activated Toolkit Ribbon.

These shared checks supersede the earlier note that the metadata foundation
had only been tested statically. No Mica disablement, physical input injection,
monitor configuration change or new external CI result is involved.

### Follow-up: preserve opaque pixels before bridge erase

The user's refresh blinking is consistent with the gap between bridge erase
and the subsequent asynchronous classic paint. The follow-up now saves the DC
and excludes the currently visible clients already governed by the opaque GDI
paint policy from the erase clip. Repair runs only against the remaining clip,
then the original DC state is restored. Transparent surfaces, native XAML slots
and the splitter sash are not protected by an opaque rectangle. HWNDs or their
ancestors with explicit window regions conservatively retain erase-and-repair
behaviour; a bounding rectangle is not used to preserve their possible holes.
Failed clip preparation restores the DC and uses the original repair path.

Exclusions use current geometry on each erase, with no retained rectangle cache.
Thus hiding or moving a control releases its old pixels for background cleanup.
The new LTR/RTL fixture checks preserved red RGB/alpha, erased transparent and
uncovered pixels, exact DC clip restoration, no unnecessary opaque-client paint,
and cleanup after Hide/Move. It still uses the actual bridge subclass message
path. This reduces an erase-before-paint cause of flicker; physical confirmation
of the new candidate remains pending.

The composite focus investigation above found the replacement attached,
visible and measured in the correct XamlRoot, with every ancestor loaded, but
its own Loaded notification not delivered by the polling fixture. Only that
fixture's replacement wait now dispatches through the active native event loop,
with a 500ms wall-clock deadline instead of Yield/Sleep polling. The Loaded,
focus, identity and ancestry assertions remain; production focus logic was not
relaxed. Conditional failure diagnostics are retained.

Final follow-up builds and aggregate checks (2026-10-04):

- Shared `test_gui`/`showcase` and static `test_gui`, Release `/warnaserror`:
  PASS; `audit108-bridge-preserve-build1-20261004.log` in each build directory.
- Both linkages: **1983 assertions / 42 cases PASS** on private desktops with
  `WX_UI_TESTS=0`, `audit108-bridge-preserve-tests1-20261004.log`. This includes
  the previously failing static aggregate, without increasing its Loaded wait.
- All four bounded component CTests also pass again for each linkage in
  `audit108-bridge-preserve-ctest-20261004.log` (13.29s static, 10.71s shared).
- Shared Showcase EXE remains
  `7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`;
  the changed core DLL is now
  `878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`.
- Static core is
  `101CA4E412B8DCFF0AFE161724DF395E99DD3A5337C8E99F90356D6AAF85827C`.
  The external-consumer smoke recorded above preceded this final paint-only
  refinement; its installed-core hash must not be represented as this version.

Mica remains requested. Neither this follow-up nor the successful HDR and
disappearance checks constitutes completion of the full component matrix,
Toolkit Ribbon adaptation, physical interaction campaign or production sign-off.

## Native Ribbon container integration in progress (2026-10-04)

The previously unconnected runtime helper is now wired behind
`wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT=ON`. This is an explicit, default-off,
non-monolithic static build-tree experiment: package/license acknowledgement
options are required, and `wxBUILD_INSTALL=OFF` is enforced until the complete
installed-consumer payload contract is implemented. One deployer, selected in
`wx_add`, owns the merged PRI and Toolkit payload for every GUI target in that
opt-in tree. The ordinary shared build remains unchanged for the user's Mica
flicker verification (core DLL hash `878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`).

The new source-private container adapter connects public wxRibbonBar/Page/Panel
creation to a WinUI page selector, a real Toolkit Ribbon per page, and real
RibbonGroup/RibbonCollapsibleGroup entries directly in Ribbon.Items. Native
buttonbar command slots mount under Group.Content, not a parallel canvas.
Their first publication waits for the group's actual template; no temporary
ordinary slot is installed while this mount is unavailable. Group arrangement
now owns the command projection's allocation instead of an unarranged wx HWND.

Core gains a source-private direct-composite mount, independent of Toolkit:
exact-identity collection insertion/removal, stable concrete Control root,
parent generation validation, native-root geometry/input/focus, and the same
causal physical-retirement fan-in as nested content mounts. Ordinary slots keep
their existing Grid/carrier representation. Root replacement is explicitly
rejected; cross-top-level composite migration remains unfinished.

Tests added exercise direct collection roots and public wxRibbon controls,
including shared-island topology, real native types, command placement and
programmatic/native page selection with veto. Configure and the Toolkit
dependency script CTest pass (`audit108-native-ribbon-configure-20261004.log`,
`audit108-native-ribbon-selftest-20261004.log` in the static build). At this entry's
creation, compilation/runtime execution of the new adapter are still pending.

This is not a completed Ribbon API contract. Native expansion/popup retirement,
custom art/style policies, page icons/highlights, toolbar/gallery adapters,
composite migration, installation/shared qualification and physical keyboard/
UIA/DPI/theme interaction proofs remain open. Existing generic minimisation and
expansion APIs have not been relabelled as qualified Toolkit equivalents.

### Native Ribbon integration: build and runtime evidence

The static opt-in `test_gui` and `showcase` targets now build in Release with
`/warnaserror` (`audit108-native-ribbon-build3-20261004.log`). Public
`wxRibbonBar::Realize()` also succeeds before the frame is shown: buttonbar
commands are prepared off-tree while the group's template is not loaded, then
published through the actual composite mount after Loaded. No temporary
top-level command slot is created, and rollback distinguishes prepared content
from content actually hosted by the previous slot.

Private-desktop runtime results for that build (`WX_UI_TESTS=0`):

- `[winui-ribbon-toolkit],[winui-composite-direct]`: **192 assertions / 6 cases
  PASS**, `audit108-native-ribbon-tests3-20261004.log`. The direct-root focus
  fixture activates its test frame and requests logical/native focus; the
  native FocusManager and wx ownership assertions remain strict.
- Six bounded CTest gates: **5 PASS, 1 FAIL** in **38.59 seconds**,
  `audit108-native-ribbon-ctest3-20261004.log`.
- Passing gates: dependency self-test, component metadata (42 assertions /
  3 cases), opaque GDI paint (806 / 4), composite contracts (624 / 19), and
  real Toolkit Ribbon (105 / 3).
- `wx_winui_ribbon_native` remains **red**: 4 of 5 cases pass, 331 of 332
  assertions pass. `WinUIRibbon::NativeButtonBarPeersAndEvents` fails its
  existing `fixture.ReparentTo(fixture.GetFrameB())` assertion at
  `tests/controls/winuiribbon.cpp:3162`. The new bar/page/panel/command graph
  reaches the deliberately unsupported composite-cohort cross-TLW migration
  path. No test was disabled or changed to accept this failure. The opt-in
  build is not a regression-clean or production-qualified Ribbon release.

The static visual candidate is
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib\showcase.exe`:

- EXE SHA256: `DCEBFA471B1993AEBC0965DDB405B27CE25CDBF85F0552FA93DA69599C556A21`.
- `wxwinui33u_core.lib` SHA256:
  `E12B2BCBF8B30F5923B70F614421255E35C991EB6D25EAC2C999B8B96B3895C4`.
- All seven Toolkit 0.8.0-alpha DLLs and the Win2D 1.4.0 DLL match their
  pinned package-cache hashes. Merged `resources.pri` SHA256:
  `EA6D4F450415C3C66727AF89E375E69BC2992BCEC5FEE99B8305D1F3B31C3929`.

No physical GUI/input campaign or visual appearance acceptance is claimed for
this candidate. The shared Mica candidate above is unchanged and its latest
anti-flicker refinement still awaits human confirmation. Cross-TLW cohort
migration is the next prerequisite before considering the native Ribbon gate
green; simply removing the rejection would detach or mis-publish children and
is not an acceptable fix.

The precise rejection is the callback-free MSW preflight
`CanReparentSubtreeNow()` (`src/winui/tlwhost.cpp`): any composite child in the
subtree currently refuses cross-TLW reparenting. The MSW caller only runs this
preflight across top levels; same-TLW reparenting instead rebinds ancestors. The
cross-TLW `MigrateSlotToCurrentTLW()` also rejects a composite child or a parent
with composite children before detach. The next migration lot must preserve
exact native collection mounts, transfer the entire parent/child ownership
cohort atomically, retain physical-disconnect gates and focused-child identity,
and roll back failed destination publication without exposing independent
Canvas children. Its acceptance must include the existing failing Ribbon case,
same-host reparenting, pending-popup refusal, destination-failure rollback and
destruction/reentrancy during transfer.

### Human repaint follow-up

The latest user reply confirms that classic controls remain visible while
moving and resizing the shared Showcase with Mica enabled. The user still sees
blinking when they refresh. This is not acceptance of flicker-free repainting;
the residual blink remains open. No workaround disabling Mica is accepted.
During the composite migration work below, the shared Showcase and core DLL
were left unchanged and their hashes rechecked against the values above.

### Composite-cohort migration implemented (2026-10-04)

The earlier cross-TLW rejection is replaced by a transaction over the entire
slotted wx subtree. Slot allocations, invalidatable lifetimes, direct Items
mounts and ContentControl mounts retain their identities. Only independent
Canvas roots cross XamlRoot, and all registry ownership is published together
before native attachment callbacks. An outstanding descendant popup receipt,
active pointer press or native resize refuses the move before mutation.

An intact destination failure restores the original root order and ownership;
the MSW boundary then restores the matching wx/USER32 parent. Ordinary slots
keep their existing deferred-retry contract. Once a reentrant logical close
has made destination retirement authoritative, all remaining independent roots
finish publication there, including roots retained for a pending child receipt.
HRESULT and standard C++ callback exceptions share this settlement path. A
terminal native failure preventing publication after the source is destroyed
or physical debt is committed is still fatal, never reported as partial success.

The focus regression uncovered by these tests was an ancestor GotFocus
notification claiming logical ownership while FocusManager still pointed to
the nested leaf. GotFocus publication now checks the nearest actual focused
slot and revalidates its identity. Destruction of the old host hands the same
migration ticket to the destination and continues it while its native authority
is still valid, before destruction of the old active frame can clear focus.
The tests require wxWindow::FindFocus(), native FocusManager, exact slot/root
identities and no duplicate logical focus events, including the return trip.

A current native-composite buttonbar Realize is idempotent: a repeated Loaded
after moving the native tree no longer replaces command peers/UIA providers.
Real model/style changes still rebuild transactionally. The existing layout
publication test now starts its second swap with a real public model mutation,
not a no-op Realize; its rollback, in-swap resize and final geometry assertions
are retained. The multi-root close hook also no longer assumes allocator/map
order when identifying the first physically attached root.

Evidence (local only, no new physical-input campaign):

- MSW Debug `wxcore` `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-cohort-msw-build1-20261004.log`.
- Static opt-in Release `test_gui` and `showcase` `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-feedback-build-static\audit108-cohort-build4-20261004.log`.
- `[winui-composite-migration],[winui-ribbon-toolkit]`: **2180 assertions /
  9 cases PASS**, private desktop, `WX_UI_TESTS=0`,
  `audit108-cohort-tests4-20261004.log` in that static build directory.
  This includes popup refusal, rollback/retry after HRESULT and C++ exceptions,
  multi-root retirement with and without a throwing callback, source shutdown,
  same/cross-TLW public Ribbon round trips and pre-acquired UIA providers.
- Six bounded CTest gates: **6 PASS / 0 FAIL**, 46.71 seconds,
  `audit108-cohort-ctest4-20261004.log`: dependency self-test, metadata (42 / 3),
  opaque GDI (806 / 4), composite contracts (1901 / 24), native Ribbon
  buttonbar (352 / 5), and native Toolkit Ribbon (903 / 4).
- Existing ordinary migration contracts: **846 assertions / 15 cases PASS**,
  `audit108-cohort-ordinary-tests4-20261004.log`, including focused transfer,
  native/GettingFocus redirection, DPI event order, popup refusal, rollback,
  immediate retry and reentrant destruction/reparent/content replacement.
- Existing exact focus-regression selection: **427 assertions / 21 cases
  PASS**, `audit108-cohort-focus-regressions4-20261004.log`. This retains
  RadioBox, unloaded content/shell, deferred focus, nested reparent, native
  redirection, activation and keyboard-focus exclusion contracts.

The static build4 Showcase hash is
`984FAF3B0D8976DEDF48D08F3270121D2E0F79121851EAE496225E99927F34DD`;
test_gui is `6D25B0044E957A2D4A2962A1CE6144662AB695049117A7A5682757C50140508D`;
core is `4E322B8ABA804620D6C4A52BFF39D1D9BFE10B25FAD1EB7F780A6C4729E88791`;
ribbon is `516FEFD337BCD3F7F79A5554907A1F784DAD4AED59FBF85CA7EAD1AE313B9F4F`.
The merged PRI remains `EA6D4F450415C3C66727AF89E375E69BC2992BCEC5FEE99B8305D1F3B31C3929`.

This closes a prerequisite, not the complete native Ribbon contract: expansion
and popup interaction, art/style/icon/highlight policies, toolbar/gallery
adapters, installed/shared payloads and physical qualification are still open.
No commit, push, shared-candidate replacement or production sign-off was made.

### Native Ribbon ToolBar integration (2026-10-04)

The previously uncompiled ToolBar draft is now integrated into the public
`wxRibbonToolBar` implementation. Its normal, toggle, dropdown and hybrid
commands use real WinUI AppBar controls and separators. In the opt-in Toolkit
configuration, they mount inside `RibbonCollapsibleGroup.Content`, sharing the
Ribbon composite tree instead of installing another independent Canvas root.
Pre-show realization stages content until that mount exists; unchanged
realization and cross-TLW migration preserve native command/provider identity.

The adapter projects rows, groups, bitmaps, help, enabled/checked state and
client data while retaining the public wx event model. Geometry and hit testing
come from the arranged native visual. Native callbacks use monotonic tool keys
and projection generations so a retained provider cannot invoke a replacement
tool reusing an ID. UpdateUI snapshots keys before calling application handlers.
Application bitmap providers are resolved only after copying the model, with
owner/revision checks across callbacks.

The first runtime pass found a real rollback defect: after a refused host
content install, the surviving tree needed synchronous arrangement before
`GetToolRect()` could again return its previous client offsets. The fix only
flushes that exact surviving publication; it does not retry the failed swap or
replace newer content. Regression assertions retain exact pre/post rectangles.
Two test oracles were also corrected: ToolTip is attached to the command Grid,
and the documented positional lookup returns null for separators. Geometry
checks independently combine native visual transforms with USER32 client
mapping, rather than reusing the coordinate helper under test.

Evidence, build2 unless stated otherwise:

- WinUI static opt-in Release `test_gui` and `showcase`, `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-feedback-build-static\audit108-toolbar-build2-20261004.log`.
- wxMSW Debug `wxribbon`, `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-toolbar-msw-build1-20261004.log`.
  The later rollback fix is confined to the WinUI-only adapter header.
- `[native-toolbar]`: **477 assertions / 7 cases PASS**,
  `audit108-toolbar-tests2-20261004.log` in the static build directory. Covers
  composite mounting, pre-show/idempotent realization, migration with retained
  UIA providers, command kinds, popup anchor/active tool, state/help/UpdateUI,
  two-row LTR/RTL geometry, separators, failed-install rollback/retry, stale
  providers and deletion/clear/owner destruction inside an invoked handler.
- Existing public construction/commands and cross-TLW stress: **955 assertions /
  3 cases PASS**, `audit108-toolbar-model-tests2-20261004.log`.
- Six bounded CTest gates: **6 PASS / 0 FAIL**, 48.56 seconds,
  `audit108-toolbar-ctest2-20261004.log`: Toolkit dependency self-test, component
  metadata, opaque GDI, composite contracts, native buttonbar and Toolkit Ribbon
  (including the new ToolBar cases).

All runtime checks above use the private desktop runner with `WX_UI_TESTS=0`,
without physical input. The Advanced Showcase page has an additional `Tools`
group demonstrating normal, dropdown, toggle, hybrid commands and a separator.
The build2 static Showcase hash is
`D41668814B43441F103D1526F3DCC796F762BC8C1F2920C18DAB2B9206372DA3`;
test_gui is `6524CEE91653D191B462C95223BACE2095887D264933AEDE53444F6978CD28E1`;
ribbon is `1E8DFA70260E7ACDC17F5B1D9668AE6A302DF5B31A2D44436BC1B1B385E76387`.
Core and the merged PRI are unchanged from composite-cohort build4 above.

The shared Showcase used for the human Mica test was not replaced: its EXE and
core DLL hashes still match the earlier recorded candidate. The reported
refresh blink remains open. A source inspection identifies classic surfaces
without paint-owned backgrounds (for example wxGrid label windows) as a
possible erase-before-repaint path, not a verified explanation for every
blinking component. No blanket opacity or Mica-disable workaround was added.

This supersedes the ToolBar-adapter-missing item, not the remaining native
Ribbon gaps: gallery, popup/expansion interactions, custom art/style policies,
installed/shared Toolkit payloads and physical qualification remain open.
The historical uncompiled `.pending.patch` is retained as provenance only.
No new dependency, build directory, commit, push or production sign-off.

### Classic grid label repaint ownership (2026-10-04)

The human follow-up confirms that classic controls remain visible while moving
and resizing the shared Showcase with Mica enabled. Refresh blinking is still
reported. This is distinct from the previously corrected HDR disappearance;
Mica remains enabled and the shared human-test candidate is unchanged.

A deterministic regression now demonstrates one erase-before-paint defect in
real `wxGrid` row labels, column labels and the corner. On the old implementation,
bridge `WM_ERASEBKGND` erased their colour and alpha and scheduled a later paint,
while the paint-owned grid body stayed intact. This was checked before dispatching
any repair paint, in LTR/RTL and with empty/nonempty grids: **132 failed assertions
out of 876**, `audit108-grid-repaint-red1-20261004.log` in the static build directory.

Only the WinUI label windows now use `wxBG_STYLE_PAINT`. Their existing paint
handlers fill the complete update region with the label background colour before
applying the scrolling origin or taking the empty-grid return. The existing
opaque GDI buffer publishes this background together with the label contents.
No blanket opacity, bridge exclusion or backdrop-disable workaround was added;
other ports retain their existing painting path.

The two grid regressions pass after this correction (**1188 assertions / 2 cases**).
The second observes the real `WM_PAINT` buffer without drawing into it: full and
partial `Refresh(true)`, custom colour changes, empty labels, space beyond the
last row/column, corner, LTR/RTL, and alpha over the entire requested region.
Build3 opaque-GDI CTest passes **1994 assertions / 6 cases** including these two;
all six bounded gates pass in 55.99 seconds (`audit108-gallery-ctest3-20261004.log`).
WinUI Release core/ribbon and test_gui/showcase compile with `/warnaserror`
(`audit108-gallery-libs-build3-20261004.log`, `audit108-gallery-build3-20261004.log`).
wxMSW Debug core/ribbon also compile with `/warnaserror`
(`F:\wxwinui-pr26890-msw-audit-build\audit108-gallery-msw-build3-20261004.log`).
The corrected static core library SHA-256 is
`071201D4E093EDD7F0A99574F9B6E3763120664ADA3F22BA490F6017CDEFB391`.

All runtime checks used the private desktop with `WX_UI_TESTS=0`. This proves
the grid-label defect and its correction, not that every reported refresh blink
is resolved or that the new build has passed a physical HDR/Mica qualification.

### Native Ribbon Gallery integration (2026-10-04)

`wxRibbonGallery` now projects its model through a real WinUI GridView,
GridViewItems, native ScrollViewer and three native buttons. With the opt-in
Toolkit Ribbon these live inside `RibbonCollapsibleGroup.Content`, not an
additional independent Canvas root. The Advanced Showcase page adds a Gallery
group with twelve icons, silent initial selection and selected/click/extension
event logging. The API reference now uses the actual `wxBitmapBundle` signatures.

The adapter retains monotonic item keys and projection generations, silent wx
selection, distinct native SelectionChanged/ItemClick semantics, LTR/RTL layout,
scrolling and EnsureVisible. Client data keeps its object/void ownership mode,
including null values. Replacement publishes the new object before destroying
the previous one; Clear detaches the old item batch before running application
destructors. Repeated realization and migration preserve current providers;
retired providers cannot act on a replacement item reusing its public ID.
Failed host installs preserve the previous projection and exact geometry.

Runtime validation found a real pre-mount API regression: the existing public
overflow test called Realize/ScrollLines before Group.Content existed. A detached
GridView still reported zero native extent even after Measure/Arrange. The fix
uses the bounded wx viewport and explicit cell dimensions as its pre-mount model,
configuring the same dimensions on ItemsWrapGrid. Requested offsets are retained
until a loaded native viewport exists, then reconciled and applied absolutely,
not added to an incidental initial selection offset. No message pump or parallel
host is introduced. The new pre-mount test requires the actual native offset to
equal one measured cell plus twelve wx pixels (within one DIP), then checks both
scroll bounds and silence of selection events. The older overflow test's
assertions remain unchanged.

The UIA selection tests acquire the GridView-created item data peer; its visual
container peer does not supply SelectionItem. No synthetic wx selection event
is used to substitute for this native path. SelectionItem is not a physical
pointer click, so these results do not qualify physical ItemClick/hover/active
state or the full input/accessibility campaign.

Build6 evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui/showcase with `/warnaserror`: PASS,
  `audit108-gallery-build6-20261004.log`.
- `[native-gallery]`, `[winui-grid-repaint]`, and four existing public Ribbon
  construction/commands/overflow/cross-TLW tests: **2714 assertions / 14 cases
  PASS**, `audit108-gallery-grid-tests6-20261004.log`. This includes eight native
  Gallery cases and the two real grid-paint regressions above.
- Six bounded CTest gates: **6 PASS / 0 FAIL**, 55.55 seconds,
  `audit108-gallery-ctest6-20261004.log`: Toolkit dependency self-test, component
  metadata (42 / 3), opaque GDI (1994 / 6), composite contracts (1901 / 24),
  native buttonbar (352 / 5), and Toolkit Ribbon (1875 / 19, including the eight
  Gallery cases). Native Gallery alone accounts for 495 assertions / 8 cases.
- wxMSW Debug core/ribbon passed `/warnaserror` in build3 above; subsequent
  production edits are confined to the WinUI-only Gallery adapter.

Static build6 SHA-256:

- Showcase: `8826035B57B378AFB11DE59CC31E2F5EC73D4469AB6D1261433FD4E2CEE7E29C`.
- test_gui: `1327F458F1EFD7FF3072EFD3BC51537437D4AA5E6DB8F296BB67AE6DAD7D8859`.
- Ribbon: `A6EB2DFCF07B528E9BBE0A11809BFE036F7D9A86AB9329BF0DEEAF4F5F5274E6`.
- Core remains the grid-correction hash above; merged PRI remains
  `EA6D4F450415C3C66727AF89E375E69BC2992BCEC5FEE99B8305D1F3B31C3929`.

The shared human Mica candidate was re-hashed and not replaced (Showcase
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`, core
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`). All runtime
tests used the private desktop, `WX_UI_TESTS=0`, without physical input.

This supersedes the missing Gallery-adapter item, not complete Ribbon or port
qualification. Popup/expansion and custom art/style contracts, additional edge
and physical interaction coverage, installed/shared Toolkit payloads, the
remaining control families and final performance/soak qualification remain open.
No new dependency or build directory, shared-candidate replacement, commit, push
or production sign-off was made.

### AUI notebook repaint ownership and Gallery pre-mount visibility (2026-10-04)

The human reports stable classic-control visibility with Mica still enabled,
but residual refresh blinking. A second concrete erase-before-paint defect was
reproduced in the real `wxAuiNotebook` used by Advanced: its manager already
clears the dock background in `OnPaint` and suppresses separate erasure, but
the notebook retained the default erase-owned background style. The WinUI Mica
erase branch and the island bridge consequently erased its pixels before paint.

The new `[winui-aui-repaint]` regression failed **16 / 104 assertions** on the
unchanged implementation (`audit108-aui-repaint-red1-20261004.log`). It checks
direct and bridge `WM_ERASEBKGND`, RGB/alpha preservation before paint, and no
unnecessary invalidation or synchronous repair; both LTR/RTL and empty/populated
notebooks are covered. Its paint observer checks the real manager painter,
changed dock colours, requested bounds and opaque alpha. A populated notebook's
page is explicitly hidden only for the exposed-background paint oracle.

The production correction is limited to `wxAuiNotebook::InitNotebook`: use
`wxBG_STYLE_PAINT` under WinUI so the existing opaque paint transaction owns its
background. Other ports, Mica policy and the shared human-test candidate remain
unchanged. This qualifies `Refresh`/`WM_PAINT`, not all interactive resize:
`wxAuiManager::Repaint(nullptr)` still has its separate `wxClientDC` path. Nor
does it prove that RichText, StyledText or HTML refresh blinking is resolved.

The native Ribbon Gallery also retains a model-keyed `EnsureVisible` request
across staged projection replacement before its first native mount. The loaded
native viewport resolves the current item and performs the scroll; Clear/Close
and a subsequent effective explicit scroll retire the pending request. No
additional layout engine or message pump was introduced. The ninth native
Gallery case requests item 47 then item 23 (last request wins), replaces the
staged projection with Append/Realize, and verifies the actual visible item
bounds after showing, without selection changes or selection/click events.

Build2 evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- WinUI Release `test_gui` and `showcase`, `/warnaserror`: PASS,
  `audit108-repaint-gallery-build2-20261004.log`.
- AUI repaint plus native Gallery plus existing AUI notebook lifecycle:
  **895 assertions / 11 cases PASS**, `audit108-repaint-gallery-tests2-20261004.log`.
  Gallery contributes **625 / 9**, AUI repaint **176 / 1**, existing AUI notebook
  **94 / 1** (also passed before this correction).
- Six bounded CTest gates: **6 PASS / 0 FAIL**, 58.01 seconds,
  `audit108-repaint-gallery-ctest2-20261004.log`: Toolkit dependency self-test,
  component metadata, opaque GDI, composites, native buttonbar and Toolkit Ribbon.
- wxMSW Debug `wxaui`, `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-aui-repaint-build1-20261004.log`.

Build2 SHA-256:

- Showcase: `0A197F64C6C9DC2E98FB513A452B129D2006C660C1B3ABFF82485BDA7A5693E9`.
- test_gui: `7EE5E49C5FB44779F7E0D64135012EE0F00B8FD767A9AD514B27B658B0CA7DC2`.
- AUI: `A4F3F410CFB620E6010D0512A757A002309042D93D043EDDEC4B42F4EC2EDF39`.
- Ribbon: `8E97C09DFEF5BAF0A3C55C2473E8900311EF0099F0D95D22C29E038070A233B5`.

Runtime checks use the private desktop and `WX_UI_TESTS=0`, not physical input
or a new human HDR/Mica sign-off. No shared binary replacement, new build tree,
dependency, commit or push is part of this lot. Full Ribbon expansion/public
panel identity, other control families and plans 109-112 remain open.

### Native Flyout foundation and AUI size painting (2026-10-04)

The popup/focus retirement implementation is now source-private in
`src/winui/popupretirement.cpp`, independent of Choice's implementation TU.
Choice uses opaque Combo retirement helpers; the reusable Flyout helper accepts
`FlyoutBase`, covering real Flyout as well as MenuFlyout. Exact Popup/Child
identity, focus correlations and the causal close tail are still required.

Regression checks exposed publication timing that a post-ShowAt-only lookup
could not handle. A missing popup before native Opened is pending, not a failed
publication. An immediate Hide can precede publication too: Closing is cancelled
only while unresolved/pre-Opened, and Hide is replayed after Opened resolves the
exact popup. Ambiguity, unavailable ancestry and nonterminal Closed without
proof still fail closed. No timer, synthetic receipt or publication-poison reset
was added. If actual TLW destruction cancels opening, an already sealed gate
stays owned by the existing terminal ledger without poisoning later controls.
This unresolved graph remains retained until framework rundown; these tests do
not claim immediate resource release or leak-free repeated shutdown.

The native timing rationale is corroborated by Microsoft's
[pinned FlyoutBase source](https://github.com/microsoft/microsoft-ui-xaml/blob/7b68d3e0b771a57d80098799406234efee479517/dxaml/xcp/dxaml/lib/FlyoutBase_partial.cpp#L1192-L1234),
not asserted to be the exact installed runtime source. Real-runtime tests cover
ordinary/reopened/transient flyouts, same-stack ShowAt/Hide and actual wxFrame
destruction before Opened. The autocomplete test now observes the existing host
contract: cross-TLW Reparent is refused without topology mutation while popup
debt is live, then succeeds after Escape and the causal tail. Its post-migration
key test waits for the exact native editor focus, not merely any focused HWND.
The CTest Flyout gate includes all autocomplete cases as regressions.

`ribbonflyoutwinui.*` resolves the pinned Toolkit collapsed-group template's
exact typed parts and rejects stale/replaced/foreign templates. A separate real
native topology test moves the same content inline -> Flyout -> inline, checks
LTR/RTL geometry, XAML/HWND focus and real Invoke providers. This is a foundation,
not the public wxRibbon expansion transaction: ShowExpanded/HideExpanded,
expanded wxPanel identity, child/sizer migration, rollback and popup composite
host ownership remain to implement. Toolkit retemplating does not automatically
detach old Content; the identity test explicitly detaches it and does not claim
production migration is solved.

For the reported residual blink, a real WM_SIZE -> AUI OnSize -> Repaint(nullptr)
regression showed one direct wxClientDC render and zero opaque paint commits in
each empty/populated LTR/RTL variant: **16 / 204 assertions failed** in
`audit108-aui-size-red7-20261004.log`. Repaint now uses Refresh(false)/Update only
for WinUI surfaces already requiring the opaque paint buffer. Its existing
OnPaint handles clear and render together; explicit DCs, other ports and Mica
policy are unchanged. This closes the AUI ClientDC bypass left open above, not
all visual blinking in RichText/STC/HTML or a new physical HDR sign-off.

Final build10 evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase, `/warnaserror`: PASS,
  `audit108-flyout-aui-build10-20261004.log`.
- AUI + autocomplete + native Flyout/topology/Toolkit parts:
  **1058 assertions / 13 cases PASS**, `audit108-flyout-aui-tests10-20261004.log`.
- Existing Choice/Combo popup and host popup-gate regressions:
  **794 assertions / 21 cases PASS**, `audit108-flyout-popup-regression10-20261004.log`.
- Seven bounded CTest gates: **7 PASS / 0 FAIL**, 61.37 seconds,
  `audit108-flyout-aui-ctest10-20261004.log`: dependency self-test, metadata
  (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24), Flyout/autocomplete
  (749 / 9), native buttonbar (352 / 5), Toolkit Ribbon (2090 / 23).
- No-PCH, `/WX` compile-only probe with Choice/ComboBox disabled after config
  checks: PASS, `audit108-flyout-nochoice10-20261004.log`. This proves TU
  independence, not a complete noChoice build configuration.
- wxMSW Debug wxaui `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-aui-size-build8-20261004.log`.

Build10 SHA-256:

- Showcase: `52B287D35B9D54F1E790FC1E973197A885CB54AEA35BA0F9B37ADDA9E6E7BA7B`.
- test_gui: `2EF8702B33ABF438ECF8F17F1FCC55053FB465B3E5CD23F15EFACA1B892EF232`.
- Core: `07DBE701A309669DF9510DC855B5FE5B60EC90DC65799ADD28905CE7D75CBBF3`.
- AUI: `EC34DC128A2E78E295EE74C97114FDFC3D156A854AB983A0BCA34B04FCD05FF9`.
- Ribbon: `8FB6CE0075EC4F42700B149BD8FE32F80962AA7A9F41054A1FB551D73CDE0BD1`.
- Merged PRI: `EA6D4F450415C3C66727AF89E375E69BC2992BCEC5FEE99B8305D1F3B31C3929`.

Runtime checks remained on the private desktop with WX_UI_TESTS=0, no physical
input. The shared human Mica candidate was re-hashed unchanged (Showcase
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`, core
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`). No new build
tree, dependency, shared-binary replacement, commit, push or production sign-off.

### Public native Ribbon panel expansion (2026-10-04)

The experimental Toolkit adapter now implements IsMinimised, ShowExpanded and
HideExpanded using the real CollapsibleGroup Flyout. The expanded wxRibbonPanel
is a distinct logical child of its dummy, not a second top-level HWND or a second
native Ribbon.Items entry. The same native command objects, composite owner,
mounts, providers and wx sizer survive expansion and restoration. Both public
expanded/dummy links are maintained. A refused child Reparent rolls back the
model; native close restores it; reentrant close and destruction clear links
before entering native callbacks. Nested native Hide requests are coalesced.

The host accepts this detached template content only through a source-private,
exact composite-content extension. The registered weak root, parent content
generation and current-template witness must still agree. Ordinary ancestry
validation remains in force elsewhere; a matching XamlRoot alone is not proof
of ownership. Foreign/replaced/stale extensions refuse without topology mutation.
The physical popup receipt uses the stable template popup mount, not command
content that adaptive layout can move inline before Opened. Reopening is refused
until the preceding exact popup/focus debt retires.

An initially empty or transiently emptied composite gate now retains its actual
DispatcherQueue. Previously, removing its final child erased the only queue
witness and real parent/child shutdown terminated on the sealed empty gate.
The fix does not skip sealed gates, synthesize completion or clear publication
poison. Real never-opened and opened/closed/pruned cases exercise frame destruction.

Expansion tests cover native button invocation and public calls, stable command
providers, proxy/dummy identity, sizer/child preservation, failed-Reparent retry,
recursive close refusal, destruction from Closing, final inline expansion,
same-stack State changes, commands added while collapsed, and repeated native
remeasure at fixed collapsed width. The fixture uses a normally active window:
ShowWithoutActivating produced timing-sensitive native closures before Opened;
the failures stopped with the same active-owner precondition as other Flyout
tests. Passive native-event traces distinguished these from State changes;
no production light-dismiss suppression was introduced. Tests
still require loaded, nonzero-size commands in the actual popup tree.

Scope is deliberately not declared complete: expansion currently admits only
already-published native command children. Arbitrary HWND/GDI children are
explicitly refused, not reported as successfully visible in a XAML popup.
Custom-template replacement, general panel/sizer API parity, other control
families and plans 109-112 remain open. These private-desktop tests do not replace
human keyboard, accessibility, HDR/Mica or soak qualification.

The combined popup regression also exposed a real ComboBox cancellation gap,
not a failed focus precondition: the fresh diagnostic showed exact editor focus,
Escape delivered, no publication poison, but two suggestions reopened. Refresh
had logically detached the old flyout while its physical retirement still held
the replacement request. Escape now cancels that request even with no current
flyout. A second real native SelectionChanged notification during close/focus
restoration repeated the already accepted range and re-armed completion; both
normal and SIMPLE editor handlers now refresh only for an actual range change.
No delay extension, forced popup close receipt or focus override was added.

The new same-stack replacement/Escape regression failed for both styles before
the unchanged-range correction (`audit108-ribbon-expansion-escape22-20261004.log`)
and then passed inside the original combined gate: **1097 assertions / 13 cases**,
`audit108-ribbon-expansion-flyout23-20261004.log`. It also checks that a subsequent
genuine edit opens suggestions from the replacement completer. Earlier failing
combined runs and debugger traces are retained in the same static build tree.

Final build23 evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase, `/warnaserror`: PASS,
  `audit108-ribbon-expansion-build23-20261004.log`.
- Seven bounded CTest gates: **7 PASS / 0 FAIL**, 65.14 seconds,
  `audit108-ribbon-expansion-ctest23-20261004.log`: dependency self-test,
  metadata (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24),
  Flyout/autocomplete (1097 / 13), native buttonbar (352 / 5),
  Toolkit Ribbon including public expansion (2651 / 32).
- Existing Choice/Combo popup and host popup-gate regressions plus the new
  Escape case: **831 assertions / 22 cases PASS**,
  `audit108-ribbon-expansion-popup-regression23-20261004.log`.
- No-PCH `/WX` compile-only noChoice/ComboBox probe of popupretirement.cpp:
  PASS, `audit108-ribbon-expansion-nochoice19-20261004.log`; that TU and its
  probe inputs are unchanged by the later ComboBox-only correction. This is
  still not a full noChoice build configuration.
- wxMSW Debug wxribbon + wxaui `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-expansion-final-20261004.log`.

Build23 SHA-256:

- Showcase: `712E3F8E67AF4AFEB0C71EF23DD521FB284A59900197F90DF9A4F79560C7F030`.
- test_gui: `39A6ACF961E8BA4273D924FA8C8CF19647499A21EE090B7D68E167795146E593`.
- Core: `FC69FAB413F8ED00AF3186F56A5B36EC9AD6D0F8F6B275D649EACB938FE693CF`.
- Ribbon: `ECB6A1D90A011FE2E4E9C44B5B9E07B4C4C2772A2718A555BC3234058A89AAE2`.

All runtime execution stayed on the private desktop with WX_UI_TESTS=0. The
shared human Mica candidate was re-hashed unchanged (Showcase
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`, core
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`). The new
candidate is the static Showcase in `lib\vc_x64_lib`; the user has not yet
qualified its residual blink or this native Ribbon expansion visually. Mica
remains enabled by policy. No new build tree, dependency, shared replacement,
commit, push or overall production-readiness claim is part of this lot.

### Native Ribbon panel sizer geometry (2026-10-04)

The native panel no longer replaces every public wxSizer with a fixed horizontal
StackPanel. Its existing Group.Content now owns a private native Grid-derived
layout panel. The original wxSizer calculates allocations for the already
published native command children, including nested/custom sizer algorithms,
spacers, borders, proportions and visibility. Their same ContentControl mounts
receive those allocations in DIP. The no-sizer path keeps horizontal native
arrangement; no extra island, generic ribbon painting or Mica policy change is
introduced.

wxSizer::SetDimension can synchronously send wxSizeEvents, and command handlers
may rebuild peers and call UpdateLayout. It therefore runs outside native
MeasureOverride/ArrangeOverride. Native layout consumes a snapshot of minimum
size and mount rectangles; changed available space schedules one coalesced wx
turn. A nested wxYield defers that work until the active sizer transaction exits.
Public panel Layout updates the snapshot without recursively realizing every
child. Expanded panels use their transferred public sizer and the same native
mounts. The host's composite ownership and physical retirement protocols are
unchanged.

The first broad run caught an implementation regression, retained as
`audit108-ribbon-sizer-toolkit2-20261004.log`: eight existing expansion cases
found loaded commands of size 0x0. RemoveChild/Sync during the logical child
transfer had published a partial layout. Layout publication now preserves its
complete snapshot throughout expansion/rollback/restoration transactions and
refreshes the sizer before starting expansion. No existing test was removed,
weakened or given a longer timeout.

Four new real-native cases cover vertical/horizontal orientation, a border of
11 physical pixels, proportions 1:2, wxEXPAND, the actual expanded Flyout,
nested GridSizer/spacer/margin geometry, hide/show, LTR/RTL, and a wxSizeEvent
handler that calls wxYield plus nested Layout. Assertions compare actual XAML
mount bounds with wx allocations, verify command roots remain loaded and inside
their allocation, and preserve root/mount identities. These tests do not claim
arbitrary HWND/GDI children are supported in the native Flyout; that previous
admission limitation and the other component/template gaps remain open.

Final build3 evidence (existing static build tree,
`F:\wxwinui-pr26890-feedback-build-static`):

- Release test_gui + showcase, `/warnaserror`: PASS,
  `audit108-ribbon-sizer-build3-20261004.log`.
- `[winui-ribbon-sizer],[winui-ribbon-expansion]`: **955 assertions / 13 cases
  PASS**, `audit108-ribbon-sizer-tests3-20261004.log`.
- Seven bounded CTest gates: **7 PASS / 0 FAIL**, 65.63 seconds,
  `audit108-ribbon-sizer-ctest3-20261004.log`: dependency self-test, metadata
  (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24), Flyout/autocomplete
  (1097 / 13), native buttonbar (352 / 5), Toolkit Ribbon (3045 / 36).
- wxMSW Debug wxribbon, `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-sizer-20261004.log`.
- `git -c core.safecrlf=false diff --check`: PASS.

Build3 SHA-256:

- Showcase: `4ABFBB542A6789E9EA54ACA8ECAA898EB2755DADBA84263F96820A46D62B5352`.
- test_gui: `42EAF6874196EA81AE017DD2302557C53618F79286ED5E51C433946B6EA41D8E`.
- Ribbon: `70479F64A0B5B1B79C41FD7D83EE9CB143B1664CC83253C0CC1E68D594501F1A`.

All runtime tests used the private desktop with WX_UI_TESTS=0, without physical
input. Shared Showcase/core were re-hashed unchanged at their preceding recorded
hashes. The static Showcase is rebuilt; no binary was launched on the user's
desktop. Residual blink, physical Ribbon/HDR/Mica qualification, the rest of
108 and plans 109-112 remain pending. No new build tree, dependency, commit,
push or overall production-readiness claim is made here.

### Native Ribbon bar help and display-mode buttons (2026-10-04)

The native TabView now exposes stable Fluent Buttons in its TabStripFooter for
wxRIBBON_BAR_SHOW_HELP_BUTTON and wxRIBBON_BAR_SHOW_TOGGLE_BUTTON. Style changes
update their visibility without replacing buttons or providers. Help keeps the
current mode; toggle switches visible panels to MINIMIZED and hidden panels to
PINNED. As in the generic implementation, the mode changes before the public
wxRibbonBarEvent, whose id/object identify the bar and whose page is null.
Programmatic ShowPanels/Realize do not synthesize user events. Accessible names
and tooltips identify both commands; the toggle's name and chevron follow the
public display state.

The callback refuses a hidden/disabled bar or a cleared style flag, including
when an old UIA provider remains alive. Native inherited enabled state still
comes from the host carrier. The event handler may delete the bar or change its
mode: the adapter performs no post-event dereference or state restoration.
Handlers are weak-owner qualified and removed during retirement. No public
class layout, installed API, generic-port behavior or Mica policy changed.

Three new real InvokeProvider cases pass **222 assertions** in
`audit108-ribbon-chrome-tests2-20261004.log`: event payload and mode ordering,
initially absent/dynamically toggled flags with stable identities, immediate
hide/disable refusal plus native enabled-state propagation, and deletion of the
bar from either command. They use the real native Button providers, not direct
calls to private handlers or synthetic wx command events.

Final evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase `/warnaserror`: PASS,
  `audit108-ribbon-chrome-build1-20261004.log`; the final added tests compile and
  link in `audit108-ribbon-chrome-build2-20261004.log`. Production sources are
  unchanged between these two builds.
- Seven bounded CTest gates: **7 PASS / 0 FAIL**, 66.93 seconds,
  `audit108-ribbon-chrome-ctest2-20261004.log`: dependency self-test, metadata
  (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24), Flyout/autocomplete
  (1097 / 13), native buttonbar (352 / 5), Toolkit Ribbon (3267 / 39).
- `git -c core.safecrlf=false diff --check`: PASS.

SHA-256: Showcase
`9B0F7C3D2DF60FF49E60DFC7EBF934FE8466D0CB3514BAD3EA624F579E9BBC73`,
test_gui `EBA7E47E76AD6DE15257A98C05AF5DB8898EFDCD919E9753BC17453E711AF6EF`,
Ribbon `CBBC7F5885FA93D12554D3D9E5ED91C0C2A78F34C8DA202F31199CFC5A57904C`.
Shared Showcase was re-hashed unchanged at
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`.

All execution remained on the private desktop with WX_UI_TESTS=0 and no physical
input. This closes these two native bar-command gaps, not tab-click temporary
expansion, full bar-style parity, panel extension buttons, arbitrary HWND/GDI
popup children, custom-template migration, or the remaining component matrix.
Physical keyboard/UIA/HDR/Mica, residual blink and soak qualification remain
pending. No new build tree, dependency, commit, push or production-ready claim.

### Native Ribbon page and minimised-panel images (2026-10-04)

Page tabs now honor SHOW_PAGE_LABELS and SHOW_PAGE_ICONS independently. Hidden
visual labels keep their full accessible page name. Page icon bundles and the
public panel minimised-icon bundle become native ImageIconSource objects using
the preferred bitmap size for the owning wxWindow's current DPI. The cache keys
on bundle identity and pixel size, not temporary bitmap reference data:
GetBitmap can clone that data when setting its scale factor. Unchanged syncs
and page-style toggles retain tab and image identities.

Native property callbacks can remove a page while its header is being updated.
The adapter checks weak ownership and current page membership after native
setters, aborts the stale pass and coalesces a deferred synchronization when the
public model changes reentrantly. The loaded callback also stops if that pass
invalidates its owner. No installed test API or other-port behavior changes.

The loaded minimised-button test exposed a real WinUI compatibility defect:
IconSourceElement held the correct ImageIconSource but created BitmapIcon with
a null image. This agrees with Microsoft's
[type-dispatch implementation](https://github.com/microsoft/microsoft-ui-xaml/blob/7b68d3e0b771a57d80098799406234efee479517/dxaml/xcp/core/core/elements/icon.cpp)
and ImageIconSource's
[BitmapIconSource-based implementation](https://github.com/microsoft/microsoft-ui-xaml/blob/7b68d3e0b771a57d80098799406234efee479517/controls/dev/IconSource/ImageIconSource.h).
The adapter now uses the public CreateIconElement factory for the image only,
inside the already-qualified CollapsedButton template snapshot. It retains
margin, alignment and visibility, and leaves the Button, chevron, Flyout,
providers and command mounts intact. Unchanged syncs keep this image too.
Unknown template shapes are not accepted as successful image presentation.

Three new real-XAML cases cover page flags, accessible names with hidden text,
bundle pixel dimensions at the current DPI, actual loaded images in the tab
and minimised button, stable identities, and deletion through public DeletePage
from a native HeaderProperty callback followed by automatic model convergence.
The final focused run passes **114 assertions / 3 cases** in
`audit108-ribbon-icons-tests9-20261004.log`. Earlier failing diagnostic runs are
retained: fixture root lookup was corrected, then the empty native image was
diagnosed without weakening the loaded-image assertion or extending timeouts.

Release test_gui and showcase build9 with `/warnaserror` passes in the existing
`F:\wxwinui-pr26890-feedback-build-static` tree:
`audit108-ribbon-icons-build9-20261004.log`.

Seven bounded CTest gates pass **7 / 7**, 66.99 seconds, in
`audit108-ribbon-icons-ctest9-20261004.log`: dependency self-test, metadata
(42 assertions / 3 cases), opaque GDI (2218 / 7), composites (1901 / 24),
Flyout/autocomplete (1097 / 13), native buttonbar (352 / 5), and Toolkit Ribbon
(3381 / 42). `git -c core.safecrlf=false diff --check` also passes.

SHA-256:

- Showcase: `D4DACADC316ED6D252BFFEB07AE9F303C3F9190A3B28C15B69DE966329EFA862`.
- test_gui: `44738E84807922C5AC33D2DDEF51457384ABEDF4E94DA9A4148F08406E5B79FB`.
- Ribbon: `68D75E45A4A51F7F09659DC3F00F34D75FB1C9E2691FAF9F0BF89FC2F52DAC6E`.

Execution uses the private desktop and WX_UI_TESTS=0, with no physical input.
Shared Showcase and core were re-hashed unchanged at their preceding hashes.
Mica remains enabled; the human-reported residual refresh blink is still open.
These cases do not establish physical mixed-DPI/HDR coverage, complete Ribbon
style/command parity, arbitrary HWND/GDI popup children, custom-template
migration, full component coverage, soak or overall production readiness.

### Native panel extension launchers (2026-10-04)

The public panel EXT_BUTTON flag and bar SHOW_PANEL_EXT_BUTTONS flag now control
real Fluent buttons beside the existing Toolkit panel labels. The inline footer
supports both RibbonGroup (NO_AUTO_MINIMISE) and RibbonCollapsibleGroup. The
collapsed dummy hides its launcher; its real expanded Flyout has a separate
native launcher. Only each label's footer is augmented: the group remains a
direct Ribbon.Items entry and commands, mounts, Flyout and providers are not
recreated. The live button and icon remain stable over ordinary synchronization,
bar-flag toggles and collapse/restore. Showcase's Actions panel now demonstrates
the extension and logs its event.

Invocation sends wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED with the displayed
panel's id, EventObject and GetPanel. The popup uses the real expanded proxy,
including its distinct id, and normal TryAfter propagation forwards the same
event to the original dummy. The callback rejects a disabled/hidden model,
cleared flags, a minimised inline panel, a closed/stale popup or replaced footer.
It performs no post-event mutation: application handlers may delete the bar.
Native enabled state is bound to the group, including outside its visual
ancestry in the popup. The native button also backs IsExtButtonHovered; physical
pointer/keyboard qualification is not inferred from provider invocation tests.

Footer ownership is checked against the exact template and declared parent
collection. Initial tests exposed that closed Flyout content has no visual
Parent even when the collection owns it; this was corrected without opening
the Flyout to qualify ownership, suppressing assertions or increasing waits.
Launcher visibility follows the Toolkit's temporary Visible measurement state
as well as the committed collapsed state, so the launcher is included in its
natural-width calculation. Public state/expansion work remains deferred out of
native measure.

Final evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase `/warnaserror`: PASS,
  `audit108-ribbon-extension-build4-20261004.log`.
- Seven bounded CTest gates: **7 PASS / 0 FAIL**, 67.12 seconds,
  `audit108-ribbon-extension-ctest4-20261004.log`: dependency self-test, metadata
  (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24), Flyout/autocomplete
  (1097 / 13), native buttonbar (352 / 5), Toolkit Ribbon (3601 / 45).
- Three new real InvokeProvider cases: **220 assertions / 3 cases PASS**,
  `audit108-ribbon-extension-tests4-20261004.log`. They cover both panel types,
  flags without a second Realize call, payload, names, stable identity,
  disable/hide refusal, effective native disabled state (also bar-disabled
  while the popup is open), adaptive collapse/restore, actual Flyout/proxy
  propagation, retained-provider refusal after closure, and reentrant deletion.
- wxMSW Debug wxribbon `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-extension-20261004.log`.
- `git -c core.safecrlf=false diff --check`: PASS.

SHA-256: Showcase
`A628D07490C26F94101F9B13BF0CFAF1845963249F74E6DE9B4E5E4E2F80F6BE`,
test_gui `367459FC6BB21770337FCA14977CC51FE96A6B275BC7C7F176FA87C878A3EC8B`,
Ribbon `7ECA6C65210C4BA2FC1E799A5EACD38745ED109B1895CFFF96D53BFBED9D3917`.

This is not full panel/bar parity. Independent expanded-proxy Enable/Hide and
UIA-state propagation still need dedicated qualification, alongside temporary
page expansion, full style/orientation support, arbitrary HWND/GDI popup
children and custom-template migration. All tests run on the private desktop,
WX_UI_TESTS=0, without physical input. Shared Showcase/core remain unchanged;
Mica is not disabled and residual refresh blink remains open. No new build
tree, dependency, commit, push or production-readiness claim is made here.

### Expanded-panel enabled/visible state (2026-10-04)

The independent proxy gap above is now locally covered for native ButtonBar
commands and the panel extension launcher. The new public-method regression
first failed at proxy `Disable()`: the popup footer still reported enabled to
the native control and its real automation peer. Baseline evidence is
`audit108-ribbon-proxy-state-baseline-20261004.log` (22 assertions passed, one
failed); the failing assertion was retained unchanged.

The proxy's `Enable()` and `Show()` now synchronize the existing dummy-owned
native tree after the inherited wx operation, guarded against owner deletion.
Their declarations use the installed WinUI configuration, not the private
Toolkit option; without the Toolkit implementation the inherited behavior is
unchanged. A private non-tab-stop carrier projects the proxy's own enabled bit
onto the extension launcher, while the existing group-state binding preserves
ancestor disablement. Hiding the proxy hides the whole footer, label included.
Command carriers continue using the common host's effective wx-parent state;
individual command flags are not overwritten. A retained ButtonBar provider
also refuses activation while its wx owner is hidden. Restoring the expansion
clears the proxy-only footer state without replacing the native tree.

The test `[winui-ribbon-proxy-state]` exercises Disable/Enable, Hide/Show and
closing a still-disabled/hidden proxy without another Realize call. It checks
real control and automation-peer enabled state, loaded native visibility,
retained-provider refusal, stable roots/buttons, the live expansion identity,
and restored command activation under the original dummy. It does not invoke
a successful command while requiring the popup to stay open: such activation
normally calls HideIfExpanded and that behavior is preserved.

Final evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase `/warnaserror`: PASS,
  `audit108-ribbon-proxy-state-build1-20261004.log`.
- Focused regression: **151 assertions / 1 case PASS**,
  `audit108-ribbon-proxy-state-test1-20261004.log`.
- Seven bounded gates: **7 PASS / 0 FAIL**, 67.80 seconds,
  `audit108-ribbon-proxy-state-ctest1-20261004.log`: dependency self-test,
  metadata (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24),
  Flyout/autocomplete (1097 / 13), native buttonbar (352 / 5), and Toolkit
  Ribbon (**3752 / 46**).
- wxMSW Debug wxribbon `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-proxy-state-20261004.log`.
- `git -c core.safecrlf=false diff --check`: PASS.

SHA-256: Showcase
`ADE0C5CFEFCD2DE1B74924665D347FBE984663DF504DA8A7FC8718C415C42C32`,
test_gui `90ADCEF427994960302E4BCCB923AEBF4169ED1EF74B679302CC5C4D0159B270`,
Ribbon `0943E9625DF2E5D857542689CDE593DDAD9155BB2F3C318FC651E6A923B64CF9`.

This is a bounded state/presentation regression, not full UIA navigation,
keyboard, pointer, HDR or physical qualification. Temporary page expansion,
the remaining style/orientation matrix, arbitrary HWND/GDI popup children and
custom-template migration remain open. All runtime checks used the private
desktop with WX_UI_TESTS=0. Shared Showcase/core were rehashed unchanged. Mica
is retained and the human-reported refresh blink remains open. No dependency,
build tree, commit or push was added; plans 108-112 are not complete.

### Temporary Ribbon display/focus work — NOT qualified (2026-10-04)

This newer worktree checkpoint supersedes the green proxy-state checkpoint
above for the current static artifacts. Do not infer that the earlier 7/7
result applies to the current code. The shared human-tested Showcase and core
DLL have not been replaced.

Implementation in progress:

- Native selection opens MINIMIZED into EXPANDED before PAGE_CHANGING, matching
  the generic event order; veto preserves the old page without pinning the bar.
- Native tab activation distinguishes reactivation of the selected tab from
  selection of another tab, using the original TabViewItem template/provider.
- Deferred, coalesced focus checks observe native FocusManager transitions and
  ordinary wx focus/activation events. Internal command focus is retained;
  focus to a native sibling or classic HWND closes temporary, not pinned mode.
- Popup focus ownership is checked against the exact current Flyout content
  and its owning Popup/presenter, including WinUI's intermediate Popup focus.
  Unrelated popups on the same XamlRoot are not accepted.
- Minimizing closes expanded group Flyouts outside the inline page tree, with
  weak owner snapshots and reentrant reopening checks.

Five new cases keep real selection/invoke/focus providers and current native
identity checks. Four pass in the focused run. The fifth (popup focus) fails
before reaching its focus/closure oracles; those oracles are NOT qualified.
Physical pointer/key tab reactivation, top-level activation loss, full layout
space release, and the remaining display/style matrix are still open.

Current evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- Release test_gui + showcase `/warnaserror`: PASS,
  `audit108-ribbon-display-owned-popup-build-20261004.log`.
- Focused display/extension run: **6 PASS / 3 FAIL** cases; 794 assertions pass
  and three fail, `audit108-ribbon-display-owned-popup-test-20261004.log`.
- Seven bounded gates: **6 PASS / 1 FAIL**, 66.96 seconds,
  `audit108-ribbon-display-final-ctest-20261004.log`. Dependency, metadata
  (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24), Flyout/autocomplete
  (1097 / 13) and native buttonbar (352 / 5) pass. Toolkit Ribbon is red:
  **47 PASS / 4 FAIL** cases, 4124 passing and four failing assertions.
- Failing cases: PageFlagsPreserveNativeImagesAndAccessibleLabels,
  ExpandedPanelExtensionUsesRealProxyAndRetiresProvider,
  ExpandedProxyProjectsEnabledAndShownStateWithoutRealize, and
  NativePopupFocusStaysInsideTemporaryRibbonBoundary. The first is native image
  readiness; the other three are native popup Loaded/visibility readiness.
- wxMSW Debug wxribbon `/warnaserror`: PASS,
  `F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-display-20261004.log`.
- A final test-only failure diagnostic builds successfully in
  `audit108-ribbon-display-host-visibility-build-20261004.log`. Its isolated
  popup-focus rerun remains red (16 assertions pass / 1 fails):
  `audit108-ribbon-display-host-visibility-test-20261004.log`. At failure the
  proxy and Flyout are open, the bar is EXPANDED, XamlRoot.IsHostVisible is true,
  and the frame is shown/non-iconized at (156,156), size 1116x419. Command and
  extension IsLoaded remain false. This is not evidence of user input failure.

The readiness assertions and 500 ms bound remain unchanged. A diagnostic late
visibility observation at 539 ms did not also require an open popup/live proxy;
it is NOT accepted as proof of a simple timeout problem. Raising the bound and
adding rendering observers did not qualify the popup. Those experiments were
removed. The State -> deferred Sync -> ForceRender path is a possible feedback
path, not a proven root cause. The source of the regression remains open; no
readiness or production claim is made for this lot.

Current SHA-256: static Showcase
`8CD3607B2B03549AD0409C5FFFEE68830AC27585AAF7AE8B05668BAD34E61D73`,
test_gui after the final test-only diagnostic
`1E3BE7AC71F4C9BCB965AF0AC6E4BA0D87B4618045CA11A90C9C1F490AB64E0D`,
Ribbon `46C15D9D22AAD9E9B9E5CF0745A0450BB8036A18CC7258097967C34F9818775B`.
The seven-gate run used test_gui
`0F1872ACB128B10C4182B1D74698458B90869CB3D4BABED666FA8105619CCBA7`
before that diagnostic-only edit; it is not a claim of a full rerun on the
later test binary.

Shared Showcase/core were rehashed unchanged as
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22` /
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`.
All runtime checks used the isolated non-input desktop with WX_UI_TESTS=0.
Mica remains enabled; HDR/move/resize visibility is human-confirmed, and the
human-reported refresh blink remains OPEN. No foreground run, physical input,
dependency, build tree, commit or push was added. Plans 108-112 remain open.

### Temporary Ribbon display/focus follow-up (2026-10-04)

The previous red checkpoint was investigated without changing its required
Loaded/visible state, native provider/focus assertions or physical-retirement
oracles. Its two distinct causes were separated:

1. The test-only FindRibbonChromeButton recursively visited Panel.Children
   and then the same panel's visual children again. Nested panels amplified
   the lookup cost inside each readiness predicate, preventing useful native
   dispatcher progress. A retained-element diagnostic passed all 111 popup
   focus/closure assertions; removing the duplicate Panel walk then made the
   original, uncached predicates pass in all three popup cases. The 500 ms
   readiness bound remains unchanged. The one-second experiment did not fix
   the failure and was removed. Native State tracing showed only one collapse
   transition, not a State/Sync oscillation; no speculative production layout
   workaround was retained.
2. The private C++/WinRT TabViewItem activation adapter exposed its first
   implemented interface, IContentControlOverrides, as its runtime class name.
   The actual rendered template was ListViewItemPresenter with two Borders,
   while the native TabViewTemplateSettings held an unmounted ImageIcon.
   Setting DefaultStyleKey alone did not repair it. The adapter now preserves
   the native TabViewItem runtime class identity; it adds no public XAML type
   or properties. The real ImageIcon is now loaded/arranged in the visual tree
   and the unchanged page-label/icon flag and accessibility checks pass.

Diagnostic logs retain the unsuccessful style-only/timeout approaches and
the corrected evidence. In particular, Loaded at 531/522 ms after the first
wait was NOT proof that increasing the timeout was a fix: the later predicate
stopped doing the redundant tree search. The root cause was discriminated by
the retained-observer and no-duplicate-walk runs, not by accepting delayed or
hidden controls. Tests retain concise native-state diagnostics on failure;
temporary Rendering/State observers and extra retry loops were removed.

Release test_gui + showcase `/warnaserror` passes in
`audit108-ribbon-display-tab-identity-build2-20261004.log`. The focused selection
passes **1191 assertions / 12 cases** in
`audit108-ribbon-display-tab-identity-test-20261004.log`, covering the five new
temporary-display/focus cases plus existing extension, proxy-state and image
cases. The popup case reaches all 111 assertions: actual command/footer focus,
host logical command ownership, retained native identities, outside focus,
temporary bar and Flyout closure, restored wx parent, and physical gate
retirement. Native sibling and classic HWND exits preserve pinned mode.

wxMSW Debug wxribbon `/warnaserror` also passes in
`F:\wxwinui-pr26890-msw-audit-build\audit108-ribbon-display-qualified-20261004.log`.
The complete seven-gate run passes **7/7, zero failures, 69.19 seconds** in
`audit108-ribbon-display-qualified-ctest-20261004.log`: dependency self-test,
metadata (42 / 3), opaque GDI (2218 / 7), composites (1901 / 24),
Flyout/autocomplete (1097 / 13), native buttonbar (352 / 5), and Toolkit Ribbon
(**4458 assertions / 51 cases**). This supersedes the prior red checkpoint
for these bounded local gates, not for physical or full-port qualification.
`git -c core.safecrlf=false diff --check` also passes.

Current SHA-256: static Showcase
`25FDAE8F2A2D6F5CDA3C36A9DF35566ADA55021F0FB09EF107F2F206F10CA78C`,
test_gui `6503723EC0ABD4225BF042187856D5F5EF8CA9AA6A71A422629B19D3059618F9`,
Ribbon `6D890181B604CE3CCD30DA38FEA46B1C049F88F7C056D11B56525C528FACFD4D`.
Shared human-tested Showcase/core were rehashed unchanged. No physical input
was injected; all runtime checks used the isolated non-input desktop,
WX_UI_TESTS=0. Physical tab reactivation/key handling, top-level deactivation,
layout-space release, full style/orientation coverage, arbitrary HWND/GDI
popup children and custom-template migration are not qualified by this lot.
Mica is preserved; refresh blink and the remaining plans 108-112 stay open.

### User-visible component priority correction (2026-10-04)

The user confirms that the shared Showcase built at 00:53 already solves the
Mica/HDR visibility problem. Further refresh/backdrop experiments are parked.
The speculative monitor-prime test was removed before implementation; no
monitor-prime product change was made. The accepted shared Showcase/core
hashes remain unchanged. The separate real RichText/STC/HTML paint test passes
166 assertions, but is not a reason to continue revisiting an accepted fix.

The reported native grip failure is reproduced at the slot routing boundary:
after the grip accepts a press, the statusbar's handledEventsToo listener routes
the same pointer again, advances the InputSite generation, and invalidates the
pending native resize. The new LTR/RTL test fails 34 assertions before the fix
(`audit108-grip-bubble-red-20261004.log`). The slot now leaves an already-owned
resize pointer to the host transaction/root cancellation path. The unchanged
dispatch validity checks and early-release cancellation remain in force.
The focused native-resize suite passes 296 assertions / 5 cases after the fix
(`audit108-grip-bubble-fix-test-20261004.log`). Actual mouse validation of the
rebuilt static sample remains pending; this is not a physical sign-off.

The native Toolkit Ribbon containers exist only in the experimental static
build, not in the user's accepted shared 00:53 executable. Its native C++
dependency is XamlToolkit.Labs.WinUI.Native, a C++ port of the Community Toolkit,
not the official C# package silently linked into wxcore. Build-tree success is
not completion of shared/install/dependency distribution or the whole Ribbon API.

At the start of this follow-up, DataView, Grid and PropertyGrid still used
generic drawing. Native colours,
rounded decoration, and passing model tests do not count as a WinUI visual
port. The 617 assertions / 48 existing DataView/TreeList cases passing in
`audit108-user-dataview-baseline-20261004.log` characterize retained model/API
behaviour only. A real port needs virtualized XAML row/cell/column peers, with
wx model notifications, hierarchy, selection, edit/validation/veto, sorting,
keyboard and UIA semantics connected to them. Showcase omissions remain open;
removing their explanatory labels is not an implementation.

The official w-ahmad/WinUI.TableView evaluation sample was compiled separately
at `F:\winui-tableview-demo\publish\WinUI.TableView.SampleApp.exe`, from commit
`f01c70efbbdbb15aca23fb6549ce424c3ddf5c7d` (MIT). The complete publish directory
is required. Build exit 0, zero errors, four upstream define warnings; no GUI
launch or visual qualification is claimed. The adjacent DEMO-LOCAL.txt records
the isolated portable SDK, aligned WinUI build dependency and complete XBF/asset
copy. This C#/.NET library is not directly a native C++/WinRT component. Using
it behind wx requires a managed/native boundary and runtime distribution;
otherwise a C++/WinRT tabular peer must be implemented. The standalone sample
does not implement wxDataView or wxGrid and does not close either family.

### Native component implementation follow-up (2026-10-04, in progress)

The default CheckListBox item no longer projects its ordinary checkbox and
label through a GDI bitmap. It declines custom drawing so that the existing
native ListView row presents its real CheckBox and string. Application-defined
owner-drawn items keep their explicit drawing path. The focused list suite
passes 633 assertions / 14 cases in
`audit108-native-checklist-regression-20261004.log`, including the new visible
native-label test in LTR/RTL and the custom drawing/focus regression cases.

The first editable ComboBox presentation test also reproduces the width
regression in a StaticBoxSizer: the wx allocation is 232 pixels but the nested
native ComboBox stays at its 70-DIP desired width. Stretching the semantic
ComboBox inside its retained Grid corrects that measurement. The initial-value
test now also passes after deferring text publication until the native editor
generation is installed. The WinUI template intentionally keeps the
editable TextBox at zero size outside focus, so the resting faceplate and the
focused editor must be tested separately, not by asserting that the dormant
editor already has a visible rectangle. Selection now invalidates measure
asynchronously when an exact text-projection ticket remains pending. The
completion fixture waits for the actual native GotFocus before injecting its
next edit (WinUI schedules SelectAll during that transition). The ComboBox
cases pass in `audit108-native-components-private-visible9-20261004.log` without
suppressing native focus behaviour or relaxing event-count assertions.

Native DataView implementation has started in `src/winui/dataview.cpp` and
`include/wx/winui/dataview.h`. It is an actual XAML ListView/ItemsStackPanel with
on-demand native cells, not the generic drawn view with a new palette. The
public dispatch distinguishes `wxHAS_WINUI_DATAVIEWCTRL` from the generic
view while retaining shared renderer and column-mutation contracts. Renderer
implementations were moved, not copied, from datavgen.cpp into datavcmn.cpp.
That mechanical separation compiled under wxMSW Debug `/warnaserror` and
preserved the 617-assertion / 48-case pre-native model baseline. Neither result
qualifies the new native view. The native library and tests now compile with
`/warnaserror` (`audit108-native-components-build6-20261004.log`). The first
visual-tree check caught a real implementation error: assigning a Canvas to
the virtualized container's Content did not attach it to the rendered tree.
The implementation now declares an explicit DataTemplate and realizes cells
in ContentTemplateRoot, retaining the native ListViewItemPresenter.

The combined private-visible9 run passes 62 cases / 2583 assertions. Native
toggle/text/progress/check-icon-text cells are found under the actual visual
tree; UIA Toggle reaches the wx model and the TreeList two/three-state cycles
are preserved. Native TextBox commit, validation and public
FinishEditing/CancelEditing tests pass, as do the portable DataView/TreeList
model and geometry tests. Two initial-loading failures in earlier runs were
isolated to entirely off-screen test windows: the HWND and XamlRoot were
correctly sized while XAML had not arranged the culled visual root. Placing
the same fixtures inside the private test desktop's viewport makes the same
sequence and strict visual-tree assertions pass; no host rendering workaround
or longer wait was added. This is not evidence of a product lifetime fix.
The 10,000-row / 40-column test verifies realization and off-screen bounds,
not full production performance. A dedicated `wx_winui_dataview_native` CTest
gate and CI step now include native DataView plus portable DataView/TreeList
contracts; remote CI execution is not claimed.
Showcase's DataViewListCtrl and TreeListCtrl sections are re-enabled only when
the real WinUI backend is selected and are included in the rebuilt demos below.
Date/choice/spin cells, column gestures, complete editing,
drag/drop and Grid/Table-specific UIA remain open. wxGrid and PropertyGrid
are not replaced by this DataView implementation.

Ribbon ScrollLines/ScrollPixels/ScrollSections now target the pinned Toolkit template's
actual ScrollViewer, including DPI conversion and native-offset observation.
The first overflow test exposed a fixture issue: ButtonBar::GetMinSize()
ignores the wxWindow minimum, so the required width is now expressed on the
panel's wxSizer. The expanded native scrolling test passes in runtime6,
including LTR/RTL, partial groups, consecutive requests, bounds, resize, and
page retirement while a native scroll is pending. Its page-ID reuse fixture
uses an explicit application ID instead of reusing a released automatic ID.
The seven metadata/GDI/composition/native-Ribbon/Toolkit gates also pass on
build8 in `audit108-native-components-cross-regression8-20261004.log`.
Vertical orientation, shared libraries, installation and the other previously
listed Ribbon contracts remain open.

### Native demo delivery (2026-10-04, build10)

The diagnostic environment switch is removed. Rendering fixtures stay inside
the viewport only on the runner's named private desktop and remain
non-activating/off-screen when invoked outside it. The final default combined
run passes **2573 assertions / 62 cases** in
`audit108-native-components-runtime10-20261004.log`; the earlier diagnostic
run included ten extra assertions checking the private desktop. The dedicated
`wx_winui_dataview_native` CTest passes in
`audit108-native-dataview-ctest10-20261004.log`. Native resize/state passes
**310 assertions / 6 cases** in
`audit108-native-components-resize9-20261004.log`; this is not physical mouse
qualification. Header dispatch and its negative fixtures pass for 36 headers.

`audit108-native-components-demos10-20261004.log` records successful
`/warnaserror` builds of test_gui, showcase, minimal, dataview, grid,
ribbondemo and widgets. Executables and their required runtime payload remain
together under `F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`.
No sample was launched on the user's input desktop and no visual sample
acceptance is claimed. Grid is provided for comparison and is still generic.

SHA-256 of the delivered static demos:

- Showcase: `34FD74F57625EA912AD1C5F0FA4BC739E181773C3608A3C3EF3A5B6B10621D19`.
- DataView: `4F42C55EC393ABCE16057613F0F2C714E24F6D62FC0F0B381C1B9ADDF2905A6C`.
- Ribbon: `BB26C3B62ECE70011A0E564787417381691E5D67870B5B9E0F67F71BAB5B806B`.
- Minimal: `AB954B2F7FCA60EE177C3141C11C3386AD86031710B8891C7BA69B5D02878BBE`.

The accepted shared Showcase/core were rehashed unchanged:
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22` and
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`.
No commit, push, new release qualification or shipping promotion is implied.

### Native editors and retained column headers (2026-10-04, build19)

The native DataView now presents standard Choice/ChoiceByIndex/Spin/Date
values with XAML text and edits them with ComboBox, NumberBox and
CalendarDatePicker. Tests exercise duplicate-label indexes, variant types,
integer bounds/invalid input, a civil date in 1600, null dates, Validate and
editing-done vetoes, public Finish/Cancel, and reentrant sizing callbacks.
Popup editors live outside recyclable rows and retain their exact native
mount until popup closure and focus completion. The host's logical-retirement
focus guard remains intact; cancelled GettingFocus transfers are accounted
for by their admitted correlation ID, not a timeout or fabricated LostFocus.

Native headers retain their Thumb across width/model changes and share one
width authority with UIA Transform. Tests cover minimum widths, LTR/RTL,
sort invocation, disabled/non-resizable columns, and retired providers.
This does not qualify physical pointer dragging or UIA Grid/Table patterns.
Ribbon SetTabCtrlMargins now affects the real native tab band with DPI
conversion, without shrinking the page content or replacing live peers.

The final /warnaserror build is recorded in
`audit108-native-editors-columns-build19-20261004.log`. The targeted native
set passes **612 assertions / 13 cases**, and the combined DataView/TreeList,
ComboBox, label, Ribbon margin/scroll and popup-retirement run passes
**3627 assertions / 79 cases** in
`audit108-native-components-runtime19-20261004.log`.
Eight CTest gates pass in
`audit108-native-components-cross-regression19-20261004.log`, including the
dedicated DataView gate, native/Toolkit Ribbon, composition, opaque GDI,
metadata and Toolkit self-tests. The common renderer metadata also compiles
under wxMSW Debug /warnaserror; its DataView/TreeList run passes
**632 assertions / 48 cases** in the MSW audit build's
`audit108-native-dataview-common-runtime-20261004.log`.

One apparent Calendar teardown failure was a fixture pump defect, not a
reason to weaken the lifetime contract: after 3109 ms of Yield/Idle/Sleep,
the actual event-loop DispatchTimeout delivered Closed and retirement in
31 ms (`audit108-native-calendar-isolate18-20261004.log`). The final fixture
uses the real loop with a two-second deadline and asserts one Opened and one
Closed. Temporary tracing, extra observation waits and speculative queued
Calendar Hide replay were removed before the successful build19 campaign.

This remains an experimental native component increment, not full parity or
production qualification. Custom renderer contracts, column reordering,
drag/drop, Grid/Table UIA and physical interaction qualification remain open.
wxGrid/PropertyGrid/EditableListBox are not ported by this work. Ribbon
vertical orientation, shared-library/install qualification and its remaining
API matrix are still open. The accepted shared Showcase/Mica build is untouched.

Build19 demos are delivered in the existing static binary directory above.
The initial Showcase link attempt was blocked by a running user-opened
executable, not a compiler error. After the user closed Showcase and DataView,
their /warnaserror rebuild succeeded in
`audit108-native-components-showcase-dataview19-20261004.log`.
Minimal, Grid, Ribbon and Widgets succeeded in
`audit108-native-components-unlocked-demos19-20261004.log`.
No process was forcibly closed and no foreground sample was launched.
Native resize/state regressions still pass **310 assertions / 6 cases** in
`audit108-native-components-resize19-20261004.log`; physical mouse validation
remains a separate requirement. Grid remains a generic comparison sample.

SHA-256 of the refreshed static demos:

- Showcase: `AE2EAB11280182BC861FFA0810CD44C49629EBD7BD44932D800EBF68D14084A3`.
- DataView: `708CFC49DD4C08824CE9A3456CAFC75C7F58D70DCEF0D2F1385A13233AA01C19`.
- Ribbon: `306BC46FFA4BC0D328B9CF1DA05B2F5BBCDDDFB31CF7227B877E5FCA83454C59`.
- Minimal: `DD6D219B4A0F377731C6B8ABAEFF5BE23A5F8770EFD59D2BD1F6436FC6BF7059`.

No commit/push, full-port completion, physical qualification or production
promotion is implied by this incremental delivery.

### Native wxGrid foundation and DataView UIA (2026-10-04)

`wxGrid` now owns a source-private native presentation over its existing
table/selection/editing state. The body, frozen panes and labels have real
XAML slots. Only the exposed rows and columns are realized: standard string,
number, float and wrapped-text renderers use native text controls, booleans
use CheckBox, and headers use native controls. Explicit custom Draw overrides
remain wxDC-rendered images, not falsely advertised as native renderers;
without RTTI this conservative compatibility route is used for all cells.
Printing and the existing scrollbar infrastructure are unchanged.

Projection publication respects table/row/column generations and the existing
batch and model-attachment barriers. Native Invoke/Toggle reuse the public
selection and editing transactions, including veto. `Update()` synchronously
publishes pending native changes, and first visible presentation establishes
the same silent initial current cell as the generic DrawHighlight path.
Tests retain stale providers across row deletion, table replacement and owner
destruction, including mutations from GetValue/Draw and recursive Update.
The native grid suite passes **178 assertions / 8 cases** in
`audit108-native-grid-runtime26-20261004.log`. This includes a 100,000-row /
2,000-column virtual model and both-axis/frozen viewport bounds; it is not a
performance or physical-input qualification. The new `wx_winui_grid_native`
CTest is included in the CI component contract selection.

DataView now supplies native Grid/Table, GridItem/TableItem and virtualized
cell providers. Off-screen GetItem does not realize or read the cell; Realize
uses the same model identity as the eventual native visual. Hidden/deleted
columns, replaced models and destroyed owners invalidate retained providers.
Header providers use the existing native headers and width authority.
The four new UIA tests pass **161 assertions**, and DataView/TreeList plus
native editor regressions pass **976 assertions / 48 cases**, in
`audit108-dataview-uia-runtime22-20261004.log` and
`audit108-dataview-regression22-20261004.log` respectively (overlapping sets).

The first UIA run exposed a delayed crash after a test had printed PASS.
The native PDB stack identified UnpegManagedPeerNoRef from an asynchronous
InvalidatePeer callback after destruction. The adapter no longer queues that
unnecessary invalidation: providers resolve live state and structure changes
still raise ChildrenInvalidated while retaining the peer for the call. Both
final runtime logs above also have a successful process exit, not just passing
Catch assertions. No timer, extra shutdown drain or lifetime guard weakening
was used to hide the fault.

The Mica grid tests now verify actual XAML-owned surfaces and transparent
backing, instead of requiring the obsolete opaque GDI label publication.
The portable wxMSW Debug build also passes /warnaserror and its Grid/DataView/
TreeList run passes **1229 assertions / 111 cases** in
`audit108-native-grid-msw-runtime22-20261004.log`. Mouse-driven common tests
which cannot run on the private desktop report skips; no physical acceptance
is inferred. Resize/state regression remains **310 assertions / 6 cases** in
`audit108-native-components-resize24-20261004.log`.

Open scope is explicit: complete wxGrid renderer/API parity, Grid-specific
UIA patterns, physical pointer/keyboard/editor behaviour, DataView cell-level
selection/editing accessibility and external UIA clients are not closed by
these tests. PropertyGrid and the other generic families are not silently
promoted to native WinUI by this increment.

### Native vertical Ribbon and integrated validation (2026-10-04, build28)

The experimental Toolkit Ribbon now has a genuine vertical XAML template,
native ScrollViewer scrolling on the selected axis, adaptive compact groups
and guarded horizontal/vertical template changes. Ribbon.Items, groups,
commands and their providers remain the same objects. A live expanded panel
must finish its native popup/focus retirement before the template can change;
the corresponding test also destroys the bar from a template callback.

The first adaptive test exposed a real icon measurement defect: the ImageIcon
created from the wx bitmap had Auto dimensions, making a compact group measure
544x549 instead of shrinking from 254x268. The replacement icon now uses the
preferred bitmap extent converted to DIPs and preserves the placeholder's
constraints/alignment. Its size is resynchronized even if the image source
is unchanged across DPI changes. Tests check Width/Height and the realized
ActualWidth/ActualHeight, not just group State. Temporary tracing, ineffective
measure invalidation and speculative viewport hooks were removed. Vertical
layout/images pass **451 assertions / 6 cases** in
`audit108-native-ribbon-vertical-runtime27-20261004.log`.

The broader build27 run also caught a default-construction ComboBox defect:
its getters returned -1 although the uncreated model's initial text range is
(0,0). Only the pristine no-peer/no-HWND/no-parent state now uses that range;
post-Create peer resolution still fails closed. The test checks that these
reads do not instantiate a native peer and supports null output pointers.

That broad selector accidentally included the hidden physical clipboard test.
It failed on the private desktop and is not a successful physical result.
The final selector explicitly excludes `[physical]`. The clipboard test now
also honors EnableUITests before touching the clipboard, creating a window
or injecting input. Three common grid mouse tests missing this same guard
were aligned with the twelve already guarded simulator tests in that file.
No physical assertion was removed or weakened.

The final /warnaserror build is
`audit108-native-components-build28-20261004.log`. Its combined native Grid,
portable Grid, Mica, DataView/TreeList, Toolkit Ribbon and item/ComboBox model
run passes **10890 assertions / 227 cases**, exit zero, in
`audit108-native-components-runtime28-20261004.log`. WX_UI_TESTS=0 and a named
private desktop were used: this remains deterministic automation, not manual
input, external accessibility, HDR/DPI visual acceptance or soak sign-off.

The nine integration CTests all pass in **87.68 seconds**, including native
Grid, DataView, native/Toolkit Ribbon, composition, opaque GDI, metadata and
both Toolkit self-tests:
`audit108-native-components-cross-regression28-20261004.log`.
The refreshed wxMSW Debug /warnaserror build and Grid/DataView/TreeList run
also pass **1229 assertions / 111 cases**, now with the missing mouse guards,
in the MSW audit directory's `audit108-native-components-msw-build28-20261004.log`
and `audit108-native-components-msw-runtime28-20261004.log`.

All six static demos were rebuilt successfully with /warnaserror in
`audit108-native-components-demos28-20261004.log`. Use the executables in
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`; no sample was
launched on the user's desktop and no running user process was closed.
Their SHA-256 hashes are:

- Showcase: `9D7E0CEAFC459B7C06D478648082B19BB79A1B54A85C30932DC7AD535743CC03`.
- DataView: `8E1969B4BF0135701A7D8D20FBFDF70D3EE45E3E15E75B03D6B78D7B28669067`.
- Grid: `8509CBC11FE5114392C8E075ED2CD64085B0D6D95E7D34C6B166F82E1AE34F2C`.
- Ribbon: `929EB6633DB7BCF182718415D325CECB3AADE64CFADA85EA77CA79D0D375066D`.
- Minimal: `582DB521495324DBB07B345096A846D0374300DB3420AFD280B3C1F64198C00A`.
- Widgets: `05F39703B454FBA1AAAB8487E7F639DF8541D653304A3FBE25D6EBA9BB066579`.

The accepted shared Showcase/core hashes remain
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22` and
`878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`.
The README and capability matrix distinguish these newly native surfaces
from remaining custom/generic paths. No commit/push, global port completion
or production promotion is claimed.

### 2026-10-04: list height, native window scrollbars, grip capture and DataView churn

The user's four reports are the priority of this increment. Native ListBox
items no longer receive the compact wx font-derived fixed height: their
WinUI template measures them, including native CheckBox content and large
fonts. Explicit application owner-drawn bitmaps retain their existing height
contract. The pinned template measures 40 DIP in the default test; Showcase
uses the controls' best size instead of clipping four rows into 120/130 DIP.

The host now projects wx-managed HWND scrollbars as actual XAML ScrollBar
peers with transparent track resources outside high contrast. SCROLLINFO,
32-bit positions, client geometry and wx scroll events remain authoritative.
The native non-client track is suppressed only after the matching peer and
bridge region are published. Generations, queued UIA supersession and
callback-stack retirement are guarded. Explicit HWND regions, internal
native-control scrollbars and windows without an existing WinUI host retain
their MSW fallback; this is not a claim that every scrollbar is migrated.
The readiness test caught a ZIndex above WinUI's supported maximum; the
native value is now bounded to 1000000 without increasing test timeouts.
The new wx_winui_window_scrollbar_native CTest is also selected by CI;
remote execution has not been performed for this uncommitted increment.

Grip resize preserves an accepted request when native capture transfers
between the same island's bridge and InputSite before the queued native
wake. Null/foreign capture, release and cancellation still cancel it. Two
new cases exercise both directions, LTR/RTL, the real USER32 resize loop
and cancellation paths on a private desktop, without physical injection.
The user's actual mouse gesture remains a required acceptance check.

DataView reconciles retained cell presentations rather than clearing each
row's Canvas on selection/focus/viewport notifications. Unsorted ValueChanged
updates only the affected cell without GetCount/GetChildren traversal or
unloading neighbours. Detached Toggle callbacks cannot write the model.
Custom renderers still receive real selection/focus/metric invalidation.
The focus state is obtained from FocusManager's actual native subtree.
Diagnostics distinguished normal first realization of cache rows 6-8 from
repainting rows 0-5: the final test permits exactly one initial value/attribute
read per new cell and no repeat read for an existing presentation, then
exactly one value/attribute read for the targeted update. Temporary tracing
was removed. UIA Value/RangeValue now uses native editing and the existing
validation/veto pipeline, including reentrant destruction. A new hostile
test originally deleted its owner while allowing wx event dispatch to
continue; it now explicitly consumes that destroying event.

The same increment finishes native standard Grid choice/enum/date/datetime
renderers; custom subclasses and no-RTTI builds retain the Draw contract.
Build29's isolated renderer run passes 255 assertions / 4 cases.

Final Release/static /warnaserror build:
audit108-native-components-build33-20261004.log. Its focused run passes
**751 assertions / 11 cases** in audit108-native-components-targeted33-20261004.log.
The broad Grid/DataView/TreeList/Ribbon/item/list/native-resize/scrollbar run
passes **12231 assertions / 247 cases**, exit zero, in
audit108-native-components-runtime33-20261004.log. The nine separate CTests
pass **9/9 in 85.44 seconds**, including composition/opaque GDI and the new
scrollbar gate, in audit108-native-components-cross-regression33-20261004.log.
All GUI tests use WX_UI_TESTS=0 on a named private desktop; the item-model
selector explicitly excludes physical tags. This does not qualify real input, external UIA clients, HDR/DPI
visuals or soak. Ordinary wxMSW Debug also builds with /warnaserror and
passes **1229 assertions / 111 cases** (audit108-native-components-msw-build30-20261004.log
and audit108-native-components-msw-runtime30-20261004.log in the MSW audit build).

The human-accepted shared Mica candidate is not rebuilt. No user process is
terminated, no new build directory is created, and no commit/push or global
production-readiness declaration is made by this increment.

Five static demos were rebuilt with /warnaserror in
audit108-native-components-demos33-20261004.log (2026-10-04, 14:24-14:25 local).
Their SHA-256 hashes are:

- Showcase: `B7B69FF2213BA1DBFD25969901C58B4C0B2506300E7747458AE5FB6F0FB1A91E`.
- Minimal: `09D95B872BC29591AF31408FF10619564B5C4CCE3728F6A86DC4CC70883B65C1`.
- Grid: `85CD8FBFEA548EB323F408BACF182B38C25096C9C0CC03FE3B0F1C207FCBC90D`.
- Ribbon: `C2788082269DF764BB952AE4F90FA4E50362167A18D34EF23A4AACCC407339CC`.
- Widgets: `3FEC74A0C7B17181659179ED08E8501B0D9CA719CFD0996352574BD5445B50CE`.

The standalone DataView executable remains at build28 (13:27:49) because
the user's process 64492 still holds it open. It was not overwritten or
terminated. The newly rebuilt Showcase includes the new DataView backend;
the separate DataView sample still requires closure and relinking.

### 2026-10-04 human video feedback on build33 (not visual acceptance)

The user supplied `20261004-1230-11.6654534.mp4` and confirmed that the
bottom-right resize handle now works. Keep its input/capture implementation
unchanged. The same recording rejects the live-resize smoothness and the
outer window scrollbar: it flickers and does not behave/look like the
native ListView scrollbars, which the user explicitly accepts as reference.
Build33's geometry/UIA test passes did not cover that presentation contract
and must not be cited as visual qualification. Mica remains required.

Follow-up scope is the outer scrollbar's native indicator lifecycle and
paint budget, retained status-bar content during size-only changes, and
avoiding repeated full bridge-region installation when there are no native
cut-outs. No physical gesture or HDR acceptance is inferred from automated
tests of these changes.

### 2026-10-04 video follow-up: native indicators and bounded repaint

The outer window bars now keep the stock WinUI template and theme resources.
Their mouse/touch indicator lifecycle follows content activity, thumb capture
and the system auto-hide preference, rather than leaving IndicatorMode at None
or overriding the native track palette. Retiring a projection also retires its
timer and native subscriptions. These are native ScrollBar peers, not GDI art.

Scrollbar position writes and unchanged projection syncs no longer invalidate
the scrolling client. Backing cleanup is restricted to changed non-client bands
and their corner; range-change tests distinguish USER32's own invalidation from
any extra invalidation introduced by the projection. A bridge with no native
cut-outs uses its natural bounds instead of reinstalling a full-window region
on every resize. Transitions to and from actual native scrollbar holes remain
covered in LTR and RTL. StatusBar size-only changes retain the same native root,
fields and resize grip instead of unloading/recreating them; the accepted grip
input/capture implementation and the Mica/HDR paint path are unchanged.

The same built source contains corrected physical-screen coordinates for
DataView UIA column reorder in RTL and an art-provider FontSize binding to the
actual native Ribbon tab presenter. The latter was necessary because the pinned
WinUI template otherwise fixes the displayed label at 12 DIP. Its regression
checks actual TextBlock size/height, retained identities and text-only native
retemplating. Direct retemplating while retaining an old icon-bearing template
raised a shared IconElement parent error in the test fixture; this run does not
qualify that separate native-template replacement scenario.

Evidence is kept, including failed intermediate test/build logs. Build35b passed
with /warnaserror; its eight cross-regression CTests passed in 96.95 seconds
(`audit108-scroll-resize-cross-regression35-20261004.log`). Build36 rebuilt
test_gui and all six static demos with /warnaserror
(`audit108-scroll-resize-build36-demos-20261004.log`). The later builds37-39 change
only test code; the isolated final Ribbon font run passes 95 assertions / 3 cases
in `audit108-scroll-resize-font39-20261004.log`. MSW Debug also built with
/warnaserror and passed 1229 assertions / 111 cases in the separate build34
MSW logs. GUI runs use WX_UI_TESTS=0 on named private desktops, not user input.
The final combined scrollbar/StatusBar/native-resize/bridge/Ribbon/DataView-reorder
run passes **10852 assertions / 90 cases**, exit zero, in
`audit108-scroll-resize-runtime39-20261004.log`.

After the user confirmed closure, Showcase and DataView were rebuilt in the
existing static directory on 2026-10-04 at 15:10:44 and 15:10:51 local time:

- Showcase SHA-256: `92E40ACCC31D1F22D9C471BC589AA46F329B81CE8EAF262A922C2700EAC6DF63`.
- DataView SHA-256: `0CCDACD8E36726C7F07C2EF0A39CCF50FBBB84014CBE76EA9ADC29FABA903469`.

Minimal, Grid, Ribbon and Widgets were rebuilt at 15:10-15:11 in the same
directory. The accepted shared Showcase/core DLL hashes remain unchanged.
No user window was closed by automation and no physical input was injected.
Real-screen resize smoothness, scrollbar appearance/hover and HDR/DPI acceptance
remain pending: passing these tests is not visual sign-off or global port parity.

### 2026-10-04 native ListCtrl/EditableListBox and wxGrid automation

The next bounded component lot adds a source-private `listprojection` over the
existing wxGenericListCtrl model. Report/list/icon/small-icon views use actual
XAML ToggleButton/TextBlock/Image/CheckBox cells and Button headers. wx geometry,
virtual callbacks, selection, item data and the existing edit transaction remain
authoritative. Report/list row metrics have a 32-DIP minimum. Viewport realization
is bounded; unchanged native objects and image sources survive value updates.
Showcase now enables its real ListCtrl and EditableListBox sections instead of
the obsolete generic-cast placeholders. The standalone HeaderCtrl is not ported
by this work, nor is complete ListCtrl API/UIA selection parity claimed.

EditableListBox guards callbacks that destroy its owner/list while replacing
strings, inserting a terminal empty row, selecting New or deleting an entry.
Runtime tests exposed two distinct generic list editor/destruction bugs: recursive
DELETE_ALL_ITEMS notification, and a pushed editor handler left attached during
destruction inside validation. Cleanup now detaches exactly once. Ordinary generic
list destruction still emits its documented deletion notification; only recursive
notification during an active deletion is suppressed. A separate regression
preserves that ordinary notification contract.

wxGrid exposes actual Grid/Table/GridItem/TableItem providers with lazy logical
cell peers. A 100000-row x 2000-column table verifies GetItem without reading
off-screen values and stable identity when the native cell is realized. Tests
cover merged/frozen cells, headers, display order, hidden/disabled cells and
callbacks destroying/replacing the table during GetValue/GetAttr. Value,
Selection, formatted/custom accessibility names and external UIA clients remain
open. Non-contiguous merged spans after reorder are rejected, not represented
as a false rectangle. Native bool actions retain the existing edit/veto contract.

The tests also caught a real shared presentation bug: a child editor's propagated
wxEVT_DESTROY closed its parent's native viewport. Both ListCtrl and Grid now
retire a pane only for that exact window. The regression destroys a real editor,
checks retained/loaded native cells, and reopens/commits again.

Evidence, including intermediate failures, remains in the existing build tree:

- `audit108-list-grid-build45-20261004.log`: static Release /warnaserror PASS.
- `audit108-list-grid-runtime45-20261004.log`: **1587 assertions / 96 cases PASS**,
  including ListCtrl, EditableListBox, Grid, ListBook and filesystem composites.
- `audit108-list-grid-demos46-20261004.log`: all six static demos rebuilt with
  /warnaserror. MSW Debug build46 and runtime46 pass **229 assertions / 44 cases**.
- `audit108-list-grid-ctest46-20261004.log`: **9/9 CTests PASS**, 100.17 seconds,
  including toolkit runtime, metadata, opaque GDI, composites, DataView, Ribbon,
  outer scrollbars and the actual ListCtrl/Grid native gate registrations.
- Builds40/41 record fixed include/macro compilation errors. Runtime42/43 record
  the destruction failures, runtime44 the missing native cell; none is a PASS.

The expanded filesystem run took about 156 seconds. A read-only non-invasive
stack observation showed TreeCtrl rebuilding icon bitmaps while DirCtrl recreated
the hidden-file tree. It ultimately passed, but this is an open performance cost,
not a responsiveness qualification; no unrelated TreeCtrl rewrite was folded in.
The new `wx_winui_listctrl_native` CTest is also wired into the WinUI CI job.
No remote CI, physical input or user-desktop visual acceptance is claimed.

Static demos were rebuilt at 16:00 local time on 2026-10-04:

- Showcase: `385752E9F2A243BD98072104CBBF2239F30CDEE2C1FC98C1E6B82FB040ADFF82`.
- DataView: `E0CD2237457F0DD6CCB778DDDC987ACED851A7263CD79C82D31F75E065C9D090`.
- Grid: `B54CEBC3679D8C80B893F89D2A2E44504EF29EBBDAAF5C9630F789F4E08798AD`.
- Minimal: `ECD0655932B04BF4AF5571D34448831849765C45ACAA54907EA7DC868964F87F`.
- Widgets: `E7E209CEC63FD55806288CDF0DA38E7931B5162114A413842ED533C18008DEAB`.
- Ribbon: `BA2BCDAE0AF1B028277FCC1FEACDADF88ED1226F40BE337ADD3180EC88BBEB7E`.

The accepted shared Showcase and core DLL hashes remain exactly unchanged.
Mica/HDR and the accepted resize-grip input implementation are not changed by
this lot. Plans 108-112 and global production readiness remain open.

### 2026-10-04 tree image reuse and ListCtrl column-order contract

TreeCtrl now reuses identical resolved normal/state image pixels instead of
allocating a new native WriteableBitmap on each item projection. The per-control
LRU retains at most 64 entries and 4 MiB of key/native pixel buffers; live nodes
can retain sources after eviction, so this is not a total tree-memory bound.
All existing image-list and custom bitmap-bundle reads and revision checks still
run before lookup. Tests cover ImageList replacement, mutable bundles, alpha,
independent DIP dimensions, DPI round trips, bounded eviction and destruction
from a previously cached bundle callback. No process-wide cache or public test
API is added. The existing expansion test now expects identical image identity
for identical pixels while still requiring its three projection passes and a
distinct expanded image. Its transaction oracle is not replaced by the cache.

The generic ListCtrl's four previously stubbed column-order APIs are implemented.
One display-to-logical permutation drives both generic and native header/cell
geometry, logical hit-testing, checkbox/icon/label regions and an active label
editor. Insert/delete/reset preserve a valid permutation; malformed inputs are
rejected atomically. A pure reorder retains existing native element/provider
identities. The capability macro and public API documentation now describe this
programmatic support, not interactive drag-to-reorder input. The latter, complete
selection accessibility and the remaining component matrix stay open.

Validation exposed a separate real wxQt defect: programmatic selection used
QItemSelectionModel::Select even for a SingleSelection view. Selecting the
previous row then produced no new selection notification, breaking EditableListBox
Up/Down. The Qt adapter now uses ClearAndSelect for positive single selection;
deselection and multiple-selection behavior are unchanged. A portable regression
checks cardinality, old/new row states, buttons and unchanged model contents.

The filesystem suite was measured before and after the production changes on
the same private desktop runner, without physical input: **254 assertions / 7
cases pass in both runs**, with summed test durations **146.188 s before** and
**141.582 s after**. This single pair shows only a modest end-to-end difference;
the deterministic allocation tests prove reuse, not general responsiveness.
The large directory reconstruction and 10k-deep native selection costs remain
performance work. Logs are `audit108-tree-list-baseline47-20261004.log` and
`audit108-tree-list-filesystem47-20261004.log` in the existing static build tree.
The old executable's complete TreeCtrl model baseline also passes **391785
assertions / 23 cases** (`audit108-tree-model-baseline47-20261004.log`), including
78.946 s for the loaded 10k-deep selection section.

Static build47 and build49 pass /warnaserror. Final focused runtime49 passes
**2030 assertions / 11 cases**. Runtime47 failed only the native topology test:
its readiness predicate accepted the old relative order after column recreation
without checking new widths/content. Readiness now requires current model
dimensions/content/position; final assertions, 2-pixel tolerance and one-second
deadline are unchanged. Generic permutation tests run where wx/listctrl.h selects
the generic implementation; native MSW/Qt do not ship that class. The failed MSW
build47 and Qt build48 preserve the initial erroneous test-link assumption.
Subsequent MSW build48 and Qt build49 pass. MSW runtime48 passes **324 assertions /
58 cases**. Qt runtime49 preserves the five EditableListBox failures corrected
above; it is not a PASS.

The complete `[winui-treemodel]` suite is registered as `wx_winui_tree_native`
with the existing private desktop runner, WX_UI_TESTS=0, serial execution and a
240-second process deadline. It is included in the local WinUI CI workflow;
this is not evidence of a remote CI run. The accepted shared Showcase and core
DLL hashes remain unchanged. No user application was closed or input injected.

Final evidence for this lot:

- `audit108-tree-list-ctest49-20261004.log`: **10/10 PASS**, 187.20 s.
  The preserved `audit108-tree-list-ctest49-details-20261004.log` includes
  **392159 assertions / 27 TreeCtrl cases** and **2248 / 62 ListCtrl cases**.
- `audit108-tree-list-build50-demos-20261004.log`: /warnaserror PASS, test_gui
  and all six static demos. WinUI production is unchanged from build49; the
  extra portable selection regression is now included.
- `audit108-tree-list-ctest50-20261004.log`: actual ListCtrl gate rerun after
  this final test addition, **2260 assertions / 63 cases PASS**, 3.05 s.
- MSW and Qt `audit108-tree-list-build50-20261004.log`: /warnaserror PASS.
  Their final `audit108-tree-list-runtime50-20261004.log` files pass respectively
  **336 / 59** and **256 / 59**, including the unchanged Up/Down regression.
  These native MSW/Qt runs are compatibility checks, not proof of generic GDI
  ListCtrl rendering on GTK/Cocoa. That separate rendering coverage remains open.

Static demos were rebuilt on 2026-10-04 at 16:26 local time, without creating a
new build directory:

- Showcase: `BC6004CDD6EB25A0699F14A60163015649D3AA07EAAE9ECC84C067035909B8FD`.
- DataView: `843C0846A4C30B1FC1C5E3562B1DB67122B221F26271E9A4FF02D51C33EDAEED`.
- Grid: `6F30FC31964B4E7829AF8FA3B714A4B675B5E56CB192FB1D0134D5312751D397`.
- Minimal: `6E9500CC65718CD77561764995089C4EFD6EB9E68ED48C35521151D650319359`.
- Widgets: `79F18DB25D64BDCD931141C595CA770C8EA3F1B383E658D249BE4D609AE55EF1`.
- Ribbon: `3E82CF1C54E4742803A937B82F335949416F9B5D3D963D311B927CEC9C63A2EE`.

No global feature-complete, physical qualification or production-readiness
claim follows from this lot. Plans 108-112 remain open.

### Standalone HeaderCtrl native presentation, 2026-10-04

The standalone public HeaderCtrl no longer relies on GDI painting in WinUI.
Its existing column model and gesture/capture authority now publish retained
XAML Button/TextBlock/Image/FontIcon peers and native reorder markers. Hidden
columns, scrolling, display order and RTL use the same logical geometry; only
the visible width is realized. Width-only and sort changes retain peers, while
column replacement retires old Invoke providers. Bitmap bundles are resolved
again on refresh, including mutable implementations with unchanged identity;
identical pixels retain their ImageSource. Native Invoke sends the wx logical
column click without introducing implicit application sorting. The public
header does not include WinRT or expose test methods. Its additional storage
and overrides are guarded to the distinct WinUI toolkit ABI.

Showcase's standalone header is moved outside the equal-height list/tree grid:
it occupies a single row, permits column reordering and demonstrates sort
indicators on click. The accepted shared Showcase/core binaries are untouched.

Evidence in the existing build directories:

- Static `audit108-header-build54-20261004.log`: Release `/warnaserror` PASS.
- `audit108-header-runtime54-20261004.log`: **321609 assertions / 18 cases
  PASS**, comprising the existing twelve generic-model cases and six new native
  cases (the large assertion count mainly comes from the existing order stress
  test). Native checks include real loaded geometry, LTR/RTL, image pixels,
  retained identities, UIA Invoke and retirement, and semantic wx resize/reorder
  with veto and XAML markers. No physical input was injected.
- `audit108-header-ctest54-20261004.log`: **8/8 PASS**, 78.70 s, including
  HeaderCtrl, DataView, ListCtrl, Grid, composite, opaque-GDI, metadata and
  window-scrollbar gates. Full output is preserved in
  `audit108-header-ctest54-details-20261004.log`.
- MSW/Qt Debug `audit108-header-build54-20261004.log`: `/warnaserror` PASS.
  Their earlier runtime51 reference runs pass **321366/10** and **321371/12**
  respectively; the generic production files are unchanged since those runs.
- Public dispatch self-test: **36 headers plus negative fixtures PASS**.

The failed build51 (missing appearance include), build52 (signedness in new
test expectations) and runtime53 remain preserved. The runtime53 dump and
disassembly locate a use-after-free in the test functor: it accessed a reference
capture after synchronously deleting the window which owned the functor. Moving
the fixture bookkeeping before the synchronous deletion fixes the fixture;
the destruction and retirement assertions remain. No production lifetime guard
was removed or test expectation relaxed.

This does not establish physical gestures, external UIA/assistive-technology
behaviour or resize/reorder automation patterns. Shared/shipping qualification
of this new projection remains pending. The CI workflow now selects the bounded
native HeaderCtrl gate; a local workflow edit is not a remote CI result.

### Current user feedback: retained data layout and PropertyGrid

The user confirmed that the reported defects concern DataView. The bounded
implementation is live DataView geometry without resetting
unchanged contents. Column width and available viewport size must update during
layout, not only after an unrelated Refresh or pointer release. Expanding or
collapsing a branch must preserve unaffected row/cell identities instead of
resetting the full ListView source. Tests must count model reads and native
identities, not substitute arbitrary elapsed-time thresholds for this contract.

PropertyGrid remains explicitly unported visually. Its pending native work is
to retain wxPGProperty/wxPropertyGridPageState as model and editing authority,
project categories/labels/values/expanders as real XAML controls, retain its
native wx editors and validation/event contracts, and qualify resize, expansion,
selection, custom-renderer compatibility and accessibility. A recoloured GDI
property grid is not completion of that work.

### DataView retained geometry and branch updates, 2026-10-04

Implemented in the native DataView backend only; the accepted shared binaries,
Mica/HDR composition and host resize-grip paths are unchanged.

- Column width/minimum-width and viewport notifications update existing XAML
  cells in place, including content constraints, instead of invalidating all
  values and walking the model. Real wx size and XAML ListView size notifications
  publish geometry synchronously, without an application Refresh/Update.
- Standard cells, their handlers and an active native editor retain identity.
  Editor geometry follows the effective column width, including the existing
  last-column stretch rule. Candidate text is not committed/cancelled by a
  geometry-only change. Custom wxDC renderers still rerender when their drawing
  size changes; automatic-width columns retain necessary value measurement.
- Collapse removes the descendant row range without calling GetChildren;
  expansion enumerates the opened branch only. Observable-vector insert/remove
  notifications preserve unaffected rows/cells and their identities. Generation
  checks and a publication guard reject stale or intermediate snapshots.
  Genuine whole-model replacement/reordering retains the bulk publication path,
  avoiding one native notification per row for a large virtual model.

Four new regression cases use real native cells, header Transform resize,
disclosure Button Invoke and frame/sizer allocation. They count GetValue,
GetAttr, GetChildren and GetCount calls, retain native row/cell references, and
verify scroll position. The active-editor case additionally checks LTR/RTL,
retained TextBox identity/text, no editing-done/model-write during resizing,
and exactly one successful final commit. No physical mouse/keyboard injection
or arbitrary elapsed-time performance threshold is used.

Evidence in `F:\wxwinui-pr26890-feedback-build-static`:

- `audit108-dataview-layout-build57-20261004.log`: Release `/warnaserror` PASS.
- `audit108-dataview-layout-ctest57-20261004.log`: **7/7 PASS**, 24.95 s;
  runtime smoke, metadata, composite, DataView, HeaderCtrl, ListCtrl and Grid.
- `audit108-dataview-layout-ctest57-details-20261004.log`: full retained output;
  DataView **2071 assertions / 62 cases**, HeaderCtrl **321609/18**,
  ListCtrl **2260/63**, Grid **656/17**, composite **1901/24**, metadata **42/3**.

Earlier runtime55/CTest56 logs remain preserved. Runtime55 exposed an old
test expecting native cell replacement after row-height change; the test now
requires retained identity while still requiring the custom renderer to redraw.
CTest56 exposed an incorrect new editor-width oracle: the last column stretches
to the remaining viewport width. The corrected test verifies that public width
formula explicitly and compares native editor geometry to the effective width;
no production guard or lifetime assertion was removed.

These deterministic checks establish retained rendering/layout contracts, not a
measured physical-drag frame-rate budget or universal absence of visual lag.
Physical interaction qualification remains in 109/110, and PropertyGrid still
needs its own native presentation and editor migration.

`audit108-dataview-layout-demos57-20261004.log` rebuilds all six static demos
with `/warnaserror`, PASS. Final files in
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`:

| Executable | Local build time, 2026-10-04 | SHA-256 |
|---|---|---|
| showcase.exe | 17:12:02 | `8BCDE4733CF7C7CC955E4650D3353D824C5D0760B9CAEF9356B3D18788A2605C` |
| dataview.exe | 17:12:09 | `E97364B8880FD77B54F8978E217172ECA0BF644E12C6D3942CC53983916AF4BC` |
| grid.exe | 17:12:17 | `2C24EE389AECD9D4385F7CA34B8A44FD5DB0F8AF4F26F32F2BB993E020F058D1` |
| minimal.exe | 17:12:21 | `EFF9B559CE67784B88619E288C0D286683A64C1C2CDB3C80925C76A01C5BDDC7` |
| widgets.exe | 17:12:25 | `2DCA8832ADD9C141E478CE20E1FC9C5AC9D644A8CE6FE97FA552EAA1DC0F6AC9` |
| ribbondemo.exe | 17:12:31 | `F8372B57146D6C49B38108DE78F6F3721D6C786FBE65515DF6ADF75D90CA12DB` |

The accepted shared Showcase/core are independently rehashed and unchanged:
`7862B14BC6A1CF516F6EAB66DC021A72DF6A3C42C34D4DC68ACAA5F931F7AD22`
and `878BB481E0D81CD36B24DCAD06DACDE42F1ED06ACBAED48A113096334DD5174C`.
No user demo was closed, no physical input was injected, and no commit/push or
remote CI action was performed for this local implementation lot.

### 2026-10-04: crash evidence and native interaction follow-up (in progress)

The reported failures are not classified as user input errors. Matching local
WER dumps establish distinct causes:

- `showcase.exe.78628/83008.dmp` and `dialogs.exe.26412/57784.dmp`: stack
  overflow from `WM_NCMOUSEMOVE`, `HTBORDER`, synchronously reflected by
  Microsoft.UI.Input into the same root-pointer route. Packet/frame logs
  preserve HWND, host, pointer, coordinates and timestamps. `dialogs.exe` was
  still the September 1 executable, not a rebuilt October 4 sample.
- `ribbondemo.exe.86496/92856.dmp`: null `m_ribbon` in the sample's size event
  during Ribbon construction, before assignment of the member.
- `showcase.exe.59152.dmp`: the sample instantiated a bare `wxComboCtrl` without
  the required `wxComboPopup`; clicking it dereferenced the absent popup.

The pointer fix qualifies only the active synchronous forwarded packet and
host lifetime/transition epoch. Unchanged Move echoes may have reconstructed
timestamps; button/wheel packets retain strict timestamp identity. A new
nested event, including an A-to-B-to-A movement, still supersedes the old
route. This is not a blanket nested-input or modal-loop suppression.

`audit108-native-controls-build59-20261004.log`: static Release build PASS
with `/warnaserror`. `audit108-pointer-echo-runtime59-20261004.log`: 229
assertions / 9 cases PASS. `audit108-dialog-runtime59-20261004.log`: 2011
assertions / 22 cases PASS. These deterministic checks do not certify every
physical dialog interaction.

`audit108-crash-demos60-20261004.log` rebuilds static `ribbondemo`, `dialogs`
and `minimal` at 17:45. The first two survived an eight-second isolated startup
check without a new WER dump; the runner intentionally times out a continuing
GUI process (exit 124), so this is a startup check, not a complete runtime PASS.

The native PropertyGrid foundation, DataView insertion feedback, pending-grip
boundary correction and scrollbar backing-paint regression are under active
qualification. Build58 compile failures and runtime59 PropertyGrid/DataView
failures are retained in their logs; no complete/native-parity sign-off is
claimed for those new paths at this checkpoint. The accepted shared build is
not overwritten by this work.

### 2026-10-04: verified interaction fixes and native PropertyGrid foundation

The follow-up static Release build (`audit108-native-controls-build61-20261004.log`)
passes with `/warnaserror`. The defects above now have bounded production fixes
and regression coverage:

- A pending size-grip drag survives PointerExited while the same pointer still
  holds the left button. Release, cancellation, foreign capture and the final
  physical-contact check still cancel obsolete requests. Six LTR/RTL boundary
  variants cover held contact and release before the native resize wake.
- USER32 can paint a classic non-client scrollbar directly when SetScrollbar
  disables or re-enables it, before a deferred paint filter runs. The projected
  scrollbar now restores its own backing band synchronously, without sending
  application messages or repainting the client. Mica remains enabled. The
  pixel regression first reproduced eight failures in 420 assertions
  (`audit108-scrollbar-backing-red60-20261004.log`), including a positive control
  that deliberately invokes classic painting; it passes after the fix.
- DataView column dragging displays a real XAML insertion marker. Preview and
  final placement use one midpoint calculation, with identity/generation guards
  and retained rows/cells. LTR/RTL, cancellation and invalidation are covered.
- PropertyGrid standard categories, labels, values and expanders are now native
  XAML, with retained visible cells, native editors and a native checkbox path.
  wxPGProperty and the existing validation/event machinery remain authoritative.
  Property/page/owner retirement, validation veto, focus, viewport changes and
  custom-renderer buffer contracts have deterministic tests. This is a native
  foundation, not completed parity: advanced custom painting, remaining editor
  variants, full UIA and physical/shared qualification are still open.

Final evidence under `F:\wxwinui-pr26890-feedback-build-static`:

- `audit108-native-views-runtime61-20261004.log`: 3025 assertions / 57 cases PASS.
- `audit108-native-controls-ctest61-20261004.log` and its `-details-` companion:
  eight suites PASS (metadata, composite, DataView, window scrollbars,
  HeaderCtrl, ListCtrl, PropertyGrid and Grid). DataView: 2305 / 64;
  scrollbars: 794 / 8; PropertyGrid including its existing contract: 1385 / 9.
- `audit108-native-input-ctest61-20261004.log` and its `-details-` companion:
  the ninth suite PASS, 2973 assertions / 39 cases, covering root pointer
  routing, native resize and dialog contracts.
- `audit108-native-controls-header60-20261004.log`: header dispatch PASS.
  Ordinary MSW and Qt 5.15 Debug `/warnaserror` builds pass again in their
  `audit108-propgrid-portable61-20261004-build.log` files. Their earlier full
  PropertyGrid replays pass 1156 / 2 and 912 / 2 respectively (portable58 logs).

The input and PropertyGrid groups are registered in the local CTest/CI
configuration. This is not a new remote CI result. No physical pointer was
injected; smoothness and end-user grip/scrollbar behaviour still need physical
verification. Passing these tests does not imply that all dialog scenarios or
all components are production-qualified.

`audit108-native-controls-demos61-20261004.log` rebuilds all eight static demos
with `/warnaserror`, PASS. The final files, under
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`, supersede the earlier
17:12/17:45 static executables:

| Executable | Local build time, 2026-10-04 | SHA-256 |
|---|---|---|
| showcase.exe | 18:05:19 | `E0E974CD61EBDD8DADC3F46B68065A4CA56859636B28DF3253BD58D4FC7A066A` |
| dataview.exe | 18:05:23 | `AFDF381BBB844A4BC5298CD5322F6542608A7BEE9E9A77E1BA4B548C8D46DA3A` |
| grid.exe | 18:05:27 | `F26C40E4B5C3D76E770C5E59193A486C53D90B27F2B0EB65E372AD8C8DE15240` |
| minimal.exe | 18:05:30 | `DE767B130B548AAFA94AFB3100B9DC3E55640D40E49ACAA80B5A0FA5BC47D6A4` |
| widgets.exe | 18:05:34 | `0A850D782B76EF47F673EE31457C78EA941F20D8F9D324D6AEAFC85B33061CD9` |
| ribbondemo.exe | 18:05:40 | `188FA093AEFAB5152C8FA6485A8335DD6300CFEF799FD185BA7153B2CA47C8BA` |
| dialogs.exe | 18:05:44 | `62709124C51E67DDD6135A36F94737C39F5C8E770B8F61EF42620511800F224F` |
| propgriddemo.exe | 18:05:53 | `1A72A5891CFC577D912614A968E626E17AFE749F21A98749E81B739EF89666C1` |

Showcase, PropertyGrid, Ribbon and Dialogs each survive eight seconds on an
isolated desktop with zero new WER dumps. Their `audit108-<sample>-start61-20261004.log`
files record the intentional runner timeout (124), not an application exit or
a complete interaction test. The accepted shared Showcase/core hashes above
are reverified unchanged. No user demo was terminated and no physical input,
commit, push or remote CI action was performed.

### 2026-10-04: physical feedback reopens grip, backing and Ribbon layout

The user's subsequent physical test of the 18:05 static executables does not
confirm the earlier grip/scrollbar fixes as sufficient. Direct window captures
also show stale thin GDI PropertyGrid dividers behind native rows, a duplicated
selected value, and a completely absent Ribbon in both Showcase Advanced and
RibbonDemo. Successful process startup is not successful component rendering.

In `audit108-showcase-physical63-20261004.log`, the user's held grip gesture
is accepted and enters USER32 sizing at tick 88410500, then exits at 88413703
without following the pointer. This disproves a simple release-before-start
explanation for that gesture. The earlier automated gesture was too brief
and was correctly rejected by the contact guard; it is not a physical PASS.

Follow-up qualification targets the message translator inside the native
sizing loop, PropertyGrid backing/editor ownership, live scrolled-window
backing, and Ribbon default-size/sizer layout (including the Showcase parent
layout). These fixes and new tests are under compilation; physical success
and complete PropertyGrid/Ribbon parity remain unclaimed. Mica is retained.

Further bounded evidence: build66 passes, and the full input/scrollbar CTest
groups pass in `audit108-input-scrollbar-ctest66-20261004.log`. The live
Showcase input66 journal nevertheless leaves grip behaviour open: one native
move reaches USER32, while later accepted gestures still only show a XAML
release before exit. A new regression must demand WM_SIZING and an enlarged
rectangle before release, not just ENTER/EXIT-SIZEMOVE.

`audit108-scrollbar-nested67-20261004.log` passes 346 assertions with a real
Mica frame/splitter/notebook/scrolled-page hierarchy. This does not reproduce
the reported visible dark track and is not visual sign-off. The native track
and non-client backing need inspection in the actual demo.

PropertyGrid native choice validation/veto and retained-editor operations are
being extended. `audit108-pg-value67-20261004.log` separately exposes an open
accessibility issue: the editable native ComboBox automation Value provider
does not return the free-form text and SetValue throws. A template TextChanged
commit test covers the editor transaction only; it must not be substituted
for the still-unqualified automation Value contract. The failed log is retained.

### 2026-10-04: physical candidate69 feedback and corrected test scope

The user confirms that RibbonDemo69 now displays its Ribbon. The autosize
regression covers default-size creation, a nested static-box/scrolled parent,
and initial hidden creation; the zero-height bootstrap uses the loaded WinUI
TabView theme metric before the final measured command layout.

The same user's held grip now follows the pointer, but releasing it restores
the initial geometry. This is a failure, not grip sign-off: Release still
entered the cancellation path and sent WM_CANCELMODE. The next change uses
the current host/request's queued WM_LBUTTONUP for a commit, preserving
WM_CANCELMODE for actual cancellation. Private tests observe the returned
native message and stale-ticket rejection; synthetic MSG coordinates cannot
substitute for an end-to-end physical geometry test. The failed private
WM_SIZING expectations in logs68/69 are retained.

The earlier ComboBox Value-provider diagnosis was at the wrong automation
layer. The new test targets the exact editable TextBox using the external
UIAutomationCore client rather than the ComboBox WinRT provider. In
`audit108-pg-editors70-20261004.log`, 183 assertions in four cases pass, including
UIA value changes, veto then accepted commit and editor identity retention.
`audit108-propgrid-ctest70-20261004.log` also passes the full PropertyGrid group.
These functional results do not qualify its appearance.

The user's `20261004-1655-38.1973736.mp4` remains negative PropertyGrid visual
evidence: plain property text exposes rich formatting commands, native editor
chrome is clipped, and the row presentation retains the classic solid accent
selection. Corrections target native formatting policy, full-row editor
allocation and theme-driven XAML row presentation. Scrollbar visual acceptance
also remains open: the actual Showcase69 trace proves a transparent native
ScrollBar and a black-cleaned owner NC DC, not the final bridge composition.
Mica must remain enabled throughout this qualification.

Candidate73 is built and launched for the user's next physical check:
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib\showcase.exe`,
local timestamp `2026-10-04 19:16:24`, SHA-256
`0B6BA8DF514132BFB7581ADCC5A740B9E61FCAE2A9C9827AB0D01B2680576F00`.
The accepted shared demo is not replaced; RibbonDemo69 remains open unchanged.

Changes in this candidate:

- Plain/password TOM-backed text peers use native
  `DisabledFormattingAccelerators::All`, so their selection flyout cannot
  offer Bold/Italic/Underline. Rich text and the supported AUTO_URL upgrade
  retain native formatting. No text/clipboard flyout is globally disabled.
- PropertyGrid text editors consume the full native row allocation, without
  the legacy HWND baseline/margin shrink. The implicit frame is borderless;
  explicitly requested border styles remain supported.
- Standard cells retain native Button/template/UIA identities with attached
  theme resources, subtle selection and an accent indicator. Explicit wxPG
  palettes and custom renderers are not silently discarded.
- Grip release transports WM_LBUTTONUP through the current native sizing
  request; cancellation still transports WM_CANCELMODE. No cursor injection
  or custom geometry loop is introduced.

Build72/73 pass with warnings-as-errors (build71's new-test signed/unsigned
warning is corrected). `audit108-regression-ctest73-20261004.log` passes all
three relevant CTest groups: input (3252 assertions/42 cases), scrollbar
(1804/11), PropertyGrid (1836/19). The new indicator test waits for actual
native arrangement, not just synchronous Visibility, without forcing layout.
`audit108-richtext-compat73-20261004.log` passes 28 assertions/two rich-text
cases. Native PropertyGrid libraries also rebuild for MSW and Qt Debug in
their respective `audit108-propgrid-compat71-20261004.log` files.

The scrollbar bridge experiment passes 132 assertions in both directions:
the injected bridge sentinel is also visible through the owner DC, and owner
WM_NCPAINT removes it. This rules out independent stale bridge pixels in that
fixture, not all display/composition issues. SDK inspection also finds the
same DefaultScrollBarStyle in ScrollViewer and standalone bars. No speculative
scrollbar repaint or palette workaround is shipped. Actual-candidate trace
`audit108-showcase-input73-20261004.log` now reports the backing surfaces and
effective native track opacity/states. The dark-band report and final grip
geometry preservation remain pending human confirmation. These test passes
do not close plans108/109 or certify the entire port.

### Persistent PropertyGrid controls and reproduced native scrollbar backing (2026-10-04)

The user explicitly rejected a control appearing only after its row is selected:
this applies to all standard value cells, not just the Enabled boolean. The
retained viewport now presents real XAML TextBox, ComboBox, CheckBox and secondary
Button controls for the matching standard wx editors. They remain visible outside
selection, including disabled, unspecified and read-only values. Presentation is
non-interactive/Raw in UIA: the enclosing cell still activates the single real wx
editor, which exclusively owns input, conversion, validation and event delivery.
Its presentation is hidden during editing and restored with the same identity
afterwards. This does not create a second live editor or rebuild the property
model on each interaction. Custom editor identities/renderers are not guessed
from their names; their display contracts remain explicit.

Standard boolean Choice editors and Flags children now use actual WinUI ComboBox
peers too, retaining double-click cycling, aggregate updates and veto. Explicit
owner-drawn images/common-values retain their owner-draw callbacks. Tests exercise
both teardown paths and require exactly one editor destruction plus release of
all owned handlers. Native previews preserve explicit foreground/background,
masked password display, the CheckBox choice-index contract, and NoEditor values
beside secondary buttons.

`audit108-propgrid-ctest78-20261004.log` passes the complete local PropertyGrid
group: **2378 assertions / 28 cases**. Earlier build74/75/77 failures were confined
to overload/variable-shadow errors in new code and have been corrected with
warnings-as-errors still enabled. Run76 preserved the outdated TextBlock/OD
expectation failures; the replacement tests assert actual native preview
visibility, masking during live editing, and the native/OD lifetime contracts.
The MSW Debug core and PropertyGrid library build also passes in
`audit108-toolbook-propgrid-compat74-20261004.log`.

Actual user trace `audit108-showcase-input73-20261004.log` establishes a separate
scrollbar backing problem: the track's effective XAML opacity is zero, but
owner/bridge pixels become `0x171717` after previously being cleaned to zero.
The isolated native frame regression reproduces this in both LTR and RTL using
the same short vertical range (49 units, page46). In run78, WM_NCACTIVATE is the
first directly identified repaint source. The targeted handler keeps native
activation/results but uses the documented no-paint flag, then the filtered
nonclient paint path. See the [Windows WM_NCACTIVATE contract](https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate).
Run79 confirms ordinary activation/deactivation are clean, but exposes additional
native -1 activation and position-update paths. These failures remain retained
in `audit108-scrollbar-frame78-20261004.log` and
`audit108-scrollbar-frame79-20261004.log`; **scrollbar visual acceptance is still
open**, not inferred from the PropertyGrid test pass. Mica is not disabled.

Independent component work re-enables the real native Toolbook/FileCtrl/DirCtrl
demo paths which still had stale startup exclusions. Toolbook now resolves
duplicate tool IDs by identity and accepts native RadioButton selection UIA
without a duplicate Click command. Its layout test waits for actual XAML
arrangement after a resize. Run78 passes seven of the eight browser/toolbook
cases; DirCtrl's native selection transfer still exposes an intermediate empty
notification and its strict one-transaction oracle fails. This is an open
integration issue, not complete family parity. No global completion or physical
sign-off is claimed by this batch.

### Native scrollbar backing completion qualified in build81 (2026-10-04)

The remaining dark band is now reproduced independently of the XAML template.
The actual Showcase73 trace shows a transparent native track (effective opacity
zero) above owner/bridge backing pixels `0x171717`. Run80 then isolates the native
writers: direct `DefWindowProc(WM_NCACTIVATE, TRUE, -1)` and direct
`SetScrollInfo(SIF_POS, FALSE)` both replace clean backing with `0x171717` in LTR
and RTL. Projection ownership remains valid immediately before and after these
calls. Thus the documented no-paint flags alone are insufficient for this themed
USER32 scrollbar path; this is not a guessed ScrollViewer style difference or
an opaque XAML track. The failed controls remain in
`audit108-scrollbar-frame80-20261004.log`.

Build81 preserves native activation state/results and the existing popup-active
behaviour. It closes the native painting transaction synchronously on return:
ordinary activation uses the filtered nonclient paint path; an original `-1`
request cleans only the already projected bands without synthesizing WM_NCPAINT.
Position updates use the same bounded backing completion already used by range
and page updates, with weak-owner, HWND and incarnation checks after USER32.
Unprojected and minimized windows retain their native handling. No client
invalidation, style mutation, polling, forced palette, or disabled Mica is used.

`audit108-scrollbar-frame81-20261004.log` passes **588 assertions / one case**,
covering both directions. Each independent operation starts with verified clean
backing: full and band-only NC painting, activation/deactivation, suppressed
activation, hover/leave, position changes, actual ScrollWindow with a native
child, client Update, and settled native projection. The test also verifies
projection ownership and that activation does not change the
active or foreground HWND. The direct native repaint controls remain diagnostic
evidence; they are not silently changed to require different OS behaviour.

Build81 passes with warnings-as-errors. MSW and Qt Debug builds of wxcore and
wxpropgrid also pass (compat81). The three complete CTest groups pass in
`audit108-regression-ctest81-20261004.log`, **8013 assertions / 82 cases** in total:
input 3252/42 (27.80 s), scrollbar 2392/12 (39.86 s), PropertyGrid 2369/28
(68.23 s); total CTest duration 135.89 s. Diff whitespace checks pass.

Showcase81 is rebuilt with warnings-as-errors and opened for the user's check
(PID35992, responding when checked). Candidate:
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib\showcase.exe`,
local timestamp `2026-10-04 19:56:23`, SHA-256
`C332098D48FCDCA576F49C72EAF892FA500C9D35C823CE59B1A6D30EDE1E9225`.
Its existing diagnostic journal is `audit108-showcase-input81-20261004.log`.
The accepted shared build is unchanged; this is a separate static candidate.

**Human visual acceptance remains open.** These results establish the causal
backing regression and its automated correction, not the final appearance of a
newly linked Showcase on the user's desktop. They do not close the complete
component-port goal or supersede the outstanding physical grip qualification.

Human follow-up on Showcase81: ordinary scrolling is now described as globally
good, but the classic scrollbar can still flash during window resize **and
movement**. The same candidate trace records owner/bridge track pixels
`0x171717` with clean adjacent pixels during the geometry change. This is a
remaining native-backing regression, not accepted visual completion; the next
regression must include both size changes and movement at constant size.

### User regressions isolated in builds82–84 (2026-10-04)

The remaining work is not a cosmetic sign-off. The user also reports unreadable
Widgets navigation labels in light mode, missing FontDialog preview on HDR,
non-interactive RadioBox, a crashing custom ComboCtrl popup, stale Showcase
scroll extents at startup/CollapsiblePane changes, and CalendarView convergence
warnings during rapid scrolling. AUI tabs and FileCtrl still need genuine native
presentation; PropertyGrid optional editor coverage is incomplete. None of these
reports is closed merely because a nearby test is green.

Evidence retained in the existing static build directory:

- `audit108-scrollbar-resize82-20261004.log`: 12 failures / 116 assertions,
  sampling the live HWND band synchronously during wx/native resize in LTR/RTL.
  A committed projection was rejected because its cached screen rectangle had
  become stale. Build83 keeps committed paint ownership while re-reading the
  live native band; bridge clipping retains its stricter geometry contract.
  `audit108-scrollbar-regression83-20261004.log` passes 2508 assertions / 13
  cases, including movement, resize, activation and position updates. No Mica
  opt-out, client repaint, polling or forced palette was introduced. Physical
  visual acceptance of the new executable remains pending.
- `audit108-font-tree82-20261004.log`: 395 assertions / four cases pass for
  native FontDialog text/format and atomic native tree selection. Font preview
  now uses the WinUI StaticText/TextBlock peer instead of GDI, retaining chosen
  colour, font, underline, clipping and identity through resize. The automated
  backdrop marker test is not physical HDR proof.
- `audit108-new-controls82-20261004.log` and
  `audit108-spin-crash82-stack-20261004.log` retain the real PropertyGrid crash:
  a native NumberBox observer destroys its owner inside WM_PAINT, after which
  MSW's paint dispatcher accesses the destroyed window. Native publication is
  moved outside WM_PAINT with owner/page revalidation. Build83's Spin tests,
  including destruction, pass; the broader suite then catches the lost
  synchronous Refresh/Update contract. Build84 adds a guarded Update override;
  the strict existing software-buffer regression is not removed.
- RadioBox's containing Border incorrectly disabled hit testing for all native
  children. The native hit-test/provider test then exposed premature focus
  queries before Create had initialized its item state. Both are corrected;
  build83's input group passes 3476 assertions / 46 cases.
- `audit108-combo-popup83-20261004.log` passes 34 assertions for the real custom
  popup/button/Dismiss/reopen scenario. Build83b additionally passes HWND and
  host-root pointer paths. The user's crash is **not reproduced or declared
  fixed**; no speculative ComboCtrl product workaround was applied.
- `audit108-regression83-20261004.log`: Toolbook passes 548 assertions / four
  cases; FileCtrl/DirCtrl contracts pass 445 / six. These semantic tests do not
  validate the poor FileCtrl visual layout. Both existing CTest groups are now
  included in the CI component job; no remote CI execution is claimed.
- `audit108-discriminating83b-20261004.log`: real TreeView labels fail on the
  dark-to-light transition in both LTR and RTL, while initial dark and explicit
  item colours work. The pending fix removes the unconditional missing-value
  Foreground binding and lets the native container inherit theme colours.
- Showcase startup scrolling passes after the fixture waits for the original
  requested frame allocation, without issuing a resize. The collapse-back
  extent assertion still fails and remains under investigation; it is not
  masked by relaxing the extent or scrollbar oracle.

The CalendarView change waits for real native viewport completion rather than
spending the finite error budget on intermediate animation frames. It includes
an actual animated ChangeView/retarget/destruction regression and keeps the
existing injected-failure budget assertions. Runtime qualification is pending.
MSW and Qt Debug wxcore/wxpropgrid compatibility builds84 pass. Optional native
PropertyGrid DatePicker composition, FileCtrl native item containers, Fluent
AUI tabs, the physical interactions and the long soak remain open.

#### Build84 qualification and candidate handoff

`audit108-regression84-20261004.log` passes all three full CTest groups:
input **3548 assertions / 46 cases**, tree **392454 / 30**, PropertyGrid
**2512 / 31**, total192.86 seconds. This includes the strict synchronous
Refresh/Update custom-renderer case which failed83 and the destructive native
Spin preview callback. The new tree theme case is green after its discriminating
red83b result, without replacing realized labels or overriding explicit colours.

`audit108-layout-range84b-safe-20261004.log` passes **2375 assertions / 50 cases**
for Showcase layout and all non-physical range/date cases, including CalendarView
animation and bounded genuine-failure recovery. The collapse-back extent was
already exactly correct (624x321). MSW reports range1/page0 for a removed
scrollbar (`nMin == nMax == 0`), so its regression now checks the real WS_VSCROLL
state and the native peer's absence/unloaded/collapsed state, retaining exact
frame/page/extent checks. No production layout workaround was added for this
sentinel. Initial publication and expansion use the sample's explicit
content-driven scrolled-page policy.

An initial overly broad range filter selected its hidden physical-keyboard case
and attempted one simulated arrow key. That run was stopped and retained as
`audit108-layout-range84b-20261004.log`, **not** accepted as qualification.
The corrected run explicitly excludes `[.]` and `[physical]`, sets WX_UI_TESTS=0,
and the CI range/date filter uses those exclusions. The physical case additionally
now requires explicit WX_UI_TESTS=1 before constructing any UI/input simulator.
This safety guard is source-only until the next test executable build; no claim
is made that the interrupted physical test passed.

`audit108-candidates84b-20261004.log` successfully links the two static candidates
with warnings-as-errors against the qualified84 libraries (project references
disabled deliberately, keeping the unfinished85 library changes out):

- `F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib\showcase.exe`,
  2026-10-04 20:39:20, SHA256
  `CF0AB4650C0D6202E70694E97EBA386ED793D7A7899E7A2BC6F5CD990D513A57`.
- `F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib\widgets.exe`,
  2026-10-04 20:39:22, SHA256
  `AD2DAF78D0630E18E3F99159624BA00DFE35525AAA684ADE671548CCA3E59883`.

The accepted shared build remains untouched. These executables have not received
the user's physical visual sign-off. FileCtrl true item containers, native AUI
TabView and optional DatePicker previews are separate in-progress85 work, not
features claimed by this candidate. The source worktree is newer than these
qualified libraries: rebuild85 and qualify its new contracts before any release.

#### Native item containers, AUI headers and date previews (build85)

This increment replaces the non-report ListCtrl/FileCtrl item buttons with real
ListView/GridView item containers. The wx model still owns item identity, item
geometry, scrolling, selection and the label editor. The bounded native slice's
SelectionItem commands act on the complete wx selection, including offscreen
rows. Report-mode cells are not covered by this replacement.

AUI now has a source-private TabView header projection per tab pane; pages remain
owned by the notebook and its docking manager. Selection and close requests enter
the existing veto-capable transactions. The previous AUI contract tests are
retained, with a separate native test TU. Multiline and custom-art presentation
remain explicitly unfinished native cases. OnPaint does not publish XAML: a
destructive Header-property callback is tested through the explicit Update path.

PropertyGrid dropdown date previews are real CalendarDatePicker controls with
the exact wx display string and shared civil-date conversion. Native preview
controls do not acquire editing/model authority. The three date-preview cases
pass, including retemplating, unspecified values and property/page/owner
retirement. Permanent wxDP_SPIN date presentation is still open.

Local evidence at this integration point:

- `audit108-native-libraries85-20261004.log` records the initial composable
  C++/WinRT conversion compile failure; the corrected libraries pass in
  `audit108-native-libraries85b-20261004.log`.
- `audit108-native-tests-build85-20261004.log` builds test_gui with
  warnings-as-errors. AUI also builds on both existing MSW and Qt Debug builds
  (`audit108-aui-compat85-20261004.log` in each build directory).
- `audit108-new-native85-20261004.log`: 10/14 cases pass, 889/895 assertions.
  Failures identify native selection-anchor/viewport work, a FileCtrl oracle
  incorrectly assuming filtering clears its text field, and an intra-pane test
  entering drag-end without the preceding real drag-motion transaction.
- `audit108-aui-pg-date-regression85-20261004.log`: 86/87 cases pass,
  4833/4834 assertions. The retained AUI overflow test detects GetTabOffset still
  reporting zero for the new native strip; this is not accepted as qualification.

All these runs exclude physical test tags and/or set WX_UI_TESTS=0 and use the
private-desktop runner. Source corrections to the remaining failures require a
new build and rerun. The build84 Showcase/Widgets hashes remain unchanged; no
build85 candidate or remote CI pass is claimed at this point.

#### Native rows, AUI viewport intent and SPIN dates (build86)

The remaining85 selection/retirement defects are corrected. Native selection
updates the wx anchor before subsequent Ctrl/Shift operations. Retiring a native
item finishes removal from the exact retained Items collection even when its
IsEnabled callback invalidates the wx snapshot. The regression checks actual
visual-tree/collection absence and inert providers, not the asynchronously cached
IsLoaded flag or an assumed fixed number of framework callbacks.

Report-mode ListCtrl now also publishes actual ListViewItem rows. Cells are
retained native ContentControl/Grid/TextBlock/Image children of those rows,
not individual ToggleButtons. CheckBox children and the native SelectionItem
adapter retain wx selection, logical column order, viewport geometry, virtual
data and label-editor authority. Tests retain the four view modes and add real
cell-to-row ancestry, report selection and a 100,000-row virtual viewport.
Grid/Table accessibility patterns and physical interaction remain open.

AUI keeps the stock TabView/virtualized header strip. MakeTabVisible maps real
connected XAML geometry without waiting for the deferred Loaded notification.
Its explicit viewport intent redirects TabView's automatic selected-header
bring-into-view while virtualized widths settle. Continuations are bounded and
qualified by request serial, peer and page membership; completion, new input,
selection, focus, retirement or a newer request revokes that intent. The old
synchronous overflow assertions and new stable-scroll, direct native scroll,
last-to-first supersession and reentrant layout/retirement cases are retained.
No GDI tab widths or fake tab offset are used to satisfy the tests.

PropertyGrid SPIN date previews are native NumberBox calendar fields (plus era
and clear controls where applicable), sharing the interactive DatePicker's
source-private civil/locale helpers. An arbitrary wx date format is represented
by one passive NumberBox preserving the exact string, not a parsed imitation
of three fields or a TextBlock overlay. Selection still creates the one wx
interactive editor. This is a presentation distinction, not a claim that an
arbitrary formatted string becomes an editable date field by itself.

Integration evidence and rejected experiments:

- The non-report visibility/retirement test passes in
  `audit108-visibility-diagnostic85e-20261004.log`; the AUI visibility failures
  in85c through85n were not accepted as qualification.
- A trial replacing the live native ItemsPanel by StackPanel was rejected:
  `audit108-visibility-diagnostic85j-20261004.log` and
  `audit108-aui-crash85j-20261004.log` record the runtime container-generator
  crash on the retained reparent test. That replacement is absent from source.
- `audit108-aui-regression85p-20261004.log`: all **1,367 assertions / 13 cases**
  pass, including stable viewport and a subsequent independent native scroll.
- `audit108-aui-compat86-20261004.log` in each existing wxMSW and wxQt Debug
  build confirms wxaui compilation with warnings-as-errors.
- `audit108-native-build86-20261004.log` records two PropertyGrid compile
  defects (FrameworkElement type and shadowed date local); both are corrected.
  `audit108-native-build86b-20261004.log` builds the complete static test_gui
  dependency graph with warnings-as-errors, including current core/AUI/PG.
- The integrated86 AUI CTest passes **1,423 assertions / 14 cases**, adding
  nested layout request and page retirement. File browsers pass **513 / 7**
  and ListCtrl passes **2,948 / 68** in
  `audit108-native-regression86-20261004.log`.
- The same integration run found six PropertyGrid height assertions: subtracting
  the legacy GDI grid-line pixel left native NumberBox content only 31 DIPs
  high. The native preview now receives the full existing row height, without
  increasing row pitch or weakening the 32-DIP/containment assertions.
  `audit108-native-build86c-20261004.log` rebuilds the complete test graph;
  `audit108-propgrid-regression86c-20261004.log` passes **2,807 / 37**.
- The date/calendar/layout suite is **not qualified** by those successes.
  Both the parallel run and the serial 50-case run fail the calendar navigation
  coalescing assertion (11 refreshes versus a bound of 10). The exact case
  alone passes 61 assertions, which does not erase either full-suite failure.
  Logs: `audit108-date-layout-regression86c-20261004.log`,
  `audit108-date-layout-serial86c-20261004.log`, and
  `audit108-calendar-coalescing86c-20261004.log`. Build86d adds trace-only
  distinction of revoked snapshots versus unresolved native month topology;
  no retry limit or oracle is relaxed.

Build84 Showcase/Widgets hashes were rechecked unchanged at 21:41 local time.
No shared candidate, remote CI or physical sign-off is inferred from this lot.

Build86d trace confirms four scans of the same unresolved native month topology
(identical day revision213, 63 visible snapshot days, viewport not intermediate)
consume the entire automatic error budget. This is distinct from the subsequent
injected revoked-snapshot test, which must retain its four attempts/three retries.
The calendar fix separates waiting for real native topology from error recovery;
its new stationary-layout/resumption/destruction test awaits the next build.

The unchanged86d binary also runs the broader component gates in
`audit108-other-native-regression86d-20261004.log`:

- Input/dialog contracts: **3,548 assertions / 46 cases** pass.
- DataView: **2,305 / 64** pass; native Ribbon buttonbar: **352 / 5** pass.
- Window scrollbars: **2,540 / 15** pass; Grid: **656 / 17** pass.
- Toolkit Ribbon: **59 / 60 cases**, **5,325 / 5,327 assertions** pass. The
  remaining failure is the replacement-page section-scroll endpoint in both
  LTR and RTL, not a global Ribbon success. Diagnostics for that exact native
  viewport are added without relaxing the endpoint or timeout assertion.

CMake's nineteen generated private-desktop GUI gates now explicitly exclude
hidden and physical tags in **each** comma alternative and set WX_UI_TESTS=0;
the range/date gate also includes Showcase initial/collapse layout. These were
checked against CTest's generated JSON inventory after configure86e. Dedicated
physical qualification is not removed or claimed by those automated gates.

Both existing MSW and Qt Debug full test_gui graphs build with warnings-as-errors
(`audit108-component-compat86e-20261004.log` in each build tree). MSW's portable
ListCtrl/PropertyGrid runtime selection passes **1,398 / 45**. The first Qt
runtime launch exits with missing-DLL status before test output because its
Qt bin directory was absent from PATH; the corrected child-environment launch
is recorded separately as runtime86f, not silently relabelled as the first run.
That corrected Qt run passes **1,051 assertions / 45 cases** in
`audit108-list-pg-runtime86f-20261004.log`; MSW's corresponding transcript is
`audit108-list-pg-runtime86e-20261004.log`.

#### Calendar native-layout resumption and report UIA integration (build87)

`audit108-native-build87-20261004.log` builds the complete static graph with
warnings-as-errors. Native month-topology waiting now compares real
dates/rectangles/identities on layout/viewport events; identical incomplete
states do not self-queue or consume the revoked-ticket error budget. Completing
the native month resumes the original request. No day peers are retained in
the waiting signature. The added real-island test collapses a realized day,
observes six stationary layout echoes, then restores the same day or destroys
the waiting calendar. The original <=6 navigation bound and four-attempt
revoked-ticket recovery assertions remain unchanged.

`audit108-calendar-regression87-20261004.log` passes the entire non-physical
date/calendar/Showcase-layout selection: **2,547 assertions / 51 cases**.
This closes the reproduced coalescence failure locally, not physical rapid
wheel/HDR/theme qualification or exhaustive calendar parity.

ListCtrl report now has native Grid/Table and logical GridItem/TableItem
providers, with the realized ContentControl returning the same peer as GetItem.
Rows remain native ListViewItem selection peers; headers retain Button peer
focus semantics. Virtual cells are metadata-only until explicitly realized,
and structure changes revoke retained providers. Four new tests cover this
contract, including 100,000 rows and destructive virtual getters. The first
full run, `audit108-list-regression87-20261004.log`, passes **71/72 cases** and
fails two header focus assertions; it is not accepted as completed UIA support.

The Ribbon replacement-page diagnostic reports stable geometry (viewport584,
extent924, limit340) but no ViewChanged event and offset0 after the accepted
request. `audit108-ribbon-scroll-trace87b-20261004.log` retains the failing
LTR/RTL assertions. The preceding87 filter-by-name attempt matched zero tests
and is explicitly not qualification.

#### Native header focus qualification and Ribbon differential (build89)

Build88 established native header focus but exposed two further defects: the
test requested Next/Previous navigation with a subtree instead of the island's
XamlRoot.Content, and focus bookkeeping during destruction cast through an
already retiring list main pane. The test now uses the actual XAML root while
retaining the next-header destination assertion. The WinUI header's
AcceptsFocus checks its live parent and retirement state, without casting
through the main pane. Other ports retain their existing focus contract.

`audit108-native-build89-20261004.log` builds the entire static test graph with
warnings-as-errors. `audit108-list-regression89-20261004.log` passes **3,243
assertions / 72 cases**, including report Grid/Table identity, virtual cells,
native header focus/Tab/Invoke and destruction. MSW and Qt core compatibility
builds also pass (`audit108-header-compat89-20261004.log` in their existing
build directories). Global offscreen UIA Selection, Value, physical input and
external accessibility-client qualification remain open.

Ribbon build88 proves the independent initial oversized-group case passes,
while replacing a page still fails. Build89 tests all eight combinations of
LTR/RTL, invoking/not invoking the retired provider, and waiting/not waiting
for a native rendering frame. All eight retain offset0 despite a nonzero
scrollable extent; frames continue during the wait. The original strict
endpoint assertion is unchanged. Thus neither the old provider nor a missing
first frame explains this defect. Logs `audit108-ribbon-scroll-trace88-20261004.log`
and `audit108-ribbon-scroll-trace89-20261004.log` are diagnostic failures, not
Ribbon qualification. No product retry or enlarged timeout is added.

The unchanged89 binary also passes the composite host/lifetime gate
(**1,901 assertions / 24 cases**) and native popup/flyout-retirement gate
(**1,098 / 13**) in `audit108-composite-regression89-20261004.log`. These use
the private desktop without physical input and do not replace external UIA
or interactive human qualification.

#### Report Value editing, retirement and sample delivery (build93)

Report cells now expose native UIA Value. Only editable logical column zero
accepts writes, through the existing wx BEGIN/END label-edit transaction and
its native editor, not a second model setter. Read-only/disabled cells, an
existing draft, vetoes, virtual model callbacks and destruction are covered.
The generic editor revalidates its owner/editor/item after application callbacks;
append-after-edited-item remains legal for EditableListBox. Public EndEditLabel
still reports whether an editor existed, independently of END veto.

Build92 found a real virtual-list presentation defect: the application model
committed but its realized cell retained the old label because generic dirty
painting was no longer the native presentation authority. Build93 refreshes
only the committed row after revalidation. `audit108-list-regression93-20261004.log`
passes **3,554 assertions / 79 cases**, including the unchanged visual commit
oracle and bounded virtual-model reads. Build93 initially failed a shadowed
diagnostic local under warnings-as-errors; `audit108-native-build93b-20261004.log`
is the successful build. MSW and Qt full Debug test graphs compile with
warnings-as-errors (`audit108-label-compat93-20261004.log`). Their native list
implementations do not execute the generic-only editor tests; these passed
on WinUI. Global offscreen UIA Selection and external/physical qualification
are still open.

The experimental Ribbon presenter SetOffset change did not fix the failing
replacement-page case in build92 and was removed. Build93 uses ChangeView.
`audit108-ribbon-scroll93-20261004.log` proves the new viewport is in the current
host XamlRoot.Content, with reciprocal parent/child relationships. One explicit
diagnostic Arrange/UpdateLayout after the original failure does not restore
scrolling. The initial oversized-group case passes, replacement fails in LTR
and RTL (**278/280 assertions, 1/2 cases**); the strict original verdict remains.
This is an unresolved native retirement/scrolling defect, not accepted parity.

`audit108-demos-build93-20261004.log` builds all seven static demos with
warnings-as-errors in the existing build directory. Executables were verified
at 22:52:49–22:53:15 on 2026-10-04 under
`F:\wxwinui-pr26890-feedback-build-static\lib\vc_x64_lib`:

| Demo | SHA256 |
|---|---|
| showcase.exe | B9DE0F1F181B92A6BD4B3B38F2E776E53EFE001AB7B58221EF9B81483B1C2746 |
| widgets.exe | 313F44E5F4DE2C36AF26E4BA0CC119D46A5DB9E30311DDDEDE1B385477B7E1F3 |
| dataview.exe | 9F2574A08A3B845E3C9D386D98A6CF406AEF6A211B7862848A4F8D6AC1182A8E |
| grid.exe | B1676B3E9D5722129C72F678EE7B9361CA072964777F148B95D05FA832273CB9 |
| dialogs.exe | 8D58ED2E3F093E66C1F049AD69495E82DE6260F53366F484F212C659E456FEB6 |
| ribbondemo.exe | 326A87E38C005A6B6C77508E036739AFC5DB4474C44472041FCEFD9DDE22869E |
| minimal.exe | 9C132CCA7AEB81A4A562E1868CDB77C66A846895442933144D73D6ACABB1D833 |

The shared Showcase built at 00:53 is untouched. These are rebuilt development
candidates, not complete-port or physical user acceptance evidence.

The same production source set passes all ten selected native component gates
in `audit108-demo-regression94-20261004.log` (310.39 seconds, private desktop,
physical/hidden cases excluded on every filter alternative):

| Gate | Assertions / cases |
|---|---|
| DataView | 2305 / 64 |
| Ribbon native buttonbar | 352 / 5 |
| Window scrollbars / Showcase layout | 2540 / 15 |
| HeaderCtrl | 321609 / 18 |
| AUI | 1423 / 14 |
| Toolbook | 548 / 4 |
| File browsers | 513 / 7 |
| TreeView including live theme changes | 392454 / 30 |
| PropertyGrid | 2814 / 37 |
| Grid | 656 / 17 |

Ribbon Toolkit is deliberately recorded separately, not included in the above
ten PASS gates. `audit108-ribbon-scroll94-20261004.log` fails the replacement
endpoint whether the previous viewport is removed immediately or after its
actual final ViewChanged and nonzero native offset. All four LTR/RTL variants
fail (490/494 assertions, 1/2 cases); pending retirement alone is not the cause.

`audit108-input-date94-20261004.log` passes the date/calendar/layout gate
(2538 assertions / 51 cases), but the combined input/dialog gate is **not
accepted**: 45/46 cases and 3546/3548 assertions pass. The queued grip movement
test observes one WM_SIZING after release where it expected none. All message
transport/capture assertions still pass. The unchanged binary passes both the
isolated motion group (111/2, `audit108-resize-motion94b-20261004.log`) and the
whole resize group (1012/11, `audit108-resize94c-20261004.log`). These reruns do
not erase the combined-suite failure. A passive test observer now records the
rectangle and already-delivered move/release counts at WM_SIZING to determine
whether this is late geometry or a native final-sizing notification; product
code and the failing assertion are unchanged pending that evidence.

#### Report global Selection and follow-up diagnostics (build95)

The report grid peer now implements Selection from the wx selection model,
not the native viewport's SelectedItems subset. Virtual enumeration uses
wxSelectionStore without reading labels, changing focus or realizing rows.
Weak canonical row peers preserve SelectionContainer identity across viewport
changes; select/add/remove use the existing wx state and event authority.
Coalesced notifications synchronize only realized selection and raise UIA
SelectionInvalidated without rebuilding rows. A real UIAutomationCore client
subscribed from an MTA observes the off-screen selection notification.

`audit108-list-regression95-20261004.log` passes **5,739 assertions / 83 cases**
on the private desktop, including sparse 100,000-row and 2,048-selection cases,
single selection, retirement and the client event. This is in-process MTA
coverage, not the separate external-client or physical-input qualification.
The terminal static build is `audit108-native-build95d-20261004.log`; earlier
95 attempts caught an explicit smart-pointer return, a final COM test handler
incompatible with C++/WinRT and a call through an unavailable inline symbol.
They were corrected without relaxing warnings or exporting another API.

`audit108-ribbon-scroll95-20261004.log` retains the strict failure: **492/496
assertions, 1/2 cases**, with all four LTR/RTL and keep/delete-old-page variants
failing to scroll the dynamically added page. Keeping the old page alive and
adding a distinct page ID also fails, so deletion/retirement is not necessary
to reproduce the defect. The initial oversized page still passes.

`audit108-input95-20261004.log` passes the unchanged input/dialog gate
(**3,548 assertions / 46 cases**). This clean run does not erase the intermittent
build94 sizing failure; its passive rectangle/message diagnostics remain.
The seven delivered demos remain build93 and are not replaced by these tests.

The bounded repeat in `audit108-input95-repeat-20261004.log` passes three more
complete input gates (3,548 assertions / 46 cases each, 95.46 seconds total).
No failing rectangle was captured, so the earlier intermittent issue remains
unexplained rather than being declared fixed by repetition.

#### Grid native Value and destructive editor lifecycle (build96/97, open)

Native Value has been implemented for the exact standard text, number, float
and auto-wrap editors. It opens the actual wx native editor and uses the
existing selection, SHOWN/CREATED/HIDDEN, validation, CHANGING/CHANGED and veto
transaction. No parallel model setter or hidden editor stands in for it.
Choice/date/custom Value and Grid Selection remain open. Tests include logical
coordinates, readonly/disabled/veto, real editor identity and destructive
application/model callbacks. These additions are not yet qualified.

Build96 compiles the full static WinUI and MSW/Qt Debug test graphs with
warnings-as-errors. Its destructive tests exposed a genuine lifecycle defect:
an EDITOR_CREATED callback deleting the Grid leaves a retained editor's event
handler attached to the child window being destroyed. The normal rerun
`audit108-grid-edit96b-20261004.log` fails with `0xC0000409`; WER dump
`test_gui.exe.95408.dmp` and `audit108-grid-edit96b-dump-20261004.log` show the
wxWindowBase destructor assertion through wxTextCtrl/Grid destruction.
The first MSW96 run timed out and the first WinUI96 live debugger attempt did
not produce a usable stack; neither is accepted as a passing run.

Grid destruction now explicitly destroys the active editor before releasing
it and destroying child windows. Repositioning retains the active editor and
weak window identities, and OnSize checks its owner before continuing.
`audit108-grid-edit97-20261004.log` on MSW proves the veto and all five editor
callback boundaries pass, but the new reposition/delete-grid case still fails
(36/37 assertions, 2/3 cases). Its WER dump `test_gui.exe.67952.dmp` and
`audit108-grid-edit97-dump-20261004.log` locate the remaining use-after-free in
wxScrollHelperEvtHandler::ProcessEvent calling HandleOnSize after the owner
was destroyed by the preceding size handler. This is kept as a failing test,
not bypassed by calling the Grid handler directly.
