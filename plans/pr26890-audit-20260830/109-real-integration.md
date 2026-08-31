# Plan 109: Qualify native input, modal loops, UIA and OLE on real surfaces

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 102, 103, 104 and affected 108 families

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

Current deterministic suites do not establish physical resize, IME, retained external UIA providers or interprocess OLE. Existing physical harnesses and captured RC failures are regression inputs, not current successful evidence.

## Scope

tests/winui integration clients and harnesses; focused tests/controls; product code only for reproduced failures

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Separate A pure-state, B real HWND/XAML/native messages and C physical/system-input gates in test names, output and publication.
2. Cover nested MessageBox/TaskDialog/menus/OLE/resize from XAML callbacks, parent destruction, cancel, capture cleanup and exactly-once keyboard dispatch.
3. Run an external UIA client with retained providers across content changes, reparent and recycled HWND; inspect Value/Text/Selection/ExpandCollapse/RangeValue, password privacy and wxAccessible authority.
4. Add interprocess OLE source/target across bridge/TLW/native holes and existing COM registrations; cover delayed formats, DragOver destroy/reparent, cancellation and teardown. Never revoke a foreign registration.
5. Execute consented keyboard/AltGr/dead-key/IME, mouse, advertised touch/pen, RTL, HC/theme and multi-monitor DPI matrices, including negative coordinates and Remote Desktop where available.

## Verification and test plan

Harness artifacts must identify SHA, binary hash, runtime version, OS, input type, actual native events, pass/fail and cleanup. Use separate processes and bounded timeouts. Start physical scripts only with explicit user consent; never auto-answer their safety prompt.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Each advertised behaviour has actual integration/physical evidence. Unsupported devices/environments and unexecuted scenarios remain OPEN, with exact requirements; no inferred or fabricated human sign-off.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Stop physical runs immediately for unexpected foreground/capture, user activity, crash/dump or inability to clean up. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

### Progress-dialog boundary regression (2026-08-30, locally resolved)

The broader non-input `[winui-dialog-lifetime]` run completes text/password/colour, MessageDialog and RichMessage tests but times out in `WinUIDialogContracts::NativeProgressBoundaryAndLifetime` in both linkages. Reproduce this auxiliary contract independently and diagnose the task-dialog worker/fixture before claiming the whole dialog integration matrix passes. Logs: audit103-dialog-contracts.log in both build roots and audit103-native-progress.log in the static build. The six scoped Window/presenter cases pass 618 assertions and do not establish this progress-dialog result.

Subsequent stack diagnosis proves the deadlock: the fixture supplies a null task-dialog HWND, while the upstream dark-mode helper calls EnumChildWindows(nullptr), enumerating top-level windows. The worker enters UIA while holding the progress lock and the GUI thread waits for that lock. Central null guards in AllowForTaskDialog and RemoveFromTaskDialog prevent accidental whole-desktop enumeration; the fixture still exercises the same callback sequence and assertions.

Shared/static Release rebuilds pass. NativeProgressBoundaryAndLifetime passes 237 assertions / 1 case, and `[winui-dialog-lifetime]` passes 353 assertions / 4 cases in each linkage. Logs: audit109-progress-build.log, audit109-native-progress-final.log and audit109-dialog-lifetime-final.log in each build; original stack audit109-native-progress-stack.log in the static build. All runs use private desktops and WX_UI_TESTS=0. This is a task-dialog API-boundary test with a hook, not a real native TaskDialog window or physical/UIA sign-off. The other integration gates in this plan remain pending.

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
