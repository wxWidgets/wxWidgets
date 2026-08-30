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
// test fixture
// ----------------------------------------------------------------------------

namespace
{

class TextFileTestCase
{
public:
    TextFileTestCase()
    {
        srand((unsigned)time(nullptr));
    }

    ~TextFileTestCase() { unlink(GetTestFileName()); }

protected:
    // return the name of the test file we use
    static const char *GetTestFileName() { return "textfiletest.txt"; }

    // create the test file with the given contents
    static void CreateTestFile(const char *contents)
    {
        CreateTestFile(strlen(contents), contents);
    }

    // create the test file with the given contents (version must be used if
    // contents contains NULs)
    static void CreateTestFile(size_t len, const char *contents)
    {
        FILE *f = fopen(GetTestFileName(), "wb");
        REQUIRE( f );

        CHECK( fwrite(contents, 1, len, f) == len );
        CHECK( fclose(f) == 0 );
    }

    wxDECLARE_NO_COPY_CLASS(TextFileTestCase);
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadEmpty", "[textfile]")
{
    CreateTestFile("");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    CHECK( f.GetLineCount() == (size_t)0 );
    CHECK( f.Eof() );
    CHECK( f.GetFirstLine() == "" );
    CHECK( f.GetLastLine() == "" );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadDOS", "[textfile]")
{
    CreateTestFile("foo\r\nbar\r\nbaz");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    CHECK( f.GetLineCount() == (size_t)3 );
    CHECK( f.GetLineType(0) == wxTextFileType_Dos );
    CHECK( f.GetLineType(2) == wxTextFileType_None );
    CHECK( f.GetLine(1) == wxString(wxT("bar")) );
    CHECK( f.GetLastLine() == wxString(wxT("baz")) );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadDOSLast", "[textfile]")
{
    CreateTestFile("foo\r\n");

    wxTextFile f;
    CHECK( f.Open(GetTestFileName()) );

    CHECK( f.GetLineCount() == 1 );
    CHECK( f.GetLineType(0) == wxTextFileType_Dos );
    CHECK( f.GetFirstLine() == "foo" );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadUnix", "[textfile]")
{
    CreateTestFile("foo\nbar\nbaz");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    CHECK( f.GetLineCount() == (size_t)3 );
    CHECK( f.GetLineType(0) == wxTextFileType_Unix );
    CHECK( f.GetLineType(2) == wxTextFileType_None );
    CHECK( f.GetLine(1) == wxString(wxT("bar")) );
    CHECK( f.GetLastLine() == wxString(wxT("baz")) );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadUnixLast", "[textfile]")
{
    CreateTestFile("foo\n");

    wxTextFile f;
    CHECK( f.Open(GetTestFileName()) );

    CHECK( f.GetLineCount() == 1 );
    CHECK( f.GetLineType(0) == wxTextFileType_Unix );
    CHECK( f.GetFirstLine() == "foo" );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadMac", "[textfile]")
{
    CreateTestFile("foo\rbar\r\rbaz");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    CHECK( f.GetLineCount() == (size_t)4 );
    CHECK( f.GetLineType(0) == wxTextFileType_Mac );
    CHECK( f.GetLineType(1) == wxTextFileType_Mac );
    CHECK( f.GetLineType(2) == wxTextFileType_Mac );
    CHECK( f.GetLineType(3) == wxTextFileType_None );
    CHECK( f.GetLine(0) == wxString(wxT("foo")) );
    CHECK( f.GetLine(1) == wxString(wxT("bar")) );
    CHECK( f.GetLine(2) == wxString(wxT("")) );
    CHECK( f.GetLastLine() == wxString(wxT("baz")) );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadMacLast", "[textfile]")
{
    CreateTestFile("foo\r");

    wxTextFile f;
    CHECK( f.Open(GetTestFileName()) );

    CHECK( f.GetLineCount() == 1 );
    CHECK( f.GetLineType(0) == wxTextFileType_Mac );
    CHECK( f.GetFirstLine() == "foo" );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadMixed", "[textfile]")
{
    CreateTestFile("foo\rbar\r\nbaz\n");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    CHECK( f.GetLineCount() == (size_t)3 );
    CHECK( f.GetLineType(0) == wxTextFileType_Mac );
    CHECK( f.GetLineType(1) == wxTextFileType_Dos );
    CHECK( f.GetLineType(2) == wxTextFileType_Unix );
    CHECK( f.GetFirstLine() == wxString(wxT("foo")) );
    CHECK( f.GetLine(1) == wxString(wxT("bar")) );
    CHECK( f.GetLastLine() == wxString(wxT("baz")) );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadMixedWithFuzzing", "[textfile]")
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
        CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );
        CHECK( f.GetLineCount() == (size_t)linesCnt );
    }
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadCRCRLF", "[textfile]")
{
    // Notepad may create files with CRCRLF line endings (see
    // https://stackoverflow.com/questions/6998506/text-file-with-0d-0d-0a-line-breaks).
    // Older versions of wx would loose all data when reading such files.
    // Test that the data are read, but don't worry about empty lines in between or
    // line endings. Also include a longer streak of CRs, because they can
    // happen as well.
    CreateTestFile("foo\r\r\nbar\r\r\r\nbaz\r\r\n");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName())) );

    wxString all;
    for ( wxString str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() )
        all += str;

    CHECK( all == "foobarbaz" );
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadUTF8", "[textfile]")
{
    CreateTestFile("П\nривет");

    wxTextFile f;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName()), wxConvUTF8) );

    CHECK( f.GetLineCount() == (size_t)2 );
    CHECK( f.GetLineType(0) == wxTextFileType_Unix );
    CHECK( f.GetLineType(1) == wxTextFileType_None );
#ifdef wxMUST_USE_U_ESCAPE
    CHECK( f.GetFirstLine() == wxString(L"\u041f") );
    CHECK( f.GetLastLine() == wxString(L"\u0440\u0438\u0432\u0435\u0442") );
#else
    CHECK( f.GetFirstLine() == wxString(L"П") );
    CHECK( f.GetLastLine() == wxString(L"ривет") );
#endif
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadUTF16", "[textfile]")
{
    CreateTestFile(16,
                   "\x1f\x04\x0d\x00\x0a\x00"
                   "\x40\x04\x38\x04\x32\x04\x35\x04\x42\x04");

    wxTextFile f;
    wxMBConvUTF16LE conv;
    CHECK( f.Open(wxString::FromAscii(GetTestFileName()), conv) );

    CHECK( f.GetLineCount() == (size_t)2 );
    CHECK( f.GetLineType(0) == wxTextFileType_Dos );
    CHECK( f.GetLineType(1) == wxTextFileType_None );

#ifdef wxMUST_USE_U_ESCAPE
    CHECK( f.GetFirstLine() == wxString(L"\u041f") );
    CHECK( f.GetLastLine() == wxString(L"\u0440\u0438\u0432\u0435\u0442") );
#else
    CHECK( f.GetFirstLine() == wxString(L"П") );
    CHECK( f.GetLastLine() == wxString(L"ривет") );
#endif
}

TEST_CASE_METHOD(TextFileTestCase, "TextFile::ReadBig", "[textfile]")
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
    CHECK( f.Open(GetTestFileName()) );

    CHECK( f.GetLineCount() == NUM_LINES );
    CHECK( f[0] == wxString("Line 1") );
    CHECK( f[998] == wxString("Line 999") );
    CHECK( f[999] == wxString("Line 1000") );
    CHECK( f[NUM_LINES - 1] == wxString::Format("Line %lu", (unsigned long)NUM_LINES) );
}

TEST_CASE("wxTextBuffer::Translate", "[textbuffer]")
{
    // Bytes with the value of LF that are part of an UTF-8 character shouldn't
    // be mangled.
    const wxString smiley = wxString::FromUTF8("😊"); // U+1F60A

    CHECK( wxTextBuffer::Translate(smiley, wxTextFileType_Dos) == smiley );
}

#ifdef __LINUX__

// Check if using wxTextFile with special files, whose reported size doesn't
// correspond to the real amount of data in them, works.
TEST_CASE("wxTextFile::Special", "[textfile][linux][special-file]")
{
    SECTION("/proc")
    {
        wxTextFile f;
        REQUIRE( f.Open("/proc/cpuinfo") );
        CHECK( f.GetLineCount() > 1 );
    }

    SECTION("/sys")
    {
        if ( !wxFile::Exists("/sys/power/state") )
        {
            WARN("/sys/power/state doesn't exist, skipping test");
            return;
        }

        wxTextFile f;
        CHECK( f.Open("/sys/power/state") );
        REQUIRE( f.GetLineCount() == 1 );
        INFO( "/sys/power/state contains \"" << f[0] << "\"" );
        CHECK( (f[0].find("mem") != wxString::npos || f[0].find("disk") != wxString::npos) );
    }
}

#endif // __LINUX__

#endif // wxUSE_TEXTFILE
