# wxWinUI Supported V0 beta-candidate automation record (2026-08-14)

## Verdict

**BLOCKED -- this record does not qualify a production-ready beta.**

The fresh shared and static Release profiles passed the narrow automated
`Supported V0` gate, its exact XRC inventory, runtime/self-tests, installed
package relocation consumers, the complete `test_base` regression suite, the
focused drawing regression and the non-physical WinUI keyboard suite. Fresh
corrected shared and static process-isolation soaks also completed for at
least 60 minutes each. These soaks are supplemental automation only and do
not sign the manual soak row.

The 60 Hz latency qualification is still red, physical keyboard injection has
not been validly exercised, and the human
[V0 manual qualification checklist](v0-manual-qualification.md) is unsigned.
Those are release gates, not optional follow-up work.

This record applies only to the inventory selected by
`[winui-v0-supported]~[.]~[physical]` and described in the
[V0 component matrix](../../plans/winui3-v0/component-matrix.md). It does not
promote any Experimental, Fallback or untagged component, and it is not a
claim of full wxMSW parity.

| Gate | Result | Release interpretation |
|---|---:|---|
| Fresh shared and static Release builds | PASS | Necessary build evidence only |
| Supported V0 component gate | PASS | Final profiles: 50 cases / 927 assertions each, WER 0 |
| Exact Supported V0 XRC gate | PASS | Final profiles: 1 case / 322 assertions each, WER 0 |
| Runtime/self-tests and installed relocation consumers | PASS | Both linkages exercised |
| Complete `test_base` suite | PASS | Both linkages, 473 cases / 1,163,111 assertions |
| Non-physical WinUI keyboard suite | PASS | Both linkages, 16 cases / 417 assertions, WER 0 |
| Focused drawing v2 regression | PASS | Both linkages; console-observed, no immutable archived log |
| Supplemental automated Supported V0 soaks | PASS after test-barrier correction | Corrected shared 228/228 and static 228/228; does not replace manual soak |
| 60 Hz latency campaign | **FAIL** | Must be repeated at a qualified physical 60 Hz cadence |
| Physical keyboard/accelerator input | **BLOCKED** | Isolated desktop was not the active input desktop |
| Full general `test_gui` | INCOMPLETE | Timed out with failures outside the Supported V0 gate |
| Human UI/accessibility/DPI/manual-soak qualification | NOT RUN | Automated soaks are supplemental only; checklist remains unsigned |
| Reproducible release source snapshot | **NOT MET** | Evidence was produced from a large dirty worktree |

## Candidate identity and reproducibility boundary

The test session was based on Git `HEAD`
`2c82ad3d302f99f18ac772c8d2ee703d35f5b089`, but it did **not** test that
commit alone. At the documentation capture time (`2026-08-14T01:45:08+02:00`)
the shared worktree reported 699 porcelain entries: 362 tracked changes and
337 untracked paths. Other qualification work continued concurrently. No
complete, immutable patch plus untracked-file manifest was captured before
the runs.

Consequently, the results below identify a candidate worktree and selected
artifacts, not a reproducible source release. A release decision requires
freezing the intended sources in a reviewable commit (or an equivalent
content-addressed manifest), rebuilding from that frozen snapshot, and
archiving the rerun logs.

A later session-only dependency-closure audit found 396 dirty files actually
read by the fresh shared/static builds (312 tracked diffs and 84 untracked
files). No immutable audit artifact was archived. The 119-file soak manifest
is therefore a mutation detector for the Supported gate, not a complete
reconstructible-source identity.

Selected source identities used to anchor this engineering session are:

| Path | SHA-256 |
|---|---|
| `src/winui/combobox.cpp` | `A0F7BAA46FA28E0B87BE29231B99CEB9AA6346A7B1F5DC9792EFAD7E3B0BA0A9` |
| `src/winui/choice.cpp` | `A96CAC178D39BFEBA0D45ADE4585610E960B2376ACFFB544201E042002D9CEAE` |
| `src/winui/winui.cpp` | `B193E1B6D4C089E8EE5FC0EF1DF80D8CF1C46045EB6895B3C2AF634B85D2F8BC` |
| `src/winui/tlwhost.cpp` | `93373C9A5CE3BE149ED7456DEBC4663730BA1800B90D3300BCC6B58705566A39` |
| `tests/xml/xrctest.cpp` (current corrected test) | `7D37D1738F77A9117E2A446429FBC9EF850622167FABAC5C029EEBEE8CBC8D19` |
| `tests/events/winuikeyboard.cpp` | `B5541F65265D00C9DD15984849B7C4DD4F04C57F6CB695D178FFD2F72AE92127` |
| `tests/winui/run-supported-beta-soak.ps1` | `9AF4481656758E53B931EEDD491A480371302A6A185DCF4B9F319443DFBF3FBF` |

These hashes are deliberately not presented as a whole-worktree digest.

## Fresh build profiles

Both builds used the `Visual Studio 17 2022` generator, platform `x64`,
configuration `Release`, toolkit `winui`, UTF-8 mode off and monolithic mode
off. NuGet packages were rooted at `F:\wqp`; the deployed Windows App SDK
runtime was `1.8.260710003` and WinUI package version was `1.8.260709004`.

| Setting | Shared profile | Static/no-exceptions profile |
|---|---|---|
| Build tree | `F:\wqsh` | `F:\wqst` |
| Relocated install | `F:\wqshr` | `F:\wqstr` |
| Out-of-tree consumer | `F:\wqshc` | `F:\wqstc` |
| `wxBUILD_SHARED` | `ON` | `OFF` |
| `wxUSE_EXCEPTIONS` | `ON` | `OFF` |
| `wxBUILD_TESTS` | `ALL` | `ALL` |
| `wxBUILD_SAMPLES` | `ALL` | `OFF` |
| `wxBUILD_INSTALL` | `ON` | `ON` |

Complete Release builds succeeded in both trees. The build console output was
observed during the session but was not preserved as an immutable release log;
this is another reason the final frozen-snapshot rerun is required.

## Passing automated evidence

All WER inventories scoped around the final component, XRC, keyboard and soak
commands below found zero new crash reports. The drawing v2 pass was observed
from the console and checked against the process WER inventory, but no
content-addressed immutable drawing log was archived.

| Exercise | Observed result |
|---|---:|
| Runtime probes and self-tests | 9 shared exercises and 8 static exercises passed |
| Final `wx_winui_supported_beta` CTest | Passed in each profile; 50 cases / 927 assertions in the underlying gate, WER 0 |
| Exact `[winui-v0-supported][xrc]~[.]~[physical]` inventory | 322 assertions passed in each profile, WER 0 |
| `[WinUIKeyboard]~[physical]` | 16 cases / 417 assertions passed in each profile, WER 0 |
| Focused drawing v2 regression | 1 case / 2 assertions passed in each profile |
| Historical ComboBox regression pair | 2 cases / 59 assertions passed |
| Template seam selection | 5 cases / 1,082 assertions passed |
| Item-model popup retirement | 6 cases / 67 assertions passed |
| Host lifecycle popup gate | 8 cases / 103 assertions passed |
| Notebook shutdown | 4 cases / 161 assertions passed |
| `[winui-009]~[.]~[physical]` | 73 cases / 3,271 assertions passed |
| Cross-top-level reparent | 1 case / 406 assertions passed |
| Drop broker/session and Tree drag | 20 cases / 888 assertions passed |
| Relocated shared installed-package consumer | PASS in 27.03 s |
| Relocated static installed-package consumer | PASS in 21.45 s |
| Complete shared `test_base` | 473 cases / 1,163,111 assertions passed |
| Complete static `test_base` | 473 cases / 1,163,111 assertions passed |
| Corrected shared Supported V0 soak | 228/228 fresh processes over 3,601,621 ms; no failure, timeout, cleanup fault or WER |
| Corrected static Supported V0 soak | 228/228 fresh processes over 3,603,181 ms; no failure, timeout, cleanup fault or WER |

The relocated package scans found no absolute source-tree or original
build-tree paths. The resulting `test_base.exe` identities were:

- shared: `11D696FFEC1556E26ACCA4688433BA7888FCA2EB799AC2CFB4889BA5EC5E8940`;
- static: `069206B798CB665D2C522E1C31D51F1B531F1FC297953F8DFE27D5698140ACD6`.

The final corrected Supported/XRC gates and soak campaigns used:

- shared `test_gui.exe`:
  `46BF159A7EC79EA427BEE947680547B22111848507CC0B9DC1076F09C0010A00`;
- static `test_gui.exe`:
  `608ED14F7B716E2E8AEF3CCCE5CDCB146E6F97352F66AD7865759340CF6BD913`;
- shared desktop runner:
  `48E509F0ECC0CFE9C826886F6B549E59D7C993E938A0061E2386465C18B03C5C`;
- static desktop runner:
  `C981FC743CCE2B48065969C9ADFF5BB8F270050A3D3B643E00643376673E53CF`.

The canonical component-gate rerun command for either frozen build tree is:

```powershell
ctest --test-dir F:\wqsh -C Release `
  -R "^wx_winui_supported_beta$" --output-on-failure
ctest --test-dir F:\wqst -C Release `
  -R "^wx_winui_supported_beta$" --output-on-failure
```

For exact XRC reruns in both profiles:

```powershell
& F:\wqsh\winui\test-tools\Release\wx_winui_desktop_test_runner.exe `
  --timeout-ms=240000 -- `
  F:\wqsh\lib\vc_x64_dll\test_gui.exe `
  "[winui-v0-supported][xrc]~[.]~[physical]"
& F:\wqst\winui\test-tools\Release\wx_winui_desktop_test_runner.exe `
  --timeout-ms=240000 -- `
  F:\wqst\lib\vc_x64_lib\test_gui.exe `
  "[winui-v0-supported][xrc]~[.]~[physical]"
```

### Focused drawing v2 evidence

The focused drawing regression passed in both profiles, with 1 test case and
2 assertions per profile. The exercised executables were:

- shared:
  `9288D8D94A76592B8EB6714077B66340926D6B371AB2959C5E5ED98E9A98BCB9`;
- static:
  `8C11C2AFD8FD62A1845E80201DBF5006305F1448CEBEC732255ACC7B99018580`.

The generated PNG identities observed during this work were:

- `Basics`: `9261A5DCA4847A3C4F1F27A31C6D0F5F09DD2BBFE532A672268A2185AC6C8B59`;
- `Fonts`: `4605E2908E35067C200FF21FF6DDDD3BD4B6839B6040A01A478D60F80EE34641`.

This was console-observed supporting evidence. No immutable qualification log
was archived, so it is not presented as a release-frozen regression record.

### Supplemental automated fresh-process soaks: retained history

These campaigns are supplemental automation only. They do not sign the
physical/manual 60-minute checklist row and do not change this document's
`BLOCKED` verdict.

The original shared campaign at
`F:\wqsh\qualification\20260814-a0f7-b193\supported-v0-soak-shared-60m`
passed 226/226 fresh processes in 3,601,735 ms, with no timeout, cleanup
failure or WER. Its `summary.json` SHA-256 is
`A9BD69DFD2C35A32CAD708F89F7B254342B1F5D38CF1AB0382A53B451F91877A`.
It predates the corrected XRC observation barrier and remains historical
runtime-stress evidence, not the final corrected-test campaign.

The first static campaign at
`F:\wqst\qualification\20260814-a0f7-b193\supported-v0-soak-static-60m`
is retained as a failed result. It passed 74 iterations, then failed closed at
iteration 75 because the XRC test observed one live Notebook callback-state
object after teardown instead of baseline zero. Cleanup succeeded and no WER
dump was created. Its `summary.json` SHA-256 is
`70541B318C7D30ABBC437DEB1427C1B3B16EC2B94E14092665354394AD4830FD`;
the failing stdout SHA-256 is
`7F4B58505FB92B91621484FEDB17846B5B8B85C9ED2CDD5ECB54B6DC1F294F0C`.
This result is neither deleted nor relabelled as a pass.

The failure exposed an insufficient temporal observation barrier in the XRC
test, not a stale callable owner: wxNotebook invalidates the owner before
retaining its peer graph for two low-priority dispatcher turns. Among the 119
files in the soak mutation manifest, only `tests/xml/xrctest.cpp` changed,
from
`12CCFD2C0F95F7B155192CE11B52E7B0312C426FC44818FE00826A6E819FF000`
to
`7D37D1738F77A9117E2A446429FBC9EF850622167FABAC5C029EEBEE8CBC8D19`.
The corrected test waits for the pending-peer, live-callback and framework
retirement counters to return causally to baseline. Runtime sources and the
soak harness remained bit-identical.

The corrected final campaigns passed:

- shared:
  `F:\wqsh\qualification\20260814-a0f7-b193\supported-v0-soak-shared-60m-rerun1`,
  228/228 in 3,601,621 ms; `summary.json`
  `9C5A7597F97EC642DB380AD288B43C69EFFBCC15843410EC2263737BCCB05DDE`,
  `summary.txt`
  `25BAA26C1489094BE143B0D533899C650F45A0A79AC5D579C9996181F5208203`;
- static/no-exceptions:
  `F:\wqst\qualification\20260814-a0f7-b193\supported-v0-soak-static-60m-rerun1`,
  228/228 in 3,603,181 ms; `summary.json`
  `EF720E0CFB5BE4F8567F660C544895BEC27250C10986FFEE606A2ED2A2C519E3`,
  `summary.txt`
  `D59C4A3AB78EADA2F3B2DD95CDD01CA238D362749074EB4450F2951A4BCBDBF1`.

Both corrected campaigns observed exactly 50 cases and 927 assertions per
iteration, zero failures/timeouts/cleanup faults/WER dumps, and stable
runner, `test_gui`, harness and 119-file manifest identities from start to
finish. Their common corrected manifest SHA-256 is
`E45F5B47B404D58C0C5DBE015C7419EE6AD2401EA0A95E5601C28BDE1EFB588C`.

### Persisted spike evidence

The diagnostic self-test reported a 68 ms sent-hook interval and `PASS`. Its
persisted artifacts are:

- `F:\wqsh\qualification\20260814-a0f7-b193\diagnostic-selftest\run-manifest.json`,
  SHA-256 `842531FA79FC000264B4D58424A85F52155453627EFBD9F00E61F92F1B89E96A`;
- `F:\wqsh\qualification\20260814-a0f7-b193\diagnostic-selftest\spike-ticks.txt`,
  SHA-256 `00FCC4393D82E17D5E196F9A21F84DE65C88B5973AD6E1910067AE6B7FE5150B`.

The input campaign completed 100/100 fresh processes with no failure,
timeout or orphan. Its executable stayed at
`B48C99C4D0B541544440AC01777EDB14C5CF247DEF0B7725543CD0E1FCD21DDB`.
The durable summary is
`F:\wqsh\qualification\20260814-a0f7-b193\input-100\summary.json`,
SHA-256 `4DD3C179884A42464FB7E39254E9F5CBE2FDC589F28B8C19B1F0C230393A6468`;
its campaign `AggregateSha256` is
`08C07442A2937D5709B869240371D4F91994C3EA7E5EB078B6730CBF851BC8D1`.

The Z-order/clip/splitter campaign passed. Its summary is
`F:\wqsh\qualification\20260814-a0f7-b193\zorder\summary.json`,
SHA-256 `2EA00041932FCDBB20F282B6B00E56D171EAFA3063867F48F172B7DEE80F7C79`;
the referenced result hash is
`93D40F08B71DD956EE00E50CF35ECA279C4597FA14994DCF396EFC8E12C5B905`.

## Red and unresolved evidence

### 60 Hz latency qualification: FAIL

`F:\wqsh\qualification\20260814-a0f7-b193\latency-60\summary.json`
(SHA-256
`5003BD769E3BEE7123CAE29465226DEC92F5B19A5E1857DB27BE03CB403074D7`)
records exit code 43 and `qualification_passed: false`. The result file hash is
`BDE82C233B498021B701669A46572C0A37F3C8DF6103F23CE073E339520A3739`.

The requested target was 60 Hz, but the virtual/off-screen session produced
only 32.870 Hz. Slot p95 was 20.766 ms against a 16.667 ms target-period
budget, and render p95 was 33.935 ms against a 33.334 ms two-period budget.
Structural gates and the safety budget relative to the observed cadence did
pass. That makes the run useful diagnostic evidence, but neither a product
regression verdict nor a qualification pass. The absolute budgets must not be
weakened to fit this environment.

Repeat it with the screen on, outside RDP, on an unlocked local physical 60 Hz
display, and require an observed cadence from 58.8 through 61.2 Hz:

```powershell
& C:\wxWidgets-master\samples\winuispike\run-scroll-latency.ps1 `
  -BuildDir F:\wqsh `
  -BinaryDir lib\vc_x64_dll `
  -OutputDir F:\wqsh\qualification\20260814-a0f7-b193\latency-60-rerun-local `
  -TargetHz 60 -Samples 24 -TimeoutSeconds 30
```

Use a new output directory for every rerun and preserve both JSON files.

### Physical keyboard input: BLOCKED

The non-physical `[WinUIKeyboard]~[physical]` suite is green in both profiles
at 16 cases / 417 assertions, with WER 0. This validates the deferred-focus
fixture and keyboard-routing logic without validating OS-level physical input
injection.

The `[WinUIKeyboard][physical]` attempt failed three assertions: the expected
accelerator count remained zero and injected text remained empty. The desktop
runner created an isolated, non-active desktop and never switched it to the
input desktop, while `keybd_event` delivered input to the active desktop.
Therefore this attempt did not validly exercise the target windows and cannot
be used either as a product failure or as a pass.

A release run must use a deliberately opt-in, guarded active-desktop path on
an idle local session, verify that the target desktop is the input desktop,
and archive the result. Switching desktops affects the interactive user and
must not be done implicitly.

### General GUI suite: incomplete

A broad default `test_gui` run timed out at 180 seconds without a new WER
report. It also exposed failures in simulator-driven input and in components
outside the positive Supported V0 inventory, including Experimental/Fallback
Grid and DataView paths. This does not invalidate the exact positive gate, but
it means there is no honest all-GUI green claim. Before sign-off, every such
failure must be rerun in a suitable environment and either fixed or mapped to
an explicit excluded matrix row and an owning issue/plan.

## Remaining production-beta gates

All of the following are required before changing the verdict:

1. Freeze the intended source snapshot, rebuild both profiles from it, and
   archive configure/build/test logs plus a complete source identity.
2. Rerun the exact Supported V0 and XRC gates on those frozen binaries in both
   shared and static/no-exceptions profiles.
3. Obtain a green 60 Hz latency campaign on a qualified local physical display
   without changing its budgets.
4. Obtain a valid physical keyboard, text-entry, accelerator and mnemonic run
   on the active local input desktop.
5. Complete every row of the
   [manual checklist](v0-manual-qualification.md), including sample smoke,
   supported component pages, transient UI, theme/high contrast/RTL,
   multi-monitor DPI, cross-top-level reparent and the full 60-minute soak.
   The two automated fresh-process soaks do not satisfy this manual row.
6. Attach and review an Accessibility Insights topology snapshot, and have a
   human operator sign the Narrator pass.
7. Disposition the broad GUI-suite failures against the component matrix and
   record an owner for every remaining excluded failure.
8. Only after all preceding evidence is green, sign the qualification verdict
   and separately review any public README or matrix status change.

Until then, the accurate state is **Supported V0 automation candidate with
passing core/package evidence and explicit release blockers**, not
**beta production ready**.
