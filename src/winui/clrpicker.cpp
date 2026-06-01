/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/clrpicker.cpp
// Purpose:     wxWinUI colour picker widget (WinUI DropDownButton + ColorPicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COLOURPICKERCTRL

#include "wx/clrpicker.h"

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

namespace
{

winrt::Windows::UI::Color wxWinUIToColor(const wxColour& c)
{
    winrt::Windows::UI::Color color{};
    color.A = c.IsOk() ? c.Alpha() : 255;
    color.R = c.IsOk() ? c.Red() : 0;
    color.G = c.IsOk() ? c.Green() : 0;
    color.B = c.IsOk() ? c.Blue() : 0;
    return color;
}

wxColour wxWinUIFromColor(const winrt::Windows::UI::Color& c)
{
    return wxColour(c.R, c.G, c.B, c.A);
}

} // namespace

class wxWinUIColourButtonImpl
{
public:
    wxWinUIControlHost host;
    MUXC::DropDownButton button{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Border swatch{ nullptr };
    MUXC::ColorPicker picker{ nullptr };
    MUXC::Flyout flyout{ nullptr };
    winrt::event_token colorChangedToken{};
    winrt::event_token closedToken{};
    bool updating = false;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUIColourButton, wxControl);

wxWinUIColourButton::wxWinUIColourButton()
{
}

wxWinUIColourButton::wxWinUIColourButton(wxWindow *parent, wxWindowID id,
                                         const wxColour& col, const wxPoint& pos,
                                         const wxSize& size, long style,
                                         const wxValidator& validator,
                                         const wxString& name)
{
    Create(parent, id, col, pos, size, style, validator, name);
}

wxWinUIColourButton::~wxWinUIColourButton() = default;

bool wxWinUIColourButton::Create(wxWindow *parent, wxWindowID id,
                                 const wxColour& col, const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxValidator& validator,
                                 const wxString& name)
{
    const long ctrlStyle = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, ctrlStyle, validator, name) )
        return false;

    m_colour = col.IsOk() ? col : *wxBLACK;

    m_winui.reset(new wxWinUIColourButtonImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        // The button face shows a colour swatch.
        m_winui->swatch = winrt::Microsoft::UI::Xaml::Controls::Border();
        m_winui->swatch.Width(FromDIP(28));
        m_winui->swatch.Height(FromDIP(16));
        m_winui->swatch.CornerRadius(
            winrt::Microsoft::UI::Xaml::CornerRadiusHelper::FromUniformRadius(3));

        // The drop-down hosts a WinUI ColorPicker.
        m_winui->picker = MUXC::ColorPicker();
        m_winui->picker.IsAlphaEnabled((style & wxCLRP_SHOW_ALPHA) != 0);

        m_winui->flyout = MUXC::Flyout();
        m_winui->flyout.Content(m_winui->picker);
        // The control's island is only as large as the button, so the flyout
        // must be allowed to escape the island root bounds (it becomes a
        // windowed popup) instead of being clipped to the button's size.
        m_winui->flyout.ShouldConstrainToRootBounds(false);

        m_winui->button = MUXC::DropDownButton();
        m_winui->button.Content(m_winui->swatch);
        m_winui->button.Flyout(m_winui->flyout);

        m_winui->colorChangedToken = m_winui->picker.ColorChanged(
            [this](MUXC::ColorPicker const&,
                   MUXC::ColorChangedEventArgs const& args)
            {
                if ( !m_winui || m_winui->updating )
                    return;
                m_colour = wxWinUIFromColor(args.NewColor());
                if ( m_winui->swatch )
                    m_winui->swatch.Background(wxWinUIBrush(m_colour.Red(),
                        m_colour.Green(), m_colour.Blue(), m_colour.Alpha()));
                SendColourEvent(wxEVT_COLOURPICKER_CURRENT_CHANGED);
            });

        m_winui->closedToken = m_winui->flyout.Closed(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&)
            {
                if ( m_winui )
                    SendColourEvent(wxEVT_COLOURPICKER_CHANGED);
            });

        ApplyColourToPeer();
        m_winui->host.SetContent(m_winui->button);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ColorPicker creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxWinUIColourButton::UpdateColour()
{
    ApplyColourToPeer();
}

void wxWinUIColourButton::ApplyColourToPeer()
{
    if ( !m_winui )
        return;

    m_winui->updating = true;
    try
    {
        if ( m_winui->swatch )
            m_winui->swatch.Background(wxWinUIBrush(m_colour.Red(),
                m_colour.Green(), m_colour.Blue(), m_colour.Alpha()));
        if ( m_winui->picker )
            m_winui->picker.Color(wxWinUIToColor(m_colour));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_winui->updating = false;
    m_winui->host.ForceRender();
}

void wxWinUIColourButton::SendColourEvent(wxEventType type)
{
    wxColourPickerEvent event(this, GetId(), m_colour, type);
    HandleWindowEvent(event);
}

wxSize wxWinUIColourButton::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(72, 32), const_cast<wxWinUIColourButton*>(this));
}

#endif // wxUSE_COLOURPICKERCTRL
