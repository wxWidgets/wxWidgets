#ifndef WX_TEST_TOOLKIT_CONTRACT_H
#define WX_TEST_TOOLKIT_CONTRACT_H

#include "wx/defs.h"
#include <type_traits>
#include <utility>

#ifndef __WINDOWS__
    #error This probe requires the Windows platform
#endif
#ifndef __WXMSW__
    #error Both Windows toolkits currently use the MSW window substrate
#endif
#ifndef WX_EXPECT_WINUI
    #error The consumer must specify which toolkit it expects
#endif
#if WX_EXPECT_WINUI
    #if !defined(__WXWINUI__) || !wxUSE_WINUI3
        #error The installed package did not select the WinUI toolkit
    #endif
#else
    #if defined(__WXWINUI__) || wxUSE_WINUI3
        #error The ordinary MSW package must not select WinUI
    #endif
#endif

// These remain available through wxWindowMSW even when a control is a XAML
// peer. Their presence does not promise a Win32 buddy HWND or native painting.
template<typename Base>
class NativeHooksProbe : public Base
{
protected:
    bool ContainsHWND(WXHWND hwnd) const override
        { return Base::ContainsHWND(hwnd); }
    WXHWND MSWGetFocusHWND() const override
        { return Base::MSWGetFocusHWND(); }
    bool MSWOnNotify(int id, WXLPARAM param, WXLPARAM* result) override
        { return Base::MSWOnNotify(id, param, result); }
    bool MSWOnScroll(int orientation, WXWORD code, WXWORD position,
                     WXHWND control) override
        { return Base::MSWOnScroll(orientation, code, position, control); }
    bool MSWShouldPreProcessMessage(WXMSG* message) override
        { return Base::MSWShouldPreProcessMessage(message); }
    void MSWUpdateFontOnDPIChange(const wxSize& dpi) override
        { Base::MSWUpdateFontOnDPIChange(dpi); }
    bool CanApplyThemeBorder() const override
        { return Base::CanApplyThemeBorder(); }
    WXHBRUSH MSWGetCustomBgBrush() override
        { return Base::MSWGetCustomBgBrush(); }
    WXHBRUSH MSWGetBgBrushForChild(WXHDC dc, wxWindowMSW* child) override
        { return Base::MSWGetBgBrushForChild(dc, child); }
};

#endif
