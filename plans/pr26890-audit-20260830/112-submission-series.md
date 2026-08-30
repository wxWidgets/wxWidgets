# Plan 112: Prepare reviewable submission boundaries without rewriting the integration branch

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P3 / maintainer decision
- Effort: L (split into independently verified commits)
- Implementation risk: MED
- Depends on: 101-111 sufficient stable boundaries

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

The audits recommend splitting a 539-file integration PR. That is an upstream review strategy, not a compiler/runtime fact. Existing commits and user changes must remain recoverable.

## Scope

local submission manifest and patch/commit mapping; PR draft text; no automatic public PR creation/history rewrite

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Map stable dependencies into proposed submissions: toolkit/build; runtime; minimal host; input/focus/move-size; UIA/tooltip/appearance/OLE; basic controls; text/items; range/date/calendar; books/tree; chrome/dialogs.
2. Separate behavioural fixes from pure extractions and remove generated evidence/history from proposed product patches without destroying the integration branch or audit trail.
3. For each proposed boundary provide scope, public contract, tests, upstream dependencies and a reproducible application/build command.
4. Request maintainer/user agreement before creating new public PRs, closing #26890, rebasing published history, force-pushing or removing features.

## Verification and test plan

Verify proposed patch ordering applies to its documented base in disposable worktrees; build each proposed boundary and compare final product tree to the integration result. Preserve a complete commit mapping.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Reviewable series plan and reproducible boundary validation are available; actual upstream split/merge is separately approved, not marked complete merely because a document exists.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

No force push, PR close/create, destructive cleanup or feature removal without explicit direction. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
