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


//---------------------------------------------------------------------------

#if defined(__WXMSW__)
#       define HELPEREXPORT __declspec(dllexport)
#else
#       define HELPEREXPORT
#endif

//----------------------------------------------------------------------

class wxPyApp: public wxApp
{
public:
    wxPyApp();
    ~wxPyApp();
    int  MainLoop(void);
    bool OnInit(void);
//#    void AfterMainLoop(void);
};

extern wxPyApp *wxPythonApp;

//----------------------------------------------------------------------

void      __wxPreStart();
PyObject* __wxStart(PyObject*, PyObject* args);

extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

void wxPyEventThunker(wxObject*, wxEvent& event);

HELPEREXPORT PyObject* wxPyConstructObject(void* ptr, char* className);
HELPEREXPORT bool wxPyRestoreThread();
HELPEREXPORT void wxPySaveThread(bool doSave);

//----------------------------------------------------------------------


#ifndef SWIGCODE
extern "C" void SWIG_MakePtr(char *, void *, char *);
extern "C" char *SWIG_GetPtr(char *, void **, char *);
extern "C" char *SWIG_GetPtrObj(PyObject *obj, void **ptr, char *type);
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

//  class wxPyMenu : public wxMenu {
//  public:
//      wxPyMenu(const wxString& title = "", PyObject* func=NULL);
//      ~wxPyMenu();

//  private:
//      static void MenuCallback(wxMenu& menu, wxCommandEvent& evt);
//      PyObject*   func;
//  };


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
// wxPyCallbackHelper.
//
// **** This class should be combined with wxPyCallback defined above.
//
//---------------------------------------------------------------------------

class HELPEREXPORT wxPyCallbackHelper {
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

#define PYPRIVATE                               \
    void _setSelf(PyObject* self) {             \
        m_myInst.setSelf(self);                 \
    }                                           \
    private: wxPyCallbackHelper m_myInst;

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__(CBNAME)                        \
    void CBNAME();                                      \
    void base_##CBNAME();


#define IMP_PYCALLBACK__(CLASS, PCLASS, CBNAME)         \
    void CLASS::CBNAME() {                              \
        bool doSave = wxPyRestoreThread();              \
        if (m_myInst.findCallback(#CBNAME))             \
            m_myInst.callCallback(Py_BuildValue("()")); \
        else                                            \
            PCLASS::CBNAME();                           \
        wxPySaveThread(doSave);                         \
    }                                                   \
    void CLASS::base_##CBNAME() {                       \
        PCLASS::CBNAME();                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINT(CBNAME)                      \
    bool CBNAME(int a, int b);                                  \
    bool base_##CBNAME(int a, int b);


#define IMP_PYCALLBACK_BOOL_INTINT(CLASS, PCLASS, CBNAME)               \
    bool CLASS::CBNAME(int a, int b) {                                  \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(ii)",a,b));    \
        else                                                            \
            rval = PCLASS::CBNAME(a,b);                                 \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(int a, int b) {                           \
        return PCLASS::CBNAME(a,b);                                     \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INT(CBNAME)                         \
    bool CBNAME(int a);                                         \
    bool base_##CBNAME(int a);


#define IMP_PYCALLBACK_BOOL_INT(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(int a) {                                         \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(i)",a));       \
        else                                                            \
            rval = PCLASS::CBNAME(a);                                   \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(int a) {                                  \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INT_pure(CBNAME)            \
    bool CBNAME(int a);


#define IMP_PYCALLBACK_BOOL_INT_pure(CLASS, PCLASS, CBNAME)             \
    bool CLASS::CBNAME(int a) {                                         \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(i)",a));       \
        else rval = false;                                              \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC(CBNAME)                      \
    void CBNAME(wxDC& a);                               \
    void base_##CBNAME(wxDC& a);


#define IMP_PYCALLBACK__DC(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(wxDC& a) {                               \
        bool doSave = wxPyRestoreThread();                      \
        if (m_myInst.findCallback(#CBNAME))                     \
            m_myInst.callCallback(Py_BuildValue("(O)",          \
                            wxPyConstructObject(&a, "wxDC")));  \
        else                                                    \
            PCLASS::CBNAME(a);                                  \
        wxPySaveThread(doSave);                                 \
    }                                                           \
    void CLASS::base_##CBNAME(wxDC& a) {                        \
        PCLASS::CBNAME(a);                                      \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b);


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(Oi)",                 \
                            wxPyConstructObject(&a, "wxDC"), (int)b));  \
        else                                                            \
            PCLASS::CBNAME(a, b);                                       \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                          \
    void CBNAME(wxDC& a, bool b);                               \
    void base_##CBNAME(wxDC& a, bool b);


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(Oi)",                 \
                            wxPyConstructObject(&a, "wxDC"), (int)b));  \
        else                                                            \
            PCLASS::CBNAME(a, b);                                       \
                wxPySaveThread(doSave);                                 \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL(CBNAME)                    \
    void CBNAME(double a, double b);                    \
    void base_##CBNAME(double a, double b);


#define IMP_PYCALLBACK__2DBL(CLASS, PCLASS, CBNAME)             \
    void CLASS::CBNAME(double a, double b) {                    \
        bool doSave = wxPyRestoreThread();                      \
        if (m_myInst.findCallback(#CBNAME))                     \
            m_myInst.callCallback(Py_BuildValue("(dd)",a,b));   \
        else                                                    \
            PCLASS::CBNAME(a, b);                               \
        wxPySaveThread(doSave);                                 \
    }                                                           \
    void CLASS::base_##CBNAME(double a, double b) {             \
        PCLASS::CBNAME(a, b);                                   \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL2INT(CBNAME)                        \
    void CBNAME(double a, double b, int c, int d);              \
    void base_##CBNAME(double a, double b, int c, int d);


#define IMP_PYCALLBACK__2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(double a, double b, int c, int d) {              \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(ddii)",               \
                                                       a,b,c,d));       \
        else                                                            \
            PCLASS::CBNAME(a, b, c, d);                                 \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b, int c, int d) {       \
        PCLASS::CBNAME(a, b, c, d);                                     \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBLBOOL(CBNAME)                              \
    void CBNAME(wxDC& a, double b, double c, double d, double e, bool f);       \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f);


#define IMP_PYCALLBACK__DC4DBLBOOL(CLASS, PCLASS, CBNAME)                               \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {       \
        bool doSave = wxPyRestoreThread();                                              \
        if (m_myInst.findCallback(#CBNAME))                                             \
            m_myInst.callCallback(Py_BuildValue("(Oddddi)",                             \
                                   wxPyConstructObject(&a, "wxDC"),                     \
                                              b, c, d, e, (int)f));                     \
        else                                                                            \
            PCLASS::CBNAME(a, b, c, d, e, f);                                           \
        wxPySaveThread(doSave);                                                         \
    }                                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {\
        PCLASS::CBNAME(a, b, c, d, e, f);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DC4DBLBOOL(CBNAME)                                  \
    bool CBNAME(wxDC& a, double b, double c, double d, double e, bool f);       \
    bool base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f);


#define IMP_PYCALLBACK_BOOL_DC4DBLBOOL(CLASS, PCLASS, CBNAME)                           \
    bool CLASS::CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {       \
        bool doSave = wxPyRestoreThread();                                              \
        if (m_myInst.findCallback(#CBNAME))                                             \
            return m_myInst.callCallback(Py_BuildValue("(Oddddi)",                      \
                                   wxPyConstructObject(&a, "wxDC"),                     \
                                              b, c, d, e, (int)f));                     \
        else                                                                            \
            return PCLASS::CBNAME(a, b, c, d, e, f);                                    \
        wxPySaveThread(doSave);                                                         \
    }                                                                                   \
    bool CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {\
        return PCLASS::CBNAME(a, b, c, d, e, f);                                        \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__BOOL2DBL2INT(CBNAME)                            \
    void CBNAME(bool a, double b, double c, int d, int e);              \
    void base_##CBNAME(bool a, double b, double c, int d, int e);


#define IMP_PYCALLBACK__BOOL2DBL2INT(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(bool a, double b, double c, int d, int e) {              \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("(idii)",                       \
                                                (int)a,b,c,d,e));               \
        else                                                                    \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
        wxPySaveThread(doSave);                                                 \
    }                                                                           \
    void CLASS::base_##CBNAME(bool a, double b, double c, int d, int e) {       \
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBL(CBNAME)                                      \
    void CBNAME(wxDC& a, double b, double c, double d, double e);               \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e);


#define IMP_PYCALLBACK__DC4DBL(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e) {       \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("(Odddd)",                      \
                                   wxPyConstructObject(&a, "wxDC"),             \
                                                     b, c, d, e));              \
        else                                                                    \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
        wxPySaveThread(doSave);                                                 \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e) {\
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b);


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(Oi)",                 \
                                   wxPyConstructObject(&a, "wxDC"),     \
                                                     (int)b));          \
        else                                                            \
            PCLASS::CBNAME(a, b);                                       \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__WXCPBOOL2DBL2INT(CBNAME)                                \
    void CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f);   \
    void base_##CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f);


#define IMP_PYCALLBACK__WXCPBOOL2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(wxControlPoint* a, bool b, double c, double d,           \
                int e, int f) {                                                 \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("(Oiddii)",                     \
                                 wxPyConstructObject(a, "wxControlPoint"),      \
                                 (int)b, c, d, e, f));                          \
        else                                                                    \
            PCLASS::CBNAME(a, b, c, d, e, f);                                   \
        wxPySaveThread(doSave);                                                 \
    }                                                                           \
    void CLASS::base_##CBNAME(wxControlPoint* a, bool b, double c, double d,    \
                       int e, int f) {                                          \
        PCLASS::CBNAME(a, b, c, d, e, f);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__WXCP2DBL2INT(CBNAME)                                    \
    void CBNAME(wxControlPoint* a, double b, double c, int d, int e);           \
    void base_##CBNAME(wxControlPoint* a, double b, double c, int d, int e);


#define IMP_PYCALLBACK__WXCP2DBL2INT(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(wxControlPoint* a, double b, double c, int d, int e) {   \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("(Oddii)",                      \
                                 wxPyConstructObject(a, "wxControlPoint"),      \
                                 b, c, d, e));                                  \
        else                                                                    \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
        wxPySaveThread(doSave);                                                 \
    }                                                                           \
    void CLASS::base_##CBNAME(wxControlPoint* a, double b, double c,            \
                       int d, int e) {                                          \
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBLINT(CBNAME)                 \
    void CBNAME(double a, double b, int c);             \
    void base_##CBNAME(double a, double b, int c);


#define IMP_PYCALLBACK__2DBLINT(CLASS, PCLASS, CBNAME)                  \
    void CLASS::CBNAME(double a, double b, int c) {                     \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(ddi)", a,b,c));       \
        else                                                            \
            PCLASS::CBNAME(a, b, c);                                    \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b, int c) {              \
        PCLASS::CBNAME(a, b, c);                                        \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__BOOL2DBLINT(CBNAME)                     \
    void CBNAME(bool a, double b, double c, int d);             \
    void base_##CBNAME(bool a, double b, double c, int d);


#define IMP_PYCALLBACK__BOOL2DBLINT(CLASS, PCLASS, CBNAME)                      \
    void CLASS::CBNAME(bool a, double b, double c, int d) {                     \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("(iddi)", (int)a,b,c,d));       \
        else                                                                    \
            PCLASS::CBNAME(a, b, c, d);                                         \
        wxPySaveThread(doSave);                                                 \
    }                                                                           \
    void CLASS::base_##CBNAME(bool a, double b, double c, int d) {              \
        PCLASS::CBNAME(a, b, c, d);                                             \
    }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif



