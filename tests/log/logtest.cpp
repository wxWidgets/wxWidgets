///////////////////////////////////////////////////////////////////////////////
// Name:        tests/log/logtest.cpp
// Purpose:     wxLog unit test
// Author:      Vadim Zeitlin
// Created:     2009-07-07
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/filefn.h"
#endif // WX_PRECOMP

#include "wx/scopeguard.h"

#if wxUSE_LOG

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
#else
    #include <errno.h>
#endif

// all calls to wxLogXXX() functions from this file will use this log component
#define wxLOG_COMPONENT "test"

#include "testlog.h"

TEST_CASE_METHOD(LogTestCase, "wxLog::Functions", "[log]")
{
    wxLogMessage("Message");
    CHECK( m_log->GetLog(wxLOG_Message) == "Message" );

    wxLogError("Error %d", 17);
    CHECK( m_log->GetLog(wxLOG_Error) == "Error 17" );

    // Logging a string containing percent signs should work.
    wxLogMessage("100%sure");
    CHECK( m_log->GetLog(wxLOG_Message) == "100%sure" );

    wxLogDebug("Debug");
#if wxDEBUG_LEVEL
    CHECK( m_log->GetLog(wxLOG_Debug) == "Debug" );
#else
    CHECK( m_log->GetLog(wxLOG_Debug) == "" );
#endif
}

TEST_CASE_METHOD(LogTestCase, "wxLogNull()", "[log]")
{
    {
        wxLogNull noLog;
        wxLogWarning("%s warning", "Not important");

        CHECK( m_log->GetLog(wxLOG_Warning) == "" );
    }

    wxLogWarning("%s warning", "Important");
    CHECK( m_log->GetLog(wxLOG_Warning) == "Important warning" );
}

TEST_CASE_METHOD(LogTestCase, "wxLog::Component", "[log]")
{
    wxLogMessage("Message");
    CPPUNIT_ASSERT_EQUAL( std::string(wxLOG_COMPONENT),
                          m_log->GetInfo(wxLOG_Message).component );

    // completely disable logging for this component
    wxLog::SetComponentLevel("test/ignore", wxLOG_FatalError);

    // but enable it for one of its subcomponents
    wxLog::SetComponentLevel("test/ignore/not", wxLOG_Max);

    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore"

    // this shouldn't be output as this component is ignored
    wxLogError("Error");
    CHECK( m_log->GetLog(wxLOG_Error) == "" );

    // and so are its subcomponents
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore/sub/subsub"
    wxLogError("Error");
    CHECK( m_log->GetLog(wxLOG_Error) == "" );

    // but one subcomponent is not
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore/not"
    wxLogError("Error");
    CHECK( m_log->GetLog(wxLOG_Error) == "Error" );

    // restore the original value
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test"
}

#if wxDEBUG_LEVEL

namespace
{

const char *TEST_MASK = "test";

// this is a test vararg function (a real one, not a variadic-template-like as
// wxVLogTrace(), so care should be taken with its arguments)
void TraceTest(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(TEST_MASK, format, argptr);
    va_end(argptr);
}

} // anonymous namespace

TEST_CASE_METHOD(LogTestCase, "wxLog::Trace", "[log]")
{
    // we use wxLogTrace() or wxVLogTrace() from inside TraceTest()
    // interchangeably here, it shouldn't make any difference

    wxLogTrace(TEST_MASK, "Not shown");
    CHECK( m_log->GetLog(wxLOG_Trace) == "" );

    wxLog::AddTraceMask(TEST_MASK);
    TraceTest("Shown");
    CPPUNIT_ASSERT_EQUAL( wxString::Format("(%s) Shown", TEST_MASK),
                          m_log->GetLog(wxLOG_Trace) );

    wxLog::RemoveTraceMask(TEST_MASK);
    m_log->Clear();

    TraceTest("Not shown again");
    CHECK( m_log->GetLog(wxLOG_Trace) == "" );
}

#endif // wxDEBUG_LEVEL

TEST_CASE_METHOD(LogTestCase, "wxLogSysError", "[log]")
{
    wxString s;

    wxLogSysError(17, "Error");
    CHECK( m_log->GetLog(wxLOG_Error).StartsWith("Error (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 17") );

    // Try to ensure that the system error is 0.
#ifdef __WINDOWS__
    ::SetLastError(0);
#else
    errno = 0;
#endif

    wxLogSysError("Success");
    CHECK( m_log->GetLog(wxLOG_Error).StartsWith("Success (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 0") );

    wxOpen("no-such-file", 0, 0);
    wxLogSysError("Not found");
    CHECK( m_log->GetLog(wxLOG_Error).StartsWith("Not found (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 2") );
}

TEST_CASE_METHOD(LogTestCase, "wxLog::NoWarnings", "[log]")
{
    // Check that "else" branch is [not] taken as expected and that this code
    // compiles without warnings (which used to not be the case).

    bool b = wxFalse;
    if ( b )
        wxLogError("Not logged");
    else
        b = !b;

    CHECK( b );

    if ( b )
        wxLogError("If");
    else
        FAIL_CHECK("Should not be taken");

    CHECK( m_log->GetLog(wxLOG_Error) == "If" );
}

// The following two functions (v, macroCompilabilityTest) are not run by
// any test, and their purpose is merely to guarantee that the wx(V)LogXXX
// macros compile without 'dangling else' warnings.
#if defined(__clang__) || defined(__GNUC__)
    // gcc 7 split -Wdangling-else from the much older -Wparentheses, so use
    // the new warning if it's available or the old one otherwise.
    #if wxCHECK_GCC_VERSION(7, 0)
        #pragma GCC diagnostic error "-Wdangling-else"
    #else
        #pragma GCC diagnostic error "-Wparentheses"
    #endif
#endif

static void v(int x, ...)
{
    va_list list;
    va_start(list, x);

    if (true)
        wxVLogGeneric(Info, "vhello generic %d", list);
    if (true)
        wxVLogTrace(wxTRACE_Messages, "vhello trace %d", list);
    if (true)
        wxVLogError("vhello error %d", list);
    if (true)
        wxVLogMessage("vhello message %d", list);
    if (true)
        wxVLogVerbose("vhello verbose %d", list);
    if (true)
        wxVLogWarning("vhello warning %d", list);
    if (true)
        wxVLogFatalError("vhello fatal %d", list);
    if (true)
        wxVLogSysError("vhello syserror %d", list);
    if (true)
        wxVLogDebug("vhello debug %d", list);

    va_end(list);
}

void macroCompilabilityTest()
{
    v(123, 456);
    if (true)
        wxLogGeneric(wxLOG_Info, "hello generic %d", 42);
    if (true)
        wxLogTrace(wxTRACE_Messages, "hello trace %d", 42);
    if (true)
        wxLogError("hello error %d", 42);
    if (true)
        wxLogMessage("hello message %d", 42);
    if (true)
        wxLogVerbose("hello verbose %d", 42);
    if (true)
        wxLogWarning("hello warning %d", 42);
    if (true)
        wxLogFatalError("hello fatal %d", 42);
    if (true)
        wxLogSysError("hello syserror %d", 42);
    if (true)
        wxLogDebug("hello debug %d", 42);
}

// This allows to check wxLogTrace() interactively by running this test with
// WXTRACE=logtest.
TEST_CASE("wxLog::WXTRACE", "[log][.]")
{
    // Running this test without setting WXTRACE is useless.
    REQUIRE( wxGetEnv("WXTRACE", nullptr) );

    wxLogTrace("logtest", "Starting test");
    wxMilliSleep(250);
    wxLogTrace("logtest", "Ending test 1/4s later");
}

#endif // wxUSE_LOG
