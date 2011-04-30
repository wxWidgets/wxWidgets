///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/windowtest.cpp
// Purpose:     wxWindow unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// RCS-ID:      $Id$
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
#endif // WX_PRECOMP

#include "asserthelper.h"
#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/caret.h"
#include "wx/cshelp.h"
#include "wx/tooltip.h"

class WindowTestCase : public CppUnit::TestCase
{
public:
    WindowTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( WindowTestCase );
        CPPUNIT_TEST( ShowHideEvent );
        WXUISIM_TEST( KeyEvent );
        CPPUNIT_TEST( FocusEvent );
        CPPUNIT_TEST( Mouse );
        CPPUNIT_TEST( Properties );
#if wxUSE_TOOLTIPS
        CPPUNIT_TEST( ToolTip );
#endif // wxUSE_TOOLTIPS
        CPPUNIT_TEST( Help );
        CPPUNIT_TEST( Parent );
        CPPUNIT_TEST( Siblings );
        CPPUNIT_TEST( Children );
        CPPUNIT_TEST( Focus );
        CPPUNIT_TEST( Positioning );
        CPPUNIT_TEST( Show );
        CPPUNIT_TEST( Enable );
        CPPUNIT_TEST( FindWindowBy );
    CPPUNIT_TEST_SUITE_END();

    void ShowHideEvent();
    void KeyEvent();
    void FocusEvent();
    void Mouse();
    void Properties();
#if wxUSE_TOOLTIPS
    void ToolTip();
#endif // wxUSE_TOOLTIPS
    void Help();
    void Parent();
    void Siblings();
    void Children();
    void Focus();
    void Positioning();
    void Show();
    void Enable();
    void FindWindowBy();

    wxWindow *m_window;

    DECLARE_NO_COPY_CLASS(WindowTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WindowTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WindowTestCase, "WindowTestCase" );

void WindowTestCase::setUp()
{
    m_window = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
}

void WindowTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void WindowTestCase::ShowHideEvent()
{
#if defined(__WXMSW__) || defined (__WXPM__)
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_window, wxEVT_SHOW);

    CPPUNIT_ASSERT(m_window->IsShown());

    m_window->Show(false);

    CPPUNIT_ASSERT(!m_window->IsShown());

    m_window->Show();

    CPPUNIT_ASSERT(m_window->IsShown());

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount());
#endif
}

void WindowTestCase::KeyEvent()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_window, wxEVT_KEY_DOWN);
    EventCounter count1(m_window, wxEVT_KEY_UP);
    EventCounter count2(m_window, wxEVT_CHAR);

    wxUIActionSimulator sim;

    m_window->SetFocus();

    sim.Text("text");
    sim.Char(WXK_SHIFT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(5, frame->GetEventCount(wxEVT_KEY_DOWN));
    CPPUNIT_ASSERT_EQUAL(5, frame->GetEventCount(wxEVT_KEY_UP));
    CPPUNIT_ASSERT_EQUAL(4, frame->GetEventCount(wxEVT_CHAR));
#endif
}

void WindowTestCase::FocusEvent()
{
#ifndef __WXOSX__
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_window, wxEVT_SET_FOCUS);
    EventCounter count1(m_window, wxEVT_KILL_FOCUS);

    m_window->SetFocus();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SET_FOCUS));
    CPPUNIT_ASSERT(m_window->HasFocus());

    wxButton* button = new wxButton(wxTheApp->GetTopWindow(), wxID_ANY);

    button->SetFocus();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_KILL_FOCUS));
    CPPUNIT_ASSERT(!m_window->HasFocus());
#endif
}

void WindowTestCase::Mouse()
{
    wxCursor cursor(wxCURSOR_CHAR);
    m_window->SetCursor(cursor);

    CPPUNIT_ASSERT(m_window->GetCursor().IsOk());

    //A plain window doesn't have a caret
    CPPUNIT_ASSERT(!m_window->GetCaret());

    wxCaret* caret = new wxCaret(m_window, 16, 16);
    m_window->SetCaret(caret);

    CPPUNIT_ASSERT(m_window->GetCaret()->IsOk());

    m_window->CaptureMouse();

    CPPUNIT_ASSERT(m_window->HasCapture());

    m_window->ReleaseMouse();

    CPPUNIT_ASSERT(!m_window->HasCapture());
}

void WindowTestCase::Properties()
{
    m_window->SetLabel("label");

    CPPUNIT_ASSERT_EQUAL("label", m_window->GetLabel());

    m_window->SetName("name");

    CPPUNIT_ASSERT_EQUAL("name", m_window->GetName());

    //As we used wxID_ANY we should have a negative id
    CPPUNIT_ASSERT(m_window->GetId() < 0);

    m_window->SetId(wxID_HIGHEST + 10);

    CPPUNIT_ASSERT_EQUAL(wxID_HIGHEST + 10, m_window->GetId());
}

#if wxUSE_TOOLTIPS
void WindowTestCase::ToolTip()
{
    CPPUNIT_ASSERT(!m_window->GetToolTip());
    CPPUNIT_ASSERT_EQUAL("", m_window->GetToolTipText());

    m_window->SetToolTip("text tip");

    CPPUNIT_ASSERT_EQUAL("text tip", m_window->GetToolTipText());

    m_window->UnsetToolTip();

    CPPUNIT_ASSERT(!m_window->GetToolTip());
    CPPUNIT_ASSERT_EQUAL("", m_window->GetToolTipText());

    wxToolTip* tip = new wxToolTip("other tip");

    m_window->SetToolTip(tip);

    CPPUNIT_ASSERT_EQUAL(tip, m_window->GetToolTip());
    CPPUNIT_ASSERT_EQUAL("other tip", m_window->GetToolTipText());
}
#endif // wxUSE_TOOLTIPS

void WindowTestCase::Help()
{
    wxHelpProvider::Set(new wxSimpleHelpProvider());

    CPPUNIT_ASSERT_EQUAL("", m_window->GetHelpText());

    m_window->SetHelpText("helptext");

    CPPUNIT_ASSERT_EQUAL("helptext", m_window->GetHelpText());
}

void WindowTestCase::Parent()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL), m_window->GetGrandParent());
    CPPUNIT_ASSERT_EQUAL(wxTheApp->GetTopWindow(), m_window->GetParent());
}

void WindowTestCase::Siblings()
{
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL), m_window->GetNextSibling());
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL), m_window->GetPrevSibling());

    wxWindow* newwin = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);

    CPPUNIT_ASSERT_EQUAL(newwin, m_window->GetNextSibling());
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL), m_window->GetPrevSibling());

    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL), newwin->GetNextSibling());
    CPPUNIT_ASSERT_EQUAL(m_window, newwin->GetPrevSibling());

    wxDELETE(newwin);
}

void WindowTestCase::Children()
{
    CPPUNIT_ASSERT_EQUAL(0, m_window->GetChildren().GetCount());

    wxWindow* child1 = new wxWindow(m_window, wxID_ANY);

    CPPUNIT_ASSERT_EQUAL(1, m_window->GetChildren().GetCount());

    m_window->RemoveChild(child1);

    CPPUNIT_ASSERT_EQUAL(0, m_window->GetChildren().GetCount());

    child1->SetId(wxID_HIGHEST + 1);
    child1->SetName("child1");

    m_window->AddChild(child1);

    CPPUNIT_ASSERT_EQUAL(1, m_window->GetChildren().GetCount());
    CPPUNIT_ASSERT_EQUAL(child1, m_window->FindWindow(wxID_HIGHEST + 1));
    CPPUNIT_ASSERT_EQUAL(child1, m_window->FindWindow("child1"));

    m_window->DestroyChildren();

    CPPUNIT_ASSERT_EQUAL(0, m_window->GetChildren().GetCount());
}

void WindowTestCase::Focus()
{
#ifndef __WXOSX__
    CPPUNIT_ASSERT(!m_window->HasFocus());

    if ( m_window->AcceptsFocus() )
    {
        m_window->SetFocus();
        CPPUNIT_ASSERT(m_window->HasFocus());
    }

    //Set the focus back to the main window
    wxTheApp->GetTopWindow()->SetFocus();

    if ( m_window->AcceptsFocusFromKeyboard() )
    {
        m_window->SetFocusFromKbd();
        CPPUNIT_ASSERT(m_window->HasFocus());
    }
#endif
}

void WindowTestCase::Positioning()
{
    //Some basic tests for consistency
    int x, y;
    m_window->GetPosition(&x, &y);

    CPPUNIT_ASSERT_EQUAL(x, m_window->GetPosition().x);
    CPPUNIT_ASSERT_EQUAL(y, m_window->GetPosition().y);
    CPPUNIT_ASSERT_EQUAL(m_window->GetPosition(),
                         m_window->GetRect().GetTopLeft());

    m_window->GetScreenPosition(&x, &y);
    CPPUNIT_ASSERT_EQUAL(x, m_window->GetScreenPosition().x);
    CPPUNIT_ASSERT_EQUAL(y, m_window->GetScreenPosition().y);
    CPPUNIT_ASSERT_EQUAL(m_window->GetScreenPosition(),
                         m_window->GetScreenRect().GetTopLeft());
}

void WindowTestCase::Show()
{
    CPPUNIT_ASSERT(m_window->IsShown());

    m_window->Hide();

    CPPUNIT_ASSERT(!m_window->IsShown());

    m_window->Show();

    CPPUNIT_ASSERT(m_window->IsShown());

    m_window->Show(false);

    CPPUNIT_ASSERT(!m_window->IsShown());

    m_window->ShowWithEffect(wxSHOW_EFFECT_BLEND);

    CPPUNIT_ASSERT(m_window->IsShown());

    m_window->HideWithEffect(wxSHOW_EFFECT_BLEND);

    CPPUNIT_ASSERT(!m_window->IsShown());
}

void WindowTestCase::Enable()
{
    CPPUNIT_ASSERT(m_window->IsEnabled());

    m_window->Disable();

    CPPUNIT_ASSERT(!m_window->IsEnabled());

    m_window->Enable();

    CPPUNIT_ASSERT(m_window->IsEnabled());

    m_window->Enable(false);

    CPPUNIT_ASSERT(!m_window->IsEnabled());
}

void WindowTestCase::FindWindowBy()
{
    m_window->SetId(wxID_HIGHEST + 1);
    m_window->SetName("name");
    m_window->SetLabel("label");

    CPPUNIT_ASSERT_EQUAL(m_window, wxWindow::FindWindowById(wxID_HIGHEST + 1));
    CPPUNIT_ASSERT_EQUAL(m_window, wxWindow::FindWindowByName("name"));
    CPPUNIT_ASSERT_EQUAL(m_window, wxWindow::FindWindowByLabel("label"));

    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL),
                         wxWindow::FindWindowById(wxID_HIGHEST + 3));
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL),
                         wxWindow::FindWindowByName("noname"));
    CPPUNIT_ASSERT_EQUAL(static_cast<wxWindow*>(NULL),
                         wxWindow::FindWindowByLabel("nolabel"));
}
