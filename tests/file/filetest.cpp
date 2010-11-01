///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/filetest.cpp
// Purpose:     wxFile unit test
// Author:      Vadim Zeitlin
// Created:     2009-09-12
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
#if wxUSE_UNICODE
        CPPUNIT_TEST( RoundTripUTF8 );
        CPPUNIT_TEST( RoundTripUTF16 );
        CPPUNIT_TEST( RoundTripUTF32 );
#endif // wxUSE_UNICODE
        CPPUNIT_TEST( TempFile );
    CPPUNIT_TEST_SUITE_END();

#if wxUSE_UNICODE
    void RoundTripUTF8() { DoRoundTripTest(wxConvUTF8); }
    void RoundTripUTF16() { DoRoundTripTest(wxMBConvUTF16()); }
    void RoundTripUTF32() { DoRoundTripTest(wxMBConvUTF32()); }
#endif // wxUSE_UNICODE

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

#if wxUSE_UNICODE

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
}

#endif // wxUSE_UNICODE

void FileTestCase::TempFile()
{
    wxTempFile tmpFile;
    CPPUNIT_ASSERT( tmpFile.Open(wxT("test2")) && tmpFile.Write(wxT("the answer is 42")) );
    CPPUNIT_ASSERT( tmpFile.Commit() );
    CPPUNIT_ASSERT( wxRemoveFile(wxT("test2")) );
}

#endif // wxUSE_FILE
