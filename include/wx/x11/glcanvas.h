/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets 2.0 for Motif.
//              Uses the GLX extension.
// Author:      Julian Smart and Wolfram Gloger
// Modified by:
// Created:     1995, 1999
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Wolfram Gloger
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include "wx/defs.h"

#if wxUSE_GLCANVAS

#include "wx/gdicmn.h"
#include "wx/palette.h"
#include "wx/scrolwin.h"

#include <GL/glx.h>

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------


class WXDLLEXPORT wxGLContext: public wxObject
{
public:
    wxGLContext( bool isRGB, wxWindow *win,
        const wxPalette& palette = wxNullPalette );
    wxGLContext( bool WXUNUSED(isRGB), wxWindow *win,
        const wxPalette& WXUNUSED(palette),
        const wxGLContext *other  /* for sharing display lists */
        );
    virtual ~wxGLContext();

    void SetCurrent();
    void SetColour(const wxChar *colour);
    void SwapBuffers();

    void SetupPixelFormat();
    void SetupPalette(const wxPalette& palette);
    wxPalette CreateDefaultPalette();

    inline wxPalette* GetPalette() const { return (wxPalette*) & m_palette; }
    inline wxWindow* GetWindow() const { return m_window; }
    // inline GtkWidget* GetWidget() const { return m_widget; }
    inline GLXContext GetContext() const { return m_glContext; }

public:
    GLXContext       m_glContext;

    // GtkWidget       *m_widget;
    wxPalette        m_palette;
    wxWindow*        m_window;

    DECLARE_CLASS(wxGLContext)
};


class WXDLLEXPORT wxGLCanvas: public wxScrolledWindow
{
public:
    inline wxGLCanvas() {
        m_glContext = (wxGLContext*) NULL;
        m_sharedContext = (wxGLContext*) NULL;
        // m_glWidget = (GtkWidget*) NULL;
        m_vi = (void*) NULL;
        // m_exposed = FALSE;
    }
    wxGLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxT("GLCanvas"),
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );
    wxGLCanvas( wxWindow *parent, const wxGLContext *shared,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxT("GLCanvas"),
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );
    wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxT("GLCanvas"),
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

    bool Create( wxWindow *parent,
        const wxGLContext *shared = (wxGLContext*)NULL,
        const wxGLCanvas *shared_context_of = (wxGLCanvas*)NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxT("GLCanvas"),
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

    virtual ~wxGLCanvas();

    void SetCurrent();
    void SetColour(const wxChar *colour);
    void SwapBuffers();

    // void OnSize(wxSizeEvent& event);

    // void OnInternalIdle();

    inline wxGLContext* GetContext() const { return m_glContext; }

    // implementation

    wxGLContext      *m_glContext,
        *m_sharedContext;
    wxGLCanvas       *m_sharedContextOf;
    void             *m_vi;
    // GtkWidget        *m_glWidget;
    // bool              m_exposed;

    DECLARE_EVENT_TABLE()
        DECLARE_CLASS(wxGLCanvas)

};

#endif
//  wxUSE_GLCANVAS

#endif
// _WX_GLCANVAS_H_
