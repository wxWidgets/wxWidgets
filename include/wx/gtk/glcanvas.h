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

#include "wx/scrolwin.h"
#include "wx/app.h"

extern "C" {
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
}

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvas;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------


class WXDLLIMPEXP_GL wxGLContext: public wxObject
{
public:
    wxGLContext(wxWindow* win, const wxGLContext* other=NULL /* for sharing display lists */);
    virtual ~wxGLContext();

public:
    // The win wxGLCanvas needs not necessarily be the same as the wxGLCanvas with which this context was created!
    void SetCurrent(const wxGLCanvas& win) const;


private:
    GLXContext m_glContext;

private:
    DECLARE_CLASS(wxGLContext)
};

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvas: public wxWindow
{
public:
    // This ctor is identical to the next, except for the fact that it
    // doesn't create an implicit wxGLContext.
    // The attribList parameter has been moved to avoid overload clashes.
    wxGLCanvas( wxWindow *parent, wxWindowID id = -1,
        int *attribList = (int*) NULL,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        const wxPalette& palette = wxNullPalette );

   wxGLCanvas( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

   wxGLCanvas( wxWindow *parent, const wxGLContext *shared,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

   wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

   bool Create( wxWindow *parent,
                const wxGLContext *shared = (wxGLContext*)NULL,
                const wxGLCanvas *shared_context_of = (wxGLCanvas*)NULL,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                int *attribList = (int*) NULL,
                const wxPalette& palette = wxNullPalette );

   virtual ~wxGLCanvas();

   void SetCurrent(const wxGLContext& RC) const;
   void SetCurrent();
   void SetColour(const wxChar *colour);
   void SwapBuffers();

   void OnSize(wxSizeEvent& event);

   void OnInternalIdle();

   inline wxGLContext* GetContext() const { return m_glContext; }

 // implementation

    wxGLContext      *m_glContext,
                     *m_sharedContext;
    wxGLCanvas       *m_sharedContextOf;
    const bool        m_createImplicitContext;
    void             *m_vi; // actually an XVisualInfo*
    GLXFBConfig      *m_fbc;
    bool              m_canFreeVi;
    bool              m_canFreeFBC;
    GtkWidget        *m_glWidget;
    bool              m_exposed;

    // returns an XVisualInfo* based on desired GL attributes;
    // returns NULL if an appropriate visual is not found. The
    // caller is reponsible for using XFree() to deallocate
    // the returned structure.
    static void* ChooseGLVisual(int *attribList);
    static void* ChooseGLFBC(int *attribList);
    static void GetGLAttribListFromWX(int *wx_attribList, int *gl_attribList );

    static void QueryGLXVersion();
    static int GetGLXVersion();
    static int m_glxVersion;
    
private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxGLCanvas)
};

#endif
    // _WX_GLCANVAS_H_

