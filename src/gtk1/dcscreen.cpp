/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dcscreen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/window.h"

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// global data initialization
//-----------------------------------------------------------------------------

GdkWindow *wxScreenDC::sm_overlayWindow  = (GdkWindow*) NULL;
int wxScreenDC::sm_overlayWindowX = 0;
int wxScreenDC::sm_overlayWindowY = 0;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC()
{
    m_ok = FALSE;
    m_cmap = gdk_colormap_get_system();
    m_window = GDK_ROOT_PARENT();

#ifdef __WXGTK20__
    m_context = gdk_pango_context_get();
    m_layout = pango_layout_new( m_context );
//    m_fontdesc = pango_font_description_copy( widget->style->font_desc );
#endif

    m_isScreenDC = TRUE;

    SetUpDC();

    gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
    gdk_gc_set_subwindow( m_brushGC, GDK_INCLUDE_INFERIORS );
    gdk_gc_set_subwindow( m_textGC, GDK_INCLUDE_INFERIORS );
    gdk_gc_set_subwindow( m_bgGC, GDK_INCLUDE_INFERIORS );
}

wxScreenDC::~wxScreenDC()
{
    gdk_gc_set_subwindow( m_penGC, GDK_CLIP_BY_CHILDREN );
    gdk_gc_set_subwindow( m_brushGC, GDK_CLIP_BY_CHILDREN );
    gdk_gc_set_subwindow( m_textGC, GDK_CLIP_BY_CHILDREN );
    gdk_gc_set_subwindow( m_bgGC, GDK_CLIP_BY_CHILDREN );

    EndDrawingOnTop();
}

bool wxScreenDC::StartDrawingOnTop( wxWindow *window )
{
    if (!window) return StartDrawingOnTop();

    int x = 0;
    int y = 0;
    window->GetPosition( &x, &y );
    int w = 0;
    int h = 0;
    window->GetSize( &w, &h );
    window->ClientToScreen( &x, &y );

    wxRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = 0;
    rect.height = 0;

    return StartDrawingOnTop( &rect );
}

bool wxScreenDC::StartDrawingOnTop( wxRect *rect )
{
    int x = 0;
    int y = 0;
    int width = gdk_screen_width();
    int height = gdk_screen_height();
    if (rect)
    {
        x = rect->x;
        y = rect->y;
        width = rect->width;
        height = rect->height;
    }

    return TRUE;
}

bool wxScreenDC::EndDrawingOnTop()
{
    return TRUE;
}

void wxScreenDC::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}
