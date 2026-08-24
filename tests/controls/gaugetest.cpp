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

#include <memory>

class GaugeTestCase
{
public:
    GaugeTestCase();

protected:
    std::unique_ptr<wxGauge> m_gauge;

    wxDECLARE_NO_COPY_CLASS(GaugeTestCase);
};

GaugeTestCase::GaugeTestCase()
{
    m_gauge = make_unique<wxGauge>(wxTheApp->GetTopWindow(), wxID_ANY, 100);
}

TEST_CASE_METHOD(GaugeTestCase, "Gauge::Direction", "[gauge]")
{
    //We should default to a horizontal gauge
    CHECK(!m_gauge->IsVertical());

    m_gauge = make_unique<wxGauge>(wxTheApp->GetTopWindow(), wxID_ANY, 100,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxGA_VERTICAL);

    CHECK(m_gauge->IsVertical());

    m_gauge = make_unique<wxGauge>(wxTheApp->GetTopWindow(), wxID_ANY, 100,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxGA_HORIZONTAL);

    CHECK(!m_gauge->IsVertical());
}

TEST_CASE_METHOD(GaugeTestCase, "Gauge::Range", "[gauge]")
{
    CHECK(m_gauge->GetRange() == 100);

    m_gauge->SetRange(50);

    CHECK(m_gauge->GetRange() == 50);

    m_gauge->SetRange(0);

    CHECK(m_gauge->GetRange() == 0);
}

TEST_CASE_METHOD(GaugeTestCase, "Gauge::Value", "[gauge]")
{
    CHECK(m_gauge->GetValue() == 0);

    m_gauge->SetValue(50);

    CHECK(m_gauge->GetValue() == 50);

    m_gauge->SetValue(0);

    CHECK(m_gauge->GetValue() == 0);

    m_gauge->SetValue(100);

    CHECK(m_gauge->GetValue() == 100);
}

#endif //wxUSE_GAUGE
