/////////////////////////////////////////////////////////////////////////////
// Name:        dllwidget_.i
// Purpose:     Load wx widgets from external DLLs
//
// Author:      Robin Dunn
//
// Created:     04-Dec-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module dllwidget_


%{
#include "wxPython.h"
#include "dllwidget.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
%}

//---------------------------------------------------------------------------

/*

wxDllWidget can be used to embed a wxWindow implemented in C++ in your
wxPython application without the need to write a SWIG interface. Widget's code
is stored in shared library or DLL that exports DLL_WidgetFactory symbol
and loaded at runtime. All you have to do is to pass the name of DLL and the class
to create to wxDllWidget's ctor.

Runtime-loadable widget must have HandleCommand method (see the example) that is
used to communicate with Python app. You call wxDllWidget.SendCommand(cmd,param) from
Python and it in turn calls HandleCommand of the loaded widget.

You must use DECLARE_DLL_WIDGET, BEGIN_WIDGET_LIBRARY, END_WIDGET_LIBRARY and
REGISTER_WIDGET macros in your C++ module in order to provide all the meat
wxDllWidget needs.

Example of use:

    #define CMD_MAKEWHITE     1

    class MyWindow : public wxWindow
    {
    public:
        MyWindow(wxWindow *parent, long style)
            : wxWindow(parent, -1) {}

        int HandleCommand(int cmd, const wxString& param)
        {
            if (cmd == CMD_MAKEWHITE)
                SetBackgroundColour(*wxWHITE);
            return 0;
        }
    };
    DECLARE_DLL_WIDGET(MyWindow)

    class MyCanvasWindow : public wxScrolledWindow
    {
        ...
    };
    DECLARE_DLL_WIDGET(MyCanvasWindow)

    BEGIN_WIDGET_LIBRARY()
        REGISTER_WIDGET(MyWindow)
        REGISTER_WIDGET(MyCanvasWindow)
    END_WIDGET_LIBRARY()

*/

class wxDllWidget : public wxPanel
{
public:
    wxDllWidget(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& dllName = wxPyEmptyString,
                const wxString& className = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    bool Ok();

    int SendCommand(int cmd, const wxString& param = wxPyEmptyString);
    wxWindow* GetWidgetWindow();

    static wxString GetDllExt();
};

//---------------------------------------------------------------------------

%init %{

%}


//---------------------------------------------------------------------------
