///////////////////////////////////////////////////////////////////////////////
// Name:        tests/textfile/textfile.cpp
// Purpose:     wxTextFile unit test
// Author:      Vadim Zeitlin
// Created:     2006-03-31
// Copyright:   (c) 2006 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_TEXTFILE

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/ffile.h"
#include "wx/textfile.h"

#ifdef __VISUALC__
    #define unlink _unlink
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextFileTestCase : public CppUnit::TestCase
{
public:
    TextFileTestCase()
    {
        srand((unsigned)time(NULL));
    }

    virtual void tearDown() wxOVERRIDE { unlink(GetTestFileName()); }

private:
    CPPUNIT_TEST_SUITE( TextFileTestCase );
        CPPUNIT_TEST( ReadEmpty );
        CPPUNIT_TEST( ReadDOS );
        CPPUNIT_TEST( ReadDOSLast );
        CPPUNIT_TEST( ReadUnix );
        CPPUNIT_TEST( ReadUnixLast );
        CPPUNIT_TEST( ReadMac );
        CPPUNIT_TEST( ReadMacLast );
        CPPUNIT_TEST( ReadMixed );
        CPPUNIT_TEST( ReadMixedWithFuzzing );
        CPPUNIT_TEST( ReadCRCRLF );
#if wxUSE_UNICODE
        CPPUNIT_TEST( ReadUTF8 );
        CPPUNIT_TEST( ReadUTF16 );
#endif // wxUSE_UNICODE
        CPPUNIT_TEST( ReadBig );
    CPPUNIT_TEST_SUITE_END();

    void ReadEmpty();
    void ReadDOS();
    void ReadDOSLast();
    void ReadUnix();
    void ReadUnixLast();
    void ReadMac();
    void ReadMacLast();
    void ReadMixed();
    void ReadMixedWithFuzzing();
    void ReadCRCRLF();
#if wxUSE_UNICODE
    void ReadUTF8();
    void ReadUTF16();
#endif // wxUSE_UNICODE
    void ReadBig();

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


    wxDECLARE_NO_COPY_CLASS(TextFileTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextFileTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextFileTestCase, "TextFileTestCase" );

void TextFileTestCase::CreateTestFile(size_t len, const char *contents)
{
    FILE *f = fopen(GetTestFileName(), "wb");
    CPPUNIT_ASSERT( f );

    CPPUNIT_ASSERT_EQUAL( len, fwrite(contents, 1, len, f) );
    CPPUNIT_ASSERT_EQUAL( 0, fclose(f) );
}

void TextFileTestCase::ReadEmpty()
{
    CreateTestFile("");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)0, f.GetLineCount() );
    CPPUNIT_ASSERT( f.Eof() );
    CPPUNIT_ASSERT_EQUAL( "", f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( "", f.GetLastLine() );
}

void TextFileTestCase::ReadDOS()
{
    CreateTestFile("foo\r\nbar\r\nbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadDOSLast()
{
    CreateTestFile("foo\r\n");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(GetTestFileName()) );

    CPPUNIT_ASSERT_EQUAL( 1, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Dos, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( "foo", f.GetFirstLine() );
}

void TextFileTestCase::ReadUnix()
{
    CreateTestFile("foo\nbar\nbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)3, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadUnixLast()
{
    CreateTestFile("foo\n");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(GetTestFileName()) );

    CPPUNIT_ASSERT_EQUAL( 1, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Unix, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( "foo", f.GetFirstLine() );
}

void TextFileTestCase::ReadMac()
{
    CreateTestFile("foo\rbar\r\rbaz");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    CPPUNIT_ASSERT_EQUAL( (size_t)4, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(1) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(2) );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_None, f.GetLineType(3) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("foo")), f.GetLine(0) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("")), f.GetLine(2) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadMacLast()
{
    CreateTestFile("foo\r");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(GetTestFileName()) );

    CPPUNIT_ASSERT_EQUAL( 1, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxTextFileType_Mac, f.GetLineType(0) );
    CPPUNIT_ASSERT_EQUAL( "foo", f.GetFirstLine() );
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
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("foo")), f.GetFirstLine() );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("bar")), f.GetLine(1) );
    CPPUNIT_ASSERT_EQUAL( wxString(wxT("baz")), f.GetLastLine() );
}

void TextFileTestCase::ReadMixedWithFuzzing()
{
    for ( int iteration = 0; iteration < 100; iteration++)
    {
        // Create a random buffer with lots of newlines. This is intended to catch
        // bad parsing in unexpected situations such as the one from ReadCRCRLF()
        // (which is so common it deserves a test of its own).
        static const char CHOICES[] = {'\r', '\n', 'X'};

        const size_t BUF_LEN = 100;
        char data[BUF_LEN + 1];
        data[0] = 'X';
        data[BUF_LEN] = '\0';
        unsigned linesCnt = 0;
        for ( size_t i = 1; i < BUF_LEN; i++ )
        {
            char ch = CHOICES[rand() % WXSIZEOF(CHOICES)];
            data[i] = ch;
            if ( ch == '\r' || (ch == '\n' && data[i-1] != '\r') )
                linesCnt++;
        }
        if (data[BUF_LEN-1] != '\r' && data[BUF_LEN-1] != '\n')
            linesCnt++; // last line was unterminated

        CreateTestFile(data);

        wxTextFile f;
        CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );
        CPPUNIT_ASSERT_EQUAL( (size_t)linesCnt, f.GetLineCount() );
    }
}

void TextFileTestCase::ReadCRCRLF()
{
    // Notepad may create files with CRCRLF line endings (see
    // https://stackoverflow.com/questions/6998506/text-file-with-0d-0d-0a-line-breaks).
    // Older versions of wx would loose all data when reading such files.
    // Test that the data are read, but don't worry about empty lines in between or
    // line endings. Also include a longer streak of CRs, because they can
    // happen as well.
    CreateTestFile("foo\r\r\nbar\r\r\r\nbaz\r\r\n");

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(wxString::FromAscii(GetTestFileName())) );

    wxString all;
    for ( wxString str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() )
        all += str;

    CPPUNIT_ASSERT_EQUAL( "foobarbaz", all );
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

void TextFileTestCase::ReadBig()
{
    static const size_t NUM_LINES = 10000;

    {
        wxFFile f(GetTestFileName(), "w");
        for ( size_t n = 0; n < NUM_LINES; n++ )
        {
            fprintf(f.fp(), "Line %lu\n", (unsigned long)n + 1);
        }
    }

    wxTextFile f;
    CPPUNIT_ASSERT( f.Open(GetTestFileName()) );

    CPPUNIT_ASSERT_EQUAL( NUM_LINES, f.GetLineCount() );
    CPPUNIT_ASSERT_EQUAL( wxString("Line 1"), f[0] );
    CPPUNIT_ASSERT_EQUAL( wxString("Line 999"), f[998] );
    CPPUNIT_ASSERT_EQUAL( wxString("Line 1000"), f[999] );
    CPPUNIT_ASSERT_EQUAL( wxString::Format("Line %lu", (unsigned long)NUM_LINES),
                          f[NUM_LINES - 1] );
}

#ifdef __LINUX__

// Check if using wxTextFile with special files, whose reported size doesn't
// correspond to the real amount of data in them, works.
TEST_CASE("wxTextFile::Special", "[textfile][linux][special-file]")
{
    // LXC containers don't (always) populate /proc and /sys, so skip these
    // tests there.
    if ( IsRunningInLXC() )
        return;

    SECTION("/proc")
    {
        wxTextFile f;
        CHECK( f.Open("/proc/cpuinfo") );
        CHECK( f.GetLineCount() > 1 );
    }

    SECTION("/sys")
    {
        wxTextFile f;
        CHECK( f.Open("/sys/power/state") );
        REQUIRE( f.GetLineCount() == 1 );
        INFO( "/sys/power/state contains \"" << f[0] << "\"" );
        CHECK( (f[0].find("mem") != wxString::npos || f[0].find("disk") != wxString::npos) );
    }
}

#endif // __LINUX__

#endif // wxUSE_TEXTFILE
