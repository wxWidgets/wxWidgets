/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/08/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "glcanvas.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/setup.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#include "wx/frame.h"
#include "wx/colour.h"
#include "wx/module.h"
#include "wx/app.h"

extern "C"
{
#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"
}

#include "wx/gtk/win_gtk.h"

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxGL")

//---------------------------------------------------------------------------
// global data
//---------------------------------------------------------------------------

XVisualInfo *g_vi = (XVisualInfo*) NULL;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win, const wxPalette& WXUNUSED(palette) )
{
    m_window = win;
    m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;
    XVisualInfo *vi = (XVisualInfo *) gc->m_vi;

    wxCHECK_RET( vi, _T("invalid visual for OpenGl") );

    m_glContext = glXCreateContext( GDK_DISPLAY(), vi, None, GL_TRUE );

    wxCHECK_RET( m_glContext, _T("Couldn't create OpenGl context") );
}

wxGLContext::wxGLContext(
               bool WXUNUSED(isRGB), wxWindow *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;
    m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;
    XVisualInfo *vi = (XVisualInfo *) gc->m_vi;

    wxCHECK_RET( vi, _T("invalid visual for OpenGl") );

    m_glContext = glXCreateContext( GDK_DISPLAY(), vi,
                                    other ? other->m_glContext : None,
                                    GL_TRUE );

    if ( !m_glContext )
    {
        wxFAIL_MSG( _T("Couldn't create OpenGl context") );
    }
}

wxGLContext::~wxGLContext()
{
    if (!m_glContext) return;

    if (m_glContext == glXGetCurrentContext())
    {
        glXMakeCurrent( GDK_DISPLAY(), None, NULL);
    }

    glXDestroyContext( GDK_DISPLAY(), m_glContext );
}

void wxGLContext::SwapBuffers()
{
    if (m_glContext)
    {
        GdkWindow *window = GTK_PIZZA(m_widget)->bin_window;
        glXSwapBuffers( GDK_DISPLAY(), GDK_WINDOW_XWINDOW( window ) );
    }
}

void wxGLContext::SetCurrent()
{
    if (m_glContext)
    {
        GdkWindow *window = GTK_PIZZA(m_widget)->bin_window;
        glXMakeCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(window), m_glContext );
    }
}

void wxGLContext::SetColour(const wxChar *colour)
{
    wxColour col = wxTheColourDatabase->Find(colour);
    if (col.Ok())
    {
        float r = (float)(col.Red()/256.0);
        float g = (float)(col.Green()/256.0);
        float b = (float)(col.Blue()/256.0);
        glColor3f( r, g, b);
    }
}

void wxGLContext::SetupPixelFormat()
{
}

void wxGLContext::SetupPalette( const wxPalette& WXUNUSED(palette) )
{
}

wxPalette wxGLContext::CreateDefaultPalette()
{
    return wxNullPalette;
}

//-----------------------------------------------------------------------------
// "realize" from m_wxwindow
//-----------------------------------------------------------------------------

static gint
gtk_glwindow_realized_callback( GtkWidget * WXUNUSED(widget), wxGLCanvas *win )
{
    // VZ: apparently in some cases we're called twice -- no idea why,
    //     but a check doesn't hurt
    if ( !win->m_glContext )
    {
        wxGLContext *share = win->m_sharedContext;
        if ( !share && win->m_sharedContextOf )
            share = win->m_sharedContextOf->GetContext();

        win->m_glContext = new wxGLContext( TRUE, win, wxNullPalette, share );
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "map" from m_wxwindow
//-----------------------------------------------------------------------------

static gint
gtk_glwindow_map_callback( GtkWidget * WXUNUSED(widget), wxGLCanvas *win )
{
    if (win->m_glContext/* && win->m_exposed*/)
    {
        wxPaintEvent event( win->GetId() );
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );

        win->m_exposed = FALSE;
        win->GetUpdateRegion().Clear();
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// "expose_event" of m_wxwindow
//-----------------------------------------------------------------------------

static void
gtk_glwindow_expose_callback( GtkWidget *WXUNUSED(widget), GdkEventExpose *gdk_event, wxGLCanvas *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    win->m_exposed = TRUE;

    win->GetUpdateRegion().Union( gdk_event->area.x,
                                  gdk_event->area.y,
                                  gdk_event->area.width,
                                  gdk_event->area.height );
}

//-----------------------------------------------------------------------------
// "draw" of m_wxwindow
//-----------------------------------------------------------------------------

#ifndef __WXGTK20__
static void
gtk_glwindow_draw_callback( GtkWidget *WXUNUSED(widget), GdkRectangle *rect, wxGLCanvas *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    win->m_exposed = TRUE;

    win->GetUpdateRegion().Union( rect->x, rect->y,
                                  rect->width, rect->height );
}
#endif

//-----------------------------------------------------------------------------
// "size_allocate" of m_wxwindow
//-----------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas( wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, NULL, NULL, id, pos, size, style, name, attribList, palette );
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
                        const wxGLContext *shared,
                        wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, shared, NULL, id, pos, size, style, name, attribList, palette );
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
                        const wxGLCanvas *shared,
                        wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, NULL, shared, id, pos, size, style, name, attribList, palette );
}

bool wxGLCanvas::Create( wxWindow *parent,
                         const wxGLContext *shared,
                         const wxGLCanvas *shared_context_of,
                         wxWindowID id,
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name,
                         int *attribList,
                         const wxPalette& palette)
{
    m_sharedContext = (wxGLContext*)shared;  // const_cast
    m_sharedContextOf = (wxGLCanvas*)shared_context_of;  // const_cast
    m_glContext = (wxGLContext*) NULL;

    m_exposed = FALSE;
    m_noExpose = TRUE;
    m_nativeSizeEvent = TRUE;

    XVisualInfo *vi = NULL;
    if (wxTheApp->m_glVisualInfo != NULL)
    {
        vi = (XVisualInfo *) wxTheApp->m_glVisualInfo;
        m_canFreeVi = FALSE; // owned by wxTheApp - don't free upon destruction
    }
    else
    {
        vi = (XVisualInfo *) ChooseGLVisual(attribList);
        m_canFreeVi = TRUE;
    }
    m_vi = vi;  // save for later use

    wxCHECK_MSG( m_vi, FALSE, _T("required visual couldn't be found") );

    GdkVisual *visual = gdkx_visual_get( vi->visualid );
    GdkColormap *colormap = gdk_colormap_new( gdkx_visual_get(vi->visualid), TRUE );

    gtk_widget_push_colormap( colormap );
    gtk_widget_push_visual( visual );

    wxWindow::Create( parent, id, pos, size, style, name );

    m_glWidget = m_wxwindow;

#ifdef __WXGTK20__
    gtk_widget_set_double_buffered( m_glWidget, FALSE );
#endif

    gtk_pizza_set_clear( GTK_PIZZA(m_wxwindow), FALSE );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "realize",
                            GTK_SIGNAL_FUNC(gtk_glwindow_realized_callback), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "map",
                            GTK_SIGNAL_FUNC(gtk_glwindow_map_callback), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "expose_event",
        GTK_SIGNAL_FUNC(gtk_glwindow_expose_callback), (gpointer)this );

#ifndef __WXGTK20__
    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "draw",
        GTK_SIGNAL_FUNC(gtk_glwindow_draw_callback), (gpointer)this );
#endif

    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_glcanvas_size_callback), (gpointer)this );

    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();

    if (GTK_WIDGET_REALIZED(m_wxwindow))
        gtk_glwindow_realized_callback( m_wxwindow, this );

    if (GTK_WIDGET_MAPPED(m_wxwindow))
        gtk_glwindow_map_callback( m_wxwindow, this );

    return TRUE;
}

wxGLCanvas::~wxGLCanvas()
{
    XVisualInfo *vi = (XVisualInfo *) m_vi;

    if (vi && m_canFreeVi) XFree( vi );
    delete m_glContext;
}

void* wxGLCanvas::ChooseGLVisual(int *attribList)
{
    int data[512];
    if (!attribList)
    {
        // default settings if attriblist = 0
        data[0] = GLX_RGBA;
        data[1] = GLX_DOUBLEBUFFER;
        data[2] = GLX_DEPTH_SIZE;   data[3] = 1;
        data[4] = GLX_RED_SIZE;     data[5] = 1;
        data[6] = GLX_GREEN_SIZE;   data[7] = 1;
        data[8] = GLX_BLUE_SIZE;    data[9] = 1;
        data[10] = GLX_ALPHA_SIZE;  data[11] = 0;
        data[12] = None;

        attribList = (int*) data;
    }
    else
    {
      int arg=0, p=0;

      while( (attribList[arg]!=0) && (p<510) )
      {
        switch( attribList[arg++] )
        {
          case WX_GL_RGBA: data[p++] = GLX_RGBA; break;
          case WX_GL_BUFFER_SIZE:
            data[p++]=GLX_BUFFER_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_LEVEL:
            data[p++]=GLX_LEVEL; data[p++]=attribList[arg++]; break;
          case WX_GL_DOUBLEBUFFER: data[p++] = GLX_DOUBLEBUFFER; break;
          case WX_GL_STEREO: data[p++] = GLX_STEREO; break;
          case WX_GL_AUX_BUFFERS:
            data[p++]=GLX_AUX_BUFFERS; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_RED:
            data[p++]=GLX_RED_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_GREEN:
            data[p++]=GLX_GREEN_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_BLUE:
            data[p++]=GLX_BLUE_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ALPHA:
            data[p++]=GLX_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_DEPTH_SIZE:
            data[p++]=GLX_DEPTH_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_STENCIL_SIZE:
            data[p++]=GLX_STENCIL_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_RED:
            data[p++]=GLX_ACCUM_RED_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_GREEN:
            data[p++]=GLX_ACCUM_GREEN_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_BLUE:
            data[p++]=GLX_ACCUM_BLUE_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_ACCUM_ALPHA:
            data[p++]=GLX_ACCUM_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
          default:
            break;
        }
      }
      data[p] = 0;

      attribList = (int*) data;
    }


    Display *dpy = GDK_DISPLAY();

    return glXChooseVisual( dpy, DefaultScreen(dpy), attribList );
}

void wxGLCanvas::SwapBuffers()
{
    if (m_glContext)
        m_glContext->SwapBuffers();
}

void wxGLCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
}

void wxGLCanvas::SetCurrent()
{
    if (m_glContext)
        m_glContext->SetCurrent();
}

void wxGLCanvas::SetColour( const wxChar *colour )
{
    if (m_glContext)
        m_glContext->SetColour( colour );
}

void wxGLCanvas::OnInternalIdle()
{
    if (m_glContext && m_exposed)
    {
        wxPaintEvent event( GetId() );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );

        m_exposed = FALSE;
        GetUpdateRegion().Clear();
    }

    wxWindow::OnInternalIdle();
}



//---------------------------------------------------------------------------
// wxGLApp
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLApp, wxApp)

wxGLApp::~wxGLApp()
{
    if (m_glVisualInfo)
        XFree(m_glVisualInfo);
}

bool wxGLApp::InitGLVisual(int *attribList)
{
    if (m_glVisualInfo)
        XFree(m_glVisualInfo);

    m_glVisualInfo = wxGLCanvas::ChooseGLVisual(attribList);

    return m_glVisualInfo != NULL;
}

#endif
    // wxUSE_GLCANVAS

