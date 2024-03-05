///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/togglebuttontest.cpp
// Purpose:     wxToggleButton unit test
// Author:      Steven Lamerton
// Created:     2010-07-14
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TOGGLEBTN


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/tglbtn.h"

class ToggleButtonTestCase : public CppUnit::TestCase
{
public:
    ToggleButtonTestCase() { }

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( ToggleButtonTestCase );
        WXUISIM_TEST( Click );
        CPPUNIT_TEST( Value );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Value();

    wxToggleButton* m_button;

    wxDECLARE_NO_COPY_CLASS(ToggleButtonTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ToggleButtonTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ToggleButtonTestCase, "ToggleButtonTestCase" );

void ToggleButtonTestCase::setUp()
{
    m_button = new wxToggleButton(wxTheApp->GetTopWindow(), wxID_ANY, "wxToggleButton");
}

void ToggleButtonTestCase::tearDown()
{
    wxDELETE(m_button);
}

void ToggleButtonTestCase::Click()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter clicked(m_button, wxEVT_TOGGLEBUTTON);

    wxUIActionSimulator sim;

    //We move in slightly to account for window decorations
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
    CPPUNIT_ASSERT(m_button->GetValue());
    clicked.Clear();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
    CPPUNIT_ASSERT(!m_button->GetValue());
#endif
}

void ToggleButtonTestCase::Value()
{
    EventCounter clicked(m_button, wxEVT_BUTTON);

    m_button->SetValue(true);

    CPPUNIT_ASSERT(m_button->GetValue());

    m_button->SetValue(false);

    CPPUNIT_ASSERT(!m_button->GetValue());

    CPPUNIT_ASSERT_EQUAL( 0, clicked.GetCount() );
}

#endif //wxUSE_TOGGLEBTN
