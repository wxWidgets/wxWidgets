///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/vsnprintf.cpp
// Purpose:     wxVsnprintf unit test
// Author:      Francesco Montorsi
//              (part of this file was taken from CMP.c of TRIO package
//               written by Bjorn Reese and Daniel Stenberg)
// Created:     2006-04-01
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Francesco Montorsi, Bjorn Reese and Daniel Stenberg
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
    #include "wx/wxchar.h"
#endif // WX_PRECOMP



#define MAX_TEST_LEN        1024


// temporary buffers
static wxChar buf[MAX_TEST_LEN];

// these macros makes it possible to write all tests without repeating a lot of times wxT() macro

#define ASSERT_STR_EQUAL( a, b ) \
    CPPUNIT_ASSERT( wxString(a) == wxString(b) );

#define CMP5(expected, x, y, z, w)                  \
    wxSnprintf(buf, MAX_TEST_LEN, wxT(x), y, z, w); \
                                                    \
    ASSERT_STR_EQUAL( wxT(expected), buf );

#define CMP4(expected, x, y, z)                     \
    wxSnprintf(buf, MAX_TEST_LEN, wxT(x), y, z);    \
                                                    \
    ASSERT_STR_EQUAL( wxT(expected), buf );

#define CMP3(expected, x, y)                        \
    wxSnprintf(buf, MAX_TEST_LEN, wxT(x), y);       \
                                                    \
    ASSERT_STR_EQUAL( wxT(expected), buf );

#define CMP2(expected, x)                           \
    wxSnprintf(buf, MAX_TEST_LEN, wxT(x));          \
                                                    \
    ASSERT_STR_EQUAL( wxT(expected), buf );

#define CMPTOSIZE(buffer, size, expected, fmt, x, y, z, w)  \
    wxSnprintf(buffer, size, wxT(fmt), x, y, z, w);         \
                                                            \
    CPPUNIT_ASSERT( wxString(wxT(expected)).Left(size - 1) == buffer )



// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VsnprintfTestCase : public CppUnit::TestCase
{
public:
    VsnprintfTestCase();

private:
    CPPUNIT_TEST_SUITE( VsnprintfTestCase );
        CPPUNIT_TEST( E );
        CPPUNIT_TEST( F );
        CPPUNIT_TEST( G );
        CPPUNIT_TEST( S );
        CPPUNIT_TEST( Asterisk );

        CPPUNIT_TEST( BigToSmallBuffer );
    CPPUNIT_TEST_SUITE_END();

    void E();
    void F();
    void G();
    void S();
    void Asterisk();

    void BigToSmallBuffer();
    void Misc(wxChar *buffer, int size);

    DECLARE_NO_COPY_CLASS(VsnprintfTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VsnprintfTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VsnprintfTestCase, "VsnprintfTestCase" );

VsnprintfTestCase::VsnprintfTestCase()
{
}

void VsnprintfTestCase::E()
{
    // NB: there are no standards about the minimum exponent width
    //     (and the width of the %e conversion specifier refers to the
    //      mantissa, not to the exponent).
    //     Since newer MSVC versions use 3 digits as minimum exponent
    //     width while GNU libc uses 2 digits as minimum width, here we
    //     workaround this problem using for the exponent values with at
    //     least three digits.
    //     Some examples:
    //       printf("%e",2.342E+02);
    //     -> under MSVC7.1 prints:      2.342000e+002
    //     -> under GNU libc 2.4 prints: 2.342000e+02
    CMP3("2.342000e+112", "%e",2.342E+112);
    CMP3("-2.3420e-112", "%10.4e",-2.342E-112);
    CMP3("-2.3420e-112", "%11.4e",-2.342E-112);
    CMP3("   -2.3420e-112", "%15.4e",-2.342E-112);

    CMP3("-0.02342", "%G",-2.342E-02);
    CMP3("3.1415E-116", "%G",3.1415e-116);
    CMP3("0003.141500e+103", "%016e", 3141.5e100);
    CMP3("   3.141500e+103", "%16e", 3141.5e100);
    CMP3("3.141500e+103   ", "%-16e", 3141.5e100);
    CMP3("3.142e+103", "%010.3e", 3141.5e100);
}

void VsnprintfTestCase::F()
{
    CMP3("3.300000", "%5f", 3.3);
    CMP3("3.000000", "%5f", 3.0);
    CMP3("0.000100", "%5f", .999999E-4);
    CMP3("0.000990", "%5f", .99E-3);
    CMP3("3333.000000", "%5f", 3333.0);
}

void VsnprintfTestCase::G()
{
    // NOTE: the same about E() testcase applies here...

    CMP3("  3.3", "%5g", 3.3);
    CMP3("    3", "%5g", 3.0);
    CMP3("9.99999e-115", "%5g", .999999E-114);
    CMP3("0.00099", "%5g", .99E-3);
    CMP3(" 3333", "%5g", 3333.0);
    CMP3(" 0.01", "%5g", 0.01);

    CMP3("    3", "%5.g", 3.3);
    CMP3("    3", "%5.g", 3.0);
    CMP3("1e-114", "%5.g", .999999E-114);
    CMP3("0.0001", "%5.g", 1.0E-4);
    CMP3("0.001", "%5.g", .99E-3);
    CMP3("3e+103", "%5.g", 3333.0E100);
    CMP3(" 0.01", "%5.g", 0.01);

    CMP3("  3.3", "%5.2g", 3.3);
    CMP3("    3", "%5.2g", 3.0);
    CMP3("1e-114", "%5.2g", .999999E-114);
    CMP3("0.00099", "%5.2g", .99E-3);
    CMP3("3.3e+103", "%5.2g", 3333.0E100);
    CMP3(" 0.01", "%5.2g", 0.01);
}

void VsnprintfTestCase::S()
{
    CMP3("  abc", "%5s", wxT("abc"));
    CMP3("    a", "%5s", wxT("a"));
    CMP3("abcdefghi", "%5s", wxT("abcdefghi"));
    CMP3("abc  ", "%-5s", wxT("abc"));
    CMP3("abcdefghi", "%-5s", wxT("abcdefghi"));

    CMP3("abcde", "%.5s", wxT("abcdefghi"));

    // some tests without any argument passed through ...
    CMP2("%", "%%");
    CMP2("%%%", "%%%%%%");

    // do not test odd number of '%' symbols as different implementations
    // of snprintf() give different outputs as this situation is not considered
    // by any standard (in fact, GCC will also warn you about a spurious % if
    // you write %%% as argument of some *printf function !)
    // Compare(wxT("%"), wxT("%%%"));
}

void VsnprintfTestCase::Asterisk()
{
    CMP5("       0.1", "%*.*f", 10, 1, 0.123);
    CMP5("    0.1230", "%*.*f", 10, 4, 0.123);
    CMP5("0.1", "%*.*f", 3, 1, 0.123);

    CMP4("%0.002", "%%%.*f", 3, 0.0023456789);
}

void VsnprintfTestCase::Misc(wxChar *buffer, int size)
{
    // NB: remember that wx*printf could be mapped either to system
    //     implementation or to wx implementation.
    //     In the first case, when the output buffer is too small, the returned
    //     value can be the number of characters required for the output buffer
    //     (conforming to ISO C99; implemented in e.g. GNU libc >= 2.1), or
    //     just a negative number, usually -1; (this is how e.g. MSVC's
    //     *printf() behaves).  Fortunately, in all implementations, when the
    //     output buffer is too small, it's nonetheless filled up to its max
    //     size.

    // test without positionals
    CMPTOSIZE(buffer, size, "123 444444444 - test - 555 -0.666",
              "%i %li - test - %d %.3f",
              123, (long int)444444444, 555, -0.666);

#if wxUSE_PRINTF_POS_PARAMS
    // test with positional
    CMPTOSIZE(buffer, size, "-0.666 123 - test - 444444444 555",
              "%4$.3f %1$i - test - %2$li %3$d",
              123, (long int)444444444, 555, -0.666);
#endif

    // test unicode/ansi conversion specifiers
    // NB: this line will output two warnings like these, on GCC:
    //     warning: use of 'h' length modifier with 's' type character (i.e.
    //     GCC warns you that 'h' is not legal on 's' conv spec) but they must
    //     be ignored as here we explicitely want to test the wxSnprintf()
    //     behaviour in such case

    CMPTOSIZE(buffer, size,
              "unicode string: unicode!! W - ansi string: ansi!! w\n\n",
              "unicode string: %ls %lc - ansi string: %hs %hc\n\n",
              L"unicode!!", L'W', "ansi!!", 'w');
}

void VsnprintfTestCase::BigToSmallBuffer()
{
    wxChar buf[1024], buf2[16], buf3[4], buf4;

    Misc(buf, 1024);
    Misc(buf2, 16);
    Misc(buf3, 4);
    Misc(&buf4, 1);
}
