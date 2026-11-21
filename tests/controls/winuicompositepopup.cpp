/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuicompositepopup.cpp
// Purpose:     Native popup topology facts for composite presentation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/math.h"
#include "wx/panel.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/window.h"
#include "../../src/winui/private.h"
#include "wx/winui/private/tlwhost.h"
#include "host-focus-test-access.h"
#include "physical-disconnect-test-access.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>

namespace
{
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace MUXCP = MUXC::Primitives;
namespace MUXAP = MUX::Automation::Peers;
namespace MUXProvider = MUX::Automation::Provider;
namespace WF = winrt::Windows::Foundation;

template <typename Predicate>
bool WaitForCompositePopup(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() )
            return true;
        if ( elapsed.Time() >= 1000 )
            return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        const long remaining = 1000 - elapsed.Time();
        if ( remaining <= 0 ||
             loop->DispatchTimeout(static_cast<unsigned long>(
                 std::min(5L, remaining))) == 0 )
            return false;
    }
}

bool IsInPopupSubtree(MUX::DependencyObject node,
                      const MUX::DependencyObject& ancestor)
{
    while ( node )
    {
        if ( node == ancestor )
            return true;
        node = MUX::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

WF::Rect PopupBoundsInRoot(const MUX::FrameworkElement& element,
                          const MUX::UIElement& root)
{
    return element.TransformToVisual(root).TransformBounds(
        WF::Rect{0, 0, float(element.ActualWidth()), float(element.ActualHeight())});
}

bool IsFinitePositive(const WF::Rect& rect)
{
    return std::isfinite(rect.X) && std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) && std::isfinite(rect.Height) &&
           rect.Width > 0 && rect.Height > 0;
}

} // namespace

TEST_CASE("HostCompositePopup::NativeFlyoutPreservesMountGeometryFocusAndProviders",
          "[winui-composite-popup][winui-popup-topology]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    unsigned clicks = 0;
    unsigned opened = 0;
    unsigned closed = 0;
    wxFrame frame(nullptr, wxID_ANY, "native composite popup topology",
                  wxPoint(80, 80), wxSize(620, 420));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxWindow owner(&frame, wxID_ANY, wxPoint(10, 10), wxSize(500, 320));
    const MUXC::StackPanel inlineRoot;
    inlineRoot.HorizontalAlignment(MUX::HorizontalAlignment::Left);
    inlineRoot.VerticalAlignment(MUX::VerticalAlignment::Top);
    inlineRoot.Spacing(12);
    const MUXC::Button anchor;
    anchor.Content(winrt::box_value(L"expand"));
    anchor.Width(140);
    anchor.Height(36);
    const MUXC::ContentControl inlineMount;
    const MUXC::StackPanel payload;
    payload.Width(240);
    payload.Height(110);
    const MUXC::ContentControl commandMount;
    const MUXC::Button command;
    command.Content(winrt::box_value(L"same command"));
    command.Width(180);
    command.Height(42);
    commandMount.Content(command);
    payload.Children().Append(commandMount);
    inlineMount.Content(payload);
    inlineRoot.Children().Append(anchor);
    inlineRoot.Children().Append(inlineMount);

    // This is deliberately one ordinary slot. The relocated mount/button are
    // native primitives, not registered popup composite slots. Passing this
    // test establishes topology facts, not an implemented host popup contract.
    wxWinUIControlHost presentation;
    REQUIRE(presentation.Initialize(&owner));
    REQUIRE(presentation.SetContent(inlineRoot, anchor));
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(&owner);
    REQUIRE(host);
    const MUXC::Flyout flyout;
    flyout.Placement(MUXCP::FlyoutPlacementMode::Bottom);
    const auto clickToken = command.Click([&](const auto&, const auto&) { ++clicks; });
    const auto openedToken = flyout.Opened([&](const auto&, const auto&) { ++opened; });
    const auto closedToken = flyout.Closed([&](const auto&, const auto&) { ++closed; });
    const auto cleanup = wxMakeGuard([&]()
    {
        try
        {
            if ( flyout.IsOpen() )
            {
                flyout.Hide();
                WaitForCompositePopup([&]() { return !flyout.IsOpen(); });
            }
            command.Click(clickToken);
            flyout.Opened(openedToken);
            flyout.Closed(closedToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        presentation.Close();
    });
    frame.Show();
    host->FlushSync();
    presentation.ForceRender();
    REQUIRE(WaitForCompositePopup([&]()
    {
        return anchor.IsLoaded() && command.IsLoaded() &&
               command.ActualWidth() > 0 && payload.ActualHeight() > 0;
    }));
    const auto xamlRoot = host->GetXamlRoot();
    REQUIRE(xamlRoot);
    REQUIRE(command.XamlRoot() == xamlRoot);
    flyout.XamlRoot(xamlRoot);
    owner.SetFocus();
    REQUIRE(command.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForCompositePopup([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(xamlRoot) == command;
    }));

    const auto peer = MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(command);
    REQUIRE(peer);
    const auto invoke = peer.GetPattern(MUXAP::PatternInterface::Invoke)
                            .try_as<MUXProvider::IInvokeProvider>();
    REQUIRE(invoke);
    const auto checkIdentityAndInvoke = [&]()
    {
        CHECK(commandMount.Content().try_as<MUXC::Button>() == command);
        REQUIRE(payload.Children().Size() == 1);
        CHECK(payload.Children().GetAt(0) == commandMount);
        CHECK(MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(command) == peer);
        CHECK(peer.GetPattern(MUXAP::PatternInterface::Invoke)
                  .try_as<MUXProvider::IInvokeProvider>() == invoke);
        const unsigned before = clicks;
        invoke.Invoke();
        REQUIRE(WaitForCompositePopup([&]() { return clicks == before + 1; }));
        CHECK(clicks == before + 1);
    };
    const auto reportFocus = [&](const char* stage)
    {
        const HWND nativeFocus = ::GetFocus();
        const HWND bridge = host->GetBridgeHwnd();
        const bool inBridge = nativeFocus && bridge &&
            (nativeFocus == bridge || ::IsChild(bridge, nativeFocus));
        const bool exactXamlFocus =
            MUX::Input::FocusManager::GetFocusedElement(xamlRoot) == command;
        std::ostringstream facts;
        facts << "native popup facts: stage=" << stage << " rtl=" << rtl
              << " focus=" << static_cast<void *>(nativeFocus)
              << " bridge=" << static_cast<void *>(bridge)
              << " focusInBridge=" << inBridge
              << " exactXamlFocus=" << exactXamlFocus;
        WARN(facts.str());
        CHECK(nativeFocus != nullptr);
        REQUIRE(exactXamlFocus);
        // inBridge is observed, not presumed: this test must not silently
        // expand the host's native focus authority to arbitrary popup HWNDs.
    };
    const WF::Rect before = PopupBoundsInRoot(command, host->Root());
    REQUIRE(IsFinitePositive(before));
    REQUIRE(IsInPopupSubtree(command, inlineRoot));
    checkIdentityAndInvoke();
    reportFocus("inline-before");

    inlineMount.Content(nullptr);
    flyout.Content(payload);
    flyout.ShowAt(anchor);
    REQUIRE(WaitForCompositePopup([&]()
    {
        return opened == 1 && flyout.IsOpen() && payload.IsLoaded() &&
               command.IsLoaded() && command.XamlRoot() == xamlRoot;
    }));
    REQUIRE_FALSE(IsInPopupSubtree(command, inlineRoot));
    MUXCP::Popup exactPopup{nullptr};
    for ( const auto& popup :
          MUX::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot) )
    {
        if ( popup.IsOpen() && popup.Child() && IsInPopupSubtree(payload, popup.Child()) )
        {
            REQUIRE_FALSE(exactPopup);
            exactPopup = popup;
        }
    }
    REQUIRE(exactPopup);
    const auto popupChild = exactPopup.Child();
    CHECK(flyout.Content() == payload);
    CHECK(command.XamlRoot() == anchor.XamlRoot());
    const WF::Rect anchorBounds = PopupBoundsInRoot(anchor, host->Root());
    const WF::Rect popupBounds = PopupBoundsInRoot(command, host->Root());
    REQUIRE(IsFinitePositive(anchorBounds));
    REQUIRE(IsFinitePositive(popupBounds));
    // A normal flyout is not constrained to the reduced anchor rectangle.
    CHECK((popupBounds.X < anchorBounds.X || popupBounds.Y < anchorBounds.Y ||
           popupBounds.X + popupBounds.Width > anchorBounds.X + anchorBounds.Width ||
           popupBounds.Y + popupBounds.Height > anchorBounds.Y + anchorBounds.Height));
    std::ostringstream geometry;
    geometry << "native popup bounds in host root: rtl=" << rtl
             << " anchor=" << anchorBounds.X << ',' << anchorBounds.Y << ','
             << anchorBounds.Width << ',' << anchorBounds.Height
             << " command=" << popupBounds.X << ',' << popupBounds.Y << ','
             << popupBounds.Width << ',' << popupBounds.Height;
    WARN(geometry.str());
    REQUIRE(command.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForCompositePopup([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(xamlRoot) == command;
    }));
    checkIdentityAndInvoke();
    reportFocus("flyout");

    flyout.Hide();
    REQUIRE(WaitForCompositePopup([&]()
    {
        return closed == 1 && !flyout.IsOpen() && !exactPopup.IsOpen() &&
               !popupChild.as<MUX::FrameworkElement>().IsLoaded();
    }));
    flyout.Content(nullptr);
    inlineMount.Content(payload);
    presentation.ForceRender();
    REQUIRE(WaitForCompositePopup([&]()
    {
        return command.IsLoaded() && command.XamlRoot() == xamlRoot &&
               IsInPopupSubtree(command, inlineRoot);
    }));
    REQUIRE(command.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForCompositePopup([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(xamlRoot) == command;
    }));
    const WF::Rect after = PopupBoundsInRoot(command, host->Root());
    REQUIRE(IsFinitePositive(after));
    CHECK(after.X == Approx(before.X).margin(0.5));
    CHECK(after.Y == Approx(before.Y).margin(0.5));
    CHECK(after.Width == Approx(before.Width).margin(0.5));
    CHECK(after.Height == Approx(before.Height).margin(0.5));
    checkIdentityAndInvoke();
    reportFocus("inline-after");
    CHECK(clicks == 3);
    CHECK(opened == 1);
    CHECK(closed == 1);
}

namespace
{
class CompositePopupFixture
{
public:
    ~CompositePopupFixture()
    {
        try
        {
            flyout.Hide();
            if ( popupGate )
                WaitForCompositePopup([&]() { return popupGate->IsSatisfied(); });
            if ( clickToken.value ) command.Click(clickToken);
            if ( openedToken.value ) flyout.Opened(openedToken);
        }
        catch ( const winrt::hresult_error& ) { }
        childHost.Close();
        parentHost.Close();
        if ( frame ) frame->Destroy();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool rtl)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "registered composite popup",
                             wxPoint(80, 80), wxSize(620, 420));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        owner = new wxWindow(frame.get(), wxID_ANY, wxPoint(10, 10), wxSize(500, 320));
        child = new wxWindow(owner, wxID_ANY, wxPoint(3, 3), wxSize(8, 8));
        proxy = new wxPanel(owner, wxID_ANY, wxPoint(0, 0), wxSize(480, 300));
        inlineRoot.HorizontalAlignment(MUX::HorizontalAlignment::Left);
        inlineRoot.VerticalAlignment(MUX::VerticalAlignment::Top);
        inlineRoot.Spacing(12);
        anchor.Content(winrt::box_value(L"expand composite"));
        anchor.Width(140);
        anchor.Height(36);
        command.Content(winrt::box_value(L"registered command"));
        command.Width(180);
        command.Height(42);
        payload.Width(240);
        payload.Height(110);
        payload.Children().Append(commandMount);
        inlineMount.Content(payload);
        inlineRoot.Children().Append(anchor);
        inlineRoot.Children().Append(inlineMount);
        REQUIRE(parentHost.Initialize(owner));
        REQUIRE(childHost.Initialize(child));
        compositeGate = wxWinUICreateCompositePhysicalDisconnectGate();
        parentHost.SetPhysicalDisconnectGate(compositeGate);
        REQUIRE(parentHost.SetContent(inlineRoot, anchor));
        extension = std::make_shared<wxWinUICompositeContentExtension>();
        extension->root = winrt::make_weak(payload.as<MUX::FrameworkElement>());
        const auto weakPayload = winrt::make_weak(payload);
        const auto epoch = witnessEpoch;
        extension->isCurrent = [weakPayload, epoch]()
        {
            return *epoch == 1 && bool(weakPayload.get());
        };
        REQUIRE(parentHost.SetCompositeContentExtension(extension));
        REQUIRE(childHost.SetContentInParent(command, command, owner, commandMount));
        host = wxWinUITopLevelHost::FindSlotOwner(owner);
        REQUIRE(host);
        slot = host->FindSlot(child);
        REQUIRE(slot);
        childRoot = slot->GetPresentationRoot();
        parentRoot = host->FindSlot(owner)->GetPresentationRoot();
        const auto count = clicks;
        clickToken = command.Click([count](const auto&, const auto&) { ++*count; });
        const auto opened = openCount;
        openedToken = flyout.Opened([opened](const auto&, const auto&) { ++*opened; });
        frame->Show();
        host->FlushSync();
        parentHost.ForceRender();
        REQUIRE(WaitForCompositePopup([&]()
        {
            return command.IsLoaded() && anchor.IsLoaded() && command.ActualWidth() > 0;
        }));
        peer = MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(command);
        REQUIRE(peer);
        invoke = peer.GetPattern(MUXAP::PatternInterface::Invoke)
                     .try_as<MUXProvider::IInvokeProvider>();
        REQUIRE(invoke);
    }

    void Open()
    {
        inlineMount.Content(nullptr);
        flyout.Content(payload);
        flyout.Placement(MUXCP::FlyoutPlacementMode::Bottom);
        const auto state = wxWinUICreateFlyoutPopupRetirementState(
            flyout, anchor.XamlRoot(), command);
        REQUIRE(state);
        popupGate = wxWinUIGetFlyoutPopupRetirementGate(state);
        REQUIRE(popupGate);
        REQUIRE(wxWinUIAddCompositePhysicalDisconnectGate(compositeGate, popupGate));
        flyout.ShowAt(anchor);
        wxWinUIResolveFlyoutPopupAfterShow(state);
        REQUIRE(WaitForCompositePopup([&]()
        {
            return *openCount == 1 && flyout.IsOpen() && command.IsLoaded();
        }));
        REQUIRE_FALSE(IsInPopupSubtree(commandMount, inlineRoot));
        REQUIRE(IsInPopupSubtree(commandMount, payload));
        REQUIRE_FALSE(popupGate->IsSatisfied());
        REQUIRE_FALSE(popupGate->IsDegraded());
    }

    void ReturnInline()
    {
        flyout.Hide();
        REQUIRE(WaitForCompositePopup([&]() { return popupGate->IsSatisfied(); }));
        flyout.Content(nullptr);
        inlineMount.Content(payload);
        parentHost.ForceRender();
        REQUIRE(WaitForCompositePopup([&]()
        {
            return command.IsLoaded() && IsInPopupSubtree(commandMount, inlineRoot);
        }));
    }

    void CheckIdentity()
    {
        REQUIRE(host->FindSlot(child) == slot);
        CHECK(wxWinUITopLevelHost::FindSlotOwner(child) == host);
        CHECK(slot->GetPresentationRoot() == childRoot);
        CHECK(host->FindSlot(owner)->GetPresentationRoot() == parentRoot);
        CHECK(childHost.GetContent() == command);
        CHECK(commandMount.Content() == childRoot);
        CHECK(payload.Children().Size() == 1);
        CHECK(payload.Children().GetAt(0) == commandMount);
        CHECK(MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(command) == peer);
        CHECK(peer.GetPattern(MUXAP::PatternInterface::Invoke)
                  .try_as<MUXProvider::IInvokeProvider>() == invoke);
        uint32_t index = 0;
        CHECK_FALSE(host->Root().Children().IndexOf(childRoot, index));
        CHECK(host->Root().Children().IndexOf(parentRoot, index));
    }

    WF::Rect CheckGeometryAndFocus()
    {
        childHost.ForceRender();
        host->FlushSync();
        REQUIRE(WaitForCompositePopup([&]()
        {
            const auto bounds = PopupBoundsInRoot(childRoot, host->Root());
            const double scale = host->GetScale();
            const wxRect expected(wxRound(bounds.X * scale), wxRound(bounds.Y * scale),
                wxRound((bounds.X + bounds.Width) * scale) - wxRound(bounds.X * scale),
                wxRound((bounds.Y + bounds.Height) * scale) - wxRound(bounds.Y * scale));
            return IsFinitePositive(bounds) && slot->GetRectInTLW() == expected;
        }));
        child->SetFocus();
        REQUIRE(command.Focus(MUX::FocusState::Programmatic));
        REQUIRE(WaitForCompositePopup([&]()
        {
            return MUX::Input::FocusManager::GetFocusedElement(command.XamlRoot()) == command &&
                   wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) == child;
        }));
        const unsigned before = *clicks;
        invoke.Invoke();
        REQUIRE(WaitForCompositePopup([&]() { return *clicks == before + 1; }));
        CHECK(*clicks == before + 1);
        return PopupBoundsInRoot(childRoot, host->Root());
    }

    wxWeakRef<wxFrame> frame;
    wxWindow *owner = nullptr;
    wxWindow *child = nullptr;
    wxPanel *proxy = nullptr;
    wxWinUIControlHost parentHost, childHost;
    wxWinUITopLevelHost *host = nullptr;
    wxWinUISlot *slot = nullptr;
    MUXC::StackPanel inlineRoot, payload;
    MUXC::Button anchor, command;
    MUXC::ContentControl inlineMount, commandMount;
    MUXC::Flyout flyout;
    MUX::FrameworkElement childRoot{nullptr}, parentRoot{nullptr};
    MUXAP::AutomationPeer peer{nullptr};
    MUXProvider::IInvokeProvider invoke{nullptr};
    std::shared_ptr<wxWinUICompositeContentExtension> extension;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> compositeGate, popupGate;
    std::shared_ptr<unsigned> witnessEpoch = std::make_shared<unsigned>(1);
    std::shared_ptr<unsigned> clicks = std::make_shared<unsigned>(0);
    std::shared_ptr<unsigned> openCount = std::make_shared<unsigned>(0);
    winrt::event_token clickToken{}, openedToken{};
};
} // namespace

TEST_CASE("HostCompositePopup::RegisteredChildMovesToFlyoutAndProxyWithoutReplacement",
          "[winui-composite-popup][winui-popup-extension]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    CompositePopupFixture fixture;
    fixture.Create(rtl);
    fixture.CheckIdentity();
    const auto before = fixture.CheckGeometryAndFocus();
    fixture.Open();
    REQUIRE(fixture.child->Reparent(fixture.proxy));
    CHECK(fixture.child->GetParent() == fixture.proxy);
    CHECK(fixture.proxy->GetParent() == fixture.owner);
    CHECK(::GetParent(reinterpret_cast<HWND>(fixture.child->GetHWND())) ==
          reinterpret_cast<HWND>(fixture.proxy->GetHWND()));
    REQUIRE(fixture.childHost.SetContentInParent(
        fixture.command, fixture.command, fixture.owner, fixture.commandMount));
    fixture.CheckIdentity();
    const auto popupBounds = fixture.CheckGeometryAndFocus();
    CHECK(IsFinitePositive(popupBounds));
    REQUIRE(fixture.parentHost.SetCompositeContentExtension(fixture.extension));
    fixture.ReturnInline();
    REQUIRE(fixture.child->Reparent(fixture.owner));
    fixture.CheckIdentity();
    const auto after = fixture.CheckGeometryAndFocus();
    CHECK(after.X == Approx(before.X).margin(0.5));
    CHECK(after.Y == Approx(before.Y).margin(0.5));
    CHECK(after.Width == Approx(before.Width).margin(0.5));
    CHECK(after.Height == Approx(before.Height).margin(0.5));
    CHECK(*fixture.clicks == 3);
    CHECK_FALSE(wxWinUIPhysicalDisconnectTestAccess::IsPublicationPoisoned());
}

TEST_CASE("HostCompositePopup::ForeignAndStaleExtensionsRefuseWithoutMutation",
          "[winui-composite-popup][winui-popup-extension]")
{
    CompositePopupFixture fixture;
    fixture.Create(false);
    fixture.Open();
    fixture.CheckIdentity();
    const MUXC::ContentControl foreignMount;
    REQUIRE_FALSE(fixture.childHost.SetContentInParent(
        fixture.command, fixture.command, fixture.owner, foreignMount));
    CHECK_FALSE(foreignMount.Content());
    fixture.CheckIdentity();
    const auto replacement = std::make_shared<wxWinUICompositeContentExtension>();
    replacement->root = fixture.extension->root;
    replacement->isCurrent = fixture.extension->isCurrent;
    REQUIRE_FALSE(fixture.parentHost.SetCompositeContentExtension(replacement));
    fixture.CheckIdentity();
    ++*fixture.witnessEpoch;
    REQUIRE_FALSE(fixture.parentHost.SetCompositeContentExtension(fixture.extension));
    REQUIRE_FALSE(fixture.childHost.SetContentInParent(
        fixture.command, fixture.command, fixture.owner, fixture.commandMount));
    fixture.CheckIdentity();
    CHECK_FALSE(fixture.popupGate->IsDegraded());
    CHECK_FALSE(wxWinUIPhysicalDisconnectTestAccess::IsPublicationPoisoned());
    fixture.ReturnInline();
    // Ordinary ancestry remains valid even though the popup witness expired.
    REQUIRE(fixture.childHost.SetContentInParent(
        fixture.command, fixture.command, fixture.owner, fixture.commandMount));
    fixture.CheckIdentity();
}

#endif // defined(__WXWINUI__) && wxUSE_WINUI3
