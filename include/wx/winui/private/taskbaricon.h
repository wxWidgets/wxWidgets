///////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/taskbaricon.h
// Purpose:     Private wxTaskBarIcon WinUI test seams and diagnostics
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TASKBARICON_H_
#define _WX_WINUI_PRIVATE_TASKBARICON_H_

#include "wx/defs.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TASKBARICON

class wxMenu;
class wxTaskBarIcon;
class wxWindow;

enum class wxWinUITaskBarIconAppliedState
{
    Absent,
    Present,
    Unknown
};

struct wxWinUITaskBarIconSnapshotForTesting
{
    bool desiredInstalled = false;
    wxWinUITaskBarIconAppliedState applied =
        wxWinUITaskBarIconAppliedState::Absent;
    bool versionApplied = false;
    bool reconciling = false;
    bool reconcilePending = false;
    bool destroying = false;
    unsigned long long desiredGeneration = 0;
    unsigned long long appliedGeneration = 0;
    unsigned long long explorerGeneration = 0;
    unsigned long long appliedExplorerGeneration = 0;
    unsigned nativeCallCount = 0;
};

// The data pointer names the NOTIFYICONDATA passed to Shell_NotifyIcon(). It is
// valid only for the duration of the hook. Returning false emulates a native
// failure without ever contacting the interactive Explorer instance.
using wxWinUITaskBarIconShellNotifyHookForTesting =
    bool (*)(unsigned message, const void *data);

// Replaces only the blocking menu presentation. The helper remains the real
// invoking wxWindow, so a hook can deterministically update the menu and route
// wxEVT_MENU through the production weak forwarding path.
using wxWinUITaskBarIconPopupHookForTesting =
    bool (*)(wxWindow *helper, wxMenu *menu);

WXDLLIMPEXP_CORE void wxWinUISetTaskBarIconShellNotifyHookForTesting(
    wxWinUITaskBarIconShellNotifyHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetTaskBarIconShellNotifyHookForTesting();
WXDLLIMPEXP_CORE void wxWinUISetTaskBarIconPopupHookForTesting(
    wxWinUITaskBarIconPopupHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetTaskBarIconPopupHookForTesting();

WXDLLIMPEXP_CORE bool wxWinUITaskBarIconRestartExplorerForTesting(
    wxTaskBarIcon *icon);
// Dispatches the registered messages through the real hidden helper HWND and
// therefore exercises wxTaskBarIconWindow::MSWWindowProc(), including its
// callback-lifetime lease. The event argument is a native WM_* or NIN_* value.
WXDLLIMPEXP_CORE bool
wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(wxTaskBarIcon *icon);
WXDLLIMPEXP_CORE bool wxWinUITaskBarIconDispatchEventForTesting(
    wxTaskBarIcon *icon,
    long nativeEvent);
// Deletes the real hidden helper synchronously. This exists solely to model
// wxApp::DeleteAllTLWs() deleting it before the non-window taskbar object. If
// called from a helper callback, the request is accepted synchronously but the
// C++ deletion is deferred until that callback unwinds.
WXDLLIMPEXP_CORE bool wxWinUITaskBarIconDestroyHelperForTesting(
    wxTaskBarIcon *icon);
WXDLLIMPEXP_CORE bool wxWinUIGetTaskBarIconSnapshotForTesting(
    wxTaskBarIcon *icon,
    wxWinUITaskBarIconSnapshotForTesting *snapshot);
WXDLLIMPEXP_CORE unsigned wxWinUIGetTaskBarIconStateCountForTesting();
WXDLLIMPEXP_CORE unsigned wxWinUIGetTaskBarIconHelperCountForTesting();

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TASKBARICON

#endif // _WX_WINUI_PRIVATE_TASKBARICON_H_
