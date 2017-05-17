/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include "wx/palette.h"

#include "wx/msw/wrapwin.h"

#include <GL/gl.h>

// ----------------------------------------------------------------------------
// wxGLContext: OpenGL rendering context
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win,
                const wxGLContext *other = NULL,
                const wxGLContextAttrs *ctxAttrs = NULL);
    virtual ~wxGLContext();

    virtual bool SetCurrent(const wxGLCanvas& win) const wxOVERRIDE;

    HGLRC GetGLRC() const { return m_glContext; }

protected:
    HGLRC m_glContext;

private:
    wxDECLARE_CLASS(wxGLContext);
};

// ----------------------------------------------------------------------------
// wxGLCanvas: OpenGL output window
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvas : public wxGLCanvasBase
{
public:
    explicit // avoid implicitly converting a wxWindow* to wxGLCanvas
    wxGLCanvas(wxWindow *parent,
               const wxGLAttributes& dispAttrs,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    explicit
    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = NULL,
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
                const int *attribList = NULL,
                const wxPalette& palette = wxNullPalette);

    virtual ~wxGLCanvas();

    // implement wxGLCanvasBase methods
    virtual bool SwapBuffers() wxOVERRIDE;


    // MSW-specific helpers
    // --------------------

    // get the HDC used for OpenGL rendering
    HDC GetHDC() const { return m_hDC; }

    // Try to find pixel format matching the given attributes list for the
    // specified HDC, return 0 on error, otherwise ppfd is filled in with the
    // information from dispAttrs
    static int FindMatchingPixelFormat(const wxGLAttributes& dispAttrs,
                                       PIXELFORMATDESCRIPTOR* ppfd = NULL);
    // Same as FindMatchingPixelFormat
    static int ChooseMatchingPixelFormat(HDC hdc, const int *attribList,
                                         PIXELFORMATDESCRIPTOR *pfd = NULL);

#if wxUSE_PALETTE
    // palette stuff
    bool SetupPalette(const wxPalette& palette);
    virtual wxPalette CreateDefaultPalette() wxOVERRIDE;
    void OnQueryNewPalette(wxQueryNewPaletteEvent& event);
    void OnPaletteChanged(wxPaletteChangedEvent& event);
#endif // wxUSE_PALETTE

    // deprecated methods using the implicit wxGLContext, associate the context
    // explicitly with the window instead
#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED(
    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const int *attribList = NULL,
               const wxPalette& palette = wxNullPalette)
    );

    wxDEPRECATED(
    wxGLCanvas(wxWindow *parent,
               const wxGLContext *shared,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const int *attribList = NULL,
               const wxPalette& palette = wxNullPalette)
    );

    wxDEPRECATED(
    wxGLCanvas(wxWindow *parent,
               const wxGLCanvas *shared,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const int *attribList = NULL,
               const wxPalette& palette = wxNullPalette)
    );
#endif // WXWIN_COMPATIBILITY_2_8

protected:
    // common part of all ctors
    void Init();

    // the real window creation function, Create() may reuse it twice as we may
    // need to create an OpenGL window to query the available extensions and
    // then potentially delete and recreate it with another pixel format
    bool CreateWindow(wxWindow *parent,
                      wxWindowID id = wxID_ANY,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxString& name = wxGLCanvasName);

    // set up the pixel format using the given attributes and palette
    int DoSetup(PIXELFORMATDESCRIPTOR &pfd, const int *attribList);


    // HDC for this window, we keep it all the time
    HDC m_hDC;

private:
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_CLASS(wxGLCanvas);
};

#endif // _WX_GLCANVAS_H_
