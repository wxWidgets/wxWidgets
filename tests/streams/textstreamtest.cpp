///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/textstreamtest.cpp
// Purpose:     wxTextXXXStream unit test
// Author:      Ryan Norton, Vince Harron
// Created:     2004-08-14
// Copyright:   (c) 2004 Ryan Norton, (c) 2006 Vince Harron
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/txtstrm.h"
#include "wx/wfstream.h"

#include "wx/longlong.h"

#include "wx/mstream.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

#if defined(__WINDOWS__)
#   define NEWLINE "\r\n"
#   define NEWLINELEN 2
#elif defined(__WXMAC__) && !defined(__DARWIN__)
#   define NEWLINE "\r"
#   define NEWLINELEN 1
#else
#   define NEWLINE "\n"
#   define NEWLINELEN 1
#endif

TEST_CASE("TextStream::Endline", "[textstream][stream]")
{
    TempFile f("test.txt");

    {
        wxFileOutputStream pOutFile(f.GetName());
        wxTextOutputStream pOutText(pOutFile);
        pOutText << wxT("Test text") << endl
                 << wxT("More Testing Text (There should be newline before this)");
    }

    wxFileInputStream pInFile(f.GetName());

    char szIn[9 + NEWLINELEN];

    pInFile.Read(szIn, 9 + NEWLINELEN);

    CHECK( memcmp(&szIn[9], NEWLINE, NEWLINELEN) == 0 );
}

TEST_CASE("TextStream::MiscTests", "[textstream][stream]")
{
    wxString filename = wxT("testdata.conf");
    wxFileInputStream fsIn(filename);
    if ( !fsIn.IsOk() )
    {
        return;
    }

    wxTextInputStream tis(fsIn);
    CHECK(tis.ReadLine() == "# this is the test data file for wxFileConfig tests");
    CHECK(tis.ReadLine() == "value1=one");
    CHECK(tis.ReadLine() == "# a comment here");
    CHECK(tis.ReadLine() == "value2=two");
    CHECK(tis.ReadLine() == "value\\ with\\ spaces\\ inside\\ it=nothing special");
    CHECK(tis.ReadLine() == "path=$PATH");
}

template <typename T>
static void DoTestRoundTrip(const T *values, size_t numValues)
{
    TempFile f("test.txt");

    {
        wxFileOutputStream fileOut(f.GetName());
        wxTextOutputStream textOut(fileOut);

        for ( size_t n = 0; n < numValues; n++ )
        {
            textOut << values[n] << endl;
        }
    }

    {
        wxFileInputStream fileIn(f.GetName());
        wxTextInputStream textIn(fileIn);

        T value;
        for ( size_t n = 0; n < numValues; n++ )
        {
            textIn >> value;

            CHECK( value == values[n] );
        }
    }
}

TEST_CASE("TextStream::LongLong", "[textstream][stream]")
{
    static const wxLongLong llvalues[] =
    {
        0,
        1,
        -1,
        0x12345678l,
        -0x12345678l,
        wxLL(0x123456789abcdef0),
        wxLL(-0x123456789abcdef0),
    };

    DoTestRoundTrip(llvalues, WXSIZEOF(llvalues));
}

TEST_CASE("TextStream::ULongLong", "[textstream][stream]")
{
    static const wxULongLong ullvalues[] =
    {
        0,
        1,
        0x12345678l,
        wxULL(0x123456789abcdef0),
    };

    DoTestRoundTrip(ullvalues, WXSIZEOF(ullvalues));
}

static const wchar_t txtWchar[4] =
{
    0x0041, // LATIN CAPITAL LETTER A
    0x0100, // A WITH BREVE, LATIN SMALL LETTER
    0x0041, // LATIN CAPITAL LETTER A
    0x0100, // A WITH BREVE, LATIN SMALL LETTER
};

static const unsigned char txtUtf8[6] =
{
    0x41, 0xc4, 0x80, 0x41, 0xc4, 0x80,
};

static const unsigned char txtUtf16le[8] =
{
    0x41, 0x00, 0x00, 0x01, 0x41, 0x00, 0x00, 0x01,
};

static const unsigned char txtUtf16be[8] =
{
    0x00, 0x41, 0x01, 0x00, 0x00, 0x41, 0x01, 0x00,
};

static const unsigned char txtUtf32le[16] =
{
    0x41, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x41, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
};

static const unsigned char txtUtf32be[16] =
{
    0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x01, 0x00,
};

static void TestInput(const wxMBConv& conv,
                      const void *encodedText,
                      size_t encodedSize);

TEST_CASE("TextStream::UTF8Input", "[textstream][stream]")
{
    TestInput(wxConvUTF8, txtUtf8, sizeof(txtUtf8));
    TestInput(wxCSConv(wxFONTENCODING_UTF8), txtUtf8, sizeof(txtUtf8));
}

TEST_CASE("TextStream::EmbeddedZerosUTF16LEInput", "[textstream][stream]")
{
    TestInput(wxMBConvUTF16LE(), txtUtf16le, sizeof(txtUtf16le));
    TestInput(wxCSConv(wxFONTENCODING_UTF16LE), txtUtf16le, sizeof(txtUtf16le));
}

TEST_CASE("TextStream::EmbeddedZerosUTF16BEInput", "[textstream][stream]")
{
    TestInput(wxMBConvUTF16BE(), txtUtf16be, sizeof(txtUtf16be));
    TestInput(wxCSConv(wxFONTENCODING_UTF16BE), txtUtf16be, sizeof(txtUtf16be));
}

TEST_CASE("TextStream::EmbeddedZerosUTF32LEInput", "[textstream][stream]")
{
    TestInput(wxMBConvUTF32LE(), txtUtf32le, sizeof(txtUtf32le));
    TestInput(wxCSConv(wxFONTENCODING_UTF32LE), txtUtf32le, sizeof(txtUtf32le));
}

TEST_CASE("TextStream::EmbeddedZerosUTF32BEInput", "[textstream][stream]")
{
    TestInput(wxMBConvUTF32BE(), txtUtf32be, sizeof(txtUtf32be));
    TestInput(wxCSConv(wxFONTENCODING_UTF32BE), txtUtf32be, sizeof(txtUtf32be));
}

static void TestInput(const wxMBConv& conv,
                      const void *encodedText,
                      size_t encodedSize)
{
    wxMemoryInputStream byteIn(encodedText, encodedSize);
    wxTextInputStream textIn(byteIn, wxT("\n"), conv);

    wxString temp;
    while ( wxChar c = textIn.GetChar() )
    {
        temp.Append(c);
    }

    CHECK( temp.length() == WXSIZEOF(txtWchar) );

    CHECK( memcmp(txtWchar, temp.wc_str(), sizeof(txtWchar)) == 0 );
}

TEST_CASE("wxTextInputStream::GetChar", "[text][input][stream][char]")
{
    // This is the simplest possible test that used to trigger assertion in
    // wxTextInputStream::GetChar().
    SECTION("starts-with-nul")
    {
        const wxUint8 buf[] = { 0x00, 0x01, };
        wxMemoryInputStream mis(buf, sizeof(buf));
        wxTextInputStream tis(mis);

        REQUIRE( tis.GetChar() == 0x00 );
        REQUIRE( tis.GetChar() == 0x01 );
        REQUIRE( tis.GetChar() == 0x00 );
        CHECK( tis.GetInputStream().Eof() );
    }

    // This exercises a problematic path in GetChar() as the first 3 bytes of
    // this stream look like the start of UTF-32BE BOM, but this is not
    // actually a BOM because the 4th byte is 0xFE and not 0xFF, so the stream
    // should decode the buffer as Latin-1 once it gets there.
    SECTION("almost-UTF-32-BOM")
    {
        const wxUint8 buf[] = { 0x00, 0x00, 0xFE, 0xFE, 0x01 };
        wxMemoryInputStream mis(buf, sizeof(buf));
        wxTextInputStream tis(mis);

        REQUIRE( tis.GetChar() == 0x00 );
        REQUIRE( tis.GetChar() == 0x00 );
        REQUIRE( tis.GetChar() == 0xFE );
        REQUIRE( tis.GetChar() == 0xFE );
        REQUIRE( tis.GetChar() == 0x01 );
        REQUIRE( tis.GetChar() == 0x00 );
        CHECK( tis.GetInputStream().Eof() );
    }

    // Two null bytes that look like the start of UTF-32BE BOM,
    // followed by 4 byte UTF-8 sequence.
    // Needs wxConvAuto to not switch to fallback on <6 bytes.
    SECTION("UTF8-with-nulls")
    {
        const wxUint8 buf[] = { 0x00, 0x00, 0xf0, 0x90, 0x8c, 0x98 };
        wxMemoryInputStream mis(buf, sizeof(buf));
        wxTextInputStream tis(mis);

        wxCharTypeBuffer<wxChar> e = wxString::FromUTF8((const char*)buf, sizeof(buf))
                                     .tchar_str<wxChar>();
        for ( size_t i = 0; i < e.length(); ++i )
        {
            INFO("i = " << i);
            REQUIRE( tis.GetChar() == e[i] );
        }
        REQUIRE( tis.GetChar() == 0x00 );
        CHECK( tis.GetInputStream().Eof() );
    }

    // Two null bytes that look like the start of UTF-32BE BOM,
    // then 3 bytes that look like the start of UTF-8 sequence.
    // Needs 6 character output buffer in GetChar().
    SECTION("almost-UTF8-with-nulls")
    {
        const wxUint8 buf[] = { 0x00, 0x00, 0xf0, 0x90, 0x8c, 0xe0 };
        wxMemoryInputStream mis(buf, sizeof(buf));
        wxTextInputStream tis(mis);

        wxCharTypeBuffer<wxChar> e = wxString((const char*)buf, wxCSConv(wxFONTENCODING_ISO8859_1),
                                              sizeof(buf)).tchar_str<wxChar>();
        for ( size_t i = 0; i < e.length(); ++i )
        {
            INFO("i = " << i);
            REQUIRE( tis.GetChar() == e[i] );
        }
        REQUIRE( tis.GetChar() == 0x00 );
        CHECK( tis.GetInputStream().Eof() );
    }
}
