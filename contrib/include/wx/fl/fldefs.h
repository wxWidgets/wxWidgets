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
#ifdef WXMAKINGDLL_FL
    #define WXDLLIMPEXP_FL WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_FL WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_FL
#endif

///////////////////////////////////////////////////////////////////////////////
// Override some of the wxArray functions to
// include our definitions
///////////////////////////////////////////////////////////////////////////////
#define WXFL_DEFINE_ARRAY(c,l)      \
    class WXDLLIMPEXP_FL l;          \
    WX_DEFINE_ARRAY(c,l)

#define WXFL_DEFINE_ARRAY_PTR(c,l)\
    class WXDLLIMPEXP_FL l;          \
    WX_DEFINE_ARRAY_PTR(c,l)

#define WXFL_DEFINE_ARRAY_LONG(t,l) \
    class WXDLLIMPEXP_FL l;          \
    WX_DEFINE_ARRAY_LONG(t,l)


#endif // __fldefs_H_INCLUDED__
