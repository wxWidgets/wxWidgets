# Plan 111: Keep support documentation and feature evidence synchronized

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: LOW
- Depends on: Start inventory after 101; close after relevant implementation/qualification lots

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

The published matrix/body still list rich TextCtrl styles, wxCB_SIMPLE, slider ticks/thumb length, date spin and calendar week numbers as absent even though code exists. Historical 50/927 counts are not current-head qualification.

## Scope

docs/winui; plans/winui3-v0/component-matrix.md; machine-readable capability manifest and generation/check tool; generated PR text draft

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Create one versioned feature-level manifest: component, rendering kind, portable/MSW-specific contract, implementation evidence, test IDs, physical requirements and validated SHA/environment.
2. Reconcile every row of the supplied audit matrix; retain all existing supported/fallback/excluded entries until verified, with no estimated percentage used as proof.
3. Generate/check matrix and concise public docs from the manifest. Separate implemented, integration-tested, physically qualified and excluded.
4. Document single-plane overlap limits, primary-pointer touch/pen policy, toolkit macros, global runtime effects and Overlay mode accurately.
5. Keep historical evidence clearly dated and non-authoritative. Prepare corrected PR body/comment text; do not silently rewrite user prose outside requested technical scope.

## Verification and test plan

Add generator --check mode and fixture tests for unknown test IDs, stale SHA claims, impossible support promotions and missing family entries. CI fails on doc/manifest drift. Validate referenced test names against test_gui inventory.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

No stale absence claim for implemented features and no promotion without evidence; every audit row maps to a manifest entry and open/closed disposition.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Do not delete historical evidence or user plans to reduce the diff; archive/exclude from a proposed submission only with recoverable provenance. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

