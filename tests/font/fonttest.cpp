///////////////////////////////////////////////////////////////////////////////
// Name:        tests/font/fonttest.cpp
// Purpose:     wxFont unit test
// Author:      Francesco Montorsi
// Created:     16.3.09
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Francesco Montorsi
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

#if wxUSE_FONTMAP

#include "wx/font.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FontTestCase : public CppUnit::TestCase
{
public:
    FontTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FontTestCase );
        CPPUNIT_TEST( GetSet );
        CPPUNIT_TEST( NativeFontInfo );
        CPPUNIT_TEST( NativeFontInfoUserDesc );
    CPPUNIT_TEST_SUITE_END();

    void GetSet();
    void NativeFontInfo();
    void NativeFontInfoUserDesc();

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

    DECLARE_NO_COPY_CLASS(FontTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FontTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FontTestCase, "FontTestCase" );

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

void FontTestCase::GetSet()
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( unsigned n = 0; n < numFonts; n++ )
    {
        wxFont test(*pf++);

        // remember: getters can only be called when wxFont::IsOk() == true
        CPPUNIT_ASSERT( test.IsOk() );


        // test Get/SetFaceName()
        CPPUNIT_ASSERT( !test.SetFaceName("a dummy face name") );
        CPPUNIT_ASSERT( !test.IsOk() );

        // if the call to SetFaceName() below fails on your system/port,
        // consider adding another branch to this #if
#if defined(__WXMSW__) || defined(__WXOSX__)
        static const char *knownGoodFaceName = "Arial";
#else
        static const char *knownGoodFaceName = "Monospace";
#endif

        WX_ASSERT_MESSAGE
        (
            ("failed to set face name \"%s\" for test font #%u\n"
             "(this failure is harmless if this face name is not "
             "available on this system)", knownGoodFaceName, n),
            test.SetFaceName(knownGoodFaceName)
        );
        CPPUNIT_ASSERT( test.IsOk() );


        // test Get/SetFamily()

        test.SetFamily( wxFONTFAMILY_ROMAN );
        CPPUNIT_ASSERT( test.IsOk() );

        // note that there is always the possibility that GetFamily() returns
        // wxFONTFAMILY_DEFAULT (meaning "unknown" in this case) so that we
        // consider it as a valid return value
        const wxFontFamily family = test.GetFamily();
        if ( family != wxFONTFAMILY_DEFAULT )
            CPPUNIT_ASSERT_EQUAL( wxFONTFAMILY_ROMAN, family );


        // test Get/SetEncoding()

        //test.SetEncoding( wxFONTENCODING_KOI8 );
        //CPPUNIT_ASSERT( test.IsOk() );
        //CPPUNIT_ASSERT_EQUAL( wxFONTENCODING_KOI8 , test.GetEncoding() );


        // test Get/SetPointSize()

        test.SetPointSize(30);
        CPPUNIT_ASSERT( test.IsOk() );
        CPPUNIT_ASSERT_EQUAL( 30, test.GetPointSize() );


        // test Get/SetPixelSize()

        test.SetPixelSize(wxSize(0,30));
        CPPUNIT_ASSERT( test.IsOk() );
        CPPUNIT_ASSERT( test.GetPixelSize().GetHeight() <= 30 );
            // NOTE: the match found by SetPixelSize() may be not 100% precise; it
            //       only grants that a font smaller than the required height will
            //       be selected


        // test Get/SetStyle()

        test.SetStyle(wxFONTSTYLE_SLANT);
        CPPUNIT_ASSERT( test.IsOk() );
#ifdef __WXMSW__
        // on wxMSW wxFONTSTYLE_SLANT==wxFONTSTYLE_ITALIC
        CPPUNIT_ASSERT( wxFONTSTYLE_SLANT == test.GetStyle() ||
                        wxFONTSTYLE_ITALIC == test.GetStyle() );
#else
        CPPUNIT_ASSERT_EQUAL( wxFONTSTYLE_SLANT, test.GetStyle() );
#endif

        // test Get/SetUnderlined()

        test.SetUnderlined(true);
        CPPUNIT_ASSERT( test.IsOk() );
        CPPUNIT_ASSERT_EQUAL( true, test.GetUnderlined() );


        // test Get/SetWeight()

        test.SetWeight(wxFONTWEIGHT_BOLD);
        CPPUNIT_ASSERT( test.IsOk() );
        CPPUNIT_ASSERT_EQUAL( wxFONTWEIGHT_BOLD, test.GetWeight() );
    }
}

void FontTestCase::NativeFontInfo()
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( size_t n = 0; n < numFonts; n++ )
    {
        wxFont test(*pf++);

        const wxString& nid = test.GetNativeFontInfoDesc();
        CPPUNIT_ASSERT( !nid.empty() );
            // documented to be never empty

        wxFont temp;
        CPPUNIT_ASSERT( temp.SetNativeFontInfo(nid) );
        CPPUNIT_ASSERT( temp.IsOk() );
        WX_ASSERT_MESSAGE(
            ("Test #%lu failed\ndump of test font: \"%s\"\ndump of temp font: \"%s\"", \
             n, DumpFont(&test), DumpFont(&temp)),
            temp == test );
    }

    // test that clearly invalid font info strings do not work
    wxFont font;
    CPPUNIT_ASSERT( !font.SetNativeFontInfo("") );

    // pango_font_description_from_string() used by wxFont in wxGTK and wxX11
    // never returns an error at all so this assertion fails there -- and as it
    // doesn't seem to be possible to do anything about it maybe we should
    // change wxMSW and other ports to also accept any strings?
#if !defined(__WXGTK__) && !defined(__WXX11__)
    CPPUNIT_ASSERT( !font.SetNativeFontInfo("bloordyblop") );
#endif
}

void FontTestCase::NativeFontInfoUserDesc()
{
    unsigned numFonts;
    const wxFont *pf = GetTestFonts(numFonts);
    for ( size_t n = 0; n < numFonts; n++ )
    {
        wxFont test(*pf++);

        const wxString& niud = test.GetNativeFontInfoUserDesc();
        CPPUNIT_ASSERT( !niud.empty() );
            // documented to be never empty

        wxFont temp2;
        CPPUNIT_ASSERT( temp2.SetNativeFontInfoUserDesc(niud) );
        CPPUNIT_ASSERT( temp2.IsOk() );

#ifdef __WXGTK__
        // Pango saves/restores all font info in the user-friendly string:
        WX_ASSERT_MESSAGE(
            ("Test #%lu failed; native info user desc was \"%s\" for test and \"%s\" for temp2", \
             n, niud, temp2.GetNativeFontInfoUserDesc()),
            temp2 == test );
#else
        // NOTE: as documented GetNativeFontInfoUserDesc/SetNativeFontInfoUserDesc
        //       are not granted to save/restore all font info.
        //       In fact e.g. the font family is not saved at all; test only those
        //       info which GetNativeFontInfoUserDesc() does indeed save:
        CPPUNIT_ASSERT_EQUAL( test.GetWeight(), temp2.GetWeight() );
        CPPUNIT_ASSERT_EQUAL( test.GetStyle(), temp2.GetStyle() );

        // if the original face name was empty, it means that any face name (in
        // this family) can be used for the new font so we shouldn't be
        // surprised to find that they differ in this case
        const wxString facename = test.GetFaceName();
        if ( !facename.empty() )
        {
            CPPUNIT_ASSERT_EQUAL( facename.Upper(), temp2.GetFaceName().Upper() );
        }

        CPPUNIT_ASSERT_EQUAL( test.GetPointSize(), temp2.GetPointSize() );
        CPPUNIT_ASSERT_EQUAL( test.GetEncoding(), temp2.GetEncoding() );
#endif
    }
}

#endif // wxUSE_FONTMAP
