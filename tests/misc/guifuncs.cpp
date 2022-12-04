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
#include "wx/scopedptr.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("GUI::DisplaySize", "[guifuncs]")
{
    // test that different (almost) overloads return the same results
    int w, h;
    wxDisplaySize(&w, &h);
    wxSize sz = wxGetDisplaySize();

    CHECK( sz.x == w );
    CHECK( sz.y == h );

    // test that passing NULL works as expected, e.g. doesn't crash
    wxDisplaySize(NULL, NULL);
    wxDisplaySize(&w, NULL);
    wxDisplaySize(NULL, &h);

    CHECK( sz.x == w );
    CHECK( sz.y == h );

    // test that display PPI is something reasonable
    sz = wxGetDisplayPPI();
    CHECK( sz.x < 1000 );
    CHECK( sz.y < 1000 );
}

#if wxUSE_DATAOBJ
TEST_CASE("GUI::TextDataObject", "[guifuncs][clipboard]")
{
    const wxString text("Hello clipboard!");

    wxTextDataObject* const dobj = new wxTextDataObject(text);
    CHECK( dobj->GetText() == text );

    wxClipboardLocker lockClip;
    CHECK( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();

    wxTextDataObject dobj2;
    REQUIRE( wxTheClipboard->GetData(dobj2) );
    CHECK( dobj2.GetText() == text );
}

TEST_CASE("GUI::URLDataObject", "[guifuncs][clipboard]")
{
    // this tests for buffer overflow, see #11102
    const char * const
        url = "http://something.long.to.overwrite.plenty.memory.example.com";
    wxURLDataObject * const dobj = new wxURLDataObject(url);
    CHECK( dobj->GetURL() == url );

    wxClipboardLocker lockClip;
    CHECK( wxTheClipboard->SetData(dobj) );
    wxTheClipboard->Flush();

    wxURLDataObject dobj2;
    REQUIRE( wxTheClipboard->GetData(dobj2) );
    CHECK( dobj2.GetURL() == url );
}

TEST_CASE("GUI::DataFormatCompare", "[guifuncs][dataformat]")
{
    const wxDataFormat df(wxDF_TEXT);
    CHECK( df == wxDF_TEXT );
    CHECK( df != wxDF_INVALID );
}
#endif // wxUSE_DATAOBJ

TEST_CASE("GUI::ParseFileDialogFilter", "[guifuncs]")
{
    wxArrayString descs,
                  filters;

    REQUIRE
    (
        wxParseCommonDialogsFilter("Image files|*.jpg;*.png", descs, filters)
        == 1
    );

    CHECK( descs[0] == "Image files" );
    CHECK( filters[0] == "*.jpg;*.png" );

    REQUIRE
    (
        wxParseCommonDialogsFilter
        (
            "All files (*.*)|*.*|Python source (*.py)|*.py",
            descs, filters
        )
        == 2
    );

    CHECK( filters[0] == "*.*" );
    CHECK( filters[1] == "*.py" );

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

TEST_CASE("GUI::ClientToScreen", "[guifuncs]")
{
    wxWindow* const tlw = wxTheApp->GetTopWindow();
    REQUIRE( tlw );

    wxScopedPtr<wxPanel> const
        p1(new wxPanel(tlw, wxID_ANY, wxPoint(0, 0), wxSize(100, 50)));
    wxScopedPtr<wxPanel> const
        p2(new wxPanel(tlw, wxID_ANY, wxPoint(0, 50), wxSize(100, 50)));
    wxWindow* const
        b = new wxWindow(p2.get(), wxID_ANY, wxPoint(10, 10), wxSize(30, 10));

    // We need this to realize the windows created above under wxGTK.
    wxYield();

    const wxPoint tlwOrig = tlw->ClientToScreen(wxPoint(0, 0));

    CHECK( p2->ClientToScreen(wxPoint(0, 0)) == tlwOrig + wxPoint(0, 50) );

    CHECK( b->ClientToScreen(wxPoint(0, 0)) == tlwOrig + wxPoint(10, 60) );
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

TEST_CASE("GUI::FindWindowAtPoint", "[guifuncs]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();
    REQUIRE( parent );

    // Set a label to allow distinguishing it from the other windows in the
    // assertion messages.
    parent->SetLabel("parent");

    wxScopedPtr<wxWindow> btn1(new TestButton(parent, "1", wxPoint(10, 10)));
    wxScopedPtr<wxWindow> btn2(new TestButton(parent, "2", wxPoint(10, 90)));

    // No need to use wxScopedPtr<> for this one, it will be deleted by btn2.
    wxWindow* btn3 = new TestButton(btn2.get(), "3", wxPoint(20, 20));

    // We need this to realize the windows created above under wxGTK.
    wxYield();

    INFO("No window for a point outside of the window");
    CHECK( GetLabelOfWindowAtPoint(parent, 900, 900) == "NONE" );

    INFO( "Point over a child control corresponds to it" );
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 11) == btn1->GetLabel() );

    INFO("Point outside of any child control returns the TLW itself");
    CHECK( GetLabelOfWindowAtPoint(parent, 5, 5) == parent->GetLabel() );

    btn2->Disable();
    INFO("Point over a disabled child control still corresponds to it");
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 91) == btn2->GetLabel() );

    btn2->Hide();
    INFO("Point over a hidden child control doesn't take it into account");
    CHECK( GetLabelOfWindowAtPoint(parent, 11, 91) == parent->GetLabel() );

    btn2->Show();
    INFO("Point over child control corresponds to the child");
    CHECK( GetLabelOfWindowAtPoint(parent, 31, 111) == btn3->GetLabel() );

    btn3->Disable();
    INFO("Point over disabled child controls still corresponds to this child");
    CHECK( GetLabelOfWindowAtPoint(parent, 31, 111) == btn3->GetLabel() );
}

TEST_CASE("wxWindow::Dump", "[window]")
{
    CHECK_NOTHROW( wxDumpWindow(NULL) );

    wxScopedPtr<wxButton>
        button(new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "bloordyblop"));

    const std::string s = wxDumpWindow(button.get()).utf8_string();

    CHECK_THAT( s, Catch::Contains("wxButton") );
    CHECK_THAT( s, Catch::Contains("bloordyblop") );
}
