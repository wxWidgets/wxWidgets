///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrldbltest.cpp
// Purpose:     wxSpinCtrlDouble unit test
// Author:      Steven Lamerton
// Created:     2010-07-22
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

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

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( SpinCtrlDoubleTestCase );
        WXUISIM_TEST( Arrows );
        WXUISIM_TEST( Wrap );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Value );
        WXUISIM_TEST( Increment );
        CPPUNIT_TEST( Digits );
    CPPUNIT_TEST_SUITE_END();

    void Arrows();
    void Wrap();
    void Range();
    void Value();
    void Increment();
    void Digits();

    wxSpinCtrlDouble* m_spin;

    DECLARE_NO_COPY_CLASS(SpinCtrlDoubleTestCase)
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

void SpinCtrlDoubleTestCase::Arrows()
{
#ifndef __WXGTK__
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_spin, wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
    CPPUNIT_ASSERT_EQUAL(1.0, m_spin->GetValue());

    sim.Char(WXK_DOWN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
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

    //Test neagtive ranges
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
    m_spin->SetDigits(2);
    m_spin->SetIncrement(0.1);

    CPPUNIT_ASSERT_EQUAL(0.0, m_spin->GetValue());

    m_spin->SetValue(50.0);

    CPPUNIT_ASSERT_EQUAL(50.0, m_spin->GetValue());

    m_spin->SetValue(49.1);

    CPPUNIT_ASSERT_EQUAL(49.1, m_spin->GetValue());
}

void SpinCtrlDoubleTestCase::Increment()
{
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXGTK__)
    CPPUNIT_ASSERT_EQUAL(1.0, m_spin->GetIncrement());

    m_spin->SetIncrement(0.1);

    CPPUNIT_ASSERT_EQUAL(0.1, m_spin->GetIncrement());

    wxUIActionSimulator sim;

    m_spin->SetFocus();

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
