/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/choice.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
		   		int n, const wxString choices[],
		   		long style,
           const wxValidator& validator,
           const wxString& name)
{

		Rect bounds ;
		Str255 title ;
	
		MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

		m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , -12345 , 0 , 
	  	kControlPopupButtonProc + kControlPopupFixedWidthVariant , (long) this ) ; 
	
		m_macPopUpMenuHandle =  NewMenu( 1 , "\pPopUp Menu" ) ;
		SetControlData( m_macControl , kControlNoPart , kControlPopupButtonMenuHandleTag , sizeof( MenuHandle ) , (char*) &m_macPopUpMenuHandle) ;
		for ( int i = 0 ; i < n ; i++ )
		{
 			Str255 label;
			wxMenuItem::MacBuildMenuString( label , NULL , NULL , choices[i] ,false);
			AppendMenu( m_macPopUpMenuHandle , label ) ;
			m_strings.Add( choices[i] ) ;
		}
		SetControlMinimum( m_macControl , 0 ) ;
		SetControlMaximum( m_macControl , Number()) ;
		if ( n > 0 )
			SetControlValue( m_macControl , 1 ) ;

		MacPostControlCreate() ;

  	return TRUE;
}

void wxChoice::Append(const wxString& item)
{
	Str255 label;
	wxMenuItem::MacBuildMenuString( label , NULL , NULL , item ,false);
	AppendMenu( m_macPopUpMenuHandle , label ) ;
	m_strings.Add( item ) ;
	SetControlMaximum( m_macControl , Number()) ;
}

void wxChoice::Delete(int n)
{
    ::DeleteMenuItem( m_macPopUpMenuHandle , n + 1) ;
    m_strings.Remove( n ) ;
	SetControlMaximum( m_macControl , Number()) ;
}

void wxChoice::Clear()
{
    for ( int i = 0 ; i < Number() ; i++ )
    {
    	::DeleteMenuItem( m_macPopUpMenuHandle , 1 ) ;
	}
    m_strings.Clear() ;
	SetControlMaximum( m_macControl , Number()) ;
}

int wxChoice::GetSelection() const
{
    return GetControlValue( m_macControl ) -1 ;
}

void wxChoice::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
    wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_windowId );
	event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);
}


void wxChoice::SetSelection(int n)
{
    SetControlValue( m_macControl , n + 1 ) ;
}

int wxChoice::FindString(const wxString& s) const
{
    for( int i = 0 ; i < Number() ; i++ )
    {
    	if ( GetString( i ) == s )
    		return i ; 
    }
    return -1;
}

wxString wxChoice::GetString(int n) const
{
	return m_strings[n] ;
}

void wxChoice::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  wxControl::SetSize( x,y,width,height,sizeFlags ) ;
}

wxString wxChoice::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxString("");
}

bool wxChoice::SetStringSelection (const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
        {
            SetSelection (sel);
            return TRUE;
        }
    else
        return FALSE;
}

void wxChoice::Command(wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

