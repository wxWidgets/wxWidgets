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


#ifdef __WXGTK__
#include "gtk/gtk.h"
#endif

#undef DEBUG
#include <Python.h>
#include "helpers.h"
#ifdef __WXMSW__
#include <wx/msw/private.h>
#undef FindWindow
#undef GetCharWidth
#undef LoadAccelerators
#undef GetClassInfo
#undef GetClassName
#endif
#include <wx/module.h>


//---------------------------------------------------------------------------

//wxHashTable*  wxPyWindows = NULL;


wxPoint     wxPyDefaultPosition;        //wxDefaultPosition);
wxSize      wxPyDefaultSize;            //wxDefaultSize);
wxString    wxPyEmptyStr("");



#ifdef __WXMSW__             // If building for win32...
//----------------------------------------------------------------------
// This gets run when the DLL is loaded.  We just need to save a handle.
//----------------------------------------------------------------------

BOOL WINAPI DllMain(
    HINSTANCE   hinstDLL,    // handle to DLL module
    DWORD       fdwReason,   // reason for calling function
    LPVOID      lpvReserved  // reserved
   )
{
    wxSetInstance(hinstDLL);
    return 1;
}
#endif

//----------------------------------------------------------------------
// Class for implementing the wxp main application shell.
//----------------------------------------------------------------------

wxPyApp *wxPythonApp = NULL;            // Global instance of application object


wxPyApp::wxPyApp() {
//    printf("**** ctor\n");
}

wxPyApp::~wxPyApp() {
//    printf("**** dtor\n");
}


// This one isn't acutally called...  See __wxStart()
bool wxPyApp::OnInit(void) {
    return false;
}

int  wxPyApp::MainLoop(void) {
    int retval = wxApp::MainLoop();
//#    AfterMainLoop();
    wxPythonApp->OnExit();  //#
    return retval;
}


//#  void wxPyApp::AfterMainLoop(void) {
//      // more stuff from wxEntry...

//      if (wxPythonApp->GetTopWindow()) {
//          // Forcibly delete the window.
//          if (wxPythonApp->GetTopWindow()->IsKindOf(CLASSINFO(wxFrame)) ||
//              wxPythonApp->GetTopWindow()->IsKindOf(CLASSINFO(wxDialog))) {

//              wxPythonApp->GetTopWindow()->Close(TRUE);
//              wxPythonApp->DeletePendingObjects();
//          }
//          else {
//              delete wxPythonApp->GetTopWindow();
//              wxPythonApp->SetTopWindow(NULL);
//          }
//      }
//  #ifdef __WXGTK__
//        wxPythonApp->DeletePendingObjects();
//  #endif

//      wxPythonApp->OnExit();
//      wxApp::CleanUp();
//  //    delete wxPythonApp;
//  }


//---------------------------------------------------------------------
// a few native methods to add to the module
//----------------------------------------------------------------------


// This is where we pick up the first part of the wxEntry functionality...
// The rest is in __wxStart and  AfterMainLoop.  This function is called when
// wxcmodule is imported.  (Before there is a wxApp object.)
void __wxPreStart()
{
    // Bail out if there is already windows created.  This means that the
    // toolkit has already been initialized, as in embedding wxPython in
    // a C++ wxWindows app.
    if (wxTopLevelWindows.Number() > 0)
        return;

#ifdef __WXMSW__
    wxApp::Initialize();
#endif

#ifdef __WXGTK__
    PyObject* sysargv = PySys_GetObject("argv");
    int argc = PyList_Size(sysargv);
    char** argv = new char*[argc+1];
    int x;
    for(x=0; x<argc; x++)
        argv[x] = PyString_AsString(PyList_GetItem(sysargv, x));
    argv[argc] = NULL;

    gtk_set_locale();
    if (!wxOKlibc()) wxConvCurrent = &wxConvLocal;
    gtk_init( &argc, &argv );
    wxSetDetectableAutoRepeat( TRUE );
    delete [] argv;

    wxApp::Initialize();     // may return FALSE. Should we check?
#endif

}


#ifdef WXP_WITH_THREAD
PyThreadState*  wxPyEventThreadState = NULL;
bool            wxPyInEvent = false;
#endif
static char* __nullArgv[1] = { 0 };



// Start the user application, user App's OnInit method is a parameter here
PyObject* __wxStart(PyObject* /* self */, PyObject* args)
{
    PyObject*   onInitFunc = NULL;
    PyObject*   arglist;
    PyObject*   result;
    long        bResult;

#ifdef WXP_WITH_THREAD
    wxPyEventThreadState = PyThreadState_Get();
#endif

    if (!PyArg_ParseTuple(args, "O", &onInitFunc))
        return NULL;

    if (wxTopLevelWindows.Number() > 0) {
        PyErr_SetString(PyExc_TypeError, "Only 1 wxApp per process!");
        return NULL;
    }


    // This is the next part of the wxEntry functionality...
    wxPythonApp->argc = 0;
    wxPythonApp->argv = NULL;
    wxPythonApp->OnInitGui();


    // Call the Python App's OnInit function
    arglist = PyTuple_New(0);
    result = PyEval_CallObject(onInitFunc, arglist);
    if (!result) {      // an exception was raised.
        return NULL;
    }

    if (! PyInt_Check(result)) {
        PyErr_SetString(PyExc_TypeError, "OnInit should return a boolean value");
        return NULL;
    }
    bResult = PyInt_AS_LONG(result);
    if (! bResult) {
        PyErr_SetString(PyExc_SystemExit, "OnInit returned false, exiting...");
        return NULL;
    }

#ifdef __WXGTK__
    wxTheApp->m_initialized = (wxTopLevelWindows.Number() > 0);
#endif

    Py_INCREF(Py_None);
    return Py_None;
}





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
#define wxPlatform "__WXMOTIF__"
#endif
#ifdef __WXQT__
#define wxPlatform "__WXQT__"
#endif
#ifdef __WXGTK__
#define wxPlatform "__WXGTK__"
#endif
#if defined(__WIN32__) || defined(__WXMSW__)
#define wxPlatform "__WXMSW__"
#endif
#ifdef __WXMAC__
#define wxPlatform "__WXMAC__"
#endif

    PyDict_SetItemString(wxPython_dict, "wxPlatform", PyString_FromString(wxPlatform));

    Py_INCREF(Py_None);
    return Py_None;
}


//---------------------------------------------------------------------------

PyObject* wxPyConstructObject(void* ptr, char* className)
{
    char    buff[64];               // should always be big enough...
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



wxPyCallback::wxPyCallback(PyObject* func) {
    m_func = func;
    Py_INCREF(m_func);
}

wxPyCallback::~wxPyCallback() {
#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_RestoreThread(wxPyEventThreadState);
#endif

    Py_DECREF(m_func);

#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_SaveThread();
#endif
}




// This function is used for all events destined for Python event handlers.
void wxPyCallback::EventThunker(wxEvent& event) {
    wxPyCallback*   cb = (wxPyCallback*)event.m_callbackUserData;
    PyObject*       func = cb->m_func;
    PyObject*       result;
    PyObject*       arg;
    PyObject*       tuple;


#ifdef WXP_WITH_THREAD
    PyEval_RestoreThread(wxPyEventThreadState);
    wxPyInEvent = true;
#endif
    arg = wxPyConstructObject((void*)&event, event.GetClassInfo()->GetClassName());

    tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(tuple, 0, arg);
    result = PyEval_CallObject(func, tuple);
    Py_DECREF(tuple);
    if (result) {
        Py_DECREF(result);
        PyErr_Clear();
    } else {
        PyErr_Print();
    }
#ifdef WXP_WITH_THREAD
    PyEval_SaveThread();
    wxPyInEvent = false;
#endif
}


//---------------------------------------------------------------------------

//  wxPyMenu::wxPyMenu(const wxString& title, PyObject* _func)
//      : wxMenu(title, (wxFunction)(func ? MenuCallback : NULL)), func(0) {

//      if (_func) {
//          func = _func;
//          Py_INCREF(func);
//      }
//  }

//  wxPyMenu::~wxPyMenu() {
//  #ifdef WXP_WITH_THREAD
//      //if (! wxPyInEvent)
//          PyEval_RestoreThread(wxPyEventThreadState);
//  #endif

//      if (func)
//          Py_DECREF(func);

//  #ifdef WXP_WITH_THREAD
//      //if (! wxPyInEvent)
//          PyEval_SaveThread();
//  #endif
//  }


//  void wxPyMenu::MenuCallback(wxMenu& menu, wxCommandEvent& evt) {
//      PyObject* evtobj;
//      PyObject* menuobj;
//      PyObject* func;
//      PyObject* args;
//      PyObject* res;

//  #ifdef WXP_WITH_THREAD
//      PyEval_RestoreThread(wxPyEventThreadState);
//      wxPyInEvent = true;
//  #endif
//      evtobj  = wxPyConstructObject((void*)&evt, "wxCommandEvent");
//      menuobj = wxPyConstructObject((void*)&menu, "wxMenu");
//      if (PyErr_Occurred()) {
//          // bail out if a problem
//          PyErr_Print();
//          goto done;
//      }
//      // Now call the callback...
//      func = ((wxPyMenu*)&menu)->func;
//      args = PyTuple_New(2);
//      PyTuple_SET_ITEM(args, 0, menuobj);
//      PyTuple_SET_ITEM(args, 1, evtobj);
//      res  = PyEval_CallObject(func, args);
//      Py_DECREF(args);
//      Py_XDECREF(res); /* In case res is a NULL pointer */
//   done:
//  #ifdef WXP_WITH_THREAD
//      PyEval_SaveThread();
//      wxPyInEvent = false;
//  #endif
//      return;
//  }


//---------------------------------------------------------------------------

wxPyTimer::wxPyTimer(PyObject* callback) {
    func = callback;
    Py_INCREF(func);
}

wxPyTimer::~wxPyTimer() {
#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_RestoreThread(wxPyEventThreadState);
#endif

    Py_DECREF(func);

#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_SaveThread();
#endif
}

void wxPyTimer::Notify() {
#ifdef WXP_WITH_THREAD
    PyEval_RestoreThread(wxPyEventThreadState);
    wxPyInEvent = true;
#endif
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
#ifdef WXP_WITH_THREAD
    PyEval_SaveThread();
    wxPyInEvent = false;
#endif
}


//----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPyEvent, wxCommandEvent)

wxPyEvent::wxPyEvent(wxEventType commandType, PyObject* userData)
    : wxCommandEvent(commandType), m_userData(Py_None)
{
    m_userData = userData;
    if (m_userData != Py_None) {
        Py_INCREF(m_userData);
    }
}


wxPyEvent::~wxPyEvent() {
#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_RestoreThread(wxPyEventThreadState);
#endif
    if (m_userData != Py_None) {
        Py_DECREF(m_userData);
        m_userData = Py_None;
    }
#ifdef WXP_WITH_THREAD
    //if (! wxPyInEvent)
        PyEval_SaveThread();
#endif
}


void wxPyEvent::SetUserData(PyObject* userData) {
    if (m_userData != Py_None) {
        Py_DECREF(m_userData);
        m_userData = Py_None;
    }
    m_userData = userData;
    if (m_userData != Py_None) {
        Py_INCREF(m_userData);
    }
}


PyObject* wxPyEvent::GetUserData() {
    return m_userData;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Some helper functions for typemaps in my_typemaps.i, so they won't be
// imcluded in every file...


byte* byte_LIST_helper(PyObject* source) {
    if (!PyList_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        return NULL;
    }
    int count = PyList_Size(source);
    byte* temp = new byte[count];
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
        temp[x] = (byte)PyInt_AsLong(o);
    }
    return temp;
}


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



//----------------------------------------------------------------------
//----------------------------------------------------------------------

wxPyCallbackHelper::wxPyCallbackHelper() {
    m_self = NULL;
    m_lastFound = NULL;
}


wxPyCallbackHelper::~wxPyCallbackHelper() {
#ifdef WXP_WITH_THREAD
    PyEval_RestoreThread(wxPyEventThreadState);
#endif

    Py_XDECREF(m_self);

#ifdef WXP_WITH_THREAD
    PyEval_SaveThread();
#endif
}

void wxPyCallbackHelper::setSelf(PyObject* self) {
    m_self = self;
    Py_INCREF(m_self);
}



bool wxPyCallbackHelper::findCallback(const wxString& name) {
    m_lastFound = NULL;
    if (m_self && PyObject_HasAttrString(m_self, (char*)name.c_str()))
        m_lastFound = PyObject_GetAttrString(m_self, (char*)name.c_str());

    return m_lastFound != NULL;
}


int wxPyCallbackHelper::callCallback(PyObject* argTuple) {
    PyObject*   result;
    int         retval = FALSE;

    result = callCallbackObj(argTuple);
    if (result) {                       // Assumes an integer return type...
        retval = PyInt_AsLong(result);
        Py_DECREF(result);
        PyErr_Clear();                  // forget about it if it's not...
    }
#ifdef WXP_WITH_THREAD
    PyEval_SaveThread();
#endif
    return retval;
}

// Invoke the Python callable object, returning the raw PyObject return
// value.  Caller should DECREF the return value and also call PyEval_SaveThread.
PyObject* wxPyCallbackHelper::callCallbackObj(PyObject* argTuple) {
#ifdef WXP_WITH_THREAD
    PyEval_RestoreThread(wxPyEventThreadState);
#endif
    PyObject*   result;

    result = PyEval_CallObject(m_lastFound, argTuple);
    Py_DECREF(argTuple);
    if (!result) {
        PyErr_Print();
    }
    return result;
}




//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



