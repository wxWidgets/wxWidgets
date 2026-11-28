/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to generic WinUI feedback fallbacks.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_GENERIC_FEEDBACK_TEST_ACCESS_H
#define WX_WINUI_GENERIC_FEEDBACK_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/animate.h"
#include "wx/bannerwindow.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#if wxUSE_BANNERWINDOW
class wxWinUIBannerTestAccess final
{
public:
    static void DrawBitmapBackground(wxBannerWindow& banner, wxDC& dc)
    {
        banner.DrawBitmapBackground(dc);
    }
};
#endif

#if wxUSE_ANIMATIONCTRL
class wxWinUIGenericAnimationTestAccess final
{
public:
    static int GetTimerId(const wxGenericAnimationCtrl& control)
    {
        return control.m_activeTimerId;
    }

    static bool IsTimerRunning(const wxGenericAnimationCtrl& control)
    {
        return control.m_timer.IsRunning();
    }

    static int GetTimerInterval(const wxGenericAnimationCtrl& control)
    {
        return control.m_timer.IsRunning() ? control.m_timer.GetInterval() : 0;
    }

    static unsigned int GetCurrentFrame(const wxGenericAnimationCtrl& control)
    {
        return control.m_currentFrame;
    }

    static bool IsPausedForHidden(const wxGenericAnimationCtrl& control)
    {
        return control.m_pausedForHidden;
    }

    static bool ScheduleNextFrame(wxGenericAnimationCtrl& control)
    {
        return control.ScheduleNextFrame();
    }

    static wxSize GetPreferredAnimationSizeForScale(
        const wxGenericAnimationCtrl& control, double scale)
    {
        const wxAnimation selected =
            control.SelectAnimationForScale(control.m_animations, scale);
        return selected.IsOk() ? selected.GetSize() : wxDefaultSize;
    }
};
#endif

#endif // __WXWINUI__ && wxUSE_WINUI3

#endif // WX_WINUI_GENERIC_FEEDBACK_TEST_ACCESS_H
