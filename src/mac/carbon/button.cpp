/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#include "wx/defs.h"

#include "wx/button.h"
#include "wx/panel.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
#endif

#include "wx/mac/uma.h"
// Button

bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    if ( !wxButtonBase::Create(parent, id, pos, size, style, validator, name) )
        return false;
    
    m_label = label ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    if ( label.Find('\n' ) == wxNOT_FOUND && label.Find('\r' ) == wxNOT_FOUND)
    {
        verify_noerr ( CreatePushButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") , (ControlRef*) &m_macControl ) ) ;
    }
    else
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlNoContent ;
        verify_noerr(CreateBevelButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds,CFSTR(""),
            kControlBevelButtonLargeBevel , kControlBehaviorPushbutton , &info , 0 , 0 , 0 , (ControlRef*) &m_macControl ) ) ;
    }
    wxASSERT_MSG( (ControlRef) m_macControl != NULL , wxT("No valid mac control") ) ;
    
    MacPostControlCreate(pos,size) ;
    
  return TRUE;
}

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxButton *btnOldDefault = NULL;
    if ( parent )
    {
        btnOldDefault = wxDynamicCast(parent->GetDefaultItem(),
           wxButton);
        parent->SetDefaultItem(this);
    }

    Boolean inData;
    if ( btnOldDefault && btnOldDefault->m_macControl )
    {
          inData = 0;
        ::SetControlData( (ControlRef) btnOldDefault->m_macControl , kControlButtonPart ,
                           kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)(&inData) ) ;
    }
    if ( (ControlRef) m_macControl )
    {
          inData = 1;
        ::SetControlData(  (ControlRef) m_macControl , kControlButtonPart ,
                           kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)(&inData) ) ;
    }
}

wxSize wxButton::DoGetBestSize() const
{
  wxSize sz = GetDefaultSize() ;
  
  int charspace = 8 ;
  if ( GetWindowVariant() == wxWINDOW_VARIANT_NORMAL || GetWindowVariant() == wxWINDOW_VARIANT_LARGE )
  {
    sz.y = 20 ;
    charspace = 10 ;
  }
  else if ( GetWindowVariant() == wxWINDOW_VARIANT_SMALL )
  {
    sz.y = 17 ;
    charspace = 8 ;
  }
  else if ( GetWindowVariant() == wxWINDOW_VARIANT_MINI )
  {
    sz.y = 15 ;
    charspace = 8 ;
  }
  
  int wBtn = m_label.Length() * charspace + 12 ;
 
  if (wBtn > sz.x) sz.x = wBtn;
  
  return sz ;
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 70 ; 
    int hBtn = 20 ;
    
    return wxSize(wBtn, hBtn);
}

void wxButton::Command (wxCommandEvent & event)
{
    if ( (ControlRef) m_macControl )
    {
        HiliteControl(  (ControlRef) m_macControl , kControlButtonPart ) ;
        unsigned long finalTicks ;
        Delay( 8 , &finalTicks ) ;
        HiliteControl(  (ControlRef) m_macControl , 0 ) ;
    }
    ProcessCommand (event);
}

wxInt32 wxButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
    return noErr ;
}

