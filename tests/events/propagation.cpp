///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/propagation.cpp
// Purpose:     Test events propagation
// Author:      Vadim Zeitlin
// Created:     2009-01-16
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/event.h"
#include "wx/window.h"
#include "wx/scopeguard.h"

namespace
{

// this string will record the execution of all handlers
wxString g_str;

// a custom event
wxDEFINE_EVENT(TEST_EVT, wxCommandEvent);

// a custom event handler
class TestEvtHandler : public wxEvtHandler
{
public:
    TestEvtHandler(char tag)
        : m_tag(tag)
    {
        Connect(TEST_EVT, wxCommandEventHandler(TestEvtHandler::OnTest));
    }

    // override ProcessEvent() to confirm that it is called for all event
    // handlers in the chain
    virtual bool ProcessEvent(wxEvent& event)
    {
        if ( event.GetEventType() == TEST_EVT )
            g_str += 'o'; // "o" == "overridden"

        return wxEvtHandler::ProcessEvent(event);
    }

private:
    void OnTest(wxCommandEvent& event)
    {
        g_str += m_tag;

        event.Skip();
    }

    const char m_tag;

    DECLARE_NO_COPY_CLASS(TestEvtHandler)
};

// a window handling the test event
class TestWindow : public wxWindow
{
public:
    TestWindow(wxWindow *parent, char tag)
        : wxWindow(parent, wxID_ANY),
          m_tag(tag)
    {
        Connect(TEST_EVT, wxCommandEventHandler(TestWindow::OnTest));
    }

private:
    void OnTest(wxCommandEvent& event)
    {
        g_str += m_tag;

        event.Skip();
    }

    const char m_tag;

    DECLARE_NO_COPY_CLASS(TestWindow)
};

int DoFilterEvent(wxEvent& event)
{
    if ( event.GetEventType() == TEST_EVT )
        g_str += 'a';

    return -1;
}

bool DoProcessEvent(wxEvent& event)
{
    if ( event.GetEventType() == TEST_EVT )
        g_str += 'A';

    return false;
}

} // anonymous namespace

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class EventPropagationTestCase : public CppUnit::TestCase
{
public:
    EventPropagationTestCase() {}

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( EventPropagationTestCase );
        CPPUNIT_TEST( OneHandler );
        CPPUNIT_TEST( TwoHandlers );
        CPPUNIT_TEST( WindowWithoutHandler );
        CPPUNIT_TEST( WindowWithHandler );
    CPPUNIT_TEST_SUITE_END();

    void OneHandler();
    void TwoHandlers();
    void WindowWithoutHandler();
    void WindowWithHandler();

    DECLARE_NO_COPY_CLASS(EventPropagationTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EventPropagationTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EventPropagationTestCase, "EventPropagationTestCase" );

void EventPropagationTestCase::setUp()
{
    SetFilterEventFunc(DoFilterEvent);
    SetProcessEventFunc(DoProcessEvent);

    g_str.clear();
}

void EventPropagationTestCase::tearDown()
{
    SetFilterEventFunc(NULL);
    SetProcessEventFunc(NULL);
}

void EventPropagationTestCase::OneHandler()
{
    wxCommandEvent event(TEST_EVT);
    TestEvtHandler h1('1');
    h1.ProcessEvent(event);
    CPPUNIT_ASSERT_EQUAL( "oa1A", g_str );
}

void EventPropagationTestCase::TwoHandlers()
{
    wxCommandEvent event(TEST_EVT);
    TestEvtHandler h1('1');
    TestEvtHandler h2('2');
    h1.SetNextHandler(&h2);
    h2.SetPreviousHandler(&h1);
    h1.ProcessEvent(event);
    CPPUNIT_ASSERT_EQUAL( "oa1o2A", g_str );
}

void EventPropagationTestCase::WindowWithoutHandler()
{
    wxCommandEvent event(TEST_EVT);
    TestWindow * const parent = new TestWindow(wxTheApp->GetTopWindow(), 'p');
    wxON_BLOCK_EXIT_OBJ0( *parent, wxWindow::Destroy );

    TestWindow * const child = new TestWindow(parent, 'c');

    child->ProcessEvent(event);
    CPPUNIT_ASSERT_EQUAL( "acpA", g_str );
}

void EventPropagationTestCase::WindowWithHandler()
{
    wxCommandEvent event(TEST_EVT);
    TestWindow * const parent = new TestWindow(wxTheApp->GetTopWindow(), 'p');
    wxON_BLOCK_EXIT_OBJ0( *parent, wxWindow::Destroy );

    TestWindow * const child = new TestWindow(parent, 'c');

    TestEvtHandler h1('1');
    child->PushEventHandler(&h1);
    wxON_BLOCK_EXIT_OBJ0( *child, wxWindow::PopEventHandler );
    TestEvtHandler h2('2');
    child->PushEventHandler(&h2);
    wxON_BLOCK_EXIT_OBJ0( *child, wxWindow::PopEventHandler );

    child->HandleWindowEvent(event);
    CPPUNIT_ASSERT_EQUAL( "oa2o1cpA", g_str );
}

