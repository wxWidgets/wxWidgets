/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.cpp
// Purpose:     wxSplitterWindow implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "splitter.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
#endif

#include <stdlib.h>

#include "wx/string.h"
#include "wx/splitter.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSplitterWindow, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxSplitterEvent, wxCommandEvent)

BEGIN_EVENT_TABLE(wxSplitterWindow, wxWindow)
    EVT_PAINT(wxSplitterWindow::OnPaint)
    EVT_SIZE(wxSplitterWindow::OnSize)
    EVT_IDLE(wxSplitterWindow::OnIdle)
    EVT_MOUSE_EVENTS(wxSplitterWindow::OnMouseEvent)

    EVT_SPLITTER_SASH_POS_CHANGED(-1, wxSplitterWindow::OnSashPosChanged)
    // NB: we borrow OnSashPosChanged for purposes of
    // EVT_SPLITTER_SASH_POS_CHANGING since default implementation is identical
    EVT_SPLITTER_SASH_POS_CHANGING(-1, wxSplitterWindow::OnSashPosChanged)
    EVT_SPLITTER_DCLICK(-1,           wxSplitterWindow::OnDoubleClick)
    EVT_SPLITTER_UNSPLIT(-1,          wxSplitterWindow::OnUnsplitEvent)
END_EVENT_TABLE()
#endif

wxSplitterWindow::wxSplitterWindow()
{
    m_splitMode = wxSPLIT_VERTICAL;
    m_permitUnsplitAlways = FALSE;
    m_windowOne = (wxWindow *) NULL;
    m_windowTwo = (wxWindow *) NULL;
    m_dragMode = wxSPLIT_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_sashSize = 7;
    m_borderSize = 2;
    m_sashPosition = 0;
    m_sashCursorWE = (wxCursor *) NULL;
    m_sashCursorNS = (wxCursor *) NULL;
    m_sashTrackerPen = (wxPen *) NULL;
    m_lightShadowPen = (wxPen *) NULL;
    m_mediumShadowPen = (wxPen *) NULL;
    m_darkShadowPen = (wxPen *) NULL;
    m_faceBrush = (wxBrush *) NULL;
    m_facePen = (wxPen *) NULL;
    m_hilightPen = (wxPen *) NULL;
    m_minimumPaneSize = 0;
    m_needUpdating = FALSE;
}

wxSplitterWindow::wxSplitterWindow(wxWindow *parent, wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
                : wxWindow(parent, id, pos, size, style, name)
{
    m_splitMode = wxSPLIT_VERTICAL;
    m_permitUnsplitAlways = (style & wxSP_PERMIT_UNSPLIT) != 0;
    m_windowOne = (wxWindow *) NULL;
    m_windowTwo = (wxWindow *) NULL;
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
    m_lightShadowPen = (wxPen *) NULL;
    m_mediumShadowPen = (wxPen *) NULL;
    m_darkShadowPen = (wxPen *) NULL;
    m_faceBrush = (wxBrush *) NULL;
    m_facePen = (wxPen *) NULL;
    m_hilightPen = (wxPen *) NULL;

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

    // For debugging purposes, to see the background.
//    SetBackground(wxBLUE_BRUSH);

    m_needUpdating = FALSE;
}

wxSplitterWindow::~wxSplitterWindow()
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

void wxSplitterWindow::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    if (m_needUpdating)
        SizeWindows();
}

void wxSplitterWindow::OnMouseEvent(wxMouseEvent& event)
{
    long x = event.GetX();
    long y = event.GetY();

    // reset the cursor
#ifdef __WXMOTIF__
    SetCursor(* wxSTANDARD_CURSOR);
#endif
#ifdef __WXMSW__
    SetCursor(wxCursor());
#endif


    if (event.LeftDown())
    {
        if ( SashHitTest(x, y) )
        {
            CaptureMouse();

            m_dragMode = wxSPLIT_DRAG_DRAGGING;

            if ((GetWindowStyleFlag() & wxSP_LIVE_UPDATE) == 0)
            {
                DrawSashTracker(x, y);
            }

            m_oldX = x;
            m_oldY = y;
            return;
        }
    }
    else if (event.LeftUp() && m_dragMode == wxSPLIT_DRAG_DRAGGING)
    {
        // We can stop dragging now and see what we've got.
        m_dragMode = wxSPLIT_DRAG_NONE;
        ReleaseMouse();

        // Erase old tracker
        if ((GetWindowStyleFlag() & wxSP_LIVE_UPDATE) == 0)
        {
            DrawSashTracker(m_oldX, m_oldY);
        }

        // Obtain window size. We are only interested in the dimension the sash
        // splits up
        int w, h;
        GetClientSize(&w, &h);
        int window_size = (m_splitMode == wxSPLIT_VERTICAL ? w : h );
        int new_sash_position =
            (int) ( m_splitMode == wxSPLIT_VERTICAL ? x : y );

        wxSplitterEvent eventSplitter(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
                                      this);
        eventSplitter.m_data.pos = new_sash_position;
        if ( GetEventHandler()->ProcessEvent(eventSplitter) )
        {
            new_sash_position = eventSplitter.GetSashPosition();
            if ( new_sash_position == -1 )
            {
                // change not allowed
                return;
            }
        }

        if ( m_permitUnsplitAlways
                || m_minimumPaneSize == 0 )
        {
            // Deal with possible unsplit scenarios
            if ( new_sash_position == 0 )
            {
                // We remove the first window from the view
                wxWindow *removedWindow = m_windowOne;
                m_windowOne = m_windowTwo;
                m_windowTwo = (wxWindow *) NULL;
                SendUnsplitEvent(removedWindow);
                m_sashPosition = 0;
            }
            else if ( new_sash_position == window_size )
            {
                // We remove the second window from the view
                wxWindow *removedWindow = m_windowTwo;
                m_windowTwo = (wxWindow *) NULL;
                SendUnsplitEvent(removedWindow);
                m_sashPosition = 0;
            }
            else
            {
                m_sashPosition = new_sash_position;
            }
        }
        else
        {
            m_sashPosition = new_sash_position;
        }

        SizeWindows();
    }  // left up && dragging
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
#ifdef __WXGTK__
        else
        {
            // where else do we unset the cursor?
            SetCursor(* wxSTANDARD_CURSOR);
        }
#endif // __WXGTK__
    }
    else if (event.Dragging() && (m_dragMode == wxSPLIT_DRAG_DRAGGING))
    {
        // Obtain window size. We are only interested in the dimension the sash
        // splits up
        int new_sash_position =
            (int) ( m_splitMode == wxSPLIT_VERTICAL ? x : y );

        wxSplitterEvent eventSplitter(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING,
                                      this);
        eventSplitter.m_data.pos = new_sash_position;
        if ( GetEventHandler()->ProcessEvent(eventSplitter) )
        {
            new_sash_position = eventSplitter.GetSashPosition();
            if ( new_sash_position == -1 )
            {
                // change not allowed
                return;
            }
        }

        // Erase old tracker
        if ((GetWindowStyleFlag() & wxSP_LIVE_UPDATE) == 0)
        {
            DrawSashTracker(m_oldX, m_oldY);
        }

        if (m_splitMode == wxSPLIT_VERTICAL)
            x = new_sash_position;
        else
            y = new_sash_position;

        if (new_sash_position != -1)
        {
            // Only modify if permitted
            m_oldX = x;
            m_oldY = y;
        }

#ifdef __WXMSW__
        // As we captured the mouse, we may get the mouse events from outside
        // our window - for example, negative values in x, y. This has a weird
        // consequence under MSW where we use unsigned values sometimes and
        // signed ones other times: the coordinates turn as big positive
        // numbers and so the sash is drawn on the *right* side of the window
        // instead of the left (or bottom instead of top). Correct this.
        if ( (short)m_oldX < 0 )
            m_oldX = 0;
        if ( (short)m_oldY < 0 )
            m_oldY = 0;
#endif // __WXMSW__

        // Draw new one
        if ((GetWindowStyleFlag() & wxSP_LIVE_UPDATE) == 0)
        {
            DrawSashTracker(m_oldX, m_oldY);
        }
        else
        {
            m_sashPosition = new_sash_position;
            m_needUpdating = TRUE;
        }
    }
    else if ( event.LeftDClick() )
    {
        wxSplitterEvent eventSplitter(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED,
                                      this);
        eventSplitter.m_data.pt.x = x;
        eventSplitter.m_data.pt.y = y;

        (void)GetEventHandler()->ProcessEvent(eventSplitter);
    }
}

void wxSplitterWindow::OnSize(wxSizeEvent& event)
{
    // only process this message if we're not iconized - otherwise iconizing
    // and restoring a window containing the splitter has a funny side effect
    // of changing the splitter position!
    wxWindow *parent = GetParent();
    while ( parent && !parent->IsTopLevel() )
    {
        parent = parent->GetParent();
    }

    bool iconized = FALSE;
    wxFrame *frame = wxDynamicCast(parent, wxFrame);
    if ( frame )
        iconized = frame->IsIconized();
    else
    {
        wxDialog *dialog = wxDynamicCast(parent, wxDialog);
        if ( dialog )
            iconized = dialog->IsIconized();
        else
            wxFAIL_MSG(wxT("should have a top level frame or dialog parent!"));
    }

    if ( iconized )
    {
        event.Skip();
    }
    else
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

        dc.SetPen(*m_facePen);
        dc.SetBrush(*m_faceBrush);
        dc.DrawRectangle(1, 1 , w-1, m_borderSize-2 ); //high
        dc.DrawRectangle(1, m_borderSize-2 , m_borderSize-2, h-1 ); // left
        dc.DrawRectangle(w-m_borderSize+2, m_borderSize-2 , w-1, h-1 ); // right
        dc.DrawRectangle(m_borderSize-2, h-m_borderSize+2 , w-m_borderSize+2, h-1 ); //bottom

        dc.SetPen(*m_mediumShadowPen);
        dc.DrawLine(m_borderSize-2, m_borderSize-2, w-m_borderSize+1, m_borderSize-2);
        dc.DrawLine(m_borderSize-2, m_borderSize-2, m_borderSize-2, h-m_borderSize+1);

        dc.SetPen(*m_darkShadowPen);
        dc.DrawLine(m_borderSize-1, m_borderSize-1, w-m_borderSize, m_borderSize-1);
        dc.DrawLine(m_borderSize-1, m_borderSize-1, m_borderSize-1, h-m_borderSize);

        dc.SetPen(*m_hilightPen);
        dc.DrawLine(m_borderSize - 2, h-m_borderSize+1, w-m_borderSize+1, h-m_borderSize+1);
        dc.DrawLine(w-m_borderSize+1, m_borderSize - 2, w-m_borderSize+1, h-m_borderSize+2); // Surely the maximum y pos. should be h - 1.
                                     /// Anyway, h is required for MSW.

        dc.SetPen(*m_lightShadowPen);
        dc.DrawLine(w-m_borderSize, m_borderSize-1, w-m_borderSize, h-m_borderSize); // Right hand side
        dc.DrawLine(m_borderSize-1, h-m_borderSize, w-m_borderSize+1, h-m_borderSize);     // Bottom
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
            dc.DrawRectangle(m_sashPosition + 2, 0 , m_sashSize - 4, h );

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
                        int xShadow = m_borderSize ? m_borderSize - 1 : 0 ;
            dc.DrawLine(m_sashPosition, xShadow , m_sashPosition, h-m_borderSize);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(m_sashPosition+1, m_borderSize - 2, m_sashPosition+1, h - m_borderSize+2);

            dc.SetPen(*m_mediumShadowPen);
                        int yMedium = m_borderSize ? h-m_borderSize+1 : h ;
            dc.DrawLine(m_sashPosition+m_sashSize-2, xShadow, m_sashPosition+m_sashSize-2, yMedium);

            dc.SetPen(*m_darkShadowPen);
            dc.DrawLine(m_sashPosition+m_sashSize-1, m_borderSize, m_sashPosition+m_sashSize-1, h-m_borderSize );
        }
        else
        {
            dc.SetPen(*m_facePen);
            dc.SetBrush(*m_faceBrush);
            dc.DrawRectangle( m_borderSize-2, m_sashPosition + 2, w-m_borderSize+2, m_sashSize - 4);

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
            dc.DrawLine(m_borderSize-1, m_sashPosition, w-m_borderSize, m_sashPosition);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(m_borderSize-2, m_sashPosition+1, w-m_borderSize+1, m_sashPosition+1);

            dc.SetPen(*m_mediumShadowPen);
            dc.DrawLine(m_borderSize-1, m_sashPosition+m_sashSize-2, w-m_borderSize+1, m_sashPosition+m_sashSize-2);

            dc.SetPen(*m_darkShadowPen);
            dc.DrawLine(m_borderSize, m_sashPosition+m_sashSize-1, w-m_borderSize, m_sashPosition+m_sashSize-1);
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

    screenDC.SetLogicalFunction(wxINVERT);
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
void wxSplitterWindow::SizeWindows()
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

            m_windowOne->SetSize(x1, y1, w1, h1);
            m_windowTwo->SetSize(x2, y2, w2, h2);

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
    if ( m_borderSize > 0 )
        DrawBorders(dc);
    DrawSash(dc);
}

// Set pane for unsplit window
void wxSplitterWindow::Initialize(wxWindow *window)
{
    m_windowOne = window;
    m_windowTwo = (wxWindow *) NULL;
    m_sashPosition = 0;
}

// Associates the given window with window 2, drawing the appropriate sash
// and changing the split mode.
// Does nothing and returns FALSE if the window is already split.
bool wxSplitterWindow::SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition)
{
    if ( IsSplit() )
        return FALSE;

    int w, h;
    GetClientSize(&w, &h);

    m_splitMode = wxSPLIT_VERTICAL;
    m_windowOne = window1;
    m_windowTwo = window2;
    if ( sashPosition > 0 )
        m_sashPosition = sashPosition;
    else if ( sashPosition < 0 )
        m_sashPosition = w - sashPosition;
    else    // default
        m_sashPosition = w/2;

    SizeWindows();

    return TRUE;
}

bool wxSplitterWindow::SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition)
{
    if ( IsSplit() )
        return FALSE;

    int w, h;
    GetClientSize(&w, &h);

    m_splitMode = wxSPLIT_HORIZONTAL;
    m_windowOne = window1;
    m_windowTwo = window2;
    if ( sashPosition > 0 )
        m_sashPosition = sashPosition;
    else if ( sashPosition < 0 )
        m_sashPosition = h - sashPosition;
    else    // default
        m_sashPosition = h/2;

    SizeWindows();

    return TRUE;
}


// Remove the specified (or second) window from the view
// Doesn't actually delete the window.
bool wxSplitterWindow::Unsplit(wxWindow *toRemove)
{
    if ( ! IsSplit() )
        return FALSE;

    wxWindow *win = NULL;
    if ( toRemove == NULL || toRemove == m_windowTwo)
    {
        win = m_windowTwo ;
        m_windowTwo = (wxWindow *) NULL;
    }
    else if ( toRemove == m_windowOne )
    {
        win = m_windowOne ;
        m_windowOne = m_windowTwo;
        m_windowTwo = (wxWindow *) NULL;
    }
    else
    {
        wxFAIL_MSG(wxT("splitter: attempt to remove a non-existent window"));

        return FALSE;
    }

    SendUnsplitEvent(win);
    m_sashPosition = 0;
    SizeWindows();

    return TRUE;
}

// Replace a window with another one
bool wxSplitterWindow::ReplaceWindow(wxWindow *winOld, wxWindow *winNew)
{
    wxCHECK_MSG( winOld, FALSE, wxT("use one of Split() functions instead") );
    wxCHECK_MSG( winNew, FALSE, wxT("use Unsplit() functions instead") );

    if ( winOld == m_windowTwo )
    {
        m_windowTwo = winNew;
    }
    else if ( winOld == m_windowOne )
    {
        m_windowOne = winNew;
    }
    else
    {
        wxFAIL_MSG(wxT("splitter: attempt to replace a non-existent window"));

        return FALSE;
    }

    SizeWindows();

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

// Initialize colours
void wxSplitterWindow::InitColours()
{
    wxDELETE( m_facePen );
    wxDELETE( m_faceBrush );
    wxDELETE( m_mediumShadowPen );
    wxDELETE( m_darkShadowPen );
    wxDELETE( m_lightShadowPen );
    wxDELETE( m_hilightPen );

    // Shadow colours
#if defined(__WIN95__)
    wxColour faceColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    m_facePen = new wxPen(faceColour, 1, wxSOLID);
    m_faceBrush = new wxBrush(faceColour, wxSOLID);

    wxColour mediumShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW));
    m_mediumShadowPen = new wxPen(mediumShadowColour, 1, wxSOLID);

    wxColour darkShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DDKSHADOW));
    m_darkShadowPen = new wxPen(darkShadowColour, 1, wxSOLID);

    wxColour lightShadowColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT));
    m_lightShadowPen = new wxPen(lightShadowColour, 1, wxSOLID);

    wxColour hilightColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHILIGHT));
    m_hilightPen = new wxPen(hilightColour, 1, wxSOLID);
#else // !Win32
    m_facePen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_faceBrush = new wxBrush("LIGHT GREY", wxSOLID);
    m_mediumShadowPen = new wxPen("GREY", 1, wxSOLID);
    m_darkShadowPen = new wxPen("BLACK", 1, wxSOLID);
    m_lightShadowPen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_hilightPen = new wxPen("WHITE", 1, wxSOLID);
#endif // Win32/!Win32
}

void wxSplitterWindow::SendUnsplitEvent(wxWindow *winRemoved)
{
    wxSplitterEvent event(wxEVT_COMMAND_SPLITTER_UNSPLIT, this);
    event.m_data.win = winRemoved;

    (void)GetEventHandler()->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// splitter event handlers
// ---------------------------------------------------------------------------

void wxSplitterWindow::OnSashPosChanged(wxSplitterEvent& event)
{
    // If within UNSPLIT_THRESHOLD from edge, set to edge to cause closure.
    const int UNSPLIT_THRESHOLD = 4;

    int newSashPosition = event.GetSashPosition();

    // Obtain relevant window dimension for bottom / right threshold check
    int w, h;
    GetClientSize(&w, &h);
    int window_size = (m_splitMode == wxSPLIT_VERTICAL) ? w : h ;

    bool unsplit_scenario = FALSE;
    if ( m_permitUnsplitAlways
            || m_minimumPaneSize == 0 )
    {
        // Do edge detection if unsplit premitted
        if ( newSashPosition <= UNSPLIT_THRESHOLD )
        {
            // threshold top / left check
            newSashPosition = 0;
            unsplit_scenario = TRUE;
        }
        if ( newSashPosition >= window_size - UNSPLIT_THRESHOLD )
        {
            // threshold bottom/right check
            newSashPosition = window_size;
            unsplit_scenario = TRUE;
        }
    }

    if ( !unsplit_scenario )
    {
        // If resultant pane would be too small, enlarge it
        if ( newSashPosition < m_minimumPaneSize )
            newSashPosition = m_minimumPaneSize;
        if ( newSashPosition > window_size - m_minimumPaneSize )
            newSashPosition = window_size - m_minimumPaneSize;
    }

    // If the result is out of bounds it means minimum size is too big,
    // so split window in half as best compromise.
    if ( newSashPosition < 0 || newSashPosition > window_size )
        newSashPosition = window_size / 2;

    // for compatibility, call the virtual function
    if ( !OnSashPositionChange(newSashPosition) )
    {
        newSashPosition = -1;
    }

    event.SetSashPosition(newSashPosition);
}

// Called when the sash is double-clicked. The default behaviour is to remove
// the sash if the minimum pane size is zero.
void wxSplitterWindow::OnDoubleClick(wxSplitterEvent& event)
{
    // for compatibility, call the virtual function
    OnDoubleClickSash(event.GetX(), event.GetY());

    if ( GetMinimumPaneSize() == 0 || m_permitUnsplitAlways )
    {
        Unsplit();
    }
}

void wxSplitterWindow::OnUnsplitEvent(wxSplitterEvent& event)
{
    wxWindow *win = event.GetWindowBeingRemoved();

    // do it before calling OnUnsplit() which may delete the window
    win->Show(FALSE);

    // for compatibility, call the virtual function
    OnUnsplit(win);
}
