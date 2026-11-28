/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to WinUI peer test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_SPINCTRL_TEST_ACCESS_H
#define WX_WINUI_SPINCTRL_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/spinctrl.h"

#if wxUSE_SPINCTRL
class WXDLLIMPEXP_CORE wxWinUISpinCtrlTestAccess final
{
public:
    static bool SetPeerValue(
        wxSpinCtrl& control,
        double value);
    static bool SetPeerText(
        wxSpinCtrl& control,
        const wxString& text);
    static bool GetPeerState(
        const wxSpinCtrl& control,
        double *minimum,
        double *maximum,
        double *increment,
        bool *wrap,
        wxString *text,
        double *value = nullptr);
    static bool GetPeerSelection(
        wxSpinCtrl& control,
        long *from, long *to);
    static bool Retemplate(wxSpinCtrl& control);
    static bool Enter(wxSpinCtrl& control);
    static bool Step(
        wxSpinCtrl& control,
        int direction);

    static bool SetPeerValue(
        wxSpinCtrlDouble& control,
        double value);
    static bool SetPeerText(
        wxSpinCtrlDouble& control,
        const wxString& text);
    static bool GetPeerState(
        const wxSpinCtrlDouble& control,
        double *minimum,
        double *maximum,
        double *increment,
        bool *wrap,
        wxString *text,
        double *value = nullptr);
    static bool GetPeerSelection(
        wxSpinCtrlDouble& control,
        long *from, long *to);
    static bool Retemplate(wxSpinCtrlDouble& control);
    static bool Enter(wxSpinCtrlDouble& control);
    static bool Step(
        wxSpinCtrlDouble& control,
        int direction);
};
#endif

#endif
