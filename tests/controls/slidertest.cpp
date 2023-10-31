///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/slidertest.cpp
// Purpose:     wxSlider unit test
// Author:      Steven Lamerton
// Created:     2010-07-20
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SLIDER


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

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( SliderTestCase );
#ifndef __WXOSX__
        WXUISIM_TEST( PageUpDown );
        WXUISIM_TEST( LineUpDown );
        WXUISIM_TEST( EvtSlider );
        WXUISIM_TEST( LinePageSize );
#endif
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Range );
        WXUISIM_TEST( Thumb );
        CPPUNIT_TEST( PseudoTest_Inversed );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Range );
    CPPUNIT_TEST_SUITE_END();

    void PageUpDown();
    void LineUpDown();
    void EvtSlider();
    void LinePageSize();
    void Value();
    void Range();
    void Thumb();
    void PseudoTest_Inversed() { ms_inversed = true; }

    static bool ms_inversed;

    wxSlider* m_slider;

    wxDECLARE_NO_COPY_CLASS(SliderTestCase);
};

bool SliderTestCase::ms_inversed = false;

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SliderTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SliderTestCase, "SliderTestCase" );

void SliderTestCase::setUp()
{
    long style = wxSL_HORIZONTAL;

    if ( ms_inversed )
        style |= wxSL_INVERSE;

    m_slider = new wxSlider(wxTheApp->GetTopWindow(), wxID_ANY, 50, 0, 100,
                            wxDefaultPosition, wxDefaultSize,
                            style);
}

void SliderTestCase::tearDown()
{
    wxDELETE(m_slider);
}

void SliderTestCase::PageUpDown()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter pageup(m_slider, wxEVT_SCROLL_PAGEUP);
    EventCounter pagedown(m_slider, wxEVT_SCROLL_PAGEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_PAGEUP);
    sim.Char(WXK_PAGEDOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, pageup.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, pagedown.GetCount());
#endif
}

void SliderTestCase::LineUpDown()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter lineup(m_slider, wxEVT_SCROLL_LINEUP);
    EventCounter linedown(m_slider, wxEVT_SCROLL_LINEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, lineup.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, linedown.GetCount());
#endif
}

void SliderTestCase::EvtSlider()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter slider(m_slider, wxEVT_SLIDER);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    sim.Char(WXK_DOWN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, slider.GetCount());
#endif
}

void SliderTestCase::LinePageSize()
{
#if wxUSE_UIACTIONSIMULATOR
    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

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
#endif
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

    // Changing range shouldn't change the value.
    m_slider->SetValue(17);
    m_slider->SetRange(0, 200);
    CPPUNIT_ASSERT_EQUAL(17, m_slider->GetValue());

    //Test negative ranges
    m_slider->SetRange(-50, 0);

    CPPUNIT_ASSERT_EQUAL(-50, m_slider->GetMin());
    CPPUNIT_ASSERT_EQUAL(0, m_slider->GetMax());
}

void SliderTestCase::Thumb()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter track(m_slider, wxEVT_SCROLL_THUMBTRACK);
    EventCounter release(m_slider, wxEVT_SCROLL_THUMBRELEASE);
    EventCounter changed(m_slider, wxEVT_SCROLL_CHANGED);

    wxUIActionSimulator sim;

    m_slider->SetValue(0);

    // use the slider real position for dragging the mouse.
    const int ypos = m_slider->GetSize().y / 2;
    sim.MouseDragDrop(m_slider->ClientToScreen(wxPoint(10, ypos)),m_slider->ClientToScreen(wxPoint(50, ypos)));
    wxYield();

    CPPUNIT_ASSERT(track.GetCount() != 0);
    CPPUNIT_ASSERT_EQUAL(1, release.GetCount());
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXQT__)
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());
#endif
#endif
}

#endif
