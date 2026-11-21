/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbontoolbar.cpp
// Purpose:     Native Ribbon ToolBar peers inside Toolkit group composites
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/app.h"
#include "wx/artprov.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/math.h"
#include "wx/menu.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/ribbon/toolbar.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "../../src/ribbon/ribboncontainerswinui.h"
#include "wx/winui/private/tlwhost.h"
#include "host-content-test-access.h"

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

namespace
{
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace Peers = MUX::Automation::Peers;
namespace Providers = MUX::Automation::Provider;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;

constexpr int NormalId = wxID_HIGHEST + 5290;
constexpr int DropdownId = NormalId + 1;
constexpr int HybridId = NormalId + 2;
constexpr int ToggleId = NormalId + 3;
const std::array<int, 4> ToolIds = {NormalId, DropdownId, HybridId, ToggleId};

void DrainNativeToolbar()
{
    for ( int i = 0; i < 12; ++i )
    {
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool WaitForNativeToolbar(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() ) return true;
        if ( elapsed.Time() >= 500 ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        const long remaining = 500 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                 static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
            return false;
    }
}

MUXC::Control FindToolbarCommand(const MUX::DependencyObject& node,
                                 const wchar_t *name)
{
    if ( !node ) return nullptr;
    const auto control = node.try_as<MUXC::Control>();
    if ( control && MUX::Automation::AutomationProperties::GetName(control) == name )
        return control;
    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
    {
        const auto found = FindToolbarCommand(
            MUX::Media::VisualTreeHelper::GetChild(node, i), name);
        if ( found ) return found;
    }
    return nullptr;
}

bool IsWithinToolbarTree(MUX::DependencyObject node,
                         const MUX::DependencyObject& ancestor)
{
    while ( node )
    {
        if ( node == ancestor ) return true;
        node = MUX::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

template <typename Provider>
Provider ToolbarPattern(const MUXC::Control& control,
                        Peers::PatternInterface pattern)
{
    if ( !control ) return nullptr;
    const auto peer = Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

template <typename Action>
void ExerciseUnavailableToolbarProvider(Action action)
{
    // A detached/disabled native provider may reject the request or accept
    // it without delivery. Neither outcome may dispatch a wx command.
    try { action(); }
    catch ( const winrt::hresult_error& ) { }
    DrainNativeToolbar();
}

class PopupToolbar final : public wxRibbonToolBar
{
public:
    explicit PopupToolbar(wxRibbonPanel *parent) : wxRibbonToolBar(parent) { }
    unsigned popupCalls = 0;
    wxPoint popupAnchor;
    wxRibbonToolBarToolBase *popupTool = nullptr;
protected:
    bool DoPopupMenu(wxMenu *, int x, int y) override
    {
        ++popupCalls;
        popupAnchor = wxPoint(x, y);
        popupTool = GetActiveTool();
        return true;
    }
};

class NativeToolbarFixture
{
public:
    ~NativeToolbarFixture()
    {
        if ( frame ) frame->Destroy();
        if ( destination ) destination->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool rtl = false)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Native Ribbon ToolBar",
                            wxPoint(-32000, -32000), wxSize(1440, 360));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        bar = new wxRibbonBar(frame.get(), wxID_ANY,
                              wxPoint(0, 0), wxSize(1400, 280));
        page = new wxRibbonPage(bar, wxID_ANY, "Home");
        panel = new wxRibbonPanel(page, wxID_ANY, "Native tools");
        tools = new PopupToolbar(panel);
        bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 16));
        REQUIRE(bitmap.IsOk());
        const auto bundle = wxBitmapBundle::FromBitmap(bitmap);
        REQUIRE(tools->AddTool(NormalId, bundle, "Tool normal"));
        REQUIRE(tools->AddDropdownTool(DropdownId, bundle, "Tool dropdown"));
        separator = tools->AddSeparator();
        REQUIRE(separator);
        REQUIRE(tools->AddHybridTool(HybridId, bundle, "Tool hybrid"));
        REQUIRE(tools->AddToggleTool(ToggleId, bundle, "Tool toggle"));
        REQUIRE(bar->Realize());
    }

    MUXC::Canvas Root() const
    {
        if ( !tools ) return nullptr;
        const auto host = wxWinUITopLevelHost::FindSlotOwner(tools.get());
        const auto slot = host ? host->FindSlot(tools.get()) : nullptr;
        return slot ? slot->GetContent().try_as<MUXC::Canvas>() : nullptr;
    }

    MUXC::Control Command(const wchar_t *name) const
        { return FindToolbarCommand(Root(), name); }

    void Ready()
    {
        REQUIRE(WaitForNativeToolbar([&]()
        {
            const auto host = tools
                ? wxWinUITopLevelHost::FindSlotOwner(tools.get()) : nullptr;
            if ( host ) host->FlushSync();
            const auto command = Command(L"Tool toggle");
            return command && command.IsLoaded() && command.ActualWidth() > 0 &&
                   command.ActualHeight() > 0;
        }));
    }

    void Show()
    {
        frame->ShowWithoutActivating();
        Ready();
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxFrame> destination;
    wxRibbonBar *bar = nullptr;
    wxRibbonPage *page = nullptr;
    wxRibbonPanel *panel = nullptr;
    wxWeakRef<PopupToolbar> tools;
    wxRibbonToolBarToolBase *separator = nullptr;
    wxBitmap bitmap;
};

struct ToolbarEvent
{
    wxEventType type;
    int id;
    wxObject *object;
    wxRibbonToolBar *bar;
    wxRibbonToolBarToolBase *active;
    bool checked;
};

using ToolbarEvents = std::vector<ToolbarEvent>;

ToolbarEvent ObserveToolbarEvent(wxRibbonToolBarEvent& event)
{
    return {event.GetEventType(), event.GetId(), event.GetEventObject(),
            event.GetBar(), event.GetBar()->GetActiveTool(), event.IsChecked()};
}

std::shared_ptr<ToolbarEvents> RecordToolbarEvents(wxRibbonToolBar *bar)
{
    const auto events = std::make_shared<ToolbarEvents>();
    const auto record = [events](wxRibbonToolBarEvent& event)
    {
        events->push_back(ObserveToolbarEvent(event));
    };
    bar->Bind(wxEVT_RIBBONTOOLBAR_CLICKED, record);
    bar->Bind(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, record);
    return events;
}

void CheckToolbarEvent(const ToolbarEvents& events, size_t count,
                       wxRibbonToolBar *bar, int id, wxEventType type,
                       bool checked = false)
{
    REQUIRE(events.size() == count);
    const auto& event = events.back();
    CHECK(event.id == id);
    CHECK(event.type == type);
    CHECK(event.object == bar);
    CHECK(event.bar == bar);
    CHECK(event.active == bar->FindById(id));
    CHECK(event.checked == checked);
    CHECK(bar->GetActiveTool() == nullptr);
}

} // namespace

TEST_CASE("RibbonToolBarNative::CompositePeersRealizeAndMigration",
          "[winui-ribbon-toolkit][native-toolbar][winui-composite-migration]")
{
    NativeToolbarFixture fixture;
    fixture.Create();
    REQUIRE(fixture.tools->Realize());
    REQUIRE(fixture.bar->Realize());
    CHECK_FALSE(fixture.frame->IsShown());
    fixture.Show();
    const auto root = fixture.Root();
    REQUIRE(root);
    const auto normal = fixture.Command(L"Tool normal");
    const auto toggle = fixture.Command(L"Tool toggle");
    REQUIRE(normal.try_as<MUXC::AppBarButton>());
    REQUIRE(toggle.try_as<MUXC::AppBarToggleButton>());
    REQUIRE(fixture.Command(L"Tool dropdown").try_as<MUXC::AppBarButton>());
    REQUIRE(fixture.Command(L"Tool hybrid").try_as<MUXC::AppBarButton>());
    REQUIRE(fixture.Command(L"Tool dropdown menu").try_as<MUXC::Button>());
    REQUIRE(fixture.Command(L"Tool hybrid menu").try_as<MUXC::Button>());
    const auto checkComposition = [&]()
    {
        const auto host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar);
        REQUIRE(host);
        for ( wxWindow *owner : {static_cast<wxWindow *>(fixture.page),
                                static_cast<wxWindow *>(fixture.panel),
                                static_cast<wxWindow *>(fixture.tools.get())} )
            CHECK(wxWinUITopLevelHost::FindSlotOwner(owner) == host);
        REQUIRE(host->FindSlot(fixture.panel));
        const auto group = host->FindSlot(fixture.panel)->GetPresentationRoot()
                               .as<Labs::RibbonCollapsibleGroup>();
        REQUIRE(group.Content());
        CHECK(IsWithinToolbarTree(root, group.Content()));
        REQUIRE(host->FindSlot(fixture.tools.get()));
        uint32_t index = 0;
        CHECK_FALSE(host->Root().Children().IndexOf(
            host->FindSlot(fixture.tools.get())->GetPresentationRoot(), index));
        CHECK(fixture.Root() == root);
        CHECK(fixture.Command(L"Tool normal") == normal);
        CHECK(fixture.Command(L"Tool toggle") == toggle);
    };
    checkComposition();
    for ( unsigned i = 0; i != 3; ++i )
    {
        REQUIRE(fixture.tools->Realize());
        REQUIRE(fixture.bar->Realize());
        fixture.Ready();
        checkComposition();
    }
    const auto invoke = ToolbarPattern<Providers::IInvokeProvider>(
        normal, Peers::PatternInterface::Invoke);
    REQUIRE(invoke);
    const auto events = RecordToolbarEvents(fixture.tools.get());
    fixture.destination = new wxFrame(nullptr, wxID_ANY, "Native tools target",
        wxPoint(-30500, -32000), wxSize(1440, 360));
    fixture.destination->ShowWithoutActivating();
    REQUIRE(fixture.bar->Reparent(fixture.destination.get()));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(fixture.tools.get()) ==
          wxWinUITopLevelHost::FindForTLW(fixture.destination.get()));
    fixture.Ready();
    checkComposition();
    invoke.Invoke();
    REQUIRE(WaitForNativeToolbar([&]() { return events->size() == 1; }));
    CheckToolbarEvent(*events, 1, fixture.tools.get(), NormalId,
                       wxEVT_RIBBONTOOLBAR_CLICKED);
}

TEST_CASE("RibbonToolBarNative::KindsEventsAndPopupActiveTool",
          "[winui-ribbon-toolkit][native-toolbar][uia]")
{
    NativeToolbarFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const tools = fixture.tools.get();
    const auto events = RecordToolbarEvents(tools);
    const auto invoke = [&](const wchar_t *name, size_t expected,
                            int id, wxEventType type)
    {
        const auto provider = ToolbarPattern<Providers::IInvokeProvider>(
            fixture.Command(name), Peers::PatternInterface::Invoke);
        REQUIRE(provider);
        provider.Invoke();
        REQUIRE(WaitForNativeToolbar([&]() { return events->size() >= expected; }));
        DrainNativeToolbar();
        CheckToolbarEvent(*events, expected, tools, id, type);
    };
    invoke(L"Tool normal", 1, NormalId, wxEVT_RIBBONTOOLBAR_CLICKED);
    invoke(L"Tool dropdown", 2, DropdownId, wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED);
    invoke(L"Tool dropdown menu", 3, DropdownId, wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED);
    invoke(L"Tool hybrid", 4, HybridId, wxEVT_RIBBONTOOLBAR_CLICKED);
    invoke(L"Tool hybrid menu", 5, HybridId, wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED);
    const auto toggle = ToolbarPattern<Providers::IToggleProvider>(
        fixture.Command(L"Tool toggle"), Peers::PatternInterface::Toggle);
    REQUIRE(toggle);
    toggle.Toggle();
    REQUIRE(WaitForNativeToolbar([&]() { return events->size() >= 6; }));
    CheckToolbarEvent(*events, 6, tools, ToggleId, wxEVT_RIBBONTOOLBAR_CLICKED, true);
    CHECK(tools->GetToolState(ToggleId));
    CHECK(toggle.ToggleState() == MUX::Automation::ToggleState::On);
    toggle.Toggle();
    REQUIRE(WaitForNativeToolbar([&]() { return events->size() >= 7; }));
    CheckToolbarEvent(*events, 7, tools, ToggleId, wxEVT_RIBBONTOOLBAR_CLICKED);
    CHECK_FALSE(tools->GetToolState(ToggleId));

    struct PopupState
    {
        bool returned = false;
        wxPoint expectedAnchor;
        wxRibbonToolBarToolBase *activeAfter = nullptr;
    };
    const auto popup = std::make_shared<PopupState>();
    tools->Bind(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED,
        [popup](wxRibbonToolBarEvent& event)
        {
            auto * const bar = event.GetBar();
            popup->expectedAnchor = bar->GetToolRect(event.GetId()).GetBottomLeft();
            ++popup->expectedAnchor.y;
            wxMenu menu;
            menu.Append(wxID_ANY, "Intercepted toolbar menu");
            popup->returned = event.PopupMenu(&menu);
            popup->activeAfter = bar->GetActiveTool();
            event.Skip();
        }, DropdownId);
    invoke(L"Tool dropdown menu", 8, DropdownId, wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED);
    CHECK(popup->returned);
    CHECK(tools->popupCalls == 1);
    CHECK(tools->popupAnchor == popup->expectedAnchor);
    CHECK(tools->popupTool == tools->FindById(DropdownId));
    CHECK(popup->activeAfter == tools->FindById(DropdownId));
}

TEST_CASE("RibbonToolBarNative::StateHelpClientDataAndUpdateUI",
          "[winui-ribbon-toolkit][native-toolbar][update-ui]")
{
    wxObject firstData, replacementData;
    NativeToolbarFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const tools = fixture.tools.get();
    const auto events = RecordToolbarEvents(tools);
    tools->SetToolClientData(NormalId, &firstData);
    CHECK(tools->GetToolClientData(NormalId) == &firstData);
    tools->SetToolClientData(NormalId, &replacementData);
    CHECK(tools->GetToolClientData(NormalId) == &replacementData);
    tools->SetToolHelpString(NormalId, "Renamed tool help");
    tools->EnableTool(NormalId, false);
    tools->ToggleTool(ToggleId, true);
    fixture.Ready();
    CHECK(tools->GetToolHelpString(NormalId) == "Renamed tool help");
    CHECK_FALSE(tools->GetToolEnabled(NormalId));
    CHECK(tools->GetToolState(ToggleId));
    const auto disabled = fixture.Command(L"Renamed tool help");
    REQUIRE(disabled);
    CHECK_FALSE(disabled.IsEnabled());
#if wxUSE_TOOLTIPS
    const auto commandRoot = MUX::Media::VisualTreeHelper::GetParent(disabled)
                                 .try_as<MUXC::Grid>();
    REQUIRE(commandRoot);
    const auto tooltip = MUXC::ToolTipService::GetToolTip(commandRoot);
    REQUIRE(tooltip);
    CHECK(winrt::unbox_value<winrt::hstring>(
        tooltip) == L"Renamed tool help");
#endif
    const auto disabledInvoke = ToolbarPattern<Providers::IInvokeProvider>(
        disabled, Peers::PatternInterface::Invoke);
    REQUIRE(disabledInvoke);
    ExerciseUnavailableToolbarProvider([&]() { disabledInvoke.Invoke(); });
    CHECK(events->empty());

    struct UpdateState { unsigned calls = 0; bool restore = false; };
    const auto update = std::make_shared<UpdateState>();
    tools->Bind(wxEVT_UPDATE_UI, [update](wxUpdateUIEvent& event)
    {
        ++update->calls;
        event.Enable(update->restore);
        event.Check(!update->restore);
    }, ToggleId);
    static_cast<wxWindow *>(tools)->UpdateWindowUI(wxUPDATE_UI_NONE);
    fixture.Ready();
    CHECK(update->calls > 0);
    CHECK_FALSE(tools->GetToolEnabled(ToggleId));
    CHECK(tools->GetToolState(ToggleId));
    auto toggle = ToolbarPattern<Providers::IToggleProvider>(
        fixture.Command(L"Tool toggle"), Peers::PatternInterface::Toggle);
    REQUIRE(toggle);
    CHECK(toggle.ToggleState() == MUX::Automation::ToggleState::On);
    ExerciseUnavailableToolbarProvider([&]() { toggle.Toggle(); });
    CHECK(events->empty());
    CHECK(tools->GetToolState(ToggleId));
    const unsigned firstUpdates = update->calls;
    update->restore = true;
    static_cast<wxWindow *>(tools)->UpdateWindowUI(wxUPDATE_UI_NONE);
    fixture.Ready();
    CHECK(update->calls > firstUpdates);
    CHECK(tools->GetToolEnabled(ToggleId));
    CHECK_FALSE(tools->GetToolState(ToggleId));
    CHECK(fixture.Command(L"Tool toggle").IsEnabled());
    toggle = ToolbarPattern<Providers::IToggleProvider>(
        fixture.Command(L"Tool toggle"), Peers::PatternInterface::Toggle);
    REQUIRE(toggle);
    CHECK(toggle.ToggleState() == MUX::Automation::ToggleState::Off);
    CHECK(events->empty());
    CHECK(tools->GetToolClientData(NormalId) == &replacementData);
}

TEST_CASE("RibbonToolBarNative::RowsSeparatorsAndNativeHitGeometry",
          "[winui-ribbon-toolkit][native-toolbar][geometry]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeToolbarFixture fixture;
    fixture.Create(rtl);
    fixture.Show();
    auto * const tools = fixture.tools.get();
    std::array<wxRibbonToolBarToolBase *, 4> items{};
    for ( size_t i = 0; i < items.size(); ++i )
        items[i] = tools->FindById(ToolIds[i]);
    const wxSize oneRowSize = tools->GetBestSize();
    tools->SetRows(2, 2);
    REQUIRE(tools->Realize());
    fixture.Ready();
    CHECK(tools->GetBestSize().y > oneRowSize.y);
    CHECK(tools->GetToolRect(NormalId).y == tools->GetToolRect(DropdownId).y);
    CHECK(tools->GetToolRect(HybridId).y == tools->GetToolRect(ToggleId).y);
    CHECK(tools->GetToolRect(NormalId).y != tools->GetToolRect(HybridId).y);
    const std::array<const wchar_t *, 4> names = {
        L"Tool normal", L"Tool dropdown", L"Tool hybrid", L"Tool toggle"
    };
    const auto host = wxWinUITopLevelHost::FindSlotOwner(tools);
    REQUIRE(host);
    const HWND frameHwnd = reinterpret_cast<HWND>(fixture.frame->GetHWND());
    const HWND toolsHwnd = reinterpret_cast<HWND>(tools->GetHWND());
    RECT frameClient{};
    REQUIRE(::GetClientRect(frameHwnd, &frameClient));
    ::MapWindowPoints(frameHwnd, nullptr,
                      reinterpret_cast<POINT *>(&frameClient), 2);
    const LONG screenLeft = std::min(frameClient.left, frameClient.right);
    const LONG screenTop = std::min(frameClient.top, frameClient.bottom);
    for ( size_t i = 0; i < items.size(); ++i )
    {
        CAPTURE(i);
        REQUIRE(items[i]);
        CHECK(tools->FindById(ToolIds[i]) == items[i]);
        const auto command = fixture.Command(names[i]);
        REQUIRE(command);
        const auto center = command.TransformToVisual(host->Root()).TransformPoint(
            winrt::Windows::Foundation::Point{
                float(command.ActualWidth() / 2), float(command.ActualHeight() / 2)});
        // The composite root has its own arranged offset relative to the wx
        // shell. Independently traverse host DIPs -> physical screen -> the
        // native client, including USER32's RTL mapping. Do not use the
        // production ElementBoundsToClient helper exercised by GetToolRect.
        POINT nativePoint{screenLeft + wxRound(center.X * host->GetScale()),
                          screenTop + wxRound(center.Y * host->GetScale())};
        REQUIRE(::ScreenToClient(toolsHwnd, &nativePoint));
        const wxPoint point(nativePoint.x, nativePoint.y);
        INFO("native centre in wx client=" << point
             << " public rectangle=" << tools->GetToolRect(ToolIds[i]));
        CHECK(tools->GetToolRect(ToolIds[i]).Contains(point));
        CHECK(tools->GetToolByPos(point.x, point.y) == items[i]);
    }
    // The position query deliberately returns null for separators (the
    // public contract), even though Add/InsertSeparator return opaque IDs.
    auto * const originalSeparator = fixture.separator;
    REQUIRE(originalSeparator);
    CHECK(tools->GetToolByPos(size_t(2)) == nullptr);
    CHECK(tools->GetToolId(originalSeparator) == wxID_SEPARATOR);
    auto * const inserted = tools->InsertSeparator(1);
    REQUIRE(inserted);
    REQUIRE(tools->Realize());
    REQUIRE(tools->GetToolCount() == 6);
    CHECK(tools->GetToolByPos(size_t(0)) == items[0]);
    CHECK(tools->GetToolId(inserted) == wxID_SEPARATOR);
    CHECK(tools->GetToolByPos(size_t(1)) == nullptr);
    CHECK(tools->GetToolByPos(size_t(2)) == items[1]);
    CHECK(tools->GetToolByPos(size_t(3)) == nullptr);
    CHECK(tools->GetToolByPos(size_t(4)) == items[2]);
    CHECK(tools->GetToolByPos(size_t(5)) == items[3]);
    REQUIRE(tools->DeleteToolByPos(1));
    REQUIRE(tools->Realize());
    CHECK(tools->GetToolCount() == 5);
    CHECK(tools->GetToolByPos(size_t(2)) == nullptr);
    CHECK(tools->GetToolId(originalSeparator) == wxID_SEPARATOR);
    CHECK_FALSE(tools->DeleteToolByPos(tools->GetToolCount()));
    for ( size_t i = 0; i < items.size(); ++i )
        CHECK(tools->FindById(ToolIds[i]) == items[i]);
}

TEST_CASE("RibbonToolBarNative::InstallFailurePreservesCommittedProjection",
          "[winui-ribbon-toolkit][native-toolbar][transaction]")
{
    NativeToolbarFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const tools = fixture.tools.get();
    const auto events = RecordToolbarEvents(tools);
    const auto oldRoot = fixture.Root();
    const auto oldCommand = fixture.Command(L"Tool normal");
    const auto oldInvoke = ToolbarPattern<Providers::IInvokeProvider>(
        oldCommand, Peers::PatternInterface::Invoke);
    REQUIRE(oldInvoke);
    const wxSize best = tools->GetBestSize();
    const wxSize minimum = tools->GetMinSize();
    std::array<wxRect, 4> rectangles{};
    for ( size_t i = 0; i < ToolIds.size(); ++i )
        rectangles[i] = tools->GetToolRect(ToolIds[i]);
    const auto clearFault = wxMakeGuard([]()
    {
        wxWinUIHostContentTestAccess::FailContentSwap(0);
    });
    {
        wxLogNull suppressExpectedFailure;
        wxWinUIHostContentTestAccess::FailContentSwap(
            wxWinUIHostContentTestAccess::ContentFault_Install);
        tools->SetRows(2, 2);
    }
    CHECK(fixture.Root() == oldRoot);
    CHECK(fixture.Command(L"Tool normal") == oldCommand);
    CHECK(tools->GetBestSize() == best);
    CHECK(tools->GetMinSize() == minimum);
    for ( size_t i = 0; i < ToolIds.size(); ++i )
        CHECK(tools->GetToolRect(ToolIds[i]) == rectangles[i]);
    CHECK(events->empty());
    REQUIRE(tools->Realize());
    fixture.Ready();
    CHECK(fixture.Root() != oldRoot);
    CHECK(tools->GetBestSize().y > best.y);
    ExerciseUnavailableToolbarProvider([&]() { oldInvoke.Invoke(); });
    CHECK(events->empty());
    const auto current = ToolbarPattern<Providers::IInvokeProvider>(
        fixture.Command(L"Tool normal"), Peers::PatternInterface::Invoke);
    REQUIRE(current);
    current.Invoke();
    REQUIRE(WaitForNativeToolbar([&]() { return events->size() == 1; }));
    CheckToolbarEvent(*events, 1, tools, NormalId, wxEVT_RIBBONTOOLBAR_CLICKED);
}

TEST_CASE("RibbonToolBarNative::RetiredProvidersCannotReachReusedIds",
          "[winui-ribbon-toolkit][native-toolbar][uia][lifetime]")
{
    NativeToolbarFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const tools = fixture.tools.get();
    const auto events = RecordToolbarEvents(tools);
    const auto oldCommand = fixture.Command(L"Tool normal");
    const auto oldInvoke = ToolbarPattern<Providers::IInvokeProvider>(
        oldCommand, Peers::PatternInterface::Invoke);
    const auto oldToggle = ToolbarPattern<Providers::IToggleProvider>(
        fixture.Command(L"Tool toggle"), Peers::PatternInterface::Toggle);
    REQUIRE(oldInvoke);
    REQUIRE(oldToggle);
    REQUIRE(tools->DeleteTool(NormalId));
    REQUIRE(tools->AddTool(NormalId, wxBitmapBundle::FromBitmap(fixture.bitmap),
                            "Replacement tool"));
    REQUIRE(tools->Realize());
    REQUIRE(WaitForNativeToolbar([&]()
    {
        const auto replacement = fixture.Command(L"Replacement tool");
        return replacement && replacement.IsLoaded();
    }));
    const auto replacement = fixture.Command(L"Replacement tool");
    CHECK(replacement != oldCommand);
    ExerciseUnavailableToolbarProvider([&]() { oldInvoke.Invoke(); });
    CHECK(events->empty());
    const auto current = ToolbarPattern<Providers::IInvokeProvider>(
        replacement, Peers::PatternInterface::Invoke);
    REQUIRE(current);
    current.Invoke();
    REQUIRE(WaitForNativeToolbar([&]() { return events->size() == 1; }));
    CheckToolbarEvent(*events, 1, tools, NormalId, wxEVT_RIBBONTOOLBAR_CLICKED);
    tools->ClearTools();
    REQUIRE(tools->Realize());
    CHECK(tools->GetToolCount() == 0);
    ExerciseUnavailableToolbarProvider([&]() { oldInvoke.Invoke(); });
    ExerciseUnavailableToolbarProvider([&]() { current.Invoke(); });
    ExerciseUnavailableToolbarProvider([&]() { oldToggle.Toggle(); });
    CHECK(events->size() == 1);
    CHECK(tools->GetActiveTool() == nullptr);
}

TEST_CASE("RibbonToolBarNative::CommandMayDeleteClearOrDestroyItsOwner",
          "[winui-ribbon-toolkit][native-toolbar][uia][lifetime]")
{
    const unsigned mode = GENERATE(0u, 1u, 2u);
    CAPTURE(mode);
    NativeToolbarFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const tools = fixture.tools.get();
    auto * const expectedTool = tools->FindById(NormalId);
    const auto provider = ToolbarPattern<Providers::IInvokeProvider>(
        fixture.Command(L"Tool normal"), Peers::PatternInterface::Invoke);
    REQUIRE(provider);
    struct Mutation
    {
        wxWeakRef<wxRibbonToolBar> bar;
        ToolbarEvents events;
        bool complete = false;
    };
    const auto mutation = std::make_shared<Mutation>();
    mutation->bar = tools;
    tools->Bind(wxEVT_RIBBONTOOLBAR_CLICKED,
        [mutation, mode](wxRibbonToolBarEvent& event)
        {
            const auto keepAlive = mutation;
            keepAlive->events.push_back(ObserveToolbarEvent(event));
            if ( mode == 0 )
                keepAlive->complete = keepAlive->bar->DeleteTool(event.GetId());
            else if ( mode == 1 )
            {
                keepAlive->bar->ClearTools();
                keepAlive->complete = keepAlive->bar->GetToolCount() == 0;
            }
            else
            {
                delete keepAlive->bar.get();
                keepAlive->complete = !keepAlive->bar;
            }
        }, NormalId);
    provider.Invoke();
    REQUIRE(WaitForNativeToolbar([&]() { return mutation->complete; }));
    REQUIRE(mutation->events.size() == 1);
    CHECK(mutation->events[0].id == NormalId);
    CHECK(mutation->events[0].object == tools);
    CHECK(mutation->events[0].bar == tools);
    CHECK(mutation->events[0].active == expectedTool);
    if ( mode == 2 )
        CHECK_FALSE(fixture.tools);
    else
    {
        REQUIRE(fixture.tools);
        CHECK(fixture.tools->FindById(NormalId) == nullptr);
        CHECK(fixture.tools->GetActiveTool() == nullptr);
    }
    ExerciseUnavailableToolbarProvider([&]() { provider.Invoke(); });
    CHECK(mutation->events.size() == 1);
}

#endif // wxWinUI Toolkit Ribbon
