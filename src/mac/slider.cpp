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

 // The dimensions of the different styles of sliders (From Aqua document)
 #define wxSLIDER_DIMENSIONACROSS 15
 #define wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS 24
 #define wxSLIDER_DIMENSIONACROSS_ARROW 18
 
 // Distance between slider and text
 #define wxSLIDER_BORDERTEXT 5
 
 /* NB!  The default orientation for a slider is horizontal however if the user specifies
  * some slider styles but dosen't specify the orientation we have to assume he wants a
  * horizontal one.  Therefore in this file when testing for the sliders orientation
  * vertical is tested for if this is not set then we use the horizontal one
  * eg.  if(GetWindowStyle() & wxSL_VERTICAL) {}  else { horizontal case }>  
  */


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
     SInt16 procID;

     m_macMinimumStatic = NULL ;
     m_macMaximumStatic = NULL ;
     m_macValueStatic = NULL ;


     m_lineSize = 1;
     m_tickFreq = 0;
 
     m_rangeMax = maxValue;
     m_rangeMin = minValue;
 
     m_pageSize = (int)((maxValue-minValue)/10);
 
     MacPreControlCreate( parent, id, "", pos, size, style,
                          validator, name, &bounds, title );
 
     procID = kControlSliderProc + kControlSliderLiveFeedback;
     if(style & wxSL_AUTOTICKS) {
         procID += kControlSliderHasTickMarks;
    }
  

     m_macControl = ::NewControl( parent->MacGetRootWindow(), &bounds, title, false,
                                   value, minValue, maxValue, procID, (long) this);
 
     wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
 
     ::SetControlAction( m_macControl , wxMacLiveScrollbarActionUPP ) ;
 
     if(style & wxSL_LABELS)
     {
         m_macMinimumStatic = new wxStaticText( this, -1, "" );
         m_macMaximumStatic = new wxStaticText( this, -1, "" );
         m_macValueStatic = new wxStaticText( this, -1, "" );
         SetRange(minValue, maxValue);
         SetValue(value);
    }
  
     else {
         m_macMinimumStatic = NULL ;
         m_macMaximumStatic = NULL ;
         m_macValueStatic = NULL ;
     }
 
     if(style & wxSL_VERTICAL) {
         SetSizeHints(10, -1, 10, -1);  // Forces SetSize to use the proper width
     }
     else {
         SetSizeHints(-1, 10, -1, 10);  // Forces SetSize to use the proper height
     }
     // NB!  SetSizeHints is overloaded by wxSlider and will substitute 10 with the
     // proper dimensions, it also means other people cannot bugger the slider with
     // other values
 
     MacPostControlCreate() ;
 
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
	if ( m_macValueStatic )
		m_macValueStatic->SetLabel( valuestring ) ;
 	SetControlValue( m_macControl , value ) ;
}

void wxSlider::SetRange(int minValue, int maxValue)
{
  wxString value;

  m_rangeMin = minValue;
  m_rangeMax = maxValue;

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

void wxSlider::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	SInt16 value = ::GetControlValue( m_macControl ) ;
	
	SetValue( value ) ;		
	
	wxScrollEvent event(wxEVT_SCROLL_THUMBTRACK, m_windowId);
	event.SetPosition(value);
	event.SetEventObject( this );
	GetEventHandler()->ProcessEvent(event);

  wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, m_windowId );
  cevent.SetInt( value );
  cevent.SetEventObject( this );

  GetEventHandler()->ProcessEvent( cevent );
}

 /* This is overloaded in wxSlider so that the proper width/height will always be used
  * for the slider different values would cause redrawing and mouse detection problems */
 void wxSlider::SetSizeHints( int minW, int minH,
                              int maxW , int maxH ,
                              int incW , int incH )
 {
     wxSize size = GetBestSize();
 
     if(GetWindowStyle() & wxSL_VERTICAL) {
         wxWindow::SetSizeHints(size.x, minH, size.x, maxH, incW, incH);
     }
     else {
         wxWindow::SetSizeHints(minW, size.y, maxW, size.y, incW, incH);
     }
 }
 
 wxSize wxSlider::DoGetBestSize() const
 {
     wxSize size;
     int textwidth, textheight;
 
     if(GetWindowStyle() & wxSL_LABELS)
     {
         wxString text;
         int ht, wd;
 
         // Get maximum text label width and height
         text.Printf("%d", m_rangeMin);
         GetTextExtent(text, &textwidth, &textheight);
         text.Printf("%d", m_rangeMax);
         GetTextExtent(text, &wd, &ht);
         if(ht > textheight) {
             textheight = ht;
         }
         if (wd > textwidth) {
             textwidth = wd;
         }
     }
 
     if(GetWindowStyle() & wxSL_VERTICAL)
     {
         if(GetWindowStyle() & wxSL_AUTOTICKS) {
             size.x = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
         }
         else {
             size.x = wxSLIDER_DIMENSIONACROSS_ARROW;
         }
         if(GetWindowStyle() & wxSL_LABELS) {
             size.x += textwidth + wxSLIDER_BORDERTEXT;
         }
         size.y = 150;
     }
     else
     {
         if(GetWindowStyle() & wxSL_AUTOTICKS) {
             size.y = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
         }
         else {
             size.y = wxSLIDER_DIMENSIONACROSS_ARROW;
         }
         if(GetWindowStyle() & wxSL_LABELS) {
             size.y += textheight + wxSLIDER_BORDERTEXT;
         }
         size.x = 150;
     }
     return size;
 }
 
 void wxSlider::DoSetSize(int x, int y, int width, int height, int sizeFlags)
 {
     Rect oldbounds, newbounds;
     int new_x, new_y, new_width, new_height;
     int mac_x, mac_y;
 
     new_x = m_x;
     new_y = m_y;
     new_width  = m_width;
     new_height = m_height;
 
     if (sizeFlags & wxSIZE_ALLOW_MINUS_ONE)
     {
         new_x      = x;
         new_y      = y;
         new_width  = width;
         new_height = height;
     }
     else
     {
         if (x != -1)      new_x      = x;
         if (y != -1)      new_y      = y;
         if (width != -1)  new_width  = width;
         if (height != -1) new_height = height;
     }
 
     if(sizeFlags & wxSIZE_AUTO)
     {
         wxSize size = GetBestSize();
         if (sizeFlags & wxSIZE_AUTO_WIDTH)
         {
             if (width == -1)   new_width = size.x;
         }
         if (sizeFlags & wxSIZE_AUTO_HEIGHT)
         {
             if (height == -1)   new_height = size.y;
         }
     }
 
     AdjustForParentClientOrigin(new_x, new_y, sizeFlags);
 
     mac_x = new_x;
     mac_y = new_y;
     if(GetParent()) {
         GetParent()->MacClientToRootWindow(&mac_x, &mac_y);
     }
 
     GetControlBounds(m_macControl, &oldbounds);
     oldbounds.right = oldbounds.left + m_width;
     oldbounds.bottom = oldbounds.top + m_height;
 
     bool doMove = false;
     bool doResize = false;
 
     if ( mac_x != oldbounds.left || mac_y != oldbounds.top )
     {
         doMove = true ;
     }
     if ( new_width != m_width || new_height != m_height )
     {
         doResize = true ;
     }
 
     if ( doMove || doResize )
     {
         // Ensure resize is within constraints
         if ((m_minWidth != -1) && (new_width < m_minWidth)) {
             new_width = m_minWidth;
         }
         if ((m_minHeight != -1) && (new_height < m_minHeight)) {
             new_height = m_minHeight;
         }
         if ((m_maxWidth != -1) && (new_width > m_maxWidth)) {
             new_width = m_maxWidth;
         }
         if ((m_maxHeight != -1) && (new_height > m_maxHeight)) {
             new_height = m_maxHeight;
         }
 
         DoMoveWindow(new_x, new_y, new_width, new_height);
 
         // Update window at old and new positions
         SetRect(&newbounds, m_x, m_y, m_x + m_width, m_y + m_height);
         WindowRef rootwindow = MacGetRootWindow();
         InvalWindowRect( rootwindow , &oldbounds );
         InvalWindowRect( rootwindow , &newbounds );
 
         if ( doMove )
         {
             wxMoveEvent event(wxPoint(m_x, m_y), m_windowId);
             event.SetEventObject(this);
             GetEventHandler()->ProcessEvent(event) ;
         }
         if ( doResize )
         {
             wxSizeEvent event(wxSize(m_width, m_height), m_windowId);
             event.SetEventObject(this);
             GetEventHandler()->ProcessEvent(event);
         }
     }
 }
 
 void wxSlider::DoMoveWindow(int x, int y, int width, int height)
 {
     m_x = x;
     m_y = y;
     m_width  = width;
     m_height = height;
 
     int  xborder, yborder;
     int  minValWidth, maxValWidth, textwidth, textheight;
     int  sliderBreadth;
 
     xborder = yborder = 0;
 
     if (GetWindowStyle() & wxSL_LABELS)
     {
         wxString text;
         int ht;
 
         // Get maximum text label width and height
         text.Printf("%d", m_rangeMin);
         GetTextExtent(text, &minValWidth, &textheight);
         text.Printf("%d", m_rangeMax);
         GetTextExtent(text, &maxValWidth, &ht);
         if(ht > textheight) {
             textheight = ht;
         }
         textwidth = (minValWidth > maxValWidth ? minValWidth : maxValWidth);
 
         xborder = textwidth + wxSLIDER_BORDERTEXT;
         yborder = textheight + wxSLIDER_BORDERTEXT;
 
         // Get slider breadth
         if(GetWindowStyle() & wxSL_AUTOTICKS) {
             sliderBreadth = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
         }
         else {
             sliderBreadth = wxSLIDER_DIMENSIONACROSS_ARROW;
         }
 
         if(GetWindowStyle() & wxSL_VERTICAL)
         {
             m_macMinimumStatic->Move(sliderBreadth + wxSLIDER_BORDERTEXT,
                                      height - yborder - textheight);
             m_macMaximumStatic->Move(sliderBreadth + wxSLIDER_BORDERTEXT, 0);
             m_macValueStatic->Move(0, height - textheight);
         }
         else
         {
             m_macMinimumStatic->Move(0, sliderBreadth + wxSLIDER_BORDERTEXT);
             m_macMaximumStatic->Move(width - xborder - maxValWidth / 2,
                                      sliderBreadth + wxSLIDER_BORDERTEXT);
             m_macValueStatic->Move(width - textwidth, 0);
         }
     }
 
     if(GetParent()) {
         GetParent()->MacClientToRootWindow(&x, &y);
     }
     UMAMoveControl(m_macControl, x, y);
     UMASizeControl(m_macControl, width - xborder, height - yborder);
}