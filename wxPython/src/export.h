////////////////////////////////////////////////////////////////////////////
// Name:        export.h
// Purpose:     To be used from modules that are not part of the core
//              wxPython extension in order to get access to some helper
//              functions that live in wxc.
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

#define wxPyUSE_EXPORT
#include "helpers.h"


static void wxPyCoreAPI_IMPORT() {
    wxPyCoreAPIPtr = (wxPyCoreAPI*)PyCObject_Import("wxPython.wxc", "wxPyCoreAPI");
    if (! wxPyCoreAPIPtr)
        wxPyCoreAPIPtr = (wxPyCoreAPI*)PyCObject_Import("wxc", "wxPyCoreAPI");
}

#define SWIG_MakePtr(a, b, c)           (wxPyCoreAPIPtr->p_SWIG_MakePtr(a, b, c))
#define SWIG_GetPtr(a, b, c)            (wxPyCoreAPIPtr->p_SWIG_GetPtr(a, b, c))
#define SWIG_GetPtrObj(a, b, c)         (wxPyCoreAPIPtr->p_SWIG_GetPtrObj(a, b, c))
#define SWIG_RegisterMapping(a, b, c)   (wxPyCoreAPIPtr->p_SWIG_RegisterMapping(a, b, c))
#define SWIG_addvarlink(a, b, c, d)     (wxPyCoreAPIPtr->p_SWIG_addvarlink(a, b, c, d))

#define wxPyBeginAllowThreads()         (wxPyCoreAPIPtr->p_wxPyBeginAllowThreads())
#define wxPyEndAllowThreads(a)          (wxPyCoreAPIPtr->p_wxPyEndAllowThreads(a))
#define wxPyBeginBlockThreads()         (wxPyCoreAPIPtr->p_wxPyBeginBlockThreads())
#define wxPyEndBlockThreads()           (wxPyCoreAPIPtr->p_wxPyEndBlockThreads())

#define wxPyConstructObject(a,b,c)      (wxPyCoreAPIPtr->p_wxPyConstructObject(a,b,c))
#define wxPy_ConvertList(a,b)           (wxPyCoreAPIPtr->p_wxPy_ConvertList(a,b))
#define byte_LIST_helper(a)             (wxPyCoreAPIPtr->p_byte_LIST_helper(a))
#define int_LIST_helper(a)              (wxPyCoreAPIPtr->p_int_LIST_helper(a))
#define long_LIST_helper(a)             (wxPyCoreAPIPtr->p_long_LIST_helper(a))
#define string_LIST_helper(a)           (wxPyCoreAPIPtr->p_string_LIST_helper(a))
#define wxPoint_LIST_helper(a,b)        (wxPyCoreAPIPtr->p_wxPoint_LIST_helper(a, b))
#define wxBitmap_LIST_helper(a)         (wxPyCoreAPIPtr->p_wxBitmap_LIST_helper(a))
#define wxString_LIST_helper(a)         (wxPyCoreAPIPtr->p_wxString_LIST_helper(a))
#define wxAcceleratorEntry_LIST_helper(a) (wxPyCoreAPIPtr->p_wxAcceleratorEntry_LIST_helper(a))

#define wxSize_helper(a,b)              (wxPyCoreAPIPtr->p_wxSize_helper(a,b))
#define wxPoint_helper(a,b)             (wxPyCoreAPIPtr->p_wxPoint_helper(a,b))
#define wxRealPoint_helper(a,b)         (wxPyCoreAPIPtr->p_wxRealPoint_helper(a,b))
#define wxRect_helper(a,b)              (wxPyCoreAPIPtr->p_wxRect_helper(a,b))
#define wxColour_helper(a,b)            (wxPyCoreAPIPtr->p_wxColour_helper(a,b))

#define wxPyCBH_setCallbackInfo(a, b, c, d)     (wxPyCoreAPIPtr->p_wxPyCBH_setCallbackInfo(a,b,c,d))
#define wxPyCBH_findCallback(a, b)      (wxPyCoreAPIPtr->p_wxPyCBH_findCallback(a, b))
#define wxPyCBH_callCallback(a, b)      (wxPyCoreAPIPtr->p_wxPyCBH_callCallback(a, b))
#define wxPyCBH_callCallbackObj(a, b)   (wxPyCoreAPIPtr->p_wxPyCBH_callCallbackObj(a, b))
#define wxPyCBH_delete(a)               (wxPyCoreAPIPtr->p_wxPyCBH_delete(a))

#define wxPyClassExists(a)              (wxPyCoreAPIPtr->p_wxPyClassExists(a))
#define wxPyMake_wxObject(a)            (wxPyCoreAPIPtr->p_wxPyMake_wxObject(a,TRUE))
#define wxPyMake_wxObject2(a,b)         (wxPyCoreAPIPtr->p_wxPyMake_wxObject(a,b))
#define wxPyMake_wxSizer(a)             (wxPyCoreAPIPtr->p_wxPyMake_wxSizer(a))
#define wxPyPtrTypeMap_Add(a, b)        (wxPyCoreAPIPtr->p_wxPyPtrTypeMap_Add(a, b))
#define wxArrayString2PyList_helper(a)  (wxPyCoreAPIPtr->p_wxArrayString2PyList_helper(a))



// This one is special.  It's the first function called in SWIG generated
// modules, so we'll use it to also import the API.
#define SWIG_newvarlink()               (wxPyCoreAPI_IMPORT(), wxPyCoreAPIPtr->p_SWIG_newvarlink())


//----------------------------------------------------------------------
#endif
