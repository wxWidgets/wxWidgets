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
    CPPUNIT_TEST_SUITE_END();

    void ReadEmpty();
    void ReadDOS();
    void ReadUnix();
    void ReadMac();
    void ReadMixed();

    // return the name of the test file we use
    static const char *GetTestFileName() { return "textfiletest.txt"; }

    // create the test file with the given contents
    static void CreateTestFile(const char *contents);


    DECLARE_NO_COPY_CLASS(TextFileTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextFileTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextFileTestCase, "TextFileTestCase" );

void TextFileTestCase::CreateTestFile(const char *contents)
{
    FILE *f = fopen(GetTestFileName(), "wb");
    CPPUNIT_ASSERT( f );

    CPPUNIT_ASSERT( fputs(contents, f) >= 0 );
    CPPUNIT_ASSERT( fclose(f) == 0 );
}

void TextFileTestCase::ReadEmpty()
{
    CreateTestFile("");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( 0u, f.GetLineCount() );
}

void TextFileTestCase::ReadDOS()
{
    CreateTestFile("foo\r\nbar\r\nbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( 3u, f.GetLineCount() );
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

    CPPUNIT_ASSERT_EQUAL( 3u, f.GetLineCount() );
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

    CPPUNIT_ASSERT_EQUAL( 3u, f.GetLineCount() );
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

    CPPUNIT_ASSERT_EQUAL( 3u, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(1) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("foo")), f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(_T("baz")), f.GetLastLine() );
}

#endif // wxUSE_TEXTFILE

