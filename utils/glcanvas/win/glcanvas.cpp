/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows under MS Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "glcanvas.h"
#endif

#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/frame.h>
#endif

#include <wx/msw/private.h>

#include "glcanvas.h"

wxChar wxGLCanvasClassName[]        = wxT("wxGLCanvasClass");

LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);

/*
 * GLContext implementation
 */

wxGLContext::wxGLContext(bool isRGB, wxGLCanvas *win, const wxPalette& palette)
{
  m_window = win;

  m_hDC = win->GetHDC();

  m_glContext = wglCreateContext((HDC) m_hDC);
  wxCHECK_RET( m_glContext, "Couldn't create OpenGl context" );

  wglMakeCurrent((HDC) m_hDC, m_glContext);
}

wxGLContext::wxGLContext(
               bool isRGB, wxGLCanvas *win,
               const wxPalette& palette,
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;

    m_hDC = win->GetHDC();

    m_glContext = wglCreateContext((HDC) m_hDC);
    wxCHECK_RET( m_glContext, "Couldn't create OpenGl context" );

    if( other != 0 )
      wglShareLists( other->m_glContext, m_glContext );

    wglMakeCurrent((HDC) m_hDC, m_glContext);
}

wxGLContext::~wxGLContext()
{
  if (m_glContext)
  {
    wglMakeCurrent(NULL, NULL);
  	wglDeleteContext(m_glContext);
  }
}

void wxGLContext::SwapBuffers()
{
  if (m_glContext)
  {
    wglMakeCurrent((HDC) m_hDC, m_glContext);
    ::SwapBuffers((HDC) m_hDC);    //blits the backbuffer into DC
  }
}

void wxGLContext::SetCurrent()
{
  if (m_glContext)
  {
    wglMakeCurrent((HDC) m_hDC, m_glContext);
  }

/*
	setupPixelFormat(hDC);
	setupPalette(hDC);
*/
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


/*
 * wxGLCanvas implementation
 */

IMPLEMENT_CLASS(wxGLCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxScrolledWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
    EVT_PALETTE_CHANGED(wxGLCanvas::OnPaletteChanged)
    EVT_QUERY_NEW_PALETTE(wxGLCanvas::OnQueryNewPalette)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name,
    int *attribList /* not used yet! */, const wxPalette& palette):
  wxScrolledWindow(parent, id, pos, size, style, name)
{
  m_hDC = (WXHDC) ::GetDC((HWND) GetHWND());

  SetupPixelFormat();
  SetupPalette(palette);

  m_glContext = new wxGLContext(TRUE, this, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
              const wxGLContext *shared, wxWindowID id,
              const wxPoint& pos, const wxSize& size, long style, const wxString& name,
              int *attribList, const wxPalette& palette )
  : wxScrolledWindow()
//  : wxScrolledWindow(parent, id, pos, size, style, name)
{
    bool ret = Create(parent, id, pos, size, style, name);

    if ( ret )
    {
        SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
        SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
    }

    m_hDC = (WXHDC) ::GetDC((HWND) GetHWND());

    SetupPixelFormat();
    SetupPalette(palette);

    m_glContext = new wxGLContext(TRUE, this, palette, shared );
}

wxGLCanvas::~wxGLCanvas()
{
  if (m_glContext)
    delete m_glContext;

  ::ReleaseDC((HWND) GetHWND(), (HDC) m_hDC);
}

// Replaces wxWindow::Create functionality, since we need to use a different window class
bool wxGLCanvas::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    static bool registeredGLCanvasClass = FALSE;

    // We have to register a special window class because we need
    // the CS_OWNDC style for GLCanvas.

/*
    From Angel Popov <jumpo@bitex.com>

    Here are two snips from a dicussion in the OpenGL Gamedev list that explains
    how this problem can be fixed:

    "There are 5 common DCs available in Win95. These are aquired when you call
    GetDC or GetDCEx from a window that does _not_ have the OWNDC flag.
    OWNDC flagged windows do not get their DC from the common DC pool, the issue
    is they require 800 bytes each from the limited 64Kb local heap for GDI."

    "The deal is, if you hold onto one of the 5 shared DC's too long (as GL apps
    do), Win95 will actually "steal" it from you.  MakeCurrent fails,
    apparently, because Windows re-assigns the HDC to a different window.  The
    only way to prevent this, the only reliable means, is to set CS_OWNDC."
*/

    if (!registeredGLCanvasClass)
    {
        WNDCLASS wndclass;

        static const long styleNormal = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;

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
        wndclass.style         = styleNormal;

        if ( !RegisterClass(&wndclass) )
        {
            wxLogLastError("RegisterClass(wxGLCanvasClass)");

            return FALSE;
        }
        registeredGLCanvasClass = TRUE;
    }

    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    parent->AddChild(this);

    DWORD msflags = 0;
    if ( style & wxBORDER )
        msflags |= WS_BORDER;
    if ( style & wxTHICK_FRAME )
        msflags |= WS_THICKFRAME;

    msflags |= WS_CHILD | WS_VISIBLE;
    if ( style & wxCLIP_CHILDREN )
        msflags |= WS_CLIPCHILDREN;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D);

    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    if ( want3D || (m_windowStyle & wxSIMPLE_BORDER) || (m_windowStyle & wxRAISED_BORDER ) ||
        (m_windowStyle & wxSUNKEN_BORDER) || (m_windowStyle & wxDOUBLE_BORDER))
    {
        msflags |= WS_BORDER;
    }

    // calculate the value to return from WM_GETDLGCODE handler
    if ( GetWindowStyleFlag() & wxWANTS_CHARS )
    {
        // want everything: i.e. all keys and WM_CHAR message
        m_lDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS |
                     DLGC_WANTTAB | DLGC_WANTMESSAGE;
    }

    MSWCreate(m_windowId, parent, wxGLCanvasClassName, this, NULL,
              pos.x, pos.y,
              WidthDefault(size.x), HeightDefault(size.y),
              msflags, NULL, exStyle);

    return TRUE;

}

void wxGLCanvas::SetupPixelFormat() // (HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),	/* size */
	1,				/* version */
	PFD_SUPPORT_OPENGL |
	PFD_DRAW_TO_WINDOW |
	PFD_DOUBLEBUFFER,		/* support double-buffering */
	PFD_TYPE_RGBA,			/* color type */
	16,				/* prefered color depth */
	0, 0, 0, 0, 0, 0,		/* color bits (ignored) */
	0,				/* no alpha buffer */
	0,				/* alpha bits (ignored) */
	0,				/* no accumulation buffer */
	0, 0, 0, 0,			/* accum bits (ignored) */
	16,				/* depth buffer */
	0,				/* no stencil buffer */
	0,				/* no auxiliary buffers */
	PFD_MAIN_PLANE,			/* main layer */
	0,				/* reserved */
	0, 0, 0,			/* no layer, visible, damage masks */
    };
    int pixelFormat;

    pixelFormat = ChoosePixelFormat((HDC) m_hDC, &pfd);
    if (pixelFormat == 0) {
	MessageBox(WindowFromDC((HDC) m_hDC), "ChoosePixelFormat failed.", "Error",
		MB_ICONERROR | MB_OK);
	exit(1);
    }

    if (SetPixelFormat((HDC) m_hDC, pixelFormat, &pfd) != TRUE) {
	MessageBox(WindowFromDC((HDC) m_hDC), "SetPixelFormat failed.", "Error",
		MB_ICONERROR | MB_OK);
	exit(1);
    }
}

void wxGLCanvas::SetupPalette(const wxPalette& palette)
{
    int pixelFormat = GetPixelFormat((HDC) m_hDC);
    PIXELFORMATDESCRIPTOR pfd;

    DescribePixelFormat((HDC) m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
    }
    else
    {
	  return;
    }

    m_palette = palette;

    if ( !m_palette.Ok() )
    {
        m_palette = CreateDefaultPalette();
    }

    if (m_palette.Ok())
    {
        SelectPalette((HDC) m_hDC, (HPALETTE) m_palette.GetHPALETTE(), FALSE);
        RealizePalette((HDC) m_hDC);
    }
}

wxPalette wxGLCanvas::CreateDefaultPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    int paletteSize;
    int pixelFormat = GetPixelFormat((HDC) m_hDC);

    DescribePixelFormat((HDC) m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	paletteSize = 1 << pfd.cColorBits;

    LOGPALETTE* pPal =
     (LOGPALETTE*) malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = paletteSize;

    /* build a simple RGB color palette */
    {
	int redMask = (1 << pfd.cRedBits) - 1;
	int greenMask = (1 << pfd.cGreenBits) - 1;
	int blueMask = (1 << pfd.cBlueBits) - 1;
	int i;

	for (i=0; i<paletteSize; ++i) {
	    pPal->palPalEntry[i].peRed =
		    (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
	    pPal->palPalEntry[i].peGreen =
		    (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
	    pPal->palPalEntry[i].peBlue =
		    (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
	    pPal->palPalEntry[i].peFlags = 0;
	}
    }

    HPALETTE hPalette = CreatePalette(pPal);
    free(pPal);

    wxPalette palette;
    palette.SetHPALETTE((WXHPALETTE) hPalette);

    return palette;
}

void wxGLCanvas::SwapBuffers()
{
  if (m_glContext)
    m_glContext->SwapBuffers();
}

void wxGLCanvas::OnSize(wxSizeEvent& event)
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
  if (m_glContext)
  {
    m_glContext->SetCurrent();
  }
}

void wxGLCanvas::SetColour(const char *colour)
{
  if (m_glContext)
    m_glContext->SetColour(colour);
}

// TODO: Have to have this called by parent frame (?)
// So we need wxFrame to call OnQueryNewPalette for all children...
void wxGLCanvas::OnQueryNewPalette(wxQueryNewPaletteEvent& event)
{
	/* realize palette if this is the current window */
	if ( GetPalette()->Ok() ) {
	    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
	    ::SelectPalette((HDC) GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
	    ::RealizePalette((HDC) GetHDC());
	    Refresh();
	    event.SetPaletteRealized(TRUE);
    }
    else
	    event.SetPaletteRealized(FALSE);
}

// I think this doesn't have to be propagated to child windows.
void wxGLCanvas::OnPaletteChanged(wxPaletteChangedEvent& event)
{
	/* realize palette if this is *not* the current window */
	if ( GetPalette() &&
       GetPalette()->Ok() && (this != event.GetChangedWindow()) )
    {
	    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
	    ::SelectPalette((HDC) GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
	    ::RealizePalette((HDC) GetHDC());
	    Refresh();
	}
}

/* Give extensions proper function names. */

/* EXT_vertex_array */
void glArrayElementEXT(GLint i)
{
}

void glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glDrawArraysEXT(GLenum mode, GLint first, GLsizei count)
{
#ifdef GL_EXT_vertex_array
    static PFNGLDRAWARRAYSEXTPROC proc = 0;

    if ( !proc )
    {
        proc = (PFNGLDRAWARRAYSEXTPROC) wglGetProcAddress("glDrawArraysEXT");
    }

    if ( proc )
        (* proc) (mode, first, count);
#endif
}

void glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *pointer)
{
}

void glGetPointervEXT(GLenum pname, GLvoid* *params)
{
}

void glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
#ifdef GL_EXT_vertex_array
    static PFNGLNORMALPOINTEREXTPROC proc = 0;

    if ( !proc )
    {
        proc = (PFNGLNORMALPOINTEREXTPROC) wglGetProcAddress("glNormalPointerEXT");
    }

    if ( proc )
        (* proc) (type, stride, count, pointer);
#endif
}

void glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
#ifdef GL_EXT_vertex_array
    static PFNGLVERTEXPOINTEREXTPROC proc = 0;

    if ( !proc )
    {
        proc = (PFNGLVERTEXPOINTEREXTPROC) wglGetProcAddress("glVertexPointerEXT");
    }

    if ( proc )
        (* proc) (size, type, stride, count, pointer);
#endif
}

/* EXT_color_subtable */
void glColorSubtableEXT(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *table)
{
}

/* EXT_color_table */
void glColorTableEXT(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
{
}

void glCopyColorTableEXT(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
}

void glGetColorTableEXT(GLenum target, GLenum format, GLenum type, GLvoid *table)
{
}

void glGetColorTableParamaterfvEXT(GLenum target, GLenum pname, GLfloat *params)
{
}

void glGetColorTavleParameterivEXT(GLenum target, GLenum pname, GLint *params)
{
}

/* SGI_compiled_vertex_array */
void glLockArraysSGI(GLint first, GLsizei count)
{
}

void glUnlockArraysSGI()
{
}


/* SGI_cull_vertex */
void glCullParameterdvSGI(GLenum pname, GLdouble* params)
{
}

void glCullParameterfvSGI(GLenum pname, GLfloat* params)
{
}

/* SGI_index_func */
void glIndexFuncSGI(GLenum func, GLclampf ref)
{
}

/* SGI_index_material */
void glIndexMaterialSGI(GLenum face, GLenum mode)
{
}

/* WIN_swap_hint */
void glAddSwapHintRectWin(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

