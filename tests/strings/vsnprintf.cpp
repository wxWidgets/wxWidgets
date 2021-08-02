///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/vsnprintf.cpp
// Purpose:     wxVsnprintf unit test
// Author:      Francesco Montorsi
//              (part of this file was taken from CMP.c of TRIO package
//               written by Bjorn Reese and Daniel Stenberg)
// Created:     2006-04-01
// Copyright:   (c) 2006 Francesco Montorsi, Bjorn Reese and Daniel Stenberg
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/crt.h"

#if wxUSE_WXVSNPRINTF

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/wxchar.h"
#endif // WX_PRECOMP

#include "wx/private/localeset.h"

// NOTE: for more info about the specification of wxVsnprintf() behaviour you can
//       refer to the following page of the GNU libc manual:
//             http://www.gnu.org/software/libc/manual/html_node/Formatted-Output.html


// ----------------------------------------------------------------------------
// global utilities for testing
// ----------------------------------------------------------------------------

#define MAX_TEST_LEN        1024

// temporary buffers
static wxChar buf[MAX_TEST_LEN];
int r;

// Helper macro verifying both the return value of wxSnprintf() and its output.
//
// NOTE: the expected string length with this macro must not exceed MAX_TEST_LEN

#define CMP(expected, fmt, ...)                          \
    r=wxSnprintf(buf, MAX_TEST_LEN, fmt, ##__VA_ARGS__); \
    CHECK( r == (int)wxStrlen(buf) );                    \
    CHECK( buf == wxString(expected) )

// Another helper which takes the size explicitly instead of using MAX_TEST_LEN
//
// NOTE: this macro is used also with too-small buffers (see Miscellaneous())
//       test function, thus the return value can be either -1 or > size and we
//       cannot check if r == (int)wxStrlen(buf)
#define CMPTOSIZE(buffer, size, failuremsg, expected, fmt, ...) \
    r=wxSnprintf(buffer, size, fmt, ##__VA_ARGS__);             \
    INFO(failuremsg);                                           \
    CHECK( buffer == wxString(expected).Left(size - 1) )

// this is the same as wxSnprintf() but it passes the format string to
// wxVsnprintf() without using WX_ATTRIBUTE_PRINTF and thus suppresses the gcc
// checks (and resulting warnings) for the format string
//
// use with extreme care and only when you're really sure the warnings must be
// suppressed!
template<typename T>
static int
wxUnsafeSnprintf(T *buf, size_t len, const wxChar *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int rc = wxVsnprintf(buf, len, fmt, args);

    va_end(args);

    return rc;
}

// ----------------------------------------------------------------------------
// test fixture
// ----------------------------------------------------------------------------

// Explicitly set C locale to avoid check failures when running on machines
// with a locale where the decimal point is not '.'
class VsnprintfTestCase : wxCLocaleSetter
{
public:
    VsnprintfTestCase() : wxCLocaleSetter() { }

protected:
    template<typename T>
        void DoBigToSmallBuffer(T *buffer, int size);

    // compares the expectedString and the result of wxVsnprintf() char by char
    // for all its length (not only for first expectedLen chars) and also
    // checks the return value
    void DoMisc(int expectedLen, const wxString& expectedString,
                size_t max, const wxChar *format, ...);

    wxDECLARE_NO_COPY_CLASS(VsnprintfTestCase);
};

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::C", "[vsnprintf]")
{
    CMP("hi!", "%c%c%c", wxT('h'), wxT('i'), wxT('!'));

    // NOTE:
    // the NULL characters _can_ be passed to %c to e.g. create strings
    // with embedded NULs (because strings are not always supposed to be
    // NUL-terminated).

    DoMisc(14, wxT("Hello \0 World!"), 16, wxT("Hello %c World!"), wxT('\0'));
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::D", "[vsnprintf]")
{
    CMP("+123456", "%+d", 123456);
    CMP("-123456", "%d", -123456);
    CMP(" 123456", "% d", 123456);
    CMP("    123456", "%10d", 123456);
    CMP("0000123456", "%010d", 123456);
    CMP("-123456   ", "%-10d", -123456);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::X", "[vsnprintf]")
{
    CMP("ABCD", "%X", 0xABCD);
    CMP("0XABCD", "%#X", 0xABCD);
    CMP("0xabcd", "%#x", 0xABCD);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::O", "[vsnprintf]")
{
    CMP("1234567", "%o", 01234567);
    CMP("01234567", "%#o", 01234567);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::P", "[vsnprintf]")
{
    // The exact format used for "%p" is not specified by the standard and so
    // varies among different platforms, so we need to expect different results
    // here (remember that while we test our own wxPrintf() code here, it uses
    // the system sprintf() for actual formatting so the results are still
    // different under different systems).

#if defined(__VISUALC__) || (defined(__MINGW32__) && \
        (!defined(__USE_MINGW_ANSI_STDIO) || !__USE_MINGW_ANSI_STDIO))
    #if SIZEOF_VOID_P == 4
        CMP("00ABCDEF", "%p", (void*)0xABCDEF);
        CMP("00000000", "%p", (void*)NULL);
    #elif SIZEOF_VOID_P == 8
        CMP("0000ABCDEFABCDEF", "%p", (void*)0xABCDEFABCDEF);
        CMP("0000000000000000", "%p", (void*)NULL);
    #endif
#elif defined(__MINGW32__)
    #if SIZEOF_VOID_P == 4
        CMP("00abcdef", "%p", (void*)0xABCDEF);
        CMP("00000000", "%p", (void*)NULL);
    #elif SIZEOF_VOID_P == 8
        CMP("0000abcdefabcdef", "%p", (void*)0xABCDEFABCDEF);
        CMP("0000000000000000", "%p", (void*)NULL);
    #endif
#elif defined(__GNUG__)
    // glibc prints pointers as %#x except for NULL pointers which are printed
    // as '(nil)'.
    CMP("0xabcdef", "%p", (void*)0xABCDEF);
    CMP("(nil)", "%p", (void*)NULL);
#endif
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::N", "[vsnprintf]")
{
    int nchar;

    wxSnprintf(buf, MAX_TEST_LEN, wxT("%d %s%n\n"), 3, wxT("bears"), &nchar);
    CHECK( nchar == 7 );
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::E", "[vsnprintf]")
{
    // NB: Use at least three digits for the exponent to workaround
    //     differences between MSVC, MinGW and GNU libc.
    //     See wxUSING_MANTISSA_SIZE_3 in testprec.h as well.
    //
    //     Some examples:
    //       printf("%e",2.342E+02);
    //     -> under MSVC7.1 prints:      2.342000e+002
    //     -> under GNU libc 2.4 prints: 2.342000e+02
    CMP("2.342000e+112", "%e",2.342E+112);
    CMP("-2.3420e-112", "%10.4e",-2.342E-112);
    CMP("-2.3420e-112", "%11.4e",-2.342E-112);
    CMP("   -2.3420e-112", "%15.4e",-2.342E-112);

    CMP("-0.02342", "%G",-2.342E-02);
    CMP("3.1415E-116", "%G",3.1415e-116);
    CMP("0003.141500e+103", "%016e", 3141.5e100);
    CMP("   3.141500e+103", "%16e", 3141.5e100);
    CMP("3.141500e+103   ", "%-16e", 3141.5e100);
    CMP("3.142e+103", "%010.3e", 3141.5e100);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::F", "[vsnprintf]")
{
    CMP("3.300000", "%5f", 3.3);
    CMP("3.000000", "%5f", 3.0);
    CMP("0.000100", "%5f", .999999E-4);
    CMP("0.000990", "%5f", .99E-3);
    CMP("3333.000000", "%5f", 3333.0);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::G", "[vsnprintf]")
{
    // NOTE: the same about E() testcase applies here...

    CMP("  3.3", "%5g", 3.3);
    CMP("    3", "%5g", 3.0);
    CMP("9.99999e-115", "%5g", .999999E-114);
    CMP("0.00099", "%5g", .99E-3);
    CMP(" 3333", "%5g", 3333.0);
    CMP(" 0.01", "%5g", 0.01);

    CMP("    3", "%5.g", 3.3);
    CMP("    3", "%5.g", 3.0);
    CMP("1e-114", "%5.g", .999999E-114);
    CMP("0.0001", "%5.g", 1.0E-4);
    CMP("0.001", "%5.g", .99E-3);
    CMP("3e+103", "%5.g", 3333.0E100);
    CMP(" 0.01", "%5.g", 0.01);

    CMP("  3.3", "%5.2g", 3.3);
    CMP("    3", "%5.2g", 3.0);
    CMP("1e-114", "%5.2g", .999999E-114);
    CMP("0.00099", "%5.2g", .99E-3);
    CMP("3.3e+103", "%5.2g", 3333.0E100);
    CMP(" 0.01", "%5.2g", 0.01);
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::S", "[vsnprintf]")
{
    CMP("  abc", "%5s", wxT("abc"));
    CMP("    a", "%5s", wxT("a"));
    CMP("abcdefghi", "%5s", wxT("abcdefghi"));
    CMP("abc  ", "%-5s", wxT("abc"));
    CMP("abcdefghi", "%-5s", wxT("abcdefghi"));

    CMP("abcde", "%.5s", wxT("abcdefghi"));

    // do the same tests but with Unicode characters:
#if wxUSE_UNICODE

    // Unicode code points from U+03B1 to U+03B9 are the greek letters alpha-iota;
    // UTF8 encoding of such code points is 0xCEB1 to 0xCEB9

#define ALPHA       "\xCE\xB1"
        // alpha
#define ABC         "\xCE\xB1\xCE\xB2\xCE\xB3"
        // alpha+beta+gamma
#define ABCDE       "\xCE\xB1\xCE\xB2\xCE\xB3\xCE\xB4\xCE\xB5"
        // alpha+beta+gamma+delta+epsilon
#define ABCDEFGHI   "\xCE\xB1\xCE\xB2\xCE\xB3\xCE\xB4\xCE\xB5\xCE\xB6\xCE\xB7\xCE\xB8\xCE\xB9"
        // alpha+beta+gamma+delta+epsilon+zeta+eta+theta+iota

    // the 'expected' and 'arg' parameters of this macro are supposed to be
    // UTF-8 strings
#define CMP_UTF8(expected, fmt, arg)                                          \
    CHECK                                                                     \
    (                                                                         \
        (int)wxString::FromUTF8(expected).length() ==                         \
        wxSnprintf(buf, MAX_TEST_LEN, fmt, wxString::FromUTF8(arg))           \
    );                                                                        \
    CHECK( wxString::FromUTF8(expected) == buf )

    CMP_UTF8("  " ABC,     "%5s",  ABC);
    CMP_UTF8("    " ALPHA, "%5s",  ALPHA);
    CMP_UTF8(ABCDEFGHI,    "%5s",  ABCDEFGHI);
    CMP_UTF8(ABC "  ",     "%-5s", ABC);
    CMP_UTF8(ABCDEFGHI,    "%-5s", ABCDEFGHI);
    CMP_UTF8(ABCDE,        "%.5s", ABCDEFGHI);
#endif // wxUSE_UNICODE

    // test a string which has a NULL character after "ab";
    // obviously it should be handled exactly like just as "ab"
    CMP("   ab", "%5s", wxT("ab\0cdefghi"));
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::Asterisk", "[vsnprintf]")
{
    CMP("       0.1", "%*.*f", 10, 1, 0.123);
    CMP("    0.1230", "%*.*f", 10, 4, 0.123);
    CMP("0.1", "%*.*f", 3, 1, 0.123);

    CMP("%0.002", "%%%.*f", 3, 0.0023456789);

    CMP("       a", "%*c", 8, 'a');
    CMP("    four", "%*s", 8, "four");
    CMP("    four   four", "%*s %*s", 8, "four", 6, "four");
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::Percent", "[vsnprintf]")
{
    // some tests without any argument passed through ...
    CMP("%", "%%");
    CMP("%%%", "%%%%%%");

    CMP("%  abc", "%%%5s", wxT("abc"));
    CMP("%  abc%", "%%%5s%%", wxT("abc"));

    // do not test odd number of '%' symbols as different implementations
    // of snprintf() give different outputs as this situation is not considered
    // by any standard (in fact, GCC will also warn you about a spurious % if
    // you write %%% as argument of some *printf function !)
    // Compare(wxT("%"), wxT("%%%"));
}

#ifdef wxLongLong_t
TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::LongLong", "[vsnprintf]")
{
    CMP("123456789", "%lld", (wxLongLong_t)123456789);
    CMP("-123456789", "%lld", (wxLongLong_t)-123456789);

    CMP("123456789", "%llu", (wxULongLong_t)123456789);

#ifdef __WINDOWS__
    CMP("123456789", "%I64d", (wxLongLong_t)123456789);
    CMP("123456789abcdef", "%I64x", wxLL(0x123456789abcdef));
#endif
}
#endif

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::WrongFormatStrings", "[vsnprintf]")
{
    // test how wxVsnprintf() behaves with wrong format string:

    // a missing positional arg should result in an assert
    WX_ASSERT_FAILS_WITH_ASSERT(
            wxSnprintf(buf, MAX_TEST_LEN, wxT("%1$d %3$d"), 1, 2, 3) );

    // positional and non-positionals in the same format string:
    errno = 0;
    r = wxSnprintf(buf, MAX_TEST_LEN, wxT("%1$d %d %3$d"), 1, 2, 3);
    CHECK( r == -1 );
    CHECK( errno == EINVAL );
}

// BigToSmallBuffer() test case helper:
template<typename T>
void VsnprintfTestCase::DoBigToSmallBuffer(T *buffer, int size)
{
    // Remember that wx*printf could be mapped either to system
    // implementation or to wx implementation.
    // In the first case, when the output buffer is too small, the returned
    // value can be the number of characters required for the output buffer
    // (conforming to ISO C99; implemented in e.g. GNU libc >= 2.1), or
    // just a negative number, usually -1; (this is how e.g. MSVC's
    // *printf() behaves). Luckily, in all implementations, when the
    // output buffer is too small, it's nonetheless filled up to its max size.
    //
    // Note that in the second case (i.e. when we're using our own implementation),
    // wxVsnprintf() will return the number of characters written in the standard
    // output or
    //   -1         if there was an error in the format string
    //   maxSize+1  if the output buffer is too small

    wxString errStr;
    errStr << "The size of the buffer was " << size;
    std::string errMsg(errStr.mb_str());

    // test without positionals
    CMPTOSIZE(buffer, size, errMsg,
              "123456789012 - test - 123 -4.567",
              "%i%li - test - %d %.3f",
              123, (long int)456789012, 123, -4.567);

#if wxUSE_PRINTF_POS_PARAMS
    // test with positional
    CMPTOSIZE(buffer, size, errMsg,
              "-4.567 123 - test - 456789012 123",
              "%4$.3f %1$i - test - %2$li %3$d",
              123, (long int)456789012, 123, -4.567);
#endif

    // test unicode/ansi conversion specifiers
    //
    // NB: we use wxUnsafeSnprintf() as %hs and %hc are invalid in printf
    //     format and gcc would warn about this otherwise

    r = wxUnsafeSnprintf(buffer, size,
                         wxT("unicode string/char: %ls/%lc -- ansi string/char: %hs/%hc"),
                         L"unicode", L'U', "ansi", 'A');
    wxString expected =
        wxString(wxT("unicode string/char: unicode/U -- ansi string/char: ansi/A")).Left(size - 1);

    CHECK( expected == buffer );
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::BigToSmallBuffer", "[vsnprintf]")
{
#if wxUSE_UNICODE
    wchar_t bufw[1024], bufw2[16], bufw3[4], bufw4;
    DoBigToSmallBuffer(bufw, 1024);
    DoBigToSmallBuffer(bufw2, 16);
    DoBigToSmallBuffer(bufw3, 4);
    DoBigToSmallBuffer(&bufw4, 1);
#endif // wxUSE_UNICODE

    char bufa[1024], bufa2[16], bufa3[4], bufa4;
    DoBigToSmallBuffer(bufa, 1024);
    DoBigToSmallBuffer(bufa2, 16);
    DoBigToSmallBuffer(bufa3, 4);
    DoBigToSmallBuffer(&bufa4, 1);
}

// Miscellaneous() test case helper:
void VsnprintfTestCase::DoMisc(
        int expectedLen,
        const wxString& expectedString,
        size_t max,
        const wxChar *format, ...)
{
    const size_t BUFSIZE = MAX_TEST_LEN - 1;
    size_t i;
    static int count = 0;

    wxASSERT(max <= BUFSIZE);

    for (i = 0; i < BUFSIZE; i++)
        buf[i] = '*';
    buf[BUFSIZE] = 0;

    va_list ap;
    va_start(ap, format);

    int n = wxVsnprintf(buf, max, format, ap);

    va_end(ap);

    // Prepare messages so that it is possible to see from the error which
    // test was running.
    wxString errStr, overflowStr;
    errStr << wxT("No.: ") << ++count << wxT(", expected: ") << expectedLen
           << wxT(" '") << expectedString << wxT("', result: ");
    overflowStr << errStr << wxT("buffer overflow");
    errStr << n << wxT(" '") << buf << wxT("'");

    // turn them into std::strings
    std::string errMsg(errStr.mb_str());
    std::string overflowMsg(overflowStr.mb_str());

    INFO(errMsg);
    if ( size_t(n) < max )
        CHECK(expectedLen == n);
    else
        CHECK(expectedLen == -1);

    CHECK(expectedString == buf);

    for (i = max; i < BUFSIZE; i++)
    {
        INFO(overflowMsg);
        CHECK(buf[i] == '*');
    }
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::Miscellaneous", "[vsnprintf]")
{
    // expectedLen, expectedString, max, format, ...
    DoMisc(5,  wxT("-1234"),   8, wxT("%d"), -1234);
    DoMisc(7,  wxT("1234567"), 8, wxT("%d"), 1234567);
    DoMisc(-1, wxT("1234567"), 8, wxT("%d"), 12345678);
    DoMisc(-1, wxT("-123456"), 8, wxT("%d"), -1234567890);

    DoMisc(6,  wxT("123456"),  8, wxT("123456"));
    DoMisc(7,  wxT("1234567"), 8, wxT("1234567"));
    DoMisc(-1, wxT("1234567"), 8, wxT("12345678"));

    DoMisc(6,  wxT("123450"),  8, wxT("12345%d"), 0);
    DoMisc(7,  wxT("1234560"), 8, wxT("123456%d"), 0);
    DoMisc(-1, wxT("1234567"), 8, wxT("1234567%d"), 0);
    DoMisc(-1, wxT("1234567"), 8, wxT("12345678%d"), 0);

    DoMisc(6,  wxT("12%45%"),  8, wxT("12%%45%%"));
    DoMisc(7,  wxT("12%45%7"), 8, wxT("12%%45%%7"));
    DoMisc(-1, wxT("12%45%7"), 8, wxT("12%%45%%78"));

    DoMisc(5,  wxT("%%%%%"),   6, wxT("%%%%%%%%%%"));
    DoMisc(6,  wxT("%%%%12"),  7, wxT("%%%%%%%%%d"), 12);
}


/* (C) Copyright C E Chew
*
* Feel free to copy, use and distribute this software provided:
*
*        1. you do not pretend that you wrote it
*        2. you leave this copyright notice intact.
*/

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::GlibcMisc1", "[vsnprintf]")
{
    CMP("     ",    "%5.s", "xyz");
    CMP("   33",    "%5.f", 33.3);
#if defined(wxDEFAULT_MANTISSA_SIZE_3)
    CMP("  3e+008", "%8.e", 33.3e7);
    CMP("  3E+008", "%8.E", 33.3e7);
    CMP("3e+001",    "%.g",  33.3);
    CMP("3E+001",    "%.G",  33.3);
#else
    CMP("   3e+08", "%8.e", 33.3e7);
    CMP("   3E+08", "%8.E", 33.3e7);
    CMP("3e+01",    "%.g",  33.3);
    CMP("3E+01",    "%.G",  33.3);
#endif
}

TEST_CASE_METHOD(VsnprintfTestCase, "Vsnprintf::GlibcMisc2", "[vsnprintf]")
{
    int prec;
    wxString test_format;

    prec = 0;
    CMP("3", "%.*g", prec, 3.3);

    prec = 0;
    CMP("3", "%.*G", prec, 3.3);

    prec = 0;
    CMP("      3", "%7.*G", prec, 3.33);

    prec = 3;
    CMP(" 041", "%04.*o", prec, 33);

    prec = 7;
    CMP("  0000033", "%09.*u", prec, 33);

    prec = 3;
    CMP(" 021", "%04.*x", prec, 33);

    prec = 3;
    CMP(" 021", "%04.*X", prec, 33);
}

#endif // wxUSE_WXVSNPRINTF

