/////////////////////////////////////////////////////////////////////////////
// Name:        sashwin.cpp
// Purpose:     wxSashWindow implementation. A sash window has an optional
//              sash on each edge, allowing it to be dragged. An event
//              is generated when the sash is released.
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "sashwin.h"
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
#include "wx/dcscreen.h"
#include "wx/sashwin.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSashWindow, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxSashEvent, wxCommandEvent)

BEGIN_EVENT_TABLE(wxSashWindow, wxWindow)
    EVT_PAINT(wxSashWindow::OnPaint)
    EVT_SIZE(wxSashWindow::OnSize)
    EVT_MOUSE_EVENTS(wxSashWindow::OnMouseEvent)
END_EVENT_TABLE()
#endif

wxSashWindow::wxSashWindow()
{
    m_draggingEdge = wxSASH_NONE;
    m_dragMode = wxSASH_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_borderSize = 3 ;
    m_extraBorderSize = 0;
    m_sashCursorWE = NULL;
    m_sashCursorNS = NULL;

    m_minimumPaneSizeX = 0;
    m_minimumPaneSizeY = 0;
    m_maximumPaneSizeX = 10000;
    m_maximumPaneSizeY = 10000;
}

wxSashWindow::wxSashWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
  :wxWindow(parent, id, pos, size, style, name)
{
    m_draggingEdge = wxSASH_NONE;
    m_dragMode = wxSASH_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_borderSize = 3;
    m_extraBorderSize = 0;
    m_minimumPaneSizeX = 0;
    m_minimumPaneSizeY = 0;
    m_maximumPaneSizeX = 10000;
    m_maximumPaneSizeY = 10000;
    m_sashCursorWE = new wxCursor(wxCURSOR_SIZEWE);
    m_sashCursorNS = new wxCursor(wxCURSOR_SIZENS);

    // Eventually, we'll respond to colour change messages
    InitColours();
}

wxSashWindow::~wxSashWindow()
{
    delete m_sashCursorWE;
    delete m_sashCursorNS;
}

void wxSashWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

#if 0
    if ( m_borderSize > 0 )
        DrawBorders(dc);
#endif

    DrawSashes(dc);
}

void wxSashWindow::OnMouseEvent(wxMouseEvent& event)
{
    long x, y;
    event.Position(&x, &y);

    wxSashEdgePosition sashHit = SashHitTest(x, y);

	if (event.LeftDown())
	{
        if ( sashHit != wxSASH_NONE )
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
            m_dragMode = wxSASH_DRAG_LEFT_DOWN;
            m_draggingEdge = sashHit;
            m_firstX = x;
            m_firstY = y;
        }
	}
    else if ( event.LeftUp() && m_dragMode == wxSASH_DRAG_LEFT_DOWN )
    {
        // Wasn't a proper drag
        ReleaseMouse();
        wxScreenDC::EndDrawingOnTop();
        m_dragMode = wxSASH_DRAG_NONE;
        m_draggingEdge = wxSASH_NONE;

        SetCursor(*wxSTANDARD_CURSOR);
    }
	else if (event.LeftUp() && m_dragMode == wxSASH_DRAG_DRAGGING)
	{
        // We can stop dragging now and see what we've got.
        m_dragMode = wxSASH_DRAG_NONE;
		ReleaseMouse();
        // Erase old tracker
        DrawSashTracker(m_draggingEdge, m_oldX, m_oldY);

        // End drawing on top (frees the window used for drawing
        // over the screen)
        wxScreenDC::EndDrawingOnTop();

        int w, h;
		GetSize(&w, &h);
        int xp, yp;
		GetPosition(&xp, &yp);

        wxSashEdgePosition edge = m_draggingEdge;
        m_draggingEdge = wxSASH_NONE;

        wxRect dragRect;
        wxSashDragStatus status = wxSASH_STATUS_OK;
        switch (edge)
        {
            case wxSASH_TOP:
            {
                if (y > (yp + h))
                    status = wxSASH_STATUS_OUT_OF_RANGE;
                int newHeight = (h - y);
		newHeight=wxMax(newHeight,m_minimumPaneSizeY);
		newHeight=wxMin(newHeight,m_maximumPaneSizeY);
                dragRect = wxRect(xp, (yp + h) - newHeight, w, newHeight);
                break;
            }
            case wxSASH_BOTTOM:
            {
                if (y < 0)
                    status = wxSASH_STATUS_OUT_OF_RANGE;
                int newHeight = y;
		newHeight=wxMax(newHeight,m_minimumPaneSizeY);
		newHeight=wxMin(newHeight,m_maximumPaneSizeY);
                dragRect = wxRect(xp, yp, w, newHeight);
                break;
            }
            case wxSASH_LEFT:
            {
                if (x > (xp + w))
                    status = wxSASH_STATUS_OUT_OF_RANGE;
                int newWidth = (w - x);
		newWidth=wxMax(newWidth,m_minimumPaneSizeX);
		newWidth=wxMin(newWidth,m_maximumPaneSizeX);
                dragRect = wxRect((xp + w) - newWidth, yp, newWidth, h);
                break;
            }
            case wxSASH_RIGHT:
            {
                if (x < 0)
                    status = wxSASH_STATUS_OUT_OF_RANGE;
                int newWidth = x;
		newWidth=wxMax(newWidth,m_minimumPaneSizeX);
		newWidth=wxMin(newWidth,m_maximumPaneSizeX);
                dragRect = wxRect(xp, yp, newWidth, h);
                break;
            }
	    case wxSASH_NONE:
	    {
	        break;
	    }
        }

        wxSashEvent event(GetId(), edge);
        event.SetEventObject(this);
        event.SetDragStatus(status);
        event.SetDragRect(dragRect);
        GetEventHandler()->ProcessEvent(event);
	}
	else if (event.Moving() && !event.Dragging())
	{
        // Just change the cursor if required
        if ( sashHit != wxSASH_NONE )
        {
            	if ( (sashHit == wxSASH_LEFT) || (sashHit == wxSASH_RIGHT) )
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
	else if ( event.Dragging() &&
              ((m_dragMode == wxSASH_DRAG_DRAGGING) || (m_dragMode == wxSASH_DRAG_LEFT_DOWN))
            )
	{
       	 if ( (m_draggingEdge == wxSASH_LEFT) || (m_draggingEdge == wxSASH_RIGHT) )
         {
	        SetCursor(*m_sashCursorWE);
         }
         else
         {
	        SetCursor(*m_sashCursorNS);
         }

        if (m_dragMode == wxSASH_DRAG_LEFT_DOWN)
        {
            m_dragMode = wxSASH_DRAG_DRAGGING;
            DrawSashTracker(m_draggingEdge, x, y);
        }
        else
        {
          if ( m_dragMode == wxSASH_DRAG_DRAGGING )
          {
            // Erase old tracker
            DrawSashTracker(m_draggingEdge, m_oldX, m_oldY);

            // Draw new one
            DrawSashTracker(m_draggingEdge, x, y);
          }
        }
        m_oldX = x;
        m_oldY = y;
	}
    else if ( event.LeftDClick() )
    {
        // Nothing
    }
    else
    {
    }
}

void wxSashWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    SizeWindows();
}

wxSashEdgePosition wxSashWindow::SashHitTest(int x, int y, int WXUNUSED(tolerance))
{
    int cx, cy;
    GetClientSize(& cx, & cy);

    int i;
    for (i = 0; i < 4; i++)
    {
        wxSashEdge& edge = m_sashes[i];
        wxSashEdgePosition position = (wxSashEdgePosition) i ;

        if (edge.m_show)
        {
            switch (position)
            {
                case wxSASH_TOP:
                {
                    if (y >= 0 && y <= GetEdgeMargin(position))
                        return wxSASH_TOP;
                    break;
                }
                case wxSASH_RIGHT:
                {
                    if ((x >= cx - GetEdgeMargin(position)) && (x <= cx))
                        return wxSASH_RIGHT;
                    break;
                }
                case wxSASH_BOTTOM:
                {
                    if ((y >= cy - GetEdgeMargin(position)) && (y <= cy))
                        return wxSASH_BOTTOM;
                    break;
                }
                case wxSASH_LEFT:
                {
                    if ((x >= GetEdgeMargin(position)) && (x >= 0))
                        return wxSASH_LEFT;
                    break;
                }
                case wxSASH_NONE:
                {
                    break;
                }
            }
        }
    }
    return wxSASH_NONE;
}

// Draw 3D effect borders
void wxSashWindow::DrawBorders(wxDC& dc)
{
    int w, h;
    GetClientSize(&w, &h);

    wxPen mediumShadowPen(m_mediumShadowColour, 1, wxSOLID);
    wxPen darkShadowPen(m_darkShadowColour, 1, wxSOLID);
    wxPen lightShadowPen(m_lightShadowColour, 1, wxSOLID);
    wxPen hilightPen(m_hilightColour, 1, wxSOLID);

    if ( GetWindowStyleFlag() & wxSP_3D )
    {
        dc.SetPen(mediumShadowPen);
        dc.DrawLine(0, 0, w-1, 0);
        dc.DrawLine(0, 0, 0, h - 1);

        dc.SetPen(darkShadowPen);
        dc.DrawLine(1, 1, w-2, 1);
        dc.DrawLine(1, 1, 1, h-2);

        dc.SetPen(hilightPen);
        dc.DrawLine(0, h-1, w-1, h-1);
        dc.DrawLine(w-1, 0, w-1, h); // Surely the maximum y pos. should be h - 1.
                                     /// Anyway, h is required for MSW.

        dc.SetPen(lightShadowPen);
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

void wxSashWindow::DrawSashes(wxDC& dc)
{
    int i;
    for (i = 0; i < 4; i++)
        if (m_sashes[i].m_show)
            DrawSash((wxSashEdgePosition) i, dc);
}

// Draw the sash
void wxSashWindow::DrawSash(wxSashEdgePosition edge, wxDC& dc)
{
    int w, h;
    GetClientSize(&w, &h);

    wxPen facePen(m_faceColour, 1, wxSOLID);
    wxBrush faceBrush(m_faceColour, wxSOLID);
    wxPen mediumShadowPen(m_mediumShadowColour, 1, wxSOLID);
    wxPen darkShadowPen(m_darkShadowColour, 1, wxSOLID);
    wxPen lightShadowPen(m_lightShadowColour, 1, wxSOLID);
    wxPen hilightPen(m_hilightColour, 1, wxSOLID);
    wxPen blackPen(wxColour(0, 0, 0), 1, wxSOLID);
    wxPen whitePen(wxColour(255, 255, 255), 1, wxSOLID);

    if ( edge == wxSASH_LEFT || edge == wxSASH_RIGHT )
    {
        int sashPosition = 0;
        if (edge == wxSASH_LEFT)
            sashPosition = 0;
        else
            sashPosition = w - GetEdgeMargin(edge);

        dc.SetPen(facePen);
        dc.SetBrush(faceBrush);
        dc.DrawRectangle(sashPosition, 0, GetEdgeMargin(edge), h);

        if (GetWindowStyleFlag() & wxSW_3D)
        {
            if (edge == wxSASH_LEFT)
            {
                // Draw a black line on the left to indicate that the
                // sash is raised
                dc.SetPen(blackPen);
                dc.DrawLine(GetEdgeMargin(edge), 0, GetEdgeMargin(edge), h);
            }
            else
            {
                // Draw a white line on the right to indicate that the
                // sash is raised
                dc.SetPen(whitePen);
                dc.DrawLine(w - GetEdgeMargin(edge), 0, w - GetEdgeMargin(edge), h);
            }
        }
    }
    else // top or bottom
    {
        int sashPosition = 0;
        if (edge == wxSASH_TOP)
            sashPosition = 0;
        else
            sashPosition = h - GetEdgeMargin(edge);

        dc.SetPen(facePen);
        dc.SetBrush(faceBrush);
        dc.DrawRectangle(0, sashPosition, w, GetEdgeMargin(edge));

        if (GetWindowStyleFlag() & wxSW_3D)
        {
            if (edge == wxSASH_BOTTOM)
            {
                // Draw a black line on the bottom to indicate that the
                // sash is raised
                dc.SetPen(blackPen);
                dc.DrawLine(0, h - GetEdgeMargin(edge), w, h - GetEdgeMargin(edge));
            }
            else
            {
                // Draw a white line on the top to indicate that the
                // sash is raised
                dc.SetPen(whitePen);
                dc.DrawLine(0, GetEdgeMargin(edge), w, GetEdgeMargin(edge));
            }
        }
    }

    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
}

// Draw the sash tracker (for whilst moving the sash)
void wxSashWindow::DrawSashTracker(wxSashEdgePosition edge, int x, int y)
{
    int w, h;
    GetClientSize(&w, &h);

    wxScreenDC screenDC;
    int x1, y1;
    int x2, y2;

    if ( edge == wxSASH_LEFT || edge == wxSASH_RIGHT )
    {
        x1 = x; y1 = 2;
        x2 = x; y2 = h-2;

        if ( (edge == wxSASH_LEFT) && (x1 > w) )
        {
            x1 = w; x2 = w;
        }
        else if ( (edge == wxSASH_RIGHT) && (x1 < 0) )
        {
            x1 = 0; x2 = 0;
        }
    }
    else
    {
        x1 = 2; y1 = y;
        x2 = w-2; y2 = y;

        if ( (edge == wxSASH_TOP) && (y1 > h) )
        {
            y1 = h;
            y2 = h;
        }
        else if ( (edge == wxSASH_BOTTOM) && (y1 < 0) )
        {
            y1 = 0;
            y2 = 0;
        }
    }

    ClientToScreen(&x1, &y1);
    ClientToScreen(&x2, &y2);

    wxPen sashTrackerPen(*wxBLACK, 2, wxSOLID);

    screenDC.SetLogicalFunction(wxXOR);
    screenDC.SetPen(sashTrackerPen);
    screenDC.SetBrush(*wxTRANSPARENT_BRUSH);

    screenDC.DrawLine(x1, y1, x2, y2);

    screenDC.SetLogicalFunction(wxCOPY);

    screenDC.SetPen(wxNullPen);
    screenDC.SetBrush(wxNullBrush);
}

// Position and size subwindows.
// Note that the border size applies to each subwindow, not
// including the edges next to the sash.
void wxSashWindow::SizeWindows()
{
    int cw, ch;
    GetClientSize(&cw, &ch);

    if (GetChildren().Number() > 0)
    {
        wxWindow* child = (wxWindow*) (GetChildren().First()->Data());

        int x = 0;
        int y = 0;
        int width = cw;
        int height = ch;

        // Top
        if (m_sashes[0].m_show)
        {
            y = m_borderSize;
            height -= m_borderSize;
        }
        y += m_extraBorderSize;

        // Left
        if (m_sashes[3].m_show)
        {
            x = m_borderSize;
            width -= m_borderSize;
        }
        x += m_extraBorderSize;

        // Right
        if (m_sashes[1].m_show)
        {
            width -= m_borderSize;
        }
        width -= 2*m_extraBorderSize;

        // Bottom
        if (m_sashes[2].m_show)
        {
            height -= m_borderSize;
        }
        height -= 2*m_extraBorderSize;

        child->SetSize(x, y, width, height);
    }

    wxClientDC dc(this);
    DrawBorders(dc);
    DrawSashes(dc);
}

// Initialize colours
void wxSashWindow::InitColours()
{
    // Shadow colours
#if defined(__WIN95__)
    m_faceColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    m_mediumShadowColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW);
    m_darkShadowColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DDKSHADOW);
    m_lightShadowColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT);
    m_hilightColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHILIGHT);
#else
    m_faceColour = *(wxTheColourDatabase->FindColour("LIGHT GREY"));
    m_mediumShadowColour = *(wxTheColourDatabase->FindColour("GREY"));
    m_darkShadowColour = *(wxTheColourDatabase->FindColour("BLACK"));
    m_lightShadowColour = *(wxTheColourDatabase->FindColour("LIGHT GREY"));
    m_hilightColour = *(wxTheColourDatabase->FindColour("WHITE"));
#endif
}

void wxSashWindow::SetSashVisible(wxSashEdgePosition edge, bool sash)
{
     m_sashes[edge].m_show = sash;
     if (sash)
        m_sashes[edge].m_margin = m_borderSize;
     else
        m_sashes[edge].m_margin = 0;
}

