/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

// right now we don't support editable comboboxes


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
		   int n, const wxString choices[],
		   long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_noStrings = n;

		Rect bounds ;
		Str255 title ;
	
		MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;
	
		m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , -12345 , 0, 
	  	kControlPopupButtonProc , (long) this ) ; 
	
		m_macPopUpMenuHandle =  NewMenu( 1 , "\pPopUp Menu" ) ;
		SetControlData( m_macControl , kControlNoPart , kControlPopupButtonMenuHandleTag , sizeof( MenuHandle ) , (char*) &m_macPopUpMenuHandle) ;
		for ( int i = 0 ; i < n ; i++ )
		{
			appendmenu( m_macPopUpMenuHandle , choices[i] ) ;
		}
		SetControlMinimum( m_macControl , 0 ) ;
		SetControlMaximum( m_macControl , m_noStrings) ;
		SetControlValue( m_macControl , 1 ) ;

		MacPostControlCreate() ;

  	return TRUE;
}

wxString wxComboBox::GetValue() const
{
    return GetStringSelection() ;
}

void wxComboBox::SetValue(const wxString& value)
{
    SetStringSelection( value ) ;
}

// Clipboard operations
void wxComboBox::Copy()
{
    // TODO
}

void wxComboBox::Cut()
{
    // TODO
}

void wxComboBox::Paste()
{
    // TODO
}

void wxComboBox::SetEditable(bool editable)
{
    // TODO
}

void wxComboBox::SetInsertionPoint(long pos)
{
    // TODO
}

void wxComboBox::SetInsertionPointEnd()
{
    // TODO
}

long wxComboBox::GetInsertionPoint() const
{
    // TODO
    return 0;
}

long wxComboBox::GetLastPosition() const
{
    // TODO
    return 0;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    // TODO
}

void wxComboBox::Remove(long from, long to)
{
    // TODO
}

void wxComboBox::SetSelection(long from, long to)
{
    // TODO
}

void wxComboBox::Append(const wxString& item)
{
	appendmenu( m_macPopUpMenuHandle , item ) ;
    m_noStrings ++;
	SetControlMaximum( m_macControl , m_noStrings) ;
}

void wxComboBox::Delete(int n)
{
	wxASSERT( n < m_noStrings ) ;
    ::DeleteMenuItem( m_macPopUpMenuHandle , n + 1) ;
    m_noStrings --;
	SetControlMaximum( m_macControl , m_noStrings) ;
}

void wxComboBox::Clear()
{
    for ( int i = 0 ; i < m_noStrings ; i++ )
    {
    	::DeleteMenuItem( m_macPopUpMenuHandle , 1 ) ;
	}
    m_noStrings = 0;
	SetControlMaximum( m_macControl , m_noStrings) ;
}

int wxComboBox::GetSelection() const
{
    return GetControlValue( m_macControl ) -1 ;
}

void wxComboBox::SetSelection(int n)
{
    SetControlValue( m_macControl , n + 1 ) ;
}

int wxComboBox::FindString(const wxString& s) const
{
    for( int i = 0 ; i < m_noStrings ; i++ )
    {
    	if ( GetString( i ) == s )
    		return i ; 
    }
    return -1;
}

wxString wxComboBox::GetString(int n) const
{
	Str255 text ;
    ::GetMenuItemText( m_macPopUpMenuHandle , n+1 , text ) ;
    p2cstr( text ) ;
    return wxString( text );
}

wxString wxComboBox::GetStringSelection() const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxString("");
}

bool wxComboBox::SetStringSelection(const wxString& sel)
{
    int s = FindString (sel);
    if (s > -1)
        {
            SetSelection (s);
            return TRUE;
        }
    else
        return FALSE;
}

void wxComboBox::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId );
	event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(copystring(GetStringSelection()));
    ProcessCommand(event);
    delete[] event.GetString();
}

