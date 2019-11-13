///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/ellipsization.cpp
// Purpose:     wxControlBase::*Ellipsize* unit test
// Author:      Francesco Montorsi
// Created:     2010-03-10
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

TEST_CASE("Ellipsization::NormalCase", "[ellipsization]")
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

    const int charWidth = dc.GetCharWidth();
    int widthsToTest[] = { 6*charWidth, 10*charWidth, 15*charWidth };

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

                    // Note that we must measure the width of the text that
                    // will be rendered, and when mnemonics are used, this
                    // means we have to remove them first.
                    const wxString
                        displayed = flagsToTest[f] & wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS
                                        ? wxControl::RemoveMnemonics(ret)
                                        : ret;
                    const int
                        width = dc.GetMultiLineTextExtent(displayed).GetWidth();

                    WX_ASSERT_MESSAGE
                    (
                     (
                        "Test #(%u,%u.%u): %s\n\"%s\" -> \"%s\"; width=%dpx > %dpx",
                        s, f, m,
                        dc.GetFont().GetNativeFontInfoUserDesc(),
                        str,
                        ret,
                        width,
                        widthsToTest[w]
                     ),
                     width <= widthsToTest[w]
                    );
                }
            }
        }
    }
}


TEST_CASE("Ellipsization::EnoughSpace", "[ellipsization]")
{
    // No ellipsization should occur if there's plenty of space.

    wxMemoryDC dc;

    wxString testString("some label");
    const int width = dc.GetTextExtent(testString).GetWidth() + 50;

    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_START, width) == testString );
    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_MIDDLE, width) == testString );
    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_END, width) == testString );
}


TEST_CASE("Ellipsization::VeryLittleSpace", "[ellipsization]")
{
    // If there's not enough space, the shortened label should still contain "..." and one character

    wxMemoryDC dc;

    const int width = dc.GetTextExtent("s...").GetWidth();

    CHECK( wxControl::Ellipsize("some label", dc, wxELLIPSIZE_START, width) == "...l" );
    CHECK( wxControl::Ellipsize("some label", dc, wxELLIPSIZE_MIDDLE, width) == "s..." );
    CHECK( wxControl::Ellipsize("some label1", dc, wxELLIPSIZE_MIDDLE, width) == "s..." );
    CHECK( wxControl::Ellipsize("some label", dc, wxELLIPSIZE_END, width) == "s..." );
}


TEST_CASE("Ellipsization::HasThreeDots", "[ellipsization]")
{
    wxMemoryDC dc;

    wxString testString("some longer text");
    const int width = dc.GetTextExtent(testString).GetWidth() - 5;

    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_START, width).StartsWith("...") );
    CHECK( !wxControl::Ellipsize(testString, dc, wxELLIPSIZE_START, width).EndsWith("...") );

    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_END, width).EndsWith("...") );

    CHECK( wxControl::Ellipsize(testString, dc, wxELLIPSIZE_MIDDLE, width).Contains("...") );
    CHECK( !wxControl::Ellipsize(testString, dc, wxELLIPSIZE_MIDDLE, width).StartsWith("...") );
    CHECK( !wxControl::Ellipsize(testString, dc, wxELLIPSIZE_MIDDLE, width).EndsWith("...") );
}
