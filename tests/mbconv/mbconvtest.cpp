///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/main.cpp
// Purpose:     wxMBConv unit test
// Author:      Vadim Zeitlin, Mike Wetherell
// Created:     14.02.04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 TT-Solutions, (c) 2005 Mike Wetherell
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/strconv.h"
#include "wx/string.h"

#if defined wxHAVE_TCHAR_SUPPORT && !defined HAVE_WCHAR_H
    #define HAVE_WCHAR_H
#endif

// ----------------------------------------------------------------------------
// Some wide character constants. "\uXXXX" escapes aren't supported by old
// compilers such as VC++ 5 and g++ 2.95.
// ----------------------------------------------------------------------------

wchar_t u41[] = { 0x41, 0 };
wchar_t u7f[] = { 0x7f, 0 };

wchar_t u80[] = { 0x80, 0 };
wchar_t u391[] = { 0x391, 0 };
wchar_t u7ff[] = { 0x7ff, 0 };

wchar_t u800[] = { 0x800, 0 };
wchar_t u2620[] = { 0x2620, 0 };
wchar_t ufffd[] = { 0xfffd, 0 };

#if SIZEOF_WCHAR_T == 4
wchar_t u10000[] = { 0x10000, 0 };
wchar_t u1000a5[] = { 0x1000a5, 0 };
wchar_t u10fffd[] = { 0x10fffd, 0 };
#else
wchar_t u10000[] = { 0xd800, 0xdc00, 0 };
wchar_t u1000a5[] = { 0xdbc0, 0xdca5, 0 };
wchar_t u10fffd[] = { 0xdbff, 0xdffd, 0 };
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MBConvTestCase : public CppUnit::TestCase
{
public:
    MBConvTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MBConvTestCase );
        CPPUNIT_TEST( WC2CP1250 );
#ifdef HAVE_WCHAR_H
        CPPUNIT_TEST( UTF8_41 );
        CPPUNIT_TEST( UTF8_7f );
        CPPUNIT_TEST( UTF8_80 );
        CPPUNIT_TEST( UTF8_c2_7f );
        CPPUNIT_TEST( UTF8_c2_80 );
        CPPUNIT_TEST( UTF8_ce_91 );
        CPPUNIT_TEST( UTF8_df_bf );
        CPPUNIT_TEST( UTF8_df_c0 );
        CPPUNIT_TEST( UTF8_e0_a0_7f );
        CPPUNIT_TEST( UTF8_e0_a0_80 );
        CPPUNIT_TEST( UTF8_e2_98_a0 );
        CPPUNIT_TEST( UTF8_ef_bf_bd );
        CPPUNIT_TEST( UTF8_ef_bf_c0 );
        CPPUNIT_TEST( UTF8_f0_90_80_7f );
        CPPUNIT_TEST( UTF8_f0_90_80_80 );
        CPPUNIT_TEST( UTF8_f4_8f_bf_bd );
        CPPUNIT_TEST( UTF8PUA_f4_80_82_a5 );
        CPPUNIT_TEST( UTF8Octal_backslash245 );
#endif // HAVE_WCHAR_H
    CPPUNIT_TEST_SUITE_END();

    void WC2CP1250();

#ifdef HAVE_WCHAR_H
    // UTF-8 tests. Test the first, last and one in the middle for sequences
    // of each length
    void UTF8_41() { UTF8("\x41", u41); }
    void UTF8_7f() { UTF8("\x7f", u7f); }
    void UTF8_80() { UTF8("\x80", NULL); }

    void UTF8_c2_7f() { UTF8("\xc2\x7f", NULL); }
    void UTF8_c2_80() { UTF8("\xc2\x80", u80); }
    void UTF8_ce_91() { UTF8("\xce\x91", u391); }
    void UTF8_df_bf() { UTF8("\xdf\xbf", u7ff); }
    void UTF8_df_c0() { UTF8("\xdf\xc0", NULL); }

    void UTF8_e0_a0_7f() { UTF8("\xe0\xa0\x7f", NULL); }
    void UTF8_e0_a0_80() { UTF8("\xe0\xa0\x80", u800); }
    void UTF8_e2_98_a0() { UTF8("\xe2\x98\xa0", u2620); }
    void UTF8_ef_bf_bd() { UTF8("\xef\xbf\xbd", ufffd); }
    void UTF8_ef_bf_c0() { UTF8("\xef\xbf\xc0", NULL); }

    void UTF8_f0_90_80_7f() { UTF8("\xf0\x90\x80\x7f", NULL); }
    void UTF8_f0_90_80_80() { UTF8("\xf0\x90\x80\x80", u10000); }
    void UTF8_f4_8f_bf_bd() { UTF8("\xf4\x8f\xbf\xbd", u10fffd); }

    // test 'escaping the escape characters' for the two escaping schemes
    void UTF8PUA_f4_80_82_a5() { UTF8PUA("\xf4\x80\x82\xa5", u1000a5); }
    void UTF8Octal_backslash245() { UTF8Octal("\\245", L"\\245"); }

    // implementation for the utf-8 tests (see comments below)
    void UTF8(const char *charSequence, const wchar_t *wideSequence);
    void UTF8PUA(const char *charSequence, const wchar_t *wideSequence);
    void UTF8Octal(const char *charSequence, const wchar_t *wideSequence);
    void UTF8(const char *charSequence, const wchar_t *wideSequence, int option);
#endif // HAVE_WCHAR_H

    DECLARE_NO_COPY_CLASS(MBConvTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MBConvTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MBConvTestCase, "MBConvTestCase" );

void MBConvTestCase::WC2CP1250()
{
    static const struct Data
    {
        const wchar_t *wc;
        const char *cp1250;
    } data[] =
    {
        { L"hello", "hello" },  // test that it works in simplest case
        { L"\xBD of \xBD is \xBC", NULL }, // this should fail as cp1250 doesn't have 1/2
    };

    wxCSConv cs1250(wxFONTENCODING_CP1250);
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        if (d.cp1250)
        {
            CPPUNIT_ASSERT( strcmp(cs1250.cWC2MB(d.wc), d.cp1250) == 0 );
        }
        else
        {
            CPPUNIT_ASSERT( (const char*)cs1250.cWC2MB(d.wc) == NULL );
        }
    }
}

// ----------------------------------------------------------------------------
// UTF-8 tests
// ----------------------------------------------------------------------------

#ifdef HAVE_WCHAR_H

// Check that 'charSequence' translates to 'wideSequence' and back.
// Invalid sequences can be tested by giving NULL for 'wideSequence'. Even
// invalid sequences should roundtrip when an option is given and this is
// checked.
//
void MBConvTestCase::UTF8(const char *charSequence,
                          const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// Use this alternative when 'charSequence' contains a PUA character. Such
// sequences should still roundtrip ok, and this is checked.
//
void MBConvTestCase::UTF8PUA(const char *charSequence,
                             const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, NULL, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// Use this alternative when 'charSequence' contains an octal escape sequence.
// Such sequences should still roundtrip ok, and this is checked.
//
void MBConvTestCase::UTF8Octal(const char *charSequence,
                               const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, NULL, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// include the option in the error messages so it's possible to see which
// test failed
#define UTF8ASSERT(expr) CPPUNIT_ASSERT_MESSAGE(#expr + errmsg,  expr)

// The test implementation
//
void MBConvTestCase::UTF8(const char *charSequence,
                          const wchar_t *wideSequence,
                          int option)
{
    const size_t BUFSIZE = 128;
    wxASSERT(strlen(charSequence) * 3 + 10 < BUFSIZE);
    char bytes[BUFSIZE];
    
    // include the option in the error messages so it's possible to see
    // which test failed
    sprintf(bytes, " (with option == %d)", option);
    std::string errmsg(bytes);
    
    // put the charSequence at the start, middle and end of a string
    strcpy(bytes, charSequence);
    strcat(bytes, "ABC");
    strcat(bytes, charSequence);
    strcat(bytes, "XYZ");
    strcat(bytes, charSequence);

    // translate it into wide characters
    wxMBConvUTF8 utf8(option);
    wchar_t widechars[BUFSIZE];
    size_t lenResult = utf8.MB2WC(NULL, bytes, 0);
    size_t result = utf8.MB2WC(widechars, bytes, BUFSIZE);
    UTF8ASSERT(result == lenResult);

    // check we got the expected result
    if (wideSequence) {
        UTF8ASSERT(result != (size_t)-1);
        wxASSERT(result < BUFSIZE);

        wchar_t expected[BUFSIZE];
        wcscpy(expected, wideSequence);
        wcscat(expected, L"ABC");
        wcscat(expected, wideSequence);
        wcscat(expected, L"XYZ");
        wcscat(expected, wideSequence);

        UTF8ASSERT(wcscmp(widechars, expected) == 0);
        UTF8ASSERT(wcslen(widechars) == result);
    }
    else {
        // If 'wideSequence' is NULL, then the result is expected to be
        // invalid.  Normally that is as far as we can go, but if there is an
        // option then the conversion should succeed anyway, and it should be
        // possible to translate back to the original
        if (!option) {
            UTF8ASSERT(result == (size_t)-1);
            return;
        }
        else {
            UTF8ASSERT(result != (size_t)-1);
        }
    }

    // translate it back and check we get the original
    char bytesAgain[BUFSIZE];
    size_t lenResultAgain = utf8.WC2MB(NULL, widechars, 0);
    size_t resultAgain = utf8.WC2MB(bytesAgain, widechars, BUFSIZE);
    UTF8ASSERT(resultAgain == lenResultAgain);
    UTF8ASSERT(resultAgain != (size_t)-1);
    wxASSERT(resultAgain < BUFSIZE);

    UTF8ASSERT(strcmp(bytes, bytesAgain) == 0);
    UTF8ASSERT(strlen(bytesAgain) == resultAgain);
}

#endif // HAVE_WCHAR_H
