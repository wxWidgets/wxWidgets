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
    namespace MUX = winrt::Microsoft::UI::Xaml;

    MUXC::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(12);

    MUXCP::RepeatButton button;
    button.Content(icon);
    button.Padding(MUX::ThicknessHelper::FromUniformLength(0));
    button.MinWidth(0);
    button.MinHeight(0);
    // Fill the cell so the two buttons together cover the whole control, like
    // the inline spin buttons of a WinUI NumberBox.
    button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
    button.VerticalAlignment(MUX::VerticalAlignment::Stretch);
    button.HorizontalContentAlignment(MUX::HorizontalAlignment::Center);
    button.VerticalContentAlignment(MUX::VerticalAlignment::Center);
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


    try
    {
        // Two chevrons side by side, like the WinUI NumberBox inline spin
        // buttons (down on the left, up on the right), regardless of the wx
        // orientation style.
        m_winui->up = wxWinUIMakeArrow(L"\uE70E");   // ChevronUp
        m_winui->down = wxWinUIMakeArrow(L"\uE70D"); // ChevronDown

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

        // Lay the two buttons out in two equal columns so they fill the control.
        namespace MUX = winrt::Microsoft::UI::Xaml;
        MUXC::Grid grid;
        auto starLength = MUX::GridLengthHelper::FromValueAndType(
            1, MUX::GridUnitType::Star);

        MUXC::ColumnDefinition c1, c2;
        c1.Width(starLength);
        c2.Width(starLength);
        grid.ColumnDefinitions().Append(c1);
        grid.ColumnDefinitions().Append(c2);
        MUXC::Grid::SetColumn(m_winui->down, 0);
        MUXC::Grid::SetColumn(m_winui->up, 1);

        grid.Children().Append(m_winui->up);
        grid.Children().Append(m_winui->down);

        m_winui->host.SetContent(grid);
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
    // The buttons are always laid out side by side (WinUI style), so the
    // control is wider than tall regardless of the wx orientation.
    return wxWindow::FromDIP(wxSize(64, 28), const_cast<wxSpinButton*>(this));
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
