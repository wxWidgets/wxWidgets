/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#ifdef __WXOSX_IPHONE__
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#define wxUSE_OPENGL_EMULATION 1
#else
#ifndef GL_SILENCE_DEPRECATION
    #define GL_SILENCE_DEPRECATION
#endif
#include <OpenGL/gl.h>
#endif

#include "wx/vector.h"

// low level calls

WXDLLIMPEXP_GL WXGLContext WXGLCreateContext( WXGLPixelFormat pixelFormat, WXGLContext shareContext );
WXDLLIMPEXP_GL void WXGLDestroyContext( WXGLContext context );

WXDLLIMPEXP_GL WXGLContext WXGLGetCurrentContext();
WXDLLIMPEXP_GL bool WXGLSetCurrentContext(WXGLContext context);

WXDLLIMPEXP_GL WXGLPixelFormat WXGLChoosePixelFormat(const int *GLAttrs = nullptr,
                                                     int n1 = 0,
                                                     const int *ctxAttrs = nullptr,
                                                     int n2 = 0);
WXDLLIMPEXP_GL void WXGLDestroyPixelFormat( WXGLPixelFormat pixelFormat );

class WXDLLIMPEXP_GL wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win,
                const wxGLContext *other = nullptr,
                const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContext();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

    // Mac-specific
    WXGLContext GetWXGLContext() const { return m_glContext; }

private:
    WXGLContext m_glContext;

    wxDECLARE_NO_COPY_CLASS(wxGLContext);
};

class WXDLLIMPEXP_GL wxGLCanvas : public wxGLCanvasBase
{
public:
    wxGLCanvas(wxWindow *parent,
               const wxGLAttributes& dispAttrs,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = nullptr,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent,
                const wxGLAttributes& dispAttrs,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const int *attribList = nullptr,
                const wxPalette& palette = wxNullPalette);

    virtual ~wxGLCanvas();

    // implement wxGLCanvasBase methods
    virtual bool SwapBuffers() override;

    // Mac-specific functions
    // ----------------------

    // return true if multisample extension is supported
    static bool IsAGLMultiSampleAvailable();

    // return the pixel format used by this window
    WXGLPixelFormat GetWXGLPixelFormat() const { return m_glFormat; }

    // Return the copy of attributes passed at ctor
    wxGLAttributes& GetGLDispAttrs() { return m_GLAttrs; }

    // update the view port of the current context to match this window
    void SetViewport();

    // implementation-only from now on

protected:
    bool DoCreate(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                  const wxString& name);

    WXGLPixelFormat m_glFormat;
    wxGLAttributes m_GLAttrs;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_CLASS(wxGLCanvas);
};

#endif // _WX_GLCANVAS_H_
