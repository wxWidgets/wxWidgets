///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/windowtest.cpp
// Purpose:     wxWindow unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
#include "wx/scopedptr.h"
#include "wx/tooltip.h"

class WindowTestCase
{
public:
    WindowTestCase()
        : m_window(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY))
    {
    }

    ~WindowTestCase()
    {
        wxTheApp->GetTopWindow()->DestroyChildren();
    }

protected:
    wxWindow* const m_window;

    wxDECLARE_NO_COPY_CLASS(WindowTestCase);
};

TEST_CASE_METHOD(WindowTestCase, "Window::ShowHideEvent", "[window]")
{
#if defined(__WXMSW__)
    EventCounter show(m_window, wxEVT_SHOW);

    CHECK(m_window->IsShown());

    m_window->Show(false);

    CHECK(!m_window->IsShown());

    m_window->Show();

    CHECK(m_window->IsShown());

    CHECK( show.GetCount() == 2 );
#endif // __WXMSW__
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

    //A plain window doesn't have a caret
    CHECK(!m_window->GetCaret());

    wxCaret* caret = new wxCaret(m_window, 16, 16);
    m_window->SetCaret(caret);

    CHECK(m_window->GetCaret()->IsOk());

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
    wxHelpProvider::Set(new wxSimpleHelpProvider());

    CHECK( m_window->GetHelpText() == "" );

    m_window->SetHelpText("helptext");

    CHECK( m_window->GetHelpText() == "helptext" );
}

TEST_CASE_METHOD(WindowTestCase, "Window::Parent", "[window]")
{
    CHECK( m_window->GetGrandParent() == static_cast<wxWindow*>(NULL) );
    CHECK( m_window->GetParent() == wxTheApp->GetTopWindow() );
}

TEST_CASE_METHOD(WindowTestCase, "Window::Siblings", "[window]")
{
    CHECK( m_window->GetNextSibling() == static_cast<wxWindow*>(NULL) );
    CHECK( m_window->GetPrevSibling() == static_cast<wxWindow*>(NULL) );

    wxWindow* newwin = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);

    CHECK( m_window->GetNextSibling() == newwin );
    CHECK( m_window->GetPrevSibling() == static_cast<wxWindow*>(NULL) );

    CHECK( newwin->GetNextSibling() == static_cast<wxWindow*>(NULL) );
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

    CHECK( wxWindow::FindWindowById(wxID_HIGHEST + 3) == NULL );
    CHECK( wxWindow::FindWindowByName("noname") == NULL );
    CHECK( wxWindow::FindWindowByLabel("nolabel") == NULL );
}

TEST_CASE_METHOD(WindowTestCase, "Window::SizerErrors", "[window][sizer][error]")
{
    wxWindow* const child = new wxWindow(m_window, wxID_ANY);
    wxScopedPtr<wxSizer> const sizer1(new wxBoxSizer(wxHORIZONTAL));
    wxScopedPtr<wxSizer> const sizer2(new wxBoxSizer(wxHORIZONTAL));

    REQUIRE_NOTHROW( sizer1->Add(child) );
    CHECK_THROWS_AS( sizer1->Add(child), TestAssertFailure );
    CHECK_THROWS_AS( sizer2->Add(child), TestAssertFailure );

    CHECK_NOTHROW( sizer1->Detach(child) );
    CHECK_NOTHROW( sizer2->Add(child) );

    REQUIRE_NOTHROW( delete child );
}
