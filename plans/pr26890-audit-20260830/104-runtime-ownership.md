# Plan 104: Define runtime startup failure and ownership of process effects

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

wxWinUI3Module::OnInit currently ignores wxWinUI3Initialize() and returns true deliberately. Renderer installation executes delete wxRendererNative::Set(new wxRendererWinUI), with no restoration. LC_NUMERIC=C is already documented; the message hook is UI-thread-local, not system-global. Quarantine/module pinning deliberately prevent use-after-unload.

## Scope

src/winui/winui.cpp; src/winui/renderer.cpp; private runtime headers; src/common/wxcrt.cpp only if policy requires; runtime/install-consumer tests; docs/winui runtime contract

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Characterize renderer custom-before-init, replacement-while-running, repeated epochs, destructor counts and teardown ordering.
2. Make renderer installation independent from repeated runtime epochs, or retain displaced ownership and restore only if the current renderer is still the one installed by this runtime. Never delete a newer application renderer.
3. Make a required WinUI-toolkit runtime failure fail startup deterministically. Preserve any intentionally optional integration/native diagnostic fallback under an explicit separate contract.
4. Write and test an effect/lifetime table for locale, hook, theme state, renderer and module pins. Distinguish clean shutdown, failed init and quarantine.
5. Retain fail-closed retirement; no locale restoration while retained XAML can still run and no unpinning callback code as leak cleanup.

## Verification and test plan

Add private runtime-policy/renderer tests and subprocess install-consumer failure cases. Run existing runtime smoke plus runtime teardown/fault-injection suites; enumerate with ctest -N before selecting. Every subprocess must have a bounded timeout and specific exit diagnostic.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Custom renderer ownership preserved; startup policy explicit and tested; global effects and irreversibility accurately documented; no false zero-leak claim from ignoring retained runtime objects.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Changing process-wide locale or unloading quarantined callback code requires proof all consumers are retired; if unavailable, document the restriction rather than weakening safety. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

