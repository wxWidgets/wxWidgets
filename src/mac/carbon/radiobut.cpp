/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      AUTHOR
// Modified by: JS Lair (99/11/15) adding the cyclic groupe notion for radiobox
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

#include "wx/radiobut.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

#include <wx/mac/uma.h>

bool wxRadioButton::Create(wxWindow *parent, wxWindowID id,
		   const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
	Rect bounds ;
	Str255 title ;
	
	m_cycle=NULL;
	
	MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 1, 
	  	kControlRadioButtonProc , (long) this ) ;
	
	MacPostControlCreate() ;

  return TRUE;
}

void wxRadioButton::SetValue(bool val)
{
	int i;
	wxRadioButton *cycle;
	
   ::SetControlValue( m_macControl , val ) ;
   
   if (val) {
   		cycle=this->NextInCycle();
  		if (cycle!=NULL) {
   			while (cycle!=this) {
   				cycle->SetValue(false);
   				cycle=cycle->NextInCycle();
   				}
   			}
   		}
}

bool wxRadioButton::GetValue() const
{
    return ::GetControlValue( m_macControl ) ;
}

void wxRadioButton::Command (wxCommandEvent & event)
{
  SetValue ( (event.GetInt() != 0) );
  ProcessCommand (event);
}

void wxRadioButton::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	SetValue(true) ;
    wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
}

wxRadioButton *wxRadioButton::AddInCycle(wxRadioButton *cycle)
{
	wxRadioButton *next,*current;
		
	if (cycle==NULL) {
		m_cycle=this;
		return(this);
		}
	else {
		current=cycle;
  		while ((next=current->m_cycle)!=cycle) current=current->m_cycle;
	  	m_cycle=cycle;
	  	current->m_cycle=this;
	  	return(cycle);
  	}
}  

