/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/stattext.cpp
// Purpose:     wxWinUI wxStaticText implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#include "private.h"

class wxWinUIStaticTextImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock{ nullptr };
};

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticText::~wxStaticText() = default;

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    UpdateLabelOrig(label);
    m_visibleLabel = label;

    m_winui.reset(new wxWinUIStaticTextImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;
        m_winui->textBlock = MUX::Controls::TextBlock();
        // Don't wrap by default: classic wxStaticText shows a single line and
        // wrapping here makes the last word disappear when the measured width is
        // slightly too small.
        m_winui->textBlock.TextWrapping(MUX::TextWrapping::NoWrap);
        m_winui->textBlock.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        m_winui->textBlock.VerticalAlignment(MUX::VerticalAlignment::Top);

        // Honor the wxStaticText alignment flags.
        MUX::TextAlignment align = MUX::TextAlignment::Left;
        if ( style & wxALIGN_RIGHT )
            align = MUX::TextAlignment::Right;
        else if ( style & wxALIGN_CENTRE_HORIZONTAL )
            align = MUX::TextAlignment::Center;
        m_winui->textBlock.TextAlignment(align);

        // Honor the wxST_ELLIPSIZE_* flags (WinUI only offers ellipsis at the
        // end, so all variants map to it).
        if ( style & (wxST_ELLIPSIZE_START | wxST_ELLIPSIZE_MIDDLE |
                      wxST_ELLIPSIZE_END) )
            m_winui->textBlock.TextTrimming(MUX::TextTrimming::CharacterEllipsis);
        else
            m_winui->textBlock.TextTrimming(MUX::TextTrimming::None);

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->textBlock);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBlock creation", e);
        return false;
    }

    return true;
}

void wxStaticText::SetLabel(const wxString& label)
{
    if ( !UpdateLabelOrig(label) )
        return;

    InvalidateBestSize();
    WXSetVisibleLabel(label);
    if ( GetContainingSizer() )
        Refresh();
    else
        AutoResizeIfNecessary();
}

bool wxStaticText::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    UpdateWinUIContent();
    AutoResizeIfNecessary();
    return rc;
}

#if wxUSE_MARKUP
bool wxStaticText::DoSetLabelMarkup(const wxString& markup)
{
    return wxControlBase::DoSetLabelMarkup(markup);
}
#endif

wxSize wxStaticText::DoGetBestClientSize() const
{
    // WinUI's default font renders slightly taller than wx measures, so add a
    // little headroom to avoid clipping the text vertically.
    wxSize best = GetTextExtent(GetLabelText());
    best.x += FromDIP(4);
    best.y += FromDIP(8);
    return best;
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return m_visibleLabel;
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    m_visibleLabel = str;
    UpdateWinUIContent();
}

void wxStaticText::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->textBlock )
        return;

    const wxSize sizeDIP = ToDIP(GetClientSize());
    if ( sizeDIP.x > 0 )
        m_winui->textBlock.Width(sizeDIP.x);

    m_winui->textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(m_visibleLabel)));
    m_winui->textBlock.UpdateLayout();
    m_winui->host.ForceRender();
}

#endif // wxUSE_STATTEXT
