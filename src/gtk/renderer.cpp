///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/renderer.cpp
// Purpose:     implementation of wxRendererNative for wxGTK
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

#include "wx/window.h"
#include "wx/dc.h"
#include "wx/renderer.h"

#ifdef __WXGTK20__
    #include "wx/settings.h"
#endif // GTK 2.0

#ifdef __WXGTK20__
    #define WXUNUSED_IN_GTK1(arg) arg
#else
    #define WXUNUSED_IN_GTK1(arg)
#endif

// ----------------------------------------------------------------------------
// wxRendererGTK: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGTK : public wxDelegateRendererNative
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

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0);

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);
};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
    static wxRendererGTK s_rendererGTK;

    return s_rendererGTK;
}

// ----------------------------------------------------------------------------
// list/tree controls drawing
// ----------------------------------------------------------------------------

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
        (GdkRectangle*) NULL,
        win->m_wxwindow,
        (char *)"button", // const_cast
        dc.XLOG2DEV(rect.x) - 1, rect.y - 1, rect.width + 2, rect.height + 2
    );
}

#ifdef __WXGTK20__

// draw a ">" or "v" button
//
// TODO: isn't there a GTK function to draw it?
void
wxRendererGTK::DrawTreeItemButton(wxWindow* WXUNUSED(win),
                                  wxDC& dc, const wxRect& rect, int flags)
{
    dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
                        wxSOLID));
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

// ----------------------------------------------------------------------------
// splitter sash drawing
// ----------------------------------------------------------------------------

// all this should probably be read from the current theme settings somehow?
#ifdef __WXGTK20__
    // the full sash size
    static const wxCoord SASH_FULL_SIZE = 5;
#else // GTK+ 1.x
    // the full sash width (should be even)
    static const wxCoord SASH_SIZE = 10;

    // margin around the sash
    static const wxCoord SASH_MARGIN = 5;

    // the full sash size
    static const wxCoord SASH_FULL_SIZE = SASH_SIZE + SASH_MARGIN;
#endif // GTK+ 2.x/1.x

wxSplitterRenderParams
wxRendererGTK::GetSplitterParams(const wxWindow * WXUNUSED(win))
{
    // we don't draw any border, hence 0 for the second field
    return wxSplitterRenderParams
           (
               SASH_FULL_SIZE,
               0,
#ifdef __WXGTK20__
               true     // hot sensitive
#else // GTK+ 1.x
               false    // not
#endif // GTK+ 2.x/1.x
           );
}

void
wxRendererGTK::DrawSplitterBorder(wxWindow * WXUNUSED(win),
                                  wxDC& WXUNUSED(dc),
                                  const wxRect& WXUNUSED(rect),
                                  int WXUNUSED(flags))
{
    // nothing to do
}

void
wxRendererGTK::DrawSplitterSash(wxWindow *win,
                                wxDC& dc,
                                const wxSize& size,
                                wxCoord position,
                                wxOrientation orient,
                                int WXUNUSED_IN_GTK1(flags))
{
    if ( !win->m_wxwindow->window )
    {
        // VZ: this happens on startup -- why?
        return;
    }

    // are we drawing vertical or horizontal splitter?
    const bool isVert = orient == wxVERTICAL;

    GdkRectangle rect;
    if ( isVert )
    {
        rect.x = position;
        rect.y = 0;
        rect.width = SASH_FULL_SIZE;
        rect.height = size.y;
    }
    else // horz
    {
        rect.x = 0;
        rect.y = position;
        rect.height = SASH_FULL_SIZE;
        rect.width = size.x;
    }

#ifdef __WXGTK20__
    gtk_paint_handle
    (
        win->m_wxwindow->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        flags & wxCONTROL_CURRENT ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
        GTK_SHADOW_NONE,
        NULL /* no clipping */,
        win->m_wxwindow,
        "paned",
        rect.x,
        rect.y,
        rect.width,
        rect.height,
        isVert ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
    );
#else // GTK+ 1.x
    // we must erase everything first, otherwise the garbage from the old sash
    // is left when dragging it
    //
    // TODO: is this the right way to draw themed background?
    gtk_paint_flat_box
    (
        win->m_wxwindow->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        GTK_STATE_NORMAL,
        GTK_SHADOW_NONE,
        &rect,
        win->m_wxwindow,
        (char *)"base", // const_cast
        0, 0, -1, -1
    );


    // leave some margin before sash itself
    position += SASH_MARGIN / 2;

    // and finally draw it using GTK paint functions
    typedef void (*GtkPaintLineFunc)(GtkStyle *, GdkWindow *,
                                                GtkStateType,
                                                GdkRectangle *, GtkWidget *,
                                                gchar *,
                                                gint, gint, gint);

    GtkPaintLineFunc func = isVert ? gtk_paint_vline : gtk_paint_hline;

    (*func)
    (
        win->m_wxwindow->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        GTK_STATE_NORMAL,
        NULL,
        win->m_wxwindow,
        (char *)"paned", // const_cast
        0, isVert ? size.y : size.x, position + SASH_SIZE / 2 - 1
    );

    gtk_paint_box
    (
        win->m_wxwindow->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        GTK_STATE_NORMAL,
        GTK_SHADOW_OUT,
        (GdkRectangle*) NULL,
        win->m_wxwindow,
        (char *)"paned", // const_cast
        isVert ? position : size.x - 2*SASH_SIZE,
        isVert ? size.y - 2*SASH_SIZE : position,
        SASH_SIZE, SASH_SIZE
    );
#endif // GTK+ 2.x/1.x
}

