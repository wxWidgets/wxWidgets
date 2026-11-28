/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to InfoBar test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_INFOBAR_TEST_ACCESS_H
#define WX_WINUI_INFOBAR_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/infobar.h"

#if wxUSE_INFOBAR
class WXDLLIMPEXP_CORE wxWinUIInfoBarTestAccess final
{
public:
    using ContentWriteHook = void (*)(void *);

    static bool IsPeerOpen(const wxInfoBar& info);
    // Invoke the realized native automation peer, without physical input.
    static bool ClickCloseButton(wxInfoBar& info);
    static bool ClickButton(wxInfoBar& info, wxWindowID btnid);
    // One-shot seam after the real Content write, before generation commit.
    static void SetNextContentWriteHook(wxInfoBar& info,
                                        ContentWriteHook hook, void* context);
    static bool HasDeferredContentProjection(const wxInfoBar& info);
    static bool IsContentProjectionQuarantined(const wxInfoBar& info);
};
#endif

#endif
