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

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxScrolledWindow, wxWindow)
    EVT_SCROLL(wxScrolledWindow::OnScroll)
    EVT_SIZE(wxScrolledWindow::OnSize)
    EVT_PAINT(wxScrolledWindow::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxScrolledWindow, wxWindow)
#endif

#ifdef __WXMSW__
#include "windows.h"
#endif

#ifdef __WXMOTIF__
// For wxRETAINED implementation
#include <Xm/Xm.h>
#endif

wxScrolledWindow::wxScrolledWindow(void)
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

    return wxWindow::Create(parent, id, pos, size, style, name);
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
      (noUnitsX < m_xScrollPosition) ||
      (noUnitsY != 0 && m_yScrollLines == 0) ||
      (noUnitsY < m_yScrollPosition) ||
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
   
   if (do_refresh && !noRefresh) Refresh();
   
#ifdef __WXMSW__
   // Necessary?
    UpdateWindow ((HWND) GetHWND());
#endif
}

void wxScrolledWindow::OnScroll(wxScrollEvent& event)
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
            ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, (const wxRect *) NULL);
       else
            Refresh();
    }
    else
    {
        if (m_yScrollingEnabled)
            ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, (const wxRect *) NULL);
        else
            Refresh();
  }
}

int wxScrolledWindow::CalcScrollInc(wxScrollEvent& event)
{
  int pos = event.GetPosition();
  int orient = event.GetOrientation();

  int nScrollInc = 0;
  switch (event.GetEventType())
  {
    case wxEVT_SCROLL_TOP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = - m_xScrollPosition;
      else
        nScrollInc = - m_yScrollPosition;
      break;
    }
    case wxEVT_SCROLL_BOTTOM:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = m_xScrollLines - m_xScrollPosition;
      else
        nScrollInc = m_yScrollLines - m_yScrollPosition;
      break;
    }
    case wxEVT_SCROLL_LINEUP:
    {
      nScrollInc = -1;
      break;
    }
    case wxEVT_SCROLL_LINEDOWN:
    {
      nScrollInc = 1;
      break;
    }
    case wxEVT_SCROLL_PAGEUP:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
      else
        nScrollInc = -GetScrollPageSize(wxVERTICAL);
      break;
    }
    case wxEVT_SCROLL_PAGEDOWN:
    {
      if (orient == wxHORIZONTAL)
        nScrollInc = GetScrollPageSize(wxHORIZONTAL);
      else
        nScrollInc = GetScrollPageSize(wxVERTICAL);
      break;
    }
    case wxEVT_SCROLL_THUMBTRACK:
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
    if (m_xScrollPixelsPerLine > 0) {
      int w, h;
      GetClientSize(&w, &h);

      int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
      int noPositions = (int) ( ((nMaxWidth - w)/(float)m_xScrollPixelsPerLine) + 0.5 );
      if (noPositions < 0)
	noPositions = 0;

      if ( (m_xScrollPosition + nScrollInc) < 0 )
	nScrollInc = -m_xScrollPosition; // As -ve as we can go
      else if ( (m_xScrollPosition + nScrollInc) > noPositions )
	nScrollInc = noPositions - m_xScrollPosition; // As +ve as we can go
    }
    else
      Refresh();
  }
  else
  {
    if (m_yScrollPixelsPerLine > 0) {
      int w, h;
      GetClientSize(&w, &h);
      
      int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
      int noPositions = (int) ( ((nMaxHeight - h)/(float)m_yScrollPixelsPerLine) + 0.5 );
      if (noPositions < 0)
	noPositions = 0;
      
      if ( (m_yScrollPosition + nScrollInc) < 0 )
	nScrollInc = -m_yScrollPosition; // As -ve as we can go
      else if ( (m_yScrollPosition + nScrollInc) > noPositions )
	nScrollInc = noPositions - m_yScrollPosition; // As +ve as we can go
    }
    else
      Refresh();
  }

  return nScrollInc;
}

// Adjust the scrollbars - new version.
void wxScrolledWindow::AdjustScrollbars(void)
{
    int w, h;
    GetClientSize(&w, &h);

    if (m_xScrollLines > 0)
    {
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (w/(float)m_xScrollPixelsPerLine) + 0.5 );
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
        int noPagePositions = (int) ( (h/(float)m_yScrollPixelsPerLine) + 0.5 );
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
#endif

void wxScrolledWindow::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
      *x_unit = m_xScrollPixelsPerLine;
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
    GetClientSize(&w, &h);

    if (x_pos != -1)
    {
        m_xScrollPosition = x_pos;
    
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (w/(float)m_xScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
        m_xScrollPosition = wxMin( m_xScrollLines-noPagePositions, m_xScrollPosition );
        m_xScrollPosition = wxMax( 0, m_xScrollPosition );
      
        SetScrollPos( wxHORIZONTAL, m_xScrollPosition, TRUE );
    }
    if (y_pos != -1)
    {
        m_yScrollPosition = y_pos;
	
	// Calculate page size i.e. number of scroll units you get on the
	// current client window
        int noPagePositions = (int) ( (h/(float)m_yScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
	// the visible portion of it or if below zero
        m_yScrollPosition = wxMin( m_yScrollLines-noPagePositions, m_yScrollPosition );
        m_yScrollPosition = wxMax( 0, m_yScrollPosition );
	
        SetScrollPos( wxVERTICAL, m_yScrollPosition, TRUE );
    }
    
    Refresh();
    
#ifdef __WXMSW__
    // Necessary?
    ::UpdateWindow ((HWND) GetHWND());
#endif
}

void wxScrolledWindow::EnableScrolling (bool x_scroll, bool y_scroll)
{
    m_xScrollingEnabled = x_scroll;
    m_yScrollingEnabled = y_scroll;
}

void wxScrolledWindow::GetVirtualSize (int *x, int *y) const
{
    *x = m_xScrollPixelsPerLine * m_xScrollLines;
    *y = m_yScrollPixelsPerLine * m_yScrollLines;
}

// Where the current view starts from
void wxScrolledWindow::ViewStart (int *x, int *y) const
{
    *x = m_xScrollPosition;
    *y = m_yScrollPosition;
}

void wxScrolledWindow::CalcScrolledPosition(int x, int y, int *xx, int *yy) const
{
    *xx = x - m_xScrollPosition * m_xScrollPixelsPerLine;
    *yy = y - m_yScrollPosition * m_yScrollPixelsPerLine;
}

void wxScrolledWindow::CalcUnscrolledPosition(int x, int y, float *xx, float *yy) const
{
    *xx = (float)(x + m_xScrollPosition * m_xScrollPixelsPerLine);
    *yy = (float)(y + m_yScrollPosition * m_yScrollPixelsPerLine);
}


