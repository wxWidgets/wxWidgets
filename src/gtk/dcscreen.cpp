/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

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
    m_ok = false;
    m_cmap = gdk_colormap_get_system();
    m_window = gdk_get_default_root_window();

    m_context = gdk_pango_context_get();
    // Note: The Sun customised version of Pango shipping with Solaris 10
    // crashes if the language is left NULL (see bug 1374114)
    pango_context_set_language( m_context, gtk_get_default_language() );
    m_layout = pango_layout_new( m_context );
//    m_fontdesc = pango_font_description_copy( widget->style->font_desc );

    m_isScreenDC = true;

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

bool wxScreenDC::StartDrawingOnTop( wxWindow * )
{
    return true;
}

bool wxScreenDC::StartDrawingOnTop( wxRect * )
{
    return true;
}

bool wxScreenDC::EndDrawingOnTop()
{
    return true;
}

void wxScreenDC::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}
