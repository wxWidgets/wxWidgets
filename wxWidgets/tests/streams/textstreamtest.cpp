///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/uris.cpp
// Purpose:     wxTextXXXStream unit test
// Author:      Ryan Norton, Vince Harron
// Created:     2004-08-14
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton, (c) 2006 Vince Harron
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

#include "wx/txtstrm.h"
#include "wx/wfstream.h"

#if wxUSE_LONGLONG
    #include "wx/longlong.h"
#endif

#if wxUSE_UNICODE
    #include "wx/mstream.h"
#endif // wxUSE_UNICODE

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

#if wxUSE_LONGLONG
        CPPUNIT_TEST( TestLongLong );
        CPPUNIT_TEST( TestLongLong );
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


    DECLARE_NO_COPY_CLASS(TextStreamTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextStreamTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextStreamTestCase, "TextStreamTestCase" );

TextStreamTestCase::TextStreamTestCase()
{
}

#if defined(__WXMSW__) || defined(__WXPM__)
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
    wxFileOutputStream* pOutFile = new wxFileOutputStream(_T("test.txt"));
    wxTextOutputStream* pOutText = new wxTextOutputStream(*pOutFile);
    *pOutText   << _T("Test text") << endl
                << _T("More Testing Text (There should be newline before this)");

    delete pOutText;
    delete pOutFile;

    wxFileInputStream* pInFile = new wxFileInputStream(_T("test.txt"));

    char szIn[9 + NEWLINELEN];

    pInFile->Read(szIn, 9 + NEWLINELEN);

    CPPUNIT_ASSERT( memcmp(&szIn[9], NEWLINE, NEWLINELEN) == 0 );

    delete pInFile;
}

#if wxUSE_LONGLONG

template <typename T>
static void DoTestRoundTrip(const T *values, size_t numValues)
{
    {
        wxFileOutputStream fileOut(_T("test.txt"));
        wxTextOutputStream textOut(fileOut);

        for ( size_t n = 0; n < numValues; n++ )
        {
            textOut << values[n] << endl;
        }
    }

    {
        wxFileInputStream fileIn(_T("test.txt"));
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

const static wchar_t txtWchar[4] =
{
    0x0041, // LATIN CAPITAL LETTER A
    0x0100, // A WITH BREVE, LATIN SMALL LETTER
    0x0041, // LATIN CAPITAL LETTER A
    0x0100, // A WITH BREVE, LATIN SMALL LETTER
};

const static unsigned char txtUtf8[6] =
{
    0x41, 0xc4, 0x80, 0x41, 0xc4, 0x80,
};

const static unsigned char txtUtf16le[8] =
{
    0x41, 0x00, 0x00, 0x01, 0x41, 0x00, 0x00, 0x01,
};

const static unsigned char txtUtf16be[8] =
{
    0x00, 0x41, 0x01, 0x00, 0x00, 0x41, 0x01, 0x00,
};

const static unsigned char txtUtf32le[16] =
{
    0x41, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x41, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
};

const static unsigned char txtUtf32be[16] =
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

    CPPUNIT_ASSERT_EQUAL( 0, memcmp(txtWchar, temp.c_str(), sizeof(txtWchar)) );
}

#endif // wxUSE_UNICODE
