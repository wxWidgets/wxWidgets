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
// "expose_event" of m_glWidget
//-----------------------------------------------------------------------------

static void gtk_window_expose_callback( GtkWidget *WXUNUSED(widget), GdkEventExpose *gdk_event, wxWindow *win )
{
    if (!win->HasVMT()) return;

    win->m_updateRegion.Union( gdk_event->area.x,
                               gdk_event->area.y,
                               gdk_event->area.width,
                               gdk_event->area.height );

    if (gdk_event->count > 0) return;

/*
    printf( "OnExpose from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    wxPaintEvent event( win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    win->m_updateRegion.Clear();
}

//-----------------------------------------------------------------------------
// "draw" of m_glWidget
//-----------------------------------------------------------------------------

static void gtk_window_draw_callback( GtkWidget *WXUNUSED(widget), GdkRectangle *rect, wxWindow *win )
{
    if (!win->HasVMT()) return;

    win->m_updateRegion.Union( rect->x, rect->y, rect->width, rect->height );

    wxPaintEvent event( win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );

    win->m_updateRegion.Clear();
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
    Create( parent, id, pos, size, style, name, attribList, palette );
}

bool wxGLCanvas::Create( wxWindow *parent, wxWindowID id,
                         const wxPoint& pos, const wxSize& size, 
			 long style, const wxString& name,
                         int *attribList, 
			 const wxPalette& palette )
{
    if (!attribList)
    {
        int data[] = { GLX_RGBA, 
	               GLX_DOUBLEBUFFER, 
		       GLX_DEPTH_SIZE, 1,
		       None };
	attribList = (int*) data;
    }
    
    Display *dpy = GDK_DISPLAY();
    
    g_vi = glXChooseVisual( dpy, DefaultScreen(dpy), attribList );
    
    GdkVisual *visual = gdkx_visual_get( g_vi->visualid );
    GdkColormap *colormap = gdk_colormap_new( gdkx_visual_get(g_vi->visualid), TRUE );
    
    gtk_widget_push_colormap( colormap );
    gtk_widget_push_visual( visual );
    
    m_glWidget = gtk_drawing_area_new();
    gtk_widget_set_events( m_glWidget,
      GDK_EXPOSURE_MASK	|
      GDK_POINTER_MOTION_HINT_MASK |
      GDK_POINTER_MOTION_MASK	|
      GDK_BUTTON_MOTION_MASK	|
      GDK_BUTTON1_MOTION_MASK	|
      GDK_BUTTON2_MOTION_MASK	|
      GDK_BUTTON3_MOTION_MASK	|
      GDK_BUTTON_PRESS_MASK	|
      GDK_BUTTON_RELEASE_MASK	|
      GDK_KEY_PRESS_MASK	|
      GDK_KEY_RELEASE_MASK	|
      GDK_ENTER_NOTIFY_MASK	|
      GDK_LEAVE_NOTIFY_MASK );
    
    GTK_WIDGET_SET_FLAGS( m_glWidget, GTK_CAN_FOCUS );
    
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();
    
    wxScrolledWindow::Create( parent, id, pos, size, style, name );
  
    gtk_myfixed_put( GTK_MYFIXED(m_wxwindow), m_glWidget, 0, 0 );
    
    gtk_signal_connect( GTK_OBJECT(m_glWidget), "expose_event",
      GTK_SIGNAL_FUNC(gtk_window_expose_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_glWidget), "draw",
      GTK_SIGNAL_FUNC(gtk_window_draw_callback), (gpointer)this );
      
    /* connect to key press and mouse handlers etc. */  
    ConnectWidget( m_glWidget );
    
    gtk_widget_show( m_glWidget );
    
    m_glContext = new wxGLContext( TRUE, this, palette );
    
    XFree( g_vi );
    g_vi = (XVisualInfo*) NULL;
    
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

void wxGLCanvas::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    if (m_resizing) return; // I don't like recursions
    m_resizing = TRUE;

    if (m_parent->m_wxwindow == NULL) // i.e. wxNotebook
    {
        // don't set the size for children of wxNotebook, just take the values.
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }
    else
    {
        int old_width = m_width;
        int old_height = m_height;

        if ((sizeFlags & wxSIZE_USE_EXISTING) == wxSIZE_USE_EXISTING)
        {
            if (x != -1) m_x = x;
            if (y != -1) m_y = y;
            if (width != -1) m_width = width;
            if (height != -1) m_height = height;
        }
        else
        {
            m_x = x;
            m_y = y;
            m_width = width;
            m_height = height;
        }

        if ((sizeFlags & wxSIZE_AUTO_WIDTH) == wxSIZE_AUTO_WIDTH)
        {
             if (width == -1) m_width = 80;
        }

        if ((sizeFlags & wxSIZE_AUTO_HEIGHT) == wxSIZE_AUTO_HEIGHT)
        {
             if (height == -1) m_height = 26;
        }

        if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
        if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
        if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_maxWidth;
        if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_maxHeight;

        gtk_myfixed_move( GTK_MYFIXED(m_parent->m_wxwindow), m_widget, m_x, m_y );

        if ((old_width != m_width) || (old_height != m_height))
	{
            gtk_widget_set_usize( m_widget, m_width, m_height );
	    
            gtk_drawing_area_size( GTK_DRAWING_AREA(m_glWidget), m_width, m_height );
	    
	    GtkAllocation allo;
	    allo.x = 0;
	    allo.y = 0;
	    allo.width = m_width;
	    allo.height = m_height;
            gtk_widget_size_allocate( m_glWidget, &allo );
	}
    }

    m_sizeSet = TRUE;

    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    m_resizing = FALSE;
}

GtkWidget *wxGLCanvas::GetConnectWidget()
{
    return m_glWidget;
}

bool wxGLCanvas::IsOwnGtkWindow( GdkWindow *window )
{
    return (window == m_glWidget->window);
}
