///////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets
//              Uses the GLX extension.
// Author:      Julian Smart and Wolfram Gloger
// Modified by: Vadim Zeitlin to update to new API
// Created:     1995, 1999
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Wolfram Gloger
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// TODO: merge this with wxGTK version in some src/unix/glcanvasx11.cpp

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
#endif

#ifdef __VMS
# pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#ifdef __VMS
# pragma message enable nosimpint
#endif
#include "wx/x11/private.h"

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

static inline Window wxGetClientAreaWindow(const wxWindow* win)
{
    return (Window)win->
#ifdef __WXMOTIF__
    GetClientXWindow();
#else
    GetClientAreaWindow();
#endif
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext, wxObject)

wxGLContext::wxGLContext(wxGLCanvas *win, const wxGLContext *other)
{
    XVisualInfo *vi = (XVisualInfo *) win->m_vi;
    wxASSERT_MSG( vi, wxT("invalid visual in wxGLCanvas") );

    m_glContext = glXCreateContext( (Display *)wxGetDisplay(),
                                    vi,
                                    other ? other->m_glContext : None,
                                    GL_TRUE);

    wxASSERT_MSG( m_glContext, wxT("Couldn't create OpenGL context") );
}

wxGLContext::~wxGLContext()
{
    if (!m_glContext)
        return;

    if (m_glContext == glXGetCurrentContext())
    {
        glXMakeCurrent( (Display*) wxGetDisplay(), None, NULL);
    }

    glXDestroyContext( (Display*) wxGetDisplay(), m_glContext );
}

void wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if (m_glContext)
    {
        Display* display = (Display*) wxGetDisplay();
        glXMakeCurrent(display, wxGetClientAreaWindow(&win), m_glContext);
    }
}

// ----------------------------------------------------------------------------
// wxGLCanvas
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

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

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
    m_vi = NULL;

    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    XVisualInfo *vi, vi_templ;
    XWindowAttributes xwa;
    int val, n;

    Display* display = (Display*) wxGetDisplay();

    // Check for the presence of the GLX extension
    if(!glXQueryExtension(display, NULL, NULL))
    {
        wxLogDebug(wxT("wxGLCanvas: GLX extension is missing"));
        return false;
    }

    if(attribList) {
      int data[512], arg=0, p=0;

      while( (attribList[arg]!=0) && (p<512) )
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

      // Get an appropriate visual
      vi = glXChooseVisual(display, DefaultScreen(display), data);
      if(!vi) return false;

      // Here we should make sure that vi is the same visual as the
      // one used by the xwindow drawable in wxCanvas.  However,
      // there is currently no mechanism for this in wx_canvs.cc.
    }
    else // default attributes
    {
        // By default, we use the visual of xwindow
      XGetWindowAttributes(display, wxGetClientAreaWindow(this), &xwa);
      vi_templ.visualid = XVisualIDFromVisual(xwa.visual);
      vi = XGetVisualInfo(display, VisualIDMask, &vi_templ, &n);
      if(!vi) return false;
      glXGetConfig(display, vi, GLX_USE_GL, &val);
      if(!val) return false;
    }

    m_vi = vi;  // safe for later use

    wxCHECK_MSG( m_vi, false, wxT("required visual couldn't be found") );

    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    XVisualInfo *vi = (XVisualInfo *) m_vi;

    if (vi)
        XFree( vi );
}

void wxGLCanvas::SwapBuffers()
{
    glXSwapBuffers((Display *)wxGetDisplay(), wxGetClientAreaWindow(this));
}

int wxGLCanvas::GetColourIndex(const wxColour& col_)
{
    wxColour& col = wx_const_cast(wxColour&, col_);

#ifdef __WXMOTIF__
    col.AllocColour(GetXDisplay());
#else
    col.CalcPixel(wxTheApp->GetMainColormap(wxGetDisplay()));
#endif

    return col.GetPixel();
}

#endif // wxUSE_GLCANVAS
