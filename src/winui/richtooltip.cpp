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

#include "wx/winui/private/tlwhost.h"

#include <chrono>
#include <winrt/Microsoft.UI.Dispatching.h>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

// Keeps the TeachingTip (and its anchor) alive until it is closed (the
// wxRichToolTip object itself is typically destroyed right after ShowFor()
// returns); removes them from the shared island and deletes itself once the
// tip goes away.
class wxWinUITeachingTipHost
{
public:
    winrt::Microsoft::UI::Xaml::Controls::Canvas root{ nullptr };
    MUXC::TeachingTip tip{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement anchor{ nullptr };
    winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer timer{ nullptr };

    ~wxWinUITeachingTipHost()
    {
        try
        {
            if ( timer )
                timer.Stop();

            // Detach from the shared island's tree.
            if ( root )
            {
                uint32_t index = 0;
                if ( tip && root.Children().IndexOf(tip, index) )
                    root.Children().RemoveAt(index);
                if ( anchor && root.Children().IndexOf(anchor, index) )
                    root.Children().RemoveAt(index);
            }
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

    if ( !GetHwndOf(win) )
        return false;

    // The tip lives on the window's shared per-TLW island.
    wxWinUITopLevelHost * const tlwHost =
        wxWinUITopLevelHost::ForWindow(win, true);
    if ( !tlwHost || !tlwHost->GetXamlRoot() )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;

        auto host = new wxWinUITeachingTipHost;
        host->root = tlwHost->Root();

        MUXC::TeachingTip tip;
        tip.Title(wxWinUIToHString(m_title));
        tip.Subtitle(wxWinUIToHString(m_message));
        tip.IsLightDismissEnabled(true);
        tip.ShouldConstrainToRootBounds(false);
        tip.PreferredPlacement(MUXC::TeachingTipPlacementMode::Top);

        // Anchor the tip on the window's own slot when it has one; any other
        // window gets a zero-size anchor at its top center, so the placement
        // is right in both cases.
        winrt::Microsoft::UI::Xaml::FrameworkElement target{ nullptr };
        if ( wxWinUISlot * const slot = tlwHost->FindSlot(win) )
        {
            target = slot->GetContainer();
        }
        else
        {
            const wxRect rect = win->GetScreenRect();
            const wxPoint dip = tlwHost->ScreenToRootDIP(
                wxPoint(rect.x + rect.width/2, rect.y));

            MUXC::Border anchor;
            anchor.IsHitTestVisible(false);
            anchor.Width(1);
            anchor.Height(1);
            MUXC::Canvas::SetLeft(anchor, dip.x);
            MUXC::Canvas::SetTop(anchor, dip.y);
            host->root.Children().Append(anchor);
            host->anchor = anchor;
            target = anchor;
        }
        tip.Target(target);

        host->root.Children().Append(tip);
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
