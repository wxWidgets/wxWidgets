///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrltest.cpp
// Purpose:     wxSpinCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-21
// RCS-ID:      $Id$
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

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( SpinCtrlTestCase );
        WXUISIM_TEST( Arrows );
        WXUISIM_TEST( Wrap );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Value );
    CPPUNIT_TEST_SUITE_END();

    void Arrows();
    void Wrap();
    void Range();
    void Value();

    wxSpinCtrl* m_spin;

    DECLARE_NO_COPY_CLASS(SpinCtrlTestCase)
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

void SpinCtrlTestCase::Arrows()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_spin, wxEVT_COMMAND_SPINCTRL_UPDATED);

    wxUIActionSimulator sim;

    m_spin->SetFocus();

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
    CPPUNIT_ASSERT_EQUAL(1, m_spin->GetValue());

    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
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

    //Test neagtive ranges
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
    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());

    m_spin->SetValue(50);

    CPPUNIT_ASSERT_EQUAL(50, m_spin->GetValue());

    m_spin->SetValue(-10);

    CPPUNIT_ASSERT_EQUAL(0, m_spin->GetValue());

    m_spin->SetValue(110);

    CPPUNIT_ASSERT_EQUAL(100, m_spin->GetValue());
}

#endif
