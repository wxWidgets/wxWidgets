///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/ellipsization.cpp
// Purpose:     wxControlBase::*Ellipsize* unit test
// Author:      Francesco Montorsi
// Created:     2010-03-10
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/control.h"
#include "wx/dcmemory.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class EllipsizationTestCase : public CppUnit::TestCase
{
public:
    EllipsizationTestCase() { }

private:
    CPPUNIT_TEST_SUITE( EllipsizationTestCase );
        CPPUNIT_TEST( NormalCase );
        CPPUNIT_TEST( EnoughSpace );
        CPPUNIT_TEST( VeryLittleSpace );
        CPPUNIT_TEST( HasThreeDots );
    CPPUNIT_TEST_SUITE_END();

    void NormalCase();
    void EnoughSpace();
    void VeryLittleSpace();
    void HasThreeDots();

    DECLARE_NO_COPY_CLASS(EllipsizationTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EllipsizationTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EllipsizationTestCase, "EllipsizationTestCase" );

void EllipsizationTestCase::NormalCase()
{
    wxMemoryDC dc;

    static const char *stringsToTest[] =
    {
        "N",
        ".",
        "x",
        "foobar",
        "\xCE\xB1", // U03B1 (GREEK SMALL LETTER ALPHA)
        "Another test",
        "a very very very very very very very long string",
        // alpha+beta+gamma+delta+epsilon+zeta+eta+theta+iota
        "\xCE\xB1\xCE\xB2\xCE\xB3\xCE\xB4\xCE\xB5\xCE\xB6\xCE\xB7\xCE\xB8\xCE\xB9",
        "\t", "\t\t\t\t\t", "a\tstring\twith\ttabs",
        "\n", "\n\n\n\n\n", "a\nstring\nwith\nnewlines",
        "&", "&&&&&&&", "a&string&with&newlines",
        "\t\n&", "a\t\n&string\t\n&with\t\n&many\t\n&chars"
    };

    static const int flagsToTest[] =
    {
        0,
        wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS,
        wxELLIPSIZE_FLAGS_EXPAND_TABS,
        wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS | wxELLIPSIZE_FLAGS_EXPAND_TABS
    };

    static const wxEllipsizeMode modesToTest[] =
    {
        wxELLIPSIZE_START,
        wxELLIPSIZE_MIDDLE,
        wxELLIPSIZE_END
    };

    int widthsToTest[] = { 50, 100, 150 };

    for ( unsigned int s = 0; s < WXSIZEOF(stringsToTest); s++ )
    {
        const wxString str = wxString::FromUTF8(stringsToTest[s]);

        for ( unsigned int  f = 0; f < WXSIZEOF(flagsToTest); f++ )
        {
            for ( unsigned int m = 0; m < WXSIZEOF(modesToTest); m++ )
            {
                for ( unsigned int w = 0; w < WXSIZEOF(widthsToTest); w++ )
                {
                    wxString ret = wxControl::Ellipsize
                                   (
                                    str,
                                    dc,
                                    modesToTest[m],
                                    widthsToTest[w],
                                    flagsToTest[f]
                                   );

                    WX_ASSERT_MESSAGE
                    (
                     (
                        "invalid ellipsization for \"%s\" (%dpx, should be <=%dpx)",
                        str,
                        dc.GetMultiLineTextExtent(ret).GetWidth(),
                        widthsToTest[w]
                     ),
                     dc.GetMultiLineTextExtent(ret).GetWidth() <= widthsToTest[w]
                    );
                }
            }
        }
    }
}


void EllipsizationTestCase::EnoughSpace()
{
    // No ellipsization should occur if there's plenty of space.

    wxMemoryDC dc;

    CPPUNIT_ASSERT_EQUAL("some label",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_START, 200));
    CPPUNIT_ASSERT_EQUAL("some label",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_MIDDLE, 200));
    CPPUNIT_ASSERT_EQUAL("some label",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_END, 200));
}


void EllipsizationTestCase::VeryLittleSpace()
{
    // If there's not enough space, the shortened label should still contain "..." and one character

    wxMemoryDC dc;

    CPPUNIT_ASSERT_EQUAL("...l",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_START, 5));
    CPPUNIT_ASSERT_EQUAL("s...",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_MIDDLE, 5));
    CPPUNIT_ASSERT_EQUAL("s...",
                         wxControl::Ellipsize("some label1", dc, wxELLIPSIZE_MIDDLE, 5));
    CPPUNIT_ASSERT_EQUAL("s...",
                         wxControl::Ellipsize("some label", dc, wxELLIPSIZE_END, 5));
}


void EllipsizationTestCase::HasThreeDots()
{
    wxMemoryDC dc;

    CPPUNIT_ASSERT( wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_START, 80).StartsWith("...") );
    CPPUNIT_ASSERT( !wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_START, 80).EndsWith("...") );

    CPPUNIT_ASSERT( wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_END, 80).EndsWith("...") );

    CPPUNIT_ASSERT( wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_MIDDLE, 80).Contains("...") );
    CPPUNIT_ASSERT( !wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_MIDDLE, 80).StartsWith("...") );
    CPPUNIT_ASSERT( !wxControl::Ellipsize("some longer text", dc, wxELLIPSIZE_MIDDLE, 80).EndsWith("...") );
}
