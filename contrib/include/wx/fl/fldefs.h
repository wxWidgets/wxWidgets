/////////////////////////////////////////////////////////////////////////////
// Name:        fldefs.h
// Purpose:     Declaration of global types and defines.
// Author:      David M. Falkinder (david_falkinder@hp.com)
// Modified by:
// Created:     18/09/2002
// RCS-ID:      $Id$
// Copyright:   (c) David M. Falkinder
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __fldefs_H_INCLUDED__
#define __fldefs_H_INCLUDED__

#include "wx/defs.h"

/*
 * If we're using wx in Dynamic Library format do we 
 * want FL to be in DLL form as well?
 */
#if defined(WXUSINGDLL) && \
    (defined(WXMAKING_FL_DLL) || defined(WXUSING_FL_DLL))

#if defined(WXMAKING_FL_DLL)
    // When building the DLL WXFLDECLSPEC exports classes
#   define WXFL_DECLSPEC            WXEXPORT
#elif defined(WXUSING_FL_DLL)
    // When building the DLL WXFLDECLSPEC imports classes
#   define WXFL_DECLSPEC            WXIMPORT
#endif // defined(WXBUILD_FL_DLL)

#else
// When building the static library nullify the effect of WXFL_DECLSPEC
#define WXFL_DECLSPEC
#endif // WXUSINGDLL && (WXMAKING_FL_DLL || WXUSING_FL_DLL)

///////////////////////////////////////////////////////////////////////////////
// Override some of the wxArray functions to
// include our definitions
///////////////////////////////////////////////////////////////////////////////
#define WXFL_DEFINE_ARRAY(c,l)      \
    class WXFL_DECLSPEC l;          \
    WX_DEFINE_ARRAY(c,l)

#define WXFL_DEFINE_ARRAY_LONG(t,l) \
    class WXFL_DECLSPEC l;          \
    WX_DEFINE_ARRAY_LONG(t,l)


#endif // __fldefs_H_INCLUDED__
