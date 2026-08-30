# Plan 105: Make toolkit identity and public source compatibility explicit

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P1
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101; before broad API cleanup

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

README says both __WXWINUI__ and __WXMSW__ are defined. ADR 0004 already selects distinct toolkit library ABIs with unchanged wx class names. Specific MSW protected APIs and inheritance are not automatically reproduced by XAML controls.

## Scope

build/cmake toolkit definitions; include/wx platform dispatch headers; docs/winui/README.md; docs/winui/adr/0004-backend-namespaces.md; public compile-contract tests

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Inventory dispatch headers and public/protected source-compatibility differences with compile probes, including RadioBox, SpinCtrl/SpinButton, theme/background helpers and ContainsHWND/MSW hooks.
2. Add a dispatch-order check and ordinary-MSW/WinUI compile matrix. Make no promise that __WXMSW__ implies every native control implementation detail.
3. Specify precise internal capabilities for HWND/COM/native fallback/XAML. Migrate reusable platform code incrementally, keeping one coherent public class definition per build.
4. Choose and document the toolkit macro migration based on compile probes; do not remove __WXMSW__ globally in one edit. No parallel public wxWinUI::Button family.

## Verification and test plan

Add installed-header consumers for both toolkit configurations; build shared/static with MSVC and Clang where supported. New lint must fail on a deliberately reversed WinUI/MSW dispatch fixture and pass current headers. Run ctest install-consumer tests for both toolkits.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Every intentional compatibility divergence is explicit and compile-tested; macros express a supported contract; no unintended wxMSW regression; public names/RTTI/XRC remain coherent.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

Public namespace, ABI or external application compatibility redesign beyond the selected toolkit model needs a recorded decision before implementation. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
