# Plan 102: Make native resizing a host-owned input transaction

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P0
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101 local compilation baseline

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

The grip posts raw WM_NCLBUTTONDOWN and its test hook bypasses PostMessageW while ignoring its return value. QueueModalNativeDispatch and DispatchPendingNativeInput already exist in the host but use CallAfter. A client-area grip must not counterfeit an ordinary native-hit snapshot: native hit refresh expects the same actual non-client hit zone.

## Scope

src/winui/statbar.cpp; src/winui/tlwhost.cpp; include/wx/winui/private/tlwhost.h; new private native move/size controller; tests/controls/winuichromefeedback.cpp; tests/winui native input tests; relevant CMake test registration

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Introduce a narrowly scoped private host request/controller: Idle, Pending, Entered, terminal completion/cancellation. Return Rejected/Scheduled distinctly; acknowledge Entered only on the native notification.
2. Queue a private window message after the XAML callback, not an idle CallAfter. Keep a unique ticket and validate owner/host/HWND generations, live style, maximized/disabled state, grip direction and physical button state at processing time.
3. Use the host capture authority. Release only capture owned by this host/gesture, cancel or refuse foreign capture, revalidate after reentrant boundaries, and clean up on close/cancel/exit. Do not broadly call ReleaseCapture on another control.
4. Keep full-width screen coordinates in the request. Reuse pending native dispatch where its invariants apply; keep a separate typed grip-origin validation from real native-hit validation.
5. Replace success-substitution tests with native message observations. Add cancellation/deduplication/lifetime tests and actual USER32 begin/exit integration. Retain physical held-drag testing as a separate required gate.

## Verification and test plan

Build shared and static test_gui/minimal. Add and run test_gui '[winui-native-resize]' for non-physical integration and regression cases. Verify one WM_ENTERSIZEMOVE/WM_EXITSIZEMOVE pair, correct cancellation and no residual capture. The opt-in physical gate must verify GetWindowRect changes and wxEVT_SIZE/WM_SIZING BEFORE button release and stops at first release; LTR/RTL, quick release, foreign capture, destroy/reparent, negative coordinates and DPI cases are separate results.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

No raw resize dispatch owned by StatusBar; no test hook that pretends a resize occurred; exactly-once native transaction demonstrated. Physical acceptance stays OPEN until executed on an identified binary.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

No physical input injection or capture takeover on the user's active desktop without explicit per-run consent. Do not claim physical correctness from a mocked button reader. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

