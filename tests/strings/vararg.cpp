///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/vararg.cpp
// Purpose:     Test for wx vararg look-alike macros
// Author:      Vaclav Slavik
// Created:     2007-02-20
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
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

#include "wx/string.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VarArgTestCase : public CppUnit::TestCase
{
public:
    VarArgTestCase() {}

private:
    CPPUNIT_TEST_SUITE( VarArgTestCase );
        CPPUNIT_TEST( StringPrintf );
        CPPUNIT_TEST( CharPrintf );
        CPPUNIT_TEST( SizetPrintf );
#if wxUSE_STD_STRING
        CPPUNIT_TEST( StdString );
#endif
#if wxUSE_LONGLONG
        CPPUNIT_TEST( LongLongPrintf );
#endif
        CPPUNIT_TEST( Sscanf );
        CPPUNIT_TEST( RepeatedPrintf );
        CPPUNIT_TEST( ArgsValidation );
    CPPUNIT_TEST_SUITE_END();

    void StringPrintf();
    void CharPrintf();
    void SizetPrintf();
#if wxUSE_STD_STRING
    void StdString();
#endif
#if wxUSE_LONGLONG
    void LongLongPrintf();
#endif
    void Sscanf();
    void RepeatedPrintf();
    void ArgsValidation();

    wxDECLARE_NO_COPY_CLASS(VarArgTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VarArgTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VarArgTestCase, "VarArgTestCase" );

void VarArgTestCase::StringPrintf()
{
    wxString s, s2;

    // test passing literals:
    s.Printf("%s %i", "foo", 42);
    CPPUNIT_ASSERT( s == "foo 42" );
    s.Printf("%s %s %i", wxT("bar"), "=", 11);

    // test passing c_str():
    CPPUNIT_ASSERT( s == "bar = 11" );
    s2.Printf("(%s)", s.c_str());
    CPPUNIT_ASSERT( s2 == "(bar = 11)" );
    s2.Printf(wxT("[%s](%s)"), s.c_str(), "str");
    CPPUNIT_ASSERT( s2 == "[bar = 11](str)" );

    s2.Printf("%s mailbox", wxString("Opening").c_str());
    CPPUNIT_ASSERT( s2 == "Opening mailbox" );

    // test passing wxString directly:
    s2.Printf(wxT("[%s](%s)"), s, "str");
    CPPUNIT_ASSERT( s2 == "[bar = 11](str)" );

    // test passing wxCharBufferType<T>:
    s = "FooBar";
    s2.Printf(wxT("(%s)"), s.mb_str());
    CPPUNIT_ASSERT( s2 == "(FooBar)" );
    s2.Printf(wxT("value=%s;"), s.wc_str());
    CPPUNIT_ASSERT( s2 == "value=FooBar;" );

    // this tests correct passing of wxCStrData constructed from string
    // literal (and we disable the warnings related to the use of a literal
    // here because we want to test that this compiles, even with warnings):
    wxGCC_WARNING_SUPPRESS(write-strings)
    wxCLANG_WARNING_SUPPRESS(c++11-compat-deprecated-writable-strings)

    bool cond = true;
    s2.Printf(wxT("foo %s"), !cond ? s.c_str() : wxT("bar"));

    wxGCC_WARNING_RESTORE(write-strings)
    wxCLANG_WARNING_RESTORE(c++11-compat-deprecated-writable-strings)
}

void VarArgTestCase::CharPrintf()
{
    wxString foo("foo");
    wxString s;

    // test using wchar_t:
    s.Printf("char=%c", L'c');
    CPPUNIT_ASSERT_EQUAL( "char=c", s );

    // test wxUniCharRef:
    s.Printf("string[1] is %c", foo[1]);
    CPPUNIT_ASSERT_EQUAL( "string[1] is o", s );

    // test char
    char c = 'z';
    s.Printf("%c to %c", 'a', c);
    CPPUNIT_ASSERT_EQUAL( "a to z", s );

    // test char used as integer:
    #ifdef _MSC_VER
        #pragma warning(disable:4309) // truncation of constant value
    #endif
    wxCLANG_WARNING_SUPPRESS(constant-conversion)
    c = 240;
    wxCLANG_WARNING_RESTORE(constant-conversion)
    #ifdef _MSC_VER
        #pragma warning(default:4309)
    #endif
    #ifndef __CHAR_UNSIGNED__
    s.Printf("value is %i (int)", c);
    CPPUNIT_ASSERT_EQUAL( wxString("value is -16 (int)"), s );
    #endif

    unsigned char u = 240;
    s.Printf("value is %i (int)", u);
    CPPUNIT_ASSERT_EQUAL( "value is 240 (int)", s );
}

void VarArgTestCase::SizetPrintf()
{
    size_t  i =  1;
    ssize_t j = -2;

    CPPUNIT_ASSERT_EQUAL
        (
            "size_t=1 ssize_t=-2",
            wxString::Format("size_t=%zu ssize_t=%zd", i, j)
        );

    CPPUNIT_ASSERT_EQUAL
        (
            "size_t=0xA0",
            wxString::Format("size_t=0x%zX", static_cast<size_t>(160))
        );
}

#if wxUSE_STD_STRING
void VarArgTestCase::StdString()
{
    // test passing std::[w]string
    wxString s;

    std::string mb("multi-byte");
    std::string wc("widechar");

    s.Printf("string %s(%i).", mb, 1);
    CPPUNIT_ASSERT_EQUAL( "string multi-byte(1).", s );

    s.Printf("string %s(%i).", wc, 2);
    CPPUNIT_ASSERT_EQUAL( "string widechar(2).", s );
}
#endif // wxUSE_STD_STRING

#if wxUSE_LONGLONG
void VarArgTestCase::LongLongPrintf()
{
    const char * const llfmt = "%" wxLongLongFmtSpec "d";

    CPPUNIT_ASSERT_EQUAL( "17", wxString::Format(llfmt, wxLL(17)) );

    wxLongLong ll = 1234567890;
    CPPUNIT_ASSERT_EQUAL( "1234567890", wxString::Format(llfmt, ll) );
}
#endif // wxUSE_LONGLONG

void VarArgTestCase::Sscanf()
{
    int i = 0;
    char str[20];

    wxString input("42 test");

    wxSscanf(input, "%d %s", &i, &str);
    CPPUNIT_ASSERT( i == 42 );
    CPPUNIT_ASSERT( wxString(str) == "test" );

#if !(defined(__MINGW32__) && \
      defined(__USE_MINGW_ANSI_STDIO) && __USE_MINGW_ANSI_STDIO == 1)
    // disable this test on mingw with __USE_MINGW_ANSI_STDIO=1
    // to prevent a segmentation fault. See:
    // https://sourceforge.net/p/mingw-w64/mailman/message/36118530/
    wchar_t wstr[20];

    i = 0;
    wxSscanf(input, L"%d %s", &i, &wstr);
    CPPUNIT_ASSERT( i == 42 );
    CPPUNIT_ASSERT( wxString(wstr) == "test" );
#endif
}

void VarArgTestCase::RepeatedPrintf()
{
    wxCharBuffer buffer(2);
    char *p = buffer.data();
    *p = 'h';
    p++;
    *p = 'i';

    wxString s;
    s = wxString::Format("buffer %s, len %d", buffer, (int)wxStrlen(buffer));
    CPPUNIT_ASSERT_EQUAL("buffer hi, len 2", s);

    s = wxString::Format("buffer %s, len %d", buffer, (int)wxStrlen(buffer));
    CPPUNIT_ASSERT_EQUAL("buffer hi, len 2", s);
}

void VarArgTestCase::ArgsValidation()
{
    void *ptr = this;
    int written;
    short int swritten;

    // these are valid:
    wxString::Format("a string(%s,%s), ptr %p, int %i",
                     wxString(), "foo", "char* as pointer", 1);

    // Microsoft has helpfully disabled support for "%n" in their CRT by
    // default starting from VC8 and somehow even calling
    // _set_printf_count_output() doesn't help here, so don't use "%n" at all
    // with it.
#if wxCHECK_VISUALC_VERSION(8)
    #define wxNO_PRINTF_PERCENT_N
#endif // VC8+

    // Similarly, many modern Linux distributions ship with g++ that uses
    // -D_FORTIFY_SOURCE=2 flag by default and this option prevents "%n" from
    // being used in a string outside of read-only memory, meaning that it
    // can't be used in wxString to which we (may, depending on build options)
    // assign it, so also disable testing of "%n" in this case lest we die with
    // an abort inside vswprintf().
#if defined(_FORTIFY_SOURCE)
    #if _FORTIFY_SOURCE >= 2
        #define wxNO_PRINTF_PERCENT_N
    #endif
#endif

#ifndef wxNO_PRINTF_PERCENT_N
    wxString::Format("foo%i%n", 42, &written);
    CPPUNIT_ASSERT_EQUAL( 5, written );
#endif

    // but these are not:
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%i", "foo") );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%s", (void*)this) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%d", ptr) );

    // we don't check wxNO_PRINTF_PERCENT_N here as these expressions should
    // result in an assert in our code before the CRT functions are even called
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%i%n", &written) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%n", ptr) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%i%n", 42, &swritten) );

    // the following test (correctly) fails at compile-time with <type_traits>
#if !defined(HAVE_TYPE_TRAITS) && !defined(HAVE_TR1_TYPE_TRAITS)
    wxObject obj;
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%s", obj) );

    wxObject& ref = obj;
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%s", ref) );
#endif

    // %c should accept integers too
    wxString::Format("%c", 80);
    wxString::Format("%c", wxChar(80) + wxChar(1));

    // check size_t handling
    size_t len = sizeof(*this);
#ifdef __WINDOWS__
    wxString::Format("%Iu", len);
#else
    wxString::Format("%zu", len);
#endif
}
