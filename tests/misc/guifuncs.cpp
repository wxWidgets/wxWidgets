///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous GUI functions
// Author:      Vadim Zeitlin
// Created:     2008-09-22
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
#include "wx/panel.h"

#include "asserthelper.h"

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
        CPPUNIT_TEST( ClientToScreen );
        CPPUNIT_TEST( FindWindowAtPoint );
    CPPUNIT_TEST_SUITE_END();

    void DisplaySize();
    void URLDataObject();
    void ParseFileDialogFilter();
    void ClientToScreen();
    void FindWindowAtPoint();

    wxDECLARE_NO_COPY_CLASS(MiscGUIFuncsTestCase);
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
    CPPUNIT_ASSERT( sz.x < 1000 );
    CPPUNIT_ASSERT( sz.y < 1000 );
}

void MiscGUIFuncsTestCase::URLDataObject()
{
#if wxUSE_DATAOBJ
    // this tests for buffer overflow, see #11102
    const char * const
        url = "http://something.long.to.overwrite.plenty.memory.example.com";
    wxURLDataObject * const dobj = new wxURLDataObject(url);
    CPPUNIT_ASSERT_EQUAL( url, dobj->GetURL() );

    wxClipboardLocker lockClip;
    CPPUNIT_ASSERT( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();
#endif // wxUSE_DATAOBJ
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

void MiscGUIFuncsTestCase::ClientToScreen()
{
    wxWindow* const tlw = wxTheApp->GetTopWindow();
    CPPUNIT_ASSERT( tlw );

    wxPanel* const
        p1 = new wxPanel(tlw, wxID_ANY, wxPoint(0, 0), wxSize(100, 50));
    wxPanel* const
        p2 = new wxPanel(tlw, wxID_ANY, wxPoint(0, 50), wxSize(100, 50));
    wxWindow* const
        b = new wxWindow(p2, wxID_ANY, wxPoint(10, 10), wxSize(30, 10));

    // We need this to realize the windows created above under wxGTK.
    wxYield();

    const wxPoint tlwOrig = tlw->ClientToScreen(wxPoint(0, 0));

    CPPUNIT_ASSERT_EQUAL
    (
        tlwOrig + wxPoint(0, 50),
        p2->ClientToScreen(wxPoint(0, 0))
    );

    CPPUNIT_ASSERT_EQUAL
    (
        tlwOrig + wxPoint(10, 60),
        b->ClientToScreen(wxPoint(0, 0))
    );

    p1->Destroy();
    p2->Destroy();
}

namespace
{

// This class is used as a test window here. We can't use a real wxButton
// because we can't create other windows as its children in wxGTK.
class TestButton : public wxWindow
{
public:
    TestButton(wxWindow* parent, const wxString& label, const wxPoint& pos)
        : wxWindow(parent, wxID_ANY, pos, wxSize(100, 50))
    {
        SetLabel(label);
    }
};

// Helper function returning the label of the window at the given point or
// "NONE" if there is no window there.
wxString GetLabelOfWindowAtPoint(wxWindow* parent, int x, int y)
{
    wxWindow* const
        win = wxFindWindowAtPoint(parent->ClientToScreen(wxPoint(x, y)));
    return win ? win->GetLabel() : wxString("NONE");
}

} // anonymous namespace

void MiscGUIFuncsTestCase::FindWindowAtPoint()
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    CPPUNIT_ASSERT( parent );

    // Set a label to allow distinguishing it from the other windows in the
    // assertion messages.
    parent->SetLabel("parent");

    wxWindow* btn1 = new TestButton(parent, "1", wxPoint(10, 10));
    wxWindow* btn2 = new TestButton(parent, "2", wxPoint(10, 90));
    wxWindow* btn3 = new TestButton(btn2, "3", wxPoint(20, 20));

    // We need this to realize the windows created above under wxGTK.
    wxYield();

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "No window for a point outside of the window",
        "NONE",
        GetLabelOfWindowAtPoint(parent, 900, 900)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a child control corresponds to it",
        btn1->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 11, 11)
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point outside of any child control returns the TLW itself",
        parent->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 5, 5)
    );

    btn2->Disable();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a disabled child control still corresponds to it",
        btn2->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 11, 91)
    );

    btn2->Hide();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over a hidden child control doesn't take it into account",
        parent->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 11, 91)
    );

    btn2->Show();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over child control corresponds to the child",
        btn3->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 31, 111)
    );

    btn3->Disable();
    CPPUNIT_ASSERT_EQUAL_MESSAGE
    (
        "Point over disabled child controls still corresponds to this child",
        btn3->GetLabel(),
        GetLabelOfWindowAtPoint(parent, 31, 111)
    );

    btn1->Destroy();
    btn2->Destroy();
    // btn3 was already deleted when its parent was
}
