/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/radiobox.cpp
// Purpose:     wxWinUI wxRadioBox implementation (WinUI RadioButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBOX

#include "wx/radiobox.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/arrstr.h"
#endif

#include "private.h"

#include <cmath>
#include <limits>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

#if wxUSE_EXTENDED_RTTI
// The XTI version is defined in src/common/radiocmn.cpp.
#else
wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);
#endif

class wxWinUIRadioBoxImpl
{
public:
    wxWinUIControlHost host;
    MUXC::RadioButtons radio{ nullptr };
    winrt::event_token selectionChangedToken{};
    std::vector<MUXC::RadioButton> buttons;
};

wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       int n, const wxString choices[], int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, n, choices, majorDim, style,
           validator, name);
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       const wxArrayString& choices, int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, choices, majorDim, style,
           validator, name);
}

wxRadioBox::~wxRadioBox() = default;

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        int n, const wxString choices[], int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings.Clear();
    for ( int i = 0; i < n; ++i )
        m_strings.Add(choices[i]);
    return DoCreate(parent, id, title, pos, size, majorDim, style,
                    validator, name);
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices, int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings = choices;
    return DoCreate(parent, id, title, pos, size, majorDim, style,
                    validator, name);
}

bool wxRadioBox::DoCreate(wxWindow *parent, wxWindowID id, const wxString& title,
                          const wxPoint& pos, const wxSize& size, int majorDim,
                          long style, const wxValidator& validator,
                          const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(title);

    SetMajorDim(majorDim == 0 ? static_cast<int>(m_strings.GetCount()) : majorDim,
                style);

    const size_t count = m_strings.GetCount();
    m_itemEnabled.assign(count, true);
    m_itemShown.assign(count, true);
    m_selection = count ? 0 : wxNOT_FOUND;

    m_winui.reset(new wxWinUIRadioBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->radio = MUXC::RadioButtons();
        if ( !title.empty() )
            m_winui->radio.Header(winrt::box_value(wxWinUIToHString(
                wxControl::GetLabelText(title))));
        m_winui->radio.MaxColumns(static_cast<int>(GetColumnCount() > 0
                                                       ? GetColumnCount() : 1));

        m_winui->selectionChangedToken = m_winui->radio.SelectionChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUXC::SelectionChangedEventArgs const&)
            {
                if ( !m_winui || m_updating )
                    return;

                const int selection = FindSelectedItem();
                if ( selection == wxNOT_FOUND )
                    return;

                if ( selection == m_selection )
                    return;

                m_selection = selection;
                SendSelectionEvent();
            });

        RebuildItems();
        m_winui->host.SetContent(m_winui->radio);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButtons creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

unsigned int wxRadioBox::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_strings.GetCount(), wxString(), wxT("invalid radiobox index") );
    return m_strings[n];
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_strings.GetCount(), wxT("invalid radiobox index") );
    m_strings[n] = s;
    RebuildItems();
}

void wxRadioBox::SetSelection(int n)
{
    wxCHECK_RET( n == wxNOT_FOUND ||
                 (n >= 0 && static_cast<unsigned int>(n) < m_strings.GetCount()),
                 wxT("invalid radiobox index") );

    m_selection = n;
    if ( m_winui && m_winui->radio )
    {
        m_updating = true;
        m_winui->radio.SelectedIndex(n);
        m_updating = false;
        m_winui->host.ForceRender();
    }
}

int wxRadioBox::GetSelection() const
{
    return m_selection;
}

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    m_itemEnabled[n] = enable;
    if ( m_winui && n < m_winui->buttons.size() && m_winui->buttons[n] )
    {
        m_winui->buttons[n].IsEnabled(enable);
        m_winui->host.ForceRender();
    }
    return true;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    m_itemShown[n] = show;
    if ( m_winui && n < m_winui->buttons.size() && m_winui->buttons[n] )
    {
        using winrt::Microsoft::UI::Xaml::Visibility;
        m_winui->buttons[n].Visibility(show ? Visibility::Visible
                                            : Visibility::Collapsed);
        m_winui->host.ForceRender();
    }
    return true;
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    return m_itemEnabled[n];
}

bool wxRadioBox::IsItemShown(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    return m_itemShown[n];
}

wxSize wxRadioBox::DoGetBestSize() const
{
    int maxTextWidth = 0;
    try
    {
        const float inf = std::numeric_limits<float>::infinity();
        for ( unsigned int i = 0; i < m_strings.GetCount(); ++i )
        {
            MUXC::TextBlock probe;
            probe.Text(wxWinUIToHString(m_strings[i]));
            probe.Measure({ inf, inf });
            maxTextWidth = wxMax(maxTextWidth,
                                 static_cast<int>(std::ceil(probe.DesiredSize().Width)));
        }
    }
    catch ( const winrt::hresult_error& )
    {
        for ( unsigned int i = 0; i < m_strings.GetCount(); ++i )
            maxTextWidth = wxMax(maxTextWidth, GetTextExtent(m_strings[i]).x);
    }

    const int cols = GetColumnCount() > 0 ? GetColumnCount() : 1;
    const int rows = GetRowCount() > 0 ? GetRowCount() : 1;

    const int itemW = maxTextWidth + FromDIP(40);
    const int itemH = FromDIP(34);
    const int headerH = GetLabel().empty() ? 0 : FromDIP(24);

    return wxSize(cols * itemW + FromDIP(8), rows * itemH + headerH + FromDIP(8));
}

void wxRadioBox::RebuildItems()
{
    if ( !m_winui || !m_winui->radio )
        return;

    m_updating = true;
    try
    {
        auto items = m_winui->radio.Items();
        items.Clear();
        m_winui->buttons.clear();

        for ( unsigned int i = 0; i < m_strings.GetCount(); ++i )
        {
            MUXC::RadioButton button;
            button.Content(winrt::box_value(wxWinUIToHString(
                wxControl::GetLabelText(m_strings[i]))));
            if ( i < m_itemEnabled.size() )
                button.IsEnabled(m_itemEnabled[i]);
            if ( i < m_itemShown.size() && !m_itemShown[i] )
                button.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            button.Checked(
                [this, i](winrt::Windows::Foundation::IInspectable const&,
                          winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    if ( !m_winui || m_updating )
                        return;

                    const int selection = static_cast<int>(i);
                    if ( selection == m_selection )
                        return;

                    m_selection = selection;
                    m_winui->radio.SelectedIndex(selection);
                    SendSelectionEvent();
                });

            items.Append(button);
            m_winui->buttons.push_back(button);
        }

        m_winui->radio.SelectedIndex(m_selection);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioButtons item update", e);
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

int wxRadioBox::FindSelectedItem() const
{
    if ( !m_winui )
        return wxNOT_FOUND;

    for ( size_t i = 0; i < m_winui->buttons.size(); ++i )
    {
        const auto checked = m_winui->buttons[i].IsChecked();
        if ( checked && checked.Value() )
            return static_cast<int>(i);
    }

    const int selected = m_winui->radio.SelectedIndex();
    if ( selected >= 0 && static_cast<size_t>(selected) < m_winui->buttons.size() )
        return selected;

    return wxNOT_FOUND;
}

void wxRadioBox::SendSelectionEvent()
{
    wxCommandEvent event(wxEVT_RADIOBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(m_selection);
    if ( m_selection != wxNOT_FOUND )
        event.SetString(m_strings[m_selection]);
    ProcessCommand(event);
}

#endif // wxUSE_RADIOBOX
