///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/windowtest.cpp
// Purpose:     wxWindow unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/button.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "asserthelper.h"
#include "testableframe.h"
#include "testwindow.h"

#include "wx/uiaction.h"
#include "wx/caret.h"
#include "wx/cshelp.h"
#include "wx/dcclient.h"
#include "wx/stopwatch.h"
#include "wx/tooltip.h"
#include "wx/wupdlock.h"

#include <memory>

class WindowTestCase
{
public:
    WindowTestCase()
        : m_window(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY))
    {
    #ifdef __WXGTK3__
        // Without this, when running this test suite solo it succeeds,
        // but not when running it together with the other tests !!
        // Not needed when run under Xvfb display.
        for ( wxStopWatch sw; sw.Time() < 50; )
            wxYield();
    #endif
    }

    ~WindowTestCase()
    {
        wxTheApp->GetTopWindow()->DestroyChildren();
    }

protected:
    wxWindow* const m_window;

    wxDECLARE_NO_COPY_CLASS(WindowTestCase);
};

static void DoTestShowHideEvent(wxWindow* window)
{
    EventCounter show(window, wxEVT_SHOW);

    CHECK(window->IsShown());

    window->Show(false);

    CHECK(!window->IsShown());

    window->Show();

    CHECK(window->IsShown());

    CHECK( show.GetCount() == 2 );
}

TEST_CASE_METHOD(WindowTestCase, "Window::ShowHideEvent", "[window]")
{
    SECTION("Normal window")
    {
        DoTestShowHideEvent(m_window);
    }

    SECTION("Frozen window")
    {
        wxWindowUpdateLocker freeze(m_window->GetParent() );
        REQUIRE( m_window->IsFrozen() );

        DoTestShowHideEvent(m_window);
    }
}

TEST_CASE_METHOD(WindowTestCase, "Window::KeyEvent", "[window]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter keydown(m_window, wxEVT_KEY_DOWN);
    EventCounter keyup(m_window, wxEVT_KEY_UP);
    EventCounter keychar(m_window, wxEVT_CHAR);

    wxUIActionSimulator sim;

    m_window->SetFocus();
    wxYield();

    sim.Text("text");
    sim.Char(WXK_SHIFT);
    wxYield();

    CHECK( keydown.GetCount() == 5 );
    CHECK( keyup.GetCount() == 5 );
    CHECK( keychar.GetCount() == 4 );
#endif
}

TEST_CASE_METHOD(WindowTestCase, "Window::FocusEvent", "[window]")
{
#ifndef __WXOSX__
    if ( IsAutomaticTest() )
    {
        // Skip this test when running under buildbot, it fails there for
        // unknown reason and this failure can't be reproduced locally.
        return;
    }

    EventCounter setfocus(m_window, wxEVT_SET_FOCUS);
    EventCounter killfocus(m_window, wxEVT_KILL_FOCUS);

    m_window->SetFocus();

    CHECK(setfocus.WaitEvent(500));
    CHECK_FOCUS_IS( m_window );

    wxButton* button = new wxButton(wxTheApp->GetTopWindow(), wxID_ANY);

    wxYield();
    button->SetFocus();

    CHECK( killfocus.GetCount() == 1 );
    CHECK(!m_window->HasFocus());
#endif
}

TEST_CASE_METHOD(WindowTestCase, "Window::Mouse", "[window]")
{
    wxCursor cursor(wxCURSOR_CHAR);
    m_window->SetCursor(cursor);

    CHECK(m_window->GetCursor().IsOk());

#if wxUSE_CARET
    CHECK(!m_window->GetCaret());

    wxCaret* caret = nullptr;

    // Try creating the caret in two different, but normally equivalent, ways.
    SECTION("Caret 1-step")
    {
        caret = new wxCaret(m_window, 16, 16);
    }

    SECTION("Caret 2-step")
    {
        caret = new wxCaret();
        caret->Create(m_window, 16, 16);
    }

    m_window->SetCaret(caret);

    CHECK(m_window->GetCaret()->IsOk());
#endif

    m_window->CaptureMouse();

    CHECK(m_window->HasCapture());

    m_window->ReleaseMouse();

    CHECK(!m_window->HasCapture());
}

TEST_CASE_METHOD(WindowTestCase, "Window::Properties", "[window]")
{
    m_window->SetLabel("label");

    CHECK( m_window->GetLabel() == "label" );

    m_window->SetName("name");

    CHECK( m_window->GetName() == "name" );

    //As we used wxID_ANY we should have a negative id
    CHECK(m_window->GetId() < 0);

    m_window->SetId(wxID_HIGHEST + 10);

    CHECK( m_window->GetId() == wxID_HIGHEST + 10 );
}

#if wxUSE_TOOLTIPS
TEST_CASE_METHOD(WindowTestCase, "Window::ToolTip", "[window]")
{
    CHECK(!m_window->GetToolTip());
    CHECK( m_window->GetToolTipText() == "" );

    m_window->SetToolTip("text tip");

    CHECK( m_window->GetToolTipText() == "text tip" );

    m_window->UnsetToolTip();

    CHECK(!m_window->GetToolTip());
    CHECK( m_window->GetToolTipText() == "" );

    wxToolTip* tip = new wxToolTip("other tip");

    m_window->SetToolTip(tip);

    CHECK( m_window->GetToolTip() == tip );
    CHECK( m_window->GetToolTipText() == "other tip" );
}
#endif // wxUSE_TOOLTIPS

TEST_CASE_METHOD(WindowTestCase, "Window::Help", "[window]")
{
#if wxUSE_HELP
    wxHelpProvider::Set(new wxSimpleHelpProvider());

    CHECK( m_window->GetHelpText() == "" );

    m_window->SetHelpText("helptext");

    CHECK( m_window->GetHelpText() == "helptext" );
#endif
}

TEST_CASE_METHOD(WindowTestCase, "Window::Parent", "[window]")
{
    CHECK( m_window->GetGrandParent() == static_cast<wxWindow*>(nullptr) );
    CHECK( m_window->GetParent() == wxTheApp->GetTopWindow() );
}

TEST_CASE_METHOD(WindowTestCase, "Window::Siblings", "[window]")
{
    CHECK( m_window->GetNextSibling() == static_cast<wxWindow*>(nullptr) );
    CHECK( m_window->GetPrevSibling() == static_cast<wxWindow*>(nullptr) );

    wxWindow* newwin = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);

    CHECK( m_window->GetNextSibling() == newwin );
    CHECK( m_window->GetPrevSibling() == static_cast<wxWindow*>(nullptr) );

    CHECK( newwin->GetNextSibling() == static_cast<wxWindow*>(nullptr) );
    CHECK( newwin->GetPrevSibling() == m_window );

    wxDELETE(newwin);
}

TEST_CASE_METHOD(WindowTestCase, "Window::Children", "[window]")
{
    CHECK( m_window->GetChildren().GetCount() == 0 );

    wxWindow* child1 = new wxWindow(m_window, wxID_ANY);

    CHECK( m_window->GetChildren().GetCount() == 1 );

    m_window->RemoveChild(child1);

    CHECK( m_window->GetChildren().GetCount() == 0 );

    child1->SetId(wxID_HIGHEST + 1);
    child1->SetName("child1");

    m_window->AddChild(child1);

    CHECK( m_window->GetChildren().GetCount() == 1 );
    CHECK( m_window->FindWindow(wxID_HIGHEST + 1) == child1 );
    CHECK( m_window->FindWindow("child1") == child1 );

    m_window->DestroyChildren();

    CHECK( m_window->GetChildren().GetCount() == 0 );
}

TEST_CASE_METHOD(WindowTestCase, "Window::Focus", "[window]")
{
#ifndef __WXOSX__
    CHECK(!m_window->HasFocus());

    if ( m_window->AcceptsFocus() )
    {
        m_window->SetFocus();
        CHECK_FOCUS_IS(m_window);
    }

    //Set the focus back to the main window
    wxTheApp->GetTopWindow()->SetFocus();

    if ( m_window->AcceptsFocusFromKeyboard() )
    {
        m_window->SetFocusFromKbd();
        CHECK_FOCUS_IS(m_window);
    }
#endif
}

TEST_CASE_METHOD(WindowTestCase, "Window::Positioning", "[window]")
{
    //Some basic tests for consistency
    int x, y;
    m_window->GetPosition(&x, &y);

    CHECK( m_window->GetPosition().x == x );
    CHECK( m_window->GetPosition().y == y );
    CHECK( m_window->GetRect().GetTopLeft() == m_window->GetPosition() );

    m_window->GetScreenPosition(&x, &y);
    CHECK( m_window->GetScreenPosition().x == x );
    CHECK( m_window->GetScreenPosition().y == y );
    CHECK( m_window->GetScreenRect().GetTopLeft() == m_window->GetScreenPosition() );
}

TEST_CASE_METHOD(WindowTestCase, "Window::PositioningBeyondShortLimit", "[window]")
{
#ifdef __WXMSW__
    //Positioning under MSW is limited to short relative coordinates

    //
    //Test window creation beyond SHRT_MAX
    int commonDim = 10;
    wxWindow* w = new wxWindow(m_window, wxID_ANY,
                               wxPoint(0, SHRT_MAX + commonDim),
                               wxSize(commonDim, commonDim));
    CHECK( w->GetPosition().y == SHRT_MAX + commonDim );

    w->Move(0, 0);

    //
    //Test window moving beyond SHRT_MAX
    w->Move(0, SHRT_MAX + commonDim);
    CHECK( w->GetPosition().y == SHRT_MAX + commonDim );

    //
    //Test window moving below SHRT_MIN
    w->Move(0, SHRT_MIN - commonDim);
    CHECK( w->GetPosition().y == SHRT_MIN - commonDim );

    //
    //Test deferred move beyond SHRT_MAX
    m_window->SetVirtualSize(-1, SHRT_MAX + 2 * commonDim);
    wxWindow* bigWin = new wxWindow(m_window, wxID_ANY, wxDefaultPosition,
                                    //size is also limited by SHRT_MAX
                                    wxSize(commonDim, SHRT_MAX));
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(bigWin);
    sizer->AddSpacer(commonDim); //add some space to go beyond SHRT_MAX
    sizer->Add(w);
    m_window->SetSizer(sizer);
    m_window->Layout();
    CHECK( w->GetPosition().y == SHRT_MAX + commonDim );
#endif
}

TEST_CASE_METHOD(WindowTestCase, "Window::Show", "[window]")
{
    CHECK(m_window->IsShown());

    m_window->Hide();

    CHECK(!m_window->IsShown());

    m_window->Show();

    CHECK(m_window->IsShown());

    m_window->Show(false);

    CHECK(!m_window->IsShown());

    m_window->ShowWithEffect(wxSHOW_EFFECT_BLEND);

    CHECK(m_window->IsShown());

    m_window->HideWithEffect(wxSHOW_EFFECT_BLEND);

    CHECK(!m_window->IsShown());
}

TEST_CASE_METHOD(WindowTestCase, "Window::Enable", "[window]")
{
    CHECK(m_window->IsEnabled());

    m_window->Disable();

    CHECK(!m_window->IsEnabled());

    m_window->Enable();

    CHECK(m_window->IsEnabled());

    m_window->Enable(false);

    CHECK(!m_window->IsEnabled());
    m_window->Enable();


    wxWindow* const child = new wxWindow(m_window, wxID_ANY);
    CHECK(child->IsEnabled());
    CHECK(child->IsThisEnabled());

    m_window->Disable();
    CHECK(!child->IsEnabled());
    CHECK(child->IsThisEnabled());

    child->Disable();
    CHECK(!child->IsEnabled());
    CHECK(!child->IsThisEnabled());

    m_window->Enable();
    CHECK(!child->IsEnabled());
    CHECK(!child->IsThisEnabled());

    child->Enable();
    CHECK(child->IsEnabled());
    CHECK(child->IsThisEnabled());
}

TEST_CASE_METHOD(WindowTestCase, "Window::FindWindowBy", "[window]")
{
    m_window->SetId(wxID_HIGHEST + 1);
    m_window->SetName("name");
    m_window->SetLabel("label");

    CHECK( wxWindow::FindWindowById(wxID_HIGHEST + 1) == m_window );
    CHECK( wxWindow::FindWindowByName("name") == m_window );
    CHECK( wxWindow::FindWindowByLabel("label") == m_window );

    CHECK( wxWindow::FindWindowById(wxID_HIGHEST + 3) == nullptr );
    CHECK( wxWindow::FindWindowByName("noname") == nullptr );
    CHECK( wxWindow::FindWindowByLabel("nolabel") == nullptr );
}

TEST_CASE_METHOD(WindowTestCase, "Window::SizerErrors", "[window][sizer][error]")
{
    wxWindow* const child = new wxWindow(m_window, wxID_ANY);
    std::unique_ptr<wxSizer> const sizer1(new wxBoxSizer(wxHORIZONTAL));
    std::unique_ptr<wxSizer> const sizer2(new wxBoxSizer(wxHORIZONTAL));

    REQUIRE_NOTHROW( sizer1->Add(child) );
#ifdef __WXDEBUG__
    CHECK_THROWS_AS( sizer1->Add(child), TestAssertFailure );
    CHECK_THROWS_AS( sizer2->Add(child), TestAssertFailure );
#else
    CHECK_NOTHROW( sizer1->Add(child) );
    CHECK_NOTHROW( sizer2->Add(child) );
#endif

    CHECK_NOTHROW( sizer1->Detach(child) );
    CHECK_NOTHROW( sizer2->Add(child) );

    REQUIRE_NOTHROW( delete child );
}

TEST_CASE_METHOD(WindowTestCase, "Window::Refresh", "[window]")
{
    wxWindow* const parent = m_window;
    wxWindow* const child1 = new wxWindow(parent, wxID_ANY, wxPoint(10, 20), wxSize(80, 50));
    wxWindow* const child2 = new wxWindow(parent, wxID_ANY, wxPoint(110, 20), wxSize(80, 50));
    wxWindow* const child3 = new wxWindow(parent, wxID_ANY, wxPoint(210, 20), wxSize(80, 50));

    m_window->SetSize(300, 100);

    // to help see the windows when debugging
    parent->SetBackgroundColour(*wxBLACK);
    child1->SetBackgroundColour(*wxBLUE);
    child2->SetBackgroundColour(*wxRED);
    child3->SetBackgroundColour(*wxGREEN);

    // Notice that using EventCounter here will give incorrect results,
    // so we have to bind each window to a distinct event handler instead.

    bool isParentPainted;
    bool isChild1Painted;
    bool isChild2Painted;
    bool isChild3Painted;

    const auto setFlagOnPaint = [](wxWindow* win, bool* flag)
    {
        win->Bind(wxEVT_PAINT, [=](wxPaintEvent&)
        {
            wxPaintDC dc(win);
            *flag = true;
        });
    };

    setFlagOnPaint(parent, &isParentPainted);
    setFlagOnPaint(child1, &isChild1Painted);
    setFlagOnPaint(child2, &isChild2Painted);
    setFlagOnPaint(child3, &isChild3Painted);

    // Prepare for the RefreshRect() call below
    wxYield();

    // Now initialize/reset the flags before calling RefreshRect()
    isParentPainted =
    isChild1Painted =
    isChild2Painted =
    isChild3Painted = false;

    parent->RefreshRect(wxRect(150, 10, 300, 80));

    for ( wxStopWatch sw; sw.Time() < 100; )
    {
        if ( isParentPainted )
            break;

        wxYield();
    }

    // child1 should be the only window not to receive the wxEVT_PAINT event
    // because it does not intersect with the refreshed rectangle.
    CHECK(isParentPainted == true);
    CHECK(isChild1Painted == false);
    CHECK(isChild2Painted == true);
    CHECK(isChild3Painted == true);
}
