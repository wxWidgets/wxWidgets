/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

BEGIN_EVENT_TABLE(wxSlider, wxControl)
END_EVENT_TABLE()
#endif



// Slider
wxSlider::wxSlider()
{
  m_pageSize = 1;
  m_lineSize = 1;
  m_rangeMax = 0;
  m_rangeMin = 0;
  m_tickFreq = 0;
}

extern ControlActionUPP wxMacLiveScrollbarActionUPP ;

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

	m_macMinimumStatic = NULL ;

  m_lineSize = 1;
  m_tickFreq = 0;

  m_rangeMax = maxValue;
  m_rangeMin = minValue;

  m_pageSize = (int)((maxValue-minValue)/10);
	if ( m_width == -1 )
	{
		m_width = 20 ;
		if ( style & wxSL_LABELS && style & wxSL_VERTICAL )
			m_width += 24 ;
		bounds.right = bounds.left + m_width ;
	}
	if ( m_height == -1 )
	{
		m_height = 20 ;
		if ( style & wxSL_LABELS && style & wxSL_HORIZONTAL )
			m_height += 24 ;
		bounds.bottom = bounds.top + m_height ;
	}
	
	if ( style & wxSL_LABELS && style & wxSL_HORIZONTAL )
	{
		bounds.top += 12 ; 
		bounds.right -= 24 ;
	}
			
	if ( style & wxSL_LABELS && style & wxSL_VERTICAL )
	{
		bounds.left += 24 ;
		bounds.top += 12 ;
	}
			
	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true ,  value , minValue , maxValue, 
  			kControlSliderProc +  kControlSliderLiveFeedback + ( ( style & wxSL_AUTOTICKS ) ? kControlSliderHasTickMarks : 0 ) , (long) this ) ;
	
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;

	::SetControlAction( m_macControl , wxMacLiveScrollbarActionUPP ) ;

	MacPostControlCreate() ;

	if ( style & wxSL_LABELS )
	{
		if ( style & wxSL_HORIZONTAL )
		{
			wxSize size( 24 , 12 ) ;
			wxPoint leftpos( 0 , 0 ) ;
			wxPoint	rightpos( m_width - 2 * 12 , 0 ) ;
			wxPoint valuepos( m_width - 12 , 20 ) ;
			wxString valuestring ;
			
			valuestring.Printf( "%d" , minValue ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , leftpos , size ) ;
			valuestring.Printf( "%d" , maxValue ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , rightpos , size ) ;
			valuestring.Printf( "%d" , value ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , valuepos , size ) ;
		}
		else
		{
			wxSize size( 24 , 12 ) ;
			wxPoint toppos( 0 , 12 ) ;
			wxPoint	bottompos( 0 , m_height - 12 ) ;
			wxPoint valuepos( 20 , 0 ) ;
			wxString valuestring ;
			
			valuestring.Printf( "%d" , minValue ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , bottompos , size ) ;
			valuestring.Printf( "%d" , maxValue ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , toppos , size ) ;
			valuestring.Printf( "%d" , value ) ;				
			m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , valuepos , size ) ;
		}
	}

  return TRUE;

}

wxSlider::~wxSlider()
{
}

int wxSlider::GetValue() const
{
 	return GetControlValue( m_macControl) ;
}

void wxSlider::SetValue(int value)
{
	wxString valuestring ;
	valuestring.Printf( "%d" , value ) ;	
	if ( m_macMinimumStatic )			
		m_macMinimumStatic->SetLabel( valuestring ) ;
 	SetControlValue( m_macControl , value ) ;
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    // TODO
}

// For trackbars only
void wxSlider::SetTickFreq(int n, int pos)
{
    // TODO
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // TODO
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // TODO
}

void wxSlider::ClearTicks()
{
    // TODO
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    // TODO
}

int wxSlider::GetLineSize() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelEnd() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelStart() const
{
    // TODO
    return 0;
}

void wxSlider::SetSelection(int minPos, int maxPos)
{
    // TODO
}

void wxSlider::SetThumbLength(int len)
{
    // TODO
}

int wxSlider::GetThumbLength() const
{
    // TODO
    return 0;
}

void wxSlider::SetTick(int tickPos)
{
    // TODO
}

void wxSlider::Command (wxCommandEvent & event)
{
  SetValue (event.GetInt());
  ProcessCommand (event);
}

bool wxSlider::Show( bool show ) 
{
	return wxWindow::Show( show ) ;
}

void wxSlider::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	SInt16 value = ::GetControlValue( m_macControl ) ;
	
	SetValue( value ) ;		
	
	wxScrollEvent event(wxEVT_SCROLL_THUMBTRACK, m_windowId);
	event.SetPosition(GetControlValue( m_macControl) );
	event.SetEventObject( this );

#if WXWIN_COMPATIBILITY

    wxEventType oldEvent = event.GetEventType();
    event.SetEventType( wxEVT_COMMAND_SLIDER_UPDATED );
    if ( !GetEventHandler()->ProcessEvent(event) )
    {
        event.SetEventType( oldEvent );
        if (!GetParent()->GetEventHandler()->ProcessEvent(event))
            event.Skip();
    }
#else
	GetEventHandler()->ProcessEvent(event);
#endif
}
