/////////////////////////////////////////////////////////////////////////////
// Private, non-installed Slot/content-lifecycle observations and adapters.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_CONTENT_TEST_ACCESS_H
#define WX_WINUI_HOST_CONTENT_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif
#include "wx/winui/private/tlwhost.h"
#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUISlotTestAccess final
{
public:
    static unsigned long long GetSyncEpoch(const wxWinUISlot& slot);
    static bool IsContentTransactionInProgress(const wxWinUISlot& slot);
    static bool IsToolTipPropertyObserverPending(const wxWinUISlot& slot);
    static unsigned GetToolTipPropertyObserverRetryFailures(const wxWinUISlot& slot);
    static bool IsContentLoaded(const wxWinUISlot& slot);
    static void SetContentLoaded(wxWinUISlot& slot, bool loaded);
    static bool IsContentLoadedObserverPending(const wxWinUISlot& slot);
    static bool HasContentLoadedObserver(const wxWinUISlot& slot);
    static winrt::Microsoft::UI::Xaml::UIElement GetAutomationNameStyleObservedContent(const wxWinUISlot& slot);
    static unsigned GetContentLoadedObserverRetryFailures(const wxWinUISlot& slot);
    static std::weak_ptr<wxWinUISlotLifetime> GetLifetime(const wxWinUISlot& slot);
    static void InvokeContentLoadedObserver(wxWinUISlot& slot);
    static void SetLastToolTipDPI(wxWinUISlot& slot, int dpi);
    static std::uintptr_t GetPhysicalDisconnectGateIdentity(const wxWinUISlot& slot);
};
class WXDLLIMPEXP_CORE wxWinUIHostContentTestAccess final
{
public:
    // Deterministic seams for host construction and the provisional first
    // OLE acquisition. The hook is invoked while an OperationGuard is held,
    // so a test may destroy the TLW or mutate presentation without allowing
    // the host allocation to disappear from underneath the transaction.
    enum class InitializationHookPoint
    {
        AfterProvisionalPublication,
        AfterSourceInitialization,
        AfterRootRouter,
        AfterTitleBarPolicy,
        AfterTLWBindings,
        AfterDropBrokerReadyBeforeCommit,
        BeforeExceptionLog
    };
    using InitializationHook =
        void (*)(wxWindow *, InitializationHookPoint);
    enum
    {
        InitializationFault_None = 0,
        InitializationFault_AfterTLWBindings = 1
    };
    static void SetInitializationHook(
        InitializationHook callback);
    static void FailInitialization(unsigned mask);

    // Fault injection for the registration-rollback tests: the Nth handler
    // subscription performed by the next BindSlotEvents() throws.  0 (the
    // default) disables the injection, and it disarms itself after firing,
    // so a forgotten reset cannot poison later registrations.
    static void FailHandlerAdd(unsigned nth);

    // Fault injection for the content-swap transaction tests: a mask of
    // ContentFault_* bits, each consumed by its phase when it fires.
    enum
    {
        ContentFault_Detach = 1,
        ContentFault_Install = 2,
        ContentFault_Restore = 4,
        ContentFault_Read = 8,  // TryGetCarrierContent() read failure
        ContentFault_ToolTipObserver = 16,
        ContentFault_AutomationNameTransferRead = 32,
        ContentFault_AutomationNameRestoreRead = 64
    };
    static void FailContentSwap(unsigned mask);
    // Fail the next N tooltip-property observer registrations. Unlike the
    // phase bit above this is a countdown, allowing deterministic coverage
    // of multiple deferred retries before a later success.
    static void FailToolTipObserverAdds(unsigned count);
    static void FailContentLoadedObserverAdds(unsigned count);
    static void FailInputPointerSourceSet(unsigned nth);

    // One-shot seam at the actual synchronous reentrancy boundary of slot
    // publication. XAML Loaded is intentionally not used by the lifecycle
    // tests: a newly attached carrier can remain Collapsed until the next
    // coalesced layout flush, making Loaded asynchronous and non-portable.
    static void OnNextSlotAttached(
        std::function<void (wxWindow *)> callback);
    // One-shot seam after the requested element has been written to the
    // carrier but before SetContent() publishes it as the slot model. It
    // models a synchronous Loaded/property callback entering FlushSync().
    static void OnNextContentCarrierSet(
        std::function<void (wxWindow *)> callback);
    // One-shot seam immediately after SetContent() publishes its transaction
    // flag and before the old element is detached. It lets tests drive the
    // real generation-bound Loaded callback and then choose commit/rollback
    // without depending on asynchronous XAML layout timing.
    static void OnNextContentTransactionStarted(
        std::function<void (wxWindow *)> callback);
    // One-shot seam immediately after ResolveWindowAtScreenPoint() reads a
    // snapshotted slot's IsHitTestVisible value. It lets tests destroy that
    // exact slot at the first callback boundary and prove no raw m_slots
    // iterator or wxWinUISlot pointer survives the getter.
    static void OnNextWindowAtPointSlotStateRead(
        std::function<void (wxWindow *)> callback);
    static void InvokeInitializationHook(
        wxWinUITopLevelHost& host, InitializationHookPoint point);
    static void Shutdown(wxWinUITopLevelHost& host);
    static std::weak_ptr<wxWinUIHostLifetime> GetLifetime(const wxWinUITopLevelHost& host);
    static unsigned GetOwnFlushRunCount(const wxWinUITopLevelHost& host);
    // Retaining application content after shutdown must not retain a Loaded
    // callback state capable of reaching wx.
    static unsigned GetLiveContentLoadedObserverCount();
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_HOST_CONTENT_TEST_ACCESS_H
