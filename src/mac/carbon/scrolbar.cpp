/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "scrolbar.h"
#endif

#include "wx/scrolbar.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

BEGIN_EVENT_TABLE(wxScrollBar, wxControl)
END_EVENT_TABLE()

#endif

extern ControlActionUPP wxMacLiveScrollbarActionUPP ;

// Scrollbar
bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  if (!parent)
      return FALSE;
    
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 100, 
		kControlScrollBarLiveProc , (long) this ) ;
	
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;

	::SetControlAction( m_macControl , wxMacLiveScrollbarActionUPP ) ;

	MacPostControlCreate() ;

  return TRUE;
}

wxScrollBar::~wxScrollBar()
{
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    ::SetControlValue( m_macControl , viewStart ) ;
}

int wxScrollBar::GetThumbPosition() const
{
    return ::GetControlValue( m_macControl ) ;
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh)
{
    m_viewSize = pageSize;
    m_pageSize = thumbSize;
    m_objectSize = range;

  	int range1 = wxMax((m_objectSize - m_pageSize), 0) ;

    SetControlMaximum( m_macControl , range1 ) ;
    SetControlMinimum(  m_macControl , 0 ) ;
    SetControlValue(  m_macControl , position ) ;

    if ( UMAGetAppearanceVersion() >= 0x0110  )
    {
#if UMA_USE_8_6    	
			SetControlViewSize( m_macControl , m_pageSize ) ;
#endif
    }
    Refresh() ;
}


void wxScrollBar::Command(wxCommandEvent& event)
{
    SetThumbPosition(event.m_commandInt);
    ProcessCommand(event);
}

void wxScrollBar::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	if ( m_macControl == NULL )
		return ;
	
  int position = GetControlValue( m_macControl) ;
  int minPos = GetControlMinimum( m_macControl) ;
  int maxPos = GetControlMaximum( m_macControl) ;

  wxEventType scrollEvent = wxEVT_NULL;
  int nScrollInc;

	switch( controlpart )
	{
		case kControlUpButtonPart :
        nScrollInc = -1;
        scrollEvent = wxEVT_SCROLL_LINEUP;
			break ;
		case kControlDownButtonPart :
        nScrollInc = 1;
        scrollEvent = wxEVT_SCROLL_LINEDOWN;
			break ;
		case kControlPageUpPart :
        nScrollInc = -m_pageSize;
        scrollEvent = wxEVT_SCROLL_PAGEUP;
			break ;
		case kControlPageDownPart :
        nScrollInc = m_pageSize;
        scrollEvent = wxEVT_SCROLL_PAGEDOWN;
			break ;
		case kControlIndicatorPart :
        nScrollInc = 0 ;
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
			break ;
	}

  int new_pos = position + nScrollInc;

  if (new_pos < 0)
      new_pos = 0;
  if (new_pos > maxPos)
      new_pos = maxPos;
	if ( nScrollInc )
  	SetThumbPosition(new_pos);
  	
  wxScrollEvent event(scrollEvent, m_windowId);
	if ( m_windowStyle & wxHORIZONTAL )
	{
		event.SetOrientation( wxHORIZONTAL ) ;
	}
	else
	{
		event.SetOrientation( wxVERTICAL ) ;
	}
  event.SetPosition(new_pos);
  event.SetEventObject( this );
  wxWindow* window = GetParent() ;
  if (window && window->MacIsWindowScrollbar(this) )
  {
  	// this is hardcoded
  	window->MacOnScroll(event);
  }
  else
  	GetEventHandler()->ProcessEvent(event);
}

