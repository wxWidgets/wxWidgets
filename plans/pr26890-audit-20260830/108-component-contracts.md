# Plan 108: Close component parity gaps and version template dependencies

- Status: TODO
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P2
- Effort: L (split into independently verified commits)
- Implementation risk: HIGH
- Depends on: 101; 105; 106; host changes affecting each family

## Context and workspace

Work in F:\\wxwinui-pr26890-feedback on codex/pr26890-feedback. C:\\wxWidgets-master is dirty and is out of scope. Existing generated build trees are F:\\wxwinui-pr26890-feedback-build (shared) and F:\\wxwinui-pr26890-feedback-build-static; packages are under F:\\wxwinui-maintainer-integration-packages. Use apply_patch for source edits, wx coding style, and preserve generation/reentrancy checks. Tests use Catch2 TEST_CASE and focused tags; follow adjacent existing cases. Public wx class names and one toolkit-selected ABI remain as in ADR 0004.

The user explicitly requested planning AND implementation. This is not permission to weaken tests, hide unimplemented features, fabricate physical sign-off, or rewrite published Git history. Make bounded implementation commits after verification; no mass opportunistic cleanup.

## Drift check and current state

Run git diff --stat c5cf4677b9..HEAD -- <in-scope paths> and git status --short first. Reconcile relevant intervening changes instead of overwriting them. Read each target before editing.

The audits distinguish implementation from qualification and report concrete SpinButton focus divergence plus template coupling and incomplete owner-draw/style/metric contracts. Feature percentages are estimates, not acceptance criteria.

## Scope

src/winui control implementations and headers; generic/MSW fallback integration only where demonstrated; tests/controls; private template contract registry

Out of scope: other worktrees, frozen beta snapshots, unrelated backend refactors, user files, new public PRs and unrequested physical input.

## Steps

1. Work family-by-family: buttons/toggle/check/radio/static/feedback; text/search; choice/combo/list/checklist; gauge/slider/scroll/spin; date/time/calendar; notebook/tree/books; menus/toolbar/status/tooltips; generic/advanced fallbacks.
2. For every family test common API, programmatic silence/user event order, best-size/font/DPI, disabled ancestors, focus/UIA, RTL/theme/HC, retemplate/virtualization and reentrant destruction.
3. Fix SpinButton Tab/focus parity first. Cover bitmap margins/states, partial markup, owner-draw expectations, rich paragraph/bullet limitations, tree line/full-row styles, toolbar rows/wrapping and tooltip delay behaviour explicitly.
4. Centralize pinned template-part contracts for Search/Combo/List/Slider/Time/Calendar/Toolbar. Missing parts must produce a controlled supported fallback or precise failure, not heuristic accidental success.
5. Test generic composites separately from native peers: panels/scrolled/splitter, header/list/DataView/grid, combo/editable/rearrange/property/filesystem, richtext/STC/HTML, WebView/media/GL/ActiveX, AUI/Ribbon/MDI/native adoption, printing/shell. Classify spatial overlap restrictions precisely instead of declaring every native surface impossible.

## Verification and test plan

Add named family regression tags, run each focused test_gui filter plus unchanged Supported V0. Build minimal/widgets/dataview/showcase/dialogs/winuispike as their affected targets allow. Template-version tests must create actual peers after Loaded and cover replacement/missing-part fixtures; physical matrices remain separate.

For every commit, git diff --check must exit 0. Record exact commands, exit status, assertion counts when applicable, build path, source SHA and whether a result is local, remote, simulated, native or physical. A missing/unavailable tool is a recorded coverage gap, not a PASS.

## Done criteria

Every family in the audit matrix has feature-level disposition and tests; implemented and physically qualified are distinct; no feature silently removed to turn a checklist green.

All criteria are required before DONE. An implementation can be LOCAL PASS / REMOTE PENDING or PHYSICAL PENDING without being completed. Update the index with evidence and unresolved gates.

## Stop conditions

No claim of native Ribbon, native titlebar or fully supported advanced surface based only on successful generic compilation. Hardware-dependent claims remain pending their gate. If a confirmed defect requires an out-of-scope change, extend the plan explicitly with its reason before editing; do not start another general review. Preserve diagnostics from failing checks.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.

