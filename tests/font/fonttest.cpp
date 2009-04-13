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
    CPPUNIT_TEST_SUITE_END();

    void GetSet();

    DECLARE_NO_COPY_CLASS(FontTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FontTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FontTestCase, "FontTestCase" );


void FontTestCase::GetSet()
{
    static const wxFont testfonts[] =
    {
        *wxNORMAL_FONT,
        *wxSMALL_FONT,
        *wxITALIC_FONT,
        *wxSWISS_FONT,
        wxFont(5, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)
    };

    for ( size_t n = 0; n < WXSIZEOF(testfonts); n++ )
    {
        wxFont test(testfonts[n]);

        // remember: getters can only be called when wxFont::IsOk() == true
        CPPUNIT_ASSERT( test.IsOk() );


        // test Get/SetFaceName()

        const wxString& fn = test.GetFaceName();
        CPPUNIT_ASSERT( !fn.empty() );

        CPPUNIT_ASSERT( !test.SetFaceName("a dummy face name") );
        CPPUNIT_ASSERT( !test.IsOk() );

        CPPUNIT_ASSERT( test.SetFaceName(fn) );
        CPPUNIT_ASSERT( test.IsOk() );


        // test Get/SetFamily()

        test.SetFamily( wxFONTFAMILY_ROMAN );
        CPPUNIT_ASSERT( test.IsOk() );
        CPPUNIT_ASSERT( wxFONTFAMILY_ROMAN == test.GetFamily() || 
                        wxFONTFAMILY_UNKNOWN == test.GetFamily() );
            // note that there is always the possibility that GetFamily() returns
            // wxFONTFAMILY_UNKNOWN so that we consider it as a valid return value


        // test Get/SetEncoding()

        //test.SetEncoding( wxFONTENCODING_KOI8 );
        //CPPUNIT_ASSERT( test.IsOk() );
        //CPPUNIT_ASSERT_EQUAL( wxFONTENCODING_KOI8 , test.GetEncoding() );

        
        // test Get/SetNativeFontInfo
        
        const wxString& nid = test.GetNativeFontInfoDesc();
        CPPUNIT_ASSERT( !nid.empty() );
            // documented to be never empty
        
        wxFont temp;
        CPPUNIT_ASSERT( temp.SetNativeFontInfo(nid) );
        CPPUNIT_ASSERT( temp.IsOk() );
        WX_ASSERT_MESSAGE( 
            ("Test #%lu failed; native info desc was \"%s\" for test and \"%s\" for temp", \
             n, nid, temp.GetNativeFontInfoDesc()),
            temp == test );

        
        // test Get/SetNativeFontInfoUserDesc 
        
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
        CPPUNIT_ASSERT( test.GetFaceName().CmpNoCase(temp2.GetFaceName()) == 0 );
        CPPUNIT_ASSERT_EQUAL( test.GetPointSize(), temp2.GetPointSize() );
        CPPUNIT_ASSERT_EQUAL( test.GetEncoding(), temp2.GetEncoding() );
#endif


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

#endif // wxUSE_FONTMAP
