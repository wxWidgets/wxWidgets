///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrldbltest.cpp
// Purpose:     wxSpinCtrlDouble unit test
// Author:      Steven Lamerton
// Created:     2010-07-22
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

class SpinCtrlDoubleTestCase : public CppUnit::TestCase
{
public:
    SpinCtrlDoubleTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( SpinCtrlDoubleTestCase );
        CPPUNIT_TEST( NoEventsInCtor );
        WXUISIM_TEST( Arrows );
        WXUISIM_TEST( Wrap );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Value );
        WXUISIM_TEST( Increment );
        CPPUNIT_TEST( Digits );
    CPPUNIT_TEST_SUITE_END();

    void NoEventsInCtor();
    void Arrows();
    void Wrap();
    void Range();
    void Value();
    void Increment();
    void Digits();

    wxSpinCtrlDouble* m_spin;

    wxDECLARE_NO_COPY_CLASS(SpinCtrlDoubleTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SpinCtrlDoubleTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SpinCtrlDoubleTestCase, "SpinCtrlDoubleTestCase" );

void SpinCtrlDoubleTestCase::setUp()
{
    m_spin = new wxSpinCtrlDouble(wxTheApp->GetTopWindow());
}

void SpinCtrlDoubleTestCase::tearDown()
{
    wxDELETE(m_spin);
}

void SpinCtrlDoubleTestCase::NoEventsInCtor()
{
    // Verify that creating the control does not generate any events. This is
    // unexpected and shouldn't happen.
    wxWindow* const parent = m_spin->GetParent();
    delete m_spin;
    m_spin = new wxSpinCtrlDouble;

    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRLDOUBLE);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    m_spin->Create(parent, wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0., 100., 17.);

    CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
}

void SpinCtrlDoubleTestCase::Arrows()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter updated(m_spin, wxEVT_SPINCTRLDOUBLE);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, updated.GetCount());
    CPPUNIT_ASSERT_EQUAL(1.0, m_spin->GetValue());
    updated.Clear();

    sim.Char(WXK_DOWN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, updated.GetCount());
    CPPUNIT_ASSERT_EQUAL(0.0, m_spin->GetValue());
#endif
}

void SpinCtrlDoubleTestCase::Wrap()
{
#if wxUSE_UIACTIONSIMULATOR
    wxDELETE(m_spin);
    m_spin = new wxSpinCtrlDouble(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                  wxDefaultPosition, wxDefaultSize,
                                  wxSP_ARROW_KEYS | wxSP_WRAP);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(100.0, m_spin->GetValue());

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(0.0, m_spin->GetValue());
#endif
}

void SpinCtrlDoubleTestCase::Range()
{
    CPPUNIT_ASSERT_EQUAL(0.0, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(100.0, m_spin->GetMax());

    // Test that the value is adjusted to be inside the new valid range but
    // that this doesn't result in any events (as this is not something done by
    // the user).
    {
        EventCounter updatedSpin(m_spin, wxEVT_SPINCTRLDOUBLE);
        EventCounter updatedText(m_spin, wxEVT_TEXT);

        m_spin->SetRange(1., 10.);
        CPPUNIT_ASSERT_EQUAL(1., m_spin->GetValue());

        CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
        CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
    }

    //Test negative ranges
    m_spin->SetRange(-10.0, 10.0);

    CPPUNIT_ASSERT_EQUAL(-10.0, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(10.0, m_spin->GetMax());

    //Test backwards ranges
    m_spin->SetRange(75.0, 50.0);

    CPPUNIT_ASSERT_EQUAL(75.0, m_spin->GetMin());
    CPPUNIT_ASSERT_EQUAL(50.0, m_spin->GetMax());
}

void SpinCtrlDoubleTestCase::Value()
{
    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRLDOUBLE);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    m_spin->SetDigits(2);
    m_spin->SetIncrement(0.1);

    CPPUNIT_ASSERT_EQUAL(0.0, m_spin->GetValue());

    m_spin->SetValue(50.0);
    CPPUNIT_ASSERT_EQUAL(50.0, m_spin->GetValue());

    m_spin->SetValue(49.1);
    CPPUNIT_ASSERT_EQUAL(49.1, m_spin->GetValue());

    // Calling SetValue() shouldn't have generated any events.
    CPPUNIT_ASSERT_EQUAL(0, updatedSpin.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, updatedText.GetCount());
}

void SpinCtrlDoubleTestCase::Increment()
{
#if wxUSE_UIACTIONSIMULATOR
    CPPUNIT_ASSERT_EQUAL(1.0, m_spin->GetIncrement());

    m_spin->SetDigits(1);
    m_spin->SetIncrement(0.1);

    CPPUNIT_ASSERT_EQUAL(0.1, m_spin->GetIncrement());

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(0.1, m_spin->GetValue());
#endif
}

void SpinCtrlDoubleTestCase::Digits()
{
    m_spin->SetDigits(5);

    CPPUNIT_ASSERT_EQUAL(5, m_spin->GetDigits());
}

static inline unsigned int GetInitialDigits(double inc)
{
    wxSpinCtrlDouble* sc = new wxSpinCtrlDouble(wxTheApp->GetTopWindow(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,
        0, 50, 0, inc);
    unsigned int digits = sc->GetDigits();
    delete sc;
    return digits;
}

TEST_CASE("SpinCtrlDoubleTestCase::InitialDigits", "[spinctrldouble][initialdigits]")
{
    REQUIRE(GetInitialDigits(15) == 0);
    REQUIRE(GetInitialDigits(10) == 0);
    REQUIRE(GetInitialDigits(1) == 0);
    REQUIRE(GetInitialDigits(0.999) == 1);
    REQUIRE(GetInitialDigits(0.15) == 1);
    REQUIRE(GetInitialDigits(0.11) == 1);
    REQUIRE(GetInitialDigits(0.1) == 1);
    REQUIRE(GetInitialDigits(0.0999) == 2);
    REQUIRE(GetInitialDigits(0.015) == 2);
    REQUIRE(GetInitialDigits(0.011) == 2);
    REQUIRE(GetInitialDigits(0.01) == 2);
    REQUIRE(GetInitialDigits(9.99e-5) == 5);
    REQUIRE(GetInitialDigits(1e-5) == 5);
    REQUIRE(GetInitialDigits(9.9999e-10) == 10);
    REQUIRE(GetInitialDigits(1e-10) == 10);
    REQUIRE(GetInitialDigits(9.9999e-20) == 20);
    REQUIRE(GetInitialDigits(1e-20) == 20);
    REQUIRE(GetInitialDigits(9.9999e-21) == 20);
    REQUIRE(GetInitialDigits(1e-21) == 20);
    REQUIRE(GetInitialDigits(9.9999e-22) == 20);
    REQUIRE(GetInitialDigits(1e-22) == 20);
}

#endif
