/////////////////////////////////////////////////////////////////////////////
// Name:        scrolwin.cpp
// Purpose:     wxScrolledWindow implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma implementation "scrolwin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/utils.h"
#include "wx/dcclient.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/generic/scrolwin.h"
#include "wx/panel.h"

BEGIN_EVENT_TABLE(wxScrolledWindow, wxPanel)
    EVT_SCROLLWIN(wxScrolledWindow::OnScroll)
    EVT_SIZE(wxScrolledWindow::OnSize)
    EVT_PAINT(wxScrolledWindow::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxScrolledWindow, wxPanel)

#ifdef __WXMSW__
#include "windows.h"
#endif

#ifdef __WXMOTIF__
// For wxRETAINED implementation
#ifdef __VMS__ //VMS's Xm.h is not (yet) compatible with C++
               //This code switches off the compiler warnings
# pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
# pragma message enable nosimpint
#endif
#endif

wxScrolledWindow::wxScrolledWindow()
{
    m_xScrollPixelsPerLine = 0;
    m_yScrollPixelsPerLine = 0;
    m_xScrollingEnabled = TRUE;
    m_yScrollingEnabled = TRUE;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
    m_xScrollLines = 0;
    m_yScrollLines = 0;
    m_xScrollLinesPerPage = 0;
    m_yScrollLinesPerPage = 0;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
}

bool wxScrolledWindow::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_xScrollPixelsPerLine = 0;
    m_yScrollPixelsPerLine = 0;
    m_xScrollingEnabled = TRUE;
    m_yScrollingEnabled = TRUE;
    m_xScrollPosition = 0;
    m_yScrollPosition = 0;
    m_xScrollLines = 0;
    m_yScrollLines = 0;
    m_xScrollLinesPerPage = 0;
    m_yScrollLinesPerPage = 0;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
    
    m_targetWindow = this;

    return wxPanel::Create(parent, id, pos, size, style, name);
}

/*
 * pixelsPerUnitX/pixelsPerUnitY: number of pixels per unit (e.g. pixels per text line)
 * noUnitsX/noUnitsY:        : no. units per scrollbar
 */
void wxScrolledWindow::SetScrollbars (int pixelsPerUnitX, int pixelsPerUnitY,
	       int noUnitsX, int noUnitsY,
	       int xPos, int yPos, bool noRefresh )
{
    bool do_refresh =
    (
      (noUnitsX != 0 && m_xScrollLines == 0) ||
      (noUnitsX < m_xScrollLines) ||
      (noUnitsY != 0 && m_yScrollLines == 0) ||
      (noUnitsY < m_yScrollLines) ||
      (xPos != m_xScrollPosition) ||
      (yPos != m_yScrollPosition) ||
      (pixelsPerUnitX != m_xScrollPixelsPerLine) ||
      (pixelsPerUnitY != m_yScrollPixelsPerLine)
    );
      
    m_xScrollPixelsPerLine = pixelsPerUnitX;
    m_yScrollPixelsPerLine = pixelsPerUnitY;
    m_xScrollPosition = xPos;
    m_yScrollPosition = yPos;
    m_xScrollLines = noUnitsX;
    m_yScrollLines = noUnitsY;

#ifdef __WXMOTIF__
    // Sorry, some Motif-specific code to implement a backing pixmap
    // for the wxRETAINED style. Implementing a backing store can't
    // be entirely generic because it relies on the wxWindowDC implementation
    // to duplicate X drawing calls for the backing pixmap.

    if ((m_windowStyle & wxRETAINED) == wxRETAINED)
    {
        Display* dpy = XtDisplay((Widget) GetMainWidget());

        int totalPixelWidth = m_xScrollLines * m_xScrollPixelsPerLine;
        int totalPixelHeight = m_yScrollLines * m_yScrollPixelsPerLine;
        if (m_backingPixmap &&
           !((m_pixmapWidth == totalPixelWidth) &&
             (m_pixmapHeight == totalPixelHeight)))
        {
            XFreePixmap (dpy, (Pixmap) m_backingPixmap);
            m_backingPixmap = (WXPixmap) 0;
        }

        if (!m_backingPixmap &&
           (noUnitsX != 0) && (noUnitsY != 0))
        {
            int depth = wxDisplayDepth();
            m_pixmapWidth = totalPixelWidth;
            m_pixmapHeight = totalPixelHeight;
            m_backingPixmap = (WXPixmap) XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
            m_pixmapWidth, m_pixmapHeight, depth);
	}

    }
#endif
      
    AdjustScrollbars();
   
    if (do_refresh && !noRefresh) 
        m_targetWindow->Refresh(); 
   
#ifdef __WXMSW__
    // GRG: if this turns out to be really necessary, we could
    //   at least move it to the above if { ... } so that it is
    //   only done if noRefresh = FALSE (the default). OTOH, if
    //   this doesn't break anything, which seems to be the
    //   case, we could just leave it out.

    // Necessary?
    // UpdateWindow ((HWND) m_targetWindow->GetHWND());
#endif
#ifdef __WXMAC__
		m_targetWindow->MacUpdateImmediately() ;
#endif
}

wxScrolledWindow::~wxScrolledWindow()
{
}

void wxScrolledWindow::SetTargetWindow( wxWindow *target )
{
    wxASSERT_MSG( target, wxT("target window must not be NULL") );
    m_targetWindow = target;
}

wxWindow *wxScrolledWindow::GetTargetWindow()
{
    return m_targetWindow;
}

void wxScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
    int orient = event.GetOrientation();

    int nScrollInc = CalcScrollInc(event);
    if (nScrollInc == 0) return;

    if (orient == wxHORIZONTAL)
    {
        int newPos = m_xScrollPosition + nScrollInc;
        SetScrollPos(wxHORIZONTAL, newPos, TRUE );
    }
    else
    {
        int newPos = m_yScrollPosition + nScrollInc;
        SetScrollPos(wxVERTICAL, newPos, TRUE );
    }

    if (orient == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
    }
    else
    {
        m_yScrollPosition += nScrollInc;
    }
  
    if (orient == wxHORIZONTAL)
    {
       if (m_xScrollingEnabled)
            m_targetWindow->ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, (const wxRect *) NULL);
       else
            m_targetWindow->Refresh();
    }
    else
    {
        if (m_yScrollingEnabled)
            m_targetWindow->ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, (const wxRect *) NULL);
        else
            m_targetWindow->Refresh();
  }
#ifdef __WXMAC__
	m_targetWindow->MacUpdateImmediately() ;
#endif
}

int wxScrolledWindow::CalcScrollInc(wxScrollWinEvent& event)
{
    int pos = event.GetPosition();
    int orient = event.GetOrientation();

    int nScrollInc = 0;
    switch (event.GetEventType())
    {
        case wxEVT_SCROLLWIN_TOP:
        {
            if (orient == wxHORIZONTAL)
                nScrollInc = - m_xScrollPosition;
            else
                nScrollInc = - m_yScrollPosition;
            break;
        }
        case wxEVT_SCROLLWIN_BOTTOM:
        {
            if (orient == wxHORIZONTAL)
                nScrollInc = m_xScrollLines - m_xScrollPosition;
            else
                nScrollInc = m_yScrollLines - m_yScrollPosition;
            break;
        }
        case wxEVT_SCROLLWIN_LINEUP:
        {
            nScrollInc = -1;
            break;
        }
        case wxEVT_SCROLLWIN_LINEDOWN:
        {
            nScrollInc = 1;
            break;
        }
        case wxEVT_SCROLLWIN_PAGEUP:
        {
            if (orient == wxHORIZONTAL)
                nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
            else
                nScrollInc = -GetScrollPageSize(wxVERTICAL);
            break;
        }
        case wxEVT_SCROLLWIN_PAGEDOWN:
        {
            if (orient == wxHORIZONTAL)
                nScrollInc = GetScrollPageSize(wxHORIZONTAL);
            else
                nScrollInc = GetScrollPageSize(wxVERTICAL);
            break;
        }
        case wxEVT_SCROLLWIN_THUMBTRACK:
        {
            if (orient == wxHORIZONTAL)
                nScrollInc = pos - m_xScrollPosition;
            else
                nScrollInc = pos - m_yScrollPosition;
            break;
        }
        default:
        {
            break;
        }
    }

    if (orient == wxHORIZONTAL)
    {
        if (m_xScrollPixelsPerLine > 0) 
        {
            int w, h;
            m_targetWindow->GetClientSize(&w, &h);

            int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
            int noPositions = (int) ( ((nMaxWidth - w)/(double)m_xScrollPixelsPerLine) + 0.5 );
            if (noPositions < 0)
	        noPositions = 0;

            if ( (m_xScrollPosition + nScrollInc) < 0 )
	        nScrollInc = -m_xScrollPosition; // As -ve as we can go
            else if ( (m_xScrollPosition + nScrollInc) > noPositions )
	        nScrollInc = noPositions - m_xScrollPosition; // As +ve as we can go
        }
        else
            m_targetWindow->Refresh();
    }
    else
    {
        if (m_yScrollPixelsPerLine > 0) 
	{
            int w, h;
            m_targetWindow->GetClientSize(&w, &h);
      
            int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
            int noPositions = (int) ( ((nMaxHeight - h)/(double)m_yScrollPixelsPerLine) + 0.5 );
            if (noPositions < 0)
	        noPositions = 0;
      
            if ( (m_yScrollPosition + nScrollInc) < 0 )
	        nScrollInc = -m_yScrollPosition; // As -ve as we can go
            else if ( (m_yScrollPosition + nScrollInc) > noPositions )
	        nScrollInc = noPositions - m_yScrollPosition; // As +ve as we can go
        }
        else
            m_targetWindow->Refresh();
    }

    return nScrollInc;
}

// Adjust the scrollbars - new version.
void wxScrolledWindow::AdjustScrollbars()
{
    int w, h;
    m_targetWindow->GetClientSize(&w, &h);
    
    int oldXScroll = m_xScrollPosition;
    int oldYScroll = m_yScrollPosition;

    if (m_xScrollLines > 0)
    {
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (w/(double)m_xScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
	m_xScrollPosition = wxMin( m_xScrollLines-noPagePositions, m_xScrollPosition);
        m_xScrollPosition = wxMax( 0, m_xScrollPosition );

        SetScrollbar(wxHORIZONTAL, m_xScrollPosition, noPagePositions, m_xScrollLines);
        // The amount by which we scroll when paging
        SetScrollPageSize(wxHORIZONTAL, noPagePositions);
    }
    else
    { 
        m_xScrollPosition = 0;
        SetScrollbar (wxHORIZONTAL, 0, 0, 0, FALSE);  
    }
    
    if (m_yScrollLines > 0)
    {
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (h/(double)m_yScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
        m_yScrollPosition = wxMin( m_yScrollLines-noPagePositions, m_yScrollPosition );
        m_yScrollPosition = wxMax( 0, m_yScrollPosition );

        SetScrollbar(wxVERTICAL, m_yScrollPosition, noPagePositions, m_yScrollLines);
        // The amount by which we scroll when paging
        SetScrollPageSize(wxVERTICAL, noPagePositions);
    }
    else
    {
        m_yScrollPosition = 0;
        SetScrollbar (wxVERTICAL, 0, 0, 0, FALSE); 
    }
    
    if (oldXScroll != m_xScrollPosition)
    {
       if (m_xScrollingEnabled)
            m_targetWindow->ScrollWindow( m_xScrollPixelsPerLine * (oldXScroll-m_xScrollPosition), 0, (const wxRect *) NULL );
       else
            m_targetWindow->Refresh();
    }
    
    if (oldYScroll != m_yScrollPosition)
    {
        if (m_yScrollingEnabled)
            m_targetWindow->ScrollWindow( 0, m_yScrollPixelsPerLine * (oldYScroll-m_yScrollPosition), (const wxRect *) NULL );
        else
            m_targetWindow->Refresh();
    }
}

// Default OnSize resets scrollbars, if any
void wxScrolledWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout()) Layout();
#endif

    AdjustScrollbars();
}

// This calls OnDraw, having adjusted the origin according to the current
// scroll position
void wxScrolledWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    OnDraw(dc);
}

// Override this function if you don't want to have wxScrolledWindow
// automatically change the origin according to the scroll position.
void wxScrolledWindow::PrepareDC(wxDC& dc)
{
    dc.SetDeviceOrigin( -m_xScrollPosition * m_xScrollPixelsPerLine, 
                        -m_yScrollPosition * m_yScrollPixelsPerLine );
    dc.SetUserScale( m_scaleX, m_scaleY );
}

#if WXWIN_COMPATIBILITY
void wxScrolledWindow::GetScrollUnitsPerPage (int *x_page, int *y_page) const
{
      *x_page = GetScrollPageSize(wxHORIZONTAL);
      *y_page = GetScrollPageSize(wxVERTICAL);
}

void wxScrolledWindow::CalcUnscrolledPosition(int x, int y, float *xx, float *yy) const
{
    if ( xx )
        *xx = (float)(x + m_xScrollPosition * m_xScrollPixelsPerLine);
    if ( yy )
        *yy = (float)(y + m_yScrollPosition * m_yScrollPixelsPerLine);
}
#endif // WXWIN_COMPATIBILITY

void wxScrolledWindow::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
    if ( x_unit )
        *x_unit = m_xScrollPixelsPerLine;
    if ( y_unit )
        *y_unit = m_yScrollPixelsPerLine;
}

int wxScrolledWindow::GetScrollPageSize(int orient) const
{
    if ( orient == wxHORIZONTAL )
        return m_xScrollLinesPerPage;
    else
        return m_yScrollLinesPerPage;
}

void wxScrolledWindow::SetScrollPageSize(int orient, int pageSize)
{
    if ( orient == wxHORIZONTAL )
        m_xScrollLinesPerPage = pageSize;
    else
        m_yScrollLinesPerPage = pageSize;
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */
void wxScrolledWindow::Scroll( int x_pos, int y_pos )
{
    if (((x_pos == -1) || (x_pos == m_xScrollPosition)) && 
        ((y_pos == -1) || (y_pos == m_yScrollPosition))) return;
  
    int w, h;
    m_targetWindow->GetClientSize(&w, &h);

    if (x_pos != -1)
    {
        int old_x = m_xScrollPosition;
        m_xScrollPosition = x_pos;
    
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (w/(double)m_xScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
        m_xScrollPosition = wxMin( m_xScrollLines-noPagePositions, m_xScrollPosition );
        m_xScrollPosition = wxMax( 0, m_xScrollPosition );
      
        m_targetWindow->SetScrollPos( wxHORIZONTAL, m_xScrollPosition, TRUE );
	
	m_targetWindow->ScrollWindow( (old_x-m_xScrollPosition)*m_xScrollPixelsPerLine, 0 );
    }
    if (y_pos != -1)
    {
        int old_y = m_yScrollPosition;
        m_yScrollPosition = y_pos;
	
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (h/(double)m_yScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
        m_yScrollPosition = wxMin( m_yScrollLines-noPagePositions, m_yScrollPosition );
        m_yScrollPosition = wxMax( 0, m_yScrollPosition );
	
        m_targetWindow->SetScrollPos( wxVERTICAL, m_yScrollPosition, TRUE );
	
	m_targetWindow->ScrollWindow( 0, (old_y-m_yScrollPosition)*m_yScrollPixelsPerLine );
    }
    
    
#ifdef __WXMSW__
//    ::UpdateWindow ((HWND) GetHWND());
#else
//    Refresh();
#endif
#ifdef __WXMAC__
		m_targetWindow->MacUpdateImmediately() ;
#endif
}

void wxScrolledWindow::EnableScrolling (bool x_scroll, bool y_scroll)
{
    m_xScrollingEnabled = x_scroll;
    m_yScrollingEnabled = y_scroll;
}

void wxScrolledWindow::GetVirtualSize (int *x, int *y) const
{
    if ( x )
        *x = m_xScrollPixelsPerLine * m_xScrollLines;
    if ( y )
        *y = m_yScrollPixelsPerLine * m_yScrollLines;
}

// Where the current view starts from
void wxScrolledWindow::ViewStart (int *x, int *y) const
{
    if ( x )
        *x = m_xScrollPosition;
    if ( y )
        *y = m_yScrollPosition;
}

void wxScrolledWindow::CalcScrolledPosition(int x, int y, int *xx, int *yy) const
{
    if ( xx )
        *xx = x - m_xScrollPosition * m_xScrollPixelsPerLine;
    if ( yy )
        *yy = y - m_yScrollPosition * m_yScrollPixelsPerLine;
}

void wxScrolledWindow::CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const
{
    if ( xx )
        *xx = x + m_xScrollPosition * m_xScrollPixelsPerLine;
    if ( yy )
        *yy = y + m_yScrollPosition * m_yScrollPixelsPerLine;
}
