///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/gaugetest.cpp
// Purpose:     wxGauge unit test
// Author:      Steven Lamerton
// Created:     2010-07-15
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_GAUGE


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/gauge.h"
#endif // WX_PRECOMP

class GaugeTestCase : public CppUnit::TestCase
{
public:
    GaugeTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( GaugeTestCase );
        CPPUNIT_TEST( Direction );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Value );
    CPPUNIT_TEST_SUITE_END();

    void Direction();
    void Range();
    void Value();

    wxGauge* m_gauge;

    wxDECLARE_NO_COPY_CLASS(GaugeTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GaugeTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GaugeTestCase, "GaugeTestCase" );

void GaugeTestCase::setUp()
{
    m_gauge = new wxGauge(wxTheApp->GetTopWindow(), wxID_ANY, 100);
}

void GaugeTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void GaugeTestCase::Direction()
{
    //We should default to a horizontal gauge
    CPPUNIT_ASSERT(!m_gauge->IsVertical());

    wxDELETE(m_gauge);
    m_gauge = new wxGauge(wxTheApp->GetTopWindow(), wxID_ANY, 100,
                          wxDefaultPosition, wxDefaultSize, wxGA_VERTICAL);

    CPPUNIT_ASSERT(m_gauge->IsVertical());

    wxDELETE(m_gauge);
    m_gauge = new wxGauge(wxTheApp->GetTopWindow(), wxID_ANY, 100,
                          wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);

    CPPUNIT_ASSERT(!m_gauge->IsVertical());
}

void GaugeTestCase::Range()
{
    CPPUNIT_ASSERT_EQUAL(100, m_gauge->GetRange());

    m_gauge->SetRange(50);

    CPPUNIT_ASSERT_EQUAL(50, m_gauge->GetRange());

    m_gauge->SetRange(0);

    CPPUNIT_ASSERT_EQUAL(0, m_gauge->GetRange());
}

void GaugeTestCase::Value()
{
    CPPUNIT_ASSERT_EQUAL(0, m_gauge->GetValue());

    m_gauge->SetValue(50);

    CPPUNIT_ASSERT_EQUAL(50, m_gauge->GetValue());

    m_gauge->SetValue(0);

    CPPUNIT_ASSERT_EQUAL(0, m_gauge->GetValue());

    m_gauge->SetValue(100);

    CPPUNIT_ASSERT_EQUAL(100, m_gauge->GetValue());
}

#endif //wxUSE_GAUGE
