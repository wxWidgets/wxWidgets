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

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/caret.h"
#include "wx/cshelp.h"

class WindowTestCase : public CppUnit::TestCase
{
public:
    WindowTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( WindowTestCase );
        CPPUNIT_TEST( ShowHideEvent );
        CPPUNIT_TEST( KeyEvent );
        CPPUNIT_TEST( FocusEvent );
        CPPUNIT_TEST( Mouse );
        CPPUNIT_TEST( Properties );
        CPPUNIT_TEST( ToolTip );
        CPPUNIT_TEST( Help );
        CPPUNIT_TEST( Parent );
        CPPUNIT_TEST( Siblings );
    CPPUNIT_TEST_SUITE_END();

    void ShowHideEvent();
    void KeyEvent();
    void FocusEvent();
    void Mouse();
    void Properties();
    void ToolTip();
    void Help();
    void Parent();
    void Siblings();

    wxWindow *m_window;

    DECLARE_NO_COPY_CLASS(WindowTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WindowTestCase );

// also include in it's own registry so that these tests can be run alone
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
#if defined(__WXMSW__) || defined(__WXGTK__) || defined (__WXPM__)
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_window, wxEVT_SHOW);

    m_window->Show();

    CPPUNIT_ASSERT(m_window->IsShown());

    m_window->Show(false);

    CPPUNIT_ASSERT(!m_window->IsShown());

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount());
#endif
}

void WindowTestCase::KeyEvent()
{
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
}

void WindowTestCase::FocusEvent()
{
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
}

void WindowTestCase::Mouse()
{
    //No cursor by default
    CPPUNIT_ASSERT(!m_window->GetCursor().IsOk());
    
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
