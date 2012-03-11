///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous GUI functions
// Author:      Vadim Zeitlin
// Created:     2008-09-22
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/filefn.h"
#endif // !PCH

#include "wx/app.h"
#include "wx/button.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MiscGUIFuncsTestCase : public CppUnit::TestCase
{
public:
    MiscGUIFuncsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MiscGUIFuncsTestCase );
        CPPUNIT_TEST( DisplaySize );
        CPPUNIT_TEST( URLDataObject );
        CPPUNIT_TEST( ParseFileDialogFilter );
        CPPUNIT_TEST( FindWindowAtPoint );
    CPPUNIT_TEST_SUITE_END();

    void DisplaySize();
    void URLDataObject();
    void ParseFileDialogFilter();
    void FindWindowAtPoint();

    DECLARE_NO_COPY_CLASS(MiscGUIFuncsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MiscGUIFuncsTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MiscGUIFuncsTestCase, "MiscGUIFuncsTestCase" );

void MiscGUIFuncsTestCase::DisplaySize()
{
    // test that different (almost) overloads return the same results
    int w, h;
    wxDisplaySize(&w, &h);
    wxSize sz = wxGetDisplaySize();

    CPPUNIT_ASSERT_EQUAL( w, sz.x );
    CPPUNIT_ASSERT_EQUAL( h, sz.y );

    // test that passing NULL works as expected, e.g. doesn't crash
    wxDisplaySize(NULL, NULL);
    wxDisplaySize(&w, NULL);
    wxDisplaySize(NULL, &h);

    CPPUNIT_ASSERT_EQUAL( w, sz.x );
    CPPUNIT_ASSERT_EQUAL( h, sz.y );

    // test that display PPI is something reasonable
    sz = wxGetDisplayPPI();
    CPPUNIT_ASSERT( sz.x < 1000 && sz.y < 1000 );
}

void MiscGUIFuncsTestCase::URLDataObject()
{
    // this tests for buffer overflow, see #11102
    const char * const
        url = "http://something.long.to.overwrite.plenty.memory.example.com";
    wxURLDataObject * const dobj = new wxURLDataObject(url);
    CPPUNIT_ASSERT_EQUAL( url, dobj->GetURL() );

    wxClipboardLocker lockClip;
    CPPUNIT_ASSERT( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();
}

void MiscGUIFuncsTestCase::ParseFileDialogFilter()
{
    wxArrayString descs,
                  filters;

    CPPUNIT_ASSERT_EQUAL
    (
        1,
        wxParseCommonDialogsFilter("Image files|*.jpg;*.png", descs, filters)
    );

    CPPUNIT_ASSERT_EQUAL( "Image files", descs[0] );
    CPPUNIT_ASSERT_EQUAL( "*.jpg;*.png", filters[0] );

    CPPUNIT_ASSERT_EQUAL
    (
        2,
        wxParseCommonDialogsFilter
        (
            "All files (*.*)|*.*|Python source (*.py)|*.py",
            descs, filters
        )
    );

    CPPUNIT_ASSERT_EQUAL( "*.*", filters[0] );
    CPPUNIT_ASSERT_EQUAL( "*.py", filters[1] );

    // Test some invalid ones too.
    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxParseCommonDialogsFilter
        (
            "All files (*.*)|*.*|Python source (*.py)|*.py|",
            descs, filters
        )
    );
}

void MiscGUIFuncsTestCase::FindWindowAtPoint()
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    CPPUNIT_ASSERT( parent );

    CPPUNIT_ASSERT_MESSAGE
    (
        "No window for a point outside of the window",
        !wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(900, 900)))
    );

    wxWindow* btn1 = new wxButton(parent, wxID_ANY, "1", wxPoint(10, 10));
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a child control corresponds to it",
        btn1,
        wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(11, 11)))
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point outside of any child control returns the TLW itself",
        parent,
        wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(5, 5)))
    );

    wxWindow* btn2 = new wxButton(parent, wxID_ANY, "2", wxPoint(10, 90));
    btn2->Disable();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a disabled child control still corresponds to it",
        btn2,
        wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(11, 91)))
    );

    btn2->Hide();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a hidden child control doesn't take it into account",
        parent,
        wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(11, 91)))
    );

    wxDELETE(btn1);
    wxDELETE(btn2);
}
