///////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/notification.h
// Purpose:     Native AppNotification boundary used by wxNotificationMessage
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_NOTIFICATION_H_
#define _WX_WINUI_PRIVATE_NOTIFICATION_H_

#include "wx/defs.h"

#if wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

#include "wx/string.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

// This is the only boundary between wxNotificationMessage and the Windows
// App SDK.  Production installs a C++/WinRT implementation.  Tests install a
// fake so their gates never depend on Explorer, notification permissions or
// an actual notification-centre surface.
struct WXDLLIMPEXP_CORE wxWinUIAppNotificationAction
{
    wxWindowID id = wxID_NONE;
    wxString label;
};

struct WXDLLIMPEXP_CORE wxWinUIAppNotificationPayload
{
    std::uint64_t generation = 0;
    wxString title;
    wxString message;
    int timeout = -1;
    std::vector<wxWinUIAppNotificationAction> actions;
};

enum class wxWinUIAppNotificationActivationKind
{
    Click,
    Action
};

using wxWinUIAppNotificationActivation =
    std::function<void (std::uint64_t,
                        wxWinUIAppNotificationActivationKind,
                        wxWindowID)>;

class WXDLLIMPEXP_CORE wxWinUIAppNotificationNative
{
public:
    virtual ~wxWinUIAppNotificationNative() = default;

    virtual bool IsSupported() = 0;
    virtual bool Register(wxWinUIAppNotificationActivation activation) = 0;
    virtual void Unregister() = 0;

    // On success, notificationId is the process-local AppNotification Id used
    // by RemoveByIdAsync().  The fake follows the same contract.
    virtual bool Show(const wxWinUIAppNotificationPayload& payload,
                      std::uint32_t& notificationId) = 0;
    // Production returns whether RemoveByIdAsync() was successfully started;
    // it never blocks the GUI thread waiting for Explorer.
    virtual bool Remove(std::uint32_t notificationId) = 0;
};

struct WXDLLIMPEXP_CORE wxWinUIAppNotificationSnapshot
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

using wxWinUIAppNotificationDispatchHookForTesting = void (*)();

// Implementation-only seams. Replacement succeeds only after every retained
// compensation has been removed by the current native boundary; it then
// shuts down that registration and invalidates every outstanding generation.
// A false return leaves the current native boundary installed; it is never
// unregistered merely to complete a failed replacement.
WXDLLIMPEXP_CORE bool wxWinUISetAppNotificationNativeForTesting(
    const std::shared_ptr<wxWinUIAppNotificationNative>& native);
WXDLLIMPEXP_CORE bool wxWinUIResetAppNotificationNativeForTesting();
WXDLLIMPEXP_CORE void wxWinUIShutdownAppNotificationsForTesting();
WXDLLIMPEXP_CORE void wxWinUIFinalizeAppNotificationsForTesting();
// Finalization is permanent in production. This seam exists solely to keep
// independent test cases isolated after exercising the terminal latch.
WXDLLIMPEXP_CORE void
wxWinUIResetAppNotificationFinalizationForTesting();
WXDLLIMPEXP_CORE void wxWinUISetAppNotificationDispatchHookForTesting(
    wxWinUIAppNotificationDispatchHookForTesting hook);
WXDLLIMPEXP_CORE void
wxWinUIResetAppNotificationDispatchHookForTesting();
WXDLLIMPEXP_CORE wxWinUIAppNotificationSnapshot
wxWinUIGetAppNotificationSnapshotForTesting();

#endif // wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

#endif // _WX_WINUI_PRIVATE_NOTIFICATION_H_
