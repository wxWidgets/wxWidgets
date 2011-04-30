///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/convauto.cpp
// Purpose:     wxConvAuto unit test
// Author:      Vadim Zeitlin
// Created:     2006-04-04
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

#if wxUSE_UNICODE

#include "wx/convauto.h"

#include "wx/mstream.h"
#include "wx/txtstrm.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ConvAutoTestCase : public CppUnit::TestCase
{
public:
    ConvAutoTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ConvAutoTestCase );
        CPPUNIT_TEST( Empty );
        CPPUNIT_TEST( Short );
        CPPUNIT_TEST( None );
        CPPUNIT_TEST( UTF32LE );
        CPPUNIT_TEST( UTF32BE );
        CPPUNIT_TEST( UTF16LE );
        CPPUNIT_TEST( UTF16BE );
        CPPUNIT_TEST( UTF8 );
        CPPUNIT_TEST( StreamUTF8NoBOM );
        CPPUNIT_TEST( StreamUTF8 );
        CPPUNIT_TEST( StreamUTF16LE );
        CPPUNIT_TEST( StreamUTF16BE );
        CPPUNIT_TEST( StreamUTF32LE );
        CPPUNIT_TEST( StreamUTF32BE );
    CPPUNIT_TEST_SUITE_END();

    // real test function: check that converting the src multibyte string to
    // wide char using wxConvAuto yields wch as the first result
    //
    // the length of the string may need to be passed explicitly if it has
    // embedded NULs, otherwise it's not necessary
    void TestFirstChar(const char *src, wchar_t wch, size_t len = wxNO_LEN);

    void Empty();
    void Short();
    void None();
    void UTF32LE();
    void UTF32BE();
    void UTF16LE();
    void UTF16BE();
    void UTF8();

    // test whether two lines of text are converted properly from a stream
    void TestTextStream(const char *src,
                        size_t srclength,
                        const wxString& line1,
                        const wxString& line2);

    void StreamUTF8NoBOM();
    void StreamUTF8();
    void StreamUTF16LE();
    void StreamUTF16BE();
    void StreamUTF32LE();
    void StreamUTF32BE();
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(ConvAutoTestCase);

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ConvAutoTestCase, "ConvAutoTestCase");

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

void ConvAutoTestCase::TestFirstChar(const char *src, wchar_t wch, size_t len)
{
    wxWCharBuffer wbuf = wxConvAuto().cMB2WC(src, len, NULL);
    CPPUNIT_ASSERT( wbuf );
    CPPUNIT_ASSERT_EQUAL( wch, *wbuf );
}

void ConvAutoTestCase::Empty()
{
    CPPUNIT_ASSERT( !wxConvAuto().cMB2WC("") );
}

void ConvAutoTestCase::Short()
{
    TestFirstChar("1", wxT('1'));
}

void ConvAutoTestCase::None()
{
    TestFirstChar("Hello world", wxT('H'));
}

void ConvAutoTestCase::UTF32LE()
{
    TestFirstChar("\xff\xfe\0\0A\0\0\0", wxT('A'), 8);
}

void ConvAutoTestCase::UTF32BE()
{
    TestFirstChar("\0\0\xfe\xff\0\0\0B", wxT('B'), 8);
}

void ConvAutoTestCase::UTF16LE()
{
    TestFirstChar("\xff\xfeZ\0", wxT('Z'), 4);
}

void ConvAutoTestCase::UTF16BE()
{
    TestFirstChar("\xfe\xff\0Y", wxT('Y'), 4);
}

void ConvAutoTestCase::UTF8()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\xef\xbb\xbf\xd0\x9f", L'\u041f');
#endif
}

void ConvAutoTestCase::TestTextStream(const char *src,
                                      size_t srclength,
                                      const wxString& line1,
                                      const wxString& line2)
{
    wxMemoryInputStream instream(src, srclength);
    wxTextInputStream text(instream);

    CPPUNIT_ASSERT_EQUAL( line1, text.ReadLine() );
    CPPUNIT_ASSERT_EQUAL( line2, text.ReadLine() );
}

// the first line of the teststring used in the following functions is an
// 'a' followed by a Japanese hiragana A (u+3042).
// The second line is a single Greek beta (u+03B2). There is no blank line
// at the end.

namespace
{

const wxString line1 = wxString::FromUTF8("a\xe3\x81\x82");
const wxString line2 = wxString::FromUTF8("\xce\xb2");

} // anonymous namespace

void ConvAutoTestCase::StreamUTF8NoBOM()
{
    // currently this test doesn't work because without the BOM wxConvAuto
    // decides that the string is in Latin-1 after finding the first (but not
    // the two subsequent ones which are part of the same UTF-8 sequence!)
    // 8-bit character
    //
    // FIXME: we need to fix this at wxTextInputStream level, see #11570
#if 0
    TestTextStream("\x61\xE3\x81\x82\x0A\xCE\xB2",
                   7, line1, line2);
#endif
}

void ConvAutoTestCase::StreamUTF8()
{
    TestTextStream("\xEF\xBB\xBF\x61\xE3\x81\x82\x0A\xCE\xB2",
                   10, line1, line2);
}

void ConvAutoTestCase::StreamUTF16LE()
{
    TestTextStream("\xFF\xFE\x61\x00\x42\x30\x0A\x00\xB2\x03",
                   10, line1, line2);
}

void ConvAutoTestCase::StreamUTF16BE()
{
    TestTextStream("\xFE\xFF\x00\x61\x30\x42\x00\x0A\x03\xB2",
                   10, line1, line2);
}

void ConvAutoTestCase::StreamUTF32LE()
{
    TestTextStream("\xFF\xFE\0\0\x61\x00\0\0\x42\x30\0\0\x0A"
                   "\x00\0\0\xB2\x03\0\0",
                   20, line1, line2);
}

void ConvAutoTestCase::StreamUTF32BE()
{
    TestTextStream("\0\0\xFE\xFF\0\0\x00\x61\0\0\x30\x42\0\0\x00\x0A"
                   "\0\0\x03\xB2",
                   20, line1, line2);
}

#endif // wxUSE_UNICODE
