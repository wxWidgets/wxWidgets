# Plan 104: Define runtime startup failure and ownership of process effects

- Status: LOCAL / REMOTE PASS
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

src/winui/winui.cpp; src/winui/renderer.cpp; private runtime headers; src/common/rendcmn.cpp for the existing lazy-initialization overwrite; src/common/wxcrt.cpp only if policy requires; runtime/install-consumer tests; docs/winui runtime contract

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

## Implementation decisions (2026-08-30)

- Extend the scope to the common renderer holder: its first Get previously discarded a renderer already supplied via Set before lazy initialization. Skip traits creation when a renderer is already owned; no public API or class layout changes.
- Choose the WinUI default renderer once, not once per runtime epoch. Traits/explicit custom renderers win; subsequent application replacements or Set(nullptr) are never overwritten during runtime restart. Ownership remains with the existing global renderer holder and callers receiving Set's previous pointer.
- The required WinUI module propagates startup failure. Failed initialization already cleans its pre-XAML COM/bootstrap state or quarantines unsafe XAML state; it cannot rely on OnExit after OnInit fails.
- Add a private one-shot before-bootstrap failure after COM acquisition to verify clean failure independently of XAML quarantine. Update the installed-consumer subprocess oracles for actual wx startup rejection and renderer ownership.

## Execution evidence (2026-08-30)

- Implementation based on d1a415743f, with only the runtime/renderer, installed-consumer and documentation changes in this lot. Concurrent PropertyGrid changes remain separately owned by 101 and are not evidence for this lot.
- Shared/static Release core and minimal builds pass (`audit104-core-build.log` in each existing WinUI build). MSW and Qt Debug core/test builds also compile the common renderer change without warnings-as-errors failures; their independently failing PropertyGrid cases remain OPEN in 101.
- Fresh `cmake --install ... --config Release` prefixes: F:\wxwinui-pr26890-audit104-install-shared and F:\wxwinui-pr26890-audit104-install-static. External core consumers configure/link against those installed packages, not in-tree headers. Both Release builds with `/warnaserror` pass (`audit104-build-stderr.log` in their consumer build directories).
- Both complete consumer CTests pass, exit 0: eleven subprocesses per linkage, 43.79 seconds shared and 43.39 seconds static. Each checks normal fifty-epoch teardown/wrong-thread rejection, MTA startup rejection/recovery, clean pre-bootstrap failure/recovery, custom/default renderer ownership over fifty epochs, and six separate terminal-quarantine faults. No new matching WER dump or observed access violation is accepted.
- Exact runner form: `wx_winui_desktop_test_runner.exe --timeout-ms=300000 -- ctest --test-dir <consumer-build> -C Release -V --output-log <consumer-build>/audit104-matrix-final.log`, with `WX_UI_TESTS=0`. Consumer builds are F:\wxwinui-pr26890-audit104-consumer-shared and F:\wxwinui-pr26890-audit104-consumer-static. No physical input injection or user-desktop activation.
- The initial MTA run exposed a fixture logging defect: failed wx startup destroys its log target, and the next expected warning fell back to MessageBoxW. Captured stacks prove this. The console fixture now reinstalls heap-owned stderr targets after initialization/cleanup; expected failure diagnostics remain visible. The failed/aborted logs are retained, not reported as passing runs.
- `ctest -C Release -R '^wx_winui_runtime_smoke$'` and `-R '^wx_winui_supported_beta$' --no-tests=error` each pass in both WinUI builds on a private desktop, exit 0. Logs: `audit104-runtime-final.log` and `audit104-supported-final.log`.
- `git diff --check`: PASS. Local Windows 11/MSVC evidence only; remote WinUI execution remains pending, and these tests do not qualify physical input or a 60-minute soak.

## Remote confirmation

- On published SHA 445f908e8d, Actions run 33319364184 completes both WinUI jobs successfully (static 99278529900, shared 99278529911). The pinned-runtime registration and installed runtime-contract step execute successfully in each job, followed by passing runtime smoke and Supported V0. These results cover the external-consumer startup/ownership tests added by this lot; they do not qualify physical input or soak behaviour.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
