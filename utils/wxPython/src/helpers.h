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

#ifdef WXP_WITH_THREAD
extern PyThreadState*   wxPyEventThreadState;
extern bool             wxPyInEvent;
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
// This class holds an instance of a Python Shadow Class object and assists
// with looking up and invoking Python callback methods from C++ virtual
// method redirections.  For all classes which have virtuals which should be
// overridable in wxPython, a new subclass is created that contains a
// wxPyCallbackList.  This list is used to hold references to the Python
// methods.
//---------------------------------------------------------------------------

class wxPyCallbackHelper {
public:
    wxPyCallbackHelper();
    ~wxPyCallbackHelper();

    void        setSelf(PyObject* self);

    bool        findCallback(const wxString& name);
    int         callCallback(PyObject* argTuple);
    PyObject*   callCallbackObj(PyObject* argTuple);

private:
    PyObject*   m_self;
    PyObject*   m_lastFound;
};



//---------------------------------------------------------------------------
// These macros are used to implement the virtual methods that should
// redirect to a Python method if one exists.  The names designate the
// return type, if any as well as any parameter types.
//---------------------------------------------------------------------------

#define PYCALLBACK_BOOL_INTINT(PCLASS, CBNAME)                          \
    bool CBNAME(int a, int b) {                                         \
        if (m_myInst.findCallback(#CBNAME))                             \
            return m_myInst.callCallback(Py_BuildValue("(ii)",a,b));    \
        else                                                            \
            return PCLASS::CBNAME(a,b);                                 \
    }                                                                   \
    bool base_##CBNAME(int a, int b) {                                  \
        return PCLASS::CBNAME(a,b);                                     \
    }

//---------------------------------------------------------------------------

#define PYCALLBACK_BOOL_INT(PCLASS, CBNAME)                             \
    bool CBNAME(int a) {                                                \
        if (m_myInst.findCallback(#CBNAME))                             \
            return m_myInst.callCallback(Py_BuildValue("(i)",a));       \
        else                                                            \
            return PCLASS::CBNAME(a);                                   \
    }                                                                   \
    bool base_##CBNAME(int a) {                                         \
        return PCLASS::CBNAME(a);                                       \
    }

#define PYCALLBACK_BOOL_INT_pure(PCLASS, CBNAME)                        \
    bool CBNAME(int a) {                                                \
        if (m_myInst.findCallback(#CBNAME))                             \
            return m_myInst.callCallback(Py_BuildValue("(i)",a));       \
        else return false;                                              \
    }


//---------------------------------------------------------------------------

#define PYCALLBACK__(PCLASS, CBNAME)                                    \
    void CBNAME() {                                                     \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("()"));                 \
        else                                                            \
            PCLASS::CBNAME();                                           \
    }                                                                   \
    void base_##CBNAME() {                                              \
        PCLASS::CBNAME();                                               \
    }

//---------------------------------------------------------------------------

#define PYPRIVATE                               \
    void _setSelf(PyObject* self) {             \
        m_myInst.setSelf(self);                 \
    }                                           \
    private: wxPyCallbackHelper m_myInst;

//---------------------------------------------------------------------------

#endif

