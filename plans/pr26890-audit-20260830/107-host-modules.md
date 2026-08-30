# Plan 107: Split host responsibilities behind private ownership boundaries

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 102, 104, 106 characterization coverage

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

wxWinUITopLevelHost owns slots, lifetime, geometry, input, capture, focus, keyboard, UIA, tooltips, OLE, appearance and transients. The audits identify coupled authorities, not merely a line-count target.

## Scope

src/winui/tlwhost.cpp; include/wx/winui/private/tlwhost.h; new private host subsystem files; CMake source list; host tests

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Extract native move/size first, then geometry/slot operations, focus+keyboard, UIA/tooltips, OLE and appearance/lifetime in separate commits.
2. Define ownership and reentrancy boundaries before each extraction. Keep host ownership, generation checks and transactional rollback intact; no include-of-cpp splitting.
3. Factor shared peer lifetime/generation/transaction/template-resolution primitives only after at least two concrete consumers demonstrate equivalent requirements.
4. Separate pure moves from behavioural changes so regressions can be attributed. Keep internal APIs narrow and unexported.

## Verification and test plan

Before and after each extraction run the identical host lifecycle/state/input/slot tests plus Supported V0 in both linkages. Build installed consumers and compare expected public exports. Every new module needs a pure state test and a real-island integration test.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Each named authority has a focused module and ownership contract; host coordinates rather than reimplementing all domains; no lost guards, public ABI growth or artificial file-size-only split.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

If two consumers have materially different rollback/retirement semantics, do not force them into a generic abstraction. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
