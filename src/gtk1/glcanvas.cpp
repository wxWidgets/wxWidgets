/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWindows and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/08/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "glcanvas.h"
#endif

#include "wx/setup.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#include "wx/frame.h"
#include "wx/colour.h"
#include "wx/module.h"
#include "wx/app.h"

extern "C" {
#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"
}

#include "wx/gtk/win_gtk.h"

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
    
    wxCHECK_RET( vi, "invalid visual for OpenGl" );
    
    m_glContext = glXCreateContext( GDK_DISPLAY(), vi, None, GL_TRUE );
  
    wxCHECK_RET( m_glContext, "Couldn't create OpenGl context" );
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
    
    wxCHECK_RET( vi, "invalid visual for OpenGl" );
    
    if( other != 0 )
        m_glContext = glXCreateContext( GDK_DISPLAY(), vi, other->m_glContext, GL_TRUE );
    else
        m_glContext = glXCreateContext( GDK_DISPLAY(), vi, None, GL_TRUE );
    
    wxCHECK_RET( m_glContext, "Couldn't create OpenGl context" );
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

void wxGLContext::SetColour(const char *colour)
{
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    wxColour *col = wxTheColourDatabase->FindColour(colour);
    if (col)
    {
        r = (float)(col->Red()/256.0);
        g = (float)(col->Green()/256.0);
        b = (float)(col->Blue()/256.0);
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
    wxGLContext *share= win->m_sharedContext;
    if (share==NULL && win->m_sharedContextOf) share=win->m_sharedContextOf->GetContext();

    win->m_glContext = new wxGLContext( TRUE, win, wxNullPalette, share );

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

static void 
gtk_glwindow_draw_callback( GtkWidget *WXUNUSED(widget), GdkRectangle *rect, wxGLCanvas *win )
{
    if (g_isIdle) 
        wxapp_install_idle_handler();

    win->m_exposed = TRUE;

    win->GetUpdateRegion().Union( rect->x, rect->y,
                                  rect->width, rect->height );
}

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

IMPLEMENT_CLASS(wxGLCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxScrolledWindow)
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
    
    if (!attribList)
    {
        int data[] = { GLX_RGBA, 
	               GLX_DOUBLEBUFFER, 
		       GLX_DEPTH_SIZE, 1,  // use largest available depth buffer
		       GLX_RED_SIZE, 1, 
		       GLX_GREEN_SIZE, 1, 
		       GLX_BLUE_SIZE, 1, 
		       GLX_ALPHA_SIZE, 0, 
		       None };
	attribList = (int*) data;
    }
    else
    {
      int data[512], arg=0, p=0;
       
      while( (attribList[arg]!=0) && (p<512) )
      {
        switch( attribList[arg++] )
        {
          case WX_GL_RGBA: data[p++] = GLX_RGBA; break;
          case WX_GL_DOUBLEBUFFER: data[p++] = GLX_DOUBLEBUFFER; break;
          case WX_GL_DEPTH_SIZE: 
            data[p++]=GLX_DEPTH_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_RED:
            data[p++]=GLX_RED_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_GREEN:
            data[p++]=GLX_GREEN_SIZE; data[p++]=attribList[arg++]; break;
          case WX_GL_MIN_BLUE:
            data[p++]=GLX_BLUE_SIZE; data[p++]=attribList[arg++]; break;
          default:
            break;
        }
      }       
      data[p] = 0; 

      attribList = (int*) data;
    }
    
    
    Display *dpy = GDK_DISPLAY();
    
    XVisualInfo *vi = glXChooseVisual( dpy, DefaultScreen(dpy), attribList );
    
    m_vi = vi;  // safe for later use
    
    wxCHECK_MSG( m_vi, FALSE, "required visual couldn't be found" );

    GdkVisual *visual = gdkx_visual_get( vi->visualid );
    GdkColormap *colormap = gdk_colormap_new( gdkx_visual_get(vi->visualid), TRUE );
    
    gtk_widget_push_colormap( colormap );
    gtk_widget_push_visual( visual );

    wxScrolledWindow::Create( parent, id, pos, size, style, name );

    m_glWidget = m_wxwindow;
    
    gtk_pizza_set_clear( GTK_PIZZA(m_wxwindow), FALSE );
    
    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "realize",
                            GTK_SIGNAL_FUNC(gtk_glwindow_realized_callback), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "map",
                            GTK_SIGNAL_FUNC(gtk_glwindow_map_callback), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "expose_event",
        GTK_SIGNAL_FUNC(gtk_glwindow_expose_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_wxwindow), "draw",
        GTK_SIGNAL_FUNC(gtk_glwindow_draw_callback), (gpointer)this );
	
    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_glcanvas_size_callback), (gpointer)this );

    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();
    
    return TRUE;
}

wxGLCanvas::~wxGLCanvas()
{
    XVisualInfo *vi = (XVisualInfo *) m_vi;
    
    if (vi) XFree( vi );
    if (m_glContext) delete m_glContext;
}

void wxGLCanvas::SwapBuffers()
{
    if (m_glContext) m_glContext->SwapBuffers();
}

void wxGLCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
    int width, height;
    GetClientSize( &width, &height );
    
    if (m_glContext && GTK_WIDGET_REALIZED(m_glWidget) )
    {
        SetCurrent(); 
	
        glViewport(0, 0, (GLint)width, (GLint)height );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
        glMatrixMode(GL_MODELVIEW);
    }
}

void wxGLCanvas::SetCurrent()
{
    if (m_glContext) m_glContext->SetCurrent();
}

void wxGLCanvas::SetColour( const char *colour )
{
    if (m_glContext) m_glContext->SetColour( colour );
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

#endif
    // wxUSE_GLCANVAS

