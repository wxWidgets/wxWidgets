/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiflyoutretirement.cpp
// Purpose:     Real FlyoutBase popup retirement, independent of wxChoice
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "../../src/winui/private.h"
#include "wx/winui/private/tlwhost.h"
#include "host-observation-test-access.h"
#include "physical-disconnect-test-access.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace
{
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace MUXCP = MUXC::Primitives;

template <typename Predicate>
bool WaitForFlyoutRetirement(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() ) return true;
        if ( elapsed.Time() >= 2000 ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        const long remaining = 2000 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
            return false;
    }
}

bool IsWithinFlyout(MUX::DependencyObject item,
                   const MUX::DependencyObject& root)
{
    for ( unsigned depth = 0; item && depth != 512; ++depth )
    {
        if ( item == root ) return true;
        item = MUX::Media::VisualTreeHelper::GetParent(item);
    }
    return false;
}

MUXCP::Popup FindExactFlyoutPopup(const MUX::XamlRoot& xamlRoot,
                                const MUX::DependencyObject& item)
{
    MUXCP::Popup result{nullptr};
    for ( const auto& popup : MUX::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot) )
    {
        if ( popup.IsOpen() && popup.Child() && IsWithinFlyout(item, popup.Child()) )
        {
            REQUIRE_FALSE(result);
            result = popup;
        }
    }
    return result;
}

class FlyoutRetirementFixture
{
public:
    ~FlyoutRetirementFixture()
    {
        try { if ( flyout ) flyout.Hide(); }
        catch ( const winrt::hresult_error& ) { }
        // The production host owns unresolved physical debt on failure, too:
        // tests never manufacture a terminal XAML boundary to release it.
        host.Close();
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool menu)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Flyout retirement",
            wxPoint(80, 80), wxSize(420, 260));
        owner = new wxWindow(frame.get(), wxID_ANY, wxPoint(10, 10), wxSize(160, 44));
        REQUIRE(host.Initialize(owner));
        composite = wxWinUICreateCompositePhysicalDisconnectGate();
        REQUIRE(composite);
        host.SetPhysicalDisconnectGate(composite);
        anchor = MUXC::Button();
        anchor.Content(winrt::box_value(L"Native popup anchor"));
        REQUIRE(host.SetContent(anchor));
        if ( menu )
        {
            menuFlyout = MUXC::MenuFlyout();
            menuItem = MUXC::MenuFlyoutItem();
            menuItem.Text(L"Retained menu item");
            menuFlyout.Items().Append(menuItem);
            flyout = menuFlyout;
            exactItem = menuItem;
        }
        else
        {
            contentFlyout = MUXC::Flyout();
            content = MUXC::StackPanel();
            content.Width(160);
            contentButton = MUXC::Button();
            contentButton.Content(winrt::box_value(L"Retained flyout content"));
            content.Children().Append(contentButton);
            contentFlyout.Content(content);
            flyout = contentFlyout;
            exactItem = contentButton;
        }
        frame->Show();
        REQUIRE(WaitForFlyoutRetirement([&]()
        {
            if ( auto *tlw = wxWinUITopLevelHost::FindSlotOwner(owner) ) tlw->FlushSync();
            return anchor.IsLoaded() && anchor.XamlRoot() &&
                   anchor.ActualWidth() > 0 && anchor.ActualHeight() > 0;
        }));
    }

    void CheckContentRetained() const
    {
        if ( menuFlyout )
        {
            REQUIRE(menuFlyout.Items().Size() == 1);
            CHECK(menuFlyout.Items().GetAt(0) == menuItem);
            CHECK(menuItem.Text() == L"Retained menu item");
        }
        else
        {
            CHECK(contentFlyout.Content() == content);
            REQUIRE(content.Children().Size() == 1);
            CHECK(content.Children().GetAt(0) == contentButton);
        }
    }

    wxWeakRef<wxFrame> frame;
    wxWindow *owner = nullptr;
    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> composite;
    MUXC::Button anchor{nullptr};
    MUXCP::FlyoutBase flyout{nullptr};
    MUXC::Flyout contentFlyout{nullptr};
    MUXC::StackPanel content{nullptr};
    MUXC::Button contentButton{nullptr};
    MUXC::MenuFlyout menuFlyout{nullptr};
    MUXC::MenuFlyoutItem menuItem{nullptr};
    MUX::DependencyObject exactItem{nullptr};
};

struct FlyoutObservation
{
    unsigned opened = 0;
    unsigned closed = 0;
    unsigned completed = 0;
    unsigned observed = 0;
    bool satisfiedInsideClosed = false;
    bool completedInsideClosed = false;
};
} // namespace

TEST_CASE("WinUIFlyoutRetirement::RealFlyoutAndMenuFlyoutCloseBeforeCompletion",
          "[winui-flyout-retirement][winui-popup-retirement][winui-native]")
{
    const unsigned mode = GENERATE(0u, 1u, 2u);
    const bool menu = mode != 0;
    const bool transient = mode == 2;
    CAPTURE(menu, transient);
    FlyoutRetirementFixture fixture;
    fixture.Create(menu);
    if ( transient )
    {
        fixture.owner->SetFocus();
        REQUIRE(fixture.anchor.Focus(MUX::FocusState::Programmatic));
        REQUIRE(MUX::Input::FocusManager::GetFocusedElement(
            fixture.anchor.XamlRoot()) == fixture.anchor);
    }
    std::vector<std::shared_ptr<wxWinUIFlyoutPopupRetirementState>> sessions;
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> gates;
    std::vector<std::shared_ptr<FlyoutObservation>> observations;

    for ( unsigned round = 0; round != 2; ++round )
    {
        CAPTURE(round);
        const auto state = wxWinUICreateFlyoutPopupRetirementState(
            fixture.flyout, fixture.anchor.XamlRoot(), fixture.exactItem);
        REQUIRE(state);
        const auto gate = wxWinUIGetFlyoutPopupRetirementGate(state);
        REQUIRE(gate);
        REQUIRE_FALSE(gate->IsDegraded());
        CHECK_FALSE(gate->IsSatisfied());
        if ( !sessions.empty() )
        {
            CHECK(state != sessions.back());
            CHECK(gate != gates.back());
            CHECK(gates.back()->IsSatisfied());
        }
        // Retain the real gate in the host fan-in even if an assertion aborts
        // before closure. Reusable leaves permit observing their completion
        // here; host retirement can later retarget the same causal gate.
        REQUIRE(wxWinUIAddCompositePhysicalDisconnectGate(fixture.composite, gate, true));
        const auto observed = std::make_shared<FlyoutObservation>();
        REQUIRE(wxWinUISetFlyoutPopupRetirementObserver(state,
            [observed]() { ++observed->observed; }));
        REQUIRE(gate->SetCompletion([observed]() { ++observed->completed; }));
        sessions.push_back(state);
        gates.push_back(gate);
        observations.push_back(observed);

        const auto openedToken = fixture.flyout.Opened(
            [observed](const auto&, const auto&) { ++observed->opened; });
        const auto closedToken = fixture.flyout.Closed(
            [observed, gate](const auto&, const auto&)
            {
                ++observed->closed;
                observed->satisfiedInsideClosed = gate->IsSatisfied();
                observed->completedInsideClosed = observed->completed != 0;
            });
        const auto revoke = wxMakeGuard([&]()
        {
            try
            {
                fixture.flyout.Opened(openedToken);
                fixture.flyout.Closed(closedToken);
            }
            catch ( const winrt::hresult_error& ) { }
        });
        if ( transient )
        {
            MUXCP::FlyoutShowOptions options;
            options.Placement(MUXCP::FlyoutPlacementMode::Bottom);
            options.ShowMode(MUXCP::FlyoutShowMode::Transient);
            fixture.flyout.ShowAt(fixture.anchor, options);
        }
        else
            fixture.flyout.ShowAt(fixture.anchor);
        // Match production: resolve in the same stack as ShowAt, before any
        // dispatcher turn or wait for Opened can hide pending publication.
        wxWinUIResolveFlyoutPopupAfterShow(state);
        REQUIRE_FALSE(gate->IsDegraded());
        CHECK_FALSE(gate->IsSatisfied());
        CHECK(observed->completed == 0);
        REQUIRE(WaitForFlyoutRetirement([&]() { return observed->opened == 1; }));
        // Repeated resolution of this exact, now-open session is idempotent.
        wxWinUIResolveFlyoutPopupAfterShow(state);
        wxWinUIResolveFlyoutPopupAfterShow(state);
        REQUIRE_FALSE(gate->IsDegraded());
        const auto popup = FindExactFlyoutPopup(fixture.anchor.XamlRoot(), fixture.exactItem);
        REQUIRE(popup);
        CHECK(popup.IsOpen());
        CHECK_FALSE(gate->IsSatisfied());
        CHECK_FALSE(fixture.composite->IsSatisfied());
        CHECK(observed->completed == 0);
        CHECK(observed->observed == 0);
        fixture.CheckContentRetained();

        fixture.flyout.Hide();
        REQUIRE(WaitForFlyoutRetirement([&]()
        {
            return observed->closed == 1 && observed->completed == 1 && observed->observed == 1;
        }));
        CHECK_FALSE(observed->satisfiedInsideClosed);
        CHECK_FALSE(observed->completedInsideClosed);
        CHECK_FALSE(popup.IsOpen());
        CHECK(gate->IsSatisfied());
        CHECK_FALSE(gate->IsDegraded());
        CHECK(fixture.composite->IsSatisfied());
        fixture.CheckContentRetained();
        for ( const auto& previous : observations )
        {
            CHECK(previous->opened == 1);
            CHECK(previous->closed == 1);
            CHECK(previous->completed == 1);
            CHECK(previous->observed == 1);
        }
    }
}

TEST_CASE("WinUIFlyoutRetirement::ImmediateHideRequiresExactClosingWitness",
          "[winui-flyout-retirement][winui-popup-retirement][winui-native]")
{
    const bool menu = GENERATE(false, true);
    CAPTURE(menu);
    FlyoutRetirementFixture fixture;
    fixture.Create(menu);
    fixture.owner->SetFocus();
    REQUIRE(fixture.anchor.Focus(MUX::FocusState::Programmatic));
    const auto state = wxWinUICreateFlyoutPopupRetirementState(
        fixture.flyout, fixture.anchor.XamlRoot(), fixture.exactItem);
    REQUIRE(state);
    const auto gate = wxWinUIGetFlyoutPopupRetirementGate(state);
    REQUIRE(gate);
    REQUIRE_FALSE(gate->IsDegraded());
    REQUIRE(wxWinUIAddCompositePhysicalDisconnectGate(fixture.composite, gate, true));
    struct ImmediateClose : FlyoutObservation
    {
        unsigned closing = 0;
        unsigned cancelledClosing = 0;
        unsigned acceptedClosing = 0;
        bool exactWitness = false;
        bool unsatisfiedAtClosing = true;
        bool degradedAtClosing = false;
        MUXCP::Popup popup{nullptr};
    };
    const auto observed = std::make_shared<ImmediateClose>();
    REQUIRE(gate->SetCompletion([observed]() { ++observed->completed; }));
    REQUIRE(wxWinUISetFlyoutPopupRetirementObserver(state,
        [observed]() { ++observed->observed; }));
    const auto root = fixture.anchor.XamlRoot();
    const auto exactItem = fixture.exactItem;
    const auto closingToken = fixture.flyout.Closing(
        [observed, gate, root, exactItem](const auto&, const auto& args)
        {
            ++observed->closing;
            observed->unsatisfiedAtClosing &= !gate->IsSatisfied();
            observed->degradedAtClosing |= gate->IsDegraded();
            if ( args.Cancel() )
            {
                ++observed->cancelledClosing;
                return;
            }
            ++observed->acceptedClosing;
            bool duplicate = false;
            observed->popup = nullptr;
            for ( const auto& popup : MUX::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root) )
            {
                if ( popup.IsOpen() && popup.Child() && IsWithinFlyout(exactItem, popup.Child()) )
                {
                    if ( observed->popup ) duplicate = true;
                    observed->popup = popup;
                }
            }
            observed->exactWitness = observed->popup && !duplicate;
        });
    const auto closedToken = fixture.flyout.Closed(
        [observed, gate](const auto&, const auto&)
        {
            ++observed->closed;
            observed->satisfiedInsideClosed = gate->IsSatisfied();
            observed->completedInsideClosed = observed->completed != 0;
        });
    const auto revoke = wxMakeGuard([&]()
    {
        try
        {
            fixture.flyout.Closing(closingToken);
            fixture.flyout.Closed(closedToken);
        }
        catch ( const winrt::hresult_error& ) { }
    });
    if ( menu )
    {
        MUXCP::FlyoutShowOptions options;
        options.Placement(MUXCP::FlyoutPlacementMode::Bottom);
        options.ShowMode(MUXCP::FlyoutShowMode::Transient);
        fixture.flyout.ShowAt(fixture.anchor, options);
    }
    else
        fixture.flyout.ShowAt(fixture.anchor);
    wxWinUIResolveFlyoutPopupAfterShow(state);
    fixture.flyout.Hide();
    // No dispatch or Opened wait intervenes between ShowAt and Hide.
    REQUIRE_FALSE(gate->IsDegraded());
    CHECK_FALSE(gate->IsSatisfied());
    CHECK(observed->completed == 0);
    CHECK(observed->observed == 0);
    REQUIRE(WaitForFlyoutRetirement([&]()
    {
        return observed->closed == 1 && observed->completed == 1 && observed->observed == 1;
    }));
    CHECK(observed->closing == observed->cancelledClosing + 1);
    CHECK(observed->acceptedClosing == 1);
    CHECK(observed->exactWitness);
    CHECK(observed->unsatisfiedAtClosing);
    CHECK_FALSE(observed->degradedAtClosing);
    CHECK_FALSE(observed->satisfiedInsideClosed);
    CHECK_FALSE(observed->completedInsideClosed);
    REQUIRE(observed->popup);
    CHECK_FALSE(observed->popup.IsOpen());
    CHECK(gate->IsSatisfied());
    CHECK_FALSE(gate->IsDegraded());
    CHECK(fixture.composite->IsSatisfied());
    fixture.CheckContentRetained();
}

TEST_CASE("WinUIFlyoutRetirement::DestroyBeforeOpenedRetainsTerminalDebt",
          "[winui-flyout-retirement][winui-popup-retirement][winui-native]")
{
    const bool menu = GENERATE(false, true);
    CAPTURE(menu);
    REQUIRE_FALSE(wxWinUIPhysicalDisconnectTestAccess::IsPublicationPoisoned());
    FlyoutRetirementFixture fixture;
    fixture.Create(menu);
    fixture.owner->SetFocus();
    REQUIRE(fixture.anchor.Focus(MUX::FocusState::Programmatic));
    const unsigned terminalsBefore = wxWinUIHostObservationTestAccess::
        GetPendingTLWTerminalRetirementCount();
    const auto state = wxWinUICreateFlyoutPopupRetirementState(
        fixture.flyout, fixture.anchor.XamlRoot(), fixture.exactItem);
    REQUIRE(state);
    const auto gate = wxWinUIGetFlyoutPopupRetirementGate(state);
    REQUIRE(gate);
    REQUIRE(gate->GetDispatcherQueue());
    REQUIRE(wxWinUIAddCompositePhysicalDisconnectGate(fixture.composite, gate, true));
    struct TerminalClose : FlyoutObservation
    {
        bool sealedInsideClosed = false;
        bool degradedInsideClosed = false;
        bool cancelledClosing = false;
    };
    const auto observed = std::make_shared<TerminalClose>();
    REQUIRE(gate->SetCompletion([observed]() { ++observed->completed; }));
    REQUIRE(wxWinUISetFlyoutPopupRetirementObserver(state,
        [observed]() { ++observed->observed; }));
    const auto openedToken = fixture.flyout.Opened(
        [observed](const auto&, const auto&) { ++observed->opened; });
    const auto closingToken = fixture.flyout.Closing(
        [observed](const auto&, const auto& args)
        {
            observed->cancelledClosing |= args.Cancel();
        });
    const auto closedToken = fixture.flyout.Closed(
        [observed, gate](const auto&, const auto&)
        {
            ++observed->closed;
            observed->sealedInsideClosed = gate->IsSealedForHostShutdown();
            observed->degradedInsideClosed = gate->IsDegraded();
            observed->satisfiedInsideClosed = gate->IsSatisfied();
            observed->completedInsideClosed = observed->completed != 0;
        });
    const auto revoke = wxMakeGuard([&]()
    {
        try
        {
            fixture.flyout.Opened(openedToken);
            fixture.flyout.Closing(closingToken);
            fixture.flyout.Closed(closedToken);
        }
        catch ( const winrt::hresult_error& ) { }
    });
    if ( menu )
    {
        MUXCP::FlyoutShowOptions options;
        options.ShowMode(MUXCP::FlyoutShowMode::Transient);
        fixture.flyout.ShowAt(fixture.anchor, options);
    }
    else
        fixture.flyout.ShowAt(fixture.anchor);
    wxWinUIResolveFlyoutPopupAfterShow(state);
    REQUIRE(observed->opened == 0);
    REQUIRE_FALSE(FindExactFlyoutPopup(fixture.anchor.XamlRoot(), fixture.exactItem));
    REQUIRE_FALSE(gate->IsDegraded());

    // Synchronous wx destruction exercises the real host Source.Close before
    // a dispatcher turn can publish Opened. No fake terminal receipt is used.
    delete fixture.frame.get();
    fixture.owner = nullptr;
    REQUIRE_FALSE(fixture.frame);
    REQUIRE(WaitForFlyoutRetirement([&]() { return observed->closed == 1; }));
    CHECK(observed->sealedInsideClosed);
    CHECK_FALSE(observed->degradedInsideClosed);
    CHECK_FALSE(observed->cancelledClosing);
    CHECK_FALSE(observed->satisfiedInsideClosed);
    CHECK_FALSE(observed->completedInsideClosed);
    CHECK(observed->completed == 0);
    CHECK(observed->observed == 0);
    CHECK(gate->IsSealedForHostShutdown());
    CHECK_FALSE(gate->IsSatisfied());
    CHECK_FALSE(gate->IsDegraded());
    CHECK_FALSE(wxWinUIPhysicalDisconnectTestAccess::IsPublicationPoisoned());
    CHECK(gate->GetDispatcherQueue());
    CHECK(wxWinUIHostObservationTestAccess::GetPendingTLWTerminalRetirementCount() ==
          terminalsBefore + 1);
    fixture.CheckContentRetained();
}

TEST_CASE("WinUIFlyoutRetirement::EmptyAndPrunedFanInRetainQueueThroughHostShutdown",
          "[winui-flyout-retirement][winui-popup-retirement][winui-native]")
{
    const bool retireTransientFirst = GENERATE(false, true);
    CAPTURE(retireTransientFirst);
    FlyoutRetirementFixture fixture;
    fixture.Create(false);
    const auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    REQUIRE(queue);
    REQUIRE(fixture.composite->GetDispatcherQueue() == queue);
    REQUIRE(fixture.composite->IsSatisfied());

    const MUXC::StackPanel root;
    const MUXC::ContentControl mount;
    const MUXC::Button anchor;
    anchor.Content(winrt::box_value(L"empty fan-in anchor"));
    anchor.Width(140);
    anchor.Height(36);
    root.Children().Append(anchor);
    root.Children().Append(mount);
    REQUIRE(fixture.host.SetContent(root, anchor));
    fixture.anchor = anchor;
    auto* const child = new wxWindow(fixture.owner, wxID_ANY,
                                     wxPoint(0, 0), wxSize(8, 8));
    wxWinUIControlHost childHost;
    const auto childGate = wxWinUICreateCompositePhysicalDisconnectGate();
    REQUIRE(childGate);
    REQUIRE(childGate->GetDispatcherQueue() == queue);
    REQUIRE(childHost.Initialize(child));
    childHost.SetPhysicalDisconnectGate(childGate);
    const MUXC::Button command;
    command.Content(winrt::box_value(L"composite child"));
    REQUIRE(childHost.SetContentInParent(command, command, fixture.owner, mount));
    fixture.host.ForceRender();
    REQUIRE(WaitForFlyoutRetirement([&]()
    {
        return fixture.anchor.IsLoaded() && command.IsLoaded();
    }));
    if ( retireTransientFirst )
    {
        const auto state = wxWinUICreateFlyoutPopupRetirementState(
            fixture.flyout, fixture.anchor.XamlRoot(), fixture.exactItem);
        REQUIRE(state);
        const auto gate = wxWinUIGetFlyoutPopupRetirementGate(state);
        REQUIRE(gate);
        // Unlike the reusable leaves in the other fixture, this real popup
        // removes its own transient fan-in entry at causal completion.
        REQUIRE(wxWinUIAddCompositePhysicalDisconnectGate(fixture.composite, gate));
        fixture.flyout.ShowAt(fixture.anchor);
        wxWinUIResolveFlyoutPopupAfterShow(state);
        REQUIRE(WaitForFlyoutRetirement([&]()
        {
            return bool(FindExactFlyoutPopup(fixture.anchor.XamlRoot(), fixture.exactItem));
        }));
        CHECK_FALSE(fixture.composite->IsSatisfied());
        fixture.flyout.Hide();
        REQUIRE(WaitForFlyoutRetirement([&]()
        {
            return gate->IsSatisfied() && fixture.composite->IsSatisfied();
        }));
        CHECK_FALSE(gate->IsDegraded());
        CHECK(fixture.composite->GetDispatcherQueue() == queue);
    }
    const unsigned terminalsBefore = wxWinUIHostObservationTestAccess::
        GetPendingTLWTerminalRetirementCount();
    delete fixture.frame.get();
    fixture.owner = nullptr;
    REQUIRE_FALSE(fixture.frame);
    CHECK(fixture.composite->IsSealedForHostShutdown());
    CHECK(childGate->IsSealedForHostShutdown());
    CHECK_FALSE(fixture.composite->IsSatisfied());
    CHECK_FALSE(childGate->IsSatisfied());
    CHECK_FALSE(fixture.composite->IsDegraded());
    CHECK_FALSE(childGate->IsDegraded());
    CHECK_FALSE(wxWinUIPhysicalDisconnectTestAccess::IsPublicationPoisoned());
    CHECK(wxWinUIHostObservationTestAccess::GetPendingTLWTerminalRetirementCount() ==
          terminalsBefore);
}

#endif // wxWinUI, intentionally independent of wxUSE_CHOICE
