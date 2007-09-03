/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets
//              Uses the GLX extension.
// Author:      Julian Smart and Wolfram Gloger
// Modified by:
// Created:     1995, 1999
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Wolfram Gloger
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxGL")

static inline WXWindow wxGetClientAreaWindow(wxWindow* win)
{
#ifdef __WXMOTIF__
    return win->GetClientXWindow();
#else
    return win->GetClientAreaWindow();
#endif
}

#ifdef OLD_MESA
// workaround for bug in Mesa's glx.c
static int bitcount( unsigned long n )
{
    int bits;
    for (bits=0; n>0;)
    {
        if(n & 1) bits++;
        n = n >> 1;
    }
    return bits;
}
#endif

/*
 * GLContext implementation
 */

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win,
                          const wxPalette& WXUNUSED(palette) )
{
    m_window = win;
    // m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;
    XVisualInfo *vi = (XVisualInfo *) gc->m_vi;

    wxCHECK_RET( vi, wxT("invalid visual for OpenGL") );

    m_glContext = glXCreateContext( (Display *)wxGetDisplay(), vi,
                                    None, GL_TRUE);

    wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGL context") );
}

wxGLContext::wxGLContext(
               bool WXUNUSED(isRGB), wxWindow *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;
    // m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;
    XVisualInfo *vi = (XVisualInfo *) gc->m_vi;

    wxCHECK_RET( vi, wxT("invalid visual for OpenGL") );

    if( other != 0 )
        m_glContext = glXCreateContext( (Display *)wxGetDisplay(), vi,
                                        other->m_glContext, GL_TRUE );
    else
        m_glContext = glXCreateContext( (Display *)wxGetDisplay(), vi,
                                        None, GL_TRUE );

    wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGL context") );
}

wxGLContext::~wxGLContext()
{
    if (!m_glContext) return;

    if (m_glContext == glXGetCurrentContext())
    {
        glXMakeCurrent( (Display*) wxGetDisplay(), None, NULL);
    }

    glXDestroyContext( (Display*) wxGetDisplay(), m_glContext );
}

void wxGLContext::SwapBuffers()
{
    if (m_glContext)
    {
        Display* display = (Display*) wxGetDisplay();
        glXSwapBuffers(display, (Window) wxGetClientAreaWindow(m_window));
    }
}

void wxGLContext::SetCurrent()
{
    if (m_glContext)
    {
        Display* display = (Display*) wxGetDisplay();
        glXMakeCurrent(display, (Window) wxGetClientAreaWindow(m_window),
                       m_glContext );
    }
}

void wxGLContext::SetColour(const wxChar *colour)
{
    wxColour the_colour = wxTheColourDatabase->Find(colour);
    if(the_colour.Ok())
    {
        GLboolean b;
        glGetBooleanv(GL_RGBA_MODE, &b);
        if(b)
        {
            glColor3ub(the_colour.Red(),
                    the_colour.Green(),
                    the_colour.Blue());
        }
        else
        {
#ifdef __WXMOTIF__
            the_colour.AllocColour(m_window->GetXDisplay());
#else
            the_colour.CalcPixel(wxTheApp->GetMainColormap(wxGetDisplay()));
#endif
            GLint pix = (GLint)the_colour.GetPixel();
            if(pix == -1)
            {
                wxLogError(wxT("wxGLCanvas: cannot allocate color\n"));
                return;
            }
            glIndexi(pix);
        }
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




/*
 * GLCanvas implementation
 */

IMPLEMENT_CLASS(wxGLCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxScrolledWindow)
//    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()


wxGLCanvas::wxGLCanvas( wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
: wxScrolledWindow(parent, id, pos, size, style, name)
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
: wxScrolledWindow(parent, id, pos, size, style, name)
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
: wxScrolledWindow(parent, id, pos, size, style, name)
{
    Create( parent, NULL, shared, id, pos, size, style, name, attribList, palette );
}


/*
bool wxGLCanvas::Create(wxWindow *parent,
  const wxGLContext *shared, const wxGLCanvas *shared_context_of,
  wxWindowID id = -1, const wxPoint& pos,
  const wxSize& size, long style,
  const wxString& name, int *attribList, const wxPalette& palette):
    wxScrolledWindow(parent, id, pos, size, style, name)
*/

bool wxGLCanvas::Create( wxWindow *parent,
                         const wxGLContext *shared,
                         const wxGLCanvas *shared_context_of,
                         wxWindowID id,
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name,
                         int *attribList,
                         const wxPalette& palette)
{
    XVisualInfo *vi, vi_templ;
    XWindowAttributes xwa;
    int val, n;

    m_sharedContext = (wxGLContext*)shared;  // const_cast
    m_sharedContextOf = (wxGLCanvas*)shared_context_of;  // const_cast
    m_glContext = (wxGLContext*) NULL;

    Display* display = (Display*) wxGetDisplay();

    // Check for the presence of the GLX extension
    if(!glXQueryExtension(display, NULL, NULL))
    {
        wxLogDebug(wxT("wxGLCanvas: GLX extension is missing\n"));
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
          case WX_GL_DOUBLEBUFFER: data[p++] = GLX_DOUBLEBUFFER; data[p++] = 1; break;
          case WX_GL_STEREO: data[p++] = GLX_STEREO; data[p++] = 1; break;
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
      // Get an appropriate visual
      vi = glXChooseVisual(display, DefaultScreen(display), attribList);
      if(!vi) return false;

      // Here we should make sure that vi is the same visual as the
      // one used by the xwindow drawable in wxCanvas.  However,
      // there is currently no mechanism for this in wx_canvs.cc.
    } else {
        // By default, we use the visual of xwindow
        // NI: is this really senseful ? opengl in e.g. color index mode ?
      XGetWindowAttributes(display, (Window)wxGetClientAreaWindow(this), &xwa);
      vi_templ.visualid = XVisualIDFromVisual(xwa.visual);
      vi = XGetVisualInfo(display, VisualIDMask, &vi_templ, &n);
      if(!vi) return false;
      glXGetConfig(display, vi, GLX_USE_GL, &val);
      if(!val) return false;
      // Basically, this is it.  It should be possible to use vi
      // in glXCreateContext() below.  But this fails with Mesa.
      // I notified the Mesa author about it; there may be a fix.
#ifdef OLD_MESA
      // Construct an attribute list matching the visual
      int a_list[32];
      n = 0;
      if(vi->c_class==TrueColor || vi->c_class==DirectColor) { // RGBA visual
          a_list[n++] = GLX_RGBA;
          a_list[n++] = GLX_RED_SIZE;
          a_list[n++] = bitcount(vi->red_mask);
          a_list[n++] = GLX_GREEN_SIZE;
          a_list[n++] = bitcount(vi->green_mask);
          a_list[n++] = GLX_BLUE_SIZE;
          a_list[n++] = bitcount(vi->blue_mask);
          glXGetConfig(display, vi, GLX_ALPHA_SIZE, &val);
          a_list[n++] = GLX_ALPHA_SIZE;
          a_list[n++] = val;
      } else { // Color index visual
          glXGetConfig(display, vi, GLX_BUFFER_SIZE, &val);
          a_list[n++] = GLX_BUFFER_SIZE;
          a_list[n++] = val;
      }
      a_list[n] = None;
      // XFree(vi);
      vi = glXChooseVisual(display, DefaultScreen(display), a_list);
      if(!vi) return false;
#endif /* OLD_MESA */
    }

    m_vi = vi;  // safe for later use

    wxCHECK_MSG( m_vi, false, wxT("required visual couldn't be found") );

    // Create the GLX context and make it current

    wxGLContext *share= m_sharedContext;
    if (share==NULL && m_sharedContextOf)
        share = m_sharedContextOf->GetContext();

    m_glContext = new wxGLContext( TRUE, this, wxNullPalette, share );

#ifndef OLD_MESA
    // XFree(vi);
#endif
    SetCurrent();

    return true;
}

wxGLCanvas::~wxGLCanvas(void)
{
    XVisualInfo *vi = (XVisualInfo *) m_vi;

    if (vi) XFree( vi );
    if (m_glContext) delete m_glContext;

    // Display* display = (Display*) GetXDisplay();
    // if(glx_cx) glXDestroyContext(display, glx_cx);
}

void wxGLCanvas::SwapBuffers()
{
    if( m_glContext ) m_glContext->SwapBuffers();

    // Display* display = (Display*) GetXDisplay();
    // if(glx_cx) glXSwapBuffers(display, (Window) GetClientAreaWindow());
}

void wxGLCanvas::SetCurrent()
{
    if( m_glContext ) m_glContext->SetCurrent();

    // Display* display = (Display*) GetXDisplay();
    // if(glx_cx) glXMakeCurrent(display, (Window) GetClientAreaWindow(), glx_cx);
}

void wxGLCanvas::SetColour(const wxChar *col)
{
    if( m_glContext ) m_glContext->SetColour(col);
}

#endif
    // wxUSE_GLCANVAS
