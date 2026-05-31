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
        m_winui->textBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
        m_winui->textBlock.Foreground(wxWinUIBrush(32, 32, 32));
        m_winui->textBlock.TextWrapping(
            winrt::Microsoft::UI::Xaml::TextWrapping::Wrap);

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

    WXSetVisibleLabel(label);
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
    return GetTextExtent(GetLabelText());
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

    m_winui->textBlock.Text(wxWinUIToHString(wxControl::GetLabelText(m_visibleLabel)));
}

#endif // wxUSE_STATTEXT
