////////////////////////////////////////////////////////////////////////////
// Name:        helpers.h
// Purpose:     Helper functions/classes for the wxPython extension module
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
#include <wx/geometry.h>

//---------------------------------------------------------------------------

typedef unsigned char byte;

void      __wxPreStart(PyObject*);
PyObject* __wxStart(PyObject*, PyObject* args);
void      __wxCleanup();

//extern PyObject* wxPython_dict;
PyObject* __wxSetDictionary(PyObject*, PyObject* args);

void wxPyEventThunker(wxObject*, wxEvent& event);

PyObject* wxPyConstructObject(void* ptr,
                              const wxString& className,
                              int setThisOwn=0);
PyObject* wxPyConstructObject(void* ptr,
                              const wxString& className,
                              PyObject* klass,
                              int setThisOwn=0);

PyObject* wx2PyString(const wxString& src);
wxString  Py2wxString(PyObject* source);

PyObject* wxPyClassExists(const wxString& className);
PyObject* wxPyMake_wxObject(wxObject* source, bool checkEvtHandler=TRUE);
PyObject* wxPyMake_wxSizer(wxSizer* source);
void      wxPyPtrTypeMap_Add(const char* commonName, const char* ptrName);

PyObject* wxPy_ConvertList(wxListBase* list, const char* className);
long      wxPyGetWinHandle(wxWindow* win);

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

#define wxPyBLOCK_THREADS(stmt) wxPyBeginBlockThreads(); stmt; wxPyEndBlockThreads()

//----------------------------------------------------------------------
// These are helpers used by the typemaps

wxString* wxString_in_helper(PyObject* source);

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

bool wxPoint2DDouble_helper(PyObject* source, wxPoint2DDouble** obj);


//----------------------------------------------------------------------
// Other helpful stuff

#if PYTHON_API_VERSION < 1009
#define PySequence_Fast_GET_ITEM(o, i) \
     (PyList_Check(o) ? PyList_GET_ITEM(o, i) : PyTuple_GET_ITEM(o, i))
#endif

bool wxPy2int_seq_helper(PyObject* source, int* i1, int* i2);
bool wxPy4int_seq_helper(PyObject* source, int* i1, int* i2, int* i3, int* i4);


PyObject* wxArrayString2PyList_helper(const wxArrayString& arr);
PyObject* wxArrayInt2PyList_helper(const wxArrayInt& arr);

#define RETURN_NONE()                 { Py_INCREF(Py_None); return Py_None; }
#define DECLARE_DEF_STRING(name)      static const wxString wxPy##name(wx##name)
#define DECLARE_DEF_STRING2(name,val) static const wxString wxPy##name(val)

//----------------------------------------------------------------------
// functions used by the DrawXXXList enhancements added to wxDC

typedef bool (*wxPyDrawListOp_t)(wxDC& dc, PyObject* coords);
PyObject* wxPyDrawXXXList(wxDC& dc, wxPyDrawListOp_t doDraw,
                          PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes);
bool wxPyDrawXXXPoint(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXLine(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXRectangle(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXEllipse(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXPolygon(wxDC& dc, PyObject* coords);

PyObject* wxPyDrawTextList(wxDC& dc, PyObject* textList, PyObject* pyPoints,
                           PyObject* foregroundList, PyObject* backgroundList);

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



//----------------------------------------------------------------------
// Forward decalre a few things used in the exported API
class wxPyClientData;
class wxPyUserData;
class wxPyOORClientData;
class wxPyCBInputStream;

void wxPyClientData_dtor(wxPyClientData* self);
void wxPyUserData_dtor(wxPyUserData* self);
void wxPyOORClientData_dtor(wxPyOORClientData* self);
wxPyCBInputStream* wxPyCBInputStream_create(PyObject *py, bool block);


//---------------------------------------------------------------------------
// Export a C API in a struct.  Other modules will be able to load this from
// the wxc module and will then have safe access to these functions, even if
// in another shared library.

class wxPyCallbackHelper;


// Make SunCC happy and make typedef's for these that are extern "C"
typedef void        (*p_SWIG_MakePtr_t)(char*, void*, char*);
typedef char*       (*p_SWIG_GetPtr_t)(char*, void**, char*);
typedef char*       (*p_SWIG_GetPtrObj_t)(PyObject*, void**, char*);
typedef void        (*p_SWIG_RegisterMapping_t)(char*, char*, void *(*cast)(void *));
typedef void        (*p_SWIG_addvarlink_t)(PyObject*, char*, PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p));
typedef PyObject*   (*p_SWIG_newvarlink_t)(void);


struct wxPyCoreAPI {

    p_SWIG_MakePtr_t           p_SWIG_MakePtr;
    p_SWIG_GetPtr_t            p_SWIG_GetPtr;
    p_SWIG_GetPtrObj_t         p_SWIG_GetPtrObj;
    p_SWIG_RegisterMapping_t   p_SWIG_RegisterMapping;
    p_SWIG_addvarlink_t        p_SWIG_addvarlink;
    p_SWIG_newvarlink_t        p_SWIG_newvarlink;

    PyThreadState* (*p_wxPyBeginAllowThreads)();
    void           (*p_wxPyEndAllowThreads)(PyThreadState* state);
    void        (*p_wxPyBeginBlockThreads)();
    void        (*p_wxPyEndBlockThreads)();

    PyObject*   (*p_wxPyConstructObject)(void *, const wxString&, int);
    PyObject*   (*p_wxPy_ConvertList)(wxListBase* list, const char* className);

    wxString*   (*p_wxString_in_helper)(PyObject* source);
    wxString    (*p_Py2wxString)(PyObject* source);
    PyObject*   (*p_wx2PyString)(const wxString& src);

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
    bool        (*p_wxPoint2DDouble_helper)(PyObject* source, wxPoint2DDouble** obj);

    void        (*p_wxPyCBH_setCallbackInfo)(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
    bool        (*p_wxPyCBH_findCallback)(const wxPyCallbackHelper& cbh, const char* name);
    int         (*p_wxPyCBH_callCallback)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    PyObject*   (*p_wxPyCBH_callCallbackObj)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    void        (*p_wxPyCBH_delete)(wxPyCallbackHelper* cbh);

    PyObject*   (*p_wxPyClassExists)(const wxString& className);
    PyObject*   (*p_wxPyMake_wxObject)(wxObject* source, bool checkEvtHandler);
    PyObject*   (*p_wxPyMake_wxSizer)(wxSizer* source);
    void        (*p_wxPyPtrTypeMap_Add)(const char* commonName, const char* ptrName);
    PyObject*   (*p_wxArrayString2PyList_helper)(const wxArrayString& arr);
    PyObject*   (*p_wxArrayInt2PyList_helper)(const wxArrayInt& arr);

    void        (*p_wxPyClientData_dtor)(wxPyClientData*);
    void        (*p_wxPyUserData_dtor)(wxPyUserData*);
    void        (*p_wxPyOORClientData_dtor)(wxPyOORClientData*);

    wxPyCBInputStream* (*p_wxPyCBInputStream_create)(PyObject *py, bool block);

};

#ifdef wxPyUSE_EXPORT
// Notice that this is static, not extern.  This is by design, each module
// needs one, but doesn't have to use it.
static wxPyCoreAPI* wxPyCoreAPIPtr = NULL;
#endif


//---------------------------------------------------------------------------


class wxPyUserData : public wxObject {
public:
    wxPyUserData(PyObject* obj) {
        m_obj = obj;
        Py_INCREF(m_obj);
    }

    ~wxPyUserData() {
#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPyUserData_dtor(this);
#else
        wxPyUserData_dtor(this);
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
        wxPyCoreAPIPtr->p_wxPyClientData_dtor(this);
#else
        wxPyClientData_dtor(this);
#endif
    }
    PyObject* m_obj;
};


class wxPyOORClientData : public wxPyClientData {
public:
    wxPyOORClientData(PyObject* obj)
        : wxPyClientData(obj) {}

    ~wxPyOORClientData() {
#ifdef wxPyUSE_EXPORT
        wxPyCoreAPIPtr->p_wxPyOORClientData_dtor(this);
#else
        wxPyOORClientData_dtor(this);
#endif
    }
};

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




//---------------------------------------------------------------------------

// This is used in C++ classes that need to be able to make callback to
// "overloaded" python methods

#define PYPRIVATE                                                               \
    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref=1) {     \
        wxPyCBH_setCallbackInfo(m_myInst, self, _class, incref);                \
    }                                                                           \
    private: wxPyCallbackHelper m_myInst


//---------------------------------------------------------------------------

enum {
    wxPYAPP_ASSERT_SUPPRESS  = 1,
    wxPYAPP_ASSERT_EXCEPTION = 2,
    wxPYAPP_ASSERT_DIALOG    = 4,
    wxPYAPP_ASSERT_LOG       = 8
};

class wxPyApp: public wxApp
{
    DECLARE_ABSTRACT_CLASS(wxPyApp);

public:
    wxPyApp();
    ~wxPyApp();
    bool OnInit();
    int  MainLoop();

    int  GetAssertMode() { return m_assertMode; }
    void SetAssertMode(int mode) { m_assertMode = mode; }

    virtual bool OnInitGui();
    virtual int OnExit();
#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file,
                          int line,
                          const wxChar *cond,
                          const wxChar *msg);
#endif
    // virtual int FilterEvent(wxEvent& event); // This one too????


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


    PYPRIVATE;
    int m_assertMode;
};

extern wxPyApp *wxPythonApp;


//----------------------------------------------------------------------
// These macros are used to implement the virtual methods that should
// redirect to a Python method if one exists.  The names designate the
// return type, if any, as well as any parameter types.
//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__(CBNAME)                        \
    void CBNAME();                                      \
    void base_##CBNAME();


#define IMP_PYCALLBACK__(CLASS, PCLASS, CBNAME)                         \
    void CLASS::CBNAME() {                                              \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));        \
        wxPyEndBlockThreads();                                          \
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

#define DEC_PYCALLBACK_VOID_(CBNAME)                    \
    void CBNAME();                                      \
    void base_##CBNAME();


#define IMP_PYCALLBACK_VOID_(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME() {                                              \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));        \
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            PCLASS::CBNAME();                                           \
    }                                                                   \
    void CLASS::base_##CBNAME() {                                       \
        PCLASS::CBNAME();                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INTINT(CBNAME)                      \
    void CBNAME(int a, int b);                                  \
    void base_##CBNAME(int a, int b);


#define IMP_PYCALLBACK_VOID_INTINT(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(int a, int b) {                                  \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));  \
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b);                                        \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b) {                           \
        PCLASS::CBNAME(a,b);                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INT(CBNAME)                         \
    void CBNAME(int a);                                         \
    void base_##CBNAME(int a);


#define IMP_PYCALLBACK_VOID_INT(CLASS, PCLASS, CBNAME)                  \
    void CLASS::CBNAME(int a) {                                         \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));     \
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            PCLASS::CBNAME(a);                                          \
    }                                                                   \
    void CLASS::base_##CBNAME(int a) {                                  \
        PCLASS::CBNAME(a);                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INT4(CBNAME)                                \
    void CBNAME(int a, int b, int c, int d);                            \
    void base_##CBNAME(int a, int b, int c, int d);


#define IMP_PYCALLBACK_VOID_INT4(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(int a, int b, int c, int d) {                    \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiii)",a,b,c,d));  \
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b,c,d);                                    \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b, int c, int d) {             \
        PCLASS::CBNAME(a,b,c,d);                                        \
    }

//---------------------------------------------------------------------------
#define DEC_PYCALLBACK_VOID_INT5(CBNAME)                                \
    void CBNAME(int a, int b, int c, int d, int e);                     \
    void base_##CBNAME(int a, int b, int c, int d, int e);


#define IMP_PYCALLBACK_VOID_INT5(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(int a, int b, int c, int d, int e) {             \
        bool found;                                                     \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiiii)",a,b,c,d,e));  \
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b,c,d,e);                                  \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b, int c, int d, int e) {      \
        PCLASS::CBNAME(a,b,c,d,e);                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INTPINTP_const(CBNAME)                              \
    void CBNAME(int* a, int* b) const;                                          \
    void base_##CBNAME(int* a, int* b) const;


#define IMP_PYCALLBACK_VOID_INTPINTP_const(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(int* a, int* b) const {                                  \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers.";   \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (PySequence_Check(ro) && PyObject_Length(ro) == 2) {         \
                    PyObject* o1 = PySequence_GetItem(ro, 0);                   \
                    PyObject* o2 = PySequence_GetItem(ro, 1);                   \
                    if (PyNumber_Check(o1) && PyNumber_Check(o2)) {             \
                        *a = PyInt_AsLong(o1); *b = PyInt_AsLong(o2);           \
                    }                                                           \
                    else                                                        \
                        PyErr_SetString(PyExc_TypeError, errmsg);               \
                    Py_DECREF(o1);                                              \
                    Py_DECREF(o2);                                              \
                }                                                               \
                else {                                                          \
                    PyErr_SetString(PyExc_TypeError, errmsg);                   \
                }                                                               \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b);                                                \
    }                                                                           \
    void CLASS::base_##CBNAME(int* a, int* b) const {                           \
        PCLASS::CBNAME(a,b);                                                    \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZE_const(CBNAME)                                       \
    wxSize CBNAME() const;                                                      \
    wxSize base_##CBNAME() const;


#define IMP_PYCALLBACK_SIZE_const(CLASS, PCLASS, CBNAME)                        \
    wxSize CLASS::CBNAME() const {                                              \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers.";   \
        bool found; wxSize rval(0,0);                                           \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (PySequence_Check(ro) && PyObject_Length(ro) == 2) {         \
                    PyObject* o1 = PySequence_GetItem(ro, 0);                   \
                    PyObject* o2 = PySequence_GetItem(ro, 1);                   \
                    if (PyNumber_Check(o1) && PyNumber_Check(o2)) {             \
                        rval = wxSize(PyInt_AsLong(o1), PyInt_AsLong(o2));      \
                    }                                                           \
                    else                                                        \
                        PyErr_SetString(PyExc_TypeError, errmsg);               \
                    Py_DECREF(o1);                                              \
                    Py_DECREF(o2);                                              \
                }                                                               \
                else {                                                          \
                    PyErr_SetString(PyExc_TypeError, errmsg);                   \
                }                                                               \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            return PCLASS::CBNAME();                                            \
        else                                                                    \
            return rval;                                                        \
    }                                                                           \
    wxSize CLASS::base_##CBNAME() const {                                       \
        return PCLASS::CBNAME();                                                \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_BOOL(CBNAME)                         \
    bool CBNAME(bool a);                                         \
    bool base_##CBNAME(bool a);


#define IMP_PYCALLBACK_BOOL_BOOL(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(bool a) {                                        \
        bool rval=FALSE, found;                                         \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));\
        wxPyEndBlockThreads();                                          \
        if (! found)                                                    \
            rval = PCLASS::CBNAME(a);                                   \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(bool a) {                                 \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INT(CBNAME)                         \
    bool CBNAME(int a);                                         \
    bool base_##CBNAME(int a);


#define IMP_PYCALLBACK_BOOL_INT(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(int a) {                                         \
        bool rval=FALSE, found;                                         \
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));\
        wxPyEndBlockThreads();                                          \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddi)", (int)a,b,c,d));\
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(a);                                       \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));            \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(a);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));     \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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

#define IMP_PYCALLBACK_BOOL_STRING_pure(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval=FALSE;                                                        \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* s = wx2PyString(a);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));     \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING_pure(CBNAME)                               \
    wxString CBNAME(const wxString& a);                                         \

#define IMP_PYCALLBACK_STRING_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            PyObject* s = wx2PyString(a);                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)", s));    \
            Py_DECREF(s);                                                       \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING(CBNAME)                                    \
    wxString CBNAME(const wxString& a);                                         \
    wxString base_##CBNAME(const wxString& a);

#define IMP_PYCALLBACK_STRING_STRING(CLASS, PCLASS, CBNAME)                     \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            PyObject* s = wx2PyString(a);                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)", s));    \
            Py_DECREF(s);                                                       \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        wxPyEndBlockThreads();                                                  \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRINGINT_pure(CBNAME)                            \
    wxString CBNAME(const wxString& a,int b);                                   \

#define IMP_PYCALLBACK_STRING_STRINGINT_pure(CLASS, PCLASS, CBNAME)             \
    wxString CLASS::CBNAME(const wxString& a,int b)  {                          \
        wxString rval;                                                          \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            PyObject* s = wx2PyString(a);                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Oi)",s,b));  \
            Py_DECREF(s);                                                       \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s1 = wx2PyString(a);                                      \
            PyObject* s2 = wx2PyString(b);                                      \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)",s1,s2)); \
            Py_DECREF(s1);                                                      \
            Py_DECREF(s2);                                                      \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_TAG_pure(CBNAME)                  \
    bool CBNAME(const wxHtmlTag& a);                          \


#define IMP_PYCALLBACK_BOOL_TAG_pure(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME(const wxHtmlTag& a)  {                                   \
        bool rval=FALSE;                                                        \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT("wxHtmlTag"), 0);     \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__CELLINTINT(CBNAME)                                      \
    void CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y);                        \
    void base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y);

#define IMP_PYCALLBACK__CELLINTINT(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y) {                \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)cell, wxT("wxHtmlCell"), 0);  \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Oii)",obj,x,y));  \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)cell, wxT("wxHtmlCell"), 0);  \
            PyObject* o2  = wxPyConstructObject((void*)&e, wxT("wxMouseEvent"), 0);  \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OiiO)",obj,x,y,o2));  \
            Py_DECREF(obj);                                                     \
            Py_DECREF(o2);                                                      \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            PCLASS::CBNAME(cell, x, y, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e) {         \
        PCLASS::CBNAME(cell, x, y, e);                                          \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK___pure(CBNAME)                         \
    void CBNAME();                                            \


#define IMP_PYCALLBACK___pure(CLASS, PCLASS, CBNAME)                            \
    void CLASS::CBNAME() {                                                      \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                \
        wxPyEndBlockThreads();                                                  \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_wxSize__pure(CBNAME)                         \
    wxSize CBNAME();                                                \


#define IMP_PYCALLBACK_wxSize__pure(CLASS, PCLASS, CBNAME)                      \
    wxSize CLASS::CBNAME() {                                                    \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers or a wxSize object."; \
        wxSize rval(0,0);                                                       \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            wxSize*   ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxSize_p"))           \
                    rval = *ptr;                                                \
                else if (PySequence_Check(ro) && PyObject_Length(ro) == 2) {    \
                    PyObject* o1 = PySequence_GetItem(ro, 0);                   \
                    PyObject* o2 = PySequence_GetItem(ro, 1);                   \
                    if (PyNumber_Check(o1) && PyNumber_Check(o2))               \
                        rval = wxSize(PyInt_AsLong(o1), PyInt_AsLong(o2));      \
                    else                                                        \
                        PyErr_SetString(PyExc_TypeError, errmsg);               \
                    Py_DECREF(o1);                                              \
                    Py_DECREF(o2);                                              \
                }                                                               \
                else {                                                          \
                    PyErr_SetString(PyExc_TypeError, errmsg);                   \
                }                                                               \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a);                               \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxWindow* a) {                                    \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_WXWINDC(CBNAME)                             \
    bool CBNAME(wxWindow* a, wxDC& b);                                  \
    bool base_##CBNAME(wxWindow* a, wxDC& b);


#define IMP_PYCALLBACK_BOOL_WXWINDC(CLASS, PCLASS, CBNAME)                        \
    bool CLASS::CBNAME(wxWindow* a, wxDC& b) {                                  \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* win = wxPyMake_wxObject(a);                               \
            PyObject* dc  = wxPyMake_wxObject(&b);                              \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)", win, dc));\
            Py_DECREF(win);                                                     \
            Py_DECREF(dc);                                                      \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b);                                        \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxWindow* a, wxDC& b) {                           \
        return PCLASS::CBNAME(a, b);                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_WXWINBASE(CBNAME)                      \
    void CBNAME(wxWindowBase* a);                                  \
    void base_##CBNAME(wxWindowBase* a);


#define IMP_PYCALLBACK_VOID_WXWINBASE(CLASS, PCLASS, CBNAME)                    \
    void CLASS::CBNAME(wxWindowBase* a) {                                       \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a);                               \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(wxWindowBase* a) {                                \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_(CBNAME)                      \
    bool CBNAME();                                        \
    bool base_##CBNAME();


#define IMP_PYCALLBACK_BOOL_(CLASS, PCLASS, CBNAME)                             \
    bool CLASS::CBNAME() {                                                      \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME() {                                               \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_const(CBNAME)                       \
    bool CBNAME() const;                                        \
    bool base_##CBNAME() const;


#define IMP_PYCALLBACK_BOOL_const(CLASS, PCLASS, CBNAME)                        \
    bool CLASS::CBNAME() const {                                                \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME() const {                                         \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                        \
        wxFSFile* rval=0;                                               \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                  \
            PyObject* ro;                                               \
            PyObject* obj = wxPyMake_wxObject(&a);                      \
            PyObject* s = wx2PyString(b);                               \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OO)",\
                                         obj, s));                      \
            if (ro) {                                                   \
                SWIG_GetPtrObj(ro, (void **)&rval, "_wxFSFILE_p");      \
                Py_DECREF(ro);                                          \
            }                                                           \
            Py_DECREF(obj);                                             \
            Py_DECREF(s);                                               \
        }                                                               \
        wxPyEndBlockThreads();                                          \
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
        wxPyBeginBlockThreads();                                        \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));\
        wxPyEndBlockThreads();                                          \
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
        wxPyBeginBlockThreads();                                        \
        int rval=0;                                                     \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                    \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads();                                          \
        return (wxDragResult)rval;                                      \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINTSTR_pure(CBNAME)                      \
    bool CBNAME(int a, int b, const wxString& c);

#define IMP_PYCALLBACK_BOOL_INTINTSTR_pure(CLASS, PCLASS, CBNAME)       \
    bool CLASS::CBNAME(int a, int b, const wxString& c) {               \
        bool rval=FALSE;                                                \
        wxPyBeginBlockThreads();                                        \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                  \
            PyObject* s = wx2PyString(c);                               \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",a,b,s));\
            Py_DECREF(s);                                               \
        }                                                               \
        wxPyEndBlockThreads();                                          \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
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
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);           \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);           \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);           \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rv = PCLASS::CBNAME(a);                                             \
        return rv;                                                              \
    }                                                                           \
    bool CLASS::base_##CBNAME(Type& a) {                                        \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_bool_anypure(CBNAME, Type)                               \
    bool CBNAME(Type& a);


#define IMP_PYCALLBACK_bool_anypure(CLASS, PCLASS, CBNAME, Type)                \
    bool CLASS::CBNAME(Type& a) {                                               \
        bool rv=FALSE;                                                          \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);           \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_LONGLONG(CBNAME)                                  \
    wxString CBNAME(long a, long b) const;                                      \
    wxString base_##CBNAME(long a, long b)const ;

#define IMP_PYCALLBACK_STRING_LONGLONG(CLASS, PCLASS, CBNAME)                   \
    wxString CLASS::CBNAME(long a, long b) const {                              \
        wxString rval;                                                          \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ll)",a,b));  \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = PyInt_AsLong(ro);                                        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
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
        wxPyBeginBlockThreads();                                                \
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
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    wxListItemAttr *CLASS::base_##CBNAME(long a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_ME(CBNAME)                                          \
    bool CBNAME(wxMouseEvent& e);                                               \
    bool base_##CBNAME(wxMouseEvent& e);

#define IMP_PYCALLBACK_BOOL_ME(CLASS, PCLASS, CBNAME)                           \
    bool CLASS::CBNAME(wxMouseEvent& e) {                                       \
        bool rval=FALSE;                                                        \
        bool found;                                                             \
        wxPyBeginBlockThreads();                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            PyObject* obj  = wxPyConstructObject((void*)&e, wxT("wxMouseEvent"), 0); \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)",obj));   \
            if (ro) {                                                           \
                rval = PyInt_AsLong(ro);                                        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        if (! found)                                                            \
            return PCLASS::CBNAME(e);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxMouseEvent& e) {                                \
        return PCLASS::CBNAME(e);                                               \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_WIZPG__pure(CBNAME)                                      \
    wxWizardPage* CBNAME() const;

#define IMP_PYCALLBACK_WIZPG__pure(CLASS, PCLASS, CBNAME)                       \
    wxWizardPage* CLASS::CBNAME() const {                                       \
        wxWizardPage* rv = NULL;                                                \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                SWIG_GetPtrObj(ro, (void **)&rv, "_wxWizardPage_p");            \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BITMAP__pure(CBNAME)                                     \
    wxBitmap CBNAME() const;

#define IMP_PYCALLBACK_BITMAP__pure(CLASS, PCLASS, CBNAME)                      \
    wxBitmap CLASS::CBNAME() const {                                            \
        wxBitmap rv;                                                            \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            wxBitmap* ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (! SWIG_GetPtrObj(ro, (void **)&ptr, "_wxBitmap_p"))         \
                    rv = *ptr;                                                  \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_OBJECT__pure(CBNAME)                                     \
    wxObject* CBNAME();

#define IMP_PYCALLBACK_OBJECT__pure(CLASS, PCLASS, CBNAME)                      \
    wxObject* CLASS::CBNAME() {                                                 \
        wxObject* rv = NULL;                                                    \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                SWIG_GetPtrObj(ro, (void **)&rv, "_wxObject_p");                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_OBJECT_STRING_pure(CBNAME)                               \
    wxObject* CBNAME(const wxString& a);

#define IMP_PYCALLBACK_OBJECT_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxObject* CLASS::CBNAME(const wxString& a) {                                \
        wxObject* rv = NULL;                                                    \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* so = wx2PyString(a);                                      \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)", so));   \
            if (ro) {                                                           \
                SWIG_GetPtrObj(ro, (void **)&rv, "_wxObject_p");                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
            Py_DECREF(so);                                                      \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_NODE_pure(CBNAME)                                   \
    bool CBNAME(wxXmlNode* a);


#define IMP_PYCALLBACK_BOOL_NODE_pure(CLASS, PCLASS, CBNAME)                    \
    bool CLASS::CBNAME(wxXmlNode* a) {                                          \
        bool rv=FALSE;                                                          \
        wxPyBeginBlockThreads();                                                \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)a, wxT("wxXmlNode"), 0);      \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads();                                                  \
        return rv;                                                              \
    }                                                                           \

//---------------------------------------------------------------------------
#endif
