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
#define wxUSE_PROPSHEET    1

// wxTreeLayout class
#define wxUSE_TREELAYOUT     1

// use wxExpr (a.k.a. PrologIO)
#define wxUSE_PROLOGIO          1

// Use .wxr resource mechanism (requires PrologIO library)
#ifdef wxUSE_WX_RESOURCES
#undef wxUSE_WX_RESOURCES
#endif
#define wxUSE_WX_RESOURCES      1

#endif
    // _WX_DEPRECATED_SETUP_H_
