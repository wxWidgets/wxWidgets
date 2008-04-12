///////////////////////////////////////////////////////////////////////////////
// Name:        tests/cmdline/cmdlinetest.cpp
// Purpose:     wxCmdLineParser unit test
// Author:      Vadim Zeitlin
// Created:     2008-04-12
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/cmdline.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class CmdLineTestCase : public CppUnit::TestCase
{
public:
    CmdLineTestCase() {}

private:
    CPPUNIT_TEST_SUITE( CmdLineTestCase );
        CPPUNIT_TEST( ConvertStringTestCase );
    CPPUNIT_TEST_SUITE_END();

    void ConvertStringTestCase();

    DECLARE_NO_COPY_CLASS(CmdLineTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CmdLineTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CmdLineTestCase, "CmdLineTestCase" );

// ============================================================================
// implementation
// ============================================================================

void CmdLineTestCase::ConvertStringTestCase()
{
    #define WX_ASSERT_ARGS_EQUAL(s, args)                                     \
        {                                                                     \
            const wxArrayString a(wxCmdLineParser::ConvertStringToArgs(args));\
            WX_ASSERT_STRARRAY_EQUAL(s, a);                                   \
        }

    // normal cases
    WX_ASSERT_ARGS_EQUAL( "foo", "foo" )
    WX_ASSERT_ARGS_EQUAL( "foo bar", "\"foo bar\"" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar", "foo bar" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar|baz", "foo bar baz" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar baz", "foo \"bar baz\"" )

    // special cases
    WX_ASSERT_ARGS_EQUAL( "", "" )
    WX_ASSERT_ARGS_EQUAL( "foo", "foo " )
    WX_ASSERT_ARGS_EQUAL( "foo", "foo \t   " )
    WX_ASSERT_ARGS_EQUAL( "foo|bar", "foo bar " )
    WX_ASSERT_ARGS_EQUAL( "foo|bar|", "foo bar \"" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar|\\", "foo bar \\" )

    // check for (broken) Windows semantics: backslash doesn't escape spaces
    WX_ASSERT_ARGS_EQUAL( "foo|bar\\|baz", "foo bar\\ baz" );
    WX_ASSERT_ARGS_EQUAL( "foo|bar\\\"baz", "foo \"bar\\\"baz\"" );

    #undef WX_ASSERT_ARGS_EQUAL
}
