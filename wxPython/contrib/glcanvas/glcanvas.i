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
#include "helpers.h"
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


%{
#if defined(__WXMSW__)
    static wxString wxPyEmptyStr("");
    static wxPoint  wxPyDefaultPosition(-1, -1);
    static wxSize   wxPyDefaultSize(-1, -1);
#endif
%}

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

class wxGLCanvas : public wxScrolledWindow {
public:
    wxGLCanvas(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize, long style = 0,
               const char* name = "GLCanvas",
               int *attribList = 0,
               const wxPalette& palette = wxNullPalette);

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void SetCurrent();
    void SetColour(const char *colour);
    void SwapBuffers();

    wxGLContext* GetContext();
};


//---------------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
