///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xlocale/xlocale.cpp
// Purpose:     wxXLocale & related unit test
// Author:      Brian Vanderburg II, Vadim Zeitlin
// Created:     2008-01-16
// Copyright:   (c) 2008 Brian Vanderburg II
//                  2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_XLOCALE

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/xlocale.h"

// --------------------------------------------------------------------------
// tests
// --------------------------------------------------------------------------

// test the different wxXLocale ctors
TEST_CASE("XLocale::Ctor", "[xlocale]")
{
    CHECK( !wxXLocale().IsOk() );
    CHECK( wxCLocale.IsOk() );
    CHECK( wxXLocale("C").IsOk() );
    CHECK( !wxXLocale("bloordyblop").IsOk() );

#ifdef wxHAS_XLOCALE_SUPPORT
    if ( wxXLocale(wxLANGUAGE_FRENCH).IsOk() )
    {
#ifdef __WINDOWS__
        CHECK( wxXLocale("french").IsOk() );
#else
        CHECK( wxXLocale("fr_FR").IsOk() );
#endif
    }
#endif // wxHAS_XLOCALE_SUPPORT
}

TEST_CASE("XLocale::PreserveLocale", "[xlocale]")
{
    // Test that using locale functions doesn't change the global C locale.
    const wxString origLocale(setlocale(LC_ALL, nullptr));

    wxStrtod_l(wxT("1.234"), nullptr, wxCLocale);

    CHECK( setlocale(LC_ALL, nullptr) == origLocale );
}

// test the ctype functions with the given locale
static void TestCtypeFunctionsWith(const wxXLocale& loc)
{
    // NOTE: here go the checks which must pass under _any_ locale "loc";
    //       checks for specific locales are in TestCtypeFunctions()


    // isalnum
    CHECK( wxIsalnum_l('0', loc) );
    CHECK( wxIsalnum_l('9', loc) );
    CHECK( wxIsalnum_l('A', loc) );
    CHECK( wxIsalnum_l('Z', loc) );
    CHECK( wxIsalnum_l('a', loc) );
    CHECK( wxIsalnum_l('z', loc) );
    CHECK( !wxIsalnum_l('*', loc) );
    CHECK( !wxIsalnum_l('@', loc) );
    CHECK( !wxIsalnum_l('+', loc) );

    // isalpha
    CHECK( !wxIsalpha_l('0', loc) );
    CHECK( !wxIsalpha_l('9', loc) );
    CHECK( wxIsalpha_l('A', loc) );
    CHECK( wxIsalpha_l('Z', loc) );
    CHECK( wxIsalpha_l('a', loc) );
    CHECK( wxIsalpha_l('z', loc) );
    CHECK( !wxIsalpha_l('*', loc) );
    CHECK( !wxIsalpha_l('@', loc) );
    CHECK( !wxIsalpha_l('+', loc) );

    // TODO: iscntrl

    // isdigit
    CHECK( wxIsdigit_l('0', loc) );
    CHECK( wxIsdigit_l('9', loc) );
    CHECK( !wxIsdigit_l('A', loc) );
    CHECK( !wxIsdigit_l('Z', loc) );
    CHECK( !wxIsdigit_l('a', loc) );
    CHECK( !wxIsdigit_l('z', loc) );

    // TODO: isgraph

    // islower
    CHECK( !wxIslower_l('A', loc) );
    CHECK( !wxIslower_l('Z', loc) );
    CHECK( wxIslower_l('a', loc) );
    CHECK( wxIslower_l('z', loc) );
    CHECK( !wxIslower_l('0', loc) );
    CHECK( !wxIslower_l('9', loc) );


    // TODO: isprint
    // TODO: ispunct

    // isspace
    CHECK( wxIsspace_l(' ', loc) );
    CHECK( wxIsspace_l('\t', loc) );
    CHECK( wxIsspace_l('\r', loc) );
    CHECK( wxIsspace_l('\n', loc) );
    CHECK( !wxIsspace_l('0', loc) );
    CHECK( !wxIsspace_l('a', loc) );
    CHECK( !wxIsspace_l('A', loc) );

    // isupper
    CHECK( !wxIsupper_l('0', loc) );
    CHECK( !wxIsupper_l('9', loc) );
    CHECK( wxIsupper_l('A', loc) );
    CHECK( wxIsupper_l('Z', loc) );
    CHECK( !wxIsupper_l('a', loc) );
    CHECK( !wxIsupper_l('z', loc) );

    // isxdigit
    CHECK( wxIsxdigit_l('0', loc) );
    CHECK( wxIsxdigit_l('9', loc) );
    CHECK( wxIsxdigit_l('A', loc) );
    CHECK( wxIsxdigit_l('F', loc) );
    CHECK( !wxIsxdigit_l('Z', loc) );
    CHECK( wxIsxdigit_l('a', loc) );
    CHECK( wxIsxdigit_l('f', loc) );
    CHECK( !wxIsxdigit_l('z', loc) );

    // tolower
    CHECK( (char)wxTolower_l('A', loc) == 'a' );
    CHECK( (char)wxTolower_l('a', loc) == 'a' );
    CHECK( (char)wxTolower_l('Z', loc) == 'z' );
    CHECK( (char)wxTolower_l('z', loc) == 'z' );
    CHECK( (char)wxTolower_l('0', loc) == '0' );
    CHECK( (char)wxTolower_l('9', loc) == '9' );

    // toupper
    CHECK( (char)wxToupper_l('A', loc) == 'A' );
    CHECK( (char)wxToupper_l('a', loc) == 'A' );
    CHECK( (char)wxToupper_l('Z', loc) == 'Z' );
    CHECK( (char)wxToupper_l('z', loc) == 'Z' );
    CHECK( (char)wxToupper_l('0', loc) == '0' );
    CHECK( (char)wxToupper_l('9', loc) == '9' );
}

// test the stdlib functions with the given locale
static void TestStdlibFunctionsWith(const wxXLocale& loc)
{
    // NOTE: here go the checks which must pass under _any_ locale "loc";
    //       checks for specific locales are in TestStdlibFunctions()

    wchar_t* endptr;

    // strtod (don't use decimal separator as it's locale-specific)
    CHECK( wxStrtod_l(wxT("0"), nullptr, loc) == 0.0 );
    CHECK( wxStrtod_l(wxT("1234"), nullptr, loc) == 1234.0 );

    // strtol
    endptr = nullptr;
    CHECK( wxStrtol_l(wxT("100"), nullptr, 0, loc) == 100 );
    CHECK( wxStrtol_l(wxT("0xFF"), nullptr, 0, loc) == 0xFF );
    CHECK( wxStrtol_l(wxT("2001 60c0c0 -1101110100110100100000 0x6fffff"), &endptr, 10, loc) == 2001 );
    CHECK( wxStrtol_l(endptr, &endptr, 16, loc) == 0x60c0c0 );
    CHECK( wxStrtol_l(endptr, &endptr, 2, loc) == -0x374D20 );
    CHECK( wxStrtol_l(endptr, nullptr, 0, loc) == 0x6fffff );

    // strtoul
    // NOTE: 3147483647 and 0xEE6B2800 are greater than LONG_MAX (on 32bit machines) but
    //       smaller than ULONG_MAX
    CHECK( wxStrtoul_l(wxT("3147483647"), nullptr, 0, loc) == 3147483647ul );
    CHECK( wxStrtoul_l(wxT("0xEE6B2800"), nullptr, 0, loc) == 0xEE6B2800ul );

    // TODO: test for "failure" behaviour of the functions above
}

TEST_CASE("XLocale::CtypeFunctions", "[xlocale]")
{
    SECTION("C")
    {
        TestCtypeFunctionsWith(wxCLocale);
    }

#ifdef wxHAS_XLOCALE_SUPPORT
    SECTION("French")
    {
        wxXLocale locFR(wxLANGUAGE_FRENCH);
        if ( !locFR.IsOk() )
        {
            // Not an error, not all systems have French locale support.
            return;
        }

        TestCtypeFunctionsWith(locFR);

        CHECK( wxIsalpha_l(wxT('\xe9'), locFR) );
        CHECK( wxIslower_l(wxT('\xe9'), locFR) );
        CHECK( !wxIslower_l(wxT('\xc9'), locFR) );
        CHECK( wxIsupper_l(wxT('\xc9'), locFR) );
        CHECK( wxIsalpha_l(wxT('\xe7'), locFR) );
        CHECK( wxIslower_l(wxT('\xe7'), locFR) );
        CHECK( wxIsupper_l(wxT('\xc7'), locFR) );
    }

    SECTION("Italian")
    {
        wxXLocale locIT(wxLANGUAGE_ITALIAN);
        if ( !locIT.IsOk() )
            return;

        TestCtypeFunctionsWith(locIT);

        CHECK( wxIsalpha_l(wxT('\xe1'), locIT) );
        CHECK( wxIslower_l(wxT('\xe1'), locIT) );
    }
#endif // wxHAS_XLOCALE_SUPPORT
}

TEST_CASE("XLocale::StdlibFunctions", "[xlocale]")
{
    SECTION("C")
    {
        TestStdlibFunctionsWith(wxCLocale);

        wchar_t* endptr;

        // strtod checks specific for C locale
        endptr = nullptr;
        CHECK( wxStrtod_l(wxT("0.000"), nullptr, wxCLocale) == 0.0 );
        CHECK( wxStrtod_l(wxT("1.234"), nullptr, wxCLocale) == 1.234 );
        CHECK( wxStrtod_l(wxT("-1.234E-5"), nullptr, wxCLocale) == -1.234E-5 );
        CHECK( wxStrtod_l(wxT("365.24 29.53"), &endptr, wxCLocale) == 365.24 );
        CHECK( wxStrtod_l(endptr, nullptr, wxCLocale) == 29.53 );
    }

#ifdef wxHAS_XLOCALE_SUPPORT
    SECTION("French")
    {
        wxXLocale locFR(wxLANGUAGE_FRENCH);
        if ( !locFR.IsOk() )
            return;

        TestCtypeFunctionsWith(locFR);

        // comma as decimal point:
        CHECK( wxStrtod_l(wxT("1,234"), nullptr, locFR) == 1.234 );

        // space as thousands separator is not recognized by wxStrtod_l():
        CHECK( 1234.5 != wxStrtod_l(wxT("1 234,5"), nullptr, locFR) );
    }


    SECTION("Italian")
    {
        wxXLocale locIT(wxLANGUAGE_ITALIAN);
        if ( !locIT.IsOk() )
            return;

        TestStdlibFunctionsWith(locIT);

        // comma as decimal point:
        CHECK( wxStrtod_l(wxT("1,234"), nullptr, locIT) == 1.234 );

        // dot as thousands separator is not recognized by wxStrtod_l():
        CHECK( 1234.5 != wxStrtod_l(wxT("1.234,5"), nullptr, locIT) );
    }
#endif // wxHAS_XLOCALE_SUPPORT
}

#endif // wxUSE_XLOCALE
