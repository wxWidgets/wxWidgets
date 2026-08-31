/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/visualcoordinates.cpp
// Purpose:     Fractional, identity-checked physical-pixel/XAML-DIP mapping
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

// Clear conflicting Win32 macros before the WinRT projection headers.
#include "private.h"
#include "visualcoordinates.h"

#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/msw/private.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>

// Is element part of root's current visual subtree?  Preferred focus targets
// are accepted and used only while this remains true: a failed content swap
// must not leave a weak reference to a detached replacement tree.
bool wxWinUIElementIsInSubtree(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& root)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    try
    {
        if ( !element || !root )
            return false;

        const auto rootDO = root.as<MUX::DependencyObject>();
        auto node = element.as<MUX::DependencyObject>();
        while ( node )
        {
            if ( node == rootDO )
                return true;

            // Parent() covers the logical tree once WinUI has materialized
            // it; templated/visual-only descendants require
            // VisualTreeHelper instead.
            MUX::DependencyObject parent{ nullptr };
            if ( const auto framework =
                     node.try_as<MUX::FrameworkElement>() )
            {
                parent = framework.Parent();
            }
            if ( !parent )
                parent = MUX::Media::VisualTreeHelper::GetParent(node);
            node = parent;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // A candidate assembled in memory has no parent chain until it is placed
    // below a live XamlRoot: FrameworkElement::Parent() and
    // VisualTreeHelper::GetParent() both return null for e.g. a TextBlock
    // already present in Border::Child. Walk the logical content collections
    // downwards as the detached-tree fallback. These are the primitive
    // composition shapes accepted by the shared host (Panel, Border and
    // ContentControl); once realized, the upward path above remains the
    // authoritative route for template descendants.
    try
    {
        std::function<bool (const MUX::UIElement&, unsigned)> contains;
        contains =
            [&](const MUX::UIElement& candidate, unsigned depth)
            {
                if ( !candidate || depth > 256 )
                    return false;
                if ( candidate == element )
                    return true;

                if ( const auto panel = candidate.try_as<MUXC::Panel>() )
                {
                    const auto children = panel.Children();
                    for ( uint32_t i = 0; i < children.Size(); ++i )
                    {
                        if ( contains(children.GetAt(i), depth + 1) )
                            return true;
                    }
                }
                else if ( const auto border =
                              candidate.try_as<MUXC::Border>() )
                {
                    if ( contains(border.Child(), depth + 1) )
                        return true;
                }
                else if ( const auto contentControl =
                              candidate.try_as<MUXC::ContentControl>() )
                {
                    if ( contains(
                             contentControl.Content().try_as<MUX::UIElement>(),
                             depth + 1) )
                    {
                        return true;
                    }
                }

                return false;
            };

        return contains(root, 0);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

// ============================================================================
// fractional, identity-checked coordinate mapping
// ============================================================================

struct wxWinUIVisualCoordinates::Snapshot
{
    wxWeakRef<wxWindow> anchor;
    wxWeakRef<wxWindow> tlw;
    wxWindow *anchorIdentity = nullptr;
    wxWindow *tlwIdentity = nullptr;
    wxWinUITopLevelHost *host = nullptr;
    wxWinUISlot *slot = nullptr;
    std::shared_ptr<wxWinUIHostLifetime> hostLifetime;
    std::shared_ptr<wxWinUISlotLifetime> slotLifetime;
    winrt::Microsoft::UI::Xaml::Controls::Canvas root{ nullptr };
    winrt::Microsoft::UI::Xaml::XamlRoot xamlRoot{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement content{ nullptr };
    winrt::Windows::Foundation::Rect rootScreenPixels{};
    winrt::Windows::Foundation::Rect anchorScreenPixels{};
    HWND anchorHwnd = nullptr;
    HWND tlwHwnd = nullptr;
    unsigned long long anchorHwndGeneration = 0;
    unsigned long long tlwHwndGeneration = 0;
    unsigned long long contentGeneration = 0;
    unsigned long long syncEpoch = 0;
    unsigned long long structureGeneration = 0;
    double scale = 0.0;
    bool anchorMirrored = false;
};

wxWinUITopLevelHost *
wxWinUIVisualCoordinates::ResolveHost(wxWindow *anchor)
{
    if ( !anchor )
        return nullptr;

    if ( wxWinUITopLevelHost * const owner =
             wxWinUITopLevelHost::FindSlotOwner(anchor) )
    {
        return owner;
    }

    wxWindow * const tlw =
        anchor->IsTopLevel() ? anchor : wxGetTopLevelParent(anchor);
    return tlw ? wxWinUITopLevelHost::FindForTLW(tlw) : nullptr;
}

bool wxWinUIVisualCoordinates::TryGetPhysicalClientRect(
    wxWindow *window,
    winrt::Windows::Foundation::Rect *screenPixels,
    bool *mirrored)
{
    if ( !window || !screenPixels || !mirrored )
        return false;

    const HWND hwnd = GetHwndOf(window);
    if ( !hwnd )
        return false;

    RECT rect{};
    if ( !::GetClientRect(hwnd, &rect) )
        return false;

    ::SetLastError(ERROR_SUCCESS);
    const int mapped = ::MapWindowPoints(
        hwnd, HWND_DESKTOP, reinterpret_cast<POINT *>(&rect), 2);
    if ( mapped == 0 && ::GetLastError() != ERROR_SUCCESS )
        return false;

    const LONG left = (std::min)(rect.left, rect.right);
    const LONG right = (std::max)(rect.left, rect.right);
    const LONG top = (std::min)(rect.top, rect.bottom);
    const LONG bottom = (std::max)(rect.top, rect.bottom);

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR exStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ( exStyle == 0 && ::GetLastError() != ERROR_SUCCESS )
        return false;

    *screenPixels = {
        static_cast<float>(left),
        static_cast<float>(top),
        static_cast<float>(right - left),
        static_cast<float>(bottom - top)
    };
    *mirrored = (exStyle & WS_EX_LAYOUTRTL) != 0;
    return true;
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::Capture(
    wxWindow *anchor,
    wxWinUITopLevelHost *host,
    wxWinUISlot *slot,
    bool requireSlot,
    Snapshot *snapshot)
{
    if ( !anchor || !host || !snapshot || (requireSlot && !slot) )
        return wxWinUICoordinateResult::Unavailable;

    Snapshot current;
    current.anchor = anchor;
    current.anchorIdentity = anchor;
    current.host = host;
    current.slot = slot;
    current.hostLifetime = host->m_hostLifetime;
    current.tlwIdentity = host->m_tlw;
    current.tlw = current.tlwIdentity;
    current.root = host->m_root;
    current.structureGeneration = host->m_structureGeneration;

    if ( !current.hostLifetime ||
         current.hostLifetime->GetHost() != host ||
         host->m_shuttingDown || !current.tlwIdentity || !current.root )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    current.anchorHwnd = GetHwndOf(anchor);
    current.tlwHwnd = GetHwndOf(current.tlwIdentity);
    if ( !current.anchorHwnd || !current.tlwHwnd )
        return wxWinUICoordinateResult::Unavailable;

    current.anchorHwndGeneration =
        wxWinUIMSWGetHwndGeneration(
            anchor, reinterpret_cast<WXHWND>(current.anchorHwnd));
    current.tlwHwndGeneration =
        wxWinUIMSWGetHwndGeneration(
            current.tlwIdentity,
            reinterpret_cast<WXHWND>(current.tlwHwnd));
    if ( !current.anchorHwndGeneration || !current.tlwHwndGeneration )
        return wxWinUICoordinateResult::Unavailable;

    if ( slot )
    {
        current.slotLifetime = slot->m_lifetime;
        current.content = slot->m_content;
        current.contentGeneration = slot->m_contentGeneration;
        current.syncEpoch = slot->m_syncEpoch;
        if ( !current.slotLifetime ||
             current.slotLifetime->GetHost() != host ||
             current.slotLifetime->GetWindow() != anchor ||
             (requireSlot && !current.content) )
        {
            return wxWinUICoordinateResult::Unavailable;
        }
    }

    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        current.xamlRoot = current.root.XamlRoot();
        if ( !current.xamlRoot || !IsCurrent(current) )
            return wxWinUICoordinateResult::Superseded;

        current.scale = current.xamlRoot.RasterizationScale();
        if ( !(current.scale > 0.0) || !std::isfinite(current.scale) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(current) )
            return wxWinUICoordinateResult::Superseded;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }

    bool rootMirrored = false;
    if ( !TryGetPhysicalClientRect(
             current.tlwIdentity,
             &current.rootScreenPixels,
             &rootMirrored) )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    wxUnusedVar(rootMirrored); // XAML root space is always physically LTR.
    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    if ( !TryGetPhysicalClientRect(
             anchor,
             &current.anchorScreenPixels,
             &current.anchorMirrored) )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    *snapshot = current;
    return wxWinUICoordinateResult::Mapped;
}

bool wxWinUIVisualCoordinates::IsCurrent(const Snapshot& snapshot)
{
    wxWinUITopLevelHost * const host = snapshot.host;
    const auto baseIsCurrent = [&]() -> bool
    {
        wxWindow * const anchor = snapshot.anchor.get();
        wxWindow * const tlw = snapshot.tlw.get();
        if ( !anchor || anchor != snapshot.anchorIdentity ||
             !tlw || tlw != snapshot.tlwIdentity ||
             !host || !snapshot.hostLifetime ||
             snapshot.hostLifetime->GetHost() != host ||
             host->m_shuttingDown || host->m_tlw != tlw ||
             host->m_root != snapshot.root ||
             host->m_structureGeneration !=
                 snapshot.structureGeneration ||
             GetHwndOf(anchor) != snapshot.anchorHwnd ||
             GetHwndOf(tlw) != snapshot.tlwHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 anchor,
                 reinterpret_cast<WXHWND>(snapshot.anchorHwnd)) !=
                 snapshot.anchorHwndGeneration ||
             wxWinUIMSWGetHwndGeneration(
                 tlw,
                 reinterpret_cast<WXHWND>(snapshot.tlwHwnd)) !=
                 snapshot.tlwHwndGeneration )
        {
            return false;
        }

        if ( snapshot.slot )
        {
            if ( host->FindSlot(anchor) != snapshot.slot ||
                 snapshot.slot->m_lifetime != snapshot.slotLifetime ||
                 !snapshot.slotLifetime ||
                 snapshot.slotLifetime->GetHost() != host ||
                 snapshot.slotLifetime->GetWindow() != anchor ||
                 snapshot.slot->m_content != snapshot.content ||
                 snapshot.slot->m_contentGeneration !=
                     snapshot.contentGeneration ||
                 snapshot.slot->m_syncEpoch != snapshot.syncEpoch )
            {
                return false;
            }
        }
        else if ( host->FindSlot(anchor) )
        {
            return false;
        }

        return wxGetTopLevelParent(anchor) == tlw ||
               (anchor->IsTopLevel() && anchor == tlw);
    };

    if ( !baseIsCurrent() )
        return false;

    // RasterizationScale and XamlRoot can change without a wx geometry event
    // while a window crosses monitors. When they have already been captured,
    // validate them as part of every publication ticket too.
    if ( snapshot.xamlRoot )
    {
        try
        {
            if ( snapshot.root.XamlRoot() != snapshot.xamlRoot )
                return false;
            if ( snapshot.scale > 0.0 )
            {
                const double currentScale =
                    snapshot.xamlRoot.RasterizationScale();
                if ( !std::isfinite(currentScale) ||
                     std::abs(currentScale - snapshot.scale) >
                         0.0000001 )
                {
                    return false;
                }
            }
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
    }

    // XamlRoot/RasterizationScale access is a callback boundary too.
    return baseIsCurrent();
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ClientToScreen(
    const Snapshot& snapshot,
    const Point& clientPixels)
{
    const Rect& rect = snapshot.anchorScreenPixels;
    return {
        snapshot.anchorMirrored
            ? rect.X + rect.Width - clientPixels.X
            : rect.X + clientPixels.X,
        rect.Y + clientPixels.Y
    };
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ScreenToClient(
    const Snapshot& snapshot,
    const Point& screenPixels)
{
    const Rect& rect = snapshot.anchorScreenPixels;
    return {
        snapshot.anchorMirrored
            ? rect.X + rect.Width - screenPixels.X
            : screenPixels.X - rect.X,
        screenPixels.Y - rect.Y
    };
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ScreenToRoot(
    const Snapshot& snapshot,
    const Point& screenPixels)
{
    return {
        static_cast<float>(
            (screenPixels.X - snapshot.rootScreenPixels.X) /
            snapshot.scale),
        static_cast<float>(
            (screenPixels.Y - snapshot.rootScreenPixels.Y) /
            snapshot.scale)
    };
}

bool wxWinUIIsFinitePoint(
    const winrt::Windows::Foundation::Point& point)
{
    return std::isfinite(point.X) && std::isfinite(point.Y);
}

namespace
{

bool wxWinUIIsFiniteRect(
    const winrt::Windows::Foundation::Rect& rect)
{
    return std::isfinite(rect.X) && std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) && std::isfinite(rect.Height);
}

template<typename Attempt>
wxWinUICoordinateResult wxWinUIRunCoordinateAttempts(Attempt&& attempt)
{
    wxWinUICoordinateResult result =
        wxWinUICoordinateResult::Unavailable;
    for ( unsigned pass = 0; pass < 2; ++pass )
    {
        result = attempt();
        if ( result != wxWinUICoordinateResult::Superseded )
            break;
    }
    return result;
}

} // anonymous namespace

wxWinUICoordinateResult wxWinUIVisualCoordinates::ClientPointToRoot(
    wxWindow *anchor,
    const Point& clientPixels,
    Point *rootDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ClientPointToRootOnce(
                anchor, clientPixels, rootDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ScreenPointToRoot(
    wxWindow *anchor,
    const Point& screenPixels,
    Point *rootDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ScreenPointToRootOnce(
                anchor, screenPixels, rootDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ClientPointToElement(
    wxWindow *anchor,
    const Point& clientPixels,
    const UIElement& element,
    Point *elementDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ClientPointToElementOnce(
                anchor, clientPixels, element, elementDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ElementPointToClient(
    wxWindow *anchor,
    const UIElement& element,
    const Point& elementDips,
    Point *clientPixels)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ElementPointToClientOnce(
                anchor, element, elementDips, clientPixels);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ElementBoundsToClient(
    wxWindow *anchor,
    const UIElement& element,
    const Rect& elementBoundsDips,
    Rect *clientBoundsPixels)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ElementBoundsToClientOnce(
                anchor, element, elementBoundsDips,
                clientBoundsPixels);
        });
}

bool wxWinUIVisualCoordinates::TestRoundTripPhysicalSpaces(
    const Rect& rootScreenPixels,
    const Rect& anchorScreenPixels,
    bool anchorMirrored,
    double scale,
    const Point& clientPixels,
    Point *rootDips,
    Point *roundTrippedClientPixels)
{
    if ( !rootDips || !roundTrippedClientPixels ||
         !wxWinUIIsFiniteRect(rootScreenPixels) ||
         !wxWinUIIsFiniteRect(anchorScreenPixels) ||
         !wxWinUIIsFinitePoint(clientPixels) ||
         !(scale > 0.0) || !std::isfinite(scale) )
    {
        return false;
    }

    Snapshot snapshot;
    snapshot.rootScreenPixels = rootScreenPixels;
    snapshot.anchorScreenPixels = anchorScreenPixels;
    snapshot.anchorMirrored = anchorMirrored;
    snapshot.scale = scale;

    const Point screen = ClientToScreen(snapshot, clientPixels);
    const Point root = ScreenToRoot(snapshot, screen);
    const Point screenAgain{
        rootScreenPixels.X +
            static_cast<float>(root.X * scale),
        rootScreenPixels.Y +
            static_cast<float>(root.Y * scale)
    };
    const Point roundTrip = ScreenToClient(snapshot, screenAgain);
    if ( !wxWinUIIsFinitePoint(root) ||
         !wxWinUIIsFinitePoint(roundTrip) )
    {
        return false;
    }

    *rootDips = root;
    *roundTrippedClientPixels = roundTrip;
    return true;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ClientPointToRootOnce(
    wxWindow *anchor,
    const Point& clientPixels,
    Point *rootDips)
{
    if ( !rootDips || !wxWinUIIsFinitePoint(clientPixels) )
        return wxWinUICoordinateResult::Unavailable;

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, false, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;

    const Point mapped =
        ScreenToRoot(snapshot, ClientToScreen(snapshot, clientPixels));
    if ( !wxWinUIIsFinitePoint(mapped) )
        return wxWinUICoordinateResult::Unavailable;
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    *rootDips = mapped;
    return wxWinUICoordinateResult::Mapped;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ScreenPointToRootOnce(
    wxWindow *anchor,
    const Point& screenPixels,
    Point *rootDips)
{
    if ( !rootDips || !wxWinUIIsFinitePoint(screenPixels) )
        return wxWinUICoordinateResult::Unavailable;

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, false, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;

    const Point mapped = ScreenToRoot(snapshot, screenPixels);
    if ( !wxWinUIIsFinitePoint(mapped) )
        return wxWinUICoordinateResult::Unavailable;
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    *rootDips = mapped;
    return wxWinUICoordinateResult::Mapped;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ClientPointToElementOnce(
    wxWindow *anchor,
    const Point& clientPixels,
    const UIElement& element,
    Point *elementDips)
{
    if ( !elementDips || !element ||
         !wxWinUIIsFinitePoint(clientPixels) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const Point rootPoint =
            ScreenToRoot(snapshot, ClientToScreen(snapshot, clientPixels));
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const auto rootToElement = elementToRoot.Inverse();
        if ( !rootToElement )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Point mapped = rootToElement.TransformPoint(rootPoint);
        if ( !wxWinUIIsFinitePoint(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *elementDips = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ElementPointToClientOnce(
    wxWindow *anchor,
    const UIElement& element,
    const Point& elementDips,
    Point *clientPixels)
{
    if ( !clientPixels || !element ||
         !wxWinUIIsFinitePoint(elementDips) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Point rootPoint = elementToRoot.TransformPoint(elementDips);
        if ( !wxWinUIIsFinitePoint(rootPoint) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        const Point screenPoint{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootPoint.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootPoint.Y * snapshot.scale)
        };
        const Point mapped = ScreenToClient(snapshot, screenPoint);
        if ( !wxWinUIIsFinitePoint(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *clientPixels = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ElementBoundsToClientOnce(
    wxWindow *anchor,
    const UIElement& element,
    const Rect& elementBoundsDips,
    Rect *clientBoundsPixels)
{
    if ( !clientBoundsPixels || !element ||
         !wxWinUIIsFiniteRect(elementBoundsDips) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Rect rootBounds =
            elementToRoot.TransformBounds(elementBoundsDips);
        if ( !wxWinUIIsFiniteRect(rootBounds) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        const Point rootTopLeft{rootBounds.X, rootBounds.Y};
        const Point rootBottomRight{
            rootBounds.X + rootBounds.Width,
            rootBounds.Y + rootBounds.Height
        };
        const Point screenTopLeft{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootTopLeft.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootTopLeft.Y * snapshot.scale)
        };
        const Point screenBottomRight{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootBottomRight.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootBottomRight.Y * snapshot.scale)
        };
        const Point clientA =
            ScreenToClient(snapshot, screenTopLeft);
        const Point clientB =
            ScreenToClient(snapshot, screenBottomRight);

        const float left = (std::min)(clientA.X, clientB.X);
        const float top = (std::min)(clientA.Y, clientB.Y);
        const float right = (std::max)(clientA.X, clientB.X);
        const float bottom = (std::max)(clientA.Y, clientB.Y);
        const Rect mapped{left, top, right - left, bottom - top};
        if ( !wxWinUIIsFiniteRect(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *clientBoundsPixels = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

#endif // wxUSE_WINUI3
