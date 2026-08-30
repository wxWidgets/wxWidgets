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
- TreeCtrl selection/expansion and two DatePicker/composite focus cases fail in their larger suites but pass in an isolated shared run. Diagnose order/lifetime effects; do not attribute them automatically to the private desktop.
- TextCtrl's read-only peer mutation case reads a RichEditBox immediately after a real TOM write, before the asynchronous TextChanged callback restores the model value. Check the same bounded native-event wait used by adjacent tests, retaining model/peer/event assertions. No production repair is yet demonstrated.
- The restored shared range run adds a Calendar recycling/coalescing budget failure: `GetWeekRefreshRunCount() <= beforeNavigation + 6` reports **11 <= 10** at `winuirangedate.cpp:3693`. This was not in the initial baseline; classify its cause before attributing it to intermittence or the API relocation. The unchanged limit remains enforced. Log: `audit106-family5-restored-range.log` in the shared build tree.

Baseline logs and exact counts are recorded in plan 106. The isolated focus run is `audit106-family5-focus-isolated.log` with its passive input trace in the shared build tree. All use `WX_UI_TESTS=0`; none supplies physical input evidence.

## Read-only text and Calendar observation corrections

- TextCtrl's unchanged production RichEditBox rolls back a read-only peer write through its asynchronous TextChanged callback. The fixture now waits for that real callback, retaining the immediate model check and the original final peer checks. Four added assertions require bounded completion, the unchanged model value, read-only state and no wx text event. No synthetic notification or production setter substitutes for the callback.
- Calendar's `GetWeekNumber()` was not a passive observer: it forced rendering and called `RefreshWeekNumbers()`. Calling it repeatedly inside the reverse-navigation wait manufactured the refresh requests whose count the case was measuring. A non-installed, test-only `ReadProjectedWeekNumber()` now reads already projected rows after validating the live owner/peer; the existing active getter keeps its rendering/refresh/lifetime checks and delegates only its final lookup. The measurement uses the passive getter and seven added assertions check observation purity and capture navigation diagnostics. The original `beforeNavigation + 6` limit is unchanged.
- Shared and static Release `/warnaserror` builds pass. Each full text group passes **859 assertions / 28 cases**; each full 70-case range group passes **3007/3009 assertions, 68/70 cases**, with only the two previously recorded DatePicker focus failures. Calendar coalescing now passes within both broader groups, not only in isolation. Logs: `audit108-text-calendar-{build,text,range-runtime}.log` shared; `audit108-text-calendar-{build,text,range}.log` static. Smoke/Supported V0 also pass 2/2 in each linkage (`audit108-text-calendar-ctest.log`); a passing observation fix does not close the remaining component work. The fixes are published in `c0f1d55a06` and require their own remote CI confirmation.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
