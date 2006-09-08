/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.i
// Purpose:     SWIG definitions for the OpenGL wxWindows classes
//
// Author:      Robin Dunn
//
// Created:     15-Mar-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%define DOCSTRING
"`GLCanvas` provides an OpenGL Context on a `wx.Window`."
%enddef

%module(package="wx", docstring=DOCSTRING) glcanvas

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

#include <wx/glcanvas.h>

%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }


MAKE_CONST_WXSTRING2(GLCanvasNameStr, wxT("GLCanvas"));
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//---------------------------------------------------------------------------

class wxPalette;

//---------------------------------------------------------------------------

MustHaveApp(wxGLContext);

class wxGLContext : public wxObject {
public:
#ifndef __WXMAC__  
    wxGLContext(wxGLCanvas *win, const wxGLContext* other = NULL);
#else
    %extend {
        wxGLContext(bool isRGB, wxGLCanvas *win,
                    const wxPalette& palette = wxNullPalette,
                    const wxGLContext* other = NULL) {
            AGLPixelFormat fmt;  // TODO: How should this be initialized?
            return new wxGLContext(fmt, win, palette, other);
        }
    }

#endif
    ~wxGLContext();

#ifndef __WXMAC__  
    void SetCurrent(const wxGLCanvas& win);
#endif
};

//---------------------------------------------------------------------------

enum {
    WX_GL_RGBA,              // use true color palette
    WX_GL_BUFFER_SIZE,       // bits for buffer if not WX_GL_RGBA
    WX_GL_LEVEL,             // 0 for main buffer, >0 for overlay, <0 for underlay
    WX_GL_DOUBLEBUFFER,      // use doublebuffer
    WX_GL_STEREO,            // use stereoscopic display
    WX_GL_AUX_BUFFERS,       // number of auxiliary buffers
    WX_GL_MIN_RED,           // use red buffer with most bits (> MIN_RED bits)
    WX_GL_MIN_GREEN,         // use green buffer with most bits (> MIN_GREEN bits)
    WX_GL_MIN_BLUE,          // use blue buffer with most bits (> MIN_BLUE bits)
    WX_GL_MIN_ALPHA,         // use blue buffer with most bits (> MIN_ALPHA bits)
    WX_GL_DEPTH_SIZE,        // bits for Z-buffer (0,16,32)
    WX_GL_STENCIL_SIZE,      // bits for stencil buffer
    WX_GL_MIN_ACCUM_RED,     // use red accum buffer with most bits (> MIN_ACCUM_RED bits)
    WX_GL_MIN_ACCUM_GREEN,   // use green buffer with most bits (> MIN_ACCUM_GREEN bits)
    WX_GL_MIN_ACCUM_BLUE,    // use blue buffer with most bits (> MIN_ACCUM_BLUE bits)
    WX_GL_MIN_ACCUM_ALPHA    // use blue buffer with most bits (> MIN_ACCUM_ALPHA bits)
};


%typemap(in) int *attribList (int *temp) {
    int i;
    if (PySequence_Check($input)) {
        int size = PyObject_Length($input);
        temp = new int[size+1]; // (int*)malloc((size + 1) * sizeof(int));
        for (i = 0; i < size; i++) {
            temp[i] = PyInt_AsLong(PySequence_GetItem($input, i));
        }
        temp[size] = 0;
        $1 = temp;
    }
}

%typemap(freearg) int *attribList
{
    delete [] $1;
}



MustHaveApp(wxGLCanvas);

class wxGLCanvas : public wxWindow {
public:
    %pythonAppend wxGLCanvas   "self._setOORInfo(self)"
    wxGLCanvas(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0,
               const wxString& name = wxPyGLCanvasNameStr,
               int *attribList = NULL,
               const wxPalette& palette = wxNullPalette);

    %pythonAppend wxGLCanvas   "val._setOORInfo(val)"
    %RenameCtor(GLCanvasWithContext, 
        wxGLCanvas( wxWindow *parent,
                    const wxGLContext *shared = NULL,
                    wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyGLCanvasNameStr,
                    int *attribList = NULL,
                    const wxPalette& palette = wxNullPalette ));

#ifdef __WXMAC__
    void SetCurrent();
#else
    void SetCurrent(const wxGLContext& RC);
#endif
    void SetColour(const wxString& colour);
    void SwapBuffers();

    wxGLContext* GetContext();

#ifdef __WXMSW__
    void SetupPixelFormat(int *attribList = NULL);
    void SetupPalette(const wxPalette& palette);
    wxPalette CreateDefaultPalette();
    wxPalette* GetPalette();
#endif
};


//---------------------------------------------------------------------------

%init %{

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
