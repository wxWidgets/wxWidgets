/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/richtooltip.cpp
// Purpose:     wxRichToolTip using a WinUI TeachingTip
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_RICHTOOLTIP && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
#endif

#include "wx/private/richtooltip.h"
#include "wx/generic/private/richtooltip.h"
#include "wx/msw/private.h"

#include "private.h"

#include <chrono>
#include <winrt/Microsoft.UI.Dispatching.h>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

// Keeps the transient island and the TeachingTip alive until the tip is
// closed (the wxRichToolTip object itself is typically destroyed right after
// ShowFor() returns); deletes itself once the tip goes away.
class wxWinUITeachingTipHost
{
public:
    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource source{ nullptr };
    MUXC::TeachingTip tip{ nullptr };
    winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer timer{ nullptr };

    ~wxWinUITeachingTipHost()
    {
        try
        {
            if ( timer )
                timer.Stop();
            if ( source )
                source.Close();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
};

class wxWinUIRichToolTipImpl : public wxRichToolTipGenericImpl
{
public:
    wxWinUIRichToolTipImpl(const wxString& title, const wxString& message)
        : wxRichToolTipGenericImpl(title, message)
    {
    }

    void SetTimeout(unsigned milliseconds,
                    unsigned millisecondsDelay = 0) override
    {
        m_winuiTimeout = milliseconds;
        wxRichToolTipGenericImpl::SetTimeout(milliseconds, millisecondsDelay);
    }

    void ShowFor(wxWindow* win, const wxRect* rect = nullptr) override
    {
        // Fall back to the generic wx-drawn balloon if the TeachingTip could
        // not be shown for any reason.
        if ( !ShowTeachingTip(win) )
            wxRichToolTipGenericImpl::ShowFor(win, rect);
    }

private:
    bool ShowTeachingTip(wxWindow* win);

    // 5s default, as in the generic implementation.
    unsigned m_winuiTimeout = 5000;
};

bool wxWinUIRichToolTipImpl::ShowTeachingTip(wxWindow* win)
{
    if ( !win || !wxWinUI3Initialize() )
        return false;

    HWND hwnd = GetHwndOf(win);
    if ( !hwnd )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI;
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        auto host = new wxWinUITeachingTipHost;

        host->source = DesktopWindowXamlSource();
        host->source.Initialize(GetWindowIdFromWindow(hwnd));
        host->source.SiteBridge().ResizePolicy(
            ContentSizePolicy::ResizeContentToParentWindow);

        const HWND hwndBridge =
            GetWindowFromWindowId(host->source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            hwndBridge,
            GWL_STYLE,
            ::GetWindowLongPtr(hwndBridge, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
        );
        ::SetWindowPos(hwndBridge, HWND_TOP, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

        MUXC::Grid root;
        root.RequestedTheme(wxWinUIGetCurrentElementTheme());
        host->source.Content(root);

        if ( !root.XamlRoot() )
        {
            delete host;
            return false;
        }

        MUXC::TeachingTip tip;
        tip.Title(wxWinUIToHString(m_title));
        tip.Subtitle(wxWinUIToHString(m_message));
        tip.IsLightDismissEnabled(true);
        tip.ShouldConstrainToRootBounds(false);
        tip.PreferredPlacement(MUXC::TeachingTipPlacementMode::Top);
        tip.Target(root);

        root.Children().Append(tip);
        host->tip = tip;

        tip.Closed(
            [host](MUXC::TeachingTip const&,
                   MUXC::TeachingTipClosedEventArgs const&)
            {
                // Don't tear the island down from inside its own callback.
                if ( wxTheApp )
                    wxTheApp->CallAfter([host]() { delete host; });
                else
                    delete host;
            });

        if ( m_winuiTimeout > 0 )
        {
            auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
                GetForCurrentThread();
            if ( queue )
            {
                auto timer = queue.CreateTimer();
                timer.Interval(std::chrono::milliseconds(m_winuiTimeout));
                timer.IsRepeating(false);
                timer.Tick(
                    [tip](winrt::Windows::Foundation::IInspectable const&,
                          winrt::Windows::Foundation::IInspectable const&)
                    {
                        MUXC::TeachingTip(tip).IsOpen(false);
                    });
                timer.Start();
                host->timer = timer;
            }
        }

        tip.IsOpen(true);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TeachingTip", e);
    }

    return false;
}

} // anonymous namespace

/* static */
wxRichToolTipImpl*
wxRichToolTipImpl::Create(const wxString& title, const wxString& message)
{
    return new wxWinUIRichToolTipImpl(title, message);
}

#endif // wxUSE_RICHTOOLTIP && wxUSE_WINUI3
