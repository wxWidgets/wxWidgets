# PR #26890: audit remediation plan

Plan established on 2026-08-30 against c5cf4677b9627eebce7b69eba427e1658dfbcbe5.
Prepared using the improve planning structure; the user explicitly requested implementation after planning. This index supplements the historical V0 plans rather than treating their old DONE labels as current qualification.

## Sources

- A: audit_pr_26890_complet.md, 1362 lines, comprehensive audit and component matrix (sections 3-14).
- F: first pasted review beginning "Verdict global", 860 lines, focused resize/build/CI review (sections 1-16).
- S: supplemental review beginning "Tu avais raison", 1378 lines, architecture and component review (sections 1-16).

The reports are evidence leads, not execution instructions. No raw audit dump or copied review prose is added to the product sources.

## Execution order

| Plan | Scope | Status |
|---|---|---|
| [101](101-ci-baseline.md) | Restore existing-port builds and execute the WinUI runtime CI | DONE at 2947664462: 45/45 REMOTE CHECKS PASS |
| [102](102-native-resize.md) | Make native resizing a host-owned input transaction | NATIVE LOCAL PASS / PHYSICAL PENDING |
| [103](103-dialog-identity.md) | Preserve public dialog identity and geometry in Window presentation | LOCAL / REMOTE PASS; EXTERNAL UIA IN 109 |
| [104](104-runtime-ownership.md) | Define runtime startup failure and ownership of process effects | LOCAL / REMOTE PASS |
| [105](105-toolkit-contract.md) | Make toolkit identity and public source compatibility explicit | LOCAL / REMOTE PASS |
| [106](106-private-test-surface.md) | Move testing facilities out of installed public control APIs | 28 CONTROLS + RETIREMENT + CONTROLHOST QUALIFIED LOCALLY; OTHER HOST HELPERS OPEN |
| [107](107-host-modules.md) | Split host responsibilities behind private ownership boundaries | 107B QUALIFIED; NATIVE 107A / FIRST 107F BOUNDARY PASS ON; OTHER MODULES OPEN |
| [108](108-component-contracts.md) | Close component parity gaps and version template dependencies | IN PROGRESS: CONCRETE FAILURES AND TEST ORACLES |
| [109](109-real-integration.md) | Qualify native input, modal loops, UIA and OLE on real surfaces | TODO |
| [110](110-performance-soak.md) | Measure pointer performance and lifecycle stability with reproducible budgets | TODO |
| [111](111-capability-manifest.md) | Keep support documentation and feature evidence synchronized | TODO |
| [112](112-submission-series.md) | Prepare reviewable submission boundaries without rewriting the integration branch | TODO |

Execute one implementation lot at a time. A remote build may run while the next independent lot is prepared, but its predecessor remains pending until the result is known. No repetitive whole-repository re-audit between lots. Each defect receives one focused verification; new failures expand the same lot rather than starting an unrelated initiative.

Architecture extension (2026-08-31): 107 now specifies six bounded foundation
sublots: host/native resize authority, minimal peer lifetime, geometry
publication, focus/input adapters, template identity, and production/test
observation boundaries. Their exact source anchors, filters and stop
conditions are in 107. Existing single-island and toolkit-ABI decisions are
preserved; changing them requires a separate design decision. The minimal
Gauge/ScrollBar lifetime primitive (107B) is implemented and locally qualified.
The native resize transaction (107A) and first production/observation boundary
(107F) now pass both test-enabled linkages; their combined shipping check and
the remaining modules are still open. This is not full host decomposition.

## Findings coverage

| Audit points | Disposition / plan |
|---|---|
| A P0-1/P0-2; F 4/5; S 5 | Confirmed compiler regressions -> 101 |
| A P0-3; F 6; S 5 | Confirmed runtime job not executed; observed failure is Appx import in pwsh -> 101 |
| A 3.9/P0-4; F 1-3; S 3 | Raw posted resize request and bypassed native test confirmed. Current physical outcome must be remeasured -> 102 |
| A 7.9/P0-6; S 11 | Text/password/colour hidden-vs-visible geometry split confirmed. MessageDialog compared with native baseline -> 103 |
| A 3.14/P1-4/P1-5/P1-6; S 4 | Renderer lifetime, required init failure and process effects -> 104 |
| A 5/P1-3; F 8; S 6 | Toolkit identity, dispatch, protected MSW APIs/inheritance -> 105 |
| A 6/P1-2/P3; F 7/11; S 7 | Installed testing surface, layout, exports and environment helper -> 106 |
| A 3.4/3.5/3.15/P1-1; F 9; S 1.3/1.5 | Host authorities and repeated peer protocols -> 107 |
| A 7/P1-7/P2 and matrix rows; S 8-10/12/14 | Every component family, template contract and fallback boundary -> 108 and 111 |
| A 3.6/3.10-3.13/9/P1-10; F 10-13; S 2 | Real HWND/input/focus/keyboard/modal/UIA/OLE/DPI/RTL/theme/HC -> 109 |
| A 3.7/9/13; F 13/15 | Pointer budgets, 60/120/165 Hz, leak growth, 60-minute soak -> 110 |
| A 3.3/8/P1-8/P1-9/P3; S 1.4/13 | Public composition limits, stale claims, source-of-truth manifest -> 111 |
| A P0-5/12; F 14; S 16 | Submission size/review strategy -> 112; external split requires agreement |

## Component worklist (plan 108, tracked at feature granularity in 111)

- Foundations: runtime, host, control host/XamlHost, slots, geometry, z-order/clip/scroll, pointer/capture, keyboard/focus/cursors, tooltip, UIA, OLE, modal/transients, theme/Mica/renderer.
- Buttons/selection: Button, BitmapButton, Toggle/BitmapToggle, CheckBox, RadioButton, RadioBox, generic CommandLink.
- Text/items: TextCtrl plain/rich/password/RTF/coordinates/IME/autocomplete, Search, Choice, Combo simple/editable/readonly, BitmapCombo, ListBox owner-draw/extent, CheckList virtualization/focus.
- Range/date: Gauge vertical, Slider ticks/thumb, ScrollBar event order, SpinButton focus, SpinCtrl/Double inheritance/numeric entry, DatePicker spin/dropdown, TimePicker seconds/period, Calendar week numbers/styles/realization.
- Display/feedback: StaticText/Bitmap/Box/Line, ActivityIndicator, Hyperlink, InfoBar, ColourPicker; bitmap/font/HC/DPI/event/lifetime contracts.
- Books/tree: Notebook top/side/bottom/overflow/drag; Simple/Choice/List/Tree/Toolbook; TreeCtrl styles/edit/multiselection/DnD; TreeList and AuiNotebook distinct fallbacks.
- Chrome/dialogs: MenuBar/Menu/submenu OPEN/CLOSE/keyboard, ToolBar rows/wrapping/overflow/controls, StatusBar/grip, ToolTip timing, RichToolTip fallback, native titlebar; Message/Text/Password/Colour dialogs and Overlay; Font/Find/File/Dir/RichMessage/Progress/Busy/pickers/Wizard/property/preferences fallbacks.
- Generic/advanced/platform: Panel/scrolled/splitter/header/list/DataView/Grid/VList/HtmlList; ComboCtrl/owner-drawn/editable/rearrange/collapsible/filesystem/property grids; RichText/STC/HTML/WebView/Media/GL/ActiveX; AUI/Ribbon/MDI/NativeWindow; Popup/MiniFrame; DC/GDI+/Direct2D/printing; taskbar/notification/splash/tip; installed/deployed consumer profiles.

## Findings qualified or not accepted literally

- Hook scope: WH_GETMESSAGE is installed for the UI thread, not globally for all processes. Test nested-loop ownership; do not report it as a system-wide hook.
- Overlay is already opt-in. Keep and test the distinction; do not treat opt-in as absent.
- LC_NUMERIC=C is already documented and protects retained XAML. Its irreversible effects need a precise lifecycle contract, not blind restoration.
- Framework pinning/quarantine is deliberate lifetime safety; retained framework objects are not automatically application leaks.
- Native MessageDialog uses TaskDialog under wxMSW too. The text-dialog geometry defect is concrete; a universal persistent-HWND rule for all common dialogs is not assumed.
- Single-plane composition prevents arbitrary overlapping interleaving. It does not alone prove that every spatially separated native/WebView/GL surface is impossible.
- Queuing a request is not proof of entered sizing. Marking a routed event handled for an owned cancellable pending request is legitimate; waiting to mutate routed args after callback return is not the proposed fix.
- Component percentages in audits are estimates. Readiness is proven per feature/test/environment, not by an estimated percentage.
- Splitting the public PR is a review-policy recommendation. Prepare boundaries without closing/replacing PR #26890 or rewriting its history automatically.

## Completion policy

DONE requires the implementation and its specified evidence. Local build, pure-state test, real-island integration, remote CI and physical/human sign-off are separate statuses. No missing test is converted to PASS, no fixture replaces the behaviour it claims to qualify, and no unsupported component is silently removed.

## Evidence log

- Remote CI subsequently completes **45/45 SUCCESS** on `00fe7a3b212fc14be7df2ebbf7657dba25c07318`, including Windows Qt 5.15/6.10 and both WinUI linkages. This covers the published retirement/ControlHost/107B batch, not the new architecture changes below it.
- 107B extracts only the common Gauge/ScrollBar owner/generation protocol into a private, allocation-free helper. Shared/static ON/OFF/install/ON pass the unchanged **332 assertions / 6 control cases**, **21 / 3** new lifetime cases, smoke/Supported **2/2**, shipping header/export checks, fresh four-TU consumers and one normal 50-epoch installed runtime process each. ScrollBar mutation policy, generation ordering and old test bodies are preserved. Both build trees end at `wxBUILD_TESTS=ALL`, with `minimal` rebuilt. The other architecture sublots, new remote CI and physical qualification remain open; exact logs and shipping hashes are in 107.
- ControlHost's nine probe operations and two Loaded adapters are isolated from shipping. The real content getter remains a production dependency under `GetContent()`. Both shared/static ON/OFF/install/ON cycles pass **1421 assertions / 33 unchanged cases**, smoke/Supported **2/2**, strict installed-header/symbol checks, fresh four-TU consumers and one normal 50-epoch installed runtime process per linkage. The initial WinRT include/macro compilation failure and its correction are recorded in 106; no test is removed. Other host/keyboard/bootstrap helpers remain open.
- 2026-08-31 final CI confirmation on `29476644627e701109cb750b083ebe0c11a6bb3c`: **45/45 SUCCESS**, including both formerly stalled Windows Qt jobs. Qt 5.15 runs 872 CTests plus 356894 GUI assertions / 1042 cases; Qt 6.10 runs 872 plus 356891 / 1042. Both WinUI linkages execute all five registered header/consumer/runtime/Supported gates successfully. Plan 101 is closed on this SHA; later local commits, physical interaction and broader parity are not covered by that result. Exact job links and archived evidence are in 101. Earlier pending entries below are historical.
- Framework-retirement adapters now complete shared/static ON/OFF/ON qualification: unchanged 161 assertions / 4 cases, smoke/Supported V0 2/2, shipping header/symbol absence and fresh toolkit/runtime consumers, including eleven real subprocesses per linkage. Actual retirement ordering, quarantine and native shutdown subscriptions remain production code. This is a separate local commit; the already-published Qt modal fix's CI is allowed to finish before another push. Other runtime/host helpers remain open in 106.
- Windows Qt's modal hang is reproduced and corrected: an INIT handler closed a dialog which the unconditional native modal loop then reopened. The unchanged reproduction and new closure/reuse cases pass locally; the final Qt preferences/modal/Toolbook/PropertySheet group passes 1067 assertions (one existing file-dialog early return remains unqualified), and MSW preferences pass 311 assertions. The Qt selection-restoration fixture now triggers at the actual synchronous selection boundary without relaxing its checks. The timeout/duplicate-execution workflow fix is already published; Windows Qt 5.15/6.10 remote confirmation and the other open audit lots remain required.
- Text/Search test-only storage and ComboBox force parameters are now removed from installed control declarations. Shared/static ON/OFF/ON retain **1913 assertions / 43 safe cases**, smoke/Supported V0, fresh installed consumers and strict 28-header/symbol checks. All 16 shipping wx libraries and six samples were rebuilt for the intentional pre-release Text/Search layout reduction. Runtime/host helpers and the separate component/physical gaps remain open. Windows Qt CI is currently under diagnosis in 101: the user's live logs reach `test_gui` after IPC passes, not an IPC hang.
- Latest integration batch: Qt editability and synchronous Treebook veto are fixed in `5d7b88f1b4`, verified on Qt 5.15.2 and MSW. The public test-surface migration in `7af848f5f5` covers 26 control headers, including removal of the shipping ComboBox UIA test launcher. Shared/static shipping consumers pass. Broader component runs expose existing activation/focus failures and a newly observed Calendar budget failure; the precise history and subsequent test-oracle corrections are tracked in 106/108. This is not full beta or physical sign-off.
- This batch plus TextCtrl/Calendar observation corrections is published through `c0f1d55a06`. TextCtrl passes 859/28 in each linkage; Calendar's unchanged budget passes within both range groups, whose two DatePicker focus failures remain open. Ubuntu Qt CI passes its complete 871 non-GUI and 1026 GUI cases on that SHA; both WinUI CI jobs also complete all five consumer/runtime gates successfully. Of 45 checks, 42 succeed and Windows Qt 5.15/6.10 plus AppVeyor remain unfinished.
- Notebook/Toolbar now complete the local shared/static ON/OFF/ON qualification: unchanged 3558/66 tests and both smoke/Supported V0 gates, fresh shipping consumers, 77 negative API/type probes per linkage, identical test/shipping sizes and no old installed test surface. The first OFF compilation failure is retained and corrected. This brings the migration to 28 public control headers; Text/Search seam storage, private ComboBox parameters and runtime/host testing exports remain in 106, alongside the separate component failures in 108.
- The later check of published `a984805c6b` confirms both WinUI shared/static CI jobs successful. Ubuntu Qt still fails on the two defects corrected above; Windows Qt jobs and AppVeyor were unfinished. This remote result does not qualify the newer local commits.
- 2026-08-30 baseline: clean integration worktree at c5cf4677b9; dirty main checkout preserved.
- Existing c5cf CI: MSVC C4189 requestedIndex; Clang delete-non-abstract-non-virtual-dtor; WinUI shared/static compile succeeded, Appx import failed, Supported V0 skipped. See plan 101 for precise corrective scope.
- 2026-08-30 lot 101: MSW Debug wxcore built with /warnaserror; WinUI shared/static test_gui and runtime smoke rebuilt. Both runtime CTests passed and both Supported V0 runs passed (51 cases, 970 assertions each). These are local Windows 11 results, not remote CI or physical input qualification.
- Remaining baseline jobs expose additional PropertyGrid destruction, DFB AUI ambiguity, VS2015 initialization and cross-platform test failures. They extend 101, not a new general audit. The current upstream master also conflicts with this PR; normal merge resolution is needed before remote validation.
- Lot 102: native USER32 resize transaction and cancellation tests pass in both linkages (190 assertions / 4 cases); Supported V0 now passes 1160 assertions / 55 cases per linkage. Physical held-drag behaviour is not signed off by these tests.
- Master 6155567922 is now integrated without rewriting history. After Catch2 migration, Supported V0 retains 1160 passing assertions in 90 individually registered cases in both linkages; smoke passes too. MSW targeted non-input regressions pass 323062 assertions / 117 cases. Separate PropertyGrid and three book-controller failures remain OPEN in 101; they are not hidden by these passing subsets.
- Subsequent fixes close those MSW failures: all six book families pass 471 assertions / 58 cases, and full PropertyGrid passes 1137 assertions / 2 cases. Full WinUI PropertyGrid still crashes in a retained-page destruction case in both linkages; 101 remains OPEN. Details and exact logs are maintained in 101.
- The retained-page fixture is now corrected: full WinUI PropertyGrid passes 1175 assertions shared and 1179 static. Final smoke + Supported V0 pass in both linkages (1162 assertions / 90 cases). GitHub reports no merge conflicts and CI has started on 0080db6c16; remote results remain pending. Lot 103 implementation starts against this locally verified baseline.
- Remote CI on 0080db6c16 exposes Qt compilation/selection/filesystem failures, non-MSW PropertyGrid teardown, MSW resize-notification counting, macOS callback tests and a missing Windows Server DDLM runtime registration. These remain in 101 with targeted corrections and reruns; no global green CI claim is made.
- Lot 103 passes both local linkages: 498 identity assertions, 618 assertions including existing presenter lifetime cases, and smoke + Supported V0 (1660 assertions / 92 cases). A separately observed auxiliary NativeProgress test timeout remains OPEN in 109.
- The auxiliary NativeProgress timeout is subsequently resolved by rejecting null task-dialog handles before dark-mode UIA enumeration (d1a415743f). All dialog-lifetime cases pass 353 assertions in each WinUI linkage; real TaskDialog/physical qualification remains distinct in 109.
- Lot 104 passes eleven installed-consumer subprocesses per linkage: required startup failure, clean COM rollback, quarantined failures and renderer ownership over fifty epochs. Runtime smoke and Supported V0 also pass both linkages. Remote results remain pending; concurrent cross-platform PropertyGrid regressions keep 101 OPEN.
- Lot 105 records the precise Windows/toolkit/feature macro and native-control inheritance contract. Thirty-five dispatch headers, negative fixtures and four installed compile consumers (MSW and WinUI, shared/static) pass locally. New CI steps retain external runtime/compile consumer evidence; remote execution is pending.
- Remote CI at 1cd4017b5a: both WinUI linkages successfully built, registered the pinned runtime, and executed smoke plus Supported V0 (Actions run 33316310653). Later local changes still need their own remote run; existing-port failures keep 101 OPEN.
- Lot 106 first family: Button/BitmapToggle public test methods removed; shared/static ON/OFF/ON builds, installed consumers, private-symbol absence in shipping libraries and unchanged 5328/68 real-island tests all pass. The other control families remain open. Cross-platform fixes and lots 104/105 were published through b6dd49c629 for new remote CI; no physical readiness claim is made.
- Lot 106 next six controls (StaticText/StaticBitmap/StaticBox, Gauge/ScrollBar/SpinButton) pass shared/static ON/OFF/ON, shipping symbol/header checks and fresh installed consumers. Their unchanged combined suite passes 5810 assertions / 79 cases per linkage. The remaining twenty public control headers and host helpers are still open. CI at 445f908e8d passes style checks but reveals further Qt/Cocoa runtime failures; the GLib/GCC compilation repairs are recorded separately in 87841e7932.
- Both WinUI jobs subsequently pass on 445f908e8d, including the installed runtime and toolkit consumers, runtime smoke and Supported V0. Lots 103-105 now have local and remote evidence; this is not a global CI pass while the Qt/Cocoa findings in 101 remain open.
- The follow-up CI repairs through 53394a7755 pass full PropertyGrid and filesystem tests on local MSW, WinUI shared/static and Qt 5.15.2; GTK3 compiles/links, and both WinUI smoke/Supported V0 gates pass. Qt's real short-TMP reproduction includes a Unicode file with a 269-character parent. Compiler/runtime confirmation on the other remote platforms is still required; exact commits and logs are in 101.
- CI at 8fd4d2426b confirms macOS ARM ASAN, MSW 32-bit GCC 14/Wine and Fedora GCC 16.2 compilation/non-GUI tests. Qt exposes further failures; the two auxiliary-window cases and Toolbook crash are repaired locally, but the full property-sheet case remains open. See 101 for exact scope, job identifiers and runtime limitations.
- Lot 106 adds RadioBox, ActivityIndicator, Hyperlink and ColourButton to the eight migrated controls. Shared/static ON/OFF/ON, fresh shipping consumers, symbol/header checks and unchanged 6075/95 non-input tests all pass. Sixteen other public control headers and private host/helpers remain open; no physical input qualification is inferred.
- InfoBar and StatusBar bring the locally qualified private-access migration to fourteen controls: shared/static ON/OFF/ON, unchanged 1240/31 tests, fresh shipping consumers and no installed test surface all pass. The Qt follow-up suite passes 1610/28; MSW's newly reached text-only toolbar defect is corrected with native mixed-image regression coverage, and full Toolbook/PropertySheet passes 746/12. The missing autotools header is verified by installed-header compilation. These fixes still require their own remote CI results; fourteen control headers and host/helpers remain in 106.
