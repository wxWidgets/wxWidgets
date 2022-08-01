/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/08/98
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/backend.h"
#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#endif
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#ifdef __WXGTK3__
extern "C" {
static gboolean draw(GtkWidget* widget, cairo_t* cr, wxGLCanvas* win)
{
    GtkAllocation a;
    gtk_widget_get_allocation(widget, &a);
    if (a.width > win->m_size.x || a.height > win->m_size.y)
    {
        // GLX buffers are apparently not reliably updated to the new size
        // before the paint event occurs, resulting in newly exposed window
        // areas sometimes not being painted at the end of a drag resize.
        gdk_display_sync(gtk_widget_get_display(widget));
    }
    win->m_size.Set(a.width, a.height);

    win->GTKSendPaintEvents(cr);
    return false;
}
}
#endif // __WXGTK3__

//-----------------------------------------------------------------------------
// emission hook for "parent-set"
//-----------------------------------------------------------------------------

#if !wxUSE_GLCANVAS_EGL
extern "C" {
static gboolean
parent_set_hook(GSignalInvocationHint*, guint, const GValue* param_values, void* data)
{
    wxGLCanvas* win = (wxGLCanvas*)data;
    if (g_value_peek_pointer(&param_values[0]) == win->m_wxwindow)
    {
        const XVisualInfo* xvi = static_cast<XVisualInfo*>(win->GetXVisualInfo());
        GdkVisual* visual = gtk_widget_get_visual(win->m_wxwindow);
        if (GDK_VISUAL_XVISUAL(visual)->visualid != xvi->visualid)
        {
            GdkScreen* screen = gtk_widget_get_screen(win->m_wxwindow);
            visual = gdk_x11_screen_lookup_visual(screen, xvi->visualid);
#ifdef __WXGTK3__
            gtk_widget_set_visual(win->m_wxwindow, visual);
#else
            GdkColormap* colormap = gdk_colormap_new(visual, false);
            gtk_widget_set_colormap(win->m_wxwindow, colormap);
            g_object_unref(colormap);
#endif
        }
        // remove hook
        return false;
    }
    return true;
}
}
#endif

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLCanvas, wxWindow);

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLAttributes& dispAttrs,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

static bool IsAvailable()
{
#if defined(__WXGTK3__) && (defined(GDK_WINDOWING_WAYLAND) || defined(GDK_WINDOWING_X11))
    GdkDisplay* display = gdk_display_get_default();
#endif

#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(display))
    {
#if wxUSE_GLCANVAS_EGL
        return true;
#else
        wxSafeShowMessage(_("Fatal Error"), _("This program wasn't compiled with EGL support required under Wayland, either\ninstall EGL libraries and rebuild or run it under X11 backend by setting\nenvironment variable GDK_BACKEND=x11 before starting your program."));
        return false;
#endif // wxUSE_GLCANVAS_EGL
    }
#endif // GDK_WINDOWING_WAYLAND

#ifdef GDK_WINDOWING_X11
#ifdef __WXGTK3__
    if (wxGTKImpl::IsX11(display))
#endif
    {
        return true;
    }
#endif

    wxSafeShowMessage(_("Fatal Error"), _("wxGLCanvas is only supported on Wayland and X11 currently.  You may be able to\nwork around this by setting environment variable GDK_BACKEND=x11 before\nstarting your program."));
    return false;
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
    if ( !IsAvailable() )
        return false;

    // Separate 'GLXFBConfig/XVisual' attributes.
    // Also store context attributes for wxGLContext ctor
    wxGLAttributes dispAttrs;
    if ( ! ParseAttribList(attribList, dispAttrs, &m_GLCTXAttrs) )
        return false;

    return Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        const wxGLAttributes& dispAttrs,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const wxPalette& palette)
{
    if ( !IsAvailable() )
        return false;

#if wxUSE_PALETTE
    wxASSERT_MSG( !palette.IsOk(), wxT("palettes not supported") );
#endif // wxUSE_PALETTE
    wxUnusedVar(palette); // Unused when wxDEBUG_LEVEL==0

    m_nativeSizeEvent = true;
#ifdef __WXGTK3__
    m_noExpose = true;
    m_backgroundStyle = wxBG_STYLE_PAINT;
#endif

    if ( !InitVisual(dispAttrs) )
        return false;

    // watch for the "parent-set" signal on m_wxwindow so we can set colormap
    // before m_wxwindow is realized (which will occur before
    // wxWindow::Create() returns if parent is already visible)
#if !wxUSE_GLCANVAS_EGL
    unsigned sig_id = g_signal_lookup("parent-set", GTK_TYPE_WIDGET);
    g_signal_add_emission_hook(sig_id, 0, parent_set_hook, this, NULL);
#endif

    wxWindow::Create( parent, id, pos, size, style, name );
#ifdef __WXGTK3__
    g_signal_connect(m_wxwindow, "draw", G_CALLBACK(draw), this);
#endif

    gtk_widget_set_double_buffered(m_wxwindow, false);

    return true;
}

bool wxGLCanvas::SetBackgroundStyle(wxBackgroundStyle /* style */)
{
    return false;
}

unsigned long wxGLCanvas::GetXWindow() const
{
#if defined(GDK_WINDOWING_X11)
    GdkWindow* window = GTKGetDrawingWindow();
    return window ? GDK_WINDOW_XID(window) : 0;
#else
    return 0;
#endif
}

void wxGLCanvas::GTKHandleRealized()
{
    BaseType::GTKHandleRealized();

#if wxUSE_GLCANVAS_EGL
    CreateSurface();
#endif
    SendSizeEvent();
}

#endif // wxUSE_GLCANVAS
