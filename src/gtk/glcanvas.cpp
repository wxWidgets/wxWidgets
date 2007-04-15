/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/08/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/colour.h"
    #include "wx/module.h"
#endif // WX_PRECOMP

extern "C"
{
#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"
}

#include "wx/gtk/win_gtk.h"
#include "wx/gtk/private.h"

#if WXWIN_COMPATIBILITY_2_8

//-----------------------------------------------------------------------------
// "realize" from m_wxwindow: used to create m_glContext implicitly
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_glwindow_realized_callback( GtkWidget *WXUNUSED(widget), wxGLCanvas *win )
{
    win->GTKInitImplicitContext();

    return FALSE;
}
}

#endif // WXWIN_COMPATIBILITY_2_8

//-----------------------------------------------------------------------------
// "map" from m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_glwindow_map_callback( GtkWidget * WXUNUSED(widget), wxGLCanvas *win )
{
    wxPaintEvent event( win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    win->m_exposed = false;
    win->GetUpdateRegion().Clear();

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "expose_event" of m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_glwindow_expose_callback( GtkWidget *WXUNUSED(widget), GdkEventExpose *gdk_event, wxGLCanvas *win )
{
    // don't need to install idle handler, its done from "event" signal

    win->m_exposed = true;

    win->GetUpdateRegion().Union( gdk_event->area.x,
                                  gdk_event->area.y,
                                  gdk_event->area.width,
                                  gdk_event->area.height );
    return false;
}
}

//-----------------------------------------------------------------------------
// "size_allocate" of m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_glcanvas_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxGLCanvas *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return;

    wxSizeEvent event( wxSize(win->m_width,win->m_height), win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}
}

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
#if WXWIN_COMPATIBILITY_2_8
    : m_createImplicitContext(false)
#endif
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

#if WXWIN_COMPATIBILITY_2_8

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const int *attribList,
                       const wxPalette& palette)
    : m_createImplicitContext(true)
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLContext *shared,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const int *attribList,
                       const wxPalette& palette)
    : m_createImplicitContext(true)
{
    m_sharedContext = wx_const_cast(wxGLContext *, shared);

    Create(parent, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLCanvas *shared,
                       wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style, const wxString& name,
                       const int *attribList,
                       const wxPalette& palette )
    : m_createImplicitContext(true)
{
    m_sharedContextOf = wx_const_cast(wxGLCanvas *, shared);

    Create(parent, id, pos, size, style, name, attribList, palette);
}

#endif // WXWIN_COMPATIBILITY_2_8

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
    m_exposed = false;
    m_noExpose = true;
    m_nativeSizeEvent = true;

    if ( !InitVisual(attribList) )
        return false;

    XVisualInfo * const xvi = GetXVisualInfo();

    GdkVisual *visual;
    GdkColormap *colormap;

    // MR: This needs a fix for lower gtk+ versions too. Might need to rethink logic (FIXME)
#if defined(__WXGTK20__) && GTK_CHECK_VERSION(2,2,0)
    if (!gtk_check_version(2,2,0))
    {
        wxWindow::Create( parent, id, pos, size, style, name );

        m_glWidget = m_wxwindow;

        GdkScreen *screen = gtk_widget_get_screen( m_glWidget );
        colormap = gdk_screen_get_default_colormap(screen);
        visual = gdk_colormap_get_visual(colormap);

        if (GDK_VISUAL_XVISUAL(visual)->visualid != xvi->visualid)
        {
            visual = gdk_x11_screen_lookup_visual( screen, xvi->visualid );
            colormap = gdk_colormap_new(visual, FALSE);
        }

        gtk_widget_set_colormap( m_glWidget, colormap );
    }
    else
#endif // GTK+ >= 2.2
    {
        visual = gdkx_visual_get( xvi->visualid );
        colormap = gdk_colormap_new( visual, TRUE );

        gtk_widget_push_colormap( colormap );

        wxWindow::Create( parent, id, pos, size, style, name );
        m_glWidget = m_wxwindow;
    }

    gtk_widget_set_double_buffered( m_glWidget, FALSE );

#if WXWIN_COMPATIBILITY_2_8
    g_signal_connect(m_wxwindow, "realize",       G_CALLBACK(gtk_glwindow_realized_callback), this);
#endif // WXWIN_COMPATIBILITY_2_8
    g_signal_connect(m_wxwindow, "map",           G_CALLBACK(gtk_glwindow_map_callback),      this);
    g_signal_connect(m_wxwindow, "expose_event",  G_CALLBACK(gtk_glwindow_expose_callback),   this);
    g_signal_connect(m_widget,   "size_allocate", G_CALLBACK(gtk_glcanvas_size_callback),     this);

    if (gtk_check_version(2,2,0) != NULL)
    {
        gtk_widget_pop_colormap();
    }

#if WXWIN_COMPATIBILITY_2_8
    // if our parent window is already visible, we had been realized before we
    // connected to the "realize" signal and hence our m_glContext hasn't been
    // initialized yet and we have to do it now
    if (GTK_WIDGET_REALIZED(m_wxwindow))
        gtk_glwindow_realized_callback( m_wxwindow, this );
#endif // WXWIN_COMPATIBILITY_2_8

    if (GTK_WIDGET_MAPPED(m_wxwindow))
        gtk_glwindow_map_callback( m_wxwindow, this );

    return true;
}

Window wxGLCanvas::GetXWindow() const
{
    GdkWindow *window = GTK_PIZZA(m_wxwindow)->bin_window;
    return window ? GDK_WINDOW_XWINDOW(window) : 0;
}

void wxGLCanvas::OnInternalIdle()
{
    if (m_exposed)
    {
        wxPaintEvent event( GetId() );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );

        m_exposed = false;
        GetUpdateRegion().Clear();
    }

    wxWindow::OnInternalIdle();
}

#if WXWIN_COMPATIBILITY_2_8

void wxGLCanvas::GTKInitImplicitContext()
{
    if ( !m_glContext && m_createImplicitContext )
    {
        wxGLContext *share = m_sharedContext;
        if ( !share && m_sharedContextOf )
            share = m_sharedContextOf->m_glContext;

        m_glContext = new wxGLContext(this, share);
    }
}

#endif // WXWIN_COMPATIBILITY_2_8

#endif // wxUSE_GLCANVAS
