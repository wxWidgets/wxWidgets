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
    m_peer = new wxMacControl() ;
    if ( label.Find('\n' ) == wxNOT_FOUND && label.Find('\r' ) == wxNOT_FOUND)
    {
        verify_noerr ( CreatePushButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") , *m_peer ) );
    }
    else
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlNoContent ;
        verify_noerr(CreateBevelButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds,CFSTR(""),
            kControlBevelButtonLargeBevel , kControlBehaviorPushbutton , &info , 0 , 0 , 0 , *m_peer ) );
    }
    
    wxASSERT_MSG( m_peer != NULL && m_peer->Ok() , wxT("No valid mac control") ) ;
    
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

    if ( btnOldDefault )
        btnOldDefault->m_peer->SetData(kControlButtonPart , kControlPushButtonDefaultTag , (Boolean) 0 ) ;
    m_peer->SetData(kControlButtonPart , kControlPushButtonDefaultTag , (Boolean) 1 ) ;
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
  
    Rect    bestsize = { 0 , 0 , 0 , 0 } ;
    short   baselineoffset ;
    ::GetBestControlRect( *m_peer , &bestsize , &baselineoffset ) ;
  
    int wBtn = 0 ;
    if ( EmptyRect( &bestsize ) )
    {
        int wBtn = m_label.Length() * charspace + 12 ;
    }
    else
    {
        sz.x = bestsize.right - bestsize.left ;
        sz.y = bestsize.bottom - bestsize.top ;
    }
    
    if (wBtn > sz.x || ( GetWindowStyle() & wxBU_EXACTFIT) ) 
        sz.x = wBtn;

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
    m_peer->Flash(kControlButtonPart) ;
    ProcessCommand (event);
}

wxInt32 wxButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId );
    event.SetEventObject(this);
    ProcessCommand(event);
    return noErr ;
}

