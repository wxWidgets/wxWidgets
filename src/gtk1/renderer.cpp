///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/renderer.cpp
// Purpose:     implementation of wxRendererNative for wxGTK
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
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

#include "wx/renderer.h"
#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

#include "wx/window.h"
#include "wx/dc.h"

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

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
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

    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;
    if (s_button == NULL)
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_button_new();
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    gtk_paint_box
    (
        s_button->style,
        GTK_PIZZA(win->m_wxwindow)->bin_window,
        flags & wxCONTROL_DISABLED ? GTK_STATE_INSENSITIVE : GTK_STATE_NORMAL,
        GTK_SHADOW_OUT,
        NULL,
        s_button,
        "button",
        dc.XLOG2DEV(rect.x) -1, rect.y -1, rect.width +2, rect.height +2
    );
}

#ifdef __WXGTK20__

// draw a ">" or "v" button
//
// TODO: replace the code below with gtk_paint_expander()
void
wxRendererGTK::DrawTreeItemButton(wxWindow* win,
                                  wxDC& dc, const wxRect& rect, int flags)
{
#define PM_SIZE 8

    GtkPizza *pizza = GTK_PIZZA( win->m_wxwindow );
    GtkStyle *style = win->m_widget->style;
    int x = rect.x;
    int y = rect.y;
    y = dc.LogicalToDeviceY( y );
    x = dc.LogicalToDeviceX( x );

    // This draws the GTK+ 2.2.4 triangle
    x--;
    GdkPoint points[3];
    
    if ( flags & wxCONTROL_EXPANDED )
    {
        points[0].x = x;
        points[0].y = y + (PM_SIZE + 2) / 6;
        points[1].x = points[0].x + (PM_SIZE + 2);
        points[1].y = points[0].y;
        points[2].x = (points[0].x + (PM_SIZE + 2) / 2);
        points[2].y = y + 2 * (PM_SIZE + 2) / 3;
    }
    else
    {  
        points[0].x = x + ((PM_SIZE + 2) / 6 + 2);
        points[0].y = y - 1;
        points[1].x = points[0].x;
        points[1].y = points[0].y + (PM_SIZE + 2);
        points[2].x = (points[0].x +
             (2 * (PM_SIZE + 2) / 3 - 1));
        points[2].y = points[0].y + (PM_SIZE + 2) / 2;
    }

    if ( flags & wxCONTROL_CURRENT )
        gdk_draw_polygon( pizza->bin_window, style->fg_gc[GTK_STATE_PRELIGHT], TRUE, points, 3);
    else
        gdk_draw_polygon( pizza->bin_window, style->base_gc[GTK_STATE_NORMAL], TRUE, points, 3);
    gdk_draw_polygon( pizza->bin_window, style->fg_gc[GTK_STATE_NORMAL], FALSE, points, 3 );
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
    static const wxCoord SASH_SIZE = 8;

    // margin around the sash
    static const wxCoord SASH_MARGIN = 2;

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
        // window not realized yet
        return;
    }

    // are we drawing vertical or horizontal splitter?
    const bool isVert = orient == wxVERTICAL;

    GdkRectangle rect;
    GdkRectangle erase_rect;
    if ( isVert )
    {
        int h = win->GetClientSize().GetHeight();
    
        rect.x = position;
        rect.y = 0;
        rect.width = SASH_FULL_SIZE;
        rect.height = h;
    
        erase_rect.x = position;
        erase_rect.y = 0;
        erase_rect.width = SASH_FULL_SIZE;
        erase_rect.height = h;
    }
    else // horz
    {
        int w = win->GetClientSize().GetWidth();
    
        rect.x = 0;
        rect.y = position;
        rect.height = SASH_FULL_SIZE;
        rect.width = w;
    
        erase_rect.y = position;
        erase_rect.x = 0;
        erase_rect.height = SASH_FULL_SIZE;
        erase_rect.width = w;
    }

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
        NULL,
        win->m_wxwindow,
        (char *)"base", // const_cast
        erase_rect.x,
        erase_rect.y,
        erase_rect.width,
        erase_rect.height
    );

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
        !isVert ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
    );
#else // GTK+ 1.x

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

void wxRendererGTK::DrawComboBoxDropButton(wxWindow *win,
                                            wxDC& dc,
                                            const wxRect& rect,
                                            int flags)
{
    dc.SetBrush(wxBrush(win->GetBackgroundColour()));
    dc.SetPen(wxPen(win->GetBackgroundColour()));
    dc.DrawRectangle(rect);

    int x = (rect.GetWidth()-9)   / 2;
    int y = (rect.GetHeight()-10) / 2;

    wxPoint pt[] =
    {
        wxPoint(x+2, y+3),
        wxPoint(x+6, y+3),
        wxPoint(x+6, y+6),
        wxPoint(x+8, y+6),
        wxPoint(x+4, y+10),
        wxPoint(x+0, y+6),
        wxPoint(x+2, y+6)
    };
    dc.SetBrush(wxBrush(win->GetForegroundColour()));
    dc.SetPen(wxPen(win->GetForegroundColour()));
    dc.DrawLine(x, y, x+9, y);
    dc.DrawPolygon(WXSIZEOF(pt), pt, rect.x, rect.y);
}

