/////////////////////////////////////////////////////////////////////////////
// Name:        helpers.cpp
// Purpose:     Helper functions/classes for the wxPython extension module
//
// Author:      Robin Dunn
//
// Created:     7/1/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#undef DEBUG
#include <Python.h>
#include "helpers.h"

#ifdef __WXGTK__
#ifdef USE_GDK_IMLIB
#include "gdk_imlib/gdk_imlib.h"
#endif
#endif

//---------------------------------------------------------------------------

//wxHashTable*  wxPyWindows = NULL;


wxPoint     wxPyDefaultPosition;        //wxDefaultPosition);
wxSize      wxPyDefaultSize;            //wxDefaultSize);
wxString    wxPyEmptyStr("");



//----------------------------------------------------------------------
// Class for implementing the wxp main application shell.
//----------------------------------------------------------------------

wxPyApp *wxPythonApp = NULL;            // Global instance of application object


// This one isn't acutally called...  See __wxStart()
bool wxPyApp::OnInit(void) {
    return false;
}

int  wxPyApp::MainLoop(void) {
    int retval = wxApp::MainLoop();
    AfterMainLoop();
    return retval;
}

void wxPyApp::AfterMainLoop(void) {
    // more stuff from wxEntry...
    if (wxPythonApp->GetTopWindow()) {
        // Forcibly delete the window.
        if (wxPythonApp->GetTopWindow()->IsKindOf(CLASSINFO(wxFrame)) ||
            wxPythonApp->GetTopWindow()->IsKindOf(CLASSINFO(wxDialog))) {

            wxPythonApp->GetTopWindow()->Close(TRUE);
            wxPythonApp->DeletePendingObjects();
        }
        else {
            delete wxPythonApp->GetTopWindow();
            wxPythonApp->SetTopWindow(NULL);
        }
    }

    wxPythonApp->OnExit();
#ifdef __WXMSW__
    wxApp::CleanUp();
#endif
#ifdef __WXGTK__
    wxApp::CommonCleanUp();
#endif
    delete wxPythonApp;
}


//----------------------------------------------------------------------
// a few native methods to add to the module
//----------------------------------------------------------------------


// Start the user application, user App's OnInit method is a parameter here
PyObject* __wxStart(PyObject* /* self */, PyObject* args)
{
    PyObject*   onInitFunc = NULL;
    PyObject*   arglist;
    PyObject*   result;
    long        bResult;

    if (!PyArg_ParseTuple(args, "O", &onInitFunc))
        return NULL;

    // This is where we pick up one part of the wxEntry functionality...
    // the rest is in AfterMainLoop.
#ifdef __WXMSW__
    wxPythonApp->argc = 0;
    wxPythonApp->argv = NULL;
    wxPythonApp->OnInitGui();
#endif
#ifdef __WXGTK__
    wxTheApp->argc = 0;
    wxTheApp->argv = NULL;

    gtk_init( &wxTheApp->argc, &wxTheApp->argv );

#ifdef USE_GDK_IMLIB

    gdk_imlib_init();

    gtk_widget_push_visual(gdk_imlib_get_visual());

    gtk_widget_push_colormap(gdk_imlib_get_colormap());

#endif

    wxApp::CommonInit();

    wxTheApp->OnInitGui();

#endif


    // Call the Python App's OnInit function
    arglist = PyTuple_New(0);
    result = PyEval_CallObject(onInitFunc, arglist);
    if (!result) {
        PyErr_Print();
        exit(1);
    }

    if (! PyInt_Check(result)) {
        PyErr_SetString(PyExc_TypeError, "OnInit should return a boolean value");
        return NULL;
    }
    bResult = PyInt_AS_LONG(result);
    if (! bResult) {
        wxPythonApp->DeletePendingObjects();
        wxPythonApp->OnExit();
#ifdef __WXMSW__
    wxApp::CleanUp();
#endif
#ifdef __WXGTK__
    wxApp::CommonCleanUp();
#endif
        PyErr_SetString(PyExc_SystemExit, "OnInit returned false, exiting...");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}


//PyObject* __wxMainLoop(PyObject* /* self */, PyObject* /* args */)
//{
//    wxPythonApp->MainLoop();
//    if (wxPythonApp->wx_frame) {
//        wxPythonApp->wx_frame->GetEventHandler()->OnClose();
//        delete wxPythonApp->wx_frame;
//    }
//    wxCleanUp();

//    Py_INCREF(Py_None);
//    return Py_None;
//}


//PyObject* __wxExitMainLoop(PyObject* /* self */, PyObject* /* args */)
//{
//    wxPythonApp->ExitMainLoop();
//    Py_INCREF(Py_None);
//    return Py_None;
//}



//PyObject* __wxAddCallback(PyObject* /* self */, PyObject* args)
//{
//    char*           swigPtr;
//    char*           name;
//    PyObject*       callback;
//    wxWindow*       win;
//    wxPyEvtHandlers* evtHdlr;

//    if (!PyArg_ParseTuple(args, "ssO", &swigPtr, &name, &callback))
//        return NULL;

//    if (!PyCallable_Check(callback)) {
//        PyErr_SetString(PyExc_TypeError, "Expected a callable object.");
//        return NULL;
//    }

//    if (SWIG_GetPtr(swigPtr, (void **)&win, "_wxWindow_p")) {
//        PyErr_SetString(PyExc_TypeError, "Expected class derived from wxWindow.");
//        return NULL;
//    }

//    evtHdlr = (wxPyEvtHandlers*)win->GetClientData();
//    if (! evtHdlr->addCallback(name, callback)) {
//        PyErr_SetString(PyExc_TypeError, "Unknown callback name.");
//        return NULL;
//    }

//    Py_INCREF(Py_None);
//    return Py_None;
//}



//PyObject* __wxSetWinEvtHdlr(PyObject* /* self */, PyObject* args)
//{
//    char*           swigPtr;
//    wxWindow*       win;
//    wxPyEvtHandlers* evtHdlr;

//    if (!PyArg_ParseTuple(args, "s", &swigPtr))
//        return NULL;

//    if (SWIG_GetPtr(swigPtr, (void **)&win, "_wxWindow_p")) {
//        PyErr_SetString(PyExc_TypeError, "Expected class derived from wxWindow.");
//        return NULL;
//    }

//    evtHdlr = new wxPyEvtHandlers;
//    win->SetClientData((char*)evtHdlr);

//    Py_INCREF(Py_None);
//    return Py_None;
//}



//PyObject* __wxDelWinEvtHdlr(PyObject* /* self */, PyObject* args)
//{
//    char*               swigPtr;
//    wxWindow*           win;
//    wxPyEvtHandlers*    evtHdlr;

//    if (!PyArg_ParseTuple(args, "s", &swigPtr))
//        return NULL;

//    if (SWIG_GetPtr(swigPtr, (void **)&win, "_wxWindow_p")) {
//        PyErr_SetString(PyExc_TypeError, "Expected class derived from wxWindow.");
//        return NULL;
//    }

//    evtHdlr = (wxPyEvtHandlers*)win->GetClientData();
//    printf("__wxDelWinEvtHdlr: %p\n", evtHdlr);
//    delete evtHdlr;

//    Py_INCREF(Py_None);
//    return Py_None;
//}



PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject* /* self */, PyObject* args)
{

    if (!PyArg_ParseTuple(args, "O", &wxPython_dict))
        return NULL;

    if (!PyDict_Check(wxPython_dict)) {
        PyErr_SetString(PyExc_TypeError, "_wxSetDictionary must have dictionary object!");
        return NULL;
    }
#ifdef __WXMOTIF__
#define wxPlatform "__MOTIF__"
#endif
#ifdef __WXGTK__
#define wxPlatform "__GTK__"
#endif
#if defined(__WIN32__) || defined(__WXMSW__)
#define wxPlatform "__WIN32__"
#endif
#ifdef __WXMAC__
#define wxPlatform "__MAC__"
#endif

    PyDict_SetItemString(wxPython_dict, "wxPlatform", PyString_FromString(wxPlatform));

    Py_INCREF(Py_None);
    return Py_None;
}


//---------------------------------------------------------------------------


static
PyObject* wxPyConstructObject(void* ptr, char* className)
{
    char    buff[64];               // should be big enough...
    char    swigptr[64];

    sprintf(buff, "_%s_p", className);
    SWIG_MakePtr(swigptr, ptr, buff);

    sprintf(buff, "%sPtr", className);
    PyObject* classobj = PyDict_GetItemString(wxPython_dict, buff);
    if (! classobj) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject* arg = Py_BuildValue("(s)", swigptr);
    PyObject* obj = PyInstance_New(classobj, arg, NULL);
    Py_DECREF(arg);

    return obj;
}


// This function is used for all events destined for Python event handlers.
void wxPyCallback::EventThunker(wxEvent& event) {
    wxPyCallback*   cb = (wxPyCallback*)event.m_callbackUserData;
    PyObject*       func = cb->m_func;
    PyObject*       result;
    PyObject*       arg;
    PyObject*       tuple;

    arg = wxPyConstructObject((void*)&event, event.GetClassInfo()->GetClassName());

    tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(tuple, 0, arg);
    result = PyEval_CallObject(func, tuple);
    Py_DECREF(arg);
    Py_DECREF(tuple);
    if (result) {
        Py_DECREF(result);
        PyErr_Clear();
    } else {
        PyErr_Print();
    }
}


//---------------------------------------------------------------------------

wxPyMenu::wxPyMenu(const wxString& title, PyObject* _func)
    : wxMenu(title, (wxFunction)(func ? MenuCallback : NULL)), func(0) {

    if (_func) {
        func = _func;
        Py_INCREF(func);
    }
}

wxPyMenu::~wxPyMenu() {
    if (func)
        Py_DECREF(func);
}


void wxPyMenu::MenuCallback(wxMenu& menu, wxCommandEvent& evt) {
    PyObject* evtobj  = wxPyConstructObject((void*)&evt, "wxCommandEvent");
    PyObject* menuobj = wxPyConstructObject((void*)&menu, "wxMenu");
    if (PyErr_Occurred()) {
        // bail out if a problem
        PyErr_Print();
        return;
    }
    // Now call the callback...
    PyObject* func = ((wxPyMenu*)&menu)->func;
    PyObject* args = Py_BuildValue("(OO)", menuobj, evtobj);
    PyObject* res  = PyEval_CallObject(func, args);
    Py_DECREF(args);
    Py_DECREF(res);
    Py_DECREF(evtobj);
    Py_DECREF(menuobj);
}


//---------------------------------------------------------------------------

wxPyTimer::wxPyTimer(PyObject* callback) {
    func = callback;
    Py_INCREF(func);
}

wxPyTimer::~wxPyTimer() {
    Py_DECREF(func);
}

void wxPyTimer::Notify() {
    PyObject*   result;
    PyObject*   args = Py_BuildValue("()");

    result = PyEval_CallObject(func, args);
    Py_DECREF(args);
    if (result) {
        Py_DECREF(result);
        PyErr_Clear();
    } else {
        PyErr_Print();
    }
}



//----------------------------------------------------------------------
// wxPyEvtHandlers class
//----------------------------------------------------------------------

//wxPyEvtHandlers::wxPyEvtHandlers()
//    :   pyOnActivate(0),
//        pyOnChar(0),
//        pyOnCharHook(0),
//        pyOnClose(0),
//        pyOnCommand(0),
//        pyOnDropFiles(0),
//        pyOnDefaultAction(0),
//        pyOnEvent(0),
//        pyOnInitMenuPopup(0),
//        pyOnKillFocus(0),
//        pyOnMenuCommand(0),
//        pyOnMenuSelect(0),
//        pyOnMove(0),
//        pyOnPaint(0),
//        pyOnScroll(0),
//        pyOnSetFocus(0),
//        pyOnSize(0),
//        pyOnSysColourChange(0),
//        pyOnLeftClick(0),
//        pyOnMouseEnter(0),
//        pyOnRightClick(0),
//        pyOnDoubleClickSash(0),
//        pyOnUnsplit(0)
//{
//}


//wxPyEvtHandlers::~wxPyEvtHandlers()
//{
//    Py_XDECREF(pyOnActivate);
//    Py_XDECREF(pyOnChar);
//    Py_XDECREF(pyOnCharHook);
//    Py_XDECREF(pyOnClose);
//    Py_XDECREF(pyOnCommand);
//    Py_XDECREF(pyOnDropFiles);
//    Py_XDECREF(pyOnDefaultAction);
//    Py_XDECREF(pyOnEvent);
//    Py_XDECREF(pyOnInitMenuPopup);
//    Py_XDECREF(pyOnKillFocus);
//    Py_XDECREF(pyOnMenuCommand);
//    Py_XDECREF(pyOnMenuSelect);
//    Py_XDECREF(pyOnMove);
//    Py_XDECREF(pyOnPaint);
//    Py_XDECREF(pyOnScroll);
//    Py_XDECREF(pyOnSetFocus);
//    Py_XDECREF(pyOnSize);
//    Py_XDECREF(pyOnSysColourChange);
//    Py_XDECREF(pyOnLeftClick);
//    Py_XDECREF(pyOnMouseEnter);
//    Py_XDECREF(pyOnRightClick);
//    Py_XDECREF(pyOnDoubleClickSash);
//    Py_XDECREF(pyOnUnsplit);

//    wxNode* node = cleanupList.First();
//    while (node) {
//        delete (wxPyEvtHandlers*)node->Data();
//        delete node;
//        node = cleanupList.First();
//    }

//    node = decrefList.First();
//    while (node) {
//        PyObject* obj = (PyObject*)node->Data();
//        Py_DECREF(obj);
//        delete node;
//        node = decrefList.First();
//    }
////    printf("~wxPyEvtHandlers: %p\n", this);
//}

////----------------------------------------------------------------------

//Bool wxPyEvtHandlers::addCallback(char* name, PyObject* callback)
//{
//    Py_INCREF(callback);

//    if (strcmp(name, "OnActivate") == 0) {
//        pyOnActivate = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnChar") == 0) {
//        pyOnChar = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnCharHook") == 0) {
//        pyOnCharHook = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnClose") == 0) {
//        pyOnClose = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnCommand") == 0) {
//        pyOnCommand = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnDropFiles") == 0) {
//        pyOnDropFiles = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnDefaultAction") == 0) {
//        pyOnDefaultAction = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnEvent") == 0) {
//        pyOnEvent = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnInitMenuPopup") == 0) {
//        pyOnInitMenuPopup = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnKillFocus") == 0) {
//        pyOnKillFocus = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnMenuCommand") == 0) {
//        pyOnMenuCommand = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnMenuSelect") == 0) {
//        pyOnMenuSelect = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnMove") == 0) {
//        pyOnMove = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnPaint") == 0) {
//        pyOnPaint = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnScroll") == 0) {
//        pyOnScroll = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnSetFocus") == 0) {
//        pyOnSetFocus = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnSize") == 0) {
//        pyOnSize = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnSysColourChange") == 0) {
//        pyOnSysColourChange = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnLeftClick") == 0) {
//        pyOnLeftClick = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnMouseEnter") == 0) {
//        pyOnMouseEnter = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnRightClick") == 0) {
//        pyOnRightClick = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnDoubleClickSash") == 0) {
//        pyOnDoubleClickSash = callback;
//        return TRUE;
//    }
//    if (strcmp(name, "OnUnsplit") == 0) {
//        pyOnUnsplit = callback;
//        return TRUE;
//    }

//    // If we get here, there was no match.
//    Py_DECREF(callback);
//    return FALSE;
//}


////----------------------------------------------------------------------
//// Helpers to assist in calling the python callable objects
////----------------------------------------------------------------------

//PyObject* wxPyEvtHandlers::constructObject(void* ptr, char* className)
//{
//    char    buff[64];               // should be big enough...
//    char    swigptr[64];

//    sprintf(buff, "_%s_p", className);
//    SWIG_MakePtr(swigptr, ptr, buff);

//    sprintf(buff, "%sPtr", className);
//    PyObject* classobj = PyDict_GetItemString(wxPython_dict, buff);
//    PyObject* arg = Py_BuildValue("(s)", swigptr);
//    PyObject* obj = PyInstance_New(classobj, arg, NULL);
//    Py_DECREF(arg);

//    return obj;
//}



//int wxPyEvtHandlers::callFunc(PyObject* func, PyObject* arglist)
//{
//    PyObject*   result;
//    int         retval = FALSE;

//    result = PyEval_CallObject(func, arglist);
//    Py_DECREF(arglist);
//    if (result) {                       // Assumes an integer return type...
//        retval = PyInt_AsLong(result);
//        Py_DECREF(result);
//        PyErr_Clear();                  // forget about it if it's not...
//    } else {
//        PyErr_Print();
//    }
//    return retval;
//}

////---------------------------------------------------------------------------
//// Methods and helpers of the wxPy* classes
////---------------------------------------------------------------------------

//IMP_OnActivate(wxFrame, wxPyFrame);
//IMP_OnCharHook(wxFrame, wxPyFrame);
//IMP_OnClose(wxFrame, wxPyFrame);
//IMP_OnMenuCommand(wxFrame, wxPyFrame);
//IMP_OnMenuSelect(wxFrame, wxPyFrame);
//IMP_OnSize(wxFrame, wxPyFrame);
//IMP_OnDropFiles(wxFrame, wxPyFrame);

//IMP_OnChar(wxCanvas, wxPyCanvas);
//IMP_OnEvent(wxCanvas, wxPyCanvas);
//IMP_OnPaint(wxCanvas, wxPyCanvas);
//IMP_OnScroll(wxCanvas, wxPyCanvas);
//IMP_OnDropFiles(wxCanvas, wxPyCanvas);

//IMP_OnChar(wxPanel, wxPyPanel);
//IMP_OnEvent(wxPanel, wxPyPanel);
//IMP_OnPaint(wxPanel, wxPyPanel);
//IMP_OnScroll(wxPanel, wxPyPanel);
//IMP_OnCommand(wxPanel, wxPyPanel);
//IMP_OnDefaultAction(wxPanel, wxPyPanel);
//IMP_OnDropFiles(wxPanel, wxPyPanel);

//IMP_OnChar(wxTextWindow, wxPyTextWindow);
//IMP_OnDropFiles(wxTextWindow, wxPyTextWindow);

//IMP_OnCharHook(wxDialogBox, wxPyDialogBox);
//IMP_OnClose(wxDialogBox, wxPyDialogBox);
//IMP_OnSize(wxDialogBox, wxPyDialogBox);
//IMP_OnDropFiles(wxDialogBox, wxPyDialogBox);
//IMP_OnChar(wxDialogBox, wxPyDialogBox);
//IMP_OnEvent(wxDialogBox, wxPyDialogBox);
//IMP_OnPaint(wxDialogBox, wxPyDialogBox);
//IMP_OnScroll(wxDialogBox, wxPyDialogBox);
//IMP_OnCommand(wxDialogBox, wxPyDialogBox);
//IMP_OnDefaultAction(wxDialogBox, wxPyDialogBox);

//IMP_OnChar(wxToolBar, wxPyToolBar);
//IMP_OnEvent(wxToolBar, wxPyToolBar);
//IMP_OnPaint(wxToolBar, wxPyToolBar);
//IMP_OnScroll(wxToolBar, wxPyToolBar);
//IMP_OnCommand(wxToolBar, wxPyToolBar);
//IMP_OnDefaultAction(wxToolBar, wxPyToolBar);
//IMP_OnDropFiles(wxToolBar, wxPyToolBar);
//IMP_OnMouseEnter(wxToolBar, wxPyToolBar);
//IMP_OnRightClick(wxToolBar, wxPyToolBar);

//IMP_OnChar(wxButtonBar, wxPyButtonBar);
//IMP_OnEvent(wxButtonBar, wxPyButtonBar);
//IMP_OnPaint(wxButtonBar, wxPyButtonBar);
//IMP_OnScroll(wxButtonBar, wxPyButtonBar);
//IMP_OnCommand(wxButtonBar, wxPyButtonBar);
//IMP_OnDefaultAction(wxButtonBar, wxPyButtonBar);
//IMP_OnDropFiles(wxButtonBar, wxPyButtonBar);
//IMP_OnMouseEnter(wxButtonBar, wxPyButtonBar);
//IMP_OnRightClick(wxButtonBar, wxPyButtonBar);

//IMP_OnDoubleClickSash(wxSplitterWindow, wxPySplitterWindow);
//IMP_OnUnsplit(wxSplitterWindow, wxPySplitterWindow);



//Bool wxPyToolBar::OnLeftClick(int a, int b) {
//    wxPyEvtHandlers* peh=(wxPyEvtHandlers*)GetClientData();
//    if (peh->pyOnLeftClick)
//        return peh->callFunc(peh->pyOnLeftClick, Py_BuildValue("(ii)",a,b));
//    else {
//        // If there is no Python callback, redirect the request to
//        // the OnMenuCommand of the parent frame.
//        wxFrame* frame = (wxFrame*)GetParent();
//        frame->OnMenuCommand(a);
//        return TRUE;
//    }
////    else
////        return wxToolBar::OnLeftClick(a,b);
//}
//Bool wxPyToolBar::baseclass_OnLeftClick(int a, int b) {
//    return wxToolBar::OnLeftClick(a,b);
//}


//Bool wxPyButtonBar::OnLeftClick(int a, int b) {
//    wxPyEvtHandlers* peh=(wxPyEvtHandlers*)GetClientData();
//    if (peh->pyOnLeftClick)
//        return peh->callFunc(peh->pyOnLeftClick, Py_BuildValue("(ii)",a,b));
//    else {
//        // If there is no Python callback, redirect the request to
//        // the OnMenuCommand of the parent frame.
//        wxFrame* frame = (wxFrame*)GetParent();
//        frame->OnMenuCommand(a);
//        return TRUE;
//    }
//}
//Bool wxPyButtonBar::baseclass_OnLeftClick(int a, int b) {
//    return wxButtonBar::OnLeftClick(a,b);
//}



//wxPyMenu::wxPyMenu(PyObject* _func)
//    : wxMenu(NULL, (wxFunction)(func ? MenuCallback : NULL)), func(0) {

//    if (_func) {
//        func = _func;
//        Py_INCREF(func);
//    }
//}

//wxPyMenu::~wxPyMenu() {
//    if (func)
//        Py_DECREF(func);
//}


//void wxPyMenu::MenuCallback(wxWindow& win, wxCommandEvent& evt) {
//    wxPyEvtHandlers* peh= new wxPyEvtHandlers;  // Used for the helper methods
//    PyObject* evtobj = peh->constructObject((void*)&evt, "wxCommandEvent");
//    PyObject* winobj = peh->constructObject((void*)&win, "wxWindow");
//    if (PyErr_Occurred()) {
//        // bail out if a problem
//        PyErr_Print();
//        delete peh;
//        return;
//    }
//    // Now call the callback...
//    PyObject* func = ((wxPyMenu*)&win)->func;
//    peh->callFunc(func, Py_BuildValue("(OO)", winobj, evtobj));
//    Py_DECREF(evtobj);
//    Py_DECREF(winobj);
//    delete peh;
//}



//wxPyTimer::wxPyTimer(PyObject* callback) {
//    func = callback;
//    Py_INCREF(func);
//}

//wxPyTimer::~wxPyTimer() {
//    Py_DECREF(func);
//}

//void wxPyTimer::Notify() {
//    wxPyEvtHandlers* peh= new wxPyEvtHandlers;  // just for the helper methods
//    peh->callFunc(func, Py_BuildValue("()"));
//    delete peh;
//}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Some helper functions for typemaps in my_typemaps.i, so they won't be
// imcluded in every file...


int* int_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    int* temp = new int[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (! PyInt_Check(o)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of integers.");
            return NULL;
        }
        temp[x] = PyInt_AsLong(o);
    }
    return temp;
}


long* long_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    long* temp = new long[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (! PyInt_Check(o)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of integers.");
            return NULL;
        }
        temp[x] = PyInt_AsLong(o);
    }
    return temp;
}


char** string_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    char** temp = new char*[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (! PyString_Check(o)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of strings.");
            return NULL;
        }
        temp[x] = PyString_AsString(o);
    }
    return temp;
}



wxPoint* wxPoint_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    wxPoint* temp = new wxPoint[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (PyString_Check(o)) {
            char*       st = PyString_AsString(o);
            wxPoint*    pt;
            if (SWIG_GetPtr(st,(void **) &pt,"_wxPoint_p")) {
                PyErr_SetString(PyExc_TypeError,"Expected _wxPoint_p.");
                return NULL;
            }
            temp[x] = *pt;
        }
        else if (PyTuple_Check(o)) {
            PyObject* o1 = PyTuple_GetItem(o, 0);
            PyObject* o2 = PyTuple_GetItem(o, 1);

            temp[x].x = PyInt_AsLong(o1);
            temp[x].y = PyInt_AsLong(o2);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Expected a list of 2-tuples or wxPoints.");
            return NULL;
        }
    }
    return temp;
}


wxBitmap** wxBitmap_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    wxBitmap** temp = new wxBitmap*[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (PyString_Check(o)) {
            char*       st = PyString_AsString(o);
            wxBitmap*    pt;
            if (SWIG_GetPtr(st,(void **) &pt,"_wxBitmap_p")) {
                PyErr_SetString(PyExc_TypeError,"Expected _wxBitmap_p.");
                return NULL;
            }
            temp[x] = pt;
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Expected a list of wxBitmaps.");
            return NULL;
        }
    }
    return temp;
}



wxString* wxString_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    wxString* temp = new wxString[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (! PyString_Check(o)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of strings.");
            return NULL;
        }
        temp[x] = PyString_AsString(o);
    }
    return temp;
}


#ifdef __WXMSW__
wxAcceleratorEntry* wxAcceleratorEntry_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count                = PyList_Size(source);
    wxAcceleratorEntry* temp = new wxAcceleratorEntry[count];
    if (! temp) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate temporary array");
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* o = PyList_GetItem(source, x);
        if (PyString_Check(o)) {
            char*               st = PyString_AsString(o);
            wxAcceleratorEntry* ae;
            if (SWIG_GetPtr(st,(void **) &ae,"_wxAcceleratorEntry_p")) {
                PyErr_SetString(PyExc_TypeError,"Expected _wxAcceleratorEntry_p.");
                return NULL;
            }
            temp[x] = *ae;
        }
        else if (PyTuple_Check(o)) {
            PyObject* o1 = PyTuple_GetItem(o, 0);
            PyObject* o2 = PyTuple_GetItem(o, 1);
            PyObject* o3 = PyTuple_GetItem(o, 2);

            temp[x].m_flags   = PyInt_AsLong(o1);
            temp[x].m_keyCode = PyInt_AsLong(o2);
            temp[x].m_command = PyInt_AsLong(o3);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Expected a list of 3-tuples or wxAcceleratorEntry objects.");
            return NULL;
        }
    }
    return temp;
}

#endif

//----------------------------------------------------------------------
// A WinMain for when wxWindows and Python are linked together in a single
// application, instead of as a dynamic module


//#if !defined(WIN_PYD) && defined(WIN32)

//extern "C" int Py_Main(int argc, char** argv);

//int APIENTRY WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR m_lpCmdLine,
//                    int nCmdShow )
//{

//    wxpCreateApp();

//        // Initialize wxWindows, but don't start the main loop
//    wxEntry(hInstance, hPrevInstance, m_lpCmdLine, nCmdShow, FALSE);

//    Py_Initialize();
//    PyObject *argvList = PyList_New(0);

//    char*   stderrfilename = "wxpstderr.log";
//    int     pyargc = 1;
//    char*   script = NULL;
//    int     argc = wxPythonApp->argc;
//    char**  argv = wxPythonApp->argv;

//    for (int i = 1; i < argc; i++) {
//        if (strncmp(argv[i], "wxpstderr=", 10) == 0)
//            stderrfilename = argv[i]+10;
//        else {
//            PyList_Append(argvList, PyString_FromString(argv[i]));
//            if (!script)
//                script = argv[i];
//            pyargc++;
//        }
//    }

//    PySys_SetObject("argv", argvList);

//#if 1
//    char buf[256];
////    //PyRun_SimpleString("import sys; sys.stdout=open('wxpstdout.log','w')");
//    sprintf(buf, "import sys; sys.stdout=sys.stderr=open('%s','w')", stderrfilename);
//    PyRun_SimpleString(buf);
//#endif

//    initwxPythonc();

//    if (script) {
//        FILE *fp = fopen(script, "r");
//        if (fp) {
//            PyRun_SimpleFile(fp, script);// This returns after wxpApp constructor
//            fclose(fp);
//        }
//        else {
//            char msg[256];
//            sprintf(msg, "Cannot open %s", script);
//            wxMessageBox(msg);
//        }
//    }
//    else
//        PyRun_SimpleString("import wxpide");

//    return 0;
//}


//#endif

//----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.4  1998/08/16 04:31:06  RD
// More wxGTK work.
//
// Revision 1.3  1998/08/15 07:36:36  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.2  1998/08/14 23:36:36  RD
// Beginings of wxGTK compatibility
//
// Revision 1.1  1998/08/09 08:25:51  RD
// Initial version
//
//
