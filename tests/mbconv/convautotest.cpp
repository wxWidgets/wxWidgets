///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/convauto.cpp
// Purpose:     wxConvAuto unit test
// Author:      Vadim Zeitlin
// Created:     2006-04-04
// Copyright:   (c) 2006 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


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
        CPPUNIT_TEST( Init );
        CPPUNIT_TEST( Empty );
        CPPUNIT_TEST( Encode );
        CPPUNIT_TEST( Short );
        CPPUNIT_TEST( None );
        CPPUNIT_TEST( UTF32LE );
        CPPUNIT_TEST( UTF32BE );
        CPPUNIT_TEST( UTF16LE );
        CPPUNIT_TEST( UTF16BE );
        CPPUNIT_TEST( UTF8 );
        CPPUNIT_TEST( UTF8NoBom );
        CPPUNIT_TEST( Fallback );
        CPPUNIT_TEST( FallbackMultibyte );
        CPPUNIT_TEST( FallbackShort );
        CPPUNIT_TEST( StreamUTF8NoBOM );
        CPPUNIT_TEST( StreamUTF8 );
        CPPUNIT_TEST( StreamUTF16LE );
        CPPUNIT_TEST( StreamUTF16BE );
        CPPUNIT_TEST( StreamUTF32LE );
        CPPUNIT_TEST( StreamUTF32BE );
        CPPUNIT_TEST( StreamFallback );
        CPPUNIT_TEST( StreamFallbackMultibyte );
    CPPUNIT_TEST_SUITE_END();

    // expected converter state, UTF-8 without BOM by default
    struct ConvState
    {
        ConvState( wxBOM bom = wxBOM_None,
                   wxFontEncoding enc = wxFONTENCODING_UTF8,
                   bool fallback = false )
            : m_bom(bom), m_enc(enc), m_fallback(fallback) {}

        void Check(const wxConvAuto& conv) const
        {
            CPPUNIT_ASSERT( conv.GetBOM() == m_bom );
            CPPUNIT_ASSERT( conv.GetEncoding() == m_enc );
            CPPUNIT_ASSERT( conv.IsUsingFallbackEncoding() == m_fallback );
            CPPUNIT_ASSERT( conv.IsUTF8() == (m_enc == wxFONTENCODING_UTF8) );
        }

        wxBOM m_bom;
        wxFontEncoding m_enc;
        bool m_fallback;
    };

    // real test function: check that converting the src multibyte string to
    // wide char using wxConvAuto yields wch as the first result
    //
    // the length of the string may need to be passed explicitly if it has
    // embedded NULs, otherwise it's not necessary
    void TestFirstChar(const char *src, wchar_t wch, size_t len = wxNO_LEN,
                       ConvState st = ConvState(),
                       wxFontEncoding fe = wxFONTENCODING_DEFAULT);

    void Init();
    void Empty();
    void Encode();
    void Short();
    void None();
    void UTF32LE();
    void UTF32BE();
    void UTF16LE();
    void UTF16BE();
    void UTF8();
    void UTF8NoBom();
    void Fallback();
    void FallbackMultibyte();
    void FallbackShort();

    // test whether two lines of text are converted properly from a stream
    void TestTextStream(const char *src,
                        size_t srclength,
                        const wxString& line1,
                        const wxString& line2,
                        wxFontEncoding fe = wxFONTENCODING_DEFAULT);

    void StreamUTF8NoBOM();
    void StreamUTF8();
    void StreamUTF16LE();
    void StreamUTF16BE();
    void StreamUTF32LE();
    void StreamUTF32BE();
    void StreamFallback();
    void StreamFallbackMultibyte();
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(ConvAutoTestCase);

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ConvAutoTestCase, "ConvAutoTestCase");

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

void ConvAutoTestCase::TestFirstChar(const char *src, wchar_t wch, size_t len,
                                     ConvState st, wxFontEncoding fe)
{
    wxConvAuto conv(fe);
    wxWCharBuffer wbuf = conv.cMB2WC(src, len, nullptr);
    CPPUNIT_ASSERT( wbuf );
    CPPUNIT_ASSERT_EQUAL( wch, *wbuf );
    st.Check(conv);
}

void ConvAutoTestCase::Init()
{
    ConvState(wxBOM_Unknown, wxFONTENCODING_MAX).Check(wxConvAuto());
}

void ConvAutoTestCase::Empty()
{
    wxConvAuto conv;
    CPPUNIT_ASSERT( !conv.cMB2WC("") );
    ConvState(wxBOM_Unknown, wxFONTENCODING_MAX).Check(conv);
}

void ConvAutoTestCase::Encode()
{
    wxConvAuto conv;
    wxString str = wxString::FromUTF8("\xd0\x9f\xe3\x81\x82");
    wxCharBuffer buf = conv.cWC2MB(str.wc_str());
    CPPUNIT_ASSERT( buf );
    CPPUNIT_ASSERT_EQUAL( str, wxString::FromUTF8(buf) );
    ConvState(wxBOM_Unknown, wxFONTENCODING_UTF8).Check(conv);
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
    TestFirstChar("\xff\xfe\0\0A\0\0\0", wxT('A'), 8, ConvState(wxBOM_UTF32LE, wxFONTENCODING_UTF32LE));
}

void ConvAutoTestCase::UTF32BE()
{
    TestFirstChar("\0\0\xfe\xff\0\0\0B", wxT('B'), 8, ConvState(wxBOM_UTF32BE, wxFONTENCODING_UTF32BE));
}

void ConvAutoTestCase::UTF16LE()
{
    TestFirstChar("\xff\xfeZ\0", wxT('Z'), 4, ConvState(wxBOM_UTF16LE, wxFONTENCODING_UTF16LE));
}

void ConvAutoTestCase::UTF16BE()
{
    TestFirstChar("\xfe\xff\0Y", wxT('Y'), 4, ConvState(wxBOM_UTF16BE, wxFONTENCODING_UTF16BE));
}

void ConvAutoTestCase::UTF8()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\xef\xbb\xbf\xd0\x9f", L'\u041f', wxNO_LEN, ConvState(wxBOM_UTF8, wxFONTENCODING_UTF8));
#endif
}

void ConvAutoTestCase::UTF8NoBom()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\xd0\x9f\xe3\x81\x82", L'\u041f', wxNO_LEN, ConvState(wxBOM_None, wxFONTENCODING_UTF8));
#endif
}

void ConvAutoTestCase::Fallback()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\xbf", L'\u041f', wxNO_LEN,
                  ConvState(wxBOM_None, wxFONTENCODING_ISO8859_5, true),
                  wxFONTENCODING_ISO8859_5);
#endif
}

void ConvAutoTestCase::FallbackMultibyte()
{
#ifdef wxHAVE_U_ESCAPE
    TestFirstChar("\x84\x50", L'\u041f', wxNO_LEN,
                  ConvState(wxBOM_None, wxFONTENCODING_CP932, true),
                  wxFONTENCODING_CP932);
#endif
}

void ConvAutoTestCase::FallbackShort()
{
    TestFirstChar("\x61\xc4", 'a', 2,
                  ConvState(wxBOM_None, wxFONTENCODING_ISO8859_5, true),
                  wxFONTENCODING_ISO8859_5);
}

void ConvAutoTestCase::TestTextStream(const char *src,
                                      size_t srclength,
                                      const wxString& line1,
                                      const wxString& line2,
                                      wxFontEncoding fe)
{
    wxMemoryInputStream instream(src, srclength);
    wxTextInputStream text(instream, wxT(" \t"), wxConvAuto(fe));

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
    TestTextStream("\x61\xE3\x81\x82\x0A\xCE\xB2",
                   7, line1, line2);
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

void ConvAutoTestCase::StreamFallback()
{
    TestTextStream("\x61\xbf\x0A\xe0",
                   4, wxString::FromUTF8("a\xd0\x9f"), wxString::FromUTF8("\xd1\x80"),
                   wxFONTENCODING_ISO8859_5);
}

void ConvAutoTestCase::StreamFallbackMultibyte()
{
    TestTextStream("\x61\x82\xa0\x0A\x83\xc0",
                   6, line1, line2, wxFONTENCODING_CP932);
}

#endif // wxUSE_UNICODE
