///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrltest.cpp
// Purpose:     wxSpinCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-21
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SPINCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/spinctrl.h"

class SpinCtrlTestCase : public CppUnit::TestCase
{
public:
    SpinCtrlTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( SpinCtrlTestCase );
        CPPUNIT_TEST( Initial );
        CPPUNIT_TEST( NoEventsInCtor );
        WXUISIM_TEST( Arrows );
        WXUISIM_TEST( Wrap );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Value );
        WXUISIM_TEST( SetValueInsideEventHandler );
    CPPUNIT_TEST_SUITE_END();

    void Initial();
    void NoEventsInCtor();
    void Arrows();
    void Wrap();
    void Range();
    void Value();
    void SetValueInsideEventHandler();

    // Helper event handler for SetValueInsideEventHandler() test.
    void OnSpinSetValue(wxSpinEvent &e);

    wxSpinCtrl* m_spin;

    wxDECLARE_NO_COPY_CLASS(SpinCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SpinCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SpinCtrlTestCase, "SpinCtrlTestCase" );

void SpinCtrlTestCase::setUp()
{
    m_spin = new wxSpinCtrl(wxTheApp->GetTopWindow());
}

void SpinCtrlTestCase::tearDown()
{
    wxDELETE(m_spin);
}

void SpinCtrlTestCase::Initial()
{
    // Initial value is defined by "initial" argument which is 0 by default.
    CPPUNIT_ASSERT_EQUAL( 0, m_spin->GetValue() );

    wxWindow* const parent = m_spin->GetParent();

    // Recreate the control with another "initial" to check this.
    delete m_spin;
    m_spin = new wxSpinCtrl(parent, wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize, 0,
                            0, 100, 17);
    CPPUNIT_ASSERT_EQUAL( 17, m_spin->GetValue() );

    // Recreate the control with another "initial" outside of standard spin
    // ctrl range.
    delete m_spin;
    m_spin = new wxSpinCtrl(parent, wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize, 0,
                            0, 200, 150);
    CPPUNIT_ASSERT_EQUAL( 150, m_spin->GetValue() );

    // But if the text string is specified, it takes precedence.
    delete m_spin;
    m_spin = new wxSpinCtrl(parent, wxID_ANY, "99",
                            wxDefaultPosition, wxDefaultSize, 0,
                            0, 100, 17);
    CPPUNIT_ASSERT_EQUAL( 99, m_spin->GetValue() );
}

void SpinCtrlTestCase::NoEventsInCtor()
{
    // Verify that creating the control does not generate any events. This is
    // unexpected and shouldn't happen.
    wxWindow* const parent = m_spin->GetParent();
    delete m_spin;
    m_spin = new wxSpinCtrl;

    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    m_spin->Create(parent, wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0, 100, 17);

    CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
}

void SpinCtrlTestCase::Arrows()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter updated(m_spin, wxEVT_SPINCTRL);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, updated.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, m_spin->GetValue());
    updated.Clear();

    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, updated.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());
#endif
}

void SpinCtrlTestCase::Wrap()
{
#if wxUSE_UIACTIONSIMULATOR
    wxDELETE(m_spin);
    m_spin = new wxSpinCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxSP_ARROW_KEYS | wxSP_WRAP);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(100, m_spin->GetValue());

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());
#endif
}

void SpinCtrlTestCase::Range()
{
    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(100, m_spin->GetMax());

    // Test that the value is adjusted to be inside the new valid range but
    // that this doesn't result in any events (as this is not something done by
    // the user).
    {
        EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
        EventCounter updatedText(m_spin, wxEVT_TEXT);

        m_spin->SetRange(1, 10);
        CPPUNIT_ASSERT_EQUAL(1, m_spin->GetValue());

        CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
        CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
    }

    //Test negative ranges
    m_spin->SetRange(-10, 10);

    CPPUNIT_ASSERT_EQUAL(-10, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(10, m_spin->GetMax());

    //Test backwards ranges
    m_spin->SetRange(75, 50);

    CPPUNIT_ASSERT_EQUAL(75, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(50, m_spin->GetMax());
}

void SpinCtrlTestCase::Value()
{
    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());

    m_spin->SetValue(50);
    CPPUNIT_ASSERT_EQUAL(50, m_spin->GetValue());

    m_spin->SetValue(-10);
    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());

    m_spin->SetValue(110);
    CPPUNIT_ASSERT_EQUAL(100, m_spin->GetValue());

    // Calling SetValue() shouldn't have generated any events.
    CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
}

void SpinCtrlTestCase::OnSpinSetValue(wxSpinEvent &e)
{
    // Constrain the value to be in the 1..16 range or 32.
    int newVal = e.GetValue();

    if ( newVal == 31 )
        m_spin->SetValue(16);
    else if ( newVal > 16 )
        m_spin->SetValue(32);
}

void SpinCtrlTestCase::SetValueInsideEventHandler()
{
#if wxUSE_UIACTIONSIMULATOR
    m_spin->Bind(wxEVT_SPINCTRL, &SpinCtrlTestCase::OnSpinSetValue, this);

    wxUIActionSimulator sim;

    // run multiple times to make sure there are no issues with keeping old value
    for ( size_t i = 0; i < 2; i++ )
    {
        m_spin->SetFocus();
        wxYield();

        sim.Char(WXK_DELETE);
        sim.Char(WXK_DELETE);
        sim.Text("20");
        wxYield();

        wxTheApp->GetTopWindow()->SetFocus();
        wxYield();

        CPPUNIT_ASSERT_EQUAL(32, m_spin->GetValue());
    }
#endif // wxUSE_UIACTIONSIMULATOR
}

#endif
