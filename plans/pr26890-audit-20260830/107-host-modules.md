# Plan 107: Split host responsibilities behind private ownership boundaries

- Status: IN PROGRESS — 107B LOCAL PASS ON/OFF/install/ON; 107A native and first 107F boundary PASS shared/static ON; remaining modules pending
- Planned at: f6ae07e1ade037e570b99df5369edd070b728a48, refreshed 2026-08-31
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 102, 104, 106 characterization coverage

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat f6ae07e1ad..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes, especially the ControlHost test-support extraction in 106, instead of overwriting them. Read each target before editing.

wxWinUITopLevelHost owns slots, lifetime, geometry, input, capture, focus, keyboard, UIA, tooltips, OLE, appearance and transients. The audits identify coupled authorities, not merely a line-count target.

This refresh incorporates the user's request for the architectural foundation
needed to continue component work. It is not an additional general audit, a
promise of universal parity, or authorization to rewrite the rendering model.
ADR 0003 keeps one XAML island per TLW and coalesced geometry publication; an
arbitrary `XAML A > HWND N > XAML B` interleaving needs a separately approved
composition design/prototype. ADR 0004 keeps toolkit-neutral public wx names
and separate toolkit-selected ABIs. No new public backend namespace/factory
is introduced by this plan.

Concrete boundaries verified in the current source:

- `src/winui/tlwhost.cpp`, `DispatchNativeResize()`: a wake encountered while
  `gs_winuiOperationDepth` is nonzero goes into `gs_deferredNativeResizeWakes`.
  Only the global operation tail rearms it. The final contact check precedes
  the real `SendMessageW(hwnd, WM_NCLBUTTONDOWN, ...)`; a posted request is not
  evidence of `WM_ENTERSIZEMOVE`.
- `wxWinUIVisualCoordinates::Snapshot` records content, native HWND and
  structure generations plus `syncEpoch`; `SyncSlot()` commits `m_rectPx`
  only after setters and revalidation. These are distinct identities.
- `FocusMigration::windowIdentity` in `include/wx/winui/private/tlwhost.h` is
  a comparison key, never a dereferenceable owner. `FocusRequest` additionally
  carries an `intentGeneration`; HWND equality alone is insufficient.
- `src/winui/gauge.cpp` and `src/winui/scrolbar.cpp` share the exact owner
  retirement sequence below. ScrollBar's mutation depth is an extra, local
  policy, not part of that common sequence:

  ```cpp
  m_owner.store(nullptr, std::memory_order_release);
  m_generation.fetch_add(1, std::memory_order_acq_rel);
  ```

- `wxWinUISlot::EndOperation()` can schedule physical disconnect and delete
  the slot; `wxWinUITopLevelHost::EndOperation()` only unwinds its operation.
  Similar RAII spelling does not justify merging these semantics.
- `src/winui/srchctrl.cpp`, `ResolvePeerPartsOnce()` invalidates the old edit
  generation before token revocation. In `src/winui/combobox.cpp`, the layout
  callback is deliberately passive and calls `QueueEditPartResolutionAtLayoutEdge()`.
  Template realization belongs to its deferred continuation.
- `RestartDropBrokerAfterPresentationInvalidation()` calls
  `m_dropBroker->GetSnapshotForTest()` in production and refuses a new broker
  if `ownsRegistration || locked`. Removing that decision as test-only would
  break rollback safety.

## Scope

Shared scope: `src/winui/tlwhost.cpp`, `include/wx/winui/private/tlwhost.h`,
`build/cmake/lib/core/CMakeLists.txt`, and the focused existing tests named
below. Each sublot adds only its stated private module/consumers. Proposed
module names are not claims that those files already exist. Do not modify
all six sublots in a single commit.

Existing authorities must be reused: `inputstate.cpp`, `inputrouter.cpp`,
`dropbroker.cpp`, `appearance.cpp` and the host registry are not replaced by
parallel services. Keep the public declaration surface stable while moving
implementation; moving declarations out of installed headers belongs to 106.

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

Execution order: **107A -> 107C -> 107D**; **107B -> 107E** is independent
once its 106 control seams are qualified; **107F** follows the corresponding
106 host seams. A sublot needs its own characterization baseline, not blanket
completion of every component or every physical campaign.

### 107A — Host authorities and native sizing transaction

Native extraction implemented on 2026-08-31 after `33ae89a111`:
`hostresize.cpp` now owns validation, posting, deferred dispatch, cancellation,
shutdown and the USER32 subclass transaction. The host still owns the request,
registry, global operation tail and native lifetime. The two non-installed
headers share the existing weak subclass context and the request state; no
second capture, teardown or deferred-wake authority is introduced.

The four real native-loop cases are unchanged. The exact baseline passes
**219 assertions / 7 cases** before and after in both DLL/static builds.
The new pure progress case adds **14 / 1**, separately checking idempotent
entry/exit; it does not replace USER32 integration. The 107F host **70/4**,
broker **206/3**, presentation **15/1** and observation **372/1** groups also
pass, along with smoke/Supported **2/2** in each linkage. `test_gui` and
`minimal` compile with `/warnaserror`, without test retries. Evidence is
`audit107a-on-*` in the two existing build trees. Shipping OFF/install/restore
is reserved for the combined architecture batch. UIA/tooltip extraction and
physical held-drag qualification remain open.

Move `NativeResizeRequest`, validation, posting, dispatch, cancellation and
termination to `src/winui/hostresize.cpp` with a private header only if needed.
The host remains the authority for registry, TLW/island ownership and shutdown;
the transaction keeps the ticket, weak grip, native target generation and
real USER32 entry/exit state. Preserve foreign capture, deferred wake ownership,
weak subclass contexts and final contact validation. No second teardown owner,
new public data member, or include-of-cpp split.

Then extract UIA/tooltip policy implementations to separate private modules,
one commit each, using their existing authority and lifetime contracts. OLE
and appearance already have modules: only the host's orchestration boundary
should remain here, not a duplicated implementation.

Prerequisites: native characterization from 102, runtime ownership from 104,
and the corresponding 106 test seam boundary. Keep the four real native-loop
tests in `tests/controls/winuichromefeedback.cpp`; add a pure transition test
for pending/entered/finished without replacing USER32 with a successful mock.

Baseline filter:
`[winui-native-resize],HostLifecycle::RegisterUnregisterLoop,HostLifecycle::DestroyTLWWithSlots,HostLifecycle::ShutdownRestoresRetainedContentState`.

### 107B — Minimal peer owner/generation primitive

Implementation evidence (2026-08-31): the first four-file extraction follows
the ControlHost source set committed as `c05fa04f05`. Gauge aliases the new
private `wxWinUIPeerLifetime<wxGauge>`; ScrollBar inherits only the matching
owner/generation protocol and retains its own mutation depth/guard. The helper
allocates nothing, exports no API, introduces no virtual methods, and does not
include WinRT. Its contract explicitly does not pin the owner or permit
cross-thread wx access.

Both original six-case baselines pass **332 assertions**. Both ON builds
then pass the same **332/6**, the three new pure tests **21/3**, and runtime
smoke/Supported **2/2**, with `/warnaserror`. New cases cover stale generation,
callbacks after invalidation/owner retirement, and distinct replacement state
for the same owner address. Existing control callbacks, their invalidation
before revocation and all old test bodies remain unchanged; this does not
qualify concurrent wx object access or physical input.

Exact four-path hashes and candidate/source verification are under
`F:\wxwinui-pr26890-audit107-peer-lifetime`; runtime/build logs are
`audit107b-*` in each build tree. Both shipping OFF builds and fresh installs
pass with `/warnaserror`. The private lifetime helper is absent from installed
headers and shared DLL exports; the preceding control/ControlHost/runtime
test-surface absence checks still pass. Fresh four-TU installed toolkit
consumers pass CTest **1/1** per linkage. One normal installed runtime process
completes **50 epochs** per linkage; this does not repeat the eleven-scenario
fault/renderer matrix from 106.

Both build trees are restored to `wxBUILD_TESTS=ALL`; `test_gui` and `minimal`
rebuild and rerun the unchanged **332/6**, new **21/3**, and smoke/Supported
**2/2** successfully. No 107B build or test failure/retry occurred. Installed
shipping core SHA256 values are:

- shared: `6ECE1B80FF82055C96BF455AE34CA8C39334F478209CB8612E061445A0D670FF`
- static: `8E006FC53F3250205EE4B1D057232053A8B5604654A3EC1ECA2E9FC3DCB10374`

This closes only 107B's local implementation/qualification. Remote CI for
this new source, physical input and 107A/C-F remain separate requirements;
the host itself is not decomposed by this small extraction.

Add `src/winui/peerlifetime.h` for the common owner/generation nucleus of
Gauge and ScrollBar only. Preserve generation initialization, memory ordering
and invalidation-before-revocation. Keep ScrollBar's mutation guard, event
silencing and each control's rollback local. Do not unify host/slot
`OperationGuard`, and do not migrate a third consumer without proving the
same contract first.

Scope adds `src/winui/gauge.cpp`, `src/winui/scrolbar.cpp`,
`tests/controls/winuirangedate.cpp`, `tests/controls/winuirangeevents.cpp`.
Add pure stale-generation/invalidation tests tagged `[winui-peer-lifetime]`;
keep real late-callback, destruction and Loaded cases unchanged.

Baseline filter: `wxWinUI Gauge*,wxWinUI ScrollBar*`. Count new primitive
assertions separately from the unchanged baseline. Risk is medium; a new
retirement policy is explicitly not part of this extraction.

### 107C — Coordinate units and atomic geometry publication

Extract `wxWinUIVisualCoordinates` implementation to
`src/winui/visualcoordinates.cpp`, then projection/coalescing to
`src/winui/hostgeometry.cpp`. Keep a single converter, fractional coordinates
until the final rounding boundary, and explicit client/screen-pixel versus
visual-DIP semantics. Use private types/names to remove demonstrated unit
ambiguity, not a mechanical type rewrite across every control.

Preserve `m_syncEpoch`, structure/content/native generations, frozen dirty
state, and publication of geometry caches only after successful XAML setters
and exact-owner revalidation. A nested flush must not observe an intermediate
`SetContent()` transaction. Depends on 107A's ownership boundaries.

Baseline filter:
`HostLifecycle::FractionalCoordinateArithmetic,HostLifecycle::SlotPointerCoordinatesRTL,HostLifecycle::FreezeThawCoalescedFlush,HostLifecycle::ReentrantSlotSyncDefersNewStructureEpoch,HostLifecycle::ExactSlotClipRectIsPhysicalAndGenerationSafe,HostState::ContentSwapFlushSeesOnlyPublishedModel`.

These tests cover arithmetic and real slot projection, not physical
cross-monitor movement. Generalized synchronous flushes, mirrors or a second
island require a separate design decision, not a refactor exception.

### 107D — Input policy versus focus/capture/native adapters

Extract host adapters to `src/winui/hostfocus.cpp` and
`src/winui/hostinput.cpp`, after 107A/C. Reuse the existing pure
`wxWinUIInputState` and router. Keep USER32 capture, XAML gesture ownership,
logical wx focus and generation-qualified native authority distinct.
Maintain the MSW bridge signatures and compile guards; this lot does not
authorize a second event state machine or a synthetic replacement for a
native notification.

In particular, carry `FocusMigration`'s weak window and comparison identity,
transport authority generation and source/destination intent generations
through reparenting. An implementation move must not emit a second wx focus
pair. Keep tests in `winuiinputstate.cpp`, `winuiinputrouter.cpp` and
`winuihostlifecycle.cpp`.

Baseline filter:
`[winui-input-state],[winui-input-router],HostLifecycle::FocusedReparentAcrossTLW,HostLifecycle::DeferredFocusSurvivesReentrantTripleAttach,HostLifecycle::FocusedReparentHonoursNativeRedirect,HostLifecycle::FocusedReparentHonoursGettingFocusRedirect`.

### 107E — Template identity/invalidation foundation

First compare SearchCtrl and ComboBox's real resolution contracts. If their
identity/invalidation core is equivalent, add `src/winui/templatecontract.h`
for a typed part result and resolved/missing/superseded identity, using only
`src/winui/srchctrl.cpp` and `src/winui/combobox.cpp` as initial consumers.
Otherwise retain two explicit local contracts; do not force a shared helper.

Neither the common result nor its cache may prolong a retired peer/XamlRoot.
Detach tokens on part replacement and revalidate after callback-bearing
operations. Search's bounded passes and Combo's passive layout/deferred
continuation remain separate policies. Versioned part inventories, fallbacks
and feature implementation stay in 108; support claims stay in 111.

Depends on 107B's lifetime vocabulary. Tests remain in `winuitextpeer.cpp`
and `winuicombomodel.cpp`; add a genuine post-Loaded cache invalidation case,
not just the existing detached/injected template fixture.

Baseline filter:
`wxWinUI SearchCtrl template actions,wxWinUI SearchCtrl survives destruction from query Click,wxWinUI ComboBox LayoutUpdated realization is deferred,wxWinUI ComboBox callbacks are destruction-safe`.

Do not widen to the ComboBox model/template-generations case that launches
an external UIA helper. If reliable identity cannot be defined without
changing retry policy, report that constraint and retain local resolvers.

### 107F — Production decisions, passive observation and fault injection

First production boundary implemented (2026-08-31, after `00fe7a3b21`):
`RestartDropBrokerAfterPresentationInvalidation()` now calls
`HasNativeOwnership()` on the broker's authoritative registration/lock state,
preserving exactly `ownsRegistration || locked`. It no longer requests a test
snapshot or native identity refresh to make that rollback decision.
`IsReady()` uses `m_initResult.status`, the same value copied by the sole
`PublishInitResult()` writer into diagnostics, and retains its UI-thread,
active-state, complete-pair and current-context checks.

The new `[winui-broker-observation]` case covers a ready pair, clean shutdown,
failed revocation retaining only registration, and failed unlock retaining
only the lock. Repeated production queries must not change native-call logs,
the shell mutation epoch, host flush scheduling/runs, or broker counters.
It adds **372 assertions / 1 case**, distinct from the unchanged host **70/4**,
ownership **206/3**, and real-island presentation rollback **15/1** baselines.
Both shared/static Release builds of `test_gui` and `minimal` pass with
`/warnaserror`; all these cases and smoke/Supported **2/2** pass in each
linkage, without retries. Logs use `audit107f-*` in the two existing build
trees, with unchanged binaries captured first as `audit107foundation-before-*`
(static presentation: `audit107f-before-presentation.log`). Shipping qualification
will be recorded against the combined architecture source after the separately
tested implementation commits; this entry alone does not claim an OFF build.

The broker's remaining installed testing declarations and the broader passive
observation/fault separation remain in 106/107F. In particular, the diagnostic
snapshot still revalidates native HWND identities for qualification: this
small change does not relabel every observer as side-effect-free.

After the relevant 106 extraction, replace the production use of the broker's
test snapshot with a narrow, non-mutating production ownership predicate in
`src/winui/dropbroker.cpp` and its existing private declaration. Preserve the
exact fail-closed `ownsRegistration || locked` meaning. Observation snapshots
must not schedule work; hooks/faults stay in non-installed test support.

Keep real counters, generation guards and opt-in production profiling. Add
no Rendering subscription, timer or `ForceRender()` merely to observe state.
Characterize repeated observation without changing mutation counters, queued
work or subscription ownership. This verifies the architecture boundary;
it is not a duplicate of 106's migration or 111's capability manifest.

Baseline filter:
`HostLifecycle::FreezeThawCoalescedFlush,HostLifecycle::ReentrantSlotSyncDefersNewStructureEpoch,HostState::ContentSwapFlushSeesOnlyPublishedModel,HostState::LoadedNotificationIsContentTransactional`.
Add the existing broker acquisition/rollback cases after reviewing their
non-input paths; do not run physical OLE as part of an automatic refactor.

## Verification and test plan

Before and after each extraction run the identical host lifecycle/state/input/slot tests plus Supported V0 in both linkages. Build installed consumers and compare expected public exports. Every new module needs a pure state test and a real-island integration test.

Use the exact sublot filter above as `$auditFilter107`, with no automatic
physical input. These commands use the existing Release build/dependency
configuration; a fresh configure must preserve its pinned Windows App SDK.

```powershell
$env:WX_UI_TESTS = '0'
foreach ($auditLinkage in @('shared', 'static')) {
    $auditBuild107 = if ($auditLinkage -eq 'shared') { 'F:\wxwinui-pr26890-feedback-build' } else { 'F:\wxwinui-pr26890-feedback-build-static' }
    $auditBin107 = Join-Path $auditBuild107 $(if ($auditLinkage -eq 'shared') { 'lib\vc_x64_dll' } else { 'lib\vc_x64_lib' })
    & 'C:\Strawberry\c\bin\cmake.exe' --build $auditBuild107 --config Release --target test_gui minimal --parallel 4 -- /warnaserror
    if ($LASTEXITCODE) { throw 'Build failed' }
    Push-Location $auditBin107
    try {
        & "$auditBuild107\winui\test-tools\Release\wx_winui_desktop_test_runner.exe" --timeout-ms=180000 -- "$auditBin107\test_gui.exe" $auditFilter107 --durations yes
        if ($LASTEXITCODE) { throw 'Focused tests failed' }
    } finally { Pop-Location }
    & 'C:\Strawberry\c\bin\ctest.exe' --test-dir $auditBuild107 -C Release --output-on-failure --no-tests=error -R '^(wx_winui_runtime_smoke|wx_winui_supported_beta)$'
    if ($LASTEXITCODE) { throw 'Smoke/Supported failed' }
}
git diff --check
```

Expected: both builds exit 0, unchanged baseline cases still execute/pass,
new tests pass separately, and each CTest invocation reports **2/2**. Archive
unique before/after logs, not just console summaries. `WX_UI_TESTS=0` is not a
universal guard: inspect any newly selected case for global input first.

For installed verification reuse the shipping/restore sequence of 106 with
fresh per-sublot prefixes: configure `-DwxBUILD_TESTS=OFF`, rebuild the affected
libraries and `minimal`, `cmake --install <build> --config Release --prefix
<fresh-prefix>`, then configure `tests/winui/toolkit-contract` in a fresh
consumer directory with `-G "Visual Studio 17 2022" -A x64`,
`-DCMAKE_PREFIX_PATH=<fresh-prefix>`, `-DwxWidgets_USE_STATIC=ON` (static) or
`OFF` (shared), and `-DWX_EXPECT_TOOLKIT=winui`. Build target
`wx_toolkit_contract` with `/warnaserror` and run its Release CTest: **1/1**.
Compare shipping exports/header surfaces against the baseline; preserve
already removed test APIs as absent. Restore `-DwxBUILD_TESTS=ALL`, rebuild
and repeat the same focused tests and 2/2 gates. A failed OFF build is not
permission to leave the shared build cache OFF or to ship test support.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Each named authority has a focused module and ownership contract; host coordinates rather than reimplementing all domains; no lost guards, public ABI growth or artificial file-size-only split.

- Each sublot has a separate reviewed implementation commit and recorded
  local/remote verification status. A plan refresh alone is not implemented.
- Each move retains the existing callback/rollback/retirement ordering;
  behavioral corrections are distinct commits with their own reproduction.
- The owner/generation and template nuclei have two concrete consumers, or
  the evidence-based decision not to share them is recorded explicitly.
- No `.cpp` includes, parallel brokers/routers, conditional exported-class
  layout, public namespace rewrite, or unapproved composition redesign.
- Plans 108/109/110/111 retain their actual component, physical, performance
  and documentation work. Foundation completion does not mark them DONE.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

If two consumers have materially different rollback/retirement semantics, do not force them into a generic abstraction. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
