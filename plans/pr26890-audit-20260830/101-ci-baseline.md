# Plan 101: Restore existing-port builds and execute the WinUI runtime CI

- Status: IN PROGRESS
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P0
- Effort: S/M
- Implementation risk: MED
- Depends on: none

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

At c5cf4677b9, settings.cpp declares `const wxSystemColour requestedIndex = index;` outside the WinUI guard. wxIDropTarget is polymorphic, non-final and uses delete-this COM lifetime. The runtime registration step uses the workflow default pwsh; the recorded failure is Appx module loading (0x80131539), before package installation. Both WinUI builds succeeded, and the Supported V0 steps were skipped.

## Scope

src/msw/settings.cpp; src/msw/ole/droptgt.cpp; .github/workflows/ci_msw.yml; focused build/CI regression tests

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Guard requestedIndex with the exact condition at its use: defined(__WXWINUI__) && wxUSE_WINUI3. Do not change colour policy in this commit.
2. Make the translation-unit wxIDropTarget class final after checking for subclasses; remove the diagnostic suppression around IMPLEMENT_IUNKNOWN_METHODS instead of adding a Clang suppression.
3. Use Windows PowerShell only for Appx registration. Preserve pinned-version selection, require exactly one matching framework MSIX, and fail on errors.
4. Execute the existing wx_winui_runtime_smoke CTest separately before wx_winui_supported_beta. Add --no-tests=error, JUnit output and CTest failure logs.
5. Build ordinary MSW with MSVC /warnaserror and, if available, Clang warnings-as-errors. Build and run WinUI shared/static smoke and Supported V0. Publish the bounded fix to the existing PR when locally verified; record remote results separately.

## Verification and test plan

MSW: configure a separate F: build with -DwxBUILD_TOOLKIT=msw -DwxBUILD_SHARED=ON -DwxBUILD_TESTS=ALL, then build wxcore with /warnaserror. Existing WinUI trees: cmake --build F:\wxwinui-pr26890-feedback-build --config Release --target test_gui wx_winui_runtime_smoke --parallel 8; repeat for F:\wxwinui-pr26890-feedback-build-static. Run ctest --test-dir <tree> -C Release -R '^wx_winui_runtime_smoke$' --no-tests=error --output-on-failure, then the same command with '^wx_winui_supported_beta$'. Read gh pr checks 26890 --repo wxWidgets/wxWidgets; pending/skipped is NOT passed.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Both known compiler regressions absent without suppressions; smoke and integration tests actually execute in both linkages; remote jobs and local results identified by SHA; any further CI failure remains OPEN, not relabelled infrastructure.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

If installation reaches a different package/dependency error, retain the evidence and diagnose that failure; do not silently skip runtime or loosen package identity. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
