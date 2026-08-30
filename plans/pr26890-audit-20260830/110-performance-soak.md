# Plan 110: Measure pointer performance and lifecycle stability with reproducible budgets

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: MED
- Depends on: 102, 104, 107 relevant paths; 109 harness safety

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

Pointer/canvas optimizations already exist; unsafe last-move coalescing and negative cursor caching were removed. Historical 165 Hz data does not substitute for current 60/120/165 Hz and long-run evidence.

## Scope

existing WinUI instrumentation and performance harnesses; hot-path fixes only from measured regressions; qualification evidence

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Define measured input-to-paint latency and work-per-move metrics with reproducible scene, sample count, warm-up and hardware/runtime identifiers. Set explicit budgets from baseline, not invented universal numbers.
2. Compare generic canvas, native child and XAML peers at available 60/120/165 Hz, including final motion delivery, capture, resize, scrolling and topology changes.
3. Keep safe hit-resolution reuse; optimize only measured dominant work without dropping terminal pointer events or stale-target checks.
4. Run creation/destruction and modal/OLE/resize soak for 60 minutes; track working set, CRT/COM objects, hooks, handles, capture and dumps. Separate process-lifetime framework caches from growing leaks.

## Verification and test plan

Run existing instrumentation with SHA/hash-bound outputs; compare p50/p95/p99 and terminal pointer delivery, not just mean. Soak must last the declared duration and produce a real completion timestamp plus cleanup proof.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Budgets and measured results recorded for available environments; no growing application-owned leaks or lost final pointer motion; signed human observations kept distinct from automatic completion.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Do not bypass input safety gates or invent refresh-rate/soak evidence for unavailable hardware. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
