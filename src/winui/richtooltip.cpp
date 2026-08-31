/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/richtooltip.cpp
// Purpose:     wxRichToolTip implementation for wxWinUI
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_RICHTOOLTIP && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
#endif

#include "wx/private/richtooltip.h"
#include "wx/generic/private/richtooltip.h"
#include "wx/log.h"
#include "wx/msw/private.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#include "private.h"

#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/richtooltip.h"
#include "wx/winui/private/tlwhost.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

namespace
{

wxWinUITeachingTipFaultForTesting gs_teachingTipFaultForTesting =
    wxWinUITeachingTipFaultForTesting::None;
wxWinUITeachingTipHookPointForTesting gs_teachingTipHookPointForTesting =
    wxWinUITeachingTipHookPointForTesting::None;
wxWinUITeachingTipHookForTesting gs_teachingTipHookForTesting = nullptr;
wxWinUITeachingTipDiagnosticsForTesting gs_teachingTipDiagnostics;
bool gs_nativeTeachingTipBackendForTesting = false;

class wxWinUIRichTipSession
{
public:
    virtual ~wxWinUIRichTipSession() = default;

    virtual wxWindow *GetSessionTarget() const = 0;
    virtual void CloseForQueueShutdown() = 0;
    virtual void NotifyReparent(wxWindow *subtreeRoot) = 0;
};

std::vector<std::weak_ptr<wxWinUIRichTipSession>> gs_richTipSessions;

void wxWinUIPruneRichTipSessions()
{
    gs_richTipSessions.erase(
        std::remove_if(
            gs_richTipSessions.begin(),
            gs_richTipSessions.end(),
            [](const std::weak_ptr<wxWinUIRichTipSession>& state)
            {
                return state.expired();
            }),
        gs_richTipSessions.end());
}

void wxWinUITrackRichTipSession(
    const std::shared_ptr<wxWinUIRichTipSession>& state)
{
    wxWinUIPruneRichTipSessions();
    gs_richTipSessions.emplace_back(state);
}

std::vector<std::shared_ptr<wxWinUIRichTipSession>>
wxWinUISnapshotRichTipSessions()
{
    wxWinUIPruneRichTipSessions();

    std::vector<std::shared_ptr<wxWinUIRichTipSession>> sessions;
    sessions.reserve(gs_richTipSessions.size());
    for ( const auto& weakState : gs_richTipSessions )
    {
        if ( const auto state = weakState.lock() )
            sessions.push_back(state);
    }
    return sessions;
}

wxWinUITeachingTipFaultForTesting wxWinUITakeTeachingTipFaultForTesting()
{
    return std::exchange(
        gs_teachingTipFaultForTesting,
        wxWinUITeachingTipFaultForTesting::None);
}

wxWinUITeachingTipHookForTesting
wxWinUITakeTeachingTipHookForTesting(
    wxWinUITeachingTipHookPointForTesting point)
{
    if ( gs_teachingTipHookPointForTesting != point )
        return nullptr;

    gs_teachingTipHookPointForTesting =
        wxWinUITeachingTipHookPointForTesting::None;
    return std::exchange(gs_teachingTipHookForTesting, nullptr);
}

bool wxWinUIIsTeachingTipUnavailable(const winrt::hresult_error& e)
{
    const HRESULT hr = static_cast<HRESULT>(e.code());
    return hr == REGDB_E_CLASSNOTREG ||
           hr == CLASS_E_CLASSNOTAVAILABLE ||
           hr == E_NOINTERFACE ||
           hr == HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}

MUXC::TeachingTipPlacementMode
wxWinUIToTeachingTipPlacement(wxWinUITeachingTipPlacement placement)
{
    switch ( placement )
    {
        case wxWinUITeachingTipPlacement::Top:
            return MUXC::TeachingTipPlacementMode::Top;
        case wxWinUITeachingTipPlacement::Bottom:
            return MUXC::TeachingTipPlacementMode::Bottom;
        case wxWinUITeachingTipPlacement::TopRight:
            return MUXC::TeachingTipPlacementMode::TopRight;
        case wxWinUITeachingTipPlacement::TopLeft:
            return MUXC::TeachingTipPlacementMode::TopLeft;
        case wxWinUITeachingTipPlacement::BottomRight:
            return MUXC::TeachingTipPlacementMode::BottomRight;
        case wxWinUITeachingTipPlacement::BottomLeft:
            return MUXC::TeachingTipPlacementMode::BottomLeft;
        case wxWinUITeachingTipPlacement::Auto:
            return MUXC::TeachingTipPlacementMode::Auto;
    }

    return MUXC::TeachingTipPlacementMode::Auto;
}

// The wxRichToolTip facade is normally destroyed immediately after ShowFor().
// This state therefore owns itself only while its TeachingTip is live. XAML
// delegates and the TLW registry hold weak references, so neither the island
// tree nor its central cancellation table can form a retention cycle.
class wxWinUITeachingTipState final
    : public wxWinUIRichTipSession,
      public std::enable_shared_from_this<wxWinUITeachingTipState>
{
public:
    enum class TimerPhase
    {
        None,
        Delay,
        Visible
    };

    wxWinUITeachingTipState()
    {
        ++gs_teachingTipDiagnostics.liveStates;
    }

    ~wxWinUITeachingTipState()
    {
        // Normally Close() has already cleared everything. This no-throw
        // backstop is essential for failures/reentrance between Append() and
        // Start(): the state already owns every appended visual at that point.
        Cleanup(false);
        --gs_teachingTipDiagnostics.liveStates;
    }

    bool PrepareTarget(wxWindow *target)
    {
        if ( !target || target->IsBeingDeleted() || !GetHwndOf(target) )
            return false;

        targetWindow = target;
        targetIdentity = target;
        targetTLW = wxGetTopLevelParent(target);
        return GetLiveTarget() != nullptr;
    }

    void SetFaultForTesting(wxWinUITeachingTipFaultForTesting fault)
    {
        faultForTesting = fault;
    }

    wxWindow *GetLiveTarget() const
    {
        wxWindow * const target = targetWindow.get();
        wxWindow * const tlw = targetTLW.get();
        if ( !target || target != targetIdentity ||
                target->IsBeingDeleted() || !GetHwndOf(target) ||
                !tlw || wxGetTopLevelParent(target) != tlw )
        {
            return nullptr;
        }

        return target;
    }

    bool IsTargetDestructionPending() const
    {
        wxWindow * const target = targetWindow.get();
        return target && target == targetIdentity &&
               target->IsBeingDeleted();
    }

    wxWindow *GetSessionTarget() const override
    {
        wxWindow * const target = targetWindow.get();
        return target && target == targetIdentity &&
                       !target->IsBeingDeleted() && GetHwndOf(target)
            ? target
            : nullptr;
    }

    void CloseForQueueShutdown() override
    {
        if ( !active || closing )
            return;

        ++gs_teachingTipDiagnostics.queueShutdownClosures;
        Close(true);
    }

    void NotifyReparent(wxWindow *subtreeRoot) override
    {
        if ( !active || closing || !subtreeRoot )
            return;

        wxWindow * const target = GetSessionTarget();
        if ( !target ||
                (target != subtreeRoot &&
                 !subtreeRoot->IsDescendant(target)) )
        {
            return;
        }

        wxWindow * const originalTLW = targetTLW.get();
        if ( !originalTLW || wxGetTopLevelParent(target) != originalTLW )
        {
            ++gs_teachingTipDiagnostics.reparentClosures;
            Close(true);
            return;
        }

        // A same-TLW reparent preserves the session, but an explicit anchor
        // must follow the target synchronously.
        if ( !RefreshTarget() )
        {
            ++gs_teachingTipDiagnostics.reparentClosures;
            Close(true);
        }
    }

    bool AppendAnchor()
    {
        if ( !root || !anchor )
            return false;

        root.Children().Append(anchor);
        anchorAppended = true;
        ++gs_teachingTipDiagnostics.liveVisuals;
        return true;
    }

    bool AppendTip()
    {
        if ( !root || !tip )
            return false;

        root.Children().Append(tip);
        tipAppended = true;
        ++gs_teachingTipDiagnostics.liveVisuals;
        return true;
    }

    bool RefreshTarget()
    {
        const auto expectedRoot = root;
        const auto expectedAnchor = anchor;
        const auto expectedTip = tip;
        const void * const expectedRootIdentity =
            winrt::get_abi(expectedRoot);
        const void * const expectedAnchorIdentity =
            winrt::get_abi(expectedAnchor);
        const void * const expectedTipIdentity =
            winrt::get_abi(expectedTip);
        if ( !expectedRoot )
            return false;

        wxWindow *target = nullptr;
        wxWinUITopLevelHost *host = nullptr;
        const auto reacquireTarget =
            [this,
             &target,
             &host,
             expectedRoot,
             expectedRootIdentity,
             expectedAnchorIdentity,
             expectedTipIdentity]()
            {
                if ( closing ||
                     winrt::get_abi(root) != expectedRootIdentity ||
                     winrt::get_abi(anchor) != expectedAnchorIdentity ||
                     winrt::get_abi(tip) != expectedTipIdentity )
                {
                    return false;
                }

                wxWindow * const candidateTarget = GetLiveTarget();
                if ( !candidateTarget )
                    return false;

                wxWinUITopLevelHost * const candidateHost =
                    wxWinUITopLevelHost::ForWindow(
                        candidateTarget, false);
                if ( !candidateHost )
                    return false;

                const auto candidateRoot = candidateHost->Root();
                const auto xamlRoot = candidateHost->GetXamlRoot();

                // GetXamlRoot() is a WinRT/re-entrancy boundary. Do not
                // dereference candidateHost or candidateTarget again until
                // both weak/topology identities have been re-resolved.
                wxWindow * const currentTarget = GetLiveTarget();
                wxWinUITopLevelHost * const currentHost =
                    currentTarget
                        ? wxWinUITopLevelHost::ForWindow(
                              currentTarget, false)
                        : nullptr;
                if ( !currentTarget ||
                     currentTarget != candidateTarget ||
                     !currentHost ||
                     currentHost != candidateHost ||
                     !xamlRoot ||
                     candidateRoot != expectedRoot ||
                     currentHost->Root() != expectedRoot ||
                     closing ||
                     winrt::get_abi(root) != expectedRootIdentity ||
                     winrt::get_abi(anchor) != expectedAnchorIdentity ||
                     winrt::get_abi(tip) != expectedTipIdentity )
                {
                    return false;
                }

                target = currentTarget;
                host = currentHost;
                return true;
            };

        if ( !reacquireTarget() )
            return false;
        const bool rightToLeft =
            target->GetLayoutDirection() == wxLayout_RightToLeft;
        if ( !reacquireTarget() )
            return false;

        const MUX::FlowDirection flowDirection =
            rightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight;

        if ( expectedTip )
        {
            expectedTip.FlowDirection(flowDirection);
            if ( !reacquireTarget() )
                return false;
        }

        if ( const auto anchorElement =
                 expectedAnchor.try_as<MUX::FrameworkElement>() )
        {
            if ( !reacquireTarget() )
                return false;
            anchorElement.FlowDirection(flowDirection);
            if ( !reacquireTarget() )
                return false;
        }

        if ( const auto hook = wxWinUITakeTeachingTipHookForTesting(
                 wxWinUITeachingTipHookPointForTesting::
                     AfterFlowDirection) )
        {
            if ( wxWindow * const currentTarget = GetLiveTarget() )
                hook(currentTarget);
        }
        if ( !reacquireTarget() )
            return false;

        if ( expectedAnchor )
        {
            winrt::Windows::Foundation::Point clientPoint{};
            if ( hasTargetRect && !targetRect.IsEmpty() )
            {
                clientPoint.X =
                    static_cast<float>(
                        targetRect.x + targetRect.width / 2.0);
                clientPoint.Y =
                    static_cast<float>(
                        targetRect.y + targetRect.height / 2.0);
            }
            else
            {
                const wxSize clientSize = target->GetClientSize();
                if ( !reacquireTarget() )
                    return false;
                clientPoint.X =
                    static_cast<float>(clientSize.x / 2.0);
                clientPoint.Y =
                    static_cast<float>(clientSize.y / 2.0);
            }

            // Size queries and coordinate projection are application/WinRT
            // boundaries. Re-resolve before entering the common mapper.
            if ( !reacquireTarget() )
                return false;

            winrt::Windows::Foundation::Point dip{};
            if ( wxWinUIVisualCoordinates::ClientPointToRoot(
                     target, clientPoint, &dip) !=
                 wxWinUICoordinateResult::Mapped )
            {
                return false;
            }
            if ( !reacquireTarget() )
                return false;

            MUXC::Canvas::SetLeft(expectedAnchor, dip.X);
            if ( !reacquireTarget() )
                return false;
            MUXC::Canvas::SetTop(expectedAnchor, dip.Y);
            if ( !reacquireTarget() )
                return false;

            gs_teachingTipDiagnostics.lastAnchorInRoot =
                wxPoint(static_cast<int>(std::lround(dip.X)),
                        static_cast<int>(std::lround(dip.Y)));
        }

        gs_teachingTipDiagnostics.lastRightToLeft = rightToLeft;
        return true;
    }

    winrt::Microsoft::UI::Xaml::Controls::Canvas root{ nullptr };
    MUXC::TeachingTip tip{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement anchor{ nullptr };
    MUXD::DispatcherQueue queue{ nullptr };
    MUXD::DispatcherQueueTimer timer{ nullptr };
    wxRect targetRect;
    bool hasTargetRect = false;

    wxWinUITeachingTipShowResult Start(unsigned timeout, unsigned delay)
    {
        // Start is a transaction: once any handler, registry token or
        // self-reference has been published, every non-Shown exit must revoke
        // all of them and close a peer which may already have opened.
        wxScopeGuard rollback = wxMakeGuard(
            [this]()
            {
                Close(true);
            });

        try
        {
            gs_teachingTipDiagnostics.lastRequestedTimeoutMs = timeout;
            gs_teachingTipDiagnostics.lastRequestedDelayMs = delay;
            wxWindow *target = GetLiveTarget();
            if ( !target )
            {
                Close(false);
                return wxWinUITeachingTipShowResult::Cancelled;
            }

            const std::weak_ptr<wxWinUITeachingTipState> weakState =
                shared_from_this();

            active = true;
            self = shared_from_this();
            timeoutMs = timeout;

            closedToken = tip.Closed(
                [weakState](
                    const MUXC::TeachingTip&,
                    const MUXC::TeachingTipClosedEventArgs&)
                {
                    if ( const auto state = weakState.lock() )
                        state->Close(false);
                });
            hasClosedToken = true;
            ++gs_teachingTipDiagnostics.liveXamlHandlers;

            queue = root.DispatcherQueue();
            if ( faultForTesting ==
                    wxWinUITeachingTipFaultForTesting::QueueUnavailable )
            {
                queue = nullptr;
            }
            if ( !queue )
                return wxWinUITeachingTipShowResult::OperationalFailure;

            shutdownToken = queue.ShutdownStarting(
                [weakState](
                    const MUXD::DispatcherQueue&,
                    const MUXD::
                        DispatcherQueueShutdownStartingEventArgs&)
                {
                    if ( const auto state = weakState.lock() )
                        state->CloseForQueueShutdown();
                });
            hasShutdownToken = true;
            ++gs_teachingTipDiagnostics.liveXamlHandlers;

            target = GetLiveTarget();
            if ( !target )
            {
                Close(false);
                return wxWinUITeachingTipShowResult::Cancelled;
            }

            registration = wxWinUIRegisterTransient(
                target,
                wxWinUITransientKind::TeachingTip,
                [weakState]()
                {
                    if ( const auto state = weakState.lock() )
                        state->Close(true);
                });
            if ( !registration )
            {
                Close(false);
                return wxWinUITeachingTipShowResult::OperationalFailure;
            }

            wxWindow * const liveTarget = GetLiveTarget();
            if ( !liveTarget )
            {
                Close(false);
                return wxWinUITeachingTipShowResult::Cancelled;
            }

            // Register after the central TLW observer. wx dispatches newest
            // dynamic handlers first, so when target==TLW this exact-target
            // callback closes the state before the manager's weak callback;
            // no pending handler can retain a raw pointer to freed state.
            liveTarget->Bind(
                wxEVT_DESTROY,
                &wxWinUITeachingTipState::OnTargetDestroy,
                this);
            targetDestroyBound = true;
            ++gs_teachingTipDiagnostics.liveTargetBindings;

            wxWinUITrackRichTipSession(shared_from_this());

            switch ( faultForTesting )
            {
                case wxWinUITeachingTipFaultForTesting::StartHResult:
                    throw winrt::hresult_error(
                        E_FAIL,
                        L"Injected HRESULT during TeachingTip startup");

                case wxWinUITeachingTipFaultForTesting::StartStdException:
                    throw std::runtime_error(
                        "Injected std::exception during TeachingTip startup");

                case wxWinUITeachingTipFaultForTesting::StartUnknownException:
                    throw 17;

                default:
                    break;
            }

            if ( delay )
            {
                ++gs_teachingTipDiagnostics.delayedStarts;
                if ( !CreateTimer(delay, TimerPhase::Delay) )
                {
                    if ( !active || closing )
                        return wxWinUITeachingTipShowResult::Cancelled;
                    Close(false);
                    return wxWinUITeachingTipShowResult::OperationalFailure;
                }

                rollback.Dismiss();
                return wxWinUITeachingTipShowResult::Shown;
            }

            const wxWinUITeachingTipShowResult result = Open();
            if ( result == wxWinUITeachingTipShowResult::Shown )
                rollback.Dismiss();
            return result;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TeachingTip startup", e);
        }
        catch ( const std::exception& e )
        {
            // wxWinUIRegisterTransient() and wx event-table allocation are
            // ordinary C++ boundaries, not WinRT boundaries. In particular,
            // never let an exception after self was published retain the
            // state, XAML delegates and target binding forever.
            wxLogWarning(
                "WinUI TeachingTip startup failed: %s",
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
            wxLogWarning(
                "WinUI TeachingTip startup failed with an unknown "
                "exception.");
        }

        return wxWinUITeachingTipShowResult::OperationalFailure;
    }

    void Close(bool closePeer)
    {
        if ( closing )
            return;

        // Keep the object alive while dropping its deliberate self-reference.
        const auto keepAlive = shared_from_this();
        closing = true;
        active = false;

        try
        {
            registration.Reset();
        }
        catch ( ... )
        {
            // Cleanup below must remain best-effort and complete even if the
            // central registry is already tearing down.
        }

        bool targetDetached = false;
        try
        {
            targetDetached = UnbindTarget();
        }
        catch ( ... )
        {
        }
        if ( targetDetached )
        {
            targetWindow = nullptr;
            targetTLW = nullptr;
        }

        Cleanup(closePeer);
        if ( targetDetached )
            self.reset();
    }

private:
    void Cleanup(bool closePeer)
    {
        // Invalidate the logical ownership flags before touching projected
        // objects. Any nested callback therefore sees a fully closing state,
        // and every operation below is independent from all subsequent ones.
        try
        {
            if ( timer )
                timer.Stop();
        }
        catch ( ... )
        {
        }

        const bool revokeTick = hasTickToken;
        hasTickToken = false;
        if ( revokeTick )
        {
            --gs_teachingTipDiagnostics.liveXamlHandlers;
            try
            {
                if ( timer )
                    timer.Tick(tickToken);
            }
            catch ( ... )
            {
            }
        }
        timer = nullptr;
        timerPhase = TimerPhase::None;

        const bool revokeShutdown = hasShutdownToken;
        hasShutdownToken = false;
        if ( revokeShutdown )
        {
            --gs_teachingTipDiagnostics.liveXamlHandlers;
            try
            {
                if ( queue )
                    queue.ShutdownStarting(shutdownToken);
            }
            catch ( ... )
            {
            }
        }
        queue = nullptr;

        const bool revokeClosed = hasClosedToken;
        hasClosedToken = false;
        if ( revokeClosed )
        {
            --gs_teachingTipDiagnostics.liveXamlHandlers;
            try
            {
                if ( tip )
                    tip.Closed(closedToken);
            }
            catch ( ... )
            {
            }
        }

        if ( closePeer && tip )
        {
            ++gs_teachingTipDiagnostics.peerCloseRequests;
            try
            {
                // Never read IsOpen() during teardown: the getter is another
                // ABI boundary and a false result is not needed for the
                // idempotent setter.
                tip.IsOpen(false);
            }
            catch ( ... )
            {
            }
        }

        const bool removeTip = tipAppended;
        tipAppended = false;
        if ( removeTip )
        {
            ++gs_teachingTipDiagnostics.cleanupTipRemovalAttempts;
            --gs_teachingTipDiagnostics.liveVisuals;
            try
            {
                if ( root && tip )
                {
                    uint32_t index = 0;
                    if ( root.Children().IndexOf(tip, index) )
                        root.Children().RemoveAt(index);
                }
            }
            catch ( ... )
            {
            }
        }

        const bool removeAnchor = anchorAppended;
        anchorAppended = false;
        if ( removeAnchor )
        {
            ++gs_teachingTipDiagnostics.cleanupAnchorRemovalAttempts;
            --gs_teachingTipDiagnostics.liveVisuals;
            try
            {
                if ( root && anchor )
                {
                    uint32_t index = 0;
                    if ( root.Children().IndexOf(anchor, index) )
                        root.Children().RemoveAt(index);
                }
            }
            catch ( ... )
            {
            }
        }

        tip = nullptr;
        anchor = nullptr;
        root = nullptr;
    }

    bool UnbindTarget()
    {
        if ( !targetDestroyBound )
            return true;

        bool detached = inTargetDestroyDispatch;
        if ( !detached )
        {
            if ( wxWindow * const target = targetWindow.get() )
            {
                detached = target->Unbind(
                    wxEVT_DESTROY,
                    &wxWinUITeachingTipState::OnTargetDestroy,
                    this);
            }
            else
            {
                // wxWeakRef was cleared by target destruction, which also
                // destroyed its dynamic event table.
                detached = true;
            }
        }

        if ( detached )
        {
            --gs_teachingTipDiagnostics.liveTargetBindings;
            targetDestroyBound = false;
        }
        else
        {
            // Keep the deliberate self-reference and the target weak
            // reference alive. This rare failure leaks only until target
            // destruction, whose still-valid handler releases the state; it
            // must never degrade into a dangling wx event sink.
            wxLogWarning("Failed to revoke WinUI TeachingTip target binding.");
        }

        return detached;
    }

    bool CreateTimer(unsigned interval, TimerPhase phase)
    {
        if ( !queue )
            return false;

        MUXD::DispatcherQueueTimer candidate = queue.CreateTimer();
        candidate.Interval(std::chrono::milliseconds(interval));
        candidate.IsRepeating(false);

        const std::weak_ptr<wxWinUITeachingTipState> weakState =
            shared_from_this();
        const winrt::event_token candidateToken = candidate.Tick(
            [weakState](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Windows::Foundation::IInspectable&)
            {
                if ( const auto state = weakState.lock() )
                    state->OnTimer();
            });

        // Timer construction is a WinRT boundary too. If native dispatch
        // destroyed the target before the candidate was published, revoke
        // the unpublished callback directly and leave no state behind.
        if ( !active || closing )
        {
            candidate.Tick(candidateToken);
            return false;
        }

        timer = candidate;
        tickToken = candidateToken;
        timerPhase = phase;
        hasTickToken = true;
        ++gs_teachingTipDiagnostics.liveXamlHandlers;
        timer.Start();
        return active && !closing && timer;
    }

    wxWinUITeachingTipShowResult Open()
    {
        if ( !active || closing )
            return wxWinUITeachingTipShowResult::Cancelled;

        if ( !RefreshTarget() )
        {
            Close(true);
            return wxWinUITeachingTipShowResult::Cancelled;
        }

        try
        {
            // Keep a projected reference locally: setting IsOpen may dispatch
            // Loaded/layout callbacks which destroy the wx target and cause
            // Close() to clear all state members before this call returns.
            const MUXC::TeachingTip peer = tip;
            peer.IsOpen(true);
            ++gs_teachingTipDiagnostics.peerOpens;
            gs_teachingTipDiagnostics.lastPeerOpenSequence =
                std::max(
                    gs_teachingTipDiagnostics.lastPeerOpenSequence,
                    gs_teachingTipDiagnostics.lastTimeoutCloseSequence) + 1;

            if ( faultForTesting ==
                    wxWinUITeachingTipFaultForTesting::
                        OpenAfterPeerHResult )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"Injected HRESULT after TeachingTip opened");
            }

            if ( const auto hook = wxWinUITakeTeachingTipHookForTesting(
                    wxWinUITeachingTipHookPointForTesting::AfterPeerOpen) )
            {
                if ( wxWindow * const target = GetLiveTarget() )
                    hook(target);
            }

            // IsOpen(true) is the presentation boundary. Any re-entrant
            // close after it succeeded (including wxWindow::Destroy(),
            // whose native destruction is deferred for TLWs) is a normal
            // lifecycle close, not a failed presentation.
            if ( !active || closing )
                return wxWinUITeachingTipShowResult::Shown;

            if ( !RefreshTarget() )
            {
                // RefreshTarget() itself crosses application/WinRT
                // boundaries and may synchronously run the target-destroy
                // binding. Preserve the same post-open lifecycle result.
                if ( !active || closing || IsTargetDestructionPending() )
                    return wxWinUITeachingTipShowResult::Shown;

                if ( active && !closing )
                    Close(true);
                return wxWinUITeachingTipShowResult::Cancelled;
            }

            if ( timeoutMs )
            {
                if ( timer )
                {
                    timer.Stop();
                    timer.Interval(std::chrono::milliseconds(timeoutMs));
                    timerPhase = TimerPhase::Visible;
                    timer.Start();
                }
                else if ( !CreateTimer(timeoutMs, TimerPhase::Visible) )
                {
                    if ( !active || closing )
                        return wxWinUITeachingTipShowResult::Cancelled;
                    // A timeout promise must not silently turn into a
                    // permanent TeachingTip.
                    Close(true);
                    return wxWinUITeachingTipShowResult::OperationalFailure;
                }
            }

            return wxWinUITeachingTipShowResult::Shown;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TeachingTip open", e);
            Close(true);
        }
        catch ( const std::exception& e )
        {
            Close(true);
            wxLogWarning(
                "WinUI TeachingTip open failed: %s",
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
            Close(true);
            wxLogWarning(
                "WinUI TeachingTip open failed with an unknown exception.");
        }

        return wxWinUITeachingTipShowResult::OperationalFailure;
    }

    void OnTimer()
    {
        if ( !active || closing )
            return;

        if ( timerPhase == TimerPhase::Delay )
        {
            const wxWinUITeachingTipShowResult result = Open();
            if ( result == wxWinUITeachingTipShowResult::OperationalFailure )
            {
                ++gs_teachingTipDiagnostics.operationalFailures;
                wxLogWarning(
                    "Delayed WinUI TeachingTip presentation failed.");
            }
            else if ( result == wxWinUITeachingTipShowResult::Cancelled )
            {
                ++gs_teachingTipDiagnostics.cancellations;
            }
        }
        else if ( timerPhase == TimerPhase::Visible )
        {
            ++gs_teachingTipDiagnostics.timeoutClosures;
            gs_teachingTipDiagnostics.lastTimeoutCloseSequence =
                std::max(
                    gs_teachingTipDiagnostics.lastPeerOpenSequence,
                    gs_teachingTipDiagnostics.lastTimeoutCloseSequence) + 1;
            Close(true);
        }
    }

    void OnTargetDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();
        if ( event.GetWindow() != targetIdentity )
            return;

        // Close() drops the deliberate self-reference. Keep this member
        // function's object alive until the destroy callback has unwound.
        const auto keepAlive = shared_from_this();
        inTargetDestroyDispatch = true;
        if ( closing )
        {
            // Recovery path for a failed Unbind(): window destruction itself
            // has now revoked the dynamic handler, so the safety
            // self-reference can finally be released.
            if ( targetDestroyBound )
            {
                targetDestroyBound = false;
                --gs_teachingTipDiagnostics.liveTargetBindings;
            }
            targetWindow = nullptr;
            targetTLW = nullptr;
            self.reset();
        }
        else
        {
            Close(true);
        }
        inTargetDestroyDispatch = false;
    }

    wxWinUITransientRegistration registration;
    std::shared_ptr<wxWinUITeachingTipState> self;
    wxWeakRef<wxWindow> targetWindow;
    wxWeakRef<wxWindow> targetTLW;
    wxWindow *targetIdentity = nullptr;
    winrt::event_token closedToken{};
    winrt::event_token shutdownToken{};
    winrt::event_token tickToken{};
    unsigned timeoutMs = 0;
    TimerPhase timerPhase = TimerPhase::None;
    bool hasClosedToken = false;
    bool hasShutdownToken = false;
    bool hasTickToken = false;
    bool targetDestroyBound = false;
    bool inTargetDestroyDispatch = false;
    bool active = false;
    bool closing = false;
    bool tipAppended = false;
    bool anchorAppended = false;
    wxWinUITeachingTipFaultForTesting faultForTesting =
        wxWinUITeachingTipFaultForTesting::None;
};

// The generic wx popup is the supported beta backend and also represents API
// customizations unsupported by TeachingTip. Keep it under the same central
// transient manager so TLW shutdown, reparent and explicit cancellation never
// leave an independently-owned popup behind.
class wxWinUIGenericTipState final
    : public wxWinUIRichTipSession,
      public std::enable_shared_from_this<wxWinUIGenericTipState>
{
public:
    ~wxWinUIGenericTipState()
    {
        if ( genericCounted )
            --gs_teachingTipDiagnostics.liveGenericFallbacks;
    }

    bool Start(wxWindow *target, wxWindow *popup)
    {
        wxScopeGuard rollback = wxMakeGuard(
            [this]()
            {
                Close(true);
            });

        try
        {
            if ( !target || target->IsBeingDeleted() || !GetHwndOf(target) ||
                    !popup || popup->IsBeingDeleted() || !GetHwndOf(popup) )
            {
                return false;
            }

            targetWindow = target;
            targetIdentity = target;
            targetTLW = wxGetTopLevelParent(target);
            popupWindow = popup;
            popupIdentity = popup;
            if ( !GetSessionTarget() || !targetTLW.get() )
                return false;

            active = true;
            self = shared_from_this();
            genericCounted = true;
            ++gs_teachingTipDiagnostics.liveGenericFallbacks;

            const std::weak_ptr<wxWinUIGenericTipState> weakState =
                shared_from_this();
            registration = wxWinUIRegisterTransient(
                target,
                wxWinUITransientKind::TeachingTip,
                [weakState]()
                {
                    if ( const auto state = weakState.lock() )
                        state->CancelFromManager();
                });
            if ( !registration )
                return false;

            if ( !GetSessionTarget() || !GetLivePopup() )
                return false;

            target->Bind(
                wxEVT_DESTROY,
                &wxWinUIGenericTipState::OnTargetDestroy,
                this);
            targetDestroyBound = true;

            popup->Bind(
                wxEVT_DESTROY,
                &wxWinUIGenericTipState::OnPopupDestroy,
                this);
            popupDestroyBound = true;

            if ( !GetSessionTarget() || !GetLivePopup() )
                return false;

            wxWinUITrackRichTipSession(shared_from_this());
            ++gs_teachingTipDiagnostics.genericFallbackStarts;
            rollback.Dismiss();
            return true;
        }
        catch ( const std::exception& e )
        {
            wxLogWarning(
                "WinUI managed rich-tooltip fallback failed: %s",
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
            wxLogWarning(
                "WinUI managed rich-tooltip fallback failed with an "
                "unknown exception.");
        }
        return false;
    }

    wxWindow *GetSessionTarget() const override
    {
        wxWindow * const target = targetWindow.get();
        return target && target == targetIdentity &&
                       !target->IsBeingDeleted() && GetHwndOf(target)
            ? target
            : nullptr;
    }

    void CloseForQueueShutdown() override
    {
        if ( !active || closing )
            return;

        ++gs_teachingTipDiagnostics.queueShutdownClosures;
        Close(true);
    }

    void NotifyReparent(wxWindow *subtreeRoot) override
    {
        if ( !active || closing || !subtreeRoot )
            return;

        wxWindow * const target = GetSessionTarget();
        if ( !target ||
                (target != subtreeRoot &&
                 !subtreeRoot->IsDescendant(target)) )
        {
            return;
        }

        if ( wxGetTopLevelParent(target) != targetTLW.get() )
        {
            ++gs_teachingTipDiagnostics.reparentClosures;
            Close(true);
        }
    }

private:
    wxWindow *GetLivePopup() const
    {
        wxWindow * const popup = popupWindow.get();
        return popup && popup == popupIdentity && !popup->IsBeingDeleted() &&
                       GetHwndOf(popup)
            ? popup
            : nullptr;
    }

    void CancelFromManager()
    {
        ++gs_teachingTipDiagnostics.genericFallbackCancellations;
        Close(true);
    }

    bool UnbindTarget()
    {
        if ( !targetDestroyBound )
            return true;

        bool detached = inTargetDestroyDispatch;
        if ( !detached )
        {
            if ( wxWindow * const target = targetWindow.get() )
            {
                detached = target->Unbind(
                    wxEVT_DESTROY,
                    &wxWinUIGenericTipState::OnTargetDestroy,
                    this);
            }
            else
            {
                detached = true;
            }
        }
        if ( detached )
            targetDestroyBound = false;
        return detached;
    }

    bool UnbindPopup()
    {
        if ( !popupDestroyBound )
            return true;

        bool detached = inPopupDestroyDispatch;
        if ( !detached )
        {
            if ( wxWindow * const popup = popupWindow.get() )
            {
                detached = popup->Unbind(
                    wxEVT_DESTROY,
                    &wxWinUIGenericTipState::OnPopupDestroy,
                    this);
            }
            else
            {
                detached = true;
            }
        }
        if ( detached )
            popupDestroyBound = false;
        return detached;
    }

    void Close(bool destroyPopup)
    {
        if ( closing )
            return;

        const auto keepAlive = shared_from_this();
        closing = true;
        active = false;

        try
        {
            registration.Reset();
        }
        catch ( ... )
        {
        }

        bool targetDetached = false;
        bool popupDetached = false;
        try
        {
            targetDetached = UnbindTarget();
        }
        catch ( ... )
        {
        }
        try
        {
            popupDetached = UnbindPopup();
        }
        catch ( ... )
        {
        }

        wxWindow * const popup = popupWindow.get();
        if ( destroyPopup && popup && popup == popupIdentity &&
                !popup->IsBeingDeleted() )
        {
            try
            {
                popup->Destroy();
            }
            catch ( ... )
            {
            }
        }

        if ( genericCounted )
        {
            genericCounted = false;
            --gs_teachingTipDiagnostics.liveGenericFallbacks;
        }
        if ( targetDetached )
        {
            targetWindow = nullptr;
            targetTLW = nullptr;
        }
        if ( popupDetached )
            popupWindow = nullptr;
        if ( targetDetached && popupDetached )
            self.reset();
    }

    void OnTargetDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();
        if ( event.GetWindow() != targetIdentity )
            return;

        const auto keepAlive = shared_from_this();
        inTargetDestroyDispatch = true;
        if ( closing )
        {
            targetDestroyBound = false;
            targetWindow = nullptr;
            targetTLW = nullptr;
            if ( !popupDestroyBound )
                self.reset();
        }
        else
        {
            Close(true);
        }
        inTargetDestroyDispatch = false;
    }

    void OnPopupDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();
        if ( event.GetWindow() != popupIdentity )
            return;

        const auto keepAlive = shared_from_this();
        inPopupDestroyDispatch = true;
        if ( closing )
        {
            popupDestroyBound = false;
            popupWindow = nullptr;
            if ( !targetDestroyBound )
                self.reset();
        }
        else
        {
            Close(false);
        }
        inPopupDestroyDispatch = false;
    }

    wxWinUITransientRegistration registration;
    std::shared_ptr<wxWinUIGenericTipState> self;
    wxWeakRef<wxWindow> targetWindow;
    wxWeakRef<wxWindow> targetTLW;
    wxWeakRef<wxWindow> popupWindow;
    wxWindow *targetIdentity = nullptr;
    wxWindow *popupIdentity = nullptr;
    bool targetDestroyBound = false;
    bool popupDestroyBound = false;
    bool inTargetDestroyDispatch = false;
    bool inPopupDestroyDispatch = false;
    bool active = false;
    bool closing = false;
    bool genericCounted = false;
};

class wxWinUIRichToolTipImpl : public wxRichToolTipGenericImpl
{
public:
    wxWinUIRichToolTipImpl(const wxString& title, const wxString& message)
        : wxRichToolTipGenericImpl(title, message)
    {
    }

    void SetBackgroundColour(const wxColour& col,
                             const wxColour& colEnd) override
    {
        m_hasCustomBackground = col.IsOk() || colEnd.IsOk();
        wxRichToolTipGenericImpl::SetBackgroundColour(col, colEnd);
    }

    void SetCustomIcon(const wxBitmapBundle& icon) override
    {
        m_hasCustomIcon = icon.IsOk();
        wxRichToolTipGenericImpl::SetCustomIcon(icon);
    }

    void SetStandardIcon(int icon) override
    {
        m_hasCustomIcon = (icon & wxICON_MASK) != wxICON_NONE;
        wxRichToolTipGenericImpl::SetStandardIcon(icon);
    }

    void SetTimeout(unsigned milliseconds,
                    unsigned millisecondsDelay = 0) override
    {
        m_winuiTimeout = milliseconds;
        m_winuiDelay = millisecondsDelay;
        wxRichToolTipGenericImpl::SetTimeout(milliseconds, millisecondsDelay);
    }

    void SetTipKind(wxTipKind kind) override
    {
        m_tipKind = kind;
        wxRichToolTipGenericImpl::SetTipKind(kind);
    }

    void SetTitleFont(const wxFont& font) override
    {
        m_hasCustomTitleFont = font.IsOk();
        wxRichToolTipGenericImpl::SetTitleFont(font);
    }

    void ShowFor(wxWindow* win, const wxRect* rect = nullptr) override
    {
        // TeachingTip::Close()/Closed is a logical notification, not a
        // physical XAML-popup rundown boundary. Until that boundary is
        // causally observable, the beta backend must not publish a native
        // TeachingTip: use the bounded, centrally cancelled generic popup.
        if ( !gs_nativeTeachingTipBackendForTesting )
        {
            ++gs_teachingTipDiagnostics.betaGenericFallbacks;
            if ( !ShowManagedGenericFallback(win, rect) )
            {
                ++gs_teachingTipDiagnostics.operationalFailures;
                wxLogWarning(
                    "WinUI rich-tooltip beta fallback could not be "
                    "registered and was destroyed.");
            }
            return;
        }

        // TeachingTip has no safe equivalent for a custom gradient, bitmap
        // icon or title font. Honour the wx API by selecting the generic
        // implementation instead of silently discarding customization.
        if ( m_hasCustomBackground ||
                m_hasCustomIcon ||
                m_hasCustomTitleFont )
        {
            ++gs_teachingTipDiagnostics.customizationFallbacks;
            if ( !ShowManagedGenericFallback(win, rect) )
            {
                ++gs_teachingTipDiagnostics.operationalFailures;
                wxLogWarning(
                    "WinUI rich-tooltip generic fallback could not be "
                    "registered and was destroyed.");
            }
            return;
        }

        const wxWinUITeachingTipShowResult result =
            ShowTeachingTip(win, rect);
        if ( wxWinUITeachingTipAllowsGenericFallback(result) )
        {
            ++gs_teachingTipDiagnostics.unsupportedFallbacks;
            if ( !ShowManagedGenericFallback(win, rect) )
            {
                ++gs_teachingTipDiagnostics.operationalFailures;
                wxLogWarning(
                    "WinUI rich-tooltip unsupported fallback could not be "
                    "registered and was destroyed.");
            }
        }
        else if ( result == wxWinUITeachingTipShowResult::OperationalFailure )
        {
            ++gs_teachingTipDiagnostics.operationalFailures;
            wxLogWarning(
                "WinUI TeachingTip presentation failed; the generic "
                "fallback was not used because the native presentation "
                "may already have performed observable work.");
        }
        else if ( result == wxWinUITeachingTipShowResult::Cancelled )
        {
            ++gs_teachingTipDiagnostics.cancellations;
        }
    }

private:
    bool ShowManagedGenericFallback(wxWindow* win, const wxRect* rect)
    {
        try
        {
            wxWeakRef<wxWindow> weakTarget(win);
            wxWindow *target = weakTarget.get();
            if ( !target || target->IsBeingDeleted() )
                return false;

            wxWindow * const popup = ShowForPopup(target, rect);
            if ( !popup )
                return false;

            wxWeakRef<wxWindow> weakPopup(popup);
            wxScopeGuard cleanup = wxMakeGuard(
                [&weakPopup]()
                {
                    if ( wxWindow * const livePopup = weakPopup.get() )
                    {
                        if ( !livePopup->IsBeingDeleted() )
                            livePopup->Destroy();
                    }
                });

            target = weakTarget.get();
            wxWindow * const livePopup = weakPopup.get();
            if ( !target || target->IsBeingDeleted() || !livePopup ||
                 livePopup->IsBeingDeleted() )
            {
                return false;
            }

            const auto state = std::make_shared<wxWinUIGenericTipState>();
            if ( !state->Start(target, livePopup) )
                return false;

            cleanup.Dismiss();
            return true;
        }
        catch ( const std::exception& e )
        {
            wxLogWarning(
                "WinUI rich-tooltip generic fallback failed: %s",
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
            wxLogWarning(
                "WinUI rich-tooltip generic fallback failed with an "
                "unknown exception.");
        }
        return false;
    }

    wxWinUITeachingTipShowResult
    ShowTeachingTip(wxWindow* win, const wxRect* rect);

    // 5s default, as in the generic implementation.
    unsigned m_winuiTimeout = 5000;
    unsigned m_winuiDelay = 0;
    wxTipKind m_tipKind = wxTipKind_Auto;
    bool m_hasCustomBackground = false;
    bool m_hasCustomIcon = false;
    bool m_hasCustomTitleFont = false;
};

wxWinUITeachingTipShowResult
wxWinUIRichToolTipImpl::ShowTeachingTip(wxWindow* win,
                                       const wxRect* rect)
{
    ++gs_teachingTipDiagnostics.nativeBackendAttempts;
    auto state = std::make_shared<wxWinUITeachingTipState>();
    if ( !state->PrepareTarget(win) )
        return wxWinUITeachingTipShowResult::Cancelled;

    const wxWinUITeachingTipFaultForTesting fault =
        wxWinUITakeTeachingTipFaultForTesting();
    if ( fault == wxWinUITeachingTipFaultForTesting::Unsupported )
        return wxWinUITeachingTipShowResult::Unsupported;
    state->SetFaultForTesting(fault);

    if ( !wxWinUI3Initialize() )
        return wxWinUITeachingTipShowResult::OperationalFailure;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;

        wxWindow *targetWindow = state->GetLiveTarget();
        if ( !targetWindow )
            return wxWinUITeachingTipShowResult::Cancelled;

        // The tip lives on the window's shared per-TLW island. ForWindow()
        // can initialize XAML and dispatch messages, so no pointer obtained
        // before it is reused afterwards.
        wxWinUITopLevelHost *tlwHost =
            wxWinUITopLevelHost::ForWindow(targetWindow, true);
        targetWindow = state->GetLiveTarget();
        if ( !targetWindow )
            return wxWinUITeachingTipShowResult::Cancelled;
        if ( !tlwHost || !tlwHost->GetXamlRoot() )
            return wxWinUITeachingTipShowResult::OperationalFailure;

        state->root = tlwHost->Root();

        MUXC::TeachingTip tip{ nullptr };
        try
        {
            tip = MUXC::TeachingTip();
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( !state->GetLiveTarget() )
                return wxWinUITeachingTipShowResult::Cancelled;
            if ( wxWinUIIsTeachingTipUnavailable(e) )
                return wxWinUITeachingTipShowResult::Unsupported;

            wxWinUILogException("WinUI TeachingTip activation", e);
            return wxWinUITeachingTipShowResult::OperationalFailure;
        }

        state->tip = tip;
        tip.Title(wxWinUIToHString(m_title));
        tip.Subtitle(wxWinUIToHString(m_message));
        MUXA::AutomationProperties::SetName(
            tip,
            wxWinUIToHString(m_title));
        MUXA::AutomationProperties::SetHelpText(
            tip,
            wxWinUIToHString(m_message));
        gs_teachingTipDiagnostics.lastAutomationName =
            wxString(MUXA::AutomationProperties::GetName(tip).c_str());
        gs_teachingTipDiagnostics.lastAutomationHelpText =
            wxString(MUXA::AutomationProperties::GetHelpText(tip).c_str());
        tip.IsLightDismissEnabled(true);
        tip.ShouldConstrainToRootBounds(false);
        const wxWinUITeachingTipContract contract =
            wxWinUIResolveTeachingTipContract(m_tipKind);
        tip.PreferredPlacement(
            wxWinUIToTeachingTipPlacement(contract.placement));
        tip.TailVisibility(
            contract.showTail
                ? MUXC::TeachingTipTailVisibility::Auto
                : MUXC::TeachingTipTailVisibility::Collapsed);

        if ( rect )
        {
            state->targetRect = *rect;
            state->hasTargetRect = true;
            gs_teachingTipDiagnostics.lastHadTargetRect = true;
            gs_teachingTipDiagnostics.lastTargetRect = *rect;
        }

        // An explicit wx client rectangle always wins. Otherwise anchor to
        // the window's own slot when available; non-slotted windows get a
        // zero-size anchor at their centre.
        winrt::Microsoft::UI::Xaml::FrameworkElement target{ nullptr };
        targetWindow = state->GetLiveTarget();
        if ( !targetWindow )
            return wxWinUITeachingTipShowResult::Cancelled;
        tlwHost = wxWinUITopLevelHost::ForWindow(targetWindow, false);
        if ( !tlwHost || !tlwHost->GetXamlRoot() ||
                tlwHost->Root() != state->root )
        {
            return wxWinUITeachingTipShowResult::Cancelled;
        }

        wxWinUISlot * const slot = tlwHost->FindSlot(targetWindow);
        if ( !rect && slot )
        {
            target = slot->GetContainer();
        }
        else
        {
            winrt::Windows::Foundation::Point clientPoint{};
            if ( rect && !rect->IsEmpty() )
            {
                clientPoint.X = static_cast<float>(
                    rect->x + rect->width / 2.0);
                clientPoint.Y = static_cast<float>(
                    rect->y + rect->height / 2.0);
            }
            else
            {
                const wxSize clientSize =
                    targetWindow->GetClientSize();
                clientPoint.X =
                    static_cast<float>(clientSize.x / 2.0);
                clientPoint.Y =
                    static_cast<float>(clientSize.y / 2.0);
            }

            targetWindow = state->GetLiveTarget();
            if ( !targetWindow )
                return wxWinUITeachingTipShowResult::Cancelled;
            tlwHost = wxWinUITopLevelHost::ForWindow(targetWindow, false);
            if ( !tlwHost || !tlwHost->GetXamlRoot() ||
                    tlwHost->Root() != state->root )
            {
                return wxWinUITeachingTipShowResult::Cancelled;
            }

            winrt::Windows::Foundation::Point dip{};
            if ( wxWinUIVisualCoordinates::ClientPointToRoot(
                     targetWindow, clientPoint, &dip) !=
                 wxWinUICoordinateResult::Mapped )
            {
                return wxWinUITeachingTipShowResult::Cancelled;
            }
            gs_teachingTipDiagnostics.lastAnchorInRoot =
                wxPoint(static_cast<int>(std::lround(dip.X)),
                        static_cast<int>(std::lround(dip.Y)));

            MUXC::Border anchor;
            anchor.IsHitTestVisible(false);
            anchor.Width(1);
            anchor.Height(1);
            MUXC::Canvas::SetLeft(anchor, dip.X);
            MUXC::Canvas::SetTop(anchor, dip.Y);
            state->anchor = anchor;
            state->AppendAnchor();
            target = anchor;

            if ( !state->GetLiveTarget() )
                return wxWinUITeachingTipShowResult::Cancelled;
        }
        tip.Target(target);
        if ( !state->GetLiveTarget() )
            return wxWinUITeachingTipShowResult::Cancelled;
        if ( !state->RefreshTarget() )
            return wxWinUITeachingTipShowResult::Cancelled;

        state->AppendTip();

        if ( const auto hook = wxWinUITakeTeachingTipHookForTesting(
                wxWinUITeachingTipHookPointForTesting::AfterAppend) )
        {
            if ( wxWindow * const targetAfterAppend = state->GetLiveTarget() )
                hook(targetAfterAppend);
        }

        if ( !state->GetLiveTarget() )
            return wxWinUITeachingTipShowResult::Cancelled;

        if ( fault ==
                wxWinUITeachingTipFaultForTesting::OperationalAfterAppend )
        {
            throw winrt::hresult_error(
                E_FAIL,
                L"Injected TeachingTip failure after visual attachment");
        }

        return state->Start(m_winuiTimeout, m_winuiDelay);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TeachingTip", e);
    }
    catch ( const std::exception& e )
    {
        wxLogWarning(
            "WinUI TeachingTip failed: %s",
            wxString::FromUTF8(e.what()));
    }
    catch ( ... )
    {
        wxLogWarning(
            "WinUI TeachingTip failed with an unknown exception.");
    }

    return wxWinUITeachingTipShowResult::OperationalFailure;
}

} // anonymous namespace

void wxWinUISetTeachingTipFaultForTesting(
    wxWinUITeachingTipFaultForTesting fault)
{
    gs_teachingTipFaultForTesting = fault;
}

void wxWinUISetNativeTeachingTipBackendForTesting(bool enable)
{
    gs_nativeTeachingTipBackendForTesting = enable;
}

void wxWinUISetTeachingTipHookForTesting(
    wxWinUITeachingTipHookPointForTesting point,
    wxWinUITeachingTipHookForTesting hook)
{
    gs_teachingTipHookPointForTesting = point;
    gs_teachingTipHookForTesting = hook;
}

void wxWinUIResetTeachingTipDiagnosticsForTesting()
{
    const unsigned liveStates = gs_teachingTipDiagnostics.liveStates;
    const unsigned liveXamlHandlers =
        gs_teachingTipDiagnostics.liveXamlHandlers;
    const unsigned liveTargetBindings =
        gs_teachingTipDiagnostics.liveTargetBindings;
    const unsigned liveVisuals = gs_teachingTipDiagnostics.liveVisuals;
    const unsigned liveGenericFallbacks =
        gs_teachingTipDiagnostics.liveGenericFallbacks;

    gs_teachingTipDiagnostics = {};
    gs_teachingTipDiagnostics.liveStates = liveStates;
    gs_teachingTipDiagnostics.liveXamlHandlers = liveXamlHandlers;
    gs_teachingTipDiagnostics.liveTargetBindings = liveTargetBindings;
    gs_teachingTipDiagnostics.liveVisuals = liveVisuals;
    gs_teachingTipDiagnostics.liveGenericFallbacks =
        liveGenericFallbacks;

    gs_teachingTipFaultForTesting =
        wxWinUITeachingTipFaultForTesting::None;
    gs_teachingTipHookPointForTesting =
        wxWinUITeachingTipHookPointForTesting::None;
    gs_teachingTipHookForTesting = nullptr;
    gs_nativeTeachingTipBackendForTesting = false;
    wxWinUIPruneRichTipSessions();
}

wxWinUITeachingTipDiagnosticsForTesting
wxWinUIGetTeachingTipDiagnosticsForTesting()
{
    return gs_teachingTipDiagnostics;
}

std::size_t
wxWinUITriggerTeachingTipQueueShutdownForTesting(wxWindow *ownerOrTLW)
{
    wxWindow * const tlw =
        ownerOrTLW ? wxGetTopLevelParent(ownerOrTLW) : nullptr;
    if ( !tlw )
        return 0;

    wxWeakRef<wxWindow> weakTLW(tlw);
    std::size_t closed = 0;
    const auto sessions = wxWinUISnapshotRichTipSessions();
    for ( const auto& state : sessions )
    {
        wxWindow * const liveTLW = weakTLW.get();
        if ( !liveTLW || liveTLW->IsBeingDeleted() )
            break;
        wxWindow * const target = state->GetSessionTarget();
        if ( target && wxGetTopLevelParent(target) == liveTLW )
        {
            ++closed;
            state->CloseForQueueShutdown();
        }
    }
    return closed;
}

void wxWinUINotifyTeachingTipReparent(wxWindow *subtreeRoot)
{
    if ( !subtreeRoot )
        return;

    // Snapshot strong state references, never windows. A callback can close
    // itself (and thereby expire the registry weak_ptr) without invalidating
    // this bounded traversal.
    wxWeakRef<wxWindow> weakSubtree(subtreeRoot);
    const auto sessions = wxWinUISnapshotRichTipSessions();
    for ( const auto& state : sessions )
    {
        wxWindow * const liveSubtree = weakSubtree.get();
        if ( !liveSubtree || liveSubtree->IsBeingDeleted() )
            break;
        state->NotifyReparent(liveSubtree);
    }
}

/* static */
wxRichToolTipImpl*
wxRichToolTipImpl::Create(const wxString& title, const wxString& message)
{
    return new wxWinUIRichToolTipImpl(title, message);
}

#endif // wxUSE_RICHTOOLTIP && wxUSE_WINUI3
