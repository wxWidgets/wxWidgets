/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/8/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

extern "C"
{
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glu.h>
}

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------


class WXDLLEXPORT wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxWindow* win, const wxGLContext* other = NULL);
    virtual ~wxGLContext();

    virtual void SetCurrent(const wxGLCanvas& win) const;

private:
    GLXContext m_glContext;

    DECLARE_CLASS(wxGLContext)
};

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

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
    // --------------------------------

    virtual void SwapBuffers();


    // GTK-specific helpers
    // --------------------

    // return GLX version: 13 means 1.3
    static int GetGLXVersion();


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

    // called from "realized" callback to create the implicit context if needed
    void GTKInitImplicitContext();
#endif // WXWIN_COMPATIBILITY_2_8

    // implementation from now on
    virtual void OnInternalIdle();


    void             *m_vi; // actually an XVisualInfo*
    GLXFBConfig      *m_fbc;
    bool              m_canFreeVi;
    bool              m_canFreeFBC;
    GtkWidget        *m_glWidget;

#if WXWIN_COMPATIBILITY_2_8
    wxGLContext      *m_sharedContext;
    wxGLCanvas       *m_sharedContextOf;
    const bool        m_createImplicitContext;
#endif // WXWIN_COMPATIBILITY_2_8

    // returns an XVisualInfo* based on desired GL attributes;
    // returns NULL if an appropriate visual is not found. The
    // caller is reponsible for using XFree() to deallocate
    // the returned structure.
    static void* ChooseGLVisual(const int *attribList);
    static void* ChooseGLFBC(const int *attribList);
    static void GetGLAttribListFromWX(const int *wx_attribList, int *gl_attribList );

private:
    DECLARE_CLASS(wxGLCanvas)
};

#endif // _WX_GLCANVAS_H_

