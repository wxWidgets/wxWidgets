/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinbutt.h"
#pragma implementation "spinbuttbase.h"
#endif

#include "wx/spinbutt.h"
#include "wx/mac/uma.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
    IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent);
#endif

wxSpinButton::wxSpinButton()
   : wxSpinButtonBase()
{
}

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_min = 0;
    m_max = 100;

  if (!parent)
      return FALSE;
    
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style,*( (wxValidator*) NULL ) , name , &bounds , title ) ;

	m_macControl = ::NewControl( parent->GetMacRootWindow() , &bounds , title , false , 0 , 0 , 100, 
		kControlLittleArrowsProc , (long) this ) ;
	
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;

	MacPostControlCreate() ;

  return TRUE;
}

wxSpinButton::~wxSpinButton()
{
}

// Attributes
////////////////////////////////////////////////////////////////////////////

int wxSpinButton::GetMin() const
{
   return m_min;
}

int wxSpinButton::GetMax() const
{
   return m_max;
}

int wxSpinButton::GetValue() const
{
    return m_value;
}

void wxSpinButton::SetValue(int val)
{
	m_value = val ;
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
	m_min = minVal;
	m_max = maxVal;
    SetControlMaximum( m_macControl , maxVal ) ;
    SetControlMinimum(  m_macControl , minVal ) ;
}

void wxSpinButton::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	if ( m_macControl == NULL )
		return ;
	
	int oldValue = m_value ;
  wxEventType scrollEvent = wxEVT_NULL;
  int nScrollInc = 0;

	switch( controlpart )
	{
		case kControlUpButtonPart :
        nScrollInc = 1;
        scrollEvent = wxEVT_SCROLL_LINEUP;
			break ;
		case kControlDownButtonPart :
        nScrollInc = -1;
        scrollEvent = wxEVT_SCROLL_LINEDOWN;
			break ;
	}

  m_value = m_value + nScrollInc;

  if (m_value < m_min)
  {
  	if ( m_windowStyle & wxSP_WRAP )
      m_value = m_max;
  	else
      m_value = m_min;
  }
  
  if (m_value > m_max)
  {
  	if ( m_windowStyle & wxSP_WRAP )
      m_value = m_min;
    else
      m_value = m_max;
  }
  	
  wxSpinEvent event(scrollEvent, m_windowId);

  event.SetPosition(m_value);
  event.SetEventObject( this );
  if ((GetEventHandler()->ProcessEvent( event )) &&
        !event.IsAllowed() )
  {
  	m_value = oldValue ;
  }
  SetControlValue(  m_macControl , m_value ) ;

    /* always send a thumbtrack event */
    if (scrollEvent != wxEVT_SCROLL_THUMBTRACK)
    {
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
        wxSpinEvent event2( scrollEvent, GetId());
        event2.SetPosition( m_value );
        event2.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event2 );
    }
}

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
  return wxSize(16,24);
}

