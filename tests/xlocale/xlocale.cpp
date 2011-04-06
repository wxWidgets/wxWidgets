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
        CPPUNIT_TEST( PreserveLocale );
        CPPUNIT_TEST( TestCtypeFunctions );
        CPPUNIT_TEST( TestStdlibFunctions );
    CPPUNIT_TEST_SUITE_END();

    void TestCtor();
    void PreserveLocale();
    void TestCtypeFunctions();
    void TestStdlibFunctions();

    void TestCtypeFunctionsWith(const wxXLocale& loc);
    void TestStdlibFunctionsWith(const wxXLocale& loc);

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
    CPPUNIT_ASSERT( !wxXLocale("bloordyblop").IsOk() );

    if (!wxLocale::IsAvailable(wxLANGUAGE_FRENCH))
        return;     // you should have french support installed to continue this test!

#ifdef wxHAS_XLOCALE_SUPPORT
    CPPUNIT_ASSERT( wxXLocale(wxLANGUAGE_FRENCH).IsOk() );
#ifdef __WXMSW__
    CPPUNIT_ASSERT( wxXLocale("french").IsOk() );
#else
    CPPUNIT_ASSERT( wxXLocale("fr_FR").IsOk() );
#endif
#endif
}

void XLocaleTestCase::PreserveLocale()
{
    // Test that using locale functions doesn't change the global C locale.
    const wxString origLocale(setlocale(LC_ALL, NULL));

    wxStrtod_l(wxT("1.234"), NULL, wxCLocale);

    CPPUNIT_ASSERT_EQUAL( origLocale, setlocale(LC_ALL, NULL) );
}

// test the ctype functions with the given locale
void XLocaleTestCase::TestCtypeFunctionsWith(const wxXLocale& loc)
{
    // NOTE: here go the checks which must pass under _any_ locale "loc";
    //       checks for specific locales are in TestCtypeFunctions()


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

// test the stdlib functions with the given locale
void XLocaleTestCase::TestStdlibFunctionsWith(const wxXLocale& loc)
{
    // NOTE: here go the checks which must pass under _any_ locale "loc";
    //       checks for specific locales are in TestStdlibFunctions()

#if wxUSE_UNICODE
    wchar_t* endptr;
#else
    char* endptr;
#endif

    // strtod (don't use decimal separator as it's locale-specific)
    CPPUNIT_ASSERT_EQUAL( 0.0,        wxStrtod_l(wxT("0"), NULL, loc) );
    CPPUNIT_ASSERT_EQUAL( 1234.0,     wxStrtod_l(wxT("1234"), NULL, loc) );

    // strtol
    endptr = NULL;
    CPPUNIT_ASSERT_EQUAL( 100,        wxStrtol_l(wxT("100"), NULL, 0, loc) );
    CPPUNIT_ASSERT_EQUAL( 0xFF,       wxStrtol_l(wxT("0xFF"), NULL, 0, loc) );
    CPPUNIT_ASSERT_EQUAL( 2001,       wxStrtol_l(wxT("2001 60c0c0 -1101110100110100100000 0x6fffff"), &endptr, 10, loc) );
    CPPUNIT_ASSERT_EQUAL( 0x60c0c0,   wxStrtol_l(endptr, &endptr, 16, loc) );
    CPPUNIT_ASSERT_EQUAL( -0x374D20,  wxStrtol_l(endptr, &endptr, 2, loc) );
    CPPUNIT_ASSERT_EQUAL( 0x6fffff,   wxStrtol_l(endptr, NULL, 0, loc) );

    // strtoul
    // NOTE: 3147483647 and 0xEE6B2800 are greater than LONG_MAX (on 32bit machines) but
    //       smaller than ULONG_MAX
    CPPUNIT_ASSERT_EQUAL( 3147483647ul,  wxStrtoul_l(wxT("3147483647"), NULL, 0, loc) );
    CPPUNIT_ASSERT_EQUAL( 0xEE6B2800ul, wxStrtoul_l(wxT("0xEE6B2800"), NULL, 0, loc) );

    // TODO: test for "failure" behaviour of the functions above
}

void XLocaleTestCase::TestCtypeFunctions()
{
    TestCtypeFunctionsWith(wxCLocale);

#ifdef wxHAS_XLOCALE_SUPPORT

    // french

    if (!wxLocale::IsAvailable(wxLANGUAGE_FRENCH))
        return;     // you should have french support installed to continue this test!

    wxXLocale locFR(wxLANGUAGE_FRENCH);
    CPPUNIT_ASSERT( locFR.IsOk() ); // doesn't make sense to continue otherwise

    TestCtypeFunctionsWith(locFR);

    CPPUNIT_ASSERT( wxIsalpha_l(wxT('\xe9'), locFR) );
    CPPUNIT_ASSERT( wxIslower_l(wxT('\xe9'), locFR) );
    CPPUNIT_ASSERT( !wxIslower_l(wxT('\xc9'), locFR) );
    CPPUNIT_ASSERT( wxIsupper_l(wxT('\xc9'), locFR) );
    CPPUNIT_ASSERT( wxIsalpha_l(wxT('\xe7'), locFR) );
    CPPUNIT_ASSERT( wxIslower_l(wxT('\xe7'), locFR) );
    CPPUNIT_ASSERT( wxIsupper_l(wxT('\xc7'), locFR) );


    // italian

    if (!wxLocale::IsAvailable(wxLANGUAGE_ITALIAN))
        return;     // you should have italian support installed to continue this test!

    wxXLocale locIT(wxLANGUAGE_ITALIAN);
    CPPUNIT_ASSERT( locIT.IsOk() ); // doesn't make sense to continue otherwise

    TestCtypeFunctionsWith(locIT);

    CPPUNIT_ASSERT( wxIsalpha_l(wxT('\xe1'), locIT) );
    CPPUNIT_ASSERT( wxIslower_l(wxT('\xe1'), locIT) );
#endif
}

void XLocaleTestCase::TestStdlibFunctions()
{
    TestStdlibFunctionsWith(wxCLocale);

#if wxUSE_UNICODE
    wchar_t* endptr;
#else
    char* endptr;
#endif

    // strtod checks specific for C locale
    endptr = NULL;
    CPPUNIT_ASSERT_EQUAL( 0.0,        wxStrtod_l(wxT("0.000"), NULL, wxCLocale) );
    CPPUNIT_ASSERT_EQUAL( 1.234,      wxStrtod_l(wxT("1.234"), NULL, wxCLocale) );
    CPPUNIT_ASSERT_EQUAL( -1.234E-5,  wxStrtod_l(wxT("-1.234E-5"), NULL, wxCLocale) );
    CPPUNIT_ASSERT_EQUAL( 365.24,     wxStrtod_l(wxT("365.24 29.53"), &endptr, wxCLocale) );
    CPPUNIT_ASSERT_EQUAL( 29.53,      wxStrtod_l(endptr, NULL, wxCLocale) );

#ifdef wxHAS_XLOCALE_SUPPORT

    // french

    if (!wxLocale::IsAvailable(wxLANGUAGE_FRENCH))
        return;     // you should have french support installed to continue this test!

    wxXLocale locFR(wxLANGUAGE_FRENCH);
    CPPUNIT_ASSERT( locFR.IsOk() ); // doesn't make sense to continue otherwise

    TestCtypeFunctionsWith(locFR);

    // comma as decimal point:
    CPPUNIT_ASSERT_EQUAL( 1.234, wxStrtod_l(wxT("1,234"), NULL, locFR) );

    // space as thousands separator is not recognized by wxStrtod_l():
    CPPUNIT_ASSERT( 1234.5 != wxStrtod_l(wxT("1 234,5"), NULL, locFR) );


    // italian

    if (!wxLocale::IsAvailable(wxLANGUAGE_ITALIAN))
        return;     // you should have italian support installed to continue this test!

    wxXLocale locIT(wxLANGUAGE_ITALIAN);
    CPPUNIT_ASSERT( locIT.IsOk() ); // doesn't make sense to continue otherwise

    TestStdlibFunctionsWith(locIT);

    // comma as decimal point:
    CPPUNIT_ASSERT_EQUAL( 1.234, wxStrtod_l(wxT("1,234"), NULL, locIT) );

    // dot as thousands separator is not recognized by wxStrtod_l():
    CPPUNIT_ASSERT( 1234.5 != wxStrtod_l(wxT("1.234,5"), NULL, locIT) );
#endif
}

#endif // wxUSE_XLOCALE
