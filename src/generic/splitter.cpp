/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.cpp
// Purpose:     wxSplitterWindow implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "splitter.h"
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <math.h>
#include <stdlib.h>

#include "wx/string.h"
#include "wx/splitter.h"
#include "wx/dcscreen.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSplitterWindow, wxWindow)

BEGIN_EVENT_TABLE(wxSplitterWindow, wxWindow)
    EVT_PAINT(wxSplitterWindow::OnPaint)
    EVT_SIZE(wxSplitterWindow::OnSize)
    EVT_MOUSE_EVENTS(wxSplitterWindow::OnMouseEvent)
END_EVENT_TABLE()
#endif

wxSplitterWindow::wxSplitterWindow(void)
{
    m_splitMode = wxSPLIT_VERTICAL;
    m_windowOne = NULL;
    m_windowTwo = NULL;
    m_dragMode = wxSPLIT_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_sashSize = 7;
    m_borderSize = 2;
    m_sashPosition = 0;
    m_sashCursorWE = NULL;
    m_sashCursorNS = NULL;
    m_sashTrackerPen = NULL;
    m_lightShadowPen = NULL;
    m_mediumShadowPen = NULL;
    m_darkShadowPen = NULL;
    m_faceBrush = NULL;
    m_facePen = NULL;
    m_hilightPen = NULL;
    m_minimumPaneSize = 0;
}

wxSplitterWindow::wxSplitterWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
  :wxWindow(parent, id, pos, size, style, name)
{
    m_splitMode = wxSPLIT_VERTICAL;
    m_windowOne = NULL;
    m_windowTwo = NULL;
    m_dragMode = wxSPLIT_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_sashSize = 7;
    m_borderSize = 2;
    m_sashPosition = 0;
    m_minimumPaneSize = 0;
    m_sashCursorWE = new wxCursor(wxCURSOR_SIZEWE);
    m_sashCursorNS = new wxCursor(wxCURSOR_SIZENS);
    m_sashTrackerPen = new wxPen(*wxBLACK, 2, wxSOLID);
    m_lightShadowPen = NULL;
    m_mediumShadowPen = NULL;
    m_darkShadowPen = NULL;
    m_faceBrush = NULL;
    m_facePen = NULL;
    m_hilightPen = NULL;

    if ( style & wxSP_3D )
    {
        m_borderSize = 2;
        m_sashSize = 7;
    }
    else if ( style & wxSP_BORDER )
    {
        m_borderSize = 1;
        m_sashSize = 3;
    }
    else
    {
        m_borderSize = 0;
        m_sashSize = 3;
    }

    // Eventually, we'll respond to colour change messages
    InitColours();

    SetDoubleClick(TRUE);

    // For debugging purposes, to see the background.
//    SetBackground(wxBLUE_BRUSH);
}

wxSplitterWindow::~wxSplitterWindow(void)
{
    delete m_sashCursorWE;
    delete m_sashCursorNS;
    delete m_sashTrackerPen;
    delete m_lightShadowPen;
    delete m_darkShadowPen;
    delete m_mediumShadowPen;
    delete m_hilightPen;
    delete m_facePen;
    delete m_faceBrush;
}

void wxSplitterWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if ( m_borderSize > 0 )
        DrawBorders(dc);
    DrawSash(dc);
}

void wxSplitterWindow::OnMouseEvent(wxMouseEvent& event)
{
    long x, y;
    event.Position(&x, &y);

	if (event.LeftDown())
	{
        if ( SashHitTest(x, y) )
        {
	        CaptureMouse();

    	    // Required for X to specify that
	        // that we wish to draw on top of all windows
	        // - and we optimise by specifying the area
	        // for creating the overlap window.
	        wxScreenDC::StartDrawingOnTop(this);

            // We don't say we're dragging yet; we leave that
            // decision for the Dragging() branch, to ensure
            // the user has dragged a little bit.
            m_dragMode = wxSPLIT_DRAG_LEFT_DOWN;
            m_firstX = x;
            m_firstY = y;
        }
	}
    else if ( event.LeftUp() && m_dragMode == wxSPLIT_DRAG_LEFT_DOWN )
    {
        // Wasn't a proper drag
        ReleaseMouse();
        wxScreenDC::EndDrawingOnTop();
        m_dragMode = wxSPLIT_DRAG_NONE;

        SetCursor(*wxSTANDARD_CURSOR);
    }
	else if (event.LeftUp() && m_dragMode == wxSPLIT_DRAG_DRAGGING)
	{
        // We can stop dragging now and see what we've got.
        m_dragMode = wxSPLIT_DRAG_NONE;
		ReleaseMouse();
        // Erase old tracker
        DrawSashTracker(m_oldX, m_oldY);

        // End drawing on top (frees the window used for drawing
        // over the screen)
        wxScreenDC::EndDrawingOnTop();

        int w, h;
		GetClientSize(&w, &h);
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            // First check if we should veto this resize because
            // the pane size is too small
            if ( wxMax(x, 0) < m_minimumPaneSize || wxMax((w - x), 0) < m_minimumPaneSize)
                return;

            if ( x <= 4 )
            {
                // We remove the first window from the view
                wxWindow *removedWindow = m_windowOne;
                m_windowOne = m_windowTwo;
                m_windowTwo = NULL;

                OnUnsplit(removedWindow);
                m_sashPosition = 0;
            }
            else if ( x >= (w - 4) )
            {
                // We remove the second window from the view
                wxWindow *removedWindow = m_windowTwo;
                m_windowTwo = NULL;
                OnUnsplit(removedWindow);
                m_sashPosition = 0;
            }
            else
            {
                m_sashPosition = x;
            }
        }
        else
        {
            // First check if we should veto this resize because
            // the pane size is too small
            if ( wxMax(y, 0) < m_minimumPaneSize || wxMax((h - y), 0) < m_minimumPaneSize)
                return;

            if ( y <= 4 )
            {
                // We remove the first window from the view
                wxWindow *removedWindow = m_windowOne;
                m_windowOne = m_windowTwo;
                m_windowTwo = NULL;

                OnUnsplit(removedWindow);
                m_sashPosition = 0;
            }
            else if ( y >= (h - 4) )
            {
                // We remove the second window from the view
                wxWindow *removedWindow = m_windowTwo;
                m_windowTwo = NULL;
                OnUnsplit(removedWindow);
                m_sashPosition = 0;
            }
            else
            {
                m_sashPosition = y;
            }
        }
        SizeWindows();
	}
	else if (event.Moving() && !event.Dragging())
	{
        // Just change the cursor if required
        if ( SashHitTest(x, y) )
        {
            	if ( m_splitMode == wxSPLIT_VERTICAL )
                {
	                SetCursor(*m_sashCursorWE);
                }
                else
                {
	                SetCursor(*m_sashCursorNS);
                }
        }
        else
        {
    	    SetCursor(*wxSTANDARD_CURSOR);
        }
	}
	else if ( (event.Dragging() && (m_dragMode == wxSPLIT_DRAG_DRAGGING)) ||
              (event.Dragging() && SashHitTest(x, y, 4)) )
	{
         if ( m_splitMode == wxSPLIT_VERTICAL )
         {
	        SetCursor(*m_sashCursorWE);
         }
         else
         {
	        SetCursor(*m_sashCursorNS);
         }

        // Detect that this is really a drag: we've moved more than 1 pixel either way
        if ((m_dragMode == wxSPLIT_DRAG_LEFT_DOWN) &&
//                (abs((int)x - m_firstX) > 1 || abs((int)y - m_firstY) > 1) )
                (abs((int)x - m_firstX) > 0 || abs((int)y - m_firstY) > 1) )
        {
            m_dragMode = wxSPLIT_DRAG_DRAGGING;
            DrawSashTracker(x, y);
        }
        else
        {
          if ( m_dragMode == wxSPLIT_DRAG_DRAGGING )
          {
            // Erase old tracker
            DrawSashTracker(m_oldX, m_oldY);

            // Draw new one
            DrawSashTracker(x, y);
          }
        }
        m_oldX = x;
        m_oldY = y;
	}
    else if ( event.LeftDClick() )
    {
        OnDoubleClickSash(x, y);
    }
    else
    {
    }
}

void wxSplitterWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    int cw, ch;
    GetClientSize( &cw, &ch );
    if ( m_windowTwo )
    {
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            if ( m_sashPosition >= (cw - 5) )
                m_sashPosition = wxMax(10, cw - 40);
        }
        if ( m_splitMode == wxSPLIT_HORIZONTAL )
        {
            if ( m_sashPosition >= (ch - 5) )
                m_sashPosition = wxMax(10, ch - 40);
        }
    }
    SizeWindows();
}

bool wxSplitterWindow::SashHitTest(int x, int y, int tolerance)
{
    if ( m_windowTwo == NULL || m_sashPosition == 0)
        return FALSE; // No sash

    if ( m_splitMode == wxSPLIT_VERTICAL )
    {
        if ( (x >= m_sashPosition - tolerance) && (x <= m_sashPosition + m_sashSize + tolerance) )
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        if ( (y >= (m_sashPosition- tolerance)) && (y <= (m_sashPosition + m_sashSize + tolerance)) )
            return TRUE;
        else
            return FALSE;
    }

    return FALSE;
}

// Draw 3D effect borders
void wxSplitterWindow::DrawBorders(wxDC& dc)
{
    int w, h;
    GetClientSize(&w, &h);

    if ( GetWindowStyleFlag() & wxSP_3D )
    {
        dc.SetPen(*m_mediumShadowPen);
        dc.DrawLine(0, 0, w-1, 0);
        dc.DrawLine(0, 0, 0, h - 1);

        dc.SetPen(*m_darkShadowPen);
        dc.DrawLine(1, 1, w-2, 1);
        dc.DrawLine(1, 1, 1, h-2);

        dc.SetPen(*m_hilightPen);
        dc.DrawLine(0, h-1, w-1, h-1);
        dc.DrawLine(w-1, 0, w-1, h); // Surely the maximum y pos. should be h - 1.
                                     /// Anyway, h is required for MSW.

        dc.SetPen(*m_lightShadowPen);
        dc.DrawLine(w-2, 1, w-2, h-2); // Right hand side
        dc.DrawLine(1, h-2, w-1, h-2);     // Bottom
    }
    else if ( GetWindowStyleFlag() & wxSP_BORDER )
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawRectangle(0, 0, w-1, h-1);
    }

    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
}

// Draw the sash
void wxSplitterWindow::DrawSash(wxDC& dc)
{
    if ( m_sashPosition == 0 || !m_windowTwo)
        return;

    int w, h;
    GetClientSize(&w, &h);

    if ( GetWindowStyleFlag() & wxSP_3D )
    {
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            dc.SetPen(*m_facePen);
            dc.SetBrush(*m_faceBrush);
            dc.DrawRectangle(m_sashPosition + 2, 0, m_sashSize - 4, h);

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
            dc.DrawLine(m_sashPosition, 1, m_sashPosition, h-2);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(m_sashPosition+1, 0, m_sashPosition+1, h);

            dc.SetPen(*m_mediumShadowPen);
            dc.DrawLine(m_sashPosition+m_sashSize-2, 1, m_sashPosition+m_sashSize-2, h-1);

            dc.SetPen(*m_darkShadowPen);
			dc.DrawLine(m_sashPosition+m_sashSize-1, 2, m_sashPosition+m_sashSize-1, h-2);
		}
        else
        {
            dc.SetPen(*m_facePen);
            dc.SetBrush(*m_faceBrush);
            dc.DrawRectangle(0, m_sashPosition + 2, w, m_sashSize - 4);

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
            dc.DrawLine(1, m_sashPosition, w-2, m_sashPosition);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(0, m_sashPosition+1, w, m_sashPosition+1);

            dc.SetPen(*m_mediumShadowPen);
            dc.DrawLine(1, m_sashPosition+m_sashSize-2, w-1, m_sashPosition+m_sashSize-2);

            dc.SetPen(*m_darkShadowPen);
            dc.DrawLine(2, m_sashPosition+m_sashSize-1, w-2, m_sashPosition+m_sashSize-1);
        }
    }
    else
    {
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            dc.SetPen(*wxBLACK_PEN);
            dc.SetBrush(*wxBLACK_BRUSH);
            int h1 = h-1;
            if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER )
                h1 += 1; // Not sure why this is necessary...
            dc.DrawRectangle(m_sashPosition, 0, m_sashSize, h1);
        }
        else
        {
            dc.SetPen(*wxBLACK_PEN);
            dc.SetBrush(*wxBLACK_BRUSH);
            int w1 = w-1;
            if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER )
                w1 ++;

            dc.DrawRectangle(0, m_sashPosition, w1, m_sashSize);
        }

    }

    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
}

// Draw the sash tracker (for whilst moving the sash)
void wxSplitterWindow::DrawSashTracker(int x, int y)
{
    int w, h;
    GetClientSize(&w, &h);

    wxScreenDC screenDC;
    int x1, y1;
    int x2, y2;

    if ( m_splitMode == wxSPLIT_VERTICAL )
    {
        x1 = x; y1 = 2;
        x2 = x; y2 = h-2;

        if ( x1 > w )
        {
            x1 = w; x2 = w;
        }
        else if ( x1 < 0 )
        {
            x1 = 0; x2 = 0;
        }
    }
    else
    {
        x1 = 2; y1 = y;
        x2 = w-2; y2 = y;

        if ( y1 > h )
        {
            y1 = h;
            y2 = h;
        }
        else if ( y1 < 0 )
        {
            y1 = 0;
            y2 = 0;
        }
    }

    ClientToScreen(&x1, &y1);
    ClientToScreen(&x2, &y2);

    screenDC.SetLogicalFunction(wxXOR);
    screenDC.SetPen(*m_sashTrackerPen);
    screenDC.SetBrush(*wxTRANSPARENT_BRUSH);

    screenDC.DrawLine(x1, y1, x2, y2);

    screenDC.SetLogicalFunction(wxCOPY);

    screenDC.SetPen(wxNullPen);
    screenDC.SetBrush(wxNullBrush);
}

// Position and size subwindows.
// Note that the border size applies to each subwindow, not
// including the edges next to the sash.
void wxSplitterWindow::SizeWindows(void)
{
    int w, h;
    GetClientSize(&w, &h);

    if ( m_windowOne && !m_windowTwo )
    {
        m_windowOne->SetSize(m_borderSize, m_borderSize, w - 2*m_borderSize, h - 2*m_borderSize);
    }
    else if ( m_windowOne && m_windowTwo )
    {
        if (m_splitMode == wxSPLIT_VERTICAL)
        {
            int x1 = m_borderSize;
            int y1 = m_borderSize;
            int w1 = m_sashPosition - m_borderSize;
            int h1 = h - 2*m_borderSize;

            int x2 = m_sashPosition + m_sashSize;
            int y2 = m_borderSize;
            int w2 = w - 2*m_borderSize - m_sashSize - w1;
            int h2 = h - 2*m_borderSize;

            m_windowOne->SetSize(x1, y1,
                w1, h1);
            m_windowTwo->SetSize(x2, y2,
                w2, h2);
        }
        else
        {
            m_windowOne->SetSize(m_borderSize, m_borderSize,
                w - 2*m_borderSize, m_sashPosition - m_borderSize);
            m_windowTwo->SetSize(m_borderSize, m_sashPosition + m_sashSize,
                w - 2*m_borderSize, h - 2*m_borderSize - m_sashSize - (m_sashPosition - m_borderSize));
        }
    }
    wxClientDC dc(this);
    DrawBorders(dc);
    DrawSash(dc);
}

// Set pane for unsplit window
void wxSplitterWindow::Initialize(wxWindow *window)
{
    m_windowOne = window;
    m_windowTwo = NULL;
    m_sashPosition = 0;
}

// Associates the given window with window 2, drawing the appropriate sash
// and changing the split mode.
// Does nothing and returns FALSE if the window is already split.
bool wxSplitterWindow::SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition)
{
    if ( IsSplit() )
        return FALSE;

    m_splitMode = wxSPLIT_VERTICAL;
    m_windowOne = window1;
    m_windowTwo = window2;
    if ( sashPosition == -1 )
        m_sashPosition = 100;
    else
        m_sashPosition = sashPosition;

    SizeWindows();

    return TRUE;
}

bool wxSplitterWindow::SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition)
{
    if ( IsSplit() )
        return FALSE;

    m_splitMode = wxSPLIT_HORIZONTAL;
    m_windowOne = window1;
    m_windowTwo = window2;
    if ( sashPosition == -1 )
        m_sashPosition = 100;
    else
        m_sashPosition = sashPosition;

    SizeWindows();

    return TRUE;
}


// Remove the specified (or second) window from the view
// Doesn't actually delete the window.
bool wxSplitterWindow::Unsplit(wxWindow *toRemove)
{
    if ( ! IsSplit() )
        return FALSE;

    if ( toRemove == NULL || toRemove == m_windowTwo)
    {
        wxWindow *win = m_windowTwo ;
        m_windowTwo = NULL;
        m_sashPosition = 0;
        OnUnsplit(win);
        SizeWindows();
    }
    else if ( toRemove == m_windowOne )
    {
        wxWindow *win = m_windowOne ;
        m_windowOne = m_windowTwo;
        m_windowTwo = NULL;
        m_sashPosition = 0;
        OnUnsplit(win);
        SizeWindows();
    }
    else
        return FALSE;

    return TRUE;
}

void wxSplitterWindow::SetSashPosition(int position, bool redraw)
{
    m_sashPosition = position;

    if ( redraw )
    {
        SizeWindows();
    }
}

// Called when the sash is double-clicked.
// The default behaviour is to remove the sash if the
// minimum pane size is zero.
void wxSplitterWindow::OnDoubleClickSash(int WXUNUSED(x), int WXUNUSED(y) )
{
    if ( GetMinimumPaneSize() == 0 )
    {
        Unsplit();
    }
}

// Initialize colours
void wxSplitterWindow::InitColours(void)
{
    if ( m_facePen )
        delete m_facePen;
    if ( m_faceBrush )
        delete m_faceBrush;
    if ( m_mediumShadowPen )
        delete m_mediumShadowPen;
    if ( m_darkShadowPen )
        delete m_darkShadowPen;
    if ( m_lightShadowPen )
        delete m_lightShadowPen;
    if ( m_hilightPen )
        delete m_hilightPen;

    // Shadow colours
#if defined(__WIN95__)
//    COLORREF ref = ::GetSysColor(COLOR_3DFACE); // Normally light grey
    wxColour faceColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    m_facePen = new wxPen(faceColour, 1, wxSOLID);
    m_faceBrush = new wxBrush(faceColour, wxSOLID);

//    ref = ::GetSysColor(COLOR_3DSHADOW); // Normally dark grey
    wxColour mediumShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW));
    m_mediumShadowPen = new wxPen(mediumShadowColour, 1, wxSOLID);

//    ref = ::GetSysColor(COLOR_3DDKSHADOW); // Normally black
    wxColour darkShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DDKSHADOW));
    m_darkShadowPen = new wxPen(darkShadowColour, 1, wxSOLID);

//    ref = ::GetSysColor(COLOR_3DLIGHT); // Normally light grey
    wxColour lightShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT));
    m_lightShadowPen = new wxPen(lightShadowColour, 1, wxSOLID);

//    ref = ::GetSysColor(COLOR_3DHILIGHT); // Normally white
    wxColour hilightColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHILIGHT));
    m_hilightPen = new wxPen(hilightColour, 1, wxSOLID);
#else
    m_facePen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_faceBrush = new wxBrush("LIGHT GREY", wxSOLID);
    m_mediumShadowPen = new wxPen("GREY", 1, wxSOLID);
    m_darkShadowPen = new wxPen("BLACK", 1, wxSOLID);
    m_lightShadowPen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_hilightPen = new wxPen("WHITE", 1, wxSOLID);
#endif
}

