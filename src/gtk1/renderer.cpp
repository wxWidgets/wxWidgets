///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/renderer.cpp
// Purpose:     implementation of wxRendererBase for wxGTK
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxRendererGTK: our wxRendererBase implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGTK : public wxDelegateRendererBase
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0);

#ifdef __WXGTK20__
    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);
#endif // GTK 2.0

};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererGTK::Get()
{
    static wxRendererGTK s_rendererGTK;

    return s_rendererGTK;
}

void
wxRendererGTK::DrawHeaderButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags)
{
    gtk_paint_box
    (
        win->m_wxwindow->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        flags & wxCONTROL_DISABLED ? GTK_STATE_INSENSITIVE : GTK_STATE_NORMAL,
        GTK_SHADOW_OUT,
        (GdkRectangle*) NULL, m_wxwindow,
        (char *)"button", // const_cast
        dc.XLOG2DEV(rect.x) - 1, rect.y - 1, rect.width + 2, rect.h + 2
    );
}

#ifdef __WXGTK20__

// draw a ">" or "v" button
//
// TODO: isn't there a GTK function to draw it?
void
wxRendererGTK::DrawTreeItemButton(wxDC& dc, const wxRect& rect, int flags)
{
    dc.SetBrush(*m_hilightBrush);
    dc.SetPen(*wxBLACK_PEN);
    wxPoint button[3];

    const wxCoord xMiddle = rect.x + rect.width/2;
    const wxCoord yMiddle = rect.y + rect.height/2;

    if ( flags & wxCONTROL_EXPANDED )
    {
        button[0].x = rect.GetLeft();
        button[0].y = yMiddle - 2;
        button[1].x = rect.GetRight();
        button[1].y = yMiddle - 2;
        button[2].x = xMiddle;
        button[2].y = yMiddle + 3;
    }
    else // collapsed
    {
        button[0].y = rect.GetBottom();
        button[0].x = xMiddle - 2;
        button[1].y = rect.GetTop();
        button[1].x = xMiddle - 2;
        button[2].y = yMiddle;
        button[2].x = xMiddle + 3;
    }

    dc.DrawPolygon(3, button);
}

#endif // GTK 2.0


