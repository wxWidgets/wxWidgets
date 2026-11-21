/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicompositegeometry.cpp
// Purpose:     Arranged geometry of native composite shared-island slots
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/frame.h"
#include "wx/math.h"
#include "wx/scopeguard.h"
#include "wx/utils.h"
#include "wx/window.h"
#include "../../src/winui/private.h"
#include "wx/winui/private/tlwhost.h"

#include <array>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace MUXM = MUX::Media;

void DrainCompositeGeometryDispatch(int rounds = 20)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

template <typename Predicate>
bool WaitForCompositeGeometry(Predicate predicate)
{
    for ( int i = 0; i < 100; ++i )
    {
        if ( predicate() )
            return true;
        DrainCompositeGeometryDispatch(1);
    }
    return predicate();
}

// Read the independently arranged native tree, not the child's HWND or the
// slot cache. Round physical endpoints, including non-integral DPI scales.
wxRect ArrangedCompositeRect(const wxWinUITopLevelHost& host,
                             const MUXC::Grid& container)
{
    const auto bounds = container.TransformToVisual(host.Root()).TransformBounds(
        winrt::Windows::Foundation::Rect{
            0, 0, float(container.ActualWidth()),
            float(container.ActualHeight())});
    const double scale = host.GetScale();
    const int left = wxRound(bounds.X * scale);
    const int top = wxRound(bounds.Y * scale);
    return wxRect(left, top,
                  wxRound((bounds.X + bounds.Width) * scale) - left,
                  wxRound((bounds.Y + bounds.Height) * scale) - top);
}

bool CompositeBranchHasHit(const wxWinUITopLevelHost& host,
                           const MUX::UIElement& branch,
                           const wxRect& rect)
{
    const double scale = host.GetScale();
    const auto hits = MUXM::VisualTreeHelper::FindElementsInHostCoordinates(
        winrt::Windows::Foundation::Point{
            float((rect.x + rect.width / 2.0) / scale),
            float((rect.y + rect.height / 2.0) / scale)},
        host.Root());
    for ( const auto& hit : hits )
    {
        MUX::DependencyObject current = hit;
        for ( int depth = 0; current && depth < 64; ++depth )
        {
            if ( current == branch )
                return true;
            if ( current == host.Root() )
                break;
            current = MUXM::VisualTreeHelper::GetParent(current);
        }
    }
    return false;
}

} // namespace

TEST_CASE("HostGeometry::CompositeChildrenFollowNativeArrange",
          "[HostGeometry][winui-composite]")
{
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "composite-geometry",
        wxPoint(-32000, -32000), wxSize(560, 340));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainCompositeGeometryDispatch();
    });

    wxWindow * const owner = new wxWindow(
        frame, wxID_ANY, wxPoint(20, 25), wxSize(320, 200));
    // Deliberately overlapping tiny shells: these must not size, position or
    // clip the children arranged by the native composite's content mounts.
    const std::array<wxWindow *, 2> windows = {
        new wxWindow(owner, wxID_ANY, wxPoint(3, 3), wxSize(8, 8)),
        new wxWindow(owner, wxID_ANY, wxPoint(3, 3), wxSize(8, 8))
    };
    const MUXC::StackPanel groups;
    groups.Orientation(MUXC::Orientation::Vertical);
    groups.Spacing(8);
    groups.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
    groups.VerticalAlignment(MUX::VerticalAlignment::Top);
    const std::array<MUXC::ContentControl, 2> mounts = {
        MUXC::ContentControl(), MUXC::ContentControl()
    };
    const std::array<MUXC::Button, 2> buttons = {
        MUXC::Button(), MUXC::Button()
    };
    const std::array<double, 2> heights = {40, 56};
    for ( size_t i = 0; i < mounts.size(); ++i )
    {
        mounts[i].HorizontalContentAlignment(MUX::HorizontalAlignment::Stretch);
        mounts[i].VerticalContentAlignment(MUX::VerticalAlignment::Stretch);
        mounts[i].Padding(MUX::Thickness{0, 0, 0, 0});
        buttons[i].Height(heights[i]);
        buttons[i].HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        buttons[i].Content(winrt::box_value(
            winrt::hstring(i == 0 ? L"First" : L"Second")));
        groups.Children().Append(mounts[i]);
    }

    // Real production hosts own the carriers and attach into the mounts;
    // tests never move a slot's container manually within the visual tree.
    wxWinUIControlHost parentHost;
    std::array<wxWinUIControlHost, 2> childHosts;
    REQUIRE(parentHost.Initialize(owner));
    REQUIRE(parentHost.SetContent(groups, groups));
    for ( size_t i = 0; i < windows.size(); ++i )
    {
        REQUIRE(childHosts[i].Initialize(windows[i]));
        REQUIRE(childHosts[i].SetContentInParent(
            buttons[i], buttons[i], owner, mounts[i]));
    }
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(owner);
    REQUIRE(host != nullptr);
    const std::array<wxWinUISlot *, 2> slots = {
        host->FindSlot(windows[0]), host->FindSlot(windows[1])
    };
    REQUIRE(slots[0] != nullptr);
    REQUIRE(slots[1] != nullptr);
    const std::array<MUXC::Grid, 2> containers = {
        slots[0]->GetContainer(), slots[1]->GetContainer()
    };
    for ( size_t i = 0; i < slots.size(); ++i )
    {
        CHECK(wxWinUITopLevelHost::FindSlotOwner(windows[i]) == host);
        CHECK(mounts[i].Content().try_as<MUXC::Grid>() == containers[i]);
    }

    frame->ShowWithoutActivating();
    host->FlushSync();
    const auto cacheMatchesArrange = [&]()
    {
        for ( size_t i = 0; i < slots.size(); ++i )
        {
            if ( !buttons[i].IsLoaded() ||
                 containers[i].ActualWidth() <= 0 ||
                 containers[i].ActualHeight() <= 0 ||
                 slots[i]->GetRectInTLW() !=
                     ArrangedCompositeRect(*host, containers[i]) )
            {
                return false;
            }
        }
        return true;
    };
    REQUIRE(WaitForCompositeGeometry(cacheMatchesArrange));
    const std::array<wxRect, 2> original = {
        slots[0]->GetRectInTLW(), slots[1]->GetRectInTLW()
    };
    REQUIRE_FALSE(original[0].Intersects(original[1]));
    REQUIRE(original[0].y + original[0].height < original[1].y);
    for ( size_t i = 0; i < slots.size(); ++i )
    {
        CHECK(original[i].width == owner->GetClientSize().x);
        CHECK(containers[i].ActualHeight() == Approx(heights[i]));
        CHECK(original[i].width > windows[i]->GetSize().x);
        CHECK(original[i].height > windows[i]->GetSize().y);
        REQUIRE(CompositeBranchHasHit(*host, buttons[i], original[i]));
    }

    SECTION("parent resize propagates through native layout and its observer")
    {
        // The frame's initial layout fills its client area with this sole
        // child. Grow the size actually published after Show(), not the
        // smaller size requested before the frame was realized.
        const wxSize sizeBefore = owner->GetClientSize();
        const wxSize requestedSize(sizeBefore.x + 100, sizeBefore.y + 20);
        owner->SetClientSize(requestedSize);
        // No manual MarkDirty/FlushSync after the mutation: native layout and
        // the composite observer must publish the new arranged rectangles.
        const bool geometryUpdated = WaitForCompositeGeometry([&]()
        {
            return cacheMatchesArrange() &&
                   slots[0]->GetRectInTLW().width > original[0].width &&
                   slots[1]->GetRectInTLW().width > original[1].width;
        });
        const wxSize sizeAfter = owner->GetClientSize();
        const std::array<wxRect, 2> arranged = {
            ArrangedCompositeRect(*host, containers[0]),
            ArrangedCompositeRect(*host, containers[1])
        };
        CAPTURE(sizeBefore.x, sizeBefore.y, requestedSize.x, requestedSize.y,
                sizeAfter.x, sizeAfter.y, original[0].width, original[1].width,
                slots[0]->GetRectInTLW().width,
                slots[1]->GetRectInTLW().width,
                arranged[0].width, arranged[1].width);
        REQUIRE(geometryUpdated);
        for ( size_t i = 0; i < slots.size(); ++i )
        {
            const wxRect rect = slots[i]->GetRectInTLW();
            CHECK(rect.width == owner->GetClientSize().x);
            CHECK(rect.height == original[i].height);
            CHECK(rect.GetPosition() == original[i].GetPosition());
            CHECK(mounts[i].Content().try_as<MUXC::Grid>() == containers[i]);
            CHECK(windows[i]->GetRect() == wxRect(3, 3, 8, 8));
            CHECK(CompositeBranchHasHit(*host, buttons[i], rect));
        }
        CHECK_FALSE(slots[0]->GetRectInTLW().Intersects(
            slots[1]->GetRectInTLW()));
    }

    SECTION("native group collapse and show reflow without hiding wx shells")
    {
        mounts[0].Visibility(MUX::Visibility::Collapsed);
        REQUIRE(WaitForCompositeGeometry([&]()
        {
            return slots[1]->GetRectInTLW() ==
                       ArrangedCompositeRect(*host, containers[1]) &&
                   slots[1]->GetRectInTLW().y < original[1].y &&
                   !CompositeBranchHasHit(*host, buttons[0], original[0]);
        }));
        CHECK(slots[1]->GetRectInTLW().y == original[0].y);
        CHECK(slots[1]->GetRectInTLW().GetSize() == original[1].GetSize());
        CHECK(windows[0]->IsShownOnScreen());
        CHECK(windows[1]->IsShownOnScreen());
        CHECK(CompositeBranchHasHit(*host, buttons[1],
                                   slots[1]->GetRectInTLW()));

        mounts[0].Visibility(MUX::Visibility::Visible);
        REQUIRE(WaitForCompositeGeometry([&]()
        {
            return cacheMatchesArrange() &&
                   slots[0]->GetRectInTLW() == original[0] &&
                   slots[1]->GetRectInTLW() == original[1] &&
                   CompositeBranchHasHit(*host, buttons[0], original[0]);
        }));
        for ( size_t i = 0; i < slots.size(); ++i )
            CHECK(mounts[i].Content().try_as<MUXC::Grid>() == containers[i]);
    }

    SECTION("explicit empty clip removes native hits but preserves arrange")
    {
        host->SetSlotClipRect(windows[0], wxRect(0, 0, 0, 0));
        REQUIRE(WaitForCompositeGeometry([&]()
        {
            const auto clip =
                containers[0].Clip().try_as<MUXM::RectangleGeometry>();
            return clip && clip.Rect().Width == 0 && clip.Rect().Height == 0 &&
                   !CompositeBranchHasHit(*host, buttons[0], original[0]);
        }));
        CHECK(containers[0].Visibility() == MUX::Visibility::Visible);
        CHECK(cacheMatchesArrange());
        CHECK(slots[0]->GetRectInTLW() == original[0]);
        CHECK(slots[1]->GetRectInTLW() == original[1]);
        CHECK(CompositeBranchHasHit(*host, buttons[1], original[1]));

        host->ClearSlotClip(windows[0]);
        REQUIRE(WaitForCompositeGeometry([&]()
        {
            return !containers[0].Clip() && cacheMatchesArrange() &&
                   CompositeBranchHasHit(*host, buttons[0], original[0]);
        }));
        CHECK(slots[0]->GetRectInTLW() == original[0]);
        CHECK(slots[1]->GetRectInTLW() == original[1]);
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3
