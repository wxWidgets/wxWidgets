/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/combobox_osx.cpp
// Purpose:     wxComboBox class using HIView ComboBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: combobox_osx.cpp 58318 2009-01-23 08:36:16Z RR $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX && defined(wxOSX_USE_NATIVE_COMBOBOX)

#include "wx/combobox.h"
#include "wx/osx/private.h"

#ifndef WX_PRECOMP
#endif

// work in progress

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

wxComboBox::~wxComboBox()
{
}

void wxComboBox::Init()
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
    m_text = NULL;
    m_choice = NULL;

    m_macIsUserPane = false;

    if ( !wxControl::Create( parent, id, pos, size, style, validator, name ) )
        return false;
        
    m_peer = wxWidgetImpl::CreateComboBox( this, parent, id, NULL, pos, size, style, GetExtraStyle() );

    MacPostControlCreate( pos, size );

    Append(n, choices);

    // Set the first item as being selected
    if (n > 0)
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

wxString wxComboBox::GetValue() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return wxEmptyString;
}

void wxComboBox::SetValue(const wxString& value)
{
    wxFAIL_MSG("Method Not Implemented.");
}

// Clipboard operations
void wxComboBox::Copy()
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::Cut()
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::Paste()
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SetEditable(bool editable)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SetInsertionPoint(long pos)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SetInsertionPointEnd()
{
    wxFAIL_MSG("Method Not Implemented.");
}

long wxComboBox::GetInsertionPoint() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

wxTextPos wxComboBox::GetLastPosition() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::Remove(long from, long to)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SetSelection(long from, long to)
{
    wxFAIL_MSG("Method Not Implemented.");
}

int wxComboBox::DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type)
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void* wxComboBox::DoGetItemClientData(unsigned int n) const
{
    wxFAIL_MSG("Method Not Implemented.");
    return NULL;
}

unsigned int wxComboBox::GetCount() const 
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

void wxComboBox::DoDeleteOneItem(unsigned int n)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::DoClear()
{
    wxFAIL_MSG("Method Not Implemented.");
}

int wxComboBox::GetSelection() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

void wxComboBox::GetSelection(long *from, long *to) const
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SetSelection(int n)
{
    wxFAIL_MSG("Method Not Implemented.");
}

int wxComboBox::FindString(const wxString& s, bool bCase) const
{
    wxFAIL_MSG("Method Not Implemented.");
    return 0;
}

wxString wxComboBox::GetString(unsigned int n) const
{
    wxFAIL_MSG("Method Not Implemented.");
    return wxEmptyString;
}

wxString wxComboBox::GetStringSelection() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return wxEmptyString;
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    wxFAIL_MSG("Method Not Implemented.");
}

bool wxComboBox::IsEditable() const
{
    return !HasFlag(wxCB_READONLY);
}

void wxComboBox::Undo()
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::Redo()
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::SelectAll()
{
    wxFAIL_MSG("Method Not Implemented.");
}

bool wxComboBox::CanCopy() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return false;
}

bool wxComboBox::CanCut() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return false;
}

bool wxComboBox::CanPaste() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return false;
}

bool wxComboBox::CanUndo() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return false;
}

bool wxComboBox::CanRedo() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return false;
}

void wxComboBox::EnableTextChangedEvents(bool enable)
{
    wxFAIL_MSG("Method Not Implemented.");
}

void wxComboBox::WriteText(const wxString& text)
{
    wxFAIL_MSG("Method Not Implemented.");
}

wxString wxComboBox::DoGetValue() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return wxEmptyString;
}

wxClientDataType wxComboBox::GetClientDataType() const
{
    wxFAIL_MSG("Method Not Implemented.");
    return wxClientData_None;
}

void wxComboBox::SetClientDataType(wxClientDataType clientDataItemsType)
{
    wxFAIL_MSG("Method Not Implemented.");
}

bool wxComboBox::OSXHandleClicked( double timestampsec )
{
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
    return true;
}

#endif // wxUSE_COMBOBOX && defined(wxOSX_USE_NATIVE_COMBOBOX)
