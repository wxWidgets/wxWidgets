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

#include "glcanvas.h"

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

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win, const wxPalette& WXUNUSED(palette) )
{
    m_window = win;
    m_widget = ((wxGLCanvas*)win)->m_glWidget;
  
    wxCHECK_RET( g_vi, "invalid visual for OpenGl" );
    
    m_glContext = glXCreateContext( GDK_DISPLAY(), g_vi, None, GL_TRUE );
  
    wxCHECK_RET( m_glContext, "Couldn't create OpenGl context" );
}

wxGLContext::wxGLContext( 
               bool WXUNUSED(isRGB), wxWindow *win, 
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;
    m_widget = ((wxGLCanvas*)win)->m_glWidget;
  
    wxCHECK_RET( g_vi, "invalid visual for OpenGl" );
    
    if( other != 0 )
      m_glContext = glXCreateContext( GDK_DISPLAY(), g_vi, other->m_glContext,
                                      GL_TRUE );
    else
      m_glContext = glXCreateContext( GDK_DISPLAY(), g_vi, None, GL_TRUE );
    
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
        glXSwapBuffers( GDK_DISPLAY(), GDK_WINDOW_XWINDOW( m_widget->window ) );
    }
}

void wxGLContext::SetCurrent()
{
    if (m_glContext) 
    { 
        glXMakeCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
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
    win->m_glContext = new wxGLContext( TRUE, win, wxNullPalette, win->m_glContext );

    XFree( g_vi );
    g_vi = (XVisualInfo*) NULL;

    return FALSE;
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
    Create( parent, NULL, id, pos, size, style, name, attribList, palette );
}

wxGLCanvas::wxGLCanvas( wxWindow *parent, 
                        const wxGLContext *shared,
                        wxWindowID id,
                        const wxPoint& pos, const wxSize& size, 
			long style, const wxString& name,
                        int *attribList, 
			const wxPalette& palette )
{			
    Create( parent, shared, id, pos, size, style, name, attribList, palette );
}

bool wxGLCanvas::Create( wxWindow *parent, 
                         const wxGLContext *shared,
                         wxWindowID id,
                         const wxPoint& pos, const wxSize& size, 
			 long style, const wxString& name,
                         int *attribList, 
			 const wxPalette& palette)
{
    m_glContext = (wxGLContext*)shared;  // const_cast
    
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
    
    g_vi = glXChooseVisual( dpy, DefaultScreen(dpy), attribList );
    
    wxCHECK_MSG( g_vi, FALSE, "required visual couldn't be found" );

    GdkVisual *visual = gdkx_visual_get( g_vi->visualid );
    GdkColormap *colormap = gdk_colormap_new( gdkx_visual_get(g_vi->visualid), TRUE );
    
    gtk_widget_push_colormap( colormap );
    gtk_widget_push_visual( visual );

    wxScrolledWindow::Create( parent, id, pos, size, style, name );

    m_glWidget = m_wxwindow;
    
    gtk_signal_connect( GTK_OBJECT(m_glWidget), "realize",
                            GTK_SIGNAL_FUNC(gtk_glwindow_realized_callback), (gpointer) this );
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();
    
    return TRUE;
}

wxGLCanvas::~wxGLCanvas()
{
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

GtkWidget *wxGLCanvas::GetConnectWidget()
{
    return m_wxwindow;
}

bool wxGLCanvas::IsOwnGtkWindow( GdkWindow *window )
{
    return (window == m_wxwindow->window);
}
