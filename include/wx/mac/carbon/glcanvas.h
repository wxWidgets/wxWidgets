/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#ifdef __DARWIN__
#  include <OpenGL/gl.h>
#  include <AGL/agl.h>
#else
#  include <gl.h>
#  include <agl.h>
#endif

class WXDLLEXPORT wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win, const wxGLContext *other = NULL);
    virtual ~wxGLContext();

    virtual void SetCurrent(const wxGLCanvas& win) const;

    // Mac-specific
    AGLContext GetAGLContext() const { return m_aglContext; }

private:
    AGLContext m_aglContext;

    DECLARE_NO_COPY_CLASS(wxGLContext)
};

class WXDLLEXPORT wxGLCanvas : public wxGLCanvasBase
{
public:
    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = NULL,
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
    virtual void SwapBuffers();


    // Mac-specific functions
    // ----------------------

    // return the pixel format used by this window
    AGLPixelFormat GetAGLPixelFormat() const { return m_aglFormat; }

    // update the view port of the current context to match this window
    void SetViewport();


    // deprecated methods
    // ------------------

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

    // implementation-only from now on

    // Unlike some other platforms, this must get called if you override it.
    // It sets the viewport correctly and update the context.
    // You shouldn't call glViewport yourself either (use SetViewport if you must reset it.)
    void OnSize(wxSizeEvent& event);

    virtual void MacSuperChangedPosition();
    virtual void MacTopLevelWindowChangedPosition();
    virtual void MacVisibilityChanged();

    void MacUpdateView();

protected:
    AGLPixelFormat m_aglFormat;

    bool m_macCanvasIsShown,
         m_needsUpdate;

    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxGLCanvas)
};

#endif // _WX_GLCANVAS_H_
