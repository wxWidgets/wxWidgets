# Plan 103: Preserve public dialog identity and geometry in Window presentation

- Status: LOCAL / REMOTE PASS (external UIA qualification remains in 109)
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

## Implementation decisions (2026-08-30)

- Window presentation borrows the public dialog's existing HWND through a private presenter entry point. It never swaps HWND pointers or destroys a borrowed public dialog during presenter cleanup.
- Text/password/colour establish default geometry once during Create; subsequent application Move/SetSize calls remain authoritative. A weak wx identity plus native HWND generation protects callback and presentation boundaries.
- MessageDialog retains its separately owned presenter shell/native fallback contract. Overlay remains opt-in and may fall back to Window on the same public dialog when applicable.
- Native non-input tests observe actual visibility, geometry, modal identity and XAML content. They are not a physical-input or external UIA sign-off.

## Verification (2026-08-30)

- Source: implementation on e12f747138, committed with this record; MSVC 19.44, Windows 11 build 26340, pinned Windows App SDK 1.8.260710003. Shared/static test_gui, minimal and runtime smoke build successfully.
- Both linkages pass 498 assertions / 2 cases for `[winui-dialog-identity]`: text/password/colour retain their public HWND, initial and changed geometry, modal/event identity, parent and result codes across three presentations; actual XAML OK-button invocation exercises validation rejection and acceptance.
- The existing activation optimization temporarily detaches GW_OWNER. Tests require the exact saved owner, current native generation and settled transaction while detached, then require the real owner link restored and transaction removed after hiding. Logical wx parenthood is checked independently throughout.
- Both linkages pass 618 assertions / 6 cases for `[winui-dialog-window],WinUIDialogContracts::DestroySourceCancelsPresenter,WinUIDialogContracts::ForceUpperWhileOpen`, including owned MessageDialog shells and opt-in Overlay destruction/validation paths.
- `ctest -C Release -R '^(wx_winui_runtime_smoke|wx_winui_supported_beta)$' --output-on-failure --no-tests=error` passes both tests in both builds; Supported V0 passes 1660 assertions / 92 cases.
- All runners use `WX_UI_TESTS=0` and private desktops, without system-input injection. Logs in each build root: audit103-owner-build.log, audit103-window-lifetime.log, audit103-gates.log and audit103-gates.xml.
- The broader auxiliary-dialog selection exposed a timeout in the unchanged NativeProgressBoundaryAndLifetime test. It was diagnosed and locally resolved separately in 109; its task-dialog hook coverage is not counted as real native-window qualification. Remote CI and external UIA observation remain pending.

## Remote confirmation

- On published SHA 445f908e8d, Actions run 33319364184 completes both WinUI jobs successfully (static 99278529900, shared 99278529911). Runtime smoke and Supported V0, including this lot's dialog-identity cases, execute and pass. This does not claim external UIA observation or physical modal-input qualification, which remain separate in 109.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
