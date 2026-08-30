///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/unicode.cpp
// Purpose:     Unicode unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-28
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/encconv.h"

// ----------------------------------------------------------------------------
// helper class holding the matching MB and WC strings
// ----------------------------------------------------------------------------

struct StringConversionData
{
    // either str or wcs (but not both) may be null, this means that the conversion
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
                INFO(Message(n, "MB2WC failed"));
                CHECK(wbuf.data());

                INFO(Message(n, "MB2WC", wbuf, wcs));
                CHECK(wxStrcmp(wbuf, wcs) == 0);
            }
            else // conversion is supposed to fail
            {
                INFO(Message(n, "MB2WC succeeded"));
                CHECK(!wbuf.data());
            }
        }

        if ( wcs && !(flags & ONLY_MB2WC) )
        {
            wxCharBuffer buf = conv.cWC2MB(wcs);

            if ( str )
            {
                INFO(Message(n, "WC2MB failed"));
                CHECK(buf.data());

                INFO(Message(n, "WC2MB", buf, str));
                CHECK(strcmp(buf, str) == 0);
            }
            else
            {
                INFO(Message(n, "WC2MB succeeded"));
                CHECK(!buf.data());
            }
        }
    }

private:
    static std::string
    Message(size_t n, const wxString& msg)
    {
        return wxString::Format("#%lu: %s", (unsigned long)n, msg).ToStdString();
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
// tests
// ----------------------------------------------------------------------------

TEST_CASE("Unicode::ToFromAscii", "[unicode]")
{

#define TEST_TO_FROM_ASCII(txt)                    \
    {                                              \
        static const char *msg = txt;              \
        wxString s = wxString::FromAscii(msg);     \
        CHECK( strcmp( s.ToAscii() , msg ) == 0 ); \
    }

    TEST_TO_FROM_ASCII( "Hello, world!" );
    TEST_TO_FROM_ASCII( "additional \" special \t test \\ component \n :-)" );
}

TEST_CASE("Unicode::ConstructorsWithConversion", "[unicode]")
{
    const unsigned char utf8Buf[] = "Déjà";
    const unsigned char utf8subBuf[] = "Déj";
    const char* utf8 = reinterpret_cast<const char*>(utf8Buf);
    const char* utf8sub = reinterpret_cast<const char*>(utf8subBuf);

    wxString s1(utf8, wxConvUTF8);

    const wchar_t wchar[] = {0x44,0xE9,0x6A,0xE0,0};
    CHECK( s1 == wchar );

    wxString s2(wchar);
    CHECK( s2 == wchar );
    CHECK( s2 == wxString::FromUTF8(utf8) );

    wxString sub(utf8sub, wxConvUTF8); // "Dej" substring
    wxString s3(utf8, wxConvUTF8, 4);
    CHECK( s3 == sub );

    wxString s4(wchar, 3);
    CHECK( s4 == sub );

    // conversion should stop with failure at pos 35
    wxString s("\t[pl]open.format.Sformatuj dyskietk\xea=gfloppy %f", wxConvUTF8);
    CHECK( s.empty() );


    // test using Unicode strings together with char* strings (this must work
    // in ANSI mode as well, of course):
    wxString s5("ascii");
    CHECK( s5 == "ascii" );

    s5 += " value";

    CHECK( strcmp(s5.mb_str(), "ascii value") == 0 );
    CHECK( s5 == "ascii value" );
    CHECK( s5 != "SomethingElse" );
}

TEST_CASE("Unicode::ConversionFixed", "[unicode]")
{
    size_t len;

    wxConvLibc.cWC2MB(L"", 0, &len);

    CHECK( len == 0 );

    // check that when we convert a fixed number of characters we obtain the
    // expected return value
    CHECK( wxConvLibc.ToWChar(nullptr, 0, "", 0) == 0 );
    CHECK( wxConvLibc.ToWChar(nullptr, 0, "x", 1) == 1 );
    CHECK( wxConvLibc.ToWChar(nullptr, 0, "x", 2) == 2 );
    CHECK( wxConvLibc.ToWChar(nullptr, 0, "xy", 2) == 2 );
}

TEST_CASE("Unicode::ConversionWithNULs", "[unicode]")
{
    static const size_t lenNulString = 10;

    wxString szTheString(L"The\0String", lenNulString);
    wxCharBuffer theBuffer = szTheString.mb_str(wxConvLibc);

    CHECK( memcmp(theBuffer.data(), "The\0String",
                    lenNulString + 1) == 0 );

    wxString szTheString2("The\0String", wxConvLocal, lenNulString);
    CHECK( szTheString2.length() == lenNulString );
    CHECK( wxTmemcmp(szTheString2.c_str(), L"The\0String",
                    lenNulString) == 0 );

    const char *null4buff = "\0\0\0\0";
    wxString null4str(null4buff, 4);
    CHECK( null4str.length() == 4 );
}

TEST_CASE("Unicode::ConversionUTF7", "[unicode]")
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
        StringConversionData("\xa3", nullptr),
        StringConversionData("+", nullptr),
        StringConversionData("+~", nullptr),
        StringConversionData("a+", nullptr),
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
#ifdef __WINDOWS__
        wxCSConv conv("utf-7");
        d.Test(n, conv);
#endif
        d.Test(n, wxConvUTF7);
    }
}

TEST_CASE("Unicode::ConversionUTF8", "[unicode]")
{
    static const StringConversionData utf8data[] =
    {
#ifdef wxMUST_USE_U_ESCAPE
        StringConversionData("\xc2\xa3", L"\u00a3"),
#else
        StringConversionData("£", L"£"),
#endif
    };

    wxCSConv conv(wxT("utf-8"));
    for ( size_t n = 0; n < WXSIZEOF(utf8data); n++ )
    {
        const StringConversionData& d = utf8data[n];
        d.Test(n, conv);
        d.Test(n, wxConvUTF8);
    }

    static const char* const u25a6 = "\xe2\x96\xa6";
    wxMBConvUTF8 c(wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
    CHECK( c.ToWChar(nullptr, 0, u25a6, wxNO_LEN) == 2 );
    CHECK( c.ToWChar(nullptr, 0, u25a6, 0) == 0 );
    CHECK( c.ToWChar(nullptr, 0, u25a6, 3) == 1 );
    CHECK( c.ToWChar(nullptr, 0, u25a6, 4) == 2 );

    // Verify that converting a string with embedded NULs works.
    CHECK( wxString::FromUTF8("abc\0\x32", 5).length() == 5 );

    // Verify that converting a string containing invalid UTF-8 does not work,
    // even if it happens after an embedded NUL.
    CHECK( wxString::FromUTF8("abc\xff").empty() );
    CHECK( wxString::FromUTF8("abc\0\xff", 5).empty() );
}

TEST_CASE("Unicode::ConversionUTF16", "[unicode]")
{
    static const StringConversionData utf16data[] =
    {
#ifdef wxMUST_USE_U_ESCAPE
        StringConversionData(
            "\x04\x1f\x04\x40\x04\x38\x04\x32\x04\x35\x04\x42\0\0",
            L"\u041f\u0440\u0438\u0432\u0435\u0442"),
#else
        StringConversionData(
            "\x04\x1f\x04\x40\x04\x38\x04\x32\x04\x35\x04\x42\0\0",
            L"Привет"),
#endif
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
    CHECK( len == 3 );

    // When using UTF-16 internally (i.e. MSW), we don't have any surrogate
    // support, so the length of the string below is 2, not 1.
#if SIZEOF_WCHAR_T == 4
    // Another one: verify that the length of the resulting string is computed
    // correctly when there is a surrogate in the input.
    wxMBConvUTF16BE().cMB2WC("\xd8\x03\xdc\x01\0" /* OLD TURKIC LETTER YENISEI A */, wxNO_LEN, &len);
    CHECK( len == 1 );
#endif // UTF-32 internal representation

#if SIZEOF_WCHAR_T == 2
    // Verify that the length of UTF-32 string is correct even when converting
    // to it from a longer UTF-16 string with surrogates.

    // Construct CAT FACE U+1F431 without using \U which is not supported by
    // ancient compilers and without using \u with surrogates which is
    // (correctly) flagged as an error by the newer ones.
    wchar_t ws[2];
    ws[0] = 0xd83d;
    ws[1] = 0xdc31;
    CHECK( wxMBConvUTF32BE().FromWChar(nullptr, 0, ws, 2) == 4 );
#endif // UTF-16 internal representation
}

TEST_CASE("Unicode::ConversionUTF32", "[unicode]")
{
    static const StringConversionData utf32data[] =
    {
#ifdef wxMUST_USE_U_ESCAPE
        StringConversionData(
            "\0\0\x04\x1f\0\0\x04\x40\0\0\x04\x38\0\0\x04\x32\0\0\x04\x35\0\0\x04\x42\0\0\0\0",
          L"\u041f\u0440\u0438\u0432\u0435\u0442"),
#else
        StringConversionData(
            "\0\0\x04\x1f\0\0\x04\x40\0\0\x04\x38\0\0\x04\x32\0\0\x04\x35\0\0\x04\x42\0\0\0\0",
          L"Привет"),
#endif
    };

    wxCSConv conv(wxFONTENCODING_UTF32BE);
    for ( size_t n = 0; n < WXSIZEOF(utf32data); n++ )
    {
        const StringConversionData& d = utf32data[n];
        d.Test(n, conv);
    }

    size_t len;
    conv.cMB2WC("\0\0\x01\0\0\0\0B\0\0\0C" /* A macron BC */, 12, &len);
    CHECK( len == 3 );
}

TEST_CASE("Unicode::IsConvOk", "[unicode]")
{
    CHECK( wxCSConv(wxFONTENCODING_SYSTEM).IsOk() );
    CHECK( wxCSConv("US-ASCII").IsOk() );
    CHECK( wxCSConv("UTF-8").IsOk() );
    CHECK( !wxCSConv("NoSuchConversion").IsOk() );

#ifdef __WINDOWS__
    CHECK( wxCSConv("WINDOWS-437").IsOk() );
#endif
}

TEST_CASE("Unicode::Iteration", "[unicode]")
{
    static const char *textUTF8 = "čeština";// "czech" in Czech
    static const wchar_t textUTF16[] = {0x10D, 0x65, 0x161, 0x74, 0x69, 0x6E, 0x61, 0};

    wxString text(wxString::FromUTF8(textUTF8));
    CHECK( wxStrcmp(text.wc_str(), textUTF16) == 0 );

    // verify the string was decoded correctly:
    {
        size_t idx = 0;
        for ( auto c : text )
        {
            CHECK( c == textUTF16[idx++] );
        }
    }

    // overwrite the string with something that is shorter in UTF-8:
    {
        for ( auto c : text )
            c = 'x';
    }

    // restore the original text now:
    {
        wxString::iterator end1 = text.end();
        wxString::const_iterator end2 = text.end();

        size_t idx = 0;
        for ( auto c : text )
        {
            c = textUTF16[idx++];

            CHECK( end1 == text.end() );
            CHECK( end2 == text.end() );
        }

        CHECK( end1 == text.end() );
        CHECK( end2 == text.end() );
    }

    // and verify it again:
    {
        size_t idx = 0;
        for ( auto c : text )
        {
            CHECK( c == textUTF16[idx++] );
        }
    }
}
