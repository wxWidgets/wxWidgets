/////////////////////////////////////////////////////////////////////////////
// Private, non-installed adapters for the existing host focus paths.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_FOCUS_TEST_ACCESS_H
#define WX_WINUI_HOST_FOCUS_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif
#include "wx/winui/private/tlwhost.h"
#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUIHostFocusTestAccess final
{
public:
    static unsigned GetPendingFocusMigrationCount();
    static void TakeFocusRequested(
        wxWinUITopLevelHost& host,
        bool previous);
    static wxWindow *FindFocusedSlot(const wxWinUITopLevelHost& host);
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_HOST_FOCUS_TEST_ACCESS_H
