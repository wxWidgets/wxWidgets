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
  wxSize slsize;
  int maxtextwidth, textheight;
  
  // Is control horizontal or vertical (Can be ambigous if user selects
  // another style without also specifying horz or vert
  if (!(style & wxSL_HORIZONTAL) && !(style & wxSL_VERTICAL)) {
    // Default is horizontal so make it so
    style |= wxSL_HORIZONTAL;
  }
  slsize = size;
  // Check that size corresponds with users selection of vertical or
  // horizontal slider and insert suitable default values
  if (style & wxSL_HORIZONTAL) 
    {
      slsize.y = 15;      // Slider width
      if (slsize.x == -1) {
        slsize.x = 150;  // Slider default length
      }
    }
  else
    {
      slsize.x = 15;      // Slider width
      if (slsize.y == -1) {
        slsize.y = 150;  // Slider default length
      }
    }
  /* Set the height and width for the slider control region.  The actual 
   * slider is set at 10 pixels across.  If the slider has labels then the
   * control region must be large enough to contain these labels
   */
  if (style & wxSL_LABELS) 
    {
      wxString text;
      int ht, wd;
      
      // Get maximum text label width and height
      text.Printf("%d", minValue);
      parent->GetTextExtent(text, &maxtextwidth, &textheight);
      text.Printf("%d", maxValue);
      parent->GetTextExtent(text, &wd, &ht);
      if(ht > textheight) {
        textheight = ht;
      }
      if (wd > maxtextwidth) {
        maxtextwidth = wd;
      }
      
      if (style & wxSL_VERTICAL) {
        slsize.x = (15 + maxtextwidth + 2); // Slider wd plus mac text width
      }
      if (style & wxSL_HORIZONTAL) {
        slsize.y = (15 + textheight);  // Slider ht plus text ht.
      }
    }
  
  MacPreControlCreate( parent , id ,  "" , pos , slsize , style,
		       validator , name , &bounds , title ) ;
  
  m_macMinimumStatic = NULL ;
  m_macMaximumStatic = NULL ;
  m_macValueStatic = NULL ;
  
  m_lineSize = 1;
  m_tickFreq = 0;

  m_rangeMax = maxValue;
  m_rangeMin = minValue;

  m_pageSize = (int)((maxValue-minValue)/10);
  
  // Must modify bounds to that of the slider dimensions from slider
  // dimensions plus text labels.
  if (style & wxSL_LABELS)
    {
      if ( style & wxSL_HORIZONTAL )
	{
	  bounds.bottom = bounds.top + 15;
          bounds.right -= (5 + maxtextwidth);
	}
      else  // Vertical slider
	{
          bounds.right = bounds.left + 15;
          bounds.bottom -= (5 + textheight);
	}
    }
  
  m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds ,
				title , true ,  value , minValue , maxValue, 
				kControlSliderProc +  kControlSliderLiveFeedback + ( ( style & wxSL_AUTOTICKS ) ? kControlSliderHasTickMarks : 0 ) , (long) this ) ;
  
  wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
  
  ::SetControlAction( m_macControl , wxMacLiveScrollbarActionUPP ) ;
  
  MacPostControlCreate() ;
  
  if ( style & wxSL_LABELS )
    {
      if ( style & wxSL_HORIZONTAL )
	{
	  wxPoint leftpos( 0 , 15 ) ;
	  wxPoint rightpos( m_width - (maxtextwidth + 20) , 15 ) ;
	  wxPoint valuepos( m_width - maxtextwidth , 0 ) ;
	  wxString valuestring ;
	  
	  valuestring.Printf( "%d" , minValue ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , leftpos ) ;

	  valuestring.Printf( "%d" , maxValue ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , rightpos ) ;

	  valuestring.Printf( "%d" , value ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , valuepos ) ;
	}
      else // Vertical slider
	{
	  wxPoint toppos( 17 , 0 ) ;
	  wxPoint bottompos( 17 , m_height - (textheight + 15) ) ;
	  wxPoint valuepos( 0 , m_height - textheight ) ;
	  wxString valuestring ;
	  
	  valuestring.Printf( "%d" , minValue ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , bottompos ) ;

	  valuestring.Printf( "%d" , maxValue ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , toppos ) ;

	  valuestring.Printf( "%d" , value ) ;
	  m_macMinimumStatic = new wxStaticText( this , -1 , valuestring , valuepos ) ;
	}
    }
  
  return true;
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
  wxString value;

  m_rangeMin = minValue;
  m_rangeMax = maxValue;

  // TODO
  SetControlMinimum(m_macControl, m_rangeMin);
  SetControlMaximum(m_macControl, m_rangeMax);
  
  if(m_macMinimumStatic) {
    value.Printf("%d", m_rangeMin);
    m_macMinimumStatic->SetLabel(value);
  }
  if(m_macMaximumStatic) {
    value.Printf("%d", m_rangeMax);
    m_macMaximumStatic->SetLabel(value);
  }
  SetValue(m_rangeMin);
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
