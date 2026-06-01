/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/srchctrl.cpp
// Purpose:     wxWinUI wxSearchCtrl implementation (WinUI AutoSuggestBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

long wxWinUIClampPos(long pos, long len)
{
    if ( pos < 0 || pos > len )
        return len;
    return pos;
}

} // namespace

class wxWinUISearchCtrlImpl
{
public:
    wxWinUIControlHost host;
    MUXC::AutoSuggestBox box{ nullptr };
    winrt::event_token textChangedToken{};
    winrt::event_token querySubmittedToken{};
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase);

wxSearchCtrl::wxSearchCtrl()
{
}

wxSearchCtrl::wxSearchCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
                           const wxPoint& pos, const wxSize& size, long style,
                           const wxValidator& validator, const wxString& name)
{
    Create(parent, id, value, pos, size, style, validator, name);
}

wxSearchCtrl::~wxSearchCtrl() = default;

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id, const wxString& value,
                          const wxPoint& pos, const wxSize& size, long style,
                          const wxValidator& validator, const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = value;
    m_insertionPoint = m_selectionStart = m_selectionEnd = m_value.length();

    m_winui.reset(new wxWinUISearchCtrlImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->box = MUXC::AutoSuggestBox();
        m_winui->box.QueryIcon(MUXC::SymbolIcon(winrt::Microsoft::UI::Xaml::Controls::Symbol::Find));

        m_winui->textChangedToken = m_winui->box.TextChanged(
            [this](MUXC::AutoSuggestBox const& sender,
                   MUXC::AutoSuggestBoxTextChangedEventArgs const& args)
            {
                if ( !m_winui || m_updatingPeer )
                    return;
                if ( args.Reason() != MUXC::AutoSuggestionBoxTextChangeReason::UserInput )
                {
                    m_value = wxWinUIFromHString(sender.Text());
                    return;
                }

                m_value = wxWinUIFromHString(sender.Text());
                m_insertionPoint = m_selectionStart = m_selectionEnd = m_value.length();
                ApplySuggestions(m_suggestions);

                wxCommandEvent event(wxEVT_TEXT, GetId());
                event.SetEventObject(this);
                event.SetString(m_value);
                ProcessCommand(event);
            });

        m_winui->querySubmittedToken = m_winui->box.QuerySubmitted(
            [this](MUXC::AutoSuggestBox const&,
                   MUXC::AutoSuggestBoxQuerySubmittedEventArgs const& args)
            {
                if ( !m_winui )
                    return;
                m_value = wxWinUIFromHString(args.QueryText());

                wxCommandEvent event(wxEVT_SEARCH, GetId());
                event.SetEventObject(this);
                event.SetString(m_value);
                ProcessCommand(event);
            });

        ApplyValueToPeer();
        m_winui->host.SetContent(m_winui->box);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI AutoSuggestBox creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

// ----------------------------------------------------------------------------
// wxSearchCtrlBase
// ----------------------------------------------------------------------------

void wxSearchCtrl::SetMenu(wxMenu *menu)
{
    m_menu = menu;
}

wxMenu *wxSearchCtrl::GetMenu()
{
    return m_menu;
}

void wxSearchCtrl::ShowSearchButton(bool show)
{
    m_searchButtonVisible = show;
    if ( m_winui && m_winui->box )
    {
        if ( show )
            m_winui->box.QueryIcon(MUXC::SymbolIcon(
                winrt::Microsoft::UI::Xaml::Controls::Symbol::Find));
        else
            m_winui->box.QueryIcon(nullptr);
        m_winui->host.ForceRender();
    }
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    return m_searchButtonVisible;
}

void wxSearchCtrl::ShowCancelButton(bool show)
{
    // The AutoSuggestBox shows its own clear affordance; just track the flag.
    m_cancelButtonVisible = show;
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
    return m_cancelButtonVisible;
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    m_descriptiveText = text;
    if ( m_winui && m_winui->box )
    {
        m_winui->box.PlaceholderText(wxWinUIToHString(text));
        m_winui->host.ForceRender();
    }
}

wxString wxSearchCtrl::GetDescriptiveText() const
{
    return m_descriptiveText;
}

// ----------------------------------------------------------------------------
// wxTextEntry
// ----------------------------------------------------------------------------

void wxSearchCtrl::WriteText(const wxString& text)
{
    long from = wxMin(m_selectionStart, m_selectionEnd);
    long to = wxMax(m_selectionStart, m_selectionEnd);
    if ( from == to )
        from = to = m_insertionPoint;

    m_value = m_value.Left(from) + text + m_value.Mid(to);
    m_insertionPoint = from + text.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyValueToPeer();
}

void wxSearchCtrl::Remove(long from, long to)
{
    const long len = m_value.length();
    from = wxWinUIClampPos(from, len);
    to = to < 0 ? len : wxWinUIClampPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    m_value = m_value.Left(from) + m_value.Mid(to);
    m_insertionPoint = m_selectionStart = m_selectionEnd = from;
    ApplyValueToPeer();
}

void wxSearchCtrl::Copy()
{
}

void wxSearchCtrl::Cut()
{
}

void wxSearchCtrl::Paste()
{
}

void wxSearchCtrl::Undo()
{
}

void wxSearchCtrl::Redo()
{
}

bool wxSearchCtrl::CanUndo() const
{
    return false;
}

bool wxSearchCtrl::CanRedo() const
{
    return false;
}

void wxSearchCtrl::SetInsertionPoint(long pos)
{
    m_insertionPoint = wxWinUIClampPos(pos, m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
}

long wxSearchCtrl::GetInsertionPoint() const
{
    return m_insertionPoint;
}

long wxSearchCtrl::GetLastPosition() const
{
    return m_value.length();
}

void wxSearchCtrl::SetSelection(long from, long to)
{
    const long len = m_value.length();
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = len;
    }
    else
    {
        from = wxWinUIClampPos(from, len);
        to = wxWinUIClampPos(to, len);
    }
    m_selectionStart = from;
    m_selectionEnd = to;
    m_insertionPoint = to;
}

void wxSearchCtrl::GetSelection(long *from, long *to) const
{
    if ( from )
        *from = m_selectionStart;
    if ( to )
        *to = m_selectionEnd;
}

bool wxSearchCtrl::IsEditable() const
{
    return m_editable;
}

void wxSearchCtrl::SetEditable(bool editable)
{
    m_editable = editable;
    if ( m_winui && m_winui->box )
        m_winui->box.IsEnabled(editable);
}

// ----------------------------------------------------------------------------
// internals
// ----------------------------------------------------------------------------

wxSize wxSearchCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(180, 36), const_cast<wxSearchCtrl*>(this));
}

void wxSearchCtrl::DoSetValue(const wxString& value, int flags)
{
    m_value = value;
    m_insertionPoint = m_selectionStart = m_selectionEnd = m_value.length();
    ApplyValueToPeer();

    if ( flags & SetValue_SendEvent )
    {
        wxCommandEvent event(wxEVT_TEXT, GetId());
        event.SetEventObject(this);
        event.SetString(m_value);
        ProcessCommand(event);
    }
}

wxString wxSearchCtrl::DoGetValue() const
{
    return m_value;
}

WXHWND wxSearchCtrl::GetEditHWND() const
{
    return GetHWND();
}

void wxSearchCtrl::ApplyValueToPeer()
{
    if ( !m_winui || !m_winui->box )
        return;

    m_updatingPeer = true;
    try
    {
        m_winui->box.Text(wxWinUIToHString(m_value));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updatingPeer = false;
    m_winui->host.ForceRender();
}

bool wxSearchCtrl::DoAutoCompleteStrings(const wxArrayString& choices)
{
    m_suggestions = choices;
    ApplySuggestions(choices);
    return true;
}

void wxSearchCtrl::ApplySuggestions(const wxArrayString& choices)
{
    if ( !m_winui || !m_winui->box )
        return;

    // The AutoSuggestBox can only accept an item source once it is part of a
    // XamlRoot (i.e. after it has been realized in the island); setting it
    // earlier throws and leaves the control in a state that crashes on the next
    // layout pass.
    try
    {
        if ( !m_winui->box.XamlRoot() )
            return;

        auto items = winrt::single_threaded_vector<
            winrt::Windows::Foundation::IInspectable>();
        for ( size_t i = 0; i < choices.GetCount(); ++i )
        {
            // Show only the suggestions matching the current text.
            if ( m_value.empty() ||
                 choices[i].Lower().StartsWith(m_value.Lower()) )
            {
                items.Append(winrt::box_value(wxWinUIToHString(choices[i])));
            }
        }
        m_winui->box.ItemsSource(items);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

#endif // wxUSE_SEARCHCTRL
