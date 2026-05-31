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
    if ( !wxChoice::Create(parent, id, pos, size, n, choices, style, validator, name) )
        return false;

    m_editable = (style & wxCB_READONLY) == 0;
    DoSetValue(value, SetValue_NoEvent);
    const int sel = FindString(value);
    if ( sel != wxNOT_FOUND )
        wxChoice::SetSelection(sel);

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
}

void wxComboBox::Dismiss()
{
}

void wxComboBox::SetSelection(int n)
{
    wxChoice::SetSelection(n);
    if ( n != wxNOT_FOUND )
        DoSetValue(wxChoice::GetString(n), SetValue_SendEvent);
}

void wxComboBox::SetSelection(long from, long to)
{
    const long len = m_value.length();
    from = wxWinUIClampComboTextPos(from, len);
    to = wxWinUIClampComboTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    m_selectionStart = from;
    m_selectionEnd = to;
    m_insertionPoint = to;
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    if ( from )
        *from = m_selectionStart;
    if ( to )
        *to = m_selectionEnd;
}

bool wxComboBox::IsEditable() const
{
    return m_editable;
}

void wxComboBox::SetEditable(bool editable)
{
    m_editable = editable;
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

    DoSetValue(m_value.Left(from) + text + m_value.Mid(to), SetValue_SendEvent);
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
}

void wxComboBox::Cut()
{
    if ( m_editable )
        RemoveSelection();
}

void wxComboBox::Paste()
{
}

void wxComboBox::Undo()
{
}

void wxComboBox::Redo()
{
}

bool wxComboBox::CanUndo() const
{
    return false;
}

bool wxComboBox::CanRedo() const
{
    return false;
}

void wxComboBox::SetInsertionPoint(long pos)
{
    m_insertionPoint = wxWinUIClampComboTextPos(pos, m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
}

long wxComboBox::GetInsertionPoint() const
{
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

    if ( flags & SetValue_SendEvent )
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
