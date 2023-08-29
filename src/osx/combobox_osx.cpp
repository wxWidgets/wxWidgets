/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/combobox_osx.cpp
// Purpose:     wxComboBox class using HIView ComboBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX && wxOSX_USE_COCOA

#include "wx/combobox.h"
#include "wx/osx/private.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/toplevel.h"
#endif

typedef wxWindowWithItems<wxControl, wxComboBoxBase> RealwxComboBoxBase;

wxBEGIN_EVENT_TABLE(wxComboBox, RealwxComboBoxBase)
    EVT_CHAR(wxComboBox::OnChar)
    EVT_KEY_DOWN(wxComboBox::OnKeyDown)
wxEND_EVENT_TABLE()

// work in progress

wxComboBox::~wxComboBox()
{
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    wxCArrayString chs( choices );

    return Create( parent, id, value, pos, size, chs.GetCount(),
                   chs.GetStrings(), style, validator, name );
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    DontCreatePeer();
    
    m_text = nullptr;
    m_choice = nullptr;
    
    if ( !wxControl::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    SetPeer(wxWidgetImpl::CreateComboBox( this, parent, id, nullptr, pos, size, style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

    Append(n, choices);

    // Set up the initial value, by default the first item is selected.
    if ( !value.empty() )
        SetValue(value);
    else if (n > 0)
        SetSelection( 0 );

    // Needed because it is a wxControlWithItems
    SetInitialSize( size );

    return true;
}

void wxComboBox::DelegateTextChanged( const wxString& value )
{
    SetStringSelection( value );
}

void wxComboBox::DelegateChoice( const wxString& value )
{
    SetStringSelection( value );
}

int wxComboBox::DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type)
{
    const unsigned int numItems = items.GetCount();
    for( unsigned int i = 0; i < numItems; ++i, ++pos )
    {
        unsigned int idx;

        idx = pos;
        GetComboPeer()->InsertItem( idx, items[i] );

        if (idx > m_datas.GetCount())
            m_datas.SetCount(idx);
        m_datas.Insert( nullptr, idx );
        AssignNewItemClientData(idx, clientData, i, type);
    }

    GetPeer()->SetMaximum( GetCount() );

    return pos - 1;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------
void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_datas[n] = (char*)clientData ;
}

void * wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return (void *)m_datas[n];
}

unsigned int wxComboBox::GetCount() const
{
    return GetComboPeer()->GetNumberOfItems();
}

void wxComboBox::DoDeleteOneItem(unsigned int n)
{
    m_datas.RemoveAt(n);
    GetComboPeer()->RemoveItem(n);
}

void wxComboBox::DoClear()
{
    m_datas.Clear();
    GetComboPeer()->Clear();
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    wxTextEntry::GetSelection(from, to);
}

int wxComboBox::GetSelection() const
{
    return GetComboPeer()->GetSelectedItem();
}

void wxComboBox::SetSelection(int n)
{
    GetComboPeer()->SetSelectedItem(n);
}

void wxComboBox::SetSelection(long from, long to)
{
    wxTextEntry::SetSelection(from, to);
}

int wxComboBox::FindString(const wxString& s, bool bCase) const
{
    if (!bCase)
    {
        for (unsigned i = 0; i < GetCount(); i++)
        {
            if (s.IsSameAs(GetString(i), false))
                return i;
        }
        return wxNOT_FOUND;
    }

    return GetComboPeer()->FindString(s);
}

wxString wxComboBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "Invalid combobox index" );

    return GetComboPeer()->GetStringAtIndex(n);
}

wxString wxComboBox::GetStringSelection() const
{
    const int sel = GetSelection();
    return sel == wxNOT_FOUND ? wxString() : GetString(sel);
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( HasFlag(wxCB_READONLY) )
        SetStringSelection( value ) ;
    else
        wxTextEntry::SetValue( value );
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    // Notice that we shouldn't delete and insert the item in this control
    // itself as this would also affect the client data which we need to
    // preserve here.
    const int sel = GetSelection();
    GetComboPeer()->RemoveItem(n);
    GetComboPeer()->InsertItem(n, s);
    // When selected item is removed its selection is invalidated
    // so we need to re-select it manually.
    if ( sel == int(n) )
    {
        SetSelection(n);
    }
    SetValue(s); // changing the item in the list won't update the display item
}

void wxComboBox::EnableTextChangedEvents(bool WXUNUSED(enable))
{
    // nothing to do here, events are never generated when we change the
    // control value programmatically anyhow by Cocoa
}

bool wxComboBox::OSXHandleClicked( double WXUNUSED(timestampsec) )
{
    wxCommandEvent event(wxEVT_COMBOBOX, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
    return true;
}

wxComboWidgetImpl* wxComboBox::GetComboPeer() const
{
    return dynamic_cast<wxComboWidgetImpl*> (GetPeer());
}

void wxComboBox::Popup()
{
    GetComboPeer()->Popup();
}

void wxComboBox::Dismiss()
{
    GetComboPeer()->Dismiss();
}

void wxComboBox::OnChar(wxKeyEvent& event)
{
    const int key = event.GetKeyCode();
    bool eat_key = false;

    switch (key)
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if (m_windowStyle & wxTE_PROCESS_ENTER)
            {
                wxCommandEvent event(wxEVT_TEXT_ENTER, m_windowId);
                event.SetEventObject(this);
                event.SetString(GetValue());
                if (HandleWindowEvent(event))
                    return;
            }

            {
                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
                if (tlw && tlw->GetDefaultItem())
                {
                    wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                    if (def && def->IsEnabled())
                    {
                        wxCommandEvent event(wxEVT_BUTTON, def->GetId());
                        event.SetEventObject(def);
                        def->Command(event);
                        return;
                    }
                }

                // this will make wxWidgets eat the ENTER key so that
                // we actually prevent line wrapping in a single line text control
                eat_key = true;
            }
            break;
    }

    if (!eat_key)
    {
        // perform keystroke handling
        event.Skip(true);
    }
}

void wxComboBox::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetModifiers() == wxMOD_CONTROL)
    {
        switch(event.GetKeyCode())
        {
            case 'A':
                SelectAll();
                return;
            case 'C':
                if (CanCopy())
                    Copy();
                return;
            case 'V':
                if (CanPaste())
                    Paste();
                return;
            case 'X':
                if (CanCut())
                    Cut();
                return;
            default:
                break;
        }
    }
    // no, we didn't process it
    event.Skip();
}

#endif // wxUSE_COMBOBOX && wxOSX_USE_COCOA
