///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/radiobuttontest.cpp
// Purpose:     wxRadioButton unit test
// Author:      Steven Lamerton
// Created:     2010-07-30
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_RADIOBTN

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/radiobut.h"
#endif // WX_PRECOMP

#include "wx/uiaction.h"
#include "testableframe.h"

class RadioButtonTestCase : public CppUnit::TestCase
{
public:
    RadioButtonTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( RadioButtonTestCase );
        WXUISIM_TEST( Click );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Group );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Value();
    void Group();

    wxRadioButton* m_radio;

    DECLARE_NO_COPY_CLASS(RadioButtonTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RadioButtonTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RadioButtonTestCase,
                                      "RadioButtonTestCase" );

void RadioButtonTestCase::setUp()
{
    m_radio = new wxRadioButton(wxTheApp->GetTopWindow(), wxID_ANY,
                                "wxRadioButton");
    m_radio->Update();
    m_radio->Refresh();
}

void RadioButtonTestCase::tearDown()
{
    wxDELETE(m_radio);
}

void RadioButtonTestCase::Click()
{
    // GTK does not support selecting a single radio button
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_radio, wxEVT_COMMAND_RADIOBUTTON_SELECTED);

    wxUIActionSimulator sim;

    sim.MouseMove(m_radio->GetScreenPosition() + wxPoint(10, 10));
    sim.MouseClick();

    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, frame->GetEventCount() );
#endif
}

void RadioButtonTestCase::Value()
{
#ifndef __WXGTK__
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_radio, wxEVT_COMMAND_RADIOBUTTON_SELECTED);

    m_radio->SetValue(true);

    CPPUNIT_ASSERT(m_radio->GetValue());

    m_radio->SetValue(false);

    CPPUNIT_ASSERT(!m_radio->GetValue());

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount());
#endif
}

void RadioButtonTestCase::Group()
{
    //Add another button to the first group and create another of two buttons
    wxRadioButton* g1radio0 = new wxRadioButton(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "wxRadioButton",
                                                wxDefaultPosition,
                                                wxDefaultSize, wxRB_GROUP);

    wxRadioButton* g1radio1 = new wxRadioButton(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "wxRadioButton");

    wxRadioButton* g2radio0 = new wxRadioButton(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "wxRadioButton",
                                                wxDefaultPosition,
                                                wxDefaultSize, wxRB_GROUP);

    wxRadioButton* g2radio1 = new wxRadioButton(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "wxRadioButton");

    g1radio0->SetValue(true);
    g2radio0->SetValue(true);

    CPPUNIT_ASSERT(g1radio0->GetValue());
    CPPUNIT_ASSERT(!g1radio1->GetValue());
    CPPUNIT_ASSERT(g2radio0->GetValue());
    CPPUNIT_ASSERT(!g2radio1->GetValue());

    g1radio1->SetValue(true);
    g2radio1->SetValue(true);

    CPPUNIT_ASSERT(!g1radio0->GetValue());
    CPPUNIT_ASSERT(g1radio1->GetValue());
    CPPUNIT_ASSERT(!g2radio0->GetValue());
    CPPUNIT_ASSERT(g2radio1->GetValue());

    g1radio0->SetValue(true);
    g2radio0->SetValue(true);

    CPPUNIT_ASSERT(g1radio0->GetValue());
    CPPUNIT_ASSERT(!g1radio1->GetValue());
    CPPUNIT_ASSERT(g2radio0->GetValue());
    CPPUNIT_ASSERT(!g2radio1->GetValue());

    wxDELETE(g1radio0);
    wxDELETE(g1radio1);
    wxDELETE(g2radio0);
    wxDELETE(g2radio1);
}

#endif //wxUSE_RADIOBTN
