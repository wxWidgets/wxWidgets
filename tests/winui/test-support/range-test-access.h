/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to Gauge/ScrollBar/SpinButton test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_RANGE_TEST_ACCESS_H
#define WX_WINUI_RANGE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/gauge.h"
#include "wx/scrolbar.h"
#include "wx/spinbutt.h"

#if wxUSE_GAUGE || wxUSE_SCROLLBAR || wxUSE_SPINBTN
class WXDLLIMPEXP_CORE wxWinUIRangeTestAccess final
{
public:
#if wxUSE_GAUGE
    static bool GetPeerState(const wxGauge& gauge,
                              double* maximum,
                              double* value,
                              bool* indeterminate,
                              bool* vertical);
    static bool HasAppProgress(const wxGauge& gauge);
#endif
#if wxUSE_SCROLLBAR
    using ScrollBarAction = wxScrollBar::WinUIPeerAction;

    static bool ApplyScrollBarAction(wxScrollBar& bar,
                                      ScrollBarAction action, int value);
    static bool GetPeerState(const wxScrollBar& bar,
                              double* minimum,
                              double* maximum,
                              double* value,
                              double* viewport,
                              double* smallChange = nullptr,
                              double* largeChange = nullptr,
                              bool* vertical = nullptr);
#endif
#if wxUSE_SPINBTN
    static bool Step(wxSpinButton& spin, int direction);
    static bool GetPeerLayout(const wxSpinButton& spin,
                               bool* vertical,
                               unsigned* rows,
                               unsigned* columns);
    static bool GetAutomation(const wxSpinButton& spin,
                               int* rootControlType,
                               wxString* rootClassName,
                               wxString* incrementId,
                               wxString* decrementId,
                               wxString* incrementName,
                               wxString* decrementName);
    static bool InvokeArrow(wxSpinButton& spin, int direction);
#endif
};
#endif

#endif
