/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbongallery.cpp
// Purpose:     Native Ribbon Gallery inside Toolkit group composites
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/app.h"
#include "wx/artprov.h"
#include "wx/clntdata.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "../../src/ribbon/ribboncontainerswinui.h"
#include "wx/winui/private/tlwhost.h"
#include "host-content-test-access.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <utility>
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

constexpr int GalleryId = wxID_HIGHEST + 5390;
constexpr int FirstItemId = GalleryId + 1;

template <typename Predicate>
bool WaitForNativeGallery(Predicate predicate)
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

void DrainNativeGallery()
{
    for ( unsigned i = 0; i != 12; ++i )
    {
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Element>
Element FindGalleryElement(const MUX::DependencyObject& node,
                           const wchar_t *name = nullptr)
{
    if ( !node ) return nullptr;
    const auto element = node.try_as<Element>();
    if ( element && (!name ||
            MUX::Automation::AutomationProperties::GetName(element) == name) )
        return element;
    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
    {
        const auto child = FindGalleryElement<Element>(
            MUX::Media::VisualTreeHelper::GetChild(node, i), name);
        if ( child ) return child;
    }
    return nullptr;
}

bool IsWithinGalleryTree(MUX::DependencyObject node,
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
Provider GalleryPattern(const MUXC::Control& control,
                        Peers::PatternInterface pattern)
{
    if ( !control ) return nullptr;
    const auto peer = Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

template <typename Action>
void ExerciseRetiredGalleryProvider(Action action)
{
    // A detached native provider may reject the request or accept it without
    // delivery. It must never target an item with a reused public ID.
    try { action(); }
    catch ( const winrt::hresult_error& ) { }
    DrainNativeGallery();
}

class NativeGalleryFixture
{
public:
    ~NativeGalleryFixture()
    {
        if ( frame ) frame->Destroy();
        if ( destination ) destination->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(unsigned count = 3, bool rtl = false)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Native Ribbon Gallery",
            wxPoint(-32000, -32000), wxSize(1440, 360));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        bar = new wxRibbonBar(frame.get(), wxID_ANY,
            wxPoint(0, 0), wxSize(1400, 280));
        page = new wxRibbonPage(bar, wxID_ANY, "Home");
        panel = new wxRibbonPanel(page, wxID_ANY, "Native gallery");
        gallery = new wxRibbonGallery(panel, GalleryId);
        const auto bitmap = wxArtProvider::GetBitmap(
            wxART_NEW, wxART_OTHER, wxSize(32, 32));
        REQUIRE(bitmap.IsOk());
        icon = wxBitmapBundle::FromBitmap(bitmap);
        for ( unsigned i = 0; i < count; ++i )
            REQUIRE(gallery->Append(icon, FirstItemId + static_cast<int>(i)));
        REQUIRE(bar->Realize());
    }

    MUXC::Grid Root() const
    {
        const auto host = gallery
            ? wxWinUITopLevelHost::FindSlotOwner(gallery.get()) : nullptr;
        const auto slot = host ? host->FindSlot(gallery.get()) : nullptr;
        return slot ? slot->GetContent().try_as<MUXC::Grid>() : nullptr;
    }

    MUXC::GridView View() const
        { return FindGalleryElement<MUXC::GridView>(Root()); }

    MUXC::GridViewItem Item(unsigned index) const
    {
        const auto view = View();
        const auto item = view ? view.ContainerFromIndex(static_cast<int>(index)) : nullptr;
        return item ? item.try_as<MUXC::GridViewItem>() : nullptr;
    }

    Providers::ISelectionItemProvider SelectProvider(unsigned index) const
    {
        const auto view = View();
        if ( !view || index >= view.Items().Size() ) return nullptr;
        const auto peer = Peers::FrameworkElementAutomationPeer::CreatePeerForElement(view);
        const auto itemsPeer = peer ? peer.try_as<Peers::ItemsControlAutomationPeer>() : nullptr;
        if ( !itemsPeer ) return nullptr;
        // A GridViewItem used as an Items entry has a visual/container peer
        // and a parent-created data peer. SelectionItem belongs to the data
        // peer exposed by the GridView, not CreatePeerForElement(container).
        const auto itemPeer = itemsPeer.CreateItemAutomationPeer(view.Items().GetAt(index));
        return itemPeer ? itemPeer.GetPattern(Peers::PatternInterface::SelectionItem)
            .try_as<Providers::ISelectionItemProvider>() : nullptr;
    }

    void Ready(unsigned visibleItem = 0)
    {
        REQUIRE(WaitForNativeGallery([&]()
        {
            const auto host = gallery
                ? wxWinUITopLevelHost::FindSlotOwner(gallery.get()) : nullptr;
            if ( host ) host->FlushSync();
            const auto view = View();
            return view && view.IsLoaded() && view.ActualWidth() > 0 &&
                view.ActualHeight() > 0 &&
                (gallery->IsEmpty() || Item(visibleItem));
        }));
    }

    void Show(unsigned visibleItem = 0)
    {
        frame->ShowWithoutActivating();
        Ready(visibleItem);
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxFrame> destination;
    wxRibbonBar *bar = nullptr;
    wxRibbonPage *page = nullptr;
    wxRibbonPanel *panel = nullptr;
    wxWeakRef<wxRibbonGallery> gallery;
    wxBitmapBundle icon;
};

struct GalleryEvent
{
    wxEventType type;
    int id;
    wxObject *object;
    wxRibbonGallery *gallery;
    wxRibbonGalleryItem *item;
};

using GalleryEvents = std::vector<GalleryEvent>;

GalleryEvent ObserveGalleryEvent(wxRibbonGalleryEvent& event)
{
    return {event.GetEventType(), event.GetId(), event.GetEventObject(),
            event.GetGallery(), event.GetGalleryItem()};
}

std::shared_ptr<GalleryEvents> RecordGalleryEvents(wxRibbonGallery *gallery)
{
    const auto events = std::make_shared<GalleryEvents>();
    const auto record = [events](wxRibbonGalleryEvent& event)
    {
        events->push_back(ObserveGalleryEvent(event));
    };
    gallery->Bind(wxEVT_RIBBONGALLERY_SELECTED, record);
    gallery->Bind(wxEVT_RIBBONGALLERY_CLICKED, record);
    return events;
}

void CheckGallerySelectionEvent(const GalleryEvents& events, size_t count,
                               wxRibbonGallery *gallery,
                               wxRibbonGalleryItem *item)
{
    REQUIRE(events.size() == count);
    const auto& event = events.back();
    CHECK(event.type == wxEVT_RIBBONGALLERY_SELECTED);
    CHECK(event.id == gallery->GetId());
    CHECK(event.object == gallery);
    CHECK(event.gallery == gallery);
    CHECK(event.item == item);
    CHECK(gallery->GetSelection() == item);
}

class GalleryClientData final : public wxClientData
{
public:
    explicit GalleryClientData(std::function<void()> destroyed)
        : m_destroyed(std::move(destroyed)) { }
    ~GalleryClientData() override { m_destroyed(); }
private:
    std::function<void()> m_destroyed;
};
} // namespace

TEST_CASE("RibbonGalleryNative::CompositeRealizeAndMigrationRetainProviders",
          "[winui-ribbon-toolkit][native-gallery][winui-composite-migration]")
{
    NativeGalleryFixture fixture;
    fixture.Create();
    REQUIRE(fixture.gallery->Realize());
    REQUIRE(fixture.bar->Realize());
    CHECK_FALSE(fixture.frame->IsShown());
    fixture.Show();
    const auto root = fixture.Root();
    const auto view = fixture.View();
    const auto item = fixture.Item(1);
    REQUIRE(root);
    REQUIRE(view);
    REQUIRE(item);
    REQUIRE(item.Content().try_as<MUXC::Image>());
    CHECK(view.Items().Size() == 3);
    CHECK(MUX::Automation::AutomationProperties::GetName(item) ==
          winrt::hstring(wxString::Format("Gallery item %d", FirstItemId + 1).wc_str()));
    const auto provider = fixture.SelectProvider(1);
    REQUIRE(provider);
    const auto events = RecordGalleryEvents(fixture.gallery.get());
    auto * const expected = fixture.gallery->GetItem(1);
    const auto checkComposition = [&]()
    {
        const auto host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar);
        REQUIRE(host);
        REQUIRE(host->FindSlot(fixture.panel));
        const auto group = host->FindSlot(fixture.panel)->GetPresentationRoot()
                               .as<Labs::RibbonCollapsibleGroup>();
        REQUIRE(group.Content());
        CHECK(IsWithinGalleryTree(root, group.Content()));
        CHECK(wxWinUITopLevelHost::FindSlotOwner(fixture.gallery.get()) == host);
        REQUIRE(host->FindSlot(fixture.gallery.get()));
        uint32_t index = 0;
        CHECK_FALSE(host->Root().Children().IndexOf(
            host->FindSlot(fixture.gallery.get())->GetPresentationRoot(), index));
        CHECK(fixture.Root() == root);
        CHECK(fixture.View() == view);
        CHECK(fixture.Item(1) == item);
    };
    checkComposition();
    for ( unsigned i = 0; i != 3; ++i )
    {
        REQUIRE(fixture.gallery->Realize());
        REQUIRE(fixture.bar->Realize());
        fixture.Ready();
        checkComposition();
    }
    fixture.destination = new wxFrame(nullptr, wxID_ANY, "Native gallery target",
        wxPoint(-30500, -32000), wxSize(1440, 360));
    fixture.destination->ShowWithoutActivating();
    REQUIRE(fixture.bar->Reparent(fixture.destination.get()));
    fixture.Ready();
    CHECK(wxWinUITopLevelHost::FindSlotOwner(fixture.gallery.get()) ==
          wxWinUITopLevelHost::FindForTLW(fixture.destination.get()));
    checkComposition();
    provider.Select();
    REQUIRE(WaitForNativeGallery([&]() { return events->size() == 1; }));
    CheckGallerySelectionEvent(*events, 1, fixture.gallery.get(), expected);
}

TEST_CASE("RibbonGalleryNative::SilentSelectionAndNativeSelectionItemEvents",
          "[winui-ribbon-toolkit][native-gallery][uia]")
{
    NativeGalleryFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    const auto events = RecordGalleryEvents(gallery);
    auto * const first = gallery->GetItem(0);
    auto * const second = gallery->GetItem(1);
    const auto selectFirst = fixture.SelectProvider(0);
    const auto selectSecond = fixture.SelectProvider(1);
    REQUIRE(selectFirst);
    REQUIRE(selectSecond);
    gallery->SetSelection(second);
    CHECK(gallery->GetSelection() == second);
    REQUIRE(WaitForNativeGallery([&]() { return selectSecond.IsSelected(); }));
    CHECK_FALSE(selectFirst.IsSelected());
    gallery->SetSelection(second);
    gallery->SetSelection(nullptr);
    REQUIRE(WaitForNativeGallery([&]() { return !selectSecond.IsSelected(); }));
    CHECK(gallery->GetSelection() == nullptr);
    DrainNativeGallery();
    CHECK(events->empty());

    selectFirst.Select();
    REQUIRE(WaitForNativeGallery([&]() { return events->size() == 1; }));
    CheckGallerySelectionEvent(*events, 1, gallery, first);
    selectFirst.Select();
    DrainNativeGallery();
    CHECK(events->size() == 1);
    selectSecond.Select();
    REQUIRE(WaitForNativeGallery([&]() { return events->size() == 2; }));
    CheckGallerySelectionEvent(*events, 2, gallery, second);
    CHECK_FALSE(selectFirst.IsSelected());
    CHECK(selectSecond.IsSelected());

    // SelectionItem is not a pointer click: it must not synthesize CLICKED.
    const auto extension = FindGalleryElement<MUXC::Button>(
        fixture.Root(), L"Gallery extension");
    const auto invoke = GalleryPattern<Providers::IInvokeProvider>(
        extension, Peers::PatternInterface::Invoke);
    REQUIRE(invoke);
    struct Extension { unsigned calls = 0; int id = 0; wxObject *object = nullptr; };
    const auto observed = std::make_shared<Extension>();
    gallery->Bind(wxEVT_BUTTON, [observed](wxCommandEvent& event)
    {
        ++observed->calls;
        observed->id = event.GetId();
        observed->object = event.GetEventObject();
    });
    invoke.Invoke();
    REQUIRE(WaitForNativeGallery([&]() { return observed->calls == 1; }));
    CHECK(observed->id == GalleryId);
    CHECK(observed->object == gallery);
    CHECK(events->size() == 2);
    CHECK(gallery->GetSelection() == second);
}

TEST_CASE("RibbonGalleryNative::RetiredProvidersCannotSelectReusedIds",
          "[winui-ribbon-toolkit][native-gallery][uia][lifetime]")
{
    NativeGalleryFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    const auto events = RecordGalleryEvents(gallery);
    const auto oldItem = fixture.Item(0);
    const auto oldProvider = fixture.SelectProvider(0);
    REQUIRE(oldProvider);
    gallery->Clear();
    CHECK(gallery->IsEmpty());
    CHECK(gallery->GetSelection() == nullptr);
    auto * const replacement = gallery->Append(fixture.icon, FirstItemId);
    REQUIRE(replacement);
    REQUIRE(gallery->Realize());
    fixture.Ready();
    CHECK(fixture.Item(0) != oldItem);
    ExerciseRetiredGalleryProvider([&]() { oldProvider.Select(); });
    CHECK(events->empty());
    CHECK(gallery->GetSelection() == nullptr);
    const auto current = fixture.SelectProvider(0);
    REQUIRE(current);
    current.Select();
    REQUIRE(WaitForNativeGallery([&]() { return events->size() == 1; }));
    CheckGallerySelectionEvent(*events, 1, gallery, replacement);
    gallery->Clear();
    REQUIRE(gallery->Realize());
    ExerciseRetiredGalleryProvider([&]() { current.Select(); });
    CHECK(events->size() == 1);
    CHECK(gallery->GetSelection() == nullptr);
}

TEST_CASE("RibbonGalleryNative::SelectionMayClearOrDestroyOwner",
          "[winui-ribbon-toolkit][native-gallery][uia][lifetime]")
{
    const bool destroy = GENERATE(false, true);
    CAPTURE(destroy);
    NativeGalleryFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    auto * const expected = gallery->GetItem(1);
    const auto provider = fixture.SelectProvider(1);
    REQUIRE(provider);
    struct Mutation
    {
        wxWeakRef<wxRibbonGallery> gallery;
        GalleryEvents events;
        bool done = false;
    };
    const auto mutation = std::make_shared<Mutation>();
    mutation->gallery = gallery;
    gallery->Bind(wxEVT_RIBBONGALLERY_SELECTED,
        [mutation, destroy](wxRibbonGalleryEvent& event)
        {
            const auto keepAlive = mutation;
            keepAlive->events.push_back(ObserveGalleryEvent(event));
            if ( destroy )
            {
                delete keepAlive->gallery.get();
                keepAlive->done = !keepAlive->gallery;
            }
            else
            {
                keepAlive->gallery->Clear();
                keepAlive->done = keepAlive->gallery->IsEmpty();
            }
        });
    provider.Select();
    REQUIRE(WaitForNativeGallery([&]() { return mutation->done; }));
    REQUIRE(mutation->events.size() == 1);
    const auto& event = mutation->events.front();
    CHECK(event.id == GalleryId);
    CHECK(event.type == wxEVT_RIBBONGALLERY_SELECTED);
    CHECK(event.object == gallery);
    CHECK(event.gallery == gallery);
    CHECK(event.item == expected);
    if ( destroy )
        CHECK_FALSE(fixture.gallery);
    else
    {
        REQUIRE(fixture.gallery);
        CHECK(fixture.gallery->GetSelection() == nullptr);
        CHECK(fixture.gallery->GetActiveItem() == nullptr);
        CHECK(fixture.gallery->GetHoveredItem() == nullptr);
    }
    ExerciseRetiredGalleryProvider([&]() { provider.Select(); });
    CHECK(mutation->events.size() == 1);
}

TEST_CASE("RibbonGalleryNative::NativeScrollingAndEnsureVisible",
          "[winui-ribbon-toolkit][native-gallery][geometry]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeGalleryFixture fixture;
    fixture.Create(48, rtl);
    fixture.gallery->SetSize(180, 100);
    REQUIRE(fixture.gallery->Realize());
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    const auto events = RecordGalleryEvents(gallery);
    const auto view = fixture.View();
    REQUIRE(view);
    CHECK(view.FlowDirection() == (rtl ? MUX::FlowDirection::RightToLeft
                                      : MUX::FlowDirection::LeftToRight));
    const auto scroll = FindGalleryElement<MUXC::ScrollViewer>(view);
    REQUIRE(scroll);
    REQUIRE(WaitForNativeGallery([&]() { return scroll.ScrollableHeight() > 0; }));
    auto * const selection = gallery->GetItem(1);
    auto * const last = gallery->GetItem(gallery->GetCount() - 1);
    gallery->SetSelection(selection);
    CHECK_FALSE(gallery->ScrollLines(-1));
    CHECK(gallery->GetUpButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED);
    const double start = scroll.VerticalOffset();
    REQUIRE(gallery->ScrollLines(1));
    REQUIRE(WaitForNativeGallery([&]() { return scroll.VerticalOffset() > start; }));
    gallery->EnsureVisible(last);
    REQUIRE(WaitForNativeGallery([&]()
    {
        const auto item = fixture.Item(gallery->GetCount() - 1);
        if ( !item || !item.IsLoaded() || item.ActualHeight() <= 0 ) return false;
        const auto bounds = item.TransformToVisual(view).TransformBounds(
            {0, 0, static_cast<float>(item.ActualWidth()),
             static_cast<float>(item.ActualHeight())});
        return bounds.Y >= -1 && bounds.Y + bounds.Height <= view.ActualHeight() + 1;
    }));
    CHECK(scroll.VerticalOffset() > 0);
    CHECK(gallery->GetSelection() == selection);
    gallery->EnsureVisible(gallery->GetItem(0));
    REQUIRE(WaitForNativeGallery([&]() { return scroll.VerticalOffset() < 1; }));
    REQUIRE(gallery->ScrollPixels(12));
    REQUIRE(WaitForNativeGallery([&]() { return scroll.VerticalOffset() > 0; }));
    REQUIRE(gallery->ScrollPixels(-1000000));
    REQUIRE(WaitForNativeGallery([&]() { return scroll.VerticalOffset() < 1; }));
    CHECK_FALSE(gallery->ScrollPixels(-1));
    CHECK(gallery->GetUpButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED);
    CHECK(gallery->GetSelection() == selection);
    CHECK(events->empty());
}

TEST_CASE("RibbonGalleryNative::ClientDataOwnershipAndReentrantClear",
          "[winui-ribbon-toolkit][native-gallery][lifetime]")
{
    NativeGalleryFixture fixture;
    fixture.Create(0);
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    const auto destroyed = std::make_shared<unsigned>(0);
    const auto countDestruction = [destroyed]() { ++*destroyed; };

    SECTION("Owned data replacement and clearing delete exactly once")
    {
        auto * const first = gallery->Append(fixture.icon, FirstItemId,
            new GalleryClientData(countDestruction));
        REQUIRE(first);
        auto * const replacement = new GalleryClientData(countDestruction);
        gallery->SetItemClientObject(first, replacement);
        CHECK(*destroyed == 1);
        CHECK(gallery->GetItemClientObject(first) == replacement);
        gallery->SetItemClientObject(first, replacement);
        CHECK(*destroyed == 1);
        REQUIRE(gallery->Append(fixture.icon, FirstItemId + 1,
            new GalleryClientData(countDestruction)));
        gallery->SetSelection(first);
        gallery->Clear();
        CHECK(*destroyed == 3);
        CHECK(gallery->GetCount() == 0);
        CHECK(gallery->GetSelection() == nullptr);
        REQUIRE(gallery->Realize());
        CHECK(fixture.View().Items().Size() == 0);
        gallery->Clear();
        CHECK(*destroyed == 3);
    }
    SECTION("Untyped client data remains owned by the caller")
    {
        auto data = std::make_unique<GalleryClientData>(countDestruction);
        auto * const item = gallery->Append(fixture.icon, FirstItemId,
            static_cast<void *>(data.get()));
        REQUIRE(item);
        CHECK(gallery->GetItemClientData(item) == data.get());
        gallery->Clear();
        CHECK(*destroyed == 0);
        data.reset();
        CHECK(*destroyed == 1);
    }
    SECTION("Old item destructor may clear and append a new model item")
    {
        struct ReentrantState
        {
            unsigned first = 0;
            unsigned second = 0;
            bool emptyBeforeReentry = false;
            wxRibbonGalleryItem *survivor = nullptr;
        };
        const auto state = std::make_shared<ReentrantState>();
        const auto icon = fixture.icon;
        REQUIRE(gallery->Append(icon, FirstItemId, new GalleryClientData(
            [state, gallery, icon]()
            {
                ++state->first;
                state->emptyBeforeReentry = gallery->IsEmpty() &&
                    gallery->GetSelection() == nullptr;
                gallery->Clear();
                state->survivor = gallery->Append(icon, FirstItemId);
            })));
        REQUIRE(gallery->Append(icon, FirstItemId + 1, new GalleryClientData(
            [state]() { ++state->second; })));
        gallery->SetSelection(gallery->GetItem(0));
        gallery->Clear();
        CHECK(state->first == 1);
        CHECK(state->second == 1);
        CHECK(state->emptyBeforeReentry);
        REQUIRE(state->survivor);
        REQUIRE(gallery->GetCount() == 1);
        CHECK(gallery->GetItem(0) == state->survivor);
        CHECK(gallery->GetSelection() == nullptr);
        REQUIRE(gallery->Realize());
        fixture.Ready();
        CHECK(fixture.View().Items().Size() == 1);
        gallery->Clear();
        CHECK(state->first == 1);
        CHECK(state->second == 1);
    }
    SECTION("Owner destruction releases owned data once")
    {
        REQUIRE(gallery->Append(fixture.icon, FirstItemId,
            new GalleryClientData(countDestruction)));
        REQUIRE(gallery->Append(fixture.icon, FirstItemId + 1,
            new GalleryClientData(countDestruction)));
        delete gallery;
        CHECK_FALSE(fixture.gallery);
        CHECK(*destroyed == 2);
        DrainNativeGallery();
        CHECK(*destroyed == 2);
    }
}

TEST_CASE("RibbonGalleryNative::PreMountScrollPublishesToNativeViewport",
          "[winui-ribbon-toolkit][native-gallery][geometry]")
{
    NativeGalleryFixture fixture;
    fixture.Create(48);
    auto * const gallery = fixture.gallery.get();
    const auto events = RecordGalleryEvents(gallery);
    auto * const selection = gallery->GetItem(1);
    const wxSize minimum = gallery->GetMinSize();
    REQUIRE(minimum.x > 0);
    REQUIRE(minimum.y > 0);
    gallery->SetSize(minimum);
    REQUIRE(gallery->Realize());
    CHECK_FALSE(fixture.frame->IsShown());
    gallery->SetSelection(selection);
    CHECK_FALSE(gallery->ScrollLines(-1));
    CHECK(gallery->GetUpButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED);
    REQUIRE(gallery->ScrollLines(1));
    CHECK(gallery->GetUpButtonState() != wxRIBBON_GALLERY_BUTTON_DISABLED);
    REQUIRE(gallery->ScrollPixels(12));
    CHECK(gallery->GetSelection() == selection);
    CHECK(events->empty());

    // Item zero may already be virtualized after publishing the requested
    // offset. The second item still intersects this one-line-plus-12 viewport.
    fixture.Show(1);
    const auto scroll = FindGalleryElement<MUXC::ScrollViewer>(fixture.View());
    REQUIRE(scroll);
    const auto visibleItem = fixture.Item(1);
    REQUIRE(visibleItem);
    REQUIRE(WaitForNativeGallery([&]()
    {
        return visibleItem.IsLoaded() && visibleItem.ActualHeight() > 0;
    }));
    const double requestedOffset = visibleItem.ActualHeight() +
                                   12.0 / gallery->GetDPIScaleFactor();
    REQUIRE(WaitForNativeGallery([&]()
    {
        return std::abs(scroll.VerticalOffset() - requestedOffset) <= 1.0;
    }));
    CHECK(scroll.VerticalOffset() == Approx(requestedOffset).margin(1.0));
    CHECK(gallery->GetSelection() == selection);
    CHECK(gallery->GetUpButtonState() != wxRIBBON_GALLERY_BUTTON_DISABLED);

    unsigned forward = 0;
    while ( forward < 512 && gallery->ScrollLines(1) ) ++forward;
    REQUIRE(forward > 0);
    REQUIRE(forward < 512);
    CHECK_FALSE(gallery->ScrollLines(1));
    CHECK(gallery->GetDownButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED);
    REQUIRE(WaitForNativeGallery([&]()
    {
        return scroll.ScrollableHeight() > 0 &&
               scroll.VerticalOffset() >= scroll.ScrollableHeight() - 1;
    }));

    unsigned backward = 0;
    while ( backward < 512 && gallery->ScrollLines(-1) ) ++backward;
    REQUIRE(backward > 0);
    REQUIRE(backward < 512);
    CHECK_FALSE(gallery->ScrollPixels(-1));
    CHECK(gallery->GetUpButtonState() == wxRIBBON_GALLERY_BUTTON_DISABLED);
    REQUIRE(WaitForNativeGallery([&]() { return scroll.VerticalOffset() < 1; }));
    CHECK(gallery->GetSelection() == selection);
    CHECK(events->empty());
}

TEST_CASE("RibbonGalleryNative::PreMountEnsureVisibleSurvivesProjectionReplacement",
          "[winui-ribbon-toolkit][native-gallery][geometry]")
{
    const unsigned targetIndex = GENERATE(47u, 23u);
    CAPTURE(targetIndex);
    NativeGalleryFixture fixture;
    fixture.Create(48);
    auto * const gallery = fixture.gallery.get();
    const auto events = RecordGalleryEvents(gallery);
    auto * const selection = gallery->GetItem(1);
    auto * const target = gallery->GetItem(targetIndex);
    gallery->SetSize(gallery->GetMinSize());
    REQUIRE(gallery->Realize());
    CHECK_FALSE(fixture.frame->IsShown());
    gallery->SetSelection(selection);
    gallery->EnsureVisible(gallery->GetItem(47));
    gallery->EnsureVisible(target);
    // Force a different staged projection before any native viewport exists.
    // The requested monotonic item key, not the old container, must survive.
    REQUIRE(gallery->Append(fixture.icon, FirstItemId + 48));
    REQUIRE(gallery->Realize());
    CHECK(gallery->GetSelection() == selection);
    CHECK(events->empty());

    fixture.Show(targetIndex);
    const auto view = fixture.View();
    REQUIRE(view);
    const auto scroll = FindGalleryElement<MUXC::ScrollViewer>(view);
    REQUIRE(scroll);
    REQUIRE(WaitForNativeGallery([&]()
    {
        const auto item = fixture.Item(targetIndex);
        if ( !item || !item.IsLoaded() || item.ActualWidth() <= 0 ||
             item.ActualHeight() <= 0 ) return false;
        const auto bounds = item.TransformToVisual(view).TransformBounds(
            {0, 0, static_cast<float>(item.ActualWidth()),
             static_cast<float>(item.ActualHeight())});
        return bounds.X >= -1 && bounds.Y >= -1 &&
            bounds.X + bounds.Width <= view.ActualWidth() + 1 &&
            bounds.Y + bounds.Height <= view.ActualHeight() + 1;
    }));
    CHECK(scroll.VerticalOffset() > 0);
    CHECK(gallery->GetItem(targetIndex) == target);
    CHECK(gallery->GetSelection() == selection);
    CHECK(view.SelectedIndex() == 1);
    DrainNativeGallery();
    CHECK(events->empty());
}

TEST_CASE("RibbonGalleryNative::InstallFailureKeepsCommittedProjectionAndRetries",
          "[winui-ribbon-toolkit][native-gallery][transaction]")
{
    NativeGalleryFixture fixture;
    fixture.Create();
    fixture.Show();
    auto * const gallery = fixture.gallery.get();
    auto * const selected = gallery->GetItem(1);
    gallery->SetSelection(selected);
    const auto events = RecordGalleryEvents(gallery);
    const auto oldRoot = fixture.Root();
    const auto oldView = fixture.View();
    const auto oldItem = fixture.Item(0);
    const auto stale = fixture.SelectProvider(0);
    REQUIRE(stale);
    const auto oldOrigin = oldItem.TransformToVisual(oldRoot).TransformPoint({0, 0});
    const double oldWidth = oldItem.ActualWidth();
    const double oldHeight = oldItem.ActualHeight();
    const auto resetFault = wxMakeGuard([]()
    {
        wxWinUIHostContentTestAccess::FailContentSwap(0);
    });
    wxRibbonGalleryItem *added = nullptr;
    {
        wxLogNull suppressExpectedFailure;
        wxWinUIHostContentTestAccess::FailContentSwap(
            wxWinUIHostContentTestAccess::ContentFault_Install);
        added = gallery->Append(fixture.icon, FirstItemId + 3);
    }
    REQUIRE(added);
    CHECK(gallery->GetCount() == 4);
    CHECK(gallery->GetSelection() == selected);
    CHECK(fixture.Root() == oldRoot);
    CHECK(fixture.View() == oldView);
    CHECK(fixture.Item(0) == oldItem);
    CHECK(oldView.Items().Size() == 3);
    CHECK(oldView.SelectedIndex() == 1);
    const auto retainedOrigin = oldItem.TransformToVisual(oldRoot).TransformPoint({0, 0});
    CHECK(retainedOrigin.X == oldOrigin.X);
    CHECK(retainedOrigin.Y == oldOrigin.Y);
    CHECK(oldItem.ActualWidth() == oldWidth);
    CHECK(oldItem.ActualHeight() == oldHeight);
    ExerciseRetiredGalleryProvider([&]() { stale.Select(); });
    CHECK(events->empty());
    CHECK(gallery->GetSelection() == selected);
    REQUIRE(gallery->Realize());
    fixture.Ready();
    CHECK(fixture.Root() != oldRoot);
    CHECK(fixture.View().Items().Size() == 4);
    CHECK(gallery->GetSelection() == selected);
    CHECK(fixture.View().SelectedIndex() == 1);
    ExerciseRetiredGalleryProvider([&]() { stale.Select(); });
    CHECK(events->empty());
    const auto current = fixture.SelectProvider(0);
    REQUIRE(current);
    current.Select();
    REQUIRE(WaitForNativeGallery([&]() { return events->size() == 1; }));
    CheckGallerySelectionEvent(*events, 1, gallery, gallery->GetItem(0));
}

#endif // wxWinUI Toolkit Ribbon
