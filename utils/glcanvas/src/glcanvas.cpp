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

#include "gtk/gtk.h"
#include "gdk/gdk.h"
extern "C" {
#include "gdk/gdkx.h"
}

//---------------------------------------------------------------------------
// global variables
//---------------------------------------------------------------------------

XVisualInfo *g_visual_info = (XVisualInfo*) NULL;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win, const wxPalette& WXUNUSED(palette) )
{
    m_window = win;
    m_widget = win->m_wxwindow;
  
    wxCHECK_RET( g_visual_info != NULL, "invalid visual for OpenGl" );
    
    m_glContext = glXCreateContext( GDK_DISPLAY(), g_visual_info, None, GL_TRUE );
  
    wxCHECK_RET( m_glContext != NULL, "Couldn't create OpenGl context" );

    glXMakeCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
}

wxGLContext::~wxGLContext()
{
    if (m_glContext)
    {
        glXMakeCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
    
        glXDestroyContext( GDK_DISPLAY(), m_glContext );
    }
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

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxScrolledWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name,
    int *WXUNUSED(attribList), const wxPalette& palette):
  wxScrolledWindow(parent, id, pos, size, style, name)
{
    m_glContext = new wxGLContext( TRUE, this, palette );
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
    GetClientSize(& width, & height);

    if (m_glContext)
    {
        m_glContext->SetCurrent();

        glViewport(0, 0, (GLint)width, (GLint)height);
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

//--------------------------------------------------------------------
// wxGLModule 
//--------------------------------------------------------------------

class wxGLModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxGLModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxGLModule, wxModule)

bool wxGLModule::OnInit() 
{
    int data[] = { GLX_RGBA,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,
                   GLX_BLUE_SIZE,1,GLX_DOUBLEBUFFER,None};

    g_visual_info = glXChooseVisual( GDK_DISPLAY(), DefaultScreen(GDK_DISPLAY()), data );
  
    wxCHECK_MSG( g_visual_info != NULL, FALSE, "Couldn't choose visual for OpenGl" );
  
    wxVisualSetByExternal = gdkx_visual_get(g_visual_info->visualid);
  
    wxColormapSetByExternal = gdk_colormap_new( gdkx_visual_get(g_visual_info->visualid), TRUE );
    
    return TRUE;
}

void wxGLModule::OnExit()
{
}

