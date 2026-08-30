# Plan 103: Preserve public dialog identity and geometry in Window presentation

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P0
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101; 102 native-loop infrastructure where shared

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

textdlg.cpp explicitly creates a hidden wxDialog, then presenter.Create(parent, caption) creates the visible shell. Initial position/size are stored but not used to size the visible shell. Text/password and colour need actual geometry/identity correction. Native wxMSW MessageDialog uses TaskDialog, so an unconditional persistent-HWND parity claim would be wrong.

## Scope

src/winui/dlgpresenter.cpp; src/winui/textdlg.cpp; src/winui/colordlg.cpp; src/winui/msgdlg.cpp; related private/public dialog headers; tests/controls/winuidialogcontracts.cpp

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Add real Window-mode tests observing visible HWND, public object, initial position/size, live Move/SetSize, activation/size events and parent ownership.
2. Let Window presentation host content on the public dialog's existing HWND, beginning with text/password, then colour. Preserve modal hook identity, deferred destruction, validation, TransferData and results.
3. Evaluate MessageDialog against its documented/native baseline; avoid adding a second public-visible identity when adopting the presenter path.
4. Keep Overlay explicitly opt-in with its distinct geometry/owner contract. Test repeated presentation, parent destruction, cancellation and fallback separately.

## Verification and test plan

Build both linkages; add/run test_gui '[winui-dialog-identity]'; run existing dialog contract tests discovered in tests/controls/winuidialogcontracts.cpp and Supported V0 CTest. Compare HWND identity with IsWindowVisible/GetWindowRect in native Window mode, not just GetHandle != nullptr.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

The public text/password/colour dialog controls the visible geometry and receives its events; destruction/modal tests pass; Overlay contract remains explicit; no doubled UIA tree is claimed without an external observation.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

If preserving the existing destruction contract requires replacing public object identity, pause that design and record the conflict instead of swapping arbitrary HWND pointers. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

