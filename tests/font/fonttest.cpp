///////////////////////////////////////////////////////////////////////////////
// Name:        tests/font/fonttest.cpp
// Purpose:     wxFont unit test
// Author:      Francesco Montorsi
// Created:     16.3.09
// Copyright:   (c) 2009 Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/font.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

// Returns a point to an array of fonts and fills the output parameter with the
// number of elements in this array.
static const wxFont *GetTestFonts(unsigned& numFonts)
{
    static const wxFont testfonts[] =
    {
        *wxNORMAL_FONT,
        *wxSMALL_FONT,
        *wxITALIC_FONT,
        *wxSWISS_FONT,
        wxFont(5, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)
    };

    numFonts = WXSIZEOF(testfonts);

    return testfonts;
}

wxString DumpFont(const wxFont *font)
{
    // dumps the internal properties of a wxFont in the same order they
    // are checked by wxFontBase::operator==()

    wxASSERT(font->IsOk());

    wxString s;
    s.Printf(wxS("%d-%d;%d-%d-%d-%d-%d-%s-%d"),
             font->GetPointSize(),
             font->GetPixelSize().x,
             font->GetPixelSize().y,
             font->GetFamily(),
             font->GetStyle(),
             font->GetWeight(),
             font->GetUnderlined() ? 1 : 0,
             font->GetFaceName(),
             font->GetEncoding());

    return s;
}

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("wxFont::Construct", "[font][ctor]")
{
    // The main purpose of this test is to verify that the font ctors below
    // compile because it's easy to introduce ambiguities due to the number of
    // overloaded wxFont ctors.

    CHECK( wxFont(10, wxFONTFAMILY_DEFAULT,
                      wxFONTSTYLE_NORMAL,
                      wxFONTWEIGHT_NORMAL).IsOk() );

#if WXWIN_COMPATIBILITY_3_0
    // Disable the warning about deprecated wxNORMAL as we use it here
    // intentionally.
    #ifdef __VISUALC__
        #pragma warning(push)
        #pragma warning(disable:4996)
    #endif

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    // Tests relying on the soon-to-be-deprecated ctor taking ints and not
    // wxFontXXX enum elements.
    CHECK( wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL).IsOk() );

    wxGCC_WARNING_RESTORE(deprecated-declarations)

    #ifdef __VISUALC__
        #pragma warning(pop)
    #endif
#endif // WXWIN_COMPATIBILITY_3_0
}

TEST_CASE("wxFont::Size", "[font][size]")
{
    const struct Sizes
    {
        int specified;      // Size in points specified in the ctor.
        int expected;       // Expected GetPointSize() return value,
                            // -1 here means "same as wxNORMAL_FONT".
    } sizes[] =
    {
        {  9,  9 },
        { 10, 10 },
        { 11, 11 },
        { -1, -1 },
        { 70, -1 }, // 70 == wxDEFAULT, should be handled specially
        { 90, 90 }, // 90 == wxNORMAL, should not be handled specially
    };

    const int sizeDefault = wxFont(wxFontInfo()).GetPointSize();

    for ( size_t n = 0; n < WXSIZEOF(sizes); n++ )
    {
        const Sizes& size = sizes[n];

        // Note: use the old-style wxFont ctor as wxFontInfo doesn't implement
        // any compatibility hacks.
        const wxFont font(size.specified,
                          wxFONTFAMILY_DEFAULT,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL);

        int expected = size.expected;
        if ( expected == -1 )
            expected = sizeDefault;

        INFO("specified = " << size.specified <<
             ", expected = " << size.expected);
        CHECK( font.GetPointSize() == expected );
    }

    // Note that the compatibility hacks only apply to the old ctors, the newer
    // one, taking wxFontInfo, doesn't support them.
    CHECK( wxFont(wxFontInfo(70)).GetPointSize() == 70 );
    CHECK( wxFont(wxFontInfo(90)).GetPointSize() == 90 );

    // Check fractional point sizes as well.
    wxFont font(wxFontInfo(12.25));
    CHECK( font.GetFractionalPointSize() == 12.25 );
    CHECK( font.GetPointSize() == 12 );

    font = *wxNORMAL_FONT;
    font.SetFractionalPointSize(9.5);
    CHECK( font.GetFractionalPointSize() == 9.5 );
    CHECK( font.GetPointSize() == 10 );
}

TEST_CASE("wxFont::Style", "[font][style]")
{
#if WXWIN_COMPATIBILITY_3_0
    // Disable the warning about deprecated wxNORMAL as we use it here
    // intentionally.
    #ifdef __VISUALC__
        #pragma warning(push)
        #pragma warning(disable:4996)
    #endif

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    wxFont fontNormal(10, wxDEFAULT, wxNORMAL, wxNORMAL);
    CHECK( fontNormal.GetStyle() == wxFONTSTYLE_NORMAL );

    wxFont fontItalic(10, wxDEFAULT, wxITALIC, wxNORMAL);
    CHECK( fontItalic.GetStyle() == wxFONTSTYLE_ITALIC );

    wxFont fontSlant(10, wxDEFAULT, wxSLANT, wxNORMAL);
#ifdef __WXMSW__
    CHECK( fontSlant.GetStyle() == wxFONTSTYLE_ITALIC );
#else
    CHECK( fontSlant.GetStyle() == wxFONTSTYLE_SLANT );
#endif

    wxGCC_WARNING_RESTORE(deprecated-declarations)

    #ifdef __VISUALC__
        #pragma warning(pop)
    #endif
#endif // WXWIN_COMPATIBILITY_3_0
}

TEST_CASE("wxFont::Weight", "[font][weight]")
{
    wxFont font;
    font.SetNumericWeight(123);

    // WX to QT font weight conversions do not map directly which is why we
    // check if the numeric weight is within a range rather than checking for
    // an exact match.
#ifdef __WXQT__
    CHECK( ( font.GetNumericWeight() > 113 && font.GetNumericWeight() < 133 ) );
#else
    CHECK( font.GetNumericWeight() == 123 );
#endif

    CHECK( font.GetWeight() == wxFONTWEIGHT_THIN );

    font.SetNumericWeight(wxFONTWEIGHT_SEMIBOLD);
    CHECK( font.GetNumericWeight() == wxFONTWEIGHT_SEMIBOLD );
    CHECK( font.GetWeight() == wxFONTWEIGHT_SEMIBOLD );

#if WXWIN_COMPATIBILITY_3_0
    // Disable the warning about deprecated wxNORMAL as we use it here
    // intentionally.
    #ifdef __VISUALC__
        #pragma warning(push)
        #pragma warning(disable:4996)
    #endif

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    wxFont fontNormal(10, wxDEFAULT, wxNORMAL, wxNORMAL);
    CHECK( fontNormal.GetWeight() == wxFONTWEIGHT_NORMAL );

    wxFont fontBold(10, wxDEFAULT, wxNORMAL, wxBOLD);
    CHECK( fontBold.GetWeight() == wxFONTWEIGHT_BOLD );

    wxFont fontLight(10, wxDEFAULT, wxNORMAL, wxLIGHT);
    CHECK( fontLight.GetWeight() == wxFONTWEIGHT_LIGHT );

    wxGCC_WARNING_RESTORE(deprecated-declarations)

    #ifdef __VISUALC__
        #pragma warning(pop)
    #endif
#endif // WXWIN_COMPATIBILITY_3_0
}

TEST_CASE("wxFont::GetSet", "[font][getters]")
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( unsigned n = 0; n < numFonts; n++ )
    {
        wxFont test(*pf++);

        // remember: getters can only be called when wxFont::IsOk() == true
        CHECK( test.IsOk() );


        // test Get/SetFaceName()
#ifndef __WXQT__
        CHECK( !test.SetFaceName("a dummy face name") );
        CHECK( !test.IsOk() );
#endif

        // if the call to SetFaceName() below fails on your system/port,
        // consider adding another branch to this #if
#if defined(__WXMSW__) || defined(__WXOSX__)
        static const char *knownGoodFaceName = "Arial";
#else
        static const char *knownGoodFaceName = "Monospace";
#endif

        INFO("Testing font #" << n);

        {
            INFO("setting face name to " << knownGoodFaceName);
            CHECK( test.SetFaceName(knownGoodFaceName) );
            CHECK( test.IsOk() );
        }


        // test Get/SetFamily()

        test.SetFamily( wxFONTFAMILY_ROMAN );
        CHECK( test.IsOk() );

        // note that there is always the possibility that GetFamily() returns
        // wxFONTFAMILY_DEFAULT (meaning "unknown" in this case) so that we
        // consider it as a valid return value
        const wxFontFamily family = test.GetFamily();
        if ( family != wxFONTFAMILY_DEFAULT )
            CHECK( wxFONTFAMILY_ROMAN == family );


        // test Get/SetEncoding()

        //test.SetEncoding( wxFONTENCODING_KOI8 );
        //CHECK( test.IsOk() );
        //CHECK( wxFONTENCODING_KOI8 == test.GetEncoding() );


        // test Get/SetPointSize()

        test.SetPointSize(30);
        CHECK( test.IsOk() );
        CHECK( 30 == test.GetPointSize() );


        // test Get/SetPixelSize()

        test.SetPixelSize(wxSize(0,30));
        CHECK( test.IsOk() );
        CHECK( test.GetPixelSize().GetHeight() <= 30 );
            // NOTE: the match found by SetPixelSize() may be not 100% precise; it
            //       only grants that a font smaller than the required height will
            //       be selected


        // test Get/SetStyle()

        test.SetStyle(wxFONTSTYLE_SLANT);
        CHECK( test.IsOk() );
#ifdef __WXMSW__
        // on wxMSW wxFONTSTYLE_SLANT==wxFONTSTYLE_ITALIC, so accept the latter
        // as a valid value too.
        if ( test.GetStyle() != wxFONTSTYLE_ITALIC )
#endif
        CHECK( wxFONTSTYLE_SLANT == test.GetStyle() );

        // test Get/SetUnderlined()

        test.SetUnderlined(true);
        CHECK( test.IsOk() );
        CHECK( test.GetUnderlined() );

        const wxFont fontBase = test.GetBaseFont();
        CHECK( fontBase.IsOk() );
        CHECK( !fontBase.GetUnderlined() );
        CHECK( !fontBase.GetStrikethrough() );
        CHECK( wxFONTWEIGHT_NORMAL == fontBase.GetWeight() );
        CHECK( wxFONTSTYLE_NORMAL == fontBase.GetStyle() );

        // test Get/SetStrikethrough()

        test.SetStrikethrough(true);
        CHECK( test.IsOk() );
        CHECK( test.GetStrikethrough() );


        // test Get/SetWeight()

        test.SetWeight(wxFONTWEIGHT_BOLD);
        CHECK( test.IsOk() );
        CHECK( wxFONTWEIGHT_BOLD == test.GetWeight() );
    }
}

TEST_CASE("wxFont::NativeFontInfo", "[font][fontinfo]")
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( unsigned n = 0; n < numFonts; n++ )
    {
        wxFont test(*pf++);

        const wxString& nid = test.GetNativeFontInfoDesc();
        CHECK( !nid.empty() );
            // documented to be never empty

        wxFont temp;
        CHECK( temp.SetNativeFontInfo(nid) );
        CHECK( temp.IsOk() );

        INFO("Testing font #" << n);
        INFO("original font user description: " << DumpFont(&test));
        INFO("the other font description: " << DumpFont(&temp));

        CHECK( temp == test );
    }

    // test that clearly invalid font info strings do not work
    wxFont font;
    CHECK( !font.SetNativeFontInfo("") );

    // pango_font_description_from_string() used by wxFont in wxGTK and wxX11
    // never returns an error at all so this assertion fails there -- and as it
    // doesn't seem to be possible to do anything about it maybe we should
    // change wxMSW and other ports to also accept any strings?
#if !defined(__WXGTK__) && !defined(__WXX11__) && !defined(__WXQT__)
    CHECK( !font.SetNativeFontInfo("bloordyblop") );
#endif

    // Pango font description doesn't have 'underlined' and 'strikethrough'
    // attributes, so wxNativeFontInfo implements these itself. Test if these
    // are properly preserved by wxNativeFontInfo or its string description.
    font.SetUnderlined(true);
    font.SetStrikethrough(true);
    CHECK(font == wxFont(font));
    CHECK(font == wxFont(*font.GetNativeFontInfo()));
    CHECK(font == wxFont(font.GetNativeFontInfoDesc()));
    font.SetUnderlined(false);
    CHECK(font == wxFont(font));
    CHECK(font == wxFont(*font.GetNativeFontInfo()));
    CHECK(font == wxFont(font.GetNativeFontInfoDesc()));
    font.SetUnderlined(true);
    font.SetStrikethrough(false);
    CHECK(font == wxFont(font));
    CHECK(font == wxFont(*font.GetNativeFontInfo()));
    CHECK(font == wxFont(font.GetNativeFontInfoDesc()));
    // note: the GetNativeFontInfoUserDesc() doesn't preserve all attributes
    // according to docs, so it is not tested.
}

TEST_CASE("wxFont::NativeFontInfoUserDesc", "[font][fontinfo]")
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( unsigned n = 0; n < numFonts; n++ )
    {
        INFO("Testing font #" << n);

        wxFont test(*pf++);

        const wxString& niud = test.GetNativeFontInfoUserDesc();
        CHECK( !niud.empty() );
            // documented to be never empty

        INFO("original font user description: " << niud);

        wxFont temp2;
        CHECK( temp2.SetNativeFontInfoUserDesc(niud) );
        CHECK( temp2.IsOk() );

        INFO("the other font description: " << niud);

#ifdef __WXGTK__
        // Pango saves/restores all font info in the user-friendly string:
        CHECK( temp2 == test );
#else
        // NOTE: as documented GetNativeFontInfoUserDesc/SetNativeFontInfoUserDesc
        //       are not granted to save/restore all font info.
        //       In fact e.g. the font family is not saved at all; test only those
        //       info which GetNativeFontInfoUserDesc() does indeed save:
        CHECK( test.GetWeight() == temp2.GetWeight() );
        CHECK( test.GetStyle() == temp2.GetStyle() );

        // if the original face name was empty, it means that any face name (in
        // this family) can be used for the new font so we shouldn't be
        // surprised to find that they differ in this case
        const wxString facename = test.GetFaceName();
        if ( !facename.empty() )
        {
            CHECK( facename.Upper() == temp2.GetFaceName().Upper() );
        }

        CHECK( test.GetPointSize() == temp2.GetPointSize() );
        CHECK( test.GetEncoding() == temp2.GetEncoding() );
#endif
    }

    // Test for a bug with handling fractional font sizes in description
    // strings (see #18590).
    wxFont font(*wxNORMAL_FONT);

    static const double sizes[] = { 12.0, 10.5, 13.8, 10.123, 11.1 };
    for ( unsigned n = 0; n < WXSIZEOF(sizes); n++ )
    {
        font.SetFractionalPointSize(sizes[n]);

        // Just setting the font can slightly change it because of rounding
        // errors, so don't expect the actual size to be exactly equal to what
        // we used -- but close enough.
        const double sizeUsed = font.GetFractionalPointSize();
        CHECK( sizeUsed == Approx(sizes[n]).epsilon(0.001) );

        const wxString& desc = font.GetNativeFontInfoDesc();
        INFO("Font description: " << desc);
        CHECK( font.SetNativeFontInfo(desc) );

        // Notice that here we use the exact comparison, there is no reason for
        // a differently rounded size to be used.
        CHECK( font.GetFractionalPointSize() == sizeUsed );
    }
}
