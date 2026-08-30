# Plan 101: Restore existing-port builds and execute the WinUI runtime CI

- Status: LOCAL PASS / REMOTE PENDING
- Planned at: c5cf4677b9627eebce7b69eba427e1658dfbcbe5, 2026-08-30
- Priority: P0
- Effort: L (expanded by reproduced cross-port regressions)
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

2026-08-30 evidence-driven extension: src/aui/auibook.cpp (DFB member ambiguity), src/propgrid/propgrid.cpp and its focused tests (forwarder reaching a destroyed owner), portable GUI test fixtures and newly failing lifetime/capture/font tests. Reconcile current origin/master by a normal merge preserving both upstream changes and WinUI work. No force-push. The three initial changes alone do not restore all CI.

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

## Execution evidence (2026-08-30)

- Three-file implementation on the c5cf product baseline: exact conditional declaration, final concrete COM wrapper without suppression, Windows PowerShell Appx registration plus distinct smoke/integration CTests and diagnostic artifacts.
- `git diff --check`: PASS. YAML parsing/structural checks and PowerShell syntax checks: PASS.
- `cmake --build F:\wxwinui-pr26890-msw-audit-build --config Debug --target wxcore --parallel 8 -- /warnaserror`: exit 0. Log: `F:\wxwinui-pr26890-msw-audit-build\msw-debug-build.log`.
- Both existing WinUI build trees: Release `test_gui` and `wx_winui_runtime_smoke` rebuilt, exit 0.
- Both `wx_winui_runtime_smoke` CTests: exit 0. Both `wx_winui_supported_beta` CTests: exit 0, 51 cases / 970 assertions per linkage. Logs and JUnit: `audit101-runtime-smoke.*` and `audit101-supported-beta.*` in each build tree.
- Environment: local Windows 11 Pro build 26340, MSVC 19.44.35228, x64. No physical input injection. No local Clang compiler found; remote Clang coverage remains required.
- Remote validation and any additional failing cross-platform jobs remain OPEN. Compilation success does not qualify user-observed grip behaviour.

## Additional verified CI blockers

- c5cf has 25 failed and 18 successful GitHub checks, none cancelled. Eleven failures duplicate the three initial causes. Ten expose PropertyGrid/macOS failures, three Qt tests, one DFB compilation failure.
- PropertyGrid: GTK UBSAN identifies editor forwarder callbacks into a wxPropertyGrid whose dynamic type is already wxWindow during base destruction (job 98012205198). Detach forwarders while the owner is alive; preserve editor delivery.
- DFB: new tabs->m_rect references are ambiguous between wxAuiTabContainer and wxWindowDFB (job 98012205168). Qualify the intended rectangle.
- AppVeyor VS2015: wxMSWOlePendingTimerCancel list initialization with default member initializers is rejected (job 7058whx65rg78tju). Use portable explicit initialization.
- HTML-listbox fixture wrongly assumes horse.gif is next to the executable (macOS bundles / AppVeyor VS2019). Use the existing test-resource contract or a self-contained fixture.
- Qt AUI selected-font and book-page destruction fixtures are corrected in the prepared master merge: explicit font override state and destruction at the common `DoSetSize` boundary. This does not claim to fix or qualify deletion from inside a native Qt resize callback.
- The HeaderCtrl failures were reproduced locally on MSW Debug, then fixed without changing assertions: empty visible order, mutation cancellation, and successful completion versus capture cancellation. `HeaderCtrlTestCase` now passes 321366 assertions (legacy single fixture case); `audit101-header-fixture-fixed.log`. Rebuild with `/warnaserror`: exit 0.
- PropertyGrid forwarder-owner lifetime regression: local MSW Debug passes 17 assertions / 1 case; WinUI shared/static also pass. GTK UBSAN and macOS remote checks are still required; their other editor assertions are not assumed fixed.
- DataView's new drag fixture sends a wx mouse event to a composite native header on MSW; it needs the real native notification boundary instead. DirCtrl's new mutation test reproduces a crash when a delete-item callback rebuilds the tree during native deletion. Both remain OPEN until corrected and tested.
- Existing DatePicker/Grid failures are not automatically blamed on the PR.
- origin/master advanced from b2502d42b2 to 6155567922. All 19 conflicts are resolved in normal merge commit 3278026ea0 on `codex/pr26890-master-refresh`, including conversion of added CppUnit cases to upstream Catch2 without removing coverage. Integration/build of this merge remains pending at this entry. GitHub reports no Actions runs for a76e0689bf; conflicting merge state, AppVeyor failure and CircleCI pending are not green qualification.

## Post-merge local evidence

- Integrated master 6155567922 by normal merges 3278026ea0 and b83c4d1db6. No conflict markers or CppUnit references remain in the migrated test sources; published history is preserved.
- MSW Debug `test_gui` rebuilt with `/warnaserror`; WinUI shared/static Release `test_gui`, `minimal` and runtime smoke rebuilt. All exit 0. Final fixture builds: `audit101-fixture-final-build.log` in each build tree.
- MSW focused Header/HTML-list/VList/book-base/AUI/TreeList/DataView regressions: exit 0, 323062 assertions / 117 registered cases, `audit101-master-regressions-final.log`. This is the non-input profile (`WX_UI_TESTS=0`); inherited simulator cases do not qualify physical input.
- DataView/TreeList debugging proved a fixture use-after-free after deleting the callback's owner: `audit101-dataview-cdb.log`. Four callbacks now publish their outputs before terminal deletion. Native headers are entered through `HDN_BEGINDRAG` notifications, not a wx mouse event sent to the composite parent. Generic-header assertions are preserved.
- HTML image fixture now uses a self-contained BMP, whose standard handler is available even when the test runs alone; no dependency on a previously executed image test or installed PNG support.
- DirCtrl's exact crashing mutation case: exit 0, 32 assertions / 1 case, including repeated rebuild requests during deletion and long Unicode paths. A 30-second aggregate timeout was not a pass; the diagnostic 60-second run completed successfully.
- Both WinUI smoke and Supported V0 CTests pass after the merge: 1160 assertions / 90 cases per linkage. The count changed from 55 to 90 because upstream Catch2 migration exposes individual cases; the assertion count is unchanged. JUnit/log: `audit101-master-gates.*`.
- Full PropertyGrid run remains FAIL: the forwarder-owner fix does not resolve the separate editor-validation and callback-removal assertions at 2945-2949, 3066-3067, 3125 and 3157-3158. These macOS-reported failures are now reproduced on local MSW; focused diagnosis continues before this lot can close.
- Additional book-control run: 55 / 58 cases pass; Listbook `SetItemTopologyReentry`, Toolbook `BitmapLookupReentryIsPrePublication` and native Treebook `ControllerVetoRestoresSelection` remain FAIL (`audit101-master-books.log`). They extend this same CI regression lot.

### Subsequent focused results

- All six book-control families now pass on MSW Debug: 471 assertions / 58 cases (`audit101-master-books-fixed.log`). Native tree selection guarding ends before the application CHANGED callback, permitting the Treebook veto rollback. Listbook checks each implementation's consumed-state return contract; Toolbook realizes native buttons before its destructive callback is armed.
- Native tree programmatic single/multiple selection passes 22 assertions / 2 cases. The interactive `SelectionChange` test is not qualified on an isolated desktop; its missing `EnableUITests()` opt-out was corrected, and it is excluded from non-input evidence.
- The four initial PropertyGrid callback-removal sections now pass in MSW Debug and WinUI shared/static: 38 assertions / 1 selected case in each environment (`audit101-propgrid-removal-fixed.log`). Effective removal at idle is checked, not just disappearance from name lookup.
- Full PropertyGrid execution then reaches further failures in composed change, toolbar rollback, splitter notification counting and an event-value destruction fixture. The full run still fails; these newly reachable failures remain within this lot until corrected and retested.
- WinUI shared/static native resize + owner-forwarder + DirCtrl group passes 313 assertions / 8 cases after master integration (`audit101-postmerge-native.log`).
- Full MSW Debug PropertyGrid now passes: 1137 assertions / 2 cases, `audit101-propgrid-complete.log`; `/warnaserror` rebuild also exits 0. The four newly corrected sections pass 55 assertions per WinUI linkage (`audit101-propgrid-final-sections.log`).
- Composed changes now track the owning page independently from the displayed page. Toolbar rollback realizes restored native buttons, with native button-count assertions. Two fixtures distinguish legitimate destination-page resize notifications and avoid reading a lambda capture after deleting its owner. The final-page count assertion now follows the existing public zero-page contract, retaining the internal page-identity check.
- Full WinUI PropertyGrid is still FAIL in both linkages: `Retained_page_dispatch_stops_at_each_destroy_boundary`, target 0, SIGSEGV after 413 successful assertions. This separate failure is being isolated; the passing MSW suite and focused WinUI sections do not qualify the full WinUI suite.
- Non-input execution explicitly sets `WX_UI_TESTS=0`, including the WinUI CI job. An inherited tree-selection test which ignored that opt-out now respects it on its system-input branch.
- The retained-page test was calling synchronous child `Destroy()`, leaving its manager with a freed internal grid. It now explicitly schedules the child/manager and asserts that they remain live and scheduled during dispatch; top-level destruction still uses `Destroy()`. Dispatch suppression and eventual destruction assertions remain unchanged. No production change was needed for this fixture.
- Full WinUI PropertyGrid now passes in both linkages: shared 1175 assertions / 2 cases, static 1179 / 2, `audit101-propgrid-all-pass.log`. Rebuilds exit 0 (`audit101-pg-retained-build.log`). All tested local regressions in this lot are now passing; remote platform coverage is still pending.
- Published normal merge/fixes through 0080db6c16. GitHub now reports MERGEABLE and has started Actions, including both WinUI jobs (MSW builds run 33314769703). No history rewrite. Local final smoke + Supported V0 gates pass 1162 assertions / 90 cases per linkage (`audit101-final-gates.log` and `.xml`).

### CI on 0080db6c16

- Remote jobs exposed additional defects; this lot remains OPEN. Qt/GCC rejects seven missing children initializers in TransactionalTreeModel; explicit empty initializers now compile in local MSW/WinUI and Qt Debug builds.
- WinUI static job 99266038086 compiled successfully, then failed bootstrap with 0x80670016 on Windows Server 2022. The pinned framework version exactly matches the runtime header (8000.921.1539.0); the job had excluded the matching DDLM package needed on this OS. Register the pinned framework and DDLM, verify architecture/version/publisher, and retain registration artifacts. Neither runtime test is skipped or relaxed.
- Windows PowerShell 5.1 parsing and three mocked-registration scenarios pass (success, missing DDLM registration, old framework); real pinned MSIX manifests are inspected, with no local Appx installation. Evidence: F:\wxwinui-pr26890-msw-audit-build\audit101-runtime-registration-check.ps1. A new remote runner execution is still required.
- CheckWhitespace identified extra blank EOF lines in plans 103-112; these are removed, not ignored by CI.
- Non-MSW PropertyGrid editor-handler teardown, the macOS callback lifetime case, and Qt callback deletion remain separately tracked while their focused fixes are tested. Passing compilation does not close these runtime failures.
- Qt 5.15.2 Debug/shared is now built locally in F:\wxwinui-pr26890-qt-audit-build using MSVC 19.44 and installed Qt MSVC64. Atomic single selection removes the intermediate clear/select notification; fixtures await Qt's deferred notification, use Windows (not wxMSW) long-path helpers, and retire controls safely on assertion failure.
- The remaining Qt FileCtrl crash was reproduced: selectionChanged dispatched an application callback that deleted the native view, then continued in QTreeView; SetItem likewise continued through a deleted model. The fix invokes the Qt base before application notification and checks QPointer lifetime after each callback/selection boundary. The unchanged crashing fixture passes 28 assertions with only the fixed production DLL; the strengthened fixture passes 31.
- Final Qt `[filesystemctrl]`: 215 assertions / 6 cases, exit 0, approximately 68 seconds. No input injection; `WX_UI_TESTS=0`, private desktop, explicit Qt bin/plugin paths. Logs: audit101-qt-filesystem-final.log and audit101-qt-filesystem-crash-stack.log. Qt 6.10 remains a remote coverage requirement.

### CI on 1cd4017b5a

- GitHub Actions run 33316310653: both `wxWinUI CMake Release x64 shared` (job 99270302085) and `wxWinUI CMake Release x64 static` (job 99270301994) finished SUCCESS. Build, pinned runtime registration, runtime smoke and Supported V0 each executed successfully; neither runtime gate was skipped.
- This confirms the Windows Server DDLM registration fix on the published SHA. It does not qualify the later local 104-106 changes or their new installed-consumer steps, which were not present at that SHA.
- Existing-port runtime failures remain OPEN. Local Qt/PropertyGrid corrections and the GTK compilation check continue before publishing the next bounded batch.

### Cross-platform callback follow-up

- Qt toolbar removal left an action in the widget action list after removing its layout widget. Reinsertion during PropertyGrid rollback then crashed inside Qt. Removing the action and widget coherently fixes the unchanged rollback case (16 assertions); embedded control ownership remains with its wx wrapper. Log: `F:\wxwinui-pr26890-qt-audit-build\audit101-propgrid-toolbar-rollback-full.log`.
- GTK3 Debug/shared now compiles and links the full `test_gui` target (GCC 11.4, GTK 3.24.33, Ubuntu 22.04 WSL). The filesystem regression fixture needed its explicit generic FileCtrl header. Log: `F:\wxwinui-pr26890-gtk-audit-build\audit101-build-test_gui-retry.log`, exit 0. This is compile-only evidence; no GTK display/runtime test was executed. The two modal-preferences fixture helpers are now guarded by their actual usage conditions; the follow-up `audit101-build-test_gui-guards.log` compiles/links without warnings or errors.
- PropertyGrid destruction now detaches only its own forwarders, preserving control-owned hint handlers and application handler ownership. Native GTK/Cocoa/Qt editors are detached before their parent dies, and a callback epoch retains their wrappers across nested idle processing. MSW native parking keeps its existing identity checks. Tests observe real editor lifetime and eventual destruction, not merely a cleared model pointer.
- A renderer deleting a selected property could synchronously repaint into its active Qt QPainter. A private drawing-depth guard queues that invalidation instead. `IsEditorFocused()` now rejects a null focus window instead of comparing it equal to absent editors. The five isolated Qt renderer/keyboard/capture regressions all pass.
- Full `[propgrid]` after these changes: Qt Debug/shared 908 assertions / 2 cases (`audit101-propgrid-qt-reentrant-draw-full.log`), MSW Debug/shared 1147 / 2 (`audit101-propgrid-qt-common-final.log`), WinUI Release/shared 1205 / 2 (`audit101-propgrid-current.log`), WinUI Release/static 1217 / 2 (`audit106-static-propgrid-final.log`), all exit 0 with `WX_UI_TESTS=0`. The MSW run retains the same 128 section/case labels; assertion counts can vary with legitimate paint callbacks. All three Windows builds use `/warnaserror`. GTK/Cocoa runtime and Qt 6 remain remote requirements.
- The capture-release fixture now uses the real native notification only on MSW, where it is defined. Other ports invoke a test-local callback after their real virtual `DoReleaseMouse()` boundary; no native notification is fabricated and all lifetime/release assertions are retained. GTK compile/link of this final fixture also passes without warnings (`audit101-build-test_gui-capturehook.log`). Native input/focus delivery remains a separate integration gate.

### CI on 445f908e8d

- Whitespace, mixed-EOL, spelling, all-headers and C++ style checks pass. The WinUI shared/static jobs are still building at the recorded snapshot; the installed-consumer and runtime steps have not yet executed. No later-SHA WinUI CI pass is inferred from the earlier 1cd4017b5a run.
- GTK builds reject the top-level `const` on the local `GtkWidget*` passed to the GLib `g_object_ref` macro (`-Werror=ignored-qualifiers`, including job 99278538282). Removing only that pointer qualification preserves the ref/remove/unref lifetime transaction. A GCC 11.4/GTK 3.24 compile-only reproduction fails with the old declaration and passes with the corrected declaration under `-Wall -Wextra -Werror`; logs are in `F:\wxwinui-pr26890-audit101-compiler-probes\glib-ref-*`.
- MSW 32-bit GCC job 99278507863 rejects the toolbar rollback fixture's unbounded `size_t`-to-`int` traversal (`-Werror=aggressive-loop-optimizations`). The fixture now checks the range and uses the API's signed index type for both loops, retaining all previous assertions. This is an observed compiler diagnostic, not evidence of an executed runtime overflow.
- The corrected full PropertyGrid test translation unit also compiles under local MinGW GCC 13.2 with `-m32 -O2 -Werror`. The wrapper requires PropertyGrid and toolbar support plus 32-bit pointers; the resulting 1,885,294-byte COFF object has machine 14C and contains the real Catch test and toolbar-failure fixture. Evidence is `propgridtest-mingw32-enabled*` in the compiler-probe directory. The earlier 9,203-byte object used a disabled PropertyGrid profile and is explicitly excluded. GCC 14 remote execution is still required.
- Additional runtime failures remain OPEN: Qt 5/6 `wxGenericDirCtrl` path selection and `wxInfoBar::Appearance` (jobs 99278564547, 99278564580 and 99278538270); Cocoa ARM ASAN detects a freed secondary editor reached by the focus-destruction fixture (job 99278566919). The exact logs are retained in `F:\wxwinui-pr26890-audit101-ci-445f908e8d`. These are being diagnosed separately from the two compilation repairs.
- Subsequent completion: both WinUI jobs in Actions run 33319364184 pass (static 99278529900, shared 99278529911). Configure/build, pinned-runtime registration, both installed-consumer steps, runtime smoke and Supported V0 all execute successfully. This confirms lots 103-105 and the Button test-access pilot on 445f908e8d. Other-port failures keep this CI lot OPEN.
- Cocoa focus-destruction repair: a commit's native focus restoration can destroy the secondary editor and emit a focus event from its surviving sibling before the commit sanitizes its slots. `HandleCustomEditorEvent()` now skips only SET/KILL_FOCUS during that transaction, before reading those slots; text-event revision handling and the native event chain remain intact. The existing fixture performs the real deletion, sends both focus events through the primary's real forwarder at that point, then retains the native `SetFocus()` call.
- That strengthened section passes 13 assertions, and the unchanged text-reentrancy section passes 12 on MSW Debug; full PropertyGrid passes 1162/2 (MSW), 1208/2 (WinUI shared) and 1218/2 (WinUI static), all with `WX_UI_TESTS=0` after `/warnaserror` builds. Logs: `audit101-mac-focus-guard-*` in the MSW build and `audit101-runtime-followup-propgrid.log` in both WinUI builds. These deterministic forwarding tests do not replace the pending Cocoa ASAN rerun.
- Qt InfoBar's pre-Create colour setter dereferenced a null widget. Qt now stores that colour until creation, applies it to the native palette instead of overwriting it with defaults, and resolves `wxNullFont` before asking for a native font handle. The original appearance case now checks the real Qt palette/font too: 18 assertions / 1 case pass; full InfoBar passes 53/7 on Qt 5.15.2, 49/7 on MSW and 108/12 on each WinUI linkage. No existing assertion or case is removed.
- Qt full PropertyGrid with the focus guard also passes 910 assertions / 2 cases (`audit101-qt-propgrid-focus-guard-after.log`). GTK3/GCC 11.4 compiles and links the full `test_gui` after these changes (`audit101-runtime-followup-build-retry.log`, exit 0; the initial attempt used a non-existent WSL distribution name and did not compile anything). No GTK GUI or Cocoa execution is claimed locally.

## Maintenance

Retest these contracts when the pinned Windows App SDK, compiler, installed-header surface or host lifecycle changes. Keep implementation, integration and physical qualification claims separate.
