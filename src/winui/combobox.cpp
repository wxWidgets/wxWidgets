/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/combobox.cpp
// Purpose:     wxWinUI wxComboBox implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

long wxWinUIClampComboTextPos(long pos, long len)
{
    if ( pos < 0 )
        return len;
    if ( pos > len )
        return len;
    return pos;
}

MUXC::TextBox wxWinUIFindTextBox(MUX::DependencyObject const& root)
{
    using MUX::Media::VisualTreeHelper;

    const int count = VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        auto child = VisualTreeHelper::GetChild(root, i);
        if ( auto tb = child.try_as<MUXC::TextBox>() )
            return tb;
        if ( auto tb = wxWinUIFindTextBox(child) )
            return tb;
    }

    return nullptr;
}

// The inner TextBox of an editable ComboBox only exists once the control
// template has been applied, so resolve (and cache) it lazily.
MUXC::TextBox wxWinUIComboEditBox(wxWinUIChoiceImpl *impl)
{
    if ( !impl || !impl->comboBox )
        return nullptr;

    if ( !impl->editBox )
    {
        try
        {
            impl->editBox = wxWinUIFindTextBox(impl->comboBox);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return impl->editBox;
}

} // namespace

wxComboBox::wxComboBox()
    : m_insertionPoint(0),
      m_selectionStart(0),
      m_selectionEnd(0),
      m_editable(true),
      m_allowTextEvents(true)
{
}

wxComboBox::wxComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n,
                       const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxComboBox()
{
    Create(parent, id, value, pos, size, n, choices, style, validator, name);
}

wxComboBox::wxComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxComboBox()
{
    Create(parent, id, value, pos, size, choices, style, validator, name);
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // The combobox-specific flags would trip wxChoice's style assert; they are
    // handled here, not by the base class.
    const long comboFlags = style & (wxCB_DROPDOWN | wxCB_SIMPLE | wxCB_READONLY);
    if ( !wxChoice::Create(parent, id, pos, size, n, choices,
                           style & ~comboFlags, validator, name) )
        return false;

    // Restore them so that HasFlag(wxCB_READONLY) etc. work as under wxMSW.
    m_windowStyle |= comboFlags;

    m_editable = (comboFlags & wxCB_READONLY) == 0;

    try
    {
        auto& combo = m_winui->comboBox;
        combo.IsEditable(m_editable);

        m_winui->textChangedCallbackToken = combo.RegisterPropertyChangedCallback(
            MUXC::ComboBox::TextProperty(),
            [this](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( !m_winui || m_updatingPeer )
                    return;
                OnPeerTextChanged();
            });

        m_winui->textSubmittedToken = combo.TextSubmitted(
            [this](MUXC::ComboBox const&,
                   MUXC::ComboBoxTextSubmittedEventArgs const& args)
            {
                if ( !m_winui )
                    return;

                if ( HasFlag(wxTE_PROCESS_ENTER) )
                {
                    wxCommandEvent event(wxEVT_TEXT_ENTER, GetId());
                    event.SetEventObject(this);
                    event.SetString(GetValue());
                    if ( ProcessCommand(event) )
                        args.Handled(true);
                }
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox setup", e);
    }

    const int sel = FindString(value);
    if ( sel != wxNOT_FOUND )
        wxChoice::SetSelection(sel);
    DoSetValue(value, SetValue_NoEvent);

    return true;
}

bool wxComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, value, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

void wxComboBox::Clear()
{
    wxChoice::Clear();
    DoSetValue(wxString(), SetValue_SendEvent);
}

wxString wxComboBox::GetValue() const
{
    return m_value;
}

void wxComboBox::SetValue(const wxString& value)
{
    DoSetValue(value, SetValue_SendEvent);
}

void wxComboBox::Popup()
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    try
    {
        m_winui->comboBox.IsDropDownOpen(true);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox popup", e);
    }
}

void wxComboBox::Dismiss()
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    try
    {
        m_winui->comboBox.IsDropDownOpen(false);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox dismiss", e);
    }
}

void wxComboBox::SetSelection(int n)
{
    wxChoice::SetSelection(n);

    // Programmatic selection updates the text without sending any event, as
    // under wxMSW.
    if ( n != wxNOT_FOUND )
        DoSetValue(wxChoice::GetString(n), SetValue_NoEvent);
}

void wxComboBox::SetSelection(long from, long to)
{
    const long len = m_value.length();
    from = wxWinUIClampComboTextPos(from, len);
    to = to == -1 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    m_selectionStart = from;
    m_selectionEnd = to;
    m_insertionPoint = to;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.Select(static_cast<int32_t>(from), static_cast<int32_t>(to - from));
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox select", e);
        }
    }
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    long start = m_selectionStart;
    long end = m_selectionEnd;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            start = tb.SelectionStart();
            end = start + tb.SelectionLength();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    if ( from )
        *from = start;
    if ( to )
        *to = end;
}

bool wxComboBox::IsEditable() const
{
    return m_editable;
}

void wxComboBox::SetEditable(bool editable)
{
    m_editable = editable;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.IsReadOnly(!editable);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox editable", e);
        }
    }
}

void wxComboBox::WriteText(const wxString& text)
{
    if ( !m_editable )
        return;

    long from = m_selectionStart;
    long to = m_selectionEnd;
    if ( from > to )
        wxSwap(from, to);
    if ( from == to )
        from = to = m_insertionPoint;

    const wxString value = m_value.Left(from) + text + m_value.Mid(to);
    DoSetValue(value, SetValue_SendEvent);
    m_insertionPoint = from + text.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
}

void wxComboBox::Remove(long from, long to)
{
    if ( !m_editable )
        return;

    const long len = m_value.length();
    from = wxWinUIClampComboTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    DoSetValue(m_value.Left(from) + m_value.Mid(to), SetValue_SendEvent);
    m_insertionPoint = from;
    m_selectionStart = m_selectionEnd = from;
}

void wxComboBox::Copy()
{
    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.CopySelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox copy", e);
        }
    }
}

void wxComboBox::Cut()
{
    if ( !m_editable )
        return;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.CutSelectionToClipboard();
            return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox cut", e);
        }
    }

    RemoveSelection();
}

void wxComboBox::Paste()
{
    if ( !m_editable )
        return;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.PasteFromClipboard();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox paste", e);
        }
    }
}

void wxComboBox::Undo()
{
    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.Undo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox undo", e);
        }
    }
}

void wxComboBox::Redo()
{
    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.Redo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox redo", e);
        }
    }
}

bool wxComboBox::CanUndo() const
{
    if ( !m_editable )
        return false;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            return tb.CanUndo();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return false;
}

bool wxComboBox::CanRedo() const
{
    if ( !m_editable )
        return false;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            return tb.CanRedo();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return false;
}

void wxComboBox::SetInsertionPoint(long pos)
{
    m_insertionPoint = wxWinUIClampComboTextPos(pos, m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;

    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            tb.Select(static_cast<int32_t>(m_insertionPoint), 0);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
}

long wxComboBox::GetInsertionPoint() const
{
    if ( auto tb = wxWinUIComboEditBox(m_winui.get()) )
    {
        try
        {
            return tb.SelectionStart();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return m_insertionPoint;
}

long wxComboBox::GetLastPosition() const
{
    return m_value.length();
}

void wxComboBox::Command(wxCommandEvent& event)
{
    if ( event.GetInt() != wxNOT_FOUND )
        SetSelection(event.GetInt());
    ProcessCommand(event);
}

wxSize wxComboBox::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(180, 32), const_cast<wxComboBox *>(this));
}

wxString wxComboBox::DoGetValue() const
{
    return m_value;
}

void wxComboBox::DoSetValue(const wxString& value, int flags)
{
    m_value = value;
    m_insertionPoint = m_value.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;

    ApplyTextToPeer();

    if ( flags & SetValue_SendEvent )
        SendTextEvent();
}

void wxComboBox::ApplyTextToPeer()
{
    if ( !m_winui || !m_winui->comboBox )
        return;

    m_updatingPeer = true;
    try
    {
        m_winui->comboBox.Text(wxWinUIToHString(m_value));
        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox text", e);
    }
    m_updatingPeer = false;
}

void wxComboBox::OnPeerTextChanged()
{
    const wxString text = wxWinUIFromHString(m_winui->comboBox.Text());
    if ( text == m_value )
        return;

    m_value = text;
    m_insertionPoint = m_value.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;

    SendTextEvent();
}

void wxComboBox::SendSelectionEvent()
{
    const int sel = wxChoice::GetSelection();
    if ( sel != wxNOT_FOUND )
    {
        m_value = wxChoice::GetString(sel);
        m_insertionPoint = m_value.length();
        m_selectionStart = m_selectionEnd = m_insertionPoint;
    }

    // wxMSW sends wxEVT_COMBOBOX for the selection followed by wxEVT_TEXT for
    // the resulting text change.
    wxCommandEvent event(wxEVT_COMBOBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(sel);
    if ( sel != wxNOT_FOUND )
        event.SetString(m_value);
    ProcessCommand(event);

    SendTextEvent();
}

void wxComboBox::EnableTextChangedEvents(bool enable)
{
    m_allowTextEvents = enable;
}

void wxComboBox::SendTextEvent()
{
    if ( !m_allowTextEvents )
        return;

    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxComboBox::ClampTextState()
{
    m_insertionPoint = wxWinUIClampComboTextPos(m_insertionPoint, m_value.length());
    m_selectionStart = wxWinUIClampComboTextPos(m_selectionStart, m_value.length());
    m_selectionEnd = wxWinUIClampComboTextPos(m_selectionEnd, m_value.length());
}

#endif // wxUSE_COMBOBOX
