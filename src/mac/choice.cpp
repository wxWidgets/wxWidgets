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
#include "wx/menu.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

short nextMenuId = 100 ; // wxMenu takes the lower ids

wxChoice::~wxChoice()
{
	// DeleteMenu( m_macPopUpMenuId ) ;
	DisposeMenu( m_macPopUpMenuHandle ) ;
}

int wxChoice::GetCount() const {
	return m_strings.Count() ;
}

void wxChoice::SetString( int n , const wxString& s ) {
	m_strings[n] = s ;
}

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

		m_macPopUpMenuHandle =  NewMenu( 1 , "\pPopUp Menu" ) ;
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
			m_dataArray.Add( NULL );
		}
		SetControlMinimum( m_macControl , 0 ) ;
		SetControlMaximum( m_macControl , Number()) ;
		if ( n > 0 )
			SetControlValue( m_macControl , 1 ) ;

		MacPostControlCreate() ;

  	return TRUE;
}

int wxChoice::DoAppend(const wxString& item)
{
	Str255 label;
	wxMenuItem::MacBuildMenuString( label , NULL , NULL , item ,false);
	AppendMenu( m_macPopUpMenuHandle , label ) ;
	m_strings.Add( item ) ;
	m_dataArray.Add( NULL );
	return m_strings.Count() ;
}

void *wxChoice::DoGetItemClientData(int N) const
{
    return (void *)m_dataArray[N];
}

void wxChoice::DoSetItemClientData( int N, void* Client_data ) 
{
	wxASSERT_MSG( m_dataArray.GetCount() >= N , "invalid client_data array" ) ;
	
	if ( m_dataArray.GetCount() > N )
	{
    	m_dataArray[N] = (char*) Client_data ;
    }
    else
    {
    	m_dataArray.Add( (char*) Client_data ) ;
    }
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
    DoSetItemClientData(n, clientData);
}

wxClientData* wxChoice::DoGetItemClientObject( int N ) const
{
	return (wxClientData *) DoGetItemClientData( N ) ;
}

void wxChoice::Delete(int n)
{
    ::DeleteMenuItem( m_macPopUpMenuHandle , n + 1) ;
    m_strings.Remove( n ) ;
	m_dataArray.Remove( n ) ;
	SetControlMaximum( m_macControl , Number()) ;
}

void wxChoice::Clear()
{
    for ( int i = 0 ; i < Number() ; i++ )
    {
    	::DeleteMenuItem( m_macPopUpMenuHandle , 1 ) ;
	}
    m_strings.Clear() ;
  	m_dataArray.Empty() ;
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

void wxChoice::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
  wxControl::SetSize( x,y,width,height,sizeFlags ) ;
}

