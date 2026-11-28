/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicompositefocus.cpp
// Purpose:     Native focus ownership of nested shared-island slots
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
#include "wx/window.h"
#include "../../src/winui/private.h"
#include "wx/winui/private/tlwhost.h"
#include "host-content-test-access.h"
#include "host-focus-test-access.h"
#include "host-observation-test-access.h"
#include "physical-disconnect-test-access.h"
#if wxUSE_NOTEBOOK
    #include "notebook-test-access.h"
#endif

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace
{

void DrainCompositeFocusDispatch(int rounds = 20)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

template <typename Predicate>
bool WaitForCompositeFocus(Predicate predicate)
{
    for ( int i = 0; i < 100; ++i )
    {
        if ( predicate() )
            return true;
        DrainCompositeFocusDispatch(1);
    }
    return predicate();
}

template <typename Predicate>
bool WaitForCompositeLoaded(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;

    // Loaded delivery needs native dispatcher progress, not just polling an
    // already-populated queue followed by sleeping. Keep the same 500ms budget
    // while allowing the active MSW loop to wait for the next native message.
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() )
            return true;
        if ( elapsed.Time() >= 500 )
            return false;

        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        const long remaining = 500 - elapsed.Time();
        if ( remaining <= 0 )
            return false;
        if ( loop->DispatchTimeout(
                 static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
        {
            return false;
        }
    }
}

std::string DescribeCompositeVisual(
    const char *name,
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::XamlRoot& expectedRoot)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    std::ostringstream text;
    text << name << " visual ancestry:\n";
    try
    {
        auto node = element.try_as<MUX::DependencyObject>();
        for ( unsigned depth = 0; node && depth < 24; ++depth )
        {
            text << depth << ": " << winrt::to_string(winrt::get_class_name(node));
            if ( const auto ui = node.try_as<MUX::UIElement>() )
            {
                const auto desired = ui.DesiredSize();
                text << " visibility=" << static_cast<int>(ui.Visibility())
                     << " desired=" << desired.Width << 'x' << desired.Height
                     << " root=" << static_cast<bool>(ui.XamlRoot())
                     << " expectedRoot=" << (ui.XamlRoot() == expectedRoot);
            }
            if ( const auto fe = node.try_as<MUX::FrameworkElement>() )
            {
                text << " loaded=" << fe.IsLoaded()
                     << " actual=" << fe.ActualWidth() << 'x' << fe.ActualHeight()
                     << " size=" << fe.Width() << 'x' << fe.Height();
            }
            text << '\n';
            node = MUX::Media::VisualTreeHelper::GetParent(node);
        }
    }
    catch ( const winrt::hresult_error& error )
    {
        text << "diagnostic HRESULT=" << error.code().value << '\n';
    }
    return text.str();
}

std::string DescribeCompositeSlot(
    wxWinUITopLevelHost& host,
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::XamlRoot& expectedRoot)
{
    std::ostringstream text;
    text << "wx slot owner=" << window
         << " shown=" << window->IsShownOnScreen()
         << " enabled=" << window->IsEnabled()
         << " frozen=" << window->IsFrozen();
    const wxSize size = window->GetSize();
    text << " size=" << size.x << 'x' << size.y << '\n';
    if ( const auto slot = host.FindSlot(window) )
    {
        std::shared_ptr<wxWinUISlotLifetime> lifetime;
        WXHWND hwnd = nullptr;
        unsigned long long hwndGeneration = 0;
        unsigned long long contentGeneration = 0;
        unsigned long long authorityGeneration = 0;
        bool accessible = false;
        const bool validIdentity = host.TryGetAccessibilityShellState(
            window, &lifetime, &hwnd, &hwndGeneration, &contentGeneration,
            &accessible, &authorityGeneration);
        const wxRect rect = slot->GetRectInTLW();
        text << "identity=" << validIdentity
             << " hwndGeneration=" << hwndGeneration
             << " contentGeneration=" << contentGeneration
             << " syncEpoch=" << wxWinUISlotTestAccess::GetSyncEpoch(*slot)
             << " transaction="
             << wxWinUISlotTestAccess::IsContentTransactionInProgress(*slot)
             << " slotLoaded=" << slot->IsContentLoaded()
             << " loadedObserver="
             << wxWinUISlotTestAccess::HasContentLoadedObserver(*slot)
             << " observerPending="
             << wxWinUISlotTestAccess::IsContentLoadedObserverPending(*slot)
             << " rect=" << rect.x << ',' << rect.y << ','
             << rect.width << ',' << rect.height << '\n';
        text << DescribeCompositeVisual("container", slot->GetContainer(),
                                        expectedRoot);
        text << DescribeCompositeVisual("content", slot->GetVisualRoot(),
                                        expectedRoot);
    }
    else
    {
        text << "slot absent\n";
    }
    return text.str();
}

struct CompositeGateObservation
{
    unsigned completionInstalls = 0;
    unsigned completionCalls = 0;
};

// Observe the existing synthetic gate's real completion destination without
// changing its causal state machine or introducing a host-side test bypass.
class ObservedCompositeDisconnectGate final
    : public wxWinUIPhysicalDisconnectGate
{
public:
    ObservedCompositeDisconnectGate(
        std::shared_ptr<wxWinUIPhysicalDisconnectGate> gate,
        std::shared_ptr<CompositeGateObservation> observation)
        : m_gate(std::move(gate)), m_observation(std::move(observation))
    {
    }

    bool IsSatisfied() const noexcept override
        { return m_gate->IsSatisfied(); }
    bool IsDegraded() const noexcept override
        { return m_gate->IsDegraded(); }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_gate->IsSealedForHostShutdown(); }
    winrt::Microsoft::UI::Dispatching::DispatcherQueue
    GetDispatcherQueue() const noexcept override
        { return m_gate->GetDispatcherQueue(); }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        ++m_observation->completionInstalls;
        try
        {
            return m_gate->SetCompletion(
                [observation = m_observation,
                 complete = std::move(complete)]()
                {
                    ++observation->completionCalls;
                    complete();
                });
        }
        catch ( ... )
        {
            return false;
        }
    }

    void SealForHostShutdown() noexcept override
        { m_gate->SealForHostShutdown(); }
    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
        { m_gate->LatchPublishedPopupDebtBeforeSourceClose(); }
    bool CompleteSealedWithoutPopup() noexcept override
        { return m_gate->CompleteSealedWithoutPopup(); }
    void CompleteAtXamlBoundary() noexcept override
        { m_gate->CompleteAtXamlBoundary(); }

private:
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> m_gate;
    const std::shared_ptr<CompositeGateObservation> m_observation;
};

} // namespace

TEST_CASE("HostFocus::CompositeSlotsResolveInnermostOwner",
          "[HostFocus][HostLifecycle][winui-composite]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = MUX::Controls;

    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "composite-focus",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainCompositeFocusDispatch();
    });

    // Match the slot map's key ordering, independent of allocator behaviour:
    // an ancestor-first map scan must fail for both nested owners below.
    std::array<wxWindow *, 3> windows = {
        new wxWindow(frame, wxID_ANY),
        new wxWindow(frame, wxID_ANY),
        new wxWindow(frame, wxID_ANY)
    };
    std::sort(windows.begin(), windows.end(), std::less<wxWindow *>());
    REQUIRE(windows[1]->Reparent(windows[0]));
    REQUIRE(windows[2]->Reparent(windows[1]));
    windows[0]->SetSize(5, 5, 380, 200);
    windows[1]->SetSize(0, 0, 360, 120);
    windows[2]->SetSize(0, 0, 340, 40);

    const MUXC::StackPanel outerRoot;
    const MUXC::StackPanel middleRoot;
    const MUXC::ContentControl middleMount;
    const MUXC::ContentControl innerMount;
    const std::array<MUXC::Button, 3> buttons = {
        MUXC::Button(), MUXC::Button(), MUXC::Button()
    };
    for ( const auto& button : buttons )
    {
        button.Width(120);
        button.Height(32);
    }
    outerRoot.Children().Append(buttons[0]);
    outerRoot.Children().Append(middleMount);
    middleRoot.Children().Append(buttons[1]);
    middleRoot.Children().Append(innerMount);

    // These are the real production proxies, not a manually rearranged XAML
    // tree. Their destruction precedes the native owners' deferred deletion.
    wxWinUIControlHost outerHost;
    wxWinUIControlHost middleHost;
    wxWinUIControlHost innerHost;
    REQUIRE(outerHost.Initialize(windows[0]));
    REQUIRE(middleHost.Initialize(windows[1]));
    REQUIRE(innerHost.Initialize(windows[2]));
    REQUIRE(outerHost.SetContent(outerRoot, buttons[0]));
    REQUIRE(middleHost.SetContentInParent(
        middleRoot, buttons[1], windows[0], middleMount));
    REQUIRE(innerHost.SetContentInParent(
        buttons[2], buttons[2], windows[1], innerMount));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(windows[0]);
    REQUIRE(host != nullptr);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(windows[1]) == host);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(windows[2]) == host);
    wxWinUISlot * const middleSlot = host->FindSlot(windows[1]);
    wxWinUISlot * const innerSlot = host->FindSlot(windows[2]);
    REQUIRE(middleSlot != nullptr);
    REQUIRE(innerSlot != nullptr);
    CHECK(middleMount.Content().try_as<MUXC::Grid>() ==
          middleSlot->GetContainer());
    CHECK(innerMount.Content().try_as<MUXC::Grid>() ==
          innerSlot->GetContainer());

    frame->Show();
    host->FlushSync();
    REQUIRE(WaitForCompositeFocus([&]()
    {
        return outerRoot.IsLoaded() && middleRoot.IsLoaded() &&
               buttons[2].IsLoaded();
    }));
    const auto root = outerRoot.XamlRoot();
    REQUIRE(root != nullptr);

    const auto checkFocus = [&](const MUXC::Button& button, wxWindow *owner)
    {
        REQUIRE(button.Focus(MUX::FocusState::Programmatic));
        REQUIRE(WaitForCompositeFocus([&]()
        {
            return MUX::Input::FocusManager::GetFocusedElement(root)
                       .try_as<MUXC::Button>() == button;
        }));
        CHECK(wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) == owner);
    };

    checkFocus(buttons[2], windows[2]);
    checkFocus(buttons[1], windows[1]);
    checkFocus(buttons[0], windows[0]);
    checkFocus(buttons[2], windows[2]);

    // The same logical slot's replacement publication must resolve through
    // its current container, without caching the previous focused element.
    const MUXC::Button replacement;
    replacement.Width(120);
    replacement.Height(32);
    REQUIRE(innerHost.SetContentInParent(
        replacement, replacement, windows[1], innerMount));
    REQUIRE(host->FindSlot(windows[2]) == innerSlot);
    REQUIRE(innerSlot->GetVisualRoot() == replacement);
    MUX::UIElement carrierContent{nullptr};
    REQUIRE(innerSlot->TryGetCarrierContent(carrierContent));
    REQUIRE(carrierContent == replacement);
    REQUIRE(innerMount.Content().try_as<MUXC::Grid>() ==
            innerSlot->GetContainer());

    // FlushSync publishes wx state but does not run XAML measure/arrange.
    // This offscreen focus fixture must realize its replacement through the
    // existing synchronous layout contract, not depend on a composition tick.
    innerHost.ForceRender();
    const bool replacementLoaded = WaitForCompositeLoaded(
        [&]() { return replacement.IsLoaded(); });
    std::string loadDiagnostic;
    if ( !replacementLoaded )
    {
        std::ostringstream text;
        text << "frame shown=" << frame->IsShownOnScreen()
             << " iconized=" << frame->IsIconized()
             << " frozen=" << frame->IsFrozen() << '\n';
        for ( wxWindow * const window : windows )
            text << DescribeCompositeSlot(*host, window, root);
        text << DescribeCompositeVisual("old button", buttons[2], root);
        text << DescribeCompositeVisual("replacement", replacement, root);
        loadDiagnostic = text.str();
    }
    INFO(loadDiagnostic);
    REQUIRE(replacementLoaded);
    checkFocus(replacement, windows[2]);
    REQUIRE(host->FindSlot(windows[2]) != nullptr);
    CHECK(host->FindSlot(windows[2])->GetVisualRoot() == replacement);

    innerHost.Close();
    REQUIRE(host->FindSlot(windows[2]) == nullptr);
    checkFocus(buttons[1], windows[1]);
    checkFocus(buttons[0], windows[0]);
}

TEST_CASE("HostFocus::CompositeMountRejectsForeignOrOccupiedTargets",
          "[HostFocus][HostLifecycle][winui-composite]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "composite-mount-rejection",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainCompositeFocusDispatch();
    });
    wxWindow * const parentWindow = new wxWindow(frame, wxID_ANY);
    wxWindow * const firstWindow = new wxWindow(parentWindow, wxID_ANY);
    wxWindow * const secondWindow = new wxWindow(parentWindow, wxID_ANY);
    wxWindow * const unrelatedWindow = new wxWindow(frame, wxID_ANY);

    const MUXC::StackPanel parentRoot;
    const MUXC::ContentControl emptyMount;
    const MUXC::ContentControl occupiedMount;
    const MUXC::ContentControl foreignMount;
    const MUXC::TextBlock applicationContent;
    applicationContent.Text(L"application-owned content");
    occupiedMount.Content(applicationContent);
    parentRoot.Children().Append(emptyMount);
    parentRoot.Children().Append(occupiedMount);

    wxWinUIControlHost parentHost;
    wxWinUIControlHost firstHost;
    wxWinUIControlHost secondHost;
    wxWinUIControlHost unrelatedHost;
    REQUIRE(parentHost.Initialize(parentWindow));
    REQUIRE(firstHost.Initialize(firstWindow));
    REQUIRE(secondHost.Initialize(secondWindow));
    REQUIRE(unrelatedHost.Initialize(unrelatedWindow));
    REQUIRE(parentHost.SetContent(parentRoot));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(parentWindow);
    REQUIRE(host != nullptr);

    const auto checkUnpublished = [&](const wxWinUIControlHost& proxy,
                                      wxWindow *window)
    {
        CHECK(proxy.GetContent() == nullptr);
        CHECK(host->FindSlot(window) == nullptr);
        CHECK(wxWinUITopLevelHost::FindSlotOwner(window) == nullptr);
    };

    const MUXC::Button firstContent;
    REQUIRE_FALSE(firstHost.SetContentInParent(
        firstContent, firstContent, parentWindow, foreignMount));
    checkUnpublished(firstHost, firstWindow);
    CHECK(foreignMount.Content() == nullptr);
    CHECK(emptyMount.Content() == nullptr);

    REQUIRE_FALSE(firstHost.SetContentInParent(
        firstContent, firstContent, parentWindow, occupiedMount));
    checkUnpublished(firstHost, firstWindow);
    CHECK(occupiedMount.Content().try_as<MUXC::TextBlock>() ==
          applicationContent);

    // This mount belongs to the current parent's publication in the same
    // island, but that owner is not an ancestor of this native wx shell.
    const MUXC::Button unrelatedContent;
    REQUIRE_FALSE(unrelatedHost.SetContentInParent(
        unrelatedContent, unrelatedContent, parentWindow, emptyMount));
    checkUnpublished(unrelatedHost, unrelatedWindow);
    CHECK(emptyMount.Content() == nullptr);

    REQUIRE(firstHost.SetContentInParent(
        firstContent, firstContent, parentWindow, emptyMount));
    wxWinUISlot * const firstSlot = host->FindSlot(firstWindow);
    REQUIRE(firstSlot != nullptr);
    const auto firstContainer = firstSlot->GetContainer();
    REQUIRE(emptyMount.Content().try_as<MUXC::Grid>() == firstContainer);

    const MUXC::Button secondContent;
    REQUIRE_FALSE(secondHost.SetContentInParent(
        secondContent, secondContent, parentWindow, emptyMount));
    checkUnpublished(secondHost, secondWindow);
    CHECK(host->FindSlot(firstWindow) == firstSlot);
    CHECK(firstSlot->GetVisualRoot() == firstContent);
    CHECK(firstHost.GetContent() == firstContent);
    CHECK(emptyMount.Content().try_as<MUXC::Grid>() == firstContainer);
    CHECK(occupiedMount.Content().try_as<MUXC::TextBlock>() ==
          applicationContent);
    CHECK(parentHost.GetContent() == parentRoot);

    // Reusing a mount by its exact current child is a content transaction,
    // unlike a competing child attempting to steal that same mount.
    const MUXC::Button replacement;
    REQUIRE(firstHost.SetContentInParent(
        replacement, replacement, parentWindow, emptyMount));
    REQUIRE(host->FindSlot(firstWindow) == firstSlot);
    CHECK(firstSlot->GetVisualRoot() == replacement);
    CHECK(firstHost.GetContent() == replacement);
    CHECK(emptyMount.Content().try_as<MUXC::Grid>() == firstContainer);
    CHECK(occupiedMount.Content().try_as<MUXC::TextBlock>() ==
          applicationContent);
}

TEST_CASE("HostLifecycle::CompositeParentWaitsForDescendantPhysicalReceipt",
          "[HostLifecycle][winui-composite][popup-gate][causal]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned pending0 = wxWinUIHostObservationTestAccess::
        GetPendingPhysicalDisconnectGateCount();
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "composite-parent-retirement",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainCompositeFocusDispatch();
    });
    wxWindow * const outerWindow = new wxWindow(
        frame, wxID_ANY, wxPoint(5, 5), wxSize(380, 200));
    wxWindow * const middleWindow = new wxWindow(
        outerWindow, wxID_ANY, wxPoint(0, 0), wxSize(360, 120));
    wxWindow * const innerWindow = new wxWindow(
        middleWindow, wxID_ANY, wxPoint(0, 0), wxSize(340, 40));

    const MUXC::StackPanel outerRoot;
    const MUXC::StackPanel middleRoot;
    const MUXC::ContentControl middleMount;
    const MUXC::ContentControl innerMount;
    const MUXC::Button innerContent;
    outerRoot.Children().Append(middleMount);
    middleRoot.Children().Append(innerMount);

    wxWinUIControlHost outerHost;
    wxWinUIControlHost middleHost;
    wxWinUIControlHost innerHost;
    REQUIRE(outerHost.Initialize(outerWindow));
    REQUIRE(middleHost.Initialize(middleWindow));
    REQUIRE(innerHost.Initialize(innerWindow));
    REQUIRE(outerHost.SetContent(outerRoot));
    REQUIRE(middleHost.SetContentInParent(
        middleRoot, middleRoot, outerWindow, middleMount));
    REQUIRE(innerHost.SetContentInParent(
        innerContent, innerContent, middleWindow, innerMount));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(outerWindow);
    REQUIRE(host != nullptr);
    const std::array<wxWindow *, 3> owners = {
        outerWindow, middleWindow, innerWindow
    };
    std::array<MUXC::Grid, 3> containers = {
        MUXC::Grid{nullptr}, MUXC::Grid{nullptr}, MUXC::Grid{nullptr}
    };
    std::array<MUXC::ContentControl, 3> carriers = {
        MUXC::ContentControl{nullptr}, MUXC::ContentControl{nullptr},
        MUXC::ContentControl{nullptr}
    };
    std::array<std::weak_ptr<wxWinUISlotLifetime>, 3> lifetimes;
    for ( std::size_t i = 0; i < owners.size(); ++i )
    {
        wxWinUISlot * const slot = host->FindSlot(owners[i]);
        REQUIRE(slot != nullptr);
        lifetimes[i] = wxWinUISlotTestAccess::GetLifetime(*slot);
        containers[i] = slot->GetContainer();
        REQUIRE(containers[i].Children().Size() == 1);
        carriers[i] = containers[i].Children().GetAt(0)
                          .as<MUXC::ContentControl>();
    }
    const auto rootCanvas = containers[0].Parent().try_as<MUXC::Canvas>();
    REQUIRE(rootCanvas != nullptr);

    frame->Show();
    host->FlushSync();
    REQUIRE(WaitForCompositeFocus([&]()
    {
        return outerRoot.IsLoaded() && middleRoot.IsLoaded() &&
               innerContent.IsLoaded();
    }));

    const auto syntheticGate = wxWinUIPhysicalDisconnectTestAccess::CreateGate(
        innerContent.DispatcherQueue(), false);
    REQUIRE(syntheticGate != nullptr);
    const auto finishGate = wxMakeGuard([syntheticGate]()
    {
        wxWinUIPhysicalDisconnectTestAccess::SetGateState(syntheticGate, true);
    });
    const auto observation = std::make_shared<CompositeGateObservation>();
    const auto observedGate = std::make_shared<ObservedCompositeDisconnectGate>(
        syntheticGate, observation);
    innerHost.SetPhysicalDisconnectGate(observedGate);
    REQUIRE(wxWinUISlotTestAccess::GetPhysicalDisconnectGateIdentity(
                *host->FindSlot(innerWindow)) ==
            reinterpret_cast<std::uintptr_t>(observedGate.get()));

    // Retire the leaf first: it is already absent from the live slot map when
    // the outer owner later collects its two-level physical subtree.
    innerHost.Close();
    REQUIRE(host->FindSlot(innerWindow) == nullptr);
    REQUIRE(observation->completionInstalls == 1);
    REQUIRE(observation->completionCalls == 0);
    REQUIRE(innerMount.Content().try_as<MUXC::Grid>() == containers[2]);

    outerHost.Close();
    for ( std::size_t i = 0; i < owners.size(); ++i )
    {
        CHECK(host->FindSlot(owners[i]) == nullptr);
        CHECK(wxWinUITopLevelHost::FindSlotOwner(owners[i]) == nullptr);
        const auto lifetime = lifetimes[i].lock();
        CHECK((!lifetime || !lifetime->IsValid()));
    }

    // Dispatcher progress is not the missing causal receipt. Neither parent
    // may detach its carrier or take over the leaf gate's completion callback.
    DrainCompositeFocusDispatch();
    CHECK(observation->completionInstalls == 1);
    CHECK(observation->completionCalls == 0);
    CHECK_FALSE(syntheticGate->IsSatisfied());
    CHECK(carriers[0].Content().try_as<MUXC::StackPanel>() == outerRoot);
    CHECK(carriers[1].Content().try_as<MUXC::StackPanel>() == middleRoot);
    CHECK(carriers[2].Content().try_as<MUXC::Button>() == innerContent);
    CHECK(middleMount.Content().try_as<MUXC::Grid>() == containers[1]);
    CHECK(innerMount.Content().try_as<MUXC::Grid>() == containers[2]);
    uint32_t index = 0;
    CHECK(rootCanvas.Children().IndexOf(containers[0], index));
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingPhysicalDisconnectGateCount() > pending0);

    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(
        syntheticGate, true));
    REQUIRE(WaitForCompositeFocus([&]()
    {
        uint32_t ignored = 0;
        return !rootCanvas.Children().IndexOf(containers[0], ignored) &&
               !middleMount.Content() && !innerMount.Content() &&
               !carriers[0].Content() && !carriers[1].Content() &&
               !carriers[2].Content() &&
               wxWinUIHostObservationTestAccess::
                   GetPendingPhysicalDisconnectGateCount() == pending0;
    }));
    CHECK(observation->completionInstalls == 1);
    CHECK(observation->completionCalls == 1);
    for ( const auto& container : containers )
        CHECK(container.Children().Size() == 0);

    // Repeated completion is not a second detach and must not resurrect any
    // parent's callback destination on the already-consumed child gate.
    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(
        syntheticGate, true));
    CHECK(observation->completionInstalls == 1);
    CHECK(observation->completionCalls == 1);
}

TEST_CASE("HostLifecycle::CompositeShutdownRetiresThroughChildGate",
          "[HostLifecycle][winui-composite][popup-gate][terminal]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    DrainCompositeFocusDispatch();
    const unsigned pending0 = wxWinUIHostObservationTestAccess::
        GetPendingPhysicalDisconnectGateCount();
    const unsigned terminals0 = wxWinUIHostObservationTestAccess::
        GetPendingTLWTerminalRetirementCount();
#if wxUSE_NOTEBOOK
    // This existing exported accessor observes the process-wide ledger, not
    // Notebook-specific state. It also detects a dormant, unpromoted ticket.
    const size_t retirements0 =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();
#endif
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "composite-shutdown-retirement",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainCompositeFocusDispatch();
    });
    wxWindow * const parentWindow = new wxWindow(
        frame, wxID_ANY, wxPoint(5, 5), wxSize(380, 200));
    wxWindow * const childWindow = new wxWindow(
        parentWindow, wxID_ANY, wxPoint(0, 0), wxSize(340, 40));
    const MUXC::StackPanel parentRoot;
    const MUXC::ContentControl childMount;
    const MUXC::Button childContent;
    parentRoot.Children().Append(childMount);

    wxWinUIControlHost parentHost;
    wxWinUIControlHost childHost;
    REQUIRE(parentHost.Initialize(parentWindow));
    REQUIRE(childHost.Initialize(childWindow));
    REQUIRE(parentHost.SetContent(parentRoot));
    REQUIRE(childHost.SetContentInParent(
        childContent, childContent, parentWindow, childMount));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(parentWindow);
    REQUIRE(host != nullptr);
    const auto hostLifetime = wxWinUIHostContentTestAccess::GetLifetime(*host);
    REQUIRE(host->FindSlot(childWindow) != nullptr);
    const auto childContainer = host->FindSlot(childWindow)->GetContainer();
    frame->Show();
    host->FlushSync();
    REQUIRE(WaitForCompositeFocus([&]() { return childContent.IsLoaded(); }));

    const auto syntheticGate = wxWinUIPhysicalDisconnectTestAccess::CreateGate(
        childContent.DispatcherQueue(), false);
    REQUIRE(syntheticGate != nullptr);
    const auto finishGate = wxMakeGuard([syntheticGate]()
    {
        wxWinUIPhysicalDisconnectTestAccess::SetGateState(syntheticGate, true);
    });
    const auto observation = std::make_shared<CompositeGateObservation>();
    const auto observedGate = std::make_shared<ObservedCompositeDisconnectGate>(
        syntheticGate, observation);
    childHost.SetPhysicalDisconnectGate(observedGate);

    childHost.Close();
    parentHost.Close();
    REQUIRE(host->FindSlot(parentWindow) == nullptr);
    REQUIRE(host->FindSlot(childWindow) == nullptr);
    REQUIRE(childMount.Content().try_as<MUXC::Grid>() == childContainer);
    REQUIRE(observation->completionInstalls == 1);
    REQUIRE(observation->completionCalls == 0);
    REQUIRE(wxWinUIHostObservationTestAccess::
                GetPendingPhysicalDisconnectGateCount() > pending0);

    // The graph ledger replaces the parent's fan-in before sealing. Once
    // Source.Close() retires the graph, individual mount-detach receipts can
    // no longer be required to complete that parent's obsolete wrapper.
    wxWinUIHostContentTestAccess::Shutdown(*host);
    CHECK(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(parentWindow) == nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(childWindow) == nullptr);
    {
        const auto lifetime = hostLifetime.lock();
        CHECK((!lifetime || lifetime->GetHost() == nullptr));
    }
    REQUIRE(observedGate->IsSealedForHostShutdown());
    CHECK(observation->completionInstalls == 2);
    CHECK(observation->completionCalls == 0);
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingPhysicalDisconnectGateCount() == pending0);
    REQUIRE(wxWinUIHostObservationTestAccess::
                GetPendingTLWTerminalRetirementCount() == terminals0 + 1);
#if wxUSE_NOTEBOOK
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          retirements0 + 1);
#endif

    DrainCompositeFocusDispatch();
    CHECK(observation->completionCalls == 0);
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingTLWTerminalRetirementCount() == terminals0 + 1);

    // Satisfy only the child's own gate: no fake physical receipt, synthetic
    // framework shutdown, or parent completion may be needed to finish.
    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(
        syntheticGate, true));
    REQUIRE(WaitForCompositeFocus([&]()
    {
        return wxWinUIHostObservationTestAccess::
                   GetPendingPhysicalDisconnectGateCount() == pending0 &&
               wxWinUIHostObservationTestAccess::
                   GetPendingTLWTerminalRetirementCount() == terminals0;
    }));
    DrainCompositeFocusDispatch();
    CHECK(observation->completionInstalls == 2);
    CHECK(observation->completionCalls == 1);
    CHECK_FALSE(observedGate->IsDegraded());
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingPhysicalDisconnectGateCount() == pending0);
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingTLWTerminalRetirementCount() == terminals0);
#if wxUSE_NOTEBOOK
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          retirements0);
#endif
}

#endif // defined(__WXWINUI__) && wxUSE_WINUI3
