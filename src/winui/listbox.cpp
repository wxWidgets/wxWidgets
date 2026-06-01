/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/listbox.cpp
// Purpose:     wxWinUI wxListBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/arrstr.h"
#endif

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

class wxWinUIListBoxImpl
{
public:
    wxWinUIControlHost host;
    // Use the modern WinUI 3 ListView (rounded selection pill) rather than the
    // legacy ListBox look.
    MUXC::ListView listView{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token doubleTappedToken{};
};

void wxListBox::Init()
{
    m_selection = wxNOT_FOUND;
    m_updatingPeer = false;
}

wxListBox::wxListBox()
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     int n, const wxString choices[],
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    Init();
    Create(parent, id, pos, size, n, choices, style, validator, name);
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     const wxArrayString& choices,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    Init();
    Create(parent, id, pos, size, choices, style, validator, name);
}

wxListBox::~wxListBox()
{
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    // The WinUI ListBox draws its own border, so suppress the native control
    // border to avoid an extra grey frame around the island.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUIListBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->listView = MUXC::ListView();

        if ( style & wxLB_EXTENDED )
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Extended);
        else if ( style & wxLB_MULTIPLE )
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Multiple);
        else
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Single);

        m_winui->selectionChangedToken = m_winui->listView.SelectionChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUXC::SelectionChangedEventArgs const&)
            {
                if ( !m_winui || m_updatingPeer )
                    return;

                m_selection = m_winui->listView.SelectedIndex();
                CalcAndSendEvent();
            });

        m_winui->doubleTappedToken = m_winui->listView.DoubleTapped(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const&)
            {
                if ( !m_winui )
                    return;

                const int sel = m_winui->listView.SelectedIndex();
                if ( sel != wxNOT_FOUND )
                    SendEvent(wxEVT_LISTBOX_DCLICK, sel, true);
            });

        m_winui->host.SetContent(m_winui->listView);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox creation", e);
        return false;
    }

    if ( n && choices )
        Append(n, choices);

    SetInitialSize(size);
    return true;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
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

unsigned int wxListBox::GetCount() const
{
    return m_items.GetCount();
}

wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_items.GetCount(), wxString(), wxT("invalid listbox index") );
    return m_items[n];
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_items.GetCount(), wxT("invalid listbox index") );
    m_items[n] = s;
    ApplyItemsToPeer();
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    for ( unsigned int i = 0; i < m_items.GetCount(); ++i )
    {
        if ( m_items[i].IsSameAs(s, bCase) )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

bool wxListBox::IsSelected(int n) const
{
    wxCHECK_MSG( n >= 0 && static_cast<unsigned int>(n) < m_items.GetCount(),
                 false, wxT("invalid listbox index") );

    if ( !m_winui || !m_winui->listView )
        return false;

    try
    {
        auto items = m_winui->listView.Items();
        if ( static_cast<uint32_t>(n) >= items.Size() )
            return false;

        if ( auto item = items.GetAt(n).try_as<MUXC::ListViewItem>() )
            return item.IsSelected();
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(), wxNOT_FOUND,
                 wxT("GetSelection() can't be used with multiple selection listbox") );
    return m_selection;
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

    if ( !m_winui || !m_winui->listView )
        return 0;

    try
    {
        auto items = m_winui->listView.Items();
        const uint32_t count = items.Size();
        for ( uint32_t i = 0; i < count; ++i )
        {
            if ( auto item = items.GetAt(i).try_as<MUXC::ListViewItem>() )
            {
                if ( item.IsSelected() )
                    aSelections.Add(static_cast<int>(i));
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return static_cast<int>(aSelections.GetCount());
}

void wxListBox::DoSetFirstItem(int n)
{
    wxCHECK_RET( n >= 0 && static_cast<unsigned int>(n) < m_items.GetCount(),
                 wxT("invalid listbox index") );

    if ( !m_winui || !m_winui->listView )
        return;

    try
    {
        auto items = m_winui->listView.Items();
        if ( static_cast<uint32_t>(n) < items.Size() )
            m_winui->listView.ScrollIntoView(items.GetAt(n));
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if ( n == wxNOT_FOUND )
    {
        m_selection = wxNOT_FOUND;
        if ( m_winui && m_winui->listView )
        {
            m_updatingPeer = true;
            m_winui->listView.SelectedIndex(-1);
            m_updatingPeer = false;
            m_winui->host.ForceRender();
        }
        return;
    }

    wxCHECK_RET( static_cast<unsigned int>(n) < m_items.GetCount(),
                 wxT("invalid listbox index") );

    if ( select )
        m_selection = n;
    else if ( m_selection == n )
        m_selection = wxNOT_FOUND;

    if ( !m_winui || !m_winui->listView )
        return;

    m_updatingPeer = true;
    try
    {
        if ( HasMultipleSelection() )
        {
            auto items = m_winui->listView.Items();
            if ( static_cast<uint32_t>(n) < items.Size() )
            {
                if ( auto item = items.GetAt(n).try_as<MUXC::ListViewItem>() )
                    item.IsSelected(select);
            }
        }
        else
        {
            m_winui->listView.SelectedIndex(select ? n : -1);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox selection", e);
    }
    m_updatingPeer = false;

    m_winui->host.ForceRender();
}

void wxListBox::DoClear()
{
    m_items.Clear();
    m_clientData.clear();
    m_checks.clear();
    m_selection = wxNOT_FOUND;
    ApplyItemsToPeer();
}

void wxListBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( n < m_items.GetCount(), wxT("invalid listbox index") );

    m_items.RemoveAt(n);
    m_clientData.erase(m_clientData.begin() + n);
    m_checks.erase(m_checks.begin() + n);

    if ( m_selection == static_cast<int>(n) )
        m_selection = wxNOT_FOUND;
    else if ( m_selection > static_cast<int>(n) )
        --m_selection;

    ApplyItemsToPeer();
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter& items,
                             unsigned int pos,
                             void **clientData,
                             wxClientDataType type)
{
    wxCHECK_MSG( pos <= m_items.GetCount(), wxNOT_FOUND, wxT("invalid listbox index") );

    int last = wxNOT_FOUND;
    for ( unsigned int i = 0; i < items.GetCount(); ++i )
    {
        unsigned int insertPos = pos + i;
        if ( IsSorted() )
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
        m_checks.insert(m_checks.begin() + insertPos, false);
        AssignNewItemClientData(insertPos, clientData, i, type);

        if ( m_selection >= static_cast<int>(insertPos) )
            ++m_selection;

        last = static_cast<int>(insertPos);
    }

    ApplyItemsToPeer();
    return last;
}

void wxListBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( n < m_clientData.size(), wxT("invalid listbox index") );
    m_clientData[n] = clientData;
}

void* wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( n < m_clientData.size(), nullptr, wxT("invalid listbox index") );
    return m_clientData[n];
}

wxSize wxListBox::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(140, 110), const_cast<wxListBox *>(this));
}

void wxListBox::WinUIRefreshItems()
{
    ApplyItemsToPeer();
}

void wxListBox::ApplyItemsToPeer()
{
    if ( !m_winui || !m_winui->listView )
        return;

    const bool checkable = WinUIIsCheckable();

    m_updatingPeer = true;
    try
    {
        auto items = m_winui->listView.Items();
        items.Clear();

        for ( unsigned int i = 0; i < m_items.GetCount(); ++i )
        {
            MUXC::ListViewItem item;

            if ( checkable )
            {
                MUXC::CheckBox check;
                check.Content(winrt::box_value(wxWinUIToHString(m_items[i])));
                check.IsChecked(i < m_checks.size() && m_checks[i]);

                const unsigned int index = i;
                check.Click(
                    [this, index](winrt::Windows::Foundation::IInspectable const& sender,
                                  winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        if ( !m_winui || m_updatingPeer )
                            return;

                        bool checked = false;
                        if ( auto cb = sender.try_as<MUXC::CheckBox>() )
                        {
                            if ( auto state = cb.IsChecked() )
                                checked = state.Value();
                        }
                        WinUIOnItemToggled(index, checked);
                    });

                item.Content(check);
            }
            else
            {
                item.Content(winrt::box_value(wxWinUIToHString(m_items[i])));
            }

            items.Append(item);
        }

        ApplySelectionToPeer();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox item update", e);
    }
    m_updatingPeer = false;

    m_winui->host.ForceRender();
}

void wxListBox::ApplySelectionToPeer()
{
    if ( !m_winui || !m_winui->listView )
        return;

    const bool wasUpdating = m_updatingPeer;
    m_updatingPeer = true;
    try
    {
        if ( !HasMultipleSelection() )
            m_winui->listView.SelectedIndex(m_selection);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updatingPeer = wasUpdating;
}

void wxListBox::SendSelectionEvent()
{
    CalcAndSendEvent();
}

#endif // wxUSE_LISTBOX
