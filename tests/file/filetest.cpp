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
        CPPUNIT_TEST( RoundTripUTF8 );
        CPPUNIT_TEST( RoundTripUTF16 );
        CPPUNIT_TEST( RoundTripUTF32 );
    CPPUNIT_TEST_SUITE_END();

    void RoundTripUTF8() { DoRoundTripTest(wxConvUTF8); }
    void RoundTripUTF16() { DoRoundTripTest(wxMBConvUTF16()); }
    void RoundTripUTF32() { DoRoundTripTest(wxMBConvUTF32()); }

    void DoRoundTripTest(const wxMBConv& conv);

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

void FileTestCase::DoRoundTripTest(const wxMBConv& conv)
{
    TestFile tf;

    const wxString data = "Hello\0UTF";

    {
        wxFile fout(tf.GetName(), wxFile::write);
        CPPUNIT_ASSERT( fout.IsOpened() );

        CPPUNIT_ASSERT( fout.Write(data, conv) );
    }

    {
        wxFile fin(tf.GetName(), wxFile::read);
        CPPUNIT_ASSERT( fin.IsOpened() );

        const wxFileOffset len = fin.Length();
        wxCharBuffer buf(len);
        CPPUNIT_ASSERT_EQUAL( len, fin.Read(buf.data(), len) );

        wxWCharBuffer wbuf(conv.cMB2WC(buf));
#if wxUSE_UNICODE
        CPPUNIT_ASSERT_EQUAL( data, wbuf );
#else // !wxUSE_UNICODE
        CPPUNIT_ASSERT
        (
            memcmp(wbuf, L"Hello\0UTF", data.length()*sizeof(wchar_t)) == 0
        );
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
    }
}

#endif // wxUSE_FILE
