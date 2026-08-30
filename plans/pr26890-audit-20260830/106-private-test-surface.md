# Plan 106: Move testing facilities out of installed public control APIs

- Status: IN PROGRESS (Button/BitmapToggle local pass; remaining families open)
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

## Implementation boundary

- Begin with the Button/BitmapToggle family and its existing shared bitmap/lifetime tests, then migrate the other families without dropping their characterization coverage.
- Public controls gain only an unconditional private friend declaration; test entry points move to a non-installed support header/target. Do not put any data member or virtual method behind a test-build macro.
- Compile private access implementations only for explicitly test-enabled builds, adjacent to the owning implementation so opaque PIMPL definitions do not leak into application headers. Shipping builds (`wxBUILD_TESTS=OFF`) must contain neither these access exports nor the corresponding helper operations. A test-enabled Release binary is an instrumented test build, not the shipping export baseline.
- Limit the opt-in compiler definition to migrated control translation units. Verify an OFF build and an ON build have the same public class layout and production behaviour, with private support available only to in-tree test consumers. Do not rename a public test method and call that API removal.

## Button/BitmapToggle migration evidence

- Removed the seven public operations from the two installed control headers. Their real peer operations now live behind a private friend accessor declared only in `tests/winui/test-support/button-test-access.h`. No data member or virtual method is conditional on the test build.
- The non-installed `wx_winui_test_support` interface is available only with GUI tests enabled. Only `button.cpp` and `tglbtn.cpp` receive its implementation definition; no test target is retained in the static library's installed link interface.
- Baseline and migrated real-island runs use exactly `[winui-appearance],[winui-html],WinUIDialogContracts::*`: **5328 assertions / 68 cases** pass in both shared and static Release. Logs `audit106-button-baseline.log` and `audit106-button-migrated.log` in each build directory. These are non-input tests (`WX_UI_TESTS=0`), not physical interaction qualification.
- The installed consumer adds seven compile-time checks rejecting the old public call expressions. MSW shared/static both pass compilation with `/warnaserror` and the compile CTest (`audit106-api-absence*` in the 105 consumer directories).
- Four compile-only COFF probes (shared/static, test definition present/absent) give identical `sizeof/alignof`: Button **872/8**, BitmapToggle **784/8** on MSVC x64. Inputs, object files, symbol/raw-data inspection and values are retained in `F:\wxwinui-pr26890-audit106-layout-proof`. This checks size/alignment, not a full historical ABI or vtable comparison.
- Shared shipping proof: reconfigure `wxBUILD_TESTS=OFF`, rebuild `wxcore`/`minimal` with `/warnaserror`, then install to `F:\wxwinui-pr26890-audit106-install-shared`. The core DLL exports no `wxWinUIButtonTestAccess` or old Button/BitmapToggle test methods; neither private header nor support target is installed. The fresh 106 installed consumer compiles and its CTest passes. Logs `audit106-shipping-*` and `audit106-consumer-*` in the shared build tree.
- Static shipping proof also passes: OFF configure/build/install, absent private/old test symbols in the archive, no installed support header/target, and a fresh installed consumer compiling four translation units with `/warnaserror` plus CTest 1/1. Prefix: `F:\wxwinui-pr26890-audit106-install-static`; logs `audit106-static-off-*`, `audit106-static-shipping-*` and `audit106-static-consumer-*` in the static build tree.
- Both trees were restored to `wxBUILD_TESTS=ALL` and rebuilt. Shared and static again pass the same 5328/68 cases (`audit106-restored-button.log`, `audit106-static-restored-pilot.log`); shared smoke/Supported V0 also pass after restoration. Static full PropertyGrid passes 1208/2 in the final run (`audit106-static-restored-propgrid.log`); its earlier smoke/Supported V0 gate passed 1660/92. ON/OFF/ON archive inspection confirms that the seven explicit private static entry points return only with test support.
- The full lot remains OPEN: the other 26 control headers and private host/helper facilities still need migration. Button coverage is not evidence that the ComboBox external helper or all public test APIs have been removed. Next bounded families: StaticText/StaticBitmap/StaticBox and Gauge/ScrollBar/SpinButton.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
