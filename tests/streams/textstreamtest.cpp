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

#if wxUSE_LONGLONG
    #include "wx/longlong.h"
#endif

#if wxUSE_UNICODE
    #include "wx/mstream.h"
#endif // wxUSE_UNICODE

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextStreamTestCase : public CppUnit::TestCase
{
public:
    TextStreamTestCase();

private:
    CPPUNIT_TEST_SUITE( TextStreamTestCase );
        CPPUNIT_TEST( Endline );
        CPPUNIT_TEST( MiscTests );

#if wxUSE_LONGLONG
        CPPUNIT_TEST( TestLongLong );
        CPPUNIT_TEST( TestULongLong );
#endif // wxUSE_LONGLONG

#if wxUSE_UNICODE
        CPPUNIT_TEST( TestUTF8Input );
        CPPUNIT_TEST( TestEmbeddedZerosUTF16LEInput );
        CPPUNIT_TEST( TestEmbeddedZerosUTF16BEInput );
        CPPUNIT_TEST( TestEmbeddedZerosUTF32LEInput );
        CPPUNIT_TEST( TestEmbeddedZerosUTF32BEInput );
#endif // wxUSE_UNICODE
    CPPUNIT_TEST_SUITE_END();

    void Endline();
    void MiscTests();

#if wxUSE_LONGLONG
    void TestLongLong();
    void TestULongLong();
#endif // wxUSE_LONGLONG

#if wxUSE_UNICODE
    void TestUTF8Input();
    void TestEmbeddedZerosUTF16LEInput();
    void TestEmbeddedZerosUTF16BEInput();
    void TestEmbeddedZerosUTF32LEInput();
    void TestEmbeddedZerosUTF32BEInput();
    void TestInput(const wxMBConv& conv,
                   const void* encodedText,
                   size_t encodedSize );
#endif // wxUSE_UNICODE


    wxDECLARE_NO_COPY_CLASS(TextStreamTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextStreamTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextStreamTestCase, "TextStreamTestCase" );

TextStreamTestCase::TextStreamTestCase()
{
}

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

void TextStreamTestCase::Endline()
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

    CPPUNIT_ASSERT( memcmp(&szIn[9], NEWLINE, NEWLINELEN) == 0 );
}

void TextStreamTestCase::MiscTests()
{
    wxString filename = wxT("testdata.conf");
    wxFileInputStream fsIn(filename);
    if ( !fsIn.IsOk() )
    {
        return;
    }

    wxTextInputStream tis(fsIn);
    CPPUNIT_ASSERT_EQUAL("# this is the test data file for wxFileConfig tests", tis.ReadLine());
    CPPUNIT_ASSERT_EQUAL("value1=one", tis.ReadLine());
    CPPUNIT_ASSERT_EQUAL("# a comment here", tis.ReadLine());
    CPPUNIT_ASSERT_EQUAL("value2=two", tis.ReadLine());
    CPPUNIT_ASSERT_EQUAL("value\\ with\\ spaces\\ inside\\ it=nothing special", tis.ReadLine());
    CPPUNIT_ASSERT_EQUAL("path=$PATH", tis.ReadLine());
}

#if wxUSE_LONGLONG

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

            CPPUNIT_ASSERT( value == values[n] );
        }
    }
}

void TextStreamTestCase::TestLongLong()
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

void TextStreamTestCase::TestULongLong()
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

#endif // wxUSE_LONGLONG

#if wxUSE_UNICODE

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

void TextStreamTestCase::TestUTF8Input()
{
    TestInput(wxConvUTF8, txtUtf8, sizeof(txtUtf8));
    TestInput(wxCSConv(wxFONTENCODING_UTF8), txtUtf8, sizeof(txtUtf8));
}

void TextStreamTestCase::TestEmbeddedZerosUTF16LEInput()
{
    TestInput(wxMBConvUTF16LE(), txtUtf16le, sizeof(txtUtf16le));
    TestInput(wxCSConv(wxFONTENCODING_UTF16LE), txtUtf16le, sizeof(txtUtf16le));
}

void TextStreamTestCase::TestEmbeddedZerosUTF16BEInput()
{
    TestInput(wxMBConvUTF16BE(), txtUtf16be, sizeof(txtUtf16be));
    TestInput(wxCSConv(wxFONTENCODING_UTF16BE), txtUtf16be, sizeof(txtUtf16be));
}

void TextStreamTestCase::TestEmbeddedZerosUTF32LEInput()
{
    TestInput(wxMBConvUTF32LE(), txtUtf32le, sizeof(txtUtf32le));
    TestInput(wxCSConv(wxFONTENCODING_UTF32LE), txtUtf32le, sizeof(txtUtf32le));
}

void TextStreamTestCase::TestEmbeddedZerosUTF32BEInput()
{
    TestInput(wxMBConvUTF32BE(), txtUtf32be, sizeof(txtUtf32be));
    TestInput(wxCSConv(wxFONTENCODING_UTF32BE), txtUtf32be, sizeof(txtUtf32be));
}

void TextStreamTestCase::TestInput(const wxMBConv& conv,
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

    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(txtWchar), temp.length() );

    CPPUNIT_ASSERT_EQUAL( 0, memcmp(txtWchar, temp.wc_str(), sizeof(txtWchar)) );
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

#endif // wxUSE_UNICODE
