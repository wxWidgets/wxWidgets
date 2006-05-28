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


// if 1 then instead of the hard-coded expected strings, the obtained results will be
// compared to the output of the system's vsnprintf() implementation.
// NOTE: this requires a vsnprintf() implementation which supports positional parameters.
#define USE_LIBC            0

// this makes it possible to write all tests without repeating a lot of times wxT() macro
#define CMP(x, y, z)        Compare(wxT(x), wxT(y), z)

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

        CPPUNIT_TEST( BigToSmallBuffer );
    CPPUNIT_TEST_SUITE_END();

    void E();
    void F();
    void G();
    void S();

    void BigToSmallBuffer();

    // some helpers
    void Misc(wxChar *buffer, int size);
    void Compare(const wxChar *expected, const wxChar *format, ...) const;
    void CompareV(wxChar *buf, wxChar *buf2, size_t len, const wxChar *format, va_list argptr) const;

    DECLARE_NO_COPY_CLASS(VsnprintfTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VsnprintfTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VsnprintfTestCase, "VsnprintfTestCase" );

VsnprintfTestCase::VsnprintfTestCase()
{
}

void VsnprintfTestCase::CompareV(wxChar *buf, wxChar *buf2, size_t len,
                                 const wxChar *format, va_list argptr) const
{
#if USE_LIBC
    va_list argptr2;
    wxVaCopy(argptr2, argptr);
#endif

    wxVsnprintf( buf, len, format, argptr );
    va_end(argptr);

#if USE_LIBC
    vsnprintf( buf2, len, format, argptr2 );       // use system's implementation
    va_end(argptr2);
#else
    wxUnusedVar(buf2);
#endif
}

void VsnprintfTestCase::Compare(const wxChar *expected, const wxChar *format, ...) const
{
    static wxChar buf[1024],
                  buf2[1024];

    va_list argptr;
    va_start( argptr, format );
    CompareV( buf, buf2, 1024, format, argptr );
    va_end(argptr);

#if USE_LIBC
    CPPUNIT_ASSERT_STR_EQUAL( buf, buf2 );
#else
    CPPUNIT_ASSERT_STR_EQUAL( buf, expected );
#endif
}

void VsnprintfTestCase::E()
{
    CMP("2.342000e+02", "%e",2.342E+02);
    CMP("-2.3420e-02", "%10.4e",-2.342E-02);
    CMP("-2.3420e-02", "%11.4e",-2.342E-02);
    CMP("    -2.3420e-02", "%15.4e",-2.342E-02);

    CMP("-0.02342", "%G",-2.342E-02);
    CMP("3.1415E-06", "%G",3.1415e-6);
    CMP("00003.141500e+03", "%016e", 3141.5);
    CMP("    3.141500e+03", "%16e", 3141.5);
    CMP("3.141500e+03    ", "%-16e", 3141.5);
    CMP("03.142e+03", "%010.3e", 3141.5);
}

void VsnprintfTestCase::F()
{
    CMP("3.300000", "%5f", 3.3);
    CMP("3.000000", "%5f", 3.0);
    CMP("0.000100", "%5f", .999999E-4);
    CMP("0.000990", "%5f", .99E-3);
    CMP("3333.000000", "%5f", 3333.0);
}

void VsnprintfTestCase::G()
{
    CMP("  3.3", "%5g", 3.3);
    CMP("    3", "%5g", 3.0);
    CMP("9.99999e-05", "%5g", .999999E-4);
    CMP("0.00099", "%5g", .99E-3);
    CMP(" 3333", "%5g", 3333.0);
    CMP(" 0.01", "%5g", 0.01);

    CMP("    3", "%5.g", 3.3);
    CMP("    3", "%5.g", 3.0);
    CMP("1e-04", "%5.g", .999999E-4);
    CMP("0.0001", "%5.g", 1.0E-4);
    CMP("0.001", "%5.g", .99E-3);
    CMP("3e+03", "%5.g", 3333.0);
    CMP(" 0.01", "%5.g", 0.01);

    CMP("  3.3", "%5.2g", 3.3);
    CMP("    3", "%5.2g", 3.0);
    CMP("1e-04", "%5.2g", .999999E-4);
    CMP("0.00099", "%5.2g", .99E-3);
    CMP("3.3e+03", "%5.2g", 3333.0);
    CMP(" 0.01", "%5.2g", 0.01);
}

void VsnprintfTestCase::S()
{
    CMP("  abc", "%5s", wxT("abc"));
    CMP("    a", "%5s", wxT("a"));
    CMP("abcdefghi", "%5s", wxT("abcdefghi"));
    CMP("abc  ", "%-5s", wxT("abc"));
    CMP("abcdefghi", "%-5s", wxT("abcdefghi"));

    CMP("abcde", "%.5s", wxT("abcdefghi"));
}

void VsnprintfTestCase::Misc(wxChar *buffer, int size)
{
    int ret;

    // test without positionals
    ret = wxSnprintf(buffer, size,
          wxT("\n\n%s %e %le %i %li - test - %d %i %% -%*.*f-\n\n"), wxT("aa"), 123.123,
          123123123123123123123123.123123123123, 456, (long int)33333333, 789, 999, 10, 1, 0.123);
    if (ret >= 0)
    {
        CPPUNIT_ASSERT_STR_EQUAL(
            wxT("\n\naa 1.231230e+02 1.231231e+23 456 33333333 - test - 789 999 %% -       0.1-\n\n"),
            buffer);
    }

    // test woth positional
    ret = wxSnprintf(buffer, size,
          wxT("\n\n%8$s %2$e %3$le %4$i %5$li - test - %6$d %7$i %% %1$.4f\n\n"), 0.123123123, 123.123,
         123123123123123123123123.123123123123, 456, (long int)33333333, 789, 999, wxT("aa"));
    if (ret >= 0)
    {
        CPPUNIT_ASSERT_STR_EQUAL(
            wxT("\n\naa 1.231230e+02 1.231231e+23 456 33333333 - test - 789 999 %% 0.1231\n\n"),
            buffer);
    }

    // test unicode/ansi conversion specifiers
    ret = wxSnprintf(buffer, size,
        wxT("\n\nunicode string: %ls %lc - ansi string: %hs %hc\n\n"), L"unicode!!", L'W', "ansi!!", 'w');
    if (ret >= 0)
    {
        CPPUNIT_ASSERT_STR_EQUAL(
            wxT("\n\nunicode string: unicode!! W - ansi string: ansi!! w\n\n"),
            buffer);
    }
}

void VsnprintfTestCase::BigToSmallBuffer()
{
    wxChar buf[1024], buf2[16], buf3[4], buf4;

    Misc(buf, 1024);
    Misc(buf2, 16);
    Misc(buf3, 4);
    Misc(&buf4, 1);
}
