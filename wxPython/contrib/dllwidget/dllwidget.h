/////////////////////////////////////////////////////////////////////////////
// Name:        dllwidget.h
// Purpose:     Dynamically loadable C++ widget for wxPython
// Author:      Vaclav Slavik
// Created:     2001/12/03
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "dllwidget.h"
#endif

#ifndef __DLLWIDGET_H__
#define __DLLWIDGET_H__

#include "wx/panel.h"

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
            : wxWindow(parent, wxID_ANY) {}

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



class WXDLLEXPORT wxDynamicLibrary;

typedef int (*wxSendCommandFunc)(wxWindow *wnd, int cmd, const wxString& param);

class wxDllWidget : public wxPanel
{
public:
    wxDllWidget(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& dllName = wxEmptyString,
                const wxString& className = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);
    virtual ~wxDllWidget();

    bool Ok() { return m_widget != NULL; }

    virtual int SendCommand(int cmd, const wxString& param = wxEmptyString);
    virtual wxWindow* GetWidgetWindow() { return m_widget; }

    virtual void AddChild(wxWindowBase *child);

    static wxString GetDllExt();


protected:
    bool LoadWidget(const wxString& dll, const wxString& className, long style);
    void UnloadWidget();

protected:
    wxWindow*           m_widget;
    wxSendCommandFunc   m_cmdFunc;
    wxDynamicLibrary*   m_lib;
    bool                m_controlAdded;

private:
    DECLARE_ABSTRACT_CLASS(wxDllWidget)
};


#define DECLARE_DLL_WIDGET(widget) \
    static int SendCommandTo##widget(wxWindow *wnd, int cmd, const wxString& param) \
    { \
        return wxStaticCast(wnd, widget)->HandleCommand(cmd, param); \
    }


#define BEGIN_WIDGET_LIBRARY() \
    extern "C" WXEXPORT bool DLL_WidgetFactory( \
                const wxString& className, \
                wxWindow *parent, \
                long style, \
                wxWindow **classInst, \
                wxSendCommandFunc *cmdFunc) \
    { \
        wxClassInfo::CleanUpClasses(); \
        wxClassInfo::InitializeClasses();


#define REGISTER_WIDGET(widget) \
        if ( className == wxT(#widget) ) \
        { \
            *classInst = new widget(parent, style); \
            *cmdFunc = SendCommandTo##widget; \
            return true; \
        }


#define END_WIDGET_LIBRARY() \
       return false; \
    }

#endif // __DLLWIDGET_H__
