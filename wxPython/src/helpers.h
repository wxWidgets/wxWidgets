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

typedef unsigned char byte;


class wxPyApp: public wxApp
{
public:
    wxPyApp();
    ~wxPyApp();
    int  MainLoop(void);
    bool OnInit(void);
};
extern wxPyApp *wxPythonApp;

//----------------------------------------------------------------------

void      __wxPreStart();
PyObject* __wxStart(PyObject*, PyObject* args);
void      __wxCleanup();

extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

void wxPyEventThunker(wxObject*, wxEvent& event);

PyObject* wxPyConstructObject(void* ptr,
                              const char* className,
                              int setThisOwn=0);
bool wxPyRestoreThread();
void wxPySaveThread(bool doSave);
PyObject* wxPy_ConvertList(wxListBase* list, const char* className);
long wxPyGetWinHandle(wxWindow* win);



//----------------------------------------------------------------------
// Handle wxInputStreams by Joerg Baumann
// See stream.i for implementations

// list class for return list of strings, e.g. readlines()
WX_DECLARE_LIST(wxString, wxStringPtrList);


// C++ class wxPyInputStream to act as base for python class wxInputStream
// Use it in python like a python file object
class wxPyInputStream {
public:
    // underlying wxInputStream
    wxInputStream* wxi;

public:
    wxPyInputStream(wxInputStream* wxi_) : wxi(wxi_) {}
    ~wxPyInputStream();

    // python file object interface for input files (most of it)
    void close();
    void flush();
    bool eof();
    wxString* read(int size=-1);
    wxString* readline(int size=-1);
    wxStringPtrList* readlines(int sizehint=-1);
    void seek(int offset, int whence=0);
    int tell();
    /*
      bool isatty();
      int fileno();
      void truncate(int size=-1);
      void write(wxString data);
      void writelines(wxStringPtrList);
    */
};


//----------------------------------------------------------------------
// These are helpers used by the typemaps

byte* byte_LIST_helper(PyObject* source);
int* int_LIST_helper(PyObject* source);
long* long_LIST_helper(PyObject* source);
char** string_LIST_helper(PyObject* source);
wxPoint* wxPoint_LIST_helper(PyObject* source);
wxBitmap** wxBitmap_LIST_helper(PyObject* source);
wxString* wxString_LIST_helper(PyObject* source);
wxAcceleratorEntry* wxAcceleratorEntry_LIST_helper(PyObject* source);

bool wxSize_helper(PyObject* source, wxSize** obj);
bool wxPoint_helper(PyObject* source, wxPoint** obj);
bool wxRealPoint_helper(PyObject* source, wxRealPoint** obj);
bool wxRect_helper(PyObject* source, wxRect** obj);
bool wxColour_helper(PyObject* source, wxColour** obj);

//----------------------------------------------------------------------

#ifndef SWIGCODE
extern "C" void SWIG_MakePtr(char *, void *, char *);
extern "C" char *SWIG_GetPtr(char *, void **, char *);
extern "C" char *SWIG_GetPtrObj(PyObject *obj, void **ptr, char *type);
#endif


#ifdef _MSC_VER
# pragma warning(disable:4800)
# pragma warning(disable:4190)
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
// Export a C API in a struct.  Other modules will be able to load this from
// the wxc module and will then have safe access to these functions, even if
// in another shared library.

class wxPyCallbackHelper;

struct wxPyCoreAPI {

    void        (*p_SWIG_MakePtr)(char*, void*, char*);
    char*       (*p_SWIG_GetPtr)(char*, void**, char*);
    char*       (*p_SWIG_GetPtrObj)(PyObject*, void**, char*);
    void        (*p_SWIG_RegisterMapping)(char*, char*, void *(*cast)(void *));
    void        (*p_SWIG_addvarlink)(PyObject*, char*, PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p));
    PyObject*   (*p_SWIG_newvarlink)(void);

    void        (*p_wxPySaveThread)(bool);
    bool        (*p_wxPyRestoreThread)();
    PyObject*   (*p_wxPyConstructObject)(void *, const char *, int);
    PyObject*   (*p_wxPy_ConvertList)(wxListBase* list, const char* className);

    byte*       (*p_byte_LIST_helper)(PyObject* source);
    int*        (*p_int_LIST_helper)(PyObject* source);
    long*       (*p_long_LIST_helper)(PyObject* source);
    char**      (*p_string_LIST_helper)(PyObject* source);
    wxPoint*    (*p_wxPoint_LIST_helper)(PyObject* source);
    wxBitmap**  (*p_wxBitmap_LIST_helper)(PyObject* source);
    wxString*   (*p_wxString_LIST_helper)(PyObject* source);
    wxAcceleratorEntry*   (*p_wxAcceleratorEntry_LIST_helper)(PyObject* source);

    bool        (*p_wxSize_helper)(PyObject* source, wxSize** obj);
    bool        (*p_wxPoint_helper)(PyObject* source, wxPoint** obj);
    bool        (*p_wxRealPoint_helper)(PyObject* source, wxRealPoint** obj);
    bool        (*p_wxRect_helper)(PyObject* source, wxRect** obj);
    bool        (*p_wxColour_helper)(PyObject* source, wxColour** obj);

    void        (*p_wxPyCBH_setSelf)(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
    bool        (*p_wxPyCBH_findCallback)(const wxPyCallbackHelper& cbh, const char* name);
    int         (*p_wxPyCBH_callCallback)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    PyObject*   (*p_wxPyCBH_callCallbackObj)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    void        (*p_wxPyCBH_delete)(wxPyCallbackHelper* cbh);

};

#ifdef wxPyUSE_EXPORT
static wxPyCoreAPI* wxPyCoreAPIPtr = NULL;  // Each module needs one, but may not use it.
#endif

//---------------------------------------------------------------------------
// This class holds an instance of a Python Shadow Class object and assists
// with looking up and invoking Python callback methods from C++ virtual
// method redirections.  For all classes which have virtuals which should be
// overridable in wxPython, a new subclass is created that contains a
// wxPyCallbackHelper.
//

class wxPyCallbackHelper {
public:
    wxPyCallbackHelper(const wxPyCallbackHelper& other);

    wxPyCallbackHelper() {
        m_class = NULL;
        m_self = NULL;
        m_lastFound = NULL;
        m_incRef = FALSE;
    }

    ~wxPyCallbackHelper() {
#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPyCBH_delete(this);
#else
        wxPyCBH_delete(this);
#endif
    }

    void        setSelf(PyObject* self, PyObject* klass, int incref=TRUE);
    bool        findCallback(const char* name) const;
    int         callCallback(PyObject* argTuple) const;
    PyObject*   callCallbackObj(PyObject* argTuple) const;

private:
    PyObject*   m_self;
    PyObject*   m_class;
    PyObject*   m_lastFound;
    int         m_incRef;

    friend      void wxPyCBH_delete(wxPyCallbackHelper* cbh);
};


void wxPyCBH_setSelf(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
bool wxPyCBH_findCallback(const wxPyCallbackHelper& cbh, const char* name);
int  wxPyCBH_callCallback(const wxPyCallbackHelper& cbh, PyObject* argTuple);
PyObject* wxPyCBH_callCallbackObj(const wxPyCallbackHelper& cbh, PyObject* argTuple);
void wxPyCBH_delete(wxPyCallbackHelper* cbh);



//----------------------------------------------------------------------

class wxPyUserData : public wxObject {
public:
    wxPyUserData(PyObject* obj) {
        m_obj = obj;
        Py_INCREF(m_obj);
    }

    ~wxPyUserData() {
        bool doSave;
#ifdef wxPyUSE_EXPORT
        doSave = wxPyCoreAPIPtr->p_wxPyRestoreThread();
#else
        doSave = wxPyRestoreThread();
#endif

        Py_DECREF(m_obj);

#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPySaveThread(doSave);
#else
        wxPySaveThread(doSave);
#endif
    }
    PyObject* m_obj;
};



//---------------------------------------------------------------------------
// These macros are used to implement the virtual methods that should
// redirect to a Python method if one exists.  The names designate the
// return type, if any, as well as any parameter types.
//---------------------------------------------------------------------------

#define PYPRIVATE                                                       \
    void _setSelf(PyObject* self, PyObject* _class, int incref=1) {     \
        wxPyCBH_setSelf(m_myInst, self, _class, incref);                \
    }                                                                   \
    private: wxPyCallbackHelper m_myInst

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__(CBNAME)                        \
    void CBNAME();                                      \
    void base_##CBNAME();


#define IMP_PYCALLBACK__(CLASS, PCLASS, CBNAME)         \
    void CLASS::CBNAME() {                              \
        bool doSave = wxPyRestoreThread();              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))             \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()")); \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));    \
        else                                                            \
            rval = PCLASS::CBNAME(a,b);                                 \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(int a, int b) {                           \
        return PCLASS::CBNAME(a,b);                                     \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INTINT(CBNAME)                      \
    void CBNAME(int a, int b);                                  \
    void base_##CBNAME(int a, int b);


#define IMP_PYCALLBACK_VOID_INTINT(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(int a, int b) {                                  \
        bool doSave = wxPyRestoreThread();                              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));           \
        else                                                            \
            PCLASS::CBNAME(a,b);                                        \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b) {                           \
        PCLASS::CBNAME(a,b);                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INT(CBNAME)                         \
    bool CBNAME(int a);                                         \
    bool base_##CBNAME(int a);


#define IMP_PYCALLBACK_BOOL_INT(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(int a) {                                         \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                   \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0); \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                     \
        }                                                       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                     \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(dd)",a,b));   \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddii)",               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                           \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);                         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));  \
            Py_DECREF(obj);                                                             \
        }                                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                           \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);                         \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));\
            Py_DECREF(obj);                                                             \
        }                                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(idii)",                       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);                 \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Odddd)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject(&a, "wxDC", 0);         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* obj = wxPyConstructObject(a, "wxPyControlPoint", 0);      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oiddii)", obj, (int)b, c, d, e, f));\
            Py_DECREF(obj);                                                     \
        }                                                                       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* obj = wxPyConstructObject(a, "wxPyControlPoint", 0);      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddii)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddi)", a,b,c));       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddi)", (int)a,b,c,d));       \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));     \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));      \
        else                                                                    \
            rval = PCLASS::CBNAME(a);                                           \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(const wxString& a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING_pure(CBNAME)         \
    bool CBNAME(const wxString& a);
                   \
#define IMP_PYCALLBACK_BOOL_STRING_pure(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval;                                                              \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));      \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING_pure(CBNAME)                               \
    wxString CBNAME(const wxString& a);                                         \

#define IMP_PYCALLBACK_STRING_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(s)", a.c_str()));     \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRINGINT_pure(CBNAME)                            \
    wxString CBNAME(const wxString& a,int b);                                   \

#define IMP_PYCALLBACK_STRING_STRINGINT_pure(CLASS, PCLASS, CBNAME)             \
    wxString CLASS::CBNAME(const wxString& a,int b)  {                          \
        wxString rval;                                                          \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(si)", a.c_str(),b));  \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRINGSTRING(CBNAME)              \
    bool CBNAME(const wxString& a, const wxString& b);        \
    bool base_##CBNAME(const wxString& a, const wxString& b);


#define IMP_PYCALLBACK_BOOL_STRINGSTRING(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(const wxString& a, const wxString& b) {                  \
        bool rval;                                                              \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ss)",                  \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));                 \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);  Py_DECREF(str);                                 \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));                 \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* obj = wxPyConstructObject((void*)&a,"wxHtmlTag", 0);      \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));            \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPySaveThread(doSave);                                                 \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK___pure(CBNAME)                         \
    void CBNAME();                                            \


#define IMP_PYCALLBACK___pure(CLASS, PCLASS, CBNAME)                            \
    void CLASS::CBNAME() {                                                      \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                         \
        wxPySaveThread(doSave);                                                 \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_wxSize__pure(CBNAME)                         \
    wxSize CBNAME();                                                \


#define IMP_PYCALLBACK_wxSize__pure(CLASS, PCLASS, CBNAME)                      \
    wxSize CLASS::CBNAME() {                                                    \
        wxSize rval(0,0);                                                       \
        bool doSave = wxPyRestoreThread();                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            wxSize*   ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));                 \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject((void*)a,"wxWindow", 0);\
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));    \
            Py_DECREF(obj);                                             \
        }                                                               \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));          \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        else                                                            \
            rval = PCLASS::CBNAME(a, b, c);                             \
        wxPySaveThread(doSave);                                         \
        return (wxDragResult)rval;                                      \
    }                                                                   \
    wxDragResult CLASS::base_##CBNAME(wxCoord a, wxCoord b, wxDragResult c) { \
        return PCLASS::CBNAME(a, b, c);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_FSF_FSSTRING_pure(CBNAME)                        \
    wxFSFile* CBNAME(wxFileSystem& fs, const wxString& location);       \

#define IMP_PYCALLBACK_FSF_FSSTRING_pure(CLASS, PCLASS, CBNAME)         \
    wxFSFile* CLASS::CBNAME(wxFileSystem& a,const wxString& b) {        \
        bool doSave = wxPyRestoreThread();                              \
        wxFSFile* rval=0;                                               \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* ro;                                               \
            PyObject* obj = wxPyConstructObject(&a, "wxFileSystem", 0); \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Os)",         \
                                          obj, b.c_str()));             \
            if (ro) {                                                   \
                SWIG_GetPtrObj(ro, (void **)&rval, "_wxFSFILE_p");      \
                Py_DECREF(ro);                                          \
            }                                                           \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    };

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DR(CBNAME)                  \
    bool CBNAME(wxDragResult a);                        \
    bool base_##CBNAME(wxDragResult a);


#define IMP_PYCALLBACK_BOOL_DR(CLASS, PCLASS, CBNAME)                   \
    bool CLASS::CBNAME(wxDragResult a) {                                \
        bool doSave = wxPyRestoreThread();                              \
        bool rval;                                                      \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));      \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                             \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iis)",a,b,c.c_str()));\
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                  \
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
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                                   \
            PyObject* ro;                                                       \
            wxDataFormat* ptr;                                                  \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)", a));             \
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


#define IMP_PYCALLBACK__constany(CLASS, PCLASS, CBNAME, Type)           \
    void CLASS::CBNAME(const Type& a) {                                 \
        bool doSave = wxPyRestoreThread();                              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);   \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));           \
            Py_DECREF(obj);                                             \
        }                                                               \
        else                                                            \
            PCLASS::CBNAME(a);                                          \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(const Type& a) {                          \
        PCLASS::CBNAME(a);                                              \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__any(CBNAME, Type)          \
    void CBNAME(Type& a);                          \
    void base_##CBNAME(Type& a);


#define IMP_PYCALLBACK__any(CLASS, PCLASS, CBNAME, Type)                \
    void CLASS::CBNAME(Type& a) {                                       \
        bool doSave = wxPyRestoreThread();                              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);   \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));           \
            Py_DECREF(obj);                                             \
        }                                                               \
        else                                                            \
            PCLASS::CBNAME(a);                                          \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void CLASS::base_##CBNAME(Type& a) {                                \
        PCLASS::CBNAME(a);                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_bool_any(CBNAME, Type)           \
    bool CBNAME(Type& a);                               \
    bool base_##CBNAME(Type& a);


#define IMP_PYCALLBACK_bool_any(CLASS, PCLASS, CBNAME, Type)            \
    bool CLASS::CBNAME(Type& a) {                                       \
        bool rv;                                                        \
        bool doSave = wxPyRestoreThread();                              \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                           \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);   \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));      \
            Py_DECREF(obj);                                             \
        }                                                               \
        else                                                            \
            rv = PCLASS::CBNAME(a);                                     \
        wxPySaveThread(doSave);                                         \
        return rv;                                                      \
    }                                                                   \
    bool CLASS::base_##CBNAME(Type& a) {                                \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#endif




