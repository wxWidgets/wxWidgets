/////////////////////////////////////////////////////////////////////////////
// Name:        helpers.h
// Purpose:     Helper functions/classes for the wxPython extenaion module
//
// Author:      Robin Dunn
//
// Created:     7/1/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxp_helpers__
#define __wxp_helpers__

#include <wx/wx.h>


//----------------------------------------------------------------------

// if we want to handle threads and Python threads are available...
#if defined(WXP_USE_THREAD) && defined(WITH_THREAD)

#define WXP_WITH_THREAD
#define wxPy_BEGIN_ALLOW_THREADS  Py_BEGIN_ALLOW_THREADS
#define wxPy_END_ALLOW_THREADS    Py_END_ALLOW_THREADS

#else  // no Python threads...
#undef WXP_WITH_THREAD
#define wxPy_BEGIN_ALLOW_THREADS
#define wxPy_END_ALLOW_THREADS
#endif


//----------------------------------------------------------------------

class wxPyApp: public wxApp
{
public:
    wxPyApp();
    ~wxPyApp();
    int  MainLoop(void);
    bool OnInit(void);
    void AfterMainLoop(void);
};

extern wxPyApp *wxPythonApp;

//----------------------------------------------------------------------

void      __wxPreStart();
PyObject* __wxStart(PyObject*, PyObject* args);

extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

extern wxHashTable*  wxPyWindows;  // keep track of all windows so we
                                   // don't accidentally delete them twice.

void wxPyEventThunker(wxObject*, wxEvent& event);

//----------------------------------------------------------------------


#ifndef SWIGCODE
extern "C" void SWIG_MakePtr(char *, void *, char *);
extern "C" char *SWIG_GetPtr(char *, void **, char *);
#endif


#ifdef _MSC_VER
# pragma warning(disable:4800)
#endif

typedef unsigned char byte;


// Non-const versions to keep SWIG happy.
extern wxPoint  wxPyDefaultPosition;
extern wxSize   wxPyDefaultSize;
extern wxString wxPyEmptyStr;

//----------------------------------------------------------------------

class wxPyCallback : public wxObject {
public:
    wxPyCallback(PyObject* func);
    ~wxPyCallback();

    void EventThunker(wxEvent& event);

    PyObject*   m_func;
};

//---------------------------------------------------------------------------

class wxPyMenu : public wxMenu {
public:
    wxPyMenu(const wxString& title = "", PyObject* func=NULL);
    ~wxPyMenu();

private:
    static void MenuCallback(wxMenu& menu, wxCommandEvent& evt);
    PyObject*   func;
};


//---------------------------------------------------------------------------

class wxPyTimer : public wxTimer {
public:
    wxPyTimer(PyObject* callback);
    ~wxPyTimer();

    void        Notify();

private:
    PyObject*   func;
};

//---------------------------------------------------------------------------

class wxPyEvent : public wxCommandEvent {
    DECLARE_DYNAMIC_CLASS(wxPyEvent)
public:
    wxPyEvent(wxEventType commandType = wxEVT_NULL, PyObject* userData = Py_None);
    ~wxPyEvent();

    void SetUserData(PyObject* userData);
    PyObject* GetUserData();

private:
    PyObject* m_userData;
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.7  1999/04/30 03:29:18  RD
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
// Revision 1.6.4.1  1999/03/27 23:29:14  RD
//
// wxPython 2.0b8
//     Python thread support
//     various minor additions
//     various minor fixes
//
// Revision 1.6  1998/11/25 08:45:26  RD
//
// Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
// Added events for wxGrid
// Other various fixes and additions
//
// Revision 1.5  1998/10/02 06:40:40  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.4  1998/08/27 21:59:09  RD
// Some chicken-and-egg problems solved for wxPython on wxGTK
//
// Revision 1.3  1998/08/16 04:31:09  RD
// More wxGTK work.
//
// Revision 1.2  1998/08/14 23:36:37  RD
// Beginings of wxGTK compatibility
//
// Revision 1.1  1998/08/09 08:25:51  RD
// Initial version
//
//

#endif

