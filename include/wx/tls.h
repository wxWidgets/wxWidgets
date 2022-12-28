///////////////////////////////////////////////////////////////////////////////
// Name:        wx/tls.h
// Purpose:     Implementation of thread local storage (obsolete)
// Author:      Vadim Zeitlin
// Created:     2008-08-08
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TLS_H_
#define _WX_TLS_H_

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// check for compiler support of thread-specific variables
// ----------------------------------------------------------------------------

// when not using threads at all, there is no need for thread-specific
// values to be really thread-specific
#if !wxUSE_THREADS
    #define wxTHREAD_SPECIFIC_DECL
#else
    #define wxTHREAD_SPECIFIC_DECL thread_local
#endif

// ----------------------------------------------------------------------------
// define wxTLS_TYPE()
// ----------------------------------------------------------------------------

// All these preprocessor symbols are obsolete and defined only for
// compatibility: C++11 compilers always support variables with thread storage,
// so you should just always use thread_local directly in the new code instead.
#define wxHAS_COMPILER_TLS

#define wxTLS_TYPE(T) wxTHREAD_SPECIFIC_DECL T
#define wxTLS_TYPE_REF(T) T&
#define wxTLS_PTR(var) (&(var))
#define wxTLS_VALUE(var) (var)

#endif // _WX_TLS_H_

