/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMac.
// Author:      Stefan Csomor
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     Rocketeer license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declatations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#include "wx/defs.h"
#include "wx/tglbtn.h"

#if wxUSE_TOGGLEBTN

#include "wx/mac/uma.h"
// Button

static const int kMacOSXHorizontalBorder = 2 ;
static const int kMacOSXVerticalBorder = 4 ;

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

// Single check box item
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
    
    m_label = label ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , "\p" , true , 0 , kControlBehaviorToggles , 1, 
          kControlBevelButtonNormalBevelProc  , (long) this ) ;
    wxASSERT_MSG( (ControlRef) m_macControl != NULL , wxT("No valid mac control") ) ;
    
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
    ::SetControl32BitValue( (ControlRef) m_macControl , val ) ;
}

bool wxToggleButton::GetValue() const
{
    return GetControl32BitValue( (ControlRef) m_macControl ) ;
}

void wxToggleButton::Command(wxCommandEvent & event)
{
   SetValue((event.GetInt() != 0));
   ProcessCommand(event);
}

void wxToggleButton::MacHandleControlClick( WXWidget WXUNUSED(control) , wxInt16 controlpart , bool WXUNUSED(mouseStillDown) ) 
{
    if ( controlpart != kControlNoPart )
    {
        wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
        event.SetInt(GetValue());
        event.SetEventObject(this);
        ProcessCommand(event);
    }
}

#endif // wxUSE_TOGGLEBTN

