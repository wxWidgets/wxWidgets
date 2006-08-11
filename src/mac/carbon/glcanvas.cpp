/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxGL")

#include "wx/mac/private.h"

/*
* GLContext implementation
*/

wxGLContext::wxGLContext(
                         AGLPixelFormat fmt, wxGLCanvas *win,
                         const wxPalette& palette,
                         const wxGLContext *other        /* for sharing display lists */
                         )
{
    m_window = win;

    m_drawable = (AGLDrawable) UMAGetWindowPort(MAC_WXHWND(win->MacGetTopLevelWindowRef()));

    m_glContext = aglCreateContext(fmt, other ? other->m_glContext : NULL);
    wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGl context") );

    GLboolean b;
    b = aglSetDrawable(m_glContext, m_drawable);
    wxCHECK_RET( b, wxT("Couldn't bind OpenGl context") );
    aglEnable(m_glContext , AGL_BUFFER_RECT ) ;
    b = aglSetCurrentContext(m_glContext);
    wxCHECK_RET( b, wxT("Couldn't activate OpenGl context") );
}

wxGLContext::~wxGLContext()
{
    if (m_glContext)
    {
        aglSetCurrentContext(NULL);
        aglDestroyContext(m_glContext);
    }
}

void wxGLContext::SwapBuffers()
{
    if (m_glContext)
    {
        aglSwapBuffers(m_glContext);
    }
}

void wxGLContext::SetCurrent()
{
    if (m_glContext)
    {
        aglSetCurrentContext(m_glContext);
    }
}

void wxGLContext::Update()
{
    if (m_glContext)
    {
        aglUpdateContext(m_glContext);
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


/*
* wxGLCanvas implementation
*/

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                       int *attribList, const wxPalette& palette)
{
    Create(parent, NULL, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
                       const wxGLContext *shared, wxWindowID id,
                       const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                       int *attribList, const wxPalette& palette )
{
    Create(parent, shared, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared, wxWindowID id,
                       const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                       int *attribList, const wxPalette& palette )
{
    Create(parent, shared ? shared->GetContext() : NULL, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::~wxGLCanvas()
{
    if (m_glContext != NULL) {
        delete m_glContext;
        m_glContext = NULL;
    }
}

static AGLPixelFormat ChoosePixelFormat(const int *attribList)
{
    GLint data[512];
    GLint defaultAttribs[] = { AGL_RGBA,
        AGL_DOUBLEBUFFER,
        AGL_MINIMUM_POLICY,
        AGL_DEPTH_SIZE, 1,  // use largest available depth buffer
        AGL_RED_SIZE, 1,
        AGL_GREEN_SIZE, 1,
        AGL_BLUE_SIZE, 1,
        AGL_ALPHA_SIZE, 0,
        AGL_NONE };
    GLint *attribs;
    if (!attribList)
    {
        attribs = defaultAttribs;
    }
    else
    {
        int arg=0, p=0;

        data[p++] = AGL_MINIMUM_POLICY; // make _SIZE tags behave more like GLX
        while( (attribList[arg]!=0) && (p<512) )
        {
            switch( attribList[arg++] )
            {
            case WX_GL_RGBA: data[p++] = AGL_RGBA; break;
            case WX_GL_BUFFER_SIZE:
                data[p++]=AGL_BUFFER_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_LEVEL:
                data[p++]=AGL_LEVEL; data[p++]=attribList[arg++]; break;
            case WX_GL_DOUBLEBUFFER: data[p++] = AGL_DOUBLEBUFFER; break;
            case WX_GL_STEREO: data[p++] = AGL_STEREO; break;
            case WX_GL_AUX_BUFFERS:
                data[p++]=AGL_AUX_BUFFERS; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_RED:
                data[p++]=AGL_RED_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_GREEN:
                data[p++]=AGL_GREEN_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_BLUE:
                data[p++]=AGL_BLUE_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_ALPHA:
                data[p++]=AGL_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_DEPTH_SIZE:
                data[p++]=AGL_DEPTH_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_STENCIL_SIZE:
                data[p++]=AGL_STENCIL_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_ACCUM_RED:
                data[p++]=AGL_ACCUM_RED_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_ACCUM_GREEN:
                data[p++]=AGL_ACCUM_GREEN_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_ACCUM_BLUE:
                data[p++]=AGL_ACCUM_BLUE_SIZE; data[p++]=attribList[arg++]; break;
            case WX_GL_MIN_ACCUM_ALPHA:
                data[p++]=AGL_ACCUM_ALPHA_SIZE; data[p++]=attribList[arg++]; break;
            default:
                break;
            }
        }
        data[p] = 0;

        attribs = data;
    }

    return aglChoosePixelFormat(NULL, 0, attribs);
}

bool wxGLCanvas::Create(wxWindow *parent, const wxGLContext *shared, wxWindowID id,
                        const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                        int *attribList, const wxPalette& palette)
{
    m_macCanvasIsShown = false ;
    m_glContext = 0 ;
    wxWindow::Create( parent, id, pos, size, style, name );

    AGLPixelFormat fmt = ChoosePixelFormat(attribList);
    wxCHECK_MSG( fmt, false, wxT("Couldn't create OpenGl pixel format") );

    m_glContext = new wxGLContext(fmt, this, palette, shared);
    m_macCanvasIsShown = true ;
    aglDestroyPixelFormat(fmt);

    return true;
}

void wxGLCanvas::SwapBuffers()
{
    if (m_glContext)
        m_glContext->SwapBuffers();
}

void wxGLCanvas::UpdateContext()
{
    if (m_glContext)
        m_glContext->Update();
}

void wxGLCanvas::SetViewport()
{
    // viewport is initially set to entire port
    // adjust glViewport to just this window
    int x = 0 ;
    int y = 0 ;

    wxWindow* iter = this ;
    while( iter->GetParent() )
    {
        iter = iter->GetParent() ;
    }

    if ( iter && iter->IsTopLevel() )
    {
        MacClientToRootWindow( &x , &y ) ;
        int width, height;
        GetClientSize(& width, & height);
        Rect bounds ;
#if 0
		// TODO in case we adopt point vs pixel coordinates, this will make the conversion
        GetWindowPortBounds( MAC_WXHWND(MacGetTopLevelWindowRef()) , &bounds ) ;
        HIRect hiRect = CGRectMake( x, y, width, height ) ;
        HIRectConvert( &hiRect, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceScreenPixel, NULL) ;
        HIRect hiBounds = CGRectMake( 0, 0, bounds.right - bounds.left , bounds.bottom - bounds.top ) ;
        HIRectConvert( &hiBounds, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceScreenPixel, NULL) ;
        GLint parms[4] ;
        parms[0] = hiRect.origin.x ;
        parms[1] = hiBounds.size.height - (hiRect.origin.y + hiRect.size.height) ;
        parms[2] = hiRect.size.width ;
        parms[3] = hiRect.size.height ;
#else
        GetWindowPortBounds( MAC_WXHWND(MacGetTopLevelWindowRef()) , &bounds ) ;
        GLint parms[4] ;
        parms[0] = x ;
        parms[1] = bounds.bottom - bounds.top - ( y + height ) ;
        parms[2] = width ;
        parms[3] = height ;
#endif
        if ( !m_macCanvasIsShown )
            parms[0] += 20000 ;
        aglSetInteger( m_glContext->m_glContext , AGL_BUFFER_RECT , parms ) ;
   }
}

void wxGLCanvas::OnSize(wxSizeEvent& event)
{
    MacUpdateView() ;
}

void wxGLCanvas::MacUpdateView()
{
    if (m_glContext)
    {
        UpdateContext();
        m_glContext->SetCurrent();
        SetViewport();
    }
}

void wxGLCanvas::MacSuperChangedPosition()
{
    MacUpdateView() ;
    wxWindow::MacSuperChangedPosition() ;
}

void wxGLCanvas::MacTopLevelWindowChangedPosition()
{
    MacUpdateView() ;
    wxWindow::MacTopLevelWindowChangedPosition() ;
}

void wxGLCanvas::SetCurrent()
{
    if (m_glContext)
    {
        m_glContext->SetCurrent();
    }
}

void wxGLCanvas::SetColour(const wxChar *colour)
{
    if (m_glContext)
        m_glContext->SetColour(colour);
}

bool wxGLCanvas::Show(bool show)
{
    if ( !wxWindow::Show( show ) )
        return false ;
/*
    if ( !show )
    {
        if ( m_macCanvasIsShown )
        {
            m_macCanvasIsShown = false ;
            SetViewport() ;
        }
    }
    else
    {
        if ( m_peer->IsVisible()&& !m_macCanvasIsShown )
        {
            m_macCanvasIsShown = true ;
            SetViewport() ;
        }
    }
*/
    return true ;
}

void wxGLCanvas::MacVisibilityChanged()
{
    if ( !MacIsReallyShown() )
    {
        if ( m_macCanvasIsShown )
        {
            m_macCanvasIsShown = false ;
            SetViewport() ;
        }
    }
    else
    {
        if ( !m_macCanvasIsShown )
        {
            m_macCanvasIsShown = true ;
            SetViewport() ;
        }
    }
    wxWindowMac::MacVisibilityChanged() ;
}

//---------------------------------------------------------------------------
// wxGLApp
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLApp, wxApp)

bool wxGLApp::InitGLVisual(int *attribList)
{
    AGLPixelFormat fmt = ChoosePixelFormat(attribList);
    if (fmt != NULL) {
        aglDestroyPixelFormat(fmt);
        return true;
    } else
        return false;
}

wxGLApp::~wxGLApp(void)
{
}

#endif // wxUSE_GLCANVAS
