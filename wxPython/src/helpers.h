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


//---------------------------------------------------------------------------

typedef unsigned char byte;


class wxPyApp: public wxApp
{
public:
    wxPyApp();
    ~wxPyApp();
    bool OnInit();
    int  MainLoop();
};

extern wxPyApp *wxPythonApp;

//----------------------------------------------------------------------

void      __wxPreStart();
PyObject* __wxStart(PyObject*, PyObject* args);
void      __wxCleanup();

//extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

void wxPyEventThunker(wxObject*, wxEvent& event);

PyObject* wxPyConstructObject(void* ptr,
                              const char* className,
                              int setThisOwn=0);
PyObject* wxPyConstructObject(void* ptr,
                              const char* className,
                              PyObject* klass,
                              int setThisOwn=0);
PyObject*  wxPyClassExists(const char* className);
PyObject*  wxPyMake_wxObject(wxObject* source, bool checkEvtHandler=TRUE);
PyObject*  wxPyMake_wxSizer(wxSizer* source);
void       wxPyPtrTypeMap_Add(const char* commonName, const char* ptrName);

PyObject* wxPy_ConvertList(wxListBase* list, const char* className);
long wxPyGetWinHandle(wxWindow* win);

//----------------------------------------------------------------------

// if we want to handle threads and Python threads are available...
#if defined(WXP_USE_THREAD) && defined(WITH_THREAD)
#define WXP_WITH_THREAD
#else  // no Python threads...
#undef WXP_WITH_THREAD
#endif


// For Python --> C++
PyThreadState* wxPyBeginAllowThreads();
void           wxPyEndAllowThreads(PyThreadState* state);

// For C++ --> Python
void wxPyBeginBlockThreads();
void wxPyEndBlockThreads();

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
wxPoint* wxPoint_LIST_helper(PyObject* source, int* npoints);
wxBitmap** wxBitmap_LIST_helper(PyObject* source);
wxString* wxString_LIST_helper(PyObject* source);
wxAcceleratorEntry* wxAcceleratorEntry_LIST_helper(PyObject* source);
wxPen** wxPen_LIST_helper(PyObject* source);

bool wxSize_helper(PyObject* source, wxSize** obj);
bool wxPoint_helper(PyObject* source, wxPoint** obj);
bool wxRealPoint_helper(PyObject* source, wxRealPoint** obj);
bool wxRect_helper(PyObject* source, wxRect** obj);
bool wxColour_helper(PyObject* source, wxColour** obj);

#if PYTHON_API_VERSION < 1009
#define PySequence_Fast_GET_ITEM(o, i) \
     (PyList_Check(o) ? PyList_GET_ITEM(o, i) : PyTuple_GET_ITEM(o, i))
#endif

bool _2int_seq_helper(PyObject* source, int* i1, int* i2);
bool _4int_seq_helper(PyObject* source, int* i1, int* i2, int* i3, int* i4);


PyObject* wxArrayString2PyList_helper(const wxArrayString& arr);


#define RETURN_NONE()   { Py_INCREF(Py_None); return Py_None; }

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
    DECLARE_ABSTRACT_CLASS(wxPyEvent)
public:
    wxPyEvent(int id=0);
    wxPyEvent(const wxPyEvent& evt);
    ~wxPyEvent();

    virtual wxEvent* Clone() const { return new wxPyEvent(*this); }
};


class wxPyCommandEvent : public wxCommandEvent, public wxPyEvtSelfRef {
    DECLARE_ABSTRACT_CLASS(wxPyCommandEvent)
public:
    wxPyCommandEvent(wxEventType commandType = wxEVT_NULL, int id=0);
    wxPyCommandEvent(const wxPyCommandEvent& evt);
    ~wxPyCommandEvent();

    virtual wxEvent* Clone() const { return new wxPyCommandEvent(*this); }
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

    PyThreadState* (*p_wxPyBeginAllowThreads)();
    void           (*p_wxPyEndAllowThreads)(PyThreadState* state);
    void        (*p_wxPyBeginBlockThreads)();
    void        (*p_wxPyEndBlockThreads)();

    PyObject*   (*p_wxPyConstructObject)(void *, const char *, int);
    PyObject*   (*p_wxPy_ConvertList)(wxListBase* list, const char* className);

    byte*       (*p_byte_LIST_helper)(PyObject* source);
    int*        (*p_int_LIST_helper)(PyObject* source);
    long*       (*p_long_LIST_helper)(PyObject* source);
    char**      (*p_string_LIST_helper)(PyObject* source);
    wxPoint*    (*p_wxPoint_LIST_helper)(PyObject* source, int* npoints);
    wxBitmap**  (*p_wxBitmap_LIST_helper)(PyObject* source);
    wxString*   (*p_wxString_LIST_helper)(PyObject* source);
    wxAcceleratorEntry*   (*p_wxAcceleratorEntry_LIST_helper)(PyObject* source);

    bool        (*p_wxSize_helper)(PyObject* source, wxSize** obj);
    bool        (*p_wxPoint_helper)(PyObject* source, wxPoint** obj);
    bool        (*p_wxRealPoint_helper)(PyObject* source, wxRealPoint** obj);
    bool        (*p_wxRect_helper)(PyObject* source, wxRect** obj);
    bool        (*p_wxColour_helper)(PyObject* source, wxColour** obj);

    void        (*p_wxPyCBH_setCallbackInfo)(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
    bool        (*p_wxPyCBH_findCallback)(const wxPyCallbackHelper& cbh, const char* name);
    int         (*p_wxPyCBH_callCallback)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    PyObject*   (*p_wxPyCBH_callCallbackObj)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    void        (*p_wxPyCBH_delete)(wxPyCallbackHelper* cbh);

    PyObject*   (*p_wxPyClassExists)(const char* className);
    PyObject*   (*p_wxPyMake_wxObject)(wxObject* source, bool checkEvtHandler);
    PyObject*   (*p_wxPyMake_wxSizer)(wxSizer* source);
    void        (*p_wxPyPtrTypeMap_Add)(const char* commonName, const char* ptrName);
    PyObject*   (*p_wxArrayString2PyList_helper)(const wxArrayString& arr);
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


void wxPyCBH_setCallbackInfo(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
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
#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyCoreAPIPtr->p_wxPyEndBlockThreads();
#else
        wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads();
#endif
    }
    PyObject* m_obj;
};



class wxPyClientData : public wxClientData {
public:
    wxPyClientData(PyObject* obj) {
        m_obj = obj;
        Py_INCREF(m_obj);
    }

    ~wxPyClientData() {
#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyCoreAPIPtr->p_wxPyEndBlockThreads();
#else
        wxPyBeginBlockThreads();
        Py_DECREF(m_obj);
        wxPyEndBlockThreads();
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
    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref=1) {     \
        wxPyCBH_setCallbackInfo(m_myInst, self, _class, incref);                \
    }                                                                   \
    private: wxPyCallbackHelper m_myInst

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__(CBNAME)                        \
    void CBNAME();                                      \
    void base_##CBNAME();


#define IMP_PYCALLBACK__(CLASS, PCLASS, CBNAME)                         \
    void CLASS::CBNAME() {                                              \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));        \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME();                                           \
    }                                                                   \
    void CLASS::base_##CBNAME() {                                       \
        PCLASS::CBNAME();                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINT(CBNAME)                      \
    bool CBNAME(int a, int b);                                  \
    bool base_##CBNAME(int a, int b);


#define IMP_PYCALLBACK_BOOL_INTINT(CLASS, PCLASS, CBNAME)               \
    bool CLASS::CBNAME(int a, int b) {                                  \
        bool rval=FALSE, found;                                         \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));    \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            rval = PCLASS::CBNAME(a,b);                                 \
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
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));  \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b);                                        \
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
        bool rval=FALSE, found;                                         \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));\
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            rval = PCLASS::CBNAME(a);                                   \
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
        bool rval=FALSE;                                                \
        wxPyBeginBlockThreads();                    \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                    \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));       \
        else rval = FALSE;                                              \
        wxPyEndBlockThreads();                                     \
        return rval;                                                    \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC(CBNAME)                      \
    void CBNAME(wxDC& a);                               \
    void base_##CBNAME(wxDC& a);


#define IMP_PYCALLBACK__DC(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxDC& a) {                                       \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a);                      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a);                                          \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a) {                                \
        PCLASS::CBNAME(a);                                              \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b);


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a);                      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
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
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a);                      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL(CBNAME)                    \
    void CBNAME(double a, double b);                    \
    void base_##CBNAME(double a, double b);


#define IMP_PYCALLBACK__2DBL(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(double a, double b) {                            \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(dd)",a,b));  \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b) {                     \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL2INT(CBNAME)                        \
    void CBNAME(double a, double b, int c, int d);              \
    void base_##CBNAME(double a, double b, int c, int d);


#define IMP_PYCALLBACK__2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(double a, double b, int c, int d) {              \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddii)",      \
                                                       a,b,c,d));       \
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b, c, d);                                 \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b, int c, int d) {       \
        PCLASS::CBNAME(a, b, c, d);                                     \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBLBOOL(CBNAME)                                      \
    void CBNAME(wxDC& a, double b, double c, double d, double e, bool f);       \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f);


#define IMP_PYCALLBACK__DC4DBLBOOL(CLASS, PCLASS, CBNAME)                               \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {       \
        bool found;                                                                     \
        wxPyBeginBlockThreads();                                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* obj = wxPyMake_wxObject(&a);                                      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));  \
            Py_DECREF(obj);                                                             \
        }                                                                               \
        wxPyEndBlockThreads();                                                     \
        if (! found)                                                                    \
            PCLASS::CBNAME(a, b, c, d, e, f);                                           \
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
        bool found;                                                                     \
        wxPyBeginBlockThreads();                                    \
        bool rval=FALSE;                                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* obj = wxPyMake_wxObject(&a);                                      \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));\
            Py_DECREF(obj);                                                             \
        }                                                                               \
        wxPyEndBlockThreads();                                                     \
        if (! found)                                                                    \
            rval = PCLASS::CBNAME(a, b, c, d, e, f);                                    \
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
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddii)",             \
                                                (int)a,b,c,d,e));               \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(bool a, double b, double c, int d, int e) {       \
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBL(CBNAME)                                          \
    void CBNAME(wxDC& a, double b, double c, double d, double e);               \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e);


#define IMP_PYCALLBACK__DC4DBL(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e) {       \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a);                              \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Odddd)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e) {\
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b);


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(wxDC& a, bool b) {                                       \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a);                              \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b)); \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b);                                               \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                                \
        PCLASS::CBNAME(a, b);                                                   \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__WXCPBOOL2DBL2INT(CBNAME)                                \
    void CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f);   \
    void base_##CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f);


#define IMP_PYCALLBACK__WXCPBOOL2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(wxControlPoint* a, bool b, double c, double d,           \
                int e, int f) {                                                 \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a);                               \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oiddii)", obj, (int)b, c, d, e, f));\
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e, f);                                   \
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
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a);                               \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddii)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(wxControlPoint* a, double b, double c,            \
                       int d, int e) {                                          \
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBLINT(CBNAME)                 \
    void CBNAME(double a, double b, int c);             \
    void base_##CBNAME(double a, double b, int c);


#define IMP_PYCALLBACK__2DBLINT(CLASS, PCLASS, CBNAME)                          \
    void CLASS::CBNAME(double a, double b, int c) {                             \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddi)", a,b,c));      \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c);                                            \
    }                                                                           \
    void CLASS::base_##CBNAME(double a, double b, int c) {                      \
        PCLASS::CBNAME(a, b, c);                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__BOOL2DBLINT(CBNAME)                     \
    void CBNAME(bool a, double b, double c, int d);             \
    void base_##CBNAME(bool a, double b, double c, int d);


#define IMP_PYCALLBACK__BOOL2DBLINT(CLASS, PCLASS, CBNAME)                      \
    void CLASS::CBNAME(bool a, double b, double c, int d) {                     \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddi)", (int)a,b,c,d));\
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d);                                         \
    }                                                                           \
    void CLASS::base_##CBNAME(bool a, double b, double c, int d) {              \
        PCLASS::CBNAME(a, b, c, d);                                             \
    }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__STRING(CBNAME)                  \
    void CBNAME(const wxString& a);                     \
    void base_##CBNAME(const wxString& a);


#define IMP_PYCALLBACK__STRING(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(const wxString& a)  {                                    \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));    \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(const wxString& a) {                              \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING(CBNAME)              \
    bool CBNAME(const wxString& a);                     \
    bool base_##CBNAME(const wxString& a);


#define IMP_PYCALLBACK_BOOL_STRING(CLASS, PCLASS, CBNAME)                       \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));\
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(const wxString& a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING_pure(CBNAME)         \
    bool CBNAME(const wxString& a);
                   \
#define IMP_PYCALLBACK_BOOL_STRING_pure(CLASS, PCLASS, CBNAME)                          \
    bool CLASS::CBNAME(const wxString& a)  {                                            \
        bool rval=FALSE;                                                                \
        wxPyBeginBlockThreads();                                    \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                                    \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(s)", a.c_str()));     \
        wxPyEndBlockThreads();                                                     \
        return rval;                                                                    \
    }                                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING_pure(CBNAME)                               \
    wxString CBNAME(const wxString& a);                                         \

#define IMP_PYCALLBACK_STRING_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(s)", a.c_str()));\
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRINGINT_pure(CBNAME)                            \
    wxString CBNAME(const wxString& a,int b);                                   \

#define IMP_PYCALLBACK_STRING_STRINGINT_pure(CLASS, PCLASS, CBNAME)             \
    wxString CLASS::CBNAME(const wxString& a,int b)  {                          \
        wxString rval;                                                          \
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(si)", a.c_str(),b));  \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRINGSTRING(CBNAME)              \
    bool CBNAME(const wxString& a, const wxString& b);        \
    bool base_##CBNAME(const wxString& a, const wxString& b);


#define IMP_PYCALLBACK_BOOL_STRINGSTRING(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(const wxString& a, const wxString& b) {                  \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ss)",         \
                                                       a.c_str(), b.c_str()));  \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b);                                        \
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
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);  Py_DECREF(str);                                 \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
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
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_TAG_pure(CBNAME)                  \
    bool CBNAME(const wxHtmlTag& a);                          \


#define IMP_PYCALLBACK_BOOL_TAG_pure(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME(const wxHtmlTag& a)  {                                   \
        bool rval=FALSE;                                                        \
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)&a, "wxHtmlTag", 0);     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__CELLINTINT(CBNAME)                                      \
    void CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y);                        \
    void base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y);

#define IMP_PYCALLBACK__CELLINTINT(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y) {                \
        wxString rval;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)cell, "wxHtmlCell", 0);  \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Oii)",obj,x,y));  \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(cell, x, y);                                         \
    }                                                                           \
    void CLASS::base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y) {         \
        PCLASS::CBNAME(cell, x, y);                                             \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__CELLINTINTME(CBNAME)                                    \
    void CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e); \
    void base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e);

#define IMP_PYCALLBACK__CELLINTINTME(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e) {                \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)cell, "wxHtmlCell", 0);  \
            PyObject* o2  = wxPyConstructObject((void*)&e, "wxMouseEvent", 0);  \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OiiO)",obj,x,y,o2));  \
            Py_DECREF(obj);                                                     \
            Py_DECREF(o2);                                                      \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(cell, x, y, e);                                         \
    }                                                                           \
    void CLASS::base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e) {         \
        PCLASS::CBNAME(cell, x, y, e);                                             \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK___pure(CBNAME)                         \
    void CBNAME();                                            \


#define IMP_PYCALLBACK___pure(CLASS, PCLASS, CBNAME)                            \
    void CLASS::CBNAME() {                                                      \
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                \
        wxPyEndBlockThreads();                                             \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_wxSize__pure(CBNAME)                         \
    wxSize CBNAME();                                                \


#define IMP_PYCALLBACK_wxSize__pure(CLASS, PCLASS, CBNAME)                      \
    wxSize CLASS::CBNAME() {                                                    \
        wxSize rval(0,0);                                                       \
        wxPyBeginBlockThreads();                            \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            wxSize*   ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxSize_p"))           \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_WXWIN(CBNAME)                      \
    bool CBNAME(wxWindow* a);                                  \
    bool base_##CBNAME(wxWindow* a);


#define IMP_PYCALLBACK_BOOL_WXWIN(CLASS, PCLASS, CBNAME)                        \
    bool CLASS::CBNAME(wxWindow* a) {                                           \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a);                               \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxWindow* a) {                                    \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_(CBNAME)                      \
    bool CBNAME();                                        \
    bool base_##CBNAME();


#define IMP_PYCALLBACK_BOOL_(CLASS, PCLASS, CBNAME)                             \
    bool CLASS::CBNAME() {                                                      \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME() {                                               \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DR_2WXCDR(CBNAME)                                \
    wxDragResult CBNAME(wxCoord x, wxCoord y, wxDragResult def);        \
    wxDragResult base_##CBNAME(wxCoord x, wxCoord y, wxDragResult def);


#define IMP_PYCALLBACK_DR_2WXCDR(CLASS, PCLASS, CBNAME)                         \
    wxDragResult CLASS::CBNAME(wxCoord a, wxCoord b, wxDragResult c) {          \
        int rval=0;                                                             \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b, c);                                     \
        return (wxDragResult)rval;                                              \
    }                                                                           \
    wxDragResult CLASS::base_##CBNAME(wxCoord a, wxCoord b, wxDragResult c) {   \
        return PCLASS::CBNAME(a, b, c);                                         \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_FSF_FSSTRING_pure(CBNAME)                        \
    wxFSFile* CBNAME(wxFileSystem& fs, const wxString& location);       \

#define IMP_PYCALLBACK_FSF_FSSTRING_pure(CLASS, PCLASS, CBNAME)         \
    wxFSFile* CLASS::CBNAME(wxFileSystem& a,const wxString& b) {        \
        wxPyBeginBlockThreads();                    \
        wxFSFile* rval=0;                                               \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                  \
            PyObject* ro;                                               \
            PyObject* obj = wxPyMake_wxObject(&a);                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Os)",\
                                          obj, b.c_str()));             \
            if (ro) {                                                   \
                SWIG_GetPtrObj(ro, (void **)&rval, "_wxFSFILE_p");      \
                Py_DECREF(ro);                                          \
            }                                                           \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads();                                     \
        return rval;                                                    \
    };

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DR(CBNAME)                  \
    bool CBNAME(wxDragResult a);                        \
    bool base_##CBNAME(wxDragResult a);


#define IMP_PYCALLBACK_BOOL_DR(CLASS, PCLASS, CBNAME)                   \
    bool CLASS::CBNAME(wxDragResult a) {                                \
        bool rval=FALSE;                                                \
        bool found;                                                     \
        wxPyBeginBlockThreads();                    \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));\
        wxPyEndBlockThreads();                                     \
        if (! found)                                                    \
            rval = PCLASS::CBNAME(a);                                   \
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
        wxPyBeginBlockThreads();                    \
        int rval=0;                                                     \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                    \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads();                                     \
        return (wxDragResult)rval;                                      \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINTSTR_pure(CBNAME)                      \
    bool CBNAME(int a, int b, const wxString& c);


#define IMP_PYCALLBACK_BOOL_INTINTSTR_pure(CLASS, PCLASS, CBNAME)       \
    bool CLASS::CBNAME(int a, int b, const wxString& c) {               \
        bool rval=FALSE;                                                \
        wxPyBeginBlockThreads();                    \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                    \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iis)",a,b,c.c_str()));\
        wxPyEndBlockThreads();                                     \
        return rval;                                                    \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZET_(CBNAME)                  \
    size_t CBNAME();                                   \
    size_t base_##CBNAME();


#define IMP_PYCALLBACK_SIZET_(CLASS, PCLASS, CBNAME)                            \
    size_t CLASS::CBNAME() {                                                    \
        size_t rval=0;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    size_t CLASS::base_##CBNAME() {                                             \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DATAFMT_SIZET(CBNAME)                                    \
    wxDataFormat  CBNAME(size_t a);                                             \
    wxDataFormat  base_##CBNAME(size_t a);


#define IMP_PYCALLBACK_DATAFMT_SIZET(CLASS, PCLASS, CBNAME)                     \
    wxDataFormat CLASS::CBNAME(size_t a) {                                      \
        wxDataFormat rval=0;                                                    \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxDataFormat* ptr;                                                  \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)", a));    \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxDataFormat_p"))     \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    wxDataFormat  CLASS::base_##CBNAME(size_t a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__constany(CBNAME, Type)          \
    void CBNAME(const Type& a);                         \
    void base_##CBNAME(const Type& a);


#define IMP_PYCALLBACK__constany(CLASS, PCLASS, CBNAME, Type)                   \
    void CLASS::CBNAME(const Type& a) {                                         \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);           \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(const Type& a) {                                  \
        PCLASS::CBNAME(a);                                                      \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__any(CBNAME, Type)          \
    void CBNAME(Type& a);                          \
    void base_##CBNAME(Type& a);


#define IMP_PYCALLBACK__any(CLASS, PCLASS, CBNAME, Type)                        \
    void CLASS::CBNAME(Type& a) {                                               \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);           \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(Type& a) {                                        \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_bool_any(CBNAME, Type)           \
    bool CBNAME(Type& a);                               \
    bool base_##CBNAME(Type& a);


#define IMP_PYCALLBACK_bool_any(CLASS, PCLASS, CBNAME, Type)                    \
    bool CLASS::CBNAME(Type& a) {                                               \
        bool rv=FALSE;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, #Type, 0);           \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rv = PCLASS::CBNAME(a);                                             \
        return rv;                                                              \
    }                                                                           \
    bool CLASS::base_##CBNAME(Type& a) {                                        \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_LONGLONG(CBNAME)                                  \
    wxString CBNAME(long a, long b) const;                                      \
    wxString base_##CBNAME(long a, long b)const ;


#define IMP_PYCALLBACK_STRING_LONGLONG(CLASS, PCLASS, CBNAME)                   \
    wxString CLASS::CBNAME(long a, long b) const {                              \
        wxString rval;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ll)",a,b));  \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyString_AsString(str);                                  \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a,b);                                         \
        return rval;                                                            \
    }                                                                           \
    wxString CLASS::base_##CBNAME(long a, long b) const {                       \
        return PCLASS::CBNAME(a,b);                                             \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_INT_LONG(CBNAME)                                         \
    int CBNAME(long a) const;                                                   \
    int base_##CBNAME(long a)const ;


#define IMP_PYCALLBACK_INT_LONG(CLASS, PCLASS, CBNAME)                          \
    int CLASS::CBNAME(long a) const {                                           \
        int rval=-1;                                                            \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = PyInt_AsLong(ro);                                        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    int CLASS::base_##CBNAME(long a) const {                                    \
        return PCLASS::CBNAME(a);                                               \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_LISTATTR_LONG(CBNAME)                                    \
    wxListItemAttr*  CBNAME(long a) const;                                      \
    wxListItemAttr*  base_##CBNAME(long a);


#define IMP_PYCALLBACK_LISTATTR_LONG(CLASS, PCLASS, CBNAME)                     \
    wxListItemAttr *CLASS::CBNAME(long a) const {                               \
        wxListItemAttr *rval = NULL;                                            \
        bool found;                                                             \
        wxPyBeginBlockThreads();                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxListItemAttr* ptr;                                                \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)", a));    \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxListItemAttr_p"))   \
                    rval = ptr;                                                 \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                             \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    wxListItemAttr *CLASS::base_##CBNAME(long a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_ME(CBNAME) \
    bool CBNAME(wxMouseEvent& e); \
    bool base_##CBNAME(wxMouseEvent& e);

#define IMP_PYCALLBACK_BOOL_ME(CLASS, PCLASS, CBNAME) \
    bool CLASS::CBNAME(wxMouseEvent& e) { \
        bool rval=FALSE; \
        bool found; \
        wxPyBeginBlockThreads(); \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) { \
            PyObject* ro; \
            PyObject* obj  = wxPyConstructObject((void*)&e, "wxMouseEvent", 0);  \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)",obj));  \
            if (ro) { \
                rval = PyInt_AsLong(ro); \
                Py_DECREF(ro); \
            } \
            Py_DECREF(obj); \
        } \
        wxPyEndBlockThreads(); \
        if (! found) \
            return PCLASS::CBNAME(e); \
        return rval; \
    } \
    bool CLASS::base_##CBNAME(wxMouseEvent& e) { \
        return PCLASS::CBNAME(e); \
    }


//---------------------------------------------------------------------------

#endif
