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

#include <Python.h>


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
#include <wx/wxPython/wxPython.h>


//----------------------------------------------------------------------
// Class definitions

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int  OnExit();
    bool Init_wxPython();
private:
    PyThreadState* m_mainTState;
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
    if ( !Init_wxPython() )
        // don't start the app if we can't initialize wxPython.
        return false;  
    
    MyFrame *frame = new MyFrame(_T("Embedded wxPython Test"),
                                 wxDefaultPosition, wxSize(700, 600));
    frame->Show(true);
    return true;
}



bool MyApp::Init_wxPython()
{
    // Initialize Python
    Py_Initialize();
    PyEval_InitThreads();

    // Load the wxPython core API.  Imports the wx._core_ module and sets a
    // local pointer to a function table located there.  The pointer is used
    // internally by the rest of the API functions.
    if ( ! wxPyCoreAPI_IMPORT() ) {
        wxLogError(wxT("***** Error importing the wxPython API! *****"));
        PyErr_Print();
        Py_Finalize();
        return false;
    }        
    
    // Save the current Python thread state and release the
    // Global Interpreter Lock.
    m_mainTState = wxPyBeginAllowThreads();

    return true;
}


int MyApp::OnExit()
{
    // Restore the thread state and tell Python to cleanup after itself.
    // wxPython will do its own cleanup as part of that process.  This is done
    // in OnExit instead of ~MyApp because OnExit is only called if OnInit is
    // successful.
    wxPyEndAllowThreads(m_mainTState);
    Py_Finalize();
    return 0;    
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
    menu->Append(ID_PYFRAME, _T("Make wx&Python frame"));
    menu->AppendSeparator();
    menu->Append(ID_EXIT, _T("&Close Frame\tAlt-X"));
    mbar->Append(menu, _T("&File"));
    SetMenuBar(mbar);

    CreateStatusBar();
    RedirectStdio();

    // Make some child windows from C++
    wxSplitterWindow* sp = new wxSplitterWindow(this, -1);
    wxPanel* p1 = new wxPanel(sp, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

    new wxStaticText(p1, -1,
                 _T("The frame, menu, splitter, this panel and this text were created in C++..."),
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
// This is where the fun begins...


char* python_code1 = "\
import wx\n\
f = wx.Frame(None, -1, 'Hello from wxPython!', size=(250, 150))\n\
f.Show()\n\
";

void MyFrame::OnPyFrame(wxCommandEvent& event)
{
    // For simple Python code that doesn't have to interact with the
    // C++ code in any way, you can execute it with PyRun_SimpleString.


    // First, whenever you do anything with Python objects or code, you
    // *MUST* aquire the Global Interpreter Lock and block other
    // Python threads from running.
    wxPyBlock_t blocked = wxPyBeginBlockThreads();

    // Execute the code in the __main__ module
    PyRun_SimpleString(python_code1);

    // Finally, release the GIL and let other Python threads run.
    wxPyEndBlockThreads(blocked);
}


void MyFrame::RedirectStdio()
{
    // This is a helpful little tidbit to help debugging and such.  It
    // redirects Python's stdout and stderr to a window that will popup
    // only on demand when something is printed, like a traceback.
    char* python_redirect = "\
import sys\n\
import wx\n\
output = wx.PyOnDemandOutputWindow()\n\
sys.stdin = sys.stderr = output\n\
";
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    PyRun_SimpleString(python_redirect);
    wxPyEndBlockThreads(blocked);
}




char* python_code2 = "\
import sys\n\
sys.path.append('.')\n\
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
    wxPyBlock_t blocked = wxPyBeginBlockThreads();

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
        wxPyEndBlockThreads(blocked);
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
        bool success = wxPyConvertSwigPtr(result, (void**)&window, _T("wxWindow"));
        wxASSERT_MSG(success, _T("Returned object was not a wxWindow!"));
        Py_DECREF(result);
    }

    // Release the python objects we still have
    Py_DECREF(globals);
    Py_DECREF(tuple);

    // Finally, after all Python stuff is done, release the GIL
    wxPyEndBlockThreads(blocked);

    return window;
}


//----------------------------------------------------------------------






