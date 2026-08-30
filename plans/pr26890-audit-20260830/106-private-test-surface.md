# Plan 106: Move testing facilities out of installed public control APIs

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101; 105 contract; proceed control-by-control

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

Many exported classes expose WinUI*ForTesting methods and snapshots. ComboBox includes an environment-driven external UIA helper. The status-bar hook demonstrates why observing a callback does not qualify USER32 behaviour.

## Scope

include/wx/winui control headers; corresponding src/winui files; private accessors and non-installed test-support target; tests/controls/winui*; CMake installation/export lists

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Inventory public test methods, layout fields, Release exports, helper subprocess code and consumers; distinguish production lifetime services from test-only APIs.
2. Move state to existing stable PIMPLs and access through narrow private friend accessors; put test operations/helper launchers in a non-installed test-support target.
3. Keep library/test object layout identical; do not hide layout-changing members behind a test-only macro applied to only one target.
4. Migrate consumers by family with characterization tests retained. Replace success-substitution hooks with observers or dependency-level fault injection preserving semantics.
5. Verify installed headers and release exports contain no accidental ForTesting surface.

## Verification and test plan

Use rg 'ForTesting|HookForTesting' against installed public headers and inspect DLL exports with dumpbin; both must have no accidental test API after migration. Build an external installed consumer without test support, plus the internal suite with test support, in shared/static.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

No test-only methods/fields in installed exported controls; helper subprocesses are test binaries only; no test/production layout mismatch; equivalent real-path regression coverage preserved.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Do not delete a lifetime/retirement service used by production merely because its name resembles a test helper. Reclassify and privatize it deliberately. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

