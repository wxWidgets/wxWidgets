///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/filetest.cpp
// Purpose:     wxFile unit test
// Author:      Vadim Zeitlin
// Created:     2009-09-12
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_FILE

#include "wx/file.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileTestCase : public CppUnit::TestCase
{
public:
    FileTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FileTestCase );
        CPPUNIT_TEST( ReadAll );
        CPPUNIT_TEST( RoundTripUTF8 );
        CPPUNIT_TEST( RoundTripUTF16 );
        CPPUNIT_TEST( RoundTripUTF32 );
        CPPUNIT_TEST( TempFile );
    CPPUNIT_TEST_SUITE_END();

    void ReadAll();
    void RoundTripUTF8() { DoRoundTripTest(wxConvUTF8); }
    void RoundTripUTF16() { DoRoundTripTest(wxMBConvUTF16()); }
    void RoundTripUTF32() { DoRoundTripTest(wxMBConvUTF32()); }

    void DoRoundTripTest(const wxMBConv& conv);
    void TempFile();

    wxDECLARE_NO_COPY_CLASS(FileTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( FileTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileTestCase, "FileTestCase" );

// ----------------------------------------------------------------------------
// tests implementation
// ----------------------------------------------------------------------------

void FileTestCase::ReadAll()
{
    TestFile tf;

    const char* text = "Ream\nde";

    {
        wxFile fout(tf.GetName(), wxFile::write);
        CPPUNIT_ASSERT( fout.IsOpened() );
        fout.Write(text, strlen(text));
        CPPUNIT_ASSERT( fout.Close() );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CPPUNIT_ASSERT( fin.IsOpened() );

        wxString s;
        CPPUNIT_ASSERT( fin.ReadAll(&s) );
        CPPUNIT_ASSERT_EQUAL( text, s );
    }
}

void FileTestCase::DoRoundTripTest(const wxMBConv& conv)
{
    TestFile tf;

    // Explicit length is needed because of the embedded NUL.
    const wxString data("Hello\0UTF!", 10);

    {
        wxFile fout(tf.GetName(), wxFile::write);
        CPPUNIT_ASSERT( fout.IsOpened() );

        CPPUNIT_ASSERT( fout.Write(data, conv) );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CPPUNIT_ASSERT( fin.IsOpened() );

        const ssize_t len = fin.Length();
        wxCharBuffer buf(len);
        CPPUNIT_ASSERT_EQUAL( len, fin.Read(buf.data(), len) );

        wxString dataReadBack(buf, conv, len);
        CPPUNIT_ASSERT_EQUAL( data, dataReadBack );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CPPUNIT_ASSERT( fin.IsOpened() );

        wxString dataReadBack;
        CPPUNIT_ASSERT( fin.ReadAll(&dataReadBack, conv) );

        CPPUNIT_ASSERT_EQUAL( data, dataReadBack );
    }
}

void FileTestCase::TempFile()
{
    wxTempFile tmpFile;
    CPPUNIT_ASSERT( tmpFile.Open(wxT("test2")) );
    CPPUNIT_ASSERT( tmpFile.Write(wxT("the answer is 42")) );
    CPPUNIT_ASSERT( tmpFile.Commit() );
    CPPUNIT_ASSERT( wxRemoveFile(wxT("test2")) );
}

#ifdef __LINUX__

// Check that GetSize() works correctly for special files.
TEST_CASE("wxFile::Special", "[file][linux][special-file]")
{
    // We can't test /proc/kcore here, unlike in the similar
    // wxFileName::GetSize() test, as wxFile must be able to open it (at least
    // for reading) and usually we don't have the permissions to do it.

    // This file is not seekable and has 0 size, but can still be read.
    wxFile fileProc("/proc/cpuinfo");
    CHECK( fileProc.IsOpened() );

    wxString s;
    CHECK( fileProc.ReadAll(&s) );
    CHECK( !s.empty() );

    // All files in /sys have the size of one kernel page, even if they don't
    // have that much data in them.
    const long pageSize = sysconf(_SC_PAGESIZE);

    wxFile fileSys("/sys/power/state");
    if ( !fileSys.IsOpened() )
    {
        WARN("/sys/power/state can't be opened, skipping test");
        return;
    }

    CHECK( fileSys.Length() == pageSize );
    CHECK( fileSys.ReadAll(&s) );
    CHECK( !s.empty() );
    CHECK( s.length() < static_cast<unsigned long>(pageSize) );
}

#endif // __LINUX__

#endif // wxUSE_FILE
