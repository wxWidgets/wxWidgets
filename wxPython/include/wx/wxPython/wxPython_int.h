////////////////////////////////////////////////////////////////////////////
// Name:        wxPython_int.h   (int == internal)
// Purpose:     Helper functions/classes for the wxPython extension module
//              This header should only be inclued directly by those source
//              modules included in the wx._core module.  All others should
//              include wx/wxPython/wxPython.h instead.
//
// Author:      Robin Dunn
//
// Created:     1-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxp_helpers__
#define __wxp_helpers__

#include <wx/wx.h>

#include <wx/busyinfo.h>
#include <wx/caret.h>
#include <wx/choicebk.h>
#include <wx/clipbrd.h>
#include <wx/colordlg.h>
#include <wx/config.h>
#include <wx/cshelp.h>
#include <wx/dcmirror.h>
#include <wx/dcps.h>
#include <wx/dirctrl.h>
#include <wx/dirdlg.h>
#include <wx/dnd.h>
#include <wx/docview.h>
#include <wx/encconv.h>
#include <wx/fdrepdlg.h>
#include <wx/fileconf.h>
#include <wx/filesys.h>
#include <wx/fontdlg.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>
#include <wx/fs_zip.h>
#include <wx/gbsizer.h>
#include <wx/geometry.h>
#include <wx/htmllbox.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/intl.h>
#include <wx/laywin.h>
#include <wx/listbook.h>
#include <wx/minifram.h>
#include <wx/notebook.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/process.h>
#include <wx/progdlg.h>
#include <wx/sashwin.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splash.h>
#include <wx/splitter.h>
#include <wx/statline.h>
#include <wx/stream.h>
#include <wx/sysopt.h>
#include <wx/taskbar.h>
#include <wx/tglbtn.h>
#include <wx/tipwin.h>
#include <wx/tooltip.h>
#include <wx/vlbox.h>
#include <wx/vscroll.h>


#ifdef _MSC_VER
# pragma warning(disable:4800)
# pragma warning(disable:4190)
#endif

#ifdef __WXMAC__  // avoid a bug in Carbon headers
#define scalb scalbn
#endif

//---------------------------------------------------------------------------

typedef unsigned char byte;
typedef wxPoint2DDouble wxPoint2D;    


#ifndef wxPyUSE_EXPORTED_API

void      __wxPyPreStart(PyObject*);
void      __wxPyCleanup();
PyObject* __wxPySetDictionary(PyObject*, PyObject* args);
PyObject* __wxPyFixStockObjects(PyObject*, PyObject* args);

void        wxSetDefaultPyEncoding(const char* encoding);
const char* wxGetDefaultPyEncoding();


void wxPyEventThunker(wxObject*, wxEvent& event);


bool wxPyCheckSwigType(const wxChar* className);
PyObject* wxPyConstructObject(void* ptr,
                              const wxChar* className,
                              int setThisOwn=0);
bool wxPyConvertSwigPtr(PyObject* obj, void **ptr,
                        const wxChar* className);
PyObject* wxPyMakeSwigPtr(void* ptr, const wxChar* classname);


PyObject* wx2PyString(const wxString& src);
wxString  Py2wxString(PyObject* source);

PyObject* wxPyMake_wxObject(wxObject* source, bool setThisOwn, bool checkEvtHandler=true);
PyObject* wxPyMake_wxSizer(wxSizer* source, bool setThisOwn);
void      wxPyPtrTypeMap_Add(const char* commonName, const char* ptrName);

PyObject* wxPy_ConvertList(wxListBase* list);
long      wxPyGetWinHandle(wxWindow* win);

void wxPy_ReinitStockObjects(int pass);

bool wxPyInstance_Check(PyObject* obj);
bool wxPySwigInstance_Check(PyObject* obj);


#endif // wxPyUSE_EXPORTED_API
//---------------------------------------------------------------------------

// if we want to handle threads and Python threads are available...
#if defined(WXP_USE_THREAD) && defined(WITH_THREAD)
#define WXP_WITH_THREAD
#else  // no Python threads...
#undef WXP_WITH_THREAD
#endif

#ifndef wxPyUSE_EXPORTED_API

// For Python --> C++
PyThreadState* wxPyBeginAllowThreads();
void           wxPyEndAllowThreads(PyThreadState* state);

// For C++ --> Python
bool wxPyBeginBlockThreads();
void wxPyEndBlockThreads(bool blocked);

#endif // wxPyUSE_EXPORTED_API


// A macro that will help to execute simple statments wrapped in
// StartBlock/EndBlockThreads calls
#define wxPyBLOCK_THREADS(stmt) \
    { bool blocked = wxPyBeginBlockThreads(); stmt; wxPyEndBlockThreads(blocked); }

// Raise the NotImplementedError exception  (blocking threads)
#define wxPyRaiseNotImplemented() \
    wxPyBLOCK_THREADS(PyErr_SetNone(PyExc_NotImplementedError))

// Raise any exception witha string value  (blocking threads)
#define wxPyErr_SetString(err, str) \
    wxPyBLOCK_THREADS(PyErr_SetString(err, str))


//---------------------------------------------------------------------------
// These are helpers used by the typemaps

#ifndef wxPyUSE_EXPORTED_API

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
bool wxPoint2D_helper(PyObject* source, wxPoint2D** obj);


bool wxPySimple_typecheck(PyObject* source, const wxChar* classname, int seqLen);
bool wxColour_typecheck(PyObject* source);

bool wxPyCheckForApp();

    
template<class T>
bool wxPyTwoIntItem_helper(PyObject* source, T** obj, const wxChar* name)
{
    // If source is an object instance then it may already be the right type
    if (wxPySwigInstance_Check(source)) {
        T* ptr;
        if (! wxPyConvertSwigPtr(source, (void **)&ptr, name))
            goto error;
        *obj = ptr;
        return true;
    }
    // otherwise a 2-tuple of integers is expected
    else if (PySequence_Check(source) && PyObject_Length(source) == 2) {
        PyObject* o1 = PySequence_GetItem(source, 0);
        PyObject* o2 = PySequence_GetItem(source, 1);
        if (!PyNumber_Check(o1) || !PyNumber_Check(o2)) {
            Py_DECREF(o1);
            Py_DECREF(o2);
            goto error;
        }
        **obj = T(PyInt_AsLong(o1), PyInt_AsLong(o2));
        Py_DECREF(o1);
        Py_DECREF(o2);
        return true;
    }

 error:
    wxString msg;
    msg.Printf(wxT("Expected a 2-tuple of integers or a %s object."), name);
    PyErr_SetString(PyExc_TypeError, msg.mb_str());
    return false;
}



// Other helpful stuff
bool wxPy2int_seq_helper(PyObject* source, int* i1, int* i2);
bool wxPy4int_seq_helper(PyObject* source, int* i1, int* i2, int* i3, int* i4);

PyObject* wxArrayString2PyList_helper(const wxArrayString& arr);
PyObject* wxArrayInt2PyList_helper(const wxArrayInt& arr);

#endif // wxPyUSE_EXPORTED_API

//---------------------------------------------------------------------------

#if PYTHON_API_VERSION < 1009
#define PySequence_Fast_GET_ITEM(o, i) \
     (PyList_Check(o) ? PyList_GET_ITEM(o, i) : PyTuple_GET_ITEM(o, i))
#endif

#define RETURN_NONE()                 { Py_INCREF(Py_None); return Py_None; }
#define DECLARE_DEF_STRING(name)      static const wxString wxPy##name(wx##name)
#define DECLARE_DEF_STRING2(name,val) static const wxString wxPy##name(val)

//---------------------------------------------------------------------------

#ifndef wxPyUSE_EXPORTED_API

class wxPyCallback : public wxObject {
    DECLARE_ABSTRACT_CLASS(wxPyCallback);
public:
    wxPyCallback(PyObject* func);
    wxPyCallback(const wxPyCallback& other);
    ~wxPyCallback();

    void EventThunker(wxEvent& event);

    PyObject*   m_func;
};

#endif // wxPyUSE_EXPORTED_API
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// These Event classes can be derived from in Python and passed through the
// event system without loosing anything.  They do this by keeping a reference
// to themselves and some special case handling in wxPyCallback::EventThunker.



class wxPyEvtSelfRef {
public:
    wxPyEvtSelfRef();
    ~wxPyEvtSelfRef();

    void SetSelf(PyObject* self, bool clone=false);
    PyObject* GetSelf() const;
    bool GetCloned() const { return m_cloned; }

protected:
    PyObject*   m_self;
    bool        m_cloned;
};


class wxPyEvent : public wxEvent, public wxPyEvtSelfRef {
    DECLARE_ABSTRACT_CLASS(wxPyEvent)
public:
    wxPyEvent(int winid=0, wxEventType commandType = wxEVT_NULL);
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
// the wx.core module and will then have safe access to these functions, even if
// in another shared library.

class wxPyCallbackHelper;
struct swig_type_info;
struct swig_const_info;

// Make SunCC happy and make typedef's for these that are extern "C"
typedef swig_type_info* (*p_SWIG_Python_TypeRegister_t)(swig_type_info *);
typedef swig_type_info* (*p_SWIG_Python_TypeCheck_t)(char *c, swig_type_info *);
typedef void*           (*p_SWIG_Python_TypeCast_t)(swig_type_info *, void *);
typedef swig_type_info* (*p_SWIG_Python_TypeDynamicCast_t)(swig_type_info *, void **);
typedef const char*     (*p_SWIG_Python_TypeName_t)(const swig_type_info *);
typedef const char *    (*p_SWIG_Python_TypePrettyName_t)(const swig_type_info *);
typedef swig_type_info* (*p_SWIG_Python_TypeQuery_t)(const char *);
typedef void            (*p_SWIG_Python_TypeClientData_t)(swig_type_info *, void *);
typedef PyObject*       (*p_SWIG_Python_newvarlink_t)(void);
typedef void            (*p_SWIG_Python_addvarlink_t)(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
typedef int             (*p_SWIG_Python_ConvertPtr_t)(PyObject *, void **, swig_type_info *, int);
typedef int             (*p_SWIG_Python_ConvertPacked_t)(PyObject *, void *, int sz, swig_type_info *, int);
typedef char*           (*p_SWIG_Python_PackData_t)(char *c, void *, int);
typedef char*           (*p_SWIG_Python_UnpackData_t)(char *c, void *, int);
typedef PyObject*       (*p_SWIG_Python_NewPointerObj_t)(void *, swig_type_info *,int own);
typedef PyObject*       (*p_SWIG_Python_NewPackedObj_t)(void *, int sz, swig_type_info *);
typedef void            (*p_SWIG_Python_InstallConstants_t)(PyObject *d, swig_const_info constants[]);
typedef void*           (*p_SWIG_Python_MustGetPtr_t)(PyObject *, swig_type_info *, int, int);


struct wxPyCoreAPI {

    p_SWIG_Python_TypeRegister_t       p_SWIG_Python_TypeRegister;
    p_SWIG_Python_TypeCheck_t          p_SWIG_Python_TypeCheck;
    p_SWIG_Python_TypeCast_t           p_SWIG_Python_TypeCast;
    p_SWIG_Python_TypeDynamicCast_t    p_SWIG_Python_TypeDynamicCast;
    p_SWIG_Python_TypeName_t           p_SWIG_Python_TypeName;
    p_SWIG_Python_TypePrettyName_t     p_SWIG_Python_TypePrettyName; 
    p_SWIG_Python_TypeQuery_t          p_SWIG_Python_TypeQuery;
    p_SWIG_Python_TypeClientData_t     p_SWIG_Python_TypeClientData;
    p_SWIG_Python_newvarlink_t         p_SWIG_Python_newvarlink;
    p_SWIG_Python_addvarlink_t         p_SWIG_Python_addvarlink;
    p_SWIG_Python_ConvertPtr_t         p_SWIG_Python_ConvertPtr;
    p_SWIG_Python_ConvertPacked_t      p_SWIG_Python_ConvertPacked;
    p_SWIG_Python_PackData_t           p_SWIG_Python_PackData;
    p_SWIG_Python_UnpackData_t         p_SWIG_Python_UnpackData;
    p_SWIG_Python_NewPointerObj_t      p_SWIG_Python_NewPointerObj;
    p_SWIG_Python_NewPackedObj_t       p_SWIG_Python_NewPackedObj;
    p_SWIG_Python_InstallConstants_t   p_SWIG_Python_InstallConstants;
    p_SWIG_Python_MustGetPtr_t         p_SWIG_Python_MustGetPtr;
    
    bool                (*p_wxPyCheckSwigType)(const wxChar* className);
    PyObject*           (*p_wxPyConstructObject)(void* ptr, const wxChar* className, int setThisOwn);
    bool                (*p_wxPyConvertSwigPtr)(PyObject* obj, void **ptr, const wxChar* className);
    PyObject*           (*p_wxPyMakeSwigPtr)(void* ptr, const wxChar* className);
        
    PyThreadState*      (*p_wxPyBeginAllowThreads)();
    void                (*p_wxPyEndAllowThreads)(PyThreadState* state);
    bool                (*p_wxPyBeginBlockThreads)();
    void                (*p_wxPyEndBlockThreads)(bool blocked);

    PyObject*           (*p_wxPy_ConvertList)(wxListBase* list);

    wxString*           (*p_wxString_in_helper)(PyObject* source);
    wxString            (*p_Py2wxString)(PyObject* source);
    PyObject*           (*p_wx2PyString)(const wxString& src);

    byte*               (*p_byte_LIST_helper)(PyObject* source);
    int*                (*p_int_LIST_helper)(PyObject* source);
    long*               (*p_long_LIST_helper)(PyObject* source);
    char**              (*p_string_LIST_helper)(PyObject* source);
    wxPoint*            (*p_wxPoint_LIST_helper)(PyObject* source, int* npoints);
    wxBitmap**          (*p_wxBitmap_LIST_helper)(PyObject* source);
    wxString*           (*p_wxString_LIST_helper)(PyObject* source);
    wxAcceleratorEntry* (*p_wxAcceleratorEntry_LIST_helper)(PyObject* source);

    bool                (*p_wxSize_helper)(PyObject* source, wxSize** obj);
    bool                (*p_wxPoint_helper)(PyObject* source, wxPoint** obj);
    bool                (*p_wxRealPoint_helper)(PyObject* source, wxRealPoint** obj);
    bool                (*p_wxRect_helper)(PyObject* source, wxRect** obj);
    bool                (*p_wxColour_helper)(PyObject* source, wxColour** obj);
    bool                (*p_wxPoint2D_helper)(PyObject* source, wxPoint2DDouble** obj);

    
    bool                (*p_wxPySimple_typecheck)(PyObject* source, const wxChar* classname, int seqLen);
    bool                (*p_wxColour_typecheck)(PyObject* source);

    void                (*p_wxPyCBH_setCallbackInfo)(wxPyCallbackHelper& cbh, PyObject* self, PyObject* klass, int incref);
    bool                (*p_wxPyCBH_findCallback)(const wxPyCallbackHelper& cbh, const char* name);
    int                 (*p_wxPyCBH_callCallback)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    PyObject*           (*p_wxPyCBH_callCallbackObj)(const wxPyCallbackHelper& cbh, PyObject* argTuple);
    void                (*p_wxPyCBH_delete)(wxPyCallbackHelper* cbh);

    PyObject*           (*p_wxPyMake_wxObject)(wxObject* source, bool setThisOwn, bool checkEvtHandler);
    PyObject*           (*p_wxPyMake_wxSizer)(wxSizer* source, bool setThisOwn);
    void                (*p_wxPyPtrTypeMap_Add)(const char* commonName, const char* ptrName);
    bool                (*p_wxPy2int_seq_helper)(PyObject* source, int* i1, int* i2);
    bool                (*p_wxPy4int_seq_helper)(PyObject* source, int* i1, int* i2, int* i3, int* i4);
    PyObject*           (*p_wxArrayString2PyList_helper)(const wxArrayString& arr);
    PyObject*           (*p_wxArrayInt2PyList_helper)(const wxArrayInt& arr);

    void                (*p_wxPyClientData_dtor)(wxPyClientData*);
    void                (*p_wxPyUserData_dtor)(wxPyUserData*);
    void                (*p_wxPyOORClientData_dtor)(wxPyOORClientData*);

    wxPyCBInputStream*  (*p_wxPyCBInputStream_create)(PyObject *py, bool block);

    bool                (*p_wxPyInstance_Check)(PyObject* obj);
    bool                (*p_wxPySwigInstance_Check)(PyObject* obj);

    bool                (*p_wxPyCheckForApp)();

};

#ifdef wxPyUSE_EXPORTED_API
// Notice that this is static, not extern.  This is by design, each module
// needs one, but doesn't have to use it.
static wxPyCoreAPI* wxPyCoreAPIPtr = NULL;
inline wxPyCoreAPI* wxPyGetCoreAPIPtr();
#endif // wxPyUSE_EXPORTED_API

//---------------------------------------------------------------------------

// A wxObject that holds a reference to a Python object
class wxPyUserData : public wxObject {
public:
    wxPyUserData(PyObject* obj) {
        m_obj = obj;
        Py_INCREF(m_obj);
    }

    ~wxPyUserData() {
#ifdef wxPyUSE_EXPORTED_API
        wxPyGetCoreAPIPtr()->p_wxPyUserData_dtor(this);
#else
        wxPyUserData_dtor(this);
#endif
    }
    PyObject* m_obj;
};


// A wxClientData that holds a refernece to a Python object
class wxPyClientData : public wxClientData {
public:
    wxPyClientData(PyObject* obj, bool incref=true) {
        m_obj = obj;
        m_incRef = incref;
        if (incref)
            Py_INCREF(m_obj);
    }
    ~wxPyClientData() {

#ifdef wxPyUSE_EXPORTED_API
        wxPyGetCoreAPIPtr()->p_wxPyClientData_dtor(this);
#else
        wxPyClientData_dtor(this);
#endif
    }
    PyObject* m_obj;
    bool      m_incRef;
};


// Just like wxPyClientData, except when this object is destroyed it does some
// OOR magic on the Python Object.
class wxPyOORClientData : public wxPyClientData {
public:
    wxPyOORClientData(PyObject* obj, bool incref=true)
        : wxPyClientData(obj, incref) {}
    ~wxPyOORClientData() {

#ifdef wxPyUSE_EXPORTED_API
        wxPyGetCoreAPIPtr()->p_wxPyOORClientData_dtor(this);
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
        m_incRef = false;
    }

    ~wxPyCallbackHelper() {
#ifdef wxPyUSE_EXPORTED_API
        wxPyGetCoreAPIPtr()->p_wxPyCBH_delete(this);
#else
        wxPyCBH_delete(this);
#endif
    }

    void        setSelf(PyObject* self, PyObject* klass, int incref=true);
    bool        findCallback(const char* name) const;
    int         callCallback(PyObject* argTuple) const;
    PyObject*   callCallbackObj(PyObject* argTuple) const;
    PyObject*   GetLastFound() const { return m_lastFound; }
    
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
// The wxPythonApp class

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

    // For catching Apple Events
    virtual void MacOpenFile(const wxString &fileName);
    virtual void MacPrintFile(const wxString &fileName);
    virtual void MacNewFile();
    virtual void MacReopenApp();

    static bool GetMacSupportPCMenuShortcuts();
    static long GetMacAboutMenuItemId();
    static long GetMacPreferencesMenuItemId();
    static long GetMacExitMenuItemId();
    static wxString GetMacHelpMenuTitleName();

    static void SetMacSupportPCMenuShortcuts(bool val);
    static void SetMacAboutMenuItemId(long val);
    static void SetMacPreferencesMenuItemId(long val);
    static void SetMacExitMenuItemId(long val);
    static void SetMacHelpMenuTitleName(const wxString& val);


    void _BootstrapApp();

    // implementation only
    void SetStartupComplete(bool val) { m_startupComplete = val; };

    PYPRIVATE;
    int m_assertMode;
    bool m_startupComplete;
};

extern wxPyApp *wxPythonApp;


//----------------------------------------------------------------------
// These macros are used to implement the virtual methods that should
// redirect to a Python method if one exists.  The names designate the
// return type, if any, as well as any parameter types.
//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__(CBNAME)                        \
    void CBNAME();                                      \
    void base_##CBNAME()


#define IMP_PYCALLBACK__(CLASS, PCLASS, CBNAME)                         \
    void CLASS::CBNAME() {                                              \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));        \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME();                                           \
    }                                                                   \
    void CLASS::base_##CBNAME() {                                       \
        PCLASS::CBNAME();                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINT(CBNAME)                      \
    bool CBNAME(int a, int b);                                  \
    bool base_##CBNAME(int a, int b)


#define IMP_PYCALLBACK_BOOL_INTINT(CLASS, PCLASS, CBNAME)               \
    bool CLASS::CBNAME(int a, int b) {                                  \
        bool rval=false, found;                                         \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));    \
        wxPyEndBlockThreads(blocked);                                   \
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
    void base_##CBNAME()


#define IMP_PYCALLBACK_VOID_(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME() {                                              \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));        \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME();                                           \
    }                                                                   \
    void CLASS::base_##CBNAME() {                                       \
        PCLASS::CBNAME();                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INTINT(CBNAME)                      \
    void CBNAME(int a, int b);                                  \
    void base_##CBNAME(int a, int b)


#define IMP_PYCALLBACK_VOID_INTINT(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(int a, int b) {                                  \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));  \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b);                                        \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b) {                           \
        PCLASS::CBNAME(a,b);                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INT(CBNAME)                         \
    void CBNAME(int a);                                         \
    void base_##CBNAME(int a)


#define IMP_PYCALLBACK_VOID_INT(CLASS, PCLASS, CBNAME)                  \
    void CLASS::CBNAME(int a) {                                         \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));     \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a);                                          \
    }                                                                   \
    void CLASS::base_##CBNAME(int a) {                                  \
        PCLASS::CBNAME(a);                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INT4(CBNAME)                                \
    void CBNAME(int a, int b, int c, int d);                            \
    void base_##CBNAME(int a, int b, int c, int d)


#define IMP_PYCALLBACK_VOID_INT4(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(int a, int b, int c, int d) {                    \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiii)",a,b,c,d));  \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b,c,d);                                    \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b, int c, int d) {             \
        PCLASS::CBNAME(a,b,c,d);                                        \
    }

//---------------------------------------------------------------------------
#define DEC_PYCALLBACK_VOID_INT5(CBNAME)                                \
    void CBNAME(int a, int b, int c, int d, int e);                     \
    void base_##CBNAME(int a, int b, int c, int d, int e)


#define IMP_PYCALLBACK_VOID_INT5(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(int a, int b, int c, int d, int e) {             \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiiii)",a,b,c,d,e));  \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a,b,c,d,e);                                  \
    }                                                                   \
    void CLASS::base_##CBNAME(int a, int b, int c, int d, int e) {      \
        PCLASS::CBNAME(a,b,c,d,e);                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_INTPINTP_const(CBNAME)                              \
    void CBNAME(int* a, int* b) const;                                          \
    void base_##CBNAME(int* a, int* b) const


#define IMP_PYCALLBACK_VOID_INTPINTP_const(CLASS, PCLASS, CBNAME)               \
    void CLASS::CBNAME(int* a, int* b) const {                                  \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers.";   \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                                  \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b);                                                \
    }                                                                           \
    void CLASS::base_##CBNAME(int* a, int* b) const {                           \
        PCLASS::CBNAME(a,b);                                                    \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZE_const(CBNAME)                                       \
    wxSize CBNAME() const;                                                      \
    wxSize base_##CBNAME() const


#define IMP_PYCALLBACK_SIZE_const(CLASS, PCLASS, CBNAME)                        \
    wxSize CLASS::CBNAME() const {                                              \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers.";   \
        bool found; wxSize rval(0,0);                                           \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                           \
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
    bool base_##CBNAME(bool a)


#define IMP_PYCALLBACK_BOOL_BOOL(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(bool a) {                                        \
        bool rval=false, found;                                         \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));\
        wxPyEndBlockThreads(blocked);                                   \
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
    bool base_##CBNAME(int a)


#define IMP_PYCALLBACK_BOOL_INT(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(int a) {                                         \
        bool rval=false, found;                                         \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));\
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            rval = PCLASS::CBNAME(a);                                   \
        return rval;                                                    \
    }                                                                   \
    bool CLASS::base_##CBNAME(int a) {                                  \
        return PCLASS::CBNAME(a);                                       \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INT_pure(CBNAME)            \
    bool CBNAME(int a)


#define IMP_PYCALLBACK_BOOL_INT_pure(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME(int a) {                                                 \
        bool rval=false;                                                        \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)",a));      \
        else rval = false;                                                      \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC(CBNAME)                      \
    void CBNAME(wxDC& a);                               \
    void base_##CBNAME(wxDC& a)


#define IMP_PYCALLBACK__DC(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxDC& a) {                                       \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a,false);                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a);                                          \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a) {                                \
        PCLASS::CBNAME(a);                                              \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b)


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a,false);                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                          \
    void CBNAME(wxDC& a, bool b);                               \
    void base_##CBNAME(wxDC& a, bool b)


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                   \
    void CLASS::CBNAME(wxDC& a, bool b) {                               \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {        \
            PyObject* obj = wxPyMake_wxObject(&a,false);                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b));  \
            Py_DECREF(obj);                                             \
        }                                                               \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
    }                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                        \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL(CBNAME)                    \
    void CBNAME(double a, double b);                    \
    void base_##CBNAME(double a, double b)


#define IMP_PYCALLBACK__2DBL(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(double a, double b) {                            \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(dd)",a,b));  \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b);                                       \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b) {                     \
        PCLASS::CBNAME(a, b);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__2DBL2INT(CBNAME)                        \
    void CBNAME(double a, double b, int c, int d);              \
    void base_##CBNAME(double a, double b, int c, int d)


#define IMP_PYCALLBACK__2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(double a, double b, int c, int d) {              \
        bool found;                                                     \
        bool blocked = wxPyBeginBlockThreads();                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))          \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddii)",      \
                                                       a,b,c,d));       \
        wxPyEndBlockThreads(blocked);                                   \
        if (! found)                                                    \
            PCLASS::CBNAME(a, b, c, d);                                 \
    }                                                                   \
    void CLASS::base_##CBNAME(double a, double b, int c, int d) {       \
        PCLASS::CBNAME(a, b, c, d);                                     \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBLBOOL(CBNAME)                                      \
    void CBNAME(wxDC& a, double b, double c, double d, double e, bool f);       \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f)


#define IMP_PYCALLBACK__DC4DBLBOOL(CLASS, PCLASS, CBNAME)                               \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {       \
        bool found;                                                                     \
        bool blocked = wxPyBeginBlockThreads();                                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* obj = wxPyMake_wxObject(&a,false);                                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));  \
            Py_DECREF(obj);                                                             \
        }                                                                               \
        wxPyEndBlockThreads(blocked);                                                   \
        if (! found)                                                                    \
            PCLASS::CBNAME(a, b, c, d, e, f);                                           \
    }                                                                                   \
    void CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {\
        PCLASS::CBNAME(a, b, c, d, e, f);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DC4DBLBOOL(CBNAME)                                  \
    bool CBNAME(wxDC& a, double b, double c, double d, double e, bool f);       \
    bool base_##CBNAME(wxDC& a, double b, double c, double d, double e, bool f)


#define IMP_PYCALLBACK_BOOL_DC4DBLBOOL(CLASS, PCLASS, CBNAME)                           \
    bool CLASS::CBNAME(wxDC& a, double b, double c, double d, double e, bool f) {       \
        bool found;                                                                     \
        bool blocked = wxPyBeginBlockThreads();                                         \
        bool rval=false;                                                                \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* obj = wxPyMake_wxObject(&a,false);                                \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddddi)", obj, b, c, d, e, (int)f));\
            Py_DECREF(obj);                                                             \
        }                                                                               \
        wxPyEndBlockThreads(blocked);                                                   \
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
    void base_##CBNAME(bool a, double b, double c, int d, int e)


#define IMP_PYCALLBACK__BOOL2DBL2INT(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(bool a, double b, double c, int d, int e) {              \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddii)",             \
                                                (int)a,b,c,d,e));               \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(bool a, double b, double c, int d, int e) {       \
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DC4DBL(CBNAME)                                          \
    void CBNAME(wxDC& a, double b, double c, double d, double e);               \
    void base_##CBNAME(wxDC& a, double b, double c, double d, double e)


#define IMP_PYCALLBACK__DC4DBL(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(wxDC& a, double b, double c, double d, double e) {       \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a,false);                        \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Odddd)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c, d, e);                                      \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, double b, double c, double d, double e) {\
        PCLASS::CBNAME(a, b, c, d, e);                                          \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCBOOL(CBNAME)                  \
    void CBNAME(wxDC& a, bool b);                       \
    void base_##CBNAME(wxDC& a, bool b)


#define IMP_PYCALLBACK__DCBOOL(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(wxDC& a, bool b) {                                       \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a,false);                        \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, (int)b)); \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b);                                               \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, bool b) {                                \
        PCLASS::CBNAME(a, b);                                                   \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__WXCPBOOL2DBL2INT(CBNAME)                                \
    void CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f);   \
    void base_##CBNAME(wxControlPoint* a, bool b, double c, double d, int e, int f)


#define IMP_PYCALLBACK__WXCPBOOL2DBL2INT(CLASS, PCLASS, CBNAME)                 \
    void CLASS::CBNAME(wxControlPoint* a, bool b, double c, double d,           \
                int e, int f) {                                                 \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a,false);                         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oiddii)", obj, (int)b, c, d, e, f));\
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    void base_##CBNAME(wxControlPoint* a, double b, double c, int d, int e)


#define IMP_PYCALLBACK__WXCP2DBL2INT(CLASS, PCLASS, CBNAME)                     \
    void CLASS::CBNAME(wxControlPoint* a, double b, double c, int d, int e) {   \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a,false);                         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oddii)", obj, b, c, d, e));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    void base_##CBNAME(double a, double b, int c)


#define IMP_PYCALLBACK__2DBLINT(CLASS, PCLASS, CBNAME)                          \
    void CLASS::CBNAME(double a, double b, int c) {                             \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ddi)", a,b,c));      \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a, b, c);                                            \
    }                                                                           \
    void CLASS::base_##CBNAME(double a, double b, int c) {                      \
        PCLASS::CBNAME(a, b, c);                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__BOOL2DBLINT(CBNAME)                     \
    void CBNAME(bool a, double b, double c, int d);             \
    void base_##CBNAME(bool a, double b, double c, int d)


#define IMP_PYCALLBACK__BOOL2DBLINT(CLASS, PCLASS, CBNAME)                      \
    void CLASS::CBNAME(bool a, double b, double c, int d) {                     \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iddi)", (int)a,b,c,d));\
        wxPyEndBlockThreads(blocked);                                           \
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
    void base_##CBNAME(const wxString& a)

#define IMP_PYCALLBACK__STRING(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(const wxString& a)  {                                    \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(a);                                       \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));            \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(const wxString& a) {                              \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING(CBNAME)              \
    bool CBNAME(const wxString& a);                     \
    bool base_##CBNAME(const wxString& a)

#define IMP_PYCALLBACK_BOOL_STRING(CLASS, PCLASS, CBNAME)                       \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(a);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));     \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(const wxString& a) {                              \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRING_pure(CBNAME)         \
    bool CBNAME(const wxString& a)

#define IMP_PYCALLBACK_BOOL_STRING_pure(CLASS, PCLASS, CBNAME)                  \
    bool CLASS::CBNAME(const wxString& a)  {                                    \
        bool rval=false;                                                        \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* s = wx2PyString(a);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", s));     \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING_pure(CBNAME)                               \
    wxString CBNAME(const wxString& a)

#define IMP_PYCALLBACK_STRING_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRING(CBNAME)                                    \
    wxString CBNAME(const wxString& a);                                         \
    wxString base_##CBNAME(const wxString& a)

#define IMP_PYCALLBACK_STRING_STRING(CLASS, PCLASS, CBNAME)                     \
    wxString CLASS::CBNAME(const wxString& a)  {                                \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_STRINGINT_pure(CBNAME)                            \
    wxString CBNAME(const wxString& a,int b)

#define IMP_PYCALLBACK_STRING_STRINGINT_pure(CLASS, PCLASS, CBNAME)             \
    wxString CLASS::CBNAME(const wxString& a,int b)  {                          \
        wxString rval;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_STRINGSTRING(CBNAME)              \
    bool CBNAME(const wxString& a, const wxString& b);        \
    bool base_##CBNAME(const wxString& a, const wxString& b)

#define IMP_PYCALLBACK_BOOL_STRINGSTRING(CLASS, PCLASS, CBNAME)                 \
    bool CLASS::CBNAME(const wxString& a, const wxString& b) {                  \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s1 = wx2PyString(a);                                      \
            PyObject* s2 = wx2PyString(b);                                      \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)",s1,s2)); \
            Py_DECREF(s1);                                                      \
            Py_DECREF(s2);                                                      \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    wxString base_##CBNAME()

#define IMP_PYCALLBACK_STRING_(CLASS, PCLASS, CBNAME)                           \
    wxString CLASS::CBNAME() {                                                  \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    wxString CLASS::base_##CBNAME() {                                           \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING__const(CBNAME)                  \
    wxString CBNAME() const;                                  \
    wxString base_##CBNAME() const;

#define IMP_PYCALLBACK_STRING__const(CLASS, PCLASS, CBNAME)                     \
    wxString CLASS::CBNAME() const {                                            \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    wxString CLASS::base_##CBNAME() const {                                     \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING__pure(CBNAME)             \
    wxString CBNAME()

#define IMP_PYCALLBACK_STRING__pure(CLASS, PCLASS, CBNAME)                      \
    wxString CLASS::CBNAME() {                                                  \
        wxString rval;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING__constpure(CBNAME)             \
    wxString CBNAME() const;

#define IMP_PYCALLBACK_STRING__constpure(CLASS, PCLASS, CBNAME)                 \
    wxString CLASS::CBNAME() const {                                            \
        wxString rval;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_TAG_pure(CBNAME)                  \
    bool CBNAME(const wxHtmlTag& a)


#define IMP_PYCALLBACK_BOOL_TAG_pure(CLASS, PCLASS, CBNAME)                     \
    bool CLASS::CBNAME(const wxHtmlTag& a)  {                                   \
        bool rval=false;                                                        \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT("wxHtmlTag"), 0);\
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__CELLINTINT(CBNAME)                                      \
    void CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y);                        \
    void base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y)

#define IMP_PYCALLBACK__CELLINTINT(CLASS, PCLASS, CBNAME)                       \
    void CLASS::CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y) {                \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)cell, wxT("wxHtmlCell"), 0);  \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Oii)",obj,x,y));  \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(cell, x, y);                                         \
    }                                                                           \
    void CLASS::base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y) {         \
        PCLASS::CBNAME(cell, x, y);                                             \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__COLOUR(CBNAME)                                      \
    void CBNAME(const wxColour& c);                                         \
    void base_##CBNAME(const wxColour& c)

#define IMP_PYCALLBACK__COLOUR(CLASS, PCLASS, CBNAME)                           \
    void CLASS::CBNAME(const wxColour& c) {                                     \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&c, wxT("wxColour"), 0); \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)",obj));        \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(c);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(const wxColour& c) {                              \
        PCLASS::CBNAME(c);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__CELLINTINTME(CBNAME)                                    \
    void CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e); \
    void base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e)

#define IMP_PYCALLBACK__CELLINTINTME(CLASS, PCLASS, CBNAME)                             \
    void CLASS::CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e) { \
        bool found;                                                                     \
        bool blocked = wxPyBeginBlockThreads();                                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* obj = wxPyConstructObject((void*)cell, wxT("wxHtmlCell"), 0);     \
            PyObject* o2  = wxPyConstructObject((void*)&e, wxT("wxMouseEvent"), 0);     \
            wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OiiO)",obj,x,y,o2));      \
            Py_DECREF(obj);                                                             \
            Py_DECREF(o2);                                                              \
        }                                                                               \
        wxPyEndBlockThreads(blocked);                                                   \
        if (! found)                                                                    \
            PCLASS::CBNAME(cell, x, y, e);                                              \
    }                                                                                   \
    void CLASS::base_##CBNAME(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& e) {\
        PCLASS::CBNAME(cell, x, y, e);                                                  \
    }



//---------------------------------------------------------------------------

#define DEC_PYCALLBACK___pure(CBNAME)                         \
    void CBNAME()


#define IMP_PYCALLBACK___pure(CLASS, PCLASS, CBNAME)                            \
    void CLASS::CBNAME() {                                                      \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                \
        wxPyEndBlockThreads(blocked);                                           \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_wxSize__pure(CBNAME)                         \
    wxSize CBNAME()


#define IMP_PYCALLBACK_wxSize__pure(CLASS, PCLASS, CBNAME)                      \
    wxSize CLASS::CBNAME() {                                                    \
        const char* errmsg = #CBNAME " should return a 2-tuple of integers or a wxSize object."; \
        wxSize rval(0,0);                                                       \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            wxSize*   ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxSize")))       \
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
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_WXWIN(CBNAME)                      \
    bool CBNAME(wxWindow* a);                                  \
    bool base_##CBNAME(wxWindow* a)


#define IMP_PYCALLBACK_BOOL_WXWIN(CLASS, PCLASS, CBNAME)                        \
    bool CLASS::CBNAME(wxWindow* a) {                                           \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a,false);                         \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));   \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    bool base_##CBNAME(wxWindow* a, wxDC& b)


#define IMP_PYCALLBACK_BOOL_WXWINDC(CLASS, PCLASS, CBNAME)                      \
    bool CLASS::CBNAME(wxWindow* a, wxDC& b) {                                  \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* win = wxPyMake_wxObject(a,false);                         \
            PyObject* dc  = wxPyMake_wxObject(&b,false);                        \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)", win, dc));\
            Py_DECREF(win);                                                     \
            Py_DECREF(dc);                                                      \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    void base_##CBNAME(wxWindowBase* a)


#define IMP_PYCALLBACK_VOID_WXWINBASE(CLASS, PCLASS, CBNAME)                    \
    void CLASS::CBNAME(wxWindowBase* a) {                                       \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(a,false);                         \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(wxWindowBase* a) {                                \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_(CBNAME)                      \
    bool CBNAME();                                        \
    bool base_##CBNAME()


#define IMP_PYCALLBACK_BOOL_(CLASS, PCLASS, CBNAME)                             \
    bool CLASS::CBNAME() {                                                      \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
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
    bool base_##CBNAME() const


#define IMP_PYCALLBACK_BOOL_const(CLASS, PCLASS, CBNAME)                        \
    bool CLASS::CBNAME() const {                                                \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
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
    wxDragResult base_##CBNAME(wxCoord x, wxCoord y, wxDragResult def)


#define IMP_PYCALLBACK_DR_2WXCDR(CLASS, PCLASS, CBNAME)                         \
    wxDragResult CLASS::CBNAME(wxCoord a, wxCoord b, wxDragResult c) {          \
        int rval=0;                                                             \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b, c);                                     \
        return (wxDragResult)rval;                                              \
    }                                                                           \
    wxDragResult CLASS::base_##CBNAME(wxCoord a, wxCoord b, wxDragResult c) {   \
        return PCLASS::CBNAME(a, b, c);                                         \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_FSF_FSSTRING_pure(CBNAME)                        \
    wxFSFile* CBNAME(wxFileSystem& fs, const wxString& location)

#define IMP_PYCALLBACK_FSF_FSSTRING_pure(CLASS, PCLASS, CBNAME)                 \
    wxFSFile* CLASS::CBNAME(wxFileSystem& a,const wxString& b) {                \
        bool blocked = wxPyBeginBlockThreads();                                 \
        wxFSFile* rval=0;                                                       \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            PyObject* obj = wxPyMake_wxObject(&a,false);                        \
            PyObject* s = wx2PyString(b);                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OO)",        \
                                         obj, s));                              \
            if (ro) {                                                           \
                wxPyConvertSwigPtr(ro, (void **)&rval, wxT("wxFSFile"));        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
            Py_DECREF(obj);                                                     \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    };

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_DR(CBNAME)                  \
    bool CBNAME(wxDragResult a);                        \
    bool base_##CBNAME(wxDragResult a)


#define IMP_PYCALLBACK_BOOL_DR(CLASS, PCLASS, CBNAME)                           \
    bool CLASS::CBNAME(wxDragResult a) {                                        \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));     \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxDragResult a) {                                 \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DR_2WXCDR_pure(CBNAME)   \
    wxDragResult CBNAME(wxCoord x, wxCoord y, wxDragResult def)


#define IMP_PYCALLBACK_DR_2WXCDR_pure(CLASS, PCLASS, CBNAME)                    \
    wxDragResult CLASS::CBNAME(wxCoord a, wxCoord b, wxDragResult c) {          \
        bool blocked = wxPyBeginBlockThreads();                                 \
        int rval=0;                                                             \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));\
        wxPyEndBlockThreads(blocked);                                           \
        return (wxDragResult)rval;                                              \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_INTINTSTR_pure(CBNAME)                      \
    bool CBNAME(int a, int b, const wxString& c)

#define IMP_PYCALLBACK_BOOL_INTINTSTR_pure(CLASS, PCLASS, CBNAME)       \
    bool CLASS::CBNAME(int a, int b, const wxString& c) {               \
        bool rval=false;                                                \
        bool blocked = wxPyBeginBlockThreads();                         \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                  \
            PyObject* s = wx2PyString(c);                               \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",a,b,s));\
            Py_DECREF(s);                                               \
        }                                                               \
        wxPyEndBlockThreads(blocked);                                   \
        return rval;                                                    \
    }                                                                   \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZET_(CBNAME)                  \
    size_t CBNAME();                                   \
    size_t base_##CBNAME()


#define IMP_PYCALLBACK_SIZET_(CLASS, PCLASS, CBNAME)                            \
    size_t CLASS::CBNAME() {                                                    \
        size_t rval=0;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    size_t CLASS::base_##CBNAME() {                                             \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_SIZET__const(CBNAME)                  \
    size_t CBNAME() const;                                   \
    size_t base_##CBNAME() const


#define IMP_PYCALLBACK_SIZET__const(CLASS, PCLASS, CBNAME)                      \
    size_t CLASS::CBNAME() const {                                              \
        size_t rval=0;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    size_t CLASS::base_##CBNAME() const {                                       \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_DATAFMT_SIZET(CBNAME)                                    \
    wxDataFormat  CBNAME(size_t a);                                             \
    wxDataFormat  base_##CBNAME(size_t a)


#define IMP_PYCALLBACK_DATAFMT_SIZET(CLASS, PCLASS, CBNAME)                     \
    wxDataFormat CLASS::CBNAME(size_t a) {                                      \
        wxDataFormat rval=0;                                                    \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxDataFormat* ptr;                                                  \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)", a));    \
            if (ro) {                                                           \
                if (wxPyCOnvertSwigPtr(ro, (void **)&ptr, wxT("wxDataFormat"))) \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    void base_##CBNAME(const Type& a)


#define IMP_PYCALLBACK__constany(CLASS, PCLASS, CBNAME, Type)                   \
    void CLASS::CBNAME(const Type& a) {                                         \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(const Type& a) {                                  \
        PCLASS::CBNAME(a);                                                      \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__any(CBNAME, Type)          \
    void CBNAME(Type& a);                          \
    void base_##CBNAME(Type& a)


#define IMP_PYCALLBACK__any(CLASS, PCLASS, CBNAME, Type)                        \
    void CLASS::CBNAME(Type& a) {                                               \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);      \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));          \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a);                                                  \
    }                                                                           \
    void CLASS::base_##CBNAME(Type& a) {                                        \
        PCLASS::CBNAME(a);                                                      \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_bool_any(CBNAME, Type)           \
    bool CBNAME(Type& a);                               \
    bool base_##CBNAME(Type& a)


#define IMP_PYCALLBACK_bool_any(CLASS, PCLASS, CBNAME, Type)                    \
    bool CLASS::CBNAME(Type& a) {                                               \
        bool rv=false;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);      \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rv = PCLASS::CBNAME(a);                                             \
        return rv;                                                              \
    }                                                                           \
    bool CLASS::base_##CBNAME(Type& a) {                                        \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_bool_anypure(CBNAME, Type)                               \
    bool CBNAME(Type& a)


#define IMP_PYCALLBACK_bool_anypure(CLASS, PCLASS, CBNAME, Type)                \
    bool CLASS::CBNAME(Type& a) {                                               \
        bool rv=false;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)&a, wxT(#Type), 0);      \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_LONGLONG(CBNAME)                                  \
    wxString CBNAME(long a, long b) const;                                      \
    wxString base_##CBNAME(long a, long b) const

#define IMP_PYCALLBACK_STRING_LONGLONG(CLASS, PCLASS, CBNAME)                   \
    wxString CLASS::CBNAME(long a, long b) const {                              \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ll)",a,b));  \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
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
    int base_##CBNAME(long a) const


#define IMP_PYCALLBACK_INT_LONG(CLASS, PCLASS, CBNAME)                          \
    int CLASS::CBNAME(long a) const {                                           \
        int rval=-1;                                                            \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = PyInt_AsLong(ro);                                        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    int CLASS::base_##CBNAME(long a) const {                                    \
        return PCLASS::CBNAME(a);                                               \
    }




#define DEC_PYCALLBACK_INT_LONG_virtual(CBNAME)                                 \
    int CBNAME(long a) const;


#define IMP_PYCALLBACK_INT_LONG_virtual(CLASS, PCLASS, CBNAME)                  \
    int CLASS::CBNAME(long a) const {                                           \
        int rval=-1;    /* this rval is important for OnGetItemImage */         \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = PyInt_AsLong(ro);                                        \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }  


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_LISTATTR_LONG(CBNAME)                                    \
    wxListItemAttr*  CBNAME(long a) const;                                      \
    wxListItemAttr*  base_##CBNAME(long a) const


#define IMP_PYCALLBACK_LISTATTR_LONG(CLASS, PCLASS, CBNAME)                     \
    wxListItemAttr *CLASS::CBNAME(long a) const {                               \
        wxListItemAttr *rval = NULL;                                            \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxListItemAttr* ptr;                                                \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)", a));    \
            if (ro) {                                                           \
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxListItemAttr")))\
                    rval = ptr;                                                 \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    wxListItemAttr *CLASS::base_##CBNAME(long a) const {                        \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_ME(CBNAME)                                          \
    bool CBNAME(wxMouseEvent& e);                                               \
    bool base_##CBNAME(wxMouseEvent& e)

#define IMP_PYCALLBACK_BOOL_ME(CLASS, PCLASS, CBNAME)                           \
    bool CLASS::CBNAME(wxMouseEvent& e) {                                       \
        bool rval=false;                                                        \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
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
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            return PCLASS::CBNAME(e);                                           \
        return rval;                                                            \
    }                                                                           \
    bool CLASS::base_##CBNAME(wxMouseEvent& e) {                                \
        return PCLASS::CBNAME(e);                                               \
    }


//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_WIZPG__pure(CBNAME)                                      \
    wxWizardPage* CBNAME() const

#define IMP_PYCALLBACK_WIZPG__pure(CLASS, PCLASS, CBNAME)                       \
    wxWizardPage* CLASS::CBNAME() const {                                       \
        wxWizardPage* rv = NULL;                                                \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                wxPyConvertSwigPtr(ro, (void **)&rv, wxT("wxWizardPage"));      \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BITMAP__pure(CBNAME)                                     \
    wxBitmap CBNAME() const

#define IMP_PYCALLBACK_BITMAP__pure(CLASS, PCLASS, CBNAME)                      \
    wxBitmap CLASS::CBNAME() const {                                            \
        wxBitmap rv;                                                            \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            wxBitmap* ptr;                                                      \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxBitmap")))     \
                    rv = *ptr;                                                  \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_OBJECT__pure(CBNAME)                                     \
    wxObject* CBNAME()

#define IMP_PYCALLBACK_OBJECT__pure(CLASS, PCLASS, CBNAME)                      \
    wxObject* CLASS::CBNAME() {                                                 \
        wxObject* rv = NULL;                                                    \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                wxPyConvertSwigPtr(ro, (void **)&rv, wxT("wxObject"));          \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_OBJECT_STRING_pure(CBNAME)                               \
    wxObject* CBNAME(const wxString& a)

#define IMP_PYCALLBACK_OBJECT_STRING_pure(CLASS, PCLASS, CBNAME)                \
    wxObject* CLASS::CBNAME(const wxString& a) {                                \
        wxObject* rv = NULL;                                                    \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* so = wx2PyString(a);                                      \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)", so));   \
            if (ro) {                                                           \
                wxPyConvertSwigPtr(ro, (void **)&rv, wxT("wxObject"));          \
                Py_DECREF(ro);                                                  \
            }                                                                   \
            Py_DECREF(so);                                                      \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_BOOL_NODE_pure(CBNAME)                                   \
    bool CBNAME(wxXmlNode* a)


#define IMP_PYCALLBACK_BOOL_NODE_pure(CLASS, PCLASS, CBNAME)                    \
    bool CLASS::CBNAME(wxXmlNode* a) {                                          \
        bool rv=false;                                                          \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* obj = wxPyConstructObject((void*)a, wxT("wxXmlNode"), 0); \
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));     \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rv;                                                              \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_COORD_SIZET_constpure(CBNAME)                            \
    wxCoord CBNAME(size_t a) const


#define IMP_PYCALLBACK_COORD_SIZET_constpure(CLASS, PCLASS, CBNAME)             \
    wxCoord CLASS::CBNAME(size_t a) const {                                     \
        wxCoord rval=0;                                                         \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VOID_SIZETSIZET_const(CBNAME)                            \
    void CBNAME(size_t a, size_t b) const;                                      \
    void base_##CBNAME(size_t a, size_t b) const


#define IMP_PYCALLBACK_VOID_SIZETSIZET_const(CLASS, PCLASS, CBNAME)             \
    void CLASS::CBNAME(size_t a, size_t b) const {                              \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));          \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b);                                                \
    }                                                                           \
    void CLASS::base_##CBNAME(size_t a, size_t b) const {                       \
        PCLASS::CBNAME(a,b);                                                    \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_COORD_const(CBNAME)                       \
    wxCoord CBNAME() const;                                      \
    wxCoord base_##CBNAME() const


#define IMP_PYCALLBACK_COORD_const(CLASS, PCLASS, CBNAME)                       \
    wxCoord CLASS::CBNAME() const {                                             \
        wxCoord rval=0;                                                         \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    wxCoord CLASS::base_##CBNAME() const {                                      \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCRECTSIZET_constpure(CBNAME)                           \
    void CBNAME(wxDC& a, const wxRect& b, size_t c) const;


#define IMP_PYCALLBACK__DCRECTSIZET_constpure(CLASS, PCLASS, CBNAME)            \
    void CLASS::CBNAME(wxDC& a, const wxRect& b, size_t c) const {              \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a,false);                        \
            PyObject* ro = wxPyConstructObject((void*)&b, wxT("wxRect"), 0);    \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOi)", obj, ro, (int)c)); \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK__DCRECTSIZET_const(CBNAME)                               \
    void CBNAME(wxDC& a, const wxRect& b, size_t c) const;                      \
    void base_##CBNAME(wxDC& a, const wxRect& b, size_t c) const


#define IMP_PYCALLBACK__DCRECTSIZET_const(CLASS, PCLASS, CBNAME)                \
    void CLASS::CBNAME(wxDC& a, const wxRect& b, size_t c) const {              \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxObject(&a,false);                        \
            PyObject* ro = wxPyConstructObject((void*)&b, wxT("wxRect"), 0);    \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOi)", obj, ro, (int)c)); \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b,c);                                              \
    }                                                                           \
    void CLASS::base_##CBNAME(wxDC& a, const wxRect& b, size_t c) const {       \
        PCLASS::CBNAME(a,b,c);                                                  \
    }

//---------------------------------------------------------------------------


#define DEC_PYCALLBACK_STRING_SIZET(CBNAME)                                     \
    wxString CBNAME(size_t a) const;                                            \
    wxString base_##CBNAME(size_t a) const

#define IMP_PYCALLBACK_STRING_SIZET(CLASS, PCLASS, CBNAME)                      \
    wxString CLASS::CBNAME(size_t a) const {                                    \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }                                                                           \
    wxString CLASS::base_##CBNAME(size_t a) const {                             \
        return PCLASS::CBNAME(a);                                               \
    }

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_STRING_SIZET_pure(CBNAME)                                \
    wxString CBNAME(size_t a) const

#define IMP_PYCALLBACK_STRING_SIZET_pure(CLASS, PCLASS, CBNAME)                 \
    wxString CLASS::CBNAME(size_t a) const {                                    \
        wxString rval;                                                          \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(l)",a));     \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }                                                                           \

//---------------------------------------------------------------------------

#define DEC_PYCALLBACK_VIZATTR_(CBNAME)                                         \
    wxVisualAttributes  CBNAME() const;                                         \
    wxVisualAttributes  base_##CBNAME()


#define IMP_PYCALLBACK_VIZATTR_(CLASS, PCLASS, CBNAME)                          \
    wxVisualAttributes CLASS::CBNAME() const {                                  \
        wxVisualAttributes rval;                                                \
        bool found;                                                             \
        bool blocked = wxPyBeginBlockThreads();                                 \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxVisualAttributes* ptr;                                            \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));        \
            if (ro) {                                                           \
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxVisualAttributes"))) \
                    rval = *ptr;                                                \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }                                                                           \
    wxVisualAttributes  CLASS::base_##CBNAME() {                                \
        return PCLASS::CBNAME();                                                \
    }

//---------------------------------------------------------------------------

#endif
