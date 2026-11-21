/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidataview.cpp
// Purpose:     Native WinUI DataView model/presentation contract
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
#include "wx/dataview.h"
#ifdef wxHAS_WINUI_DATAVIEWCTRL

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <UIAutomation.h>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = winrt::Microsoft::UI::Xaml::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;

template <typename Predicate> bool DrainDataView(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
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
        // wxYield uses a selective nested pump: it can starve native Flyout
        // closing work even while wx idle/pending callbacks keep advancing.
        // Exercise the actual dispatcher used by the running application.
        if ( remaining <= 0 || loop->DispatchTimeout(
                static_cast<unsigned long>(std::min(5L, remaining))) == 0 ) return false;
    }
}

template <typename T> T FindNative(const X::DependencyObject& element)
{
    if ( !element ) return nullptr;
    if ( auto found = element.try_as<T>() ) return found;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(element); ++n )
        if ( auto found = FindNative<T>(X::Media::VisualTreeHelper::GetChild(element, n)) )
            return found;
    return nullptr;
}

C::ListView NativeDataView(wxDataViewCtrl* view)
{
    auto host = wxWinUITopLevelHost::FindSlotOwner(view);
    if ( !host ) return nullptr;
    auto slot = host->FindSlot(view);
    return slot ? slot->GetSemanticTarget().try_as<C::ListView>() : nullptr;
}

class NativeModel : public wxDataViewVirtualListModel
{
public:
    explicit NativeModel(unsigned count) : wxDataViewVirtualListModel(count) {}
    unsigned GetCount() const override
    { ++countQueries; return wxDataViewVirtualListModel::GetCount(); }
    unsigned GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const override
    { ++childQueries; return wxDataViewVirtualListModel::GetChildren(item, children); }
    void GetValueByRow(wxVariant& value, unsigned row, unsigned col) const override
    {
        ++reads;
        ++cellReads[(static_cast<std::uint64_t>(row) << 32) | col];
        if ( onRead ) { auto callback = std::move(onRead); onRead = {}; callback(); }
        const auto it = values.find(row);
        value = col == 0 && it != values.end() ? it->second :
            wxString::Format("row %u column %u", row, col);
    }
    bool SetValueByRow(const wxVariant& value, unsigned row, unsigned) override
    {
        values[row] = value.GetString();
        return true;
    }
    int Compare(const wxDataViewItem& a, const wxDataViewItem& b,
                 unsigned col, bool ascending) const override
    {
        ++comparisons;
        if ( notifyWhileComparing ) const_cast<NativeModel*>(this)->Resort();
        if ( onCompare ) { auto callback = std::move(onCompare); onCompare = {}; callback(); }
        return wxDataViewVirtualListModel::Compare(a, b, col, ascending);
    }
    mutable unsigned reads = 0;
    mutable unsigned attrQueries = 0;
    mutable unsigned countQueries = 0;
    mutable unsigned childQueries = 0;
    mutable unsigned comparisons = 0;
    mutable std::unordered_map<std::uint64_t, unsigned> cellReads;
    mutable std::unordered_map<std::uint64_t, unsigned> cellAttrs;
    bool notifyWhileComparing = false;
    mutable std::function<void()> onRead;
    mutable std::function<void()> onCompare;
    std::unordered_map<unsigned, wxString> values;
    bool HasValue(const wxDataViewItem& item, unsigned col) const override
        { return !(emptyCell && GetRow(item) == 1 && col == 1); }
    bool emptyCell = false;
    bool GetAttr(const wxDataViewItem& item, unsigned col, wxDataViewItemAttr&) const override
    {
        ++attrQueries;
        ++cellAttrs[(static_cast<std::uint64_t>(GetRow(item)) << 32) | col];
        return false;
    }
};

class NativeBranchModel final : public wxDataViewModel
{
public:
    static wxDataViewItem Node(unsigned id)
    { return wxDataViewItem(reinterpret_cast<void*>(static_cast<std::uintptr_t>(id))); }
    static unsigned Id(const wxDataViewItem& item)
    { return static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(item.GetID())); }
    void GetValue(wxVariant& value, const wxDataViewItem& item, unsigned column) const override
    {
        ++reads.at(Id(item) * 2 + column);
        value = wxString::Format("tree %u column %u", Id(item), column);
    }
    bool SetValue(const wxVariant&, const wxDataViewItem&, unsigned) override { return false; }
    bool GetAttr(const wxDataViewItem& item, unsigned column, wxDataViewItemAttr&) const override
    { ++attrs.at(Id(item) * 2 + column); return false; }
    wxDataViewItem GetParent(const wxDataViewItem& item) const override
    { return Id(item) == 3 || Id(item) == 4 ? Node(2) : wxDataViewItem(); }
    bool IsContainer(const wxDataViewItem& item) const override
    { return !item.IsOk() || Id(item) == 2; }
    bool HasContainerColumns(const wxDataViewItem&) const override { return true; }
    unsigned GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& children) const override
    {
        ++enumerations;
        ++childrenQueries.at(Id(parent));
        if ( !parent.IsOk() ) { children.Add(Node(1)); children.Add(Node(2)); children.Add(Node(5)); }
        else if ( Id(parent) == 2 ) { children.Add(Node(3)); children.Add(Node(4)); }
        return static_cast<unsigned>(children.size());
    }
    mutable std::array<unsigned, 12> reads{}, attrs{};
    mutable std::array<unsigned, 6> childrenQueries{};
    mutable unsigned enumerations = 0;
};

class NativeTypedModel : public wxDataViewVirtualListModel
{
public:
    explicit NativeTypedModel(const wxVector<wxVariant>& initial)
        : wxDataViewVirtualListModel(1), values(initial) {}
    void GetValueByRow(wxVariant& value, unsigned, unsigned col) const override
    {
        value = col < values.size() ? values[col] : wxVariant();
    }
    bool SetValueByRow(const wxVariant& value, unsigned, unsigned col) override
    {
        if ( col >= values.size() || !acceptWrites ) return false;
        ++writes;
        values[col] = value;
        return true;
    }
    wxVector<wxVariant> values;
    unsigned writes = 0;
    bool acceptWrites = true;
    bool enabled = true;
    bool IsEnabled(const wxDataViewItem&, unsigned) const override { return enabled; }
};

X::DependencyObject NativeDataViewRoot(wxDataViewCtrl* view)
{
    auto host = wxWinUITopLevelHost::FindSlotOwner(view);
    auto slot = host ? host->FindSlot(view) : nullptr;
    return slot ? slot->GetContent() : nullptr;
}

bool HasNativeText(const X::DependencyObject& root, const wchar_t* value)
{
    if ( !root ) return false;
    if ( auto text = root.try_as<C::TextBlock>() )
        if ( text.Text() == value ) return true;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( HasNativeText(X::Media::VisualTreeHelper::GetChild(root, n), value) ) return true;
    return false;
}

bool ContainsNativeFocus(const X::FrameworkElement& root)
{
    if ( !root || !root.XamlRoot() ) return false;
    auto focused = X::Input::FocusManager::GetFocusedElement(root.XamlRoot()).try_as<X::DependencyObject>();
    while ( focused )
    {
        if ( focused == root ) return true;
        focused = X::Media::VisualTreeHelper::GetParent(focused);
    }
    return false;
}

struct NativeFixture
{
    explicit NativeFixture(long style = wxDV_MULTIPLE)
    {
        // CTest's isolated desktop is the qualified rendering path. XAML can
        // defer layout indefinitely for off-screen roots: keep the fixture
        // in that private viewport, never on the user's input desktop.
        wxPoint position(-30000, -30000);
        wchar_t desktopName[256]{};
        DWORD needed = 0;
        const auto desktop = ::GetThreadDesktop(::GetCurrentThreadId());
        if ( ::GetUserObjectInformationW(desktop, UOI_NAME,
                desktopName, sizeof(desktopName), &needed) &&
             wxString(desktopName).StartsWith("wxWinUITest-") )
            position = wxPoint(80, 80);
        frame.reset(new wxFrame(nullptr, wxID_ANY, "WinUI DataView contract",
                                 position, wxSize(620, 260)));
        std::unique_ptr<wxDataViewCtrl> created(new wxDataViewCtrl);
        REQUIRE(created->Create(frame.get(), wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, style));
        view = created.release();
        auto sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(view, 1, wxEXPAND);
        frame->SetSizer(sizer);
        frame->ShowWithoutActivating();
        frame->Layout();
        view->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, [this](wxDataViewEvent& event)
        { ++starting; event.Skip(); });
        view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, [this](wxDataViewEvent& event)
        { ++started; event.Skip(); });
        view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [this](wxDataViewEvent& event)
        { ++finished; event.Skip(); });
    }
    void SetModel(wxDataViewModel* model)
    {
        REQUIRE(view->AssociateModel(model));
        model->DecRef();
    }
    C::ListView Loaded()
    {
        const bool loaded = DrainDataView([&]
        {
            const auto list = NativeDataView(view);
            return list && list.IsLoaded() && list.ActualWidth() > 0 && list.ItemsPanelRoot();
        });
        const auto list = NativeDataView(view);
        auto host = wxWinUITopLevelHost::FindSlotOwner(view);
        auto slot = host ? host->FindSlot(view) : nullptr;
        auto root = slot ? slot->GetContent().try_as<X::FrameworkElement>() : nullptr;
        auto presentation = slot ? slot->GetPresentationRoot() : nullptr;
        auto direct = slot ? slot->GetVisualRoot().try_as<X::FrameworkElement>() : nullptr;
        auto xamlRoot = host ? host->GetXamlRoot() : nullptr;
        auto top = xamlRoot ? xamlRoot.Content().try_as<X::FrameworkElement>() : nullptr;
        RECT frameClient{}, bridgeClient{};
        const HWND bridge = host ? host->GetBridgeHwnd() : nullptr;
        ::GetClientRect(static_cast<HWND>(frame->GetHandle()), &frameClient);
        if ( bridge ) ::GetClientRect(bridge, &bridgeClient);
        INFO("wx view=" << view << " frame=" << frame.get()
             << " shown=" << frame->IsShownOnScreen() << '/' << view->IsShownOnScreen()
             << " client=" << view->GetClientSize()
             << " host=" << host << " slot=" << slot
             << " list=" << static_cast<bool>(list)
             << " listLoaded=" << (list && list.IsLoaded())
             << " listSize=" << (list ? list.ActualWidth() : 0) << 'x'
             << (list ? list.ActualHeight() : 0)
             << " panel=" << (list && list.ItemsPanelRoot())
             << " rootLoaded=" << (root && root.IsLoaded())
             << " rootSize=" << (root ? root.ActualWidth() : 0) << 'x'
             << (root ? root.ActualHeight() : 0)
             << " presentationLoaded=" << (presentation && presentation.IsLoaded())
             << " presentationVisibility=" << (presentation ? static_cast<int>(presentation.Visibility()) : -1)
             << " presentationSize=" << (presentation ? presentation.ActualWidth() : 0) << 'x'
             << (presentation ? presentation.ActualHeight() : 0)
             << " directLoaded=" << (direct && direct.IsLoaded())
             << " directSize=" << (direct ? direct.ActualWidth() : 0) << 'x'
             << (direct ? direct.ActualHeight() : 0)
             << " rootCanvasLoaded=" << (top && top.IsLoaded())
             << " rootCanvasSize=" << (top ? top.ActualWidth() : 0) << 'x'
             << (top ? top.ActualHeight() : 0)
             << " presentationRequested=" << (presentation ? presentation.Width() : 0) << 'x'
             << (presentation ? presentation.Height() : 0)
             << " frameHWND=" << frame->GetHandle() << " bridgeHWND=" << bridge
             << " frameClient=" << frameClient.right << 'x' << frameClient.bottom
             << " bridgeClient=" << bridgeClient.right << 'x' << bridgeClient.bottom
             << " xamlRootSize=" << (xamlRoot ? xamlRoot.Size().Width : 0) << 'x'
             << (xamlRoot ? xamlRoot.Size().Height : 0));
        REQUIRE(loaded);
        return NativeDataView(view);
    }
    template <typename T> T EditAs(const wxDataViewItem& item, wxDataViewColumn* column)
    {
        starting = started = finished = 0;
        auto list = Loaded();
        view->EditItem(item, column);
        T editor{nullptr};
        const bool found = DrainDataView([&]
        {
            editor = FindNative<T>(NativeDataViewRoot(view));
            return editor != nullptr;
        });
        auto row = list.ContainerFromIndex(0).try_as<C::ListViewItem>();
        auto canvas = row ? row.ContentTemplateRoot().try_as<C::Canvas>() : nullptr;
        INFO("start/started/done=" << starting << '/' << started << '/' << finished
             << " list loaded=" << list.IsLoaded()
             << " size=" << list.ActualWidth() << 'x' << list.ActualHeight()
             << " canvas children=" << (canvas ? canvas.Children().Size() : 0));
        REQUIRE(found);
        CHECK(starting == 1);
        CHECK(started == 1);
        return editor;
    }
    C::TextBox Edit(const wxDataViewItem& item, wxDataViewColumn* column)
    {
        return EditAs<C::TextBox>(item, column);
    }
    std::unique_ptr<wxFrame> frame;
    wxDataViewCtrl* view;
    unsigned starting = 0, started = 0, finished = 0;
};

A::AutomationPeer ProviderPeer(const A::AutomationPeer& grid,
                               const P::IRawElementProviderSimple& provider)
{
    REQUIRE(provider);
    auto peer = grid.as<A::IAutomationPeerProtected>().PeerFromProvider(provider);
    REQUIRE(peer);
    return peer;
}

void CheckUnavailable(const std::function<void()>& action)
{
    bool unavailable = false;
    try { action(); }
    catch ( const winrt::hresult_error& error )
        { unavailable = error.code() == UIA_E_ELEMENTNOTAVAILABLE; }
    CHECK(unavailable);
}

void CheckUIAError(HRESULT expected, const std::function<void()>& action)
{
    HRESULT actual = S_OK;
    try { action(); }
    catch ( const winrt::hresult_error& error ) { actual = error.code(); }
    CHECK(actual == expected);
}

A::AutomationPeer DataViewCell(NativeFixture& fixture, int row, int column)
{
    const auto nativePeer = A::FrameworkElementAutomationPeer::CreatePeerForElement(fixture.Loaded());
    const auto grid = nativePeer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
    return ProviderPeer(nativePeer, grid.GetItem(row, column));
}

C::Canvas RowCanvas(const C::ListView& list, int row)
{
    const auto container = list.ContainerFromIndex(row).try_as<C::ListViewItem>();
    return container ? container.ContentTemplateRoot().try_as<C::Canvas>() : nullptr;
}
}

TEST_CASE("WinUI DataView keeps native cells mounted across selection focus and unchanged viewport",
          "[dataview][winui][winui-dataview][winui-dataview-churn]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 180));
    REQUIRE(fixture.view->AppendTextColumn("Other", 1, wxDATAVIEW_CELL_INERT, 180));
    auto model = new NativeModel(100);
    fixture.SetModel(model);
    const auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&]
    {
        const auto first = RowCanvas(list, 0), second = RowCanvas(list, 1);
        return first && second && first.Children().Size() == 2 && second.Children().Size() == 2 &&
               first.Children().GetAt(1).as<X::FrameworkElement>().IsLoaded();
    }));
    const auto first = RowCanvas(list, 0), second = RowCanvas(list, 1);
    const auto cell00 = first.Children().GetAt(0), cell01 = first.Children().GetAt(1);
    const auto cell10 = second.Children().GetAt(0), cell11 = second.Children().GetAt(1);
    unsigned neighbourUnloads = 0;
    const auto unloadToken = cell01.as<X::FrameworkElement>().Unloaded(
        [&](const auto&, const auto&) { ++neighbourUnloads; });
    auto unloadGuard = wxMakeGuard([&] { cell01.as<X::FrameworkElement>().Unloaded(unloadToken); });
    wxUnusedVar(unloadGuard);
    const auto realize = DataViewCell(fixture, 0, 0).GetPattern(A::PatternInterface::VirtualizedItem)
                            .as<P::IVirtualizedItemProvider>();
    const unsigned reads = model->reads;
    const unsigned attrs = model->attrQueries;
    const unsigned counts = model->countQueries, children = model->childQueries;
    const auto initialCellReads = model->cellReads;
    const auto initialCellAttrs = model->cellAttrs;
    fixture.view->SetFocus();
    for ( unsigned n = 0; n != 3; ++n )
    {
        list.SelectedItem(list.Items().GetAt(0));
        list.SelectedItem(list.Items().GetAt(1));
        fixture.view->EnsureVisible(model->GetItem(0));
        // This is a real UIA viewport realization request, not a private test
        // seam: already realized cells must not be disconnected and recreated.
        realize.Realize();
    }
    // ListView may extend its cache beyond the initially visible rows during
    // the first interaction. Permit one initial read per newly realized cell,
    // but no repeat value/attribute read of any existing presentation.
    for ( const auto& entry : model->cellReads )
    {
        const auto old = initialCellReads.find(entry.first);
        INFO("value row=" << (entry.first >> 32) << " col=" << (entry.first & 0xffffffff));
        CHECK(entry.second == (old == initialCellReads.end() ? 1 : old->second));
    }
    for ( const auto& entry : model->cellAttrs )
    {
        const auto old = initialCellAttrs.find(entry.first);
        INFO("attribute row=" << (entry.first >> 32) << " col=" << (entry.first & 0xffffffff));
        CHECK(entry.second == (old == initialCellAttrs.end() ? 1 : old->second));
    }
    CHECK(first == RowCanvas(list, 0));
    CHECK(second == RowCanvas(list, 1));
    CHECK(first.Children().GetAt(0) == cell00);
    CHECK(first.Children().GetAt(1) == cell01);
    CHECK(second.Children().GetAt(0) == cell10);
    CHECK(second.Children().GetAt(1) == cell11);
    CHECK(model->reads - reads == model->cellReads.size() - initialCellReads.size());
    CHECK(model->attrQueries - attrs == model->cellAttrs.size() - initialCellAttrs.size());
    CHECK(model->countQueries == counts);
    CHECK(model->childQueries == children);

    const unsigned readsAfterInteraction = model->reads;
    const unsigned attrsAfterInteraction = model->attrQueries;
    model->values[0] = "targeted update";
    REQUIRE(model->ValueChanged(model->GetItem(0), 0));
    CHECK(HasNativeText(first, L"targeted update"));
    CHECK(first.Children().GetAt(1) == cell01);
    CHECK(second.Children().GetAt(0) == cell10);
    CHECK(second.Children().GetAt(1) == cell11);
    CHECK(model->reads == readsAfterInteraction + 1);
    CHECK(model->attrQueries == attrsAfterInteraction + 1);
    CHECK(model->countQueries == counts);
    CHECK(model->childQueries == children);
    CHECK(neighbourUnloads == 0);
    CHECK_FALSE(X::Media::VisualTreeHelper::GetParent(cell00));
}

TEST_CASE("WinUI DataView expanding one branch retains native rows outside that branch",
          "[dataview][winui][winui-dataview][winui-dataview-churn][native-branch-update]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Tree", 0, wxDATAVIEW_CELL_INERT, 180));
    REQUIRE(fixture.view->AppendTextColumn("Value", 1, wxDATAVIEW_CELL_INERT, 180));
    auto* const model = new NativeBranchModel;
    fixture.SetModel(model);
    const auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&]
    {
        for ( int row = 0; row < 3; ++row )
        {
            const auto canvas = RowCanvas(list, row);
            if ( !canvas || canvas.Children().Size() != 2 ||
                 !canvas.Children().GetAt(1).as<X::FrameworkElement>().IsLoaded() ) return false;
        }
        return FindNative<C::Button>(RowCanvas(list, 1)) != nullptr;
    }));
    const auto prefixContainer = list.ContainerFromIndex(0);
    const auto suffixContainer = list.ContainerFromIndex(2);
    const auto prefix = RowCanvas(list, 0), suffix = RowCanvas(list, 2);
    const auto prefix0 = prefix.Children().GetAt(0), prefix1 = prefix.Children().GetAt(1);
    const auto suffix0 = suffix.Children().GetAt(0), suffix1 = suffix.Children().GetAt(1);
    const auto reads = model->reads, attrs = model->attrs;
    unsigned unloaded = 0;
    const auto token = suffix.Unloaded([&](const auto&, const auto&) { ++unloaded; });
    auto cleanup = wxMakeGuard([&] { suffix.Unloaded(token); });
    wxUnusedVar(cleanup);
    for ( const bool expand : {true, false, true, false} )
    {
        CAPTURE(expand);
        const auto childrenQueries = model->childrenQueries;
        const auto disclosure = FindNative<C::Button>(RowCanvas(list, 1));
        REQUIRE(disclosure);
        // Exercise the real XAML disclosure callback, including its retained
        // model identity. No synthetic model notification or Refresh is used.
        A::ButtonAutomationPeer(disclosure).Invoke();
        const int suffixIndex = expand ? 4 : 2;
        REQUIRE(DrainDataView([&]
        {
            const auto current = RowCanvas(list, suffixIndex);
            return list.Items().Size() == (expand ? 5u : 3u) && current &&
                   current.Children().Size() == 2 && HasNativeText(current, L"tree 5 column 0");
        }));
        CHECK(fixture.view->IsExpanded(NativeBranchModel::Node(2)) == expand);
        CHECK(list.ContainerFromIndex(0) == prefixContainer);
        CHECK(list.ContainerFromIndex(suffixIndex) == suffixContainer);
        CHECK(RowCanvas(list, 0) == prefix);
        CHECK(RowCanvas(list, suffixIndex) == suffix);
        CHECK(prefix.Children().GetAt(0) == prefix0);
        CHECK(prefix.Children().GetAt(1) == prefix1);
        CHECK(suffix.Children().GetAt(0) == suffix0);
        CHECK(suffix.Children().GetAt(1) == suffix1);
        CHECK(unloaded == 0);
        for ( unsigned id = 0; id < model->childrenQueries.size(); ++id )
        {
            CAPTURE(id);
            if ( expand && id == 2 ) CHECK(model->childrenQueries[id] > childrenQueries[id]);
            else CHECK(model->childrenQueries[id] == childrenQueries[id]);
        }
        for ( const unsigned id : {1u, 5u} )
            for ( unsigned column = 0; column < 2; ++column )
            {
                CAPTURE(id, column);
                CHECK(model->reads[id * 2 + column] == reads[id * 2 + column]);
                CHECK(model->attrs[id * 2 + column] == attrs[id * 2 + column]);
            }
    }
}

TEST_CASE("WinUI DataView viewport resize follows the sizer without rereading surviving cells",
          "[dataview][winui][winui-dataview][winui-dataview-churn][native-live-resize]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 180));
    REQUIRE(fixture.view->AppendTextColumn("Value", 1, wxDATAVIEW_CELL_INERT, 160));
    auto* const model = new NativeModel(100);
    fixture.SetModel(model);
    const auto list = fixture.Loaded();
    const auto scroll = FindNative<C::ScrollViewer>(list);
    REQUIRE(scroll);
    fixture.view->EnsureVisible(model->GetItem(25));
    REQUIRE(DrainDataView([&]
    {
        const auto row = RowCanvas(list, 25);
        return row && row.Children().Size() == 2 && scroll.VerticalOffset() > 0 &&
               row.Children().GetAt(1).as<X::FrameworkElement>().IsLoaded();
    }));
    const auto container = list.ContainerFromIndex(25);
    const auto row = RowCanvas(list, 25);
    const auto first = row.Children().GetAt(0), second = row.Children().GetAt(1);
    const double vertical = scroll.VerticalOffset(), horizontal = scroll.HorizontalOffset();
    const double scale = fixture.view->GetDPIScaleFactor();
    for ( const wxSize size : {wxSize(510, 240), wxSize(740, 280), wxSize(570, 250)} )
    {
        CAPTURE(size);
        const auto reads = model->cellReads, attrs = model->cellAttrs;
        std::unordered_set<std::uint64_t> presented;
        for ( int index = 0; index < 100; ++index )
        {
            const auto canvas = RowCanvas(list, index);
            if ( !canvas || !canvas.IsLoaded() || canvas.Children().Size() != 2 ) continue;
            for ( unsigned column = 0; column < 2; ++column )
                presented.insert((static_cast<std::uint64_t>(index) << 32) | column);
        }
        fixture.frame->SetClientSize(size);
        // Do not call the child's Refresh(), Update(), Layout() or a second
        // interaction: its ordinary wx size notification must update XAML.
        REQUIRE(DrainDataView([&]
        {
            const wxSize client = fixture.view->GetClientSize();
            const auto current = RowCanvas(list, 25);
            return client == size && current &&
                   std::abs(list.ActualWidth() - client.x / scale) < 1 &&
                   std::abs(current.ActualWidth() - list.ActualWidth()) < 1;
        }));
        CHECK(list.ContainerFromIndex(25) == container);
        CHECK(RowCanvas(list, 25) == row);
        CHECK(row.Children().GetAt(0) == first);
        CHECK(row.Children().GetAt(1) == second);
        CHECK(scroll.VerticalOffset() == Approx(vertical));
        CHECK(scroll.HorizontalOffset() == Approx(horizontal));
        // Viewport growth may realize genuinely new rows. It must never
        // invalidate already presented values/attributes just for geometry.
        for ( const auto& entry : model->cellReads )
        {
            const auto old = reads.find(entry.first);
            CAPTURE(entry.first);
            if ( old == reads.end() ) CHECK(entry.second == 1);
            else if ( presented.count(entry.first) ) CHECK(entry.second == old->second);
            else CHECK(entry.second <= old->second + 1);
        }
        for ( const auto& entry : model->cellAttrs )
        {
            const auto old = attrs.find(entry.first);
            CAPTURE(entry.first);
            if ( old == attrs.end() ) CHECK(entry.second == 1);
            else if ( presented.count(entry.first) ) CHECK(entry.second == old->second);
            else CHECK(entry.second <= old->second + 1);
        }
    }
}

TEST_CASE("WinUI DataView retained neighbour callbacks survive a targeted value notification",
          "[dataview][winui][winui-dataview][winui-dataview-churn]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 180));
    REQUIRE(fixture.view->AppendToggleColumn("Check", 1, wxDATAVIEW_CELL_ACTIVATABLE, 180));
    auto model = new NativeTypedModel({wxVariant("original"), wxVariant(false)});
    fixture.SetModel(model);
    const auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return FindNative<C::CheckBox>(RowCanvas(list, 0)) != nullptr; }));
    const auto canvas = RowCanvas(list, 0);
    const auto check = FindNative<C::CheckBox>(canvas);
    model->values[0] = wxVariant("changed");
    REQUIRE(model->ValueChanged(model->GetItem(0), 0));
    CHECK(HasNativeText(canvas, L"changed"));
    CHECK(FindNative<C::CheckBox>(canvas) == check);
    A::CheckBoxAutomationPeer(check).Toggle();
    CHECK(model->values[1].GetBool());
    CHECK(model->writes == 1);
    const auto current = FindNative<C::CheckBox>(canvas);
    REQUIRE(current);
    CHECK(current.IsChecked().Value());
    CHECK(current != check);
    // Retaining a detached native presentation must not resurrect its handler.
    A::CheckBoxAutomationPeer(check).Toggle();
    CHECK(model->writes == 1);
}

TEST_CASE("WinUI DataView invalidates custom selection and geometry without replacing native neighbours",
          "[dataview][winui][winui-dataview][winui-dataview-churn]")
{
    class StateRenderer : public wxDataViewCustomRenderer
    {
    public:
        StateRenderer() : wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT) {}
        bool SetValue(const wxVariant& value) override { m_value = value; return true; }
        bool GetValue(wxVariant& value) const override { value = m_value; return true; }
        wxSize GetSize() const override { return wxSize(100, 20); }
        bool Render(wxRect, wxDC*, int state) override
        { ++renders; lastState = state; return true; }
        unsigned renders = 0;
        int lastState = 0;
        wxVariant m_value;
    };
    NativeFixture fixture;
    auto renderer = new StateRenderer;
    REQUIRE(fixture.view->AppendColumn(new wxDataViewColumn("Custom", renderer, 0, 180)));
    const auto nativeColumn = fixture.view->AppendTextColumn("Native", 1, wxDATAVIEW_CELL_INERT, 180);
    REQUIRE(nativeColumn);
    fixture.SetModel(new NativeTypedModel({wxVariant("custom"), wxVariant("native")}));
    const auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { const auto row = RowCanvas(list, 0); return row && row.Children().Size() == 2; }));
    const auto row = RowCanvas(list, 0);
    const auto nativeCell = row.Children().GetAt(1);
    const unsigned renders = renderer->renders;
    list.SelectedItem(list.Items().GetAt(0));
    CHECK(renderer->renders > renders);
    CHECK((renderer->lastState & wxDATAVIEW_CELL_SELECTED) != 0);
    CHECK(row.Children().GetAt(1) == nativeCell);
    fixture.view->SetFocus();
    REQUIRE(DrainDataView([&] { return ContainsNativeFocus(list); }));
    CHECK((renderer->lastState & wxDATAVIEW_CELL_FOCUSED) != 0);
    CHECK(row.Children().GetAt(1) == nativeCell);
    const unsigned rendersBeforeHeight = renderer->renders;
    REQUIRE(fixture.view->SetRowHeight(48));
    CHECK(row.Children().GetAt(1).as<X::FrameworkElement>().Height() == 48 / fixture.view->GetDPIScaleFactor());
    // The custom wxDC surface needs fresh pixels for its new height, while
    // the adjacent native cell changes geometry on the same XAML object.
    CHECK(renderer->renders > rendersBeforeHeight);
    CHECK(row.Children().GetAt(1) == nativeCell);
    const unsigned rendersBeforeNativeWidth = renderer->renders;
    nativeColumn->SetWidth(240);
    CHECK(row.Children().GetAt(1).as<X::FrameworkElement>().Width() >= 240 / fixture.view->GetDPIScaleFactor());
    CHECK(row.Children().GetAt(1) == nativeCell);
    CHECK(renderer->renders == rendersBeforeNativeWidth);
}

TEST_CASE("WinUI DataView active TextBox survives column and viewport geometry changes",
          "[dataview][winui][winui-dataview][winui-dataview-churn][native-live-resize][native-editor]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeFixture fixture;
    if ( rtl ) fixture.view->SetLayoutDirection(wxLayout_RightToLeft);
    auto* const prefix = fixture.view->AppendTextColumn("Prefix", 0, wxDATAVIEW_CELL_INERT, 180);
    auto* const edited = fixture.view->AppendTextColumn("Edit", 1, wxDATAVIEW_CELL_EDITABLE, 160);
    REQUIRE(prefix);
    REQUIRE(edited);
    auto* const model = new NativeTypedModel({wxVariant("prefix"), wxVariant("original")});
    fixture.SetModel(model);
    const auto editor = fixture.Edit(model->GetItem(0), edited);
    REQUIRE(DrainDataView([&] { return editor.IsLoaded() && editor.ActualWidth() > 0; }));
    const auto list = fixture.Loaded();
    const auto row = RowCanvas(list, 0);
    REQUIRE(row);
    const auto mount = X::Media::VisualTreeHelper::GetParent(editor);
    editor.Text(L"candidate survives live geometry");
    const double scale = fixture.view->GetDPIScaleFactor();
    for ( const wxSize widths : {wxSize(230, 210), wxSize(150, 190)} )
    {
        CAPTURE(widths);
        prefix->SetWidth(widths.x);
        edited->SetWidth(widths.y);
        // The last column fills the remaining viewport when its specified
        // width is smaller. The editor follows this public effective width.
        CHECK(edited->GetWidth() == std::max(widths.y,
            fixture.view->GetClientSize().x - widths.x));
        const bool geometryReady = DrainDataView([&]
        {
            return editor.IsLoaded() &&
                   std::abs(editor.ActualWidth() - edited->GetWidth() / scale) < 1 &&
                   std::abs(C::Canvas::GetLeft(editor) - widths.x / scale) < 1;
        });
        INFO("column geometry: scale=" << scale
             << " requested=" << widths
             << " effective=" << prefix->GetWidth() << '/' << edited->GetWidth()
             << " specified=" << prefix->WXGetSpecifiedWidth() << '/' << edited->WXGetSpecifiedWidth()
             << " loaded=" << editor.IsLoaded()
             << " width/actual=" << editor.Width() << '/' << editor.ActualWidth()
             << " left/top=" << C::Canvas::GetLeft(editor) << '/' << C::Canvas::GetTop(editor)
             << " sameMount=" << (X::Media::VisualTreeHelper::GetParent(editor) == mount)
             << " hasParent=" << static_cast<bool>(X::Media::VisualTreeHelper::GetParent(editor))
             << " inNativeTree=" << (FindNative<C::TextBox>(NativeDataViewRoot(fixture.view)) == editor)
             << " focused=" << ContainsNativeFocus(editor)
             << " finished/writes=" << fixture.finished << '/' << model->writes
             << " text=" << wxString(editor.Text().c_str())
             << " model=" << model->values[1].GetString()
             << " client=" << fixture.view->GetClientSize()
             << " row/listWidth=" << row.ActualWidth() << '/' << list.ActualWidth());
        REQUIRE(geometryReady);
        CHECK(FindNative<C::TextBox>(NativeDataViewRoot(fixture.view)) == editor);
        CHECK(X::Media::VisualTreeHelper::GetParent(editor) == mount);
        CHECK(editor.Text() == L"candidate survives live geometry");
        CHECK(fixture.finished == 0);
        CHECK(model->writes == 0);
    }
    for ( const wxSize size : {wxSize(760, 300), wxSize(540, 240)} )
    {
        CAPTURE(size);
        fixture.frame->SetClientSize(size);
        const bool geometryReady = DrainDataView([&]
        {
            return fixture.view->GetClientSize() == size && editor.IsLoaded() &&
                   std::abs(list.ActualWidth() - size.x / scale) < 1 &&
                   std::abs(row.ActualWidth() - list.ActualWidth()) < 1 &&
                   std::abs(editor.ActualWidth() - edited->GetWidth() / scale) < 1 &&
                   std::abs(C::Canvas::GetLeft(editor) - prefix->GetWidth() / scale) < 1;
        });
        INFO("viewport geometry: requested=" << size
             << " client=" << fixture.view->GetClientSize()
             << " row/listWidth=" << row.ActualWidth() << '/' << list.ActualWidth()
             << " effective=" << prefix->GetWidth() << '/' << edited->GetWidth()
             << " specified=" << prefix->WXGetSpecifiedWidth() << '/' << edited->WXGetSpecifiedWidth()
             << " loaded=" << editor.IsLoaded()
             << " width/actual=" << editor.Width() << '/' << editor.ActualWidth()
             << " left/top=" << C::Canvas::GetLeft(editor) << '/' << C::Canvas::GetTop(editor)
             << " sameMount=" << (X::Media::VisualTreeHelper::GetParent(editor) == mount)
             << " finished/writes=" << fixture.finished << '/' << model->writes
             << " text=" << wxString(editor.Text().c_str()));
        REQUIRE(geometryReady);
        CHECK(FindNative<C::TextBox>(NativeDataViewRoot(fixture.view)) == editor);
        CHECK(X::Media::VisualTreeHelper::GetParent(editor) == mount);
        CHECK(editor.Text() == L"candidate survives live geometry");
        CHECK(fixture.finished == 0);
        CHECK(model->writes == 0);
        CHECK(model->values[1].GetString() == "original");
    }
    REQUIRE(edited->GetRenderer()->FinishEditing());
    CHECK(fixture.finished == 1);
    CHECK(model->writes == 1);
    CHECK(model->values[0].GetString() == "prefix");
    CHECK(model->values[1].GetString() == "candidate survives live geometry");
    REQUIRE(DrainDataView([&]
    {
        return !FindNative<C::TextBox>(NativeDataViewRoot(fixture.view)) &&
               HasNativeText(RowCanvas(list, 0), L"candidate survives live geometry");
    }));
}

TEST_CASE("WinUI DataView UIA Value realizes a virtual cell and uses its native edit transaction",
          "[dataview][winui][winui-dataview][winui-dataview-value]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Value", 0, wxDATAVIEW_CELL_EDITABLE, 220));
    auto model = new NativeModel(10000);
    fixture.SetModel(model);
    auto peer = DataViewCell(fixture, 9999, 0);
    auto value = peer.GetPattern(A::PatternInterface::Value).try_as<P::IValueProvider>();
    REQUIRE(value);
    CHECK_FALSE(value.IsReadOnly());
    CHECK(value.Value() == L"row 9999 column 0");
    CHECK(peer.IsOffscreen());
    C::TextBox actualEditor{nullptr};
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, [&](wxDataViewEvent& event)
    {
        actualEditor = FindNative<C::TextBox>(NativeDataViewRoot(fixture.view));
        REQUIRE(actualEditor);
        CHECK(actualEditor.Text() == L"row 9999 column 0");
        event.Skip();
    });
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [&](wxDataViewEvent& event)
    {
        CHECK_FALSE(event.IsEditCancelled());
        CHECK(event.GetValue().GetString() == "from UIA");
        CHECK(model->values.empty());
        event.Skip();
    });
    value.SetValue(L"from UIA");
    CHECK(fixture.starting == 1);
    CHECK(fixture.started == 1);
    CHECK(fixture.finished == 1);
    CHECK(model->values[9999] == "from UIA");
    CHECK(value.Value() == L"from UIA");
    REQUIRE(actualEditor);
    CHECK(actualEditor.Text() == L"from UIA");
    CHECK_FALSE(X::Media::VisualTreeHelper::GetParent(actualEditor));
    CHECK(model->reads < 1000);
}

TEST_CASE("WinUI DataView UIA Value obeys readonly disabled and retired identities",
          "[dataview][winui][winui-dataview][winui-dataview-value]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Read only", 0, wxDATAVIEW_CELL_INERT, 180));
    auto editable = fixture.view->AppendTextColumn("Editable", 1, wxDATAVIEW_CELL_EDITABLE, 180);
    REQUIRE(editable);
    auto model = new NativeTypedModel({wxVariant("read only"), wxVariant("original")});
    fixture.SetModel(model);
    auto readOnly = DataViewCell(fixture, 0, 0).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    auto editablePeer = DataViewCell(fixture, 0, 1);
    auto value = editablePeer.GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    CHECK(readOnly.IsReadOnly());
    CheckUIAError(UIA_E_INVALIDOPERATION, [&] { readOnly.SetValue(L"not allowed"); });
    model->enabled = false;
    CHECK_FALSE(editablePeer.IsEnabled());
    CheckUIAError(UIA_E_ELEMENTNOTENABLED, [&] { value.SetValue(L"not allowed"); });
    model->enabled = true;
    fixture.view->Enable(false);
    CheckUIAError(UIA_E_ELEMENTNOTENABLED, [&] { value.SetValue(L"not allowed"); });
    fixture.view->Enable(true);
    CHECK(fixture.started == 0);
    CHECK(model->writes == 0);
    editable->SetHidden(true);
    CheckUnavailable([&] { value.SetValue(L"stale"); });
    editable->SetHidden(false);
    auto live = DataViewCell(fixture, 0, 1).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    fixture.frame.reset();
    CheckUnavailable([&] { live.SetValue(L"destroyed"); });
    CheckUnavailable([&] { (void)live.Value(); });
}

TEST_CASE("WinUI DataView UIA Value preserves native Choice Spin and Date variant types",
          "[dataview][winui][winui-dataview][winui-dataview-value]")
{
    NativeFixture fixture;
    const wxArrayString choices{"First", "Duplicate", "Duplicate", "Last"};
    REQUIRE(fixture.view->AppendColumn(new wxDataViewColumn("Choice", new wxDataViewChoiceRenderer(choices), 0, 160)));
    REQUIRE(fixture.view->AppendColumn(new wxDataViewColumn("Index", new wxDataViewChoiceByIndexRenderer(choices), 1, 160)));
    wxVector<wxVariant> values{wxVariant("First"), wxVariant(2L)};
#if wxUSE_SPINCTRL
    const unsigned spinIndex = static_cast<unsigned>(values.size());
    REQUIRE(fixture.view->AppendColumn(new wxDataViewColumn("Spin", new wxDataViewSpinRenderer(-3, 12), spinIndex, 160)));
    values.push_back(wxVariant(4L));
#endif
#if wxUSE_DATEPICKCTRL
    const unsigned dateIndex = static_cast<unsigned>(values.size());
    REQUIRE(fixture.view->AppendColumn(new wxDataViewColumn("Date", new wxDataViewDateRenderer, dateIndex, 240)));
    values.push_back(wxVariant(wxDateTime(29, wxDateTime::Feb, 1600)));
#endif
    auto model = new NativeTypedModel(values);
    fixture.SetModel(model);
    unsigned comboEditors = 0, spinEditors = 0, dateEditors = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, [&](wxDataViewEvent& event)
    {
        const auto root = NativeDataViewRoot(fixture.view);
        if ( FindNative<C::ComboBox>(root) ) ++comboEditors;
        if ( FindNative<C::NumberBox>(root) ) ++spinEditors;
        if ( FindNative<C::CalendarDatePicker>(root) ) ++dateEditors;
        event.Skip();
    });
    auto choice = DataViewCell(fixture, 0, 0).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    auto index = DataViewCell(fixture, 0, 1).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    choice.SetValue(L"Last");
    CHECK(model->values[0].GetType() == "string");
    CHECK(model->values[0].GetString() == "Last");
    index.SetValue(L"Duplicate");
    CHECK(model->values[1].GetType() == "long");
    CHECK(model->values[1].GetLong() == 2);
    index.SetValue(L"First");
    CHECK(model->values[1].GetLong() == 0);
    CHECK(comboEditors == 3);
    CheckUIAError(E_INVALIDARG, [&] { choice.SetValue(L"missing choice"); });
#if wxUSE_SPINCTRL
    auto spinPeer = DataViewCell(fixture, 0, spinIndex);
    auto spinValue = spinPeer.GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    auto range = spinPeer.GetPattern(A::PatternInterface::RangeValue).try_as<P::IRangeValueProvider>();
    REQUIRE(range);
    CHECK_FALSE(range.IsReadOnly());
    CHECK(range.Minimum() == -3);
    CHECK(range.Maximum() == 12);
    CHECK(range.SmallChange() == 1);
    CHECK(range.LargeChange() == 10);
    CHECK(range.Value() == 4);
    range.SetValue(12);
    CHECK(model->values[spinIndex].GetType() == "long");
    CHECK(model->values[spinIndex].GetLong() == 12);
    spinValue.SetValue(L"-3");
    CHECK(model->values[spinIndex].GetLong() == -3);
    CHECK(spinValue.Value() == L"-3");
    CHECK(spinEditors == 2);
    CheckUIAError(E_INVALIDARG, [&] { range.SetValue(13); });
    CheckUIAError(E_INVALIDARG, [&] { range.SetValue(1.5); });
    CheckUIAError(E_INVALIDARG, [&] { spinValue.SetValue(L"NaN"); });
    CHECK(range.Value() == -3);
#else
    wxUnusedVar(spinEditors);
#endif
#if wxUSE_DATEPICKCTRL
    auto date = DataViewCell(fixture, 0, dateIndex).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    CHECK(date.Value() == L"1600-02-29");
    date.SetValue(L"1596-02-29");
    CHECK(model->values[dateIndex].GetType() == "datetime");
    CHECK(model->values[dateIndex].GetDateTime().IsSameDate(wxDateTime(29, wxDateTime::Feb, 1596)));
    date.SetValue(L"");
    CHECK_FALSE(model->values[dateIndex].GetDateTime().IsValid());
    CHECK(date.Value().empty());
    CHECK(dateEditors == 2);
    CheckUIAError(E_INVALIDARG, [&] { date.SetValue(L"2023-02-29"); });
#else
    wxUnusedVar(dateEditors);
#endif
}

TEST_CASE("WinUI DataView UIA Value honors renderer validation veto and reentrant destruction",
          "[dataview][winui][winui-dataview][winui-dataview-value]")
{
    class ValidatingText : public wxDataViewTextRenderer
    {
    public:
        ValidatingText() : wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE) {}
        bool Validate(wxVariant& value) override
        {
            ++validations;
            return value.GetString() != "invalid";
        }
        unsigned validations = 0;
    };
    const int failure = GENERATE(0, 1, 2, 3, 4);
    NativeFixture fixture;
    auto renderer = new ValidatingText;
    auto column = new wxDataViewColumn("Value", renderer, 0, 220);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeTypedModel({wxVariant("original")});
    fixture.SetModel(model);
    model->IncRef();
    auto modelGuard = wxMakeGuard([model] { model->DecRef(); });
    wxUnusedVar(modelGuard);
    auto value = DataViewCell(fixture, 0, 0).GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    if ( failure == 2 ) model->acceptWrites = false;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [&](wxDataViewEvent& event)
    {
        CHECK(model->writes == 0);
        event.Skip();
        if ( failure == 0 ) CHECK(event.IsEditCancelled());
        if ( failure == 1 ) event.Veto();
        if ( failure == 3 ) REQUIRE(fixture.view->DeleteColumn(column));
        if ( failure == 4 )
        {
            // Do not ask wx to continue dispatching handlers on the object
            // just synchronously destroyed by this deliberately hostile case.
            event.Skip(false);
            fixture.frame.reset();
        }
    });
    CheckUIAError(failure >= 3 ? UIA_E_ELEMENTNOTAVAILABLE : UIA_E_INVALIDOPERATION,
        [&] { value.SetValue(failure == 0 ? L"invalid" : L"candidate"); });
    CHECK(model->writes == 0);
    CHECK(model->values[0].GetString() == "original");
    CHECK(fixture.finished == (failure == 4 ? 0u : 1u));
    if ( failure < 3 ) CHECK(renderer->validations == 1);
}

TEST_CASE("WinUI DataView native Grid and Table peers preserve virtualization",
          "[dataview][winui][winui-dataview][winui-dataview-automation]")
{
    NativeFixture fixture;
    for ( unsigned col = 0; col < 40; ++col )
        REQUIRE(fixture.view->AppendTextColumn(wxString::Format("Column %u", col), col,
                                                wxDATAVIEW_CELL_INERT, 160));
    auto model = new NativeModel(10000);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return list.ContainerFromIndex(0) != nullptr; }));
    auto nativePeer = A::FrameworkElementAutomationPeer::CreatePeerForElement(list);
    REQUIRE(nativePeer);
    CHECK(nativePeer.GetAutomationControlType() == A::AutomationControlType::DataGrid);
    auto grid = nativePeer.GetPattern(A::PatternInterface::Grid).try_as<P::IGridProvider>();
    auto table = nativePeer.GetPattern(A::PatternInterface::Table).try_as<P::ITableProvider>();
    REQUIRE(grid);
    REQUIRE(table);
    CHECK(grid.RowCount() == 10000);
    CHECK(grid.ColumnCount() == 40);
    CHECK(table.RowOrColumnMajor() == X::Automation::RowOrColumnMajor::RowMajor);
    CHECK(table.GetRowHeaders().empty());
    CHECK_THROWS_AS(grid.GetItem(-1, 0), winrt::hresult_invalid_argument);
    CHECK_THROWS_AS(grid.GetItem(0, 40), winrt::hresult_invalid_argument);
    model->reads = 0;
    auto cellPeer = ProviderPeer(nativePeer, grid.GetItem(9999, 39));
    CHECK(model->reads == 0);
    CHECK_FALSE(list.ContainerFromIndex(9999));
    CHECK(cellPeer.IsOffscreen());
    CHECK(cellPeer.GetBoundingRectangle().Width == 0);
    auto cell = cellPeer.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    auto tableCell = cellPeer.GetPattern(A::PatternInterface::TableItem).as<P::ITableItemProvider>();
    CHECK(cell.Row() == 9999);
    CHECK(cell.Column() == 39);
    CHECK(cell.RowSpan() == 1);
    CHECK(cell.ColumnSpan() == 1);
    CHECK(ProviderPeer(nativePeer, cell.ContainingGrid()) == nativePeer);
    CHECK(tableCell.GetRowHeaderItems().empty());
    auto headers = table.GetColumnHeaders();
    CHECK(model->reads == 0);
    REQUIRE(headers.size() == 40);
    auto lastHeader = ProviderPeer(nativePeer, headers[39]);
    CHECK(lastHeader.GetName() == L"Column 39");
    CHECK(lastHeader.GetAutomationControlType() == A::AutomationControlType::HeaderItem);
    CHECK(lastHeader.GetPattern(A::PatternInterface::Transform).try_as<P::ITransformProvider>());
    CHECK(lastHeader.IsOffscreen());
    CHECK(lastHeader.GetBoundingRectangle().Width == 0);
    auto relatedHeaders = tableCell.GetColumnHeaderItems();
    REQUIRE(relatedHeaders.size() == 1);
    CHECK(ProviderPeer(nativePeer, relatedHeaders[0]) == lastHeader);

    cellPeer.GetPattern(A::PatternInterface::VirtualizedItem).as<P::IVirtualizedItemProvider>().Realize();
    REQUIRE(DrainDataView([&] { return !cellPeer.IsOffscreen(); }));
    CHECK(cellPeer.GetBoundingRectangle().Width > 0);
    CHECK(cellPeer.GetName() == L"row 9999 column 39");
    CHECK(model->reads < 1000);
    auto row = list.ContainerFromIndex(9999).try_as<C::ListViewItem>();
    REQUIRE(row);
    auto canvas = row.ContentTemplateRoot().try_as<C::Canvas>();
    REQUIRE(canvas);
    bool identicalNativePeer = false;
    for ( auto visual : canvas.Children() )
    {
        auto peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(visual.as<X::FrameworkElement>());
        if ( peer == cellPeer ) identicalNativePeer = true;
    }
    CHECK(identicalNativePeer);
    REQUIRE(cellPeer.GetParent());
    const auto rowCells = cellPeer.GetParent().GetChildren();
    REQUIRE(rowCells);
    CHECK(rowCells.Size() == 40);
    CHECK(std::find(rowCells.begin(), rowCells.end(), cellPeer) != rowCells.end());
    CHECK_FALSE(lastHeader.IsOffscreen());
}

TEST_CASE("WinUI DataView Grid coordinates follow visible column order and retire identities",
          "[dataview][winui][winui-dataview][winui-dataview-automation]")
{
    NativeFixture fixture;
    for ( unsigned col = 0; col < 3; ++col )
        REQUIRE(fixture.view->AppendTextColumn(wxString::Format("Column %u", col), col,
                                                wxDATAVIEW_CELL_INERT, 160));
    auto model = new NativeModel(4);
    model->emptyCell = true;
    fixture.SetModel(model);
    const auto nativePeer = A::FrameworkElementAutomationPeer::CreatePeerForElement(fixture.Loaded());
    const auto grid = nativePeer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
    const auto table = nativePeer.GetPattern(A::PatternInterface::Table).as<P::ITableProvider>();
    const auto first = fixture.view->GetColumn(0), middle = fixture.view->GetColumn(1);
    auto firstPeer = ProviderPeer(nativePeer, grid.GetItem(2, 0));
    auto firstCell = firstPeer.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    auto emptyPeer = ProviderPeer(nativePeer, grid.GetItem(1, 1));
    CHECK(emptyPeer.GetName().empty());
    emptyPeer.GetPattern(A::PatternInterface::VirtualizedItem).as<P::IVirtualizedItemProvider>().Realize();
    REQUIRE(DrainDataView([&] { return !emptyPeer.IsOffscreen(); }));
    middle->SetHidden(true);
    CHECK(grid.ColumnCount() == 2);
    CheckUnavailable([&] { emptyPeer.GetName(); });
    fixture.view->ColumnMoved(first, 2);
    CHECK(firstCell.Column() == 1);
    CHECK(firstPeer.GetName() == L"row 2 column 0");
    auto headerList = table.GetColumnHeaders();
    REQUIRE(headerList.size() == 2);
    CHECK(ProviderPeer(nativePeer, headerList[0]).GetName() == L"Column 2");
    CHECK(ProviderPeer(nativePeer, headerList[1]).GetName() == L"Column 0");
    middle->SetHidden(false);
    CHECK(grid.ColumnCount() == 3);
    CHECK(firstCell.Column() == 2);
    CheckUnavailable([&] { emptyPeer.GetName(); });
    auto replacementEmpty = ProviderPeer(nativePeer, grid.GetItem(1, 0));
    CHECK(replacementEmpty.GetName().empty());
    CHECK(replacementEmpty != emptyPeer);
    REQUIRE(fixture.view->DeleteColumn(first));
    CheckUnavailable([&] { (void)firstCell.Column(); });
    CHECK(grid.ColumnCount() == 2);
}

TEST_CASE("WinUI DataView retained Grid providers fail closed after model and owner retirement",
          "[dataview][winui][winui-dataview][winui-dataview-automation]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Value", 0, wxDATAVIEW_CELL_INERT, 160));
    auto model = new NativeModel(4);
    fixture.SetModel(model);
    auto nativePeer = A::FrameworkElementAutomationPeer::CreatePeerForElement(fixture.Loaded());
    auto grid = nativePeer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
    auto held = ProviderPeer(nativePeer, grid.GetItem(3, 0));
    auto heldRow = held.GetParent();
    REQUIRE(heldRow);
    auto heldVirtual = held.GetPattern(A::PatternInterface::VirtualizedItem).as<P::IVirtualizedItemProvider>();
    model->Reset(2);
    CHECK(grid.RowCount() == 2);
    CheckUnavailable([&] { held.GetName(); });
    CheckUnavailable([&] { heldVirtual.Realize(); });
    CheckUnavailable([&] { heldRow.GetChildren(); });
    auto next = ProviderPeer(nativePeer, grid.GetItem(0, 0));
    fixture.SetModel(new NativeModel(2));
    CheckUnavailable([&] { next.GetName(); });
    auto live = ProviderPeer(nativePeer, grid.GetItem(0, 0));
    CHECK(live.GetName() == L"row 0 column 0");
    fixture.frame.reset();
    CheckUnavailable([&] { live.GetName(); });
    CheckUnavailable([&] { (void)grid.RowCount(); });
}

TEST_CASE("WinUI DataView UIA value callbacks cannot return a retired cell",
          "[dataview][winui][winui-dataview][winui-dataview-automation]")
{
    const bool destroy = GENERATE(false, true);
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Value", 0, wxDATAVIEW_CELL_INERT, 160));
    auto model = new NativeModel(2);
    fixture.SetModel(model);
    auto nativePeer = A::FrameworkElementAutomationPeer::CreatePeerForElement(fixture.Loaded());
    auto grid = nativePeer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
    auto held = ProviderPeer(nativePeer, grid.GetItem(0, 0));
    model->onRead = [&]
    {
        if ( destroy ) fixture.frame.reset();
        else REQUIRE(fixture.view->DeleteColumn(fixture.view->GetColumn(0)));
    };
    CheckUnavailable([&] { held.GetName(); });
}

TEST_CASE("WinUI DataView realizes native cells without reading the whole model",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    for ( unsigned n = 0; n < 40; ++n )
        REQUIRE(fixture.view->AppendTextColumn(wxString::Format("Column %u", n), n,
                                                wxDATAVIEW_CELL_INERT, 160));
    auto model = new NativeModel(10000);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return list.ContainerFromIndex(0) != nullptr; }));
    REQUIRE(list.ItemsPanelRoot().try_as<C::ItemsStackPanel>());
    CHECK(list.Items().Size() == 10000);
    auto row = list.ContainerFromIndex(0).as<C::ListViewItem>();
    REQUIRE(row.ContentTemplateRoot().try_as<C::Canvas>());
    REQUIRE(DrainDataView([&] { return FindNative<C::TextBlock>(row) != nullptr; }));
    // Neither rows nor off-screen columns may eagerly materialize cell values.
    CHECK(model->reads < 1000);
    CHECK(row.ContentTemplateRoot().as<C::Canvas>().Children().Size() < 40);
    CHECK_FALSE(list.ContainerFromIndex(9999));
}

TEST_CASE("WinUI DataView wx selection setters are silent, native changes notify",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Name", 0));
    auto model = new NativeModel(20);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    unsigned events = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
        [&](wxDataViewEvent&) { ++events; });
    fixture.view->Select(model->GetItem(3));
    CHECK(events == 0);
    CHECK(fixture.view->IsSelected(model->GetItem(3)));
    CHECK(list.SelectedItems().Size() == 1);
    fixture.view->UnselectAll();
    CHECK(events == 0);
    list.SelectedItem(list.Items().GetAt(5));
    CHECK(events == 1);
    CHECK(fixture.view->GetSelection() == model->GetItem(5));
}

TEST_CASE("WinUI DataView TextBox edits commit to the wx model before column mutation",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    auto column = fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_EDITABLE, 220);
    REQUIRE(column);
    auto model = new NativeModel(5);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return list.ContainerFromIndex(0) != nullptr; }));
    auto editor = fixture.Edit(model->GetItem(0), column);
    editor.Text(L"native edit");
    unsigned completed = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent& event)
        {
            CHECK_FALSE(event.IsEditCancelled());
            ++completed;
        });
    REQUIRE(fixture.view->AppendTextColumn("Second", 1));
    CHECK(completed == 1);
    CHECK(model->values[0] == "native edit");
    CHECK(fixture.view->GetColumnCount() == 2);
}

TEST_CASE("WinUI DataView invalidates a sort snapshot when model callbacks mutate columns",
          "[dataview][winui][winui-dataview][reentrancy]")
{
    NativeFixture fixture;
    auto column = fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT,
                                                   160, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE);
    REQUIRE(column);
    auto model = new NativeModel(12);
    fixture.SetModel(model);
    fixture.Loaded();
    bool ran = false;
    model->onCompare = [&]
    {
        ran = true;
        REQUIRE(fixture.view->ClearColumns());
    };
    column->SetSortOrder(true);
    REQUIRE(ran);
    CHECK(fixture.view->GetColumnCount() == 0);
    CHECK(fixture.view->GetModel() == model);
    REQUIRE(DrainDataView([&] { return NativeDataView(fixture.view).Items().Size() == 12; }));
}

TEST_CASE("WinUI DataView native editing-done veto leaves the model unchanged",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    auto column = fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_EDITABLE, 220);
    REQUIRE(column);
    auto model = new NativeModel(5);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return list.ContainerFromIndex(0) != nullptr; }));
    auto editor = fixture.Edit(model->GetItem(0), column);
    editor.Text(L"must be vetoed");
    unsigned completed = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent& event)
        {
            ++completed;
            CHECK(model->values.empty());
            event.Veto();
        });
    auto extra = new wxDataViewColumn("Second", new wxDataViewTextRenderer, 1);
    CHECK_FALSE(fixture.view->AppendColumn(extra));
    delete extra;
    CHECK(completed == 1);
    CHECK(model->values.empty());
    CHECK(fixture.view->GetColumnCount() == 1);
}

TEST_CASE("WinUI DataView single selection clears through the native property",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture(wxDV_SINGLE);
    REQUIRE(fixture.view->AppendTextColumn("Name", 0));
    auto model = new NativeModel(5);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    unsigned events = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
        [&](wxDataViewEvent&) { ++events; });
    fixture.view->Select(model->GetItem(1));
    REQUIRE(list.SelectedItem());
    CHECK(fixture.view->GetSelection() == model->GetItem(1));
    fixture.view->UnselectAll();
    CHECK_FALSE(list.SelectedItem());
    CHECK_FALSE(fixture.view->GetSelection().IsOk());
    fixture.view->Select(model->GetItem(2));
    CHECK(fixture.view->GetSelection() == model->GetItem(2));
    CHECK(events == 0);
}

TEST_CASE("WinUI DataView renderer validation rejects a native edit before mutation",
          "[dataview][winui][winui-dataview]")
{
    class RejectingTextRenderer : public wxDataViewTextRenderer
    {
    public:
        RejectingTextRenderer() : wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE) {}
        bool Validate(wxVariant&) override { return false; }
    };
    NativeFixture fixture;
    auto column = new wxDataViewColumn("Name", new RejectingTextRenderer, 0, 220);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeModel(5);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return list.ContainerFromIndex(0) != nullptr; }));
    auto editor = fixture.Edit(model->GetItem(0), column);
    editor.Text(L"invalid");
    unsigned cancelled = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent& event)
        {
            CHECK(event.IsEditCancelled());
            ++cancelled;
        });
    auto extra = new wxDataViewColumn("Second", new wxDataViewTextRenderer, 1);
    CHECK_FALSE(fixture.view->AppendColumn(extra));
    delete extra;
    CHECK(cancelled == 1);
    CHECK(model->values.empty());
}

TEST_CASE("WinUI DataView renderer finish and cancel control the native TextBox",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    auto column = fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_EDITABLE, 220);
    REQUIRE(column);
    auto model = new NativeModel(5);
    fixture.SetModel(model);
    auto editor = fixture.Edit(model->GetItem(0), column);
    editor.Text(L"committed through renderer");
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(model->values[0] == "committed through renderer");
    CHECK(fixture.finished == 1);
    editor = fixture.Edit(model->GetItem(0), column);
    editor.Text(L"cancelled through renderer");
    unsigned cancelled = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [&](wxDataViewEvent& event)
    {
        if ( event.IsEditCancelled() ) ++cancelled;
        event.Skip();
    });
    column->GetRenderer()->CancelEditing();
    CHECK(cancelled == 1);
    CHECK(fixture.finished == 1);
    CHECK(model->values[0] == "committed through renderer");
    CHECK_FALSE(FindNative<C::TextBox>(NativeDataView(fixture.view)));
}

TEST_CASE("WinUI DataView replacing a populated model with empty or null clears native items",
          "[dataview][winui][winui-dataview]")
{
    NativeFixture fixture;
    REQUIRE(fixture.view->AppendTextColumn("Name", 0));
    fixture.SetModel(new NativeModel(5));
    auto list = fixture.Loaded();
    REQUIRE(list.Items().Size() == 5);
    fixture.SetModel(new NativeModel(0));
    CHECK(list.Items().Size() == 0);
    fixture.SetModel(new NativeModel(3));
    REQUIRE(list.Items().Size() == 3);
    REQUIRE(fixture.view->AssociateModel(nullptr));
    CHECK(list.Items().Size() == 0);
}

TEST_CASE("WinUI DataView bounds retries from a self-notifying comparator",
          "[dataview][winui][winui-dataview][reentrancy]")
{
    NativeFixture fixture;
    auto column = fixture.view->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT,
                                                   160, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE);
    REQUIRE(column);
    auto model = new NativeModel(12);
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    model->notifyWhileComparing = true;
    column->SetSortOrder(true);
    CHECK(model->comparisons == 2);
    CHECK(list.Items().Size() == 0);
    for ( unsigned n = 0; n < 5; ++n )
    {
        wxYield();
        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
    }
    CHECK(model->comparisons == 2);
    model->notifyWhileComparing = false;
    model->Resort();
    CHECK(list.Items().Size() == 12);
}

TEST_CASE("WinUI DataView Choice editors preserve strings and duplicate-label indexes",
          "[dataview][winui][winui-dataview][winui-dataview-editors]")
{
    NativeFixture fixture;
    const wxArrayString choices{"First", "Duplicate", "Duplicate"};
    auto textColumn = new wxDataViewColumn("Choice", new wxDataViewChoiceRenderer(choices), 0, 240);
    auto indexColumn = new wxDataViewColumn("Index", new wxDataViewChoiceByIndexRenderer(choices), 1, 240);
    REQUIRE(fixture.view->AppendColumn(textColumn));
    REQUIRE(fixture.view->AppendColumn(indexColumn));
    auto model = new NativeTypedModel({wxVariant("First"), wxVariant(1L)});
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return HasNativeText(list, L"First") && HasNativeText(list, L"Duplicate"); }));
    auto combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), textColumn);
    CHECK(combo.Items().Size() == 3);
    CHECK(combo.SelectedIndex() == 0);
    combo.SelectedIndex(1);
    REQUIRE(textColumn->GetRenderer()->FinishEditing());
    CHECK(model->values[0].GetType() == "string");
    CHECK(model->values[0].GetString() == "Duplicate");
    REQUIRE(DrainDataView([&] { return !FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view)); }));
    combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), indexColumn);
    CHECK(combo.SelectedIndex() == 1);
    combo.SelectedIndex(2);
    REQUIRE(indexColumn->GetRenderer()->FinishEditing());
    CHECK(model->values[1].GetType() == "long");
    CHECK(model->values[1].GetLong() == 2);
    CHECK(model->writes == 2);
}

TEST_CASE("WinUI DataView editing guards reentrant model reads during column sizing",
          "[dataview][winui][winui-dataview][winui-dataview-editors][reentrancy]")
{
    struct SizingProbe
    {
        NativeModel* model = nullptr;
        bool armed = false;
        unsigned calls = 0;
    };
    class ModelMeasuringColumn : public wxDataViewColumn
    {
    public:
        explicit ModelMeasuringColumn(const std::shared_ptr<SizingProbe>& probe)
            : wxDataViewColumn("Measured", new wxDataViewTextRenderer, 0, wxCOL_WIDTH_AUTOSIZE),
              m_probe(probe) {}
        int GetWidth() const override
        {
            const auto probe = m_probe;
            if ( probe->armed )
            {
                probe->armed = false;
                ++probe->calls;
                wxVariant value;
                // A public autosizing override may read the model every time,
                // even when the default best-width implementation is cached.
                probe->model->GetValue(value, probe->model->GetItem(0), 0);
                // The callback may have deleted this column and its owner.
                return 140;
            }
            return wxDataViewColumn::GetWidth();
        }
    private:
        std::shared_ptr<SizingProbe> m_probe;
    };
    NativeFixture fixture;
    auto probe = std::make_shared<SizingProbe>();
    auto measured = new ModelMeasuringColumn(probe);
    REQUIRE(fixture.view->AppendColumn(measured));
    auto edited = fixture.view->AppendTextColumn("Editable", 1, wxDATAVIEW_CELL_EDITABLE, 220);
    REQUIRE(edited);
    auto model = new NativeModel(5);
    probe->model = model;
    fixture.SetModel(model);
    fixture.Loaded();
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, [&](wxDataViewEvent& event)
    {
        probe->armed = true;
        event.Skip();
    });
    SECTION("a column mutation is rejected while the edit is reserved")
    {
        bool attempted = false;
        bool removed = true;
        std::function<void()> onSizingRead;
        onSizingRead = [&]
        {
            if ( !probe->calls ) { model->onRead = onSizingRead; return; }
            attempted = true;
            removed = fixture.view->DeleteColumn(measured);
        };
        model->onRead = onSizingRead;
        fixture.view->EditItem(model->GetItem(0), edited);
        model->onRead = {};
        CHECK(probe->calls == 1);
        CHECK(attempted);
        CHECK_FALSE(removed);
        CHECK(fixture.view->GetColumnCount() == 2);
        REQUIRE(FindNative<C::TextBox>(NativeDataViewRoot(fixture.view)));
        edited->GetRenderer()->CancelEditing();
    }
    SECTION("destruction from sizing aborts before native publication")
    {
        wxWeakRef<wxDataViewCtrl> surviving(fixture.view);
        std::function<void()> onSizingRead;
        onSizingRead = [&]
        {
            if ( !probe->calls ) { model->onRead = onSizingRead; return; }
            auto doomed = fixture.view;
            fixture.view = nullptr;
            fixture.frame->GetSizer()->Detach(doomed);
            delete doomed;
        };
        model->onRead = onSizingRead;
        fixture.view->EditItem(model->GetItem(0), edited);
        if ( surviving ) model->onRead = {};
        CHECK(probe->calls == 1);
        CHECK_FALSE(surviving.get());
        CHECK(fixture.started == 0);
    }
}

TEST_CASE("WinUI DataView Choice public finish commits while popup retirement is pending",
          "[dataview][winui][winui-dataview][winui-dataview-editors][popup]")
{
    NativeFixture fixture;
    auto column = new wxDataViewColumn("Choice",
        new wxDataViewChoiceRenderer(wxArrayString{"First", "Second"}), 0, 240);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeTypedModel({wxVariant("First")});
    fixture.SetModel(model);
    auto combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), column);
    combo.IsDropDownOpen(true);
    REQUIRE(DrainDataView([&] { return combo.IsDropDownOpen(); }));
    // Focus transfers into the popup must not finish the cell edit.
    wxYield();
    wxTheApp->ProcessPendingEvents();
    CHECK(fixture.finished == 0);
    combo.SelectedIndex(1);
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(fixture.finished == 1);
    CHECK(model->writes == 1);
    CHECK(model->values[0].GetString() == "Second");
    REQUIRE(DrainDataView([&]
    {
        return !combo.IsDropDownOpen() && !FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view));
    }));
    combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), column);
    combo.IsDropDownOpen(true);
    combo.SelectedIndex(0);
    column->GetRenderer()->CancelEditing();
    CHECK(model->writes == 1);
    CHECK(model->values[0].GetString() == "Second");
    REQUIRE(DrainDataView([&] { return !FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view)); }));
}

TEST_CASE("WinUI DataView Choice validation and done veto precede model writes",
          "[dataview][winui][winui-dataview][winui-dataview-editors]")
{
    class ValidatingChoice : public wxDataViewChoiceRenderer
    {
    public:
        ValidatingChoice() : wxDataViewChoiceRenderer(wxArrayString{"First", "Second"}) {}
        bool Validate(wxVariant& value) override
        {
            ++validations;
            CHECK(value.GetType() == "string");
            return valid;
        }
        unsigned validations = 0;
        bool valid = false;
    };
    NativeFixture fixture;
    auto renderer = new ValidatingChoice;
    auto column = new wxDataViewColumn("Choice", renderer, 0, 240);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeTypedModel({wxVariant("First")});
    fixture.SetModel(model);
    auto combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), column);
    combo.SelectedIndex(1);
    bool cancelled = false;
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [&](wxDataViewEvent& event)
    {
        CHECK(model->writes == 0);
        cancelled = event.IsEditCancelled();
        event.Veto();
    });
    CHECK_FALSE(renderer->FinishEditing());
    CHECK(cancelled);
    CHECK(renderer->validations == 1);
    REQUIRE(DrainDataView([&] { return !FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view)); }));
    renderer->valid = true;
    combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), column);
    combo.SelectedIndex(1);
    CHECK_FALSE(renderer->FinishEditing());
    CHECK_FALSE(cancelled);
    CHECK(renderer->validations == 2);
    CHECK(model->writes == 0);
    CHECK(model->values[0].GetString() == "First");
}

TEST_CASE("WinUI DataView closing a Choice popup preserves the focused cell editor",
          "[dataview][winui][winui-dataview][winui-dataview-editors][popup]")
{
    NativeFixture fixture;
    auto column = new wxDataViewColumn("Choice",
        new wxDataViewChoiceRenderer(wxArrayString{"First", "Second"}), 0, 240);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeTypedModel({wxVariant("First")});
    fixture.SetModel(model);
    auto combo = fixture.EditAs<C::ComboBox>(model->GetItem(0), column);
    REQUIRE(DrainDataView([&] { return ContainsNativeFocus(combo); }));
    combo.IsDropDownOpen(true);
    REQUIRE(DrainDataView([&]
    {
        return combo.IsDropDownOpen() &&
            X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(combo.XamlRoot()).Size() != 0;
    }));
    combo.SelectedIndex(1);
    combo.IsDropDownOpen(false);
    REQUIRE(DrainDataView([&] { return !combo.IsDropDownOpen() && ContainsNativeFocus(combo); }));
    wxYield();
    wxTheApp->ProcessPendingEvents();
    CHECK(fixture.finished == 0);
    CHECK(model->writes == 0);
    CHECK(FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view)) == combo);
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(fixture.finished == 1);
    CHECK(model->values[0].GetString() == "Second");
    REQUIRE(DrainDataView([&] { return !FindNative<C::ComboBox>(NativeDataViewRoot(fixture.view)); }));
}

#if wxUSE_SPINCTRL
TEST_CASE("WinUI DataView Spin uses NumberBox with integer values and renderer bounds",
          "[dataview][winui][winui-dataview][winui-dataview-editors]")
{
    NativeFixture fixture;
    auto column = new wxDataViewColumn("Count", new wxDataViewSpinRenderer(-3, 12), 0, 240);
    REQUIRE(fixture.view->AppendColumn(column));
    auto model = new NativeTypedModel({wxVariant(4L)});
    fixture.SetModel(model);
    auto list = fixture.Loaded();
    REQUIRE(DrainDataView([&] { return HasNativeText(list, L"4"); }));
    auto spin = fixture.EditAs<C::NumberBox>(model->GetItem(0), column);
    CHECK(spin.Minimum() == -3);
    CHECK(spin.Maximum() == 12);
    CHECK(spin.SmallChange() == 1);
    CHECK(spin.SpinButtonPlacementMode() == C::NumberBoxSpinButtonPlacementMode::Inline);
    auto input = FindNative<C::TextBox>(spin);
    REQUIRE(input);
    input.Text(L"999");
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(model->values[0].GetType() == "long");
    CHECK(model->values[0].GetLong() == 12);
    spin = fixture.EditAs<C::NumberBox>(model->GetItem(0), column);
    input = FindNative<C::TextBox>(spin);
    REQUIRE(input);
    input.Text(L"not an integer");
    CHECK_FALSE(column->GetRenderer()->FinishEditing());
    CHECK(model->writes == 1);
    spin = fixture.EditAs<C::NumberBox>(model->GetItem(0), column);
    spin.Value(-2);
    column->GetRenderer()->CancelEditing();
    CHECK(model->writes == 1);
    CHECK(model->values[0].GetLong() == 12);
}
#endif

#if wxUSE_DATEPICKCTRL
TEST_CASE("WinUI DataView Date editor preserves civil dates and nullable variants",
          "[dataview][winui][winui-dataview][winui-dataview-editors]")
{
    NativeFixture fixture;
    auto column = new wxDataViewColumn("Date", new wxDataViewDateRenderer, 0, 280);
    REQUIRE(fixture.view->AppendColumn(column));
    const wxDateTime original(29, wxDateTime::Feb, 1600);
    auto model = new NativeTypedModel({wxVariant(original)});
    fixture.SetModel(model);
    auto date = fixture.EditAs<C::CalendarDatePicker>(model->GetItem(0), column);
    REQUIRE(date.Date());
    winrt::Windows::Globalization::Calendar calendar;
    calendar.ChangeCalendarSystem(winrt::Windows::Globalization::CalendarIdentifiers::Gregorian());
    calendar.SetDateTime(date.Date().Value());
    CHECK(calendar.Year() == 1600);
    CHECK(calendar.Month() == 2);
    CHECK(calendar.Day() == 29);
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(model->values[0].GetType() == "datetime");
    CHECK(model->values[0].GetDateTime().IsSameDate(original));
    REQUIRE(DrainDataView([&] { return !FindNative<C::CalendarDatePicker>(NativeDataViewRoot(fixture.view)); }));
    date = fixture.EditAs<C::CalendarDatePicker>(model->GetItem(0), column);
    date.Date(nullptr);
    REQUIRE(column->GetRenderer()->FinishEditing());
    CHECK(model->values[0].GetType() == "datetime");
    CHECK_FALSE(model->values[0].GetDateTime().IsValid());
    REQUIRE(DrainDataView([&] { return !FindNative<C::CalendarDatePicker>(NativeDataViewRoot(fixture.view)); }));
    date = fixture.EditAs<C::CalendarDatePicker>(model->GetItem(0), column);
    CHECK_FALSE(date.Date());
    struct CalendarEvents { unsigned opened = 0, closed = 0; };
    const auto observed = std::make_shared<CalendarEvents>();
    date.Opened([observed](const auto&, const auto&)
    {
        ++observed->opened;
    });
    date.Closed([observed](const auto&, const auto&)
    {
        ++observed->closed;
    });
    date.IsCalendarOpen(true);
    REQUIRE(DrainDataView([&] { return date.IsCalendarOpen(); }));
    column->GetRenderer()->CancelEditing();
    CHECK(model->writes == 2);
    const bool retired = DrainDataView([&] { return !FindNative<C::CalendarDatePicker>(NativeDataViewRoot(fixture.view)); });
    INFO("calendar open=" << date.IsCalendarOpen() << " loaded=" << date.IsLoaded()
         << " root=" << static_cast<bool>(date.XamlRoot())
         << " size=" << date.ActualWidth() << 'x' << date.ActualHeight()
         << " opened=" << observed->opened << " closed=" << observed->closed
         << " focused=" << ContainsNativeFocus(date)
         << " done=" << fixture.finished);
    REQUIRE(retired);
    CHECK(observed->opened == 1);
    CHECK(observed->closed == 1);
}
#endif

#endif // wxHAS_WINUI_DATAVIEWCTRL
#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
