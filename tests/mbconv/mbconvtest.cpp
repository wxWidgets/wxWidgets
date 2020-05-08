///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/main.cpp
// Purpose:     wxMBConv unit test
// Author:      Vadim Zeitlin, Mike Wetherell, Vince Harron
// Created:     14.02.04
// Copyright:   (c) 2003 TT-Solutions, (c) 2005 Mike Wetherell, Vince Harron
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/strconv.h"
#include "wx/string.h"
#include "wx/txtstrm.h"
#include "wx/mstream.h"

#if defined wxHAVE_TCHAR_SUPPORT && !defined HAVE_WCHAR_H
    #define HAVE_WCHAR_H
#endif

// ----------------------------------------------------------------------------
// Some wide character constants. "\uXXXX" escapes aren't supported by old
// compilers such as VC++ 5 and g++ 2.95.
// ----------------------------------------------------------------------------

wchar_t u41[] = { 0x41, 0 };
wchar_t u7f[] = { 0x7f, 0 };

wchar_t u80[] = { 0x80, 0 };
wchar_t u391[] = { 0x391, 0 };
wchar_t u7ff[] = { 0x7ff, 0 };

wchar_t u800[] = { 0x800, 0 };
wchar_t u2620[] = { 0x2620, 0 };
wchar_t ufffd[] = { 0xfffd, 0 };

#if SIZEOF_WCHAR_T == 4
wchar_t u10000[] = { 0x10000, 0 };
wchar_t u1000a5[] = { 0x1000a5, 0 };
wchar_t u10fffd[] = { 0x10fffd, 0 };
#else
wchar_t u10000[] = { 0xd800, 0xdc00, 0 };
wchar_t u1000a5[] = { 0xdbc0, 0xdca5, 0 };
wchar_t u10fffd[] = { 0xdbff, 0xdffd, 0 };
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MBConvTestCase : public CppUnit::TestCase
{
public:
    MBConvTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MBConvTestCase );
        CPPUNIT_TEST( UTF32LETests );
        CPPUNIT_TEST( UTF32BETests );
        CPPUNIT_TEST( WC2CP1250 );
        CPPUNIT_TEST( UTF7Tests );
        CPPUNIT_TEST( UTF8Tests );
        CPPUNIT_TEST( UTF16LETests );
        CPPUNIT_TEST( UTF16BETests );
        CPPUNIT_TEST( CP932Tests );
        CPPUNIT_TEST( CP1252Tests ); // depends on UTF8 Decoder functioning correctly
        CPPUNIT_TEST( LibcTests );
        CPPUNIT_TEST( IconvTests );
        CPPUNIT_TEST( Latin1Tests );
        CPPUNIT_TEST( FontmapTests );
        CPPUNIT_TEST( BufSize );
        CPPUNIT_TEST( FromWCharTests );
        CPPUNIT_TEST( NonBMPCharTests );
#ifdef HAVE_WCHAR_H
        CPPUNIT_TEST( UTF8_41 );
        CPPUNIT_TEST( UTF8_7f );
        CPPUNIT_TEST( UTF8_80 );
        CPPUNIT_TEST( UTF8_c2_7f );
        CPPUNIT_TEST( UTF8_c2_80 );
        CPPUNIT_TEST( UTF8_ce_91 );
        CPPUNIT_TEST( UTF8_df_bf );
        CPPUNIT_TEST( UTF8_df_c0 );
        CPPUNIT_TEST( UTF8_e0_a0_7f );
        CPPUNIT_TEST( UTF8_e0_a0_80 );
        CPPUNIT_TEST( UTF8_e2_98_a0 );
        CPPUNIT_TEST( UTF8_ef_bf_bd );
        CPPUNIT_TEST( UTF8_ef_bf_c0 );
        CPPUNIT_TEST( UTF8_f0_90_80_7f );
        CPPUNIT_TEST( UTF8_f0_90_80_80 );
        CPPUNIT_TEST( UTF8_f4_8f_bf_bd );
        CPPUNIT_TEST( UTF8PUA_f4_80_82_a5 );
        CPPUNIT_TEST( UTF8Octal_backslash245 );
#endif // HAVE_WCHAR_H
    CPPUNIT_TEST_SUITE_END();

    void WC2CP1250();
    void UTF7Tests();
    void UTF8Tests();
    void UTF16LETests();
    void UTF16BETests();
    void UTF32LETests();
    void UTF32BETests();
    void CP932Tests();
    void CP1252Tests();
    void LibcTests();
    void FontmapTests();
    void BufSize();
    void FromWCharTests();
    void NonBMPCharTests();
    void IconvTests();
    void Latin1Tests();

    // verifies that the specified multibyte sequence decodes to the specified wchar_t sequence
    void TestDecoder(
        const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
        size_t         wideChars,   // the number of wide characters at wideBuffer
        const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
        size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
        wxMBConv&      converter,   // the wxMBConv object that can decode multiBuffer into a wide character sequence
        int            sizeofNull   // number of bytes occupied by terminating null in this encoding
        );

    // verifies that the specified wchar_t sequence encodes to the specified multibyte sequence
    void TestEncoder(
        const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
        size_t         wideChars,   // the number of wide characters at wideBuffer
        const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
        size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
        wxMBConv&      converter,   // the wxMBConv object that can decode multiBuffer into a wide character sequence
        int            sizeofNull   // number of bytes occupied by terminating null in this encoding
        );

#if wxUSE_UNICODE && wxUSE_STREAMS
    // use wxTextInputStream to exercise wxMBConv interface
    // (this reveals some bugs in certain wxMBConv subclasses)
    void TestStreamDecoder(
        const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
        size_t         wideChars,   // the number of wide characters at wideBuffer
        const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
        size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
        wxMBConv&      converter    // the wxMBConv object that can decode multiBuffer into a wide character sequence
        );

    // use wxTextOutputStream to exercise wxMBConv interface
    // (this reveals some bugs in certain wxMBConv subclasses)
    void TestStreamEncoder(
        const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
        size_t         wideChars,   // the number of wide characters at wideBuffer
        const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
        size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
        wxMBConv&      converter    // the wxMBConv object that can decode multiBuffer into a wide character sequence
        );
#endif

    // tests the encoding and decoding capability of an wxMBConv object
    //
    // decodes the utf-8 bytes into wide characters
    // encodes the wide characters to compare against input multiBuffer
    // decodes the multiBuffer to compare against wide characters
    // decodes the multiBuffer into wide characters
    void TestCoder(
        const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
        size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
        const char*    utf8Buffer,  // the same character sequence as multiBuffer, encoded as UTF-8
        size_t         utf8Bytes,   // the byte length of the UTF-8 encoded character sequence
        wxMBConv&      converter,   // the wxMBConv object that can decode multiBuffer into a wide character sequence
        int            sizeofNull   // the number of bytes occupied by a terminating null in the converter's encoding
        );

#ifdef HAVE_WCHAR_H
    // UTF-8 tests. Test the first, last and one in the middle for sequences
    // of each length
    void UTF8_41() { UTF8("\x41", u41); }
    void UTF8_7f() { UTF8("\x7f", u7f); }
    void UTF8_80() { UTF8("\x80", NULL); }

    void UTF8_c2_7f() { UTF8("\xc2\x7f", NULL); }
    void UTF8_c2_80() { UTF8("\xc2\x80", u80); }
    void UTF8_ce_91() { UTF8("\xce\x91", u391); }
    void UTF8_df_bf() { UTF8("\xdf\xbf", u7ff); }
    void UTF8_df_c0() { UTF8("\xdf\xc0", NULL); }

    void UTF8_e0_a0_7f() { UTF8("\xe0\xa0\x7f", NULL); }
    void UTF8_e0_a0_80() { UTF8("\xe0\xa0\x80", u800); }
    void UTF8_e2_98_a0() { UTF8("\xe2\x98\xa0", u2620); }
    void UTF8_ef_bf_bd() { UTF8("\xef\xbf\xbd", ufffd); }
    void UTF8_ef_bf_c0() { UTF8("\xef\xbf\xc0", NULL); }

    void UTF8_f0_90_80_7f() { UTF8("\xf0\x90\x80\x7f", NULL); }
    void UTF8_f0_90_80_80() { UTF8("\xf0\x90\x80\x80", u10000); }
    void UTF8_f4_8f_bf_bd() { UTF8("\xf4\x8f\xbf\xbd", u10fffd); }

    // test 'escaping the escape characters' for the two escaping schemes
    void UTF8PUA_f4_80_82_a5() { UTF8PUA("\xf4\x80\x82\xa5", u1000a5); }
    void UTF8Octal_backslash245() { UTF8Octal("\\245", L"\\245"); }

    // Test that converting string with incomplete surrogates in them fails
    // (surrogates are only used in UTF-16, i.e. when wchar_t is 16 bits).
#if SIZEOF_WCHAR_T == 2
    void UTF8_fail_broken_surrogates();
#endif // SIZEOF_WCHAR_T == 2

    // implementation for the utf-8 tests (see comments below)
    void UTF8(const char *charSequence, const wchar_t *wideSequence);
    void UTF8PUA(const char *charSequence, const wchar_t *wideSequence);
    void UTF8Octal(const char *charSequence, const wchar_t *wideSequence);
    void UTF8(const char *charSequence, const wchar_t *wideSequence, int option);
#endif // HAVE_WCHAR_H

    wxDECLARE_NO_COPY_CLASS(MBConvTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MBConvTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MBConvTestCase, "MBConv" );

void MBConvTestCase::WC2CP1250()
{
    static const struct Data
    {
        const wchar_t *wc;
        const char *cp1250;
    } data[] =
    {
        { L"hello", "hello" },  // test that it works in simplest case
        { L"\xBD of \xBD is \xBC", NULL }, // this should fail as cp1250 doesn't have 1/2
    };

    wxCSConv cs1250(wxFONTENCODING_CP1250);
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        if (d.cp1250)
        {
            CPPUNIT_ASSERT( strcmp(cs1250.cWC2MB(d.wc), d.cp1250) == 0 );
        }
        else
        {
            CPPUNIT_ASSERT( (const char*)cs1250.cWC2MB(d.wc) == NULL );
        }
    }
}

// Print an unsigned character array as a C unsigned character array.
// NB: Please don't remove this function even though it's not used anywhere,
//     it's very useful when debugging a failed test.
wxString CByteArrayFormat( const void* data, size_t len, const wxChar* name )
{
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    wxString result;

    result.Printf( wxT("static const unsigned char %s[%i] = \n{"), name, (int)len );

    for ( size_t i = 0; i < len; i++ )
    {
        if ( i != 0 )
        {
            result.append( wxT(",") );
        }
        if ((i%16)==0)
        {
            result.append( wxT("\n    ") );
        }
        wxString byte = wxString::Format( wxT("0x%02x"), bytes[i] );
        result.append(byte);
    }
    result.append( wxT("\n};\n") );
    return result;
}

// The following bytes represent the same string, containing Japanese and English
// characters, encoded in several different formats.

// encoded by iconv
static const unsigned char welcome_utf7_iconv[84] =
{
    0x57,0x65,0x6c,0x63,0x6f,0x6d,0x65,0x20,0x74,0x6f,0x20,0x6f,0x75,0x72,0x20,0x63,
    0x79,0x62,0x65,0x72,0x20,0x73,0x70,0x61,0x63,0x65,0x20,0x66,0x6f,0x72,0x63,0x65,
    0x2e,0x20,0x20,0x2b,0x4d,0x46,0x6b,0x77,0x55,0x49,0x74,0x6d,0x57,0x39,0x38,0x77,
    0x61,0x35,0x62,0x37,0x69,0x6e,0x45,0x77,0x6b,0x6a,0x42,0x5a,0x4d,0x49,0x73,0x77,
    0x65,0x7a,0x42,0x47,0x4d,0x45,0x77,0x77,0x52,0x44,0x42,0x45,0x4d,0x47,0x63,0x77,
    0x57,0x54,0x41,0x43
};
// encoded by wxWindows (iconv can decode this successfully)
static const unsigned char welcome_utf7_wx[109] =
{
    0x57,0x65,0x6c,0x63,0x6f,0x6d,0x65,0x2b,0x41,0x43,0x41,0x2d,0x74,0x6f,0x2b,0x41,
    0x43,0x41,0x2d,0x6f,0x75,0x72,0x2b,0x41,0x43,0x41,0x2d,0x63,0x79,0x62,0x65,0x72,
    0x2b,0x41,0x43,0x41,0x2d,0x73,0x70,0x61,0x63,0x65,0x2b,0x41,0x43,0x41,0x2d,0x66,
    0x6f,0x72,0x63,0x65,0x2e,0x2b,0x41,0x43,0x41,0x41,0x49,0x44,0x42,0x5a,0x4d,0x46,
    0x43,0x4c,0x5a,0x6c,0x76,0x66,0x4d,0x47,0x75,0x57,0x2b,0x34,0x70,0x78,0x4d,0x4a,
    0x49,0x77,0x57,0x54,0x43,0x4c,0x4d,0x48,0x73,0x77,0x52,0x6a,0x42,0x4d,0x4d,0x45,
    0x51,0x77,0x52,0x44,0x42,0x6e,0x4d,0x46,0x6b,0x77,0x41,0x67,0x2d
};
// encoded by iconv
static const unsigned char welcome_utf8[89] =
{
    0x57,0x65,0x6c,0x63,0x6f,0x6d,0x65,0x20,0x74,0x6f,0x20,0x6f,0x75,0x72,0x20,0x63,
    0x79,0x62,0x65,0x72,0x20,0x73,0x70,0x61,0x63,0x65,0x20,0x66,0x6f,0x72,0x63,0x65,
    0x2e,0x20,0x20,0xe3,0x81,0x99,0xe3,0x81,0x90,0xe8,0xad,0xa6,0xe5,0xaf,0x9f,0xe3,
    0x81,0xab,0xe9,0x9b,0xbb,0xe8,0xa9,0xb1,0xe3,0x82,0x92,0xe3,0x81,0x99,0xe3,0x82,
    0x8b,0xe3,0x81,0xbb,0xe3,0x81,0x86,0xe3,0x81,0x8c,0xe3,0x81,0x84,0xe3,0x81,0x84,
    0xe3,0x81,0xa7,0xe3,0x81,0x99,0xe3,0x80,0x82
};
// encoded by iconv
static const unsigned char welcome_utf16le[106] =
{
    0x57,0x00,0x65,0x00,0x6c,0x00,0x63,0x00,0x6f,0x00,0x6d,0x00,0x65,0x00,0x20,0x00,
    0x74,0x00,0x6f,0x00,0x20,0x00,0x6f,0x00,0x75,0x00,0x72,0x00,0x20,0x00,0x63,0x00,
    0x79,0x00,0x62,0x00,0x65,0x00,0x72,0x00,0x20,0x00,0x73,0x00,0x70,0x00,0x61,0x00,
    0x63,0x00,0x65,0x00,0x20,0x00,0x66,0x00,0x6f,0x00,0x72,0x00,0x63,0x00,0x65,0x00,
    0x2e,0x00,0x20,0x00,0x20,0x00,0x59,0x30,0x50,0x30,0x66,0x8b,0xdf,0x5b,0x6b,0x30,
    0xfb,0x96,0x71,0x8a,0x92,0x30,0x59,0x30,0x8b,0x30,0x7b,0x30,0x46,0x30,0x4c,0x30,
    0x44,0x30,0x44,0x30,0x67,0x30,0x59,0x30,0x02,0x30
};
// encoded by iconv
static const unsigned char welcome_utf16be[106] =
{
    0x00,0x57,0x00,0x65,0x00,0x6c,0x00,0x63,0x00,0x6f,0x00,0x6d,0x00,0x65,0x00,0x20,
    0x00,0x74,0x00,0x6f,0x00,0x20,0x00,0x6f,0x00,0x75,0x00,0x72,0x00,0x20,0x00,0x63,
    0x00,0x79,0x00,0x62,0x00,0x65,0x00,0x72,0x00,0x20,0x00,0x73,0x00,0x70,0x00,0x61,
    0x00,0x63,0x00,0x65,0x00,0x20,0x00,0x66,0x00,0x6f,0x00,0x72,0x00,0x63,0x00,0x65,
    0x00,0x2e,0x00,0x20,0x00,0x20,0x30,0x59,0x30,0x50,0x8b,0x66,0x5b,0xdf,0x30,0x6b,
    0x96,0xfb,0x8a,0x71,0x30,0x92,0x30,0x59,0x30,0x8b,0x30,0x7b,0x30,0x46,0x30,0x4c,
    0x30,0x44,0x30,0x44,0x30,0x67,0x30,0x59,0x30,0x02
};
// encoded by iconv
static const unsigned char welcome_utf32le[212] =
{
    0x57,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x63,0x00,0x00,0x00,
    0x6f,0x00,0x00,0x00,0x6d,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
    0x74,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,
    0x75,0x00,0x00,0x00,0x72,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x63,0x00,0x00,0x00,
    0x79,0x00,0x00,0x00,0x62,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x72,0x00,0x00,0x00,
    0x20,0x00,0x00,0x00,0x73,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x61,0x00,0x00,0x00,
    0x63,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x66,0x00,0x00,0x00,
    0x6f,0x00,0x00,0x00,0x72,0x00,0x00,0x00,0x63,0x00,0x00,0x00,0x65,0x00,0x00,0x00,
    0x2e,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x59,0x30,0x00,0x00,
    0x50,0x30,0x00,0x00,0x66,0x8b,0x00,0x00,0xdf,0x5b,0x00,0x00,0x6b,0x30,0x00,0x00,
    0xfb,0x96,0x00,0x00,0x71,0x8a,0x00,0x00,0x92,0x30,0x00,0x00,0x59,0x30,0x00,0x00,
    0x8b,0x30,0x00,0x00,0x7b,0x30,0x00,0x00,0x46,0x30,0x00,0x00,0x4c,0x30,0x00,0x00,
    0x44,0x30,0x00,0x00,0x44,0x30,0x00,0x00,0x67,0x30,0x00,0x00,0x59,0x30,0x00,0x00,
    0x02,0x30,0x00,0x00
};
// encoded by iconv
static const unsigned char welcome_utf32be[212] =
{
    0x00,0x00,0x00,0x57,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x63,
    0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x6d,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x20,
    0x00,0x00,0x00,0x74,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x6f,
    0x00,0x00,0x00,0x75,0x00,0x00,0x00,0x72,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x63,
    0x00,0x00,0x00,0x79,0x00,0x00,0x00,0x62,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x72,
    0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x73,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x61,
    0x00,0x00,0x00,0x63,0x00,0x00,0x00,0x65,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x66,
    0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x72,0x00,0x00,0x00,0x63,0x00,0x00,0x00,0x65,
    0x00,0x00,0x00,0x2e,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x30,0x59,
    0x00,0x00,0x30,0x50,0x00,0x00,0x8b,0x66,0x00,0x00,0x5b,0xdf,0x00,0x00,0x30,0x6b,
    0x00,0x00,0x96,0xfb,0x00,0x00,0x8a,0x71,0x00,0x00,0x30,0x92,0x00,0x00,0x30,0x59,
    0x00,0x00,0x30,0x8b,0x00,0x00,0x30,0x7b,0x00,0x00,0x30,0x46,0x00,0x00,0x30,0x4c,
    0x00,0x00,0x30,0x44,0x00,0x00,0x30,0x44,0x00,0x00,0x30,0x67,0x00,0x00,0x30,0x59,
    0x00,0x00,0x30,0x02
};
// encoded by iconv
static const unsigned char welcome_cp932[71] =
{
    0x57,0x65,0x6c,0x63,0x6f,0x6d,0x65,0x20,0x74,0x6f,0x20,0x6f,0x75,0x72,0x20,0x63,
    0x79,0x62,0x65,0x72,0x20,0x73,0x70,0x61,0x63,0x65,0x20,0x66,0x6f,0x72,0x63,0x65,
    0x2e,0x20,0x20,0x82,0xb7,0x82,0xae,0x8c,0x78,0x8e,0x40,0x82,0xc9,0x93,0x64,0x98,
    0x62,0x82,0xf0,0x82,0xb7,0x82,0xe9,0x82,0xd9,0x82,0xa4,0x82,0xaa,0x82,0xa2,0x82,
    0xa2,0x82,0xc5,0x82,0xb7,0x81,0x42
};

#if wxBYTE_ORDER == wxBIG_ENDIAN
    #if SIZEOF_WCHAR_T == 2
        #define welcome_wchar_t welcome_utf16be
    #elif SIZEOF_WCHAR_T == 4
        #define welcome_wchar_t welcome_utf32be
    #endif
#elif wxBYTE_ORDER == wxLITTLE_ENDIAN
    #if SIZEOF_WCHAR_T == 2
        #define welcome_wchar_t welcome_utf16le
    #elif SIZEOF_WCHAR_T == 4
        #define welcome_wchar_t welcome_utf32le
    #endif
#endif

void MBConvTestCase::UTF7Tests()
{
#if 0
    wxCSConv convUTF7(wxFONTENCODING_UTF7);
#else
    wxMBConvUTF7 convUTF7;
#endif

    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf7_iconv,
        sizeof(welcome_utf7_iconv),
        convUTF7,
        1
        );
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf7_wx,
        sizeof(welcome_utf7_wx),
        convUTF7,
        1
        );
#if 0
    // wxWidget's UTF-7 encoder generates different byte sequences than iconv's.
    // but both seem to be equally legal.
    // This test won't work and that's okay.
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf7_iconv,
        sizeof(welcome_utf7_iconv),
        convUTF7,
        1
        );
#endif
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf7_wx,
        sizeof(welcome_utf7_wx),
        convUTF7,
        1
        );
}

void MBConvTestCase::UTF8Tests()
{
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf8,
        sizeof(welcome_utf8),
        wxConvUTF8,
        1
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf8,
        sizeof(welcome_utf8),
        wxConvUTF8,
        1
        );

#if SIZEOF_WCHAR_T == 2
    // Can't use \ud800 as it's an invalid Unicode character.
    const wchar_t wc = 0xd800;
    CPPUNIT_ASSERT_EQUAL(wxCONV_FAILED, wxConvUTF8.FromWChar(NULL, 0, &wc, 1));
#endif // SIZEOF_WCHAR_T == 2

    SECTION("UTF-8-FFFF")
    {
        const wchar_t wcFFFF = 0xFFFF;
        REQUIRE(wxConvUTF8.FromWChar(NULL, 0, &wcFFFF, 1) == 3);

        char buf[4];
        buf[3] = '\0';
        REQUIRE(wxConvUTF8.FromWChar(buf, 3, &wcFFFF, 1) == 3);

        CHECK(static_cast<unsigned char>(buf[0]) == 0xef);
        CHECK(static_cast<unsigned char>(buf[1]) == 0xbf);
        CHECK(static_cast<unsigned char>(buf[2]) == 0xbf);
    }
}

void MBConvTestCase::UTF16LETests()
{
    wxMBConvUTF16LE convUTF16LE;
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf16le,
        sizeof(welcome_utf16le),
        convUTF16LE,
        2
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf16le,
        sizeof(welcome_utf16le),
        convUTF16LE,
        2
        );
}

void MBConvTestCase::UTF16BETests()
{
    wxMBConvUTF16BE convUTF16BE;
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf16be,
        sizeof(welcome_utf16be),
        convUTF16BE,
        2
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf16be,
        sizeof(welcome_utf16be),
        convUTF16BE,
        2
        );
}

void MBConvTestCase::UTF32LETests()
{
    wxMBConvUTF32LE convUTF32LE;
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf32le,
        sizeof(welcome_utf32le),
        convUTF32LE,
        4
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf32le,
        sizeof(welcome_utf32le),
        convUTF32LE,
        4
        );
}

void MBConvTestCase::UTF32BETests()
{
    wxMBConvUTF32BE convUTF32BE;
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf32be,
        sizeof(welcome_utf32be),
        convUTF32BE,
        4
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_utf32be,
        sizeof(welcome_utf32be),
        convUTF32BE,
        4
        );
}

void MBConvTestCase::CP932Tests()
{
    wxCSConv convCP932( wxFONTENCODING_CP932 );
    TestDecoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_cp932,
        sizeof(welcome_cp932),
        convCP932,
        1
        );
    TestEncoder
        (
        (const wchar_t*)welcome_wchar_t,
        sizeof(welcome_wchar_t)/sizeof(wchar_t),
        (const char*)welcome_cp932,
        sizeof(welcome_cp932),
        convCP932,
        1
        );
}

// a character sequence encoded as iso8859-1 (iconv)
static const unsigned char iso8859_1[251] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,
    0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,
    0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,
    0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
    0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,
    0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,
    0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};
// the above character sequence encoded as UTF-8 (iconv)
static const unsigned char iso8859_1_utf8[379] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0xc2,0x80,0xc2,0x81,0xc2,
    0x82,0xc2,0x83,0xc2,0x84,0xc2,0x85,0xc2,0x86,0xc2,0x87,0xc2,0x88,0xc2,0x89,0xc2,
    0x8a,0xc2,0x8b,0xc2,0x8c,0xc2,0x8d,0xc2,0x8e,0xc2,0x8f,0xc2,0x90,0xc2,0x91,0xc2,
    0x92,0xc2,0x93,0xc2,0x94,0xc2,0x95,0xc2,0x96,0xc2,0x97,0xc2,0x98,0xc2,0x99,0xc2,
    0x9a,0xc2,0x9b,0xc2,0x9c,0xc2,0x9d,0xc2,0x9e,0xc2,0x9f,0xc2,0xa0,0xc2,0xa1,0xc2,
    0xa2,0xc2,0xa3,0xc2,0xa4,0xc2,0xa5,0xc2,0xa6,0xc2,0xa7,0xc2,0xa8,0xc2,0xa9,0xc2,
    0xaa,0xc2,0xab,0xc2,0xac,0xc2,0xad,0xc2,0xae,0xc2,0xaf,0xc2,0xb0,0xc2,0xb1,0xc2,
    0xb2,0xc2,0xb3,0xc2,0xb4,0xc2,0xb5,0xc2,0xb6,0xc2,0xb7,0xc2,0xb8,0xc2,0xb9,0xc2,
    0xba,0xc2,0xbb,0xc2,0xbc,0xc2,0xbd,0xc2,0xbe,0xc2,0xbf,0xc3,0x80,0xc3,0x81,0xc3,
    0x82,0xc3,0x83,0xc3,0x84,0xc3,0x85,0xc3,0x86,0xc3,0x87,0xc3,0x88,0xc3,0x89,0xc3,
    0x8a,0xc3,0x8b,0xc3,0x8c,0xc3,0x8d,0xc3,0x8e,0xc3,0x8f,0xc3,0x90,0xc3,0x91,0xc3,
    0x92,0xc3,0x93,0xc3,0x94,0xc3,0x95,0xc3,0x96,0xc3,0x97,0xc3,0x98,0xc3,0x99,0xc3,
    0x9a,0xc3,0x9b,0xc3,0x9c,0xc3,0x9d,0xc3,0x9e,0xc3,0x9f,0xc3,0xa0,0xc3,0xa1,0xc3,
    0xa2,0xc3,0xa3,0xc3,0xa4,0xc3,0xa5,0xc3,0xa6,0xc3,0xa7,0xc3,0xa8,0xc3,0xa9,0xc3,
    0xaa,0xc3,0xab,0xc3,0xac,0xc3,0xad,0xc3,0xae,0xc3,0xaf,0xc3,0xb0,0xc3,0xb1,0xc3,
    0xb2,0xc3,0xb3,0xc3,0xb4,0xc3,0xb5,0xc3,0xb6,0xc3,0xb7,0xc3,0xb8,0xc3,0xb9,0xc3,
    0xba,0xc3,0xbb,0xc3,0xbc,0xc3,0xbd,0xc3,0xbe,0xc3,0xbf
};

// a character sequence encoded as CP1252 (iconv)
static const unsigned char CP1252[246] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0xa0,0xa1,0xa2,0xa3,0xa4,
    0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,
    0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
    0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,
    0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,
    0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,0x8c,0x9c,0x8a,0x9a,0x9f,
    0x8e,0x9e,0x83,0x88,0x98,0x96,0x97,0x91,0x92,0x82,0x93,0x94,0x84,0x86,0x87,0x95,
    0x85,0x89,0x8b,0x9b,0x80,0x99
};
// the above character sequence encoded as UTF-8 (iconv)
static const unsigned char CP1252_utf8[386] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0xc2,0xa0,0xc2,0xa1,0xc2,
    0xa2,0xc2,0xa3,0xc2,0xa4,0xc2,0xa5,0xc2,0xa6,0xc2,0xa7,0xc2,0xa8,0xc2,0xa9,0xc2,
    0xaa,0xc2,0xab,0xc2,0xac,0xc2,0xad,0xc2,0xae,0xc2,0xaf,0xc2,0xb0,0xc2,0xb1,0xc2,
    0xb2,0xc2,0xb3,0xc2,0xb4,0xc2,0xb5,0xc2,0xb6,0xc2,0xb7,0xc2,0xb8,0xc2,0xb9,0xc2,
    0xba,0xc2,0xbb,0xc2,0xbc,0xc2,0xbd,0xc2,0xbe,0xc2,0xbf,0xc3,0x80,0xc3,0x81,0xc3,
    0x82,0xc3,0x83,0xc3,0x84,0xc3,0x85,0xc3,0x86,0xc3,0x87,0xc3,0x88,0xc3,0x89,0xc3,
    0x8a,0xc3,0x8b,0xc3,0x8c,0xc3,0x8d,0xc3,0x8e,0xc3,0x8f,0xc3,0x90,0xc3,0x91,0xc3,
    0x92,0xc3,0x93,0xc3,0x94,0xc3,0x95,0xc3,0x96,0xc3,0x97,0xc3,0x98,0xc3,0x99,0xc3,
    0x9a,0xc3,0x9b,0xc3,0x9c,0xc3,0x9d,0xc3,0x9e,0xc3,0x9f,0xc3,0xa0,0xc3,0xa1,0xc3,
    0xa2,0xc3,0xa3,0xc3,0xa4,0xc3,0xa5,0xc3,0xa6,0xc3,0xa7,0xc3,0xa8,0xc3,0xa9,0xc3,
    0xaa,0xc3,0xab,0xc3,0xac,0xc3,0xad,0xc3,0xae,0xc3,0xaf,0xc3,0xb0,0xc3,0xb1,0xc3,
    0xb2,0xc3,0xb3,0xc3,0xb4,0xc3,0xb5,0xc3,0xb6,0xc3,0xb7,0xc3,0xb8,0xc3,0xb9,0xc3,
    0xba,0xc3,0xbb,0xc3,0xbc,0xc3,0xbd,0xc3,0xbe,0xc3,0xbf,0xc5,0x92,0xc5,0x93,0xc5,
    0xa0,0xc5,0xa1,0xc5,0xb8,0xc5,0xbd,0xc5,0xbe,0xc6,0x92,0xcb,0x86,0xcb,0x9c,0xe2,
    0x80,0x93,0xe2,0x80,0x94,0xe2,0x80,0x98,0xe2,0x80,0x99,0xe2,0x80,0x9a,0xe2,0x80,
    0x9c,0xe2,0x80,0x9d,0xe2,0x80,0x9e,0xe2,0x80,0xa0,0xe2,0x80,0xa1,0xe2,0x80,0xa2,
    0xe2,0x80,0xa6,0xe2,0x80,0xb0,0xe2,0x80,0xb9,0xe2,0x80,0xba,0xe2,0x82,0xac,0xe2,
    0x84,0xa2
};

// this is unused currently so avoid warnings about this
#if 0

// a character sequence encoded as iso8859-5 (iconv)
static const unsigned char iso8859_5[251] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,
    0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa0,0xfd,0xad,0xa1,0xa2,
    0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,
    0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,
    0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,
    0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,
    0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf1,0xf2,0xf3,0xf4,
    0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfe,0xff,0xf0
};
// the above character sequence encoded as UTF-8 (iconv)
static const unsigned char iso8859_5_utf8[380] =
{
    0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,
    0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,
    0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,
    0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,
    0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0xc2,0x80,0xc2,0x81,0xc2,
    0x82,0xc2,0x83,0xc2,0x84,0xc2,0x85,0xc2,0x86,0xc2,0x87,0xc2,0x88,0xc2,0x89,0xc2,
    0x8a,0xc2,0x8b,0xc2,0x8c,0xc2,0x8d,0xc2,0x8e,0xc2,0x8f,0xc2,0x90,0xc2,0x91,0xc2,
    0x92,0xc2,0x93,0xc2,0x94,0xc2,0x95,0xc2,0x96,0xc2,0x97,0xc2,0x98,0xc2,0x99,0xc2,
    0x9a,0xc2,0x9b,0xc2,0x9c,0xc2,0x9d,0xc2,0x9e,0xc2,0x9f,0xc2,0xa0,0xc2,0xa7,0xc2,
    0xad,0xd0,0x81,0xd0,0x82,0xd0,0x83,0xd0,0x84,0xd0,0x85,0xd0,0x86,0xd0,0x87,0xd0,
    0x88,0xd0,0x89,0xd0,0x8a,0xd0,0x8b,0xd0,0x8c,0xd0,0x8e,0xd0,0x8f,0xd0,0x90,0xd0,
    0x91,0xd0,0x92,0xd0,0x93,0xd0,0x94,0xd0,0x95,0xd0,0x96,0xd0,0x97,0xd0,0x98,0xd0,
    0x99,0xd0,0x9a,0xd0,0x9b,0xd0,0x9c,0xd0,0x9d,0xd0,0x9e,0xd0,0x9f,0xd0,0xa0,0xd0,
    0xa1,0xd0,0xa2,0xd0,0xa3,0xd0,0xa4,0xd0,0xa5,0xd0,0xa6,0xd0,0xa7,0xd0,0xa8,0xd0,
    0xa9,0xd0,0xaa,0xd0,0xab,0xd0,0xac,0xd0,0xad,0xd0,0xae,0xd0,0xaf,0xd0,0xb0,0xd0,
    0xb1,0xd0,0xb2,0xd0,0xb3,0xd0,0xb4,0xd0,0xb5,0xd0,0xb6,0xd0,0xb7,0xd0,0xb8,0xd0,
    0xb9,0xd0,0xba,0xd0,0xbb,0xd0,0xbc,0xd0,0xbd,0xd0,0xbe,0xd0,0xbf,0xd1,0x80,0xd1,
    0x81,0xd1,0x82,0xd1,0x83,0xd1,0x84,0xd1,0x85,0xd1,0x86,0xd1,0x87,0xd1,0x88,0xd1,
    0x89,0xd1,0x8a,0xd1,0x8b,0xd1,0x8c,0xd1,0x8d,0xd1,0x8e,0xd1,0x8f,0xd1,0x91,0xd1,
    0x92,0xd1,0x93,0xd1,0x94,0xd1,0x95,0xd1,0x96,0xd1,0x97,0xd1,0x98,0xd1,0x99,0xd1,
    0x9a,0xd1,0x9b,0xd1,0x9c,0xd1,0x9e,0xd1,0x9f,0xe2,0x84,0x96
};
#endif // 0

// DecodeUTF8
// decodes the specified *unterminated* UTF-8 byte array
wxWCharBuffer DecodeUTF8(
    const void* data, // an unterminated UTF-8 encoded byte array
    size_t size       // the byte length of data
    )
{
    // the decoder requires a null terminated buffer.
    // the input data is not null terminated.
    // copy to null terminated buffer

    wxCharBuffer nullTerminated( size+1 );
    memcpy( nullTerminated.data(), data, size );
    nullTerminated.data()[size] = 0;
    return wxConvUTF8.cMB2WC(nullTerminated.data());
}

// tests the encoding and decoding capability of an wxMBConv object
//
// decodes the utf-8 bytes into wide characters
// encodes the wide characters to compare against input multiBuffer
// decodes the multiBuffer to compare against wide characters
// decodes the multiBuffer into wide characters
void MBConvTestCase::TestCoder(
    const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
    size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
    const char*    utf8Buffer,  // the same character sequence as multiBuffer, encoded as UTF-8
    size_t         utf8Bytes,   // the byte length of the UTF-8 encoded character sequence
    wxMBConv&      converter,   // the wxMBConv object thta can decode multiBuffer into a wide character sequence
    int            sizeofNull   // the number of bytes occupied by a terminating null in the converter's encoding
    )
{
    // wide character size and endian-ess varies from platform to platform
    // compiler support for wide character literals varies from compiler to compiler
    // so we should store the wide character version as UTF-8 and depend on
    // the UTF-8 converter's ability to decode it to platform specific wide characters
    // this test is invalid if the UTF-8 converter can't decode
    const wxWCharBuffer wideBuffer(DecodeUTF8(utf8Buffer, utf8Bytes));
    const size_t wideChars = wxWcslen(wideBuffer);

    TestDecoder
        (
        wideBuffer.data(),
        wideChars,
        multiBuffer,
        multiBytes,
        converter,
        sizeofNull
        );
    TestEncoder
        (
        wideBuffer.data(),
        wideChars,
        multiBuffer,
        multiBytes,
        converter,
        sizeofNull
        );
}


WXDLLIMPEXP_BASE wxMBConv* new_wxMBConv_wxwin( const char* name );

void MBConvTestCase::FontmapTests()
{
#ifdef wxUSE_FONTMAP
    wxMBConv* converter = new_wxMBConv_wxwin("CP1252");
    if ( !converter )
    {
        return;
    }
    TestCoder(
        (const char*)CP1252,
        sizeof(CP1252),
        (const char*)CP1252_utf8,
        sizeof(CP1252_utf8),
        *converter,
        1
        );
    delete converter;
#endif
}

void MBConvTestCase::BufSize()
{
    wxCSConv conv1251(wxT("CP1251"));
    CPPUNIT_ASSERT( conv1251.IsOk() );
    const char *cp1251text =
        "\313\301\326\305\324\323\321 \325\304\301\336\316\331\315";

    const size_t lenW = conv1251.MB2WC(NULL, cp1251text, 0);
    CPPUNIT_ASSERT_EQUAL( strlen(cp1251text), lenW );
    wxWCharBuffer wbuf(lenW + 1); // allocates lenW + 2 characters
    wbuf.data()[lenW + 1] = L'!';

    // lenW is not enough because it's the length and we need the size
    CPPUNIT_ASSERT_EQUAL(
        wxCONV_FAILED, conv1251.MB2WC(wbuf.data(), cp1251text, lenW) );

    // lenW+1 is just fine
    CPPUNIT_ASSERT(
        conv1251.MB2WC(wbuf.data(), cp1251text, lenW + 1) != wxCONV_FAILED );

    // of course, greater values work too
    CPPUNIT_ASSERT(
        conv1251.MB2WC(wbuf.data(), cp1251text, lenW + 2) != wxCONV_FAILED );

    // but they shouldn't write more stuff to the buffer
    CPPUNIT_ASSERT_EQUAL( L'!', wbuf[lenW + 1] );


    // test in the other direction too, using an encoding with multibyte NUL
    wxCSConv convUTF16(wxT("UTF-16LE"));
    CPPUNIT_ASSERT( convUTF16.IsOk() );
    const wchar_t *utf16text = L"Hello";

    const size_t lenMB = convUTF16.WC2MB(NULL, utf16text, 0);
    CPPUNIT_ASSERT_EQUAL( wcslen(utf16text)*2, lenMB );
    wxCharBuffer buf(lenMB + 2); // it only adds 1 for NUL on its own, we need 2
                                 // for NUL and an extra one for the guard byte
    buf.data()[lenMB + 2] = '?';

    CPPUNIT_ASSERT_EQUAL(
        wxCONV_FAILED, convUTF16.WC2MB(buf.data(), utf16text, lenMB) );
    CPPUNIT_ASSERT_EQUAL(
        wxCONV_FAILED, convUTF16.WC2MB(buf.data(), utf16text, lenMB + 1) );
    CPPUNIT_ASSERT(
        convUTF16.WC2MB(buf.data(), utf16text, lenMB + 2) != wxCONV_FAILED );
    CPPUNIT_ASSERT(
        convUTF16.WC2MB(buf.data(), utf16text, lenMB + 3) != wxCONV_FAILED );
    CPPUNIT_ASSERT_EQUAL( '?', buf[lenMB + 2] );

    // Test cWC2MB() too.
    const wxCharBuffer buf2 = convUTF16.cWC2MB(utf16text);
    CHECK( buf2.length() == lenMB );
    CHECK( memcmp(buf, buf2, lenMB) == 0 );

    const wxWCharBuffer utf16buf = wxWCharBuffer::CreateNonOwned(utf16text);
    const wxCharBuffer buf3 = convUTF16.cWC2MB(utf16buf);
    CHECK( buf3.length() == lenMB );
    CHECK( memcmp(buf, buf3, lenMB) == 0 );
}

void MBConvTestCase::FromWCharTests()
{
    wxCSConv conv950("CP950");
    char mbuf[10];
    // U+4e00 is 2 bytes (0xa4 0x40) in cp950
    wchar_t wbuf[] = { 0x4e00, 0, 0x4e00, 0 };

    // test simple ASCII text
    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 0, L"a", 1));
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[0]);

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( 1, conv950.FromWChar(mbuf, 1, L"a", 1));
    CPPUNIT_ASSERT_EQUAL( 'a', mbuf[0]);
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[1]);

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 1, L"a", 2));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( 2, conv950.FromWChar(mbuf, 2, L"a", 2));
    CPPUNIT_ASSERT_EQUAL( 'a', mbuf[0]);
    CPPUNIT_ASSERT_EQUAL( '\0', mbuf[1]);
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[2]);

    // test non-ASCII text, 1 wchar -> 2 char
    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 0, wbuf, 1));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 1, wbuf, 1));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( 2, conv950.FromWChar(mbuf, 2, wbuf, 1));
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[2]);

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 2, wbuf, 2));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( 3, conv950.FromWChar(mbuf, 3, wbuf, 2));
    CPPUNIT_ASSERT_EQUAL( '\0', mbuf[2]);
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[3]);

    // test text with embedded NUL-character and srcLen specified
    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 3, wbuf, 3));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 4, wbuf, 3));
    CPPUNIT_ASSERT_EQUAL( 5, conv950.FromWChar(mbuf, 5, wbuf, 3));
    CPPUNIT_ASSERT_EQUAL( '\0', mbuf[2]);
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[5]);

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( wxCONV_FAILED, conv950.FromWChar(mbuf, 5, wbuf, 4));

    memset(mbuf, '!', sizeof(mbuf));
    CPPUNIT_ASSERT_EQUAL( 6, conv950.FromWChar(mbuf, 6, wbuf, 4));
    CPPUNIT_ASSERT_EQUAL( '\0', mbuf[2]);
    CPPUNIT_ASSERT_EQUAL( '\0', mbuf[5]);
    CPPUNIT_ASSERT_EQUAL( '!', mbuf[6]);
}

void MBConvTestCase::NonBMPCharTests()
{
    // U+1F363 (UTF-16: D83C DF63, UTF-8: F0 9F 8D A3) sushi (emoji)
    // U+732B (UTF-8: E7 8C AB) cat (kanji)
    // U+1F408 (UTF-16: D83D DC08, UTF-8: F0 9F 90 88) cat (emoji)
    // U+845B U+E0101 (UTF-16: 845B DB40 DD01, UTF-8: E8 91 9B F3 A0 84 81) (a kanji + an IVS)
    const char u8[] =
        "\xF0\x9F\x8D\xA3" /* U+1F363 */
        "\xE7\x8C\xAB\xF0\x9F\x90\x88" /* U+732B U+1F408 */
        "\xE8\x91\x9B\xF3\xA0\x84\x81"; /* U+845B U+E0101 */
    const wxChar16 u16[] = {
        0xD83C, 0xDF63,
        0x732B, 0xD83D, 0xDC08,
        0x845B, 0xDB40, 0xDD01,
        0};
    const wxChar32 u32[] = {
        0x1F363,
        0x732B, 0x1F408,
        0x845B, 0xE0101,
        0};
#if SIZEOF_WCHAR_T == 2
    const wchar_t *const w = u16;
    const size_t wchars = sizeof(u16)/sizeof(wxChar16) - 1;
#else
    const wchar_t *const w = u32;
    const size_t wchars = sizeof(u32)/sizeof(wxChar32) - 1;
#endif
    {
        // Notice that these tests can only be done with strict UTF-8
        // converter, the use of any MAP_INVALID_UTF8_XXX options currently
        // completely breaks wxTextInputStream use.
        TestDecoder(w, wchars, u8, sizeof(u8)-1, wxConvUTF8, 1);
        TestEncoder(w, wchars, u8, sizeof(u8)-1, wxConvUTF8, 1);
    }
    SECTION("wxMBConvUTF16LE")
    {
        char u16le[sizeof(u16)];
        for (size_t i = 0; i < sizeof(u16)/2; ++i) {
            u16le[2*i]   = (char)(unsigned char)(u16[i] & 0xFF);
            u16le[2*i+1] = (char)(unsigned char)((u16[i] >> 8) & 0xFF);
        }
        wxMBConvUTF16LE conv;
        TestDecoder(w, wchars, u16le, sizeof(u16le)-2, conv, 2);
        TestEncoder(w, wchars, u16le, sizeof(u16le)-2, conv, 2);
    }
    SECTION("wxMBConvUTF16BE")
    {
        char u16be[sizeof(u16)];
        for (size_t i = 0; i < sizeof(u16)/2; ++i) {
            u16be[2*i]   = (char)(unsigned char)((u16[i] >> 8) & 0xFF);
            u16be[2*i+1] = (char)(unsigned char)(u16[i] & 0xFF);
        }
        wxMBConvUTF16BE conv;
        TestDecoder(w, wchars, u16be, sizeof(u16be)-2, conv, 2);
        TestEncoder(w, wchars, u16be, sizeof(u16be)-2, conv, 2);
    }
    SECTION("wxMBConvUTF32LE")
    {
        char u32le[sizeof(u32)];
        for (size_t i = 0; i < sizeof(u32)/4; ++i) {
            u32le[4*i]   = (char)(unsigned char)(u32[i] & 0xFF);
            u32le[4*i+1] = (char)(unsigned char)((u32[i] >> 8) & 0xFF);
            u32le[4*i+2] = (char)(unsigned char)((u32[i] >> 16) & 0xFF);
            u32le[4*i+3] = (char)(unsigned char)((u32[i] >> 24) & 0xFF);
        }
        wxMBConvUTF32LE conv;
        TestDecoder(w, wchars, u32le, sizeof(u32le)-4, conv, 4);
        TestEncoder(w, wchars, u32le, sizeof(u32le)-4, conv, 4);
    }
    SECTION("wxMBConvUTF32BE")
    {
        char u32be[sizeof(u32)];
        for (size_t i = 0; i < sizeof(u32)/4; ++i) {
            u32be[4*i]   = (char)(unsigned char)((u32[i] >> 24) & 0xFF);
            u32be[4*i+1] = (char)(unsigned char)((u32[i] >> 16) & 0xFF);
            u32be[4*i+2] = (char)(unsigned char)((u32[i] >> 8) & 0xFF);
            u32be[4*i+3] = (char)(unsigned char)(u32[i] & 0xFF);
        }
        wxMBConvUTF32BE conv;
        TestDecoder(w, wchars, u32be, sizeof(u32be)-4, conv, 4);
        TestEncoder(w, wchars, u32be, sizeof(u32be)-4, conv, 4);
    }
}

WXDLLIMPEXP_BASE wxMBConv* new_wxMBConv_iconv( const char* name );

void MBConvTestCase::IconvTests()
{
#ifdef HAVE_ICONV
    wxMBConv* converter = new_wxMBConv_iconv("CP932");
    if ( !converter )
    {
        return;
    }
    TestCoder(
        (const char*)welcome_cp932,
        sizeof(welcome_cp932),
        (const char*)welcome_utf8,
        sizeof(welcome_utf8),
        *converter,
        1
        );
    delete converter;
#endif
}

void MBConvTestCase::Latin1Tests()
{
    TestCoder(
        (const char*)iso8859_1,
        sizeof(iso8859_1),
        (const char*)iso8859_1_utf8,
        sizeof(iso8859_1_utf8),
        wxConvISO8859_1,
        1
        );

    static const char nulstr[] = "foo\0bar\0";
    static const size_t mbLen = WXSIZEOF(nulstr) - 1;
    size_t wcLen;
    wxConvISO8859_1.cMB2WC(nulstr, mbLen, &wcLen);
    CPPUNIT_ASSERT_EQUAL( mbLen, wcLen );
}

void MBConvTestCase::CP1252Tests()
{
    wxCSConv convCP1252( wxFONTENCODING_CP1252 );
    TestCoder(
        (const char*)CP1252,
        sizeof(CP1252),
        (const char*)CP1252_utf8,
        sizeof(CP1252_utf8),
        convCP1252,
        1
        );
}

void MBConvTestCase::LibcTests()
{
    // The locale name are OS-dependent so this test is done only under Windows
    // when using MSVC (surprisingly it fails with MinGW, even though it's
    // supposed to use the same CRT -- no idea why and unfortunately gdb is too
    // flaky to debug it)
#ifdef __VISUALC__
    LocaleSetter loc("English_United States.1252");

    wxMBConvLibc convLibc;
    TestCoder(
        (const char*)CP1252,
        sizeof(CP1252),
        (const char*)CP1252_utf8,
        sizeof(CP1252_utf8),
        convLibc,
        1
        );
#endif // __VISUALC__
}

// verifies that the specified mb sequences decode to the specified wc sequence
void MBConvTestCase::TestDecoder(
    const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
    size_t         wideChars,   // the number of wide characters at wideBuffer
    const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
    size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
    wxMBConv&      converter,   // the wxMBConv object that can decode multiBuffer into a wide character sequence
    int            sizeofNull   // number of bytes occupied by terminating null in this encoding
    )
{
    const unsigned UNINITIALIZED = 0xcd;

    // copy the input bytes into a null terminated buffer
    wxCharBuffer inputCopy( multiBytes+sizeofNull );
    memcpy( inputCopy.data(), multiBuffer, multiBytes );
    memset( &inputCopy.data()[multiBytes], 0, sizeofNull );

    // calculate the output size
    size_t outputWritten = converter.MB2WC
        (
        0,
        (const char*)inputCopy.data(),
        0
        );
    // make sure the correct output length was calculated
    WX_ASSERT_EQUAL_MESSAGE
    (
        ("while converting \"%s\"", multiBuffer),
        wideChars,
        outputWritten
    );

    // convert the string
    size_t guardChars = 8; // to make sure we're not overrunning the output buffer
    size_t nullCharacters = 1;
    size_t outputBufferChars = outputWritten + nullCharacters + guardChars;
    wxWCharBuffer outputBuffer(outputBufferChars);
    memset( outputBuffer.data(), UNINITIALIZED, outputBufferChars*sizeof(wchar_t) );

    outputWritten = converter.MB2WC
        (
        outputBuffer.data(),
        (const char*)inputCopy.data(),
        outputBufferChars
        );
    // make sure the correct number of characters were outputs
    CPPUNIT_ASSERT_EQUAL( wideChars, outputWritten );

    // make sure the characters generated are correct
    CPPUNIT_ASSERT( 0 == memcmp( outputBuffer, wideBuffer, wideChars*sizeof(wchar_t) ) );

    // the output buffer should be null terminated
    CPPUNIT_ASSERT(  outputBuffer[outputWritten] == 0 );

    // make sure the rest of the output buffer is untouched
    for ( size_t i = (wideChars+1)*sizeof(wchar_t); i < (outputBufferChars*sizeof(wchar_t)); i++ )
    {
        CPPUNIT_ASSERT( ((unsigned char*)outputBuffer.data())[i] == UNINITIALIZED );
    }

#if wxUSE_UNICODE && wxUSE_STREAMS
    TestStreamDecoder( wideBuffer, wideChars, multiBuffer, multiBytes, converter );
#endif
}

// verifies that the specified wc sequences encodes to the specified mb sequence
void MBConvTestCase::TestEncoder(
    const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
    size_t         wideChars,   // the number of wide characters at wideBuffer
    const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
    size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
    wxMBConv&      converter,   // the wxMBConv object that can decode multiBuffer into a wide character sequence
    int            sizeofNull   // number of bytes occupied by terminating null in this encoding
    )
{
    const unsigned UNINITIALIZED = 0xcd;

    // copy the input bytes into a null terminated buffer
    wxWCharBuffer inputCopy( wideChars + 1 );
    memcpy( inputCopy.data(), wideBuffer, (wideChars*sizeof(wchar_t)) );
    inputCopy.data()[wideChars] = 0;

    // calculate the output size: notice that it can be greater than the real
    // size as the converter is allowed to estimate the maximal size needed
    // instead of computing it precisely
    size_t outputWritten = converter.WC2MB
        (
        0,
        (const wchar_t*)inputCopy.data(),
        0
        );
    CPPUNIT_ASSERT( outputWritten >= multiBytes );

    // convert the string
    size_t guardBytes = 8; // to make sure we're not overrunning the output buffer
    size_t outputBufferSize = outputWritten + sizeofNull + guardBytes;
    wxCharBuffer outputBuffer(outputBufferSize);
    memset( outputBuffer.data(), UNINITIALIZED, outputBufferSize );

    outputWritten = converter.WC2MB
        (
        outputBuffer.data(),
        (const wchar_t*)inputCopy.data(),
        outputBufferSize
        );

    // make sure the correct number of characters were output
    CPPUNIT_ASSERT_EQUAL( multiBytes, outputWritten );

    // make sure the characters generated are correct
    CPPUNIT_ASSERT( 0 == memcmp( outputBuffer, multiBuffer, multiBytes ) );

    size_t i;

    // the output buffer should be null terminated
    for ( i = multiBytes; i < multiBytes + sizeofNull; i++ )
    {
        CPPUNIT_ASSERT( ((unsigned char*)outputBuffer.data())[i] == 0 );
    }

    // make sure the rest of the output buffer is untouched
    for ( i = multiBytes + sizeofNull; i < outputBufferSize; i++ )
    {
        CPPUNIT_ASSERT( ((unsigned char*)outputBuffer.data())[i] == UNINITIALIZED );
    }

#if wxUSE_UNICODE && wxUSE_STREAMS
    TestStreamEncoder( wideBuffer, wideChars, multiBuffer, multiBytes, converter );
#endif
}

#if wxUSE_UNICODE && wxUSE_STREAMS
// use wxTextInputStream to exercise wxMBConv interface
// (this reveals some bugs in certain wxMBConv subclasses)
void MBConvTestCase::TestStreamDecoder(
                                       const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
                                       size_t         wideChars,   // the number of wide characters at wideBuffer
                                       const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
                                       size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
                                       wxMBConv&      converter    // the wxMBConv object that can decode multiBuffer into a wide character sequence
                                       )
{
    // this isn't meant to test wxMemoryInputStream or wxTextInputStream
    // it's meant to test the way wxTextInputStream uses wxMBConv
    // (which has exposed some problems with wxMBConv)
    wxMemoryInputStream memoryInputStream( multiBuffer, multiBytes );
    wxTextInputStream textInputStream( memoryInputStream, wxT(""), converter );
    for ( size_t i = 0; i < wideChars; i++ )
    {
        wxChar wc = textInputStream.GetChar();
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            wxString::Format("At position %lu", (unsigned long)i).ToStdString(),
            wideBuffer[i],
            wc
        );
    }
    CPPUNIT_ASSERT( 0 == textInputStream.GetChar() );
    CPPUNIT_ASSERT( memoryInputStream.Eof() );
}
#endif

#if wxUSE_UNICODE && wxUSE_STREAMS
// use wxTextInputStream to exercise wxMBConv interface
// (this reveals some bugs in certain wxMBConv subclasses)
void MBConvTestCase::TestStreamEncoder(
    const wchar_t* wideBuffer,  // the same character sequence as multiBuffer, encoded as wchar_t
    size_t         wideChars,   // the number of wide characters at wideBuffer
    const char*    multiBuffer, // a multibyte encoded character sequence that can be decoded by "converter"
    size_t         multiBytes,  // the byte length of the multibyte character sequence that can be decoded by "converter"
    wxMBConv&      converter    // the wxMBConv object that can decode multiBuffer into a wide character sequence
    )
{
    // this isn't meant to test wxMemoryOutputStream or wxTextOutputStream
    // it's meant to test the way wxTextOutputStream uses wxMBConv
    // (which has exposed some problems with wxMBConv)
    wxMemoryOutputStream memoryOutputStream;
    // wxEOL_UNIX will pass \n \r unchanged
    wxTextOutputStream textOutputStream( memoryOutputStream, wxEOL_UNIX, converter );
    for ( size_t i = 0; i < wideChars; i++ )
    {
        textOutputStream.PutChar( wideBuffer[i] );
    }

    textOutputStream.Flush();

    CPPUNIT_ASSERT_EQUAL( multiBytes, size_t(memoryOutputStream.TellO()) );
    wxCharBuffer copy( memoryOutputStream.TellO() );
    memoryOutputStream.CopyTo( copy.data(), memoryOutputStream.TellO());
    CPPUNIT_ASSERT_EQUAL( 0, memcmp( copy.data(), multiBuffer, multiBytes ) );
}
#endif


// ----------------------------------------------------------------------------
// UTF-8 tests
// ----------------------------------------------------------------------------

#ifdef HAVE_WCHAR_H

// Check that 'charSequence' translates to 'wideSequence' and back.
// Invalid sequences can be tested by giving NULL for 'wideSequence'. Even
// invalid sequences should roundtrip when an option is given and this is
// checked.
//
void MBConvTestCase::UTF8(const char *charSequence,
                          const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// Use this alternative when 'charSequence' contains a PUA character. Such
// sequences should still roundtrip ok, and this is checked.
//
void MBConvTestCase::UTF8PUA(const char *charSequence,
                             const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, NULL, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// Use this alternative when 'charSequence' contains an octal escape sequence.
// Such sequences should still roundtrip ok, and this is checked.
//
void MBConvTestCase::UTF8Octal(const char *charSequence,
                               const wchar_t *wideSequence)
{
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_NOT);
    UTF8(charSequence, wideSequence, wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
    UTF8(charSequence, NULL, wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
}

// in case wcscpy is missing
//
static wchar_t *wx_wcscpy(wchar_t *dest, const wchar_t *src)
{
    wchar_t *d = dest;
    while ((*d++ = *src++) != 0)
        ;
    return dest;
}

// in case wcscat is missing
//
static wchar_t *wx_wcscat(wchar_t *dest, const wchar_t *src)
{
    wchar_t *d = dest;
    while (*d)
        d++;
    while ((*d++ = *src++) != 0)
        ;
    return dest;
}

// in case wcscmp is missing
//
static int wx_wcscmp(const wchar_t *s1, const wchar_t *s2)
{
    while (*s1 == *s2 && *s1 != 0)
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// in case wcslen is missing
//
static size_t wx_wcslen(const wchar_t *s)
{
    const wchar_t *t = s;
    while (*t != 0)
        t++;
    return t - s;
}

// include the option in the error messages so it's possible to see which
// test failed
#define UTF8ASSERT(expr) CPPUNIT_ASSERT_MESSAGE(#expr + errmsg,  expr)

// The test implementation
//
void MBConvTestCase::UTF8(const char *charSequence,
                          const wchar_t *wideSequence,
                          int option)
{
    const size_t BUFSIZE = 128;
    wxASSERT(strlen(charSequence) * 3 + 10 < BUFSIZE);
    char bytes[BUFSIZE];

    // include the option in the error messages so it's possible to see
    // which test failed
    sprintf(bytes, " (with option == %d)", option);
    std::string errmsg(bytes);

    // put the charSequence at the start, middle and end of a string
    strcpy(bytes, charSequence);
    strcat(bytes, "ABC");
    strcat(bytes, charSequence);
    strcat(bytes, "XYZ");
    strcat(bytes, charSequence);

    // translate it into wide characters
    wxMBConvUTF8 utf8(option);
    wchar_t widechars[BUFSIZE];
    size_t lenResult = utf8.MB2WC(NULL, bytes, 0);
    size_t result = utf8.MB2WC(widechars, bytes, BUFSIZE);
    UTF8ASSERT(result == lenResult);

    // check we got the expected result
    if (wideSequence) {
        UTF8ASSERT(result != (size_t)-1);
        wxASSERT(result < BUFSIZE);

        wchar_t expected[BUFSIZE];
        wx_wcscpy(expected, wideSequence);
        wx_wcscat(expected, L"ABC");
        wx_wcscat(expected, wideSequence);
        wx_wcscat(expected, L"XYZ");
        wx_wcscat(expected, wideSequence);

        UTF8ASSERT(wx_wcscmp(widechars, expected) == 0);
        UTF8ASSERT(wx_wcslen(widechars) == result);
    }
    else {
        // If 'wideSequence' is NULL, then the result is expected to be
        // invalid.  Normally that is as far as we can go, but if there is an
        // option then the conversion should succeed anyway, and it should be
        // possible to translate back to the original
        if (!option) {
            UTF8ASSERT(result == (size_t)-1);
            return;
        }
        else {
            UTF8ASSERT(result != (size_t)-1);
        }
    }

    // translate it back and check we get the original
    char bytesAgain[BUFSIZE];
    size_t lenResultAgain = utf8.WC2MB(NULL, widechars, 0);
    size_t resultAgain = utf8.WC2MB(bytesAgain, widechars, BUFSIZE);
    UTF8ASSERT(resultAgain == lenResultAgain);
    UTF8ASSERT(resultAgain != (size_t)-1);
    wxASSERT(resultAgain < BUFSIZE);

    UTF8ASSERT(strcmp(bytes, bytesAgain) == 0);
    UTF8ASSERT(strlen(bytesAgain) == resultAgain);
}

#endif // HAVE_WCHAR_H

TEST_CASE("wxMBConv::cWC2MB", "[mbconv][wc2mb]")
{
    wxMBConvUTF16 convUTF16;

    CHECK( convUTF16.cWC2MB(L"").length() == 0 );
    CHECK( convUTF16.cWC2MB(wxWCharBuffer()).length() == 0 );
    CHECK( convUTF16.cWC2MB(L"Hi").length() == 4 );
    CHECK( convUTF16.cWC2MB(wxWCharBuffer::CreateNonOwned(L"Hi")).length() == 4 );

    CHECK( wxConvUTF7.cWC2MB(L"").length() == 0 );
    CHECK( wxConvUTF7.cWC2MB(wxWCharBuffer()).length() == 0 );
    CHECK( wxConvUTF7.cWC2MB(L"\xa3").length() == 5 );
    // This test currently fails, the returned value is 3 because the
    // conversion object doesn't return to its unshifted state -- which is
    // probably a bug in wxMBConvUTF7.
    // TODO: fix it there and reenable the test.
    CHECK_NOFAIL( wxConvUTF7.cWC2MB(wxWCharBuffer::CreateNonOwned(L"\xa3")).length() == 5 );
}

TEST_CASE("wxMBConv::cMB2WC", "[mbconv][mb2wc]")
{
    wxMBConvUTF16 convUTF16;

    CHECK( convUTF16.cMB2WC("\0").length() == 0 );
    CHECK( convUTF16.cMB2WC(wxCharBuffer()).length() == 0 );
    CHECK( convUTF16.cMB2WC("H\0i\0\0").length() == 2 );
    CHECK( convUTF16.cMB2WC(wxCharBuffer::CreateNonOwned("H\0i\0\0", 4)).length() == 2 );

    CHECK( wxConvUTF7.cMB2WC("").length() == 0 );
    CHECK( wxConvUTF7.cMB2WC(wxCharBuffer()).length() == 0 );
    CHECK( wxConvUTF7.cMB2WC("+AKM-").length() == 1 );
}
