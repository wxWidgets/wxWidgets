/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cppunit.h
// Purpose:     wrapper header for CppUnit headers
// Author:      Vadim Zeitlin
// Created:     15.02.04
// Copyright:   (c) 2004 Vadim Zeitlin
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CPPUNIT_H_
#define _WX_CPPUNIT_H_

///////////////////////////////////////////////////////////////////////////////
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

#ifndef CPPUNIT_STD_NEED_ALLOCATOR
    #define CPPUNIT_STD_NEED_ALLOCATOR 0
#endif

///////////////////////////////////////////////////////////////////////////////
// Set the default format for the errors, which can be used by an IDE to jump
// to the error location. This default gets overridden by the cppunit headers
// for some compilers (e.g. VC++).

#ifndef CPPUNIT_COMPILER_LOCATION_FORMAT
    #define CPPUNIT_COMPILER_LOCATION_FORMAT "%p:%l:"
#endif


///////////////////////////////////////////////////////////////////////////////
// Include all needed cppunit headers.
//

#include "wx/beforestd.h"
#ifdef __VISUALC__
    #pragma warning(push)

    // with cppunit 1.12 we get many bogus warnings 4701 (local variable may be
    // used without having been initialized) in TestAssert.h
    #pragma warning(disable:4701)

    // and also 4100 (unreferenced formal parameter) in extensions/
    // ExceptionTestCaseDecorator.h
    #pragma warning(disable:4100)
#endif

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#ifdef __VISUALC__
    #pragma warning(pop)
#endif
#include "wx/afterstd.h"

#include "wx/string.h"


///////////////////////////////////////////////////////////////////////////////
// Set of helpful test macros.
//

CPPUNIT_NS_BEGIN

// provide an overload of cppunit assertEquals(T, T) which can be used to
// compare wxStrings directly with C strings
inline void
assertEquals(const char *expected,
             const char *actual,
             CppUnit::SourceLine sourceLine,
             const std::string& message)
{
    assertEquals(wxString(expected), wxString(actual), sourceLine, message);
}

inline void
assertEquals(const char *expected,
             const wxString& actual,
             CppUnit::SourceLine sourceLine,
             const std::string& message)
{
    assertEquals(wxString(expected), actual, sourceLine, message);
}

inline void
assertEquals(const wxString& expected,
             const char *actual,
             CppUnit::SourceLine sourceLine,
             const std::string& message)
{
    assertEquals(expected, wxString(actual), sourceLine, message);
}

inline void
assertEquals(const wchar_t *expected,
             const wxString& actual,
             CppUnit::SourceLine sourceLine,
             const std::string& message)
{
    assertEquals(wxString(expected), actual, sourceLine, message);
}

inline void
assertEquals(const wxString& expected,
             const wchar_t *actual,
             CppUnit::SourceLine sourceLine,
             const std::string& message)
{
    assertEquals(expected, wxString(actual), sourceLine, message);
}

CPPUNIT_NS_END

// define an assertEquals() overload for the given types, this is a helper used
// by WX_CPPUNIT_ALLOW_EQUALS_TO_INT() below
#define WX_CPPUNIT_ASSERT_EQUALS(T1, T2)                                      \
    inline void                                                               \
    assertEquals(T1 expected,                                                 \
                 T2 actual,                                                   \
                 CppUnit::SourceLine sourceLine,                              \
                 const std::string& message)                                  \
    {                                                                         \
        if ( !assertion_traits<T1>::equal(expected,actual) )                  \
        {                                                                     \
            Asserter::failNotEqual( assertion_traits<T1>::toString(expected), \
                                    assertion_traits<T2>::toString(actual),   \
                                    sourceLine,                               \
                                    message );                                \
        }                                                                     \
    }

// this macro allows us to specify (usually literal) ints as expected values
// for functions returning integral types different from "int"
#define WX_CPPUNIT_ALLOW_EQUALS_TO_INT(T) \
    CPPUNIT_NS_BEGIN \
        WX_CPPUNIT_ASSERT_EQUALS(int, T) \
        WX_CPPUNIT_ASSERT_EQUALS(T, int) \
    CPPUNIT_NS_END

WX_CPPUNIT_ALLOW_EQUALS_TO_INT(long)
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(short)
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(unsigned)
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(unsigned long)

#if defined( __VMS ) && defined( __ia64 )
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(std::basic_streambuf<char>::pos_type);
#endif

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(wxLongLong_t)
WX_CPPUNIT_ALLOW_EQUALS_TO_INT(unsigned wxLongLong_t)
#endif // wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

// Use this macro to assert with the given formatted message (it should contain
// the format string and arguments in a separate pair of parentheses)
#define WX_ASSERT_MESSAGE(msg, cond) \
    CPPUNIT_ASSERT_MESSAGE(std::string(wxString::Format msg .mb_str()), (cond))

#define WX_ASSERT_EQUAL_MESSAGE(msg, expected, actual) \
    CPPUNIT_ASSERT_EQUAL_MESSAGE(std::string(wxString::Format msg .mb_str()), \
                                 (expected), (actual))

///////////////////////////////////////////////////////////////////////////////
// define stream inserter for wxString if it's not defined in the main library,
// we need it to output the test failures involving wxString
#if !wxUSE_STD_IOSTREAM

#include "wx/string.h"

#include <iostream>

inline std::ostream& operator<<(std::ostream& o, const wxString& s)
{
#if wxUSE_UNICODE
    return o << (const char *)wxSafeConvertWX2MB(s.wc_str());
#else
    return o << s.c_str();
#endif
}

#endif // !wxUSE_STD_IOSTREAM

///////////////////////////////////////////////////////////////////////////////
// Some more compiler warning tweaking and auto linking.
//

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

