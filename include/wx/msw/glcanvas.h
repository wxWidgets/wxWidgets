/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include "wx/palette.h"
#include "wx/scrolwin.h"

#include <windows.h>
#include "wx/msw/winundef.h"

#include <GL/gl.h>

//---------------------------------------------------------------------------
// Constants for attriblist
//---------------------------------------------------------------------------

// The generic GL implementation doesn't support most of these options,
// such as stereo, auxiliary buffers, alpha channel, and accum buffer.
// Other implementations may actually support them.

enum
{
    WX_GL_RGBA=1,          /* use true color palette */
    WX_GL_BUFFER_SIZE,     /* bits for buffer if not WX_GL_RGBA */
    WX_GL_LEVEL,           /* 0 for main buffer, >0 for overlay, <0 for underlay */
    WX_GL_DOUBLEBUFFER,    /* use doublebuffer */
    WX_GL_STEREO,          /* use stereoscopic display */
    WX_GL_AUX_BUFFERS,     /* number of auxiliary buffers */
    WX_GL_MIN_RED,         /* use red buffer with most bits (> MIN_RED bits) */
    WX_GL_MIN_GREEN,       /* use green buffer with most bits (> MIN_GREEN bits) */
    WX_GL_MIN_BLUE,        /* use blue buffer with most bits (> MIN_BLUE bits) */
    WX_GL_MIN_ALPHA,       /* use blue buffer with most bits (> MIN_ALPHA bits) */
    WX_GL_DEPTH_SIZE,      /* bits for Z-buffer (0,16,32) */
    WX_GL_STENCIL_SIZE,    /* bits for stencil buffer */
    WX_GL_MIN_ACCUM_RED,   /* use red accum buffer with most bits (> MIN_ACCUM_RED bits) */
    WX_GL_MIN_ACCUM_GREEN, /* use green buffer with most bits (> MIN_ACCUM_GREEN bits) */
    WX_GL_MIN_ACCUM_BLUE,  /* use blue buffer with most bits (> MIN_ACCUM_BLUE bits) */
    WX_GL_MIN_ACCUM_ALPHA  /* use blue buffer with most bits (> MIN_ACCUM_ALPHA bits) */
};

class WXDLLIMPEXP_GL wxGLCanvas;     /* forward reference */

class WXDLLIMPEXP_GL wxGLContext: public wxObject
{
public:
    wxGLContext(bool isRGB, wxGLCanvas *win, const wxPalette& palette = wxNullPalette);

    wxGLContext( bool isRGB, wxGLCanvas *win,
        const wxPalette& WXUNUSED(palette),
        const wxGLContext *other /* for sharing display lists */ );

    virtual ~wxGLContext();


    void SetCurrent();

    void SetColour(const wxChar *colour);

    void SwapBuffers();


    inline wxWindow* GetWindow() const { return m_window; }

    inline WXHDC GetHDC() const { return m_hDC; }

    inline HGLRC GetGLRC() const { return m_glContext; }

public:
    HGLRC            m_glContext;
    WXHDC            m_hDC;
    wxWindow*        m_window;
};

class WXDLLIMPEXP_GL wxGLCanvas: public wxWindow
{
    DECLARE_CLASS(wxGLCanvas)
public:
    wxGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxGLCanvasName, int *attribList = 0,
        const wxPalette& palette = wxNullPalette);

    wxGLCanvas(wxWindow *parent,
        const wxGLContext *shared,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxGLCanvasName,
        int *attribList = (int *) NULL,
        const wxPalette& palette = wxNullPalette);

    wxGLCanvas(wxWindow *parent,
        const wxGLCanvas *shared,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxGLCanvasName,
        int *attribList = 0,
        const wxPalette& palette = wxNullPalette);

    virtual ~wxGLCanvas();

    // Replaces wxWindow::Create functionality, since
    // we need to use a different window class
    bool Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size,
        long style, const wxString& name);

    void SetCurrent();

#ifdef __WXUNIVERSAL__
    virtual bool SetCurrent(bool doit) { return wxWindow::SetCurrent(doit); };
#endif

    void SetColour(const wxChar *colour);

    void SwapBuffers();

    void OnSize(wxSizeEvent& event);

    void OnQueryNewPalette(wxQueryNewPaletteEvent& event);

    void OnPaletteChanged(wxPaletteChangedEvent& event);

    inline wxGLContext* GetContext() const { return m_glContext; }

    inline WXHDC GetHDC() const { return m_hDC; }

    void SetupPixelFormat(int *attribList = (int *) NULL);

    void SetupPalette(const wxPalette& palette);

    wxPalette CreateDefaultPalette();

    inline wxPalette* GetPalette() const { return (wxPalette *) &m_palette; }

protected:
    wxGLContext*   m_glContext;  // this is typedef-ed ptr, in fact
    wxPalette      m_palette;
    WXHDC          m_hDC;

    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_GLCANVAS_H_

