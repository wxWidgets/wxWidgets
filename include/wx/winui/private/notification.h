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

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

// This is the only boundary between wxNotificationMessage and the Windows
// App SDK. Production installs the C++/WinRT implementation; deterministic
// replacement and observation live in the non-installed test-access header.
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

#endif // wxUSE_WINUI3 && wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

#endif // _WX_WINUI_PRIVATE_NOTIFICATION_H_
