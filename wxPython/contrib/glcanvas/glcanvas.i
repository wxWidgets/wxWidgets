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
#include "export.h"
#include <wx/glcanvas.h>
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

//---------------------------------------------------------------------------

class wxPalette;
class wxWindow;
class wxSize;
class wxPoint;
class wxGLCanvas;

//---------------------------------------------------------------------------

class wxGLContext {
public:
    wxGLContext(bool isRGB, wxGLCanvas *win, const wxPalette& palette = wxNullPalette);
    ~wxGLContext();

    void SetCurrent();
    void SetColour(const char *colour);
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



class wxGLCanvas : public wxScrolledWindow {
public:
    wxGLCanvas(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0,
               const char* name = "GLCanvas",
               int *attribList = NULL,
               const wxPalette& palette = wxNullPalette);

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void SetCurrent();
    void SetColour(const char *colour);
    void SwapBuffers();

    wxGLContext* GetContext();
};


//---------------------------------------------------------------------------

%init %{

//    wxClassInfo::CleanUpClasses();
//    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
