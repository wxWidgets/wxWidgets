/////////////////////////////////////////////////////////////////////////////
// Name:        wx.i
// Purpose:     SWIG interface file for a python wxWindows module
//
// Author:      Robin Dunn
//
// Created:     5/22/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module wx


%{
#include "helpers.h"
%}

//----------------------------------------------------------------------
// This is where we include the other wrapper definition files for SWIG
//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i
%include _defs.i

%include pointer.i

%import misc.i
%import misc2.i
%import windows.i
%import events.i
%import gdi.i
%import mdi.i
%import controls.i
%import controls2.i
%import windows2.i
%import cmndlgs.i
%import stattool.i
%import frames.i
%import windows3.i
%import image.i
%import printfw.i
%import sizers.i
%import streams.i
%import filesys.i
%import utils.i
%import fonts.i


%native(_wxStart)           __wxStart;
%native(_wxSetDictionary)   __wxSetDictionary;

//---------------------------------------------------------------------------

%readonly
wxPoint     wxDefaultPosition;
wxSize      wxDefaultSize;
%readwrite

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum {
    wxPYAPP_ASSERT_SUPPRESS  = 1,
    wxPYAPP_ASSERT_EXCEPTION = 2,
    wxPYAPP_ASSERT_DIALOG    = 4,
    wxPYAPP_ASSERT_LOG       = 8
};


class wxPyApp : public wxEvtHandler {
public:
    %addmethods {
        wxPyApp() {
            wxPythonApp = new wxPyApp();
            return wxPythonApp;
        }
    }

    ~wxPyApp();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyApp)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"


    wxString GetAppName();
    wxString GetClassName();
    bool GetExitOnFrameDelete();
    int GetPrintMode();
    wxWindow * GetTopWindow();
    wxString GetVendorName();
    bool GetUseBestVisual();

    void Dispatch();
    void ExitMainLoop();
    bool Initialized();
    int  MainLoop();
    bool Pending();
    bool ProcessIdle();
    bool Yield(bool onlyIfNeeded = FALSE);

    void SetAppName(const wxString& name);
    void SetClassName(const wxString& name);
    void SetExitOnFrameDelete(bool flag);
    void SetPrintMode(int mode);
    void SetTopWindow(wxWindow* window);
    void SetVendorName(const wxString& name);
    void SetUseBestVisual(bool flag);

    int  GetAssertMode();
    void SetAssertMode(int mode);


    static bool GetMacDefaultEncodingIsPC();
    static bool GetMacSupportPCMenuShortcuts();
    static long GetMacAboutMenuItemId();
    static long GetMacPreferencesMenuItemId();
    static long GetMacExitMenuItemId();
    static wxString GetMacHelpMenuTitleName();

    static void SetMacDefaultEncodingIsPC(bool val);
    static void SetMacSupportPCMenuShortcuts(bool val);
    static void SetMacAboutMenuItemId(long val);
    static void SetMacPreferencesMenuItemId(long val);
    static void SetMacExitMenuItemId(long val);
    static void SetMacHelpMenuTitleName(const wxString& val);
};

%inline %{
    wxPyApp* wxGetApp() {
        //return wxPythonApp;
        return (wxPyApp*)wxTheApp;
    }
%}


//----------------------------------------------------------------------
// this is used to cleanup after wxWindows when Python shuts down.

%inline %{
    void wxApp_CleanUp() {
        __wxCleanup();
    }
%}

//----------------------------------------------------------------------
// This code gets added to the module initialization function
//----------------------------------------------------------------------

%{


extern "C" SWIGEXPORT(void) initwindowsc();
extern "C" SWIGEXPORT(void) initwindows2c();
extern "C" SWIGEXPORT(void) initeventsc();
extern "C" SWIGEXPORT(void) initmiscc();
extern "C" SWIGEXPORT(void) initmisc2c();
extern "C" SWIGEXPORT(void) initgdic();
extern "C" SWIGEXPORT(void) initmdic();
extern "C" SWIGEXPORT(void) initcontrolsc();
extern "C" SWIGEXPORT(void) initcontrols2c();
extern "C" SWIGEXPORT(void) initcmndlgsc();
extern "C" SWIGEXPORT(void) initstattoolc();
extern "C" SWIGEXPORT(void) initframesc();
extern "C" SWIGEXPORT(void) initwindows3c();
extern "C" SWIGEXPORT(void) initimagec();
extern "C" SWIGEXPORT(void) initprintfwc();
extern "C" SWIGEXPORT(void) initsizersc();
extern "C" SWIGEXPORT(void) initclip_dndc();
extern "C" SWIGEXPORT(void) initstreamsc();
extern "C" SWIGEXPORT(void) initfilesysc();
extern "C" SWIGEXPORT(void) initutilsc();
extern "C" SWIGEXPORT(void) initfontsc();



// Export a C API in a struct.  Other modules will be able to load this from
// the wxc module and will then have safe access to these functions, even if
// in another shared library.
static wxPyCoreAPI API = {
    (p_SWIG_MakePtr_t)SWIG_MakePtr,
    (p_SWIG_GetPtr_t)SWIG_GetPtr,
    (p_SWIG_GetPtrObj_t)SWIG_GetPtrObj,
    (p_SWIG_RegisterMapping_t)SWIG_RegisterMapping,
    (p_SWIG_addvarlink_t)SWIG_addvarlink,
    (p_SWIG_newvarlink_t)SWIG_newvarlink,

    wxPyBeginAllowThreads,
    wxPyEndAllowThreads,
    wxPyBeginBlockThreads,
    wxPyEndBlockThreads,

    wxPyConstructObject,
    wxPy_ConvertList,

    wxString_in_helper,
    Py2wxString,
    wx2PyString,

    byte_LIST_helper,
    int_LIST_helper,
    long_LIST_helper,
    string_LIST_helper,
    wxPoint_LIST_helper,
    wxBitmap_LIST_helper,
    wxString_LIST_helper,
    wxAcceleratorEntry_LIST_helper,

    wxSize_helper,
    wxPoint_helper,
    wxRealPoint_helper,
    wxRect_helper,
    wxColour_helper,
    wxPoint2DDouble_helper,

    wxPyCBH_setCallbackInfo,
    wxPyCBH_findCallback,
    wxPyCBH_callCallback,
    wxPyCBH_callCallbackObj,
    wxPyCBH_delete,

    wxPyClassExists,
    wxPyMake_wxObject,
    wxPyMake_wxSizer,
    wxPyPtrTypeMap_Add,
    wxArrayString2PyList_helper,
    wxArrayInt2PyList_helper,

    wxPyClientData_dtor,
    wxPyUserData_dtor,
    wxPyOORClientData_dtor,

    wxPyCBInputStream_create
};


%}



%init %{
    // Make our API structure a CObject so other modules can import it
    // from this module.
    PyObject* v = PyCObject_FromVoidPtr(&API, NULL);
    PyDict_SetItemString(d,"wxPyCoreAPI", v);
    Py_XDECREF(v);


    __wxPreStart(d);     // initialize the GUI toolkit, if needed.


        // Since these modules are all linked together, initialize them now
        // because Python won't be able to find their shared library files,
        // (since there isn't any.)
    initwindowsc();
    initwindows2c();
    initeventsc();
    initmiscc();
    initmisc2c();
    initgdic();
    initmdic();
    initcontrolsc();
    initcontrols2c();
    initcmndlgsc();
    initstattoolc();
    initframesc();
    initwindows3c();
    initimagec();
    initprintfwc();
    initsizersc();
    initclip_dndc();
    initstreamsc();
    initfilesysc();
    initutilsc();
    initfontsc();


    // Although these are redfined in __version__ they need to be here too so
    // that an assert can be done to ensure that the wxPython and the wxWindows
    // versions match.
    PyDict_SetItemString(d,"wxMAJOR_VERSION", PyInt_FromLong((long)wxMAJOR_VERSION ));
    PyDict_SetItemString(d,"wxMINOR_VERSION", PyInt_FromLong((long)wxMINOR_VERSION ));
    PyDict_SetItemString(d,"wxRELEASE_VERSION", PyInt_FromLong((long)wxRELEASE_NUMBER ));

%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_extras.py";


