# Plan 105: Make toolkit identity and public source compatibility explicit

- Status: LOCAL / REMOTE PASS
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

## Decision and evidence (2026-08-30)

- Keep the existing distinct toolkit ABIs and shared `__WXMSW__` window substrate. Use `__WINDOWS__` for OS services, `__WXWINUI__` for XAML implementation dispatch, subsystem `wxUSE_*` guards for features, and exclude WinUI explicitly when accessing ordinary MSW control internals. No global macro removal or parallel public class family.
- Correct README's rendering-backend-only description and qualify ADR 0004's source-compatibility statement. The new `docs/winui/toolkit-contract.md` records RadioBox/SpinCtrl/SpinButton inheritance, static-box/buddy helpers, and inherited HWND/theme/background hooks. Presence of a method is not a claim of native rendering/input semantics; component behaviour remains in 108/109.
- The dispatch self-test checks all 35 directly dispatched public headers (30 MSW alternatives, five generic/other alternatives). Reversed MSW-first and incorrectly guarded fixtures must be rejected. Registered CTest passes in the shared build (`audit105-dispatch.log`); standalone script exits 0 too.
- New standalone installed-package consumer compiles four separate translation units, without source-tree includes or a PCH. It checks the toolkit macros, all directly dispatched headers, portable operations, intentional inheritance differences, native buddy API presence/absence and common protected hook overrides. These are explicitly compile-only OBJECT targets, not linked/running GUI applications or ABI interchangeability tests.
- MSVC 19.44 x64 `/warnaserror` builds and compile-only CTests pass against four installed configurations: WinUI shared/static Release (`F:\wxwinui-pr26890-audit104-install-{shared,static}`), ordinary MSW shared Debug (`F:\wxwinui-pr26890-audit105-install-msw`), and ordinary MSW static Release (`F:\wxwinui-pr26890-audit105-install-msw-static`). Consumer build directories are `F:\wxwinui-pr26890-audit105-consumer-{shared,static,msw,msw-static}`. Logs: `audit105-compile-contract.log` for the first three; `audit105-msw-static-consumer-{configure,build,ctest}.log` and CTest XML for the fourth.
- The static MSW installation is a core-focused profile: optional AUI/GL/HTML/media/PropertyGrid/Ribbon/RichText/STC/WebView/XRC/XML/network/debug-report libraries disabled, all 35 probed controls enabled. It is not full-feature static MSW runtime qualification. Configure/build/install logs are `audit105-msw-static-{configure,build,install}.log` in `F:\wxwinui-pr26890-audit105-msw-static-build`.
- A separate consumer deliberately configured with `WX_EXPECT_TOOLKIT=msw` against the WinUI shared installation fails compilation with the exact wrong-toolkit diagnostic. Expected-failure logs are retained as `audit105-negative-{configure,build}.log` in the shared consumer directory.
- Both WinUI CI linkages now run the dispatch self-test, install their built package, execute 104's external runtime consumer and compile this external header consumer. Every native command checks its exit status; bounded timeouts and JUnit/log artifacts are retained. YAML and rendered shared/static PowerShell syntax/control-flow checks pass locally. These new CI steps have not yet passed remotely.
- `git diff --check`: PASS. WinUI remains documented as MSVC-supported; no local Clang execution or unsupported-compiler qualification is claimed. RTTI/XRC implementation is unchanged; these probes do not replace their existing runtime tests.

## Remote confirmation

- On published SHA 445f908e8d, Actions run 33319364184 completes both WinUI jobs successfully (static 99278529900, shared 99278529911). The installed toolkit-header consumer step executes and passes in both jobs, alongside the installed runtime consumer, runtime smoke and Supported V0. Later test-access migrations need their own source-SHA evidence; the successful published consumer is not silently attributed to untested future headers.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
