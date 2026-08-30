/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/notebook.cpp
// Purpose:     wxWinUI wxNotebook implementation (WinUI TabView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/log.h"
#endif

#include "private.h"
#ifdef WXWINUI_TEST_SUPPORT
    #include "notebook-test-access.h"
    #include "runtime-test-access.h"
#endif
#include "wx/imaglist.h"
#include "wx/private/windowlifetime.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Dispatching.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace WF = winrt::Windows::Foundation;
namespace WS = winrt::Windows::System;

namespace
{
constexpr int DefaultTabStripHeightDIP = 40;
constexpr double DefaultHeaderSpacingDIP = 6.0;

// A visual reached as DependencyObject, UIElement or one of its concrete
// control interfaces can expose a different ABI interface pointer for the
// same WinRT instance. QI every comparison operand to the controlling
// IUnknown before deciding exact visual ancestry/ownership.
template<typename T>
WF::IUnknown wxWinUINotebookObjectIdentity(const T& object) noexcept
{
    return object ? object.template try_as<WF::IUnknown>() : nullptr;
}

enum wxWinUINotebookTabPart
{
    wxWinUINotebookTabPart_Item,
    wxWinUINotebookTabPart_Icon,
    wxWinUINotebookTabPart_Label
};

struct wxWinUINotebookTabMetric
{
    double width = 0.0;
    double height = 0.0;
    double advance = 0.0;
    double leadingLTR = 0.0;
    double leadingRTL = 0.0;
    double iconWidth = 0.0;
    double iconHeight = 0.0;
    double labelWidth = 0.0;
    double labelHeight = 0.0;
};

// The stock TabView has a single horizontal strip. Extended wxMSW notebook
// styles use a second presentation of the same logical item model inside the
// same shared slot: standalone TabViewItem peers retain tab semantics/UIA,
// while a Canvas gives exact physical-LTR placement on every side and
// deterministic multiline wrapping. No additional island or HWND is created.
struct wxWinUINotebookSurfacePeer
{
    MUXC::TabViewItem logicalItem{ nullptr };
    MUXC::TabViewItem item{ nullptr };
    MUXC::StackPanel header{ nullptr };
    MUXC::Image icon{ nullptr };
    MUXC::TextBlock label{ nullptr };
    int64_t isSelectedCallbackToken = 0;
    winrt::event_token tappedToken{};
    winrt::event_token accessKeyInvokedToken{};
    winrt::event_token previewKeyDownToken{};
};

template<typename Remove>
void wxWinUIRevokeNotebookToken(
    winrt::event_token& token,
    Remove&& remove) noexcept
{
    if ( !token.value )
        return;

    // Publish the token as gone before crossing the ABI: a routed-event
    // source already in teardown may report ERROR_NOT_FOUND, which means the
    // desired detached state has already won and must not escape this path.
    const winrt::event_token current = token;
    token = {};
    try
    {
        remove(current);
    }
    catch ( ... )
    {
    }
}

void wxWinUIRevokeNotebookSurfacePeerHandlers(
    wxWinUINotebookSurfacePeer& peer) noexcept
{
    if ( peer.isSelectedCallbackToken )
    {
        const int64_t token = peer.isSelectedCallbackToken;
        peer.isSelectedCallbackToken = 0;
        try
        {
            if ( peer.item )
            {
                peer.item.UnregisterPropertyChangedCallback(
                    MUXCP::SelectorItem::IsSelectedProperty(), token);
            }
        }
        catch ( ... )
        {
        }
    }

    wxWinUIRevokeNotebookToken(
        peer.tappedToken,
        [&peer](winrt::event_token token)
        {
            if ( peer.item )
                peer.item.Tapped(token);
        });
    wxWinUIRevokeNotebookToken(
        peer.accessKeyInvokedToken,
        [&peer](winrt::event_token token)
        {
            if ( peer.item )
                peer.item.AccessKeyInvoked(token);
        });
    wxWinUIRevokeNotebookToken(
        peer.previewKeyDownToken,
        [&peer](winrt::event_token token)
        {
            if ( peer.item )
                peer.item.PreviewKeyDown(token);
        });
}

void wxWinUIRevokeNotebookButtonHandler(
    MUXC::Button& button,
    winrt::event_token& token) noexcept
{
    wxWinUIRevokeNotebookToken(
        token,
        [&button](winrt::event_token current)
        {
            if ( button )
                button.Click(current);
        });
}

enum class wxWinUINotebookFocusTarget
{
    None,
    Notebook,
    Page
};

#ifdef WXWINUI_TEST_SUPPORT
std::atomic<std::size_t> gs_liveNotebookCallbackStates{0};
std::atomic<std::size_t> gs_pendingNotebookPeerRetirements{0};
#endif
std::atomic<std::uint64_t> gs_nextNotebookCallbackGeneration{0};
#ifdef WXWINUI_TEST_SUPPORT
std::atomic<wxWinUINotebookTestAccess::PeerRetirementQueueFault>
    gs_notebookPeerRetirementQueueFault{
        wxWinUINotebookTestAccess::PeerRetirementQueueFault::None};
#endif

} // anonymous namespace

// XAML owns its delegates and may keep a queued copy alive while Close() is
// revoking the registered token. Delegates therefore retain this state, never
// a raw wxNotebook. The generation makes stale work from a previous lifetime
// distinguishable even if an allocator later reuses the same wx address.
class wxWinUINotebookCallbackState final
{
public:
    explicit wxWinUINotebookCallbackState(wxNotebook *owner)
        : m_owner(owner),
          m_generation(
              gs_nextNotebookCallbackGeneration.fetch_add(
                  1, std::memory_order_relaxed) + 1)
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveNotebookCallbackStates.fetch_add(
            1, std::memory_order_relaxed);
#endif
    }

    ~wxWinUINotebookCallbackState()
    {
        Invalidate();
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveNotebookCallbackStates.fetch_sub(
            1, std::memory_order_relaxed);
#endif
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxNotebook *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;

        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
        m_operation.fetch_add(1, std::memory_order_acq_rel);
    }

    std::uint64_t BeginOperation()
    {
        return m_operation.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    bool IsCurrentOperation(std::uint64_t operation) const
    {
        return operation == m_operation.load(std::memory_order_acquire);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndPeerMutation()
    {
        const unsigned previous =
            m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
        wxASSERT_MSG(previous != 0,
                     "unbalanced WinUI notebook peer mutation");
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

    wxWinUINotebookFocusTarget ExchangeFocusTarget(
        wxWinUINotebookFocusTarget target)
    {
        return m_focusTarget.exchange(target, std::memory_order_acq_rel);
    }

    void SetFocusTarget(wxWinUINotebookFocusTarget target)
    {
        m_focusTarget.store(target, std::memory_order_release);
    }

    wxWinUINotebookFocusTarget GetFocusTarget() const
    {
        return m_focusTarget.load(std::memory_order_acquire);
    }

    bool TryBeginGeometryRealization()
    {
        bool expected = false;
        return m_geometryRealization.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    void EndGeometryRealization()
    {
        m_geometryRealization.store(false, std::memory_order_release);
    }

    bool IsGeometryRealizationInProgress() const
    {
        return m_geometryRealization.load(std::memory_order_acquire);
    }

private:
    std::atomic<wxNotebook *> m_owner;
    std::atomic<std::uint64_t> m_generation;
    std::atomic<std::uint64_t> m_operation{0};
    std::atomic<unsigned> m_peerMutationDepth{0};
    std::atomic<wxWinUINotebookFocusTarget> m_focusTarget{
        wxWinUINotebookFocusTarget::None};
    std::atomic<bool> m_geometryRealization{false};
};

namespace
{

class wxWinUINotebookPeerMutationGuard final
{
public:
    explicit wxWinUINotebookPeerMutationGuard(
        const std::shared_ptr<wxWinUINotebookCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUINotebookPeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

    wxWinUINotebookPeerMutationGuard(
        const wxWinUINotebookPeerMutationGuard&) = delete;
    wxWinUINotebookPeerMutationGuard& operator=(
        const wxWinUINotebookPeerMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUINotebookCallbackState> m_state;
};

class wxWinUINotebookFocusGuard final
{
public:
    wxWinUINotebookFocusGuard(
        const std::shared_ptr<wxWinUINotebookCallbackState>& state,
        wxWinUINotebookFocusTarget target)
        : m_state(state),
          m_previous(
              state
                  ? state->ExchangeFocusTarget(target)
                  : wxWinUINotebookFocusTarget::None)
    {
    }

    ~wxWinUINotebookFocusGuard()
    {
        // The state, unlike the wxNotebook, is guaranteed to remain alive
        // until this stack frame is gone.
        if ( m_state )
            m_state->SetFocusTarget(m_previous);
    }

    wxWinUINotebookFocusGuard(const wxWinUINotebookFocusGuard&) = delete;
    wxWinUINotebookFocusGuard& operator=(
        const wxWinUINotebookFocusGuard&) = delete;

private:
    std::shared_ptr<wxWinUINotebookCallbackState> m_state;
    wxWinUINotebookFocusTarget m_previous;
};

class wxWinUINotebookGeometryGuard final
{
public:
    explicit wxWinUINotebookGeometryGuard(
        const std::shared_ptr<wxWinUINotebookCallbackState>& state)
        : m_state(state),
          m_acquired(state && state->TryBeginGeometryRealization())
    {
    }

    ~wxWinUINotebookGeometryGuard()
    {
        if ( m_acquired )
            m_state->EndGeometryRealization();
    }

    bool IsAcquired() const
    {
        return m_acquired;
    }

    wxWinUINotebookGeometryGuard(
        const wxWinUINotebookGeometryGuard&) = delete;
    wxWinUINotebookGeometryGuard& operator=(
        const wxWinUINotebookGeometryGuard&) = delete;

private:
    std::shared_ptr<wxWinUINotebookCallbackState> m_state;
    bool m_acquired;
};

// Removing a notebook slot invalidates all wx-facing callbacks immediately,
// but XAML can still have layout/template work queued for the just-detached
// tree. Releasing that tree in the same stack has been observed to raise a
// stowed ERROR_NOT_FOUND exception in Microsoft.UI.Xaml.dll when the next
// notebook is created. Keep the complete peer graph coherent for two low
// dispatcher turns: the first drains work already queued by detachment and
// the second drains any layout work produced by that first turn.
class wxWinUINotebookPeerRetirement final
{
public:
    wxWinUINotebookPeerRetirement()
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_pendingNotebookPeerRetirements.fetch_add(
            1, std::memory_order_relaxed);
#endif
    }

    ~wxWinUINotebookPeerRetirement()
    {
        Complete();
    }

    void RequestComplete() noexcept
    {
        completionRequested.store(true, std::memory_order_release);
        if ( completionArmed.load(std::memory_order_acquire) )
            Complete();
    }

    void ArmForCompletion() noexcept
    {
        completionArmed.store(true, std::memory_order_release);
        if ( completionRequested.load(std::memory_order_acquire) )
            Complete();
    }

    void Complete() noexcept
    {
        bool expected = false;
        if ( !completed.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            return;
        }

        // Release external leaf references before the roots which still own
        // those leaves. The callback state is last so every delegate released
        // with the tree continues to resolve only an invalidated owner.
        for ( auto& peer : extendedPeers )
            wxWinUIRevokeNotebookSurfacePeerHandlers(peer);
        wxWinUIRevokeNotebookButtonHandler(
            extendedBackButton, extendedBackClickToken);
        wxWinUIRevokeNotebookButtonHandler(
            extendedForwardButton, extendedForwardClickToken);
        extendedPeers.clear();
        items.clear();
        headers.clear();
        icons.clear();
        labels.clear();
        extendedForwardButton = nullptr;
        extendedBackButton = nullptr;
        extendedTabCanvas = nullptr;
        extendedPageChrome = nullptr;
        extendedRoot = nullptr;
        tabView = nullptr;
        callbackState.reset();

#ifdef WXWINUI_TEST_SUPPORT
        gs_pendingNotebookPeerRetirements.fetch_sub(
            1, std::memory_order_relaxed);
#endif
    }

    std::shared_ptr<wxWinUINotebookCallbackState> callbackState;
    MUXC::TabView tabView{ nullptr };
    MUXC::Canvas extendedRoot{ nullptr };
    MUXC::Canvas extendedTabCanvas{ nullptr };
    MUXC::Border extendedPageChrome{ nullptr };
    MUXC::Button extendedBackButton{ nullptr };
    MUXC::Button extendedForwardButton{ nullptr };
    winrt::event_token extendedBackClickToken{};
    winrt::event_token extendedForwardClickToken{};
    std::vector<wxWinUINotebookSurfacePeer> extendedPeers;
    std::vector<MUXC::TabViewItem> items;
    std::vector<MUXC::StackPanel> headers;
    std::vector<MUXC::Image> icons;
    std::vector<MUXC::TextBlock> labels;

private:
    std::atomic<bool> completionArmed{false};
    std::atomic<bool> completionRequested{false};
    std::atomic<bool> completed{false};
};

template<typename Handler>
bool wxWinUITryEnqueueNotebookPeerRetirement(
    const MUXD::DispatcherQueue& queue,
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUINotebookTestAccess::PeerRetirementQueueFault fault,
#endif
    Handler&& handler) noexcept
{
#ifdef WXWINUI_TEST_SUPPORT
    auto expected = fault;
    if ( gs_notebookPeerRetirementQueueFault.compare_exchange_strong(
             expected,
             wxWinUINotebookTestAccess::PeerRetirementQueueFault::None,
             std::memory_order_acq_rel) )
    {
        return false;
    }
#endif

    try
    {
        return queue &&
               queue.TryEnqueue(
                   MUXD::DispatcherQueuePriority::Low,
                   std::forward<Handler>(handler));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    catch ( const std::exception& )
    {
    }

    return false;
}

void wxWinUIQueueNotebookPeerRetirement(
    const MUXD::DispatcherQueue& queue,
    wxWinUIFrameworkRetirementId retirementId)
    noexcept
{
    if ( !retirementId )
        return;

    if ( wxWinUITryEnqueueNotebookPeerRetirement(
             queue,
#ifdef WXWINUI_TEST_SUPPORT
             wxWinUINotebookTestAccess::PeerRetirementQueueFault::
                 RejectFirstTurn,
#endif
             [queue, retirementId]() noexcept
             {
                 if ( wxWinUITryEnqueueNotebookPeerRetirement(
                          queue,
#ifdef WXWINUI_TEST_SUPPORT
                          wxWinUINotebookTestAccess::PeerRetirementQueueFault::
                              RejectSecondTurn,
#endif
                          [retirementId]() noexcept
                          {
                              wxWinUICompleteFrameworkRetirement(
                                  retirementId);
                          }) )
                 {
                     return;
                 }

                 // Enqueue rejection is not a framework-shutdown barrier.
                 // The ticket remains strongly held by the runtime ledger;
                 // after rundown starts, only the deferrable XAML terminal
                 // boundary may consume it.
             }) )
    {
        return;
    }

    // As above, rejection of the first turn leaves the pre-registered ticket
    // quarantined. Never release the graph in the detach stack merely because
    // TryEnqueue() returned false.
}

template<typename Validator>
bool wxWinUIGetElementBoundsInNotebook(
    const MUX::FrameworkElement& element,
    const MUX::FrameworkElement& relativeTo,
    winrt::Windows::Foundation::Rect *bounds,
    Validator&& isCurrent)
{
    if ( !bounds || !element || !relativeTo || !isCurrent() )
    {
        return false;
    }

    const double width = element.ActualWidth();
    if ( !isCurrent() )
        return false;
    const double height = element.ActualHeight();
    if ( !isCurrent() || !std::isfinite(width) ||
         !std::isfinite(height) || width <= 0.0 || height <= 0.0 )
    {
        return false;
    }

    const auto transform = element.TransformToVisual(relativeTo);
    if ( !isCurrent() || !transform )
        return false;

    *bounds = transform.TransformBounds(
        winrt::Windows::Foundation::Rect{
            0.0f,
            0.0f,
            static_cast<float>(width),
            static_cast<float>(height)});
    return isCurrent() && std::isfinite(bounds->X) &&
           std::isfinite(bounds->Y) && std::isfinite(bounds->Width) &&
           std::isfinite(bounds->Height) &&
           bounds->Width > 0.0f && bounds->Height > 0.0f;
}

wxRect wxWinUIRoundOutClientBounds(
    const winrt::Windows::Foundation::Rect& bounds)
{
    const double left = bounds.X;
    const double top = bounds.Y;
    const double right = bounds.X + bounds.Width;
    const double bottom = bounds.Y + bounds.Height;
    if ( !std::isfinite(left) || !std::isfinite(top) ||
         !std::isfinite(right) || !std::isfinite(bottom) ||
         right <= left || bottom <= top )
    {
        return wxRect();
    }

    const double roundedLeft = std::floor(left);
    const double roundedTop = std::floor(top);
    const double roundedRight = std::ceil(right);
    const double roundedBottom = std::ceil(bottom);
    const double intMin = (std::numeric_limits<int>::min)();
    const double intMax = (std::numeric_limits<int>::max)();
    if ( roundedLeft < intMin || roundedTop < intMin ||
         roundedRight > intMax || roundedBottom > intMax ||
         roundedRight - roundedLeft > intMax ||
         roundedBottom - roundedTop > intMax )
    {
        return wxRect();
    }

    const int clientLeft = static_cast<int>(roundedLeft);
    const int clientTop = static_cast<int>(roundedTop);
    const int clientRight = static_cast<int>(roundedRight);
    const int clientBottom = static_cast<int>(roundedBottom);
    return wxRect(
        clientLeft,
        clientTop,
        clientRight - clientLeft,
        clientBottom - clientTop);
}

struct wxWinUINotebookTemplateSnapshot
{
    MUXC::ListViewBase tabList{ nullptr };
    std::vector<MUXCP::ButtonBase> buttons;
    bool complete = false;
};

template<typename Validator>
wxWinUINotebookTemplateSnapshot wxWinUIInspectNotebookTemplate(
    const MUXC::TabView& tabView,
    Validator&& isCurrent)
{
    wxWinUINotebookTemplateSnapshot snapshot;
    if ( !tabView || !isCurrent() )
        return snapshot;

    // TabView doesn't expose its horizontal ListView publicly. Resolve the
    // template descendant by interface rather than by a private x:Name so
    // this keeps working across Windows App SDK template revisions.
    std::vector<MUX::DependencyObject> pending;
    pending.push_back(tabView);
    constexpr size_t MaxTemplateNodes = 4096;
    for ( size_t index = 0;
          index < pending.size();
          ++index )
    {
        if ( index >= MaxTemplateNodes || !isCurrent() )
            return snapshot;

        const MUX::DependencyObject current = pending[index];
        if ( !snapshot.tabList )
        {
            snapshot.tabList =
                current.try_as<MUXC::ListViewBase>();
        }
        if ( const auto button = current.try_as<MUXCP::ButtonBase>() )
            snapshot.buttons.push_back(button);

        const int count =
            MUX::Media::VisualTreeHelper::GetChildrenCount(current);
        if ( !isCurrent() )
            return snapshot;
        for ( int child = 0;
              child < count;
              ++child )
        {
            if ( pending.size() >= MaxTemplateNodes )
                return snapshot;
            pending.push_back(
                MUX::Media::VisualTreeHelper::GetChild(
                    current, child));
            if ( !isCurrent() )
                return snapshot;
        }
    }

    snapshot.complete = isCurrent();
    return snapshot;
}

template<typename Validator>
wxRect wxWinUIMapNotebookBoundsToClient(
    wxWindow *anchor,
    const MUX::UIElement& element,
    const winrt::Windows::Foundation::Rect& elementBoundsDIPs,
    Validator&& isCurrent)
{
    if ( !anchor || !element || !isCurrent() )
        return wxRect();

    winrt::Windows::Foundation::Rect clientBounds{};
    if ( wxWinUIVisualCoordinates::ElementBoundsToClient(
             anchor, element, elementBoundsDIPs, &clientBounds) !=
             wxWinUICoordinateResult::Mapped ||
         !isCurrent() )
    {
        return wxRect();
    }

    return wxWinUIRoundOutClientBounds(clientBounds);
}

template<typename Validator>
wxRect wxWinUIGetRealizedElementRectInNotebookImpl(
    wxWindow *anchor,
    const MUX::FrameworkElement& element,
    bool respectClip,
    Validator&& isCurrent)
{
    if ( !anchor || !element || !isCurrent() )
        return wxRect();

    const double width = element.ActualWidth();
    if ( !isCurrent() )
        return wxRect();
    const double height = element.ActualHeight();
    if ( !isCurrent() || !std::isfinite(width) ||
         !std::isfinite(height) || width <= 0.0 || height <= 0.0 )
    {
        return wxRect();
    }

    const MUX::UIElement visual = element.as<MUX::UIElement>();
    if ( !isCurrent() )
        return wxRect();

    winrt::Windows::Foundation::Rect localBounds{
        0.0f,
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height)};
    if ( respectClip )
    {
        const MUXM::Geometry clip = visual.Clip();
        if ( !isCurrent() )
            return wxRect();
        if ( clip )
        {
            const MUXM::RectangleGeometry rectangle =
                clip.try_as<MUXM::RectangleGeometry>();
            if ( !rectangle )
                return wxRect();

            const winrt::Windows::Foundation::Rect clipRect =
                rectangle.Rect();
            if ( !isCurrent() || !std::isfinite(clipRect.X) ||
                 !std::isfinite(clipRect.Y) ||
                 !std::isfinite(clipRect.Width) ||
                 !std::isfinite(clipRect.Height) )
            {
                return wxRect();
            }

            const float left = wxMax(0.0f, clipRect.X);
            const float top = wxMax(0.0f, clipRect.Y);
            const float right = wxMin(
                static_cast<float>(width),
                clipRect.X + wxMax(0.0f, clipRect.Width));
            const float bottom = wxMin(
                static_cast<float>(height),
                clipRect.Y + wxMax(0.0f, clipRect.Height));
            if ( right <= left || bottom <= top )
                return wxRect();

            localBounds = winrt::Windows::Foundation::Rect{
                left, top, right - left, bottom - top};
        }
    }

    return wxWinUIMapNotebookBoundsToClient(
        anchor,
        visual,
        localBounds,
        std::forward<Validator>(isCurrent));
}

template<typename Validator>
wxRect wxWinUIGetRealizedElementRectInNotebook(
    wxWindow *anchor,
    const MUX::FrameworkElement& element,
    Validator&& isCurrent)
{
    return wxWinUIGetRealizedElementRectInNotebookImpl(
        anchor,
        element,
        false,
        std::forward<Validator>(isCurrent));
}

template<typename Validator>
wxRect wxWinUIGetVisibleRealizedElementRectInNotebook(
    wxWindow *anchor,
    const MUX::FrameworkElement& element,
    Validator&& isCurrent)
{
    return wxWinUIGetRealizedElementRectInNotebookImpl(
        anchor,
        element,
        true,
        std::forward<Validator>(isCurrent));
}

// The extended Canvas is deliberately arranged in the island's physical-LTR
// coordinates. wx public geometry, however, uses the client coordinate system
// of the notebook HWND, whose X axis is mirrored by WS_EX_LAYOUTRTL. Keep the
// two spaces separate instead of letting the physical Canvas model leak from
// GetTabRect()/HitTest(). This is the projection-free equivalent of the common
// ElementBoundsToClient() mapper used by the stock TabView path.
wxRect wxWinUIExtendedPhysicalRectToClient(
    const wxRect& physicalRect,
    const wxSize& clientSize,
    wxLayoutDirection direction)
{
    if ( direction != wxLayout_RightToLeft || physicalRect.IsEmpty() )
        return physicalRect;

    return wxRect(clientSize.x - physicalRect.x - physicalRect.width,
                  physicalRect.y,
                  physicalRect.width,
                  physicalRect.height);
}
}

class wxWinUINotebookImpl
{
public:
    ~wxWinUINotebookImpl()
    {
        Close();
    }

    void Close()
    {
        // Invalidate first: event-token revocation and host detachment can
        // synchronously drain a callback.
        if ( callbackState )
            callbackState->Invalidate();

        if ( closed )
            return;
        closed = true;

        MUXD::DispatcherQueue retirementQueue{ nullptr };
        try
        {
            if ( extendedRoot )
                retirementQueue = extendedRoot.DispatcherQueue();
            else if ( tabView )
                retirementQueue = tabView.DispatcherQueue();
        }
        catch ( const winrt::hresult_error& )
        {
        }

        if ( tabView && selectionChangedToken.value )
        {
            try
            {
                tabView.SelectionChanged(selectionChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TabView SelectionChanged removal", e);
            }
        }
        selectionChangedToken = {};

        if ( tabView && layoutUpdatedToken.value )
        {
            try
            {
                tabView.LayoutUpdated(layoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TabView LayoutUpdated removal", e);
            }
        }
        layoutUpdatedToken = {};

        wxWinUIRevokeNotebookToken(
            extendedLayoutUpdatedToken,
            [this](winrt::event_token token)
            {
                if ( extendedRoot )
                    extendedRoot.LayoutUpdated(token);
            });

        // Routed event sources keep their delegates alive independently of
        // the wx owner. Revoke every extended-surface token while the peers
        // are still attached; otherwise terminal retirement can release the
        // graph yet leave one callback-state reference per notebook behind.
        for ( auto& peer : extendedPeers )
            wxWinUIRevokeNotebookSurfacePeerHandlers(peer);
        wxWinUIRevokeNotebookButtonHandler(
            extendedBackButton, extendedBackClickToken);
        wxWinUIRevokeNotebookButtonHandler(
            extendedForwardButton, extendedForwardClickToken);

        std::shared_ptr<wxWinUINotebookPeerRetirement> retirement;
        wxWinUIFrameworkRetirementId retirementId = 0;
        if ( tabView || extendedRoot )
        {
            try
            {
                retirement =
                    std::make_shared<wxWinUINotebookPeerRetirement>();
                retirementId = wxWinUIRegisterFrameworkRetirement(
                    retirementQueue,
                    [retirement]() noexcept
                    {
                        retirement->RequestComplete();
                    });
            }
            catch ( ... )
            {
            }

            // Detaching without a ledger owner would release a callback-
            // bearing XAML graph in this Close() stack. OOM or registration
            // failure has no safe recovery once this path has started: stop
            // before host.Close() can sever the native/XAML relationship.
            if ( !retirement || !retirementId )
            {
                ::RaiseFailFastException(nullptr, nullptr, 0);
                ::TerminateProcess(
                    ::GetCurrentProcess(), 0xC0000602u);
                for ( ;; )
                {
                }
            }
        }

        // The framework-shutdown ticket above is installed before detach.
        // Its callback only requests completion until the peer graph has been
        // moved and this detach stack has crossed host.Close().
        host.Close();

        if ( retirement )
        {
            retirement->callbackState = std::move(callbackState);
            retirement->tabView = std::move(tabView);
            retirement->extendedRoot = std::move(extendedRoot);
            retirement->extendedTabCanvas =
                std::move(extendedTabCanvas);
            retirement->extendedPageChrome =
                std::move(extendedPageChrome);
            retirement->extendedBackButton =
                std::move(extendedBackButton);
            retirement->extendedForwardButton =
                std::move(extendedForwardButton);
            retirement->extendedBackClickToken =
                extendedBackClickToken;
            extendedBackClickToken = {};
            retirement->extendedForwardClickToken =
                extendedForwardClickToken;
            extendedForwardClickToken = {};
            retirement->extendedPeers.swap(extendedPeers);
            retirement->items.swap(items);
            retirement->headers.swap(headers);
            retirement->icons.swap(icons);
            retirement->labels.swap(labels);
            retirement->ArmForCompletion();
        }
        else
        {
            extendedPeers.clear();
            extendedForwardButton = nullptr;
            extendedBackButton = nullptr;
            extendedTabCanvas = nullptr;
            extendedPageChrome = nullptr;
            extendedRoot = nullptr;
            items.clear();
            headers.clear();
            icons.clear();
            labels.clear();
            tabView = nullptr;
            callbackState.reset();
        }

        extendedTabRects.clear();
        extendedIconRects.clear();
        extendedLabelRects.clear();
        extendedLabelMetricTexts.clear();
        extendedLabelNaturalPixelSizes.clear();
        extendedLabelMetricPending.clear();
        extendedLabelMetricProbeEpochs.clear();
        extendedLabelMetricRelayoutRequested.clear();
        extendedLabelMetricScale = 0.0;
        extendedLayoutContinuationScheduled = false;
        extendedLayoutContinuationEpoch = 0;
        if ( ++extendedLayoutContinuationGeneration == 0 )
            ++extendedLayoutContinuationGeneration;
        extendedAttachedGeneration = 0;
        extendedAttachmentInProgress = false;
        iconPixelSizes.clear();
        iconGenerations.clear();
        tabMetrics.clear();
        tabPrefixWidths.clear();
        geometryValid = false;
#ifdef WXWINUI_TEST_SUPPORT
        failNextMutation = false;
#endif
        projectionRequested = false;
        projectionInProgress = false;
        projectionReplayScheduled = false;
#ifdef WXWINUI_TEST_SUPPORT
        nextProjectionHookForTesting = nullptr;
        nextProjectionHookContextForTesting = nullptr;
#endif

        if ( retirement )
        {
            wxWinUIQueueNotebookPeerRetirement(
                retirementQueue, retirementId);
        }
    }

#ifdef WXWINUI_TEST_SUPPORT
    bool ShouldFail(wxWinUINotebookTestAccess::PeerMutation mutation)
    {
        if ( !failNextMutation || failedMutation != mutation )
            return false;

        failNextMutation = false;
        return true;
    }

    void InvokeProjectionHook(
        wxNotebook *owner,
        wxWinUINotebookTestAccess::ProjectionPoint point)
    {
        if ( nextProjectionHookForTesting == nullptr ||
             nextProjectionPointForTesting != point )
        {
            return;
        }

        const wxWinUINotebookTestAccess::ProjectionHook hook =
            nextProjectionHookForTesting;
        void * const context = nextProjectionHookContextForTesting;
        nextProjectionHookForTesting = nullptr;
        nextProjectionHookContextForTesting = nullptr;

        // Deliberately last operation: the hook may synchronously destroy
        // the notebook and this implementation. The caller resolves the
        // owner through callbackState again.
        hook(owner, context);
    }
#endif

    std::uint64_t BumpModelRevision()
    {
        // A nested mutation belongs to the transaction already consuming the
        // retry budget. It must not buy another deferred wave, otherwise an
        // application callback can keep a CallAfter chain alive forever by
        // changing the model once per projection pass. A later, genuinely
        // external mutation rearms the single deferred wave.
        const bool rearmDeferredBudget =
            !projectionInProgress && !projectionReplayScheduled;

        ++modelRevision;
        if ( modelRevision == 0 )
            ++modelRevision;
        geometryValid = false;
        extendedLayoutValid = false;
        extendedManualScroll = false;
        projectionRequested = true;
        projectionDeferredRevision = modelRevision;
        if ( rearmDeferredBudget )
        {
            projectionDeferredAttempts = 0;
            projectionWarningIssued = false;
        }
        return modelRevision;
    }

    std::uint64_t BumpStyleRevision()
    {
        if ( ++styleRevision == 0 )
            ++styleRevision;
        return styleRevision;
    }

    void InvalidateGeometry()
    {
        geometryValid = false;
        extendedLayoutValid = false;
        ++layoutRevision;
        if ( layoutRevision == 0 )
            ++layoutRevision;
    }

    bool RestorePeerItems(int selection)
    {
        if ( !tabView )
            return false;

        wxWinUINotebookPeerMutationGuard mutation(callbackState);
        try
        {
            const auto tabItems = tabView.TabItems();
            tabItems.Clear();
            for ( const auto& item : items )
                tabItems.Append(item);

            tabView.SelectedIndex(
                selection >= 0 &&
                        static_cast<std::size_t>(selection) < items.size()
                    ? selection
                    : wxNOT_FOUND);
            InvalidateGeometry();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TabView transaction rollback", e);
            return false;
        }
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUINotebookCallbackState> callbackState;
    MUXC::TabView tabView{ nullptr };
    MUXC::Canvas extendedRoot{ nullptr };
    MUXC::Canvas extendedTabCanvas{ nullptr };
    MUXC::Border extendedPageChrome{ nullptr };
    MUXC::Button extendedBackButton{ nullptr };
    MUXC::Button extendedForwardButton{ nullptr };
    winrt::event_token extendedBackClickToken{};
    winrt::event_token extendedForwardClickToken{};
    std::vector<wxWinUINotebookSurfacePeer> extendedPeers;
    std::vector<wxRect> extendedTabRects;
    std::vector<wxRect> extendedIconRects;
    std::vector<wxRect> extendedLabelRects;
    // TextBlock's intrinsic frame is not the wx text extent (the default XAML
    // font adds its own overhang). Never call Measure() while a connected
    // island may already be in measure/arrange: cache only sizes observed from
    // a completed XAML frame. A text change first publishes an unconstrained
    // probe; the next layout turn adopts that real size monotonically.
    std::vector<wxString> extendedLabelMetricTexts;
    std::vector<wxSize> extendedLabelNaturalPixelSizes;
    std::vector<bool> extendedLabelMetricPending;
    std::vector<std::uint64_t> extendedLabelMetricProbeEpochs;
    std::vector<bool> extendedLabelMetricRelayoutRequested;
    double extendedLabelMetricScale = 0.0;
    std::uint64_t extendedLayoutEpoch = 1;
    bool extendedLayoutContinuationScheduled = false;
    std::uint64_t extendedLayoutContinuationEpoch = 0;
    std::uint64_t extendedLayoutContinuationGeneration = 0;
    std::uint64_t extendedLayoutContinuationCount = 0;
    std::uint64_t extendedSurfaceGeneration = 0;
    std::uint64_t extendedAttachedGeneration = 0;
    std::uint64_t extendedLayoutModelRevision = 0;
    std::uint64_t extendedLayoutRevision = 0;
    wxSize extendedLayoutClientSize;
    wxRect extendedViewportRect;
    long extendedLayoutStyle = 0;
    double extendedLayoutScale = 0.0;
    int extendedBandExtentPx = 0;
    int extendedRowCount = 1;
    int extendedScrollOffsetPx = 0;
    int extendedScrollMaximumPx = 0;
    int extendedScrollViewportPx = 0;
    bool extendedLayoutValid = false;
    // Revision owned by the geometry transaction currently holding
    // callbackState's realization guard. A nested style writer is allowed to
    // defer its layout until that guard is released, but same-revision nested
    // geometry must still fail instead of observing a partial publication.
    std::uint64_t extendedLayoutInProgressStyleRevision = 0;
    bool extendedAttachmentInProgress = false;
    bool extendedManualScroll = false;
    std::uint64_t styleRevision = 1;
    winrt::event_token selectionChangedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token extendedLayoutUpdatedToken{};

    // One TabViewItem per page, kept parallel to wxBookCtrlBase's m_pages
    // array. The items deliberately have null Content: wx pages are native
    // HWND children, and giving TabView a XAML UIElement as page content makes
    // it reparent that element through its ContentPresenter on every
    // selection change.
    std::vector<MUXC::TabViewItem> items;

    // The stable header elements make hit-test geometry independent of
    // private TabView template-part names. They are kept parallel to items.
    std::vector<MUXC::StackPanel> headers;
    std::vector<MUXC::Image> icons;
    std::vector<MUXC::TextBlock> labels;
    std::vector<wxSize> iconPixelSizes;
    std::vector<std::uint64_t> iconGenerations;
    std::vector<wxWinUINotebookTabMetric> tabMetrics;
    std::vector<double> tabPrefixWidths;
    std::uint64_t geometryModelRevision = 0;
    std::uint64_t layoutRevision = 1;
    std::uint64_t geometryLayoutRevision = 0;
    double geometryScale = 0.0;
    double geometryOriginDIP = 0.0;
    double geometryTopDIP = 0.0;
    wxSize geometryClientSize;
    int geometrySelection = wxNOT_FOUND;
    wxLayoutDirection geometryDirection = wxLayout_Default;
    size_t lastGeometryAnchor = 0;
    bool geometryValid = false;
    std::uint64_t nextIconGeneration = 0;
    std::uint64_t modelRevision = 1;
    bool projectionRequested = false;
    bool projectionInProgress = false;
    bool projectionReplayScheduled = false;
    bool projectionWarningIssued = false;
    std::uint64_t projectionDeferredRevision = 0;
    unsigned projectionDeferredAttempts = 0;
#ifdef WXWINUI_TEST_SUPPORT
    double projectionScaleOverrideForTesting = 0.0;

    wxWinUINotebookTestAccess::ProjectionPoint
        nextProjectionPointForTesting =
            wxWinUINotebookTestAccess::ProjectionPoint::LabelText;
    wxWinUINotebookTestAccess::ProjectionHook
        nextProjectionHookForTesting = nullptr;
    void *nextProjectionHookContextForTesting = nullptr;

    wxWinUINotebookTestAccess::PeerMutation failedMutation =
        wxWinUINotebookTestAccess::PeerMutation::InsertPage;
    bool failNextMutation = false;
#endif
    bool closed = false;
};

void wxNotebook::Init()
{
    m_tabStripHeightDIP = -1;
}

wxNotebook::wxNotebook()
{
    Init();
}

wxNotebook::wxNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, name);
}

wxNotebook::~wxNotebook()
{
    Unbind(wxEVT_DPI_CHANGED, &wxNotebook::OnDPIChanged, this);

    if ( m_winui )
        m_winui->Close();
}

bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    // wxMSW accepts every notebook presentation style. Normalize an
    // accidentally combined alignment to the same stable priority used by
    // wxNotebookBase::GetTabOrientation(), instead of rejecting creation.
    const long alignment = style & wxBK_ALIGN_MASK;
    long normalizedAlignment = wxNB_TOP;
    if ( alignment & wxNB_BOTTOM )
        normalizedAlignment = wxNB_BOTTOM;
    else if ( alignment & wxNB_RIGHT )
        normalizedAlignment = wxNB_RIGHT;
    else if ( alignment & wxNB_LEFT )
        normalizedAlignment = wxNB_LEFT;
    style = (style & ~wxBK_ALIGN_MASK) | normalizedAlignment;

    // The TabView draws its own surface; suppress the native control border.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUINotebookImpl);
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        m_winui.reset();
        return false;
    }

    m_winui->callbackState =
        std::make_shared<wxWinUINotebookCallbackState>(this);
    wxWinUINotebookImpl * const createImpl = m_winui.get();
    const auto callbackState = createImpl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();

    try
    {
        createImpl->tabView = MUXC::TabView();
        createImpl->tabView.IsAddTabButtonVisible(false);
        createImpl->tabView.CanReorderTabs(false);
        createImpl->tabView.CanDragTabs(false);
        createImpl->tabView.TabWidthMode(
            HasFlag(wxNB_FIXEDWIDTH)
                ? MUXC::TabViewWidthMode::Equal
                : MUXC::TabViewWidthMode::SizeToContent);
        createImpl->tabView.FlowDirection(
            GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);

        createImpl->extendedRoot = MUXC::Canvas();
        // Canvas coordinates are always the physical-LTR slot coordinate
        // system. Individual header contents inherit the requested text flow.
        createImpl->extendedRoot.FlowDirection(
            MUX::FlowDirection::LeftToRight);
        createImpl->extendedRoot.HorizontalAlignment(
            MUX::HorizontalAlignment::Stretch);
        createImpl->extendedRoot.VerticalAlignment(
            MUX::VerticalAlignment::Stretch);
        MUXA::AutomationProperties::SetAccessibilityView(
            createImpl->extendedRoot,
            MUXA::Peers::AccessibilityView::Raw);

        createImpl->extendedTabCanvas = MUXC::Canvas();
        createImpl->extendedTabCanvas.FlowDirection(
            MUX::FlowDirection::LeftToRight);
        MUXA::AutomationProperties::SetAccessibilityView(
            createImpl->extendedTabCanvas,
            MUXA::Peers::AccessibilityView::Raw);

        createImpl->extendedPageChrome = wxWinUICreateThemeBrushBorder(
            "ControlStrokeColorDefaultBrush",
            wxWinUIThemeBrushProperty::BorderBrush);
        if ( !createImpl->extendedPageChrome )
        {
            createImpl->extendedPageChrome =
                wxWinUICreateThemeBrushBorder(
                    "CardStrokeColorDefaultBrush",
                    wxWinUIThemeBrushProperty::BorderBrush);
        }
        if ( !createImpl->extendedPageChrome )
            createImpl->extendedPageChrome = MUXC::Border();
        createImpl->extendedPageChrome.IsHitTestVisible(false);
        MUXA::AutomationProperties::SetAccessibilityView(
            createImpl->extendedPageChrome,
            MUXA::Peers::AccessibilityView::Raw);

        const auto makeOverflowButton =
            [](const wxString& accessibleName)
            {
                MUXC::Button button;
                button.IsTabStop(false);
                button.Visibility(MUX::Visibility::Collapsed);
                button.MinWidth(0.0);
                button.MinHeight(0.0);
                button.Padding(MUX::Thickness{});
                button.HorizontalContentAlignment(
                    MUX::HorizontalAlignment::Center);
                button.VerticalContentAlignment(
                    MUX::VerticalAlignment::Center);
                MUXA::AutomationProperties::SetName(
                    button, wxWinUIToHString(accessibleName));
                return button;
            };
        createImpl->extendedBackButton =
            makeOverflowButton(_("Scroll tabs backward"));
        createImpl->extendedForwardButton =
            makeOverflowButton(_("Scroll tabs forward"));

        const auto bindOverflowButton =
            [callbackState,
             callbackGeneration,
             createImpl](const MUXC::Button& button, int direction)
                -> winrt::event_token
            {
                return button.Click(
                    [callbackState,
                     callbackGeneration,
                     createImpl,
                     direction](
                        const winrt::Windows::Foundation::IInspectable&,
                        const MUX::RoutedEventArgs&)
                    {
                        wxNotebook *owner =
                            callbackState->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != createImpl ||
                             owner->m_winui->closed ||
                             !owner->UsesExtendedPeerSurface() ||
                             (owner->GetWindowStyleFlag() &
                                  wxNB_MULTILINE) != 0 )
                        {
                            return;
                        }

                        const int step = wxMax(
                            1,
                            createImpl->extendedScrollViewportPx * 3 / 4);
                        const int target = wxMax(
                            0,
                            wxMin(createImpl->extendedScrollMaximumPx,
                                  createImpl->extendedScrollOffsetPx +
                                      direction * step));
                        if ( target ==
                             createImpl->extendedScrollOffsetPx )
                        {
                            return;
                        }

                        createImpl->extendedScrollOffsetPx = target;
                        createImpl->extendedManualScroll = true;
                        createImpl->InvalidateGeometry();
                        owner = callbackState->GetOwner(
                            callbackGeneration);
                        if ( owner && owner->m_winui &&
                             owner->m_winui.get() == createImpl )
                        {
                            (void)owner->LayoutExtendedPeerSurface();
                        }
                    });
            };
        createImpl->extendedBackClickToken =
            bindOverflowButton(createImpl->extendedBackButton, -1);
        createImpl->extendedForwardClickToken =
            bindOverflowButton(createImpl->extendedForwardButton, 1);

        createImpl->extendedRoot.Children().Append(
            createImpl->extendedPageChrome);
        createImpl->extendedRoot.Children().Append(
            createImpl->extendedTabCanvas);
        createImpl->extendedRoot.Children().Append(
            createImpl->extendedBackButton);
        createImpl->extendedRoot.Children().Append(
            createImpl->extendedForwardButton);

        createImpl->selectionChangedToken =
            createImpl->tabView.SelectionChanged(
            [callbackState, callbackGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                MUXC::SelectionChangedEventArgs const&)
            {
                if ( callbackState->IsPeerMutationInProgress() )
                    return;

                wxNotebook * const owner =
                    callbackState->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->tabView )
                {
                    return;
                }

                owner->OnTabViewSelectionChanged(
                    owner->m_winui->tabView.SelectedIndex());
            });

        const std::weak_ptr<wxWinUINotebookCallbackState>
            weakCallbackState(callbackState);
        createImpl->extendedLayoutUpdatedToken =
            createImpl->extendedRoot.LayoutUpdated(
            [weakCallbackState, callbackGeneration, createImpl](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                // Timestamp the completed XAML frame, then post the one
                // coalesced consumer of any intrinsic label metrics which
                // became observable in it.
                // QueueExtendedPeerLayoutContinuation() never projects
                // inline from LayoutUpdated.
                const auto state = weakCallbackState.lock();
                if ( !state )
                    return;
                wxNotebook * const owner =
                    state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->closed )
                {
                    return;
                }

                if ( ++createImpl->extendedLayoutEpoch == 0 )
                    ++createImpl->extendedLayoutEpoch;
                owner->QueueExtendedPeerLayoutContinuation();
            });

        createImpl->layoutUpdatedToken =
            createImpl->tabView.LayoutUpdated(
            [callbackState, callbackGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( callbackState->IsPeerMutationInProgress() )
                    return;

                if ( wxNotebook * const owner =
                         callbackState->GetOwner(callbackGeneration) )
                {
                    // This delegate belongs to the stock TabView. It can have
                    // a queued LayoutUpdated after a hot transition to the
                    // extended Canvas, but that detached tree no longer owns
                    // notebook geometry. Invalidating the Canvas from this
                    // callback restarts measure/arrange while WinUI is raising
                    // LayoutUpdated and can fail-fast with AG_E_LAYOUT_CYCLE.
                    if ( owner->UsesExtendedPeerSurface() )
                        return;

                    if ( owner->m_winui &&
                         owner->m_winui->callbackState == callbackState &&
                         !callbackState->
                             IsGeometryRealizationInProgress() )
                    {
                        owner->m_winui->InvalidateGeometry();
                    }
                    owner->UpdateTabStripHeightFromLayout();
                }
            });

        const MUXC::TabView tabView = createImpl->tabView;
        const MUX::UIElement initialSurface =
            UsesExtendedPeerSurface()
                ? createImpl->extendedRoot.as<MUX::UIElement>()
                : tabView.as<MUX::UIElement>();
        const bool contentSet =
            createImpl->host.SetContent(initialSurface);
        wxNotebook *liveOwner =
            callbackState->GetOwner(callbackGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != callbackState ||
             liveOwner->m_winui->tabView != tabView )
        {
            return false;
        }

        if ( !contentSet )
        {
            createImpl->Close();
            liveOwner->m_winui.reset();
            return false;
        }

        // The native page starts below the same 40-DIP fallback used by
        // GetPageRect(). Apply that clip synchronously, before the first
        // LayoutUpdated measurement, so the full-height TabView island can
        // never temporarily cover the page/input surface. The measured
        // template band replaces this conservative value below.
        if ( !liveOwner->UsesExtendedPeerSurface() )
        {
            createImpl->host.SetBridgeHeightLimit(
                liveOwner->FromDIP(DefaultTabStripHeightDIP));
        }
        liveOwner = callbackState->GetOwner(callbackGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != callbackState ||
             liveOwner->m_winui->tabView != tabView )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView creation", e);
        wxNotebook * const liveOwner =
            callbackState->GetOwner(callbackGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == callbackState )
        {
            createImpl->Close();
            liveOwner->m_winui.reset();
        }
        return false;
    }

    wxNotebook *liveOwner =
        callbackState->GetOwner(callbackGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }
    liveOwner->SetInitialSize(size);
    liveOwner = callbackState->GetOwner(callbackGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }
    liveOwner->Bind(
        wxEVT_DPI_CHANGED, &wxNotebook::OnDPIChanged, liveOwner);
    if ( liveOwner->UsesExtendedPeerSurface() )
        (void)liveOwner->LayoutExtendedPeerSurface();
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
void
wxWinUINotebookTestAccess::FailNextPeerMutation(
    wxNotebook& control,
    wxWinUINotebookTestAccess::PeerMutation mutation)
{
    wxNotebook * const notebook = &control;

    wxCHECK_RET( notebook->m_winui && !notebook->m_winui->closed,
                 "uninitialized wxNotebook test seam" );

    notebook->m_winui->failedMutation = mutation;
    notebook->m_winui->failNextMutation = true;
}

void
wxWinUINotebookTestAccess::SetNextProjectionHook(
    wxNotebook& control,
    wxWinUINotebookTestAccess::ProjectionPoint point,
    wxWinUINotebookTestAccess::ProjectionHook hook,
    void *context)
{
    wxNotebook * const notebook = &control;

    wxCHECK_RET( notebook->m_winui && !notebook->m_winui->closed,
                 "uninitialized wxNotebook projection seam" );

    notebook->m_winui->nextProjectionPointForTesting = point;
    notebook->m_winui->nextProjectionHookForTesting = hook;
    notebook->m_winui->nextProjectionHookContextForTesting = context;
}

size_t
wxWinUINotebookTestAccess::GetPeerPageCount(
    const wxNotebook& control)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || !notebook->m_winui->tabView )
        return 0;

    try
    {
        return notebook->m_winui->tabView.TabItems().Size();
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

wxString
wxWinUINotebookTestAccess::GetPeerPageText(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || page >= notebook->m_winui->labels.size() )
        return wxString();

    try
    {
        return wxWinUIFromHString(notebook->m_winui->labels[page].Text());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

wxString
wxWinUINotebookTestAccess::GetPeerAutomationName(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || page >= notebook->m_winui->items.size() )
        return wxString();

    try
    {
        return wxWinUIFromHString(
            MUXA::AutomationProperties::GetName(notebook->m_winui->items[page]));
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

bool
wxWinUINotebookTestAccess::QueueSelectionCallback(
    wxNotebook& control,
    size_t page)
{
    wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || !notebook->m_winui->callbackState ||
         page >= notebook->m_pages.size() || !wxTheApp )
    {
        return false;
    }

    const auto callbackState = notebook->m_winui->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    wxTheApp->CallAfter(
        [callbackState, callbackGeneration, page]()
        {
            if ( wxNotebook * const owner =
                     callbackState->GetOwner(callbackGeneration) )
            {
                owner->OnTabViewSelectionChanged(
                    static_cast<int>(page));
            }
        });
    return true;
}

void
wxWinUINotebookTestAccess::ClosePeer(
    wxNotebook& control)
{
    wxNotebook * const notebook = &control;

    if ( notebook->m_winui )
        notebook->m_winui->Close();
}

size_t
wxWinUINotebookTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveNotebookCallbackStates.load(
        std::memory_order_relaxed);
}

size_t
wxWinUINotebookTestAccess::GetPendingPeerRetirementCount()
{
    return gs_pendingNotebookPeerRetirements.load(
        std::memory_order_relaxed);
}

size_t
wxWinUINotebookTestAccess::GetFrameworkRetirementCount()
{
    return wxWinUIRuntimeTestAccess::GetFrameworkRetirementCount();
}

void
wxWinUINotebookTestAccess::SetPeerRetirementQueueFault(
    wxWinUINotebookTestAccess::PeerRetirementQueueFault fault)
{
    gs_notebookPeerRetirementQueueFault.store(
        fault, std::memory_order_release);
}

wxWinUINotebookTestAccess::PeerRetirementQueueFault
wxWinUINotebookTestAccess::GetPeerRetirementQueueFault()
{
    return gs_notebookPeerRetirementQueueFault.load(
        std::memory_order_acquire);
}

void
wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
    wxWinUINotebookTestAccess::FrameworkRetirementHookFault fault)
{
    wxWinUIRuntimeTestAccess::SetFrameworkRetirementHookFault(
        static_cast<unsigned>(fault));
}

wxWinUINotebookTestAccess::FrameworkRetirementHookFault
wxWinUINotebookTestAccess::GetFrameworkRetirementHookFault()
{
    return static_cast<wxWinUINotebookTestAccess::FrameworkRetirementHookFault>(
        wxWinUIRuntimeTestAccess::GetFrameworkRetirementHookFault());
}

wxWinUINotebookTestAccess::FrameworkRetirementSnapshot
wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot()
{
    const wxWinUIRuntimeTestAccess::FrameworkRetirementSnapshot source =
        wxWinUIRuntimeTestAccess::GetFrameworkRetirementSnapshot();
    wxWinUINotebookTestAccess::FrameworkRetirementSnapshot snapshot;
    snapshot.entries = source.entries;
    snapshot.unboundEntries = source.unboundEntries;
    snapshot.queueStates = source.queueStates;
    snapshot.shutdownStartingHooks = source.shutdownStartingHooks;
    snapshot.frameworkStartingHooks = source.frameworkStartingHooks;
    snapshot.frameworkHooks = source.frameworkHooks;
    snapshot.shutdownHooks = source.shutdownHooks;
    snapshot.activeStates = source.activeStates;
    snapshot.shutdownStartingStates = source.shutdownStartingStates;
    snapshot.frameworkStartingStates = source.frameworkStartingStates;
    snapshot.xamlCompletedStates = source.xamlCompletedStates;
    snapshot.frameworkDoneStates = source.frameworkDoneStates;
    snapshot.shutdownDoneStates = source.shutdownDoneStates;
    snapshot.xamlShutdownHookInstalled =
        source.xamlShutdownHookInstalled;
    snapshot.rundown = source.rundown;
    snapshot.xamlTerminal = source.xamlTerminal;
    snapshot.xamlCompletionActive = source.xamlCompletionActive;
    snapshot.runtimeTerminal = source.runtimeTerminal;
    snapshot.phaseOrderValid = source.phaseOrderValid;
    return snapshot;
}

std::uint64_t
wxWinUINotebookTestAccess::RegisterUnboundFrameworkRetirement(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    return wxWinUIRegisterFrameworkRetirement(
        MUXD::DispatcherQueue{ nullptr },
        [hook, context]()
        {
            if ( hook )
                hook(context);
        });
}

void
wxWinUINotebookTestAccess::CompleteFrameworkRetirement(
    std::uint64_t id)
{
    wxWinUICompleteFrameworkRetirement(id);
}

void
wxWinUINotebookTestAccess::SimulateShutdownStarting(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    wxWinUIRuntimeTestAccess::SimulateShutdownStarting(hook, context);
}

void
wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    wxWinUIRuntimeTestAccess::SimulateFrameworkShutdownStarting(hook, context);
}

void
wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    wxWinUIRuntimeTestAccess::SimulateFrameworkShutdownCompleted(hook, context);
}

void
wxWinUINotebookTestAccess::SimulateShutdownCompleted(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    wxWinUIRuntimeTestAccess::SimulateShutdownCompleted(hook, context);
}

void
wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted(
    wxWinUINotebookTestAccess::FrameworkRetirementPhaseHook hook,
    void *context)
{
    wxWinUIRuntimeTestAccess::SimulateXamlShutdownCompleted(hook, context);
}

void
wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime()
{
    MUXD::DispatcherQueue queue{ nullptr };
    try
    {
        queue = MUXD::DispatcherQueue::GetForCurrentThread();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    wxWinUIRuntimeTestAccess::ResetFrameworkRetirementRuntime(queue);
}

wxRect
wxWinUINotebookTestAccess::GetTabIconRect(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxCHECK_MSG( page < notebook->m_pages.size(), wxRect(),
                 "invalid notebook page" );
    return notebook->GetRealizedTabElementRect(
        page, wxWinUINotebookTabPart_Icon, true);
}

wxRect
wxWinUINotebookTestAccess::GetTabLabelRect(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxCHECK_MSG( page < notebook->m_pages.size(), wxRect(),
                 "invalid notebook page" );
    return notebook->GetRealizedTabElementRect(
        page, wxWinUINotebookTabPart_Label, true);
}

wxRect
wxWinUINotebookTestAccess::GetActualTabRect(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxCHECK_MSG( page < notebook->m_pages.size(), wxRect(),
                 "invalid notebook page" );
    return notebook->GetRealizedTabElementRect(
        page, wxWinUINotebookTabPart_Item, false);
}

wxRect
wxWinUINotebookTestAccess::GetActualTabIconRect(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxCHECK_MSG( page < notebook->m_pages.size(), wxRect(),
                 "invalid notebook page" );
    return notebook->GetRealizedTabElementRect(
        page, wxWinUINotebookTabPart_Icon, false);
}

wxRect
wxWinUINotebookTestAccess::GetActualTabLabelRect(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxCHECK_MSG( page < notebook->m_pages.size(), wxRect(),
                 "invalid notebook page" );
    return notebook->GetRealizedTabElementRect(
        page, wxWinUINotebookTabPart_Label, false);
}

bool
wxWinUINotebookTestAccess::HasPendingExtendedLabelMetrics(
    const wxNotebook& control)
{
    const wxNotebook * const notebook = &control;

    return notebook->m_winui && notebook->UsesExtendedPeerSurface() &&
           std::any_of(
               notebook->m_winui->extendedLabelMetricPending.begin(),
               notebook->m_winui->extendedLabelMetricPending.end(),
               [](bool pending)
               {
                   return pending;
               });
}

std::uint64_t
wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(
    const wxNotebook& control)
{
    const wxNotebook * const notebook = &control;

    return notebook->m_winui ? notebook->m_winui->extendedLayoutContinuationCount : 0;
}

wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot
wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot snapshot;
    if ( !notebook->m_winui || !notebook->m_winui->callbackState ||
         page >= notebook->m_winui->extendedLabelNaturalPixelSizes.size() ||
         page >= notebook->m_winui->extendedLabelRects.size() ||
         page >= notebook->m_winui->extendedLabelMetricPending.size() ||
         page >= notebook->m_winui->extendedLabelMetricProbeEpochs.size() ||
         page >= notebook->m_winui->extendedPeers.size() )
    {
        return snapshot;
    }

    wxWinUINotebookImpl * const impl = notebook->m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    const std::uint64_t surfaceGeneration =
        impl->extendedSurfaceGeneration;
    const std::uint64_t observationEpoch = impl->extendedLayoutEpoch;
    const auto isCurrent =
        [notebook,
         state,
         callbackGeneration,
         impl,
         surfaceGeneration,
         observationEpoch,
         page]()
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner == notebook && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == state &&
                   !owner->m_winui->closed &&
                   impl->extendedSurfaceGeneration == surfaceGeneration &&
                   impl->extendedLayoutEpoch == observationEpoch &&
                   page < impl->extendedPeers.size();
        };
    snapshot.naturalSize = impl->extendedLabelNaturalPixelSizes[page];
    snapshot.publishedRect = impl->extendedLabelRects[page];
    snapshot.probeEpoch = impl->extendedLabelMetricProbeEpochs[page];
    snapshot.layoutEpoch = impl->extendedLayoutEpoch;
    snapshot.surfaceGeneration = surfaceGeneration;
    snapshot.modelRevision = impl->modelRevision;
    snapshot.layoutRevision = impl->layoutRevision;
    snapshot.styleRevision = impl->styleRevision;
    snapshot.publishedModelRevision =
        impl->extendedLayoutModelRevision;
    snapshot.publishedLayoutRevision =
        impl->extendedLayoutRevision;
    snapshot.continuationCount =
        impl->extendedLayoutContinuationCount;
    snapshot.pending = impl->extendedLabelMetricPending[page];

    double scale = impl->extendedLabelMetricScale;
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;
    try
    {
        const MUXC::TextBlock label = impl->extendedPeers[page].label;
        if ( !isCurrent() )
            return wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot();
        snapshot.labelIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_unknown(label));
        const double width = label.ActualWidth() * scale;
        if ( !isCurrent() )
            return wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot();

        const double height = label.ActualHeight() * scale;
        if ( !isCurrent() || !std::isfinite(width) ||
             !std::isfinite(height) ||
             impl->extendedLayoutEpoch != observationEpoch ||
             width <= 0.0 || height <= 0.0 )
        {
            return snapshot;
        }

        const double intMaximum =
            (std::numeric_limits<int>::max)();
        if ( width <= intMaximum && height <= intMaximum )
        {
            snapshot.actualSize = wxSize(
                wxMax(1, static_cast<int>(std::ceil(width))),
                wxMax(1, static_cast<int>(std::ceil(height))));
        }
        snapshot.liveActualRect =
            wxWinUIGetVisibleRealizedElementRectInNotebook(
                const_cast<wxNotebook *>(notebook), label, isCurrent);
        if ( !isCurrent() )
            return wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    return snapshot;
}

wxSize
wxWinUINotebookTestAccess::GetPeerTabPadding(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || !notebook->m_hasTabPadding ||
         page >= notebook->m_winui->items.size() )
        return wxSize();

    try
    {
        const MUX::Thickness padding = notebook->m_winui->items[page].Padding();
        const double scale = notebook->GetDPIScaleFactor();
        return wxSize(
            static_cast<int>(std::lround(padding.Left * scale)),
            static_cast<int>(std::lround(padding.Top * scale)));
    }
    catch ( const winrt::hresult_error& )
    {
        return wxSize();
    }
}

wxSize
wxWinUINotebookTestAccess::GetPeerTabSize(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || !notebook->m_hasTabSize ||
         page >= notebook->m_winui->items.size() )
        return wxSize();

    try
    {
        const auto& item = notebook->m_winui->items[page];
        const double scale = notebook->GetDPIScaleFactor();
        return wxSize(
            static_cast<int>(std::lround(item.MinWidth() * scale)),
            static_cast<int>(std::lround(item.MinHeight() * scale)));
    }
    catch ( const winrt::hresult_error& )
    {
        return wxSize();
    }
}

wxSize
wxWinUINotebookTestAccess::GetPeerIconPixelSize(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || page >= notebook->m_winui->icons.size() )
        return wxSize();

    try
    {
        const auto source =
            notebook->m_winui->icons[page].Source().try_as<MUXMI::WriteableBitmap>();
        return source
                   ? wxSize(source.PixelWidth(), source.PixelHeight())
                   : wxSize();
    }
    catch ( const winrt::hresult_error& )
    {
        return wxSize();
    }
}

wxSize
wxWinUINotebookTestAccess::GetPeerIconDIPSize(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || page >= notebook->m_winui->icons.size() )
        return wxSize();

    try
    {
        const MUXC::Image& icon = notebook->m_winui->icons[page];
        if ( !icon.Source() )
            return wxSize();

        return wxSize(
            static_cast<int>(std::lround(icon.Width())),
            static_cast<int>(std::lround(icon.Height())));
    }
    catch ( const winrt::hresult_error& )
    {
        return wxSize();
    }
}

std::uint64_t
wxWinUINotebookTestAccess::GetPeerIconGeneration(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || page >= notebook->m_winui->iconGenerations.size() )
        return 0;

    return notebook->m_winui->iconGenerations[page];
}

bool
wxWinUINotebookTestAccess::RefreshForScale(
    wxNotebook& control,
    double scale)
{
    wxNotebook * const notebook = &control;

    if ( !std::isfinite(scale) || scale <= 0.0 ||
         !notebook->m_winui || !notebook->m_winui->callbackState || notebook->m_winui->closed )
    {
        return false;
    }

    wxWinUINotebookImpl * const impl = notebook->m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    impl->projectionScaleOverrideForTesting = scale;
    notebook->OnImagesChanged();

    wxNotebook * const owner =
        state->GetOwner(callbackGeneration);
    if ( owner != notebook || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state ||
         impl->projectionInProgress ||
         impl->projectionReplayScheduled ||
         impl->iconGenerations.size() != impl->items.size() )
    {
        return false;
    }

    for ( const std::uint64_t generation :
          impl->iconGenerations )
    {
        if ( generation != impl->nextIconGeneration )
            return false;
    }
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

// ----------------------------------------------------------------------------
// page management
// ----------------------------------------------------------------------------

bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    if ( IsDeletingAllPages() )
        return false;

    wxCHECK_MSG( pPage, false, wxT("null page in wxNotebook::InsertPage()") );
    // Establish the page identity before any WinRT or virtual boundary. A
    // projection callback is allowed to destroy the candidate; creating a
    // weak reference only after publication would then dereference a dangling
    // m_pages entry while trying to recover.
    const wxWeakRef<wxWindow> pageLifetime(pPage);
    wxCHECK_MSG( !pPage->IsBeingDeleted(), false,
                 wxT("can't insert a page being deleted") );
    wxCHECK_MSG( nPage <= m_pages.size(), false,
                 wxT("invalid page index in wxNotebook::InsertPage()") );
    wxCHECK_MSG( m_winui && m_winui->tabView, false,
                 wxT("uninitialized wxNotebook in InsertPage()") );

    wxWinUINotebookImpl* const impl = m_winui.get();
    const auto callbackState = impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    const auto getOwner = [&]() -> wxNotebook*
    {
        wxNotebook* const owner =
            callbackState->GetOwner(callbackGeneration);
        return owner && owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState
                    ? owner
                    : nullptr;
    };

    const decltype(m_pages) pagesBefore = m_pages;
    std::vector<wxWeakRef<wxWindow>> pageLifetimesBefore;
    pageLifetimesBefore.reserve(pagesBefore.size());
    for ( wxWindow* const existingPage : pagesBefore )
        pageLifetimesBefore.emplace_back(existingPage);
    const auto preModelIsCurrent = [&](wxNotebook* const owner)
    {
        if ( !owner || owner->m_pages.size() != pagesBefore.size() )
            return false;
        for ( size_t i = 0; i < pagesBefore.size(); ++i )
        {
            wxWindow* const existingPage = pagesBefore[i];
            if ( owner->m_pages[i] != existingPage ||
                    (existingPage &&
                     (pageLifetimesBefore[i].get() != existingPage ||
                      wxWindowIsUnavailableForCallbacks(existingPage) ||
                      existingPage->GetParent() != owner)) )
            {
                return false;
            }
        }
        return true;
    };

    MUXC::TabViewItem item;
    MUXC::StackPanel header;
    MUXC::Image icon;
    MUXC::TextBlock label;
    try
    {
        header.Orientation(MUXC::Orientation::Horizontal);
        header.Spacing(DefaultHeaderSpacingDIP);
        header.VerticalAlignment(MUX::VerticalAlignment::Center);

        icon.Visibility(MUX::Visibility::Collapsed);
        icon.VerticalAlignment(MUX::VerticalAlignment::Center);
        header.Children().Append(icon);

        label.Text(wxWinUIToHString(strText));
        label.VerticalAlignment(MUX::VerticalAlignment::Center);
        header.Children().Append(label);

        item.Header(header);
        item.IsClosable(false);
        MUXA::AutomationProperties::SetName(
            item, wxWinUIToHString(strText));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView InsertPage item", e);
        return false;
    }

    // Stage every wx/peer mirror first. No live model changes until the XAML
    // collection mutation has succeeded, and all subsequent swaps are
    // non-throwing.
    decltype(m_pages) stagedPages;
    wxArrayString stagedTexts;
    wxArrayInt stagedImages;
    std::vector<MUXC::TabViewItem> stagedItems;
    std::vector<MUXC::StackPanel> stagedHeaders;
    std::vector<MUXC::Image> stagedIcons;
    std::vector<MUXC::TextBlock> stagedLabels;
    std::vector<wxSize> stagedIconPixelSizes;
    std::vector<std::uint64_t> stagedIconGenerations;
    try
    {
        stagedPages = m_pages;
        stagedTexts = m_pageTexts;
        stagedImages = m_pageImages;
        stagedItems = impl->items;
        stagedHeaders = impl->headers;
        stagedIcons = impl->icons;
        stagedLabels = impl->labels;
        stagedIconPixelSizes = impl->iconPixelSizes;
        stagedIconGenerations = impl->iconGenerations;

        stagedPages.insert(stagedPages.begin() + nPage, pPage);
        stagedTexts.Insert(strText, nPage);
        stagedImages.Insert(imageId, nPage);
        stagedItems.insert(stagedItems.begin() + nPage, item);
        stagedHeaders.insert(stagedHeaders.begin() + nPage, header);
        stagedIcons.insert(stagedIcons.begin() + nPage, icon);
        stagedLabels.insert(stagedLabels.begin() + nPage, label);
        stagedIconPixelSizes.insert(
            stagedIconPixelSizes.begin() + nPage, wxSize());
        stagedIconGenerations.insert(
            stagedIconGenerations.begin() + nPage, 0);
    }
    catch ( const std::exception& )
    {
        wxLogError("Unable to stage wxWinUI notebook page insertion");
        return false;
    }

    const bool failAfterPeerMutation =
#ifdef WXWINUI_TEST_SUPPORT
        impl->ShouldFail(
            wxWinUINotebookTestAccess::PeerMutation::InsertPage);
#else
        false;
#endif
    const std::uint64_t preMutationRevision = impl->modelRevision;
    const std::uint64_t operation = callbackState->BeginOperation();

    wxNotebook* owner = getOwner();
    if ( !preModelIsCurrent(owner) || pageLifetime.get() != pPage ||
            wxWindowIsUnavailableForCallbacks(pPage) ||
            pPage->GetParent() != owner )
    {
        return pageLifetime.get() != pPage;
    }

    try
    {
        wxWinUINotebookPeerMutationGuard mutation(callbackState);
        impl->tabView.TabItems().InsertAt(
            static_cast<uint32_t>(nPage), item);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView InsertPage", e);
        if ( wxNotebook* const currentOwner = getOwner() )
            impl->RestorePeerItems(currentOwner->m_selection);
        return pageLifetime.get() != pPage;
    }

    owner = getOwner();
    if ( !owner )
        return true;

    if ( !callbackState->IsCurrentOperation(operation) ||
            impl->modelRevision != preMutationRevision ||
            !preModelIsCurrent(owner) )
    {
        (void)impl->RestorePeerItems(owner->m_selection);
        return pageLifetime.get() != pPage;
    }

    // The XAML collection mutation is still pre-publication. If it consumed
    // the candidate through a callback, restore the peer from the unchanged
    // wx model and report ownership consumed without touching pPage again.
    if ( pageLifetime.get() != pPage ||
            wxWindowIsUnavailableForCallbacks(pPage) ||
            pPage->GetParent() != owner )
    {
        (void)impl->RestorePeerItems(owner->m_selection);
        return true;
    }

    if ( failAfterPeerMutation )
    {
        if ( !impl->RestorePeerItems(owner->m_selection) )
        {
            wxLogError(
                "Unable to roll back injected WinUI notebook insertion");
        }
        return false;
    }

    const int selectionBefore = owner->m_selection;
    const std::uint64_t iconGenerationBefore =
        impl->nextIconGeneration;
    owner->m_pages.swap(stagedPages);
    owner->m_pageTexts.swap(stagedTexts);
    owner->m_pageImages.swap(stagedImages);
    impl->items.swap(stagedItems);
    impl->headers.swap(stagedHeaders);
    impl->icons.swap(stagedIcons);
    impl->labels.swap(stagedLabels);
    impl->iconPixelSizes.swap(stagedIconPixelSizes);
    impl->iconGenerations.swap(stagedIconGenerations);

    // Preserve the identity of the selected page when inserting before it.
    // TabView does this for its selected item; keep the wx index in lockstep.
    if ( static_cast<int>(nPage) <= owner->m_selection )
        ++owner->m_selection;

    ++impl->nextIconGeneration;
    if ( impl->nextIconGeneration == 0 )
        ++impl->nextIconGeneration;
    const std::uint64_t insertionRevision =
        impl->BumpModelRevision();

    const auto publishedModelIsCurrent = [&]() -> wxNotebook*
    {
        wxNotebook* const current = getOwner();
        return current &&
                       impl->modelRevision == insertionRevision &&
                       nPage < current->m_pages.size() &&
                       current->m_pages[nPage] == pPage &&
                       current->m_pages.size() == impl->items.size() &&
                       current->m_pages.size() == impl->headers.size() &&
                       current->m_pages.size() == impl->icons.size() &&
                       current->m_pages.size() == impl->labels.size() &&
                       current->m_pages.size() ==
                           impl->iconPixelSizes.size() &&
                       current->m_pages.size() ==
                           impl->iconGenerations.size()
                    ? current
                    : nullptr;
    };
    const auto insertionIsCurrent = [&]() -> wxNotebook*
    {
        wxNotebook* const current = publishedModelIsCurrent();
        return current && callbackState->IsCurrentOperation(operation)
                    ? current
                    : nullptr;
    };
    const auto rollbackPublishedInsertion = [&]() -> bool
    {
        wxNotebook* const current = publishedModelIsCurrent();
        if ( !current )
            return false;

        // Preserve a newer selection-only writer by identity. A topology or
        // projection writer bumps modelRevision and is rejected above.
        int selectionToRestore = selectionBefore;
        if ( current->m_selection != wxNOT_FOUND &&
                static_cast<size_t>(current->m_selection) <
                    current->m_pages.size() )
        {
            wxWindow* const selectedPage =
                current->m_pages[static_cast<size_t>(current->m_selection)];
            const auto selectedInOld =
                std::find(stagedPages.begin(), stagedPages.end(),
                          selectedPage);
            if ( selectedInOld != stagedPages.end() )
            {
                selectionToRestore = static_cast<int>(
                    selectedInOld - stagedPages.begin());
            }
        }

        // Supersede queued work before restoring every mirror. The staged
        // vectors now contain the complete pre-insertion state because all
        // publication swaps above were non-throwing.
        callbackState->BeginOperation();
        current->m_pages.swap(stagedPages);
        current->m_pageTexts.swap(stagedTexts);
        current->m_pageImages.swap(stagedImages);
        impl->items.swap(stagedItems);
        impl->headers.swap(stagedHeaders);
        impl->icons.swap(stagedIcons);
        impl->labels.swap(stagedLabels);
        impl->iconPixelSizes.swap(stagedIconPixelSizes);
        impl->iconGenerations.swap(stagedIconGenerations);
        current->m_selection = selectionToRestore;
        impl->nextIconGeneration = iconGenerationBefore;
        impl->BumpModelRevision();
        if ( !impl->RestorePeerItems(selectionToRestore) )
        {
            wxLogError(
                "Unable to restore WinUI notebook after page destruction");
        }
        return true;
    };

    (void)owner->SyncPeerProjection();

    owner = getOwner();
    if ( !owner )
        return true;

    if ( pageLifetime.get() != pPage ||
            wxWindowIsUnavailableForCallbacks(pPage) ||
            pPage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }

    // If a nested model writer superseded this insertion, its topology is
    // authoritative. It has also taken responsibility for the candidate.
    if ( !insertionIsCurrent() )
        return true;

    owner->DoInvalidateBestSize();

    owner = getOwner();
    if ( !owner )
        return true;
    if ( pageLifetime.get() != pPage ||
            wxWindowIsUnavailableForCallbacks(pPage) ||
            pPage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }
    if ( !publishedModelIsCurrent() || !insertionIsCurrent() )
        return true;

    // The page is hidden until it becomes the selected one.
    pPage->Show(false);
    owner = getOwner();
    wxWindow *livePage = pageLifetime.get();
    if ( !owner )
        return true;
    if ( !livePage || wxWindowIsUnavailableForCallbacks(livePage) ||
         livePage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }
    if ( !publishedModelIsCurrent() || !insertionIsCurrent() ||
         nPage >= owner->m_pages.size() ||
         owner->m_pages[nPage] != livePage )
    {
        return true;
    }

    // GetPageRect() is virtual and can synchronously run application code.
    // Keep its result separate from SetSize() so neither the notebook nor the
    // page can be reused across that boundary without a lifetime check.
    const wxRect pageRect = owner->GetPageRect();
    owner = getOwner();
    livePage = pageLifetime.get();
    if ( !owner )
        return true;
    if ( !livePage || wxWindowIsUnavailableForCallbacks(livePage) ||
            livePage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }
    if ( !publishedModelIsCurrent() || !insertionIsCurrent() ||
         nPage >= owner->m_pages.size() ||
         owner->m_pages[nPage] != livePage )
    {
        return true;
    }

    livePage->SetSize(pageRect);
    owner = getOwner();
    livePage = pageLifetime.get();
    if ( !owner )
        return true;
    if ( !livePage || wxWindowIsUnavailableForCallbacks(livePage) ||
            livePage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }
    if ( !publishedModelIsCurrent() || !insertionIsCurrent() ||
         nPage >= owner->m_pages.size() ||
         owner->m_pages[nPage] != livePage )
    {
        return true;
    }

    const bool selected =
        owner->DoSetSelectionAfterInsertion(nPage, bSelect);
    owner = getOwner();
    livePage = pageLifetime.get();
    if ( !owner )
        return true;
    if ( !livePage || wxWindowIsUnavailableForCallbacks(livePage) ||
            livePage->GetParent() != owner )
    {
        (void)rollbackPublishedInsertion();
        return true;
    }
    if ( !publishedModelIsCurrent() )
        return true;
    if ( !callbackState->IsCurrentOperation(operation) )
        return true;
    if ( !selected &&
         nPage < owner->m_pages.size() &&
         owner->m_pages[nPage] == livePage )
    {
        // Final operation: Show() can synchronously destroy either window.
        livePage->Show(false);
        owner = getOwner();
        livePage = pageLifetime.get();
        if ( owner &&
                (!livePage ||
                 wxWindowIsUnavailableForCallbacks(livePage) ||
                 livePage->GetParent() != owner) )
        {
            (void)rollbackPublishedInsertion();
        }
    }

    return true;
}

wxWindow *wxNotebook::DoRemovePage(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), nullptr,
                 wxT("invalid page index in wxNotebook::DoRemovePage()") );
    wxCHECK_MSG( m_winui && m_winui->tabView, nullptr,
                 wxT("uninitialized wxNotebook in DoRemovePage()") );

    wxWindow *page = m_pages[nPage];
    const wxWeakRef<wxWindow> pageLifetime(page);
    wxWinUINotebookImpl* const impl = m_winui.get();
    const auto callbackState = impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    const std::uint64_t preMutationRevision = impl->modelRevision;
    const std::uint64_t preMutationOperation =
        callbackState->BeginOperation();
    const auto getOwner = [&]() -> wxNotebook*
    {
        wxNotebook* const owner =
            callbackState->GetOwner(callbackGeneration);
        return owner && owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState
                    ? owner
                    : nullptr;
    };

    decltype(m_pages) stagedPages;
    wxArrayString stagedTexts;
    wxArrayInt stagedImages;
    std::vector<MUXC::TabViewItem> stagedItems;
    std::vector<MUXC::StackPanel> stagedHeaders;
    std::vector<MUXC::Image> stagedIcons;
    std::vector<MUXC::TextBlock> stagedLabels;
    std::vector<wxSize> stagedIconPixelSizes;
    std::vector<std::uint64_t> stagedIconGenerations;
    try
    {
        stagedPages = m_pages;
        stagedTexts = m_pageTexts;
        stagedImages = m_pageImages;
        stagedItems = impl->items;
        stagedHeaders = impl->headers;
        stagedIcons = impl->icons;
        stagedLabels = impl->labels;
        stagedIconPixelSizes = impl->iconPixelSizes;
        stagedIconGenerations = impl->iconGenerations;

        stagedPages.erase(stagedPages.begin() + nPage);
        stagedTexts.RemoveAt(nPage);
        stagedImages.RemoveAt(nPage);
        stagedItems.erase(stagedItems.begin() + nPage);
        stagedHeaders.erase(stagedHeaders.begin() + nPage);
        stagedIcons.erase(stagedIcons.begin() + nPage);
        stagedLabels.erase(stagedLabels.begin() + nPage);
        stagedIconPixelSizes.erase(
            stagedIconPixelSizes.begin() + nPage);
        stagedIconGenerations.erase(
            stagedIconGenerations.begin() + nPage);
    }
    catch ( const std::exception& )
    {
        wxLogError("Unable to stage wxWinUI notebook page removal");
        return nullptr;
    }

    const bool failAfterPeerMutation =
#ifdef WXWINUI_TEST_SUPPORT
        impl->ShouldFail(
            wxWinUINotebookTestAccess::PeerMutation::RemovePage);
#else
        false;
#endif

    try
    {
        wxWinUINotebookPeerMutationGuard mutation(callbackState);
        impl->tabView.TabItems().RemoveAt(static_cast<uint32_t>(nPage));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView DoRemovePage", e);
        if ( wxNotebook* const owner = getOwner() )
            impl->RestorePeerItems(owner->m_selection);
        return nullptr;
    }

    wxNotebook* owner = getOwner();
    if ( !owner )
        return nullptr;

    const bool preMutationIsCurrent =
        callbackState->IsCurrentOperation(preMutationOperation) &&
        impl->modelRevision == preMutationRevision &&
        nPage < owner->m_pages.size() &&
        owner->m_pages[nPage] == page;
    if ( !preMutationIsCurrent )
    {
        // RemoveAt() already changed the peer, while a nested operation made
        // the staged wx snapshot stale. Rebuild from the authoritative latest
        // wx model before returning; otherwise a selection-only callback can
        // leave TabItems short by one while m_pages is unchanged.
        (void)impl->RestorePeerItems(owner->m_selection);
        return nullptr;
    }

    const bool pageWasConsumed =
        pageLifetime.get() != page ||
        wxWindowIsUnavailableForCallbacks(page) ||
        page->GetParent() != owner;

    if ( failAfterPeerMutation && !pageWasConsumed )
    {
        if ( !impl->RestorePeerItems(owner->m_selection) )
        {
            wxLogError(
                "Unable to roll back injected WinUI notebook removal");
        }
        return nullptr;
    }

    const std::uint64_t operation = callbackState->BeginOperation();
    owner->m_pages.swap(stagedPages);
    owner->m_pageTexts.swap(stagedTexts);
    owner->m_pageImages.swap(stagedImages);
    impl->items.swap(stagedItems);
    impl->headers.swap(stagedHeaders);
    impl->icons.swap(stagedIcons);
    impl->labels.swap(stagedLabels);
    impl->iconPixelSizes.swap(stagedIconPixelSizes);
    impl->iconGenerations.swap(stagedIconGenerations);

    const std::uint64_t removalRevision = impl->BumpModelRevision();
    const auto getCurrentOwner = [&]() -> wxNotebook*
    {
        wxNotebook* const current = getOwner();
        return current && callbackState->IsCurrentOperation(operation) &&
                       impl->modelRevision == removalRevision &&
                       current->m_pages.size() == impl->items.size() &&
                       current->m_pages.size() == impl->headers.size() &&
                       current->m_pages.size() == impl->icons.size() &&
                       current->m_pages.size() == impl->labels.size()
                    ? current
                    : nullptr;
    };
    const auto getTransferredPage = [&]() -> wxWindow*
    {
        wxNotebook* const current = getOwner();
        wxWindow* const livePage = pageLifetime.get();
        if ( !current || livePage != page ||
                wxWindowIsUnavailableForCallbacks(livePage) ||
                livePage->GetParent() != current )
        {
            return nullptr;
        }

        return current->wxBookCtrlBase::FindPage(livePage) == wxNOT_FOUND
                    ? livePage
                    : nullptr;
    };

    (void)owner->SyncPeerProjection();
    owner = getCurrentOwner();
    if ( !owner )
        return getTransferredPage();

    owner->DoInvalidateBestSize();
    owner = getCurrentOwner();
    if ( !owner )
        return getTransferredPage();

    owner->DoSetSelectionAfterRemoval(nPage);

    // wxBookCtrlBase adjusts m_selection directly when a page before the
    // selected one is removed. In that branch it deliberately doesn't call
    // SetSelection(), so refresh the parallel extended peers here. If it did
    // call SetSelection(), that newer operation supersedes this one and the
    // selection transaction has already performed the refresh itself.
    owner = getCurrentOwner();
    if ( owner && owner->UsesExtendedPeerSurface() )
    {
        owner->UpdateExtendedPeerSelection(
            owner->m_selection == wxNOT_FOUND
                ? static_cast<size_t>(wxNOT_FOUND)
                : static_cast<size_t>(owner->m_selection));
    }

    // A selection handler can synchronously destroy the notebook, consume or
    // republish the page, or start a newer model operation. Return ownership
    // only when the exact weak identity is still the book's detached child.
    return getTransferredPage();
}

bool wxNotebook::DeleteAllPages()
{
    wxCHECK_MSG( m_winui && m_winui->tabView, false,
                 wxT("uninitialized wxNotebook in DeleteAllPages()") );

#ifdef WXWINUI_TEST_SUPPORT
    // Preserve the dedicated all-pages failure seam, but fail before touching
    // either model. The common implementation now drives the existing atomic
    // DoRemovePage() transaction one page at a time, so clearing the TabView
    // and its parallel mirrors here would make the first removal observe an
    // already-empty peer.
    if ( m_winui->ShouldFail(
             wxWinUINotebookTestAccess::PeerMutation::ClearPages) )
        return false;
#endif

    return wxNotebookBase::DeleteAllPages();
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxNotebook::SetSelection(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxNotebook::SetSelection()") );

    return DoSetSelectionPreservingFocus(nPage, SetSelection_SendEvent);
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    wxCHECK_MSG( nPage < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxNotebook::ChangeSelection()") );

    return DoSetSelectionPreservingFocus(nPage, 0);
}

int wxNotebook::DoSetSelectionPreservingFocus(size_t nPage, int flags)
{
    wxCHECK_MSG( nPage < m_pages.size(), wxNOT_FOUND,
                 wxT("invalid page index in wxNotebook selection") );
    wxCHECK_MSG( m_winui && m_winui->callbackState, wxNOT_FOUND,
                 wxT("uninitialized wxNotebook in selection") );

    const int oldSel = m_selection;
    if ( nPage == static_cast<size_t>(oldSel) )
        return oldSel;

    const auto callbackState = m_winui->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    const std::uint64_t operation =
        callbackState->BeginOperation();
    const auto getCurrentOwner =
        [callbackState,
         callbackGeneration,
         operation]() -> wxNotebook *
        {
            wxNotebook * const owner =
                callbackState->GetOwner(callbackGeneration);
            return owner &&
                           callbackState->IsCurrentOperation(operation)
                       ? owner
                       : nullptr;
        };

    // wxBookCtrlBase hides the old page before UpdateSelectedPage(). Capture
    // focus ownership now: Windows may redirect focus as a side effect of
    // hiding that HWND, which is too late to decide whether the new page
    // should inherit it.
    wxWindow * const focused = wxWindow::FindFocus();
    const wxWinUINotebookFocusTarget focusTarget =
        focused == this
            ? wxWinUINotebookFocusTarget::Notebook
            : focused && IsDescendant(focused)
                ? wxWinUINotebookFocusTarget::Page
                : wxWinUINotebookFocusTarget::None;
    wxWinUINotebookFocusGuard focusGuard(
        callbackState, focusTarget);

    std::unique_ptr<wxBookCtrlEvent> event;
    bool allowed = true;
    if ( flags & SetSelection_SendEvent )
    {
        // This factory is virtual. Adopt the returned event immediately, but
        // don't touch either it through this notebook or the notebook itself
        // until the selection transaction has been revalidated.
        event.reset(CreatePageChangingEvent());
        wxNotebook *owner = getCurrentOwner();
        if ( !owner )
            return oldSel;

        wxCHECK_MSG( event, oldSel,
                     wxT("CreatePageChangingEvent() returned null") );

        event->SetSelection(static_cast<int>(nPage));
        event->SetOldSelection(oldSel);
        event->SetEventObject(owner);

        const bool processed =
            owner->HandleWindowEvent(*event);

        owner = getCurrentOwner();
        if ( !owner )
            return oldSel;

        allowed = !processed || event->IsAllowed();

        // Preserve the pre-hide snapshot, but honour an event handler that
        // deliberately moved focus out of the notebook.
        if ( callbackState->GetFocusTarget() !=
             wxWinUINotebookFocusTarget::None )
        {
            wxWindow * const focusAfterEvent = wxWindow::FindFocus();
            if ( focusAfterEvent )
            {
                callbackState->SetFocusTarget(
                    focusAfterEvent == owner
                        ? wxWinUINotebookFocusTarget::Notebook
                        : owner->IsDescendant(focusAfterEvent)
                            ? wxWinUINotebookFocusTarget::Page
                            : wxWinUINotebookFocusTarget::None);
            }
        }
    }

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return oldSel;

    if ( !allowed )
    {
        // A user-driven TabView selection has already changed the peer. Put it
        // back only if this is still the newest selection transaction.
        if ( oldSel != wxNOT_FOUND &&
             static_cast<size_t>(oldSel) < owner->m_pages.size() )
        {
            owner->m_selection = oldSel;
            owner->UpdateSelectedPage(static_cast<size_t>(oldSel));
        }
        return oldSel;
    }

    if ( oldSel != wxNOT_FOUND )
    {
        wxWindow * const oldPage =
            owner->TryGetNonNullPage(static_cast<size_t>(oldSel));
        owner = getCurrentOwner();
        if ( !owner )
            return oldSel;

        if ( oldPage )
        {
            if ( static_cast<size_t>(oldSel) >= owner->m_pages.size() ||
                 owner->m_pages[static_cast<size_t>(oldSel)] != oldPage )
            {
                return oldSel;
            }

            owner->DoShowPage(oldPage, false);
        }

        owner = getCurrentOwner();
        if ( !owner )
            return oldSel;
    }

    wxWindow * const page = owner->TryGetNonNullPage(nPage);
    owner = getCurrentOwner();
    if ( !owner )
        return oldSel;

    if ( page )
    {
        const wxWeakRef<wxWindow> pageLifetime(page);
        const wxRect pageRect = owner->GetPageRect();
        owner = getCurrentOwner();
        wxWindow *livePage = pageLifetime.get();
        if ( !owner || !livePage ||
             nPage >= owner->m_pages.size() ||
             owner->m_pages[nPage] != livePage )
        {
            return oldSel;
        }

        livePage->SetSize(pageRect);
        owner = getCurrentOwner();
        livePage = pageLifetime.get();
        if ( !owner || !livePage ||
             nPage >= owner->m_pages.size() ||
             owner->m_pages[nPage] != livePage )
        {
            return oldSel;
        }

        owner->DoShowPage(livePage, true);
        owner = getCurrentOwner();
        wxWindow * const shownPage = pageLifetime.get();
        if ( !owner || !shownPage ||
             nPage >= owner->m_pages.size() ||
             owner->m_pages[nPage] != shownPage )
        {
            return oldSel;
        }
    }

    owner->m_selection = static_cast<int>(nPage);
    owner->UpdateSelectedPage(nPage);
    owner = getCurrentOwner();
    if ( !owner )
        return oldSel;

    if ( event )
    {
        owner->MakeChangedEvent(*event);
        owner = getCurrentOwner();
        if ( !owner ||
             owner->m_selection != static_cast<int>(nPage) )
        {
            return oldSel;
        }

        (void)owner->HandleWindowEvent(*event);
    }

    return oldSel;
}

bool wxNotebook::EnsureSelectionVisible(size_t selection)
{
    if ( !m_winui || !m_winui->tabView || !m_winui->callbackState ||
         selection >= m_winui->items.size() )
    {
        return false;
    }

    if ( UsesExtendedPeerSurface() )
    {
        m_winui->extendedLayoutValid = false;
        return LayoutExtendedPeerSurface();
    }

    const auto callbackState = m_winui->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const auto getIdentityOwner =
        [callbackState,
         callbackGeneration,
         impl,
         tabView]() -> wxNotebook *
        {
            wxNotebook * const owner =
                callbackState->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView
                       ? owner
                       : nullptr;
        };
    const MUXC::TabViewItem selectedItem = impl->items[selection];
    const auto getCurrentOwner =
        [&getIdentityOwner, selection]() -> wxNotebook *
        {
            wxNotebook *owner = getIdentityOwner();
            if ( !owner )
                return nullptr;

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            return owner &&
                           currentSelection ==
                               static_cast<int>(selection)
                       ? owner
                       : nullptr;
        };

    // The current peer value/layout must be committed once before the private
    // TabViewListView exists in the visual tree.
    impl->host.ForceRender();
    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return false;

    try
    {
        // SelectedIndex normally scrolls the header strip on its own, but the
        // WinUI TabView template does not do so reliably after a hot RTL
        // transition. Resolve the template by public base interface and bring
        // the exact selected item into view.
        wxWinUINotebookPeerMutationGuard mutation(callbackState);
        tabView.ApplyTemplate();
        tabView.UpdateLayout();
        if ( !getCurrentOwner() )
            return false;

        const wxWinUINotebookTemplateSnapshot templateSnapshot =
            wxWinUIInspectNotebookTemplate(
                tabView,
                [&getCurrentOwner]()
                {
                    return getCurrentOwner() != nullptr;
                });
        if ( !getCurrentOwner() )
            return false;
        if ( !templateSnapshot.complete || !templateSnapshot.tabList )
            return false;

        templateSnapshot.tabList.ScrollIntoView(
            selectedItem,
            MUXC::ScrollIntoViewAlignment::Leading);
        if ( !getCurrentOwner() )
            return false;
        templateSnapshot.tabList.UpdateLayout();
        if ( !getCurrentOwner() )
            return false;
        tabView.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView scroll selection", e);
        return false;
    }

    owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->m_winui->host.ForceRender();

    return getCurrentOwner() != nullptr;
}

void wxNotebook::UpdateSelectedPage(size_t newsel)
{
    if ( !m_winui || !m_winui->tabView || !m_winui->callbackState ||
         newsel >= m_winui->items.size() )
        return;

    const auto callbackState = m_winui->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const auto getIdentityOwner =
        [callbackState,
         callbackGeneration,
         impl,
         tabView]() -> wxNotebook *
        {
            wxNotebook * const owner =
                callbackState->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView
                       ? owner
                       : nullptr;
        };
    try
    {
        wxWinUINotebookPeerMutationGuard mutation(callbackState);
        impl->InvalidateGeometry();
        tabView.SelectedIndex(static_cast<int>(newsel));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView selection", e);
    }

    wxNotebook *owner = getIdentityOwner();
    if ( !owner )
        return;
    if ( owner->UsesExtendedPeerSurface() )
    {
        owner->UpdateExtendedPeerSelection(newsel);
        owner = getIdentityOwner();
        if ( !owner )
            return;
        owner->m_winui->extendedLayoutValid = false;
        (void)owner->LayoutExtendedPeerSurface();
    }
    else
    {
        (void)owner->EnsureSelectionVisible(newsel);
    }

    owner = getIdentityOwner();
    if ( !owner )
        return;
    const int currentSelection = owner->GetSelection();
    owner = getIdentityOwner();
    if ( !owner || currentSelection != static_cast<int>(newsel) )
        return;
    const size_t pageCount = owner->GetPageCount();
    owner = getIdentityOwner();
    if ( !owner || newsel >= pageCount )
    {
        return;
    }

    const wxWinUINotebookFocusTarget focusTarget =
        callbackState->GetFocusTarget();
    if ( focusTarget == wxWinUINotebookFocusTarget::Notebook )
    {
        owner->SetFocus();
    }
    else if ( focusTarget == wxWinUINotebookFocusTarget::Page )
    {
        if ( wxWindow *page = owner->m_pages[newsel] )
            page->SetFocus();
    }
}

void wxNotebook::OnTabViewSelectionChanged(int sel)
{
    if ( sel == wxNOT_FOUND || sel == m_selection ||
         sel < 0 || static_cast<size_t>(sel) >= m_pages.size() )
    {
        return;
    }

    if ( m_winui )
        m_winui->InvalidateGeometry();
    DoSetSelectionPreservingFocus(
        static_cast<size_t>(sel), SetSelection_SendEvent);
}

wxBookCtrlEvent* wxNotebook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_NOTEBOOK_PAGE_CHANGING, GetId());
}

void wxNotebook::MakeChangedEvent(wxBookCtrlEvent& event)
{
    event.SetEventType(wxEVT_NOTEBOOK_PAGE_CHANGED);
}

// ----------------------------------------------------------------------------
// page attributes
// ----------------------------------------------------------------------------

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    wxCHECK_MSG( nPage < m_pages.size(), false, wxT("invalid notebook page") );

    m_pageTexts[nPage] = strText;
    if ( m_winui )
    {
        m_winui->BumpModelRevision();
        (void)SyncPeerProjection();
    }

    return true;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    wxCHECK_MSG( nPage < m_pageTexts.GetCount(), wxString(), wxT("invalid notebook page") );
    return m_pageTexts[nPage];
}

int wxNotebook::GetPageImage(size_t nPage) const
{
    wxCHECK_MSG( nPage < m_pageImages.GetCount(), NO_IMAGE, wxT("invalid notebook page") );
    return m_pageImages[nPage];
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
    wxCHECK_MSG( nPage < m_pageImages.GetCount(), false, wxT("invalid notebook page") );

    m_pageImages[nPage] = nImage;
    if ( m_winui )
    {
        ++m_winui->nextIconGeneration;
        if ( m_winui->nextIconGeneration == 0 )
            ++m_winui->nextIconGeneration;
        m_winui->BumpModelRevision();
        (void)SyncPeerProjection();
    }
    return true;
}

bool wxNotebook::IsPeerProjectionCurrent(
    std::uint64_t revision,
    const std::shared_ptr<wxWinUINotebookCallbackState>& state,
    std::uint64_t callbackGeneration,
    wxWinUINotebookImpl *impl) const
{
    return state && m_winui && m_winui.get() == impl &&
           m_winui->callbackState == state && !m_winui->closed &&
           state->GetOwner(callbackGeneration) == this &&
           m_winui->modelRevision == revision;
}

bool wxNotebook::ProjectPeerModelPass(
    std::uint64_t revision,
    const std::shared_ptr<wxWinUINotebookCallbackState>& state,
    std::uint64_t callbackGeneration,
    wxWinUINotebookImpl *impl)
{
    auto getCurrentOwner =
        [revision, state, callbackGeneration, impl]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state ? state->GetOwner(callbackGeneration) : nullptr;
            return owner &&
                           owner->IsPeerProjectionCurrent(
                               revision, state, callbackGeneration, impl)
                       ? owner
                       : nullptr;
        };

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return false;

    const bool projectExtended = owner->UsesExtendedPeerSurface();
    if ( projectExtended && !owner->EnsureExtendedPeerSurface() )
        return false;
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    const size_t pageCount = owner->m_pages.size();
    if ( owner->m_pageTexts.GetCount() != pageCount ||
         owner->m_pageImages.GetCount() != pageCount ||
         impl->items.size() != pageCount ||
         impl->labels.size() != pageCount ||
         impl->icons.size() != pageCount ||
         impl->iconPixelSizes.size() != pageCount ||
         impl->iconGenerations.size() != pageCount ||
         (projectExtended && impl->extendedPeers.size() != pageCount) )
    {
        return false;
    }

    // LayoutUpdated is an implementation callback, not an application-visible
    // commit point. Suppress it while a projection pass is in flight; explicit
    // application/test re-entry is still handled by the revision checks below.
    wxWinUINotebookPeerMutationGuard mutation(state);

    try
    {
        for ( size_t page = 0; page < pageCount; ++page )
        {
            owner = getCurrentOwner();
            if ( !owner )
                return false;

            const wxString text = owner->m_pageTexts[page];
            const int imageId = owner->m_pageImages[page];
            const bool hasImages = owner->HasImages();
            const bool hasPadding = owner->m_hasTabPadding;
            const bool hasTabSize = owner->m_hasTabSize;
            const wxSize padding = owner->m_tabPadding;
            const wxSize tabSize = owner->m_tabSize;
            wxImageList * const imageList = owner->GetImageList();
            double scale = 0.0;
#ifdef WXWINUI_TEST_SUPPORT
            scale = impl->projectionScaleOverrideForTesting;
#endif
            if ( !std::isfinite(scale) || scale <= 0.0 )
            {
                // GetDPIScaleFactor() is virtual application code. Invoke it
                // once, then validate this exact model revision before the
                // first peer write.
                scale = owner->GetDPIScaleFactor();
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                if ( !std::isfinite(scale) || scale <= 0.0 )
                    scale = 1.0;
            }
            const std::uint64_t iconGeneration =
                impl->nextIconGeneration;

            // Strong WinRT handles keep the old peer safe if a nested model
            // mutation replaces the vectors. Revision checks prevent the old
            // pass from writing any subsequent property or cache entry.
            const MUXC::TabViewItem item = impl->items[page];
            const MUXC::TextBlock label = impl->labels[page];
            const MUXC::Image icon = impl->icons[page];
            const wxString displayText =
                wxWinUIRemoveMnemonics(text);
            const winrt::hstring xamlText =
                wxWinUIToHString(displayText);
            const MUXC::TabViewItem extendedItem =
                projectExtended
                    ? impl->extendedPeers[page].item
                    : MUXC::TabViewItem{ nullptr };
            const MUXC::TextBlock extendedLabel =
                projectExtended
                    ? impl->extendedPeers[page].label
                    : MUXC::TextBlock{ nullptr };
            const MUXC::Image extendedIcon =
                projectExtended
                    ? impl->extendedPeers[page].icon
                    : MUXC::Image{ nullptr };

            label.Text(xamlText);
            owner = getCurrentOwner();
            if ( !owner )
                return false;
            if ( extendedLabel )
            {
                extendedLabel.Text(xamlText);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
            }
#ifdef WXWINUI_TEST_SUPPORT
            owner->m_winui->InvokeProjectionHook(
                owner, wxWinUINotebookTestAccess::ProjectionPoint::LabelText);
#endif
            owner = getCurrentOwner();
            if ( !owner )
                return false;

            MUXA::AutomationProperties::SetName(item, xamlText);
            owner = getCurrentOwner();
            if ( !owner )
                return false;
            if ( extendedItem )
            {
                MUXA::AutomationProperties::SetName(
                    extendedItem, xamlText);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;

                // Preserve wx mnemonic semantics without drawing the marker.
                winrt::hstring accessKey;
                for ( size_t index = 0; index + 1 < text.length(); ++index )
                {
                    if ( text[index] != '&' )
                        continue;
                    if ( text[index + 1] == '&' )
                    {
                        ++index;
                        continue;
                    }
                    const wchar_t access =
                        static_cast<wchar_t>(text[index + 1]);
                    accessKey = winrt::hstring(&access, 1);
                    break;
                }
                extendedItem.AccessKey(accessKey);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
            }
#ifdef WXWINUI_TEST_SUPPORT
            owner->m_winui->InvokeProjectionHook(
                owner, wxWinUINotebookTestAccess::ProjectionPoint::AutomationName);
#endif
            owner = getCurrentOwner();
            if ( !owner )
                return false;

            wxBitmap bitmap;
            if ( imageId != NO_IMAGE && hasImages )
            {
                const wxWithImages::Images images =
                    owner->GetImages();
                if ( !images.empty() &&
                     imageId >= 0 &&
                     static_cast<size_t>(imageId) < images.size() )
                {
                    const wxBitmapBundle selectedBundle =
                        images[imageId];
                    struct SizePreference
                    {
                        wxSize size;
                        int count = 0;
                    };
                    std::vector<SizePreference> preferences;
                    std::vector<bool> preferredValid(
                        images.size(), false);
                    for ( size_t bundleIndex = 0;
                          bundleIndex < images.size();
                          ++bundleIndex )
                    {
                        const wxBitmapBundle& candidate =
                            images[bundleIndex];
                        if ( !candidate.IsOk() )
                            continue;

                        const wxSize preferred =
                            candidate.GetPreferredBitmapSizeAtScale(scale);
                        owner = getCurrentOwner();
                        if ( !owner )
                            return false;
                        if ( preferred.x <= 0 || preferred.y <= 0 )
                            continue;

                        preferredValid[bundleIndex] = true;
                        const auto existing = std::find_if(
                            preferences.begin(),
                            preferences.end(),
                            [&preferred](const SizePreference& entry)
                            {
                                return entry.size == preferred;
                            });
                        if ( existing != preferences.end() )
                        {
                            ++existing->count;
                        }
                        else
                        {
                            preferences.push_back({preferred, 1});
                        }
                    }

                    int maximumVotes = 0;
                    wxSize pixelSize;
                    for ( const SizePreference& preference :
                          preferences )
                    {
                        if ( preference.count > maximumVotes ||
                             (preference.count == maximumVotes &&
                              preference.size.y > pixelSize.y) )
                        {
                            maximumVotes = preference.count;
                            pixelSize = preference.size;
                        }
                    }

                    if ( pixelSize.x > 0 && pixelSize.y > 0 &&
                         preferredValid[imageId] &&
                         selectedBundle.IsOk() )
                    {
                        // Split wxBitmapBundle::GetBitmap() at its hidden
                        // GetDefaultSize()/implementation virtual boundaries.
                        // Either callback may destroy the notebook or publish
                        // a newer image revision.
                        const wxSize defaultSize =
                            selectedBundle.GetDefaultSize();
                        owner = getCurrentOwner();
                        if ( !owner )
                            return false;

                        wxBitmapBundleImpl * const bundleImpl =
                            selectedBundle.GetImpl();
                        if ( defaultSize.x > 0 &&
                             defaultSize.y > 0 &&
                             bundleImpl )
                        {
                            bitmap = bundleImpl->GetBitmap(pixelSize);
                            owner = getCurrentOwner();
                            if ( !owner )
                                return false;

                            if ( bitmap.IsOk() )
                            {
                                const double bitmapScale =
                                    static_cast<double>(pixelSize.y) /
                                    defaultSize.y;
                                if ( std::isfinite(bitmapScale) &&
                                     bitmapScale > 0.0 )
                                {
                                    // Match wxBitmapBundle::GetBitmap()
                                    // without crossing another application
                                    // callback after the last revalidation.
                                    bitmap.SetScaleFactor(bitmapScale);
                                }
                                else
                                {
                                    bitmap = wxBitmap();
                                }
                            }
                        }
                    }
                }
                else if ( imageList )
                {
                    const int imageCount = imageList->GetImageCount();
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
                    if ( imageId >= 0 && imageId < imageCount )
                    {
                        bitmap = imageList->GetBitmap(imageId);
                        owner = getCurrentOwner();
                        if ( !owner )
                            return false;
                    }
                }
            }
            owner = getCurrentOwner();
            if ( !owner )
                return false;

            wxSize appliedPixelSize;
            if ( bitmap.IsOk() )
            {
                if ( const auto source =
                         wxWinUIWriteableBitmapFromBitmap(bitmap) )
                {
                    const wxSize dipSize = bitmap.GetDIPSize();
                    icon.Source(source);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    owner->m_winui->InvokeProjectionHook(
                        owner, wxWinUINotebookTestAccess::ProjectionPoint::IconSource);
#endif
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;

                    icon.Width(static_cast<double>(dipSize.x));
                    if ( !getCurrentOwner() )
                        return false;
                    icon.Height(static_cast<double>(dipSize.y));
                    if ( !getCurrentOwner() )
                        return false;
                    icon.Visibility(MUX::Visibility::Visible);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
                    appliedPixelSize = bitmap.GetSize();
                }
                else
                {
                    icon.Source(nullptr);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    owner->m_winui->InvokeProjectionHook(
                        owner, wxWinUINotebookTestAccess::ProjectionPoint::IconSource);
#endif
                    if ( !getCurrentOwner() )
                        return false;
                    icon.Visibility(MUX::Visibility::Collapsed);
                    if ( !getCurrentOwner() )
                        return false;
                }
            }
            else
            {
                icon.Source(nullptr);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
#ifdef WXWINUI_TEST_SUPPORT
                owner->m_winui->InvokeProjectionHook(
                    owner, wxWinUINotebookTestAccess::ProjectionPoint::IconSource);
#endif
                if ( !getCurrentOwner() )
                    return false;
                icon.Visibility(MUX::Visibility::Collapsed);
                if ( !getCurrentOwner() )
                    return false;
            }

            if ( extendedIcon )
            {
                const auto source = icon.Source();
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                extendedIcon.Source(source);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                extendedIcon.Width(icon.Width());
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                extendedIcon.Height(icon.Height());
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                extendedIcon.Visibility(icon.Visibility());
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
            }

            if ( hasPadding )
            {
                MUX::Thickness xamlPadding{};
                xamlPadding.Left = xamlPadding.Right =
                    static_cast<double>(padding.x) / scale;
                xamlPadding.Top = xamlPadding.Bottom =
                    static_cast<double>(padding.y) / scale;
                item.Padding(xamlPadding);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                if ( extendedItem )
                {
                    extendedItem.Padding(xamlPadding);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
                }
#ifdef WXWINUI_TEST_SUPPORT
                owner->m_winui->InvokeProjectionHook(
                    owner, wxWinUINotebookTestAccess::ProjectionPoint::TabPadding);
#endif
                if ( !getCurrentOwner() )
                    return false;
            }

            if ( hasTabSize )
            {
                const double width =
                    static_cast<double>(tabSize.x) / scale;
                const double height =
                    static_cast<double>(tabSize.y) / scale;
                item.MinWidth(0.0);
                if ( !getCurrentOwner() )
                    return false;
                item.MaxWidth(width);
                if ( !getCurrentOwner() )
                    return false;
                item.MinWidth(width);
                if ( !getCurrentOwner() )
                    return false;
                item.MinHeight(0.0);
                if ( !getCurrentOwner() )
                    return false;
                item.MaxHeight(height);
                if ( !getCurrentOwner() )
                    return false;
                item.MinHeight(height);
                if ( !getCurrentOwner() )
                    return false;
                item.Width(width);
                if ( !getCurrentOwner() )
                    return false;
                item.Height(height);
                owner = getCurrentOwner();
                if ( !owner )
                    return false;
                if ( extendedItem )
                {
                    extendedItem.MinWidth(0.0);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.MaxWidth(width);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.MinWidth(width);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.MinHeight(0.0);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.MaxHeight(height);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.MinHeight(height);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.Width(width);
                    if ( !getCurrentOwner() )
                        return false;
                    extendedItem.Height(height);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;
                }
#ifdef WXWINUI_TEST_SUPPORT
                owner->m_winui->InvokeProjectionHook(
                    owner, wxWinUINotebookTestAccess::ProjectionPoint::TabSize);
#endif
                if ( !getCurrentOwner() )
                    return false;
            }

            owner = getCurrentOwner();
            if ( !owner )
                return false;
            impl->iconPixelSizes[page] = appliedPixelSize;
            impl->iconGenerations[page] = iconGeneration;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView model projection", e);
        return false;
    }

    return getCurrentOwner() != nullptr;
}

bool wxNotebook::SyncPeerProjection()
{
    if ( !m_winui || m_winui->closed || !m_winui->callbackState )
        return false;

    wxWinUINotebookImpl * const initialImpl = m_winui.get();
    const auto state = initialImpl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();

    initialImpl->projectionRequested = true;
    if ( initialImpl->projectionInProgress )
        return true;

    initialImpl->projectionInProgress = true;
    constexpr unsigned MaxSynchronousPasses = 8;
    for ( unsigned pass = 0; pass < MaxSynchronousPasses; ++pass )
    {
        wxNotebook *owner = state->GetOwner(callbackGeneration);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != initialImpl ||
             owner->m_winui->callbackState != state )
        {
            return false;
        }

        initialImpl->projectionRequested = false;
        const std::uint64_t revision = initialImpl->modelRevision;
        const bool projected = owner->ProjectPeerModelPass(
            revision, state, callbackGeneration, initialImpl);

        owner = state->GetOwner(callbackGeneration);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != initialImpl ||
             owner->m_winui->callbackState != state )
        {
            return false;
        }

        if ( projected &&
             initialImpl->modelRevision == revision &&
             !initialImpl->projectionRequested )
        {
            initialImpl->projectionInProgress = false;
            initialImpl->projectionDeferredAttempts = 0;
            initialImpl->projectionWarningIssued = false;
            initialImpl->extendedLayoutValid = false;
            if ( owner->UsesExtendedPeerSurface() &&
                 !owner->LayoutExtendedPeerSurface() )
            {
                return false;
            }
            return true;
        }

        initialImpl->projectionRequested = true;
    }

    wxNotebook *owner = state->GetOwner(callbackGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != initialImpl ||
         owner->m_winui->callbackState != state )
    {
        return false;
    }

    initialImpl->projectionInProgress = false;
    const std::uint64_t currentRevision =
        initialImpl->modelRevision;
    // Record the newest revision for diagnostics, but deliberately keep the
    // attempt count: revisions produced from inside this transaction share
    // one bounded retry budget.
    initialImpl->projectionDeferredRevision = currentRevision;

    if ( initialImpl->projectionDeferredAttempts == 0 &&
         !initialImpl->projectionReplayScheduled && wxTheApp )
    {
        initialImpl->projectionDeferredAttempts = 1;
        initialImpl->projectionReplayScheduled = true;
        wxTheApp->CallAfter(
            [state, callbackGeneration]()
            {
                wxNotebook * const liveOwner =
                    state->GetOwner(callbackGeneration);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui->callbackState != state )
                {
                    return;
                }

                liveOwner->m_winui->projectionReplayScheduled = false;
                (void)liveOwner->SyncPeerProjection();
            });
    }
    else if ( !initialImpl->projectionWarningIssued )
    {
        initialImpl->projectionWarningIssued = true;
        initialImpl->projectionRequested = false;
        wxLogWarning(
            "wxWinUI notebook peer projection did not converge after "
            "bounded retries; it will resume on the next model change");
    }

    return false;
}

void wxNotebook::OnImagesChanged()
{
    if ( !m_winui )
        return;

    DoInvalidateBestSize();
    ++m_winui->nextIconGeneration;
    if ( m_winui->nextIconGeneration == 0 )
        ++m_winui->nextIconGeneration;
    m_winui->BumpModelRevision();
    (void)SyncPeerProjection();
}

void wxNotebook::OnDPIChanged(wxDPIChangedEvent& event)
{
    m_tabStripHeightDIP = -1;
    if ( !m_winui || !m_winui->callbackState )
    {
        event.Skip();
        return;
    }

    wxWinUINotebookImpl * const impl = m_winui.get();
#ifdef WXWINUI_TEST_SUPPORT
    impl->projectionScaleOverrideForTesting = 0.0;
#endif
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    if ( !UsesExtendedPeerSurface() )
    {
        const int fallbackHeight = FromDIP(DefaultTabStripHeightDIP);
        impl->host.SetBridgeHeightLimit(fallbackHeight);
    }
    else
    {
        impl->extendedLayoutValid = false;
    }

    wxNotebook * const owner = state->GetOwner(callbackGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        event.Skip();
        return;
    }

    // wxBookCtrlBase's already-bound DPI handler calls OnImagesChanged() after
    // this handler only when an image collection exists. With no images we
    // still must reproject pixel-backed padding/tab-size state. Publish Skip
    // first because the projection below may synchronously destroy this.
    const bool baseWillProjectImages = HasImages();
    event.Skip();
    if ( !baseWillProjectImages )
        OnImagesChanged();
}

void wxNotebook::SetPadding(const wxSize& padding)
{
    wxCHECK_RET( padding.x >= 0 && padding.y >= 0,
                 "negative wxNotebook tab padding is invalid" );

    m_tabPadding = padding;
    m_hasTabPadding = true;
    DoInvalidateBestSize();
    if ( m_winui )
    {
        m_winui->BumpModelRevision();
        (void)SyncPeerProjection();
    }
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    wxCHECK_RET( sz.x > 0 && sz.y > 0,
                 "wxNotebook tab size must be positive" );

    m_tabSize = sz;
    m_hasTabSize = true;
    DoInvalidateBestSize();
    if ( m_winui )
    {
        m_winui->BumpModelRevision();
        (void)SyncPeerProjection();
    }
}

bool wxNotebook::MSWOnEffectiveLayoutDirectionChanged()
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->tabView )
        return true;

    m_winui->InvalidateGeometry();
    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const auto getIdentityOwner =
        [state,
         callbackGeneration,
         impl,
         tabView]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView
                       ? owner
                       : nullptr;
        };
    wxNotebook *owner = getIdentityOwner();
    if ( !owner )
        return false;
    const wxLayoutDirection effectiveDirection =
        owner->GetLayoutDirection();
    owner = getIdentityOwner();
    if ( !owner )
        return false;

    try
    {
        wxWinUINotebookPeerMutationGuard mutation(state);
        tabView.FlowDirection(
            effectiveDirection == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView layout direction", e);
        return false;
    }

    owner = getIdentityOwner();
    if ( owner )
    {
        owner->m_winui->InvalidateGeometry();
        const int selection = owner->GetSelection();
        owner = getIdentityOwner();
        if ( !owner )
            return false;
        if ( selection >= 0 )
        {
            const size_t selectedPage =
                static_cast<size_t>(selection);
            if ( selectedPage >= owner->m_winui->items.size() )
                return false;
            return owner->EnsureSelectionVisible(
                selectedPage);
        }
        return true;
    }
    return false;
}

#ifdef WXWINUI_TEST_SUPPORT
bool
wxWinUINotebookTestAccess::IsPeerRTL(
    const wxNotebook& control)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->m_winui || !notebook->m_winui->tabView )
        return false;

    try
    {
        if ( notebook->UsesExtendedPeerSurface() &&
             !notebook->m_winui->extendedPeers.empty() )
        {
            return notebook->m_winui->extendedPeers.front().item.FlowDirection() ==
                   MUX::FlowDirection::RightToLeft;
        }
        return notebook->m_winui->tabView.FlowDirection() ==
               MUX::FlowDirection::RightToLeft;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

bool wxNotebook::UsesExtendedPeerSurface() const
{
    const long style = GetWindowStyleFlag();
    const long alignment = style & wxBK_ALIGN_MASK;
    return alignment != wxNB_TOP ||
           (style & (wxNB_MULTILINE | wxNB_NOPAGETHEME)) != 0;
}

#ifdef WXWINUI_TEST_SUPPORT
bool
wxWinUINotebookTestAccess::IsUsingExtendedSurface(
    const wxNotebook& control)
{
    const wxNotebook * const notebook = &control;

    return notebook->UsesExtendedPeerSurface();
}

bool
wxWinUINotebookTestAccess::IsPeerTabStop(
    const wxNotebook& control,
    size_t page)
{
    const wxNotebook * const notebook = &control;

    if ( !notebook->UsesExtendedPeerSurface() )
        return false;

    wxNotebook * const self = const_cast<wxNotebook *>(notebook);
    if ( !self->EnsureExtendedPeerSurface() || !notebook->m_winui ||
         !notebook->m_winui->callbackState ||
         page >= notebook->m_winui->extendedPeers.size() )
    {
        return false;
    }

    const auto state = notebook->m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = notebook->m_winui.get();
    const std::uint64_t surfaceGeneration =
        impl->extendedSurfaceGeneration;
    const MUXC::TabViewItem item = impl->extendedPeers[page].item;
    try
    {
        const bool isTabStop = item.IsTabStop();
        wxNotebook * const owner =
            state->GetOwner(callbackGeneration);
        return owner == notebook && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       impl->extendedSurfaceGeneration ==
                           surfaceGeneration
                   ? isTabStop
                   : false;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool
wxWinUINotebookTestAccess::InvokeTabOverflow(
    wxNotebook& control,
    bool forward)
{
    wxNotebook * const notebook = &control;

    if ( !notebook->UsesExtendedPeerSurface() || !notebook->LayoutExtendedPeerSurface() ||
         !notebook->m_winui || !notebook->m_winui->callbackState )
    {
        return false;
    }

    const auto state = notebook->m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = notebook->m_winui.get();
    const MUXC::Button button =
        forward ? impl->extendedForwardButton
                : impl->extendedBackButton;
    if ( !button )
        return false;

    const auto isCurrent = [&]()
    {
        wxNotebook * const owner =
            state->GetOwner(callbackGeneration);
        return owner == notebook && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == state &&
               !owner->m_winui->closed;
    };

    try
    {
        if ( button.Visibility() != MUX::Visibility::Visible )
            return false;
        if ( !isCurrent() || !button.IsEnabled() )
            return false;
        if ( !isCurrent() )
            return false;

        // Exercise the real Invoke provider used by UIA and accessibility,
        // rather than calling the layout callback directly.
        MUXA::Peers::ButtonAutomationPeer peer(button);
        peer.Invoke();
        return isCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

bool wxNotebook::EnsureExtendedPeerSurface()
{
    if ( !m_winui || !m_winui->callbackState ||
         !m_winui->extendedRoot || !m_winui->extendedTabCanvas ||
         !m_winui->extendedBackButton ||
         !m_winui->extendedForwardButton || m_winui->closed )
    {
        return false;
    }

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    const std::uint64_t modelRevision = impl->modelRevision;
    const auto getIdentityOwner =
        [state, callbackGeneration, impl]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed
                       ? owner
                       : nullptr;
        };
    const auto getOwner =
        [getIdentityOwner, modelRevision]() -> wxNotebook *
        {
            wxNotebook * const owner = getIdentityOwner();
            return owner && owner->m_winui->modelRevision == modelRevision
                       ? owner
                       : nullptr;
        };

    bool peersCurrent = impl->extendedPeers.size() == impl->items.size();
    for ( size_t page = 0;
          peersCurrent && page < impl->items.size();
          ++page )
    {
        peersCurrent =
            impl->extendedPeers[page].logicalItem &&
            winrt::get_unknown(
                impl->extendedPeers[page].logicalItem) ==
                winrt::get_unknown(impl->items[page]);
    }
    if ( peersCurrent &&
         impl->extendedAttachedGeneration ==
             impl->extendedSurfaceGeneration )
    {
        return true;
    }

    // A visual-tree mutation may synchronously run layout or application
    // callbacks. Never let a nested pass clear/append the same Children
    // collection while the outer transaction is still attaching it.
    if ( impl->extendedAttachmentInProgress )
        return false;

    std::uint64_t surfaceGeneration = impl->extendedSurfaceGeneration;

    std::vector<wxWinUINotebookSurfacePeer> staged;
    if ( !peersCurrent )
    {
        surfaceGeneration = impl->extendedSurfaceGeneration + 1;
        if ( surfaceGeneration == 0 )
            ++surfaceGeneration;

        try
        {
            staged.reserve(impl->items.size());
            for ( size_t page = 0; page < impl->items.size(); ++page )
            {
                wxNotebook *owner = getOwner();
                if ( !owner )
                    return false;

            wxWinUINotebookSurfacePeer peer;
            peer.logicalItem = impl->items[page];
            peer.item = MUXC::TabViewItem();
            peer.item.IsClosable(false);
            peer.item.IsTabStop(
                static_cast<int>(page) == owner->m_selection);
            // The explicit item rectangle already includes wx tab padding.
            // Keep the content presenter from applying a second inset so the
            // realized icon/label bounds match the published hit geometry.
            peer.item.Padding(MUX::Thickness{});
            peer.item.HorizontalContentAlignment(
                MUX::HorizontalAlignment::Center);
            peer.item.VerticalContentAlignment(
                MUX::VerticalAlignment::Center);

            peer.header = MUXC::StackPanel();
            peer.header.Spacing(DefaultHeaderSpacingDIP);
            peer.header.HorizontalAlignment(
                MUX::HorizontalAlignment::Center);
            peer.header.VerticalAlignment(
                MUX::VerticalAlignment::Center);
            peer.header.IsHitTestVisible(false);
            MUXA::AutomationProperties::SetAccessibilityView(
                peer.header, MUXA::Peers::AccessibilityView::Raw);

            peer.icon = MUXC::Image();
            peer.icon.Visibility(MUX::Visibility::Collapsed);
            peer.icon.HorizontalAlignment(
                MUX::HorizontalAlignment::Left);
            peer.icon.VerticalAlignment(
                MUX::VerticalAlignment::Top);
            peer.icon.IsHitTestVisible(false);
            MUXA::AutomationProperties::SetAccessibilityView(
                peer.icon, MUXA::Peers::AccessibilityView::Raw);

            peer.label = MUXC::TextBlock();
            peer.label.HorizontalAlignment(
                MUX::HorizontalAlignment::Left);
            peer.label.VerticalAlignment(
                MUX::VerticalAlignment::Top);
            peer.label.IsHitTestVisible(false);
            MUXA::AutomationProperties::SetAccessibilityView(
                peer.label, MUXA::Peers::AccessibilityView::Raw);

            // Content is used intentionally: a standalone TabViewItem keeps
            // its SelectorItem/Tab automation peer while presenting this
            // exact header without depending on TabView's private strip.
            peer.item.Content(peer.header);
            peer.item.IsSelected(
                static_cast<int>(page) == owner->m_selection);
            MUXA::AutomationProperties::SetLocalizedControlType(
                peer.item, wxWinUIToHString(_("tab")));
            MUXA::AutomationProperties::SetPositionInSet(
                peer.item, static_cast<int>(page + 1));
            MUXA::AutomationProperties::SetSizeOfSet(
                peer.item, static_cast<int>(impl->items.size()));
            if ( !getOwner() )
                return false;

            const MUXC::TabViewItem logicalItem = peer.logicalItem;
            const auto activate =
                [state,
                 callbackGeneration,
                 impl,
                 surfaceGeneration,
                 logicalItem](bool moveFocus)
                {
                    wxNotebook *owner =
                        state->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->closed ||
                         owner->m_winui->extendedSurfaceGeneration !=
                             surfaceGeneration )
                    {
                        return;
                    }

                    size_t page = owner->m_winui->items.size();
                    for ( size_t candidate = 0;
                          candidate < owner->m_winui->items.size();
                          ++candidate )
                    {
                        if ( wxWinUINotebookObjectIdentity(
                                 owner->m_winui->items[candidate]) ==
                             wxWinUINotebookObjectIdentity(logicalItem) )
                        {
                            page = candidate;
                            break;
                        }
                    }
                    if ( page >= owner->m_pages.size() )
                        return;

                    owner->OnTabViewSelectionChanged(
                        static_cast<int>(page));
                    owner = state->GetOwner(callbackGeneration);
                    if ( !moveFocus || !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->extendedSurfaceGeneration !=
                             surfaceGeneration ||
                         page >= owner->m_winui->extendedPeers.size() )
                    {
                        return;
                    }
                    try
                    {
                        owner->m_winui->extendedPeers[page].item.Focus(
                            MUX::FocusState::Keyboard);
                    }
                    catch ( const winrt::hresult_error& )
                    {
                    }
                };

            // SelectionItemAutomationPeer::Select() changes IsSelected on a
            // SelectorItem. These standalone peers have no Selector parent,
            // so bridge that real UIA command back to the wx selection
            // transaction. Projection writes are guarded below and therefore
            // cannot echo a second wx event through this callback.
            peer.isSelectedCallbackToken =
                peer.item.RegisterPropertyChangedCallback(
                    MUXCP::SelectorItem::IsSelectedProperty(),
                    [state,
                     callbackGeneration,
                     impl,
                     surfaceGeneration,
                     activate](
                        const MUX::DependencyObject& sender,
                        const MUX::DependencyProperty&)
                    {
                        if ( state->IsPeerMutationInProgress() )
                            return;

                        wxNotebook * const owner =
                            state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->closed ||
                             owner->m_winui->extendedSurfaceGeneration !=
                                 surfaceGeneration )
                        {
                            return;
                        }

                        const MUXCP::SelectorItem item =
                            sender.try_as<MUXCP::SelectorItem>();
                        if ( item && item.IsSelected() )
                            activate(false);
                    });

            peer.tappedToken = peer.item.Tapped(
                [activate](
                    const winrt::Windows::Foundation::IInspectable&,
                    const MUXI::TappedRoutedEventArgs& event)
                {
                    event.Handled(true);
                    activate(false);
                });

            peer.accessKeyInvokedToken = peer.item.AccessKeyInvoked(
                [activate](
                    const winrt::Windows::Foundation::IInspectable&,
                    const MUXI::AccessKeyInvokedEventArgs& event)
                {
                    event.Handled(true);
                    activate(true);
                });

            peer.previewKeyDownToken = peer.item.PreviewKeyDown(
                [state,
                 callbackGeneration,
                 impl,
                 surfaceGeneration,
                 logicalItem,
                 activate](
                    const winrt::Windows::Foundation::IInspectable&,
                    const MUXI::KeyRoutedEventArgs& event)
                {
                    const WS::VirtualKey key = event.Key();
                    if ( key == WS::VirtualKey::Enter ||
                         key == WS::VirtualKey::Space )
                    {
                        event.Handled(true);
                        activate(true);
                        return;
                    }

                    wxNotebook *owner =
                        state->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->extendedSurfaceGeneration !=
                             surfaceGeneration ||
                         owner->m_pages.empty() )
                    {
                        return;
                    }

                    size_t current = owner->m_winui->items.size();
                    for ( size_t candidate = 0;
                          candidate < owner->m_winui->items.size();
                          ++candidate )
                    {
                        if ( wxWinUINotebookObjectIdentity(
                                 owner->m_winui->items[candidate]) ==
                             wxWinUINotebookObjectIdentity(logicalItem) )
                        {
                            current = candidate;
                            break;
                        }
                    }
                    if ( current >= owner->m_pages.size() )
                        return;

                    const wxDirection orientation =
                        owner->GetTabOrientation();
                    const bool rtl =
                        owner->GetLayoutDirection() ==
                            wxLayout_RightToLeft;
                    int delta = 0;
                    if ( orientation == wxTOP ||
                         orientation == wxBOTTOM )
                    {
                        if ( key == WS::VirtualKey::Left )
                            delta = rtl ? 1 : -1;
                        else if ( key == WS::VirtualKey::Right )
                            delta = rtl ? -1 : 1;
                    }
                    else
                    {
                        if ( key == WS::VirtualKey::Up )
                            delta = -1;
                        else if ( key == WS::VirtualKey::Down )
                            delta = 1;
                    }

                    size_t target = current;
                    if ( key == WS::VirtualKey::Home )
                        target = 0;
                    else if ( key == WS::VirtualKey::End )
                        target = owner->m_pages.size() - 1;
                    else if ( delta != 0 )
                    {
                        const int count =
                            static_cast<int>(owner->m_pages.size());
                        target = static_cast<size_t>(
                            (static_cast<int>(current) + delta + count) %
                            count);
                    }
                    else
                    {
                        return;
                    }

                    event.Handled(true);
                    owner->OnTabViewSelectionChanged(
                        static_cast<int>(target));
                    owner = state->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->extendedSurfaceGeneration !=
                             surfaceGeneration ||
                         target >= owner->m_winui->extendedPeers.size() )
                    {
                        return;
                    }
                    try
                    {
                        owner->m_winui->extendedPeers[target].item.Focus(
                            MUX::FocusState::Keyboard);
                    }
                    catch ( const winrt::hresult_error& )
                    {
                    }
                });

                staged.push_back(std::move(peer));
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI extended notebook peer creation", e);
            return false;
        }
        catch ( const std::exception& )
        {
            return false;
        }

        wxNotebook *owner = getOwner();
        if ( !owner )
            return false;

        // The Canvas still owns the previous peers until attachment below
        // clears it. Remove their routed delegates before dropping our last
        // token ledger, otherwise a hot item rebuild retains callbackState
        // even after the old visual tree is detached.
        for ( auto& peer : impl->extendedPeers )
            wxWinUIRevokeNotebookSurfacePeerHandlers(peer);
        impl->extendedSurfaceGeneration = surfaceGeneration;
        impl->extendedAttachedGeneration = 0;
        impl->extendedPeers.swap(staged);
        impl->extendedLabelMetricTexts.clear();
        impl->extendedLabelNaturalPixelSizes.clear();
        impl->extendedLabelMetricPending.clear();
        impl->extendedLabelMetricProbeEpochs.clear();
        impl->extendedLabelMetricRelayoutRequested.clear();
        impl->extendedLabelMetricScale = 0.0;
        impl->extendedLayoutValid = false;
    }

    wxNotebook *owner = getOwner();
    if ( !owner )
        return false;

    impl->extendedAttachmentInProgress = true;
    const auto finishAttachment =
        [getIdentityOwner, getOwner, surfaceGeneration](bool attached)
        {
            wxNotebook * const identityOwner = getIdentityOwner();
            if ( !identityOwner )
                return false;

            wxWinUINotebookImpl * const currentImpl =
                identityOwner->m_winui.get();
            currentImpl->extendedAttachmentInProgress = false;
            if ( attached && getOwner() &&
                 currentImpl->extendedSurfaceGeneration ==
                     surfaceGeneration )
            {
                currentImpl->extendedAttachedGeneration =
                    surfaceGeneration;
                return true;
            }
            return false;
        };

    try
    {
        const auto rootChildren = impl->extendedRoot.Children();
        rootChildren.Clear();
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);
        rootChildren.Append(impl->extendedPageChrome);
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);
        rootChildren.Append(impl->extendedTabCanvas);
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);
        rootChildren.Append(impl->extendedBackButton);
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);
        rootChildren.Append(impl->extendedForwardButton);
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);

        const auto tabChildren = impl->extendedTabCanvas.Children();
        tabChildren.Clear();
        owner = getOwner();
        if ( !owner )
            return finishAttachment(false);
        for ( const auto& peer : impl->extendedPeers )
        {
            tabChildren.Append(peer.item);
            owner = getOwner();
            if ( !owner )
                return finishAttachment(false);
            tabChildren.Append(peer.icon);
            owner = getOwner();
            if ( !owner )
                return finishAttachment(false);
            tabChildren.Append(peer.label);
            owner = getOwner();
            if ( !owner )
                return finishAttachment(false);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI extended notebook peer attachment", e);
        return finishAttachment(false);
    }

    return finishAttachment(true);
}

void wxNotebook::QueueExtendedPeerLayoutContinuation()
{
    if ( !m_winui || !m_winui->callbackState || m_winui->closed ||
         !UsesExtendedPeerSurface() || !m_winui->extendedRoot )
    {
        return;
    }

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    const std::uint64_t layoutEpoch = impl->extendedLayoutEpoch;
    const bool hasReadyMetric =
        impl->extendedLabelMetricPending.size() ==
            impl->extendedLabelMetricProbeEpochs.size() &&
        impl->extendedLabelMetricPending.size() ==
            impl->extendedLabelMetricRelayoutRequested.size() &&
        [&]()
        {
            for ( size_t page = 0;
                  page < impl->extendedLabelMetricPending.size();
                  ++page )
            {
                if ( impl->extendedLabelMetricPending[page] &&
                     impl->extendedLabelMetricProbeEpochs[page] !=
                         layoutEpoch )
                {
                    return true;
                }
            }
            return false;
        }();
    if ( !hasReadyMetric )
        return;

    // Several LayoutUpdated notifications can belong to the same XAML frame.
    // They all advance the requested epoch, but exactly one queued callback
    // consumes the newest one after the current notification has unwound.
    impl->extendedLayoutContinuationEpoch = layoutEpoch;
    if ( impl->extendedLayoutContinuationScheduled )
        return;

    MUXD::DispatcherQueue queue{ nullptr };
    try
    {
        queue = impl->extendedRoot.DispatcherQueue();
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }
    if ( !queue )
        return;

    if ( ++impl->extendedLayoutContinuationGeneration == 0 )
        ++impl->extendedLayoutContinuationGeneration;
    const std::uint64_t continuationGeneration =
        impl->extendedLayoutContinuationGeneration;
    impl->extendedLayoutContinuationScheduled = true;

    const std::weak_ptr<wxWinUINotebookCallbackState> weakState(state);
    bool queued = false;
    try
    {
        queued = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [weakState,
             callbackGeneration,
             continuationGeneration,
             impl]() noexcept
            {
                const auto liveState = weakState.lock();
                if ( !liveState )
                    return;

                wxNotebook *owner =
                    liveState->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != liveState ||
                     owner->m_winui->closed ||
                     impl->extendedLayoutContinuationGeneration !=
                         continuationGeneration )
                {
                    return;
                }

                const std::uint64_t requestedEpoch =
                    impl->extendedLayoutContinuationEpoch;
                impl->extendedLayoutContinuationScheduled = false;
                impl->extendedLayoutContinuationEpoch = 0;
                if ( requestedEpoch == 0 ||
                     requestedEpoch != impl->extendedLayoutEpoch )
                {
                    return;
                }

                if ( ++impl->extendedLayoutContinuationCount == 0 )
                    ++impl->extendedLayoutContinuationCount;

                // LayoutUpdated is shared by the connected tree: its first
                // notification after ClearValue() can belong to neighbouring
                // chrome while the label still exposes its preceding pinned
                // ActualWidth. Never force UpdateLayout here: doing so can
                // leave a stowed XAML error which fail-fasts on the next tree.
                // Instead, request one ordinary asynchronous measure pass per
                // probe and fence that request at this completed epoch. A
                // later LayoutUpdated is then the only event allowed to make
                // the intrinsic metric consumable.
                bool requestedRelayout = false;
                try
                {
                    if ( impl->extendedLabelMetricPending.size() !=
                             impl->extendedLabelMetricProbeEpochs.size() ||
                         impl->extendedLabelMetricPending.size() !=
                             impl->extendedLabelMetricRelayoutRequested.size() ||
                         impl->extendedLabelMetricPending.size() !=
                             impl->extendedPeers.size() )
                    {
                        return;
                    }

                    for ( size_t page = 0;
                          page < impl->extendedLabelMetricPending.size();
                          ++page )
                    {
                        if ( !impl->extendedLabelMetricPending[page] ||
                             impl->extendedLabelMetricProbeEpochs[page] ==
                                 requestedEpoch ||
                             impl->extendedLabelMetricRelayoutRequested[page] )
                        {
                            continue;
                        }

                        impl->extendedLabelMetricRelayoutRequested[page] = true;
                        impl->extendedLabelMetricProbeEpochs[page] =
                            requestedEpoch;
                        impl->extendedPeers[page].label.InvalidateMeasure();
                        requestedRelayout = true;

                        owner = liveState->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != liveState ||
                             owner->m_winui->closed ||
                             impl->extendedLayoutContinuationGeneration !=
                                 continuationGeneration )
                        {
                            return;
                        }
                    }
                }
                catch ( ... )
                {
                    // A dispatcher delegate must never unwind through WinRT.
                    return;
                }

                if ( requestedRelayout )
                    return;

                try
                {
                    (void)owner->LayoutExtendedPeerSurface();
                }
                catch ( ... )
                {
                    // A dispatcher delegate must never unwind through WinRT.
                }
            });
    }
    catch ( const winrt::hresult_error& )
    {
    }
    catch ( const std::exception& )
    {
    }

    if ( queued )
        return;

    wxNotebook * const owner = state->GetOwner(callbackGeneration);
    if ( owner == this && owner->m_winui &&
         owner->m_winui.get() == impl &&
         owner->m_winui->callbackState == state &&
         impl->extendedLayoutContinuationGeneration ==
             continuationGeneration )
    {
        impl->extendedLayoutContinuationScheduled = false;
        impl->extendedLayoutContinuationEpoch = 0;
    }
}

bool wxNotebook::LayoutExtendedPeerSurface()
{
    if ( !UsesExtendedPeerSurface() || !m_winui ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookGeometryGuard geometryGuard(state);
    if ( !geometryGuard.IsAcquired() )
    {
        // A XAML/native size callback can ask for geometry while this same
        // surface is between setters. It may consume the last complete
        // publication, but must never start another measure/arrange wave.
        wxNotebook * const owner =
            state->GetOwner(callbackGeneration);
        return owner == this && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == state &&
               !owner->m_winui->closed &&
               (impl->extendedLayoutValid ||
                (impl->extendedLayoutInProgressStyleRevision != 0 &&
                 impl->styleRevision !=
                     impl->extendedLayoutInProgressStyleRevision));
    }

    // Publish the writer that owns this guard before any XAML operation can
    // synchronously re-enter SetWindowStyleFlag(). A strictly newer writer is
    // then accepted as deferred; the outer pass rejects itself at getOwner().
    impl->extendedLayoutInProgressStyleRevision = impl->styleRevision;

    if ( !EnsureExtendedPeerSurface() )
        return false;
    wxNotebook * const guardedOwner =
        state->GetOwner(callbackGeneration);
    if ( guardedOwner != this || !guardedOwner->m_winui ||
         guardedOwner->m_winui.get() != impl ||
         guardedOwner->m_winui->callbackState != state ||
         guardedOwner->m_winui->closed )
    {
        return false;
    }

    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const long style = GetWindowStyleFlag();
    const wxSize clientSize = GetClientSize();
    const wxLayoutDirection layoutDirection = GetLayoutDirection();
    double scale = GetDPIScaleFactor();
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;

    const auto getOwner =
        [state,
         callbackGeneration,
         impl,
         modelRevision,
         layoutRevision,
         style,
         clientSize,
         layoutDirection,
         scale]() -> wxNotebook *
        {
            wxNotebook *owner = state->GetOwner(callbackGeneration);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->closed ||
                 owner->m_winui->modelRevision != modelRevision ||
                 owner->m_winui->layoutRevision != layoutRevision ||
                 owner->GetWindowStyleFlag() != style ||
                 owner->GetClientSize() != clientSize ||
                 owner->GetLayoutDirection() != layoutDirection )
            {
                return nullptr;
            }

            double currentScale = owner->GetDPIScaleFactor();
            if ( !std::isfinite(currentScale) || currentScale <= 0.0 )
                currentScale = 1.0;
            return std::abs(currentScale - scale) < 0.0001
                       ? owner
                       : nullptr;
        };

    wxNotebook *owner = getOwner();
    if ( !owner || impl->extendedPeers.size() != owner->m_pages.size() ||
         impl->labels.size() != owner->m_pages.size() ||
         owner->m_pageTexts.GetCount() != owner->m_pages.size() ||
         impl->iconPixelSizes.size() != owner->m_pages.size() )
    {
        return false;
    }

    std::vector<wxString> labelMetricTexts;
    std::vector<wxSize> labelNaturalPixelSizes;
    std::vector<bool> labelMetricPending;
    std::vector<std::uint64_t> labelMetricProbeEpochs;
    std::vector<bool> labelMetricRelayoutRequested;
    bool labelMetricsChanged = false;
    try
    {
        const size_t pageCount = owner->m_pages.size();
        const bool cacheShapeCurrent =
            impl->extendedLabelMetricTexts.size() == pageCount &&
            impl->extendedLabelNaturalPixelSizes.size() == pageCount &&
            impl->extendedLabelMetricPending.size() == pageCount &&
            impl->extendedLabelMetricProbeEpochs.size() == pageCount &&
            impl->extendedLabelMetricRelayoutRequested.size() == pageCount;
        if ( cacheShapeCurrent )
        {
            labelMetricTexts = impl->extendedLabelMetricTexts;
            labelNaturalPixelSizes =
                impl->extendedLabelNaturalPixelSizes;
            labelMetricPending = impl->extendedLabelMetricPending;
            labelMetricProbeEpochs =
                impl->extendedLabelMetricProbeEpochs;
            labelMetricRelayoutRequested =
                impl->extendedLabelMetricRelayoutRequested;
        }
        else
        {
            labelMetricTexts.resize(pageCount);
            labelNaturalPixelSizes.assign(pageCount, wxSize());
            labelMetricPending.assign(pageCount, true);
            labelMetricProbeEpochs.assign(
                pageCount, impl->extendedLayoutEpoch);
            labelMetricRelayoutRequested.assign(pageCount, false);
            labelMetricsChanged = true;
        }

        if ( !cacheShapeCurrent ||
             std::abs(impl->extendedLabelMetricScale - scale) >= 0.0001 )
        {
            std::fill(labelNaturalPixelSizes.begin(),
                      labelNaturalPixelSizes.end(),
                      wxSize());
            std::fill(labelMetricPending.begin(),
                      labelMetricPending.end(),
                      true);
            std::fill(labelMetricProbeEpochs.begin(),
                      labelMetricProbeEpochs.end(),
                      impl->extendedLayoutEpoch);
            std::fill(labelMetricRelayoutRequested.begin(),
                      labelMetricRelayoutRequested.end(),
                      false);
            labelMetricsChanged = true;
        }

        const double intMaximum =
            (std::numeric_limits<int>::max)();
        for ( size_t page = 0; page < pageCount; ++page )
        {
            const wxString display =
                wxWinUIRemoveMnemonics(owner->m_pageTexts[page]);
            const bool textChanged =
                labelMetricTexts[page] != display;
            if ( textChanged )
            {
                // ActualWidth still belongs to the preceding text until XAML
                // completes another frame. Do not relabel that stale positive
                // size as the new text's metric.
                labelMetricTexts[page] = display;
                labelNaturalPixelSizes[page] = wxSize();
                labelMetricPending[page] = true;
                labelMetricProbeEpochs[page] =
                    impl->extendedLayoutEpoch;
                labelMetricRelayoutRequested[page] = false;
                labelMetricsChanged = true;
                continue;
            }

            if ( labelMetricPending[page] &&
                 labelMetricProbeEpochs[page] ==
                     impl->extendedLayoutEpoch )
            {
                continue;
            }

            const MUXC::TextBlock observedLabel =
                impl->extendedPeers[page].label;
            const std::uint64_t observationEpoch =
                impl->extendedLayoutEpoch;
            const double actualWidth = observedLabel.ActualWidth();
            owner = getOwner();
            if ( !owner )
                return false;
            const double actualHeight = observedLabel.ActualHeight();
            owner = getOwner();
            if ( !owner )
                return false;
            if ( impl->extendedLayoutEpoch != observationEpoch )
                continue;

            const double actualWidthPx = actualWidth * scale;
            const double actualHeightPx = actualHeight * scale;
            if ( !std::isfinite(actualWidthPx) ||
                 !std::isfinite(actualHeightPx) ||
                 actualWidthPx <= 0.0 || actualHeightPx <= 0.0 ||
                 actualWidthPx > intMaximum ||
                 actualHeightPx > intMaximum )
            {
                continue;
            }

            const wxSize observed(
                wxMax(1, static_cast<int>(std::ceil(actualWidthPx))),
                wxMax(1, static_cast<int>(std::ceil(actualHeightPx))));
            wxSize& cached = labelNaturalPixelSizes[page];
            const wxSize monotone(
                wxMax(cached.x, observed.x),
                wxMax(cached.y, observed.y));
            if ( labelMetricPending[page] || monotone != cached )
            {
                cached = monotone;
                labelMetricPending[page] = false;
                labelMetricRelayoutRequested[page] = false;
                labelMetricsChanged = true;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI extended notebook label metric observation", e);
        return false;
    }
    catch ( const std::exception& )
    {
        return false;
    }

    if ( !labelMetricsChanged && impl->extendedLayoutValid &&
         impl->extendedLayoutModelRevision == modelRevision &&
         impl->extendedLayoutRevision == layoutRevision &&
         impl->extendedLayoutStyle == style &&
         impl->extendedLayoutClientSize == clientSize &&
         std::abs(impl->extendedLayoutScale - scale) < 0.0001 )
    {
        return true;
    }

    struct Metric
    {
        int width = 0;
        int height = 0;
        int iconWidth = 0;
        int iconHeight = 0;
        int labelWidth = 0;
        int labelHeight = 0;
    };

    std::vector<Metric> metrics(owner->m_pages.size());
    const wxSize defaultPadding = owner->FromDIP(wxSize(12, 6));
    owner = getOwner();
    if ( !owner )
        return false;
    const wxSize minimumTab = owner->FromDIP(wxSize(48, 40));
    owner = getOwner();
    if ( !owner )
        return false;
    const int spacing = owner->FromDIP(
        static_cast<int>(DefaultHeaderSpacingDIP));
    owner = getOwner();
    if ( !owner )
        return false;

    const wxSize padding =
        owner->m_hasTabPadding ? owner->m_tabPadding : defaultPadding;
    const bool hasTabSize = owner->m_hasTabSize;
    const wxSize requestedTabSize = owner->m_tabSize;
    const bool fixedWidth = (style & wxNB_FIXEDWIDTH) != 0;
    const bool verticalHeaders =
        (style & (wxNB_LEFT | wxNB_RIGHT)) != 0;

    int fixedTabWidth = 0;
    int fixedTabHeight = 0;
    for ( size_t page = 0; page < metrics.size(); ++page )
    {
        const wxString& display = labelMetricTexts[page];
        const wxSize textSize = owner->GetTextExtent(display);
        owner = getOwner();
        if ( !owner )
            return false;

        Metric& metric = metrics[page];
        metric.labelWidth = wxMax(1, textSize.x);
        metric.labelHeight = wxMax(1, textSize.y);
        if ( !labelMetricPending[page] &&
             labelNaturalPixelSizes[page].x > 0 &&
             labelNaturalPixelSizes[page].y > 0 )
        {
            metric.labelWidth = labelNaturalPixelSizes[page].x;
            metric.labelHeight = labelNaturalPixelSizes[page].y;
        }
        metric.iconWidth = wxMax(0, impl->iconPixelSizes[page].x);
        metric.iconHeight = wxMax(0, impl->iconPixelSizes[page].y);
        // Horizontal headers place icon and label beside one another. The
        // extended LEFT/RIGHT surface deliberately keeps both glyphs
        // readable and stacks them along the tab advance axis instead. In the
        // absence of SetTabSize(), derive the item from that same topology;
        // an explicit size remains exact and is clipped below when needed.
        const int contentWidth = verticalHeaders
            ? wxMax(metric.iconWidth, metric.labelWidth)
            : metric.iconWidth +
                  (metric.iconWidth > 0 ? spacing : 0) +
                  metric.labelWidth;
        const int contentHeight = verticalHeaders
            ? metric.iconHeight +
                  (metric.iconHeight > 0 ? spacing : 0) +
                  metric.labelHeight
            : wxMax(metric.iconHeight, metric.labelHeight);
        if ( hasTabSize )
        {
            metric.width = wxMax(1, requestedTabSize.x);
            metric.height = wxMax(1, requestedTabSize.y);
        }
        else
        {
            metric.width = wxMax(
                minimumTab.x,
                contentWidth + 2 * wxMax(0, padding.x));
            metric.height = wxMax(
                minimumTab.y,
                contentHeight + 2 * wxMax(0, padding.y));
        }
        fixedTabWidth = wxMax(fixedTabWidth, metric.width);
        fixedTabHeight = wxMax(fixedTabHeight, metric.height);
    }

    if ( fixedWidth )
    {
        for ( Metric& metric : metrics )
        {
            metric.width = fixedTabWidth;
            metric.height = fixedTabHeight;
        }
    }

    // Keep the extended presentation on the same callback-bearing geometry
    // boundary as the stock TabView path. A nested hot style mutation is a
    // newer writer: getOwner() below rejects this pass before it can publish
    // or roll back any of the superseded surface state.
#ifdef WXWINUI_TEST_SUPPORT
    owner->m_winui->InvokeProjectionHook(
        owner, wxWinUINotebookTestAccess::ProjectionPoint::TabGeometryMeasure);
#endif
    owner = getOwner();
    if ( !owner )
        return false;

    const wxDirection orientation = owner->GetTabOrientation();
    owner = getOwner();
    if ( !owner )
        return false;
    const bool horizontal =
        orientation == wxTOP || orientation == wxBOTTOM;
    const bool multiline = (style & wxNB_MULTILINE) != 0;
    const int primaryAvailable =
        wxMax(1, horizontal ? clientSize.x : clientSize.y);
    const int crossExtent =
        metrics.empty()
            ? (hasTabSize
                   ? wxMax(1,
                           horizontal ? requestedTabSize.y
                                      : requestedTabSize.x)
                   : (horizontal ? minimumTab.y
                                 : wxMax(minimumTab.x,
                                         owner->FromDIP(88))))
            : (horizontal ? fixedTabHeight : fixedTabWidth);
    owner = getOwner();
    if ( !owner )
        return false;

    std::vector<int> bands(metrics.size(), 0);
    std::vector<int> offsets(metrics.size(), 0);
    int band = 0;
    int offset = 0;
    for ( size_t page = 0; page < metrics.size(); ++page )
    {
        const int advance =
            horizontal ? metrics[page].width : metrics[page].height;
        if ( multiline && offset > 0 &&
             offset + advance > primaryAvailable )
        {
            ++band;
            offset = 0;
        }
        bands[page] = band;
        offsets[page] = offset;
        offset += advance;
    }

    const int totalAdvance = offset;
    const int desiredOverflowButtonExtent = owner->FromDIP(28);
    owner = getOwner();
    if ( !owner )
        return false;
    const bool overflow =
        !multiline && totalAdvance > primaryAvailable;
    const int overflowButtonExtent =
        overflow
            ? wxMin(wxMax(1, desiredOverflowButtonExtent),
                    wxMax(1, primaryAvailable / 4))
            : 0;
    const int viewportAdvance = overflow
        ? wxMax(1, primaryAvailable - 2 * overflowButtonExtent)
        : primaryAvailable;
    const int maximumScroll =
        overflow ? wxMax(0, totalAdvance - viewportAdvance) : 0;
    int scroll = overflow
        ? wxMin(maximumScroll,
                wxMax(0, impl->extendedScrollOffsetPx))
        : 0;

    if ( overflow && !impl->extendedManualScroll &&
         owner->m_selection >= 0 &&
         static_cast<size_t>(owner->m_selection) < metrics.size() )
    {
        const size_t selected =
            static_cast<size_t>(owner->m_selection);
        const int selectedAdvance = horizontal
            ? metrics[selected].width
            : metrics[selected].height;
        const int selectedStart = offsets[selected];
        const int selectedEnd = selectedStart + selectedAdvance;
        if ( selectedStart < scroll )
            scroll = selectedStart;
        else if ( selectedEnd > scroll + viewportAdvance )
            scroll = selectedEnd - viewportAdvance;
        scroll = wxMin(maximumScroll, wxMax(0, scroll));
    }
    if ( !multiline )
    {
        for ( int& itemOffset : offsets )
            itemOffset += overflowButtonExtent - scroll;
    }

    const int rowCount = metrics.empty() ? 1 : band + 1;
    const int bandExtent = wxMax(1, rowCount * crossExtent);
    std::vector<wxRect> tabRects(metrics.size());
    std::vector<wxRect> iconRects(metrics.size());
    std::vector<wxRect> labelRects(metrics.size());
    std::vector<wxRect> elementIconRects(metrics.size());
    std::vector<wxRect> elementLabelRects(metrics.size());
    wxRect chromeRect;
    wxRect viewportRect;
    wxRect backButtonRect;
    wxRect forwardButtonRect;
    MUX::Thickness chromeThickness{};

    switch ( orientation )
    {
        case wxBOTTOM:
            chromeRect = wxRect(
                0,
                wxMax(0, clientSize.y - bandExtent),
                wxMax(0, clientSize.x),
                wxMin(wxMax(0, clientSize.y), bandExtent));
            chromeThickness.Top = 1.0;
            break;
        case wxLEFT:
            chromeRect = wxRect(
                0,
                0,
                wxMin(wxMax(0, clientSize.x), bandExtent),
                wxMax(0, clientSize.y));
            chromeThickness.Right = 1.0;
            break;
        case wxRIGHT:
            chromeRect = wxRect(
                wxMax(0, clientSize.x - bandExtent),
                0,
                wxMin(wxMax(0, clientSize.x), bandExtent),
                wxMax(0, clientSize.y));
            chromeThickness.Left = 1.0;
            break;
        case wxTOP:
        default:
            chromeRect = wxRect(
                0,
                0,
                wxMax(0, clientSize.x),
                wxMin(wxMax(0, clientSize.y), bandExtent));
            chromeThickness.Bottom = 1.0;
            break;
    }

    viewportRect = chromeRect;
    if ( overflow )
    {
        if ( horizontal )
        {
            viewportRect.x = overflowButtonExtent;
            viewportRect.width = wxMax(
                0, clientSize.x - 2 * overflowButtonExtent);
            const int leftButtonX = 0;
            const int rightButtonX = wxMax(
                0, clientSize.x - overflowButtonExtent);
            backButtonRect = wxRect(
                layoutDirection == wxLayout_RightToLeft
                    ? rightButtonX
                    : leftButtonX,
                chromeRect.y,
                overflowButtonExtent,
                chromeRect.height);
            forwardButtonRect = wxRect(
                layoutDirection == wxLayout_RightToLeft
                    ? leftButtonX
                    : rightButtonX,
                chromeRect.y,
                overflowButtonExtent,
                chromeRect.height);
        }
        else
        {
            viewportRect.y = overflowButtonExtent;
            viewportRect.height = wxMax(
                0, clientSize.y - 2 * overflowButtonExtent);
            backButtonRect = wxRect(
                chromeRect.x,
                0,
                chromeRect.width,
                overflowButtonExtent);
            forwardButtonRect = wxRect(
                chromeRect.x,
                wxMax(0, clientSize.y - overflowButtonExtent),
                chromeRect.width,
                overflowButtonExtent);
        }
    }

    for ( size_t page = 0; page < metrics.size(); ++page )
    {
        const Metric& metric = metrics[page];
        int x = 0;
        int y = 0;
        if ( horizontal )
        {
            x = offsets[page];
            if ( layoutDirection == wxLayout_RightToLeft )
                x = clientSize.x - x - metric.width;
            y = bands[page] * crossExtent;
            if ( orientation == wxBOTTOM )
                y += clientSize.y - bandExtent;
        }
        else
        {
            y = offsets[page];
            x = bands[page] * crossExtent;
            if ( orientation == wxRIGHT )
                x += clientSize.x - bandExtent;
        }

        tabRects[page] =
            wxRect(x, y, metric.width, metric.height);
        if ( horizontal )
        {
            const int contentWidth =
                metric.iconWidth +
                (metric.iconWidth > 0 ? spacing : 0) +
                metric.labelWidth;
            // Keep an oversized explicit header centred too. Its public part
            // rectangles are clipped to the exact SetTabSize() item below,
            // matching TCM_SETITEMSIZE instead of silently growing the tab.
            int contentX = x + (metric.width - contentWidth) / 2;
            if ( metric.iconWidth > 0 )
            {
                iconRects[page] = wxRect(
                    contentX,
                    y + (metric.height - metric.iconHeight) / 2,
                    metric.iconWidth,
                    metric.iconHeight);
                contentX += metric.iconWidth + spacing;
            }
            labelRects[page] = wxRect(
                contentX,
                y + (metric.height - metric.labelHeight) / 2,
                metric.labelWidth,
                metric.labelHeight);
        }
        else
        {
            // Side tabs expose the same icon/text regions as wxMSW along the
            // tab advance axis. Keep their readable WinUI text horizontal,
            // but stack both realized elements vertically so a centre-line
            // sweep can reach each exact region (label then icon on the left,
            // icon then label on the right).
            const int contentHeight =
                metric.iconHeight +
                (metric.iconHeight > 0 ? spacing : 0) +
                metric.labelHeight;
            int contentY = y + (metric.height - contentHeight) / 2;
            const auto placeIcon = [&]()
            {
                if ( metric.iconWidth <= 0 )
                    return;
                iconRects[page] = wxRect(
                    x + (metric.width - metric.iconWidth) / 2,
                    contentY,
                    metric.iconWidth,
                    metric.iconHeight);
                contentY += metric.iconHeight + spacing;
            };
            const auto placeLabel = [&]()
            {
                labelRects[page] = wxRect(
                    x + (metric.width - metric.labelWidth) / 2,
                    contentY,
                    metric.labelWidth,
                    metric.labelHeight);
                contentY += metric.labelHeight +
                    (metric.iconHeight > 0 ? spacing : 0);
            };

            if ( orientation == wxLEFT )
            {
                placeLabel();
                placeIcon();
            }
            else
            {
                placeIcon();
                placeLabel();
            }
        }

        elementIconRects[page] = iconRects[page];
        elementLabelRects[page] = labelRects[page];

        // SetTabSize() maps to the exact TCM_SETITEMSIZE dimensions on MSW.
        // Keep that contract even when a natural high-DPI header is larger:
        // only its visible intersection participates in rendering and hit
        // flags, and no icon/label rectangle can escape its owning item.
        if ( !iconRects[page].IsEmpty() )
            iconRects[page].Intersect(tabRects[page]);
        if ( !labelRects[page].IsEmpty() )
            labelRects[page].Intersect(tabRects[page]);
    }

    // Stage the public wx-client geometry before crossing any XAML setter.
    // The Canvas continues to consume the physical vectors below; only the
    // revisioned cache published after a successful layout uses this mirrored
    // representation. Allocation failure therefore cannot publish a half
    // physical, half logical model.
    std::vector<wxRect> clientTabRects;
    std::vector<wxRect> clientIconRects;
    std::vector<wxRect> clientLabelRects;
    try
    {
        clientTabRects.reserve(tabRects.size());
        clientIconRects.reserve(iconRects.size());
        clientLabelRects.reserve(labelRects.size());
        for ( const wxRect& rect : tabRects )
        {
            clientTabRects.push_back(
                wxWinUIExtendedPhysicalRectToClient(
                    rect, clientSize, layoutDirection));
        }
        for ( const wxRect& rect : iconRects )
        {
            clientIconRects.push_back(
                wxWinUIExtendedPhysicalRectToClient(
                    rect, clientSize, layoutDirection));
        }
        for ( const wxRect& rect : labelRects )
        {
            clientLabelRects.push_back(
                wxWinUIExtendedPhysicalRectToClient(
                    rect, clientSize, layoutDirection));
        }
    }
    catch ( const std::exception& )
    {
        return false;
    }
    const wxRect clientViewportRect =
        wxWinUIExtendedPhysicalRectToClient(
            viewportRect, clientSize, layoutDirection);

    try
    {
        impl->extendedTabCanvas.Width(
            wxMax(0, clientSize.x) / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        impl->extendedTabCanvas.Height(
            wxMax(0, clientSize.y) / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        MUXM::RectangleGeometry viewportClip;
        viewportClip.Rect(winrt::Windows::Foundation::Rect{
            static_cast<float>(viewportRect.x / scale),
            static_cast<float>(viewportRect.y / scale),
            static_cast<float>(viewportRect.width / scale),
            static_cast<float>(viewportRect.height / scale)});
        owner = getOwner();
        if ( !owner )
            return false;
        impl->extendedTabCanvas.Clip(viewportClip);
        owner = getOwner();
        if ( !owner )
            return false;

        const winrt::hstring backGlyph =
            horizontal
                ? layoutDirection == wxLayout_RightToLeft
                    ? winrt::hstring(L"\u203a")
                    : winrt::hstring(L"\u2039")
                : winrt::hstring(L"\u25b2");
        const winrt::hstring forwardGlyph =
            horizontal
                ? layoutDirection == wxLayout_RightToLeft
                    ? winrt::hstring(L"\u2039")
                    : winrt::hstring(L"\u203a")
                : winrt::hstring(L"\u25bc");
        const auto layoutOverflowButton =
            [&](const MUXC::Button& button,
                const wxRect& rect,
                const winrt::hstring& glyph,
                bool enabled)
            {
                button.Visibility(
                    overflow
                        ? MUX::Visibility::Visible
                        : MUX::Visibility::Collapsed);
                owner = getOwner();
                if ( !owner )
                    return false;
                button.IsEnabled(overflow && enabled);
                owner = getOwner();
                if ( !owner )
                    return false;
                if ( !overflow )
                    return true;

                button.Content(winrt::box_value(glyph));
                owner = getOwner();
                if ( !owner )
                    return false;
                button.Width(rect.width / scale);
                owner = getOwner();
                if ( !owner )
                    return false;
                button.Height(rect.height / scale);
                owner = getOwner();
                if ( !owner )
                    return false;
                MUXC::Canvas::SetLeft(button, rect.x / scale);
                owner = getOwner();
                if ( !owner )
                    return false;
                MUXC::Canvas::SetTop(button, rect.y / scale);
                owner = getOwner();
                return owner != nullptr;
            };
        if ( !layoutOverflowButton(
                 impl->extendedBackButton,
                 backButtonRect,
                 backGlyph,
                 scroll > 0) ||
             !layoutOverflowButton(
                 impl->extendedForwardButton,
                 forwardButtonRect,
                 forwardGlyph,
                 scroll < maximumScroll) )
        {
            return false;
        }

        for ( size_t page = 0; page < impl->extendedPeers.size(); ++page )
        {
            wxWinUINotebookSurfacePeer& peer =
                impl->extendedPeers[page];
            const wxRect& rect = tabRects[page];
            // The semantic TabViewItem remains the sole input/UIA peer. Its
            // template is free to arrange the empty header, while the visible
            // icon and label are Canvas siblings constrained to the exact
            // physical rectangles published by this transaction.
            peer.header.FlowDirection(
                MUX::FlowDirection::LeftToRight);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.label.FlowDirection(
                layoutDirection == wxLayout_RightToLeft
                    ? MUX::FlowDirection::RightToLeft
                    : MUX::FlowDirection::LeftToRight);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.FlowDirection(
                layoutDirection == wxLayout_RightToLeft
                    ? MUX::FlowDirection::RightToLeft
                    : MUX::FlowDirection::LeftToRight);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.Width(rect.width / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.Height(rect.height / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.MinWidth(rect.width / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.MinHeight(rect.height / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.MaxWidth(rect.width / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.MaxHeight(rect.height / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            MUXC::Canvas::SetLeft(peer.item, rect.x / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            MUXC::Canvas::SetTop(peer.item, rect.y / scale);
            owner = getOwner();
            if ( !owner )
                return false;
            {
                wxWinUINotebookPeerMutationGuard mutation(state);
                peer.item.IsSelected(
                    static_cast<int>(page) == owner->m_selection);
            }
            owner = getOwner();
            if ( !owner )
                return false;
            peer.item.IsTabStop(
                static_cast<int>(page) == owner->m_selection);
            owner = getOwner();
            if ( !owner )
                return false;

            const auto applyPartFrame =
                [&](const auto& element,
                    const wxRect& elementRect,
                    const wxRect& visibleRect)
                {
                    MUXM::RectangleGeometry clip{ nullptr };
                    if ( visibleRect != elementRect )
                    {
                        clip = MUXM::RectangleGeometry();
                        clip.Rect(winrt::Windows::Foundation::Rect{
                            static_cast<float>(
                                (visibleRect.x - elementRect.x) / scale),
                            static_cast<float>(
                                (visibleRect.y - elementRect.y) / scale),
                            static_cast<float>(visibleRect.width / scale),
                            static_cast<float>(visibleRect.height / scale)});
                    }
                    element.Clip(clip);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    MUXC::Canvas::SetLeft(
                        element, elementRect.x / scale);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    MUXC::Canvas::SetTop(
                        element, elementRect.y / scale);
                    owner = getOwner();
                    return owner != nullptr;
                };
            const auto layoutPart =
                [&](const auto& element,
                    const wxRect& elementRect,
                    const wxRect& visibleRect)
                {
                    const double width = elementRect.width / scale;
                    const double height = elementRect.height / scale;
                    // Keep the intrinsic element frame authoritative and use
                    // a real XAML clip for the visible/hit-testable subset.
                    // This avoids publishing a constrained wx rectangle while
                    // ActualWidth still belongs to the preceding natural
                    // frame, without synchronously forcing another layout.
                    element.MinWidth(0.0);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.MaxWidth(width);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.MinWidth(width);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.Width(width);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.MinHeight(0.0);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.MaxHeight(height);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.MinHeight(height);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    element.Height(height);
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    return applyPartFrame(
                        element, elementRect, visibleRect);
                };
            const auto layoutNaturalLabelProbe =
                [&](const MUXC::TextBlock& label,
                    const wxRect& elementRect,
                    const wxRect& visibleRect)
                {
                    // A changed/unrealized string needs one ordinary XAML
                    // frame before its intrinsic metric can be trusted. Clear
                    // the previous pin and observe the result on a later turn;
                    // invoking Measure() synchronously here can poison the
                    // island's active measure/arrange wave.
                    label.ClearValue(MUX::FrameworkElement::MinWidthProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    label.ClearValue(MUX::FrameworkElement::MaxWidthProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    label.ClearValue(MUX::FrameworkElement::WidthProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    label.ClearValue(MUX::FrameworkElement::MinHeightProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    label.ClearValue(MUX::FrameworkElement::MaxHeightProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    label.ClearValue(MUX::FrameworkElement::HeightProperty());
                    owner = getOwner();
                    if ( !owner )
                        return false;
                    return applyPartFrame(
                        label, elementRect, visibleRect);
                };
            if ( !layoutPart(
                     peer.icon,
                     elementIconRects[page],
                     iconRects[page]) ||
                 (labelMetricPending[page]
                       ? !layoutNaturalLabelProbe(
                             peer.label,
                             elementLabelRects[page],
                             labelRects[page])
                       : !layoutPart(
                             peer.label,
                             elementLabelRects[page],
                             labelRects[page])) )
            {
                return false;
            }
        }

        impl->extendedPageChrome.BorderThickness(chromeThickness);
        owner = getOwner();
        if ( !owner )
            return false;
        impl->extendedPageChrome.Width(chromeRect.width / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        impl->extendedPageChrome.Height(chromeRect.height / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        MUXC::Canvas::SetLeft(
            impl->extendedPageChrome, chromeRect.x / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        MUXC::Canvas::SetTop(
            impl->extendedPageChrome, chromeRect.y / scale);
        owner = getOwner();
        if ( !owner )
            return false;
        impl->extendedPageChrome.Visibility(
            (style & wxNB_NOPAGETHEME) != 0
                ? MUX::Visibility::Collapsed
                : MUX::Visibility::Visible);
        owner = getOwner();
        if ( !owner )
            return false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI extended notebook layout", e);
        return false;
    }

    owner = getOwner();
    if ( !owner )
        return false;
    std::vector<wxWeakRef<wxWindow>> pageLifetimes;
    try
    {
        pageLifetimes.reserve(owner->m_pages.size());
        for ( wxWindow * const page : owner->m_pages )
            pageLifetimes.emplace_back(page);
    }
    catch ( const std::exception& )
    {
        return false;
    }

    // LayoutUpdated can run synchronously between any two dependency-property
    // setters above. None of those partial epochs proves that an unconstrained
    // label has completed an ordinary frame. Fence every pending probe at the
    // final epoch of this XAML transaction immediately before publishing it;
    // only a later LayoutUpdated can make its ActualWidth/ActualHeight valid.
    for ( size_t page = 0; page < labelMetricPending.size(); ++page )
    {
        if ( labelMetricPending[page] )
            labelMetricProbeEpochs[page] = impl->extendedLayoutEpoch;
    }

    impl->extendedTabRects.swap(clientTabRects);
    impl->extendedIconRects.swap(clientIconRects);
    impl->extendedLabelRects.swap(clientLabelRects);
    impl->extendedLabelMetricTexts.swap(labelMetricTexts);
    impl->extendedLabelNaturalPixelSizes.swap(
        labelNaturalPixelSizes);
    impl->extendedLabelMetricPending.swap(labelMetricPending);
    impl->extendedLabelMetricProbeEpochs.swap(
        labelMetricProbeEpochs);
    impl->extendedLabelMetricRelayoutRequested.swap(
        labelMetricRelayoutRequested);
    impl->extendedLabelMetricScale = scale;
    impl->extendedViewportRect = clientViewportRect;
    impl->extendedBandExtentPx = bandExtent;
    impl->extendedRowCount = rowCount;
    impl->extendedScrollOffsetPx = scroll;
    impl->extendedScrollMaximumPx = maximumScroll;
    impl->extendedScrollViewportPx = viewportAdvance;
    if ( !overflow )
        impl->extendedManualScroll = false;
    impl->extendedLayoutModelRevision = modelRevision;
    impl->extendedLayoutRevision = layoutRevision;
    impl->extendedLayoutClientSize = clientSize;
    impl->extendedLayoutStyle = style;
    impl->extendedLayoutScale = scale;
    impl->extendedLayoutValid = true;
    m_tabStripHeightDIP = horizontal
        ? wxMax(1, static_cast<int>(std::lround(bandExtent / scale)))
        : DefaultTabStripHeightDIP;

    wxRect clip;
    switch ( orientation )
    {
        case wxBOTTOM:
            clip = wxRect(0,
                          wxMax(0, clientSize.y - bandExtent),
                          clientSize.x,
                          wxMin(clientSize.y, bandExtent));
            break;
        case wxLEFT:
            clip = wxRect(0,
                          0,
                          wxMin(clientSize.x, bandExtent),
                          clientSize.y);
            break;
        case wxRIGHT:
            clip = wxRect(wxMax(0, clientSize.x - bandExtent),
                          0,
                          wxMin(clientSize.x, bandExtent),
                          clientSize.y);
            break;
        case wxTOP:
        default:
            clip = wxRect(0,
                          0,
                          clientSize.x,
                          wxMin(clientSize.y, bandExtent));
            break;
    }
    impl->host.SetBridgeClipRect(clip);
    owner = getOwner();
    if ( !owner )
        return false;

    // The wrapped band is part of the native wx page geometry, not just the
    // XAML presentation. Publish and apply the matching page rectangle in the
    // same revision so text/image/metric/topology changes cannot leave hidden
    // HWND pages at the preceding row count until the next notebook resize.
    wxRect pageRect(wxPoint(), clientSize);
    switch ( orientation )
    {
        case wxBOTTOM:
            pageRect.height = wxMax(0, pageRect.height - bandExtent);
            break;
        case wxLEFT:
            pageRect.x += bandExtent;
            pageRect.width = wxMax(0, pageRect.width - bandExtent);
            break;
        case wxRIGHT:
            pageRect.width = wxMax(0, pageRect.width - bandExtent);
            break;
        case wxTOP:
        default:
            pageRect.y += bandExtent;
            pageRect.height = wxMax(0, pageRect.height - bandExtent);
            break;
    }

    for ( const wxWeakRef<wxWindow>& weakPage : pageLifetimes )
    {
        owner = getOwner();
        wxWindow *page = weakPage.get();
        if ( !owner )
            return false;
        if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
             page->GetParent() != owner ||
             std::find(owner->m_pages.begin(),
                       owner->m_pages.end(),
                       page) == owner->m_pages.end() )
        {
            return false;
        }

        page->SetSize(pageRect);
        owner = getOwner();
        page = weakPage.get();
        if ( !owner )
            return false;
        if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
             page->GetParent() != owner ||
             std::find(owner->m_pages.begin(),
                       owner->m_pages.end(),
                       page) == owner->m_pages.end() )
        {
            return false;
        }
    }

    return true;
}

bool wxNotebook::InstallPeerSurfaceForCurrentStyle()
{
    if ( !m_winui || !m_winui->callbackState || m_winui->closed )
        return false;

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    const std::uint64_t styleRevision = impl->styleRevision;
    const long style = GetWindowStyleFlag();
    const long alignment = style & wxBK_ALIGN_MASK;
    const bool extended =
        alignment != wxNB_TOP ||
        (style & (wxNB_MULTILINE | wxNB_NOPAGETHEME)) != 0;
    const auto getOwner =
        [state,
         callbackGeneration,
         impl,
         styleRevision,
         style]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->styleRevision == styleRevision &&
                           owner->GetWindowStyleFlag() == style
                       ? owner
                       : nullptr;
        };
    if ( extended && !EnsureExtendedPeerSurface() )
        return false;

    wxNotebook *owner = getOwner();
    if ( owner != this )
        return false;

    const MUX::UIElement surface =
        extended
            ? impl->extendedRoot.as<MUX::UIElement>()
            : impl->tabView.as<MUX::UIElement>();
    if ( !impl->host.SetContent(surface) )
        return false;

    owner = getOwner();
    if ( owner != this )
        return false;

    if ( extended )
        return owner->LayoutExtendedPeerSurface();

    impl->host.ClearBridgeClipRect();
    owner = getOwner();
    if ( owner != this )
        return false;
    const int physicalHeight = owner->FromDIP(
        owner->m_tabStripHeightDIP > 0
            ? owner->m_tabStripHeightDIP
            : DefaultTabStripHeightDIP);
    owner = getOwner();
    if ( owner != this )
        return false;
    impl->host.SetBridgeHeightLimit(physicalHeight);
    return getOwner() == this;
}

void wxNotebook::SetWindowStyleFlag(long style)
{
    long alignment = style & wxBK_ALIGN_MASK;
    long normalizedAlignment = wxNB_TOP;
    if ( alignment & wxNB_BOTTOM )
        normalizedAlignment = wxNB_BOTTOM;
    else if ( alignment & wxNB_RIGHT )
        normalizedAlignment = wxNB_RIGHT;
    else if ( alignment & wxNB_LEFT )
        normalizedAlignment = wxNB_LEFT;
    style = (style & ~wxBK_ALIGN_MASK) | normalizedAlignment;
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    const long oldStyle = GetWindowStyleFlag();
    if ( style == oldStyle )
        return;

    wxControl::SetWindowStyleFlag(style);
    if ( !m_winui || !m_winui->callbackState || m_winui->closed )
        return;

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    const std::uint64_t styleRevision = impl->BumpStyleRevision();
    const auto getStyleOwner =
        [state,
         callbackGeneration,
         impl,
         styleRevision,
         style]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->styleRevision == styleRevision &&
                           owner->GetWindowStyleFlag() == style
                       ? owner
                       : nullptr;
        };
    bool peerStyleApplied = true;
    try
    {
        impl->tabView.TabWidthMode(
            (style & wxNB_FIXEDWIDTH) != 0
                ? MUXC::TabViewWidthMode::Equal
                : MUXC::TabViewWidthMode::SizeToContent);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI notebook style mutation", e);
        peerStyleApplied = false;
    }
    wxNotebook *owner = getStyleOwner();
    if ( owner != this )
        return;
    impl->BumpModelRevision();
    impl->InvalidateGeometry();

    if ( !peerStyleApplied || !InstallPeerSurfaceForCurrentStyle() )
    {
        owner = getStyleOwner();
        if ( owner == this )
        {
            // Rollback is itself a revisioned writer. Revalidate it across
            // every callback-bearing setter too: a style authored from one
            // of those callbacks is newer and must not be overwritten by the
            // remainder of this recovery pass.
            const std::uint64_t rollbackRevision =
                impl->BumpStyleRevision();
            wxControl::SetWindowStyleFlag(oldStyle);
            const auto getRollbackOwner =
                [state,
                 callbackGeneration,
                 impl,
                 rollbackRevision,
                 oldStyle]() -> wxNotebook *
                {
                    wxNotebook * const current =
                        state->GetOwner(callbackGeneration);
                    return current && current->m_winui &&
                                   current->m_winui.get() == impl &&
                                   current->m_winui->callbackState == state &&
                                   !current->m_winui->closed &&
                                   current->m_winui->styleRevision ==
                                       rollbackRevision &&
                                   current->GetWindowStyleFlag() == oldStyle
                               ? current
                               : nullptr;
                };
            owner = getRollbackOwner();
            if ( owner )
            {
                try
                {
                    impl->tabView.TabWidthMode(
                        (oldStyle & wxNB_FIXEDWIDTH) != 0
                            ? MUXC::TabViewWidthMode::Equal
                            : MUXC::TabViewWidthMode::SizeToContent);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI notebook style rollback", e);
                    return;
                }
                owner = getRollbackOwner();
                if ( owner )
                {
                    impl->BumpModelRevision();
                    impl->InvalidateGeometry();
                    (void)InstallPeerSurfaceForCurrentStyle();
                }
            }
        }
        else
        {
            // A nested style mutation can supersede this writer while the
            // outer extended-layout guard is held. Its peer installation is
            // deliberately accepted as deferred above; now that the outer
            // LayoutExtendedPeerSurface() call has returned and released the
            // guard, synchronously publish the newest extended geometry.
            wxNotebook * const latest =
                state->GetOwner(callbackGeneration);
            if ( latest && latest->m_winui &&
                 latest->m_winui.get() == impl &&
                 latest->m_winui->callbackState == state &&
                 !latest->m_winui->closed &&
                 impl->styleRevision != styleRevision &&
                 latest->UsesExtendedPeerSurface() )
            {
                (void)latest->LayoutExtendedPeerSurface();
            }
        }
        return;
    }

    (void)SyncPeerProjection();

    owner = getStyleOwner();
    if ( owner != this )
        return;
    owner->DoInvalidateBestSize();
    owner = getStyleOwner();
    if ( owner == this )
        owner->DoSize();
}

int wxNotebook::GetRowCount() const
{
    if ( !UsesExtendedPeerSurface() )
        return 1;

    wxNotebook * const self = const_cast<wxNotebook *>(this);
    if ( !m_winui ||
         (!m_winui->extendedLayoutValid &&
          !self->LayoutExtendedPeerSurface()) )
    {
        return 1;
    }
    return wxMax(1, m_winui->extendedRowCount);
}

wxRect wxNotebook::GetExtendedPageRect() const
{
    const wxSize client = GetClientSize();
    int extent = m_winui && m_winui->extendedLayoutValid
                     ? m_winui->extendedBandExtentPx
                     : FromDIP(DefaultTabStripHeightDIP);
    extent = wxMax(0, extent);

    wxRect rect(wxPoint(), client);
    switch ( GetTabOrientation() )
    {
        case wxBOTTOM:
            rect.height = wxMax(0, rect.height - extent);
            break;
        case wxLEFT:
            rect.x += extent;
            rect.width = wxMax(0, rect.width - extent);
            break;
        case wxRIGHT:
            rect.width = wxMax(0, rect.width - extent);
            break;
        case wxTOP:
        default:
            rect.y += extent;
            rect.height = wxMax(0, rect.height - extent);
            break;
    }
    return rect;
}

void wxNotebook::UpdateExtendedPeerSelection(size_t selection)
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    impl->extendedManualScroll = false;
    try
    {
        wxWinUINotebookPeerMutationGuard mutation(state);
        for ( size_t page = 0; page < impl->extendedPeers.size(); ++page )
        {
            impl->extendedPeers[page].item.IsSelected(page == selection);
            wxNotebook *owner =
                state->GetOwner(callbackGeneration);
            if ( owner != this || !owner->m_winui ||
                 owner->m_winui.get() != impl )
            {
                return;
            }
            impl->extendedPeers[page].item.IsTabStop(page == selection);
            owner = state->GetOwner(callbackGeneration);
            if ( owner != this || !owner->m_winui ||
                 owner->m_winui.get() != impl )
            {
                return;
            }
        }
        if ( selection < impl->extendedPeers.size() )
        {
            impl->host.SetPreferredFocus(
                impl->extendedPeers[selection].item);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI extended notebook selection", e);
    }
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

template <typename Validator>
MUX::FrameworkElement wxWinUIGetRealizedTabItem(
    const MUXC::TabView& tabView,
    const MUXC::TabViewItem& item,
    size_t page,
    Validator&& isCurrent)
{
    if ( !tabView || !item || !isCurrent() )
        return nullptr;

    const auto generated =
        tabView.ContainerFromIndex(static_cast<int>(page))
            .try_as<MUX::FrameworkElement>();
    if ( !isCurrent() )
        return nullptr;
    if ( generated )
    {
        return generated;
    }

    // TabItems contains explicit TabViewItem controls, not arbitrary data
    // objects. During the first layout pass (and on a non-input desktop) the
    // internal generator can lag behind while that exact item is already in
    // the visual tree and arranged. It is then the authoritative container.
    // Never use a detached/virtualized item: this preserves the contract that
    // GetTabRect() reports only real, hit-testable geometry.
    const MUX::DependencyObject parent =
        MUX::Media::VisualTreeHelper::GetParent(item);
    if ( !isCurrent() || !parent )
        return nullptr;
    return item.as<MUX::FrameworkElement>();
}

enum class wxWinUINotebookAnchorResult
{
    NotFound,
    Found,
    Stale
};

template <typename Validator>
wxWinUINotebookAnchorResult wxWinUIResolveNotebookAnchor(
    const MUXC::TabView& tabView,
    int selection,
    size_t lastGeometryAnchor,
    const std::vector<MUXC::TabViewItem>& items,
    const std::vector<wxWinUINotebookTabMetric>& metrics,
    const std::vector<double>& prefix,
    Validator&& isCurrent,
    double *originDIP,
    double *topDIP,
    size_t *anchorPage)
{
    if ( !tabView || !originDIP || !topDIP || !anchorPage ||
         metrics.empty() || items.size() != metrics.size() ||
         prefix.size() != metrics.size() + 1 || !isCurrent() )
    {
        return wxWinUINotebookAnchorResult::NotFound;
    }

    const size_t pageCount = metrics.size();
    std::vector<size_t> candidates;
    candidates.reserve(pageCount + 2);
    const auto addCandidate =
        [&candidates, pageCount](size_t candidate)
        {
            if ( candidate >= pageCount )
                return;
            for ( const size_t existing : candidates )
            {
                if ( existing == candidate )
                    return;
            }
            candidates.push_back(candidate);
        };

    if ( selection >= 0 )
        addCandidate(static_cast<size_t>(selection));
    addCandidate(lastGeometryAnchor);
    for ( size_t page = 0; page < pageCount; ++page )
        addCandidate(page);

    const auto relativeTo = tabView.as<MUX::FrameworkElement>();
    for ( const size_t candidate : candidates )
    {
        const auto element =
            wxWinUIGetRealizedTabItem(
                tabView, items[candidate], candidate, isCurrent);
        winrt::Windows::Foundation::Rect bounds{};
        const bool hasBounds =
            wxWinUIGetElementBoundsInNotebook(
                element, relativeTo, &bounds, isCurrent);
        if ( !isCurrent() )
            return wxWinUINotebookAnchorResult::Stale;
        if ( !hasBounds )
            continue;

        *anchorPage = candidate;
        *topDIP = bounds.Y;
        // TransformToVisual(tabView) reports coordinates in TabView's
        // logical local axis: page indices increase along X even when the
        // peer is RTL. This is the same convention as TCM_GETITEMRECT on a
        // WS_EX_LAYOUTRTL native notebook, so don't mirror the model a second
        // time here.
        *originDIP =
            bounds.X -
            prefix[candidate] -
            metrics[candidate].leadingLTR;
        return wxWinUINotebookAnchorResult::Found;
    }

    return wxWinUINotebookAnchorResult::NotFound;
}

wxNotebook *wxNotebook::RealizeTabGeometryForQuery() const
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->tabView )
        return nullptr;

    wxWinUINotebookImpl * const impl = m_winui.get();
    const auto callbackState = impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    const MUXC::TabView tabView = impl->tabView;
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;

    const auto getIdentityOwner =
        [callbackState,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision]() -> wxNotebook *
        {
            wxNotebook * const owner =
                callbackState->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return nullptr;

    // GetDPIScaleFactor() is virtual application code. Invoke it once and
    // re-resolve the owner before reading any other model state.
    const double requestedScale = snapshotOwner->GetDPIScaleFactor();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return nullptr;
    const double scale =
        std::isfinite(requestedScale) && requestedScale > 0.0
            ? requestedScale
            : 1.0;
    const size_t pageCount = snapshotOwner->m_pages.size();

    // Each of these calls is virtual application code. Never carry the raw
    // owner obtained before a call across its boundary: an override can
    // synchronously destroy or mutate the notebook.
    const int selection = snapshotOwner->GetSelection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return nullptr;
    const wxLayoutDirection direction =
        snapshotOwner->GetLayoutDirection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return nullptr;
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return nullptr;
    const double clientWidthDIP =
        static_cast<double>(clientSize.x) / scale;
    const bool hasTabSize = snapshotOwner->m_hasTabSize;
    const wxSize tabSize = snapshotOwner->m_tabSize;
    const bool hasTabPadding = snapshotOwner->m_hasTabPadding;
    const wxSize tabPadding = snapshotOwner->m_tabPadding;
    const int tabStripHeightDIP =
        snapshotOwner->m_tabStripHeightDIP;
    const bool fixedWidth =
        snapshotOwner->HasFlag(wxNB_FIXEDWIDTH);

    const auto getStructuralOwner =
        [getIdentityOwner,
         pageCount,
         selection,
         direction,
         clientSize]() -> wxNotebook *
        {
            wxNotebook *owner = getIdentityOwner();
            if ( !owner || owner->m_pages.size() != pageCount )
            {
                return nullptr;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || owner->m_pages.size() != pageCount ||
                 currentSelection != selection )
            {
                return nullptr;
            }

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || owner->m_pages.size() != pageCount ||
                 currentDirection != direction )
            {
                return nullptr;
            }

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || owner->m_pages.size() != pageCount ||
                 currentClientSize != clientSize )
            {
                return nullptr;
            }
            return owner;
        };

    const auto getCurrentOwner =
        [getStructuralOwner, scale]() -> wxNotebook *
        {
            wxNotebook *owner = getStructuralOwner();
            if ( !owner )
                return nullptr;

            const double requestedCurrentScale =
                owner->GetDPIScaleFactor();
            owner = getStructuralOwner();
            if ( !owner )
                return nullptr;
            const double currentScale =
                std::isfinite(requestedCurrentScale) &&
                        requestedCurrentScale > 0.0
                    ? requestedCurrentScale
                    : 1.0;
            return std::abs(currentScale - scale) < 0.0001
                ? owner
                : nullptr;
        };

    wxWinUINotebookGeometryGuard guard(callbackState);

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return nullptr;

    // Nested queries never cross another XAML boundary and never publish a
    // partially built cache. GetTabRect() may consume an already-published
    // cache before reaching this function; all other nested callers stop.
    if ( !guard.IsAcquired() )
        return nullptr;

    std::vector<MUXC::TabViewItem> anchorItems;
    size_t lastGeometryAnchor = pageCount;
    try
    {
        anchorItems = impl->items;
        lastGeometryAnchor = impl->lastGeometryAnchor;
    }
    catch ( const std::exception& )
    {
        return nullptr;
    }
    owner = getCurrentOwner();
    if ( !owner || anchorItems.size() != pageCount )
        return nullptr;

    if ( impl->geometryValid &&
         impl->geometryModelRevision == modelRevision &&
         impl->geometryLayoutRevision == layoutRevision &&
         std::abs(impl->geometryScale - scale) < 0.0001 &&
         impl->geometryClientSize == clientSize &&
         impl->geometrySelection == selection &&
         impl->geometryDirection == direction &&
         impl->tabMetrics.size() == pageCount &&
         impl->tabPrefixWidths.size() == pageCount + 1 )
    {
        try
        {
            double originDIP = impl->geometryOriginDIP;
            double topDIP = impl->geometryTopDIP;
            size_t anchorPage = pageCount;
            const wxWinUINotebookAnchorResult anchorResult =
                wxWinUIResolveNotebookAnchor(
                    tabView,
                    selection,
                    lastGeometryAnchor,
                    anchorItems,
                    impl->tabMetrics,
                    impl->tabPrefixWidths,
                    [&getCurrentOwner]()
                    {
                        return getCurrentOwner() != nullptr;
                    },
                    &originDIP,
                    &topDIP,
                    &anchorPage);
            owner = getCurrentOwner();
            if ( !owner ||
                 anchorResult ==
                     wxWinUINotebookAnchorResult::Stale )
            {
                return nullptr;
            }
            if ( anchorResult ==
                 wxWinUINotebookAnchorResult::Found )
            {
                impl->geometryOriginDIP = originDIP;
                impl->geometryTopDIP = topDIP;
                impl->lastGeometryAnchor = anchorPage;
            }
            return owner;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI TabView geometry anchor refresh", e);
            return nullptr;
        }
        catch ( const std::exception& )
        {
            return nullptr;
        }
    }

    std::vector<MUXC::TabViewItem> items;
    std::vector<MUXC::Image> icons;
    std::vector<MUXC::TextBlock> labels;
    std::vector<wxSize> iconPixelSizes;
    wxArrayString texts;
    try
    {
        items = impl->items;
        icons = impl->icons;
        labels = impl->labels;
        iconPixelSizes = impl->iconPixelSizes;
        texts = m_pageTexts;
    }
    catch ( const std::exception& )
    {
        wxLogError("Unable to stage wxWinUI notebook tab geometry");
        return nullptr;
    }

    if ( items.size() != pageCount ||
         icons.size() != pageCount ||
         labels.size() != pageCount ||
         iconPixelSizes.size() != pageCount ||
         texts.GetCount() != pageCount )
    {
        return nullptr;
    }

    try
    {
        // TabView owns an internal virtualizing list. Realize its current
        // viewport once; off-screen metrics below are measured directly from
        // the explicit TabViewItem objects and never scrolled into view.
        tabView.ApplyTemplate();
        owner = getCurrentOwner();
        if ( !owner )
            return nullptr;

        impl->host.ForceRender();
        owner = getCurrentOwner();
        if ( !owner )
            return nullptr;

        tabView.UpdateLayout();
        owner = getCurrentOwner();
        if ( !owner )
            return nullptr;

        std::vector<wxWinUINotebookTabMetric> metrics(pageCount);
        const float infinity =
            std::numeric_limits<float>::infinity();
        const winrt::Windows::Foundation::Size measureConstraint{
            infinity,
            static_cast<float>(
                wxMax(
                    DefaultTabStripHeightDIP * 4,
                    tabStripHeightDIP > 0
                        ? tabStripHeightDIP * 2
                        : DefaultTabStripHeightDIP * 2))};

        for ( size_t page = 0; page < pageCount; ++page )
        {
            const MUXC::TabViewItem item = items[page];
            const MUXC::Image icon = icons[page];
            const MUXC::TextBlock label = labels[page];

            item.ApplyTemplate();
            item.Measure(measureConstraint);
            owner = getCurrentOwner();
            if ( !owner )
                return nullptr;

#ifdef WXWINUI_TEST_SUPPORT
            owner->m_winui->InvokeProjectionHook(
                owner, wxWinUINotebookTestAccess::ProjectionPoint::TabGeometryMeasure);
#endif
            owner = getCurrentOwner();
            if ( !owner )
                return nullptr;

            label.Measure(measureConstraint);
            if ( icon.Visibility() == MUX::Visibility::Visible )
                icon.Measure(measureConstraint);
            owner = getCurrentOwner();
            if ( !owner )
                return nullptr;

            const auto desired = item.DesiredSize();
            const auto labelDesired = label.DesiredSize();
            const auto iconDesired = icon.DesiredSize();
            const MUX::Thickness margin = item.Margin();
            owner = getCurrentOwner();
            if ( !owner )
                return nullptr;

            wxWinUINotebookTabMetric metric;
            metric.labelWidth =
                std::isfinite(labelDesired.Width) &&
                        labelDesired.Width > 0.0f
                    ? labelDesired.Width
                    : 0.0;
            metric.labelHeight =
                std::isfinite(labelDesired.Height) &&
                        labelDesired.Height > 0.0f
                    ? labelDesired.Height
                    : 0.0;
            metric.iconWidth =
                icon.Visibility() == MUX::Visibility::Visible &&
                        std::isfinite(iconDesired.Width) &&
                        iconDesired.Width > 0.0f
                    ? iconDesired.Width
                    : 0.0;
            metric.iconHeight =
                icon.Visibility() == MUX::Visibility::Visible &&
                        std::isfinite(iconDesired.Height) &&
                        iconDesired.Height > 0.0f
                    ? iconDesired.Height
                    : 0.0;

            if ( metric.iconWidth <= 0.0 &&
                 iconPixelSizes[page].x > 0 )
            {
                metric.iconWidth =
                    static_cast<double>(iconPixelSizes[page].x) / scale;
                metric.iconHeight =
                    static_cast<double>(iconPixelSizes[page].y) / scale;
            }
            if ( metric.labelWidth <= 0.0 ||
                 metric.labelHeight <= 0.0 )
            {
                const wxSize textSize = owner->GetTextExtent(texts[page]);
                owner = getCurrentOwner();
                if ( !owner )
                    return nullptr;
                metric.labelWidth =
                    wxMax(1.0, static_cast<double>(textSize.x) / scale);
                metric.labelHeight =
                    wxMax(1.0, static_cast<double>(textSize.y) / scale);
            }

            if ( hasTabSize )
            {
                metric.width =
                    static_cast<double>(tabSize.x) / scale;
                metric.height =
                    static_cast<double>(tabSize.y) / scale;
            }
            else
            {
                metric.width =
                    std::isfinite(desired.Width) && desired.Width > 0.0f
                        ? desired.Width
                        : 0.0;
                metric.height =
                    std::isfinite(desired.Height) && desired.Height > 0.0f
                        ? desired.Height
                        : 0.0;

                const bool hasIcon = metric.iconWidth > 0.0;
                const double contentWidth =
                    metric.iconWidth + metric.labelWidth +
                    (hasIcon ? DefaultHeaderSpacingDIP : 0.0);
                const double contentHeight =
                    wxMax(metric.iconHeight, metric.labelHeight);
                if ( metric.width <= 0.0 )
                {
                    const double horizontalChrome =
                        hasTabPadding
                            ? 2.0 * tabPadding.x / scale
                            : 24.0;
                    metric.width =
                        wxMax(1.0, contentWidth + horizontalChrome);
                }
                if ( metric.height <= 0.0 )
                {
                    const double verticalChrome =
                        hasTabPadding
                            ? 2.0 * tabPadding.y / scale
                            : 12.0;
                    metric.height =
                        wxMax(1.0, contentHeight + verticalChrome);
                }
            }

            metric.leadingLTR = wxMax(0.0, margin.Left);
            metric.leadingRTL = wxMax(0.0, margin.Right);
            metric.advance =
                metric.leadingLTR + metric.width + metric.leadingRTL;
            if ( metric.advance <= 0.0 )
            {
                metric.width = 1.0;
                metric.advance =
                    metric.leadingLTR + metric.width + metric.leadingRTL;
            }

            metrics[page] = metric;
        }

        if ( fixedWidth && !hasTabSize && !metrics.empty() )
        {
            // TabWidthMode::Equal is configured at Create(), so a geometry
            // query must not mutate Min/Max/Width. Use an arranged tab as the
            // common logical width when available, otherwise the widest
            // measured header is the deterministic off-screen fallback.
            double commonWidth = 0.0;
            for ( size_t page = 0; page < pageCount; ++page )
            {
                const auto element =
                    wxWinUIGetRealizedTabItem(
                        tabView,
                        items[page],
                        page,
                        [&getCurrentOwner]()
                        {
                            return getCurrentOwner() != nullptr;
                        });
                if ( element && element.ActualWidth() > 0.0 )
                {
                    commonWidth = element.ActualWidth();
                    break;
                }
                if ( !getCurrentOwner() )
                    return nullptr;
            }
            if ( commonWidth <= 0.0 )
            {
                commonWidth = 1.0;
                for ( const auto& metric : metrics )
                    commonWidth = wxMax(commonWidth, metric.width);
            }

            for ( size_t page = 0; page < pageCount; ++page )
            {
                metrics[page].width = commonWidth;
                metrics[page].advance =
                    metrics[page].leadingLTR +
                    commonWidth +
                    metrics[page].leadingRTL;
            }
        }

        // Measuring an attached item with an unconstrained width is local to
        // this query. Restore the parent's normal constraint before publishing
        // the immutable metric snapshot.
        tabView.UpdateLayout();
        owner = getCurrentOwner();
        if ( !owner )
            return nullptr;

        if ( !hasTabSize && !fixedWidth )
        {
            for ( size_t page = 0; page < pageCount; ++page )
            {
                const auto element =
                    wxWinUIGetRealizedTabItem(
                        tabView,
                        items[page],
                        page,
                        [&getCurrentOwner]()
                        {
                            return getCurrentOwner() != nullptr;
                        });
                if ( element && element.ActualWidth() > 0.0 )
                {
                    metrics[page].width = element.ActualWidth();
                    metrics[page].advance =
                        metrics[page].leadingLTR +
                        metrics[page].width +
                        metrics[page].leadingRTL;
                }
                if ( element && element.ActualHeight() > 0.0 )
                    metrics[page].height = element.ActualHeight();

                owner = getCurrentOwner();
                if ( !owner )
                    return nullptr;
            }
        }

        std::vector<double> prefix(pageCount + 1, 0.0);
        for ( size_t page = 0; page < pageCount; ++page )
            prefix[page + 1] = prefix[page] + metrics[page].advance;

        double originDIP = 0.0;
        double topDIP = 0.0;
        size_t anchorPage = pageCount;
        const wxWinUINotebookAnchorResult anchorResult =
            wxWinUIResolveNotebookAnchor(
                tabView,
                selection,
                lastGeometryAnchor,
                items,
                metrics,
                prefix,
                [&getCurrentOwner]()
                {
                    return getCurrentOwner() != nullptr;
                },
                &originDIP,
                &topDIP,
                &anchorPage);
        owner = getCurrentOwner();
        if ( !owner ||
             anchorResult == wxWinUINotebookAnchorResult::Stale )
        {
            return nullptr;
        }

        if ( anchorResult == wxWinUINotebookAnchorResult::NotFound &&
             selection >= 0 &&
             static_cast<size_t>(selection) < pageCount &&
             clientWidthDIP > 0.0 )
        {
            const size_t selected = static_cast<size_t>(selection);
            const bool hasOverflow =
                !prefix.empty() && prefix.back() > clientWidthDIP;
            const double overflowReserve =
                hasOverflow
                    ? wxMin(clientWidthDIP,
                            static_cast<double>(
                                DefaultTabStripHeightDIP))
                    : 0.0;
            const double selectedRight =
                originDIP +
                prefix[selected] +
                metrics[selected].leadingLTR +
                metrics[selected].width;
            const double viewportRight =
                wxMax(0.0, clientWidthDIP - overflowReserve);
            if ( selectedRight > viewportRight )
                originDIP -= selectedRight - viewportRight;
        }

        impl->tabMetrics = std::move(metrics);
        impl->tabPrefixWidths = std::move(prefix);
        impl->geometryModelRevision = modelRevision;
        impl->geometryLayoutRevision = layoutRevision;
        impl->geometryScale = scale;
        impl->geometryOriginDIP = originDIP;
        impl->geometryTopDIP = topDIP;
        impl->geometryClientSize = clientSize;
        impl->geometrySelection = selection;
        impl->geometryDirection = direction;
        if ( anchorPage < pageCount )
            impl->lastGeometryAnchor = anchorPage;
        impl->geometryValid = true;
        return owner;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView geometry realization", e);
        return nullptr;
    }
}

wxRect wxNotebook::GetCachedTabRect(
    size_t page,
    int selection,
    wxLayoutDirection direction,
    double scale) const
{
    if ( !m_winui || !m_winui->callbackState ||
         !m_winui->tabView || !m_winui->geometryValid ||
         page >= m_winui->tabMetrics.size() ||
         page >= m_pages.size() ||
         page >= m_winui->items.size() ||
         m_winui->tabPrefixWidths.size() !=
             m_winui->tabMetrics.size() + 1 ||
         m_winui->geometryModelRevision != m_winui->modelRevision ||
         m_winui->geometryLayoutRevision != m_winui->layoutRevision ||
         m_winui->geometrySelection != selection ||
         m_winui->geometryDirection != direction ||
         scale <= 0.0 ||
         std::abs(m_winui->geometryScale - scale) >= 0.0001 )
    {
        return wxRect();
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const auto getIdentityOwner =
        [this,
         state,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision,
         selection,
         direction,
         scale,
         page]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner == this && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->geometryValid &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision &&
                           owner->m_winui->geometryModelRevision ==
                               modelRevision &&
                           owner->m_winui->geometryLayoutRevision ==
                               layoutRevision &&
                           owner->m_winui->geometrySelection == selection &&
                           owner->m_winui->geometryDirection == direction &&
                           std::abs(owner->m_winui->geometryScale - scale) <
                               0.0001 &&
                           page < owner->m_pages.size() &&
                           page < owner->m_winui->items.size() &&
                           page < owner->m_winui->tabMetrics.size()
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner ||
         snapshotOwner->m_winui->geometryClientSize != clientSize )
    {
        return wxRect();
    }
    const wxWeakRef<wxWindow> targetPage(snapshotOwner->m_pages[page]);
    const MUXC::TabViewItem targetItem =
        snapshotOwner->m_winui->items[page];
    wxWinUITopLevelHost * const coordinateHost =
        wxWinUITopLevelHost::FindSlotOwner(snapshotOwner);
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || !coordinateHost ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost )
        return wxRect();

    const auto isCurrent =
        [getIdentityOwner,
         selection,
         direction,
         scale,
         clientSize,
         targetPage,
         targetItem,
         coordinateHost,
         page]() -> bool
        {
            wxNotebook *owner = getIdentityOwner();
            wxWindow * const livePage = targetPage.get();
            if ( !owner || !livePage ||
                 owner->m_winui->geometryClientSize != clientSize ||
                 owner->m_pages[page] != livePage ||
                 winrt::get_unknown(owner->m_winui->items[page]) !=
                     winrt::get_unknown(targetItem) )
            {
                return false;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || currentSelection != selection )
                return false;

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || currentDirection != direction )
                return false;

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || currentClientSize != clientSize )
                return false;

            if ( wxWinUITopLevelHost::FindSlotOwner(owner) !=
                 coordinateHost )
            {
                return false;
            }
            owner = getIdentityOwner();
            if ( !owner )
                return false;

            const double currentScale = coordinateHost->GetScale();
            owner = getIdentityOwner();
            return owner &&
                   std::isfinite(currentScale) &&
                   std::abs(currentScale - scale) < 0.0001;
        };
    if ( !isCurrent() )
        return wxRect();

    const wxWinUINotebookTabMetric& metric =
        impl->tabMetrics[page];
    const double leftDIP =
        impl->geometryOriginDIP +
        impl->tabPrefixWidths[page] +
        metric.leadingLTR;
    const double rightDIP = leftDIP + metric.width;
    const double topDIP = impl->geometryTopDIP;
    const double bottomDIP = topDIP + metric.height;

    try
    {
        const MUX::UIElement tabViewElement =
            tabView.as<MUX::UIElement>();
        if ( !isCurrent() )
            return wxRect();

        // geometryOriginDIP and the prefix metrics use TabView's logical
        // local axis. Mapping their enclosing bounds through the shared
        // mapper yields the same wx client-space convention as native
        // TCM_GETITEMRECT in both LTR and RTL.
        return wxWinUIMapNotebookBoundsToClient(
            const_cast<wxNotebook *>(this),
            tabViewElement,
            winrt::Windows::Foundation::Rect{
                static_cast<float>(leftDIP),
                static_cast<float>(topDIP),
                static_cast<float>(rightDIP - leftDIP),
                static_cast<float>(bottomDIP - topDIP)},
            isCurrent);
    }
    catch ( const winrt::hresult_error& )
    {
        return wxRect();
    }
}

wxRect wxNotebook::GetCachedTabPartRect(
    size_t page,
    int part,
    int selection,
    wxLayoutDirection direction,
    double scale) const
{
    if ( part == wxWinUINotebookTabPart_Item )
        return GetCachedTabRect(page, selection, direction, scale);

    if ( !m_winui || !m_winui->callbackState ||
         !m_winui->tabView || !m_winui->geometryValid ||
         page >= m_winui->tabMetrics.size() ||
         page >= m_pages.size() ||
         page >= m_winui->items.size() ||
         m_winui->tabPrefixWidths.size() !=
             m_winui->tabMetrics.size() + 1 ||
         m_winui->geometryModelRevision != m_winui->modelRevision ||
         m_winui->geometryLayoutRevision != m_winui->layoutRevision ||
         m_winui->geometrySelection != selection ||
         m_winui->geometryDirection != direction ||
         scale <= 0.0 ||
         std::abs(m_winui->geometryScale - scale) >= 0.0001 )
    {
        return wxRect();
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const auto getIdentityOwner =
        [this,
         state,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision,
         selection,
         direction,
         scale,
         page]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner == this && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->geometryValid &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision &&
                           owner->m_winui->geometryModelRevision ==
                               modelRevision &&
                           owner->m_winui->geometryLayoutRevision ==
                               layoutRevision &&
                           owner->m_winui->geometrySelection == selection &&
                           owner->m_winui->geometryDirection == direction &&
                           std::abs(owner->m_winui->geometryScale - scale) <
                               0.0001 &&
                           page < owner->m_pages.size() &&
                           page < owner->m_winui->items.size() &&
                           page < owner->m_winui->tabMetrics.size()
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner ||
         snapshotOwner->m_winui->geometryClientSize != clientSize )
    {
        return wxRect();
    }
    const wxWeakRef<wxWindow> targetPage(snapshotOwner->m_pages[page]);
    const MUXC::TabViewItem targetItem =
        snapshotOwner->m_winui->items[page];
    wxWinUITopLevelHost * const coordinateHost =
        wxWinUITopLevelHost::FindSlotOwner(snapshotOwner);
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || !coordinateHost ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost )
        return wxRect();

    const auto isCurrent =
        [getIdentityOwner,
         selection,
         direction,
         scale,
         clientSize,
         targetPage,
         targetItem,
         coordinateHost,
         page]() -> bool
        {
            wxNotebook *owner = getIdentityOwner();
            wxWindow * const livePage = targetPage.get();
            if ( !owner || !livePage ||
                 owner->m_winui->geometryClientSize != clientSize ||
                 owner->m_pages[page] != livePage ||
                 winrt::get_unknown(owner->m_winui->items[page]) !=
                     winrt::get_unknown(targetItem) )
            {
                return false;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || currentSelection != selection )
                return false;

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || currentDirection != direction )
                return false;

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || currentClientSize != clientSize )
                return false;

            if ( wxWinUITopLevelHost::FindSlotOwner(owner) !=
                 coordinateHost )
            {
                return false;
            }
            owner = getIdentityOwner();
            if ( !owner )
                return false;

            const double currentScale = coordinateHost->GetScale();
            owner = getIdentityOwner();
            return owner &&
                   std::isfinite(currentScale) &&
                   std::abs(currentScale - scale) < 0.0001;
        };
    if ( !isCurrent() )
        return wxRect();

    const wxWinUINotebookTabMetric metric =
        impl->tabMetrics[page];
    double partWidth = 0.0;
    double partHeight = 0.0;
    switch ( part )
    {
        case wxWinUINotebookTabPart_Icon:
            partWidth = metric.iconWidth;
            partHeight = metric.iconHeight;
            break;

        case wxWinUINotebookTabPart_Label:
            partWidth = metric.labelWidth;
            partHeight = metric.labelHeight;
            break;

        default:
            return wxRect();
    }

    if ( partWidth <= 0.0 || partHeight <= 0.0 )
        return wxRect();

    const bool hasIcon = metric.iconWidth > 0.0;
    const double spacing = hasIcon ? DefaultHeaderSpacingDIP : 0.0;
    const double contentWidth =
        metric.iconWidth + spacing + metric.labelWidth;
    const double tabLeftDIP =
        impl->geometryOriginDIP +
        impl->tabPrefixWidths[page] +
        metric.leadingLTR;
    const double tabTopDIP = impl->geometryTopDIP;
    const double contentLeftDIP =
        tabLeftDIP + wxMax(0.0, (metric.width - contentWidth) / 2.0);

    double leftDIP = contentLeftDIP;
    // Like the item bounds, child bounds mapped back into wx client
    // coordinates already use the WS_EX_LAYOUTRTL logical axis. The TabView
    // reports icon then label in that axis for both directions; mirroring the
    // cached sub-parts again would make HitTest label an actual icon as text.
    if ( part == wxWinUINotebookTabPart_Label )
        leftDIP += metric.iconWidth + spacing;

    const double topDIP =
        tabTopDIP + wxMax(0.0, (metric.height - partHeight) / 2.0);

    try
    {
        const MUX::UIElement tabViewElement =
            tabView.as<MUX::UIElement>();
        if ( !isCurrent() )
            return wxRect();

        return wxWinUIMapNotebookBoundsToClient(
            const_cast<wxNotebook *>(this),
            tabViewElement,
            winrt::Windows::Foundation::Rect{
                static_cast<float>(leftDIP),
                static_cast<float>(topDIP),
                static_cast<float>(partWidth),
                static_cast<float>(partHeight)},
            isCurrent);
    }
    catch ( const winrt::hresult_error& )
    {
        return wxRect();
    }
}

wxRect wxNotebook::GetRealizedTabElementRect(
    size_t page,
    int part,
    bool allowCachedFallback) const
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->tabView ||
         page >= m_pages.size() || page >= m_winui->items.size() )
    {
        return wxRect();
    }

    if ( UsesExtendedPeerSurface() )
    {
        wxWinUINotebookImpl * const initialImpl = m_winui.get();
        const auto state = initialImpl->callbackState;
        const std::uint64_t callbackGeneration = state->Generation();

        const auto getEntryOwner =
            [this, state, callbackGeneration, initialImpl]()
            {
                wxNotebook * const owner =
                    state->GetOwner(callbackGeneration);
                return owner == this && owner->m_winui &&
                       owner->m_winui.get() == initialImpl &&
                       owner->m_winui->callbackState == state &&
                       !owner->m_winui->closed
                           ? owner
                           : nullptr;
            };

        wxWinUINotebookImpl * const impl = initialImpl;
        wxNotebook *owner = getEntryOwner();
        if ( !owner )
            return wxRect();

        if ( allowCachedFallback )
        {
            if ( !owner->LayoutExtendedPeerSurface() )
                return wxRect();
            owner = getEntryOwner();
        }
        else if ( state->IsGeometryRealizationInProgress() ||
                  !impl->extendedLayoutValid || !impl->extendedRoot ||
                  impl->extendedAttachedGeneration !=
                      impl->extendedSurfaceGeneration ||
                  impl->extendedLayoutModelRevision !=
                      impl->modelRevision ||
                  impl->extendedLayoutRevision != impl->layoutRevision )
        {
            // Strict actual-tree seams are observations, never repair paths.
            // If no current publication already exists, fail closed and let
            // the caller pump the ordinary dispatcher/layout lifecycle.
            return wxRect();
        }

        if ( !owner || page >= owner->m_pages.size() ||
             page >= impl->extendedPeers.size() )
        {
            return wxRect();
        }

        // The production/cache path may synchronize and therefore retains the
        // realization guard across its subsequent read. The strict seam does
        // not even acquire that mutating guard: it only rejects an in-progress
        // transaction above and observes the already-published generation.
        std::unique_ptr<wxWinUINotebookGeometryGuard> geometryGuard;
        if ( allowCachedFallback )
        {
            geometryGuard =
                std::make_unique<wxWinUINotebookGeometryGuard>(state);
            if ( !geometryGuard->IsAcquired() )
                return wxRect();
        }
        owner = getEntryOwner();
        if ( !owner )
            return wxRect();

        const std::uint64_t surfaceGeneration =
            impl->extendedSurfaceGeneration;
        const std::uint64_t modelRevision = impl->modelRevision;
        const std::uint64_t layoutRevision = impl->layoutRevision;
        const std::uint64_t styleRevision = impl->styleRevision;
        const std::uint64_t publishedModelRevision =
            impl->extendedLayoutModelRevision;
        const std::uint64_t publishedLayoutRevision =
            impl->extendedLayoutRevision;
        const MUXC::Canvas surfaceRoot = impl->extendedRoot;
        const auto getIdentityOwner =
            [this,
             state,
             callbackGeneration,
              impl,
              surfaceGeneration,
              modelRevision,
              layoutRevision,
              styleRevision,
              publishedModelRevision,
              publishedLayoutRevision,
              surfaceRoot,
              page]()
            {
                wxNotebook * const owner =
                    state->GetOwner(callbackGeneration);
                return owner == this && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == state &&
                       owner->m_winui->extendedSurfaceGeneration ==
                            surfaceGeneration &&
                       owner->m_winui->modelRevision == modelRevision &&
                       owner->m_winui->layoutRevision == layoutRevision &&
                       owner->m_winui->styleRevision == styleRevision &&
                       owner->m_winui->extendedLayoutModelRevision ==
                           publishedModelRevision &&
                       owner->m_winui->extendedLayoutRevision ==
                           publishedLayoutRevision &&
                       owner->m_winui->extendedRoot == surfaceRoot &&
                       page < owner->m_pages.size() &&
                       page < owner->m_winui->extendedPeers.size()
                           ? owner
                           : nullptr;
            };
        const auto isCurrent =
            [getIdentityOwner]()
            {
                wxNotebook * const owner = getIdentityOwner();
                return owner && owner->m_winui->extendedLayoutValid &&
                       owner->m_winui->extendedLayoutModelRevision ==
                           owner->m_winui->modelRevision &&
                       owner->m_winui->extendedLayoutRevision ==
                           owner->m_winui->layoutRevision &&
                       owner->m_winui->extendedAttachedGeneration ==
                           owner->m_winui->extendedSurfaceGeneration;
            };

        wxRect publishedRect;
        switch ( part )
        {
            case wxWinUINotebookTabPart_Item:
                if ( page < impl->extendedTabRects.size() )
                    publishedRect = impl->extendedTabRects[page];
                break;
            case wxWinUINotebookTabPart_Icon:
                if ( page < impl->extendedIconRects.size() )
                    publishedRect = impl->extendedIconRects[page];
                break;
            case wxWinUINotebookTabPart_Label:
                if ( page < impl->extendedLabelRects.size() )
                    publishedRect = impl->extendedLabelRects[page];
                break;
            default:
                return wxRect();
        }
        const auto isPublishedFrame =
            [&publishedRect, impl, page, part](const wxRect& actual)
            {
                // During the probe turn the cached wx rectangle is only a
                // staging frame: the unconstrained TextBlock is deliberately
                // realizing its intrinsic size. Never let an accidentally
                // equal stale rectangle satisfy the strict actual-tree seam.
                if ( part == wxWinUINotebookTabPart_Label &&
                     (page >= impl->extendedLabelMetricPending.size() ||
                      impl->extendedLabelMetricPending[page]) )
                {
                    return false;
                }
                const auto isNear = [](int first, int second)
                {
                    return std::abs(
                               static_cast<long long>(first) -
                               static_cast<long long>(second)) <= 1;
                };
                return !actual.IsEmpty() && !publishedRect.IsEmpty() &&
                       isNear(actual.x, publishedRect.x) &&
                       isNear(actual.y, publishedRect.y) &&
                       isNear(actual.width, publishedRect.width) &&
                       isNear(actual.height, publishedRect.height);
            };

        MUX::FrameworkElement element{ nullptr };
        switch ( part )
        {
            case wxWinUINotebookTabPart_Item:
                element = impl->extendedPeers[page].item;
                break;
            case wxWinUINotebookTabPart_Icon:
                element = impl->extendedPeers[page].icon;
                break;
            case wxWinUINotebookTabPart_Label:
                element = impl->extendedPeers[page].label;
                break;
            default:
                return wxRect();
        }

        try
        {
            // Prefer an already-arranged peer. In particular, don't
            // invalidate a connected child while an ancestor layout tick may
            // still be pending: following that with another synchronous
            // ForceRender/UpdateLayout pass makes WinUI exhaust its layout
            // iteration budget and fail-fast with AG_E_LAYOUT_CYCLE.
            const auto readActual = [&]()
            {
                return part == wxWinUINotebookTabPart_Item
                    ? wxWinUIGetRealizedElementRectInNotebook(
                          owner, element, isCurrent)
                    : wxWinUIGetVisibleRealizedElementRectInNotebook(
                          owner, element, isCurrent);
            };
            const wxRect actual = readActual();
            owner = getIdentityOwner();
            if ( !owner || !isCurrent() )
                return wxRect();
            // This seam is deliberately passive. The XAML element retains
            // its natural ActualWidth/ActualHeight and the real live Clip
            // supplies the visible frame, so no synchronous render/layout is
            // needed (or permitted) to make the two agree.
            if ( isPublishedFrame(actual) )
                return actual;

            if ( !allowCachedFallback )
                return wxRect();
        }
        catch ( const winrt::hresult_error& )
        {
            if ( !allowCachedFallback )
                return wxRect();
        }

        if ( !isCurrent() )
            return wxRect();
        switch ( part )
        {
            case wxWinUINotebookTabPart_Item:
                return page < impl->extendedTabRects.size()
                           ? impl->extendedTabRects[page]
                           : wxRect();
            case wxWinUINotebookTabPart_Icon:
                return page < impl->extendedIconRects.size()
                           ? impl->extendedIconRects[page]
                           : wxRect();
            case wxWinUINotebookTabPart_Label:
                return page < impl->extendedLabelRects.size()
                           ? impl->extendedLabelRects[page]
                           : wxRect();
            default:
                return wxRect();
        }
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const auto getIdentityOwner =
        [state,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const int selection = snapshotOwner->GetSelection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const wxLayoutDirection direction =
        snapshotOwner->GetLayoutDirection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    wxWinUITopLevelHost * const coordinateHost =
        wxWinUITopLevelHost::FindSlotOwner(
            snapshotOwner);
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || !coordinateHost ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost )
        return wxRect();
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || page >= snapshotOwner->m_pages.size() ||
         page >= snapshotOwner->m_winui->items.size() )
    {
        return wxRect();
    }
    const wxWeakRef<wxWindow> targetPage(snapshotOwner->m_pages[page]);
    const MUXC::TabViewItem targetItem =
        snapshotOwner->m_winui->items[page];
    const MUXC::Image targetIcon =
        page < snapshotOwner->m_winui->icons.size()
            ? snapshotOwner->m_winui->icons[page]
            : MUXC::Image{ nullptr };
    const MUXC::TextBlock targetLabel =
        page < snapshotOwner->m_winui->labels.size()
            ? snapshotOwner->m_winui->labels[page]
            : MUXC::TextBlock{ nullptr };
    const double scale = coordinateHost->GetScale();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost ||
         !(scale > 0.0) || !std::isfinite(scale) )
        return wxRect();

    const auto getCurrentOwner =
        [getIdentityOwner,
         selection,
         direction,
         coordinateHost,
         scale,
         clientSize,
         targetPage,
         targetItem,
         targetIcon,
         targetLabel,
         page]() -> wxNotebook *
        {
            wxNotebook *owner = getIdentityOwner();
            wxWindow * const livePage = targetPage.get();
            if ( !owner || !livePage ||
                 page >= owner->m_pages.size() ||
                 page >= owner->m_winui->items.size() ||
                 owner->m_pages[page] != livePage ||
                 winrt::get_unknown(owner->m_winui->items[page]) !=
                     winrt::get_unknown(targetItem) ||
                 (targetIcon &&
                  (page >= owner->m_winui->icons.size() ||
                   winrt::get_unknown(owner->m_winui->icons[page]) !=
                       winrt::get_unknown(targetIcon))) ||
                 (targetLabel &&
                  (page >= owner->m_winui->labels.size() ||
                   winrt::get_unknown(owner->m_winui->labels[page]) !=
                       winrt::get_unknown(targetLabel))) )
            {
                return nullptr;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || currentSelection != selection )
                return nullptr;

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || currentDirection != direction )
                return nullptr;

            if ( wxWinUITopLevelHost::FindSlotOwner(owner) !=
                 coordinateHost )
            {
                return nullptr;
            }
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || currentClientSize != clientSize )
                return nullptr;

            const double currentScale = coordinateHost->GetScale();
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;
            return std::isfinite(currentScale) &&
                           std::abs(currentScale - scale) < 0.0001
                ? owner
                : nullptr;
        };

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return wxRect();

    if ( state->IsGeometryRealizationInProgress() )
        return wxRect();

    if ( allowCachedFallback )
    {
        owner = owner->RealizeTabGeometryForQuery();
        if ( !owner || !getCurrentOwner() )
            return wxRect();
    }
    // The strict actual-tree seam is an observation, just like its extended
    // counterpart above. In particular, it must not force a nested
    // ApplyTemplate/UpdateLayout pass while the stock TabView is raising
    // Loaded. If the requested peer has not been naturally arranged yet, the
    // reads below fail closed and the caller can pump the normal XAML
    // lifecycle.

    try
    {
        MUX::FrameworkElement element{ nullptr };
        switch ( part )
        {
            case wxWinUINotebookTabPart_Item:
                element = wxWinUIGetRealizedTabItem(
                    tabView,
                    targetItem,
                    page,
                    [&getCurrentOwner]()
                    {
                        return getCurrentOwner() != nullptr;
                    });
                if ( !getCurrentOwner() )
                    return wxRect();
                break;

            case wxWinUINotebookTabPart_Icon:
                if ( !targetIcon )
                    return wxRect();
                element = targetIcon.as<MUX::FrameworkElement>();
                if ( !getCurrentOwner() )
                    return wxRect();
                break;

            case wxWinUINotebookTabPart_Label:
                if ( !targetLabel )
                    return wxRect();
                element = targetLabel.as<MUX::FrameworkElement>();
                if ( !getCurrentOwner() )
                    return wxRect();
                break;

            default:
                return wxRect();
        }

        const wxRect rect =
            wxWinUIGetRealizedElementRectInNotebook(
                owner, element, [&getCurrentOwner]()
                {
                    return getCurrentOwner() != nullptr;
                });
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();

        if ( !allowCachedFallback )
            return rect;

        // A non-input/private desktop can arrange the TabViewItem and its
        // label at a temporary template origin while leaving an Image child
        // without an ActualSize. Keep this implementation-only seam coherent
        // by preferring the atomically published header sub-rect, but only for
        // a logical tab that intersects the visible header band.
        const wxRect itemRect =
            owner->GetCachedTabRect(
                page, selection, direction, scale);
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        const wxSize currentClientSize = owner->GetClientSize();
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        const wxRect pageRect = owner->GetPageRect();
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        const int fallbackStripHeight =
            owner->FromDIP(DefaultTabStripHeightDIP);
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        const int stripBottom =
            wxMin(currentClientSize.y,
                  wxMax(pageRect.y, fallbackStripHeight));
        const wxRect visibleStrip(
            0, 0, currentClientSize.x, wxMax(0, stripBottom));
        if ( !itemRect.IsEmpty() && itemRect.Intersects(visibleStrip) )
        {
            const wxRect cachedPart =
                owner->GetCachedTabPartRect(
                    page, part, selection, direction, scale);
            owner = getCurrentOwner();
            if ( !owner )
                return wxRect();
            if ( !cachedPart.IsEmpty() )
                return cachedPart;
        }
        return rect;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView tab geometry", e);
        return wxRect();
    }
}

wxRect wxNotebook::GetTabRect(size_t page) const
{
    wxCHECK_MSG( page < m_pages.size(), wxRect(),
                 "invalid notebook page" );

    if ( !m_winui || !m_winui->callbackState || !m_winui->tabView ||
         page >= m_winui->items.size() )
    {
        return wxRect();
    }

    if ( UsesExtendedPeerSurface() )
    {
        wxNotebook * const self = const_cast<wxNotebook *>(this);
        if ( !self->LayoutExtendedPeerSurface() || !m_winui ||
             !m_winui->extendedLayoutValid ||
             page >= m_winui->extendedTabRects.size() )
        {
            return wxRect();
        }
        return m_winui->extendedTabRects[page];
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const MUXC::TabView tabView = impl->tabView;
    const auto getIdentityOwner =
        [state,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const int selection = snapshotOwner->GetSelection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    const wxLayoutDirection direction =
        snapshotOwner->GetLayoutDirection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxRect();
    wxWinUITopLevelHost * const coordinateHost =
        wxWinUITopLevelHost::FindSlotOwner(
            snapshotOwner);
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || !coordinateHost ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost )
        return wxRect();
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || page >= snapshotOwner->m_pages.size() ||
         page >= snapshotOwner->m_winui->items.size() )
    {
        return wxRect();
    }
    const wxWeakRef<wxWindow> targetPage(snapshotOwner->m_pages[page]);
    const MUXC::TabViewItem targetItem =
        snapshotOwner->m_winui->items[page];
    const double scale = coordinateHost->GetScale();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost ||
         !(scale > 0.0) || !std::isfinite(scale) )
        return wxRect();

    const auto getCurrentOwner =
        [getIdentityOwner,
         selection,
         direction,
         coordinateHost,
         scale,
         clientSize,
         targetPage,
         targetItem,
         page]() -> wxNotebook *
        {
            wxNotebook *owner = getIdentityOwner();
            wxWindow * const livePage = targetPage.get();
            if ( !owner || !livePage ||
                 page >= owner->m_pages.size() ||
                 page >= owner->m_winui->items.size() ||
                 owner->m_pages[page] != livePage ||
                 winrt::get_unknown(owner->m_winui->items[page]) !=
                     winrt::get_unknown(targetItem) )
            {
                return nullptr;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || currentSelection != selection )
                return nullptr;

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || currentDirection != direction )
                return nullptr;

            if ( wxWinUITopLevelHost::FindSlotOwner(owner) !=
                 coordinateHost )
            {
                return nullptr;
            }
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || currentClientSize != clientSize )
                return nullptr;

            const double currentScale = coordinateHost->GetScale();
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;
            return std::isfinite(currentScale) &&
                           std::abs(currentScale - scale) < 0.0001
                ? owner
                : nullptr;
        };

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return wxRect();

    if ( state->IsGeometryRealizationInProgress() )
    {
        const wxRect cached =
            owner->GetCachedTabRect(
                page, selection, direction, scale);
        return getCurrentOwner() ? cached : wxRect();
    }

    owner = owner->RealizeTabGeometryForQuery();
    if ( !owner )
        return wxRect();

    owner = getCurrentOwner();
    if ( !owner )
        return wxRect();

    try
    {
        const auto element =
            wxWinUIGetRealizedTabItem(
                tabView,
                targetItem,
                page,
                [&getCurrentOwner]()
                {
                    return getCurrentOwner() != nullptr;
                });
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        const wxRect clientRect =
            wxWinUIGetRealizedElementRectInNotebook(
                owner, element, [&getCurrentOwner]()
                {
                    return getCurrentOwner() != nullptr;
                });
        owner = getCurrentOwner();
        if ( !owner )
            return wxRect();
        if ( !clientRect.IsEmpty() )
            return clientRect;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView tab geometry", e);
        if ( !getCurrentOwner() )
            return wxRect();
    }

    const wxRect rect =
        owner->GetCachedTabRect(page, selection, direction, scale);
    return getCurrentOwner() ? rect : wxRect();
}

int wxNotebook::HitTest(const wxPoint& pt, long *flags) const
{
    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    if ( !m_winui || !m_winui->callbackState || !m_winui->tabView )
        return wxNOT_FOUND;

    if ( UsesExtendedPeerSurface() )
    {
        wxNotebook * const self = const_cast<wxNotebook *>(this);
        if ( !self->LayoutExtendedPeerSurface() || !m_winui ||
             !m_winui->extendedLayoutValid )
        {
            return wxNOT_FOUND;
        }

        const wxRect client(wxPoint(), GetClientSize());
        if ( !client.Contains(pt) )
            return wxNOT_FOUND;
        const bool insideTabViewport =
            m_winui->extendedViewportRect.Contains(pt);

        for ( size_t page = 0;
              insideTabViewport &&
                  page < m_winui->extendedTabRects.size();
              ++page )
        {
            const wxRect& item = m_winui->extendedTabRects[page];
            if ( !item.Contains(pt) )
                continue;

            // Match native TCM_HITTEST conservatism on the exact arranged
            // boundary (rounded template corners do not own those pixels).
            if ( pt.x == item.GetLeft() || pt.x == item.GetRight() ||
                 pt.y == item.GetTop() || pt.y == item.GetBottom() )
            {
                return wxNOT_FOUND;
            }

            if ( flags )
            {
                const wxRect icon =
                    page < m_winui->extendedIconRects.size()
                        ? m_winui->extendedIconRects[page]
                        : wxRect();
                const wxRect label =
                    page < m_winui->extendedLabelRects.size()
                        ? m_winui->extendedLabelRects[page]
                        : wxRect();
                *flags =
                    !icon.IsEmpty() && icon.Contains(pt)
                        ? wxBK_HITTEST_ONICON
                        : !label.IsEmpty() && label.Contains(pt)
                            ? wxBK_HITTEST_ONLABEL
                            : wxBK_HITTEST_ONITEM;
            }
            return static_cast<int>(page);
        }

        if ( flags && GetExtendedPageRect().Contains(pt) )
            *flags |= wxBK_HITTEST_ONPAGE;
        return wxNOT_FOUND;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    wxWinUINotebookImpl * const impl = m_winui.get();
    const MUXC::TabView tabView = impl->tabView;
    const std::uint64_t modelRevision = impl->modelRevision;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const auto getIdentityOwner =
        [state,
         callbackGeneration,
         impl,
         tabView,
         modelRevision,
         layoutRevision]() -> wxNotebook *
        {
            wxNotebook * const owner =
                state->GetOwner(callbackGeneration);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !owner->m_winui->closed &&
                           owner->m_winui->tabView == tabView &&
                           owner->m_winui->modelRevision == modelRevision &&
                           owner->m_winui->layoutRevision == layoutRevision
                       ? owner
                       : nullptr;
        };

    wxNotebook *snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxNOT_FOUND;
    const int selection = snapshotOwner->GetSelection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxNOT_FOUND;
    const wxLayoutDirection direction =
        snapshotOwner->GetLayoutDirection();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxNOT_FOUND;
    wxWinUITopLevelHost * const coordinateHost =
        wxWinUITopLevelHost::FindSlotOwner(
            snapshotOwner);
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner || !coordinateHost ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost )
        return wxNOT_FOUND;
    const wxSize clientSize = snapshotOwner->GetClientSize();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner )
        return wxNOT_FOUND;

    std::vector<MUXC::TabViewItem> items;
    std::vector<MUXC::Image> icons;
    std::vector<MUXC::TextBlock> labels;
    try
    {
        items = snapshotOwner->m_winui->items;
        icons = snapshotOwner->m_winui->icons;
        labels = snapshotOwner->m_winui->labels;
    }
    catch ( const std::exception& )
    {
        return wxNOT_FOUND;
    }

    const double scale = coordinateHost->GetScale();
    snapshotOwner = getIdentityOwner();
    if ( !snapshotOwner ||
         wxWinUITopLevelHost::FindSlotOwner(snapshotOwner) !=
             coordinateHost ||
         !(scale > 0.0) || !std::isfinite(scale) )
        return wxNOT_FOUND;

    const auto getCurrentOwner =
        [getIdentityOwner,
         selection,
         direction,
         coordinateHost,
         scale,
         clientSize,
         &items,
         &icons,
         &labels]() -> wxNotebook *
        {
            wxNotebook *owner = getIdentityOwner();
            if ( !owner ||
                 owner->m_pages.size() != items.size() ||
                 owner->m_winui->items.size() != items.size() ||
                 owner->m_winui->icons.size() != icons.size() ||
                 owner->m_winui->labels.size() != labels.size() )
            {
                return nullptr;
            }

            const int currentSelection = owner->GetSelection();
            owner = getIdentityOwner();
            if ( !owner || currentSelection != selection )
                return nullptr;

            const wxLayoutDirection currentDirection =
                owner->GetLayoutDirection();
            owner = getIdentityOwner();
            if ( !owner || currentDirection != direction )
                return nullptr;

            if ( wxWinUITopLevelHost::FindSlotOwner(owner) !=
                 coordinateHost )
            {
                return nullptr;
            }
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;

            const wxSize currentClientSize = owner->GetClientSize();
            owner = getIdentityOwner();
            if ( !owner || currentClientSize != clientSize )
                return nullptr;

            const double currentScale = coordinateHost->GetScale();
            owner = getIdentityOwner();
            if ( !owner )
                return nullptr;
            return std::isfinite(currentScale) &&
                           std::abs(currentScale - scale) < 0.0001
                ? owner
                : nullptr;
        };

    wxNotebook *owner = getCurrentOwner();
    if ( !owner )
        return wxNOT_FOUND;
    if ( state->IsGeometryRealizationInProgress() )
        return wxNOT_FOUND;
    owner = owner->RealizeTabGeometryForQuery();
    if ( !owner || !getCurrentOwner() )
        return wxNOT_FOUND;

    bool allowLogicalFallback = false;
    wxRect measuredViewport;
    bool hasMeasuredViewport = false;
    try
    {
        const auto tabViewElement = tabView.as<MUX::UIElement>();
        const auto tabViewIdentity =
            wxWinUINotebookObjectIdentity(tabViewElement);
        const auto xamlRoot = tabViewElement.XamlRoot();
        const auto rootContent =
            xamlRoot ? xamlRoot.Content() : MUX::UIElement{ nullptr };
        if ( !getCurrentOwner() )
            return wxNOT_FOUND;

        int exactCandidate = wxNOT_FOUND;
        long exactCandidateFlags = wxBK_HITTEST_NOWHERE;
        if ( rootContent )
        {
            winrt::Windows::Foundation::Point hostPoint{};
            if ( wxWinUIVisualCoordinates::ClientPointToRoot(
                     owner,
                     winrt::Windows::Foundation::Point{
                         static_cast<float>(pt.x),
                         static_cast<float>(pt.y)},
                     &hostPoint) != wxWinUICoordinateResult::Mapped ||
                 !getCurrentOwner() )
            {
                return wxNOT_FOUND;
            }

            const auto hits =
                MUX::Media::VisualTreeHelper::
                    FindElementsInHostCoordinates(
                        hostPoint, tabViewElement);
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;

            bool buttonOwnsPoint = false;
            for ( const auto& hit : hits )
            {
                if ( !getCurrentOwner() )
                    return wxNOT_FOUND;
                if ( wxWinUINotebookObjectIdentity(hit) ==
                     tabViewIdentity )
                {
                    continue;
                }

                // Inspect every returned visual and its complete ancestor
                // chain. FindElementsInHostCoordinates() doesn't document an
                // ordering contract, so a background reported before an
                // overflow button must not make the logical fallback pierce
                // that button.
                MUX::DependencyObject node = hit;
                constexpr size_t MaxAncestorDepth = 512;
                size_t depth = 0;
                while ( node && depth++ < MaxAncestorDepth )
                {
                    const auto identity =
                        wxWinUINotebookObjectIdentity(node);
                    for ( size_t page = 0;
                          page < items.size();
                          ++page )
                    {
                        long hitFlags = wxBK_HITTEST_ONITEM;
                        bool matches = false;
                        if ( page < icons.size() && icons[page] &&
                             identity ==
                                 wxWinUINotebookObjectIdentity(icons[page]) )
                        {
                            hitFlags = wxBK_HITTEST_ONICON;
                            matches = true;
                        }
                        else if ( page < labels.size() && labels[page] &&
                                  identity ==
                                      wxWinUINotebookObjectIdentity(
                                          labels[page]) )
                        {
                            hitFlags = wxBK_HITTEST_ONLABEL;
                            matches = true;
                        }
                        else if ( items[page] &&
                                  identity ==
                                      wxWinUINotebookObjectIdentity(
                                          items[page]) )
                        {
                            matches = true;
                        }

                        if ( matches )
                        {
                            // Keep scanning: a later hit can be an overflow
                            // ButtonBase overlaying this otherwise valid tab.
                            // Prefer the more specific icon/label descendant
                            // over its TabViewItem ancestor.
                            if ( exactCandidate == wxNOT_FOUND ||
                                 exactCandidateFlags ==
                                     wxBK_HITTEST_ONITEM )
                            {
                                exactCandidate =
                                    static_cast<int>(page);
                                exactCandidateFlags = hitFlags;
                            }
                        }
                    }

                    if ( identity == tabViewIdentity )
                        break;
                    if ( node.try_as<MUXCP::ButtonBase>() )
                        buttonOwnsPoint = true;
                    node =
                        MUX::Media::VisualTreeHelper::GetParent(node);
                    if ( !getCurrentOwner() )
                        return wxNOT_FOUND;
                }
                if ( node && depth >= MaxAncestorDepth )
                    return wxNOT_FOUND;
            }

            if ( buttonOwnsPoint )
                return wxNOT_FOUND;
        }

        const bool geometryOverflows =
            !impl->tabPrefixWidths.empty() &&
            impl->tabPrefixWidths.back() * scale > clientSize.x;
        if ( !geometryOverflows )
        {
            // Create() hides the add button and makes every TabViewItem
            // non-closable. Without overflow there is therefore no header
            // chrome to pierce, even when a cold/non-input desktop hasn't
            // materialized the private visual tree yet.
            allowLogicalFallback = true;
        }
        else
        {
            // The XAML island can report only a ScrollViewer background over
            // a realized tab -- and an isolated desktop can temporarily
            // expose no XamlRoot content at all. For an overflowing strip,
            // geometry is authoritative only inside the actual private
            // ListView viewport and away from every visible template button.
            const wxWinUINotebookTemplateSnapshot templateSnapshot =
                wxWinUIInspectNotebookTemplate(
                    tabView,
                    [&getCurrentOwner]()
                    {
                        return getCurrentOwner() != nullptr;
                    });
            owner = getCurrentOwner();
            if ( !owner || !templateSnapshot.complete ||
                 !templateSnapshot.tabList )
            {
                return wxNOT_FOUND;
            }

            bool unknownChromeBounds = false;
            for ( const MUXCP::ButtonBase& button :
                  templateSnapshot.buttons )
            {
                if ( !button )
                    continue;

                const MUX::Visibility visibility = button.Visibility();
                if ( !getCurrentOwner() )
                    return wxNOT_FOUND;
                const bool isHitTestVisible = button.IsHitTestVisible();
                if ( !getCurrentOwner() )
                    return wxNOT_FOUND;
                if ( visibility != MUX::Visibility::Visible ||
                     !isHitTestVisible )
                {
                    continue;
                }

                const double buttonWidth = button.ActualWidth();
                if ( !getCurrentOwner() )
                    return wxNOT_FOUND;
                const double buttonHeight = button.ActualHeight();
                if ( !getCurrentOwner() )
                    return wxNOT_FOUND;
                if ( !std::isfinite(buttonWidth) ||
                     !std::isfinite(buttonHeight) )
                {
                    unknownChromeBounds = true;
                    continue;
                }
                // A template can leave collapsed close/add/navigation
                // ButtonBase descendants at Visibility::Visible while their
                // arranged size is zero. They cannot own a point and are not
                // active chrome.
                if ( buttonWidth <= 0.0 || buttonHeight <= 0.0 )
                    continue;

                const wxRect buttonRect =
                    wxWinUIGetRealizedElementRectInNotebook(
                        owner,
                        button.as<MUX::FrameworkElement>(),
                        [&getCurrentOwner]()
                        {
                            return getCurrentOwner() != nullptr;
                        });
                owner = getCurrentOwner();
                if ( !owner )
                    return wxNOT_FOUND;
                if ( buttonRect.IsEmpty() )
                {
                    unknownChromeBounds = true;
                    continue;
                }
                if ( buttonRect.Contains(pt) )
                    return wxNOT_FOUND;
            }

            measuredViewport =
                wxWinUIGetRealizedElementRectInNotebook(
                    owner,
                    templateSnapshot.tabList.as<MUX::FrameworkElement>(),
                    [&getCurrentOwner]()
                    {
                        return getCurrentOwner() != nullptr;
                    });
            owner = getCurrentOwner();
            if ( !owner || unknownChromeBounds ||
                 measuredViewport.IsEmpty() )
            {
                return wxNOT_FOUND;
            }

            hasMeasuredViewport = true;
            if ( exactCandidate != wxNOT_FOUND )
            {
                if ( !measuredViewport.Contains(pt) )
                    return wxNOT_FOUND;
                if ( exactCandidateFlags == wxBK_HITTEST_ONITEM )
                {
                    const wxRect logicalItem =
                        owner->GetCachedTabRect(
                            static_cast<size_t>(exactCandidate),
                            selection,
                            direction,
                            scale);
                    owner = getCurrentOwner();
                    if ( !owner || logicalItem.IsEmpty() ||
                         !logicalItem.Contains(pt) ||
                         pt.x == logicalItem.GetLeft() ||
                         pt.x == logicalItem.GetRight() ||
                         pt.y == logicalItem.GetTop() ||
                         pt.y == logicalItem.GetBottom() )
                    {
                        return wxNOT_FOUND;
                    }

                    // The input hit stack is allowed to expose only the
                    // TabViewItem even when its Image/TextBlock children are
                    // arranged at this point. Once the exact item, measured
                    // viewport and all template buttons have been validated,
                    // use the same revision-checked subpart geometry as the
                    // logical fallback to retain the more precise wx flags.
                    const wxRect logicalIcon =
                        owner->GetCachedTabPartRect(
                            static_cast<size_t>(exactCandidate),
                            wxWinUINotebookTabPart_Icon,
                            selection,
                            direction,
                            scale);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return wxNOT_FOUND;
                    const wxRect logicalLabel =
                        owner->GetCachedTabPartRect(
                            static_cast<size_t>(exactCandidate),
                            wxWinUINotebookTabPart_Label,
                            selection,
                            direction,
                            scale);
                    owner = getCurrentOwner();
                    if ( !owner )
                        return wxNOT_FOUND;

                    if ( !logicalIcon.IsEmpty() &&
                         logicalIcon.Contains(pt) )
                    {
                        exactCandidateFlags = wxBK_HITTEST_ONICON;
                    }
                    else if ( !logicalLabel.IsEmpty() &&
                              logicalLabel.Contains(pt) )
                    {
                        exactCandidateFlags = wxBK_HITTEST_ONLABEL;
                    }
                }
                if ( flags )
                    *flags = exactCandidateFlags;
                return exactCandidate;
            }
            allowLogicalFallback = true;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TabView hit test", e);
        // A failed native hit-test cannot prove that overflow chrome is
        // absent. Stay conservative instead of piercing an unknown overlay.
        allowLogicalFallback = false;
    }

    owner = getCurrentOwner();
    if ( !owner )
        return wxNOT_FOUND;

    if ( allowLogicalFallback && pt.x >= 0 && pt.y >= 0 &&
         pt.x < clientSize.x && pt.y < clientSize.y )
    {
        int stripBottom = owner->GetPageRect().y;
        owner = getCurrentOwner();
        if ( !owner )
            return wxNOT_FOUND;
        if ( stripBottom <= 0 )
        {
            stripBottom = owner->FromDIP(DefaultTabStripHeightDIP);
            owner = getCurrentOwner();
            if ( !owner )
                return wxNOT_FOUND;
        }
        stripBottom = wxMin(stripBottom, clientSize.y);

        wxRect visibleStrip(0, 0, clientSize.x, wxMax(0, stripBottom));
        if ( hasMeasuredViewport )
            visibleStrip.Intersect(measuredViewport);

        const double totalWidth =
            impl->tabPrefixWidths.empty()
                ? 0.0
                : impl->tabPrefixWidths.back() * scale;
        if ( !hasMeasuredViewport && totalWidth > clientSize.x )
        {
            // With no live template viewport, an overflowing strip has
            // unknown navigation chrome. A guessed fixed-width reserve can
            // pierce a button after a template or DPI change.
            allowLogicalFallback = false;
        }

        for ( size_t page = 0;
              allowLogicalFallback && page < items.size();
              ++page )
        {
            const wxRect itemRect =
                owner->GetCachedTabRect(
                    page, selection, direction, scale);
            owner = getCurrentOwner();
            if ( !owner )
                return wxNOT_FOUND;
            if ( itemRect.IsEmpty() ||
                 !itemRect.Contains(pt) ||
                 !visibleStrip.Contains(pt) )
            {
                continue;
            }

            const wxRect iconRect =
                owner->GetCachedTabPartRect(
                    page,
                    wxWinUINotebookTabPart_Icon,
                    selection,
                    direction,
                    scale);
            owner = getCurrentOwner();
            if ( !owner )
                return wxNOT_FOUND;
            const wxRect labelRect =
                owner->GetCachedTabPartRect(
                    page,
                    wxWinUINotebookTabPart_Label,
                    selection,
                    direction,
                    scale);
            owner = getCurrentOwner();
            if ( !owner )
                return wxNOT_FOUND;

            if ( pt.x == itemRect.GetLeft() ||
                 pt.x == itemRect.GetRight() ||
                 pt.y == itemRect.GetTop() ||
                 pt.y == itemRect.GetBottom() )
            {
                return wxNOT_FOUND;
            }

            if ( flags )
            {
                *flags =
                    !iconRect.IsEmpty() && iconRect.Contains(pt)
                        ? wxBK_HITTEST_ONICON
                        : !labelRect.IsEmpty() &&
                                  labelRect.Contains(pt)
                            ? wxBK_HITTEST_ONLABEL
                            : wxBK_HITTEST_ONITEM;
            }
            return static_cast<int>(page);
        }
    }

    if ( flags && owner->GetPageRect().Contains(pt) )
        *flags |= wxBK_HITTEST_ONPAGE;

    return wxNOT_FOUND;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    if ( UsesExtendedPeerSurface() )
    {
        const wxDirection orientation = GetTabOrientation();
        const int fallbackExtent = FromDIP(DefaultTabStripHeightDIP);
        const int margin = FromDIP(8);
        const auto state = m_winui ? m_winui->callbackState : nullptr;
        const std::uint64_t callbackGeneration =
            state ? state->Generation() : 0;
        wxWinUINotebookImpl * const impl = m_winui.get();
        wxNotebook * const self = const_cast<wxNotebook *>(this);
        const bool laidOut = self->LayoutExtendedPeerSurface();
        wxNotebook * const owner =
            state ? state->GetOwner(callbackGeneration) : nullptr;
        const int extent =
            laidOut && owner == this && owner->m_winui &&
                    owner->m_winui.get() == impl &&
                    impl->extendedLayoutValid
                ? impl->extendedBandExtentPx
                : fallbackExtent;
        return orientation == wxTOP || orientation == wxBOTTOM
                   ? wxSize(sizePage.x + margin,
                            sizePage.y + extent + margin)
                   : wxSize(sizePage.x + extent + margin,
                            sizePage.y + margin);
    }

    // wxNotebook tab sizes are expressed in client pixels, while the native
    // strip measurement is cached in XAML DIPs. Keep these units distinct:
    // scale the native/fallback band exactly once and never scale m_tabSize.
    int tabHeight =
        m_tabStripHeightDIP > 0
            ? FromDIP(m_tabStripHeightDIP)
            : 0;
    if ( m_hasTabSize )
        tabHeight = wxMax(tabHeight, m_tabSize.y);
    if ( tabHeight <= 0 )
        tabHeight = FromDIP(DefaultTabStripHeightDIP);

    const int margin = FromDIP(8);
    return wxSize(sizePage.x + margin,
                  sizePage.y + tabHeight + margin);
}

wxRect wxNotebook::GetPageRect() const
{
    if ( UsesExtendedPeerSurface() )
    {
        const wxSize client = GetClientSize();
        const wxDirection orientation = GetTabOrientation();
        const int fallbackExtent = FromDIP(DefaultTabStripHeightDIP);
        const auto state = m_winui ? m_winui->callbackState : nullptr;
        const std::uint64_t callbackGeneration =
            state ? state->Generation() : 0;
        wxWinUINotebookImpl * const impl = m_winui.get();
        wxNotebook * const self = const_cast<wxNotebook *>(this);
        const bool laidOut = self->LayoutExtendedPeerSurface();
        wxNotebook * const owner =
            state ? state->GetOwner(callbackGeneration) : nullptr;
        const int extent =
            laidOut && owner == this && owner->m_winui &&
                    owner->m_winui.get() == impl &&
                    impl->extendedLayoutValid
                ? impl->extendedBandExtentPx
                : fallbackExtent;

        wxRect rect(wxPoint(), client);
        switch ( orientation )
        {
            case wxBOTTOM:
                rect.height = wxMax(0, rect.height - extent);
                break;
            case wxLEFT:
                rect.x += extent;
                rect.width = wxMax(0, rect.width - extent);
                break;
            case wxRIGHT:
                rect.width = wxMax(0, rect.width - extent);
                break;
            case wxTOP:
            default:
                rect.y += extent;
                rect.height = wxMax(0, rect.height - extent);
                break;
        }
        return rect;
    }

    // Derive the page area deterministically from the current client size and
    // the (cached) tab-strip height.  Computing it from the live size on every
    // call -- rather than resizing pages from the asynchronous layout callback
    // -- avoids repeated SetSize() calls that would otherwise reset the scroll
    // position of a scrolled page on each TabView layout pass.
    const wxSize client = GetClientSize();
    const int tabH = FromDIP(
        m_tabStripHeightDIP > 0
            ? m_tabStripHeightDIP
            : DefaultTabStripHeightDIP);

    wxRect rect(0, tabH, client.x, client.y - tabH);
    if ( rect.height < 0 )
        rect.height = 0;
    return rect;
}

void wxNotebook::DoSize()
{
    if ( UsesExtendedPeerSurface() && m_winui &&
         m_winui->callbackState )
    {
        const auto state = m_winui->callbackState;
        if ( state->IsGeometryRealizationInProgress() )
        {
            // Slot synchronization and explicit realization both cross XAML
            // measure/arrange. A synchronous HWND size notification from that
            // pass must not invalidate and restart the same extended layout.
            return;
        }
        const std::uint64_t generation = state->Generation();
        m_winui->InvalidateGeometry();
        if ( !LayoutExtendedPeerSurface() ||
             state->GetOwner(generation) != this )
        {
            return;
        }

        // Extended layout applies the new band and all HWND page rectangles
        // as one generation. Repeating the loop below would deliver duplicate
        // page size events and make reentrant last-writer ordering ambiguous.
        return;
    }

    if ( m_winui &&
         (!m_winui->callbackState ||
          !m_winui->callbackState->IsGeometryRealizationInProgress()) )
    {
        m_winui->InvalidateGeometry();
    }

    const wxRect rect = GetPageRect();
    std::vector<wxWeakRef<wxWindow>> pages;
    pages.reserve(m_pages.size());
    for ( wxWindow * const page : m_pages )
        pages.emplace_back(page);

    const auto callbackState =
        m_winui ? m_winui->callbackState : nullptr;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->Generation() : 0;
    const wxWeakRef<wxNotebook> weakOwner(this);
    for ( const wxWeakRef<wxWindow>& weakPage : pages )
    {
        wxNotebook * const owner =
            callbackState
                ? callbackState->GetOwner(callbackGeneration)
                : weakOwner.get();
        wxWindow * const page = weakPage.get();
        if ( !owner || !page )
            return;
        if ( std::find(owner->m_pages.begin(),
                       owner->m_pages.end(),
                       page) == owner->m_pages.end() )
            continue;

        // No owner/model access after SetSize(): the next iteration resolves
        // both weak endpoints again.
        page->SetSize(rect);
    }
}

void wxNotebook::UpdateTabStripHeightFromLayout()
{
    if ( UsesExtendedPeerSurface() )
    {
        (void)LayoutExtendedPeerSurface();
        return;
    }

    if ( !m_winui || !m_winui->tabView || m_selection == wxNOT_FOUND )
        return;

    if ( m_selection < 0 ||
         static_cast<size_t>(m_selection) >= m_winui->items.size() )
        return;

    try
    {
        // The stock TabView has a single horizontal row. The lower edge of
        // its selected, realized item is therefore the start of the native
        // page-content band, without installing a XAML UIElement as the tab's
        // Content and asking TabView to reparent it during selection changes.
        const MUXC::TabViewItem item = m_winui->items[m_selection];
        const double itemHeight = item.ActualHeight();
        if ( itemHeight <= 0 )
            return;

        const auto transform = item.TransformToVisual(m_winui->tabView);
        const auto origin =
            transform.TransformPoint(winrt::Windows::Foundation::Point{ 0, 0 });

        const int tabStrip =
            static_cast<int>(std::lround(origin.Y + itemHeight));
        if ( tabStrip <= 0 || tabStrip == m_tabStripHeightDIP )
            return;

        m_tabStripHeightDIP = tabStrip;
        DoInvalidateBestSize();

        // Shrink the TabView island to the tab strip only, so the page area
        // below it (and the pages' native scrollbars) is not covered by the
        // island's composition surface.
        m_winui->host.SetBridgeHeightLimit(FromDIP(tabStrip));

        DoSize();
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

#endif // wxUSE_NOTEBOOK
