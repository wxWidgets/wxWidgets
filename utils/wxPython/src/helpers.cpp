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

#ifdef __WXMSW__
#include <windows.h>
#undef FindWindow
#undef GetCharWidth
#undef LoadAccelerators
#endif

#undef DEBUG
#include <Python.h>
#include "helpers.h"

#ifdef __WXGTK__
#ifdef wxUSE_GDK_IMLIB
#include "gdk_imlib/gdk_imlib.h"
#endif
#endif

//---------------------------------------------------------------------------

//wxHashTable*  wxPyWindows = NULL;


wxPoint     wxPyDefaultPosition;        //wxDefaultPosition);
wxSize      wxPyDefaultSize;            //wxDefaultSize);
wxString    wxPyEmptyStr("");



#ifdef __WXMSW__             // If building for win32...
extern HINSTANCE wxhInstance;

//----------------------------------------------------------------------
// This gets run when the DLL is loaded.  We just need to save a handle.
//----------------------------------------------------------------------

BOOL WINAPI DllMain(
    HINSTANCE   hinstDLL,    // handle to DLL module
    DWORD       fdwReason,   // reason for calling function
    LPVOID      lpvReserved  // reserved
   )
{
    wxhInstance = hinstDLL;
    return 1;
}
#endif

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

#ifdef __WXMSW__
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
#endif
#ifdef __WXGTK__
      wxPythonApp->DeletePendingObjects();
#endif

    wxPythonApp->OnExit();
#ifdef __WXMSW__
    wxApp::CleanUp();
#endif
#ifdef __WXGTK__
    wxApp::CommonCleanUp();
#endif
    delete wxPythonApp;
}


//---------------------------------------------------------------------
// a few native methods to add to the module
//----------------------------------------------------------------------


// This is where we pick up the first part of the wxEntry functionality...
// The rest is in __wxStart and  AfterMainLoop.  Thi function is called when
// wxpc is imported.  (Before there is a wxApp object.)
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
    wxClassInfo::InitializeClasses();

    PyObject* sysargv = PySys_GetObject("argv");
    int argc = PyList_Size(sysargv);
    char** argv = new char*[argc+1];
    int x;
    for(x=0; x<argc; x++)
        argv[x] = PyString_AsString(PyList_GetItem(sysargv, x));
    argv[argc] = NULL;

    gtk_init( &argc, &argv );
    delete [] argv;

#ifdef wxUSE_GDK_IMLIB
    gdk_imlib_init();
    gtk_widget_push_visual(gdk_imlib_get_visual());
    gtk_widget_push_colormap(gdk_imlib_get_colormap());
#endif

    wxApp::CommonInit();
#endif

}



static char* __nullArgv[1] = { 0 };

// Start the user application, user App's OnInit method is a parameter here
PyObject* __wxStart(PyObject* /* self */, PyObject* args)
{
    PyObject*   onInitFunc = NULL;
    PyObject*   arglist;
    PyObject*   result;
    long        bResult;


    if (!PyArg_ParseTuple(args, "O", &onInitFunc))
        return NULL;

    if (wxTopLevelWindows.Number() > 0) {
        PyErr_SetString(PyExc_TypeError, "Only 1 wxApp per process!");
        return NULL;
    }


    // This is the next part of the wxEntry functionality...
    wxPythonApp->argc = 0;
    wxPythonApp->argv = __nullArgv;
    wxPythonApp->OnInitGui();


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


static
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
// Revision 1.10  1998/10/02 06:40:39  RD
// Version 0.4 of wxPython for MSW.
//
// Revision 1.9  1998/09/25 13:28:52  VZ
//
// USE_xxx constants renamed to wxUSE_xxx. This is an incompatible change, you
// must recompile everything after upgrading!
//
// Revision 1.8  1998/08/27 21:59:08  RD
// Some chicken-and-egg problems solved for wxPython on wxGTK
//
// Revision 1.7  1998/08/27 00:00:26  RD
// - more tweaks
// - have discovered some problems but not yet discovered solutions...
//
// Revision 1.6  1998/08/18 21:54:12  RD
//
// ifdef out some wxGTK specific code
//
// Revision 1.5  1998/08/18 19:48:17  RD
// more wxGTK compatibility things.
//
// It builds now but there are serious runtime problems...
//
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
