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


%native(_wxStart)           __wxStart;
%native(_wxSetDictionary)   __wxSetDictionary;

//---------------------------------------------------------------------------


#define __version__ "0.0.0"   // The real value is now in build.py...

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

    ~wxPyApp();

    wxString GetAppName();
#ifdef __WXMSW__
    bool GetAuto3D();
#endif
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

    void SetAppName(const wxString& name);
#ifdef __WXMSW__
    void SetAuto3D(bool auto3D);
#endif
    void SetClassName(const wxString& name);
    void SetExitOnFrameDelete(bool flag);
    void SetPrintMode(int mode);
    void SetTopWindow(wxWindow* window);
    void SetVendorName(const wxString& name);
    void SetUseBestVisual(bool flag);
    wxIcon GetStdIcon(int which);


};


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
extern "C" SWIGEXPORT(void) initgridc();
%}



%init %{

    __wxPreStart();     // initialize the GUI toolkit, if needed.


        // Since these modules are all linked together, initialize them now
        // because python won't be able to find their shared library files,
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
    initgridc();
%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_extras.py";


