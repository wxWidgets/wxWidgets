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

#include "wx/wxprec.h"

#include "wx/frame.h"
#include "wx/colour.h"
#include "glcanvas.h"
#include <gdk/gdkx.h>

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win, const wxPalette& WXUNUSED(palette) )
{
  m_window = win;
  m_widget = win->m_wxwindow;
  
  int data[] = {GLX_RGBA,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,
                GLX_BLUE_SIZE,1,GLX_DOUBLEBUFFER,None};

  Display *display = GDK_WINDOW_XDISPLAY( m_widget->window );
  XVisualInfo *visual_info = glXChooseVisual( display, DefaultScreen(display), data );
  
  wxCHECK_RET( visual_info != NULL, "Couldn't choose visual for OpenGl" );

  m_glContext = glXCreateContext( display, visual_info, None, GL_TRUE );
  
  wxCHECK_RET( m_glContext != NULL, "Couldn't create OpenGl context" );

  glXMakeCurrent( display, GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
}

wxGLContext::~wxGLContext()
{
  if (m_glContext)
  {
    Display *display = GDK_WINDOW_XDISPLAY( m_widget->window );
    glXMakeCurrent( display, GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
    
    glXDestroyContext( display, m_glContext );
  }
}

void wxGLContext::SwapBuffers()
{
  if (m_glContext)
  {
    Display *display = GDK_WINDOW_XDISPLAY( m_widget->window );
    glXSwapBuffers( display, GDK_WINDOW_XWINDOW( m_widget->window ) );
  }
}

void wxGLContext::SetCurrent()
{
  if (m_glContext) 
  { 
    Display *display = GDK_WINDOW_XDISPLAY( m_widget->window );
    glXMakeCurrent( display, GDK_WINDOW_XWINDOW(m_widget->window), m_glContext );
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


