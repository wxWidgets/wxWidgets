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

#include "wx/convauto.h"

#include "wx/mstream.h"
#include "wx/txtstrm.h"

// ----------------------------------------------------------------------------
// test helpers
// ----------------------------------------------------------------------------

namespace
{

// expected converter state, UTF-8 without BOM by default
struct ConvState
{
    ConvState( wxBOM bom = wxBOM_None,
               wxFontEncoding enc = wxFONTENCODING_UTF8,
               bool fallback = false )
        : m_bom(bom), m_enc(enc), m_fallback(fallback) {}

    void Check(const wxConvAuto& conv) const
    {
        CHECK( conv.GetBOM() == m_bom );
        CHECK( conv.GetEncoding() == m_enc );
        CHECK( conv.IsUsingFallbackEncoding() == m_fallback );
        CHECK( conv.IsUTF8() == (m_enc == wxFONTENCODING_UTF8) );
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
                   wxFontEncoding fe = wxFONTENCODING_DEFAULT)
{
    wxConvAuto conv(fe);
    wxWCharBuffer wbuf = conv.cMB2WC(src, len, nullptr);
    REQUIRE( wbuf );
    CHECK( *wbuf == wch );
    st.Check(conv);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("ConvAuto::Init", "[convauto]")
{
    ConvState(wxBOM_Unknown, wxFONTENCODING_MAX).Check(wxConvAuto());
}

TEST_CASE("ConvAuto::Empty", "[convauto]")
{
    wxConvAuto conv;
    CHECK( !conv.cMB2WC("") );
    ConvState(wxBOM_Unknown, wxFONTENCODING_MAX).Check(conv);
}

TEST_CASE("ConvAuto::Encode", "[convauto]")
{
    wxConvAuto conv;
    wxString str = wxString::FromUTF8("Пあ");
    wxCharBuffer buf = conv.cWC2MB(str.wc_str());
    CHECK( buf );
    CHECK( wxString::FromUTF8(buf) == str );
    ConvState(wxBOM_Unknown, wxFONTENCODING_UTF8).Check(conv);
}

TEST_CASE("ConvAuto::Short", "[convauto]")
{
    TestFirstChar("1", wxT('1'));
}

TEST_CASE("ConvAuto::None", "[convauto]")
{
    TestFirstChar("Hello world", wxT('H'));
}

TEST_CASE("ConvAuto::UTF32LE", "[convauto]")
{
    TestFirstChar("\xff\xfe\0\0A\0\0\0", wxT('A'), 8, ConvState(wxBOM_UTF32LE, wxFONTENCODING_UTF32LE));
}

TEST_CASE("ConvAuto::UTF32BE", "[convauto]")
{
    TestFirstChar("\0\0\xfe\xff\0\0\0B", wxT('B'), 8, ConvState(wxBOM_UTF32BE, wxFONTENCODING_UTF32BE));
}

TEST_CASE("ConvAuto::UTF16LE", "[convauto]")
{
    TestFirstChar("\xff\xfeZ\0", wxT('Z'), 4, ConvState(wxBOM_UTF16LE, wxFONTENCODING_UTF16LE));
}

TEST_CASE("ConvAuto::UTF16BE", "[convauto]")
{
    TestFirstChar("\xfe\xff\0Y", wxT('Y'), 4, ConvState(wxBOM_UTF16BE, wxFONTENCODING_UTF16BE));
}

#ifdef wxMUST_USE_U_ESCAPE
constexpr wchar_t CYRILLIC_LETTER_P = L'\u041f';
#else
constexpr wchar_t CYRILLIC_LETTER_P = L'П';
#endif

TEST_CASE("ConvAuto::UTF8", "[convauto]")
{
    TestFirstChar("\xef\xbb\xbfП", CYRILLIC_LETTER_P, wxNO_LEN, ConvState(wxBOM_UTF8, wxFONTENCODING_UTF8));
}

TEST_CASE("ConvAuto::UTF8NoBom", "[convauto]")
{
    TestFirstChar("Пあ", CYRILLIC_LETTER_P, wxNO_LEN, ConvState(wxBOM_None, wxFONTENCODING_UTF8));
}

TEST_CASE("ConvAuto::Fallback", "[convauto]")
{
    TestFirstChar("\xbf", CYRILLIC_LETTER_P, wxNO_LEN,
                  ConvState(wxBOM_None, wxFONTENCODING_ISO8859_5, true),
                  wxFONTENCODING_ISO8859_5);
}

TEST_CASE("ConvAuto::FallbackMultibyte", "[convauto]")
{
    TestFirstChar("\x84\x50", CYRILLIC_LETTER_P, wxNO_LEN,
                  ConvState(wxBOM_None, wxFONTENCODING_CP932, true),
                  wxFONTENCODING_CP932);
}

TEST_CASE("ConvAuto::FallbackShort", "[convauto]")
{
    TestFirstChar("\x61\xc4", 'a', 2,
                  ConvState(wxBOM_None, wxFONTENCODING_ISO8859_5, true),
                  wxFONTENCODING_ISO8859_5);
}

// test whether two lines of text are converted properly from a stream
static void TestTextStream(const char *src,
                           size_t srclength,
                           const wxString& line1,
                           const wxString& line2,
                           wxFontEncoding fe = wxFONTENCODING_DEFAULT)
{
    wxMemoryInputStream instream(src, srclength);
    wxTextInputStream text(instream, wxT(" \t"), wxConvAuto(fe));

    CHECK( text.ReadLine() == line1 );
    CHECK( text.ReadLine() == line2 );
}

// the first line of the teststring used in the following functions is an
// 'a' followed by a Japanese hiragana A (u+3042).
// The second line is a single Greek beta (u+03B2). There is no blank line
// at the end.

namespace
{

const wxString line1 = wxString::FromUTF8("aあ");
const wxString line2 = wxString::FromUTF8("β");

} // anonymous namespace

TEST_CASE("ConvAuto::StreamUTF8NoBOM", "[convauto]")
{
    TestTextStream("\x61\xE3\x81\x82\x0A\xCE\xB2",
                   7, line1, line2);
}

TEST_CASE("ConvAuto::StreamUTF8", "[convauto]")
{
    TestTextStream("\xEF\xBB\xBF\x61\xE3\x81\x82\x0A\xCE\xB2",
                   10, line1, line2);
}

TEST_CASE("ConvAuto::StreamUTF16LE", "[convauto]")
{
    TestTextStream("\xFF\xFE\x61\x00\x42\x30\x0A\x00\xB2\x03",
                   10, line1, line2);
}

TEST_CASE("ConvAuto::StreamUTF16BE", "[convauto]")
{
    TestTextStream("\xFE\xFF\x00\x61\x30\x42\x00\x0A\x03\xB2",
                   10, line1, line2);
}

TEST_CASE("ConvAuto::StreamUTF32LE", "[convauto]")
{
    TestTextStream("\xFF\xFE\0\0\x61\x00\0\0\x42\x30\0\0\x0A"
                   "\x00\0\0\xB2\x03\0\0",
                   20, line1, line2);
}

TEST_CASE("ConvAuto::StreamUTF32BE", "[convauto]")
{
    TestTextStream("\0\0\xFE\xFF\0\0\x00\x61\0\0\x30\x42\0\0\x00\x0A"
                   "\0\0\x03\xB2",
                   20, line1, line2);
}

TEST_CASE("ConvAuto::StreamFallback", "[convauto]")
{
    TestTextStream("\x61\xbf\x0A\xe0",
                   4, wxString::FromUTF8("a\xd0\x9f"), wxString::FromUTF8("\xd1\x80"),
                   wxFONTENCODING_ISO8859_5);
}

TEST_CASE("ConvAuto::StreamFallbackMultibyte", "[convauto]")
{
    TestTextStream("\x61\x82\xa0\x0A\x83\xc0",
                   6, line1, line2, wxFONTENCODING_CP932);
}
