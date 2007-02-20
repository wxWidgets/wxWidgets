///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/unicode.cpp
// Purpose:     Unicode unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-28
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
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

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

#if wxUSE_WCHAR_T && !wxUSE_UNICODE

// in case wcscmp is missing
static int wx_wcscmp(const wchar_t *s1, const wchar_t *s2)
{
    while (*s1 == *s2 && *s1 != 0)
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

#endif // wxUSE_WCHAR_T && !wxUSE_UNICODE

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class UnicodeTestCase : public CppUnit::TestCase
{
public:
    UnicodeTestCase();

private:
    CPPUNIT_TEST_SUITE( UnicodeTestCase );
        CPPUNIT_TEST( ToFromAscii );
#if wxUSE_WCHAR_T
        CPPUNIT_TEST( ConstructorsWithConversion );
        CPPUNIT_TEST( ConversionEmpty );
        CPPUNIT_TEST( ConversionWithNULs );
        CPPUNIT_TEST( ConversionUTF7 );
        CPPUNIT_TEST( ConversionUTF8 );
        CPPUNIT_TEST( ConversionUTF16 );
        CPPUNIT_TEST( ConversionUTF32 );
        CPPUNIT_TEST( IsConvOk );
#endif // wxUSE_WCHAR_T
    CPPUNIT_TEST_SUITE_END();

    void ToFromAscii();
#if wxUSE_WCHAR_T
    void ConstructorsWithConversion();
    void ConversionEmpty();
    void ConversionWithNULs();
    void ConversionUTF7();
    void ConversionUTF8();
    void ConversionUTF16();
    void ConversionUTF32();
    void IsConvOk();

    // test if converting s using the given encoding gives ws and vice versa
    //
    // if either of the first 2 arguments is NULL, the conversion is supposed
    // to fail
    void DoTestConversion(const char *s, const wchar_t *w, wxMBConv& conv);
#endif // wxUSE_WCHAR_T


    DECLARE_NO_COPY_CLASS(UnicodeTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( UnicodeTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( UnicodeTestCase, "Unicode" );

UnicodeTestCase::UnicodeTestCase()
{
}

void UnicodeTestCase::ToFromAscii()
{

#define TEST_TO_FROM_ASCII(txt)                              \
    {                                                        \
        static const char *msg = txt;                        \
        wxString s = wxString::FromAscii(msg);               \
        CPPUNIT_ASSERT( strcmp( s.ToAscii() , msg ) == 0 );  \
    }

    TEST_TO_FROM_ASCII( "Hello, world!" );
    TEST_TO_FROM_ASCII( "additional \" special \t test \\ component \n :-)" );
}

#if wxUSE_WCHAR_T
void UnicodeTestCase::ConstructorsWithConversion()
{
    // the string "Déjà" in UTF-8 and wchar_t:
    const unsigned char utf8Buf[] = {0x44,0xC3,0xA9,0x6A,0xC3,0xA0,0};
    const wchar_t wchar[] = {0x44,0xE9,0x6A,0xE0,0};
    const unsigned char utf8subBuf[] = {0x44,0xC3,0xA9,0x6A,0}; // just "Déj"
    const char *utf8 = (char *)utf8Buf;
    const char *utf8sub = (char *)utf8subBuf;

    wxString s1(utf8, wxConvUTF8);
    wxString s2(wchar, wxConvUTF8);

#if wxUSE_UNICODE
    CPPUNIT_ASSERT( s1 == wchar );
    CPPUNIT_ASSERT( s2 == wchar );
#else
    CPPUNIT_ASSERT( s1 == utf8 );
    CPPUNIT_ASSERT( s2 == utf8 );
#endif

    wxString sub(utf8sub, wxConvUTF8); // "Dej" substring
    wxString s3(utf8, wxConvUTF8, 4);
    wxString s4(wchar, wxConvUTF8, 3);

    CPPUNIT_ASSERT( s3 == sub );
    CPPUNIT_ASSERT( s4 == sub );

#if wxUSE_UNICODE
    CPPUNIT_ASSERT ( wxString("\t[pl]open.format.Sformatuj dyskietkê=gfloppy %f", 
                               wxConvUTF8) == wxT("") ); //should stop at pos 35 
#endif


    // test using Unicode strings together with char* strings (this must work
    // in ANSI mode as well, of course):
    wxString s5("ascii");
    CPPUNIT_ASSERT( s5 == "ascii" );

    s5 += " value";

    CPPUNIT_ASSERT( strcmp(s5.mb_str(), "ascii value") == 0 );
    CPPUNIT_ASSERT( s5 == "ascii value" );
    CPPUNIT_ASSERT( s5 != "SomethingElse" );
}

void UnicodeTestCase::ConversionEmpty()
{
    size_t len;

#if wxUSE_UNICODE
    wxCharBuffer buf = wxConvLibc.cWC2MB(L"", 0, &len);
#else // !wxUSE_UNICODE
    wxWCharBuffer wbuf = wxConvLibc.cMB2WC("", 0, &len);
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    CPPUNIT_ASSERT(len == 0);
}

void UnicodeTestCase::ConversionWithNULs()
{
#if wxUSE_UNICODE
    static const size_t lenNulString = 10;

    wxString szTheString(L"The\0String", wxConvLibc, lenNulString);
    wxCharBuffer theBuffer = szTheString.mb_str();

    CPPUNIT_ASSERT( memcmp(theBuffer.data(), "The\0String",
                    lenNulString + 1) == 0 );

    wxString szTheString2("The\0String", wxConvLocal, lenNulString);
    CPPUNIT_ASSERT_EQUAL( lenNulString, szTheString2.length() );
    CPPUNIT_ASSERT( wxTmemcmp(szTheString2.c_str(), L"The\0String",
                    lenNulString + 1) == 0 );
#else // !wxUSE_UNICODE
    wxString szTheString(wxT("TheString"));
    szTheString.insert(3, 1, '\0');
    wxWCharBuffer theBuffer = szTheString.wc_str(wxConvLibc);

    CPPUNIT_ASSERT( memcmp(theBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );

    wxString szLocalTheString(wxT("TheString"));
    szLocalTheString.insert(3, 1, '\0');
    wxWCharBuffer theLocalBuffer = szLocalTheString.wc_str(wxConvLocal);

    CPPUNIT_ASSERT( memcmp(theLocalBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}

void
UnicodeTestCase::DoTestConversion(const char *s,
                                  const wchar_t *ws,
                                  wxMBConv& conv)
{
#if wxUSE_UNICODE
    if ( ws )
    {
        wxCharBuffer buf = conv.cWC2MB(ws, (size_t)-1, NULL);

        CPPUNIT_ASSERT( strcmp(buf, s) == 0 );
    }
#else // wxUSE_UNICODE
    if ( s )
    {
        wxWCharBuffer wbuf = conv.cMB2WC(s, (size_t)-1, NULL);

        if ( ws )
        {
            CPPUNIT_ASSERT( wbuf.data() );
            CPPUNIT_ASSERT( wx_wcscmp(wbuf, ws) == 0 );
        }
        else // conversion is supposed to fail
        {
            CPPUNIT_ASSERT_EQUAL( (wchar_t *)NULL, wbuf.data() );
        }
    }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}

struct StringConversionData
{
    const char *str;
    const wchar_t *wcs;
};

void UnicodeTestCase::ConversionUTF7()
{
    static const StringConversionData utf7data[] =
    {
        { "+-", L"+" },
        { "+--", L"+-" },

#ifdef wxHAVE_U_ESCAPE
        { "+AKM-", L"\u00a3" },
#endif // wxHAVE_U_ESCAPE

        // the following are invalid UTF-7 sequences
        { "+", NULL },
        { "+~", NULL },
        { "a+", NULL },
    };

    wxCSConv conv(_T("utf-7"));
    for ( size_t n = 0; n < WXSIZEOF(utf7data); n++ )
    {
        const StringConversionData& d = utf7data[n];

        // converting to/from UTF-7 using iconv() currently doesn't work
        // because of several problems:
        //  - GetMBNulLen() doesn't return correct result (iconv converts L'\0'
        //    to an incomplete and anyhow nonsensical "+AA" string)
        //  - iconv refuses to convert "+-" (although it converts "+-\n" just
        //    fine, go figure)
        //
        // I have no idea how to fix this so just disable the test for now
#if 0
        DoTestConversion(d.str, d.wcs, conv);
#endif
        DoTestConversion(d.str, d.wcs, wxConvUTF7);
    }
}

void UnicodeTestCase::ConversionUTF8()
{
    static const StringConversionData utf8data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        { "\xc2\xa3", L"\u00a3" },
#endif
        { "\xc2", NULL },
    };

    wxCSConv conv(_T("utf-8"));
    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const StringConversionData& d = utf8data[n];
        DoTestConversion(d.str, d.wcs, conv);
        DoTestConversion(d.str, d.wcs, wxConvUTF8);
    }
}

void UnicodeTestCase::ConversionUTF16()
{
    static const StringConversionData utf16data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        { "\x04\x1f\x04\x40\x04\x38\x04\x32\x04\x35\x04\x42\0\0",
          L"\u041f\u0440\u0438\u0432\u0435\u0442" },
        { "\x01\0\0b\x01\0\0a\x01\0\0r\0\0", L"\u0100b\u0100a\u0100r" },
#endif
        { "\0f\0o\0o\0\0", L"foo" },
    };

    wxCSConv conv(wxFONTENCODING_UTF16BE);
    for ( size_t n = 0; n < WXSIZEOF(utf16data); n++ )
    {
        const StringConversionData& d = utf16data[n];
        DoTestConversion(d.str, d.wcs, conv);
    }

    // special case: this string has consecutive NULs inside it which don't
    // terminate the string, this exposed a bug in our conversion code which
    // got confused in this case
    size_t len;
    wxWCharBuffer wbuf(conv.cMB2WC("\x01\0\0B\0C" /* A macron BC */, 6, &len));
    CPPUNIT_ASSERT_EQUAL( (size_t)3, len );
}

void UnicodeTestCase::ConversionUTF32()
{
    static const StringConversionData utf32data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        {
            "\0\0\x04\x1f\0\0\x04\x40\0\0\x04\x38\0\0\x04\x32\0\0\x04\x35\0\0\x04\x42\0\0\0\0",
          L"\u041f\u0440\u0438\u0432\u0435\u0442" },
#endif
        { "\0\0\0f\0\0\0o\0\0\0o\0\0\0\0", L"foo" },
    };

    wxCSConv conv(wxFONTENCODING_UTF32BE);
    for ( size_t n = 0; n < WXSIZEOF(utf32data); n++ )
    {
        const StringConversionData& d = utf32data[n];
        DoTestConversion(d.str, d.wcs, conv);
    }

    size_t len;
    wxWCharBuffer wbuf(conv.cMB2WC("\0\0\x01\0\0\0\0B\0\0\0C" /* A macron BC */,
                                   12, &len));
    CPPUNIT_ASSERT_EQUAL( (size_t)3, len );
}

void UnicodeTestCase::IsConvOk()
{
    CPPUNIT_ASSERT( wxCSConv(wxFONTENCODING_SYSTEM).IsOk() );
    CPPUNIT_ASSERT( wxCSConv(_T("UTF-8")).IsOk() );
    CPPUNIT_ASSERT( !wxCSConv(_T("NoSuchConversion")).IsOk() );

#ifdef __WINDOWS__
    CPPUNIT_ASSERT( wxCSConv(_T("WINDOWS-437")).IsOk() );
#endif
}

#endif // wxUSE_WCHAR_T

