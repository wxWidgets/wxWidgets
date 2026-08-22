/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuirichtooltip.cpp
// Purpose:     Tests for wxWinUI rich-tooltip backend policy and lifetimes
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

#include "wx/bmpbndl.h"
#include "wx/font.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/panel.h"
#include "wx/richtooltip.h"
#include "wx/utils.h"

#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/richtooltip.h"
#include "wx/winui/private/tlwhost.h"

#include <cmath>
#include <chrono>
#include <functional>

namespace
{

void CheckPlacement(wxTipKind kind,
                    wxWinUITeachingTipPlacement placement,
                    bool showTail = true)
{
    const wxWinUITeachingTipContract contract =
        wxWinUIResolveTeachingTipContract(kind);
    CHECK(contract.placement == placement);
    CHECK(contract.showTail == showTail);
}

void DestroyTeachingTipTarget(wxWindow *target)
{
    target->Destroy();
}

void CheckNoTeachingTipResidue()
{
    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.liveStates == 0);
    CHECK(diagnostics.liveXamlHandlers == 0);
    CHECK(diagnostics.liveTargetBindings == 0);
    CHECK(diagnostics.liveVisuals == 0);
    CHECK(diagnostics.liveGenericFallbacks == 0);
}

enum class TeachingTipTestBackend
{
    BetaPolicy,
    NativeBackend
};

class TeachingTipTestReset final
{
public:
    explicit TeachingTipTestReset(TeachingTipTestBackend backend)
    {
        wxWinUIResetTeachingTipDiagnosticsForTesting();
        wxWinUISetNativeTeachingTipBackendForTesting(
            backend == TeachingTipTestBackend::NativeBackend);
    }

    ~TeachingTipTestReset()
    {
        wxWinUIResetTeachingTipDiagnosticsForTesting();
    }
};

bool WaitForTeachingTip(
    const std::function<bool ()>& predicate,
    std::chrono::milliseconds timeout = std::chrono::seconds(2))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while ( std::chrono::steady_clock::now() < deadline )
    {
        wxYield();
        if ( predicate() )
            return true;
        wxMilliSleep(2);
    }
    wxYield();
    return predicate();
}

} // anonymous namespace

TEST_CASE("WinUIRichToolTip::PlacementContract",
          "[winui-rich-tooltip]")
{
    CheckPlacement(wxTipKind_None,
                   wxWinUITeachingTipPlacement::Auto,
                   false);
    CheckPlacement(wxTipKind_Auto,
                   wxWinUITeachingTipPlacement::Auto);
    CheckPlacement(wxTipKind_Top,
                   wxWinUITeachingTipPlacement::Bottom);
    CheckPlacement(wxTipKind_Bottom,
                   wxWinUITeachingTipPlacement::Top);
    CheckPlacement(wxTipKind_TopLeft,
                   wxWinUITeachingTipPlacement::BottomRight);
    CheckPlacement(wxTipKind_TopRight,
                   wxWinUITeachingTipPlacement::BottomLeft);
    CheckPlacement(wxTipKind_BottomLeft,
                   wxWinUITeachingTipPlacement::TopRight);
    CheckPlacement(wxTipKind_BottomRight,
                   wxWinUITeachingTipPlacement::TopLeft);

    CHECK(wxWinUITeachingTipAllowsGenericFallback(
        wxWinUITeachingTipShowResult::Unsupported));
    CHECK_FALSE(wxWinUITeachingTipAllowsGenericFallback(
        wxWinUITeachingTipShowResult::Shown));
    CHECK_FALSE(wxWinUITeachingTipAllowsGenericFallback(
        wxWinUITeachingTipShowResult::Cancelled));
    CHECK_FALSE(wxWinUITeachingTipAllowsGenericFallback(
        wxWinUITeachingTipShowResult::OperationalFailure));
}

TEST_CASE("WinUIRichToolTip::BetaPolicyUsesOnlyGenericPopup",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::BetaPolicy);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "Rich tooltip beta fallback",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip tip("Beta", "Causally bounded generic popup");
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }

    wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.betaGenericFallbacks == 1);
    CHECK(diagnostics.nativeBackendAttempts == 0);
    CHECK(diagnostics.genericFallbackStarts == 1);
    CHECK(diagnostics.liveGenericFallbacks == 1);
    CHECK(diagnostics.liveStates == 0);
    CHECK(diagnostics.liveXamlHandlers == 0);
    CHECK(diagnostics.liveVisuals == 0);
    CHECK(diagnostics.peerOpens == 0);
    CHECK(diagnostics.operationalFailures == 0);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);

    wxWinUICancelTransientSessions(owner);
    diagnostics = wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.genericFallbackCancellations == 1);
    CheckNoTeachingTipResidue();

    owner->Destroy();
    wxYield();
}

TEST_CASE("WinUIRichToolTip::UnsupportedUsesGenericFallback",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip unsupported fallback",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    wxWinUISetTeachingTipFaultForTesting(
        wxWinUITeachingTipFaultForTesting::Unsupported);
    {
        wxRichToolTip tip("Fallback", "TeachingTip is unavailable");
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }

    wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.unsupportedFallbacks == 1);
    CHECK(diagnostics.operationalFailures == 0);
    CHECK(diagnostics.cancellations == 0);
    CHECK(diagnostics.liveGenericFallbacks == 1);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);

    wxWinUICancelTransientSessions(owner);
    diagnostics = wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.genericFallbackCancellations == 1);
    CheckNoTeachingTipResidue();

    owner->Destroy();
    wxYield();
}

TEST_CASE("WinUIRichToolTip::OperationalFailureNeverFallsBack",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip operational failure",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    wxWinUISetTeachingTipFaultForTesting(
        wxWinUITeachingTipFaultForTesting::OperationalAfterAppend);
    {
        // The injected HRESULT and the explicit no-fallback diagnostic are
        // expected. Keep the test output quiet without weakening the result
        // counters or the lifetime assertions below.
        wxLogNull noLog;
        wxRichToolTip tip("Failure", "Must not create a second popup");
        tip.SetTimeout(0);
        tip.ShowFor(owner);
    }

    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.unsupportedFallbacks == 0);
    CHECK(diagnostics.operationalFailures == 1);
    CHECK(diagnostics.cancellations == 0);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();

    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::TargetDestroyedAfterAppendIsSafe",
           "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip append reentrance",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const target = new wxPanel(owner, wxID_ANY);

    wxWinUISetTeachingTipHookForTesting(
        wxWinUITeachingTipHookPointForTesting::AfterAppend,
        &DestroyTeachingTipTarget);
    {
        wxRichToolTip tip("Reentrant append", "Target destroys itself");
        tip.SetTimeout(0);
        tip.ShowFor(target);
    }

    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.unsupportedFallbacks == 0);
    CHECK(diagnostics.operationalFailures == 0);
    CHECK(diagnostics.cancellations == 1);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();

    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::TargetDestroyedAfterFlowDirectionIsSafe",
          "[winui-rich-tooltip][winui-009]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip flow-direction reentrance",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const target = new wxPanel(owner, wxID_ANY);
    target->SetLayoutDirection(wxLayout_RightToLeft);

    wxWinUISetTeachingTipHookForTesting(
        wxWinUITeachingTipHookPointForTesting::AfterFlowDirection,
        &DestroyTeachingTipTarget);
    {
        wxRichToolTip tip(
            "Reentrant flow direction",
            "Target destroys itself after the XAML setters");
        tip.SetTimeout(0);
        tip.ShowFor(target);
    }

    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.unsupportedFallbacks == 0);
    CHECK(diagnostics.operationalFailures == 0);
    CHECK(diagnostics.cancellations == 1);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();

    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::OwnerDestroyedDuringOpenIsSafe",
           "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip open reentrance",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    wxWinUISetTeachingTipHookForTesting(
        wxWinUITeachingTipHookPointForTesting::AfterPeerOpen,
        &DestroyTeachingTipTarget);
    {
        wxRichToolTip tip("Reentrant open", "Owner destroys itself");
        tip.SetTimeout(0);
        tip.ShowFor(owner);
    }
    wxYield();

    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.unsupportedFallbacks == 0);
    CHECK(diagnostics.operationalFailures == 0);
    // wxWindow::Destroy() is deferred: ShowFor() legitimately completes as
    // Shown, then the target-destroy binding closes the already-live session.
    // This is a lifecycle close, not a failed presentation.
    CHECK(diagnostics.cancellations == 0);
    CheckNoTeachingTipResidue();
}

TEST_CASE("WinUIRichToolTip::CentralCancellation",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip transient owner",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip tip("Title", "Message");
        // A zero timeout is deliberately persistent. This makes the registry
        // state deterministic without sleeping or driving physical input.
        tip.SetTimeout(0);
        tip.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);

    const wxWinUITeachingTipDiagnosticsForTesting activeDiagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(activeDiagnostics.liveStates == 1);
    CHECK(activeDiagnostics.liveXamlHandlers >= 2);
    CHECK(activeDiagnostics.liveTargetBindings == 1);
    CHECK(activeDiagnostics.liveVisuals >= 1);

    wxWinUICancelTransientSessions(owner);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();

    // Repeated cancellation is a no-op and destruction cannot call a stale
    // XAML delegate back into the already-destroyed wxRichToolTip facade.
    wxWinUICancelTransientSessions(owner);
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::ExplicitRectAnchorsThePeer",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip explicit rectangle",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const target = new wxPanel(
        owner, wxID_ANY, wxPoint(17, 19), wxSize(180, 100));
    target->SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(target->GetLayoutDirection() == wxLayout_RightToLeft);
    const wxRect rect(11, 13, 41, 29);

    {
        wxRichToolTip tip("Rect", "Explicit target rectangle");
        tip.SetTimeout(0);
        tip.ShowFor(target, &rect);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.lastHadTargetRect);
    CHECK(diagnostics.lastRightToLeft);
    CHECK(diagnostics.lastTargetRect == rect);
    winrt::Windows::Foundation::Point expectedDIP{};
    REQUIRE(
        wxWinUIVisualCoordinates::ClientPointToRoot(
            target,
            winrt::Windows::Foundation::Point{
                static_cast<float>(
                    rect.x + rect.width / 2.0),
                static_cast<float>(
                    rect.y + rect.height / 2.0)},
            &expectedDIP) ==
        wxWinUICoordinateResult::Mapped);
    const wxPoint expectedAnchor(
        static_cast<int>(std::lround(expectedDIP.X)),
        static_cast<int>(std::lround(expectedDIP.Y)));
    CHECK(diagnostics.lastAnchorInRoot == expectedAnchor);

    wxWinUICancelTransientSessions(owner);
    const wxWinUITeachingTipDiagnosticsForTesting closedDiagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(closedDiagnostics.cleanupTipRemovalAttempts == 1);
    CHECK(closedDiagnostics.cleanupAnchorRemovalAttempts == 1);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::DelayOpenTimeoutSequence",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip timer phases",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip tip("Timed", "Delay then timeout");
        tip.SetTimeout(80, 20);
        tip.ShowFor(owner);
    }

    wxWinUITeachingTipDiagnosticsForTesting diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.lastRequestedDelayMs == 20);
    CHECK(diagnostics.lastRequestedTimeoutMs == 80);
    CHECK(diagnostics.delayedStarts == 1);
    CHECK(diagnostics.peerOpens == 0);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);

    REQUIRE(WaitForTeachingTip(
        []()
        {
            return wxWinUIGetTeachingTipDiagnosticsForTesting().
                       peerOpens == 1;
        }));
    REQUIRE(WaitForTeachingTip(
        [owner]()
        {
            return wxWinUIGetTeachingTipDiagnosticsForTesting().
                       timeoutClosures == 1 &&
                   wxWinUITransientCountForTesting(
                       owner,
                       wxWinUITransientKind::TeachingTip) == 0;
        }));
    diagnostics = wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.lastPeerOpenSequence != 0);
    CHECK(diagnostics.lastTimeoutCloseSequence >
          diagnostics.lastPeerOpenSequence);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::CustomAppearanceUsesGenericFallback",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip custom appearance fallback",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip tip("Colours", "Gradient fallback");
        tip.SetBackgroundColour(*wxRED, *wxBLUE);
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              customizationFallbacks == 1);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();

    {
        wxRichToolTip tip("Icon", "Bitmap fallback");
        tip.SetIcon(wxBitmapBundle::FromBitmap(wxBitmap(16, 16)));
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              customizationFallbacks == 2);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();

    {
        wxRichToolTip tip("Font", "Title-font fallback");
        tip.SetTitleFont(wxFont(wxFontInfo(12).Bold()));
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              customizationFallbacks == 3);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();

    owner->Destroy();
    wxYield();
}

TEST_CASE("WinUIRichToolTip::StartupIsTransactionalForAllExceptions",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    const wxWinUITeachingTipFaultForTesting faults[] =
    {
        wxWinUITeachingTipFaultForTesting::StartHResult,
        wxWinUITeachingTipFaultForTesting::StartStdException,
        wxWinUITeachingTipFaultForTesting::StartUnknownException,
        wxWinUITeachingTipFaultForTesting::QueueUnavailable
    };

    for ( const auto fault : faults )
    {
        wxFrame * const owner = new wxFrame(
            nullptr,
            wxID_ANY,
            "TeachingTip startup transaction",
            wxPoint(-32000, -32000),
            wxSize(320, 200));

        wxWinUISetTeachingTipFaultForTesting(fault);
        {
            wxLogNull noLog;
            wxRichToolTip tip("Transactional", "Injected startup failure");
            tip.SetTimeout(0);
            tip.ShowFor(owner);
        }

        const wxWinUITeachingTipDiagnosticsForTesting diagnostics =
            wxWinUIGetTeachingTipDiagnosticsForTesting();
        CHECK(diagnostics.operationalFailures == 1);
        CHECK(wxWinUITransientCountForTesting(
                  owner, wxWinUITransientKind::TeachingTip) == 0);
        CheckNoTeachingTipResidue();

        owner->Destroy();
        wxYield();
        wxWinUIResetTeachingTipDiagnosticsForTesting();
        wxWinUISetNativeTeachingTipBackendForTesting(true);
    }
}

TEST_CASE("WinUIRichToolTip::DelayedOpenFailureClosesPeer",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip delayed open failure",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    wxWinUISetTeachingTipFaultForTesting(
        wxWinUITeachingTipFaultForTesting::OpenAfterPeerHResult);
    {
        wxLogNull noLog;
        wxRichToolTip tip("Delayed failure", "Peer must be closed");
        tip.SetTimeout(0, 10);
        tip.ShowFor(owner);
    }

    REQUIRE(WaitForTeachingTip(
        [owner]()
        {
            const auto diagnostics =
                wxWinUIGetTeachingTipDiagnosticsForTesting();
            return diagnostics.operationalFailures == 1 &&
                   wxWinUITransientCountForTesting(
                       owner,
                       wxWinUITransientKind::TeachingTip) == 0;
        }));
    const auto diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.peerOpens == 1);
    CHECK(diagnostics.peerCloseRequests >= 1);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::MultipleSessionsCancelTogether",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip multiple sessions",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip first("First", "Persistent first tip");
        first.SetTimeout(0);
        first.ShowFor(owner);
        wxRichToolTip second("Second", "Persistent second tip");
        second.SetTimeout(0);
        second.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 2);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().liveStates == 2);
    wxWinUICancelTransientSessions(owner);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::QueueShutdownClosesNativeAndFallback",
          "[winui-rich-tooltip][winui-beta-transients]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip queue shutdown",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip nativeTip("Native", "Queue-owned TeachingTip");
        nativeTip.SetTimeout(0);
        nativeTip.ShowFor(owner);

        wxRichToolTip fallback("Fallback", "Managed generic popup");
        fallback.SetBackgroundColour(*wxRED, *wxBLUE);
        fallback.SetTimeout(0, 60000);
        fallback.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 2);
    CHECK(wxWinUITriggerTeachingTipQueueShutdownForTesting(owner) == 2);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              queueShutdownClosures == 2);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::ConcurrentPopupAndQueuedTeachingTip",
          "[winui-rich-tooltip][winui-beta-transients]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip concurrent popup",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    const wxWinUITransientRegistrySnapshot baseline =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    unsigned popupCancels = 0;
    auto popup = wxWinUIRegisterTransient(
        owner,
        wxWinUITransientKind::Popup,
        [&]() { ++popupCancels; });
    REQUIRE(popup);

    {
        wxRichToolTip tip(
            "Queued TeachingTip",
            "Shares the authoritative per-TLW registry with the popup");
        // Keep opening pending long enough for the deterministic queue-shutdown
        // seam to win. No timer sleep or physical popup interaction is needed.
        tip.SetTimeout(0, 60000);
        tip.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::Popup) == 1);
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    const wxWinUITransientRegistrySnapshot concurrent =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CHECK(concurrent.ownerCount == baseline.ownerCount + 1);
    CHECK(concurrent.transientCount == baseline.transientCount + 2);
    CHECK(concurrent.cancelCallbackCount ==
          baseline.cancelCallbackCount + 2);
    CHECK(concurrent.popupCount == baseline.popupCount + 1);
    CHECK(concurrent.teachingTipCount ==
          baseline.teachingTipCount + 1);

    CHECK(wxWinUITriggerTeachingTipQueueShutdownForTesting(owner) == 1);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);
    CHECK(popupCancels == 0);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              queueShutdownClosures == 1);
    CheckNoTeachingTipResidue();

    popup.Reset();
    CHECK(popupCancels == 0);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);
    const wxWinUITransientRegistrySnapshot cleaned =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CHECK(cleaned.ownerCount == baseline.ownerCount);
    CHECK(cleaned.transientCount == baseline.transientCount);
    CHECK(cleaned.cancelCallbackCount == baseline.cancelCallbackCount);
    CHECK(cleaned.popupCount == baseline.popupCount);
    CHECK(cleaned.teachingTipCount == baseline.teachingTipCount);
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::StandardIconResetAndUIA",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip icon and UIA",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip fallback("Warning", "Standard icon fallback");
        fallback.SetIcon(wxICON_WARNING);
        fallback.SetTimeout(0, 60000);
        fallback.ShowFor(owner);
    }
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              customizationFallbacks == 1);
    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();

    {
        wxRichToolTip nativeTip("Accessible title", "Accessible message");
        nativeTip.SetIcon(wxICON_WARNING);
        nativeTip.SetIcon(wxICON_NONE);
        nativeTip.SetIcon(
            wxBitmapBundle::FromBitmap(wxBitmap(16, 16)));
        nativeTip.SetIcon(wxBitmapBundle());
        nativeTip.SetTitleFont(wxFont(wxFontInfo(12).Bold()));
        nativeTip.SetTitleFont(wxNullFont);
        nativeTip.SetBackgroundColour(*wxRED, *wxBLUE);
        nativeTip.SetBackgroundColour(wxNullColour, wxNullColour);
        nativeTip.SetTimeout(0);
        nativeTip.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    const auto diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.customizationFallbacks == 1);
    CHECK(diagnostics.lastAutomationName == "Accessible title");
    CHECK(diagnostics.lastAutomationHelpText == "Accessible message");
    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::CrossTLWAncestorReparentCancels",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const firstOwner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip old owner",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxFrame * const secondOwner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip new owner",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const ancestor = new wxPanel(firstOwner, wxID_ANY);
    wxPanel * const target = new wxPanel(ancestor, wxID_ANY);

    {
        wxRichToolTip tip("Reparent", "Must not retain the old island");
        tip.SetTimeout(0);
        tip.ShowFor(target);
    }
    REQUIRE(wxWinUITransientCountForTesting(
                firstOwner, wxWinUITransientKind::TeachingTip) == 1);

    REQUIRE(ancestor->Reparent(secondOwner));
    CHECK(wxWinUITransientCountForTesting(
              firstOwner, wxWinUITransientKind::TeachingTip) == 0);
    CHECK(wxWinUITransientCountForTesting(
              secondOwner, wxWinUITransientKind::TeachingTip) == 0);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().reparentClosures == 1);
    CheckNoTeachingTipResidue();

    firstOwner->Destroy();
    secondOwner->Destroy();
    wxYield();
}

TEST_CASE("WinUIRichToolTip::SameTLWReparentRefreshesAnchor",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip same owner reparent",
        wxPoint(-32000, -32000),
        wxSize(360, 220));
    wxPanel * const firstParent = new wxPanel(
        owner, wxID_ANY, wxPoint(10, 10), wxSize(100, 100));
    wxPanel * const secondParent = new wxPanel(
        owner, wxID_ANY, wxPoint(180, 10), wxSize(100, 100));
    wxPanel * const target = new wxPanel(
        firstParent, wxID_ANY, wxPoint(5, 5), wxSize(40, 30));
    const wxRect anchorRect(2, 3, 10, 8);

    {
        wxRichToolTip tip("Rebind", "Anchor follows within one TLW");
        tip.SetTimeout(0);
        tip.ShowFor(target, &anchorRect);
    }
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    const wxPoint before =
        wxWinUIGetTeachingTipDiagnosticsForTesting().lastAnchorInRoot;

    REQUIRE(target->Reparent(secondParent));
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 1);
    const auto diagnostics =
        wxWinUIGetTeachingTipDiagnosticsForTesting();
    CHECK(diagnostics.reparentClosures == 0);
    CHECK(diagnostics.lastAnchorInRoot != before);

    wxWinUICancelTransientSessions(owner);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::TargetDestructionCancelsDelayedTip",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip target lifetime",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const target = new wxPanel(owner, wxID_ANY);

    {
        wxRichToolTip tip("Delayed", "Must never outlive its target");
        tip.SetTimeout(0, 1000);
        tip.ShowFor(target);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);

    target->Destroy();
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();

    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::TargetDestructionCancelsGenericFallback",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "Generic fallback target lifetime",
        wxPoint(-32000, -32000),
        wxSize(320, 200));
    wxPanel * const target = new wxPanel(owner, wxID_ANY);

    {
        wxRichToolTip tip("Fallback", "Delayed and centrally owned");
        tip.SetBackgroundColour(*wxRED, *wxBLUE);
        tip.SetTimeout(0, 60000);
        tip.ShowFor(target);
    }
    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    CHECK(wxWinUIGetTeachingTipDiagnosticsForTesting().
              liveGenericFallbacks == 1);

    target->Destroy();
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::TeachingTip) == 0);
    CheckNoTeachingTipResidue();
    owner->Destroy();
}

TEST_CASE("WinUIRichToolTip::TopLevelTargetDestroyIsSafe",
          "[winui-rich-tooltip]")
{
    TeachingTipTestReset reset(TeachingTipTestBackend::NativeBackend);
    wxFrame * const owner = new wxFrame(
        nullptr,
        wxID_ANY,
        "TeachingTip exact TLW target",
        wxPoint(-32000, -32000),
        wxSize(320, 200));

    {
        wxRichToolTip tip("Persistent", "Destroyed with its exact TLW");
        tip.SetTimeout(0);
        tip.ShowFor(owner);
    }

    REQUIRE(wxWinUITransientCountForTesting(
                owner, wxWinUITransientKind::TeachingTip) == 1);
    owner->Destroy();
    wxYield();
    CheckNoTeachingTipResidue();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RICHTOOLTIP
