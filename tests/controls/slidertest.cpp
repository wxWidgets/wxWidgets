///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/slidertest.cpp
// Purpose:     wxSlider unit test
// Author:      Steven Lamerton
// Created:     2010-07-20
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SLIDER

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/slider.h"
#endif // WX_PRECOMP

#include "wx/uiaction.h"
#include "testableframe.h"

class SliderTestCase : public CppUnit::TestCase
{
public:
    SliderTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( SliderTestCase );
        CPPUNIT_TEST( PageUpDown );
        CPPUNIT_TEST( LineUpDown );
        CPPUNIT_TEST( LinePageSize );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Thumb );
    CPPUNIT_TEST_SUITE_END();

    void PageUpDown();
    void LineUpDown();
    void LinePageSize();
    void Value();
    void Range();
    void Thumb();

    wxSlider* m_slider;

    DECLARE_NO_COPY_CLASS(SliderTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SliderTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SliderTestCase, "SliderTestCase" );

void SliderTestCase::setUp()
{
    m_slider = new wxSlider(wxTheApp->GetTopWindow(), wxID_ANY, 50, 0, 100);
}

void SliderTestCase::tearDown()
{
    wxDELETE(m_slider);
}

void SliderTestCase::PageUpDown()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_slider, wxEVT_SCROLL_PAGEUP);
    EventCounter count1(m_slider, wxEVT_SCROLL_PAGEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();

    sim.Char(WXK_PAGEUP);
    sim.Char(WXK_PAGEDOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_PAGEUP));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_PAGEDOWN));
}

void SliderTestCase::LineUpDown()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_slider, wxEVT_SCROLL_LINEUP);
    EventCounter count1(m_slider, wxEVT_SCROLL_LINEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();

    sim.Char(WXK_UP);
    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_LINEUP));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_LINEDOWN));
}

void SliderTestCase::LinePageSize()
{
    wxUIActionSimulator sim;
    m_slider->SetFocus();

    m_slider->SetPageSize(20);

    sim.Char(WXK_PAGEUP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(20, m_slider->GetPageSize());
    CPPUNIT_ASSERT_EQUAL(30, m_slider->GetValue());

    m_slider->SetLineSize(2);

    sim.Char(WXK_UP);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, m_slider->GetLineSize());
    CPPUNIT_ASSERT_EQUAL(28, m_slider->GetValue());
}

void SliderTestCase::Value()
{
    m_slider->SetValue(30);

    CPPUNIT_ASSERT_EQUAL(30, m_slider->GetValue());

    //When setting a value larger that max or smaller than min
    //max and min are set
    m_slider->SetValue(-1);

    CPPUNIT_ASSERT_EQUAL(0, m_slider->GetValue());

    m_slider->SetValue(110);

    CPPUNIT_ASSERT_EQUAL(100, m_slider->GetValue());
}

void SliderTestCase::Range()
{
    CPPUNIT_ASSERT_EQUAL(0, m_slider->GetMin());
    CPPUNIT_ASSERT_EQUAL(100, m_slider->GetMax());

    //Test negative ranges
    m_slider->SetRange(-50, 0);

    CPPUNIT_ASSERT_EQUAL(-50, m_slider->GetMin());
    CPPUNIT_ASSERT_EQUAL(0, m_slider->GetMax());

    //Test backwards ranges
    m_slider->SetRange(75, 50);

    CPPUNIT_ASSERT_EQUAL(75, m_slider->GetMin());
    CPPUNIT_ASSERT_EQUAL(50, m_slider->GetMax());
}

void SliderTestCase::Thumb()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_slider, wxEVT_SCROLL_THUMBTRACK);
    EventCounter count1(m_slider, wxEVT_SCROLL_THUMBRELEASE);
    EventCounter count2(m_slider, wxEVT_SCROLL_CHANGED);

    wxUIActionSimulator sim;

    m_slider->SetValue(0);

    sim.MouseMove(m_slider->ClientToScreen(wxPoint(10, 10)));
    sim.MouseDown();
    sim.MouseMove(m_slider->ClientToScreen(wxPoint(50, 10)));
    sim.MouseUp();

    wxYield();

    CPPUNIT_ASSERT(frame->GetEventCount(wxEVT_SCROLL_THUMBTRACK) != 0);
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_THUMBRELEASE));
#ifdef __WXMSW__
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_SCROLL_CHANGED));
#endif
}

#endif
