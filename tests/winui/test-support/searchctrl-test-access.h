/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxSearchCtrl test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_SEARCHCTRL_TEST_ACCESS_H
#define WX_WINUI_SEARCHCTRL_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/srchctrl.h"

#if wxUSE_SEARCHCTRL
class WXDLLIMPEXP_CORE wxWinUISearchCtrlTestAccess final
{
public:
    using CallbackHook = void (*)(wxSearchCtrl*, void*);

    static bool InvokeSearchButton(wxSearchCtrl& searchCtrl);
    static bool InvokeCancelButton(wxSearchCtrl& searchCtrl);
    static bool SetPeerText(wxSearchCtrl& searchCtrl, const wxString& text);
    // Swap detached test parts through the production generation/revocation
    // transaction; this does not claim to retemplate the native control.
    static bool Retemplate(wxSearchCtrl& searchCtrl);
    static bool InvokeRetiredSearchButton(wxSearchCtrl& searchCtrl);
    static unsigned GetSuggestionCount(const wxSearchCtrl& searchCtrl);
    static unsigned GetTemplateState(const wxSearchCtrl& searchCtrl);
    static void SetNextCreateLoadedHook(wxSearchCtrl& searchCtrl,
                                        CallbackHook hook,
                                        void *context);
};
#endif // wxUSE_SEARCHCTRL

#endif // WX_WINUI_SEARCHCTRL_TEST_ACCESS_H
