/////////////////////////////////////////////////////////////////////////////
// Name:        activex.i
// Purpose:     ActiveX controls (such as Internet Explorer) in a wxWindow
//
// Author:      Robin Dunn
//
// Created:     18-Mar-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module(package="wx") activex

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"

#include "wxactivex.h"
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING_NOSWIG(PanelNameStr);


//---------------------------------------------------------------------------

typedef unsigned short  USHORT;
typedef long            DISPID;
typedef long            MEMBERID;
typedef unsigned short  VARTYPE;


%{
// Since SWIG doesn't support nested classes, we need to fool it a bit
// and make them look like global classes.  These defines make the C++ code
// know what we are doing.
#define wxParamX        wxActiveX::ParamX
#define wxFuncX         wxActiveX::FuncX
#define wxPropX         wxActiveX::PropX
#define wxParamXArray   wxActiveX::ParamXArray
#define wxFuncXArray    wxActiveX::FuncXArray
#define wxPropXArray    wxActiveX::PropXArray
%}


%{
// Some conversion helpers
static wxVariant _PyObj2Variant(PyObject* value);
static bool  _PyObj2Variant(PyObject* value, wxVariant& wv);
static PyObject* _Variant2PyObj(wxVariant& value, bool useNone=false);
static wxString  _VARTYPEname(VARTYPE vt);

// Check if an exception has been raised  (blocking threads)
inline bool wxPyErr_Occurred()
{
    bool rval;
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    rval = PyErr_Occurred() != NULL;
    wxPyEndBlockThreads(blocked);
    return rval;
}

%}

//---------------------------------------------------------------------------
%newgroup

DocStr(CLSID,
"This class wraps the Windows CLSID structure and is used to
specify the class of the ActiveX object that is to be created.  A
CLSID can be constructed from either a ProgID string, (such as
'WordPad.Document.1') or a classID string, (such as
'{CA8A9783-280D-11CF-A24D-444553540000}').", "");

class CLSID {
public:
    %extend {
        CLSID(const wxString& id)
        {
            int result;
            CLSID* self = new CLSID;
            memset(self, 0, sizeof(CLSID));

            if (id[0] == _T('{')) {
                // Looks like a classID string
                result =
                    CLSIDFromString(
                        (LPOLESTR)(const wchar_t *)id.wc_str(wxConvUTF8),
                        self);
            } else {
                // Try a progID
                result =
                    CLSIDFromProgID(
                        (LPOLESTR)(const wchar_t *)id.wc_str(wxConvUTF8),
                        self);
            }
            if (result != NOERROR) {
                wxPyErr_SetString(PyExc_ValueError, "Not a recognized classID or progID");
                delete self;
                return NULL;
            }
            return self;
        }

        ~CLSID() { delete self; }

        wxString GetCLSIDString()
        {
            LPOLESTR s;
            wxString str;
            if (StringFromCLSID(*self, &s) == S_OK) {
                str = s;
                CoTaskMemFree(s);
            }
            else {
                str = _T("Error!");  // TODO: raise exception?
            }
            return str;
        }
        wxString GetProgIDString()
        {
            LPOLESTR s;
            wxString str;
            if (ProgIDFromCLSID(*self, &s) == S_OK) {
                str = s;
                CoTaskMemFree(s);
            }
            else {
                str = _T("Error!"); // TODO: raise exception?
            }
            return str;
        }
    }
    %pythoncode { def __str__(self):   return self.GetCLSIDString() }
};


//---------------------------------------------------------------------------
%newgroup

%define MAKE_ARRAY_WRAPPER(basetype, arrayname)
class arrayname
{
public:
    %extend {
        bool __nonzero__()          { return self->size() > 0; }
        int  __len__()              { return self->size(); }

        const basetype& __getitem__(int idx) {
            if ( idx >= 0 && idx < self->size() )
                return (*self)[idx];
            else {
                static basetype BadVal;
                wxPyErr_SetString(PyExc_IndexError, "Index out of range");
                return BadVal;
            }
        }
        // TODO  __iter__??
    }
};
%enddef

//---------------------------------------------------------------------------

%immutable;

class wxParamX
{
public:
    USHORT      flags;
    bool        isPtr;
    bool        isSafeArray;
    bool        isOptional;
    VARTYPE     vt;
    wxString    name;
    
    %feature("shadow") vt_type_get "vt_type = property(_activex.ParamX_vt_type_get)";
    %extend { wxString vt_type_get() { return _VARTYPEname(self->vt); } }

    %feature("shadow") IsIn     "isIn = property(_activex.ParamX_IsIn)";
    %feature("shadow") IsOut    "isOut = property(_activex.ParamX_IsOut)";
    %feature("shadow") IsRetVal "isRetVal = property(_activex.ParamX_IsRetVal)";
    bool IsIn() const;
    bool IsOut() const;
    bool IsRetVal() const;

};


class wxFuncX
{
public:
    wxString      name;
    MEMBERID      memid;
    bool          hasOut;

    wxParamX      retType;
    wxParamXArray params;
};


class wxPropX
{
public:
    wxString    name;
    MEMBERID    memid;
    wxParamX    type;
    wxParamX    arg;
    bool        putByRef;

    %feature("shadow") CanGet "canGet = property(_activex.PropX_CanGet)";
    %feature("shadow") CanSet "canSet = property(_activex.PropX_CanSet)";
    bool CanGet() const;
    bool CanSet() const;
};
%mutable;


MAKE_ARRAY_WRAPPER(wxParamX, wxParamXArray);
MAKE_ARRAY_WRAPPER(wxFuncX, wxFuncXArray);
MAKE_ARRAY_WRAPPER(wxPropX, wxPropXArray);


//---------------------------------------------------------------------------
%newgroup

%{
// C++ version of a Python-aware wxActiveX    
class wxActiveXWindow : public wxActiveX
{
private:
    CLSID       m_CLSID;
    
    DECLARE_ABSTRACT_CLASS(wxActiveXWindow);

public:
    wxActiveXWindow( wxWindow* parent, const CLSID& clsId, wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxString& name = wxPyPanelNameStr)
        : wxActiveX(parent, clsId, id, pos, size, style, name)
    {
        m_CLSID = clsId;
    }

    const CLSID& GetCLSID() const { return m_CLSID; }
    
    
    // Renamed versions of some base class methods that delegate
    // to the base where appropriate, and raise Python exceptions
    // when needed.
    int GetAXEventCount() const  { return wxActiveX::GetEventCount(); }
    int GetAXPropCount() const   { return wxActiveX::GetPropCount(); }
    int GetAXMethodCount() const { return wxActiveX::GetMethodCount(); }

    const wxFuncX& GetAXEventDesc(int idx) const
    {
        static wxFuncX BadVal;
        if (idx < 0 || idx >= GetAXEventCount()) {
            wxPyErr_SetString(PyExc_IndexError, "Index out of range");
            return BadVal;
        }
        return m_events[idx];
    }
    const wxFuncX& GetAXMethodDesc(int idx) const
    {
        static wxFuncX BadVal;
        if (idx < 0 || idx >= GetAXMethodCount()) {
            wxPyErr_SetString(PyExc_IndexError, "Index out of range");
            return BadVal;
        }
        return m_methods[idx];
    }
    const wxPropX& GetAXPropDesc(int idx) const
    {
        static wxPropX BadVal;
        if (idx < 0 || idx >= GetAXPropCount()) {
            wxPyErr_SetString(PyExc_IndexError, "Index out of range");
            return BadVal;
        }
        return m_props[idx];
    }

    const wxFuncX& GetAXMethodDesc(const wxString& name) const
    {
        NameMap::const_iterator it = m_methodNames.find(name);
        if (it == m_methodNames.end())     {
            wxString msg;
            msg << _T("method <") << name << _T("> not found");
            wxPyErr_SetString(PyExc_KeyError, msg.mb_str());
            static wxFuncX BadVal;
            return BadVal;
        };
        return GetAXMethodDesc(it->second);
    }
    const wxPropX& GetAXPropDesc(const wxString& name) const
    {
        NameMap::const_iterator it = m_propNames.find(name);
        if (it == m_propNames.end())     {
            wxString msg;
            msg << _T("property <") << name << _T("> not found");
            wxPyErr_SetString(PyExc_KeyError, msg.mb_str());
            static wxPropX BadVal;
            return BadVal;
        };
        return GetAXPropDesc(it->second);
    }

    // Accessors for the internal vectors of events, methods and
    // proprties.  Can be used as sequence like objects from
    // Python.
    const wxFuncXArray& GetAXEvents()      { return m_events; }
    const wxFuncXArray& GetAXMethods()     { return m_methods; }
    const wxPropXArray& GetAXProperties()  { return m_props; }


    // Set a property from a Python object
    void SetAXProp(const wxString& name, PyObject* value)
    {        
        const wxPropX& prop = GetAXPropDesc(name);
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (! PyErr_Occurred() ) {
            if (! prop.CanSet()) {
                wxString msg;
                msg << _T("property <") << name << _T("> is readonly");
                PyErr_SetString(PyExc_TypeError, msg.mb_str());
                goto done;
            } else {
                wxVariant wxV = _PyObj2Variant(value);
                if (PyErr_Occurred())
                    goto done;
                VARIANT v = {prop.arg.vt};
                if (!VariantToMSWVariant(wxV, v) || PyErr_Occurred()) {
                    wxString msg;
                    msg << _T("Unable to convert value to expected type: (")
                        << _VARTYPEname(prop.arg.vt) << _T(") for property <")
                        << name << _T(">");
                    PyErr_SetString(PyExc_TypeError, msg.mb_str());
                    goto done;
                }
                PyThreadState* tstate = wxPyBeginAllowThreads();
                SetProp(prop.memid, v);
                VariantClear(&v);
                wxPyEndAllowThreads(tstate);
            }
        }
    done:
        wxPyEndBlockThreads(blocked);
    }

    
    // Get a property and convert it to a Python object
    PyObject* GetAXProp(const wxString& name)
    {        
        PyObject* rval = NULL;
        const wxPropX& prop = GetAXPropDesc(name);
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (! PyErr_Occurred() ) {
            if (! prop.CanGet()) {
                wxString msg;
                msg << _T("property <") << name << _T("> is writeonly");
                PyErr_SetString(PyExc_TypeError, msg.mb_str());
                goto done;
            } else {
                PyThreadState* tstate = wxPyBeginAllowThreads();
                VARIANT v = GetPropAsVariant(prop.memid);
                wxPyEndAllowThreads(tstate);
                wxVariant wv;
                if (!MSWVariantToVariant(v, wv) || PyErr_Occurred()) {
                    wxString msg;
                    msg << _T("Unable to convert value to expected type: (")
                        << _VARTYPEname(prop.arg.vt) << _T(") for property <")
                        << name << _T(">");
                    PyErr_SetString(PyExc_TypeError, msg.mb_str());
                    goto done;
                }
                rval = _Variant2PyObj(wv);
                VariantClear(&v);
            }
        }
    done:
        wxPyEndBlockThreads(blocked);
        return rval;
    }


    // If both IsIn and isOut are false, assume it is actually an
    // input param
    bool paramIsIn(const wxParamX& p)
    {
        return p.IsIn() || (!p.IsIn() && !p.IsOut());
    }
    

    // Call a method of the ActiveX object
    PyObject* _CallAXMethod(const wxString& name, PyObject* args)
    {
        VARIANTARG *vargs = NULL;
        int nargs = 0;
        PyObject* rval = NULL;
        const wxFuncX& func = GetAXMethodDesc(name);
        
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (! PyErr_Occurred() ) {
            nargs = func.params.size();
            if (nargs > 0)
                vargs = new VARIANTARG[nargs];

            if (vargs) {
                // init type of vargs, in reverse order
                int i;
                for (i = 0; i < nargs; i++)
                    vargs[nargs - i - 1].vt = func.params[i].vt;

                // Map the args coming from Python to the input parameters in vargs
                int pi = 0;
                i = 0;
                while ( i<nargs && pi<PyTuple_Size(args) ) {
                    // Move to the next input param. 
                    if (! paramIsIn(func.params[i])) {
                        i += 1;
                        continue;
                    }
                    // convert the python object
                    PyObject* obj = PyTuple_GetItem(args, pi);
                    if (obj == Py_None) // special-case None?
                        vargs[nargs - i - 1].vt = VT_EMPTY;
                    else {
                        wxVariant wxV = _PyObj2Variant(obj);
                        if (PyErr_Occurred())
                            goto done;
                        if (!VariantToMSWVariant(wxV, vargs[nargs - i - 1]) || PyErr_Occurred()) {
                            wxString msg;
                            msg << _T("Unable to convert value to expected type: (")
                                << _VARTYPEname(vargs[nargs - i - 1].vt)
                                << _T(") for parameter ") << i;
                            PyErr_SetString(PyExc_TypeError, msg.mb_str());
                            goto done;
                        }
                    }
                    i += 1;
                    pi += 1;                        
                }
            }

            // call the method
            PyThreadState* tstate = wxPyBeginAllowThreads();
            VARIANT rv = CallMethod(func.memid, vargs, nargs);
            wxPyEndAllowThreads(tstate);

            // Convert the return value and any out-params, ignoring
            // conversion errors for now
            wxVariant wv;
            MSWVariantToVariant(rv, wv);
            rval = _Variant2PyObj(wv, true);
            VariantClear(&rv);

            if (func.hasOut) {
                // make a list and put the rval in it if it is not None
                PyObject* lst = PyList_New(0);
                if (rval != Py_None)
                    PyList_Append(lst, rval);
                else
                    Py_DECREF(rval);

                // find the out params and convert them
                for (int i = 0; i < nargs; i++) {
                    VARIANTARG& va = vargs[nargs - i - 1];
                    const wxParamX &px = func.params[i];
                    if (px.IsOut()) {
                        MSWVariantToVariant(va, wv);
                        PyObject* obj = _Variant2PyObj(wv, true);
                        PyList_Append(lst, obj);
                    }
                }
                rval = PyList_AsTuple(lst);
                Py_DECREF(lst);
            }
            if (PyErr_Occurred())
                PyErr_Clear();
        }
    done:
        wxPyEndBlockThreads(blocked);
        if (vargs) {
            for (int i = 0; i < nargs; i++)
                VariantClear(&vargs[i]);
            delete [] vargs;
        }
        return rval;
    }
};

IMPLEMENT_ABSTRACT_CLASS( wxActiveXWindow, wxWindow );
%}



// Now tell SWIG about this new class that is implemented above.

DocStr(wxActiveXWindow,
"ActiveXWindow derives from wxWindow and the constructor accepts a
CLSID for the ActiveX Control that should be created.  The
ActiveXWindow class simply adds methods that allow you to query
some of the TypeInfo exposed by the ActiveX object, and also to
get/set properties or call methods by name.  The Python
implementation automatically handles converting parameters and
return values to/from the types expected by the ActiveX code as
specified by the TypeInfo.
", "");


MustHaveApp(wxActiveXWindow);

class wxActiveXWindow : public wxWindow
{
public:
    %pythonAppend wxActiveXWindow      "self._setOORInfo(self)"

    DocCtorStr(
        wxActiveXWindow( wxWindow* parent, const CLSID& clsId, wxWindowID id = -1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = 0,
                         const wxString& name = wxPyPanelNameStr),
        "Creates an ActiveX control from the clsID given and makes it act
as much like a regular wx.Window as possible.", "");

    DocDeclStr(
        const CLSID& , GetCLSID() const,
        "Return the CLSID used to construct this ActiveX window", "");
    

    DocDeclStr(
        int , GetAXEventCount() const,
        "Number of events defined for this control", "");

    DocDeclStr(
        const wxFuncX& , GetAXEventDesc(int idx) const,
        "Returns event description by index", "");


    DocDeclStr(
        int , GetAXPropCount() const,
        "Number of properties defined for this control", "");

    %nokwargs GetAXPropDesc;
    DocStr(GetPropDesc, "Returns property description by index or by name", "");
    const wxPropX& GetAXPropDesc(int idx) const;
    const wxPropX& GetAXPropDesc(const wxString& name) const;



    DocDeclStr(
        int , GetAXMethodCount() const,
        "Number of methods defined for this control", "");

    %nokwargs GetAXMethodDesc;
    DocStr(GetMethodDesc, "Returns method description by index or name", "");
    const wxFuncX& GetAXMethodDesc(int idx) const;
    const wxFuncX& GetAXMethodDesc(const wxString& name) const;


    DocDeclStr(
        const wxFuncXArray& , GetAXEvents(),
        "Returns a sequence of FuncX objects describing the events
available for this ActiveX object.", "");

    DocDeclStr(
        const wxFuncXArray& , GetAXMethods(),
        "Returns a sequence of FuncX objects describing the methods
available for this ActiveX object.", "");

    DocDeclStr(
        const wxPropXArray& , GetAXProperties(),
        "Returns a sequence of PropX objects describing the properties
available for this ActiveX object.", "");



    DocDeclStr(
        void , SetAXProp(const wxString& name, PyObject* value),
        "Set a property of the ActiveX object by name.", "");


    DocDeclStr(
        PyObject* , GetAXProp(const wxString& name),
        "Get the value of an ActiveX property by name.", "");

    
    %nokwargs _CallAXMethod;
    DocDeclStr(
        PyObject* , _CallAXMethod(const wxString& name, PyObject* args),
        "The implementation for CallMethod.  Calls an ActiveX method, by
name passing the parameters given in args.", "");
    %pythoncode {
        def CallAXMethod(self, name, *args):
            """
            Front-end for _CallMethod.  Simply passes all positional args
            after the name as a single tuple to _CallMethod.
            """
            return self._CallAXMethod(name, args)
    }
};

//---------------------------------------------------------------------------
%newgroup

DocDeclStr(
    wxEventType , RegisterActiveXEvent(const wxString& eventName),
    "Creates a standard wx event ID for the given eventName.", "");



DocStr(wxActiveXEvent,
"An instance of ActiveXEvent is sent to the handler for all bound
ActiveX events.  Any event parameters from the ActiveX cntrol are
turned into attributes of the Python proxy for this event object.
Additionally, there is a property called eventName that will
return (surprisingly <wink>) the name of the ActiveX event.", "");

class wxActiveXEvent : public wxCommandEvent
{
public:
    %feature("shadow") EventName "eventName = property(_activex.ActiveXEvent_EventName)";
    wxString EventName();

    %extend {

        // This is called by the EventThunker before calling the
        // handler. We'll convert and load the ActiveX event parameters into
        // attributes of the Python event object.
        void _preCallInit(PyObject* pyself) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* pList = PyList_New(0);
            PyObject_SetAttrString(pyself, "paramList", pList);
            Py_DECREF(pList);
            for (int i=0; i<self->ParamCount(); i+=1) {
                PyObject* name = PyString_FromString((char*)(const char*)self->ParamName(i).mb_str());
                PyObject* val = _Variant2PyObj((*self)[i], true);
                PyObject_SetAttr(pyself, name, val);
                PyList_Append(pList, name);
                Py_DECREF(val);
                Py_DECREF(name);
            }
            wxPyEndBlockThreads(blocked);
        }

        // This one is called by the EventThunker after calling the
        // handler. It reloads any "out" parameters from the python attributes
        // back into the wxVariant they came from.        
        void _postCallCleanup(PyObject* pyself) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            for (int i=0; i<self->ParamCount(); i+=1) {
                PyObject* val = PyObject_GetAttrString(
                    pyself, (char*)(const char*)self->ParamName(i).mb_str());
                _PyObj2Variant(val, (*self)[i]);
                Py_DECREF(val);
            }
            wxPyEndBlockThreads(blocked);
        }
        
        
    }
};    

//---------------------------------------------------------------------------

%{

// Caller should already have the GIL!
wxVariant _PyObj2Variant(PyObject* value)
{
    wxVariant rval;

    if (value == Py_None)
        return rval;
    
#if PYTHON_API_VERSION >= 1012  // Python 2.3+
    else if (PyBool_Check(value))
        rval = (value == Py_True) ? true : false;
#endif
    
    else if (PyInt_Check(value))
        rval = PyInt_AS_LONG(value);

    else if (PyFloat_Check(value))
        rval = PyFloat_AS_DOUBLE(value);

    else if (PyString_Check(value) || PyUnicode_Check(value))
        rval = Py2wxString(value);

    // TODO:    PyList of strings --> wxArrayString
    //          wxDateTime
    //          list of objects
    //          etc.

    else {
        PyErr_SetString(PyExc_TypeError, "Unsupported object type in _PyObj2Variant");
        rval = (long)0;
    }

    return rval;
}

// This one uses the type of the variant to try and force the conversion
bool  _PyObj2Variant(PyObject* value, wxVariant& wv)
{
    wxString type = wv.GetType();

    if ( type == _T("long") || type == _T("bool") || type == _T("char") )
        wv = PyInt_AsLong(value);

    else if ( type == _T("string") )
        wv = Py2wxString(value);

    else if ( type == _T("double") )
        wv  = PyFloat_AsDouble(value);

    else {
        // it's some other type that we dont' handle yet.  Log it?
        return false;
    }
    return true;
}
 
// Caller should already have the GIL!
PyObject* _Variant2PyObj(wxVariant& value, bool useNone)
{
    PyObject* rval = NULL;

    if (value.IsNull()) {
        rval = Py_None;
        Py_INCREF(rval);
    }
    
    // should "char" be treated as an int or as a string?
    else if (value.IsType(_T("char")) || value.IsType(_T("long")))
        rval = PyInt_FromLong(value);
    
    else if (value.IsType(_T("double")))
        rval = PyFloat_FromDouble(value);

    else if (value.IsType(_T("bool"))) {
        rval = (bool)value ? Py_True : Py_False;
        Py_INCREF(rval);
    }
    
    else if (value.IsType(_T("string")))
        rval = wx2PyString(value);

    else {
        if (useNone) {
            rval = Py_None;
            Py_INCREF(rval);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Unsupported object type in _Variant2PyObj");
        }
    }
    return rval;
}


wxString _VARTYPEname(VARTYPE vt)
{
    if (vt & VT_BYREF)
        vt &= ~(VT_BYREF);

    switch(vt) {
    case VT_VARIANT:
        return _T("VT_VARIANT");
        
    // 1 byte chars
    case VT_I1:
    case VT_UI1:
    // 2 byte shorts
    case VT_I2:
    case VT_UI2:
    // 4 bytes longs
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
        return _T("int");

    // 4 byte floats
    case VT_R4:
    // 8 byte doubles
    case VT_R8:
    // decimals are converted from doubles too
    case VT_DECIMAL:
        return _T("double");
        
    case VT_BOOL:
        return _T("bool");
        
    case VT_DATE:
        return _T("wx.DateTime");
        
    case VT_BSTR:
        return _T("string");

    case VT_UNKNOWN:
        return _T("VT_UNKNOWN");
        
    case VT_DISPATCH: 
        return _T("VT_DISPATCH");

    case VT_EMPTY:
        return _T("VT_EMPTY");
        
    case VT_NULL:
        return _T("VT_NULL");
        
    case VT_VOID:
        return _T("VT_VOID");
        
    default:
        wxString msg;
        msg << _T("unsupported type ") << vt;
        return msg;
    }
}

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%newgroup


%{
// A class derived from our wxActiveXWindow for the IE WebBrowser
// control that will serve as a base class for a Python
// implementation.  This is done so we can "eat our own dog food"
// and use a class at least mostly generated by genaxmodule, but
// also get some of the extra stuff like loading a document from
// a string or a stream, getting text contents, etc. that
// Lindsay's version gives us.
//

#include <wx/mstream.h>
#include <oleidl.h>
#include <winerror.h>
#include <exdispid.h>
#include <exdisp.h>
#include <olectl.h>
#include <Mshtml.h>
#include <sstream>

#include "IEHtmlStream.h"

class wxIEHtmlWindowBase : public wxActiveXWindow {
private:
    wxAutoOleInterface<IWebBrowser2>  m_webBrowser;

    DECLARE_ABSTRACT_CLASS(wxIEHtmlWindowBase);

public:

    wxIEHtmlWindowBase ( wxWindow* parent, const CLSID& clsId, wxWindowID id = -1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = 0,
                         const wxString& name = wxPyPanelNameStr)
        : wxActiveXWindow(parent, clsId, id, pos, size, style, name)
    {
        HRESULT hret;

        // Get IWebBrowser2 Interface
        hret = m_webBrowser.QueryInterface(IID_IWebBrowser2, m_ActiveX);
        wxASSERT(SUCCEEDED(hret));

        // web browser setup
        m_webBrowser->put_MenuBar(VARIANT_FALSE);
        m_webBrowser->put_AddressBar(VARIANT_FALSE);
        m_webBrowser->put_StatusBar(VARIANT_FALSE);
        m_webBrowser->put_ToolBar(VARIANT_FALSE);

        m_webBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
        m_webBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);

        m_webBrowser->Navigate( L"about:blank", NULL, NULL, NULL, NULL );
    }


    void SetCharset(const wxString& charset)
    {
        HRESULT hret;
        
        // HTML Document ?
        IDispatch *pDisp = NULL;
        hret = m_webBrowser->get_Document(&pDisp);
        wxAutoOleInterface<IDispatch> disp(pDisp);

        if (disp.Ok())
        {
            wxAutoOleInterface<IHTMLDocument2> doc(IID_IHTMLDocument2, disp);
            if (doc.Ok())
                doc->put_charset((BSTR) (const wchar_t *) charset.wc_str(wxConvUTF8));
            //doc->put_charset((BSTR) wxConvUTF8.cMB2WC(charset).data());
        }
    }


    bool LoadString(const wxString& html)
    {
        char *data = NULL;
        size_t len = html.length();
        len *= sizeof(wxChar);
        data = (char *) malloc(len);
        memcpy(data, html.c_str(), len);
        return LoadStream(new wxOwnedMemInputStream(data, len));
    }

    
    bool LoadStream(IStreamAdaptorBase *pstrm)
    {
        // need to prepend this as poxy MSHTML will not recognise a HTML comment
        // as starting a html document and treats it as plain text
        // Does nayone know how to force it to html mode ?
#if wxUSE_UNICODE
        // TODO: What to do in this case???
#else
        pstrm->prepend = _T("<html>");
#endif

        // strip leading whitespace as it can confuse MSHTML
        wxAutoOleInterface<IStream> strm(pstrm);

        // Document Interface
        IDispatch *pDisp = NULL;
        HRESULT hret = m_webBrowser->get_Document(&pDisp);
        if (! pDisp)
            return false;
        wxAutoOleInterface<IDispatch> disp(pDisp);


        // get IPersistStreamInit
        wxAutoOleInterface<IPersistStreamInit>
            pPersistStreamInit(IID_IPersistStreamInit, disp);

        if (pPersistStreamInit.Ok())
        {
            HRESULT hr = pPersistStreamInit->InitNew();
            if (SUCCEEDED(hr))
                hr = pPersistStreamInit->Load(strm);

            return SUCCEEDED(hr);
        }
        else
            return false;
    }

    bool LoadStream(wxInputStream *is)
    {
        // wrap reference around stream
        IwxStreamAdaptor *pstrm = new IwxStreamAdaptor(is);
        pstrm->AddRef();

        return LoadStream(pstrm);
    }

    
    wxString GetStringSelection(bool asHTML)
    {
        wxAutoOleInterface<IHTMLTxtRange> tr(wxieGetSelRange(m_oleObject));
        if (! tr)
            return wxEmptyString;

        BSTR text = NULL;
        HRESULT hr = E_FAIL;
    
        if (asHTML)
            hr = tr->get_htmlText(&text);
        else
            hr = tr->get_text(&text);
        if (hr != S_OK)
            return wxEmptyString;

        wxString s = text;
        SysFreeString(text);

        return s;
    };

    wxString GetText(bool asHTML)
    {
        if (! m_webBrowser.Ok())
            return wxEmptyString;

        // get document dispatch interface
        IDispatch *iDisp = NULL;
        HRESULT hr = m_webBrowser->get_Document(&iDisp);
        if (hr != S_OK)
            return wxEmptyString;

        // Query for Document Interface
        wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
        iDisp->Release();

        if (! hd.Ok())
            return wxEmptyString;

        // get body element
        IHTMLElement *_body = NULL;
        hd->get_body(&_body);
        if (! _body)
            return wxEmptyString;
        wxAutoOleInterface<IHTMLElement> body(_body);

        // get inner text
        BSTR text = NULL;
        hr = E_FAIL;
    
        if (asHTML)
            hr = body->get_innerHTML(&text);
        else
            hr = body->get_innerText(&text);
        if (hr != S_OK)
            return wxEmptyString;

        wxString s = text;
        SysFreeString(text);

        return s;   
    }


//     void wxIEHtmlWin::SetEditMode(bool seton)
//     {
//         m_bAmbientUserMode = ! seton;
//         AmbientPropertyChanged(DISPID_AMBIENT_USERMODE);
//     };

//     bool wxIEHtmlWin::GetEditMode()
//     {
//         return ! m_bAmbientUserMode;
//     };
};

IMPLEMENT_ABSTRACT_CLASS( wxIEHtmlWindowBase, wxActiveXWindow );

%}


// we'll document it in the derived Python class
%feature("noautodoc") wxIEHtmlWindowBase;
%feature("noautodoc") wxIEHtmlWindowBase::SetCharset;
%feature("noautodoc") wxIEHtmlWindowBase::LoadString;
%feature("noautodoc") wxIEHtmlWindowBase::LoadStream;
%feature("noautodoc") wxIEHtmlWindowBase::GetStringSelection;
%feature("noautodoc") wxIEHtmlWindowBase::GetText;


MustHaveApp(wxIEHtmlWindowBase);

class wxIEHtmlWindowBase : public wxActiveXWindow {
public:
    %pythonAppend wxIEHtmlWindowBase    "self._setOORInfo(self)"

    wxIEHtmlWindowBase ( wxWindow* parent, const CLSID& clsId, wxWindowID id = -1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = 0,
                         const wxString& name = wxPyPanelNameStr);

    void SetCharset(const wxString& charset);
    bool LoadString(const wxString& html);
    bool LoadStream(wxInputStream *is);
    wxString GetStringSelection(bool asHTML);
    wxString GetText(bool asHTML);
};











#if 0
enum wxIEHtmlRefreshLevel
{
    wxIEHTML_REFRESH_NORMAL = 0,
    wxIEHTML_REFRESH_IFEXPIRED = 1,
    wxIEHTML_REFRESH_CONTINUE = 2,
    wxIEHTML_REFRESH_COMPLETELY = 3
};

DocStr(wxIEHtmlWin,
"");
class wxIEHtmlWin : public wxWindow 
{
public:
    %pythonAppend wxIEHtmlWin      "self._setOORInfo(self)"

    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);

    void LoadUrl(const wxString& url);
    bool LoadString(wxString html);
    bool LoadStream(wxInputStream *is);

    %pythoncode { Navigate = LoadUrl }

    void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
    wxString GetText(bool asHTML = false);

    bool GoBack();
    bool GoForward();
    bool GoHome();
    bool GoSearch();
    %name(RefreshPage)bool Refresh(wxIEHtmlRefreshLevel level);
    bool Stop();
};



%pythoncode {
wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2  = RegisterActiveXEvent('BeforeNavigate2')
wxEVT_COMMAND_MSHTML_NEWWINDOW2       = RegisterActiveXEvent('NewWindow2')
wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE = RegisterActiveXEvent('DocumentComplete')
wxEVT_COMMAND_MSHTML_PROGRESSCHANGE   = RegisterActiveXEvent('ProgressChange')
wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE = RegisterActiveXEvent('StatusTextChange')
wxEVT_COMMAND_MSHTML_TITLECHANGE      = RegisterActiveXEvent('TitleChange')

EVT_MSHTML_BEFORENAVIGATE2      = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, 1)
EVT_MSHTML_NEWWINDOW2           = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_NEWWINDOW2, 1)
EVT_MSHTML_DOCUMENTCOMPLETE     = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, 1)
EVT_MSHTML_PROGRESSCHANGE       = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, 1)
EVT_MSHTML_STATUSTEXTCHANGE     = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, 1)
EVT_MSHTML_TITLECHANGE          = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_TITLECHANGE, 1)
}

#endif

//---------------------------------------------------------------------------
// Include some extra Python code into the proxy module

%pythoncode "_activex_ex.py"

//---------------------------------------------------------------------------
