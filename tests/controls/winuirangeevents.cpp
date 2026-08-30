///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuirangeevents.cpp
// Purpose:     deterministic WinUI wxSlider/wxScrollBar contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "range-test-access.h"
#include "slider-test-access.h"

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/scrolbar.h"
#include "wx/slider.h"
#include "wx/scopeguard.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#include <cmath>
#include <limits>
#include <vector>

namespace
{

struct ObservedRangeEvent
{
    wxEventType type;
    int position;
};

#if wxUSE_SLIDER

class SliderHighContrastGuard final
{
public:
    explicit SliderHighContrastGuard(
        wxWinUIHighContrastOverrideForTesting value)
    {
        wxWinUISetHighContrastOverrideForTesting(value);
    }

    ~SliderHighContrastGuard()
    {
        wxWinUISetHighContrastOverrideForTesting(
            wxWinUIHighContrastOverrideForTesting::System);
    }

    void Set(wxWinUIHighContrastOverrideForTesting value)
    {
        wxWinUISetHighContrastOverrideForTesting(value);
    }

    wxDECLARE_NO_COPY_CLASS(SliderHighContrastGuard);
};

bool WaitForSliderVisualState(
    wxSlider& slider,
    wxWinUISliderTestAccess::VisualState *visual)
{
    return WaitFor(
        "realized WinUI Slider visual state",
        [&slider, visual]()
        {
            // The seam returns false until the shared slot, native template
            // and custom overlay belong to the same completed layout pass.
            return wxWinUISliderTestAccess::GetVisualState(slider, visual);
        });
}

const char *SliderConvergenceFailureName(
    wxWinUISliderTestAccess::ConvergenceFailure failure)
{
    using Failure = wxWinUISliderTestAccess::ConvergenceFailure;
    switch ( failure )
    {
        case Failure::None: return "none";
        case Failure::MissingPeer: return "missing-peer";
        case Failure::LifetimeChanged: return "lifetime-changed";
        case Failure::NotLoaded: return "not-loaded";
        case Failure::EmptyGeometry: return "empty-geometry";
        case Failure::MissingThumb: return "missing-thumb";
        case Failure::MissingTrack: return "missing-track";
        case Failure::ThumbNotArranged: return "thumb-not-arranged";
        case Failure::VisualStateDirty: return "visual-state-dirty";
        case Failure::CacheInvalid: return "cache-invalid";
        case Failure::LayoutNotReady: return "layout-not-ready";
        case Failure::SnapshotRejected: return "snapshot-rejected";
    }
    return "unknown";
}

void CheckSliderVisualTree(
    const wxWinUISliderTestAccess::VisualState& visual)
{
    CHECK(visual.visualTreeCoherent);
    CHECK(visual.sameXamlRoot);
    CHECK(visual.thumbIdentity != 0);
    CHECK(visual.trackIdentity != 0);
    CHECK(visual.rootWidth > 0.0);
    CHECK(visual.rootHeight > 0.0);
    CHECK(visual.axisStart < visual.axisEnd);
    CHECK(visual.nativeThumbAxis >= visual.axisStart - 1.0);
    CHECK(visual.nativeThumbAxis <= visual.axisEnd + 1.0);
    CHECK(visual.nativeTrack.width > 0.0);
    CHECK(visual.nativeTrack.height > 0.0);
    // The stock Windows App SDK Slider template must resolve by its exact
    // orientation-specific name. Unnamed custom-template fallback is tested
    // by production constraints, never silently substituted here.
    CHECK(visual.thumbNamedForOrientation);
}

double SliderAxisPosition(const wxWinUISliderTestAccess::VisualState& visual,
                          double ratio)
{
    if ( visual.axisReversed )
        ratio = 1.0 - ratio;
    return visual.axisStart + ratio * (visual.axisEnd - visual.axisStart);
}

void CheckWideSliderPrimaryLayout(
    const wxWinUISliderTestAccess::VisualState& visual,
    double selectionStartRatio,
    double selectionEndRatio)
{
    const auto checkInsideRoot = [&visual](
        const wxWinUISliderTestAccess::VisualRect& rect)
    {
        CHECK(rect.width > 0.0);
        CHECK(rect.height > 0.0);
        CHECK(rect.x >= -0.25);
        CHECK(rect.y >= -0.25);
        CHECK(rect.x + rect.width <= visual.rootWidth + 0.25);
        CHECK(rect.y + rect.height <= visual.rootHeight + 0.25);
    };
    checkInsideRoot(visual.minimumLabel);
    checkInsideRoot(visual.maximumLabel);

    // As on wxMSW, the endpoint labels are laid out at the extremities of the
    // control -- each on its own side of the axis -- and the slider itself is
    // shortened by their extent, so that they are never drawn over the track.
    const double minimumStart = visual.vertical
        ? visual.minimumLabel.y : visual.minimumLabel.x;
    const double minimumExtent = visual.vertical
        ? visual.minimumLabel.height : visual.minimumLabel.width;
    const double maximumStart = visual.vertical
        ? visual.maximumLabel.y : visual.maximumLabel.x;
    const double maximumExtent = visual.vertical
        ? visual.maximumLabel.height : visual.maximumLabel.width;
    const double rootExtent = visual.vertical
        ? visual.rootHeight : visual.rootWidth;

    const double startLabelEnd = visual.axisReversed
        ? maximumStart + maximumExtent : minimumStart + minimumExtent;
    const double endLabelStart = visual.axisReversed
        ? minimumStart : maximumStart;
    CHECK((visual.axisReversed ? maximumStart : minimumStart) ==
          Approx(0.0).margin(1.0));
    CHECK((visual.axisReversed ? minimumStart + minimumExtent
                               : maximumStart + maximumExtent) ==
          Approx(rootExtent).margin(1.0));
    CHECK(visual.axisStart >= startLabelEnd - 0.25);
    CHECK(visual.axisEnd <= endLabelStart + 0.25);
    CHECK(visual.renderedTickAxisMinimum ==
          Approx(visual.axisStart).margin(1.0));
    CHECK(visual.renderedTickAxisMaximum ==
          Approx(visual.axisEnd).margin(1.0));

    const double first =
        SliderAxisPosition(visual, selectionStartRatio);
    const double second =
        SliderAxisPosition(visual, selectionEndRatio);
    const double low = wxMin(first, second);
    const double high = wxMax(first, second);
    CHECK(visual.selectionVisible);
    if ( visual.vertical )
    {
        CHECK(visual.selection.y == Approx(low).margin(1.0));
        CHECK(visual.selection.y + visual.selection.height ==
              Approx(high).margin(1.0));
    }
    else
    {
        CHECK(visual.selection.x == Approx(low).margin(1.0));
        CHECK(visual.selection.x + visual.selection.width ==
              Approx(high).margin(1.0));
    }
}

void CheckSliderLayoutStable(
    const wxWinUISliderTestAccess::VisualState& expected,
    const wxWinUISliderTestAccess::VisualState& observed)
{
    CHECK(observed.sliderMarginLeft ==
          Approx(expected.sliderMarginLeft).margin(0.01));
    CHECK(observed.sliderMarginTop ==
          Approx(expected.sliderMarginTop).margin(0.01));
    CHECK(observed.sliderMarginRight ==
          Approx(expected.sliderMarginRight).margin(0.01));
    CHECK(observed.sliderMarginBottom ==
          Approx(expected.sliderMarginBottom).margin(0.01));
    CHECK(observed.axisStart == Approx(expected.axisStart).margin(0.1));
    CHECK(observed.axisEnd == Approx(expected.axisEnd).margin(0.1));
    CHECK(observed.axisCross == Approx(expected.axisCross).margin(0.1));
    CHECK(observed.nativeThumbAxis ==
          Approx(expected.nativeThumbAxis).margin(0.1));
    CHECK(observed.axisReversed == expected.axisReversed);
    CHECK(observed.thumbIdentity == expected.thumbIdentity);
    CHECK(observed.trackIdentity == expected.trackIdentity);
    CHECK(observed.renderedTickCount == expected.renderedTickCount);

    const auto checkRect = [](const wxWinUISliderTestAccess::VisualRect& first,
                              const wxWinUISliderTestAccess::VisualRect& second)
    {
        CHECK(second.x == Approx(first.x).margin(0.1));
        CHECK(second.y == Approx(first.y).margin(0.1));
        CHECK(second.width == Approx(first.width).margin(0.1));
        CHECK(second.height == Approx(first.height).margin(0.1));
    };
    checkRect(expected.minimumLabel, observed.minimumLabel);
    checkRect(expected.maximumLabel, observed.maximumLabel);
    checkRect(expected.selection, observed.selection);
}

struct SliderRetemplateDestroyProbe
{
    wxSlider **owner = nullptr;
    unsigned calls = 0;
};

void DestroySliderWhileTemplateDetached(wxSlider *slider, void *context)
{
    SliderRetemplateDestroyProbe * const probe =
        static_cast<SliderRetemplateDestroyProbe *>(context);
    ++probe->calls;
    CHECK(probe->owner != nullptr);
    CHECK(*probe->owner == slider);
    delete slider;
    *probe->owner = nullptr;
}

void BindSliderEvents(wxSlider& slider,
                      std::vector<ObservedRangeEvent>& events)
{
    const auto onScroll = [&events](wxScrollEvent& event)
    {
        events.push_back({event.GetEventType(), event.GetPosition()});
    };
    slider.Bind(wxEVT_SCROLL_LINEUP, onScroll);
    slider.Bind(wxEVT_SCROLL_LINEDOWN, onScroll);
    slider.Bind(wxEVT_SCROLL_PAGEUP, onScroll);
    slider.Bind(wxEVT_SCROLL_PAGEDOWN, onScroll);
    slider.Bind(wxEVT_SCROLL_TOP, onScroll);
    slider.Bind(wxEVT_SCROLL_BOTTOM, onScroll);
    slider.Bind(wxEVT_SCROLL_THUMBTRACK, onScroll);
    slider.Bind(wxEVT_SCROLL_THUMBRELEASE, onScroll);
    slider.Bind(wxEVT_SCROLL_CHANGED, onScroll);
    slider.Bind(wxEVT_SLIDER, [&events](wxCommandEvent& event)
    {
        events.push_back({event.GetEventType(), event.GetInt()});
    });
}

#endif // wxUSE_SLIDER

#if wxUSE_SCROLLBAR

void BindScrollBarEvents(wxScrollBar& bar,
                         std::vector<ObservedRangeEvent>& events)
{
    const auto onScroll = [&events](wxScrollEvent& event)
    {
        events.push_back({event.GetEventType(), event.GetPosition()});
    };
    bar.Bind(wxEVT_SCROLL_LINEUP, onScroll);
    bar.Bind(wxEVT_SCROLL_LINEDOWN, onScroll);
    bar.Bind(wxEVT_SCROLL_PAGEUP, onScroll);
    bar.Bind(wxEVT_SCROLL_PAGEDOWN, onScroll);
    bar.Bind(wxEVT_SCROLL_TOP, onScroll);
    bar.Bind(wxEVT_SCROLL_BOTTOM, onScroll);
    bar.Bind(wxEVT_SCROLL_THUMBTRACK, onScroll);
    bar.Bind(wxEVT_SCROLL_THUMBRELEASE, onScroll);
    bar.Bind(wxEVT_SCROLL_CHANGED, onScroll);
}

#endif // wxUSE_SCROLLBAR

void CheckEvent(const std::vector<ObservedRangeEvent>& events,
                std::size_t index,
                wxEventType type,
                int position)
{
    REQUIRE(index < events.size());
    CHECK(events[index].type == type);
    CHECK(events[index].position == position);
}

} // anonymous namespace

#if wxUSE_SLIDER

TEST_CASE("wxWinUI Slider has a canonical silent programmatic model",
          "[winui-range-events][slider][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider;
    CHECK(slider.GetValue() == 0);
    CHECK(slider.GetMin() == 0);
    CHECK(slider.GetMax() == 100);
    CHECK(slider.GetLineSize() == 1);
    CHECK(slider.GetPageSize() == 10);
    CHECK(slider.GetTickFreq() == 0);

    REQUIRE(slider.Create(
        parent, wxID_ANY, 150, 0, 100,
        wxDefaultPosition, wxDefaultSize,
        wxSL_VERTICAL | wxSL_INVERSE | wxSL_TICKS | wxSL_BOTH));
    CHECK(slider.GetValue() == 100);
    CHECK(slider.GetTickFreq() == 1);

    std::vector<ObservedRangeEvent> events;
    BindSliderEvents(slider, events);

    slider.SetValue(-25);
    CHECK(slider.GetValue() == 0);
    slider.SetRange(200, 300);
    CHECK(slider.GetValue() == 200);
    slider.SetRange(-50, 0);
    CHECK(slider.GetValue() == 0);
    slider.SetLineSize(3);
    slider.SetPageSize(17);
    slider.SetThumbLength(24);
    CHECK(events.empty());

    double minimum = 0;
    double maximum = 0;
    double value = 0;
    double smallChange = 0;
    double largeChange = 0;
    bool vertical = false;
    bool reversed = false;
    REQUIRE(wxWinUISliderTestAccess::GetPeerState(slider,
        &minimum, &maximum, &value, &smallChange, &largeChange,
        &vertical, &reversed));
    CHECK(minimum == -50.0);
    CHECK(maximum == 0.0);
    CHECK(value == 0.0);
    CHECK(smallChange == 3.0);
    CHECK(largeChange == 17.0);
    CHECK(vertical);
    CHECK(reversed);
    CHECK(slider.GetThumbLength() == 24);

    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.thumbLengthDIPs * visual.rasterScale ==
          Approx(24.0).margin(0.6));
    CHECK(visual.renderedTickCount > 4);
    CHECK(visual.firstSideTickCount == visual.secondSideTickCount);
}

TEST_CASE("wxWinUI Slider implements the complete Win32 decoration contract",
          "[winui-range-events][slider][ticks][selection][labels][thumb]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(
        parent, wxID_ANY, 50, 0, 100,
        wxDefaultPosition, wxSize(360, 100),
        wxSL_HORIZONTAL | wxSL_TICKS | wxSL_BOTH |
            wxSL_SELRANGE | wxSL_LABELS);

    slider.SetThumbLength(31);
    slider.SetTickFreq(20);
    slider.SetTick(10);
    slider.SetTick(37);
    slider.SetTick(90);
    slider.SetTick(37); // duplicate native ticks are idempotent too
    slider.SetTick(-1); // outside the current range is ignored
    slider.SetSelection(80, 20);

    CHECK(slider.GetThumbLength() == 31);
    CHECK(slider.GetTickFreq() == 20);
    CHECK(slider.GetSelStart() == 20);
    CHECK(slider.GetSelEnd() == 80);
    CHECK(wxWinUISliderTestAccess::HasTick(slider, 10));
    CHECK(wxWinUISliderTestAccess::HasTick(slider, 37));
    CHECK(wxWinUISliderTestAccess::HasTick(slider, 90));

    int thumbLength = 0;
    int selectionStart = 0;
    int selectionEnd = 0;
    unsigned manualTickCount = 0;
    bool selectionVisible = false;
    bool minMaxLabelsVisible = false;
    bool valueLabelVisible = false;
    bool rightToLeft = false;
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(slider,
        &thumbLength, &selectionStart, &selectionEnd, &manualTickCount,
        &selectionVisible, &minMaxLabelsVisible, &valueLabelVisible,
        &rightToLeft));
    CHECK(thumbLength == 31);
    CHECK(selectionStart == 20);
    CHECK(selectionEnd == 80);
    CHECK(manualTickCount == 3);
    CHECK(selectionVisible);
    CHECK(minMaxLabelsVisible);
    CHECK(valueLabelVisible);
    CHECK_FALSE(rightToLeft);

    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.renderedTickCount == 18);
    CHECK(visual.firstSideTickCount == 9);
    CHECK(visual.secondSideTickCount == 9);
    CHECK(visual.thumbLengthDIPs * visual.rasterScale ==
          Approx(31.0).margin(0.6));
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);
    CHECK(visual.selection.width > 0.0);
    CHECK(visual.selection.height > 0.0);
    CHECK(visual.renderedTickAxisMinimum ==
          Approx(visual.axisStart).margin(1.0));
    CHECK(visual.renderedTickAxisMaximum ==
          Approx(visual.axisEnd).margin(1.0));
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.5)).margin(1.0));
    CHECK(visual.selection.x ==
          Approx(SliderAxisPosition(visual, 0.2)).margin(1.0));
    CHECK(visual.selection.x + visual.selection.width ==
          Approx(SliderAxisPosition(visual, 0.8)).margin(1.0));
    // Min/max labels are laid out at the extremities of the control and
    // centred on the track band as on wxMSW, while the current value is on
    // the side opposite the default BOTTOM/RIGHT tick preference (top for a
    // horizontal slider).
    CHECK(visual.minimumLabel.y + visual.minimumLabel.height / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.maximumLabel.y + visual.maximumLabel.height / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.minimumLabel.y >= -0.25);
    CHECK(visual.maximumLabel.y >= -0.25);
    CHECK(visual.minimumLabel.y + visual.minimumLabel.height <=
          visual.rootHeight + 0.25);
    CHECK(visual.maximumLabel.y + visual.maximumLabel.height <=
          visual.rootHeight + 0.25);
    CHECK(visual.minimumLabel.x == Approx(0.0).margin(1.0));
    CHECK(visual.maximumLabel.x + visual.maximumLabel.width ==
          Approx(visual.rootWidth).margin(1.0));
    CHECK(visual.axisStart >=
          visual.minimumLabel.x + visual.minimumLabel.width - 0.25);
    CHECK(visual.axisEnd <= visual.maximumLabel.x + 0.25);
    // All three rectangles are observations transformed from the realized
    // XAML subtree into the root, not cached Canvas coordinates. The native
    // peer is cross-centred without authoring a cross-axis Margin, and the
    // value label remains wholly on the requested side inside that same root.
    CHECK(visual.sliderMarginTop == Approx(0.0));
    CHECK(visual.sliderMarginBottom == Approx(0.0));
    CHECK(visual.axisCross == Approx(visual.rootHeight / 2.0).margin(1.0));
    CHECK(visual.valueLabel.x >= -0.25);
    CHECK(visual.valueLabel.y >= -0.25);
    CHECK(visual.valueLabel.x + visual.valueLabel.width <=
          visual.rootWidth + 0.25);
    CHECK(visual.valueLabel.y + visual.valueLabel.height <=
          visual.rootHeight + 0.25);
    CHECK(visual.valueLabel.y + visual.valueLabel.height <=
          visual.axisCross);

    // Range changes clamp selection and discard ticks the native trackbar can
    // no longer represent, while preserving the surviving arbitrary tick.
    slider.SetRange(25, 75);
    CHECK(slider.GetSelStart() == 25);
    CHECK(slider.GetSelEnd() == 75);
    CHECK_FALSE(wxWinUISliderTestAccess::HasTick(slider, 10));
    CHECK(wxWinUISliderTestAccess::HasTick(slider, 37));
    CHECK_FALSE(wxWinUISliderTestAccess::HasTick(slider, 90));

    slider.SetLayoutDirection(wxLayout_RightToLeft);
    bool rootRightToLeft = false;
    bool sliderRightToLeft = false;
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(slider,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, &rightToLeft,
        &rootRightToLeft, &sliderRightToLeft));
    CAPTURE(rootRightToLeft, sliderRightToLeft);
    CHECK(rightToLeft);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.axisReversed);
    CHECK(visual.minimumLabel.x > visual.maximumLabel.x);
    // Right to left: the axis starts on the right, so the minimum label is the
    // one flush with the right edge and the maximum one with the left edge.
    CHECK(visual.maximumLabel.x == Approx(0.0).margin(1.0));
    CHECK(visual.minimumLabel.x + visual.minimumLabel.width ==
          Approx(visual.rootWidth).margin(1.0));

    // The RTL midpoint used to feed transformed Track coordinates back into
    // Slider.Margin and alternate forever inside LayoutUpdated. Prove that a
    // bounded series of dispatcher/layout drains leaves both the authored
    // native margin and every primary-axis visual unchanged.
    const wxWinUISliderTestAccess::VisualState stableRTL = visual;
    for ( unsigned drain = 0; drain < 4; ++drain )
    {
        INFO("RTL stability drain " << drain);
        wxYield();
        REQUIRE(WaitForSliderVisualState(slider, &visual));
        CheckSliderLayoutStable(stableRTL, visual);
    }

    slider.ClearTicks();
    CHECK(slider.GetTickFreq() == 20);
    CHECK_FALSE(wxWinUISliderTestAccess::HasTick(slider, 37));
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(slider,
        nullptr, nullptr, nullptr, &manualTickCount));
    CHECK(manualTickCount == 0);

    slider.ClearSel();
    CHECK(slider.GetSelStart() == 25);
    CHECK(slider.GetSelEnd() == 25);
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(slider,
        nullptr, nullptr, nullptr, nullptr, &selectionVisible));
    CHECK_FALSE(selectionVisible);
}

TEST_CASE("wxWinUI Slider AUTOTICKS mutates the realized tick tree",
          "[winui-range-events][slider][autoticks][visual-tree]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(
        parent, wxID_ANY, 5, 0, 10,
        wxDefaultPosition, wxSize(320, 80),
        wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_BOTTOM);
    wxWinUISliderTestAccess::VisualState visual;
    CHECK(slider.GetTickFreq() == 1);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.renderedTickCount == 11);
    CHECK(visual.firstSideTickCount == 0);
    CHECK(visual.secondSideTickCount == 11);
    CHECK(visual.renderedTickAxisMinimum ==
          Approx(visual.axisStart).margin(1.0));
    CHECK(visual.renderedTickAxisMaximum ==
          Approx(visual.axisEnd).margin(1.0));

    slider.SetTickFreq(2);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(slider.GetTickFreq() == 2);
    CHECK(visual.renderedTickCount == 6);

    slider.ClearTicks();
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.renderedTickCount == 2);

    slider.SetTick(3);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.renderedTickCount == 3);

    // Setting a frequency re-enables automatic marks without discarding an
    // independently authored native tick.
    slider.SetTickFreq(5);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.renderedTickCount == 4);
}

TEST_CASE("wxWinUI Slider labels oppose ticks and track endpoint direction",
          "[winui-range-events][slider][labels][rtl][inverse][visual-tree]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider horizontal(
        parent, wxID_ANY, 40, 0, 100,
        wxDefaultPosition, wxSize(360, 110),
        wxSL_TOP | wxSL_AUTOTICKS | wxSL_LABELS);
    horizontal.SetTickFreq(25);

    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(horizontal, &visual));
    CheckSliderVisualTree(visual);
    CHECK_FALSE(visual.vertical);
    CHECK(visual.firstSideTickCount > 0);
    CHECK(visual.secondSideTickCount == 0);
    CHECK(visual.axisCross == Approx(visual.rootHeight / 2.0).margin(1.0));
    CHECK(visual.valueLabel.x >= -0.25);
    CHECK(visual.valueLabel.y >= -0.25);
    CHECK(visual.valueLabel.x + visual.valueLabel.width <=
          visual.rootWidth + 0.25);
    CHECK(visual.valueLabel.y + visual.valueLabel.height <=
          visual.rootHeight + 0.25);
    CHECK(visual.valueLabel.y >= visual.axisCross);
    CHECK(visual.minimumLabel.y + visual.minimumLabel.height / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.maximumLabel.y + visual.maximumLabel.height / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.minimumLabel.x < visual.maximumLabel.x);
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.4)).margin(1.0));

    horizontal.SetLayoutDirection(wxLayout_RightToLeft);
    bool rootRightToLeft = false;
    bool sliderRightToLeft = false;
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(horizontal,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        &rootRightToLeft, &sliderRightToLeft));
    CAPTURE(rootRightToLeft, sliderRightToLeft);
    REQUIRE(WaitForSliderVisualState(horizontal, &visual));
    CHECK(visual.axisReversed);
    CHECK(visual.minimumLabel.x > visual.maximumLabel.x);
    // Right to left: the axis starts on the right, so the minimum label is the
    // one flush with the right edge and the maximum one with the left edge.
    CHECK(visual.maximumLabel.x == Approx(0.0).margin(1.0));
    CHECK(visual.minimumLabel.x + visual.minimumLabel.width ==
          Approx(visual.rootWidth).margin(1.0));
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.4)).margin(1.0));

    wxSlider vertical(
        parent, wxID_ANY, 40, 0, 100,
        wxDefaultPosition, wxSize(140, 360),
        wxSL_LEFT | wxSL_AUTOTICKS | wxSL_LABELS | wxSL_INVERSE);
    vertical.SetTickFreq(25);

    REQUIRE(WaitForSliderVisualState(vertical, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.vertical);
    CHECK(visual.firstSideTickCount > 0);
    CHECK(visual.secondSideTickCount == 0);
    CHECK(visual.axisCross == Approx(visual.rootWidth / 2.0).margin(1.0));
    CHECK(visual.valueLabel.x >= -0.25);
    CHECK(visual.valueLabel.y >= -0.25);
    CHECK(visual.valueLabel.x + visual.valueLabel.width <=
          visual.rootWidth + 0.25);
    CHECK(visual.valueLabel.y + visual.valueLabel.height <=
          visual.rootHeight + 0.25);
    CHECK(visual.valueLabel.x >= visual.axisCross);
    CHECK(visual.minimumLabel.x + visual.minimumLabel.width / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.maximumLabel.x + visual.maximumLabel.width / 2.0 ==
          Approx(visual.axisCross).margin(1.0));
    CHECK(visual.minimumLabel.y < visual.maximumLabel.y);
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.4)).margin(1.0));
}

TEST_CASE("wxWinUI Slider reserves its native track for wide endpoint labels",
          "[winui-range-events][slider][labels][wide-range][layout]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider horizontal(
        parent, wxID_ANY, -25000, -100000, 100000,
        wxDefaultPosition, wxSize(520, 120),
        wxSL_TOP | wxSL_TICKS | wxSL_LABELS | wxSL_SELRANGE);
    horizontal.SetTickFreq(50000);
    horizontal.SetSelection(-50000, 50000);

    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(horizontal, &visual));
    CheckSliderVisualTree(visual);
    CHECK_FALSE(visual.vertical);
    CHECK_FALSE(visual.axisReversed);
    CheckWideSliderPrimaryLayout(visual, 0.25, 0.75);
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.375)).margin(1.0));

    horizontal.SetLayoutDirection(wxLayout_RightToLeft);
    bool rootRightToLeft = false;
    bool sliderRightToLeft = false;
    REQUIRE(wxWinUISliderTestAccess::GetDecorations(horizontal,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        &rootRightToLeft, &sliderRightToLeft));
    REQUIRE(rootRightToLeft);
    REQUIRE(sliderRightToLeft);
    REQUIRE(WaitForSliderVisualState(horizontal, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.axisReversed);
    CheckWideSliderPrimaryLayout(visual, 0.25, 0.75);
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.375)).margin(1.0));

    wxSlider vertical(
        parent, wxID_ANY, -25000, -100000, 100000,
        wxDefaultPosition, wxSize(200, 520),
        wxSL_LEFT | wxSL_TICKS | wxSL_LABELS | wxSL_SELRANGE);
    vertical.SetTickFreq(50000);
    vertical.SetSelection(-50000, 50000);

    REQUIRE(WaitForSliderVisualState(vertical, &visual));
    CheckSliderVisualTree(visual);
    REQUIRE(visual.vertical);
    CheckWideSliderPrimaryLayout(visual, 0.25, 0.75);
    CHECK(visual.nativeThumbAxis ==
          Approx(SliderAxisPosition(visual, 0.375)).margin(1.0));
}

TEST_CASE("wxWinUI Slider thumb remains DIP-stable across XamlRoot migration",
          "[winui-range-events][slider][dpi][thumb][visual-tree]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(
        parent, wxID_ANY, 50, 0, 100,
        wxDefaultPosition, wxSize(500, 80));
    double sourceScale = 0.0;
    std::uintptr_t sourceXamlRoot = 0;
    std::uintptr_t sourceContentRoot = 0;
    REQUIRE(WaitFor(
        "source Slider XamlRoot",
        [&]()
        {
            return wxWinUISliderTestAccess::GetHostScaleState(slider,
                &sourceScale, &sourceXamlRoot, &sourceContentRoot);
        }));

    const auto physicalSizeAtScale = [](double scale,
                                        int widthDIPs,
                                        int heightDIPs)
    {
        return wxSize(
            static_cast<int>(std::lround(widthDIPs * scale)),
            static_cast<int>(std::lround(heightDIPs * scale)));
    };
    const wxSize sourceSliderSize =
        physicalSizeAtScale(sourceScale, 500, 80);
    slider.SetSize(sourceSliderSize);
    const bool refreshedAtSource =
        wxWinUISliderTestAccess::RefreshForScale(slider, sourceScale);
    INFO("convergence: " << SliderConvergenceFailureName(
             wxWinUISliderTestAccess::GetLastConvergenceFailure(slider)));
    REQUIRE(refreshedAtSource);

    const int sourcePixels =
        static_cast<int>(std::lround(300.0 * sourceScale));
    slider.SetThumbLength(sourcePixels);
    const bool thumbConverged =
        wxWinUISliderTestAccess::RefreshVisualState(slider);
    INFO("thumb convergence: " << SliderConvergenceFailureName(
             wxWinUISliderTestAccess::GetLastConvergenceFailure(slider)));
    REQUIRE(thumbConverged);

    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.rasterScale == Approx(sourceScale));
    CHECK(visual.rootWidth == Approx(500.0).margin(1.0));
    CHECK(visual.nativeTrack.width > visual.thumbLengthDIPs);
    CHECK(visual.thumbLengthDIPs * sourceScale ==
          Approx(sourcePixels).margin(0.6));
    CHECK(slider.GetThumbLength() == sourcePixels);
    const double stableThumbDIPs = visual.thumbLengthDIPs;

    wxFrame destination(
        nullptr, wxID_ANY, "Slider XamlRoot migration destination",
        wxPoint(-30000, -30000),
        physicalSizeAtScale(sourceScale, 700, 220));
    const auto restoreSourceParent = wxMakeGuard(
        [&]()
        {
            // Keep the stack-owned Slider out of the destination's child
            // destruction path even when a fatal assertion unwinds the test.
            if ( slider.GetParent() != parent )
                (void)slider.Reparent(parent);
            destination.Hide();
        });
    wxUnusedVar(restoreSourceParent);
    destination.Show();
    REQUIRE(slider.Reparent(&destination));

    double destinationScale = 0.0;
    std::uintptr_t destinationXamlRoot = 0;
    std::uintptr_t destinationContentRoot = 0;
    REQUIRE(WaitFor(
        "destination Slider XamlRoot",
        [&]()
        {
            return wxWinUISliderTestAccess::GetHostScaleState(slider,
                       &destinationScale,
                       &destinationXamlRoot,
                       &destinationContentRoot) &&
                   destinationXamlRoot != sourceXamlRoot;
        }));
    CAPTURE(sourceScale, destinationScale);
    CHECK(destinationXamlRoot != sourceXamlRoot);
    CHECK(destinationContentRoot == sourceContentRoot);

    // Both values come from real XamlRoots. They may legitimately be equal
    // when both TLWs are on the same monitor; the identity transition and the
    // per-root pixel/DIP relation are the deterministic parity proof here.
    destination.SetSize(
        physicalSizeAtScale(destinationScale, 700, 220));
    slider.SetSize(
        physicalSizeAtScale(destinationScale, 500, 80));
    REQUIRE(wxWinUISliderTestAccess::RefreshForScale(slider, destinationScale));
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.rasterScale == Approx(destinationScale));
    CHECK(visual.rootWidth == Approx(500.0).margin(1.0));
    CHECK(visual.nativeTrack.width > visual.thumbLengthDIPs);
    CHECK(visual.thumbLengthDIPs == Approx(stableThumbDIPs).margin(0.1));
    const int destinationPixels = static_cast<int>(
        std::lround(stableThumbDIPs * destinationScale));
    CHECK(slider.GetThumbLength() == destinationPixels);

    // The public setter consumes pixels in the destination root. Reapplying
    // its current physical value must not double-scale the stable DIP extent.
    slider.SetThumbLength(destinationPixels);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.thumbLengthDIPs == Approx(stableThumbDIPs).margin(0.1));
    CHECK(visual.thumbLengthDIPs * destinationScale ==
          Approx(destinationPixels).margin(0.6));

    REQUIRE(slider.Reparent(parent));
    slider.SetSize(sourceSliderSize);
    REQUIRE(WaitFor(
        "restored Slider XamlRoot",
        [&]()
        {
            double restoredScale = 0.0;
            std::uintptr_t restoredXamlRoot = 0;
            return wxWinUISliderTestAccess::GetHostScaleState(slider,
                       &restoredScale, &restoredXamlRoot) &&
                   restoredXamlRoot == sourceXamlRoot;
        }));
}

TEST_CASE("wxWinUI Slider reapplies the exact vertical thumb after retemplate",
          "[winui-range-events][slider][thumb][vertical][retemplate]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(
        parent, wxID_ANY, 50, 0, 100,
        wxDefaultPosition, wxSize(140, 360),
        wxSL_VERTICAL | wxSL_TICKS | wxSL_LEFT);
    slider.SetThumbLength(47);

    wxWinUISliderTestAccess::VisualState before;
    REQUIRE(WaitForSliderVisualState(slider, &before));
    CheckSliderVisualTree(before);
    REQUIRE(before.vertical);
    REQUIRE(before.thumbLengthDIPs * before.rasterScale ==
            Approx(47.0).margin(0.6));
    const std::uintptr_t retiredThumb = before.thumbIdentity;
    const std::uintptr_t retiredTrack = before.trackIdentity;
    CHECK(before.renderedTickAxisMinimum ==
          Approx(before.axisStart).margin(1.0));
    CHECK(before.renderedTickAxisMaximum ==
          Approx(before.axisEnd).margin(1.0));

    REQUIRE(wxWinUISliderTestAccess::RetemplatePeer(slider));
    wxWinUISliderTestAccess::VisualState after;
    REQUIRE(WaitForSliderVisualState(slider, &after));
    CheckSliderVisualTree(after);
    CHECK(after.vertical);
    CHECK(after.thumbIdentity != retiredThumb);
    CHECK(after.trackIdentity != retiredTrack);
    CHECK(after.thumbLengthDIPs * after.rasterScale ==
          Approx(47.0).margin(0.6));
    CHECK(after.renderedTickAxisMinimum ==
          Approx(after.axisStart).margin(1.0));
    CHECK(after.renderedTickAxisMaximum ==
          Approx(after.axisEnd).margin(1.0));
    CHECK(slider.GetThumbLength() == 47);
}

TEST_CASE("wxWinUI Slider selection follows enabled theme and high contrast",
          "[winui-range-events][slider][selection][theme][high-contrast]")
{
    SliderHighContrastGuard highContrast(
        wxWinUIHighContrastOverrideForTesting::ForceOff);
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(
        parent, wxID_ANY, 50, 0, 100,
        wxDefaultPosition, wxSize(320, 80),
        wxSL_HORIZONTAL | wxSL_SELRANGE);
    slider.SetSelection(20, 80);

    wxWinUISliderTestAccess::VisualState visual;
    const bool initialRefresh = wxWinUISliderTestAccess::RefreshVisualState(slider);
    INFO("convergence: " << SliderConvergenceFailureName(
             wxWinUISliderTestAccess::GetLastConvergenceFailure(slider)));
    REQUIRE(initialRefresh);
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.selectionVisual ==
          wxWinUISliderTestAccess::SelectionVisual::Normal);
    CHECK(visual.selectionThemeBound);
    CHECK(visual.selectionVisible);
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);

    slider.Enable(false);
    REQUIRE(wxWinUISliderTestAccess::RefreshVisualState(slider));
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.selectionVisual ==
          wxWinUISliderTestAccess::SelectionVisual::Disabled);
    CHECK(visual.selectionThemeBound);
    CHECK(visual.selectionVisible);
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);

    highContrast.Set(wxWinUIHighContrastOverrideForTesting::ForceOn);
    REQUIRE(wxWinUISliderTestAccess::RefreshVisualState(slider));
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.selectionVisual ==
          wxWinUISliderTestAccess::SelectionVisual::HighContrastDisabled);
    CHECK(visual.selectionThemeBound);
    CHECK(visual.selectionVisible);
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);

    slider.Enable(true);
    REQUIRE(wxWinUISliderTestAccess::RefreshVisualState(slider));
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CHECK(visual.selectionVisual ==
          wxWinUISliderTestAccess::SelectionVisual::HighContrast);
    CHECK(visual.selectionThemeBound);
    CHECK(visual.selectionVisible);
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);

    highContrast.Set(wxWinUIHighContrastOverrideForTesting::ForceOff);
    REQUIRE(wxWinUISliderTestAccess::RefreshVisualState(slider));
    wxWinUISliderTestAccess::VisualState beforeTheme;
    REQUIRE(WaitForSliderVisualState(slider, &beforeTheme));
    CheckSliderVisualTree(beforeTheme);
    REQUIRE(beforeTheme.selectionVisual ==
            wxWinUISliderTestAccess::SelectionVisual::Normal);
    REQUIRE(beforeTheme.visibleSelectionCount == 1);
    REQUIRE(beforeTheme.selectionBrushIdentity != 0);
    REQUIRE(beforeTheme.selectionHasSolidColor);
    REQUIRE((beforeTheme.selectionColorARGB & 0xff000000U) != 0);

    REQUIRE(wxWinUISliderTestAccess::DeliverThemeChanged(slider));
    REQUIRE(WaitForSliderVisualState(slider, &visual));
    CheckSliderVisualTree(visual);
    CHECK(visual.selectionVisual ==
          wxWinUISliderTestAccess::SelectionVisual::Normal);
    CHECK(visual.selectionThemeBound);
    CHECK(visual.selectionVisible);
    CHECK(visual.visibleSelectionCount == 1);
    CHECK(visual.selectionBrushIdentity != 0);
    CHECK(visual.selectionHasSolidColor);
    CHECK((visual.selectionColorARGB & 0xff000000U) != 0);
    CHECK(visual.actualTheme != beforeTheme.actualTheme);
    // Theme resources are allowed to mutate the same brush in place (and an
    // accent colour may legitimately be equal in Light/Dark). The oracle
    // therefore observes the concrete identity and ARGB on both sides while
    // requiring the peer's actual theme transition, without inventing an
    // expected colour from the wx model.
    INFO("before brush=" << beforeTheme.selectionBrushIdentity
         << " argb=" << beforeTheme.selectionColorARGB
         << ", after brush=" << visual.selectionBrushIdentity
         << " argb=" << visual.selectionColorARGB);
}

TEST_CASE("wxWinUI Slider setters cancel deferred unclassified changes",
          "[winui-range-events][slider][programmatic-race]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(parent, wxID_ANY, 50, 0, 100);
    std::vector<ObservedRangeEvent> events;
    BindSliderEvents(slider, events);

    REQUIRE(wxWinUISliderTestAccess::SetUnclassifiedPeerValue(slider, 70));
    CHECK(slider.GetValue() == 70);
    slider.SetValue(40);
    wxYield();

    double minimum = -1;
    double maximum = -1;
    double peerValue = -1;
    REQUIRE(wxWinUISliderTestAccess::GetPeerState(slider,
        &minimum, &maximum, &peerValue));
    CHECK(slider.GetValue() == 40);
    CHECK(peerValue == 40.0);
    CHECK(events.empty());

    REQUIRE(wxWinUISliderTestAccess::SetUnclassifiedPeerValue(slider, 80));
    CHECK(slider.GetValue() == 80);
    slider.SetRange(10, 30);
    wxYield();

    REQUIRE(wxWinUISliderTestAccess::GetPeerState(slider,
        &minimum, &maximum, &peerValue));
    CHECK(slider.GetMin() == 10);
    CHECK(slider.GetMax() == 30);
    CHECK(slider.GetValue() == 30);
    CHECK(minimum == 10.0);
    CHECK(maximum == 30.0);
    CHECK(peerValue == 30.0);
    CHECK(events.empty());
}

TEST_CASE("wxWinUI Slider maps each input to one exact wx sequence",
          "[winui-range-events][slider][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(parent, wxID_ANY, 50, 0, 100);
    std::vector<ObservedRangeEvent> events;
    BindSliderEvents(slider, events);

    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::LineDecrement, 49));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_LINEUP, 49);
    CheckEvent(events, 1, wxEVT_SLIDER, 49);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 49);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::LineIncrement, 50));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_LINEDOWN, 50);
    CheckEvent(events, 1, wxEVT_SLIDER, 50);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 50);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::PageDecrement, 30));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEUP, 30);
    CheckEvent(events, 1, wxEVT_SLIDER, 30);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 30);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::PageIncrement, 50));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEDOWN, 50);
    CheckEvent(events, 1, wxEVT_SLIDER, 50);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 50);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::Minimum, 0));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_TOP, 0);
    CheckEvent(events, 1, wxEVT_SLIDER, 0);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 0);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::Maximum, 100));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_BOTTOM, 100);
    CheckEvent(events, 1, wxEVT_SLIDER, 100);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 100);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::Wheel, 90));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_CHANGED, 90);
    CheckEvent(events, 1, wxEVT_SLIDER, 90);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::Automation, 80));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_CHANGED, 80);
    CheckEvent(events, 1, wxEVT_SLIDER, 80);

    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::ThumbTrack, 70, false));
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::ThumbTrack, 75, true));
    REQUIRE(events.size() == 7);
    CheckEvent(events, 0, wxEVT_SCROLL_THUMBTRACK, 70);
    CheckEvent(events, 1, wxEVT_SLIDER, 70);
    CheckEvent(events, 2, wxEVT_SCROLL_THUMBTRACK, 75);
    CheckEvent(events, 3, wxEVT_SLIDER, 75);
    CheckEvent(events, 4, wxEVT_SCROLL_THUMBRELEASE, 75);
    CheckEvent(events, 5, wxEVT_SLIDER, 75);
    CheckEvent(events, 6, wxEVT_SCROLL_CHANGED, 75);

    // Neither a movement blocked at an endpoint nor a setter followed by a
    // release may manufacture an event.
    events.clear();
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::LineIncrement, 75));
    CHECK(events.empty());
    slider.SetValue(40);
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::ThumbRelease, 40));
    CHECK(events.empty());

    slider.Enable(false);
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(slider,
        wxWinUISliderTestAccess::Input::LineDecrement, 20));
    CHECK(slider.GetValue() == 40);
    CHECK(events.empty());
}

TEST_CASE("wxWinUI Slider accepts the complete int range without overflow",
          "[winui-range-events][slider][wide-range]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    constexpr int minimum = std::numeric_limits<int>::min();
    constexpr int maximum = std::numeric_limits<int>::max();
    wxSlider slider(parent, wxID_ANY, 0, minimum, maximum);

    const std::int64_t expectedPageSize =
        (static_cast<std::int64_t>(maximum) -
         static_cast<std::int64_t>(minimum)) / 10;
    CHECK(slider.GetMin() == minimum);
    CHECK(slider.GetMax() == maximum);
    CHECK(slider.GetValue() == 0);
    CHECK(slider.GetPageSize() == static_cast<int>(expectedPageSize));
}

TEST_CASE("wxWinUI Slider distinguishes track pages from thumb drags",
          "[winui-range-events][slider][pointer-classification]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider slider(parent, wxID_ANY, 50, 0, 100);
    std::vector<ObservedRangeEvent> events;
    BindSliderEvents(slider, events);

    // Exercise the ordering where the Slider class handler changes Value
    // before our handled-routed PointerPressed observer.
    REQUIRE(wxWinUISliderTestAccess::ApplyPointerInput(slider,
        false, 70, true));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEDOWN, 70);
    CheckEvent(events, 1, wxEVT_SLIDER, 70);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 70);

    // And the inverse ordering: PointerPressed is classified first and the
    // first ValueChanged delta resolves the page direction.
    events.clear();
    slider.SetValue(50);
    REQUIRE(wxWinUISliderTestAccess::ApplyPointerInput(slider,
        false, 30, false));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEUP, 30);
    CheckEvent(events, 1, wxEVT_SLIDER, 30);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 30);

    // A real Thumb source retains the drag release notification.
    events.clear();
    slider.SetValue(50);
    REQUIRE(wxWinUISliderTestAccess::ApplyPointerInput(slider,
        true, 75, true));
    REQUIRE(events.size() == 5);
    CheckEvent(events, 0, wxEVT_SCROLL_THUMBTRACK, 75);
    CheckEvent(events, 1, wxEVT_SLIDER, 75);
    CheckEvent(events, 2, wxEVT_SCROLL_THUMBRELEASE, 75);
    CheckEvent(events, 3, wxEVT_SLIDER, 75);
    CheckEvent(events, 4, wxEVT_SCROLL_CHANGED, 75);

    wxSlider inverse(
        parent, wxID_ANY, 50, 0, 100,
        wxDefaultPosition, wxDefaultSize,
        wxSL_HORIZONTAL | wxSL_INVERSE);
    events.clear();
    BindSliderEvents(inverse, events);
    REQUIRE(wxWinUISliderTestAccess::ApplyPointerInput(inverse,
        false, 70, true));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEUP, 70);
    CheckEvent(events, 1, wxEVT_SLIDER, 70);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 70);
}

TEST_CASE("wxWinUI Slider callbacks survive owner destruction",
          "[winui-range-events][slider][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider *slider =
        new wxSlider(parent, wxID_ANY, 50, 0, 100);
    unsigned events = 0;
    slider->Bind(wxEVT_SCROLL_LINEUP, [&](wxScrollEvent&)
    {
        ++events;
        wxSlider * const doomed = slider;
        slider = nullptr;
        delete doomed;
    });

    wxSlider * const invoking = slider;
    REQUIRE(wxWinUISliderTestAccess::ApplyInput(*invoking,
        wxWinUISliderTestAccess::Input::LineDecrement, 49));
    CHECK(slider == nullptr);
    CHECK(events == 1);

    for ( int i = 0; i < 50; ++i )
    {
        wxSlider * const transient =
            new wxSlider(parent, wxID_ANY, 5, 0, 10);
        delete transient;
    }
}

TEST_CASE("wxWinUI Slider template detach cannot outlive its owner",
          "[winui-range-events][slider][retemplate][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSlider *slider = new wxSlider(
        parent, wxID_ANY, 5, 0, 10,
        wxDefaultPosition, wxSize(120, 300),
        wxSL_VERTICAL);
    slider->SetThumbLength(39);
    wxWinUISliderTestAccess::VisualState visual;
    REQUIRE(WaitForSliderVisualState(*slider, &visual));
    CheckSliderVisualTree(visual);

    SliderRetemplateDestroyProbe probe;
    probe.owner = &slider;
    wxSlider * const invoking = slider;
    CHECK_FALSE(wxWinUISliderTestAccess::RetemplatePeer(*invoking,
        &DestroySliderWhileTemplateDetached, &probe));
    CHECK(probe.calls == 1);
    CHECK(slider == nullptr);

    // Drain any LayoutUpdated/ActualThemeChanged delegate copied by XAML
    // before the template was detached. It owns only invalidated state.
    wxYield();
    wxYield();
}

TEST_CASE("wxWinUI Slider Create survives Loaded and final-size destruction",
          "[winui-range-events][slider][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("shared-host Loaded boundary")
    {
        wxSlider *slider = new wxSlider;
        wxSlider * const invoking = slider;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                attached = true;
                CHECK(window == invoking);
                wxSlider * const doomed = slider;
                slider = nullptr;
                delete doomed;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, 5, 0, 10);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(slider == nullptr);
        delete slider;
    }

    SECTION("SetInitialSize wx callback boundary")
    {
        wxSlider *slider = new wxSlider;
        wxSlider * const invoking = slider;
        bool attached = false;
        bool sizeCallback = false;
        slider->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent& event)
            {
                if ( !attached )
                {
                    event.Skip();
                    return;
                }

                sizeCallback = true;
                wxSlider * const doomed = slider;
                slider = nullptr;
                delete doomed;
            });
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                CHECK(window == invoking);
                attached = true;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, 5, 0, 10,
            wxDefaultPosition, wxDefaultSize);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK(sizeCallback);
        CHECK_FALSE(created);
        CHECK(slider == nullptr);
        delete slider;
    }
}

#endif // wxUSE_SLIDER

#if wxUSE_SCROLLBAR

TEST_CASE("wxWinUI ScrollBar keeps its model and peer canonically clamped",
          "[winui-range-events][scrollbar][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxScrollBar bar;
    CHECK(bar.GetThumbPosition() == 0);
    CHECK(bar.GetThumbSize() == 1);
    CHECK(bar.GetRange() == 0);
    CHECK(bar.GetPageSize() == 1);
    REQUIRE(bar.Create(
        parent, wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxSB_VERTICAL));

    std::vector<ObservedRangeEvent> events;
    BindScrollBarEvents(bar, events);
    bar.SetScrollbar(99, 10, 40, 7);
    CHECK(bar.GetThumbPosition() == 30);
    CHECK(bar.GetThumbSize() == 10);
    CHECK(bar.GetRange() == 40);
    CHECK(bar.GetPageSize() == 7);
    bar.SetThumbPosition(-50);
    CHECK(bar.GetThumbPosition() == 0);
    CHECK(events.empty());

    double minimum = -1;
    double maximum = -1;
    double value = -1;
    double viewport = -1;
    double smallChange = -1;
    double largeChange = -1;
    bool vertical = false;
    REQUIRE(wxWinUIRangeTestAccess::GetPeerState(bar,
        &minimum, &maximum, &value, &viewport,
        &smallChange, &largeChange, &vertical));
    CHECK(minimum == 0.0);
    CHECK(maximum == 30.0);
    CHECK(value == 0.0);
    CHECK(viewport == 10.0);
    CHECK(smallChange == 1.0);
    CHECK(largeChange == 7.0);
    CHECK(vertical);
}

TEST_CASE("wxWinUI ScrollBar maps named peer actions one-for-one",
          "[winui-range-events][scrollbar][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxScrollBar bar(parent, wxID_ANY);
    bar.SetScrollbar(10, 10, 100, 20);
    std::vector<ObservedRangeEvent> events;
    BindScrollBarEvents(bar, events);

    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::SmallDecrement, 9));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 9));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_LINEUP, 9);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 9);

    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::SmallIncrement, 10));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 10));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_LINEDOWN, 10);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 10);

    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::LargeDecrement, 0));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 0));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEUP, 0);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 0);

    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::LargeIncrement, 20));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 20));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_PAGEDOWN, 20);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 20);

    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::First, 0));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 0));
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_TOP, 0);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 0);

    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::ThumbTrack, 30));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::ThumbPosition, 35));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 35));
    REQUIRE(events.size() == 3);
    CheckEvent(events, 0, wxEVT_SCROLL_THUMBTRACK, 30);
    CheckEvent(events, 1, wxEVT_SCROLL_THUMBRELEASE, 35);
    CheckEvent(events, 2, wxEVT_SCROLL_CHANGED, 35);

    // Clamp before storing or publishing. A repeated movement at the clamped
    // endpoint is suppressed, but EndScroll remains a single CHANGED.
    events.clear();
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::Last, 500));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::SmallIncrement, 500));
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::EndScroll, 500));
    CHECK(bar.GetThumbPosition() == 90);
    REQUIRE(events.size() == 2);
    CheckEvent(events, 0, wxEVT_SCROLL_BOTTOM, 90);
    CheckEvent(events, 1, wxEVT_SCROLL_CHANGED, 90);

    events.clear();
    bar.SetThumbPosition(10);
    CHECK(events.empty());
    bar.Enable(false);
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(bar,
        wxWinUIRangeTestAccess::ScrollBarAction::SmallIncrement, 11));
    CHECK(bar.GetThumbPosition() == 10);
    CHECK(events.empty());
}

TEST_CASE("wxWinUI ScrollBar callbacks survive owner destruction",
          "[winui-range-events][scrollbar][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxScrollBar *bar = new wxScrollBar(parent, wxID_ANY);
    bar->SetScrollbar(5, 10, 100, 20);
    unsigned events = 0;
    bar->Bind(wxEVT_SCROLL_LINEDOWN, [&](wxScrollEvent&)
    {
        ++events;
        wxScrollBar * const doomed = bar;
        bar = nullptr;
        delete doomed;
    });

    wxScrollBar * const invoking = bar;
    REQUIRE(wxWinUIRangeTestAccess::ApplyScrollBarAction(*invoking,
        wxWinUIRangeTestAccess::ScrollBarAction::SmallIncrement, 6));
    CHECK(bar == nullptr);
    CHECK(events == 1);

    for ( int i = 0; i < 50; ++i )
    {
        wxScrollBar * const transient =
            new wxScrollBar(parent, wxID_ANY);
        delete transient;
    }
}

TEST_CASE("wxWinUI ScrollBar Create survives synchronous Loaded destruction",
          "[winui-range-events][scrollbar][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxScrollBar *bar = new wxScrollBar;
    wxScrollBar * const invoking = bar;
    bool attached = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *window)
        {
            attached = true;
            CHECK(window == invoking);
            wxScrollBar * const doomed = bar;
            bar = nullptr;
            delete doomed;
        });

    const bool created = invoking->Create(parent, wxID_ANY);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    CHECK(attached);
    CHECK_FALSE(created);
    CHECK(bar == nullptr);
    delete bar;
}

#endif // wxUSE_SCROLLBAR

#endif // __WXWINUI__ && wxUSE_WINUI3
