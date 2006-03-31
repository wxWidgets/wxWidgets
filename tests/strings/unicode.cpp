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
        CPPUNIT_TEST( Conversion );
        CPPUNIT_TEST( ConversionUTF7 );
        CPPUNIT_TEST( ConversionUTF8 );
#endif // wxUSE_WCHAR_T
    CPPUNIT_TEST_SUITE_END();

    void ToFromAscii();
#if wxUSE_WCHAR_T
    void ConstructorsWithConversion();
    void Conversion();
    void ConversionUTF7();
    void ConversionUTF8();

    // test if converting s using the given encoding gives ws and vice versa
    //
    // if either of the first 2 arguments is NULL, the conversion is supposed
    // to fail
    void DoTestConversion(const char *s, const wchar_t *w, wxCSConv& conv);
#endif // wxUSE_WCHAR_T


    DECLARE_NO_COPY_CLASS(UnicodeTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( UnicodeTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( UnicodeTestCase, "UnicodeTestCase" );

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
}

void UnicodeTestCase::Conversion()
{
#if wxUSE_UNICODE
        wxString szTheString(L"The\0String", wxConvLibc, 10);
        wxCharBuffer theBuffer = szTheString.mb_str();

        CPPUNIT_ASSERT( memcmp(theBuffer.data(), "The\0String", 11) == 0 );

        wxString szTheString2("The\0String", wxConvLocal, 10);
        CPPUNIT_ASSERT( szTheString2.length() == 11 );
        CPPUNIT_ASSERT( wxTmemcmp(szTheString2.c_str(), L"The\0String", 11) == 0 );
#else
        wxString szTheString(wxT("TheString"));
        szTheString.insert(3, 1, '\0');
        wxWCharBuffer theBuffer = szTheString.wc_str(wxConvLibc);

        CPPUNIT_ASSERT( memcmp(theBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );

        wxString szLocalTheString(wxT("TheString"));
        szLocalTheString.insert(3, 1, '\0');
        wxWCharBuffer theLocalBuffer = szLocalTheString.wc_str(wxConvLocal);

        CPPUNIT_ASSERT( memcmp(theLocalBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );
#endif
}

#if !wxUSE_UNICODE
// in case wcscmp is missing
//
static int wx_wcscmp(const wchar_t *s1, const wchar_t *s2)
{
    while (*s1 == *s2 && *s1 != 0)
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}
#endif

void
UnicodeTestCase::DoTestConversion(const char *s,
                                 const wchar_t *ws,
                                 wxCSConv& conv)
{
#if wxUSE_UNICODE
    if ( ws )
    {
        wxCharBuffer buf(wxString(ws).mb_str(conv));

        CPPUNIT_ASSERT( strcmp(buf, s) == 0 );
    }
#else // wxUSE_UNICODE
    if ( s )
    {
        wxWCharBuffer wbuf(wxString(s).wc_str(conv));

        if ( ws )
            CPPUNIT_ASSERT( wx_wcscmp(wbuf, ws) == 0 );
        else
            CPPUNIT_ASSERT( !*wbuf );
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
        //\u isn't recognized on MSVC 6
#if !defined(_MSC_VER)
#if !defined(__GNUC__) || (__GNUC__ >= 3)
        { "+AKM-", L"\u00a3" },
#endif
#endif
        // Windows accepts invalid UTF-7 strings and so does our UTF-7
        // conversion code -- this is wrong IMO but the way it is for now
        //
        // notice that converting "+" still behaves as expected because the
        // result is just an empty string, i.e. the same as if there were an
        // error, but converting "a+" results in "a" while it really should
        // fail
        { "+", NULL },
        { "a+", L"a" },
    };

    wxCSConv conv(_T("utf-7"));
    for ( size_t n = 0; n < WXSIZEOF(utf7data); n++ )
    {
        const StringConversionData& d = utf7data[n];
        DoTestConversion(d.str, d.wcs, conv);
    }
}

void UnicodeTestCase::ConversionUTF8()
{
    static const StringConversionData utf8data[] =
    {
        //\u isn't recognized on MSVC 6
#if !defined(_MSC_VER)
#if !defined(__GNUC__) || (__GNUC__ >= 3)
        { "\xc2\xa3", L"\u00a3" },
#endif
#endif
        { "\xc2", NULL },
    };

    wxCSConv conv(_T("utf-8"));
    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const StringConversionData& d = utf8data[n];
        DoTestConversion(d.str, d.wcs, conv);
    }
}

#endif // wxUSE_WCHAR_T

