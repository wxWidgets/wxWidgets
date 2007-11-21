///////////////////////////////////////////////////////////////////////////////
// Name:        tests/formatconverter/formatconverter.cpp
// Purpose:     Test wxFormatConverter
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

//
// Notes:
//
// The conversions wxFormatConverter currently does are as follows:
//
//    %s, %lS   ->  %ls
//    %S, %hS   ->  %s
//    %c, %lC   ->  %lc
//    %C, %hC   ->  %c
//
// %hs and %hc stay the same.
//
// %S and %C aren't actually in the ISO C or C++ standards, but they can be
// avoided when writing portable code.
//
// Nor are %hs or %hc in the standards, which means wxWidgets currently doesn't
// have a specifier for 'char' types that is ok for all builds and platforms.
//
// The effect of using %hs/%hc is undefined, though RTLs are quite likely
// to just ignore the 'h', so maybe it works as required even though it's
// not legal.
//
// I've put in some checks, such as this which will flag up any platforms
// where this is not the case:
//
//  CPPUNIT_ASSERT(wxString::Format(_T("%hs"), "test") == _T("test"));
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// wxFormatConverter can only be tested in a Unicode non-Windows debug build
//
#if defined(wxNEED_PRINTF_CONVERSION) && defined(__WXDEBUG__)
#define CAN_TEST
extern wxString wxConvertFormat(const wxChar *format);
#endif

using CppUnit::TestCase;
using std::string;

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// wxFormatConverter only changes %s, %c, %S and %C, all others are treated
// equally, therefore it is enough to choose just one other for testing, %d
// will do.

class FormatConverterTestCase : public TestCase
{
    CPPUNIT_TEST_SUITE(FormatConverterTestCase);
        CPPUNIT_TEST(format_d);
        CPPUNIT_TEST(format_hd);
        CPPUNIT_TEST(format_ld);
        CPPUNIT_TEST(format_s);
        CPPUNIT_TEST(format_hs);
        CPPUNIT_TEST(format_ls);
        CPPUNIT_TEST(format_c);
        CPPUNIT_TEST(format_hc);
        CPPUNIT_TEST(format_lc);
#ifdef CAN_TEST
        CPPUNIT_TEST(format_S);
        CPPUNIT_TEST(format_hS);
        CPPUNIT_TEST(format_lS);
        CPPUNIT_TEST(format_C);
        CPPUNIT_TEST(format_hC);
        CPPUNIT_TEST(format_lC);
        CPPUNIT_TEST(testLonger);
#endif
    CPPUNIT_TEST_SUITE_END();

    void format_d();
    void format_hd();
    void format_ld();
    void format_s();
    void format_hs();
    void format_ls();
    void format_c();
    void format_hc();
    void format_lc();

#ifdef CAN_TEST
    void format_S();
    void format_hS();
    void format_lS();
    void format_C();
    void format_hC();
    void format_lC();
    void testLonger();

    void doTest(const wxChar *input, const wxChar *expected);
    void check(const wxString& input, const wxString& expected);
#endif
};

void FormatConverterTestCase::format_d()
{
#ifdef CAN_TEST
    doTest(_T("d"), _T("d"));
#endif
    CPPUNIT_ASSERT(wxString::Format(_T("%d"), 255) == _T("255"));
    CPPUNIT_ASSERT(wxString::Format(_T("%05d"), 255) == _T("00255"));
    CPPUNIT_ASSERT(wxString::Format(_T("% 5d"), 255) == _T("  255"));
    CPPUNIT_ASSERT(wxString::Format(_T("% 5d"), -255) == _T(" -255"));
    CPPUNIT_ASSERT(wxString::Format(_T("%-5d"), -255) == _T("-255 "));
    CPPUNIT_ASSERT(wxString::Format(_T("%+5d"), 255) == _T(" +255"));
    CPPUNIT_ASSERT(wxString::Format(_T("%*d"), 5, 255) == _T("  255"));
}

void FormatConverterTestCase::format_hd()
{
#ifdef CAN_TEST
    doTest(_T("hd"), _T("hd"));
#endif
    short s = 32767;
    CPPUNIT_ASSERT(wxString::Format(_T("%hd"), s) == _T("32767"));
}

void FormatConverterTestCase::format_ld()
{
#ifdef CAN_TEST
    doTest(_T("ld"), _T("ld"));
#endif
    long l = 2147483647L;
    CPPUNIT_ASSERT(wxString::Format(_T("%ld"), l) == _T("2147483647"));
}

void FormatConverterTestCase::format_s()
{
#ifdef CAN_TEST
    doTest(_T("s"), _T("ls"));
#endif
    CPPUNIT_ASSERT(wxString::Format(_T("%s!"), _T("test")) == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%6s!"), _T("test")) == _T("  test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%-6s!"), _T("test")) == _T("test  !"));
    CPPUNIT_ASSERT(wxString::Format(_T("%.6s!"), _T("test")) == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%6.4s!"), _T("testing")) == _T("  test!"));
}

void FormatConverterTestCase::format_hs()
{
#ifdef CAN_TEST
    doTest(_T("hs"), _T("hs"));
#endif
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%hs!")), "test") == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%6hs!")), "test") == _T("  test!"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%-6hs!")), "test") == _T("test  !"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%.6hs!")), "test") == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%6.4hs!")), "testing") == _T("  test!"));
}

void FormatConverterTestCase::format_ls()
{
#ifdef CAN_TEST
    doTest(_T("ls"), _T("ls"));
#endif
    CPPUNIT_ASSERT(wxString::Format(_T("%ls!"), L"test") == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%6ls!"), L"test") == _T("  test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%-6ls!"), L"test") == _T("test  !"));
    CPPUNIT_ASSERT(wxString::Format(_T("%.6ls!"), L"test") == _T("test!"));
    CPPUNIT_ASSERT(wxString::Format(_T("%6.4ls!"), L"testing") == _T("  test!"));
}

void FormatConverterTestCase::format_c()
{
#ifdef CAN_TEST
    doTest(_T("c"), _T("lc"));
#endif
    CPPUNIT_ASSERT(wxString::Format(_T("%c"), _T('x')) == _T("x"));
    CPPUNIT_ASSERT(wxString::Format(_T("%2c"), _T('x')) == _T(" x"));
    CPPUNIT_ASSERT(wxString::Format(_T("%-2c"), _T('x')) == _T("x "));
}

void FormatConverterTestCase::format_hc()
{
#ifdef CAN_TEST
    doTest(_T("hc"), _T("hc"));
#endif
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%hc")), 'x') == _T("x"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%2hc")), 'x') == _T(" x"));
    CPPUNIT_ASSERT(wxString::Format(wxString(_T("%-2hc")), 'x') == _T("x "));
}

void FormatConverterTestCase::format_lc()
{
#ifdef CAN_TEST
    doTest(_T("lc"), _T("lc"));
#endif
    CPPUNIT_ASSERT(wxString::Format(_T("%lc"), L'x') == _T("x"));
    CPPUNIT_ASSERT(wxString::Format(_T("%2lc"), L'x') == _T(" x"));
    CPPUNIT_ASSERT(wxString::Format(_T("%-2lc"), L'x') == _T("x "));
}

#ifdef CAN_TEST

void FormatConverterTestCase::format_S()  { doTest(_T("S"),  _T("s"));  }
void FormatConverterTestCase::format_hS() { doTest(_T("hS"), _T("s"));  }
void FormatConverterTestCase::format_lS() { doTest(_T("lS"), _T("ls")); }

void FormatConverterTestCase::format_C()  { doTest(_T("C"),  _T("c"));  }
void FormatConverterTestCase::format_hC() { doTest(_T("hC"), _T("c"));  }
void FormatConverterTestCase::format_lC() { doTest(_T("lC"), _T("lc")); }

// It's possible that although a format converts correctly alone, it leaves
// the converter in a bad state that will affect subsequent formats, so
// check with a selection of longer patterns.
//
void FormatConverterTestCase::testLonger()
{
    struct {
        const wxChar *input;
        const wxChar *expected;
    } formats[] = {
        { _T("%d"),     _T("%d"),    },
        { _T("%*hd"),   _T("%*hd")   },
        { _T("%.4ld"),  _T("%.4ld")  },
        { _T("%-.*s"),  _T("%-.*ls") },
        { _T("%.*hs"),  _T("%.*hs"), },
        { _T("%-.9ls"), _T("%-.9ls") },
        { _T("%-*c"),   _T("%-*lc")  },
        { _T("%3hc"),   _T("%3hc")   },
        { _T("%-5lc"),  _T("%-5lc")  }
    };
    size_t i, j;

    // exclude patterns that don't translate correctly alone from the test
    for (i = 0; i < WXSIZEOF(formats); i++)
        if (wxConvertFormat(formats[i].input) != formats[i].expected)
            formats[i].input = NULL;

    // test all possible pairs of the above patterns
    for (i = 0; i < WXSIZEOF(formats); i++) {
        if (formats[i].input) {
            wxString input(formats[i].input);
            wxString expected(formats[i].expected);

            for (j = 0; j < WXSIZEOF(formats); j++)
                if (formats[j].input)
                    check(input + formats[j].input,
                          expected + formats[j].expected);
        }
    }
}

void FormatConverterTestCase::doTest(const wxChar *input,
                                     const wxChar *expected)
{
    static const wxChar *flag_width[] =
        { _T(""), _T("*"), _T("10"), _T("-*"), _T("-10"), NULL };
    static const wxChar *precision[] =
        { _T(""), _T(".*"), _T(".10"), NULL };
    static const wxChar *empty[] =
        { _T(""), NULL };

    // no precision for %c or %C
    const wxChar **precs = wxTolower(input[wxStrlen(input)-1]) == _T('c') ?
        empty : precision;

    wxString fmt(_T("%"));

    // try the test for a variety of combinations of flag, width and precision
    for (const wxChar **prec = precs; *prec; prec++)
        for (const wxChar **width = flag_width; *width; width++)
            check(fmt + *width + *prec + input,
                  fmt + *width + *prec + expected);
}

void FormatConverterTestCase::check(const wxString& input,
                                    const wxString& expected)
{
    wxString result = wxConvertFormat(input);
    wxString msg = _T("input: '") + input +
                   _T("', result: '") + result +
                   _T("', expected: '") + expected + _T("'");
    CPPUNIT_ASSERT_MESSAGE(string(msg.mb_str()), result == expected);
}

#endif // CAN_TEST

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(FormatConverterTestCase);

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FormatConverterTestCase,
                                      "FormatConverterTestCase");

