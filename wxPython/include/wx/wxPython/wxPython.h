////////////////////////////////////////////////////////////////////////////
// Name:        wxPython.h
// Purpose:     To be used from modules that are not part of the core
//              wxPython extension--or from 3rd party apps and modules--in
//              order to get access to the wxPython API (some helper
//              functions and such) that lives in the wx.core module
//
// Author:      Robin Dunn
//
// Created:     14-Dec-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxp_export__
#define __wxp_export__

//----------------------------------------------------------------------

#define wxPyUSE_EXPORTED_API
#include "wx/wxPython/wxPython_int.h"

//----------------------------------------------------------------------

// wxPyCoreAPI_IMPORT can be called in extension modules or embedding
// applications in order to preload the API pointer, and then handle error
// conditions if it is not able to be loaded for any reason.  If the import
// fails or there is any other Python related problem then there will be a
// Python exception set that can be checked with PyErr_Occurred and/or
// PyErr_Print.
//
// Returns true on success, false if there was an error.
//
// See also the definition of the wxPyCoreAPI struct in wxPython_int.h
//
static bool wxPyCoreAPI_IMPORT()
{
    wxPyCoreAPIPtr = (wxPyCoreAPI*)PyCObject_Import("wx._core_", "_wxPyCoreAPI");
    return wxPyCoreAPIPtr != NULL;
}


// Used by the macros below to fetch the API pointer, importing it first if
// needed.  This should never need to be called directly.
inline wxPyCoreAPI* wxPyGetCoreAPIPtr()
{
    if (wxPyCoreAPIPtr == NULL) wxPyCoreAPI_IMPORT();
    // wxASSERT_MSG(wxPyCoreAPIPtr != NULL, wxT("wxPyCoreAPIPtr is NULL!!!"));  // uncomment when needed for debugging
    return wxPyCoreAPIPtr;
}


// The following macros call functions located in wx._core_ of the same name
// via the API pointer retrieved above.  TODO: Should these be made into
// inline fucntions?

#define wxPyCheckSwigType(a)                    (wxPyGetCoreAPIPtr()->p_wxPyCheckSwigType(a))
#define wxPyConstructObject(a,b,c)              (wxPyGetCoreAPIPtr()->p_wxPyConstructObject(a,b,c))
#define wxPyConvertSwigPtr(a,b,c)               (wxPyGetCoreAPIPtr()->p_wxPyConvertSwigPtr(a,b,c))
#define wxPyMakeSwigPtr(a,b)                    (wxPyGetCoreAPIPtr()->p_wxPyMakeSwigPtr(a,b))

#define wxPyBeginAllowThreads()                 (wxPyGetCoreAPIPtr()->p_wxPyBeginAllowThreads())
#define wxPyEndAllowThreads(a)                  (wxPyGetCoreAPIPtr()->p_wxPyEndAllowThreads(a))
#define wxPyBeginBlockThreads()                 (wxPyGetCoreAPIPtr()->p_wxPyBeginBlockThreads())
#define wxPyEndBlockThreads(a)                  (wxPyGetCoreAPIPtr()->p_wxPyEndBlockThreads(a))

#define wxPyConstructObject(a,b,c)              (wxPyGetCoreAPIPtr()->p_wxPyConstructObject(a,b,c))
#define wxPy_ConvertList(a)                     (wxPyGetCoreAPIPtr()->p_wxPy_ConvertList(a))
                                         
#define wxString_in_helper(a)                   (wxPyGetCoreAPIPtr()->p_wxString_in_helper(a))
#define Py2wxString(a)                          (wxPyGetCoreAPIPtr()->p_Py2wxString(a))
#define wx2PyString(a)                          (wxPyGetCoreAPIPtr()->p_wx2PyString(a))
                                         
#define byte_LIST_helper(a)                     (wxPyGetCoreAPIPtr()->p_byte_LIST_helper(a))
#define int_LIST_helper(a)                      (wxPyGetCoreAPIPtr()->p_int_LIST_helper(a))
#define long_LIST_helper(a)                     (wxPyGetCoreAPIPtr()->p_long_LIST_helper(a))
#define string_LIST_helper(a)                   (wxPyGetCoreAPIPtr()->p_string_LIST_helper(a))
#define wxPoint_LIST_helper(a,b)                (wxPyGetCoreAPIPtr()->p_wxPoint_LIST_helper(a, b))
#define wxBitmap_LIST_helper(a)                 (wxPyGetCoreAPIPtr()->p_wxBitmap_LIST_helper(a))
#define wxString_LIST_helper(a)                 (wxPyGetCoreAPIPtr()->p_wxString_LIST_helper(a))
#define wxAcceleratorEntry_LIST_helper(a)       (wxPyGetCoreAPIPtr()->p_wxAcceleratorEntry_LIST_helper(a))
                                                
#define wxSize_helper(a,b)                      (wxPyGetCoreAPIPtr()->p_wxSize_helper(a,b))
#define wxPoint_helper(a,b)                     (wxPyGetCoreAPIPtr()->p_wxPoint_helper(a,b))
#define wxRealPoint_helper(a,b)                 (wxPyGetCoreAPIPtr()->p_wxRealPoint_helper(a,b))
#define wxRect_helper(a,b)                      (wxPyGetCoreAPIPtr()->p_wxRect_helper(a,b))
#define wxColour_helper(a,b)                    (wxPyGetCoreAPIPtr()->p_wxColour_helper(a,b))
#define wxPoint2D_helper(a,b)                   (wxPyGetCoreAPIPtr()->p_wxPoint2D_helper(a,b))

#define wxPySimple_typecheck(a,b,c)             (wxPyGetCoreAPIPtr()->p_wxPySimple_typecheck(a,b,c))
#define wxColour_typecheck(a)                   (wxPyGetCoreAPIPtr()->p_wxColour_typecheck(a))

#define wxPyCBH_setCallbackInfo(a, b, c, d)     (wxPyGetCoreAPIPtr()->p_wxPyCBH_setCallbackInfo(a,b,c,d))
#define wxPyCBH_findCallback(a, b)              (wxPyGetCoreAPIPtr()->p_wxPyCBH_findCallback(a, b, true))
#define wxPyCBH_findCallback2(a, b, c)          (wxPyGetCoreAPIPtr()->p_wxPyCBH_findCallback(a, b, c))
#define wxPyCBH_callCallback(a, b)              (wxPyGetCoreAPIPtr()->p_wxPyCBH_callCallback(a, b))
#define wxPyCBH_callCallbackObj(a, b)           (wxPyGetCoreAPIPtr()->p_wxPyCBH_callCallbackObj(a, b))
#define wxPyCBH_delete(a)                       (wxPyGetCoreAPIPtr()->p_wxPyCBH_delete(a))

#define wxPyMake_wxObject(a,b)                  (wxPyGetCoreAPIPtr()->p_wxPyMake_wxObject(a,b,true))
#define wxPyMake_wxObject2(a,b,c)               (wxPyGetCoreAPIPtr()->p_wxPyMake_wxObject(a,b,c))
#define wxPyMake_wxSizer(a,b)                   (wxPyGetCoreAPIPtr()->p_wxPyMake_wxSizer(a,b))
#define wxPyPtrTypeMap_Add(a, b)                (wxPyGetCoreAPIPtr()->p_wxPyPtrTypeMap_Add(a, b))
#define wxPy2int_seq_helper(a, b, c)            (wxPyGetCoreAPIPtr()->p_wxPy2int_seq_helper(a, b, c))
#define wxPy4int_seq_helper(a, b, c, d, e)      (wxPyGetCoreAPIPtr()->p_wxPy4int_seq_helper(a, b, c, d, e))
#define wxArrayString2PyList_helper(a)          (wxPyGetCoreAPIPtr()->p_wxArrayString2PyList_helper(a))
#define wxArrayInt2PyList_helper(a)             (wxPyGetCoreAPIPtr()->p_wxArrayInt2PyList_helper(a))
                                                
#define wxPyClientData_dtor(a)                  (wxPyGetCoreAPIPtr()->p_wxPyClientData_dtor(a))
#define wxPyUserData_dtor(a)                    (wxPyGetCoreAPIPtr()->p_wxPyUserData_dtor(a))
#define wxPyOORClientData_dtor(a)               (wxPyGetCoreAPIPtr()->p_wxPyOORClientData_dtor(a))
                                                
#define wxPyCBInputStream_create(a, b)          (wxPyGetCoreAPIPtr()->p_wxPyCBInputStream_create(a, b))
#define wxPyCBInputStream_copy(a)               (wxPyGetCoreAPIPtr()->p_wxPyCBInputStream_copy(a))

#define wxPyInstance_Check(a)                   (wxPyGetCoreAPIPtr()->p_wxPyInstance_Check(a))
#define wxPySwigInstance_Check(a)               (wxPyGetCoreAPIPtr()->p_wxPySwigInstance_Check(a))

#define wxPyCheckForApp()                       (wxPyGetCoreAPIPtr()->p_wxPyCheckForApp())

#define wxArrayDouble2PyList_helper(a)          (wxPyGetCoreAPIPtr()->p_wxArrayDoublePyList_helper(a))
#define wxPoint2D_LIST_helper(a,b)              (wxPyGetCoreAPIPtr()->p_wxPoint2D_LIST_helper(a, b))
#define wxRect2D_helper(a,b)                    (wxPyGetCoreAPIPtr()->p_wxRect2D_helper(a,b))
#define wxPosition_helper(a,b)                  (wxPyGetCoreAPIPtr()->p_wxPosition_helper(a,b))


//----------------------------------------------------------------------
#endif
