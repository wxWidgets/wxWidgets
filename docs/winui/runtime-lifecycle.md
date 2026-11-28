# Runtime startup and process lifetime

The WinUI toolkit requires the pinned Windows App SDK runtime. Its wx module
returns failure when initialization fails; wxInitializer / wxEntryStart must
then be treated as failed. This is not an optional wxMSW integration mode, and
applications must not create controls after failed wx initialization.

Initialization belongs to one STA UI thread. Calls from another thread and
reentrant transitions are rejected. Repeated calls while Running do not
acquire another runtime reference. Explicit complete shutdown/reinitialization
epochs are supported only after the application's windows and peer graphs
have been retired, not as a way to repair a live UI.

## Effects and ownership

| Effect | Owner and success path | Failure/shutdown policy |
|---|---|---|
| COM/WinRT apartment | Runtime owns exactly its acquired reference, including S_FALSE on an existing STA. | Pre-XAML failure and clean shutdown balance only this reference. Unsafe XAML teardown retains it in quarantine; the caller's reference is never consumed. |
| Bootstrap/package dependency | One active runtime epoch. | Released after clean framework retirement, or immediately on clean pre-XAML failure. Retained with an unsafe epoch. |
| XAML application, manager and owned dispatcher queue | Runtime epoch and ordered framework-retirement callbacks. | Peer graphs and application retire under the framework shutdown deferral before the manager. Missing terminal evidence is quarantine, not a successful shutdown. |
| Fluent wxRendererNative | Existing wx global renderer holder, independent of XAML epochs. | Chosen once if no explicit or traits-provided renderer exists. Runtime restart/shutdown neither overwrites a later application renderer nor takes ownership of a pointer returned by an application's Set call. |
| LC_NUMERIC=C | Process policy enabled before XAML-manager activation. | Not restored on shutdown or failed XAML activation: retained framework objects may still parse numeric markup. Use wxNumberFormatter/wxUILocale for localized numbers; direct CRT setlocale bypasses the invariant. |
| WH_GETMESSAGE hook | UI thread only, not a system-global hook. | Disable callbacks before unhooking. If removal fails, keep it inert and pin its code module. If installation fails, the wx event-loop fallback remains, but nested native-loop input is degraded and logged. |
| Theme/tooltip policy | Theme reflects current application/system settings; tooltip registrations belong to live peers/runtime. | Shutdown invalidates tooltip callbacks before retiring XAML. Application/system appearance is not rolled back as a synthetic runtime snapshot. |
| XAML module pin | Process lifetime once obtained. | Intentionally not released across clean epochs; quarantine additionally pins callback-owning code. Pins are reported retained resources, not hidden as zero leaks. |

## Renderer replacement

An application may call wxRendererNative::Set before the first Get or before
wx initialization. Lazy initialization preserves that renderer. A renderer
provided by wxAppTraits also takes precedence over the Fluent default.

Set transfers the new pointer to wx's renderer holder and returns the previous
owned pointer to its caller. The caller is responsible for that returned
pointer. Installing another renderer, or resetting to the platform default
with Set(nullptr), remains authoritative across later WinUI runtime epochs.
Runtime shutdown does not restore an older application renderer behind its
back. The Fluent renderer itself uses wx drawing primitives, not a retained
XAML peer graph.

## Clean failure versus quarantine

A failure before XAML activation releases the acquired apartment/bootstrap
resources and returns the runtime to Idle. Failure after activation may be
unsafe to unwind: if retirement cannot be proved complete, the epoch enters
terminal Quarantined state. Further initialization is rejected and cleanup is
an idempotent no-op. If retaining callback code safely is itself impossible,
the process fails fast instead of allowing use-after-unload.

A wx module whose OnInit fails does not receive OnExit. All initialization
failure paths must therefore complete their own clean rollback or quarantine.
Direct runtime probes used by tests do not authorize continuing ordinary wx
application startup after wxInitializer has failed.

The installed-consumer subprocess tests distinguish clean pre-bootstrap
failure, MTA rejection, renderer ownership and six intentionally quarantined
faults. They monitor access violations through process exit and inventory WER
dumps. These checks do not replace physical-input qualification or a measured
lifecycle soak; retained process resources and growing application leaks must
be reported separately.
