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

#include "wx/defs.h"

#if wxUSE_GLCANVAS

#include "wx/palette.h"
#include "wx/scrolwin.h"
#include "wx/app.h"

#ifdef __DARWIN__
#  include <OpenGL/gl.h>
#  include <AGL/agl.h>
#else
#  include <gl.h>
#  include <agl.h>
#endif

class WXDLLEXPORT wxGLCanvas;     /* forward reference */

class WXDLLEXPORT wxGLContext: public wxObject
{
public:
   wxGLContext(AGLPixelFormat fmt, wxGLCanvas *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
    );
   virtual ~wxGLContext();

   void SetCurrent();
   void Update();  // must be called after window drag/grows/zoom or clut change
   void SetColour(const wxChar *colour);
   void SwapBuffers();


   inline wxWindow* GetWindow() const { return m_window; }
   inline AGLDrawable GetDrawable() const { return m_drawable; }

public:
   AGLContext       m_glContext;
   AGLDrawable      m_drawable;
   wxWindow*        m_window;
};

class WXDLLEXPORT wxGLCanvas: public wxWindow
{
   DECLARE_CLASS(wxGLCanvas)
 public:
   wxGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("GLCanvas") , int *attribList = 0, const wxPalette& palette = wxNullPalette);
   wxGLCanvas( wxWindow *parent, const wxGLContext *shared,
        wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("GLCanvas"),
          int *attribList = (int*) NULL, const wxPalette& palette = wxNullPalette );

   wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("GLCanvas"), int *attribList = 0, const wxPalette& palette = wxNullPalette );

   virtual ~wxGLCanvas();

   bool Create(wxWindow *parent, const wxGLContext *shared, wxWindowID id,
               const wxPoint& pos, const wxSize& size, long style, const wxString& name,
           int *attribList, const wxPalette& palette);

   void SetCurrent();
   void SetColour(const wxChar *colour);
   void SwapBuffers();
   void UpdateContext();
   void SetViewport();
   virtual bool Show(bool show = true) ;

   // Unlike some other platforms, this must get called if you override it.
   // It sets the viewport correctly and update the context.
   // You shouldn't call glViewport yourself either (use SetViewport if you must reset it.)
   void OnSize(wxSizeEvent& event);

   virtual void MacSuperChangedPosition() ;
   virtual void MacTopLevelWindowChangedPosition() ;
   virtual void         MacVisibilityChanged() ;

   void MacUpdateView() ;

   inline wxGLContext* GetContext() const { return m_glContext; }

protected:
    wxGLContext*   m_glContext;
    bool m_macCanvasIsShown ;
DECLARE_EVENT_TABLE()
};

#endif // wxUSE_GLCANVAS
#endif // _WX_GLCANVAS_H_
