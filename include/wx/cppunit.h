/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cppunit.h
// Purpose:     wrapper header for CppUnit headers
// Author:      Vadim Zeitlin
// Created:     15.02.04
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CPPUNIT_H_
#define _WX_CPPUNIT_H_

// using CPPUNIT_TEST() macro results in this warning, disable it as there is
// no other way to get rid of it and it's not very useful anyhow
#ifdef __VISUALC__
    // typedef-name 'foo' used as synonym for class-name 'bar'
    #pragma warning(disable:4097)

    // unreachable code: we don't care about warnings in CppUnit headers
    #pragma warning(disable:4702)

    // 'id': identifier was truncated to 'num' characters in the debug info
    #pragma warning(disable:4786)
#endif // __VISUALC__

#ifdef __BORLANDC__
    #pragma warn -8022
#endif

#include "wx/beforestd.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "wx/afterstd.h"

#ifdef __BORLANDC__
    #pragma warn .8022
#endif

#ifdef _MSC_VER
  #pragma warning(default:4702)
#endif // _MSC_VER

// for VC++ automatically link in cppunit library
#ifdef __VISUALC__
  #ifdef NDEBUG
    #pragma comment(lib, "cppunit.lib")
  #else // Debug
    #pragma comment(lib, "cppunitd.lib")
  #endif // Release/Debug
#endif

#endif // _WX_CPPUNIT_H_

