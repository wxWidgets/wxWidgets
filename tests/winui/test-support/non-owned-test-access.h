/////////////////////////////////////////////////////////////////////////////
// Non-installed deterministic boundaries for real non-owned HWND transactions.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_NON_OWNED_TEST_ACCESS_H
#define WX_WINUI_NON_OWNED_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxWinUINonOwnedTestAccess final
{
public:
    using OwnerPublishHook = void (*)(wxWindow *window);
    using LayoutRefreshHook = void (*)(wxWindow *window);

    // Fail the next native publication before USER32; logical/native parent
    // reconciliation and the real verified failure result remain authoritative.
    static void FailNextOwnerWrite();
    // Exact callback boundaries after native publication / inherited RTL.
    static void SetOwnerPublishHook(OwnerPublishHook hook);
    static void ResetOwnerPublishHook();
    static void SetLayoutRefreshHook(LayoutRefreshHook hook);
    static void ResetLayoutRefreshHook();
};
#endif // wxUSE_WINUI3

#endif // WX_WINUI_NON_OWNED_TEST_ACCESS_H
