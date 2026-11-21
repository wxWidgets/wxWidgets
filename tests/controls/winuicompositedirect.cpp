/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicompositedirect.cpp
// Purpose:     Direct native roots in composite Panel.Children collections
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/math.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/window.h"
#include "../../src/winui/private.h"
#include "wx/winui/private/tlwhost.h"
#include "host-content-test-access.h"
#include "host-focus-test-access.h"
#include "host-observation-test-access.h"
#include "physical-disconnect-test-access.h"

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;

void DrainDirectComposite(int rounds = 20)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

template <typename Predicate>
bool WaitForDirectComposite(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() )
            return true;
        if ( elapsed.Time() >= 500 )
            return false;
        // Native Loaded/focus delivery requires dispatcher progress. Merely
        // draining the populated queue and sleeping can starve an offscreen
        // tree even though its model and collection membership are committed.
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        const long remaining = 500 - elapsed.Time();
        if ( remaining <= 0 ||
             loop->DispatchTimeout(static_cast<unsigned long>(
                 std::min(5L, remaining))) == 0 )
            return false;
    }
}

struct DirectMountObservation
{
    unsigned inserts = 0;
    unsigned removals = 0;
    bool identityPublishedAtInsert = false;
    std::function<bool (const MUX::UIElement&)> inspectIdentity;
};

std::shared_ptr<wxWinUIDirectCompositeMount> MakeDirectPanelMount(
    const MUXC::Panel& panel,
    const std::shared_ptr<DirectMountObservation>& observation)
{
    const auto weakPanel = winrt::make_weak(panel);
    auto mount = std::make_shared<wxWinUIDirectCompositeMount>();
    mount->owner = winrt::make_weak(panel.as<MUX::FrameworkElement>());
    mount->insert = [weakPanel, observation](const MUX::UIElement& element)
    {
        const auto current = weakPanel.get();
        if ( !current || !element )
            return false;
        uint32_t index = 0;
        if ( current.Children().IndexOf(element, index) )
            return false;
        if ( observation->inspectIdentity )
        {
            observation->identityPublishedAtInsert =
                observation->inspectIdentity(element);
        }
        current.Children().Append(element);
        ++observation->inserts;
        return true;
    };
    mount->contains = [weakPanel](const MUX::UIElement& element)
    {
        const auto current = weakPanel.get();
        uint32_t index = 0;
        return current && element && current.Children().IndexOf(element, index);
    };
    mount->remove = [weakPanel, observation](const MUX::UIElement& element)
    {
        const auto current = weakPanel.get();
        uint32_t index = 0;
        if ( !current || !element || !current.Children().IndexOf(element, index) )
            return false;
        current.Children().RemoveAt(index);
        ++observation->removals;
        return true;
    };
    return mount;
}

wxRect ArrangedDirectRect(const wxWinUITopLevelHost& host,
                         const MUX::FrameworkElement& root)
{
    const auto bounds = root.TransformToVisual(host.Root()).TransformBounds(
        winrt::Windows::Foundation::Rect{
            0, 0, float(root.ActualWidth()), float(root.ActualHeight())});
    const double scale = host.GetScale();
    const int left = wxRound(bounds.X * scale);
    const int top = wxRound(bounds.Y * scale);
    return wxRect(left, top,
        wxRound((bounds.X + bounds.Width) * scale) - left,
        wxRound((bounds.Y + bounds.Height) * scale) - top);
}

class DirectCompositeMigrationFixture
{
public:
    ~DirectCompositeMigrationFixture()
    {
        leafHost.Close();
        directHost.Close();
        parentHost.Close();
        if ( source ) source->Destroy();
        if ( destination ) destination->Destroy();
        DrainDirectComposite();
    }

    void Create(bool activateSource = false)
    {
        source = new wxFrame(nullptr, wxID_ANY, "direct-migration-source",
                             wxPoint(-32000, -32000), wxSize(420, 260));
        destination = new wxFrame(nullptr, wxID_ANY, "direct-migration-target",
                                  wxPoint(-31500, -32000), wxSize(420, 260));
        windows = {
            new wxWindow(source, wxID_ANY, wxPoint(5, 5), wxSize(380, 200)),
            nullptr, nullptr
        };
        windows[1] = new wxWindow(windows[0], wxID_ANY,
                                  wxPoint(3, 3), wxSize(8, 8));
        windows[2] = new wxWindow(windows[1], wxID_ANY,
                                  wxPoint(2, 2), wxSize(6, 6));
        direct.Width(200);
        direct.Height(100);
        direct.HorizontalContentAlignment(MUX::HorizontalAlignment::Stretch);
        direct.VerticalContentAlignment(MUX::VerticalAlignment::Stretch);
        leaf.Width(120);
        leaf.Height(40);
        leafPanel.Children().Append(leafMount);
        direct.Content(leafPanel);
        REQUIRE(parentHost.Initialize(windows[0]));
        REQUIRE(directHost.Initialize(windows[1]));
        REQUIRE(leafHost.Initialize(windows[2]));
        REQUIRE(parentHost.SetContent(panel));
        mount = MakeDirectPanelMount(panel, observation);
        REQUIRE(directHost.SetContentInParentDirect(
            direct, direct, windows[0], mount));
        REQUIRE(leafHost.SetContentInParent(
            leaf, leaf, windows[1], leafMount));
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::FindSlotOwner(windows[0]);
        REQUIRE(host);
        for ( size_t i = 0; i < windows.size(); ++i )
        {
            slots[i] = host->FindSlot(windows[i]);
            REQUIRE(slots[i]);
            roots[i] = slots[i]->GetPresentationRoot();
            lifetimes[i] = wxWinUISlotTestAccess::GetLifetime(*slots[i]);
        }
        destination->ShowWithoutActivating();
        if ( activateSource )
            source->Show();
        else
            source->ShowWithoutActivating();
        host->FlushSync();
        REQUIRE(WaitForDirectComposite([&]() { return leaf.IsLoaded(); }));
        CheckOwnedBy(source);
    }

    void CheckOwnedBy(wxFrame *target) const
    {
        CHECK(windows[0]->GetParent() == target);
        CHECK(::GetParent(reinterpret_cast<HWND>(windows[0]->GetHWND())) ==
              reinterpret_cast<HWND>(target->GetHWND()));
        CHECK(windows[1]->GetParent() == windows[0]);
        CHECK(windows[2]->GetParent() == windows[1]);
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::FindForTLW(target);
        REQUIRE(host);
        wxFrame * const otherFrame = target == source ? destination : source;
        const auto other = otherFrame
            ? wxWinUITopLevelHost::FindForTLW(otherFrame) : nullptr;
        const std::array<MUX::UIElement, 3> content = {panel, direct, leaf};
        for ( size_t i = 0; i < windows.size(); ++i )
        {
            CHECK(wxWinUITopLevelHost::FindSlotOwner(windows[i]) == host);
            REQUIRE(host->FindSlot(windows[i]) == slots[i]);
            CHECK(slots[i]->GetPresentationRoot() == roots[i]);
            CHECK(slots[i]->GetVisualRoot() == content[i]);
            REQUIRE(lifetimes[i].lock());
            CHECK(lifetimes[i].lock()->IsValid());
            CHECK(wxWinUISlotTestAccess::GetLifetime(*slots[i]).lock() ==
                  lifetimes[i].lock());
            if ( other )
                CHECK(other->FindSlot(windows[i]) == nullptr);
        }
        REQUIRE(panel.Children().Size() == 1);
        CHECK(panel.Children().GetAt(0) == direct);
        CHECK(MUX::Media::VisualTreeHelper::GetParent(direct) == panel);
        CHECK(direct.Content().try_as<MUXC::StackPanel>() == leafPanel);
        REQUIRE(leafPanel.Children().Size() == 1);
        CHECK(leafPanel.Children().GetAt(0) == leafMount);
        CHECK(leafMount.Content().try_as<MUXC::Grid>() ==
              slots[2]->GetContainer());
        CHECK(parentHost.GetContent() == panel);
        CHECK(directHost.GetContent() == direct);
        CHECK(leafHost.GetContent() == leaf);
        CHECK(observation->inserts == 1);
        CHECK(observation->removals == 0);
        unsigned rootOccurrences = 0;
        for ( const auto& element : host->Root().Children() )
            rootOccurrences += element == roots[0];
        CHECK(rootOccurrences == 1);
        if ( other )
        {
            uint32_t ignored = 0;
            CHECK_FALSE(other->Root().Children().IndexOf(roots[0], ignored));
        }
    }

    void FocusLeaf()
    {
        source->Raise();
        const HWND nativeBefore = ::GetFocus();
        const HWND activeBefore = ::GetActiveWindow();
        windows[2]->SetFocus();
        REQUIRE(leaf.Focus(MUX::FocusState::Programmatic));
        const bool focused = WaitForDirectComposite([&]()
        {
            const auto host = wxWinUITopLevelHost::FindSlotOwner(windows[2]);
            return host && wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) ==
                           windows[2] &&
                   wxWindow::FindFocus() == windows[2] &&
                   MUX::Input::FocusManager::GetFocusedElement(leaf.XamlRoot())
                       .try_as<MUXC::Button>() == leaf;
        });
        std::string diagnostic;
        if ( !focused )
        {
            const auto host = wxWinUITopLevelHost::FindSlotOwner(windows[2]);
            const auto nativeElement =
                MUX::Input::FocusManager::GetFocusedElement(leaf.XamlRoot());
            std::ostringstream text;
            text << "migration initial focus timeout: active=" << source->IsActive()
                 << " shown=" << source->IsShownOnScreen()
                 << " nativeBefore=" << nativeBefore
                 << " activeBefore=" << activeBefore
                 << " nativeNow=" << ::GetFocus()
                 << " activeNow=" << ::GetActiveWindow()
                 << " foreground=" << ::GetForegroundWindow()
                 << " bridge=" << (host ? host->GetBridgeHwnd() : nullptr)
                 << " wxFocus=" << wxWindow::FindFocus()
                 << " slot=" << (host
                     ? wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) : nullptr)
                 << " expected=" << windows[2]
                 << " loaded=" << leaf.IsLoaded()
                 << " enabled=" << leaf.IsEnabled()
                 << " visibility=" << static_cast<int>(leaf.Visibility())
                 << " size=" << leaf.ActualWidth() << 'x' << leaf.ActualHeight()
                 << " nativeElement=" << (nativeElement
                     ? winrt::to_string(winrt::get_class_name(nativeElement)) : "null");
            diagnostic = text.str();
        }
        INFO(diagnostic);
        REQUIRE(focused);
    }

    wxFrame *source = nullptr;
    wxFrame *destination = nullptr;
    std::array<wxWindow *, 3> windows{};
    std::array<wxWinUISlot *, 3> slots{};
    std::array<MUX::FrameworkElement, 3> roots = {nullptr, nullptr, nullptr};
    std::array<std::weak_ptr<wxWinUISlotLifetime>, 3> lifetimes;
    const MUXC::StackPanel panel;
    const MUXC::ContentControl direct;
    const MUXC::StackPanel leafPanel;
    const MUXC::ContentControl leafMount;
    const MUXC::Button leaf;
    const std::shared_ptr<DirectMountObservation> observation =
        std::make_shared<DirectMountObservation>();
    std::shared_ptr<wxWinUIDirectCompositeMount> mount;
    wxWinUIControlHost parentHost;
    wxWinUIControlHost directHost;
    wxWinUIControlHost leafHost;
};

} // namespace

TEST_CASE("HostComposite::DirectRootOwnsStateGeometryAndFocus",
          "[HostFocus][HostGeometry][winui-composite][winui-composite-direct]")
{
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "direct-composite-state",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainDirectComposite();
    });
    wxWindow * const owner = new wxWindow(
        frame, wxID_ANY, wxPoint(5, 5), wxSize(380, 200));
    // Native composite layout, not this deliberately tiny wx shell, owns
    // the direct child's measured size and position.
    wxWindow * const child = new wxWindow(
        owner, wxID_ANY, wxPoint(3, 3), wxSize(8, 8));
    const MUXC::StackPanel panel;
    const MUXC::Button parentButton;
    parentButton.Width(140);
    parentButton.Height(36);
    panel.Children().Append(parentButton);
    const MUXC::Button button;
    button.Width(120);
    button.Height(40);
    wxWinUIControlHost parentHost;
    wxWinUIControlHost childHost;
    REQUIRE(parentHost.Initialize(owner));
    REQUIRE(childHost.Initialize(child));
    REQUIRE(parentHost.SetContent(panel, parentButton));
    const auto observation = std::make_shared<DirectMountObservation>();
    const wxWeakRef<wxWindow> weakChild(child);
    observation->inspectIdentity = [weakChild](const MUX::UIElement& element)
    {
        wxWindow * const current = weakChild.get();
        if ( !current )
            return false;
        const auto host = wxWinUITopLevelHost::FindSlotOwner(current);
        const auto slot = host ? host->FindSlot(current) : nullptr;
        return slot && slot->GetVisualRoot() == element &&
               slot->GetPresentationRoot() == element;
    };
    const auto mount = MakeDirectPanelMount(panel, observation);
    REQUIRE(childHost.SetContentInParentDirect(button, button, owner, mount));
    CHECK(observation->identityPublishedAtInsert);
    CHECK(observation->inserts == 1);
    REQUIRE(panel.Children().Size() == 2);
    CHECK(panel.Children().GetAt(0) == parentButton);
    CHECK(panel.Children().GetAt(1) == button);
    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindSlotOwner(owner);
    REQUIRE(host);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot);
    CHECK(slot->GetPresentationRoot() == button);
    CHECK(childHost.GetContent() == button);

    // This case exercises native focus, like the other host-focus fixtures:
    // ShowWithoutActivating is suitable for geometry but not this oracle.
    frame->Show();
    host->FlushSync();
    parentHost.ForceRender();
    childHost.ForceRender();
    REQUIRE(WaitForDirectComposite([&]()
    {
        return button.IsLoaded() && button.ActualWidth() > 0 &&
               slot->GetRectInTLW() == ArrangedDirectRect(*host, button);
    }));
    CHECK(MUX::Media::VisualTreeHelper::GetParent(button) == panel);
    CHECK(button.ActualWidth() == Approx(120));
    CHECK(button.ActualHeight() == Approx(40));
    CHECK(slot->GetRectInTLW().width > child->GetSize().x);
    const auto xamlRoot = button.XamlRoot();
    REQUIRE(xamlRoot);
    const HWND nativeFocusBefore = ::GetFocus();
    const HWND activeBefore = ::GetActiveWindow();
    // Acquire the island through the real wx API before testing native focus.
    child->SetFocus();
    REQUIRE(button.Focus(MUX::FocusState::Programmatic));
    const bool focused = WaitForDirectComposite([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(xamlRoot)
                   .try_as<MUXC::Button>() == button;
    });
    std::string focusDiagnostic;
    if ( !focused )
    {
        const HWND nativeFocus = ::GetFocus();
        const auto nativeElement =
            MUX::Input::FocusManager::GetFocusedElement(xamlRoot);
        std::ostringstream text;
        text << "direct focus timeout: frame=" << frame->GetHWND()
             << " active=" << frame->IsActive()
             << " shown=" << frame->IsShownOnScreen()
             << " nativeBefore=" << nativeFocusBefore
             << " activeBefore=" << activeBefore
             << " nativeNow=" << nativeFocus
             << " activeNow=" << ::GetActiveWindow()
             << " foreground=" << ::GetForegroundWindow()
             << " bridge=" << host->GetBridgeHwnd()
             << " islandContainsFocus=" << childHost.ContainsFocus(nativeFocus)
             << " wxFindFocus=" << wxWindow::FindFocus()
             << " focusedSlot="
             << wxWinUIHostFocusTestAccess::FindFocusedSlot(*host)
             << " expected=" << child
             << " loaded=" << button.IsLoaded()
             << " enabled=" << button.IsEnabled()
             << " visible=" << static_cast<int>(button.Visibility())
             << " focusState=" << static_cast<int>(button.FocusState())
             << " nativeElement="
             << (nativeElement ? winrt::to_string(winrt::get_class_name(nativeElement))
                               : "null");
        focusDiagnostic = text.str();
    }
    INFO(focusDiagnostic);
    REQUIRE(focused);
    CHECK(wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) == child);

    child->Enable(false);
    host->FlushSync();
    CHECK_FALSE(button.IsEnabled());
    CHECK_FALSE(button.IsHitTestVisible());
    child->Enable(true);
    child->Hide();
    host->FlushSync();
    CHECK(button.IsEnabled());
    CHECK(button.Visibility() == MUX::Visibility::Collapsed);
    child->Show();
    host->FlushSync();
    CHECK(button.Visibility() == MUX::Visibility::Visible);
    CHECK(button.IsHitTestVisible());
    CHECK(panel.Children().GetAt(1) == button);
    CHECK(observation->inserts == 1);
    CHECK(observation->removals == 0);

    REQUIRE(parentButton.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForDirectComposite([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(xamlRoot)
                   .try_as<MUXC::Button>() == parentButton;
    }));
    CHECK(wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) == owner);
}

TEST_CASE("HostComposite::DirectRootReplacementIsRejectedBeforeMutation",
          "[HostLifecycle][winui-composite][winui-composite-direct]")
{
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "direct-composite-replacement",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainDirectComposite();
    });
    wxWindow * const owner = new wxWindow(frame, wxID_ANY);
    wxWindow * const child = new wxWindow(owner, wxID_ANY);
    const MUXC::StackPanel panel;
    const MUXC::Button button;
    const MUXC::Button replacement;
    wxWinUIControlHost parentHost;
    wxWinUIControlHost childHost;
    REQUIRE(parentHost.Initialize(owner));
    REQUIRE(childHost.Initialize(child));
    REQUIRE(parentHost.SetContent(panel));
    const auto observation = std::make_shared<DirectMountObservation>();
    const auto mount = MakeDirectPanelMount(panel, observation);
    REQUIRE(childHost.SetContentInParentDirect(button, button, owner, mount));
    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindSlotOwner(owner);
    REQUIRE(host);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot);
    REQUIRE_FALSE(childHost.SetContentInParentDirect(
        replacement, replacement, owner, mount));
    CHECK(host->FindSlot(child) == slot);
    CHECK(slot->GetPresentationRoot() == button);
    CHECK(slot->GetVisualRoot() == button);
    CHECK(slot->GetSemanticTarget() == button);
    CHECK(childHost.GetContent() == button);
    REQUIRE(panel.Children().Size() == 1);
    CHECK(panel.Children().GetAt(0) == button);
    frame->ShowWithoutActivating();
    host->FlushSync();
    parentHost.ForceRender();
    childHost.ForceRender();
    REQUIRE(WaitForDirectComposite([&]() { return button.IsLoaded(); }));
    CHECK(MUX::Media::VisualTreeHelper::GetParent(button) == panel);
    CHECK(MUX::Media::VisualTreeHelper::GetParent(replacement) == nullptr);
    CHECK(observation->inserts == 1);
    CHECK(observation->removals == 0);
}

TEST_CASE("HostComposite::DirectChildrenRetireBeforeTheirParent",
          "[HostLifecycle][winui-composite][winui-composite-direct][popup-gate][causal]")
{
    DrainDirectComposite();
    const unsigned pending0 = wxWinUIHostObservationTestAccess::
        GetPendingPhysicalDisconnectGateCount();
    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "direct-composite-retirement",
        wxPoint(-32000, -32000), wxSize(420, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        DrainDirectComposite();
    });
    wxWindow * const owner = new wxWindow(
        frame, wxID_ANY, wxPoint(5, 5), wxSize(380, 200));
    const std::array<wxWindow *, 2> children = {
        new wxWindow(owner, wxID_ANY), new wxWindow(owner, wxID_ANY)
    };
    const MUXC::StackPanel panel;
    const std::array<MUXC::Button, 2> buttons = {
        MUXC::Button(), MUXC::Button()
    };
    wxWinUIControlHost parentHost;
    std::array<wxWinUIControlHost, 2> childHosts;
    REQUIRE(parentHost.Initialize(owner));
    REQUIRE(parentHost.SetContent(panel));
    const auto observation = std::make_shared<DirectMountObservation>();
    const auto mount = MakeDirectPanelMount(panel, observation);
    for ( size_t i = 0; i < children.size(); ++i )
    {
        buttons[i].Width(120);
        buttons[i].Height(40);
        REQUIRE(childHosts[i].Initialize(children[i]));
        REQUIRE(childHosts[i].SetContentInParentDirect(
            buttons[i], buttons[i], owner, mount));
    }
    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindSlotOwner(owner);
    REQUIRE(host);
    REQUIRE(host->FindSlot(owner));
    const auto parentContainer = host->FindSlot(owner)->GetContainer();
    REQUIRE(parentContainer.Children().Size() == 1);
    const auto carrier = parentContainer.Children().GetAt(0)
                             .as<MUXC::ContentControl>();
    const auto rootCanvas = parentContainer.Parent().try_as<MUXC::Canvas>();
    REQUIRE(rootCanvas);
    frame->ShowWithoutActivating();
    host->FlushSync();
    REQUIRE(WaitForDirectComposite([&]()
    {
        return buttons[0].IsLoaded() && buttons[1].IsLoaded();
    }));
    const std::array<std::shared_ptr<wxWinUIPhysicalDisconnectGate>, 2> gates = {
        wxWinUIPhysicalDisconnectTestAccess::CreateGate(
            buttons[0].DispatcherQueue(), false),
        wxWinUIPhysicalDisconnectTestAccess::CreateGate(
            buttons[1].DispatcherQueue(), false)
    };
    REQUIRE(gates[0]);
    REQUIRE(gates[1]);
    const auto finishGates = wxMakeGuard([gates]()
    {
        for ( const auto& gate : gates )
            wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true);
    });
    for ( size_t i = 0; i < children.size(); ++i )
        childHosts[i].SetPhysicalDisconnectGate(gates[i]);

    childHosts[0].Close();
    REQUIRE(host->FindSlot(children[0]) == nullptr);
    parentHost.Close();
    CHECK(host->FindSlot(owner) == nullptr);
    CHECK(host->FindSlot(children[1]) == nullptr);
    DrainDirectComposite();
    CHECK(panel.Children().Size() == 2);
    CHECK(carrier.Content().try_as<MUXC::StackPanel>() == panel);
    CHECK(observation->removals == 0);
    uint32_t index = 0;
    CHECK(rootCanvas.Children().IndexOf(parentContainer, index));

    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gates[0], true));
    REQUIRE(WaitForDirectComposite([&]()
    {
        return panel.Children().Size() == 1;
    }));
    CHECK(panel.Children().GetAt(0) == buttons[1]);
    CHECK(observation->removals == 1);
    CHECK(carrier.Content().try_as<MUXC::StackPanel>() == panel);
    CHECK(rootCanvas.Children().IndexOf(parentContainer, index));
    CHECK_FALSE(gates[1]->IsSatisfied());

    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gates[1], true));
    REQUIRE(WaitForDirectComposite([&]()
    {
        uint32_t ignored = 0;
        return panel.Children().Size() == 0 && !carrier.Content() &&
               !rootCanvas.Children().IndexOf(parentContainer, ignored) &&
               wxWinUIHostObservationTestAccess::
                   GetPendingPhysicalDisconnectGateCount() == pending0;
    }));
    CHECK(observation->removals == 2);
    CHECK(parentContainer.Children().Size() == 0);
    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gates[0], true));
    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gates[1], true));
    DrainDirectComposite();
    CHECK(observation->removals == 2);
}

TEST_CASE("HostComposite::ReparentPreservesDirectAndMountedDescendants",
          "[HostLifecycle][HostFocus][winui-composite][winui-composite-migration]")
{
    DirectCompositeMigrationFixture fixture;
    fixture.Create(true);
    fixture.FocusLeaf();
    const unsigned pending0 =
        wxWinUIHostFocusTestAccess::GetPendingFocusMigrationCount();
    const auto focusEvents = std::make_shared<std::array<unsigned, 2>>();
    fixture.windows[2]->Bind(wxEVT_SET_FOCUS,
        [focusEvents](wxFocusEvent& event)
        {
            ++(*focusEvents)[0];
            event.Skip();
        });
    fixture.windows[2]->Bind(wxEVT_KILL_FOCUS,
        [focusEvents](wxFocusEvent& event)
        {
            ++(*focusEvents)[1];
            event.Skip();
        });

    for ( wxFrame *target : {fixture.destination, fixture.source} )
    {
        const auto previousRoot = fixture.leaf.XamlRoot();
        REQUIRE(fixture.windows[0]->Reparent(target));
        // Reparent itself must commit the entire cohort before returning.
        // No dispatcher progress may hide split source/destination ownership.
        fixture.CheckOwnedBy(target);
        const auto host = wxWinUITopLevelHost::FindForTLW(target);
        REQUIRE(host);
        REQUIRE(WaitForDirectComposite([&]()
        {
            return fixture.leaf.IsLoaded() &&
                   fixture.leaf.XamlRoot() == host->Root().XamlRoot() &&
                   MUX::Input::FocusManager::GetFocusedElement(
                       fixture.leaf.XamlRoot()).try_as<MUXC::Button>() ==
                       fixture.leaf &&
                   wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) ==
                       fixture.windows[2] &&
                   wxWindow::FindFocus() == fixture.windows[2] &&
                   wxWinUIHostFocusTestAccess::
                       GetPendingFocusMigrationCount() == pending0;
        }));
        CHECK(fixture.leaf.XamlRoot() != previousRoot);
        CHECK((*focusEvents)[0] == 0);
        CHECK((*focusEvents)[1] == 0);
        fixture.CheckOwnedBy(target);
    }
}

TEST_CASE("HostComposite::ReparentWaitsForDescendantPhysicalGate",
          "[HostLifecycle][winui-composite][winui-composite-migration][popup-gate][causal]")
{
    DirectCompositeMigrationFixture fixture;
    fixture.Create();
    const auto gate = wxWinUIPhysicalDisconnectTestAccess::CreateGate(
        fixture.leaf.DispatcherQueue(), false);
    REQUIRE(gate);
    const auto finishGate = wxMakeGuard([gate]()
    {
        wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true);
    });
    fixture.leafHost.SetPhysicalDisconnectGate(gate);
    const auto gateIdentity = reinterpret_cast<std::uintptr_t>(gate.get());
    const auto sourceRoot = fixture.leaf.XamlRoot();

    CHECK_FALSE(fixture.windows[0]->Reparent(fixture.destination));
    fixture.CheckOwnedBy(fixture.source);
    CHECK(fixture.leaf.XamlRoot() == sourceRoot);
    CHECK(wxWinUISlotTestAccess::GetPhysicalDisconnectGateIdentity(
              *fixture.slots[2]) == gateIdentity);
    DrainDirectComposite();
    fixture.CheckOwnedBy(fixture.source);
    CHECK_FALSE(gate->IsSatisfied());

    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true));
    REQUIRE(fixture.windows[0]->Reparent(fixture.destination));
    fixture.CheckOwnedBy(fixture.destination);
    CHECK(wxWinUISlotTestAccess::GetPhysicalDisconnectGateIdentity(
              *fixture.slots[2]) == gateIdentity);
    REQUIRE(WaitForDirectComposite([&]()
    {
        const auto host = wxWinUITopLevelHost::FindForTLW(fixture.destination);
        return host && fixture.leaf.IsLoaded() &&
               fixture.leaf.XamlRoot() == host->Root().XamlRoot();
    }));
    fixture.CheckOwnedBy(fixture.destination);
}

TEST_CASE("HostComposite::ReparentDestinationFaultRestoresWholeNativeTree",
          "[HostLifecycle][winui-composite][winui-composite-migration]")
{
    bool standardException = false;
    SECTION("HRESULT exception before logical close")
    {
    }
    SECTION("standard exception before logical close")
    {
        standardException = true;
    }
    DirectCompositeMigrationFixture fixture;
    fixture.Create();
    const auto sourceRoot = fixture.leaf.XamlRoot();
    bool injected = false;
    bool attachedAtDestination = false;
    const auto clearHook = wxMakeGuard([]()
    {
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});
    });
    wxWinUIHostContentTestAccess::OnNextSlotAttached(
        [&](wxWindow *attached)
        {
            injected = attached == fixture.windows[0];
            const auto host =
                wxWinUITopLevelHost::FindForTLW(fixture.destination);
            attachedAtDestination = host &&
                MUX::Media::VisualTreeHelper::GetParent(fixture.roots[0]) ==
                    host->Root();
            if ( standardException )
                throw std::runtime_error("direct cohort destination attach fault");
            throw winrt::hresult_error(E_FAIL);
        });
    {
        wxLogNull suppressExpectedFailure;
        CHECK_FALSE(fixture.windows[0]->Reparent(fixture.destination));
    }
    REQUIRE(injected);
    REQUIRE(attachedAtDestination);
    fixture.CheckOwnedBy(fixture.source);
    CHECK(fixture.leaf.XamlRoot() == sourceRoot);
    DrainDirectComposite();
    fixture.CheckOwnedBy(fixture.source);

    // The one-shot failure leaves no sticky transaction or detached child.
    REQUIRE(fixture.windows[0]->Reparent(fixture.destination));
    fixture.CheckOwnedBy(fixture.destination);
    REQUIRE(WaitForDirectComposite([&]()
    {
        const auto host = wxWinUITopLevelHost::FindForTLW(fixture.destination);
        return host && fixture.leaf.IsLoaded() &&
               fixture.leaf.XamlRoot() == host->Root().XamlRoot();
    }));
    fixture.CheckOwnedBy(fixture.destination);
}

TEST_CASE("HostComposite::ReparentSurvivesSourceShutdownAfterDestinationAttach",
          "[HostLifecycle][HostFocus][winui-composite][winui-composite-migration]")
{
    DirectCompositeMigrationFixture fixture;
    fixture.Create(true);
    fixture.FocusLeaf();
    const unsigned pending0 =
        wxWinUIHostFocusTestAccess::GetPendingFocusMigrationCount();
    wxWinUITopLevelHost * const sourceHost =
        wxWinUITopLevelHost::FindForTLW(fixture.source);
    REQUIRE(sourceHost);
    const auto sourceLifetime =
        wxWinUIHostContentTestAccess::GetLifetime(*sourceHost);
    bool cohortPublished = false;
    bool sourceInvalidated = false;
    bool sourceDestroyRequested = false;
    const auto clearHook = wxMakeGuard([]()
    {
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});
    });
    wxWinUIHostContentTestAccess::OnNextSlotAttached(
        [&](wxWindow *attached)
        {
            const auto targetHost =
                wxWinUITopLevelHost::FindForTLW(fixture.destination);
            cohortPublished = attached == fixture.windows[0] && targetHost &&
                MUX::Media::VisualTreeHelper::GetParent(fixture.roots[0]) ==
                    targetHost->Root();
            for ( size_t i = 0; cohortPublished && i < fixture.windows.size(); ++i )
            {
                const auto lifetime = fixture.lifetimes[i].lock();
                cohortPublished =
                    targetHost->FindSlot(fixture.windows[i]) == fixture.slots[i] &&
                    wxWinUITopLevelHost::FindSlotOwner(fixture.windows[i]) ==
                        targetHost &&
                    lifetime && lifetime->GetHost() == targetHost;
            }
            if ( !cohortPublished )
                return;

            // The old host must cease being an authority immediately, not
            // just after wx's deferred frame deletion eventually runs.
            wxWinUIHostContentTestAccess::Shutdown(*sourceHost);
            const auto lifetime = sourceLifetime.lock();
            sourceInvalidated =
                !wxWinUITopLevelHost::FindForTLW(fixture.source) &&
                (!lifetime || !lifetime->GetHost());
            wxFrame * const retiredSource = fixture.source;
            fixture.source = nullptr;
            sourceDestroyRequested = retiredSource->Destroy();
            wxYield();
        });

    REQUIRE(fixture.windows[0]->Reparent(fixture.destination));
    REQUIRE(cohortPublished);
    REQUIRE(sourceInvalidated);
    REQUIRE(sourceDestroyRequested);
    fixture.CheckOwnedBy(fixture.destination);
    const auto targetHost =
        wxWinUITopLevelHost::FindForTLW(fixture.destination);
    REQUIRE(targetHost);
    REQUIRE(WaitForDirectComposite([&]()
    {
        return fixture.leaf.IsLoaded() &&
               fixture.leaf.XamlRoot() == targetHost->Root().XamlRoot() &&
               MUX::Input::FocusManager::GetFocusedElement(
                   fixture.leaf.XamlRoot()).try_as<MUXC::Button>() ==
                   fixture.leaf &&
               wxWinUIHostFocusTestAccess::FindFocusedSlot(*targetHost) ==
                   fixture.windows[2] &&
               wxWindow::FindFocus() == fixture.windows[2] &&
               wxWinUIHostFocusTestAccess::
                   GetPendingFocusMigrationCount() == pending0;
    }));
    fixture.CheckOwnedBy(fixture.destination);
    const auto retiredLifetime = sourceLifetime.lock();
    CHECK((!retiredLifetime || !retiredLifetime->GetHost()));
}

TEST_CASE("HostComposite::ReparentCloseRetiresOnlyFromPublishedDestination",
          "[HostLifecycle][winui-composite][winui-composite-migration][popup-gate][causal]")
{
    bool throwAfterClose = false;
    SECTION("logical close during destination attachment")
    {
    }
    SECTION("logical close followed by a standard exception")
    {
        throwAfterClose = true;
    }
    DirectCompositeMigrationFixture fixture;
    fixture.Create();
    // This wx descendant has an independent Canvas root, not a composite
    // mount. Closing the composite must not strand this live sibling root.
    wxWindow * const extraWindow = new wxWindow(
        fixture.windows[0], wxID_ANY, wxPoint(220, 20), wxSize(120, 40));
    const MUXC::Button extraContent;
    wxWinUIControlHost extraHost;
    REQUIRE(extraHost.Initialize(extraWindow));
    REQUIRE(extraHost.SetContent(extraContent));
    const unsigned pending0 = wxWinUIHostObservationTestAccess::
        GetPendingPhysicalDisconnectGateCount();
    const auto sourceHost = wxWinUITopLevelHost::FindForTLW(fixture.source);
    REQUIRE(sourceHost);
    const auto sourceCanvas = sourceHost->Root();
    REQUIRE(sourceHost->FindSlot(extraWindow));
    const auto extraSourceRoot =
        sourceHost->FindSlot(extraWindow)->GetPresentationRoot();
    uint32_t extraIndex = 0;
    REQUIRE(sourceCanvas.Children().IndexOf(extraSourceRoot, extraIndex));
    const auto parentContainer = fixture.slots[0]->GetContainer();
    const auto leafContainer = fixture.slots[2]->GetContainer();
    REQUIRE(parentContainer.Children().Size() == 1);
    const auto parentCarrier = parentContainer.Children().GetAt(0)
                                   .as<MUXC::ContentControl>();
    const auto gate = wxWinUIPhysicalDisconnectTestAccess::CreateGate(
        fixture.leaf.DispatcherQueue(), true);
    REQUIRE(gate);
    const auto finishGate = wxMakeGuard([gate]()
    {
        wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true);
    });
    fixture.leafHost.SetPhysicalDisconnectGate(gate);
    MUXC::Canvas destinationCanvas{nullptr};
    bool attachedAtDestination = false;
    bool gateOpened = false;
    bool closedAtDestination = false;
    wxWindow *firstAttached = nullptr;
    const auto clearHook = wxMakeGuard([]()
    {
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});
    });
    wxWinUIHostContentTestAccess::OnNextSlotAttached(
        [&](wxWindow *attached)
        {
            const auto host =
                wxWinUITopLevelHost::FindForTLW(fixture.destination);
            firstAttached = attached;
            if ( !attached || !host ||
                 wxWinUITopLevelHost::FindSlotOwner(attached) != host )
                return;
            const auto slot = host->FindSlot(attached);
            if ( !slot )
                return;
            const auto lifetime = wxWinUISlotTestAccess::GetLifetime(*slot).lock();
            if ( !lifetime || lifetime->GetHost() != host )
                return;
            destinationCanvas = host->Root();
            const auto attachedRoot = slot->GetPresentationRoot();
            uint32_t index = 0;
            // The one-shot hook can see either independent root first.
            // Validate the root actually attached, not allocator key order.
            attachedAtDestination = attachedRoot &&
                destinationCanvas.Children().IndexOf(attachedRoot, index) &&
                MUX::Media::VisualTreeHelper::GetParent(attachedRoot) ==
                    destinationCanvas;
            if ( !attachedAtDestination )
                return;
            gateOpened = wxWinUIPhysicalDisconnectTestAccess::SetGateState(
                gate, false);
            if ( !gateOpened )
                return;
            fixture.parentHost.Close();
            closedAtDestination = true;
            if ( throwAfterClose )
                throw std::runtime_error("direct cohort fault after logical close");
        });

    // Destination publication already won. A subsequent logical Close must
    // not report migration failure and send wx/USER32 back to the old TLW.
    bool reparented = false;
    {
        wxLogNull suppressExpectedFailure;
        reparented = fixture.windows[0]->Reparent(fixture.destination);
    }
    INFO("destination close: attached=" << attachedAtDestination
         << " gateOpened=" << gateOpened << " closed=" << closedAtDestination
         << " standardException=" << throwAfterClose
         << " firstAttached=" << firstAttached
         << " composite=" << fixture.windows[0] << " extra=" << extraWindow);
    REQUIRE(reparented);
    REQUIRE(attachedAtDestination);
    REQUIRE(gateOpened);
    REQUIRE(closedAtDestination);
    REQUIRE(destinationCanvas);
    CHECK(fixture.windows[0]->GetParent() == fixture.destination);
    CHECK(::GetParent(reinterpret_cast<HWND>(fixture.windows[0]->GetHWND())) ==
          reinterpret_cast<HWND>(fixture.destination->GetHWND()));
    const auto destinationHost =
        wxWinUITopLevelHost::FindForTLW(fixture.destination);
    REQUIRE(destinationHost);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(extraWindow) == destinationHost);
    REQUIRE(destinationHost->FindSlot(extraWindow));
    const auto extraDestinationRoot =
        destinationHost->FindSlot(extraWindow)->GetPresentationRoot();
    REQUIRE(extraDestinationRoot);
    CHECK(destinationHost->FindSlot(extraWindow)->GetVisualRoot() == extraContent);
    CHECK(extraHost.GetContent() == extraContent);
    CHECK(extraWindow->GetParent() == fixture.windows[0]);
    CHECK(sourceHost->FindSlot(extraWindow) == nullptr);
    CHECK_FALSE(sourceCanvas.Children().IndexOf(extraSourceRoot, extraIndex));
    CHECK(destinationCanvas.Children().IndexOf(extraDestinationRoot, extraIndex));
    CHECK(MUX::Media::VisualTreeHelper::GetParent(extraDestinationRoot) ==
          destinationCanvas);
    for ( size_t i = 0; i < fixture.windows.size(); ++i )
    {
        CHECK(wxWinUITopLevelHost::FindSlotOwner(fixture.windows[i]) == nullptr);
        CHECK(sourceHost->FindSlot(fixture.windows[i]) == nullptr);
        CHECK(destinationHost->FindSlot(fixture.windows[i]) == nullptr);
        const auto lifetime = fixture.lifetimes[i].lock();
        CHECK((!lifetime || !lifetime->IsValid()));
    }

    DrainDirectComposite();
    uint32_t index = 0;
    CHECK(destinationCanvas.Children().IndexOf(parentContainer, index));
    CHECK_FALSE(sourceCanvas.Children().IndexOf(parentContainer, index));
    CHECK(parentCarrier.Content().try_as<MUXC::StackPanel>() == fixture.panel);
    REQUIRE(fixture.panel.Children().Size() == 1);
    CHECK(fixture.panel.Children().GetAt(0) == fixture.direct);
    CHECK(MUX::Media::VisualTreeHelper::GetParent(fixture.direct) == fixture.panel);
    CHECK(fixture.leafMount.Content().try_as<MUXC::Grid>() == leafContainer);
    CHECK(fixture.observation->inserts == 1);
    CHECK(fixture.observation->removals == 0);
    CHECK_FALSE(gate->IsSatisfied());
    CHECK(wxWinUIHostObservationTestAccess::
              GetPendingPhysicalDisconnectGateCount() > pending0);

    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true));
    REQUIRE(WaitForDirectComposite([&]()
    {
        uint32_t ignored = 0;
        return !destinationCanvas.Children().IndexOf(parentContainer, ignored) &&
               !sourceCanvas.Children().IndexOf(parentContainer, ignored) &&
               !parentCarrier.Content() && !fixture.leafMount.Content() &&
               fixture.panel.Children().Size() == 0 &&
               wxWinUIHostObservationTestAccess::
                   GetPendingPhysicalDisconnectGateCount() == pending0;
    }));
    CHECK(fixture.observation->inserts == 1);
    CHECK(fixture.observation->removals == 1);
    CHECK(parentContainer.Children().Size() == 0);
    CHECK(leafContainer.Children().Size() == 0);
    REQUIRE(wxWinUIPhysicalDisconnectTestAccess::SetGateState(gate, true));
    fixture.parentHost.Close();
    DrainDirectComposite();
    CHECK(fixture.observation->removals == 1);
    CHECK_FALSE(sourceCanvas.Children().IndexOf(parentContainer, index));
    CHECK_FALSE(destinationCanvas.Children().IndexOf(parentContainer, index));
    REQUIRE(destinationHost->FindSlot(extraWindow));
    CHECK(destinationHost->FindSlot(extraWindow)->GetPresentationRoot() ==
          extraDestinationRoot);
    CHECK(destinationCanvas.Children().IndexOf(extraDestinationRoot, extraIndex));
    CHECK(MUX::Media::VisualTreeHelper::GetParent(extraDestinationRoot) ==
          destinationCanvas);
}

#endif // defined(__WXWINUI__) && wxUSE_WINUI3
