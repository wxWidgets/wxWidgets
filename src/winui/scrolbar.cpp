/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/scrolbar.cpp
// Purpose:     wxWinUI wxScrollBar implementation (WinUI ScrollBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SCROLLBAR

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

// wxScrollBar RTTI and the wxEVT_SCROLL_* events are provided by the common
// code (src/common/scrolbarcmn.cpp and src/common/event.cpp).

class wxWinUIScrollBarImpl
{
public:
    wxWinUIControlHost host;
    MUXCP::ScrollBar bar{ nullptr };
    winrt::event_token scrollToken{};
};

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::wxScrollBar(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                         const wxSize& size, long style,
                         const wxValidator& validator, const wxString& name)
{
    Create(parent, id, pos, size, style, validator, name);
}

wxScrollBar::~wxScrollBar() = default;

bool wxScrollBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                         const wxSize& size, long style,
                         const wxValidator& validator, const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUIScrollBarImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->bar = MUXCP::ScrollBar();
        m_winui->bar.Orientation((style & wxSB_VERTICAL)
            ? winrt::Microsoft::UI::Xaml::Controls::Orientation::Vertical
            : winrt::Microsoft::UI::Xaml::Controls::Orientation::Horizontal);

        m_winui->scrollToken = m_winui->bar.Scroll(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUXCP::ScrollEventArgs const& args)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerScroll(static_cast<int>(args.NewValue()),
                             static_cast<int>(args.ScrollEventType()));
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->bar);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ScrollBar creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    m_position = viewStart;
    ApplyToPeer();
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range,
                               int pageSize, bool WXUNUSED(refresh))
{
    m_position = position;
    m_thumbSize = thumbSize;
    m_range = range;
    m_pageSize = pageSize;
    ApplyToPeer();
}

void wxScrollBar::ApplyToPeer()
{
    if ( !m_winui || !m_winui->bar )
        return;

    m_updating = true;
    try
    {
        // The WinUI ScrollBar value range is [Minimum, Maximum]; the thumb
        // occupies ViewportSize, so the maximum reachable value is
        // range - thumbSize.
        const int maxValue = wxMax(0, m_range - m_thumbSize);
        m_winui->bar.Minimum(0);
        m_winui->bar.Maximum(maxValue);
        m_winui->bar.ViewportSize(m_thumbSize);
        m_winui->bar.SmallChange(1);
        m_winui->bar.LargeChange(wxMax(1, m_pageSize));
        m_winui->bar.Value(wxClip(m_position, 0, maxValue));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxScrollBar::OnPeerScroll(int newValue, int eventType)
{
    m_position = newValue;

    const int orient = (GetWindowStyle() & wxSB_VERTICAL) ? wxVERTICAL
                                                          : wxHORIZONTAL;

    // Map the WinUI ScrollEventType to the corresponding wx scroll event.
    // (Values per Microsoft.UI.Xaml.Controls.Primitives.ScrollEventType.)
    wxEventType type;
    switch ( eventType )
    {
        case 0:  type = wxEVT_SCROLL_LINEUP;       break; // SmallDecrement
        case 1:  type = wxEVT_SCROLL_LINEDOWN;     break; // SmallIncrement
        case 2:  type = wxEVT_SCROLL_PAGEUP;       break; // LargeDecrement
        case 3:  type = wxEVT_SCROLL_PAGEDOWN;     break; // LargeIncrement
        case 4:  type = wxEVT_SCROLL_THUMBRELEASE; break; // ThumbPosition
        case 5:  type = wxEVT_SCROLL_THUMBTRACK;   break; // ThumbTrack
        case 6:  type = wxEVT_SCROLL_TOP;          break; // First
        case 7:  type = wxEVT_SCROLL_BOTTOM;       break; // Last
        default: type = wxEVT_SCROLL_CHANGED;      break; // EndScroll
    }

    wxScrollEvent event(type, GetId(), m_position, orient);
    event.SetEventObject(this);
    HandleWindowEvent(event);

    // Always follow up with a CHANGED event so handlers that only watch for it
    // (the common case) are notified.
    if ( type != wxEVT_SCROLL_CHANGED )
    {
        wxScrollEvent changed(wxEVT_SCROLL_CHANGED, GetId(), m_position, orient);
        changed.SetEventObject(this);
        HandleWindowEvent(changed);
    }
}

wxSize wxScrollBar::DoGetBestSize() const
{
    const bool vertical = (GetWindowStyle() & wxSB_VERTICAL) != 0;
    return wxWindow::FromDIP(vertical ? wxSize(16, 140) : wxSize(140, 16),
                            const_cast<wxScrollBar*>(this));
}

#endif // wxUSE_SCROLLBAR
