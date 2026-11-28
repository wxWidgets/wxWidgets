/////////////////////////////////////////////////////////////////////////////
// Private, non-installed host UIA test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_HOST_UIA_TEST_ACCESS_H
#define WX_WINUI_HOST_UIA_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include "wx/msw/wrapwin.h"

#if wxUSE_WINUI3

class wxWindow;
struct IRawElementProviderSimple;

class WXDLLIMPEXP_CORE wxWinUIHostUIATestAccess final
{
public:
    // Observe the existing real observer/provider lifetime counters.
    static unsigned GetLiveAutomationNameStyleObserverCount();
    static unsigned GetAccessibilityShellProviderRetireCount();
    static unsigned GetLiveInvisibleShellProviderCount();

    // Build the same Raw-only provider used by WM_GETOBJECT without
    // USER32 result marshalling. The returned COM reference is caller-owned.
    static HRESULT CreateInvisibleShellProvider(
        wxWindow *window, IRawElementProviderSimple **provider);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_HOST_UIA_TEST_ACCESS_H
