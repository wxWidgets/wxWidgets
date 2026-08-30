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
| [101](101-ci-baseline.md) | Restore existing-port builds and execute the WinUI runtime CI | OPEN: REMOTE FAILURES UNDER REPAIR |
| [102](102-native-resize.md) | Make native resizing a host-owned input transaction | NATIVE LOCAL PASS / PHYSICAL PENDING |
| [103](103-dialog-identity.md) | Preserve public dialog identity and geometry in Window presentation | LOCAL PASS / REMOTE PENDING |
| [104](104-runtime-ownership.md) | Define runtime startup failure and ownership of process effects | TODO |
| [105](105-toolkit-contract.md) | Make toolkit identity and public source compatibility explicit | TODO |
| [106](106-private-test-surface.md) | Move testing facilities out of installed public control APIs | TODO |
| [107](107-host-modules.md) | Split host responsibilities behind private ownership boundaries | TODO |
| [108](108-component-contracts.md) | Close component parity gaps and version template dependencies | TODO |
| [109](109-real-integration.md) | Qualify native input, modal loops, UIA and OLE on real surfaces | TODO |
| [110](110-performance-soak.md) | Measure pointer performance and lifecycle stability with reproducible budgets | TODO |
| [111](111-capability-manifest.md) | Keep support documentation and feature evidence synchronized | TODO |
| [112](112-submission-series.md) | Prepare reviewable submission boundaries without rewriting the integration branch | TODO |

Execute one implementation lot at a time. A remote build may run while the next independent lot is prepared, but its predecessor remains pending until the result is known. No repetitive whole-repository re-audit between lots. Each defect receives one focused verification; new failures expand the same lot rather than starting an unrelated initiative.

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
