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

#ifdef __WXQT__
    #include <QtGlobal> // QT_VERSION and QT_VERSION_CHECK
#endif

#include "wx/uiaction.h"
#include "testableframe.h"

#include <memory>

class SliderTestCase
{
public:
    SliderTestCase() { Create(wxSL_HORIZONTAL); }

protected:
    // Recreate the slider using the given style instead of the default one.
    void Create(long style)
    {
        m_slider = make_unique<wxSlider>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         50, 0, 100,
                                         wxDefaultPosition, wxDefaultSize,
                                         style);
    }

    std::unique_ptr<wxSlider> m_slider;

    wxDECLARE_NO_COPY_CLASS(SliderTestCase);
};

// These tests don't pass under macOS, where the keys used below don't work.
#ifndef __WXOSX__

TEST_CASE_METHOD(SliderTestCase, "Slider::PageUpDown", "[slider]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter pageup(m_slider.get(), wxEVT_SCROLL_PAGEUP);
    EventCounter pagedown(m_slider.get(), wxEVT_SCROLL_PAGEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_PAGEUP);
    sim.Char(WXK_PAGEDOWN);

    wxYield();

    CHECK(pageup.GetCount() == 1);
    CHECK(pagedown.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(SliderTestCase, "Slider::LineUpDown", "[slider]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter lineup(m_slider.get(), wxEVT_SCROLL_LINEUP);
    EventCounter linedown(m_slider.get(), wxEVT_SCROLL_LINEDOWN);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    sim.Char(WXK_DOWN);

    wxYield();

    CHECK(lineup.GetCount() == 1);
    CHECK(linedown.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(SliderTestCase, "Slider::EvtSlider", "[slider]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter slider(m_slider.get(), wxEVT_SLIDER);

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    sim.Char(WXK_UP);
    sim.Char(WXK_DOWN);

    wxYield();

    CHECK(slider.GetCount() == 2);
#endif
}

TEST_CASE_METHOD(SliderTestCase, "Slider::LinePageSize", "[slider]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;

    m_slider->SetFocus();
    wxYield();

    m_slider->SetPageSize(20);

    sim.Char(WXK_PAGEUP);

    wxYield();

    CHECK(m_slider->GetPageSize() == 20);
    CHECK(m_slider->GetValue() == 30);

    m_slider->SetLineSize(2);

    sim.Char(WXK_UP);

    wxYield();

    CHECK(m_slider->GetLineSize() == 2);
    CHECK(m_slider->GetValue() == 28);
#endif
}

#endif // !__WXOSX__

TEST_CASE_METHOD(SliderTestCase, "Slider::Value", "[slider]")
{
    // Run the test for both normal and inversed sliders.
    const bool inversed = GENERATE(false, true);
    CAPTURE(inversed);
    if ( inversed )
        Create(wxSL_HORIZONTAL | wxSL_INVERSE);

    m_slider->SetValue(30);

    CHECK(m_slider->GetValue() == 30);

    //When setting a value larger that max or smaller than min
    //max and min are set
    m_slider->SetValue(-1);

    CHECK(m_slider->GetValue() == 0);

    m_slider->SetValue(110);

    CHECK(m_slider->GetValue() == 100);
}

TEST_CASE_METHOD(SliderTestCase, "Slider::Range", "[slider]")
{
    // Run the test for both normal and inversed sliders.
    const bool inversed = GENERATE(false, true);
    CAPTURE(inversed);
    if ( inversed )
        Create(wxSL_HORIZONTAL | wxSL_INVERSE);

    CHECK(m_slider->GetMin() == 0);
    CHECK(m_slider->GetMax() == 100);

    // Changing range shouldn't change the value.
    m_slider->SetValue(17);
    m_slider->SetRange(0, 200);
    CHECK(m_slider->GetValue() == 17);

    //Test negative ranges
    m_slider->SetRange(-50, 0);

    CHECK(m_slider->GetMin() == -50);
    CHECK(m_slider->GetMax() == 0);
}

TEST_CASE_METHOD(SliderTestCase, "Slider::Thumb", "[slider]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter track(m_slider.get(), wxEVT_SCROLL_THUMBTRACK);
    EventCounter release(m_slider.get(), wxEVT_SCROLL_THUMBRELEASE);
    EventCounter changed(m_slider.get(), wxEVT_SCROLL_CHANGED);

    wxUIActionSimulator sim;

    m_slider->SetValue(0);

    // use the slider real position for dragging the mouse.
    const int ypos = m_slider->GetSize().y / 2;
    sim.MouseDragDrop(m_slider->ClientToScreen(wxPoint(10, ypos)),m_slider->ClientToScreen(wxPoint(50, ypos)));
    wxYield();

    CHECK(track.GetCount() != 0);
    CHECK(release.GetCount() == 1);

#ifdef __WXQT__
    #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        WARN("wxEVT_SCROLL_CHANGED is generated twice with Qt 6.8, skipping test");
        return;
    #endif
#endif
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXQT__)
    CHECK(changed.GetCount() == 1);
#endif
#endif
}

#endif
