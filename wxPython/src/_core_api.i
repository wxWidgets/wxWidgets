/////////////////////////////////////////////////////////////////////////////
// Name:        _core_api.i
// Purpose:
//
// Author:      Robin Dunn
//
// Created:     13-Sept-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%{
#ifndef wxPyUSE_EXPORT
// Helper functions for dealing with SWIG objects and such.  These are
// located here so they know about the SWIG types and functions declared
// in the wrapper code.

#include <wx/hashmap.h>
WX_DECLARE_STRING_HASH_MAP( swig_type_info*, wxPyTypeInfoHashMap );


// Maintains a hashmap of className to swig_type_info pointers.  Given the
// name of a class either looks up the type info in the cache, or scans the
// SWIG tables for it.
extern PyObject* wxPyPtrTypeMap; 
static
swig_type_info* wxPyFindSwigType(const wxChar* className) {

    static wxPyTypeInfoHashMap* typeInfoCache = NULL;

    if (typeInfoCache == NULL)
        typeInfoCache = new wxPyTypeInfoHashMap;

    wxString name(className);
    swig_type_info* swigType = (*typeInfoCache)[name];

    if (! swigType) {
        // it wasn't in the cache, so look it up from SWIG
        name.Append(wxT(" *"));
        swigType = SWIG_TypeQuery(name.mb_str());
        
        // if it still wasn't found, try looking for a mapped name
        if (!swigType) {
            PyObject* item;
            name = className;
            
            if ((item = PyDict_GetItemString(wxPyPtrTypeMap,
                               (char*)(const char*)name.mbc_str())) != NULL) {
                name = wxString(PyString_AsString(item), *wxConvCurrent);
                name.Append(wxT(" *"));
                swigType = SWIG_TypeQuery(name.mb_str());
            }
        }
        if (swigType) {
            // and add it to the map if found
            (*typeInfoCache)[className] = swigType;
        }
    }
    return swigType;    
}


// Check if a class name is a type known to SWIG
bool wxPyCheckSwigType(const wxChar* className) {

    swig_type_info* swigType = wxPyFindSwigType(className);
    return swigType != NULL;
}
 
    
// Given a pointer to a C++ object and a class name, construct a Python proxy
// object for it.    
PyObject* wxPyConstructObject(void* ptr,
                              const wxChar* className,
                              int setThisOwn) {

    swig_type_info* swigType = wxPyFindSwigType(className);
    wxCHECK_MSG(swigType != NULL, NULL, wxT("Unknown type in wxPyConstructObject"));

    return SWIG_Python_NewPointerObj(ptr, swigType, setThisOwn);
}


// Extract a pointer to the wrapped C++ object from a Python proxy object.
// Ensures that the proxy object is of the specified (or derived) type.  If
// not able to perform the conversion then a Python exception is set and the
// error should be handled properly in the caller.  Returns True on success.
bool wxPyConvertSwigPtr(PyObject* obj, void **ptr,
                        const wxChar* className) {

    swig_type_info* swigType = wxPyFindSwigType(className);
    wxCHECK_MSG(swigType != NULL, false, wxT("Unknown type in wxPyConvertSwigPtr"));

    return SWIG_Python_ConvertPtr(obj, ptr, swigType, SWIG_POINTER_EXCEPTION) != -1;
}

%}


#if SWIG_VERSION < 0x010328
%{
// Make a SWIGified pointer object suitable for a .this attribute
PyObject* wxPyMakeSwigPtr(void* ptr, const wxChar* className) {
    
    PyObject* robj = NULL;

    swig_type_info* swigType = wxPyFindSwigType(className);
    wxCHECK_MSG(swigType != NULL, NULL, wxT("Unknown type in wxPyMakeSwigPtr"));

#ifdef SWIG_COBJECT_TYPES
    robj = PySwigObject_FromVoidPtrAndDesc((void *) ptr, (char *)swigType->name);
#else
    {
        char result[1024];
        robj = SWIG_PackVoidPtr(result, ptr, swigType->name, sizeof(result)) ?
            PyString_FromString(result) : 0;
    }
#endif
    return robj;
}
%}

#else // SWIG_VERSION >= 1.3.28
%{
// Make a SWIGified pointer object suitable for a .this attribute
PyObject* wxPyMakeSwigPtr(void* ptr, const wxChar* className) {
    
    PyObject* robj = NULL;

    swig_type_info* swigType = wxPyFindSwigType(className);
    wxCHECK_MSG(swigType != NULL, NULL, wxT("Unknown type in wxPyMakeSwigPtr"));

    robj = PySwigObject_New(ptr, swigType, 0);
    return robj;
}
%}
#endif



%{    
// Python's PyInstance_Check does not return True for instances of new-style
// classes.  This should get close enough for both new and old classes but I
// should re-evaluate the need for doing instance checks...
bool wxPyInstance_Check(PyObject* obj) {
    return PyObject_HasAttrString(obj, "__class__") != 0;
}


// This one checks if the object is an instance of a SWIG proxy class (it has
// a .this attribute, and the .this attribute is a PySwigObject.)
bool wxPySwigInstance_Check(PyObject* obj) {
    static PyObject* this_str = NULL;
    if (this_str == NULL)
        this_str = PyString_FromString("this");
    
    PyObject* this_attr = PyObject_GetAttr(obj, this_str);
    if (this_attr) {
        bool retval = (PySwigObject_Check(this_attr) != 0);
        Py_DECREF(this_attr);
        return retval;
    }

    PyErr_Clear();
    return false;
}


// Export a C API in a struct.  Other modules will be able to load this from
// the wx._core_ module and will then have safe access to these functions,
// even if they are located in another shared library.
static wxPyCoreAPI API = {

    wxPyCheckSwigType,
    wxPyConstructObject,
    wxPyConvertSwigPtr,
    wxPyMakeSwigPtr,
                                             
    wxPyBeginAllowThreads,
    wxPyEndAllowThreads,
    wxPyBeginBlockThreads,
    wxPyEndBlockThreads,
                                             
    wxPy_ConvertList,
                                             
    wxString_in_helper,
    Py2wxString,
    wx2PyString,
                                             
    byte_LIST_helper,
    int_LIST_helper,
    long_LIST_helper,
    string_LIST_helper,
    wxPoint_LIST_helper,
    wxBitmap_LIST_helper,
    wxString_LIST_helper,
    wxAcceleratorEntry_LIST_helper,
                                             
    wxSize_helper,
    wxPoint_helper,
    wxRealPoint_helper,
    wxRect_helper,
    wxColour_helper,
    wxPoint2D_helper,
                                             
    wxPySimple_typecheck,
    wxColour_typecheck,

    wxPyCBH_setCallbackInfo,
    wxPyCBH_findCallback,
    wxPyCBH_callCallback,
    wxPyCBH_callCallbackObj,
    wxPyCBH_delete,
                                             
    wxPyMake_wxObject,
    wxPyMake_wxSizer,
    wxPyPtrTypeMap_Add,
    wxPy2int_seq_helper,
    wxPy4int_seq_helper,
    wxArrayString2PyList_helper,
    wxArrayInt2PyList_helper,
                                             
    wxPyClientData_dtor,
    wxPyUserData_dtor,
    wxPyOORClientData_dtor,
                                             
    wxPyCBInputStream_create,
    wxPyCBInputStream_copy,
    
    wxPyInstance_Check,
    wxPySwigInstance_Check,

    wxPyCheckForApp,

    wxArrayDouble2PyList_helper,
    wxPoint2D_LIST_helper
};

#endif
%}




%init %{
#ifndef wxPyUSE_EXPORT
    // Make our API structure a CObject so other modules can import it
    // from this module.
    PyObject* cobj = PyCObject_FromVoidPtr(&API, NULL);
    PyDict_SetItemString(d,"_wxPyCoreAPI", cobj);
    Py_XDECREF(cobj);
#endif
%}

//---------------------------------------------------------------------------
