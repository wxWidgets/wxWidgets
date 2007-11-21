/////////////////////////////////////////////////////////////////////////////
// Name:        wx/deprecated/setup.h
// Purpose:     Configuration for deprecated features of the library
// Author:      Julian Smart
// Modified by:
// Created:     2003-03-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEPRECATED_SETUP_H_
#define _WX_DEPRECATED_SETUP_H_

// wxUSE_TIMEDATE enables compilation of the old wxDate and wxTime classes (not
// the same as wxDateTime!). These classes are obsolete and shouldn't be used
// in new code
//
// Default is 0
//
// Recommended setting: 0 unless you have legacy code which uses these classes
#ifdef wxUSE_TIMEDATE
#undef wxUSE_TIMEDATE
#endif

#define wxUSE_TIMEDATE 0

// wxProperty[Value/Form/List] classes, used by Dialog Editor
#ifdef wxUSE_PROPSHEET
#undef wxUSE_PROPSHEET
#endif
#define wxUSE_PROPSHEET    1

// wxTreeLayout class
#ifdef wxUSE_TREELAYOUT
#undef wxUSE_TREELAYOUT
#endif
#define wxUSE_TREELAYOUT     1

// use wxExpr (a.k.a. PrologIO)
#ifdef wxUSE_PROLOGIO
#undef wxUSE_PROLOGIO
#endif
#define wxUSE_PROLOGIO          1

// Use .wxr resource mechanism (requires PrologIO library)
#ifdef wxUSE_WX_RESOURCES
#undef wxUSE_WX_RESOURCES
#endif
#define wxUSE_WX_RESOURCES      1

// Use simple, generic toolbar
#ifdef wxUSE_TOOLBAR_SIMPLE
#undef wxUSE_TOOLBAR_SIMPLE
#endif
#define wxUSE_TOOLBAR_SIMPLE 1


#ifdef WXMAKINGDLL_DEPRECATED
    #define WXDLLIMPEXP_DEPRECATED WXEXPORT
    #define WXDLLIMPEXP_DATA_DEPRECATED(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_DEPRECATED WXIMPORT
    #define WXDLLIMPEXP_DATA_DEPRECATED(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_DEPRECATED
    #define WXDLLIMPEXP_DATA_DEPRECATED(type) type
#endif

#endif
    // _WX_DEPRECATED_SETUP_H_
