/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/splitter.cpp
// Purpose:     wxSplitterWindow implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/log.h"

    #include "wx/dcscreen.h"

    #include "wx/window.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"

    #include "wx/settings.h"
#endif

#include "wx/splitter.h"

#include <stdlib.h>

DEFINE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_UNSPLIT)

IMPLEMENT_DYNAMIC_CLASS(wxSplitterWindow, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxSplitterEvent, wxNotifyEvent)

BEGIN_EVENT_TABLE(wxSplitterWindow, wxWindow)
    EVT_PAINT(wxSplitterWindow::OnPaint)
    EVT_SIZE(wxSplitterWindow::OnSize)
    EVT_IDLE(wxSplitterWindow::OnIdle)
    EVT_MOUSE_EVENTS(wxSplitterWindow::OnMouseEvent)

#if defined( __WXMSW__ ) || defined( __WXMAC__)
    EVT_SET_CURSOR(wxSplitterWindow::OnSetCursor)
#endif // wxMSW

    WX_EVENT_TABLE_CONTROL_CONTAINER(wxSplitterWindow)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxSplitterWindow);

bool wxSplitterWindow::Create(wxWindow *parent, wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
{
    // allow TABbing from one window to the other
    style |= wxTAB_TRAVERSAL;

    if (!wxWindow::Create(parent, id, pos, size, style, name))
        return FALSE;

    m_permitUnsplitAlways = (style & wxSP_PERMIT_UNSPLIT) != 0;

    if ( style & wxSP_3DSASH )
        m_sashSize = 7;
    else
        m_sashSize = 3;

    if ( style & wxSP_3DBORDER )
        m_borderSize = 2;
    else if ( style & wxSP_BORDER )
        m_borderSize = 1;
    else
        m_borderSize = 0;

#ifdef __WXMAC__
    int major,minor;
    wxGetOsVersion( &major, &minor );
    if (major >= 10)
        m_windowStyle |= wxSP_SASH_AQUA;
#endif

    return TRUE;
}

void wxSplitterWindow::Init()
{
    m_container.SetContainerWindow(this);

    m_splitMode = wxSPLIT_VERTICAL;
    m_permitUnsplitAlways = TRUE;
    m_windowOne = (wxWindow *) NULL;
    m_windowTwo = (wxWindow *) NULL;
    m_dragMode = wxSPLIT_DRAG_NONE;
    m_oldX = 0;
    m_oldY = 0;
    m_firstX = 0;
    m_firstY = 0;
    m_sashSize = 3;
    m_borderSize = 0;
    m_sashPosition = m_requestedSashPosition = 0;
    m_minimumPaneSize = 0;
    m_sashCursorWE = wxCursor(wxCURSOR_SIZEWE);
    m_sashCursorNS = wxCursor(wxCURSOR_SIZENS);
    m_sashTrackerPen = new wxPen(*wxBLACK, 2, wxSOLID);
    m_lightShadowPen = (wxPen *) NULL;
    m_mediumShadowPen = (wxPen *) NULL;
    m_darkShadowPen = (wxPen *) NULL;
    m_faceBrush = (wxBrush *) NULL;
    m_facePen = (wxPen *) NULL;
    m_hilightPen = (wxPen *) NULL;

    InitColours();

    m_needUpdating = FALSE;
}

wxSplitterWindow::~wxSplitterWindow()
{
    delete m_sashTrackerPen;
    delete m_lightShadowPen;
    delete m_darkShadowPen;
    delete m_mediumShadowPen;
    delete m_hilightPen;
    delete m_facePen;
    delete m_faceBrush;
}

void wxSplitterWindow::SetResizeCursor()
{
    SetCursor(m_splitMode == wxSPLIT_VERTICAL ? m_sashCursorWE
                                              : m_sashCursorNS);
}

void wxSplitterWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if ( m_borderSize > 0 )
        DrawBorders(dc);
    DrawSash(dc);
}

void wxSplitterWindow::OnIdle(wxIdleEvent& event)
{
    if (m_needUpdating)
        SizeWindows();

    event.Skip();
}

void wxSplitterWindow::OnMouseEvent(wxMouseEvent& event)
{
    int x = (int)event.GetX(),
        y = (int)event.GetY();

#if defined(__WXMSW__)
    // SetCursor(wxCursor());   // Is this required?
#endif

    if (GetWindowStyle() & wxSP_NOSASH)
        return;

    // with wxSP_LIVE_UPDATE style the splitter windows are always resized
    // following the mouse movement while it drags the sash, without it we only
    // draw the sash at the new position but only resize the windows when the
    // dragging is finished
    bool isLive = (GetWindowStyleFlag() & wxSP_LIVE_UPDATE) != 0;

    if (event.LeftDown())
    {
        if ( SashHitTest(x, y) )
        {
            // Start the drag now
            m_dragMode = wxSPLIT_DRAG_DRAGGING;
            
            // Capture mouse and set the cursor
            CaptureMouse();
            SetResizeCursor();

            if ( !isLive )
            {
                // remember the initial sash position and draw the initial
                // shadow sash
                m_sashPositionCurrent = m_sashPosition;

                DrawSashTracker(x, y);
            }

            m_oldX = x;
            m_oldY = y;

            SetResizeCursor();
            return;
        }
    }
    else if (event.LeftUp() && m_dragMode == wxSPLIT_DRAG_DRAGGING)
    {
        // We can stop dragging now and see what we've got.
        m_dragMode = wxSPLIT_DRAG_NONE;
        
        // Release mouse and unset the cursor
        ReleaseMouse();
        SetCursor(* wxSTANDARD_CURSOR);

        // exit if unsplit after doubleclick
        if ( !IsSplit() )
        {
            return;
        }

        // Erase old tracker
        if ( !isLive )
        {
            DrawSashTracker(m_oldX, m_oldY);
        }

        // the position of the click doesn't exactly correspond to
        // m_sashPosition, rather it changes it by the distance by which the
        // mouse has moved
        int diff = m_splitMode == wxSPLIT_VERTICAL ? x - m_oldX : y - m_oldY;

        int posSashOld = isLive ? m_sashPosition : m_sashPositionCurrent;
        int posSashNew = OnSashPositionChanging(posSashOld + diff);
        if ( posSashNew == -1 )
        {
            // change not allowed
            return;
        }

        if ( m_permitUnsplitAlways || m_minimumPaneSize == 0 )
        {
            // Deal with possible unsplit scenarios
            if ( posSashNew == 0 )
            {
                // We remove the first window from the view
                wxWindow *removedWindow = m_windowOne;
                m_windowOne = m_windowTwo;
                m_windowTwo = (wxWindow *) NULL;
                OnUnsplit(removedWindow);
                SetSashPositionAndNotify(0);
            }
            else if ( posSashNew == GetWindowSize() )
            {
                // We remove the second window from the view
                wxWindow *removedWindow = m_windowTwo;
                m_windowTwo = (wxWindow *) NULL;
                OnUnsplit(removedWindow);
                SetSashPositionAndNotify(0);
            }
            else
            {
                SetSashPositionAndNotify(posSashNew);
            }
        }
        else
        {
            SetSashPositionAndNotify(posSashNew);
        }

        SizeWindows();
    }  // left up && dragging
    else if (event.Moving() && !event.Dragging())
    {
        // Just change the cursor as required
        if ( SashHitTest(x, y) )
            SetResizeCursor();
        else
            SetCursor(* wxSTANDARD_CURSOR);
    }
    else if (event.Dragging() && (m_dragMode == wxSPLIT_DRAG_DRAGGING))
    {
        int diff = m_splitMode == wxSPLIT_VERTICAL ? x - m_oldX : y - m_oldY;
        if ( !diff )
        {
            // nothing to do, mouse didn't really move far enough
            return;
        }

        int posSashOld = isLive ? m_sashPosition : m_sashPositionCurrent;
        int posSashNew = OnSashPositionChanging(posSashOld + diff);
        if ( posSashNew == -1 )
        {
            // change not allowed
            return;
        }

        if ( posSashNew == m_sashPosition )
            return;

        // Erase old tracker
        if ( !isLive )
        {
            DrawSashTracker(m_oldX, m_oldY);
        }

        if (m_splitMode == wxSPLIT_VERTICAL)
            x = posSashNew;
        else
            y = posSashNew;

        // Remember old positions
        m_oldX = x;
        m_oldY = y;

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
        if ( !isLive )
        {
            m_sashPositionCurrent = posSashNew;

            DrawSashTracker(m_oldX, m_oldY);
        }
        else
        {
            SetSashPositionAndNotify(posSashNew);
            m_needUpdating = TRUE;
        }
    }
    else if ( event.LeftDClick() && m_windowTwo )
    {
        OnDoubleClickSash(x, y);
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

    // wxMotif doesn't yet have a wxTopLevelWindow implementation
#ifdef __WXMOTIF__
    wxFrame *winTop = wxDynamicCast(parent, wxFrame);
#else
    wxTopLevelWindow *winTop = wxDynamicCast(parent, wxTopLevelWindow);
#endif
    if ( winTop )
    {
        iconized = winTop->IsIconized();
    }
#ifndef __WXMOTIF__
    else
    {
        wxFAIL_MSG(wxT("should have a top level parent!"));

        iconized = FALSE;
    }
#endif

    if ( iconized )
    {
        event.Skip();

        return;
    }

    int cw, ch;
    GetClientSize( &cw, &ch );
    if ( m_windowTwo )
    {
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            if ( m_sashPosition >= (cw - 5) )
                SetSashPositionAndNotify(wxMax(10, cw - 40));
        }
        else // m_splitMode == wxSPLIT_HORIZONTAL
        {
            if ( m_sashPosition >= (ch - 5) )
                SetSashPositionAndNotify(wxMax(10, ch - 40));
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
}

// Draw 3D effect borders
void wxSplitterWindow::DrawBorders(wxDC& dc)
{
    int w, h;
    GetClientSize(&w, &h);

    if ( GetWindowStyleFlag() & wxSP_3DBORDER )
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
    if (GetWindowStyle() & wxSP_NOSASH)
        return;

    int w, h;
    GetClientSize(&w, &h);

    if ( GetWindowStyleFlag() & wxSP_3DSASH )
    {
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            dc.SetPen(*m_facePen);

            if (HasFlag( wxSP_SASH_AQUA ))
                dc.SetBrush(*wxWHITE_BRUSH);
            else
                dc.SetBrush(*m_faceBrush);
            dc.DrawRectangle(m_sashPosition + 2, 0 , m_sashSize - 4, h );

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
            int xShadow = m_borderSize ? m_borderSize - 1 : 0 ;
            dc.DrawLine(m_sashPosition, xShadow , m_sashPosition, h-m_borderSize);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(m_sashPosition+1, m_borderSize - 2, m_sashPosition+1, h - m_borderSize+2);

            if (!HasFlag( wxSP_SASH_AQUA ))
                dc.SetPen(*m_mediumShadowPen);

            int yMedium = m_borderSize ? h-m_borderSize+1 : h ;
            dc.DrawLine(m_sashPosition+m_sashSize-2, xShadow, m_sashPosition+m_sashSize-2, yMedium);

            if (HasFlag( wxSP_SASH_AQUA ))
                dc.SetPen(*m_lightShadowPen);
            else
                dc.SetPen(*m_darkShadowPen);
            dc.DrawLine(m_sashPosition+m_sashSize-1, m_borderSize, m_sashPosition+m_sashSize-1, h-m_borderSize );

            // Draw the top and bottom edges of the sash, if requested
            if (GetWindowStyle() & wxSP_FULLSASH)
            {
                // Top
                dc.SetPen(*m_hilightPen);
                dc.DrawLine(m_sashPosition+1, m_borderSize, m_sashPosition+m_sashSize-1, m_borderSize);

                // Bottom
                dc.SetPen(*m_darkShadowPen);
                dc.DrawLine(m_sashPosition+1, h-m_borderSize-1, m_sashPosition+m_sashSize-1, h-m_borderSize-1);
            }
        }
        else // wxSPLIT_HORIZONTAL
        {
            dc.SetPen(*m_facePen);
            if (HasFlag( wxSP_SASH_AQUA ))
                dc.SetBrush(*wxWHITE_BRUSH);
            else
                dc.SetBrush(*m_faceBrush);
            dc.DrawRectangle( m_borderSize-2, m_sashPosition + 2, w-m_borderSize+2, m_sashSize - 4);

            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            dc.SetPen(*m_lightShadowPen);
            dc.DrawLine(m_borderSize-1, m_sashPosition, w-m_borderSize, m_sashPosition);

            dc.SetPen(*m_hilightPen);
            dc.DrawLine(m_borderSize-2, m_sashPosition+1, w-m_borderSize+1, m_sashPosition+1);

            if (!HasFlag( wxSP_SASH_AQUA ))
                dc.SetPen(*m_mediumShadowPen);
            dc.DrawLine(m_borderSize-1, m_sashPosition+m_sashSize-2, w-m_borderSize+1, m_sashPosition+m_sashSize-2);

            if (HasFlag( wxSP_SASH_AQUA ))
                dc.SetPen(*m_lightShadowPen);
            else
                dc.SetPen(*m_darkShadowPen);
            dc.DrawLine(m_borderSize, m_sashPosition+m_sashSize-1, w-m_borderSize, m_sashPosition+m_sashSize-1);

            // Draw the left and right edges of the sash, if requested
            if (GetWindowStyle() & wxSP_FULLSASH)
            {
                // Left
                dc.SetPen(*m_hilightPen);
                dc.DrawLine(m_borderSize, m_sashPosition, m_borderSize, m_sashPosition+m_sashSize);

                // Right
                dc.SetPen(*m_darkShadowPen);
                dc.DrawLine(w-m_borderSize-1, m_sashPosition+1, w-m_borderSize-1, m_sashPosition+m_sashSize-1);
            }
        }
    }
    else // !wxSP_3DSASH
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*m_faceBrush);
        if ( m_splitMode == wxSPLIT_VERTICAL )
        {
            int h1 = h-1;
            int y1 = 0;
            if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER && (GetWindowStyleFlag() & wxSP_3DBORDER) != wxSP_3DBORDER )
                h1 += 1; // Not sure why this is necessary...
            if ( (GetWindowStyleFlag() & wxSP_3DBORDER) == wxSP_3DBORDER)
            {
                y1 = 2; h1 -= 3;
            }
            dc.DrawRectangle(m_sashPosition, y1, m_sashSize, h1);
        }
        else // wxSPLIT_HORIZONTAL
        {
            int w1 = w-1;
            int x1 = 0;
            if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER && (GetWindowStyleFlag() & wxSP_3DBORDER) != wxSP_3DBORDER )
                w1 ++;
            if ( (GetWindowStyleFlag() & wxSP_3DBORDER) == wxSP_3DBORDER)
            {
                x1 = 2; w1 -= 3;
            }
            dc.DrawRectangle(x1, m_sashPosition, w1, m_sashSize);
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

int wxSplitterWindow::GetWindowSize() const
{
    wxSize size = GetClientSize();

    return m_splitMode == wxSPLIT_VERTICAL ? size.x : size.y;
}

int wxSplitterWindow::AdjustSashPosition(int sashPos) const
{
    int window_size = GetWindowSize();

    wxWindow *win;

    win = GetWindow1();
    if ( win )
    {
        // the window shouldn't be smaller than its own minimal size nor
        // smaller than the minimual pane size specified for this splitter
        int minSize = m_splitMode == wxSPLIT_VERTICAL ? win->GetMinWidth()
                                                      : win->GetMinHeight();

        if ( minSize == -1 || m_minimumPaneSize > minSize )
            minSize = m_minimumPaneSize;

        minSize += GetBorderSize();

        if ( sashPos < minSize )
            sashPos = minSize;
    }

    win = GetWindow2();
    if ( win )
    {
        int minSize = m_splitMode == wxSPLIT_VERTICAL ? win->GetMinWidth()
                                                      : win->GetMinHeight();

        if ( minSize == -1 || m_minimumPaneSize > minSize )
            minSize = m_minimumPaneSize;

        int maxSize = window_size - minSize - GetBorderSize();
        if ( sashPos > maxSize )
            sashPos = maxSize;
    }

    return sashPos;
}

bool wxSplitterWindow::DoSetSashPosition(int sashPos)
{
    int newSashPosition = AdjustSashPosition(sashPos);

    if ( newSashPosition == m_sashPosition )
        return FALSE;

    m_sashPosition = newSashPosition;

    return TRUE;
}

void wxSplitterWindow::SetSashPositionAndNotify(int sashPos)
{
    if ( DoSetSashPosition(sashPos) )
    {
        wxSplitterEvent event(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, this);
        event.m_data.pos = m_sashPosition;

        (void)DoSendEvent(event);
    }
}

// Position and size subwindows.
// Note that the border size applies to each subwindow, not
// including the edges next to the sash.
void wxSplitterWindow::SizeWindows()
{
    // check if we have delayed setting the real sash position
    if ( m_requestedSashPosition != INT_MAX )
    {
        int newSashPosition = ConvertSashPosition(m_requestedSashPosition);
        if ( newSashPosition != m_sashPosition )
        {
            DoSetSashPosition(newSashPosition);
        }

        if ( newSashPosition <= m_sashPosition
            && newSashPosition >= m_sashPosition - GetBorderSize() )
        {
            // don't update it any more
            m_requestedSashPosition = INT_MAX;
        }
    }

    int w, h;
    GetClientSize(&w, &h);

    if ( GetWindow1() && !GetWindow2() )
    {
        GetWindow1()->SetSize(GetBorderSize(), GetBorderSize(),
                              w - 2*GetBorderSize(), h - 2*GetBorderSize());
    }
    else if ( GetWindow1() && GetWindow2() )
    {
        if (GetSplitMode() == wxSPLIT_VERTICAL)
        {
            int x1 = GetBorderSize();
            int y1 = GetBorderSize();
            int w1 = GetSashPosition() - GetBorderSize();
            int h1 = h - 2*GetBorderSize();

            int x2 = GetSashPosition() + GetSashSize();
            int y2 = GetBorderSize();
            int w2 = w - 2*GetBorderSize() - GetSashSize() - w1;
            int h2 = h - 2*GetBorderSize();

            GetWindow1()->SetSize(x1, y1, w1, h1);
            GetWindow2()->SetSize(x2, y2, w2, h2);
        }
        else
        {
            GetWindow1()->SetSize(GetBorderSize(), GetBorderSize(),
                w - 2*GetBorderSize(), GetSashPosition() - GetBorderSize());
            GetWindow2()->SetSize(GetBorderSize(), GetSashPosition() + GetSashSize(),
                w - 2*GetBorderSize(), h - 2*GetBorderSize() - GetSashSize() - (GetSashPosition() - GetBorderSize()));
        }
    }
    wxClientDC dc(this);
    if ( GetBorderSize() > 0 )
        DrawBorders(dc);
    DrawSash(dc);

    SetNeedUpdating(FALSE);
}

// Set pane for unsplit window
void wxSplitterWindow::Initialize(wxWindow *window)
{
    wxASSERT_MSG( window && window->GetParent() == this,
                  _T("windows in the splitter should have it as parent!") );

    m_windowOne = window;
    m_windowTwo = (wxWindow *) NULL;
    DoSetSashPosition(0);
}

// Associates the given window with window 2, drawing the appropriate sash
// and changing the split mode.
// Does nothing and returns FALSE if the window is already split.
bool wxSplitterWindow::DoSplit(wxSplitMode mode,
                               wxWindow *window1, wxWindow *window2,
                               int sashPosition)
{
    if ( IsSplit() )
        return FALSE;

    wxCHECK_MSG( window1 && window2, FALSE,
                 _T("can not split with NULL window(s)") );

    wxCHECK_MSG( window1->GetParent() == this && window2->GetParent() == this, FALSE,
                  _T("windows in the splitter should have it as parent!") );

    m_splitMode = mode;
    m_windowOne = window1;
    m_windowTwo = window2;

    // remember the sash position we want to set for later if we can't set it
    // right now (e.g. because the window is too small)
    m_requestedSashPosition = sashPosition;

    DoSetSashPosition(ConvertSashPosition(sashPosition));

    SizeWindows();

    return TRUE;
}

int wxSplitterWindow::ConvertSashPosition(int sashPosition) const
{
    if ( sashPosition > 0 )
    {
        return sashPosition;
    }
    else if ( sashPosition < 0 )
    {
        // It's negative so adding is subtracting
        return GetWindowSize() + sashPosition;
    }
    else // sashPosition == 0
    {
        // default, put it in the centre
        return GetWindowSize() / 2;
    }
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

    win->Show(FALSE);
    DoSetSashPosition(0);
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

void wxSplitterWindow::SetMinimumPaneSize(int min)
{
    m_minimumPaneSize = min;
    SetSashPosition(m_sashPosition); // re-check limits
}

void wxSplitterWindow::SetSashPosition(int position, bool redraw)
{
    DoSetSashPosition(position);

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
#ifndef __WIN16__
    wxColour faceColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_facePen = new wxPen(faceColour, 1, wxSOLID);
    m_faceBrush = new wxBrush(faceColour, wxSOLID);

    wxColour mediumShadowColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
    m_mediumShadowPen = new wxPen(mediumShadowColour, 1, wxSOLID);

    wxColour darkShadowColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
    m_darkShadowPen = new wxPen(darkShadowColour, 1, wxSOLID);

    wxColour lightShadowColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
    m_lightShadowPen = new wxPen(lightShadowColour, 1, wxSOLID);

    wxColour hilightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT));
    m_hilightPen = new wxPen(hilightColour, 1, wxSOLID);
#else
    m_facePen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_faceBrush = new wxBrush("LIGHT GREY", wxSOLID);
    m_mediumShadowPen = new wxPen("GREY", 1, wxSOLID);
    m_darkShadowPen = new wxPen("BLACK", 1, wxSOLID);
    m_lightShadowPen = new wxPen("LIGHT GREY", 1, wxSOLID);
    m_hilightPen = new wxPen("WHITE", 1, wxSOLID);
#endif // __WIN16__
}

bool wxSplitterWindow::DoSendEvent(wxSplitterEvent& event)
{
    return !GetEventHandler()->ProcessEvent(event) || event.IsAllowed();
}

// ---------------------------------------------------------------------------
// wxSplitterWindow virtual functions: they now just generate the events
// ---------------------------------------------------------------------------

bool wxSplitterWindow::OnSashPositionChange(int WXUNUSED(newSashPosition))
{
    // always allow by default
    return TRUE;
}

int wxSplitterWindow::OnSashPositionChanging(int newSashPosition)
{
    // If within UNSPLIT_THRESHOLD from edge, set to edge to cause closure.
    const int UNSPLIT_THRESHOLD = 4;

    // first of all, check if OnSashPositionChange() doesn't forbid this change
    if ( !OnSashPositionChange(newSashPosition) )
    {
        // it does
        return -1;
    }

    // Obtain relevant window dimension for bottom / right threshold check
    int window_size = GetWindowSize();

    bool unsplit_scenario = FALSE;
    if ( m_permitUnsplitAlways || m_minimumPaneSize == 0 )
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
        newSashPosition = AdjustSashPosition(newSashPosition);
    }

    // If the result is out of bounds it means minimum size is too big,
    // so split window in half as best compromise.
    if ( newSashPosition < 0 || newSashPosition > window_size )
        newSashPosition = window_size / 2;

    // now let the event handler have it
    //
    // FIXME: shouldn't we do it before the adjustments above so as to ensure
    //        that the sash position is always reasonable?
    wxSplitterEvent event(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, this);
    event.m_data.pos = newSashPosition;

    if ( !DoSendEvent(event) )
    {
        // the event handler vetoed the change
        newSashPosition = -1;
    }
    else
    {
        // it could have been changed by it
        newSashPosition = event.GetSashPosition();
    }

    return newSashPosition;
}

// Called when the sash is double-clicked. The default behaviour is to remove
// the sash if the minimum pane size is zero.
void wxSplitterWindow::OnDoubleClickSash(int x, int y)
{
    wxCHECK_RET(m_windowTwo, wxT("splitter: no window to remove"));

    // new code should handle events instead of using the virtual functions
    wxSplitterEvent event(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, this);
    event.m_data.pt.x = x;
    event.m_data.pt.y = y;
    if ( DoSendEvent(event) )
    {
        if ( GetMinimumPaneSize() == 0 || m_permitUnsplitAlways )
        {
            wxWindow* win = m_windowTwo;
            if (Unsplit(win))
                OnUnsplit(win);
        }
    }
    //else: blocked by user
}

void wxSplitterWindow::OnUnsplit(wxWindow *winRemoved)
{
    // do it before calling the event handler which may delete the window
    winRemoved->Show(FALSE);

    wxSplitterEvent event(wxEVT_COMMAND_SPLITTER_UNSPLIT, this);
    event.m_data.win = winRemoved;

    (void)DoSendEvent(event);
}

#if defined( __WXMSW__ ) || defined( __WXMAC__)

// this is currently called (and needed) under MSW only...
void wxSplitterWindow::OnSetCursor(wxSetCursorEvent& event)
{
    // if we don't do it, the resizing cursor might be set for child window:
    // and like this we explicitly say that our cursor should not be used for
    // children windows which overlap us

    if ( SashHitTest(event.GetX(), event.GetY()) )
    {
        // default processing is ok
        event.Skip();
    }
    //else: do nothing, in particular, don't call Skip()
}

#endif // wxMSW

