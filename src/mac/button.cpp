/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#include "wx/button.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
#endif

#include <wx/mac/uma.h>
// Button


bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 1, 
	  	kControlPushButtonProc , (long) this ) ;
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
	
	MacPostControlCreate() ;

  return TRUE;
}

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxButton *btnOldDefault = NULL;
    wxPanel *panel = wxDynamicCast(parent, wxPanel);
    if ( panel )
    {
        btnOldDefault = panel->GetDefaultItem();
        panel->SetDefaultItem(this);
    }
  
  if ( btnOldDefault && btnOldDefault->m_macControl )
  {
		UMASetControlData( btnOldDefault->m_macControl , kControlButtonPart , kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)((Boolean)0) ) ;
  }
  if ( m_macControl )
  {
		UMASetControlData( m_macControl , kControlButtonPart , kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)((Boolean)1) ) ;
  }
}

wxSize wxButton::DoGetBestSize() const
{
    int wBtn = m_label.Length() * 8 + 12 ;
	int hBtn = 20 ;
	
	if ( wBtn < 80 )
		wBtn = 80 ;

    return wxSize(wBtn, hBtn);
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 80 /* + 2 * m_macHorizontalBorder */ ; 
	int hBtn = 20 /* +  2 * m_macVerticalBorder */ ;

    return wxSize(wBtn, hBtn);
}

void wxButton::Command (wxCommandEvent & event)
{
	if ( m_macControl )
	{
		HiliteControl( m_macControl , kControlButtonPart ) ;
		unsigned long finalTicks ;
		Delay( 8 , &finalTicks ) ;
		HiliteControl( m_macControl , 0 ) ;
	}
    ProcessCommand (event);
}

void wxButton::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
}

