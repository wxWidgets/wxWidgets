/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows 2.0 for Motif.
//              Uses the GLX extension.
// Author:      Julian Smart and Wolfram Gloger
// Modified by:
// Created:     1995, 1999
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Wolfram Gloger
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "glcanvas.h"
#endif

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include <wx/setup.h>

#if wxUSE_GLCANVAS

#include <wx/gdicmn.h>
#include <wx/palette.h>
#include <wx/scrolwin.h>

#include <GL/glx.h>

class WXDLLEXPORT wxGLCanvas: public wxScrolledWindow
{
DECLARE_CLASS(wxGLCanvas)
public:
    GLXContext glx_cx;

    inline wxGLCanvas() { glx_cx = 0; }

    wxGLCanvas(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = "GLCanvas", int *attribList = 0, const wxPalette& palette = wxNullPalette);
    ~wxGLCanvas(void);

    void SetCurrent();
    void SwapBuffers();
    void SetColour(const char *col);
};

#endif
  //  wxUSE_GLCANVAS

#endif
  // _WX_GLCANVAS_H_
