///////////////////////////////////////////////////////////////////////////////
// Private, non-installed wxTaskBarIcon test adapters and diagnostics.
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TASKBAR_ICON_TEST_ACCESS_H
#define WX_WINUI_TASKBAR_ICON_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/taskbaricon.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TASKBARICON

class wxMenu;
class wxTaskBarIcon;
class wxWindow;

class WXDLLIMPEXP_CORE wxWinUITaskBarIconTestAccess final
{
public:
    using AppliedState = wxWinUITaskBarIconAppliedState;

    struct Snapshot
    {
        bool desiredInstalled = false;
        AppliedState applied = AppliedState::Absent;
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

    // The data pointer borrows the NOTIFYICONDATA for this synchronous call.
    // False emulates a native failure without contacting Explorer.
    using ShellNotifyHook = bool (*)(unsigned message, const void *data);
    // Replaces only blocking menu presentation; the real hidden helper still
    // forwards UpdateUI and command events through production weak ownership.
    using PopupHook = bool (*)(wxWindow *helper, wxMenu *menu);

    static void SetShellNotifyHook(ShellNotifyHook hook);
    static void ResetShellNotifyHook();
    static void SetPopupHook(PopupHook hook);
    static void ResetPopupHook();
    static bool RestartExplorer(wxTaskBarIcon *icon);
    // These dispatch through the real hidden helper HWND and callback lease.
    static bool DispatchTaskbarCreated(wxTaskBarIcon *icon);
    static bool DispatchEvent(wxTaskBarIcon *icon, long nativeEvent);
    // Models DeleteAllTLWs deleting the helper before its non-window owner;
    // callback-active deletion remains deferred until the callback unwinds.
    static bool DestroyHelper(wxTaskBarIcon *icon);
    // Passive: copies the current sidecar fields without native calls.
    static bool GetSnapshot(wxTaskBarIcon *icon, Snapshot *snapshot);
    static unsigned GetStateCount();
    static unsigned GetHelperCount();
};

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TASKBARICON
#endif // WX_WINUI_TASKBAR_ICON_TEST_ACCESS_H
