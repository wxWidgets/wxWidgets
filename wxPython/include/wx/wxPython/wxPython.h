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

// This needs to be called in modules that make calls to any of the functions
// exported by the wxPython API.  It sets a static pointer to a structure of
// function pointers located in the wx._core extension module.
static bool wxPyCoreAPI_IMPORT() {
    wxPyCoreAPIPtr = (wxPyCoreAPI*)PyCObject_Import("wx._core_", "_wxPyCoreAPI");
    return wxPyCoreAPIPtr != NULL;
}



// The following macros call functions located in wx._core_ of the same name
// via the API pointer retrieved above.


#define SWIG_Python_TypeRegister(a)             (wxPyCoreAPIPtr->p_SWIG_Python_TypeRegister(a))
#define SWIG_Python_TypeCheck(a,b)              (wxPyCoreAPIPtr->p_SWIG_Python_TypeCheck(a,b))
#define SWIG_Python_TypeCast(a,b)               (wxPyCoreAPIPtr->p_SWIG_Python_TypeCast(a,b))
#define SWIG_Python_TypeDynamicCast(a,b)        (wxPyCoreAPIPtr->p_SWIG_Python_TypeDynamicCast(a,b))
#define SWIG_Python_TypeName(a)                 (wxPyCoreAPIPtr->p_SWIG_Python_TypeName(a))
#define SWIG_Python_TypeQuery(a)                (wxPyCoreAPIPtr->p_SWIG_Python_TypeQuery(a))
#define SWIG_Python_TypeClientData(a,b)         (wxPyCoreAPIPtr->p_SWIG_Python_TypeClientData(a,b))
#define SWIG_Python_addvarlink(a,b,c,d)         (wxPyCoreAPIPtr->p_SWIG_Python_addvarlink(a,b,c,d))
#define SWIG_Python_ConvertPtr(a,b,c,d)         (wxPyCoreAPIPtr->p_SWIG_Python_ConvertPtr(a,b,c,d))
#define SWIG_Python_ConvertPacked(a,b,c,d,e)    (wxPyCoreAPIPtr->p_SWIG_Python_ConvertPacked(a,b,c,d,e))
#define SWIG_Python_PackData(a,b,c)             (wxPyCoreAPIPtr->p_SWIG_Python_PackData(a,b,c))
#define SWIG_Python_UnpackData(a,b,c)           (wxPyCoreAPIPtr->p_SWIG_Python_UnpackData(a,b,c))
#define SWIG_Python_NewPointerObj(a,b,c)        (wxPyCoreAPIPtr->p_SWIG_Python_NewPointerObj(a,b,c))
#define SWIG_Python_NewPackedObj(a,b,c)         (wxPyCoreAPIPtr->p_SWIG_Python_NewPackedObj(a,b,c))
#define SWIG_Python_InstallConstants(a,b)       (wxPyCoreAPIPtr->p_SWIG_Python_InstallConstants(a,b))
#define SWIG_Python_MustGetPtr(a,b,c,d)         (wxPyCoreAPIPtr->p_SWIG_Python_MustGetPtr(a,b,c,d))

#define wxPyCheckSwigType(a)                    (wxPyCoreAPIPtr->p_wxPyCheckSwigType(a))
#define wxPyConstructObject(a,b,c)              (wxPyCoreAPIPtr->p_wxPyConstructObject(a,b,c))
#define wxPyConvertSwigPtr(a,b,c)               (wxPyCoreAPIPtr->p_wxPyConvertSwigPtr(a,b,c))
#define wxPyMakeSwigPtr(a,b)                    (wxPyCoreAPIPtr->p_wxPyMakeSwigPtr(a,b))

#define wxPyBeginAllowThreads()                 (wxPyCoreAPIPtr->p_wxPyBeginAllowThreads())
#define wxPyEndAllowThreads(a)                  (wxPyCoreAPIPtr->p_wxPyEndAllowThreads(a))
#define wxPyBeginBlockThreads()                 (wxPyCoreAPIPtr->p_wxPyBeginBlockThreads())
#define wxPyEndBlockThreads(a)                  (wxPyCoreAPIPtr->p_wxPyEndBlockThreads(a))

#define wxPyConstructObject(a,b,c)              (wxPyCoreAPIPtr->p_wxPyConstructObject(a,b,c))
#define wxPy_ConvertList(a)                     (wxPyCoreAPIPtr->p_wxPy_ConvertList(a))
                                         
#define wxString_in_helper(a)                   (wxPyCoreAPIPtr->p_wxString_in_helper(a))
#define Py2wxString(a)                          (wxPyCoreAPIPtr->p_Py2wxString(a))
#define wx2PyString(a)                          (wxPyCoreAPIPtr->p_wx2PyString(a))
                                         
#define byte_LIST_helper(a)                     (wxPyCoreAPIPtr->p_byte_LIST_helper(a))
#define int_LIST_helper(a)                      (wxPyCoreAPIPtr->p_int_LIST_helper(a))
#define long_LIST_helper(a)                     (wxPyCoreAPIPtr->p_long_LIST_helper(a))
#define string_LIST_helper(a)                   (wxPyCoreAPIPtr->p_string_LIST_helper(a))
#define wxPoint_LIST_helper(a,b)                (wxPyCoreAPIPtr->p_wxPoint_LIST_helper(a, b))
#define wxBitmap_LIST_helper(a)                 (wxPyCoreAPIPtr->p_wxBitmap_LIST_helper(a))
#define wxString_LIST_helper(a)                 (wxPyCoreAPIPtr->p_wxString_LIST_helper(a))
#define wxAcceleratorEntry_LIST_helper(a)       (wxPyCoreAPIPtr->p_wxAcceleratorEntry_LIST_helper(a))
                                                
#define wxSize_helper(a,b)                      (wxPyCoreAPIPtr->p_wxSize_helper(a,b))
#define wxPoint_helper(a,b)                     (wxPyCoreAPIPtr->p_wxPoint_helper(a,b))
#define wxRealPoint_helper(a,b)                 (wxPyCoreAPIPtr->p_wxRealPoint_helper(a,b))
#define wxRect_helper(a,b)                      (wxPyCoreAPIPtr->p_wxRect_helper(a,b))
#define wxColour_helper(a,b)                    (wxPyCoreAPIPtr->p_wxColour_helper(a,b))
#define wxPoint2D_helper(a,b)                   (wxPyCoreAPIPtr->p_wxPoint2D_helper(a,b))

#define wxPySimple_typecheck(a,b,c)             (wxPyCoreAPIPtr->p_wxPySimple_typecheck(a,b,c))
#define wxColour_typecheck(a)                   (wxPyCoreAPIPtr->p_wxColour_typecheck(a))

#define wxPyCBH_setCallbackInfo(a, b, c, d)     (wxPyCoreAPIPtr->p_wxPyCBH_setCallbackInfo(a,b,c,d))
#define wxPyCBH_findCallback(a, b)              (wxPyCoreAPIPtr->p_wxPyCBH_findCallback(a, b))
#define wxPyCBH_callCallback(a, b)              (wxPyCoreAPIPtr->p_wxPyCBH_callCallback(a, b))
#define wxPyCBH_callCallbackObj(a, b)           (wxPyCoreAPIPtr->p_wxPyCBH_callCallbackObj(a, b))
#define wxPyCBH_delete(a)                       (wxPyCoreAPIPtr->p_wxPyCBH_delete(a))

#define wxPyMake_wxObject(a)                    (wxPyCoreAPIPtr->p_wxPyMake_wxObject(a,True))
#define wxPyMake_wxObject2(a,b)                 (wxPyCoreAPIPtr->p_wxPyMake_wxObject(a,b))
#define wxPyMake_wxSizer(a)                     (wxPyCoreAPIPtr->p_wxPyMake_wxSizer(a))
#define wxPyPtrTypeMap_Add(a, b)                (wxPyCoreAPIPtr->p_wxPyPtrTypeMap_Add(a, b))
#define wxPy2int_seq_helper(a, b, c)            (wxPyCoreAPIPtr->p_wxPy2int_seq_helper(a, b, c))
#define wxPy4int_seq_helper(a, b, c, d, e)      (wxPyCoreAPIPtr->p_wxPy4int_seq_helper(a, b, c, d, e))
#define wxArrayString2PyList_helper(a)          (wxPyCoreAPIPtr->p_wxArrayString2PyList_helper(a))
#define wxArrayInt2PyList_helper(a)             (wxPyCoreAPIPtr->p_wxArrayInt2PyList_helper(a))
                                                
#define wxPyClientData_dtor(a)                  (wxPyCoreAPIPtr->p_wxPyClientData_dtor(a))
#define wxPyUserData_dtor(a)                    (wxPyCoreAPIPtr->p_wxPyUserData_dtor(a))
#define wxPyOORClientData_dtor(a)               (wxPyCoreAPIPtr->p_wxPyOORClientData_dtor(a))
                                                
#define wxPyCBInputStream_create(a, b)          (wxPyCoreAPIPtr->p_wxPyCBInputStream_create(a, b))

#define wxPyInstance_Check(a)                   (wxPyCoreAPIPtr->p_wxPyInstance_Check(a))
#define wxPySwigInstance_Check(a)               (wxPyCoreAPIPtr->p_wxPySwigInstance_Check(a))


// This one is special.  It's the first function called in SWIG generated
// modules, so we'll use it to also import the API.
#define SWIG_Python_newvarlink()     (wxPyCoreAPI_IMPORT(), wxPyCoreAPIPtr->p_SWIG_Python_newvarlink())


//----------------------------------------------------------------------
#endif
