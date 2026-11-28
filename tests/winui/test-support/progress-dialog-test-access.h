/////////////////////////////////////////////////////////////////////////////
// Private, non-installed generic progress-dialog test adapter.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_PROGRESS_DIALOG_TEST_ACCESS_H
#define WX_WINUI_PROGRESS_DIALOG_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

class WXDLLIMPEXP_FWD_CORE wxGenericProgressDialog;

class WXDLLIMPEXP_CORE wxWinUIProgressDialogTestAccess final
{
public:
    using YieldHook = void (*)(void *);

    static void SetYieldHook(wxGenericProgressDialog& dialog,
                             void *context,
                             YieldHook hook);
};

#endif // __WXWINUI__ && wxUSE_WINUI3

#endif // WX_WINUI_PROGRESS_DIALOG_TEST_ACCESS_H
