///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/buttontest.cpp
// Purpose:     wxButton unit test
// Author:      Steven Lamerton
// Created:     2010-06-21
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/button.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"

class ButtonTestCase : public CppUnit::TestCase
{
public:
    ButtonTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( ButtonTestCase );
        CPPUNIT_TEST( Click );
        CPPUNIT_TEST( Disabled );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Disabled();

    wxButton* m_button;

    DECLARE_NO_COPY_CLASS(ButtonTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ButtonTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ButtonTestCase, "ButtonTestCase" );

void ButtonTestCase::setUp()
{
    //We use wxTheApp->GetTopWindow() as there is only a single testable frame
    //so it will always be returned
    m_button = new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "wxButton");
}

void ButtonTestCase::Click()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    //We use CountWindowEvents to reduce the typing need for Connect
    EventCounter count(m_button, wxEVT_COMMAND_BUTTON_CLICKED);

    wxUIActionSimulator sim;

    //We move in slightly to account for window decorations
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    sim.MouseClick();

    //We have to yield to let event processing take place
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, frame->GetEventCount() );
}

void ButtonTestCase::Disabled()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_button, wxEVT_COMMAND_BUTTON_CLICKED);

    wxUIActionSimulator sim;

    //In this test we disable the button and check events are not sent
    m_button->Disable();

    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    sim.MouseClick();

    wxYield();

    CPPUNIT_ASSERT_EQUAL( 0, frame->GetEventCount() );

    //Always remember to return controls to their default state
    //for following tests
    m_button->Enable();
}

void ButtonTestCase::tearDown()
{
    delete m_button;
}
