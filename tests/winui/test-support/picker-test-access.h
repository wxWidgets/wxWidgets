/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to ColourButton test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_PICKER_TEST_ACCESS_H
#define WX_WINUI_PICKER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/clrpicker.h"

#if wxUSE_COLOURPICKERCTRL
class WXDLLIMPEXP_CORE wxWinUIColourButtonTestAccess final
{
public:
    // Crosses the real WinRT Color property and ColorChanged callback.
    static bool SetPeerColour(wxWinUIColourButton& button, const wxColour& colour);
    static bool GetPeerState(const wxWinUIColourButton& button,
                             wxColour* colour, bool* alphaEnabled,
                             wxString* label);
    // Delivers the final wx event, not a native Flyout::Closed notification.
    static bool DeliverClosed(wxWinUIColourButton& button);
    static unsigned GetLiveCallbackStateCount();
};
#endif

#endif
