# wxWinUI spike diagnostics

`winuispike` is the focused validation harness for the shared WinUI island.
Its expensive probes are disabled by default: a normal run installs no message
hooks, samples neither `GetCursorPos()` nor `WindowFromPoint()`, creates no
active `ProgressRing`, subscribes to no render callback, and writes no
diagnostic file. The normal 400 ms timer still animates and repaints the
wanderer; that visual workload is part of the harness rather than a probe.

## Instrumented run

Build the target, then use the runner so every trace shares one run directory:

```powershell
cmake --build build-winui-clean --config Release --target winuispike -- /m:4 /nr:false
samples\winuispike\run-instrumented.ps1 -BuildDir build-winui-clean
```

The default binary profile is `lib\vc_x64_dll`. Pass `-BinaryDir` for another
configured architecture/output profile. An explicit `-OutputDir` must be new
or empty so traces from separate binaries/runs cannot be mixed. The runner has
a bounded 30-minute watchdog by default; select another bound with
`-TimeoutSeconds`. On expiry it requests a normal close for five seconds, then
terminates the child. Exit `124` means the watchdog expired and cleaned up;
exit `125` means forced cleanup failed and the PID remains in the manifest.
Unexpected runner failures use the same bounded cleanup path before returning
exit `1`; they cannot silently orphan a started child.

The runner sets these variables directly on the child `ProcessStartInfo`; the
calling PowerShell process environment is never modified:

- `WX_WINUI_DIAGNOSTICS=1`;
- `WX_WINUI_SPIKE_DIAGNOSTICS=1` (compatibility alias);
- `WX_WINUI_DIAGNOSTICS_DIR=<run-dir>`;
- `WX_WINUI_INPUT_LOG=<run-dir>\input-router.txt`.

To launch manually, set the same variables before starting the executable.
The expensive `winuispike` probes accept only `1`, `true`, `on`, or `yes`
(ignoring surrounding whitespace and case) as an activation value. Empty,
zero, false, misspelled, and all other values stay disabled. For these spike
probes, `WX_WINUI_DIAGNOSTICS_DIR` alone has no effect; once a flag is enabled,
the directory is mandatory and the harness fails closed instead of creating an
untracked fallback under `%TEMP%`.

The core-library crash logger has a deliberately different, directory-only
contract. When wxWidgets was built with `wxUSE_WINUI3_DEBUG_LOG=1`,
`wxWinUIDebugLog()` always writes to the debugger and additionally appends to
`<WX_WINUI_DIAGNOSTICS_DIR>\winui-debug.txt` whenever that variable names an
existing writable directory. It does not inspect either spike activation
flag, and does not create the directory. Without it the optional file logger
remains debugger-only.
The cursor-target probe uses the same all-or-nothing activation: it runs only
when a diagnostics flag is set and `spike-ticks.txt` has opened successfully.
The runner waits, within its watchdog bound, for the sample to exit; invoke the
collector from a second PowerShell terminal while reproducing a freeze.
Both the per-run and active-run manifests are published by an atomic
same-directory replacement, so a concurrent collector sees either the old
complete JSON or the new complete JSON. `StartedAt` is taken from the retained
child `Process.StartTime`, not from a clock sample after launch.

All explicit relative paths follow one rule: `BuildDir` is relative to the
repository root, while `BinaryDir` and `OutputDir` are relative to that resolved
build. Each tool holds an atomic `CreateNew` lock for its complete campaign and
removes it in `finally`. All producer runners share one campaign lock, so
different probes cannot publish into the same directory concurrently. The
collector uses a separate collector lock so one collector can still snapshot a
live instrumented run, while two collectors cannot write it concurrently.

### Default-off policy gate

The existing self-test also provides a non-interactive negative gate. With both
activation flags absent, it must exit `20` before creating its message-only
test window, installing a hook, or opening the diagnostics directory:

```powershell
$exe = (Resolve-Path `
    'build-winui-clean\lib\vc_x64_dll\winuispike.exe').Path
$probeDir = Join-Path $env:TEMP `
    ('wx-winui-default-off-' + [guid]::NewGuid().ToString('N'))
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe
$start.Arguments = '--diagnostics-self-test'
$start.UseShellExecute = $false
foreach ($name in @(
    'WX_WINUI_DIAGNOSTICS',
    'WX_WINUI_SPIKE_DIAGNOSTICS',
    'WX_WINUI_INPUT_LOG'))
{
    [void]$start.EnvironmentVariables.Remove($name)
}
$start.EnvironmentVariables['WX_WINUI_DIAGNOSTICS_DIR'] = $probeDir
$process = [Diagnostics.Process]::Start($start)
if ( !$process.WaitForExit(15000) )
{
    $process.Kill()
    throw 'default-off diagnostics gate timed out'
}
if ( $process.ExitCode -ne 20 -or (Test-Path -LiteralPath $probeDir) )
{
    throw "default-off diagnostics gate failed (exit $($process.ExitCode))"
}
```

Exit `20` is intentional here: it is the self-test's fail-closed result for an
explicitly disabled probe, while the missing directory proves that even an
output path alone cannot enable logging.

## Headless diagnostics self-test

The sent-message probe has a deterministic, non-interactive gate:

```powershell
samples\winuispike\run-instrumented.ps1 `
    -BuildDir build-winui-clean `
    -ArgumentList '--diagnostics-self-test' `
    -TimeoutSeconds 15
```

It creates only a message-only HWND and installs only
`WH_CALLWNDPROC`/`WH_CALLWNDPROCRET`—never `WH_GETMESSAGE`. A controlled slow
`SendMessage` must be published by the return hook while the normal 250 ms
batch window is still open. Success is exit `0` and a timestamped
`selftest: PASS` line in `spike-ticks.txt`. Self-test exits `20`–`29` identify,
respectively, disabled diagnostics, log/class/window/hook setup failures,
unexpected dispatch, missing publication, missing forced sub-250 ms flush, a
missing publication timestamp, or failure of the pure activation-token matrix.
The test neither moves the pointer nor activates a foreground window.

Every queue, sent-message, render, and self-test record begins with the
`GetTickCount64()` timestamp from the instrumented process, so separate probe
families can be correlated without depending on wall-clock changes.

## Deterministic root-pointer campaign

The phase-007a input gate is separate from freeze instrumentation. It starts a
fresh process for each repetition and requires a new or empty output
directory:

```powershell
samples\winuispike\run-input-scenarios.ps1 `
    -BuildDir build-winui-clean `
    -Iterations 100
```

Each process creates one borderless TLW at a position calculated beyond the
current virtual-desktop rectangle, adds `WS_EX_NOACTIVATE`, shows it with no
activation, and exits automatically. If no representable position exists, the
setup fails closed instead of truncating the signed 16-bit coordinates packed
in native mouse-message `LPARAM`s. It does not call `SendInput`, query or move
the cursor position, request foreground/focus, install diagnostic hooks, or
start the interactive spike timer. Normalized samples pass through
`wxWinUITopLevelHost::TestRouteRootPointerSample()` and the native target
counts the actual synchronous messages emitted by the production adapter.

Nine scenarios run in a fixed order:

1. 128 `MOVE`s at one physical pixel;
2. 128 `MOVE`s alternating across two pixels;
3. client → non-client → client hit zones;
4. native → XAML-owned → native logical crossings;
5. actual root `EXIT`/`ENTER` through the post-normalization boundary seam;
6. a hit-zone mutation during `WM_SETCURSOR`, rejected by the final refresh;
7. nested routing during `WM_SETCURSOR`, which must supersede the stale outer
   route;
8. a balancing release outside the TLW while the native target owns capture;
9. native capture loss followed by a late release, which must cancel exactly
   once and never fabricate an `UP`.

Scenarios 1 and 2 prove that an ordinary, unarmed input path delivers both
repeated same-pixel samples and real two-pixel motion without suppression. The
armed storm-breaker state machine is proved separately by the deterministic
`[winui-input-state]` gate; this process campaign does not claim to arm and
re-prove that pure-state matrix.

The last two scenarios use real USER32 capture for a few synchronous
instructions, but still inject no physical input and never activate the
window. As with any capture test, run the 100-process campaign in an announced
short idle window if another person is actively using the desktop.

`result-NNNN.json` records outcome, action, hover, hit-test, message, boundary,
and interruption counts. The executable refuses to overwrite a result and
publishes it atomically. Its exit codes are `30` invalid CLI, `31` window
setup, `32` host setup, `33` failed scenario, `34` internal timeout, `35`
result publication, and `36` unhandled exception.

The runner adds a harder per-process watchdog around the synchronous adapter
path, terminates through the retained `Process` object on expiry, and verifies
that exact child is gone before continuing. The outer watchdog must leave at
least five seconds beyond `-ScenarioTimeoutMs` for process/XAML startup and
atomic result publication. The runner requires the nine scenario records and
their names in the documented order; the summary counter alone is not trusted.

`summary.json` records the executable SHA-256 immediately before and after
every iteration, the raw result SHA-256, and a semantic SHA-256 that excludes
PID/timing fields. Any per-iteration binary drift stops the campaign. All
passing runs must have one identical semantic hash. Runner exit codes are `0`
pass, `1` scenario/result failure, `2` runner or binary-drift failure, `124`
timeout with successful cleanup, and `125` cleanup failure (the exact PID is
retained in the summary).

This gate begins after the XAML event has already been normalized. It proves
the production routing/boundary/interruption cores, but deliberately does not
claim to test WinRT `OriginalSource` inspection or `PointerPoint` property
extraction. Those remain covered by adapter review and the final physical
gate, not by a second simulated implementation in the sample.

## Z-order, clip and splitter scenarios

Phase 007c has a separate input-free process gate:

```powershell
samples\winuispike\run-zorder-scenarios.ps1 `
    -BuildDir build-winui-clean
```

By default the evidence is written below
`<BuildDir>\logs\winui-zorder-scenarios`. A relative `-OutputDir` is resolved
against `BuildDir`, keeping generated evidence outside the source tree.
`BinaryDir` is likewise build-relative and is rejected if it escapes that
build. A fresh output directory is exclusively locked for the whole campaign.

The child uses one off-screen `WS_EX_NOACTIVATE` window. It never reads or
moves the pointer and runs no interactive spike timer. In a fixed order it
checks XAML siblings against real USER32 order before and after
`Raise()`/`Lower()`, records the unrepresentable `XAML > native > XAML`
signature, validates ancestor/scroll/hidden/TabView clips, and exercises
vertical/horizontal splitter geometry while synchronously dispatching its
light/dark paint paths. Pixel appearance remains an explicit manual gate.

`result.json` is atomically published and `summary.json` binds it to the
executable SHA-256. Exit `0` passes; exit `1` is a scenario/schema failure;
`124` is watchdog expiry with successful cleanup; `125` means exact-child
cleanup failed. The result lists the visual High Contrast, physical DPI,
minimized-window and splitter-input checks which remain manual.

The alternating-topology scenario resets the production host diagnostic
counter, establishes the native `A>N>B` signature, flushes the shared host,
and requires exactly one impossible-topology diagnostic. The runner rejects a
missing, duplicated or simulated diagnostic.

## Scroll-to-render latency

The QPC campaign is also opt-in and independent of the general freeze probes:

```powershell
samples\winuispike\run-scroll-latency.ps1 `
    -BuildDir build-winui-clean `
    -TargetHz 60 `
    -Samples 24
```

By default the evidence is written below
`<BuildDir>\logs\winui-scroll-latency`. A relative `-OutputDir` is resolved
against `BuildDir`. `BinaryDir` must resolve below the same build, and the
fresh output directory remains exclusively locked for the whole campaign.

Each invocation launches one fresh process for the selected 60, 120 or 165 Hz
profile. Change the display mode, then archive a separate invocation for each
profile; the runner never relabels one display cadence as three profiles. Each
process first waits for four rendering callbacks and performs one unmeasured
scroll batch. This fixed warm-up realizes templates and primes the compositor;
it is never selected or discarded according to its timing. Each measured
sample then batches four real `wxScrolledWindow::Scroll()` mutations, observes
the final geometry only when the production slot commits `GetRectInTLW()`, and
records the first following `CompositionTarget.Rendering`. It therefore
measures a conservative mutation → committed slot → render bound without a
second implementation of `SyncSlot()`.

The gates are p95 mutation-to-slot no greater than one target period, p95
mutation-to-render no greater than two target periods, no stale result after
250 ms, backlog at most four, and at most one newly scheduled coalesced flush
per four-mutation batch. The observed render cadence is gated within ±2%,
with a 1 Hz minimum tolerance. Schema v2 reports:

- `pass` and exit `0` only when cadence and all target budgets qualify;
- `unqualified` and exit `2` when integrity, slot and cadence-relative safety
  pass but the observed cadence does not match the target;
- `fail` for technical/structural failures, a slot miss, a render delay beyond
  twice the larger of target period and observed median period, or a matching
  cadence that misses the target render budget.

The JSON includes the raw render intervals. The PowerShell runner requires
strict JSON types and independently recomputes every sample, p95, cadence,
budget, safety gate and final status. Off-screen throttling therefore cannot
be mistaken for either a pass or a target-budget regression.

The runner retains the exact `Process` object in a `try/finally` cleanup path.
Every profile record includes the PID and any cleanup failure records an
actionable `orphan_process_id`; the campaign summary also lists all orphan
PIDs. Runner watchdog values up to 120 seconds are supported while the
in-process timeout is bounded to the sample's 60-second CLI maximum.

These modes are disabled by default. Their runners explicitly remove
`WX_WINUI_DIAGNOSTICS`, `WX_WINUI_SPIKE_DIAGNOSTICS`,
`WX_WINUI_DIAGNOSTICS_DIR` and `WX_WINUI_INPUT_LOG` from each child. The
architecture decision and result table live in
`docs/winui/adr/0003-scroll-sync-and-zorder.md`.

## Freeze snapshot

While an instrumented sample is still running:

```powershell
samples\winuispike\collect-freeze.ps1 -BuildDir build-winui-clean
```

When exactly one process started by the runner is active, the collector reads
its `active-run-<pid>.json` manifest and appends the snapshot to that same run
directory. The manifest binds the PID to the executable path, build directory,
and SHA-256; processes with the same name from another build are never
attached. `-OutputDir` remains available for manual launches or concurrent
runs. In manual mode, pass `-ProcessId` when more than one matching sample is
running; the collector refuses to guess. `-BinaryDir` selects a non-default
architecture/output profile.

The collector records GUI thread focus/capture state, captures bounded,
non-invasive CDB stacks for `widgets`, `showcase`, `winuispike`, and
`dataview`, and gathers the known input/render logs. It discovers `cdb.exe`
from `PATH` or the Windows Kits installation; pass `-Debugger` to select
another copy. Its symbol cache lives under the chosen output directory, never
in a machine-global hard-coded path.

Automatic debugger discovery selects the Windows Kits x64 CDB used by the
qualified profile. For an x86 or ARM64 `-BinaryDir`, pass the matching
architecture's `cdb.exe` explicitly with `-Debugger`. A timed-out debugger is
terminated through its retained `Process` object and verified gone; if that
cleanup fails, its PID is reported, no further debugger is launched, and the
collector exits `125`. Immediately before every attach, executable path and
the exact UTC ticks of `Process.StartTime` are revalidated so a recycled PID
can never be accepted on name alone.

If CDB is unavailable, collection remains useful and explicitly reports that
only GUI state and logs were captured.

Without an active runner manifest, `-OutputDir` selects manual compatibility
mode: only matching executables from `-BuildDir` are inspected, and legacy
input logs are searched in that build and the selected output directory.
