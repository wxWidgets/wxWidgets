/////////////////////////////////////////////////////////////////////////////
// Name:        wxp.i
// Purpose:     SWIG interface file for a python wxWindows module
//
// Author:      Robin Dunn
//
// Created:     5/22/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module wxp


%{

#ifdef __WXMSW__
#include <windows.h>
#undef FindWindow
#undef GetCharWidth
#undef LoadAccelerators
#endif


#include "helpers.h"
%}

//----------------------------------------------------------------------
// This is where we include the other wrapper definition files for SWIG
//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i
%include _defs.i

%import misc.i
%import windows.i
%import events.i
%import gdi.i
%import mdi.i
%import controls.i
%import controls2.i
%import windows2.i
%import cmndlgs.i



%native(_wxStart)           __wxStart;
%native(_wxSetDictionary)   __wxSetDictionary;

//---------------------------------------------------------------------------

#define __version__ "0.3.1"

wxPoint     wxPyDefaultPosition;
wxSize      wxPyDefaultSize;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxPyApp : public wxEvtHandler {
public:
    %addmethods {
        wxPyApp() {
            wxPythonApp = new wxPyApp();
            return wxPythonApp;
        }
    }


    wxString GetAppName();
#ifdef __WXMSW__
    bool GetAuto3D();
#endif
    wxString GetClassName();
    bool GetExitOnFrameDelete();
    int GetPrintMode();
    wxWindow * GetTopWindow();
    wxString GetVendorName();

    void Dispatch();
    void ExitMainLoop();
    bool Initialized();
    int  MainLoop();
    bool Pending();

    void SetAppName(const wxString& name);
#ifdef __WXMSW__
    void SetAuto3D(bool auto3D);
#endif
    void SetClassName(const wxString& name);
    void SetExitOnFrameDelete(bool flag);
    void SetPrintMode(int mode);
    void SetTopWindow(wxWindow* window);
    void SetVendorName(const wxString& name);

    // This one is wxPython specific.  If you override MainLoop,
    // call this when done.
    void AfterMainLoop();
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


%{
#ifdef __WXMSW__             // If building for win32...
extern HINSTANCE wxhInstance;

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

%}




//----------------------------------------------------------------------
// This code gets added to the module initialization function
//----------------------------------------------------------------------

%{
extern "C" SWIGEXPORT(void,initwindowsc)();
extern "C" SWIGEXPORT(void,initwindows2c)();
extern "C" SWIGEXPORT(void,initeventsc)();
extern "C" SWIGEXPORT(void,initmiscc)();
extern "C" SWIGEXPORT(void,initgdic)();
extern "C" SWIGEXPORT(void,initmdic)();
extern "C" SWIGEXPORT(void,initcontrolsc)();
extern "C" SWIGEXPORT(void,initcontrols2c)();
extern "C" SWIGEXPORT(void,initcmndlgsc)();

%}

%init %{
        // We don't want to run the wxEntry or OnInit yet, so we just do the
        // beginings of what it would have done...  See __wxStart() for the
        // rest.
#ifdef __WXMSW__
    wxApp::Initialize((WXHINSTANCE)wxhInstance);
#endif
#ifdef __WXGTK__
    wxApp::CommonInit();
#endif


//    wxPyWindows = new wxHashTable(wxKEY_INTEGER, 100);

        // Since these modules are all linked together, initialize them now
        // because python won't be able to find their shared library files,
        // (since there isn't any.)
    initwindowsc();
    initwindows2c();
    initeventsc();
    initmiscc();
    initgdic();
    initmdic();
    initcontrolsc();
    initcontrols2c();
    initcmndlgsc();
%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_extras.py";


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.3  1998/08/15 07:36:53  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.2  1998/08/14 23:36:49  RD
// Beginings of wxGTK compatibility
//
// Revision 1.1  1998/08/09 08:25:53  RD
// Initial version
//
//


