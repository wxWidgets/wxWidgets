///////////////////////////////////////////////////////////////////////////////
// Private, non-installed AppNotification injection and observation access.
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_NOTIFICATION_TEST_ACCESS_H
#define WX_WINUI_NOTIFICATION_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/notification.h"

#include <cstddef>

#if wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

class WXDLLIMPEXP_CORE wxWinUIAppNotificationTestAccess final
{
public:
    struct Snapshot
    {
        bool enabled = false;
        bool registered = false;
        bool shuttingDown = false;
        bool finalized = false;
        std::size_t activeGenerationCount = 0;
        std::size_t liveInstanceCount = 0;
        std::size_t queuedCallbackCount = 0;
        std::size_t dispatchLeaseCount = 0;
        std::size_t nativeShowLeaseCount = 0;
        std::size_t cleanupOnlyCount = 0;
        bool dispatchEnabled = false;
        std::uint64_t lastGeneration = 0;
        std::uint64_t shutdownSerial = 0;
    };

    using DispatchHook = void (*)();

    // Replacement is transactional: the old native remains installed when
    // cleanup compensation, a lease or finalization prevents replacement.
    static bool SetNative(
        const std::shared_ptr<wxWinUIAppNotificationNative>& native);
    static bool ResetNative();
    static void Shutdown();
    static void Finalize();
    // Production finalization is permanent; only isolated tests may reset it
    // after the broker has reached the same quiescent state as OnExit().
    static void ResetFinalization();
    static void SetDispatchHook(DispatchHook hook);
    static void ResetDispatchHook();
    // Passive mutex-protected copy; it invokes no native operation or queue.
    static Snapshot GetSnapshot();
};

#endif // wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
#endif // WX_WINUI_NOTIFICATION_TEST_ACCESS_H
