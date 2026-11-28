/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbonexpansion.cpp
// Purpose:     Public Ribbon panel expansion in the real Toolkit Flyout
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/app.h"
#include "wx/artprov.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/ribbon/toolbar.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "../../src/ribbon/ribboncontainerswinui.h"
#include "../../src/ribbon/ribbonflyoutwinui.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;
namespace Peers = X::Automation::Peers;
namespace Providers = X::Automation::Provider;

constexpr int ExpansionCommandId = wxID_HIGHEST + 5340;

template <typename Predicate>
bool WaitForExpansion(Predicate ready)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( ready() ) return true;
        if ( elapsed.Time() >= 500 ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( ready() ) return true;
        const long remaining = 500 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                 static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
            return false;
    }
}

C::Control FindExpansionCommand(const X::DependencyObject& node)
{
    if ( !node ) return nullptr;
    const auto control = node.try_as<C::Control>();
    if ( control && X::Automation::AutomationProperties::GetName(control) ==
                        L"Expansion command" )
        return control;
    for ( int i = 0, count = X::Media::VisualTreeHelper::GetChildrenCount(node);
          i < count; ++i )
    {
        const auto found = FindExpansionCommand(
            X::Media::VisualTreeHelper::GetChild(node, i));
        if ( found ) return found;
    }
    return nullptr;
}

bool WithinExpansionTree(X::DependencyObject node,
                         const X::DependencyObject& ancestor)
{
    while ( node )
    {
        if ( node == ancestor ) return true;
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

Providers::IInvokeProvider ExpansionInvoke(const C::Control& control)
{
    if ( !control ) return nullptr;
    const auto peer = Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control);
    return peer ? peer.GetPattern(Peers::PatternInterface::Invoke)
                     .try_as<Providers::IInvokeProvider>() : nullptr;
}

class ExpansionToolbar final : public wxRibbonToolBar
{
public:
    explicit ExpansionToolbar(wxRibbonPanel* parent) : wxRibbonToolBar(parent) { }

    bool Reparent(wxWindowBase* parent) override
    {
        if ( rejectNextReparent && parent != GetParent() )
        {
            rejectNextReparent = false;
            ++rejectedReparents;
            return false;
        }
        if ( beforeReparent && parent == observedParent )
        {
            // Keep a failing implementation bounded: a recursive restoration
            // must be observable, not an unbounded test-induced stack overflow.
            auto callback = std::move(beforeReparent);
            beforeReparent = {};
            const wxWeakRef<ExpansionToolbar> alive(this);
            callback();
            if ( !alive ) return false;
        }
        return wxRibbonToolBar::Reparent(parent);
    }

    bool rejectNextReparent = false;
    unsigned rejectedReparents = 0;
    wxWindowBase* observedParent = nullptr;
    std::function<void ()> beforeReparent;
};

struct ExpansionFixture
{
    struct NativeTransition
    {
        const char* event;
        int state;
    };

    struct NativeTrace
    {
        std::vector<NativeTransition> transitions;
        unsigned dropped = 0;
    };

    ~ExpansionFixture()
    {
        // Native callbacks retain only the trace, never this fixture. Remove
        // them before teardown itself starts closing or destroying controls.
        try
        {
            if ( stateTraceInstalled ) group.UnregisterPropertyChangedCallback(
                Labs::RibbonCollapsibleGroup::StateProperty(), stateTraceToken);
            if ( openingTraceToken.value ) parts.flyout.Opening(openingTraceToken);
            if ( openedTraceToken.value ) parts.flyout.Opened(openedTraceToken);
            if ( closingTraceToken.value ) parts.flyout.Closing(closingTraceToken);
            if ( closedTraceToken.value ) parts.flyout.Closed(closedTraceToken);
        }
        catch ( const winrt::hresult_error& ) { }
        if ( panel && panel->GetExpandedPanel() ) panel->HideExpanded();
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool rtl = false)
    {
        // Flyout placement needs a real on-screen anchor. This fixture never
        // injects keyboard/mouse input or moves the user's cursor.
        frame = new wxFrame(nullptr, wxID_ANY, "Native Ribbon expansion",
                            wxDefaultPosition, wxSize(1000, 320));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        bar = new wxRibbonBar(frame.get(), wxID_ANY,
                              wxPoint(0, 0), wxSize(960, 240));
        page = new wxRibbonPage(bar, wxID_ANY, "Home");
        panel = new wxRibbonPanel(page, wxID_ANY, "Expandable commands");
        const auto bitmap = wxArtProvider::GetBitmap(
            wxART_NEW, wxART_OTHER, wxSize(16, 16));
        REQUIRE(bitmap.IsOk());
        const auto bundle = wxBitmapBundle::FromBitmap(bitmap);
        sizer = new wxBoxSizer(wxHORIZONTAL);
        for ( size_t i = 0; i < tools.size(); ++i )
        {
            tools[i] = new ExpansionToolbar(panel.get());
            for ( int command = 0; command < 4; ++command )
                REQUIRE(tools[i]->AddTool(
                    ExpansionCommandId + static_cast<int>(i) * 4 + command,
                    bundle, i == 0 && command == 0
                        ? "Expansion command" : "Other expansion command"));
            sizer->Add(tools[i].get());
        }
        panel->SetSizer(sizer);
        REQUIRE(bar->Realize());
        // Standard Flyouts participate in activation and light-dismiss. Use
        // an active owner, as for a real invocation in the visible ribbon.
        frame->Show();
        REQUIRE(WaitForExpansion([&]()
        {
            const auto host = wxWinUITopLevelHost::FindSlotOwner(bar);
            if ( host ) host->FlushSync();
            const auto command = Command();
            return command && command.IsLoaded() && command.ActualWidth() > 0;
        }));
        const auto host = wxWinUITopLevelHost::FindSlotOwner(panel.get());
        REQUIRE(host);
        REQUIRE(host->FindSlot(panel.get()));
        REQUIRE(host->FindSlot(page));
        group = host->FindSlot(panel.get())->GetPresentationRoot()
                    .as<Labs::RibbonCollapsibleGroup>();
        ribbon = host->FindSlot(page)->GetPresentationRoot().as<Labs::Ribbon>();
        REQUIRE(wxRibbonResolveFlyoutParts(group, parts) ==
                    wxRibbonFlyoutResolveResult::Ready);
        nativeTrace = std::make_shared<NativeTrace>();
        nativeTrace->transitions.reserve(64);
        const auto record = [trace = nativeTrace, weakGroup = winrt::make_weak(group)]
                            (const char* event)
        {
            if ( trace->transitions.size() == 64 )
            {
                ++trace->dropped;
                return;
            }
            const auto current = weakGroup.get();
            trace->transitions.push_back({event,
                current ? static_cast<int>(current.State()) : -1});
        };
        stateTraceToken = group.RegisterPropertyChangedCallback(
            Labs::RibbonCollapsibleGroup::StateProperty(),
            [record](const auto&, const auto&) { record("State"); });
        stateTraceInstalled = true;
        openingTraceToken = parts.flyout.Opening(
            [record](const auto&, const auto&) { record("Opening"); });
        openedTraceToken = parts.flyout.Opened(
            [record](const auto&, const auto&) { record("Opened"); });
        closingTraceToken = parts.flyout.Closing(
            [record](const auto&, const auto&) { record("Closing"); });
        closedTraceToken = parts.flyout.Closed(
            [record](const auto&, const auto&) { record("Closed"); });
        record("TraceInstalled");
        CHECK_FALSE(panel->IsMinimised());
        CHECK_FALSE(panel->ShowExpanded());
        CHECK_FALSE(panel->HideExpanded());
        CHECK(panel->GetExpandedPanel() == nullptr);
        CHECK(panel->GetExpandedDummy() == nullptr);
    }

    X::UIElement ToolRoot(size_t index = 0) const
    {
        const auto host = tools[index]
            ? wxWinUITopLevelHost::FindSlotOwner(tools[index].get()) : nullptr;
        const auto slot = host ? host->FindSlot(tools[index].get()) : nullptr;
        return slot ? slot->GetContent() : nullptr;
    }

    C::Control Command() const { return FindExpansionCommand(ToolRoot()); }

    void Collapse()
    {
        // Exercise the Toolkit's real adaptive layout, not a fabricated wx
        // minimised bit or a test-only expansion entry point.
        ribbon.MaxWidth(80);
        ribbon.InvalidateMeasure();
        ribbon.UpdateLayout();
        REQUIRE(WaitForExpansion([&]()
        {
            ribbon.UpdateLayout();
            return panel->IsMinimised() &&
                   parts.button.Visibility() == X::Visibility::Visible;
        }));
        REQUIRE(wxRibbonFlyoutPartsAreCurrent(parts));
        REQUIRE(parts.popupMount.Content() == group.Content());
        REQUIRE_FALSE(parts.flyout.IsOpen());
    }

    void ReadyExpanded()
    {
        ++expandedWaits;
        const bool ready = WaitForExpansion([&]()
        {
            const auto command = Command();
            return panel && panel->GetExpandedPanel() && parts.flyout.IsOpen() &&
                   command && command.IsLoaded() && command.ActualWidth() > 0 &&
                   WithinExpansionTree(command, parts.flyout.Content());
        });
        const auto command = Command();
        auto* const expanded = panel ? panel->GetExpandedPanel() : nullptr;
        INFO("ReadyExpanded call=" << expandedWaits
             << " ready=" << ready
             << " nativeState=" << static_cast<int>(group.State())
             << " dummy=" << panel.get()
             << " expanded=" << expanded
             << " expandedParent=" << (expanded ? expanded->GetParent() : nullptr)
             << " expandedDummy=" << (expanded ? expanded->GetExpandedDummy() : nullptr)
             << " commandParent=" << (tools[0] ? tools[0]->GetParent() : nullptr)
             << " flyoutOpen=" << parts.flyout.IsOpen()
             << " commandExists=" << static_cast<bool>(command)
             << " commandLoaded=" << (command && command.IsLoaded())
             << " commandWidth=" << (command ? command.ActualWidth() : 0)
             << " commandHeight=" << (command ? command.ActualHeight() : 0)
             << " withinFlyout=" << WithinExpansionTree(command, parts.flyout.Content())
             << " withinInline=" << WithinExpansionTree(command, parts.inlineMount)
             << " popupOwnsCommands=" << (parts.popupMount.Content() == group.Content())
             << " inlineOwnsCommands=" << (parts.inlineMount.Content() == group.Content()));
        std::ostringstream transitions;
        if ( !ready && nativeTrace )
        {
            for ( const auto& entry : nativeTrace->transitions )
                transitions << entry.event << "(state=" << entry.state << ") ";
            transitions << "dropped=" << nativeTrace->dropped;
        }
        INFO("Native transition trace: " << transitions.str());
        REQUIRE(ready);
    }

    void ReadyClosed()
    {
        REQUIRE(WaitForExpansion([&]()
        {
            return panel && !panel->GetExpandedPanel() && !parts.flyout.IsOpen() &&
                   wxWinUITopLevelHost::CanReparentSubtreeNow(bar);
        }));
    }

    wxWeakRef<wxFrame> frame;
    wxRibbonBar* bar = nullptr;
    wxRibbonPage* page = nullptr;
    wxWeakRef<wxRibbonPanel> panel;
    std::array<wxWeakRef<ExpansionToolbar>, 2> tools;
    wxBoxSizer* sizer = nullptr;
    Labs::RibbonCollapsibleGroup group{nullptr};
    Labs::Ribbon ribbon{nullptr};
    wxRibbonFlyoutParts parts;
    unsigned expandedWaits = 0;
    std::shared_ptr<NativeTrace> nativeTrace;
    int64_t stateTraceToken = 0;
    bool stateTraceInstalled = false;
    winrt::event_token openingTraceToken{};
    winrt::event_token openedTraceToken{};
    winrt::event_token closingTraceToken{};
    winrt::event_token closedTraceToken{};
};

void CheckNativePanelSizer(ExpansionFixture& fixture, bool vertical)
{
    CAPTURE(vertical);
    const auto content = fixture.group.Content().as<X::FrameworkElement>();
    const double scale = fixture.group.XamlRoot().RasterizationScale();
    REQUIRE(scale > 0);
    const double border = 11.0 / scale;
    const double tolerance = 1.5 / scale;
    std::array<C::ContentControl, 2> mounts{nullptr, nullptr};
    for ( size_t i = 0; i != mounts.size(); ++i )
    {
        wxRibbonControl* composite = nullptr;
        REQUIRE(wxRibbonContainerWinUI::CommandMount(
                    fixture.tools[i].get(), composite, mounts[i]) ==
                wxRibbonContainerWinUI::MountStatus::Ready);
        CHECK(composite == fixture.panel.get());
    }
    std::array<winrt::Windows::Foundation::Rect, 2> bounds{};
    const auto nearlyEqual = [tolerance](double value, double expected)
        { return std::abs(value - expected) <= tolerance; };
    const bool ready = WaitForExpansion([&]()
    {
        fixture.group.UpdateLayout();
        for ( size_t i = 0; i != mounts.size(); ++i )
        {
            if ( !mounts[i].IsLoaded() || mounts[i].ActualWidth() <= 0 ||
                 mounts[i].ActualHeight() <= 0 ) return false;
            bounds[i] = mounts[i].TransformToVisual(content).TransformBounds(
                {0, 0, static_cast<float>(mounts[i].ActualWidth()),
                       static_cast<float>(mounts[i].ActualHeight())});
        }
        const auto& first = bounds[0];
        const auto& second = bounds[1];
        if ( vertical )
            return nearlyEqual(first.X, second.X) && nearlyEqual(first.Width, second.Width) &&
                   nearlyEqual(second.Y - first.Y - first.Height, border) &&
                   nearlyEqual(second.Height, 2 * (first.Height + border));
        return nearlyEqual(first.Y, second.Y) && nearlyEqual(first.Height, second.Height) &&
               nearlyEqual(second.X - first.X - first.Width, border) &&
               nearlyEqual(second.Width, 2 * (first.Width + border));
    });
    INFO("Native sizer rectangles: first=" << bounds[0].X << ',' << bounds[0].Y
         << ' ' << bounds[0].Width << 'x' << bounds[0].Height
         << " second=" << bounds[1].X << ',' << bounds[1].Y
         << ' ' << bounds[1].Width << 'x' << bounds[1].Height
         << " borderDIP=" << border << " scale=" << scale);
    REQUIRE(ready);
    for ( size_t i = 0; i != mounts.size(); ++i )
    {
        CAPTURE(i);
        auto* const item = fixture.sizer->GetItem(fixture.tools[i].get());
        REQUIRE(item);
        const auto rectangle = item->GetRect();
        CHECK(bounds[i].Width == Approx(rectangle.width / scale).margin(tolerance));
        CHECK(bounds[i].Height == Approx(rectangle.height / scale).margin(tolerance));
        const auto root = fixture.ToolRoot(i).as<X::FrameworkElement>();
        REQUIRE(root.IsLoaded());
        CHECK(WithinExpansionTree(root, mounts[i]));
        const auto nativeRoot = root.TransformToVisual(content).TransformBounds(
            {0, 0, static_cast<float>(root.ActualWidth()),
                   static_cast<float>(root.ActualHeight())});
        CHECK(nativeRoot.Width > 0);
        CHECK(nativeRoot.Height > 0);
        CHECK(nativeRoot.X >= bounds[i].X - tolerance);
        CHECK(nativeRoot.Y >= bounds[i].Y - tolerance);
        CHECK(nativeRoot.X + nativeRoot.Width <= bounds[i].X + bounds[i].Width + tolerance);
        CHECK(nativeRoot.Y + nativeRoot.Height <= bounds[i].Y + bounds[i].Height + tolerance);
    }
}

void ConfigurePanelSizer(ExpansionFixture& fixture, bool vertical)
{
    fixture.sizer->SetOrientation(vertical ? wxVERTICAL : wxHORIZONTAL);
    for ( size_t i = 0; i != fixture.tools.size(); ++i )
    {
        auto* const item = fixture.sizer->GetItem(fixture.tools[i].get());
        REQUIRE(item);
        item->SetProportion(static_cast<int>(i) + 1);
        item->SetFlag(wxEXPAND | (i == 0 ? (vertical ? wxBOTTOM : wxRIGHT) : 0));
        item->SetBorder(i == 0 ? 11 : 0);
    }
}

class PanelSizerReentryProbe final : public wxEvtHandler
{
public:
    void OnSize(wxSizeEvent& event)
    {
        event.Skip();
        ++sizeEvents;
        ++depth;
        maxDepth = std::max(maxDepth, depth);
        const auto leave = wxMakeGuard([this]() { --depth; });
        if ( entered ) return;
        entered = true;
        // One-shot before pumping: even an incorrect reentrant layout cannot
        // turn the test's callback into an unbounded recursive event source.
        wxYield();
        yieldReturned = true;
        if ( panel )
        {
            ++layoutCalls;
            layoutSucceeded = panel->Layout();
        }
    }

    wxWeakRef<wxRibbonPanel> panel;
    bool entered = false;
    bool yieldReturned = false;
    bool layoutSucceeded = false;
    unsigned layoutCalls = 0;
    unsigned sizeEvents = 0;
    unsigned depth = 0;
    unsigned maxDepth = 0;
};
} // namespace

TEST_CASE("RibbonPanelNative::ExpandedPanelTransfersPublicModelWithoutRemount",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    const auto root = fixture.ToolRoot();
    const auto command = fixture.Command();
    const auto invoke = ExpansionInvoke(command);
    REQUIRE(invoke);
    wxRibbonControl* composite = nullptr;
    C::ContentControl mount{nullptr};
    REQUIRE(wxRibbonContainerWinUI::CommandMount(fixture.tools[0].get(), composite, mount) ==
                wxRibbonContainerWinUI::MountStatus::Ready);
    REQUIRE(composite == fixture.panel.get());
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    REQUIRE(expanded);
    CHECK(expanded.get() != fixture.panel.get());
    CHECK(expanded->GetExpandedDummy() == fixture.panel.get());
    CHECK(expanded->GetExpandedPanel() == nullptr);
    CHECK(fixture.panel->GetExpandedDummy() == nullptr);
    CHECK(expanded->GetParent() == fixture.panel.get());
    CHECK(wxGetTopLevelParent(expanded.get()) == fixture.frame.get());
    CHECK(fixture.panel->IsMinimised());
    CHECK_FALSE(expanded->IsMinimised());
    CHECK_FALSE(fixture.panel->ShowExpanded());
    CHECK_FALSE(expanded->ShowExpanded());
    CHECK(fixture.panel->GetSizer() == nullptr);
    CHECK(expanded->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools ) CHECK(tool->GetParent() == expanded.get());
    REQUIRE(expanded->Realize());
    REQUIRE(expanded->Layout());
    CHECK(fixture.ToolRoot() == root);
    CHECK(fixture.Command() == command);
    wxRibbonControl* expandedComposite = nullptr;
    C::ContentControl expandedMount{nullptr};
    REQUIRE(wxRibbonContainerWinUI::CommandMount(fixture.tools[0].get(),
                expandedComposite, expandedMount) == wxRibbonContainerWinUI::MountStatus::Ready);
    CHECK(expandedComposite == composite);
    CHECK(expandedMount == mount);

    unsigned delivered = 0;
    fixture.panel->Bind(wxEVT_RIBBONTOOLBAR_CLICKED,
        [&](wxRibbonToolBarEvent& event)
        {
            ++delivered;
            CHECK(event.GetId() == ExpansionCommandId);
            CHECK(event.GetBar() == fixture.tools[0].get());
        });
    // The provider was acquired before expansion: the command's native
    // identity and event lifetime must survive the public wx Reparent.
    invoke.Invoke();
    REQUIRE(WaitForExpansion([&]() { return delivered != 0; }));
    CHECK(delivered == 1);

    const bool closeFromExpanded = GENERATE(false, true);
    CAPTURE(closeFromExpanded);
    const bool closed = closeFromExpanded
        ? expanded->HideExpanded() : fixture.panel->HideExpanded();
    REQUIRE(closed);
    fixture.ReadyClosed();
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools ) CHECK(tool->GetParent() == fixture.panel.get());
    CHECK(fixture.ToolRoot() == root);
    CHECK(fixture.Command() == command);
    CHECK_FALSE(fixture.panel->HideExpanded());
    CHECK(fixture.frame);
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    CHECK(fixture.ToolRoot() == root);
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
}

TEST_CASE("RibbonPanelNative::NativeCollapsedButtonUsesSameExpandedPanelContract",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    fixture.Collapse();
    const auto invoke = ExpansionInvoke(fixture.parts.button);
    REQUIRE(invoke);
    invoke.Invoke();
    fixture.ReadyExpanded();
    auto* const expanded = fixture.panel->GetExpandedPanel();
    REQUIRE(expanded);
    CHECK(expanded->GetExpandedDummy() == fixture.panel.get());
    CHECK(expanded->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools ) CHECK(tool->GetParent() == expanded);
    // A real native close (also used by light-dismiss) must restore the wx
    // model without requiring a subsequent public HideExpanded call.
    fixture.parts.flyout.Hide();
    fixture.ReadyClosed();
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools ) CHECK(tool->GetParent() == fixture.panel.get());
    CHECK_FALSE(fixture.panel->HideExpanded());
}

TEST_CASE("RibbonPanelNative::FailedChildReparentRollsBackExpansion",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    const auto firstRoot = fixture.ToolRoot(0);
    const auto secondRoot = fixture.ToolRoot(1);
    fixture.Collapse();
    fixture.tools[1]->rejectNextReparent = true;
    REQUIRE_FALSE(fixture.panel->ShowExpanded());
    CHECK(fixture.tools[1]->rejectedReparents == 1);
    CHECK(fixture.panel->GetExpandedPanel() == nullptr);
    CHECK(fixture.panel->GetExpandedDummy() == nullptr);
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools ) CHECK(tool->GetParent() == fixture.panel.get());
    CHECK_FALSE(fixture.parts.flyout.IsOpen());
    CHECK(fixture.ToolRoot(0) == firstRoot);
    CHECK(fixture.ToolRoot(1) == secondRoot);
    CHECK(fixture.frame);
    // The failure is transactional, not a permanent inability to expand.
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
}

TEST_CASE("RibbonPanelNative::ReentrantHideDuringChildRestorationIsRefused",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();

    unsigned nestedCalls = 0;
    bool nestedResult = true;
    fixture.tools[0]->observedParent = fixture.panel.get();
    fixture.tools[0]->beforeReparent = [&]()
    {
        ++nestedCalls;
        nestedResult = fixture.panel->HideExpanded();
    };
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
    CHECK(nestedCalls == 1);
    CHECK_FALSE(nestedResult);
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools )
    {
        REQUIRE(tool);
        CHECK(tool->GetParent() == fixture.panel.get());
    }
    CHECK(fixture.panel->GetExpandedPanel() == nullptr);
    CHECK_FALSE(fixture.panel->HideExpanded());
}

TEST_CASE("RibbonPanelNative::ClosingCanDestroyDummyAfterExpandedPanelDestruction",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    wxWeakRef<wxRibbonPanel> dummy(fixture.panel);
    wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    REQUIRE(expanded);
    const auto flyout = fixture.parts.flyout;
    unsigned closingCalls = 0;
    bool deletionRequested = false;
    const auto closingToken = flyout.Closing([&](const auto&, const auto&)
    {
        ++closingCalls;
        if ( deletionRequested ) return;
        deletionRequested = true;
        if ( dummy ) delete dummy.get();
    });
    const auto disconnect = wxMakeGuard([&]() { flyout.Closing(closingToken); });

    // Synchronous C++ deletion deliberately stresses Close's callback
    // boundary. Native Hide must be deferred until this proxy has left its
    // parent's wx child list, before Closing can delete that parent.
    delete expanded.get();
    CHECK_FALSE(expanded);
    REQUIRE(WaitForExpansion([&]()
    {
        return deletionRequested && !dummy && !flyout.IsOpen();
    }));
    CHECK(closingCalls == 1);
    CHECK_FALSE(fixture.panel);
    for ( const auto& tool : fixture.tools ) CHECK_FALSE(tool);
    REQUIRE(fixture.frame);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
}

TEST_CASE("RibbonPanelNative::AdaptiveLayoutRestoresExpandedPanelInline",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    const auto root = fixture.ToolRoot();
    const auto command = fixture.Command();
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();

    // Give the native Ribbon enough room again. Its layout owns State and
    // must close the popup while restoring the same public wx children.
    fixture.ribbon.MaxWidth(960);
    fixture.ribbon.InvalidateMeasure();
    fixture.ribbon.UpdateLayout();
    REQUIRE(WaitForExpansion([&]()
    {
        fixture.ribbon.UpdateLayout();
        return !fixture.panel->IsMinimised();
    }));
    fixture.ReadyClosed();
    CHECK(fixture.parts.inlineMount.Content() == fixture.group.Content());
    CHECK_FALSE(fixture.parts.popupMount.Content());
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools )
        CHECK(tool->GetParent() == fixture.panel.get());
    CHECK(fixture.ToolRoot() == root);
    CHECK(fixture.Command() == command);
    CHECK_FALSE(fixture.panel->ShowExpanded());
    CHECK_FALSE(fixture.panel->HideExpanded());
}

TEST_CASE("RibbonPanelNative::SameStackVisibleStateRetiresExactPopup",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    const auto root = fixture.ToolRoot();
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    // Intentionally no dispatcher turn or ReadyExpanded here. Commands can
    // move inline before the popup's Opened notification is delivered. The
    // fixed popup template mount remains the exact retirement witness.
    fixture.ribbon.MaxWidth(960);
    fixture.group.State(X::Visibility::Visible);
    fixture.ReadyClosed();
    CHECK_FALSE(fixture.panel->IsMinimised());
    CHECK(fixture.parts.inlineMount.Content() == fixture.group.Content());
    CHECK_FALSE(fixture.parts.popupMount.Content());
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools )
        CHECK(tool->GetParent() == fixture.panel.get());
    CHECK(fixture.ToolRoot() == root);
    CHECK_FALSE(fixture.panel->HideExpanded());
    // Retirement must finish, not simply hide a popup while permanently
    // disabling subsequent expansion on the complete Ribbon cohort.
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
}

TEST_CASE("RibbonPanelNative::CommandAddedWhileCollapsedMountsBeforeOpening",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    fixture.Collapse();

    const wxWeakRef<ExpansionToolbar> late(new ExpansionToolbar(fixture.panel.get()));
    const auto bitmap = wxArtProvider::GetBitmap(
        wxART_NEW, wxART_OTHER, wxSize(16, 16));
    REQUIRE(bitmap.IsOk());
    REQUIRE(late->AddTool(ExpansionCommandId + 8,
                         wxBitmapBundle::FromBitmap(bitmap), "Late expansion command"));
    REQUIRE(late->Realize());
    fixture.sizer->Add(late.get());
    // The group's template is loaded, but its closed Flyout command tree is
    // not. Publication must not wait for opening, which itself requires all
    // child commands to have valid native mounts already.
    CHECK_FALSE(fixture.parts.flyout.IsOpen());
    const auto host = wxWinUITopLevelHost::FindSlotOwner(late.get());
    REQUIRE(host);
    REQUIRE(host->FindSlot(late.get()));
    const auto root = host->FindSlot(late.get())->GetContent().as<X::FrameworkElement>();
    REQUIRE(root);
    wxRibbonControl* composite = nullptr;
    C::ContentControl mount{nullptr};
    REQUIRE(wxRibbonContainerWinUI::CommandMount(late.get(), composite, mount) ==
                wxRibbonContainerWinUI::MountStatus::Ready);
    CHECK(composite == fixture.panel.get());

    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    REQUIRE(WaitForExpansion([&]()
    {
        return root.IsLoaded() && root.ActualWidth() > 0 &&
               WithinExpansionTree(root, fixture.parts.flyout.Content());
    }));
    auto* const expanded = fixture.panel->GetExpandedPanel();
    REQUIRE(expanded);
    CHECK(late->GetParent() == expanded);
    CHECK(expanded->GetChildren().GetCount() == 3);
    REQUIRE(expanded->GetSizer() == fixture.sizer);
    CHECK(fixture.sizer->GetItem(late.get()) != nullptr);
    CHECK(host->FindSlot(late.get())->GetContent() == root);

    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
    REQUIRE(late);
    CHECK(late->GetParent() == fixture.panel.get());
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    CHECK(fixture.sizer->GetItemCount() == 3);
    for ( const auto& tool : fixture.tools )
        CHECK(tool->GetParent() == fixture.panel.get());
    CHECK(host->FindSlot(late.get())->GetContent() == root);
}

TEST_CASE("RibbonPanelNative::ExpandedPanelSurvivesCollapsedRemeasure",
          "[winui-ribbon-toolkit][winui-ribbon-expansion]")
{
    ExpansionFixture fixture;
    fixture.Create();
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    const wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    const auto command = fixture.Command();
    REQUIRE(expanded);
    REQUIRE(command);
    REQUIRE(fixture.ribbon.MaxWidth() == 80);
    auto* const loop = wxEventLoopBase::GetActive();
    REQUIRE(loop);

    const auto checkExpanded = [&]()
    {
        CHECK(fixture.group.State() == X::Visibility::Collapsed);
        CHECK(fixture.panel->IsMinimised());
        CHECK(expanded);
        CHECK(fixture.panel->GetExpandedPanel() == expanded.get());
        CHECK(fixture.parts.flyout.IsOpen());
        CHECK(command.IsLoaded());
        CHECK(fixture.Command() == command);
        CHECK(WithinExpansionTree(command, fixture.parts.flyout.Content()));
    };

    for ( unsigned measurement = 0; measurement != 3; ++measurement )
    {
        CAPTURE(measurement);
        // Real adaptive measure at the same constrained width: no manual
        // State writes and no new ShowExpanded may hide a native close.
        fixture.ribbon.InvalidateMeasure();
        fixture.ribbon.UpdateLayout();
        checkExpanded();
        for ( unsigned turn = 0; turn != 3; ++turn )
        {
            CAPTURE(turn);
            wxTheApp->ProcessPendingEvents();
            REQUIRE(wxEventLoopBase::GetActive() == loop);
            REQUIRE(loop->DispatchTimeout(5) != 0);
            wxTheApp->ProcessPendingEvents();
            checkExpanded();
        }
    }
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
}

TEST_CASE("RibbonPanelNative::SizerControlsNativeGeometryAndProportions",
          "[winui-ribbon-toolkit][winui-ribbon-sizer]")
{
    ExpansionFixture fixture;
    fixture.Create();
    const auto firstRoot = fixture.ToolRoot(0);
    const auto secondRoot = fixture.ToolRoot(1);
    ConfigurePanelSizer(fixture, true);
    REQUIRE(fixture.panel->Layout());
    CheckNativePanelSizer(fixture, true);
    ConfigurePanelSizer(fixture, false);
    REQUIRE(fixture.panel->Layout());
    CheckNativePanelSizer(fixture, false);
    CHECK(fixture.ToolRoot(0) == firstRoot);
    CHECK(fixture.ToolRoot(1) == secondRoot);
}

TEST_CASE("RibbonPanelNative::ExpandedPanelRetainsNativeSizerGeometry",
          "[winui-ribbon-toolkit][winui-ribbon-sizer]")
{
    ExpansionFixture fixture;
    fixture.Create();
    ConfigurePanelSizer(fixture, true);
    REQUIRE(fixture.panel->Layout());
    CheckNativePanelSizer(fixture, true);
    const auto firstRoot = fixture.ToolRoot(0);
    const auto secondRoot = fixture.ToolRoot(1);
    fixture.Collapse();
    REQUIRE(fixture.panel->ShowExpanded());
    fixture.ReadyExpanded();
    auto* const expanded = fixture.panel->GetExpandedPanel();
    REQUIRE(expanded);
    REQUIRE(expanded->GetSizer() == fixture.sizer);
    REQUIRE(expanded->Layout());
    CheckNativePanelSizer(fixture, true);
    ConfigurePanelSizer(fixture, false);
    REQUIRE(expanded->Layout());
    CheckNativePanelSizer(fixture, false);
    CHECK(fixture.parts.flyout.IsOpen());
    CHECK(fixture.ToolRoot(0) == firstRoot);
    CHECK(fixture.ToolRoot(1) == secondRoot);
    REQUIRE(fixture.panel->HideExpanded());
    fixture.ReadyClosed();
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
}

TEST_CASE("RibbonPanelNative::SizerLayoutSurvivesReentrantSizeDispatch",
          "[winui-ribbon-toolkit][winui-ribbon-sizer]")
{
    ExpansionFixture fixture;
    fixture.Create();
    PanelSizerReentryProbe probe;
    probe.panel = fixture.panel;
    fixture.tools[1]->Bind(wxEVT_SIZE, &PanelSizerReentryProbe::OnSize, &probe);
    const auto unbind = wxMakeGuard([&]()
    {
        if ( fixture.tools[1] )
            fixture.tools[1]->Unbind(wxEVT_SIZE, &PanelSizerReentryProbe::OnSize, &probe);
    });
    // The second proportional item grows vertically, giving a real native
    // size event rather than manually invoking the probe's handler.
    ConfigurePanelSizer(fixture, true);
    REQUIRE(fixture.panel->Layout());
    CheckNativePanelSizer(fixture, true);
    CHECK(probe.entered);
    CHECK(probe.yieldReturned);
    CHECK(probe.layoutCalls == 1);
    CHECK(probe.layoutSucceeded);
    CHECK(probe.depth == 0);
    CHECK(probe.maxDepth <= 2);
    CHECK(probe.sizeEvents < 32);
    CHECK(fixture.panel->GetSizer() == fixture.sizer);
    for ( const auto& tool : fixture.tools )
        CHECK(tool->GetParent() == fixture.panel.get());
}

TEST_CASE("RibbonPanelNative::NestedGridSizerSpacersAndVisibilityUseNativeRectangles",
          "[winui-ribbon-toolkit][winui-ribbon-sizer]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    ExpansionFixture fixture;
    fixture.Create(rtl);
    const std::array<X::UIElement, 2> roots = {fixture.ToolRoot(0), fixture.ToolRoot(1)};
    std::array<C::ContentControl, 2> mounts{nullptr, nullptr};
    for ( size_t i = 0; i != mounts.size(); ++i )
    {
        wxRibbonControl* composite = nullptr;
        REQUIRE(wxRibbonContainerWinUI::CommandMount(
                    fixture.tools[i].get(), composite, mounts[i]) ==
                wxRibbonContainerWinUI::MountStatus::Ready);
    }
    // Keep the same public outer sizer, but give it a nested, non-Box layout
    // whose diagonal controls are separated by real spacer cells and gaps.
    fixture.sizer->Clear(false);
    auto* const grid = new wxGridSizer(2, 2, 9, 13);
    grid->Add(fixture.tools[0].get(), 0, wxEXPAND | wxALL, 3);
    grid->Add(17, 11);
    grid->Add(23, 19);
    grid->Add(fixture.tools[1].get(), 0, wxEXPAND | wxALL, 5);
    fixture.sizer->Add(grid, 1, wxEXPAND | wxALL, 7);
    const auto content = fixture.group.Content().as<X::FrameworkElement>();
    const double scale = fixture.group.XamlRoot().RasterizationScale();
    REQUIRE(scale > 0);
    const double tolerance = 1.5 / scale;
    std::array<winrt::Windows::Foundation::Rect, 2> actual{};
    std::array<wxRect, 2> expected{};
    const auto checkRectangles = [&]()
    {
        REQUIRE(fixture.panel->Layout());
        const bool ready = WaitForExpansion([&]()
        {
            fixture.group.UpdateLayout();
            for ( size_t i = 0; i != mounts.size(); ++i )
            {
                auto* const item = grid->GetItem(fixture.tools[i].get());
                if ( !item ) return false;
                if ( !item->IsShown() ) continue;
                if ( !mounts[i].IsLoaded() ) return false;
                expected[i] = item->GetRect();
                actual[i] = mounts[i].TransformToVisual(content).TransformBounds(
                    {0, 0, static_cast<float>(mounts[i].ActualWidth()),
                           static_cast<float>(mounts[i].ActualHeight())});
                if ( std::abs(actual[i].X - expected[i].x / scale) > tolerance ||
                     std::abs(actual[i].Y - expected[i].y / scale) > tolerance ||
                     std::abs(actual[i].Width - expected[i].width / scale) > tolerance ||
                     std::abs(actual[i].Height - expected[i].height / scale) > tolerance )
                    return false;
            }
            return true;
        });
        INFO("Grid actual0=" << actual[0].X << ',' << actual[0].Y << ' '
             << actual[0].Width << 'x' << actual[0].Height << " expected0=" << expected[0]
             << " actual1=" << actual[1].X << ',' << actual[1].Y << ' '
             << actual[1].Width << 'x' << actual[1].Height << " expected1=" << expected[1]
             << " scale=" << scale);
        REQUIRE(ready);
        for ( size_t i = 0; i != mounts.size(); ++i )
        {
            CHECK(fixture.ToolRoot(i) == roots[i]);
            wxRibbonControl* composite = nullptr;
            C::ContentControl current{nullptr};
            REQUIRE(wxRibbonContainerWinUI::CommandMount(
                        fixture.tools[i].get(), composite, current) ==
                    wxRibbonContainerWinUI::MountStatus::Ready);
            CHECK(current == mounts[i]);
            CHECK(composite == fixture.panel.get());
        }
    };
    checkRectangles();
    CHECK(actual[1].X > actual[0].X);
    CHECK(actual[1].Y > actual[0].Y);
    REQUIRE(grid->Show(fixture.tools[1].get(), false));
    checkRectangles();
    CHECK_FALSE(fixture.tools[1]->IsShown());
    REQUIRE(WaitForExpansion([&]()
    {
        if ( !roots[1].as<X::FrameworkElement>().IsLoaded() ) return true;
        X::DependencyObject node = roots[1];
        while ( node && node != content )
        {
            if ( const auto element = node.try_as<X::UIElement>();
                 element && element.Visibility() == X::Visibility::Collapsed ) return true;
            node = X::Media::VisualTreeHelper::GetParent(node);
        }
        return !node;
    }));
    REQUIRE(grid->Show(fixture.tools[1].get(), true));
    checkRectangles();
    CHECK(fixture.tools[1]->IsShown());
    CHECK(roots[1].as<X::FrameworkElement>().IsLoaded());
    CHECK(WithinExpansionTree(roots[1], mounts[1]));
}

#endif
