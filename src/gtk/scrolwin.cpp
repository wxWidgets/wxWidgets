/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/scrolwin.cpp
// Purpose:     wxScrolledWindow implementation
// Author:      Robert Roebling
// Modified by: Ron Lee
//              Vadim Zeitlin: removed 90% of duplicated common code
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/scrolwin.h"
#include "wx/gtk/private.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxScrollHelper implementation
// ----------------------------------------------------------------------------

void wxScrollHelperNative::SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                                         int noUnitsX, int noUnitsY,
                                         int xPos, int yPos,
                                         bool noRefresh)
{
    int xs, ys;
    GetViewStart(& xs, & ys);

    int old_x = m_xScrollPixelsPerLine * xs;
    int old_y = m_yScrollPixelsPerLine * ys;

    m_xScrollPixelsPerLine = pixelsPerUnitX;
    m_yScrollPixelsPerLine = pixelsPerUnitY;

    m_win->m_scrollBar[0]->adjustment->value =
    m_xScrollPosition = xPos;
    m_win->m_scrollBar[1]->adjustment->value =
    m_yScrollPosition = yPos;

    // Setting hints here should arguably be deprecated, but without it
    // a sizer might override this manual scrollbar setting in old code.
    // m_targetWindow->SetVirtualSizeHints( noUnitsX * pixelsPerUnitX, noUnitsY * pixelsPerUnitY );

    int w = noUnitsX * pixelsPerUnitX;
    int h = noUnitsY * pixelsPerUnitY;
    m_targetWindow->SetVirtualSize( w ? w : wxDefaultCoord,
                                    h ? h : wxDefaultCoord);

    // If the target is not the same as the window with the scrollbars,
    // then we need to update the scrollbars here, since they won't have
    // been updated by SetVirtualSize().
    if (m_targetWindow != m_win)
    {
        AdjustScrollbars();
    }

    if (!noRefresh)
    {
        int new_x = m_xScrollPixelsPerLine * m_xScrollPosition;
        int new_y = m_yScrollPixelsPerLine * m_yScrollPosition;

        m_targetWindow->ScrollWindow( old_x - new_x, old_y - new_y );
    }
}

void wxScrollHelperNative::DoAdjustScrollbar(GtkRange* range,
                                             int pixelsPerLine,
                                             int winSize,
                                             int virtSize,
                                             int *lines,
                                             int *linesPerPage)
{
    // GtkRange won't allow upper == lower, so for disabled state use [0,1]
    //   with a page size of 1. This will also clamp position to 0.
    int upper = 1;
    int page_size = 1;
    if (pixelsPerLine > 0 && winSize > 0 && winSize < virtSize)
    {
        upper = (virtSize + pixelsPerLine - 1) / pixelsPerLine;
        page_size = winSize / pixelsPerLine;
    }

    *lines = upper;
    *linesPerPage = page_size;

    GtkAdjustment* adj = range->adjustment;
    adj->step_increment = 1;
    adj->page_increment =
    adj->page_size = page_size;
    gtk_range_set_range(range, 0, upper);
}

void wxScrollHelperNative::AdjustScrollbars()
{
    int w, h;
    int vw, vh;

    m_targetWindow->m_hasScrolling = m_xScrollPixelsPerLine != 0 || m_yScrollPixelsPerLine != 0;

    m_targetWindow->GetClientSize( &w, &h );
    m_targetWindow->GetVirtualSize( &vw, &vh );

    DoAdjustScrollbar(m_win->m_scrollBar[0], m_xScrollPixelsPerLine, w, vw,
                      &m_xScrollLines, &m_xScrollLinesPerPage);
    DoAdjustScrollbar(m_win->m_scrollBar[1], m_yScrollPixelsPerLine, h, vh,
                      &m_yScrollLines, &m_yScrollLinesPerPage);
}

void wxScrollHelperNative::DoScroll(int orient,
                                    int pos,
                                    int pixelsPerLine,
                                    int *posOld)
{
    if ( pos != -1 && pos != *posOld && pixelsPerLine )
    {
        m_win->SetScrollPos(orient, pos);
        pos = m_win->GetScrollPos(orient);

        int diff = (*posOld - pos)*pixelsPerLine;
        m_targetWindow->ScrollWindow(orient == wxHORIZONTAL ? diff : 0,
                                     orient == wxHORIZONTAL ? 0 : diff);

        *posOld = pos;
    }
}

void wxScrollHelperNative::Scroll( int x_pos, int y_pos )
{
    wxCHECK_RET( m_targetWindow != 0, _T("No target window") );

    DoScroll(wxHORIZONTAL, x_pos, m_xScrollPixelsPerLine, &m_xScrollPosition);
    DoScroll(wxVERTICAL, y_pos, m_yScrollPixelsPerLine, &m_yScrollPosition);
}
