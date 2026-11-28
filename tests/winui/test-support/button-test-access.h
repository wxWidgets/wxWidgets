/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to Button/BitmapToggle test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_BUTTON_TEST_ACCESS_H
#define WX_WINUI_BUTTON_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/anybutton.h"
#include "wx/button.h"
#include "wx/tglbtn.h"
#include <cstdint>

#if wxUSE_BUTTON || wxUSE_TOGGLEBTN
class WXDLLIMPEXP_CORE wxWinUIButtonTestAccess final
{
public:
    using State = wxAnyButton::State;

#if wxUSE_BUTTON
    static bool QueueClick(wxButton& button);
    static unsigned LiveCallbackStateCount();
    static unsigned PeerInvokeAttemptCount();
    static bool ProjectBitmap(wxButton& button, State state, double scale);
    static bool GetBitmapProjection(const wxButton& button,
                                    wxSize* bitmapPixelSize,
                                    wxSize* authPixelSize,
                                    State* state,
                                    std::uint64_t* generation);
#endif
#if wxUSE_TOGGLEBTN
    static bool ProjectBitmap(wxBitmapToggleButton& button,
                              State state, double scale);
    static bool GetBitmapProjection(const wxBitmapToggleButton& button,
                                    wxSize* pixelSize,
                                    State* state,
                                    std::uint64_t* generation);
#endif
};
#endif

#endif
