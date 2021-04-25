///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrldbltest.cpp
// Purpose:     wxSpinCtrlDouble unit test
// Author:      Steven Lamerton
// Created:     2010-07-22
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SPINCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/scopedptr.h"
#include "wx/spinctrl.h"

class SpinCtrlDoubleTestCase
{
public:
    SpinCtrlDoubleTestCase(int style = wxSP_ARROW_KEYS)
        : m_spin(new wxSpinCtrlDouble(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                      wxDefaultPosition, wxDefaultSize,
                                      style))
    {
    }

    ~SpinCtrlDoubleTestCase()
    {
        delete m_spin;
    }

protected:
    wxSpinCtrlDouble* const m_spin;

    wxDECLARE_NO_COPY_CLASS(SpinCtrlDoubleTestCase);
};

class SpinCtrlDoubleTestCaseWrap : public SpinCtrlDoubleTestCase
{
public:
    SpinCtrlDoubleTestCaseWrap()
        : SpinCtrlDoubleTestCase(wxSP_ARROW_KEYS | wxSP_WRAP)
    {
    }
};


TEST_CASE("SpinCtrlDouble::NoEventsInCtor", "[spinctrl][spinctrldouble]")
{
    // Verify that creating the control does not generate any events. This is
    // unexpected and shouldn't happen.
    wxScopedPtr<wxSpinCtrlDouble> m_spin(new wxSpinCtrlDouble);

    EventCounter updatedSpin(m_spin.get(), wxEVT_SPINCTRLDOUBLE);
    EventCounter updatedText(m_spin.get(), wxEVT_TEXT);

    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0., 100., 17.);

    CHECK( updatedSpin.GetCount() == 0 );
    CHECK( updatedText.GetCount() == 0 );
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SpinCtrlDoubleTestCase,
                 "SpinCtrlDouble::Arrows", "[spinctrl][spinctrldouble]")
{
    EventCounter updated(m_spin, wxEVT_SPINCTRLDOUBLE);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    wxYield();

    CHECK( updated.GetCount() == 1 );
    CHECK( m_spin->GetValue() == 1.0 );
    updated.Clear();

    sim.Char(WXK_DOWN);
    wxYield();

    CHECK( updated.GetCount() == 1 );
    CHECK( m_spin->GetValue() == 0.0 );
}

TEST_CASE_METHOD(SpinCtrlDoubleTestCaseWrap,
                 "SpinCtrlDouble::Wrap", "[spinctrl][spinctrldouble]")
{
    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_DOWN);

    wxYield();

    CHECK( m_spin->GetValue() == 100.0 );

    sim.Char(WXK_UP);

    wxYield();

    CHECK( m_spin->GetValue() == 0.0 );
}
#endif // wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SpinCtrlDoubleTestCase,
                 "SpinCtrlDouble::Range", "[spinctrl][spinctrldouble]")
{
    CHECK( m_spin->GetMin() == 0.0 );
    CHECK( m_spin->GetMax() == 100.0 );

    // Test that the value is adjusted to be inside the new valid range but
    // that this doesn't result in any events (as this is not something done by
    // the user).
    {
        EventCounter updatedSpin(m_spin, wxEVT_SPINCTRLDOUBLE);
        EventCounter updatedText(m_spin, wxEVT_TEXT);

        m_spin->SetRange(1., 10.);
        CHECK( m_spin->GetValue() == 1. );

        CHECK( updatedSpin.GetCount() == 0 );
        CHECK( updatedText.GetCount() == 0 );
    }

    //Test negative ranges
    m_spin->SetRange(-10.0, 10.0);

    CHECK( m_spin->GetMin() == -10.0 );
    CHECK( m_spin->GetMax() == 10.0 );

    //Test backwards ranges
    m_spin->SetRange(75.0, 50.0);

    CHECK( m_spin->GetMin() == 75.0 );
    CHECK( m_spin->GetMax() == 50.0 );
}

TEST_CASE_METHOD(SpinCtrlDoubleTestCase,
                 "SpinCtrlDouble::Value", "[spinctrl][spinctrldouble]")
{
    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRLDOUBLE);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    m_spin->SetDigits(2);
    m_spin->SetIncrement(0.1);

    CHECK( m_spin->GetValue() == 0.0 );

    m_spin->SetValue(50.0);
    CHECK( m_spin->GetValue() == 50.0 );

    m_spin->SetValue(49.1);
    CHECK( m_spin->GetValue() == 49.1 );

    // Calling SetValue() shouldn't have generated any events.
    CHECK( updatedSpin.GetCount() == 0 );
    CHECK( updatedText.GetCount() == 0 );

    // Also test that setting the text value works.
    CHECK( m_spin->GetTextValue() == "49.10" );

    m_spin->SetValue("57.30");
    CHECK( m_spin->GetTextValue() == "57.30" );
    CHECK( m_spin->GetValue() == 57.3 );

    CHECK( updatedSpin.GetCount() == 0 );
    CHECK( updatedText.GetCount() == 0 );

    m_spin->SetValue("");
    CHECK( m_spin->GetTextValue() == "" );
    CHECK( m_spin->GetValue() == 0 );

    CHECK( updatedSpin.GetCount() == 0 );
    CHECK( updatedText.GetCount() == 0 );
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SpinCtrlDoubleTestCase,
                 "SpinCtrlDouble::Increment", "[spinctrl][spinctrldouble]")
{
    CHECK( m_spin->GetIncrement() == 1.0 );

    m_spin->SetDigits(1);
    m_spin->SetIncrement(0.1);

    CHECK( m_spin->GetIncrement() == 0.1 );

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);

    wxYield();

    CHECK( m_spin->GetValue() == 0.1 );
}

#endif // wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SpinCtrlDoubleTestCase,
                 "SpinCtrlDouble::Digits", "[spinctrl][spinctrldouble]")
{
    // Setting increment should adjust the number of digits shown to be big
    // enough to show numbers with the corresponding granularity.
    m_spin->SetIncrement(0.1);
    m_spin->SetValue(1.23456789);
    CHECK( m_spin->GetTextValue() == "1.2" );

    m_spin->SetIncrement(0.01);
    m_spin->SetValue(1.23456789);
    CHECK( m_spin->GetTextValue() == "1.23" );

    m_spin->SetDigits(5);
    CHECK( m_spin->GetDigits() == 5 );
    m_spin->SetValue(1.23456789);
    CHECK( m_spin->GetTextValue() == "1.23457" );

    // The number of digits shouldn't (implicitly) decrease however.
    m_spin->SetIncrement(0.001);
    m_spin->SetValue(1.23456789);
    CHECK( m_spin->GetTextValue() == "1.23457" );

    // Check that using increment greater than 1 also works.
    m_spin->SetDigits(0);
    m_spin->SetIncrement(2.5);
    m_spin->SetValue(7.5);
    CHECK( m_spin->GetTextValue() == "7.5" );
}

static inline unsigned int GetInitialDigits(double inc)
{
    wxScopedPtr<wxSpinCtrlDouble> sc(new wxSpinCtrlDouble
        (
            wxTheApp->GetTopWindow(),
            wxID_ANY,
            wxEmptyString,
            wxDefaultPosition,
            wxDefaultSize,
            wxSP_ARROW_KEYS,
            0, 50, 0,
            inc
        ));
    return sc->GetDigits();
}

TEST_CASE("SpinCtrlDouble::InitialDigits", "[spinctrldouble][initialdigits]")
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
