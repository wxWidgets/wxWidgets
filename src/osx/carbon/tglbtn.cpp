/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMac.
// Author:      Stefan Csomor
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declatations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"
#include "wx/osx/uma.h"
// Button

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

// ============================================================================
// implementation
// ============================================================================
// ----------------------------------------------------------------------------
// wxToggleButton
// ----------------------------------------------------------------------------

bool wxToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;
    
    m_labelOrig = m_label = label ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    
    m_peer = new wxMacControl(this) ;
    verify_noerr ( CreateBevelButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") , 
        kControlBevelButtonNormalBevel , kControlBehaviorToggles , NULL , 0 , 0 , 0 , m_peer->GetControlRefAddr() ) );
    
    
    MacPostControlCreate(pos,size) ;
    
  return TRUE;
}

wxSize wxToggleButton::DoGetBestSize() const
{
    int wBtn = 70 ; 
    int hBtn = 20 ;

    int lBtn = m_label.Length() * 8 + 12 ;
    if (lBtn > wBtn) 
        wBtn = lBtn;

    return wxSize ( wBtn , hBtn ) ;
}

void wxToggleButton::SetValue(bool val)
{
    m_peer->SetValue( val ) ;
}

bool wxToggleButton::GetValue() const
{
    return m_peer->GetValue() ;
}

void wxToggleButton::Command(wxCommandEvent & event)
{
   SetValue((event.GetInt() != 0));
   ProcessCommand(event);
}

wxInt32 wxToggleButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return noErr ;
}

// ----------------------------------------------------------------------------
// wxBitmapToggleButton
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxControl)

bool wxBitmapToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxBitmap& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    m_bitmap = label;
    
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;
    
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    
    m_peer = new wxMacControl(this) ;
    
    ControlButtonContentInfo info;
    wxMacCreateBitmapButton( &info, m_bitmap );
    verify_noerr ( CreateBevelButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") , 
        kControlBevelButtonNormalBevel , kControlBehaviorOffsetContents | kControlBehaviorToggles , &info , 0 , 0 , 0 , m_peer->GetControlRefAddr() ) );
    
    MacPostControlCreate(pos,size) ;
    
    return TRUE;
}

wxSize wxBitmapToggleButton::DoGetBestSize() const
{
    if (!m_bitmap.IsOk())
       return wxSize(20,20);
       
    return wxSize ( m_bitmap.GetWidth()+6, m_bitmap.GetHeight()+6 ) ;
}

void wxBitmapToggleButton::SetValue(bool val)
{
    m_peer->SetValue( val ) ;
}

bool wxBitmapToggleButton::GetValue() const
{
    return m_peer->GetValue() ;
}

void wxBitmapToggleButton::Command(wxCommandEvent & event)
{
   SetValue((event.GetInt() != 0));
   ProcessCommand(event);
}

wxInt32 wxBitmapToggleButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) ) 
{
    wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return noErr ;
}

#endif // wxUSE_TOGGLEBTN

