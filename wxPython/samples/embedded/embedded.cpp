//----------------------------------------------------------------------
// Name:        embedded.cpp
// Purpose:     To serve as an example of how to use wxPython from
//              within a C++ wxWindows program.
//
// Author:      Robin Dunn
//
// Created:     1-May-2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
//----------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/splitter.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
    #include "mondrian.xpm"
#endif

// Import Python and wxPython headers
#include <Python.h>
#include <wxPython.h>

//----------------------------------------------------------------------
// Class definitions

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual ~MyApp();
    void Init_wxPython();
private:
    PyThreadState* main_tstate;
};


class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void RedirectStdio();
    wxWindow* DoPythonStuff(wxWindow* parent);
    void OnExit(wxCommandEvent& event);
    void OnPyFrame(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------
// MyApp methods


bool MyApp::OnInit()
{
    Init_wxPython();
    MyFrame *frame = new MyFrame(_T("Embedded wxPython Test"),
                                 wxPoint(50, 50), wxSize(700, 600));
    frame->Show(TRUE);
    return TRUE;
}


void MyApp::Init_wxPython()
{
    // Initialize Python
    Py_Initialize();
    PyEval_InitThreads();

    // Load the wxPython core API.  Imports the wxPython.wxc
    // module and sets a pointer to a function table located there.
    wxPyCoreAPI_IMPORT();

    // Save the current Python thread state and release the
    // Global Interpreter Lock.
    main_tstate = wxPyBeginAllowThreads();
}


MyApp::~MyApp()
{
    // Restore the thread state and tell Python to cleanup after itself.
    wxPyEndAllowThreads(main_tstate);
    Py_Finalize();
}

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------

enum
{
    ID_EXIT=1001,
    ID_PYFRAME
};


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_EXIT,      MyFrame::OnExit)
    EVT_MENU(ID_PYFRAME,   MyFrame::OnPyFrame)
END_EVENT_TABLE()



MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, -1, title, pos, size,
              wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)
{
    SetIcon(wxICON(mondrian));

    wxMenuBar* mbar = new wxMenuBar;
    wxMenu*    menu = new wxMenu;
    menu->Append(ID_PYFRAME, "Make wx&Python frame");
    menu->AppendSeparator();
    menu->Append(ID_EXIT, "&Close Frame\tAlt-X");
    mbar->Append(menu, "&File");
    SetMenuBar(mbar);

    CreateStatusBar();
    RedirectStdio();

    // Make some child windows from C++
    wxSplitterWindow* sp = new wxSplitterWindow(this, -1);
    wxPanel* p1 = new wxPanel(sp, -1);
    p1->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD));
    new wxStaticText(p1, -1,
                 wxT("The frame, menu, splitter, this panel and this text were created in C++..."),
                 wxPoint(10,10));

    // And get a panel from Python
    wxWindow* p2 = DoPythonStuff(sp);

    sp->SplitHorizontally(p1, p2, GetClientSize().y/4);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close();
}


//----------------------------------------------------------------------
// This is were the fun begins...


char* python_code1 = "\
from wxPython.wx import wxFrame\n\
f = wxFrame(None, -1, 'Hello from wxPython!', size=(250, 150))\n\
f.Show()\n\
";

void MyFrame::OnPyFrame(wxCommandEvent& event)
{
    // For simple Python code that doesn't have to interact with the
    // C++ code in any way, you can execute it with PyRun_SimpleString.


    // First, whenever you do anyting with Python objects or code, you
    // *MUST* aquire the Global Interpreter Lock and block other
    // Python threads from running.
    wxPyBeginBlockThreads();

    // Execute the code in the __main__ module
    PyRun_SimpleString(python_code1);

    // Finally, release the GIL and let other Python threads run.
    wxPyEndBlockThreads();
}


void MyFrame::RedirectStdio()
{
    // This is a helpful little tidbit to help debugging and such.  It
    // redirects Python's stdout and stderr to a window that will popup
    // only on demand when something is printed, like a traceback.
    char* python_redirect = "\
import sys\n\
from wxPython.wx import wxPyOnDemandOutputWindow\n\
output = wxPyOnDemandOutputWindow()\n\
sys.stdin = sys.stderr = output\n\
";
    wxPyBeginBlockThreads();
    PyRun_SimpleString(python_redirect);
    wxPyEndBlockThreads();
}




char* python_code2 = "\
import embedded_sample\n\
\n\
def makeWindow(parent):\n\
    win = embedded_sample.MyPanel(parent)\n\
    return win\n\
";

wxWindow* MyFrame::DoPythonStuff(wxWindow* parent)
{
    // More complex embedded situations will require passing C++ objects to
    // Python and/or returning objects from Python to be used in C++.  This
    // sample shows one way to do it.  NOTE: The above code could just have
    // easily come from a file, or the whole thing could be in the Python
    // module that is imported and manipulated directly in this C++ code.  See
    // the Python API for more details.

    wxWindow* window = NULL;
    PyObject* result;

    // As always, first grab the GIL
    wxPyBeginBlockThreads();

    // Now make a dictionary to serve as the global namespace when the code is
    // executed.  Put a reference to the builtins module in it.  (Yes, the
    // names are supposed to be different, I don't know why...)
    PyObject* globals = PyDict_New();
    PyObject* builtins = PyImport_ImportModule("__builtin__");
    PyDict_SetItemString(globals, "__builtins__", builtins);
    Py_DECREF(builtins);

    // Execute the code to make the makeWindow function
    result = PyRun_String(python_code2, Py_file_input, globals, globals);
    // Was there an exception?
    if (! result) {
        PyErr_Print();
        wxPyEndBlockThreads();
        return NULL;
    }
    Py_DECREF(result);

    // Now there should be an object named 'makeWindow' in the dictionary that
    // we can grab a pointer to:
    PyObject* func = PyDict_GetItemString(globals, "makeWindow");
    wxASSERT(PyCallable_Check(func));

    // Now build an argument tuple and call the Python function.  Notice the
    // use of another wxPython API to take a wxWindows object and build a
    // wxPython object that wraps it.
    PyObject* arg = wxPyMake_wxObject(parent);
    wxASSERT(arg != NULL);

    PyObject* tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(tuple, 0, arg);
    result = PyEval_CallObject(func, tuple);

    // Was there an exception?
    if (! result)
        PyErr_Print();
    else {
        // Otherwise, get the returned window out of Python-land and
        // into C++-ville...
        bool error = SWIG_GetPtrObj(result, (void**)&window, "_wxWindow_p");
        wxASSERT_MSG(!error, wxT("Returned object was not a wxWindow!"));
        Py_DECREF(result);
    }

    // Release the python objects we still have
    Py_DECREF(globals);
    Py_DECREF(tuple);

    // Finally, after all Python stuff is done, release the GIL
    wxPyEndBlockThreads();

    return window;
}


//----------------------------------------------------------------------






