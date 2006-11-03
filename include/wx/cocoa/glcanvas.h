/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/glcanvas.h
// Purpose:     wxGLCanvas class
// Author:      David Elliott
// Modified by:
// Created:     2004/09/29
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_GLCANVAS_H__
#define _WX_COCOA_GLCANVAS_H__

#include "wx/window.h"

// #include "wx/cocoa/NSOpenGLView.h"
// Include gl.h from the OpenGL framework
#include <OpenGL/gl.h>

class WXDLLIMPEXP_GL wxGLCanvas;
DECLARE_WXCOCOA_OBJC_CLASS(NSOpenGLContext);
//DECLARE_WXCOCOA_OBJC_CLASS(NSOpenGLView);

// ========================================================================
// wxGLContext
// ========================================================================
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


    inline wxWindow* GetWindow() const
    {   return m_window; }

    inline WX_NSOpenGLContext GetNSOpenGLContext() const
    {   return m_cocoaNSOpenGLContext; }

public:
    WX_NSOpenGLContext m_cocoaNSOpenGLContext;
    wxWindow*        m_window;
};



// ========================================================================
// wxGLCanvas
// ========================================================================
class WXDLLIMPEXP_GL wxGLCanvas: public wxWindow// , protected wxCocoaNSOpenGLView
{
    DECLARE_DYNAMIC_CLASS(wxGLCanvas)
    DECLARE_EVENT_TABLE()
//    WX_DECLARE_COCOA_OWNER(NSOpenGLView,NSView,NSView)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxGLCanvas()
    :   m_glContext(NULL)
    { }
    wxGLCanvas(wxWindow *parent, wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxGLCanvasName,
            int *attribList = NULL, const wxPalette& palette = wxNullPalette);

    wxGLCanvas(wxWindow *parent,
            const wxGLContext *shared = NULL,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxGLCanvasName,
            int *attribList = NULL, const wxPalette& palette = wxNullPalette);

    wxGLCanvas(wxWindow *parent,
            const wxGLCanvas *shared = NULL,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxGLCanvasName,
            int *attribList = NULL, const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0, const wxString& name = wxGLCanvasName);
    virtual ~wxGLCanvas();

// ------------------------------------------------------------------------
// Cocoa callbacks
// ------------------------------------------------------------------------
protected:
    // NSOpenGLView cannot be enabled/disabled
    virtual void CocoaSetEnabled(bool enable) { }
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    void SetCurrent();
    void SwapBuffers();

    void OnSize(wxSizeEvent& event)
    {}
    inline wxGLContext* GetContext() const
    {   return m_glContext; }
protected:
    wxGLContext *m_glContext;
    wxPalette m_palette;
};

#endif //ndef _WX_COCOA_GLCANVAS_H__
