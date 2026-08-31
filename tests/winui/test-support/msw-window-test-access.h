/////////////////////////////////////////////////////////////////////////////
// Private, non-installed MSW window transaction test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_MSW_WINDOW_TEST_ACCESS_H
#define WX_WINUI_MSW_WINDOW_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include "wx/gdicmn.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxWinUIMSWWindowTestAccess final
{
public:
    using ReparentDPIQuery = wxSize (*)(wxWindow *window);
    using WindowCallback = void (*)(wxWindow *window);

    // Real cross-TLW native-parent/DPI transactions, without a monitor move.
    static void SetReparentDPIQuery(ReparentDPIQuery query);
    static void FailNextSetParent();
    static void SetAfterSetParent(WindowCallback callback);

    // Consume-before-call boundaries on the real style and HWND publication
    // paths. Detach still retires UIA from its HWND snapshot after the hook.
    static void SetAfterEnsureControlParentStyle(WindowCallback callback);
    static void SetAfterHandleDepublished(WindowCallback callback);
    static void SetAfterLayoutDirectionNativeWrite(WindowCallback callback);
};

#endif // __WXWINUI__ && wxUSE_WINUI3

#endif // WX_WINUI_MSW_WINDOW_TEST_ACCESS_H
