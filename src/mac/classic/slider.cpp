/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/slider.cpp
// Purpose:     wxSlider
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SLIDER

#include "wx/slider.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

BEGIN_EVENT_TABLE(wxSlider, wxControl)
END_EVENT_TABLE()

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
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

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

    MacPreControlCreate( parent, id, wxEmptyString, pos, size, style,
        validator, name, &bounds, title );

    procID = kControlSliderProc + kControlSliderLiveFeedback;
    if(style & wxSL_AUTOTICKS) {
        procID += kControlSliderHasTickMarks;
    }


    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()), &bounds, title, false,
        value, minValue, maxValue, procID, (long) this);

    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;

    ::SetControlAction( (ControlHandle) m_macControl , wxMacLiveScrollbarActionUPP ) ;

    if(style & wxSL_LABELS)
    {
        m_macMinimumStatic = new wxStaticText( this, wxID_ANY, wxEmptyString );
        m_macMaximumStatic = new wxStaticText( this, wxID_ANY, wxEmptyString );
        m_macValueStatic = new wxStaticText( this, wxID_ANY, wxEmptyString );
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
    return GetControl32BitValue( (ControlHandle) m_macControl) ;
}

void wxSlider::SetValue(int value)
{
    wxString valuestring ;
    valuestring.Printf( wxT("%d") , value ) ;
    if ( m_macValueStatic )
        m_macValueStatic->SetLabel( valuestring ) ;
    SetControl32BitValue( (ControlHandle) m_macControl , value ) ;
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    wxString value;

    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    SetControl32BitMinimum( (ControlHandle) m_macControl, m_rangeMin);
    SetControl32BitMaximum( (ControlHandle) m_macControl, m_rangeMax);

    if(m_macMinimumStatic) {
        value.Printf(wxT("%d"), m_rangeMin);
        m_macMinimumStatic->SetLabel(value);
    }
    if(m_macMaximumStatic) {
        value.Printf(wxT("%d"), m_rangeMax);
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

void wxSlider::MacHandleControlClick( WXWidget control , wxInt16 controlpart, bool mouseStillDown )
{
    SInt16 value = ::GetControl32BitValue( (ControlHandle) m_macControl ) ;

    SetValue( value ) ;

    wxEventType scrollEvent = wxEVT_NULL ;

   if ( mouseStillDown )
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
    else
        scrollEvent = wxEVT_SCROLL_THUMBRELEASE;

    wxScrollEvent event(scrollEvent, m_windowId);
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
void wxSlider::DoSetSizeHints( int minW, int minH,
                               int maxW , int maxH ,
                               int incW , int incH )
{
    wxSize size = GetBestSize();

    if(GetWindowStyle() & wxSL_VERTICAL) {
        wxWindow::DoSetSizeHints(size.x, minH, size.x, maxH, incW, incH);
    }
    else {
        wxWindow::DoSetSizeHints(minW, size.y, maxW, size.y, incW, incH);
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
        text.Printf(wxT("%d"), m_rangeMin);
        GetTextExtent(text, &textwidth, &textheight);
        text.Printf(wxT("%d"), m_rangeMax);
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
    wxControl::DoSetSize( x, y , width , height ,sizeFlags ) ;
}

void wxSlider::MacUpdateDimensions()
{
    // actually in the current systems this should never be possible, but later reparenting
    // may become a reality

    if ( (ControlHandle) m_macControl == NULL )
        return ;

    if ( GetParent() == NULL )
        return ;

    WindowRef rootwindow = (WindowRef) MacGetRootWindow() ;
    if ( rootwindow == NULL )
        return ;

    int  xborder, yborder;
    int  minValWidth, maxValWidth, textwidth, textheight;
    int  sliderBreadth;

    xborder = yborder = 0;

    if (GetWindowStyle() & wxSL_LABELS)
    {
        wxString text;
        int ht;

        // Get maximum text label width and height
        text.Printf(wxT("%d"), m_rangeMin);
        GetTextExtent(text, &minValWidth, &textheight);
        text.Printf(wxT("%d"), m_rangeMax);
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
                m_height - yborder - textheight);
            m_macMaximumStatic->Move(sliderBreadth + wxSLIDER_BORDERTEXT, 0);
            m_macValueStatic->Move(0, m_height - textheight);
        }
        else
        {
            m_macMinimumStatic->Move(0, sliderBreadth + wxSLIDER_BORDERTEXT);
            m_macMaximumStatic->Move(m_width - xborder - maxValWidth / 2,
                sliderBreadth + wxSLIDER_BORDERTEXT);
            m_macValueStatic->Move(m_width - textwidth, 0);
        }
    }

    Rect oldBounds ;
    GetControlBounds( (ControlHandle) m_macControl , &oldBounds ) ;

    int new_x = m_x + MacGetLeftBorderSize() + m_macHorizontalBorder ;
    int new_y = m_y + MacGetTopBorderSize() + m_macVerticalBorder ;
    int new_width = m_width - MacGetLeftBorderSize() - MacGetRightBorderSize() - 2 * m_macHorizontalBorder - xborder ;
    int new_height = m_height - MacGetTopBorderSize() - MacGetBottomBorderSize() - 2 * m_macVerticalBorder - yborder ;

    GetParent()->MacWindowToRootWindow( & new_x , & new_y ) ;
    bool doMove = new_x != oldBounds.left || new_y != oldBounds.top ;
    bool doResize =  ( oldBounds.right - oldBounds.left ) != new_width || (oldBounds.bottom - oldBounds.top ) != new_height ;
    if ( doMove || doResize )
    {
        InvalWindowRect( rootwindow, &oldBounds ) ;
        if ( doMove )
        {
            UMAMoveControl( (ControlHandle) m_macControl , new_x , new_y ) ;
        }
        if ( doResize )
        {
            UMASizeControl( (ControlHandle) m_macControl , new_width , new_height ) ;
        }
    }
}

void wxSlider::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x,y,width,height) ;
}

#endif // wxUSE_SLIDER
