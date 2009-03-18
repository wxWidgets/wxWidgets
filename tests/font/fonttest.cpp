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

        test.SetFamily( wxFONTFAMILY_MODERN );
        //CPPUNIT_ASSERT_EQUAL( wxFONTFAMILY_MODERN, test.GetFamily() );

        
        // test Get/SetEncoding()

        //test.SetEncoding( wxFONTENCODING_KOI8 );
        //CPPUNIT_ASSERT_EQUAL( wxFONTENCODING_KOI8 , test.GetEncoding() );

        
        // test Get/SetNativeFontInfo
        
        const wxString& nid = test.GetNativeFontInfoDesc();
        CPPUNIT_ASSERT( !nid.empty() );
        
        wxFont temp;
        temp.SetNativeFontInfo(nid);
        CPPUNIT_ASSERT( temp == test );
        
        
        // test Get/SetNativeFontInfoUserDesc 
        
        const wxString& niud = test.GetNativeFontInfoUserDesc();
        CPPUNIT_ASSERT( !niud.empty() );
        
        wxFont temp2;
        temp2.SetNativeFontInfoUserDesc(niud);
        CPPUNIT_ASSERT( temp2 == test );
        
        
        // test Get/SetPointSize()
        
        test.SetPointSize(30);
        CPPUNIT_ASSERT_EQUAL( 30, test.GetPointSize() );
        
        
        // test Get/SetPixelSize()
        
        test.SetPixelSize(wxSize(0,30));
        CPPUNIT_ASSERT( test.GetPixelSize().GetHeight() <= 30 );
            // NOTE: the match found by SetPixelSize() may be not 100% precise; it
            //       only grants that a font smaller than the required height will
            //       be selected

        
        // test Get/SetStyle()
        
        test.SetStyle(wxFONTSTYLE_SLANT);
        CPPUNIT_ASSERT_EQUAL( wxFONTSTYLE_SLANT, test.GetStyle() );
        
        
        // test Get/SetUnderlined()
        
        test.SetUnderlined(true);
        CPPUNIT_ASSERT_EQUAL( true, test.GetUnderlined() );
        
        
        // test Get/SetWeight()
        
        test.SetWeight(wxFONTWEIGHT_BOLD);
        CPPUNIT_ASSERT_EQUAL( wxFONTWEIGHT_BOLD, test.GetWeight() );
    }
}

#endif // wxUSE_FONTMAP
