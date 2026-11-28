# ADR 0003: Coalesced slot synchronization and z-order qualification

- Status: Accepted, 165 Hz automated profile qualified
- Date: 2026-07-24
- Scope: shared-TLW WinUI slot geometry, clipping, z-order and scroll latency

## Context

Every WinUI-backed wx child is represented by a slot on the single XAML
island of its top-level window. Native HWND mutations are coalesced, then
`FlushSync()` publishes geometry and clipping through `SyncSlot()` and rebuilds
the XAML order. This keeps ordinary moves and scrolls cheap, but it creates two
risks which cannot be settled by visual inspection:

- a slot may trail a native scroll or ancestor mutation for too long;
- a XAML-only ordering may disagree with the real USER32 sibling order.

A single XAML plane also cannot represent every possible alternating topology.
For example, native sibling order `XAML A > native N > XAML B` asks one native
HWND to be simultaneously below and above different elements of the same
island. The implementation must diagnose this limit; silently showing a
plausible but false order is not acceptable.

The previous spike had a continuously animated ring and a render callback
under its general diagnostics flag. Those probes are useful for freeze
collection, but they do not provide a deterministic scroll qualification and
must never become normal runtime overhead.

## Measurement contract

The dedicated command-line modes are opt-in:

```powershell
samples\winuispike\run-zorder-scenarios.ps1 `
    -BuildDir build-winui-clean

samples\winuispike\run-scroll-latency.ps1 `
    -BuildDir build-winui-clean `
    -TargetHz 60 `
    -Samples 24
```

A normal `winuispike` invocation installs no additional render subscription,
timer or measurement callback. The runners also remove the general spike
diagnostic environment variables from each child process.

The topology process uses an off-screen `WS_EX_NOACTIVATE` tool window and no
physical input. It records, in a stable JSON schema and fixed check order:

- real direct-child HWND order and both XAML `Canvas.ZIndex` values before and
  after `Raise()` and `Lower()`;
- the exact alternating HWND/XAML signature;
- ancestor, scroll-viewport, hidden-slot and TabView-band clips;
- vertical and horizontal splitter geometry with synchronous light/dark paint
  dispatch (not a pixel-colour assertion).

The latency process first waits for four rendering callbacks and performs one
unmeasured scroll batch. This fixed warm-up realizes templates and primes the
compositor without deleting an outlier after the fact. It then performs four
scroll mutations in one dispatcher turn for every measured sample.
This intentionally creates the maximum permitted backlog while retaining the
production coalescing path. The harness also snapshots the host's cumulative
flush-schedule counter around each four-mutation batch; at most one new
callback may be scheduled. It timestamps with QPC:

1. immediately before the first native mutation;
2. when `wxWinUISlot::GetRectInTLW()` exposes the final HWND geometry;
3. the first `CompositionTarget.Rendering` callback after step 2.

`GetRectInTLW()` is committed only after the production `SyncSlot()` XAML
setters and identity checks succeed. The observed timestamp is therefore a
conservative UI-thread upper bound for `SyncSlot`, not a duplicate
implementation or a guessed internal timestamp. Exact entry/exit timings
inside `SyncSlot()` would require a separate core diagnostic seam and are not
needed for the alpha budget.

Each result is atomically published to a fresh file. The process watchdog
retains the exact child object, terminates it on expiry, and verifies cleanup.
The executable SHA-256 is checked before and after every campaign. Schema v2
also serializes the measured render intervals. The runner independently
recomputes sample validity, p95 values, cadence, budgets, safety gates and the
final status using strict JSON types; it does not trust booleans emitted by the
sample.

## Budgets

The alpha qualification budgets are:

| Measurement | Budget |
|---|---:|
| mutation to committed slot, p95 | at most one target refresh period |
| mutation to next render, p95 | at most two target refresh periods |
| render safety when cadence differs | at most twice the larger of target period and observed median render period |
| stale final geometry | none after 250 ms |
| outstanding mutations in a batch | at most 4 |
| scheduled flush callbacks per batch | at most 1 |

Each runner invocation evaluates exactly one selected 60, 120 or 165 Hz
profile. Run and archive them separately after selecting the corresponding
display mode. The JSON records the observed render cadence and qualifies it
only within ±2% of the target, with a 1 Hz minimum tolerance.

Schema v2 has three non-overlapping outcomes:

- `pass` / exit `0`: cadence and every target budget qualify the profile;
- `unqualified` / exit `2`: structural, slot and cadence-relative safety gates
  pass, but the observed cadence does not match the selected profile;
- `fail` / exit `43` (or a setup/watchdog code): a technical, structural, slot
  or cadence-relative safety gate fails, or a matching cadence misses its
  target render budget.

An unqualified result is useful diagnostic evidence but never qualifies a
profile. The cadence-relative safety gate prevents a slow compositor from
turning a multi-frame render regression into `unqualified`.

The result table is intentionally filled from archived runner output, not from
hand-transcribed console text:

| Profile | slot p95 | render p95 | observed cadence | Result |
|---:|---:|---:|---:|---|
| 60 Hz | 9.261 ms | 9.846 ms | 161.290 Hz | unqualified (contract check on the 165 Hz display) |
| 120 Hz | pending | pending | pending | pending |
| 165 Hz | 4.952 ms | 6.239 ms | 164.636 Hz | pass |

The qualified 165 Hz exemplar is archived under
`build-winui-clean/phase007c-latency-v2-strict-165-20260725-231627`. Ten additional
fresh-process campaigns passed with observed cadence 164.53–165.10 Hz, slot
p95 1.374–2.913 ms and render p95 5.744–5.979 ms. The 60 Hz row is deliberately
labelled unqualified because it was run on that same 165 Hz display. The first
schema-v1 attempt, throttled around 31.7 Hz and polluted by a cold-start
sample, remains archived as failed evidence and is superseded by the fixed
warm-up and tri-state schema v2 contract.

## Options considered

### Keep the coalesced shared-slot architecture

One dispatcher publication per batch preserves the one-island-per-TLW model
and bounds repeated scroll work. It is the smallest design and remains the
baseline while it satisfies the budgets.

### Add a targeted synchronous flush

A synchronous flush can reduce a measured one-frame lag for a narrowly
identified mutation class. It also runs XAML setters inside native window
mutation stacks, increasing reentrancy and destruction risk. It is permitted
only after archived measurements identify a specific budget failure and a
focused test proves the reentrant lifetime path.

### Mirror native content into XAML canvases

Mirrors could represent more composition orders, but introduce duplicate
rendering, input, accessibility, invalidation, DPI and lifetime authorities.
No current measurement demonstrates that cost is necessary.

### Add multiple bridges or islands

Multiple bridges can represent alternating native/XAML planes, but violate
the accepted one-island-per-TLW architecture and reopen input, focus, D&D and
UI Automation ownership. This is a stop condition requiring a superseding ADR
and user approval.

## Decision

Keep one shared island and coalesced synchronization.

- Do not add canvas mirrors without an archived, repeatable budget failure.
- Do not add a synchronous flush globally. A targeted flush requires measured
  evidence, a reentrancy review and a superseding amendment to this ADR.
- Recompute XAML order from real HWND sibling order whenever a relevant native
  mutation occurs.
- Detect an unrepresentable alternating HWND/XAML topology in debug builds and
  expose a deterministic test counter/signature. The sample resets the
  production counter, establishes `A>N>B`, flushes once and requires exactly
  one diagnostic; the runner gates the serialized count.
- Keep all measurement hooks opt-in and absent from the default sample path.

The qualified 165 Hz campaigns satisfy both target budgets through the normal
coalesced path. They therefore provide no evidence for canvas mirrors or a
targeted synchronous flush.

## Consequences

- The performance decision is based on repeatable QPC evidence rather than an
  animated visual impression.
- Coalescing remains the only normal scheduling mechanism and its backlog has
  an explicit bound.
- The single-plane limitation is visible and reviewable instead of being
  hidden by an accidental creation-order rendering.
- JSON timing fields naturally vary, while schema, field order, scenario order
  and signatures are deterministic. Campaign summaries bind every result to
  the exact executable hash.

## Manual gates and limitations

Automation deliberately does not move the pointer, acquire capture, activate
the frame, change global High Contrast settings or move a window between
physical monitors. Before phase 007c is marked fully qualified, perform and
archive:

- splitter live/tracker drag, cursor, capture, double-click, min/gravity in
  Light, Dark and High Contrast;
- visual inspection at 100%, 150% and 200% DPI, including a per-monitor move;
- minimized/restored composition and TabView-page clipping;
- the 60 and 120 Hz latency profiles on matching display modes (165 Hz is
  qualified above).

The automated light/dark splitter step proves stable layout and exercises
theme changes, but it does not claim pixel-perfect colour validation.

## Rollback and supersession

The harness is isolated behind command-line switches and can be removed
without changing runtime behaviour. Any move to mirrors, multiple islands or
a general synchronous flush supersedes this ADR and requires the archived
before/after measurements, failure signatures and lifetime test results.
