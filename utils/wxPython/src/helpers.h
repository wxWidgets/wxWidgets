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

class wxPyApp: public wxApp
{
public:
    int  MainLoop(void);
    bool OnInit(void);
    void AfterMainLoop(void);
};

extern wxPyApp *wxPythonApp;

//----------------------------------------------------------------------

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


// Non-const versions to keep SWIG happy.
extern wxPoint  wxPyDefaultPosition;
extern wxSize   wxPyDefaultSize;
extern char*    wxPyPanelNameStr;
extern wxString wxPyEmptyStr;

//----------------------------------------------------------------------

class wxPyCallback : public wxObject {
public:
    wxPyCallback(PyObject* func)    { m_func = func; Py_INCREF(m_func); }
    ~wxPyCallback()                 { Py_DECREF(m_func); }

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
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.1  1998/08/09 08:25:51  RD
// Initial version
//
//

#endif

