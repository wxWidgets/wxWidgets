/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statbox.cpp
// Purpose:     wxWinUI wxStaticBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
#endif

#include "private.h"

class wxWinUIStaticBoxImpl
{
public:
    wxWinUIControlHost host;
};

wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBox::wxStaticBox(wxWindow *parent,
                         wxWindowID id,
                         wxWindow *label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBox::~wxStaticBox() = default;

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIStaticBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    UpdateWinUIContent();
    return true;
}

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         wxWindow *label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( label, false, wxS("Label window can't be null") );

    if ( !Create(parent, id, wxString(), pos, size, style, name) )
        return false;

    m_labelWin = label;
    m_labelWin->Reparent(this);
    PositionLabelWindow();

    return true;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    UpdateWinUIContent();
}

bool wxStaticBox::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxStaticBoxBase::SetBackgroundColour(colour) )
        return false;

    UpdateWinUIContent();
    return true;
}

bool wxStaticBox::SetForegroundColour(const wxColour& colour)
{
    if ( !wxCompositeWindowSettersOnly<wxStaticBoxBase>::SetForegroundColour(colour) )
        return false;

    UpdateWinUIContent();
    return true;
}

bool wxStaticBox::SetFont(const wxFont& font)
{
    if ( !wxCompositeWindowSettersOnly<wxStaticBoxBase>::SetFont(font) )
        return false;

    if ( m_labelWin )
        PositionLabelWindow();

    InvalidateBestSize();
    UpdateWinUIContent();
    return true;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    const int margin = FromDIP(10);

    if ( m_labelWin )
        *borderTop = m_labelWin->GetBestSize().y + FromDIP(6);
    else if ( !GetLabel().empty() )
        *borderTop = GetCharHeight() + FromDIP(8);
    else
        *borderTop = margin;

    *borderOther = margin;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    wxSize best;

    if ( GetSizer() )
        best = ClientToWindowSize(GetSizer()->CalcMin());

    const wxString label = GetLabelText(GetLabel());
    if ( !label.empty() )
    {
        wxSize labelSize = GetTextExtent(label);
        labelSize.x += FromDIP(30);
        labelSize.y += FromDIP(24);
        best.IncTo(labelSize);
    }

    if ( best.x <= 0 || best.y <= 0 )
        best = FromDIP(wxSize(80, 48));

    return best;
}

wxWindowList wxStaticBox::GetCompositeWindowParts() const
{
    wxWindowList parts;
    if ( m_labelWin )
        parts.push_back(m_labelWin);
    return parts;
}

void wxStaticBox::PositionLabelWindow()
{
    if ( !m_labelWin )
        return;

    m_labelWin->SetSize(m_labelWin->GetBestSize());
    m_labelWin->Move(FromDIP(10), 0);
}

void wxStaticBox::UpdateWinUIContent()
{
    if ( !m_winui )
        return;

    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

        MUXC::Grid root;
        root.IsHitTestVisible(false);

        MUXC::Border border;
        MUX::Thickness borderThickness{};
        borderThickness.Left = 1;
        borderThickness.Top = 1;
        borderThickness.Right = 1;
        borderThickness.Bottom = 1;
        border.BorderThickness(borderThickness);

        MUX::CornerRadius radius{};
        radius.TopLeft = 8;
        radius.TopRight = 8;
        radius.BottomRight = 8;
        radius.BottomLeft = 8;
        border.CornerRadius(radius);

        const bool dark = wxWinUIIsDarkTheme();
        border.BorderBrush(dark
            ? wxWinUIBrush(255, 255, 255, 38)
            : wxWinUIBrush(0, 0, 0, 38));

        MUX::Thickness borderMargin{};
        borderMargin.Top = GetLabel().empty() && !m_labelWin ? 0 : FromDIP(8);
        border.Margin(borderMargin);
        root.Children().Append(border);

        if ( !m_labelWin )
        {
            const wxString label = GetLabelText(GetLabel());
            if ( !label.empty() )
            {
                // Put the label in a panel with an opaque, theme-coloured
                // background so it masks the border line running behind it
                // (the classic "group box" look where the frame is broken by
                // the title), instead of the line showing through the text.
                MUXC::Border labelBg;
                labelBg.Background(dark ? wxWinUIBrush(32, 32, 32)
                                        : wxWinUIBrush(243, 243, 243));
                labelBg.VerticalAlignment(MUX::VerticalAlignment::Top);
                labelBg.HorizontalAlignment(MUX::HorizontalAlignment::Left);

                MUX::Thickness labelMargin{};
                labelMargin.Left = FromDIP(8);
                labelBg.Margin(labelMargin);

                MUX::Thickness labelPad{};
                labelPad.Left = FromDIP(4);
                labelPad.Right = FromDIP(4);
                labelBg.Padding(labelPad);

                MUXC::TextBlock text;
                text.Text(wxWinUIToHString(label));
                labelBg.Child(text);

                root.Children().Append(labelBg);
            }
        }

        m_winui->host.SetContent(root);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI StaticBox creation", e);
    }
}

#endif // wxUSE_STATBOX
