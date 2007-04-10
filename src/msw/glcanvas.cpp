/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under MS Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/module.h"
#endif

#include "wx/msw/private.h"

#include "wx/glcanvas.h"

// from src/msw/window.cpp
LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);

#ifdef GL_EXT_vertex_array
    #define WXUNUSED_WITHOUT_GL_EXT_vertex_array(name) name
#else
    #define WXUNUSED_WITHOUT_GL_EXT_vertex_array(name) WXUNUSED(name)
#endif

// ----------------------------------------------------------------------------
// libraries
// ----------------------------------------------------------------------------

/*
  The following two compiler directives are specific to the Microsoft Visual
  C++ family of compilers

  Fundementally what they do is instruct the linker to use these two libraries
  for the resolution of symbols. In essence, this is the equivalent of adding
  these two libraries to either the Makefile or project file.

  This is NOT a recommended technique, and certainly is unlikely to be used
  anywhere else in wxWidgets given it is so specific to not only wxMSW, but
  also the VC compiler. However, in the case of opengl support, it's an
  applicable technique as opengl is optional in setup.h This code (wrapped by
  wxUSE_GLCANVAS), now allows opengl support to be added purely by modifying
  setup.h rather than by having to modify either the project or DSP fle.

  See MSDN for further information on the exact usage of these commands.
*/
#ifdef _MSC_VER
#  pragma comment( lib, "opengl32" )
#  pragma comment( lib, "glu32" )
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const wxChar *wxGLCanvasClassName = wxT("wxGLCanvasClass");
static const wxChar *wxGLCanvasClassNameNoRedraw = wxT("wxGLCanvasClassNR");

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGLModule is responsible for unregistering wxGLCanvasClass Windows class
// ----------------------------------------------------------------------------

class wxGLModule : public wxModule
{
public:
    bool OnInit() { return true; }
    void OnExit() { UnregisterClasses(); }

    // register the GL classes if not done yet, return true if ok, false if
    // registration failed
    static bool RegisterClasses();

    // unregister the classes, done automatically on program termination
    static void UnregisterClasses();

private:
    // wxGLCanvas is only used from the main thread so this is MT-ok
    static bool ms_registeredGLClasses;

    DECLARE_DYNAMIC_CLASS(wxGLModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxGLModule, wxModule)

bool wxGLModule::ms_registeredGLClasses = false;

/* static */
bool wxGLModule::RegisterClasses()
{
    if ( ms_registeredGLClasses )
        return true;

    // We have to register a special window class because we need the CS_OWNDC
    // style for GLCanvas: some OpenGL drivers are buggy and don't work with
    // windows without this style
    WNDCLASS wndclass;

    // the fields which are common to all classes
    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof( DWORD ); // VZ: what is this DWORD used for?
    wndclass.hInstance     = wxhInstance;
    wndclass.hIcon         = (HICON) NULL;
    wndclass.hCursor       = ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wndclass.lpszMenuName  = NULL;

    // Register the GLCanvas class name
    wndclass.hbrBackground = (HBRUSH)NULL;
    wndclass.lpszClassName = wxGLCanvasClassName;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;

    if ( !::RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(wxGLCanvasClass)"));
        return false;
    }

    // Register the GLCanvas class name for windows which don't do full repaint
    // on resize
    wndclass.lpszClassName = wxGLCanvasClassNameNoRedraw;
    wndclass.style        &= ~(CS_HREDRAW | CS_VREDRAW);

    if ( !::RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(wxGLCanvasClassNameNoRedraw)"));

        ::UnregisterClass(wxGLCanvasClassName, wxhInstance);

        return false;
    }

    ms_registeredGLClasses = true;

    return true;
}

/* static */
void wxGLModule::UnregisterClasses()
{
    // we need to unregister the classes in case we're in a DLL which is
    // unloaded and then loaded again because if we don't, the registration is
    // going to fail in wxGLCanvas::Create() the next time we're loaded
    if ( ms_registeredGLClasses )
    {
        ::UnregisterClass(wxGLCanvasClassName, wxhInstance);
        ::UnregisterClass(wxGLCanvasClassNameNoRedraw, wxhInstance);

        ms_registeredGLClasses = false;
    }
}

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext, wxObject)

wxGLContext::wxGLContext(wxGLCanvas *win, const wxGLContext* other)
{
    m_glContext = wglCreateContext(win->GetHDC());
    wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGL context") );

    if ( other )
    {
        if ( !wglShareLists(other->m_glContext, m_glContext) )
            wxLogLastError(_T("wglShareLists"));
    }
}

wxGLContext::~wxGLContext()
{
    // note that it's ok to delete the context even if it's the current one
    wglDeleteContext(m_glContext);
}

void wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !wglMakeCurrent(win.GetHDC(), m_glContext) )
    {
        wxLogLastError(_T("wglMakeCurrent"));
    }
}

// ============================================================================
// wxGLCanvas
// ============================================================================

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_PALETTE_CHANGED(wxGLCanvas::OnPaletteChanged)
    EVT_QUERY_NEW_PALETTE(wxGLCanvas::OnQueryNewPalette)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxGLCanvas construction
// ----------------------------------------------------------------------------

void wxGLCanvas::Init()
{
#if WXWIN_COMPATIBILITY_2_8
    m_glContext = NULL;
#endif
    m_hDC = NULL;
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Init();

    (void)Create(parent, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::~wxGLCanvas()
{
#if WXWIN_COMPATIBILITY_2_8
    delete m_glContext;
#endif

    ::ReleaseDC(GetHwnd(), m_hDC);
}

// Replaces wxWindow::Create functionality, since we need to use a different
// window class
bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
    wxCHECK_MSG( parent, false, wxT("can't create wxWindow without parent") );

    if ( !wxGLModule::RegisterClasses() )
    {
        wxLogError(_("Failed to register OpenGL window class."));

        return false;
    }

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    parent->AddChild(this);

    /*
       A general rule with OpenGL and Win32 is that any window that will have a
       HGLRC built for it must have two flags:  WS_CLIPCHILDREN & WS_CLIPSIBLINGS.
       You can find references about this within the knowledge base and most OpenGL
       books that contain the wgl function descriptions.
     */
    WXDWORD exStyle = 0;
    DWORD msflags = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    msflags |= MSWGetStyle(style, &exStyle);

    if ( !MSWCreate(wxGLCanvasClassName, NULL, pos, size, msflags, exStyle) )
        return false;

    m_hDC = ::GetDC(GetHwnd());
    if ( !m_hDC )
        return false;

    if ( !DoSetup(attribList) )
        return false;

#if wxUSE_PALETTE
    if ( !SetupPalette(palette) )
        return false;
#else // !wxUSE_PALETTE
    wxUnusedVar(palette);
#endif // wxUSE_PALETTE/!wxUSE_PALETTE

    return true;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void wxGLCanvas::SwapBuffers()
{
    if ( !::SwapBuffers(m_hDC) )
        wxLogLastError(_T("SwapBuffers"));
}

// ----------------------------------------------------------------------------
// pixel format stuff
// ----------------------------------------------------------------------------

static void
AdjustPFDForAttributes(PIXELFORMATDESCRIPTOR& pfd, const int *attribList)
{
    if ( !attribList )
        return;

    pfd.dwFlags &= ~PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_COLORINDEX;
    pfd.cColorBits = 0;
    int arg=0;

    while ( attribList[arg] )
    {
        switch ( attribList[arg++] )
        {
            case WX_GL_RGBA:
                pfd.iPixelType = PFD_TYPE_RGBA;
                break;
            case WX_GL_BUFFER_SIZE:
                pfd.cColorBits = attribList[arg++];
                break;
            case WX_GL_LEVEL:
                // this member looks like it may be obsolete
                if ( attribList[arg] > 0 )
                    pfd.iLayerType = PFD_OVERLAY_PLANE;
                else if ( attribList[arg] < 0 )
                    pfd.iLayerType = (BYTE)PFD_UNDERLAY_PLANE;
                else
                    pfd.iLayerType = PFD_MAIN_PLANE;
                arg++;
                break;
            case WX_GL_DOUBLEBUFFER:
                pfd.dwFlags |= PFD_DOUBLEBUFFER;
                break;
            case WX_GL_STEREO:
                pfd.dwFlags |= PFD_STEREO;
                break;
            case WX_GL_AUX_BUFFERS:
                pfd.cAuxBuffers = attribList[arg++];
                break;
            case WX_GL_MIN_RED:
                pfd.cColorBits = (pfd.cColorBits +
                        (pfd.cRedBits = attribList[arg++]));
                break;
            case WX_GL_MIN_GREEN:
                pfd.cColorBits = (pfd.cColorBits +
                        (pfd.cGreenBits = attribList[arg++]));
                break;
            case WX_GL_MIN_BLUE:
                pfd.cColorBits = (pfd.cColorBits +
                        (pfd.cBlueBits = attribList[arg++]));
                break;
            case WX_GL_MIN_ALPHA:
                // doesn't count in cColorBits
                pfd.cAlphaBits = attribList[arg++];
                break;
            case WX_GL_DEPTH_SIZE:
                pfd.cDepthBits = attribList[arg++];
                break;
            case WX_GL_STENCIL_SIZE:
                pfd.cStencilBits = attribList[arg++];
                break;
            case WX_GL_MIN_ACCUM_RED:
                pfd.cAccumBits = (pfd.cAccumBits +
                        (pfd.cAccumRedBits = attribList[arg++]));
                break;
            case WX_GL_MIN_ACCUM_GREEN:
                pfd.cAccumBits = (pfd.cAccumBits +
                        (pfd.cAccumGreenBits = attribList[arg++]));
                break;
            case WX_GL_MIN_ACCUM_BLUE:
                pfd.cAccumBits = (pfd.cAccumBits +
                        (pfd.cAccumBlueBits = attribList[arg++]));
                break;
            case WX_GL_MIN_ACCUM_ALPHA:
                pfd.cAccumBits = (pfd.cAccumBits +
                        (pfd.cAccumAlphaBits = attribList[arg++]));
                break;
        }
    }
}

/* static */
int
wxGLCanvas::ChooseMatchingPixelFormat(HDC hdc,
                                      const int *attribList,
                                      PIXELFORMATDESCRIPTOR *ppfd)
{
    // default neutral pixel format
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // size
        1,                              // version
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,               // support double-buffering
        PFD_TYPE_RGBA,                  // color type
        16,                             // preferred color depth
        0, 0, 0, 0, 0, 0,               // color bits (ignored)
        0,                              // no alpha buffer
        0,                              // alpha bits (ignored)
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accumulator bits (ignored)
        16,                             // depth buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffers
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0,                        // no layer, visible, damage masks
    };

    if ( !ppfd )
        ppfd = &pfd;
    else
        *ppfd = pfd;

    AdjustPFDForAttributes(*ppfd, attribList);

    return ::ChoosePixelFormat(hdc, ppfd);
}

bool wxGLCanvas::DoSetup(const int *attribList)
{
    PIXELFORMATDESCRIPTOR pfd;
    const int pixelFormat = ChooseMatchingPixelFormat(m_hDC, attribList, &pfd);
    if ( !pixelFormat )
    {
        wxLogLastError(_T("ChoosePixelFormat"));
        return false;
    }

    if ( !::SetPixelFormat(m_hDC, pixelFormat, &pfd) )
    {
        wxLogLastError(_T("SetPixelFormat"));
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// palette stuff
// ----------------------------------------------------------------------------

#if wxUSE_PALETTE

bool wxGLCanvas::SetupPalette(const wxPalette& palette)
{
    const int pixelFormat = ::GetPixelFormat(m_hDC);
    if ( !pixelFormat )
    {
        wxLogLastError(_T("GetPixelFormat"));
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if ( !::DescribePixelFormat(m_hDC, pixelFormat, sizeof(pfd), &pfd) )
    {
        wxLogLastError(_T("DescribePixelFormat"));
        return false;
    }

    if ( !(pfd.dwFlags & PFD_NEED_PALETTE) )
        return true;

    m_palette = palette;

    if ( !m_palette.Ok() )
    {
        m_palette = CreateDefaultPalette();
        if ( !m_palette.Ok() )
            return false;
    }

    if ( !::SelectPalette(m_hDC, GetHpaletteOf(m_palette), FALSE) )
    {
        wxLogLastError(_T("SelectPalette"));
        return false;
    }

    if ( ::RealizePalette(m_hDC) == GDI_ERROR )
    {
        wxLogLastError(_T("RealizePalette"));
        return false;
    }

    return true;
}

wxPalette wxGLCanvas::CreateDefaultPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    int paletteSize;
    int pixelFormat = GetPixelFormat(m_hDC);

    DescribePixelFormat(m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    paletteSize = 1 << pfd.cColorBits;

    LOGPALETTE* pPal =
     (LOGPALETTE*) malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = (WORD)paletteSize;

    /* build a simple RGB color palette */
    {
    int redMask = (1 << pfd.cRedBits) - 1;
    int greenMask = (1 << pfd.cGreenBits) - 1;
    int blueMask = (1 << pfd.cBlueBits) - 1;
    int i;

    for (i=0; i<paletteSize; ++i) {
        pPal->palPalEntry[i].peRed =
            (BYTE)((((i >> pfd.cRedShift) & redMask) * 255) / redMask);
        pPal->palPalEntry[i].peGreen =
            (BYTE)((((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask);
        pPal->palPalEntry[i].peBlue =
            (BYTE)((((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask);
        pPal->palPalEntry[i].peFlags = 0;
    }
    }

    HPALETTE hPalette = CreatePalette(pPal);
    free(pPal);

    wxPalette palette;
    palette.SetHPALETTE((WXHPALETTE) hPalette);

    return palette;
}

void wxGLCanvas::OnQueryNewPalette(wxQueryNewPaletteEvent& event)
{
  /* realize palette if this is the current window */
  if ( GetPalette()->Ok() ) {
    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
    ::SelectPalette(GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
    ::RealizePalette(GetHDC());
    Refresh();
    event.SetPaletteRealized(true);
  }
  else
    event.SetPaletteRealized(false);
}

void wxGLCanvas::OnPaletteChanged(wxPaletteChangedEvent& event)
{
  /* realize palette if this is *not* the current window */
  if ( GetPalette() &&
       GetPalette()->Ok() && (this != event.GetChangedWindow()) )
  {
    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
    ::SelectPalette(GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
    ::RealizePalette(GetHDC());
    Refresh();
  }
}

#endif // wxUSE_PALETTE

// ----------------------------------------------------------------------------
// deprecated wxGLCanvas methods using implicit wxGLContext
// ----------------------------------------------------------------------------

// deprecated constructors creating an implicit m_glContext
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
    Init();

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
    Init();

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
    Init();

    if ( Create(parent, id, pos, size, style, name, attribList, palette) )
        m_glContext = new wxGLContext(this, shared ? shared->m_glContext : NULL);
}

#endif // WXWIN_COMPATIBILITY_2_8


// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    if ( !wxGLCanvas::ChooseMatchingPixelFormat(ScreenHDC(), attribList) )
    {
        wxLogError(_("Failed to initialize OpenGL"));
        return false;
    }

    return true;
}

#endif // wxUSE_GLCANVAS
