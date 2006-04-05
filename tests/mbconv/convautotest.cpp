///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/convauto.cpp
// Purpose:     wxConvAuto unit test
// Author:      Vadim Zeitlin
// Created:     2006-04-04
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_WCHAR_T

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/convauto.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ConvAutoTestCase : public CppUnit::TestCase
{
public:
    ConvAutoTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ConvAutoTestCase );
        CPPUNIT_TEST( Empty );
        CPPUNIT_TEST( Short );
        CPPUNIT_TEST( None );
        CPPUNIT_TEST( UTF32LE );
        CPPUNIT_TEST( UTF32BE );
        CPPUNIT_TEST( UTF16LE );
        CPPUNIT_TEST( UTF16BE );
        CPPUNIT_TEST( UTF8 );
    CPPUNIT_TEST_SUITE_END();

    // real test function: check that converting the src multibyte string to
    // wide char using wxConvAuto yields wch as the first result
    void TestFirstChar(const char *src, wchar_t wch);

    void Empty();
    void Short();
    void None();
    void UTF32LE();
    void UTF32BE();
    void UTF16LE();
    void UTF16BE();
    void UTF8();
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(ConvAutoTestCase);

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ConvAutoTestCase, "ConvAuto");

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

void ConvAutoTestCase::TestFirstChar(const char *src, wchar_t wch)
{
    wxWCharBuffer wbuf = wxConvAuto().cMB2WC(src);
    CPPUNIT_ASSERT( wbuf );
    CPPUNIT_ASSERT_EQUAL( wch, *wbuf );
}

void ConvAutoTestCase::Empty()
{
    TestFirstChar("", _T('\0'));
}

void ConvAutoTestCase::Short()
{
    TestFirstChar("1", _T('1'));
}

void ConvAutoTestCase::None()
{
    TestFirstChar("Hello world", _T('H'));
}

void ConvAutoTestCase::UTF32LE()
{
    TestFirstChar("\xff\xfe\0\0A\0\0\0", _T('A'));
}

void ConvAutoTestCase::UTF32BE()
{
    TestFirstChar("\0\0\xfe\xff\0\0\0B", _T('B'));
}

void ConvAutoTestCase::UTF16LE()
{
    TestFirstChar("\xff\xfeZ\0", _T('Z'));
}

void ConvAutoTestCase::UTF16BE()
{
    TestFirstChar("\xfe\xff\0Y", _T('Y'));
}

void ConvAutoTestCase::UTF8()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\xef\xbb\xbf\xd0\x9f", L'\u041f');
#endif
}

#endif // wxUSE_WCHAR_T

