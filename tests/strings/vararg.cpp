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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/string.h"

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("StringPrintf", "[wxString][Printf][vararg]")
{
    wxString s, s2;

    // test passing literals:
    s.Printf("%s %i", "foo", 42);
    CHECK( s == "foo 42" );
    s.Printf("%s %s %i", wxT("bar"), "=", 11);

    // test passing c_str():
    CHECK( s == "bar = 11" );
    s2.Printf("(%s)", s.c_str());
    CHECK( s2 == "(bar = 11)" );
    s2.Printf(wxT("[%s](%s)"), s.c_str(), "str");
    CHECK( s2 == "[bar = 11](str)" );

    s2.Printf("%s mailbox", wxString("Opening").c_str());
    CHECK( s2 == "Opening mailbox" );

    // test passing wxString directly:
    s2.Printf(wxT("[%s](%s)"), s, "str");
    CHECK( s2 == "[bar = 11](str)" );

    // test passing wxCharBufferType<T>:
    s = "FooBar";
    s2.Printf(wxT("(%s)"), s.mb_str());
    CHECK( s2 == "(FooBar)" );
    s2.Printf(wxT("value=%s;"), s.wc_str());
    CHECK( s2 == "value=FooBar;" );

    // this tests correct passing of wxCStrData constructed from string
    // literal (and we disable the warnings related to the use of a literal
    // here because we want to test that this compiles, even with warnings):
    wxGCC_WARNING_SUPPRESS(write-strings)
    wxCLANG_WARNING_SUPPRESS(c++11-compat-deprecated-writable-strings)

    bool cond = true;
    s2.Printf(wxT("foo %s"), !cond ? s.c_str() : wxT("bar"));

    wxGCC_WARNING_RESTORE(write-strings)
    wxCLANG_WARNING_RESTORE(c++11-compat-deprecated-writable-strings)

#ifdef __cpp_lib_string_view
    CHECK( wxString::Format("%s", std::string_view{"foobar", 3}) == "foo" );
    CHECK( wxString::Format("%s", std::string_view{"bar"}) == "bar" );
#endif // __cpp_lib_string_view
}

TEST_CASE("CharPrintf", "[wxString][Printf][vararg]")
{
    wxString foo("foo");
    wxString s;

    // test using wchar_t:
    s.Printf("char=%c", L'c');
    CHECK( s == "char=c" );

    // test wxUniCharRef:
    s.Printf("string[1] is %c", foo[1]);
    CHECK( s == "string[1] is o" );

    // test char
    char c = 'z';
    s.Printf("%c to %c", 'a', c);
    CHECK( s == "a to z" );

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
    CHECK( s == wxString("value is -16 (int)") );
    #endif

    unsigned char u = 240;
    s.Printf("value is %i (int)", u);
    CHECK( s == "value is 240 (int)" );
}

TEST_CASE("SizetPrintf", "[wxString][Printf][vararg]")
{
    size_t  i =  1;
    ssize_t j = -2;

    CHECK( wxString::Format("size_t=%zu ssize_t=%zd", i, j)
                == "size_t=1 ssize_t=-2" );

    CHECK( wxString::Format("size_t=0x%zX", static_cast<size_t>(160))
                == "size_t=0xA0" );
}

#if wxUSE_STD_STRING
TEST_CASE("StdString", "[wxString][Printf][vararg]")
{
    // test passing std::[w]string
    wxString s;

    std::string mb("multi-byte");
    std::string wc("widechar");

    s.Printf("string %s(%i).", mb, 1);
    CHECK( s == "string multi-byte(1)." );

    s.Printf("string %s(%i).", wc, 2);
    CHECK( s == "string widechar(2)." );
}
#endif // wxUSE_STD_STRING

#if wxUSE_LONGLONG
TEST_CASE("LongLongPrintf", "[wxString][Printf][vararg]")
{
    const char * const llfmt = "%" wxLongLongFmtSpec "d";

    CHECK( wxString::Format(llfmt, wxLL(17)) == "17" );

    wxLongLong ll = 1234567890;
    CHECK( wxString::Format(llfmt, ll) == "1234567890" );
}
#endif // wxUSE_LONGLONG

TEST_CASE("Sscanf", "[wxSscanf][vararg]")
{
    int i = 0;
    char str[20];

    wxString input("42 test");

    wxSscanf(input, "%d %s", &i, &str);
    CHECK( i == 42 );
    CHECK( wxString(str) == "test" );

#if !(defined(__MINGW32__) && \
      defined(__USE_MINGW_ANSI_STDIO) && __USE_MINGW_ANSI_STDIO == 1)
    // disable this test on mingw with __USE_MINGW_ANSI_STDIO=1
    // to prevent a segmentation fault. See:
    // https://sourceforge.net/p/mingw-w64/mailman/message/36118530/
    wchar_t wstr[20];

    i = 0;
    wxSscanf(input, L"%d %s", &i, &wstr);
    CHECK( i == 42 );
    CHECK( wxString(wstr) == "test" );
#endif
}

TEST_CASE("RepeatedPrintf", "[wxString][Printf][vararg]")
{
    wxCharBuffer buffer(2);
    char *p = buffer.data();
    *p = 'h';
    p++;
    *p = 'i';

    wxString s;
    s = wxString::Format("buffer %s, len %d", buffer, (int)wxStrlen(buffer));
    CHECK( s == "buffer hi, len 2" );

    s = wxString::Format("buffer %s, len %d", buffer, (int)wxStrlen(buffer));
    CHECK( s == "buffer hi, len 2" );
}

TEST_CASE("ArgsValidation", "[wxString][vararg][error]")
{
    int written;
    void *ptr = &written;
    short int swritten = 0;
    wxUnusedVar(swritten); // We're not really going to use it.

    // these are valid:
    wxString::Format("a string(%s,%s), ptr %p, int %i",
                     wxString(), "foo", "char* as pointer", 1);

    // Unfortunately we can't check the result as different standard libraries
    // implementations format it in different ways, so just check that it
    // compiles.
    wxString::Format("null pointer is %p", nullptr);

    // Microsoft has helpfully disabled support for "%n" in their CRT by
    // default starting from VC8 and somehow even calling
    // _set_printf_count_output() doesn't help here, so don't use "%n" at all
    // with it.
#if defined(__VISUALC__)
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
    CHECK( written == 5 );
#endif

    // but these are not:
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%i", "foo") );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%s", (void*)&written) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("%d", ptr) );

    // we don't check wxNO_PRINTF_PERCENT_N here as these expressions should
    // result in an assert in our code before the CRT functions are even called
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%i%n", &written) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%n", ptr) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxString::Format("foo%i%n", 42, &swritten) );

    // %c should accept integers too
    wxString::Format("%c", 80);
    wxString::Format("%c", wxChar(80) + wxChar(1));

    // check size_t handling
    size_t len = sizeof(ptr);
#ifdef __WINDOWS__
    wxString::Format("%Iu", len);
#else
    wxString::Format("%zu", len);
#endif
}

TEST_CASE("VeryLongArg", "[wxString][Format][vararg]")
{
    const size_t LENGTH = 70000;
    wxString veryLongString('.', LENGTH);
    REQUIRE( veryLongString.length() == LENGTH );

    const wxString s = wxString::Format("%s", veryLongString);

    // Check the length first to avoid very long output if this fails.
    REQUIRE( s.length() == LENGTH );
    CHECK( s == veryLongString );
}

namespace
{

// Helpers for the "PrintfError" test: we must pass by these functions
// because specifying "%c" directly inline would convert it to "%lc" and avoid
// the error.
wxString CallPrintfV(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    wxString s;
    s.PrintfV(wxString::FromAscii(format), ap);
    va_end(ap);
    return s;
}

} // anonymous namespace

TEST_CASE("PrintfError", "[wxString][Format][vararg][error]")
{
    // Check that using invalid argument doesn't keep doubling the buffer until
    // we run out of memory and die.
    const int invalidChar = 0x1780;
    REQUIRE_NOTHROW( CallPrintfV("%c", invalidChar) );
}
