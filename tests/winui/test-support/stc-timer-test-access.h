/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI STC native timer observation.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_STC_TIMER_TEST_ACCESS_H
#define WX_WINUI_STC_TIMER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/msw/private/timer.h"
#include "wx/stc/stc.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_STC

// wxTimer::GetId() is the wx event id, whereas this test needs the USER32
// identity carried by WM_TIMER::wParam. This friend adapter is compiled only
// into test-enabled WinUI builds.
class wxMSWTimerTestAccess final
{
public:
    static WPARAM GetNativeId(const wxMSWTimerImpl& timer)
    {
        return timer.m_id;
    }
};

class WXDLLIMPEXP_STC wxSTCTimerTestAccess final
{
public:
    static wxUIntPtr GetCaretTimerId(wxStyledTextCtrl* control);
};

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_STC

#endif // WX_WINUI_STC_TIMER_TEST_ACCESS_H
