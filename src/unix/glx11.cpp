///////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/glx11.cpp
// Purpose:     code common to all X11-based wxGLCanvas implementations
// Author:      Vadim Zeitlin
// Created:     2007-04-15
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/glcanvas.h"

// ============================================================================
// wxGLContext implementation
// ============================================================================

IMPLEMENT_CLASS(wxGLContext, wxObject)

wxGLContext::wxGLContext(wxGLCanvas *gc, const wxGLContext *other)
{
    if ( wxGLCanvas::GetGLXVersion() >= 13 )
    {
        GLXFBConfig *fbc = gc->GetGLXFBConfig();
        wxCHECK_RET( fbc, _T("invalid GLXFBConfig for OpenGL") );

        m_glContext = glXCreateNewContext( wxGetX11Display(), fbc[0], GLX_RGBA_TYPE,
                                           other ? other->m_glContext : None,
                                           GL_TRUE );
    }
    else // GLX <= 1.2
    {
        XVisualInfo *vi = gc->GetXVisualInfo();
        wxCHECK_RET( vi, _T("invalid visual for OpenGL") );

        m_glContext = glXCreateContext( wxGetX11Display(), vi,
                                        other ? other->m_glContext : None,
                                        GL_TRUE );
    }

    wxASSERT_MSG( m_glContext, _T("Couldn't create OpenGL context") );
}

wxGLContext::~wxGLContext()
{
    if ( !m_glContext )
        return;

    if ( m_glContext == glXGetCurrentContext() )
        MakeCurrent(None, NULL);

    glXDestroyContext( wxGetX11Display(), m_glContext );
}

void wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return;

    const Window xid = win.GetXWindow();
    wxCHECK_RET( xid, _T("window must be shown") );

    MakeCurrent(xid, m_glContext);
}

// wrapper around glXMakeContextCurrent/glXMakeCurrent depending on GLX
// version
/* static */
void wxGLContext::MakeCurrent(GLXDrawable drawable, GLXContext context)
{
    if (wxGLCanvas::GetGLXVersion() >= 13)
        glXMakeContextCurrent( wxGetX11Display(), drawable, drawable, context);
    else // GLX <= 1.2 doesn't have glXMakeContextCurrent()
        glXMakeCurrent( wxGetX11Display(), drawable, context);
}

// ============================================================================
// wxGLCanvasX11 implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization methods and dtor
// ----------------------------------------------------------------------------

wxGLCanvasX11::wxGLCanvasX11()
{
    m_fbc = NULL;
    m_vi = NULL;
}

bool wxGLCanvasX11::InitVisual(const int *attribList)
{
    return InitXVisualInfo(attribList, &m_fbc, &m_vi);
}

wxGLCanvasX11::~wxGLCanvasX11()
{
    if ( m_fbc && m_fbc != ms_glFBCInfo )
        XFree(m_fbc);

    if ( m_vi && m_vi != ms_glVisualInfo )
        XFree(m_vi);
}

// ----------------------------------------------------------------------------
// working with GL attributes
// ----------------------------------------------------------------------------

bool
wxGLCanvasX11::ConvertWXAttrsToGL(const int *wxattrs, int *glattrs, size_t n)
{
    wxCHECK_MSG( n >= 16, false, _T("GL attributes buffer too small") );

    if ( !wxattrs )
    {
        if ( GetGLXVersion() >= 13 )
        {
            // leave GLX >= 1.3 choose the default attributes
            glattrs[0] = None;
        }
        else // GLX < 1.3
        {
            // default settings if attriblist = 0
            size_t i = 0;
            glattrs[i++] = GLX_RGBA;
            glattrs[i++] = GLX_DOUBLEBUFFER;
            glattrs[i++] = GLX_DEPTH_SIZE;   glattrs[i++] = 1;
            glattrs[i++] = GLX_RED_SIZE;     glattrs[i++] = 1;
            glattrs[i++] = GLX_GREEN_SIZE;   glattrs[i++] = 1;
            glattrs[i++] = GLX_BLUE_SIZE;    glattrs[i++] = 1;
            glattrs[i++] = GLX_ALPHA_SIZE;   glattrs[i++] = 0;
            glattrs[i++] = None;

            wxASSERT_MSG( i < n, _T("GL attributes buffer too small") );
        }
    }
    else // have non-default attributes
    {
        size_t p = 0;
        for ( int arg = 0; wxattrs[arg] != 0; )
        {
            // check if we have any space left, knowing that we may insert 2
            // more elements during this loop iteration and we always need to
            // terminate the list with None (hence -3)
            if ( p >= n - 2 )
                return false;

            // notice that for boolean attributes we use "continue" in the
            // switch to skip the assignment of the attribute value at the end
            // of the loop which is done for integer attributes
            switch ( wxattrs[arg++] )
            {
                case WX_GL_RGBA:
                    // for GLX >= 1.3, GLX_RGBA is useless and apparently
                    // harmful for some implementations
                    //
                    // FIXME: is this true?
                    if ( GetGLXVersion() <= 12 )
                    {
                        glattrs[p++] = GLX_RGBA;
                    }
                    continue;

                case WX_GL_BUFFER_SIZE:
                    glattrs[p++] = GLX_BUFFER_SIZE;
                    break;

                case WX_GL_LEVEL:
                    glattrs[p++] = GLX_LEVEL;
                    break;

                case WX_GL_DOUBLEBUFFER:
                    glattrs[p++] = GLX_DOUBLEBUFFER;
                    continue;

                case WX_GL_STEREO:
                    glattrs[p++] = GLX_STEREO;
                    break;

                case WX_GL_AUX_BUFFERS:
                    glattrs[p++] = GLX_AUX_BUFFERS;
                    break;

                case WX_GL_MIN_RED:
                    glattrs[p++] = GLX_RED_SIZE;
                    break;

                case WX_GL_MIN_GREEN:
                    glattrs[p++] = GLX_GREEN_SIZE;
                    break;

                case WX_GL_MIN_BLUE:
                    glattrs[p++] = GLX_BLUE_SIZE;
                    break;

                case WX_GL_MIN_ALPHA:
                    glattrs[p++] = GLX_ALPHA_SIZE;
                    break;

                case WX_GL_DEPTH_SIZE:
                    glattrs[p++] = GLX_DEPTH_SIZE;
                    break;

                case WX_GL_STENCIL_SIZE:
                    glattrs[p++] = GLX_STENCIL_SIZE;
                    break;

                case WX_GL_MIN_ACCUM_RED:
                    glattrs[p++] = GLX_ACCUM_RED_SIZE;
                    break;

                case WX_GL_MIN_ACCUM_GREEN:
                    glattrs[p++] = GLX_ACCUM_GREEN_SIZE;
                    break;

                case WX_GL_MIN_ACCUM_BLUE:
                    glattrs[p++] = GLX_ACCUM_BLUE_SIZE;
                    break;

                case WX_GL_MIN_ACCUM_ALPHA:
                    glattrs[p++] = GLX_ACCUM_ALPHA_SIZE;
                    break;

                default:
                    wxLogDebug(_T("Unsupported OpenGL attribute %d"),
                               wxattrs[arg - 1]);
                    continue;
            }

            // copy attribute value as is
            glattrs[p++] = wxattrs[arg++];
        }

        glattrs[p] = None;
    }

    return true;
}

/* static */
bool
wxGLCanvasX11::InitXVisualInfo(const int *attribList,
                               GLXFBConfig **pFBC,
                               XVisualInfo **pXVisual)
{
    int data[512];
    if ( !ConvertWXAttrsToGL(attribList, data, WXSIZEOF(data)) )
        return false;

    Display * const dpy = wxGetX11Display();

    if ( GetGLXVersion() >= 13 )
    {
        int returned;
        *pFBC = glXChooseFBConfig(dpy, DefaultScreen(dpy), data, &returned);

        if ( *pFBC )
        {
            *pXVisual = glXGetVisualFromFBConfig(wxGetX11Display(), **pFBC);
            if ( !*pXVisual )
            {
                XFree(*pFBC);
                *pFBC = NULL;
            }
        }
    }
    else // GLX <= 1.2
    {
        *pFBC = NULL;
        *pXVisual = glXChooseVisual(dpy, DefaultScreen(dpy), data);
    }

    return *pXVisual != NULL;
}

// ----------------------------------------------------------------------------
// default visual management
// ----------------------------------------------------------------------------

XVisualInfo *wxGLCanvasX11::ms_glVisualInfo = NULL;
GLXFBConfig *wxGLCanvasX11::ms_glFBCInfo = NULL;

/* static */
bool wxGLCanvasX11::InitDefaultVisualInfo(const int *attribList)
{
    FreeDefaultVisualInfo();

    return InitXVisualInfo(attribList, &ms_glFBCInfo, &ms_glVisualInfo);
}

/* static */
void wxGLCanvasX11::FreeDefaultVisualInfo()
{
    if ( ms_glFBCInfo )
    {
        XFree(ms_glFBCInfo);
        ms_glFBCInfo = NULL;
    }

    if ( ms_glVisualInfo )
    {
        XFree(ms_glVisualInfo);
        ms_glVisualInfo = NULL;
    }
}

// ----------------------------------------------------------------------------
// other GL methods
// ----------------------------------------------------------------------------

/* static */
int wxGLCanvasX11::GetGLXVersion()
{
    static int s_glxVersion = 0;
    if ( s_glxVersion == 0 )
    {
        // check the GLX version
        int glxMajorVer, glxMinorVer;
        bool ok = glXQueryVersion(wxGetX11Display(), &glxMajorVer, &glxMinorVer);
        wxASSERT_MSG( ok, _T("GLX version not found") );
        if (!ok)
            s_glxVersion = 10; // 1.0 by default
        else
            s_glxVersion = glxMajorVer*10 + glxMinorVer;
    }

    return s_glxVersion;
}

void wxGLCanvasX11::SwapBuffers()
{
    const Window xid = GetXWindow();
    wxCHECK_RET( xid, _T("window must be shown") );

    glXSwapBuffers(wxGetX11Display(), xid);
}

bool wxGLCanvasX11::IsShownOnScreen() const
{
    return GetXWindow() && wxGLCanvasBase::IsShownOnScreen();
}

#endif // wxUSE_GLCANVAS

