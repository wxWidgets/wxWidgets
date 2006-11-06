/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/glcanvas.h
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

class WXDLLEXPORT wxGLContext;
class WXDLLEXPORT wxGLCanvas;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------


class WXDLLEXPORT wxGLContext: public wxObject
{
public:
    wxGLContext( bool isRGB, wxWindow *win, const wxPalette& palette = wxNullPalette );
    wxGLContext(
               bool WXUNUSED(isRGB), wxWindow *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
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
    inline GtkWidget* GetWidget() const { return m_widget; }
    inline GLXContext GetContext() const { return m_glContext; }

public:
   GLXContext       m_glContext;

   GtkWidget       *m_widget;
   wxPalette        m_palette;
   wxWindow*        m_window;

private:
  DECLARE_CLASS(wxGLContext)
};

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

class WXDLLEXPORT wxGLCanvas: public wxWindow
{
public:
   inline wxGLCanvas() {
      m_glContext = (wxGLContext*) NULL;
      m_sharedContext = (wxGLContext*) NULL;
      m_glWidget = (GtkWidget*) NULL;
      m_vi = (void*) NULL;
      m_exposed = FALSE;
   }
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

