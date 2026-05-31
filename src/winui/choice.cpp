/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/choice.cpp
// Purpose:     wxWinUI wxChoice implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/validate.h"
#endif

#include "private.h"

class wxWinUIChoiceImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::ComboBox comboBox{ nullptr };
    winrt::event_token selectionChangedToken{};
};

wxChoice::wxChoice()
    : m_selection(wxNOT_FOUND),
      m_updatingPeer(false)
{
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   int n,
                   const wxString choices[],
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, n, choices, style, validator, name);
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, choices, style, validator, name);
}

wxChoice::~wxChoice()
{
    Clear();
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n,
                      const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxASSERT_MSG( !(style & wxCB_DROPDOWN) &&
                  !(style & wxCB_READONLY) &&
                  !(style & wxCB_SIMPLE),
                  wxT("this style flag is ignored by wxChoice") );

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUIChoiceImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->comboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox();
        m_winui->comboBox.Background(wxWinUIBrush(255, 255, 255));
        m_winui->comboBox.BorderBrush(wxWinUIBrush(128, 128, 128));
        m_winui->comboBox.Foreground(wxWinUIBrush(32, 32, 32));
        m_winui->comboBox.IsEditable(false);
        m_winui->comboBox.PlaceholderText(wxWinUIToHString("Select an item"));
        m_winui->comboBox.MaxDropDownHeight(240);

        m_winui->selectionChangedToken = m_winui->comboBox.SelectionChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&)
            {
                if ( !m_winui || m_updatingPeer )
                    return;

                m_selection = m_winui->comboBox.SelectedIndex();
                SendSelectionEvent();
            });

        m_winui->host.SetContent(m_winui->comboBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox creation", e);
        return false;
    }

    if ( n && choices )
        Append(n, choices);

    SetInitialSize(size);
    return true;
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

void wxChoice::SetLabel(const wxString& label)
{
    const int sel = FindString(label);
    SetSelection(sel);
    wxControl::SetLabel(label);
}

unsigned int wxChoice::GetCount() const
{
    return m_items.GetCount();
}

int wxChoice::GetSelection() const
{
    return m_selection;
}

void wxChoice::SetSelection(int n)
{
    if ( n < 0 || static_cast<unsigned int>(n) >= m_items.GetCount() )
        n = wxNOT_FOUND;

    m_selection = n;
    ApplySelectionToPeer();
}

int wxChoice::FindString(const wxString& s, bool bCase) const
{
    for ( unsigned int i = 0; i < m_items.GetCount(); ++i )
    {
        if ( m_items[i].IsSameAs(s, bCase) )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_items.GetCount(), wxString(), wxT("invalid choice index") );
    return m_items[n];
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_items.GetCount(), wxT("invalid choice index") );
    m_items[n] = s;
    ApplyItemsToPeer();
}

void wxChoice::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( n < m_items.GetCount(), wxT("invalid choice index") );

    m_items.RemoveAt(n);
    m_clientData.erase(m_clientData.begin() + n);

    if ( m_selection == static_cast<int>(n) )
        m_selection = wxNOT_FOUND;
    else if ( m_selection > static_cast<int>(n) )
        --m_selection;

    ApplyItemsToPeer();
}

void wxChoice::DoClear()
{
    m_items.Clear();
    m_clientData.clear();
    m_selection = wxNOT_FOUND;
    ApplyItemsToPeer();
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter& items,
                            unsigned int pos,
                            void **clientData,
                            wxClientDataType type)
{
    wxCHECK_MSG( pos <= m_items.GetCount(), wxNOT_FOUND, wxT("invalid choice index") );

    int last = wxNOT_FOUND;
    for ( unsigned int i = 0; i < items.GetCount(); ++i )
    {
        unsigned int insertPos = pos + i;
        if ( HasFlag(wxCB_SORT) )
        {
            insertPos = 0;
            while ( insertPos < m_items.GetCount() &&
                    m_items[insertPos].CmpNoCase(items[i]) <= 0 )
            {
                ++insertPos;
            }
        }

        m_items.Insert(items[i], insertPos);
        m_clientData.insert(m_clientData.begin() + insertPos, nullptr);
        AssignNewItemClientData(insertPos, clientData, i, type);

        if ( m_selection >= static_cast<int>(insertPos) )
            ++m_selection;

        last = static_cast<int>(insertPos);
    }

    ApplyItemsToPeer();
    return last;
}

void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( n < m_clientData.size(), wxT("invalid choice index") );
    m_clientData[n] = clientData;
}

void* wxChoice::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( n < m_clientData.size(), nullptr, wxT("invalid choice index") );
    return m_clientData[n];
}

wxSize wxChoice::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(180, 32), const_cast<wxChoice *>(this));
}

void wxChoice::ApplyItemsToPeer()
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    m_updatingPeer = true;
    try
    {
        auto items = m_winui->comboBox.Items();
        items.Clear();
        for ( unsigned int i = 0; i < m_items.GetCount(); ++i )
        {
            winrt::Microsoft::UI::Xaml::Controls::ComboBoxItem item;
            item.Content(winrt::box_value(wxWinUIToHString(m_items[i])));
            item.Foreground(wxWinUIBrush(32, 32, 32));
            items.Append(item);
        }

        ApplySelectionToPeer();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox item update", e);
    }
    m_updatingPeer = false;
}

void wxChoice::ApplySelectionToPeer()
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    m_updatingPeer = true;
    m_winui->comboBox.SelectedIndex(m_selection);
    m_updatingPeer = false;
}

void wxChoice::SendSelectionEvent()
{
    wxCommandEvent event(wxEVT_CHOICE, GetId());
    event.SetEventObject(this);
    event.SetInt(m_selection);
    if ( m_selection != wxNOT_FOUND )
        event.SetString(m_items[m_selection]);
    ProcessCommand(event);
}

#endif // wxUSE_CHOICE
