///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xlocale/xlocale.cpp
// Purpose:     wxXLocale & related unit test
// Author:      Brian Vanderburg II, Vadim Zeitlin
// Created:     2008-01-16
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Brian Vanderburg II
//                  2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XLOCALE

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/xlocale.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class XLocaleTestCase : public CppUnit::TestCase
{
public:
    XLocaleTestCase() { }

private:
    CPPUNIT_TEST_SUITE( XLocaleTestCase );
        CPPUNIT_TEST( TestCtor );
        CPPUNIT_TEST( TestCtypeFunctions );
    CPPUNIT_TEST_SUITE_END();

    void TestCtor();
    void TestCtypeFunctions();

    void TestCtypeFunctionsWith(const wxXLocale& loc);

    DECLARE_NO_COPY_CLASS(XLocaleTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( XLocaleTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( XLocaleTestCase, "XLocaleTestCase" );


// test the different wxXLocale ctors
void XLocaleTestCase::TestCtor()
{
    CPPUNIT_ASSERT( !wxXLocale().IsOk() );
    CPPUNIT_ASSERT( wxCLocale.IsOk() );
    CPPUNIT_ASSERT( wxXLocale("C").IsOk() );
#ifdef wxHAS_XLOCALE_SUPPORT
    CPPUNIT_ASSERT( wxXLocale(wxLANGUAGE_FRENCH).IsOk() );
#ifdef __WXMSW__
    CPPUNIT_ASSERT( wxXLocale("french").IsOk() );
#else
    CPPUNIT_ASSERT( wxXLocale("fr_FR").IsOk() );
#endif
#endif
    CPPUNIT_ASSERT( !wxXLocale("bloordyblop").IsOk() );
}

// test the ctype functions with the given locale
void XLocaleTestCase::TestCtypeFunctionsWith(const wxXLocale& loc)
{
    // isalnum
    CPPUNIT_ASSERT( wxIsalnum_l('0', loc) );
    CPPUNIT_ASSERT( wxIsalnum_l('9', loc) );
    CPPUNIT_ASSERT( wxIsalnum_l('A', loc) );
    CPPUNIT_ASSERT( wxIsalnum_l('Z', loc) );
    CPPUNIT_ASSERT( wxIsalnum_l('a', loc) );
    CPPUNIT_ASSERT( wxIsalnum_l('z', loc) );
    CPPUNIT_ASSERT( !wxIsalnum_l('*', loc) );
    CPPUNIT_ASSERT( !wxIsalnum_l('@', loc) );
    CPPUNIT_ASSERT( !wxIsalnum_l('+', loc) );

    // isalpha
    CPPUNIT_ASSERT( !wxIsalpha_l('0', loc) );
    CPPUNIT_ASSERT( !wxIsalpha_l('9', loc) );
    CPPUNIT_ASSERT( wxIsalpha_l('A', loc) );
    CPPUNIT_ASSERT( wxIsalpha_l('Z', loc) );
    CPPUNIT_ASSERT( wxIsalpha_l('a', loc) );
    CPPUNIT_ASSERT( wxIsalpha_l('z', loc) );
    CPPUNIT_ASSERT( !wxIsalpha_l('*', loc) );
    CPPUNIT_ASSERT( !wxIsalpha_l('@', loc) );
    CPPUNIT_ASSERT( !wxIsalpha_l('+', loc) );

    // TODO: iscntrl

    // isdigit
    CPPUNIT_ASSERT( wxIsdigit_l('0', loc) );
    CPPUNIT_ASSERT( wxIsdigit_l('9', loc) );
    CPPUNIT_ASSERT( !wxIsdigit_l('A', loc) );
    CPPUNIT_ASSERT( !wxIsdigit_l('Z', loc) );
    CPPUNIT_ASSERT( !wxIsdigit_l('a', loc) );
    CPPUNIT_ASSERT( !wxIsdigit_l('z', loc) );

    // TODO: isgraph

    // islower
    CPPUNIT_ASSERT( !wxIslower_l('A', loc) );
    CPPUNIT_ASSERT( !wxIslower_l('Z', loc) );
    CPPUNIT_ASSERT( wxIslower_l('a', loc) );
    CPPUNIT_ASSERT( wxIslower_l('z', loc) );
    CPPUNIT_ASSERT( !wxIslower_l('0', loc) );
    CPPUNIT_ASSERT( !wxIslower_l('9', loc) );


    // TODO: isprint
    // TODO: ispunct

    // isspace
    CPPUNIT_ASSERT( wxIsspace_l(' ', loc) );
    CPPUNIT_ASSERT( wxIsspace_l('\t', loc) );
    CPPUNIT_ASSERT( wxIsspace_l('\r', loc) );
    CPPUNIT_ASSERT( wxIsspace_l('\n', loc) );
    CPPUNIT_ASSERT( !wxIsspace_l('0', loc) );
    CPPUNIT_ASSERT( !wxIsspace_l('a', loc) );
    CPPUNIT_ASSERT( !wxIsspace_l('A', loc) );

    // isupper
    CPPUNIT_ASSERT( !wxIsupper_l('0', loc) );
    CPPUNIT_ASSERT( !wxIsupper_l('9', loc) );
    CPPUNIT_ASSERT( wxIsupper_l('A', loc) );
    CPPUNIT_ASSERT( wxIsupper_l('Z', loc) );
    CPPUNIT_ASSERT( !wxIsupper_l('a', loc) );
    CPPUNIT_ASSERT( !wxIsupper_l('z', loc) );

    // isxdigit
    CPPUNIT_ASSERT( wxIsxdigit_l('0', loc) );
    CPPUNIT_ASSERT( wxIsxdigit_l('9', loc) );
    CPPUNIT_ASSERT( wxIsxdigit_l('A', loc) );
    CPPUNIT_ASSERT( wxIsxdigit_l('F', loc) );
    CPPUNIT_ASSERT( !wxIsxdigit_l('Z', loc) );
    CPPUNIT_ASSERT( wxIsxdigit_l('a', loc) );
    CPPUNIT_ASSERT( wxIsxdigit_l('f', loc) );
    CPPUNIT_ASSERT( !wxIsxdigit_l('z', loc) );

    // tolower
    CPPUNIT_ASSERT_EQUAL( 'a', (char)wxTolower_l('A', loc) );
    CPPUNIT_ASSERT_EQUAL( 'a', (char)wxTolower_l('a', loc) );
    CPPUNIT_ASSERT_EQUAL( 'z', (char)wxTolower_l('Z', loc) );
    CPPUNIT_ASSERT_EQUAL( 'z', (char)wxTolower_l('z', loc) );
    CPPUNIT_ASSERT_EQUAL( '0', (char)wxTolower_l('0', loc) );
    CPPUNIT_ASSERT_EQUAL( '9', (char)wxTolower_l('9', loc) );

    // toupper
    CPPUNIT_ASSERT_EQUAL( 'A', (char)wxToupper_l('A', loc) );
    CPPUNIT_ASSERT_EQUAL( 'A', (char)wxToupper_l('a', loc) );
    CPPUNIT_ASSERT_EQUAL( 'Z', (char)wxToupper_l('Z', loc) );
    CPPUNIT_ASSERT_EQUAL( 'Z', (char)wxToupper_l('z', loc) );
    CPPUNIT_ASSERT_EQUAL( '0', (char)wxToupper_l('0', loc) );
    CPPUNIT_ASSERT_EQUAL( '9', (char)wxToupper_l('9', loc) );
}

void XLocaleTestCase::TestCtypeFunctions()
{
    TestCtypeFunctionsWith(wxCLocale);

#ifdef wxHAS_XLOCALE_SUPPORT
    wxXLocale locFR(wxLANGUAGE_FRENCH);
    CPPUNIT_ASSERT( locFR.IsOk() ); // doesn't make sense to continue otherwise

    TestCtypeFunctionsWith(locFR);

    CPPUNIT_ASSERT( wxIsalpha_l('\xe9', locFR) );
    CPPUNIT_ASSERT( wxIslower_l('\xe9', locFR) );
    CPPUNIT_ASSERT( !wxIslower_l('\xc9', locFR) );
    CPPUNIT_ASSERT( wxIsupper_l('\xc9', locFR) );
#endif
}

#endif // wxUSE_XLOCALE
