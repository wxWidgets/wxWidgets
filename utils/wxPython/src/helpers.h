////////////////////////////////////////////////////////////////////////////
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

typedef unsigned char byte;

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
void      __wxCleanup();

extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

void wxPyEventThunker(wxObject*, wxEvent& event);

HELPEREXPORT PyObject* wxPyConstructObject(void* ptr, const char* className);
HELPEREXPORT bool wxPyRestoreThread();
HELPEREXPORT void wxPySaveThread(bool doSave);
HELPEREXPORT PyObject* wxPy_ConvertList(wxListBase* list, const char* className);
HELPEREXPORT long wxPyGetWinHandle(wxWindow* win);

//----------------------------------------------------------------------

class wxPyUserData : public wxObject {
public:
    wxPyUserData(PyObject* obj) { m_obj = obj; Py_INCREF(m_obj); }
    ~wxPyUserData() {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_obj);
        wxPySaveThread(doSave);
    }
    PyObject* m_obj;
};

//----------------------------------------------------------------------
// These are helpers used by the typemaps

HELPEREXPORT byte* byte_LIST_helper(PyObject* source);
HELPEREXPORT int* int_LIST_helper(PyObject* source);
HELPEREXPORT long* long_LIST_helper(PyObject* source);
HELPEREXPORT char** string_LIST_helper(PyObject* source);
HELPEREXPORT wxPoint* wxPoint_LIST_helper(PyObject* source);
HELPEREXPORT wxBitmap** wxBitmap_LIST_helper(PyObject* source);
HELPEREXPORT wxString* wxString_LIST_helper(PyObject* source);
HELPEREXPORT wxAcceleratorEntry* wxAcceleratorEntry_LIST_helper(PyObject* source);

HELPEREXPORT bool wxSize_helper(PyObject* source, wxSize** obj);
HELPEREXPORT bool wxPoint_helper(PyObject* source, wxPoint** obj);
HELPEREXPORT bool wxRealPoint_helper(PyObject* source, wxRealPoint** obj);
HELPEREXPORT bool wxRect_helper(PyObject* source, wxRect** obj);

//----------------------------------------------------------------------

#ifndef SWIGCODE
extern "C" void SWIG_MakePtr(char *, void *, char *);
extern "C" char *SWIG_GetPtr(char *, void **, char *);
extern "C" char *SWIG_GetPtrObj(PyObject *obj, void **ptr, char *type);
#endif


#ifdef _MSC_VER
# pragma warning(disable:4800)
#endif



// Non-const versions to keep SWIG happy.
extern wxPoint  wxPyDefaultPosition;
extern wxSize   wxPyDefaultSize;
extern wxString wxPyEmptyStr;

//----------------------------------------------------------------------

class wxPyCallback : public wxObject {
    DECLARE_ABSTRACT_CLASS(wxPyCallback);
public:
    wxPyCallback(PyObject* func);
    wxPyCallback(const wxPyCallback& other);
    ~wxPyCallback();

    void EventThunker(wxEvent& event);

    PyObject*   m_func;
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




//---------------------------------------------------------------------------
// This class holds an instance of a Python Shadow Class object and assists
// with looking up and invoking Python callback methods from C++ virtual
// method redirections.  For all classes which have virtuals which should be
// overridable in wxPython, a new subclass is created that contains a
// wxPyCallbackHelper.
//
// TODO: This class should be combined with wxPyCallback defined above.
//

class HELPEREXPORT wxPyCallbackHelper {
public:
    wxPyCallbackHelper();
    ~wxPyCallbackHelper();

    wxPyCallbackHelper(const wxPyCallbackHelper& other);

    void        setSelf(PyObject* self, PyObject* _class, int incref=TRUE);

    bool        findCallback(const wxString& name) const;
    int         callCallback(PyObject* argTuple) const;
    PyObject*   callCallbackObj(PyObject* argTuple) const;

private:
    PyObject*   m_self;
    PyObject*   m_class;
    PyObject*   m_lastFound;
    int         m_incRef;
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// These Event classes can be derived from in Python and passed through the
// event system without loosing anything.  They do this by keeping a reference
// to themselves and some special case handling in wxPyCallback::EventThunker.


class wxPyEvtSelfRef {
public:
    wxPyEvtSelfRef();
    ~wxPyEvtSelfRef();

    void SetSelf(PyObject* self, bool clone=FALSE);
    PyObject* GetSelf() const;

protected:
    PyObject*   m_self;
    bool        m_cloned;
};


class wxPyEvent : public wxEvent, public wxPyEvtSelfRef {
    DECLARE_DYNAMIC_CLASS(wxPyEvent)
public:
    wxPyEvent(int id=0);
    ~wxPyEvent();

    void CopyObject(wxObject& dest) const;
};


class wxPyCommandEvent : public wxCommandEvent, public wxPyEvtSelfRef {
    DECLARE_DYNAMIC_CLASS(wxPyCommandEvent)
public:
    wxPyCommandEvent(wxEventType commandType = wxEVT_NULL, int id=0);
    ~wxPyCommandEvent();

    void CopyObject(wxObject& dest) const;
};


//---------------------------------------------------------------------------
// These macros are used to implement the virtual methods that should
// redirect to a Python method if one exists.  The names designate the
// return type, if any, as well as any parameter types.
//---------------------------------------------------------------------------

#define PYPRIVATE                                                       \
    void _setSelf(PyObject* self, PyObject* _class, int incref=1) {     \
        m_myInst.setSelf(self, _class, incref);                         \
    }                                                                   \
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
        else rval = FALSE;                                              \
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
        bool rval;                                                                      \
        if (m_myInst.findCallback(#CBNAME))                                             \
            rval = m_myInst.callCallback(Py_BuildValue("(Oddddi)",                      \
                                   wxPyConstructObject(&a, "wxDC"),                     \
                                              b, c, d, e, (int)f));                     \
        else                                                                            \
            rval = PCLASS::CBNAME(a, b, c, d, e, f);                                    \
        wxPySaveThread(doSave);                                                         \
        return rval;                                                                    \
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
                                 wxPyConstructObject(a, "wxPyControlPoint"),    \
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
                                 wxPyConstructObject(a, "wxPyControlPoint"),    \
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

#define DEC_PYCALLBACK__STRING(CBNAME)                  \
    void CBNAME(const wxString& a);                     \
    void base_##CBNAME(const wxString& a);


#define IMP_PYCALLBACK__STRING(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(const wxString& a)  {                            \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(s)", a.c_str()));     \
        else                                                            \
            PCLASS::CBNAME(a);                                          \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(const wxString& a) {                      \
        PCLASS::CBNAME(a);                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING(CBNAME)              \
    bool CBNAME(const wxString& a);                     \
    bool base_##CBNAME(const wxString& a);


#define IMP_PYCALLBACK_BOOL_STRING(CLASS, PCLASS, CBNAME)                       \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval;                                                              \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            rval = m_myInst.callCallback(Py_BuildValue("(s)", a.c_str()));      \
        else                                                                    \
            rval = PCLASS::CBNAME(a);                                           \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(const wxString& a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRINGSTRING(CBNAME)              \
    bool CBNAME(const wxString& a, const wxString& b);        \
    bool base_##CBNAME(const wxString& a, const wxString& b);


#define IMP_PYCALLBACK_BOOL_STRINGSTRING(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(const wxString& a, const wxString& b) {                  \
        bool rval;                                                              \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            rval = m_myInst.callCallback(Py_BuildValue("(ss)",                  \
                                                       a.c_str(), b.c_str()));  \
        else                                                                    \
            rval = PCLASS::CBNAME(a, b);                                        \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(const wxString& a, const wxString& b) {           \
        return PCLASS::CBNAME(a, b);                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_(CBNAME)                  \
    wxString CBNAME();                                  \
    wxString base_##CBNAME();


#define IMP_PYCALLBACK_STRING_(CLASS, PCLASS, CBNAME)                           \
    wxString CLASS::CBNAME() {                                                  \
        wxString rval;                                                          \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = m_myInst.callCallbackObj(Py_BuildValue("()"));                 \
            if (ro) {                                                           \
                rval = PyString_AsString(PyObject_Str(ro));                     \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        else                                                                    \
            rval = PCLASS::CBNAME();                                            \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    wxString CLASS::base_##CBNAME() {                                           \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING__pure(CBNAME)             \
    wxString CBNAME();


#define IMP_PYCALLBACK_STRING__pure(CLASS, PCLASS, CBNAME)                      \
    wxString CLASS::CBNAME() {                                                  \
        wxString rval;                                                          \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = m_myInst.callCallbackObj(Py_BuildValue("()"));                 \
            if (ro) {                                                           \
                rval = PyString_AsString(PyObject_Str(ro));                     \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_TAG_pure(CBNAME)                  \
    bool CBNAME(const wxHtmlTag& a);                          \


#define IMP_PYCALLBACK_BOOL_TAG_pure(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME(const wxHtmlTag& a)  {                                   \
        bool rval = FALSE;                                                      \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            rval = m_myInst.callCallback(Py_BuildValue("(O)",                   \
                                         wxPyConstructObject((void*)&a,"wxHtmlTag"))); \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK___pure(CBNAME)                         \
    void CBNAME();                                            \


#define IMP_PYCALLBACK___pure(CLASS, PCLASS, CBNAME)                            \
    void CLASS::CBNAME() {                                                      \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            m_myInst.callCallback(Py_BuildValue("()"));                         \
        wxPySaveThread(doSave);                                                 \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_wxSize__pure(CBNAME)                         \
    wxSize CBNAME();                                                \


#define IMP_PYCALLBACK_wxSize__pure(CLASS, PCLASS, CBNAME)                      \
    wxSize CLASS::CBNAME() {                                                    \
        wxSize rval(0,0);                                                       \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME)) {                                   \
            PyObject* ro;                                                       \
            wxSize*   ptr;                                                      \
            ro = m_myInst.callCallbackObj(Py_BuildValue("()"));                 \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxSize_p"))           \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_WXWIN(CBNAME)                      \
    bool CBNAME(wxWindow* a);                                  \
    bool base_##CBNAME(wxWindow* a);


#define IMP_PYCALLBACK_BOOL_WXWIN(CLASS, PCLASS, CBNAME)                \
    bool CLASS::CBNAME(wxWindow* a) {                                   \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(O)",           \
                            wxPyConstructObject((void*)a,"wxWindow"))); \
        else                                                            \
            rval = PCLASS::CBNAME(a);                                   \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(wxWindow* a) {                            \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_(CBNAME)                      \
    bool CBNAME();                                        \
    bool base_##CBNAME();


#define IMP_PYCALLBACK_BOOL_(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME() {                                              \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("()"));          \
        else                                                            \
            rval = PCLASS::CBNAME();                                    \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME() {                                       \
        return PCLASS::CBNAME();                                        \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DR_2WXCDR(CBNAME)                 \
    wxDragResult CBNAME(wxCoord x, wxCoord y, wxDragResult def);    \
    wxDragResult base_##CBNAME(wxCoord x, wxCoord y, wxDragResult def);


#define IMP_PYCALLBACK_DR_2WXCDR(CLASS, PCLASS, CBNAME)                 \
    wxDragResult CLASS::CBNAME(wxCoord a, wxCoord b, wxDragResult c) {  \
        bool doSave = wxPyRestoreThread();                              \
        int rval;                                                       \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(iii)", a,b,c));\
        else                                                            \
            rval = PCLASS::CBNAME(a, b, c);                             \
        wxPySaveThread(doSave);                                         \
        return (wxDragResult)rval;                                      \
    }                                                                   \
    wxDragResult CLASS::base_##CBNAME(wxCoord a, wxCoord b, wxDragResult c) { \
        return PCLASS::CBNAME(a, b, c);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DR(CBNAME)                  \
    bool CBNAME(wxDragResult a);                        \
    bool base_##CBNAME(wxDragResult a);


#define IMP_PYCALLBACK_BOOL_DR(CLASS, PCLASS, CBNAME)                   \
    bool CLASS::CBNAME(wxDragResult a) {                                \
        bool doSave = wxPyRestoreThread();                              \
        bool rval;                                                      \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(i)", a));      \
        else                                                            \
            rval = PCLASS::CBNAME(a);                                   \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(wxDragResult a) {                         \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DR_2WXCDR_pure(CBNAME)   \
    wxDragResult CBNAME(wxCoord x, wxCoord y, wxDragResult def);


#define IMP_PYCALLBACK_DR_2WXCDR_pure(CLASS, PCLASS, CBNAME)            \
    wxDragResult CLASS::CBNAME(wxCoord a, wxCoord b, wxDragResult c) {  \
        bool doSave = wxPyRestoreThread();                              \
        int rval;                                                       \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(iii)", a,b,c));\
        wxPySaveThread(doSave);                                         \
        return (wxDragResult)rval;                                      \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINTSTR_pure(CBNAME)                      \
    bool CBNAME(int a, int b, const wxString& c);


#define IMP_PYCALLBACK_BOOL_INTINTSTR_pure(CLASS, PCLASS, CBNAME)       \
    bool CLASS::CBNAME(int a, int b, const wxString& c) {               \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(iis)",a,b,c.c_str()));\
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZET_(CBNAME)                  \
    size_t CBNAME();                                   \
    size_t base_##CBNAME();


#define IMP_PYCALLBACK_SIZET_(CLASS, PCLASS, CBNAME)                            \
    size_t CLASS::CBNAME() {                                                    \
        size_t rval;                                                            \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME))                                     \
            rval = m_myInst.callCallback(Py_BuildValue("()"));                  \
        else                                                                    \
            rval = PCLASS::CBNAME();                                            \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    size_t CLASS::base_##CBNAME() {                                             \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DATAFMT_SIZET(CBNAME)                  \
    wxDataFormat  CBNAME();                                        \
    wxDataFormat  base_##CBNAME();


#define IMP_PYCALLBACK_DATAFMT_SIZET(CLASS, PCLASS, CBNAME)                     \
    wxDataFormat CLASS::CBNAME(size_t a) {                                      \
        wxDataFormat rval;                                                      \
        bool doSave = wxPyRestoreThread();                                      \
        if (m_myInst.findCallback(#CBNAME)) {                                   \
            PyObject* ro;                                                       \
            wxDataFormat* ptr;                                                  \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(i)", a));             \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxDataFormat_p"))     \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        else                                                                    \
            rval = PCLASS::CBNAME(a);                                           \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    wxDataFormat  CLASS::base_##CBNAME(size_t a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__constany(CBNAME, Type)          \
    void CBNAME(const Type& a);                         \
    void base_##CBNAME(const Type& a);


#define IMP_PYCALLBACK__constany(CLASS, PCLASS, CBNAME, Type)   \
    void CLASS::CBNAME(const Type& a) {                         \
        bool doSave = wxPyRestoreThread();                      \
        if (m_myInst.findCallback(#CBNAME))                     \
            m_myInst.callCallback(Py_BuildValue("(O)",          \
                     wxPyConstructObject((void*)&a, #Type)));   \
        else                                                    \
            PCLASS::CBNAME(a);                                  \
        wxPySaveThread(doSave);                                 \
    }                                                           \
    void CLASS::base_##CBNAME(const Type& a) {                  \
        PCLASS::CBNAME(a);                                      \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__any(CBNAME, Type)          \
    void CBNAME(Type& a);                          \
    void base_##CBNAME(Type& a);


#define IMP_PYCALLBACK__any(CLASS, PCLASS, CBNAME, Type)        \
    void CLASS::CBNAME(Type& a) {                               \
        bool doSave = wxPyRestoreThread();                      \
        if (m_myInst.findCallback(#CBNAME))                     \
            m_myInst.callCallback(Py_BuildValue("(O)",          \
                            wxPyConstructObject(&a, #Type)));   \
        else                                                    \
            PCLASS::CBNAME(a);                                  \
        wxPySaveThread(doSave);                                 \
    }                                                           \
    void CLASS::base_##CBNAME(Type& a) {                        \
        PCLASS::CBNAME(a);                                      \
    }

//---------------------------------------------------------------------------

#endif




