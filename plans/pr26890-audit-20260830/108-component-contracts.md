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

### Next bounded component lot: ToolBar rows

The audit's `SetRows()` gap is confirmed: WinUI inherits the common no-op and
does not consult `GetMaxRows()`/`GetMaxCols()`. Address it through the retained
native Grid, not a state-only setter. One coherent lot must cover transactional
`SetRows()`/`Realize()`, row-aware overflow and best size, per-row RTL placement,
embedded-control rectangles, and rollback of Grid definitions/coordinates.
Retain peer identity, event ordering and reentrant destruction guards. Tests
must cover one-to-many-to-one rows, vertical/RTL layout, shrinking/expanding
overflow, embedded controls and failed/reentrant rebuilds. This lot is not yet
implemented or qualified.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
