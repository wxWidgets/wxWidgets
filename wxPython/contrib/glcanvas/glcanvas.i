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


%module glcanvas

%{
#include "wxPython.h"
#ifdef __WXMSW__
#include "myglcanvas.h"
#else
#include <wx/glcanvas.h>
#endif

%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern windows2.i
%extern windows3.i
%extern frames.i
%extern _defs.i
%extern misc.i
%extern gdi.i
%extern controls.i
%extern events.i


%pragma(python) code = "import wx"

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyGLCanvasNameStr(wxT("GLCanvas"));
    static const wxString wxPyEmptyString(wxT(""));
%}

//---------------------------------------------------------------------------

class wxPalette;
class wxWindow;
class wxSize;
class wxPoint;
class wxGLCanvas;

//---------------------------------------------------------------------------

class wxGLContext : public wxObject {
public:
#ifndef __WXMAC__  //  fix this?
    wxGLContext(bool isRGB, wxGLCanvas *win,
                const wxPalette& palette = wxNullPalette);
#endif
    ~wxGLContext();

    void SetCurrent();
    void SetColour(const wxString& colour);
    void SwapBuffers();

#ifdef __WXGTK__
    void SetupPixelFormat();
    void SetupPalette(const wxPalette& palette);
    wxPalette CreateDefaultPalette();
    wxPalette* GetPalette();
#endif

    wxWindow* GetWindow();
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


%typemap(python, in) int *attribList (int *temp) {
    int i;
    if (PySequence_Check($source)) {
        int size = PyObject_Length($source);
        temp = new int[size+1]; // (int*)malloc((size + 1) * sizeof(int));
        for (i = 0; i < size; i++) {
            temp[i] = PyInt_AsLong(PySequence_GetItem($source, i));
        }
        temp[size] = 0;
        $target = temp;
    }
}

%typemap(python, freearg) int *attribList
{
    delete [] $source;
}



class wxGLCanvas : public wxWindow {
public:
    wxGLCanvas(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0,
               const wxString& name = wxPyGLCanvasNameStr,
               int *attribList = NULL,
               const wxPalette& palette = wxNullPalette);

    %name(wxGLCanvasWithContext)
        wxGLCanvas( wxWindow *parent,
                    const wxGLContext *shared = NULL,
                    wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyGLCanvasNameStr,
                    int *attribList = NULL,
                    const wxPalette& palette = wxNullPalette );

//      bool Create(wxWindow *parent, wxWindowID id,
//                  const wxPoint& pos, const wxSize& size, long style, const wxString& name);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxGLCanvasWithContext:val._setOORInfo(self)"

    void SetCurrent();
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

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
