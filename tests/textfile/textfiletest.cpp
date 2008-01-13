///////////////////////////////////////////////////////////////////////////////
// Name:        tests/textfile/textfile.cpp
// Purpose:     wxTextFile unit test
// Author:      Vadim Zeitlin
// Created:     2006-03-31
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTFILE

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/textfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextFileTestCase : public CppUnit::TestCase
{
public:
    TextFileTestCase() { }

    virtual void tearDown() { unlink(GetTestFileName()); }

private:
    CPPUNIT_TEST_SUITE( TextFileTestCase );
        CPPUNIT_TEST( ReadEmpty );
        CPPUNIT_TEST( ReadDOS );
        CPPUNIT_TEST( ReadUnix );
        CPPUNIT_TEST( ReadMac );
        CPPUNIT_TEST( ReadMixed );
#if wxUSE_UNICODE
        CPPUNIT_TEST( ReadUTF8 );
        CPPUNIT_TEST( ReadUTF16 );
#endif // wxUSE_UNICODE
    CPPUNIT_TEST_SUITE_END();

    void ReadEmpty();
    void ReadDOS();
    void ReadUnix();
    void ReadMac();
    void ReadMixed();
#if wxUSE_UNICODE
    void ReadUTF8();
    void ReadUTF16();
#endif // wxUSE_UNICODE

    // return the name of the test file we use
    static const char *GetTestFileName() { return "textfiletest.txt"; }

    // create the test file with the given contents
    static void CreateTestFile(const char *contents)
    {
        CreateTestFile(strlen(contents), contents);
    }

    // create the test file with the given contents (version must be used if
    // contents contains NULs)
    static void CreateTestFile(size_t len, const char *contents);


    DECLARE_NO_COPY_CLASS(TextFileTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextFileTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextFileTestCase, "TextFileTestCase" );

void TextFileTestCase::CreateTestFile(size_t len, const char *contents)
{
    FILE *f = fopen(GetTestFileName(), "wb");
    CPPUNIT_ASSERT( f );

    CPPUNIT_ASSERT( fwrite(contents, 1, len, f) >= 0 );
    CPPUNIT_ASSERT( fclose(f) == 0 );
}

void TextFileTestCase::ReadEmpty()
{
    CreateTestFile("");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)0, f.GetLineCount() );
}

void TextFileTestCase::ReadDOS()
{
    CreateTestFile("foo\r\nbar\r\nbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadUnix()
{
    CreateTestFile("foo\nbar\nbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadMac()
{
    CreateTestFile("foo\rbar\rbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadMixed()
{
    CreateTestFile("foo\rbar\r\nbaz\n");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(1) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("foo")), f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("baz")), f.GetLastLine() );
}

#if wxUSE_UNICODE

void TextFileTestCase::ReadUTF8()
{
    CreateTestFile("\xd0\x9f\n"
                   "\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName()), wxConvUTF8) );

    CPPUNIT_ASSERT_EQUAL( (size_t)2, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(1) );
#ifdef wxHAVE_U_ESCAPE
    CPPUNIT_ASSERT_EQUAL( wxString(L"\u041f"), f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( wxString(L"\u0440\u0438\u0432\u0435\u0442"),
                          f.GetLastLine() );
#endif // wxHAVE_U_ESCAPE
}

void TextFileTestCase::ReadUTF16()
{
    CreateTestFile(16,
                   "\x1f\x04\x0d\x00\x0a\x00"
                   "\x40\x04\x38\x04\x32\x04\x35\x04\x42\x04");

    wxTextFile f;
    wxMBConvUTF16LE conv;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName()), conv) );

    CPPUNIT_ASSERT_EQUAL( (size_t)2, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(1) );

#ifdef wxHAVE_U_ESCAPE
    CPPUNIT_ASSERT_EQUAL( wxString(L"\u041f"), f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( wxString(L"\u0440\u0438\u0432\u0435\u0442"),
                          f.GetLastLine() );
#endif // wxHAVE_U_ESCAPE
}

#endif // wxUSE_UNICODE

#endif // wxUSE_TEXTFILE

