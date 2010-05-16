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
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/encconv.h"

// ----------------------------------------------------------------------------
// helper class holding the matching MB and WC strings
// ----------------------------------------------------------------------------

struct StringConversionData
{
    // either str or wcs (but not both) may be NULL, this means that the conversion
    // to it should fail
    StringConversionData(const char *str_, const wchar_t *wcs_, int flags_ = 0)
        : str(str_), wcs(wcs_), flags(flags_)
    {
    }

    const char * const str;
    const wchar_t * const wcs;

    enum
    {
        TEST_BOTH  = 0, // test both str -> wcs and wcs -> str
        ONLY_MB2WC = 1  // only test str -> wcs conversion
    };

    const int flags;

    // test that the conversion between str and wcs (subject to flags) succeeds
    //
    // the first argument is the index in the test array and is used solely for
    // diagnostics
    void Test(size_t n, wxMBConv& conv) const
    {
        if ( str )
        {
            wxWCharBuffer wbuf = conv.cMB2WC(str);

            if ( wcs )
            {
                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "MB2WC failed"),
                    wbuf.data()
                );

                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "MB2WC", wbuf, wcs),
                    wxStrcmp(wbuf, wcs) == 0
                );
            }
            else // conversion is supposed to fail
            {
                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "MB2WC succeeded"),
                    !wbuf.data()
                );
            }
        }

        if ( wcs && !(flags & ONLY_MB2WC) )
        {
            wxCharBuffer buf = conv.cWC2MB(wcs);

            if ( str )
            {
                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "WC2MB failed"),
                    buf.data()
                );

                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "WC2MB", buf, str),
                    strcmp(buf, str) == 0
                );
            }
            else
            {
                CPPUNIT_ASSERT_MESSAGE
                (
                    Message(n, "WC2MB succeeded"),
                    !buf.data()
                );
            }
        }
    }

private:
    static std::string
    Message(size_t n, const wxString& msg)
    {
        return std::string(wxString::Format("#%lu: %s", (unsigned long)n, msg));
    }

    template <typename T>
    static std::string
    Message(size_t n,
            const char *func,
            const wxCharTypeBuffer<T>& actual,
            const T *expected)
    {
        return Message(n,
                       wxString::Format("%s returned \"%s\", expected \"%s\"",
                                        func, actual.data(), expected));
    }
};

// ----------------------------------------------------------------------------
// test data for UnicodeTestCase::Utf8()
// ----------------------------------------------------------------------------

static const unsigned char utf8koi8r[] =
{
    208, 157, 208, 181, 209, 129, 208, 186, 208, 176, 208, 183, 208, 176,
    208, 189, 208, 189, 208, 190, 32, 208, 191, 208, 190, 209, 128, 208,
    176, 208, 180, 208, 190, 208, 178, 208, 176, 208, 187, 32, 208, 188,
    208, 181, 208, 189, 209, 143, 32, 209, 129, 208, 178, 208, 190, 208,
    181, 208, 185, 32, 208, 186, 209, 128, 209, 131, 209, 130, 208, 181,
    208, 185, 209, 136, 208, 181, 208, 185, 32, 208, 189, 208, 190, 208,
    178, 208, 190, 209, 129, 209, 130, 209, 140, 209, 142, 0
};

static const unsigned char utf8iso8859_1[] =
{
    0x53, 0x79, 0x73, 0x74, 0xc3, 0xa8, 0x6d, 0x65, 0x73, 0x20, 0x49, 0x6e,
    0x74, 0xc3, 0xa9, 0x67, 0x72, 0x61, 0x62, 0x6c, 0x65, 0x73, 0x20, 0x65,
    0x6e, 0x20, 0x4d, 0xc3, 0xa9, 0x63, 0x61, 0x6e, 0x69, 0x71, 0x75, 0x65,
    0x20, 0x43, 0x6c, 0x61, 0x73, 0x73, 0x69, 0x71, 0x75, 0x65, 0x20, 0x65,
    0x74, 0x20, 0x51, 0x75, 0x61, 0x6e, 0x74, 0x69, 0x71, 0x75, 0x65, 0
};

static const unsigned char utf8Invalid[] =
{
    0x3c, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x3e, 0x32, 0x30, 0x30,
    0x32, 0xe5, 0xb9, 0xb4, 0x30, 0x39, 0xe6, 0x9c, 0x88, 0x32, 0x35, 0xe6,
    0x97, 0xa5, 0x20, 0x30, 0x37, 0xe6, 0x99, 0x82, 0x33, 0x39, 0xe5, 0x88,
    0x86, 0x35, 0x37, 0xe7, 0xa7, 0x92, 0x3c, 0x2f, 0x64, 0x69, 0x73, 0x70,
    0x6c, 0x61, 0x79, 0
};

static const struct Utf8Data
{
    const unsigned char *text;
    size_t len;
    const wxChar *charset;
    wxFontEncoding encoding;
} utf8data[] =
{
    { utf8Invalid, WXSIZEOF(utf8Invalid), wxT("iso8859-1"), wxFONTENCODING_ISO8859_1 },
    { utf8koi8r, WXSIZEOF(utf8koi8r), wxT("koi8-r"), wxFONTENCODING_KOI8 },
    { utf8iso8859_1, WXSIZEOF(utf8iso8859_1), wxT("iso8859-1"), wxFONTENCODING_ISO8859_1 },
};


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
        CPPUNIT_TEST( ConstructorsWithConversion );
        CPPUNIT_TEST( ConversionFixed );
        CPPUNIT_TEST( ConversionWithNULs );
        CPPUNIT_TEST( ConversionUTF7 );
        CPPUNIT_TEST( ConversionUTF8 );
        CPPUNIT_TEST( ConversionUTF16 );
        CPPUNIT_TEST( ConversionUTF32 );
        CPPUNIT_TEST( IsConvOk );
#if wxUSE_UNICODE
        CPPUNIT_TEST( Iteration );
#endif
        CPPUNIT_TEST( Utf8 );
        CPPUNIT_TEST( EncodingConverter );
    CPPUNIT_TEST_SUITE_END();

    void ToFromAscii();
    void ConstructorsWithConversion();
    void ConversionFixed();
    void ConversionWithNULs();
    void ConversionUTF7();
    void ConversionUTF8();
    void ConversionUTF16();
    void ConversionUTF32();
    void IsConvOk();
#if wxUSE_UNICODE
    void Iteration();
#endif
    void Utf8();
    void EncodingConverter();

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

void UnicodeTestCase::ConstructorsWithConversion()
{
    // the string "Déjà" in UTF-8 and wchar_t:
    const unsigned char utf8Buf[] = {0x44,0xC3,0xA9,0x6A,0xC3,0xA0,0};
    const unsigned char utf8subBuf[] = {0x44,0xC3,0xA9,0x6A,0}; // just "Déj"
    const char *utf8 = (char *)utf8Buf;
    const char *utf8sub = (char *)utf8subBuf;

    wxString s1(utf8, wxConvUTF8);

#if wxUSE_UNICODE
    const wchar_t wchar[] = {0x44,0xE9,0x6A,0xE0,0};
    CPPUNIT_ASSERT_EQUAL( wchar, s1 );

    wxString s2(wchar);
    CPPUNIT_ASSERT_EQUAL( wchar, s2 );
    CPPUNIT_ASSERT_EQUAL( wxString::FromUTF8(utf8), s2 );
#else
    CPPUNIT_ASSERT_EQUAL( utf8, s1 );
#endif

    wxString sub(utf8sub, wxConvUTF8); // "Dej" substring
    wxString s3(utf8, wxConvUTF8, 4);
    CPPUNIT_ASSERT_EQUAL( sub, s3 );

#if wxUSE_UNICODE
    wxString s4(wchar, wxConvUTF8, 3);
    CPPUNIT_ASSERT_EQUAL( sub, s4 );

    // conversion should stop with failure at pos 35
    wxString s("\t[pl]open.format.Sformatuj dyskietk\xea=gfloppy %f", wxConvUTF8);
    CPPUNIT_ASSERT( s.empty() );
#endif // wxUSE_UNICODE


    // test using Unicode strings together with char* strings (this must work
    // in ANSI mode as well, of course):
    wxString s5("ascii");
    CPPUNIT_ASSERT_EQUAL( "ascii", s5 );

    s5 += " value";

    CPPUNIT_ASSERT( strcmp(s5.mb_str(), "ascii value") == 0 );
    CPPUNIT_ASSERT_EQUAL( "ascii value", s5 );
    CPPUNIT_ASSERT( s5 != "SomethingElse" );
}

void UnicodeTestCase::ConversionFixed()
{
    size_t len;

#if wxUSE_UNICODE
    wxConvLibc.cWC2MB(L"", 0, &len);
#else // !wxUSE_UNICODE
    wxConvLibc.cMB2WC("", 0, &len);
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    CPPUNIT_ASSERT_EQUAL( 0, len );

#if wxUSE_UNICODE
    // check that when we convert a fixed number of characters we obtain the
    // expected return value
    CPPUNIT_ASSERT_EQUAL( 0, wxConvLibc.ToWChar(NULL, 0, "", 0) );
    CPPUNIT_ASSERT_EQUAL( 1, wxConvLibc.ToWChar(NULL, 0, "x", 1) );
    CPPUNIT_ASSERT_EQUAL( 2, wxConvLibc.ToWChar(NULL, 0, "x", 2) );
    CPPUNIT_ASSERT_EQUAL( 2, wxConvLibc.ToWChar(NULL, 0, "xy", 2) );
#endif // wxUSE_UNICODE
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
    wxString szTheString("TheString");
    szTheString.insert(3, 1, '\0');
    wxWCharBuffer theBuffer = szTheString.wc_str(wxConvLibc);

    CPPUNIT_ASSERT( memcmp(theBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );

    wxString szLocalTheString("TheString");
    szLocalTheString.insert(3, 1, '\0');
    wxWCharBuffer theLocalBuffer = szLocalTheString.wc_str(wxConvLocal);

    CPPUNIT_ASSERT( memcmp(theLocalBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}

void UnicodeTestCase::ConversionUTF7()
{
    static const StringConversionData utf7data[] =
    {
        // normal fragments
        StringConversionData("+AKM-", L"\xa3"),
        StringConversionData("+AOk-t+AOk-", L"\xe9t\xe9"),

        // this one is an alternative valid encoding of the same string
        StringConversionData("+AOk-t+AOk", L"\xe9t\xe9",
                             StringConversionData::ONLY_MB2WC),

        // some special cases
        StringConversionData("+-", L"+"),
        StringConversionData("+--", L"+-"),

        // the following are invalid UTF-7 sequences
        StringConversionData("\xa3", NULL),
        StringConversionData("+", NULL),
        StringConversionData("+~", NULL),
        StringConversionData("a+", NULL),
    };

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
        d.Test(n, wxCSConv("utf-7"));
#endif
        d.Test(n, wxConvUTF7);
    }
}

void UnicodeTestCase::ConversionUTF8()
{
    static const StringConversionData utf8data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        StringConversionData("\xc2\xa3", L"\u00a3"),
#endif
        StringConversionData("\xc2", NULL),
    };

    wxCSConv conv(wxT("utf-8"));
    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const StringConversionData& d = utf8data[n];
        d.Test(n, conv);
        d.Test(n, wxConvUTF8);
    }
}

void UnicodeTestCase::ConversionUTF16()
{
    static const StringConversionData utf16data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        StringConversionData(
            "\x04\x1f\x04\x40\x04\x38\x04\x32\x04\x35\x04\x42\0\0",
            L"\u041f\u0440\u0438\u0432\u0435\u0442"),
        StringConversionData(
            "\x01\0\0b\x01\0\0a\x01\0\0r\0\0",
            L"\u0100b\u0100a\u0100r"),
#endif
        StringConversionData("\0f\0o\0o\0\0", L"foo"),
    };

    wxCSConv conv(wxFONTENCODING_UTF16BE);
    for ( size_t n = 0; n < WXSIZEOF(utf16data); n++ )
    {
        const StringConversionData& d = utf16data[n];
        d.Test(n, conv);
    }

    // special case: this string has consecutive NULs inside it which don't
    // terminate the string, this exposed a bug in our conversion code which
    // got confused in this case
    size_t len;
    conv.cMB2WC("\x01\0\0B\0C" /* A macron BC */, 6, &len);
    CPPUNIT_ASSERT_EQUAL( 3, len );
}

void UnicodeTestCase::ConversionUTF32()
{
    static const StringConversionData utf32data[] =
    {
#ifdef wxHAVE_U_ESCAPE
        StringConversionData(
            "\0\0\x04\x1f\0\0\x04\x40\0\0\x04\x38\0\0\x04\x32\0\0\x04\x35\0\0\x04\x42\0\0\0\0",
          L"\u041f\u0440\u0438\u0432\u0435\u0442"),
#endif
        StringConversionData("\0\0\0f\0\0\0o\0\0\0o\0\0\0\0", L"foo"),
    };

    wxCSConv conv(wxFONTENCODING_UTF32BE);
    for ( size_t n = 0; n < WXSIZEOF(utf32data); n++ )
    {
        const StringConversionData& d = utf32data[n];
        d.Test(n, conv);
    }

    size_t len;
    conv.cMB2WC("\0\0\x01\0\0\0\0B\0\0\0C" /* A macron BC */, 12, &len);
    CPPUNIT_ASSERT_EQUAL( 3, len );
}

void UnicodeTestCase::IsConvOk()
{
    CPPUNIT_ASSERT( wxCSConv(wxFONTENCODING_SYSTEM).IsOk() );
    CPPUNIT_ASSERT( wxCSConv("US-ASCII").IsOk() );
    CPPUNIT_ASSERT( wxCSConv("UTF-8").IsOk() );
    CPPUNIT_ASSERT( !wxCSConv("NoSuchConversion").IsOk() );

#ifdef __WINDOWS__
    CPPUNIT_ASSERT( wxCSConv("WINDOWS-437").IsOk() );
#endif
}

#if wxUSE_UNICODE
void UnicodeTestCase::Iteration()
{
    // "czech" in Czech ("cestina"):
    static const char *textUTF8 = "\304\215e\305\241tina";
    static const wchar_t textUTF16[] = {0x10D, 0x65, 0x161, 0x74, 0x69, 0x6E, 0x61, 0};

    wxString text(wxString::FromUTF8(textUTF8));
    CPPUNIT_ASSERT( wxStrcmp(text.wc_str(), textUTF16) == 0 );

    // verify the string was decoded correctly:
    {
        size_t idx = 0;
        for ( wxString::const_iterator i = text.begin(); i != text.end(); ++i, ++idx )
        {
            CPPUNIT_ASSERT( *i == textUTF16[idx] );
        }
    }

    // overwrite the string with something that is shorter in UTF-8:
    {
        for ( wxString::iterator i = text.begin(); i != text.end(); ++i )
            *i = 'x';
    }

    // restore the original text now:
    {
        wxString::iterator end1 = text.end();
        wxString::const_iterator end2 = text.end();

        size_t idx = 0;
        for ( wxString::iterator i = text.begin(); i != text.end(); ++i, ++idx )
        {
            *i = textUTF16[idx];

            CPPUNIT_ASSERT( end1 == text.end() );
            CPPUNIT_ASSERT( end2 == text.end() );
        }

        CPPUNIT_ASSERT( end1 == text.end() );
        CPPUNIT_ASSERT( end2 == text.end() );
    }

    // and verify it again:
    {
        size_t idx = 0;
        for ( wxString::const_iterator i = text.begin(); i != text.end(); ++i, ++idx )
        {
            CPPUNIT_ASSERT( *i == textUTF16[idx] );
        }
    }
}
#endif // wxUSE_UNICODE

void UnicodeTestCase::Utf8()
{
    // test code extracted from console sample r64320

    char buf[1024];
    wchar_t wbuf[1024];

    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const Utf8Data& u8d = utf8data[n];
        CPPUNIT_ASSERT( wxConvUTF8.MB2WC(wbuf, (const char *)u8d.text, WXSIZEOF(wbuf)) != (size_t)-1 );

#if 0       // FIXME: this conversion seem not to work...
        wxCSConv conv(u8d.charset);
        CPPUNIT_ASSERT( conv.WC2MB(buf, wbuf, WXSIZEOF(buf)) != (size_t)-1 );
#endif
        wxString s(wxConvUTF8.cMB2WC((const char *)u8d.text));
        CPPUNIT_ASSERT( !s.empty() );
    }
}

void UnicodeTestCase::EncodingConverter()
{
    // test code extracted from console sample r64320

#if 0
    char buf[1024];
    wchar_t wbuf[1024];

    CPPUNIT_ASSERT( wxConvUTF8.MB2WC(wbuf, (const char *)utf8koi8r, WXSIZEOF(utf8koi8r)) != (size_t)-1 );

    wxString s1(wxConvUTF8.cMB2WC((const char *)utf8koi8r));
    CPPUNIT_ASSERT( !s1.empty() );

    wxEncodingConverter ec;
    ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_KOI8);
    ec.Convert(wbuf, buf);
    wxString s2(buf);

    CPPUNIT_ASSERT_EQUAL( s1, s2 );
#endif
}