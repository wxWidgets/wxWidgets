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

class WindowTestCase : public CppUnit::TestCase
{
public:
    WindowTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( WindowTestCase );
        CPPUNIT_TEST( ShowHide );
        CPPUNIT_TEST( Key );
        CPPUNIT_TEST( Focus );
    CPPUNIT_TEST_SUITE_END();

    void ShowHide();
    void Key();
    void Focus();

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
    wxDELETE(m_window);
}

void WindowTestCase::ShowHide()
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

void WindowTestCase::Key()
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

void WindowTestCase::Focus()
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
