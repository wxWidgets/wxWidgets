/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/spinbutt.cpp
// Purpose:     wxWinUI wxSpinButton implementation (WinUI RepeatButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

// wxSpinButton RTTI (and wxSpinEvent) are provided by src/common/spinbtncmn.cpp.

class wxWinUISpinButtonImpl
{
public:
    wxWinUIControlHost host;
    MUXCP::RepeatButton up{ nullptr };
    MUXCP::RepeatButton down{ nullptr };
    winrt::event_token upToken{};
    winrt::event_token downToken{};
};

namespace
{

// Build a RepeatButton showing a Segoe Fluent chevron glyph.
MUXCP::RepeatButton wxWinUIMakeArrow(const wchar_t* glyph)
{
    MUXC::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(8);

    MUXCP::RepeatButton button;
    button.Content(icon);
    button.Padding(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromUniformLength(0));
    button.MinWidth(0);
    button.MinHeight(0);
    return button;
}

} // namespace

wxSpinButton::wxSpinButton()
{
}

wxSpinButton::wxSpinButton(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                           const wxSize& size, long style, const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxSpinButton::~wxSpinButton() = default;

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                          const wxSize& size, long style, const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_value = m_min;

    m_winui.reset(new wxWinUISpinButtonImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    const bool horizontal = (style & wxSP_HORIZONTAL) != 0;

    try
    {
        // Up/down (or left/right) chevrons, laid out in a stack panel.
        m_winui->up = wxWinUIMakeArrow(horizontal ? L"" : L"");   // right / up
        m_winui->down = wxWinUIMakeArrow(horizontal ? L"" : L""); // left / down

        m_winui->upToken = m_winui->up.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                Step(+1);
            });
        m_winui->downToken = m_winui->down.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                Step(-1);
            });

        MUXC::StackPanel panel;
        panel.Orientation(horizontal
            ? winrt::Microsoft::UI::Xaml::Controls::Orientation::Horizontal
            : winrt::Microsoft::UI::Xaml::Controls::Orientation::Vertical);
        // For a vertical control the "up" arrow goes on top; for a horizontal
        // one the "down"/left arrow goes first.
        if ( horizontal )
        {
            panel.Children().Append(m_winui->down);
            panel.Children().Append(m_winui->up);
        }
        else
        {
            panel.Children().Append(m_winui->up);
            panel.Children().Append(m_winui->down);
        }

        m_winui->host.SetContent(panel);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI SpinButton creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

int wxSpinButton::GetValue() const
{
    return m_value;
}

void wxSpinButton::SetValue(int val)
{
    m_value = wxClip(val, m_min, m_max);
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxSpinButtonBase::SetRange(minVal, maxVal);
    m_value = wxClip(m_value, m_min, m_max);
}

void wxSpinButton::SetIncrement(int value)
{
    m_increment = value;
}

int wxSpinButton::GetIncrement() const
{
    return m_increment;
}

wxSize wxSpinButton::DoGetBestSize() const
{
    const bool horizontal = (GetWindowStyle() & wxSP_HORIZONTAL) != 0;
    return wxWindow::FromDIP(horizontal ? wxSize(48, 22) : wxSize(22, 44),
                            const_cast<wxSpinButton*>(this));
}

void wxSpinButton::Step(int direction)
{
    int newValue = m_value + direction * m_increment;

    if ( newValue < m_min )
        newValue = (m_windowStyle & wxSP_WRAP) ? m_max : m_min;
    else if ( newValue > m_max )
        newValue = (m_windowStyle & wxSP_WRAP) ? m_min : m_max;

    if ( newValue == m_value )
        return;

    // The directional event is vetoable.
    wxSpinEvent eventArrow(direction > 0 ? wxEVT_SPIN_UP : wxEVT_SPIN_DOWN, GetId());
    eventArrow.SetPosition(newValue);
    eventArrow.SetEventObject(this);
    if ( HandleWindowEvent(eventArrow) && !eventArrow.IsAllowed() )
        return;

    m_value = newValue;

    wxSpinEvent eventSpin(wxEVT_SPIN, GetId());
    eventSpin.SetPosition(m_value);
    eventSpin.SetEventObject(this);
    HandleWindowEvent(eventSpin);
}

#endif // wxUSE_SPINBTN
