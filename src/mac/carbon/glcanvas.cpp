///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/mac/uma.h"

#include "wx/mac/private.h"

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

static void wxLogAGLError(const char *func)
{
    const int err = aglGetError();

    wxLogError(_("OpenGL function \"%s\" failed: %s (error %d)"),
               func, aglErrorString(err), err);
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

wxGLContext::wxGLContext(wxGLCanvas *win, const wxGLContext *other)
{
    m_aglContext = aglCreateContext(win->GetAGLPixelFormat(),
                                    other ? other->m_aglContext : NULL);
    if ( !m_aglContext )
        wxLogAGLError("aglCreateContext");

    return;
    if ( !aglEnable(m_aglContext, AGL_BUFFER_RECT) )
        wxLogAGLError("aglEnable(AGL_BUFFER_RECT)");
}

wxGLContext::~wxGLContext()
{
    if ( m_aglContext )
    {
        // it's ok to pass the current context to this function
        if ( !aglDestroyContext(m_aglContext) )
            wxLogAGLError("aglDestroyContext");
    }
}

void wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_aglContext )
        return;

    AGLDrawable drawable = (AGLDrawable)UMAGetWindowPort(
                                MAC_WXHWND(win.MacGetTopLevelWindowRef()));
    if ( !aglSetDrawable(m_aglContext, drawable) )
        wxLogAGLError("aglSetDrawable");

    if ( !aglSetCurrentContext(m_aglContext) )
        wxLogAGLError("aglSetCurrentContext");

    wx_const_cast(wxGLCanvas&, win).SetViewport();
}

// ----------------------------------------------------------------------------
// wxGLCanvas
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

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

#if WXWIN_COMPATIBILITY_2_8

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const int *attribList,
                       const wxPalette& palette)
{
    if ( Create(parent, id, pos, size, style, name, attribList, palette) )
        m_glContext = new wxGLContext(this);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLContext *shared,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const int *attribList,
                       const wxPalette& palette)
{
    if ( Create(parent, id, pos, size, style, name, attribList, palette) )
        m_glContext = new wxGLContext(this, shared);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLCanvas *shared,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const int *attribList,
                       const wxPalette& palette)
{
    if ( Create(parent, id, pos, size, style, name, attribList, palette) )
        m_glContext = new wxGLContext(this, shared ? shared->m_glContext : NULL);
}

#endif // WXWIN_COMPATIBILITY_2_8

static AGLPixelFormat ChoosePixelFormat(const int *attribList)
{
    GLint data[512];
    const GLint defaultAttribs[] =
    {
        AGL_RGBA,
        AGL_DOUBLEBUFFER,
        AGL_MINIMUM_POLICY, // never choose less than requested
        AGL_DEPTH_SIZE, 1,  // use largest available depth buffer
        AGL_RED_SIZE, 1,
        AGL_GREEN_SIZE, 1,
        AGL_BLUE_SIZE, 1,
        AGL_ALPHA_SIZE, 0,
        AGL_NONE
    };

    const GLint *attribs;
    if ( !attribList )
    {
        attribs = defaultAttribs;
    }
    else
    {
        unsigned p = 0;
        data[p++] = AGL_MINIMUM_POLICY; // make _SIZE tags behave more like GLX

        for ( unsigned arg = 0; attribList[arg] !=0 && p < WXSIZEOF(data); )
        {
            switch ( attribList[arg++] )
            {
                case WX_GL_RGBA:
                    data[p++] = AGL_RGBA;
                    break;

                case WX_GL_BUFFER_SIZE:
                    data[p++] = AGL_BUFFER_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_LEVEL:
                    data[p++]=AGL_LEVEL;
                    data[p++]=attribList[arg++];
                    break;

                case WX_GL_DOUBLEBUFFER:
                    data[p++] = AGL_DOUBLEBUFFER;
                    break;

                case WX_GL_STEREO:
                    data[p++] = AGL_STEREO;
                    break;

                case WX_GL_AUX_BUFFERS:
                    data[p++] = AGL_AUX_BUFFERS;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_RED:
                    data[p++] = AGL_RED_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_GREEN:
                    data[p++] = AGL_GREEN_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_BLUE:
                    data[p++] = AGL_BLUE_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ALPHA:
                    data[p++] = AGL_ALPHA_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_DEPTH_SIZE:
                    data[p++] = AGL_DEPTH_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_STENCIL_SIZE:
                    data[p++] = AGL_STENCIL_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_RED:
                    data[p++] = AGL_ACCUM_RED_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_GREEN:
                    data[p++] = AGL_ACCUM_GREEN_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_BLUE:
                    data[p++] = AGL_ACCUM_BLUE_SIZE;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_ALPHA:
                    data[p++] = AGL_ACCUM_ALPHA_SIZE;
                    data[p++] = attribList[arg++];
                    break;
            }
        }

        data[p] = AGL_NONE;

        attribs = data;
    }

    return aglChoosePixelFormat(NULL, 0, attribs);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& WXUNUSED(palette))
{
    m_needsUpdate = false;
    m_macCanvasIsShown = false;

    m_aglFormat = ChoosePixelFormat(attribList);
    if ( !m_aglFormat )
        return false;

    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    m_macCanvasIsShown = true;

    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    if ( m_aglFormat )
        aglDestroyPixelFormat(m_aglFormat);
}

void wxGLCanvas::SwapBuffers()
{
    AGLContext context = aglGetCurrentContext();
    wxCHECK_RET( context, _T("should have current context") );

    aglSwapBuffers(context);
}

void wxGLCanvas::SetViewport()
{
    if ( !m_needsUpdate )
        return;

    m_needsUpdate = false;

    AGLContext context = aglGetCurrentContext();
    if ( !context )
        return;

    // viewport is initially set to entire port, adjust it to just this window
    int x = 0,
        y = 0;
    MacClientToRootWindow(&x , &y);

    int width, height;
    GetClientSize(&width, &height);

    Rect bounds;
    GetWindowPortBounds(MAC_WXHWND(MacGetTopLevelWindowRef()) , &bounds);

#if 0
    // TODO in case we adopt point vs pixel coordinates, this will make the conversion
    HIRect hiRect = CGRectMake( x, y, width, height );
    HIRectConvert( &hiRect, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceScreenPixel, NULL);
    HIRect hiBounds = CGRectMake( 0, 0, bounds.right - bounds.left , bounds.bottom - bounds.top );
    HIRectConvert( &hiBounds, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceScreenPixel, NULL);
    GLint parms[4];
    parms[0] = hiRect.origin.x;
    parms[1] = hiBounds.size.height - (hiRect.origin.y + hiRect.size.height);
    parms[2] = hiRect.size.width;
    parms[3] = hiRect.size.height;
#else
    GLint parms[4];
    parms[0] = x;
    parms[1] = bounds.bottom - bounds.top - ( y + height );
    parms[2] = width;
    parms[3] = height;
#endif

    // move the buffer rect out of sight if we're hidden
    if ( !m_macCanvasIsShown )
        parms[0] += 20000;

    if ( !aglSetInteger(context, AGL_BUFFER_RECT, parms) )
        wxLogAGLError("aglSetInteger(AGL_BUFFER_RECT)");

    if ( !aglUpdateContext(context) )
        wxLogAGLError("aglUpdateContext");
}

void wxGLCanvas::OnSize(wxSizeEvent& event)
{
    MacUpdateView();
    event.Skip();
}

void wxGLCanvas::MacUpdateView()
{
    m_needsUpdate = true;
    Refresh(false);
}

void wxGLCanvas::MacSuperChangedPosition()
{
    MacUpdateView();
    wxWindow::MacSuperChangedPosition();
}

void wxGLCanvas::MacTopLevelWindowChangedPosition()
{
    MacUpdateView();
    wxWindow::MacTopLevelWindowChangedPosition();
}

void wxGLCanvas::MacVisibilityChanged()
{
    if ( MacIsReallyShown() != m_macCanvasIsShown )
    {
        m_macCanvasIsShown = !m_macCanvasIsShown;
        MacUpdateView();
    }

    wxWindowMac::MacVisibilityChanged();
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    AGLPixelFormat fmt = ChoosePixelFormat(attribList);
    if ( !fmt )
        return false;

    aglDestroyPixelFormat(fmt);
    return true;
}

#endif // wxUSE_GLCANVAS
