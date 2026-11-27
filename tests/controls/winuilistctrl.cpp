/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuilistctrl.cpp
// Purpose:     Real WinUI presentation of the public wxListCtrl model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_LISTCTRL

#include "wx/app.h"
#include "wx/dcmemory.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/scopeguard.h"
#if wxUSE_TEXTCTRL
#include "wx/textctrl.h"
#endif
#if wxUSE_EDITABLELISTBOX && wxUSE_BMPBUTTON && wxUSE_TEXTCTRL
#include "wx/bmpbuttn.h"
#include "wx/editlbox.h"
#endif
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <UIAutomation.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <set>
#include <thread>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace CP = C::Primitives;
namespace A = X::Automation;

template<typename Predicate>
bool WaitList(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1000 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

wxPoint ListPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

X::DependencyObject ListNativeTree(wxWindow* window)
{
    auto* const host = window ? wxWinUITopLevelHost::ForWindow(window, false) : nullptr;
    return host && host->GetXamlRoot() ? host->GetXamlRoot().Content() : nullptr;
}

template<typename T>
T FindListElement(const X::DependencyObject& node, const winrt::hstring& id = {})
{
    if ( !node ) return nullptr;
    if ( const auto element = node.try_as<T>(); element &&
         (id.empty() || A::AutomationProperties::GetAutomationId(element) == id) )
        return element;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( const auto found = FindListElement<T>(
                X::Media::VisualTreeHelper::GetChild(node, i), id) ) return found;
    return nullptr;
}

C::ContentControl ListCell(wxWindow* list, long row, int column = 0)
{
    return FindListElement<C::ContentControl>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Item.%ld.%d", row, column).wc_str()));
}

C::ListViewItem ListReportRow(wxWindow* list, long row)
{
    return FindListElement<C::ListViewItem>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Row.%ld", row).wc_str()));
}

CP::SelectorItem ListSelectorCell(wxWindow* list, long row)
{
    return FindListElement<CP::SelectorItem>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Item.%ld.0", row).wc_str()));
}

CP::SelectorItem ListNativeItem(wxWindow* list, long row)
{
    return list->HasFlag(wxLC_REPORT)
        ? ListReportRow(list, row).try_as<CP::SelectorItem>() : ListSelectorCell(list, row);
}

C::ListViewItem ListReportAncestor(X::DependencyObject element)
{
    for ( unsigned depth = 0; element && depth < 64; ++depth )
    {
        if ( const auto row = element.try_as<C::ListViewItem>() ) return row;
        element = X::Media::VisualTreeHelper::GetParent(element);
    }
    return nullptr;
}

C::Control ListDisplayedCell(wxWindow* list, long row, int column = 0)
{
    return FindListElement<C::Control>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Item.%ld.%d", row, column).wc_str()));
}

A::Provider::ISelectionItemProvider ListSelection(const CP::SelectorItem& cell)
{
    const auto peer = cell
        ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::SelectionItem)
        .try_as<A::Provider::ISelectionItemProvider>() : nullptr;
}

double ListNaturalTextWidth(const C::TextBlock& text)
{
    C::TextBlock measured;
    measured.Text(text.Text());
    measured.FontFamily(text.FontFamily());
    measured.FontSize(text.FontSize());
    measured.FontWeight(text.FontWeight());
    measured.FontStyle(text.FontStyle());
    measured.FontStretch(text.FontStretch());
    measured.CharacterSpacing(text.CharacterSpacing());
    measured.Language(text.Language());
    measured.TextWrapping(X::TextWrapping::NoWrap);
    measured.Measure({std::numeric_limits<float>::infinity(),
                      std::numeric_limits<float>::infinity()});
    return measured.DesiredSize().Width;
}

C::CheckBox ListCheck(wxWindow* list, long row)
{
    return FindListElement<C::CheckBox>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Check.%ld", row).wc_str()));
}

C::Button ListHeader(wxWindow* list, int column)
{
    return FindListElement<C::Button>(ListNativeTree(list),
        winrt::hstring(wxString::Format("wxList.Header.%d", column).wc_str()));
}

wxArrayInt ListOrder(std::initializer_list<int> columns)
{
    wxArrayInt order;
    for ( const int column : columns ) order.Add(column);
    return order;
}

wxRect ListScreenRect(wxWindow* list, const X::FrameworkElement& element)
{
    auto* const host = wxWinUITopLevelHost::ForWindow(list, false);
    if ( !host || !element || !element.IsLoaded() ) return {};
    // The island root is physical LTR. Converting through it mirrors the
    // element once, unlike adding an RTL wx client origin to a XAML offset.
    const auto bounds = element.TransformToVisual(host->Root()).TransformBounds(
        {0, 0, static_cast<float>(element.ActualWidth()),
               static_cast<float>(element.ActualHeight())});
    const auto origin = host->GetClientScreenRect();
    const double scale = host->GetScale();
    return wxRect(origin.left + static_cast<int>(std::lround(bounds.X * scale)),
                  origin.top + static_cast<int>(std::lround(bounds.Y * scale)),
                  static_cast<int>(std::lround(bounds.Width * scale)),
                  static_cast<int>(std::lround(bounds.Height * scale)));
}

wxString ListCellText(const X::DependencyObject& cell)
{
    const auto text = FindListElement<C::TextBlock>(cell);
    return text ? wxString(text.Text().c_str()) : wxString();
}

bool ListHasNativeOrder(wxListCtrl* list, const wxArrayInt& order, bool rtl)
{
    wxRect previous;
    for ( const int column : order )
    {
        const auto cell = ListCell(list, 0, column);
        const auto header = ListHeader(list, column);
        if ( !cell || !header ) return false;
        const wxRect cellRect = ListScreenRect(list, cell);
        const wxRect headerRect = ListScreenRect(list, header);
        if ( cellRect.IsEmpty() || headerRect.IsEmpty() ||
             std::abs(cellRect.x - headerRect.x) > 2 ||
             std::abs(cellRect.width - headerRect.width) > 2 ||
             std::abs(cellRect.width - list->GetColumnWidth(column)) > 2 ) return false;
        // Column indices can be reused by ClearAll/InsertColumn before the
        // queued projection has replaced the old native contents. Relative
        // order alone cannot distinguish this from the requested revision.
        wxListItem info;
        info.SetMask(wxLIST_MASK_TEXT);
        wxRect publicRect;
        // A label being edited is shown by its editor, in place of the text
        // of its cell (these lists only ever edit their first item).
        const wxString shown =
            column == 0 && list->GetEditControl()
                ? wxString()
                : list->GetItemText(0, column);
        if ( !list->GetColumn(column, info) ||
             ListCellText(header) != info.GetText() ||
             ListCellText(cell) != shown ||
             !list->GetSubItemRect(0, column, publicRect, wxLIST_RECT_BOUNDS) ) return false;
        const wxPoint centre = list->ScreenToClient(
            wxPoint(cellRect.x + cellRect.width / 2,
                    cellRect.y + cellRect.height / 2));
        if ( std::abs(centre.x - (publicRect.x + publicRect.width / 2)) > 2 ||
             std::abs(centre.y - (publicRect.y + publicRect.height / 2)) > 2 ) return false;
        if ( !previous.IsEmpty() &&
             (rtl ? cellRect.GetRight() > previous.GetLeft() + 2
                  : cellRect.GetLeft() < previous.GetRight() - 2) ) return false;
        previous = cellRect;
    }
    return true;
}

void CheckListOrder(wxListCtrl* list, const wxArrayInt& order, bool rtl)
{
    CAPTURE(list->GetColumnCount(), list->GetItemText(0));
    REQUIRE(list->GetColumnsOrder() == order);
    REQUIRE(WaitList([&]() { return ListHasNativeOrder(list, order, rtl); }));
    int offset = 0;
    for ( size_t position = 0; position < order.size(); ++position )
    {
        const int column = order[position];
        CAPTURE(position, column, rtl);
        CHECK(list->GetColumnIndexFromOrder(static_cast<int>(position)) == column);
        CHECK(list->GetColumnOrder(column) == static_cast<int>(position));
        const wxRect cellRect = ListScreenRect(list, ListCell(list, 0, column));
        CHECK(std::abs(cellRect.width - list->GetColumnWidth(column)) <= 2);
        const wxPoint centre(cellRect.x + cellRect.width / 2,
                             cellRect.y + cellRect.height / 2);
        int flags = 0;
        long subItem = wxNOT_FOUND;
        CHECK(list->HitTest(list->ScreenToClient(centre), flags, &subItem) == 0);
        CHECK(subItem == column);
        wxRect publicRect;
        REQUIRE(list->GetSubItemRect(0, column, publicRect, wxLIST_RECT_BOUNDS));
        CHECK(publicRect.x == offset);
        CHECK(publicRect.width == list->GetColumnWidth(column));
        offset += list->GetColumnWidth(column);
    }
}

unsigned CountListCells(const X::DependencyObject& node)
{
    if ( !node ) return 0;
    unsigned total = 0;
    if ( const auto element = node.try_as<C::ContentControl>() )
    {
        if ( wxString(A::AutomationProperties::GetAutomationId(element).c_str())
                 .StartsWith("wxList.Item.") ) ++total;
    }
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        total += CountListCells(X::Media::VisualTreeHelper::GetChild(node, i));
    return total;
}

A::Provider::IToggleProvider ListToggle(const C::Control& element)
{
    const auto peer = element
        ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(element) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Toggle)
        .try_as<A::Provider::IToggleProvider>() : nullptr;
}

A::Provider::IInvokeProvider ListInvoke(const C::Control& element)
{
    const auto peer = element
        ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(element) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
        .try_as<A::Provider::IInvokeProvider>() : nullptr;
}

void SelectRetiredListPeer(const A::Provider::ISelectionItemProvider& provider)
{
    try
    {
        provider.Select();
    }
    catch ( const winrt::hresult_error& error )
    {
        // A retired native provider may be explicitly unavailable/disabled.
        // Other failures are not an acceptable substitute for safe retirement.
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) ||
               code == static_cast<int32_t>(0x80040201u)));
    }
}

#if wxUSE_EDITABLELISTBOX && wxUSE_BMPBUTTON && wxUSE_TEXTCTRL
C::Button PublicListButton(wxWindow* button)
{
    auto* const host = button ? wxWinUITopLevelHost::FindSlotOwner(button) : nullptr;
    auto* const slot = host ? host->FindSlot(button) : nullptr;
    return slot ? slot->GetSemanticTarget().try_as<C::Button>() : nullptr;
}
#endif

wxBitmap ListTestBitmap(int dimension)
{
    wxBitmap bitmap(dimension, dimension, 32);
    wxMemoryDC dc(bitmap);
    dc.SetBackground(*wxBLUE_BRUSH);
    dc.Clear();
    dc.SelectObject(wxNullBitmap);
    return bitmap;
}

struct NativeListFixture
{
    explicit NativeListFixture(long style = wxLC_REPORT, bool rtl = false)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "native public wxListCtrl",
                            ListPosition(), wxSize(680, 380));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        list = new wxListCtrl(frame, wxID_ANY, wxDefaultPosition,
                              wxDefaultSize, style);
        list->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(list, 1, wxEXPAND);
        frame->SetSizer(sizer);
    }
    ~NativeListFixture()
    {
        delete frame;
        wxTheApp->ProcessPendingEvents();
    }
    void Show(long row = 0)
    {
        frame->Layout();
        frame->ShowWithoutActivating();
        REQUIRE(WaitList([&]()
        {
            const auto cell = ListDisplayedCell(list, row);
            return cell && cell.IsLoaded() && cell.ActualWidth() > 0 &&
                   cell.ActualHeight() > 0;
        }));
    }
    wxFrame* frame = nullptr;
    wxListCtrl* list = nullptr;
};

class VirtualNativeList final : public wxListCtrl
{
public:
    explicit VirtualNativeList(wxWindow* parent, bool editable = false, long extraStyle = 0)
        : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     wxLC_REPORT | wxLC_VIRTUAL | (editable ? wxLC_EDIT_LABELS : 0) | extraStyle) {}
    mutable unsigned readCalls = 0;
    mutable std::set<long> readRows;
    mutable std::function<void()> onRead;
    long changedRow = -1;
    long editedRow = -1;
    wxString editedValue;
protected:
    wxString OnGetItemText(long item, long column) const override
    {
        ++readCalls;
        readRows.insert(item);
        const auto result = item == editedRow && column == 0 ? editedValue
            : wxString::Format(item == changedRow ? "Changed %ld/%ld" : "Row %ld/%ld", item, column);
        if ( onRead )
        {
            const auto callback = std::move(onRead);
            callback();
        }
        return result;
    }
};

A::Peers::AutomationPeer ListGridPeer(wxWindow* list)
{
    const auto native = FindListElement<C::ListView>(ListNativeTree(list), L"wxList.NativeItems");
    return native ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(native) : nullptr;
}

A::Peers::AutomationPeer ListProviderPeer(const A::Peers::AutomationPeer& grid,
                                        const A::Provider::IRawElementProviderSimple& provider)
{
    return grid.as<A::Peers::IAutomationPeerProtected>().PeerFromProvider(provider);
}

template<typename Action> void ListUnavailable(Action action)
{
    try { action(); FAIL("retired UIA provider remained available"); }
    catch ( const winrt::hresult_error& error ) { CHECK(error.code() == UIA_E_ELEMENTNOTAVAILABLE); }
}

template<typename Action> void ListAutomationError(Action action, int32_t expected)
{
    try { action(); FAIL("UIA action unexpectedly succeeded"); }
    catch ( const winrt::hresult_error& error ) { CHECK(static_cast<int32_t>(error.code()) == expected); }
}

std::vector<A::Peers::AutomationPeer> ListSelectedPeers(const A::Peers::AutomationPeer& grid)
{
    const auto selection = grid.GetPattern(A::Peers::PatternInterface::Selection)
        .as<A::Provider::ISelectionProvider>();
    std::vector<A::Peers::AutomationPeer> result;
    for ( const auto& provider : selection.GetSelection() )
        result.push_back(ListProviderPeer(grid, provider));
    return result;
}

struct ListSelectionEventState
{
    HANDLE stop = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
    std::atomic<unsigned> events{0};
    std::atomic<bool> ready{false}, done{false};
    std::atomic<long> error{S_OK};
    ~ListSelectionEventState() { if ( stop ) ::CloseHandle(stop); }
};
struct ListSelectionEventHandler : winrt::implements<ListSelectionEventHandler,
                                                          ::IUIAutomationEventHandler>
{
    explicit ListSelectionEventHandler(std::shared_ptr<ListSelectionEventState> state)
        : m_state(std::move(state)) {}
    HRESULT __stdcall HandleAutomationEvent(::IUIAutomationElement*, EVENTID id) noexcept override
    {
        if ( id == UIA_Selection_InvalidatedEventId ) ++m_state->events;
        return S_OK;
    }
    std::shared_ptr<ListSelectionEventState> m_state;
};

// A real UIAutomationCore client subscribes from an MTA, while the test thread
// continues dispatching the provider's UI loop. No focus or physical input.
class ListSelectionObserver
{
public:
    explicit ListSelectionObserver(HWND bridge) : state(std::make_shared<ListSelectionEventState>())
    {
        worker = std::thread([observed = state, bridge]
        {
            try
            {
                winrt::check_hresult(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));
                const auto uninitialize = wxMakeGuard([] { ::CoUninitialize(); });
                winrt::com_ptr<::IUIAutomation> automation;
                winrt::check_hresult(::CoCreateInstance(__uuidof(CUIAutomation8), nullptr,
                    CLSCTX_INPROC_SERVER, __uuidof(::IUIAutomation), automation.put_void()));
                winrt::com_ptr<::IUIAutomationElement> root;
                winrt::check_hresult(automation->ElementFromHandle(bridge, root.put()));
                VARIANT id;
                ::VariantInit(&id);
                id.vt = VT_BSTR;
                id.bstrVal = ::SysAllocString(L"wxList.NativeItems");
                const auto clear = wxMakeGuard([&] { ::VariantClear(&id); });
                winrt::com_ptr<::IUIAutomationCondition> condition;
                winrt::check_hresult(automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, id, condition.put()));
                winrt::com_ptr<::IUIAutomationElement> target;
                winrt::check_hresult(root->FindFirst(TreeScope_Subtree, condition.get(), target.put()));
                if ( !target ) throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE);
                const auto handler = winrt::make_self<ListSelectionEventHandler>(observed);
                winrt::check_hresult(automation->AddAutomationEventHandler(UIA_Selection_InvalidatedEventId,
                    target.get(), TreeScope_Element, nullptr, handler.get()));
                observed->ready.store(true, std::memory_order_release);
                ::WaitForSingleObject(observed->stop, 10000);
                winrt::check_hresult(automation->RemoveAutomationEventHandler(UIA_Selection_InvalidatedEventId,
                    target.get(), handler.get()));
            }
            catch ( const winrt::hresult_error& error )
                { observed->error.store(static_cast<long>(error.code())); }
            observed->done.store(true, std::memory_order_release);
        });
    }
    ~ListSelectionObserver() { Stop(); }
    bool Stop()
    {
        if ( !worker.joinable() ) return state->done.load();
        ::SetEvent(state->stop);
        const bool stopped = WaitList([&] { return state->done.load(std::memory_order_acquire); });
        // A failed client never retains wx pointers or blocks test teardown.
        if ( stopped ) worker.join(); else worker.detach();
        return stopped;
    }
    std::shared_ptr<ListSelectionEventState> state;
private:
    std::thread worker;
};
}

TEST_CASE("WinUIListCtrl::ReportGlobalSelectionIncludesVirtualRowsWithoutRealizing",
          "[winui-listctrl][winui-listctrl-selection]")
{
    NativeListFixture fixture;
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame);
    fixture.list = list;
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    list->SetItemCount(100000);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto selection = peer.GetPattern(A::Peers::PatternInterface::Selection)
        .as<A::Provider::ISelectionProvider>();
    CHECK(selection.CanSelectMultiple());
    CHECK_FALSE(selection.IsSelectionRequired());
    CHECK(selection.GetSelection().empty());
    const long rows[] = {2, 50000, 99999};
    for ( const long row : rows )
        REQUIRE(list->SetItemState(row, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    const auto items = FindListElement<C::ListView>(ListNativeTree(list), L"wxList.NativeItems");
    REQUIRE(items);
    const auto nativeCount = items.Items().Size();
    const auto view = list->GetViewStart();
    const auto nativeFirst = ListReportRow(list, 0);
    const auto focused = X::Input::FocusManager::GetFocusedElement(items.XamlRoot());
    list->readCalls = 0;
    const auto selected = ListSelectedPeers(peer);
    REQUIRE(selected.size() == 3);
    const auto again = ListSelectedPeers(peer);
    REQUIRE(again.size() == selected.size());
    for ( size_t i = 0; i < selected.size(); ++i )
    {
        CHECK(selected[i] == again[i]);
        CHECK(selected[i].GetPositionInSet() == rows[i] + 1);
        CHECK(selected[i].GetSizeOfSet() == 100000);
        const auto item = selected[i].GetPattern(A::Peers::PatternInterface::SelectionItem)
            .as<A::Provider::ISelectionItemProvider>();
        CHECK(item.IsSelected());
        CHECK(ListProviderPeer(peer, item.SelectionContainer()) == peer);
        CHECK(selected[i].GetParent() == peer);
        if ( i )
        {
            CHECK(selected[i].IsOffscreen());
            CHECK(selected[i].GetBoundingRectangle().Width == 0);
            CHECK(selected[i].GetChildren().Size() == 0u);
        }
    }
    CHECK(list->readCalls == 0u);
    CHECK(items.Items().Size() == nativeCount);
    CHECK(ListReportRow(list, 0) == nativeFirst);
    CHECK(list->GetViewStart() == view);
    CHECK(X::Input::FocusManager::GetFocusedElement(items.XamlRoot()) == focused);
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListReportRow(list, 2)) == selected[0]);
    const auto distant = selected[1].GetPattern(A::Peers::PatternInterface::SelectionItem)
        .as<A::Provider::ISelectionItemProvider>();
    distant.RemoveFromSelection();
    CHECK_FALSE(distant.IsSelected());
    CHECK(ListSelectedPeers(peer).size() == 2);
    distant.AddToSelection();
    CHECK(distant.IsSelected());
    CHECK(ListSelectedPeers(peer)[1] == selected[1]);
    CHECK(items.Items().Size() < 64u);
    selected[1].GetPattern(A::Peers::PatternInterface::VirtualizedItem)
        .as<A::Provider::IVirtualizedItemProvider>().Realize();
    REQUIRE(WaitList([&]
    {
        const auto row = ListReportRow(list, 50000);
        return row && row.IsLoaded() && row.ActualHeight() > 0;
    }));
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListReportRow(list, 50000)) == selected[1]);
    CHECK_FALSE(selected[1].IsOffscreen());
    CHECK(selected[0].IsOffscreen());
    CHECK(ListSelectedPeers(peer)[0] == selected[0]);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto cell = ListProviderPeer(peer, grid.GetItem(50000, 0));
    CHECK(cell.GetParent() == selected[1]);
    distant.Select();
    REQUIRE(ListSelectedPeers(peer).size() == 1);
    CHECK(ListSelectedPeers(peer)[0] == selected[1]);
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK(list->GetItemState(2, wxLIST_STATE_SELECTED) == 0);
    CHECK(list->GetItemState(99999, wxLIST_STATE_SELECTED) == 0);
    list->Disable();
    ListAutomationError([&] { distant.RemoveFromSelection(); }, UIA_E_ELEMENTNOTENABLED);
    CHECK(distant.IsSelected());
    CHECK(ListSelectedPeers(peer).size() == 1);
    list->Enable();
    list->SetItemCount(3);
    ListUnavailable([&] { (void)distant.IsSelected(); });
    ListUnavailable([&] { distant.Select(); });
    CHECK(selection.GetSelection().empty());
}

TEST_CASE("WinUIListCtrl::ReportGlobalSelectionSupportsBulkAndOptionalSingle",
          "[winui-listctrl][winui-listctrl-selection]")
{
    const bool single = GENERATE(false, true);
    NativeListFixture fixture;
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame, false, single ? wxLC_SINGLE_SEL : 0);
    fixture.list = list;
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    list->SetItemCount(2048);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto selection = peer.GetPattern(A::Peers::PatternInterface::Selection)
        .as<A::Provider::ISelectionProvider>();
    CHECK(selection.CanSelectMultiple() == !single);
    CHECK_FALSE(selection.IsSelectionRequired());
    CHECK(selection.GetSelection().empty());
    const auto first = ListSelection(ListReportRow(list, 0));
    const auto second = ListSelection(ListReportRow(list, 1));
    first.Select();
    if ( single )
    {
        ListAutomationError([&] { second.AddToSelection(); }, UIA_E_INVALIDOPERATION);
        CHECK(first.IsSelected());
        second.Select();
        CHECK_FALSE(first.IsSelected());
        CHECK(second.IsSelected());
        REQUIRE(ListSelectedPeers(peer).size() == 1);
        CHECK(ListSelectedPeers(peer)[0].GetPositionInSet() == 2);
        second.RemoveFromSelection();
        CHECK(selection.GetSelection().empty());
    }
    else
    {
        REQUIRE(list->SetItemState(-1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
        const auto native = FindListElement<C::ListView>(ListNativeTree(list), L"wxList.NativeItems");
        const auto nativeCount = native.Items().Size();
        const auto view = list->GetViewStart();
        list->readCalls = 0;
        const auto all = ListSelectedPeers(peer);
        REQUIRE(all.size() == 2048);
        for ( size_t i = 0; i < all.size(); ++i )
            CHECK(all[i].GetPositionInSet() == static_cast<int>(i + 1));
        CHECK(list->readCalls == 0u);
        CHECK(native.Items().Size() == nativeCount);
        CHECK(nativeCount < 64u);
        CHECK(list->GetViewStart() == view);
        REQUIRE(list->SetItemState(-1, 0, wxLIST_STATE_SELECTED));
        CHECK(selection.GetSelection().empty());
    }
}

TEST_CASE("WinUIListCtrl::ReportGlobalSelectionRetiresRowsAndContainerSafely",
          "[winui-listctrl][winui-listctrl-selection]")
{
    const bool destroy = GENERATE(false, true);
    NativeListFixture fixture;
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    REQUIRE(list->InsertItem(0, "Old") == 0);
    REQUIRE(list->InsertItem(1, "Survivor") == 1);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto selection = peer.GetPattern(A::Peers::PatternInterface::Selection)
        .as<A::Provider::ISelectionProvider>();
    REQUIRE(list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    const auto old = ListSelectedPeers(peer)[0];
    const auto oldSelection = old.GetPattern(A::Peers::PatternInterface::SelectionItem)
        .as<A::Provider::ISelectionItemProvider>();
    REQUIRE(list->DeleteItem(0));
    REQUIRE(list->InsertItem(0, "Replacement") == 0);
    ListUnavailable([&] { (void)oldSelection.IsSelected(); });
    ListUnavailable([&] { oldSelection.Select(); });
    REQUIRE(list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    const auto replacement = ListSelectedPeers(peer)[0];
    CHECK(replacement != old);
    const auto current = replacement.GetPattern(A::Peers::PatternInterface::SelectionItem)
        .as<A::Provider::ISelectionItemProvider>();
    unsigned events = 0;
    list->Bind(wxEVT_LIST_ITEM_DESELECTED, [&](wxListEvent&)
    {
        ++events;
        if ( destroy )
        {
            fixture.list = nullptr;
            fixture.frame->GetSizer()->Detach(list);
            delete list;
        }
        else REQUIRE(list->DeleteItem(0));
    });
    current.RemoveFromSelection();
    CHECK(events == 1u);
    ListUnavailable([&] { (void)current.IsSelected(); });
    ListUnavailable([&] { current.Select(); });
    if ( destroy ) ListUnavailable([&] { (void)selection.GetSelection(); });
    else
    {
        CHECK(list->GetItemText(0) == "Survivor");
        CHECK(selection.GetSelection().empty());
    }
}

TEST_CASE("WinUIListCtrl::ReportOffscreenSelectionRaisesRealAutomationEvent",
          "[winui-listctrl][winui-listctrl-selection]")
{
    NativeListFixture fixture;
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame);
    fixture.list = list;
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    list->SetItemCount(100000);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    REQUIRE(ListSelectedPeers(peer).empty());
    auto* const host = wxWinUITopLevelHost::ForWindow(list, false);
    REQUIRE(host);
    ListSelectionObserver observer(host->GetBridgeHwnd());
    const auto state = observer.state;
    const bool ready = WaitList([&] { return state->ready.load() || state->done.load(); });
    INFO("UIA listener HRESULT=" << state->error.load());
    REQUIRE(ready);
    REQUIRE(state->ready.load());
    const auto visible = ListCell(list, 0);
    const auto view = list->GetViewStart();
    const auto native = FindListElement<C::ListView>(ListNativeTree(list), L"wxList.NativeItems");
    const auto nativeCount = native.Items().Size();
    const auto before = state->events.load();
    list->readRows.clear();
    REQUIRE(list->SetItemState(90000, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    REQUIRE(WaitList([&] { return state->events.load() > before; }));
    CHECK(ListCell(list, 0) == visible);
    CHECK(native.Items().Size() == nativeCount);
    CHECK(list->GetViewStart() == view);
    // wxEVT_LIST_ITEM_SELECTED itself reads the changed row's event payload.
    // Publication must not reread values from any surviving viewport row.
    for ( const auto readRow : list->readRows ) CHECK(readRow == 90000);
    const auto selected = ListSelectedPeers(peer);
    REQUIRE(selected.size() == 1);
    CHECK(selected[0].GetPositionInSet() == 90001);
    CHECK(selected[0].IsOffscreen());
    CHECK(observer.Stop());
    CHECK(state->error.load() == S_OK);
}

#if wxUSE_TEXTCTRL
TEST_CASE("WinUIListCtrl::ReportValueUsesNativeLabelEditorAndWxVeto",
          "[winui-listctrl][winui-listctrl-value]")
{
    const int veto = GENERATE(0, 1, 2);
    const bool rtl = GENERATE(false, true);
    NativeListFixture fixture(wxLC_REPORT | wxLC_EDIT_LABELS, rtl);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    REQUIRE(list->InsertColumn(1, "Read only", wxLIST_FORMAT_LEFT, 180) == 1);
    REQUIRE(list->InsertItem(0, "Original") == 0);
    REQUIRE(list->SetItem(0, 1, "Exact subitem text"));
    REQUIRE(list->SetColumnsOrder(ListOrder({1, 0})));
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto cell = ListProviderPeer(peer, grid.GetItem(0, 1));
    const auto value = cell.GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    CHECK_FALSE(value.IsReadOnly());
    CHECK(value.Value() == L"Original");
    unsigned begins = 0, ends = 0;
    C::RichEditBox nativeEditor{nullptr};
    list->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++begins;
        CHECK(event.GetIndex() == 0);
        CHECK(event.GetEventObject() == list);
        CHECK(event.GetText() == "Original");
        auto* const editor = list->GetEditControl();
        REQUIRE(editor);
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editor);
        auto* const slot = host ? host->FindSlot(editor) : nullptr;
        nativeEditor = slot ? slot->GetSemanticTarget().try_as<C::RichEditBox>() : nullptr;
        CHECK(nativeEditor);
        if ( veto == 1 ) event.Veto();
    });
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++ends;
        CHECK_FALSE(event.IsEditCancelled());
        CHECK(event.GetIndex() == 0);
        CHECK(event.GetText() == "Renamed label");
        CHECK(list->GetEditControl()->GetValue() == "Renamed label");
        CHECK(list->GetItemText(0) == "Original");
        if ( veto == 2 ) event.Veto();
    });
    if ( veto ) ListAutomationError([&] { value.SetValue(L"Renamed label"); }, UIA_E_INVALIDOPERATION);
    else value.SetValue(L"Renamed label");
    CHECK(begins == 1u);
    CHECK(ends == (veto == 1 ? 0u : 1u));
    CHECK_FALSE(list->GetEditControl());
    CHECK(value.Value() == (veto ? L"Original" : L"Renamed label"));
    REQUIRE(WaitList([&] { return ListCellText(ListCell(list, 0)) == (veto ? "Original" : "Renamed label"); }));
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListCell(list, 0)) == cell);
    const auto other = ListProviderPeer(peer, grid.GetItem(0, 0))
        .GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    CHECK(other.IsReadOnly());
    CHECK(other.Value() == L"Exact subitem text");
    ListAutomationError([&] { other.SetValue(L"must not replace a subitem"); }, UIA_E_INVALIDOPERATION);
    list->Disable();
    CHECK_FALSE(value.IsReadOnly()); // Disabled is distinct from non-editable.
    ListAutomationError([&] { value.SetValue(L"disabled"); }, UIA_E_ELEMENTNOTENABLED);
    CHECK(begins == 1u);
}

TEST_CASE("WinUIListCtrl::ReportValuePreservesAnExistingLabelDraft",
          "[winui-listctrl][winui-listctrl-value]")
{
    NativeListFixture fixture(wxLC_REPORT | wxLC_EDIT_LABELS);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    REQUIRE(list->InsertItem(0, "Original") == 0);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto value = ListProviderPeer(peer, grid.GetItem(0, 0))
        .GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    auto* const editor = list->EditLabel(0);
    REQUIRE(editor);
    editor->SetValue("Uncommitted user draft");
    unsigned ended = 0;
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent&) { ++ended; });
    ListAutomationError([&] { value.SetValue(L"automation overwrite"); }, UIA_E_INVALIDOPERATION);
    CHECK(ended == 0u);
    CHECK(list->GetEditControl() == editor);
    CHECK(editor->GetValue() == "Uncommitted user draft");
    CHECK(value.Value() == L"Original");
    REQUIRE(list->EndEditLabel(true));
    CHECK(ended == 1u);
    CHECK(value.Value() == L"Original");
}

TEST_CASE("WinUIListCtrl::ReportVirtualValueUsesApplicationEndEditModel",
          "[winui-listctrl][winui-listctrl-value]")
{
    NativeListFixture fixture;
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame, true);
    fixture.list = list;
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    list->SetItemCount(100000);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto cell = ListProviderPeer(peer, grid.GetItem(90000, 0));
    const auto value = cell.GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    unsigned committed = 0;
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent& event)
    {
        CHECK(event.GetIndex() == 90000);
        CHECK_FALSE(event.IsEditCancelled());
        list->editedRow = event.GetIndex();
        list->editedValue = event.GetText();
        ++committed;
    });
    list->readCalls = 0;
    list->readRows.clear();
    CHECK(value.Value() == L"Row 90000/0");
    CHECK(list->readCalls == 1u);
    value.SetValue(L"Virtual model commit");
    CHECK(committed == 1u);
    CHECK(value.Value() == L"Virtual model commit");
    CHECK(list->readRows.size() < 128);
    const bool reflected = WaitList([&]
        { return ListCellText(ListCell(list, 90000)) == "Virtual model commit"; });
    const auto displayed = ListCell(list, 90000);
    INFO("committed row present=" << bool(displayed)
         << " text=" << ListCellText(displayed)
         << " view=" << list->GetViewStart().x << ',' << list->GetViewStart().y);
    REQUIRE(reflected);
    CHECK(list->readRows.size() < 128);
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListCell(list, 90000)) == cell);
    CHECK_FALSE(list->GetEditControl());
    list->onRead = [&] { list->SetItemCount(10); };
    ListUnavailable([&] { (void)value.Value(); });
    ListUnavailable([&] { value.SetValue(L"retired"); });
    CHECK(committed == 1u);
}

TEST_CASE("WinUIListCtrl::ReportValueRejectsDestructiveBeginAndEndCallbacks",
          "[winui-listctrl][winui-listctrl-value]")
{
    const bool atBegin = GENERATE(false, true);
    const bool destroy = GENERATE(false, true);
    NativeListFixture fixture(wxLC_REPORT | wxLC_EDIT_LABELS);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    REQUIRE(list->InsertItem(0, "Original") == 0);
    REQUIRE(list->InsertItem(1, "Survivor") == 1);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto value = ListProviderPeer(peer, grid.GetItem(0, 0))
        .GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    unsigned callbacks = 0;
    list->Bind(atBegin ? wxEVT_LIST_BEGIN_LABEL_EDIT : wxEVT_LIST_END_LABEL_EDIT,
        [&](wxListEvent&)
        {
            ++callbacks;
            if ( destroy )
            {
                fixture.frame->GetSizer()->Detach(list);
                fixture.list = nullptr;
                delete list;
            }
            else REQUIRE(list->DeleteItem(0));
            // Consumed: dispatch must not continue on a destroyed wx handler.
        });
    ListUnavailable([&] { value.SetValue(L"Never write into the surviving row"); });
    CHECK(callbacks == 1u);
    if ( !destroy )
    {
        CHECK(list->GetItemCount() == 1);
        CHECK(list->GetItemText(0) == "Survivor");
        CHECK_FALSE(list->GetEditControl());
    }
    ListUnavailable([&] { (void)value.Value(); });
}
#endif // wxUSE_TEXTCTRL

TEST_CASE("WinUIListCtrl::ReportGridTableUsesRealCellsAndDisplayedColumnOrder",
          "[winui-listctrl][winui-listctrl-uia]")
{
    const bool rtl = GENERATE(false, true);
    NativeListFixture fixture(wxLC_REPORT, rtl);
    auto* const list = fixture.list;
    for ( int column = 0; column < 3; ++column )
        REQUIRE(list->InsertColumn(column, wxString::Format("Column %d", column),
                    wxLIST_FORMAT_LEFT, list->FromDIP(140)) == column);
    for ( long row = 0; row < 4; ++row )
    {
        REQUIRE(list->InsertItem(row, wxString::Format("Name %ld", row)) == row);
        REQUIRE(list->SetItem(row, 1, wxString::Format("Value %ld", row)));
        REQUIRE(list->SetItem(row, 2, wxString::Format("Other %ld", row)));
    }
    fixture.Show();
    const auto peer = ListGridPeer(list);
    REQUIRE(peer);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).try_as<A::Provider::IGridProvider>();
    const auto table = peer.GetPattern(A::Peers::PatternInterface::Table).try_as<A::Provider::ITableProvider>();
    REQUIRE(grid);
    REQUIRE(table);
    CHECK(peer.GetAutomationControlType() == A::Peers::AutomationControlType::DataGrid);
    CHECK(grid.RowCount() == 4);
    CHECK(grid.ColumnCount() == 3);
    CHECK(table.RowOrColumnMajor() == A::RowOrColumnMajor::RowMajor);
    CHECK(table.GetRowHeaders().empty());
    const auto cell = ListProviderPeer(peer, grid.GetItem(1, 2));
    const auto item = cell.GetPattern(A::Peers::PatternInterface::GridItem).as<A::Provider::IGridItemProvider>();
    const auto tableItem = cell.GetPattern(A::Peers::PatternInterface::TableItem).as<A::Provider::ITableItemProvider>();
    CHECK(item.Row() == 1);
    CHECK(item.Column() == 2);
    CHECK(item.RowSpan() == 1);
    CHECK(item.ColumnSpan() == 1);
    CHECK(ListProviderPeer(peer, item.ContainingGrid()) == peer);
    CHECK(cell.GetName() == L"Other 1");
    const auto readValue = cell.GetPattern(A::Peers::PatternInterface::Value).as<A::Provider::IValueProvider>();
    CHECK(readValue.IsReadOnly());
    CHECK(readValue.Value() == L"Other 1");
    REQUIRE(WaitList([&] { return ListCell(list, 1, 2) && !cell.IsOffscreen(); }));
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListCell(list, 1, 2)) == cell);
    const auto visualRect = ListScreenRect(list, ListCell(list, 1, 2));
    const auto bounds = cell.GetBoundingRectangle();
    CHECK(std::abs(bounds.X - visualRect.x) <= 2);
    CHECK(std::abs(bounds.Y - visualRect.y) <= 2);
    CHECK(std::abs(bounds.Width - visualRect.width) <= 2);
    const auto row = ListReportRow(list, 1);
    const auto rowPeer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(row);
    const auto rowChildren = rowPeer.GetChildren();
    REQUIRE(rowChildren);
    uint32_t cellIndex = 0;
    CHECK(rowChildren.IndexOf(cell, cellIndex));
    CHECK(cell.GetParent() == rowPeer);
    CHECK(ListProviderPeer(peer, grid.GetItem(1, 2)) == cell);
    CHECK(cell.GetParent() == rowPeer); // GetItem cannot steal a realized cell from its row.
    const auto selection = ListSelection(row);
    REQUIRE(selection);
    selection.Select();
    CHECK(list->GetItemState(1, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    CHECK(ListProviderPeer(peer, selection.SelectionContainer()) == peer);

    const auto headers = table.GetColumnHeaders();
    REQUIRE(headers.size() == 3);
    const auto header = ListProviderPeer(peer, headers[2]);
    CHECK(header.GetName() == L"Column 2");
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListHeader(list, 2)) == header);
    auto* const headerWindow = wxWindow::FindWindowByName("wxlistctrlcolumntitles", list);
    REQUIRE(headerWindow);
    CHECK(headerWindow->CanAcceptFocus());
    CHECK(headerWindow->CanAcceptFocusFromKeyboard());
    CHECK(header.IsKeyboardFocusable());
    list->SetFocus();
    header.SetFocus();
    REQUIRE(WaitList([&] { return header.HasKeyboardFocus(); }));
    const auto firstHeader = ListProviderPeer(peer, headers[0]);
    const auto secondHeader = ListProviderPeer(peer, headers[1]);
    firstHeader.SetFocus();
    REQUIRE(WaitList([&] { return firstHeader.HasKeyboardFocus(); }));
    X::Input::FindNextElementOptions navigation;
    // WinUI TryMoveFocusStatic permits Next/Previous in an island only with
    // its public XamlRoot.Content as SearchRoot; an arbitrary subtree is for
    // directional XY navigation and is explicitly rejected with E_INVALIDARG.
    navigation.SearchRoot(ListNativeTree(list));
    bool moved = false;
    try
    {
        moved = X::Input::FocusManager::TryMoveFocus(X::Input::FocusNavigationDirection::Next, navigation);
    }
    catch ( const winrt::hresult_error& error )
    {
        FAIL("native header Tab navigation HRESULT=" << static_cast<int32_t>(error.code())
             << " message=" << winrt::to_string(error.message()));
    }
    REQUIRE(moved);
    REQUIRE(WaitList([&] { return secondHeader.HasKeyboardFocus(); }));
    REQUIRE(tableItem.GetColumnHeaderItems().size() == 1);
    CHECK(ListProviderPeer(peer, tableItem.GetColumnHeaderItems()[0]) == header);
    int clicked = -1;
    list->Bind(wxEVT_LIST_COL_CLICK, [&](wxListEvent& event) { clicked = event.GetColumn(); });
    header.GetPattern(A::Peers::PatternInterface::Invoke).as<A::Provider::IInvokeProvider>().Invoke();
    CHECK(clicked == 2);
    REQUIRE(list->SetColumnsOrder(ListOrder({2, 0, 1})));
    CheckListOrder(list, ListOrder({2, 0, 1}), rtl);
    CHECK(ListProviderPeer(peer, grid.GetItem(1, 0)) == cell);
    CHECK(item.Column() == 0);
    CHECK(ListReportRow(list, 1) == row);
    CHECK(ListProviderPeer(peer, table.GetColumnHeaders()[0]) == header);
    REQUIRE(list->SetColumnWidth(2, 0));
    REQUIRE(WaitList([&] { return cell.IsOffscreen(); }));
    CHECK(grid.ColumnCount() == 3); // Zero-width columns retain logical identity.
    CHECK(ListProviderPeer(peer, grid.GetItem(1, 0)) == cell);
    CHECK_THROWS_AS(cell.GetPattern(A::Peers::PatternInterface::VirtualizedItem)
        .as<A::Provider::IVirtualizedItemProvider>().Realize(), winrt::hresult_error);
    CHECK_THROWS_AS(grid.GetItem(-1, 0), winrt::hresult_error);
    CHECK_THROWS_AS(grid.GetItem(4, 0), winrt::hresult_error);
    CHECK_THROWS_AS(grid.GetItem(0, 3), winrt::hresult_error);
}

TEST_CASE("WinUIListCtrl::ReportGridVirtualCellRealizesBoundedWxViewport",
          "[winui-listctrl][winui-listctrl-uia]")
{
    const bool rtl = GENERATE(false, true);
    NativeListFixture fixture(wxLC_REPORT, rtl);
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame);
    fixture.list = list;
    list->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    for ( int column = 0; column < 12; ++column )
        REQUIRE(list->InsertColumn(column, wxString::Format("Header %d", column),
                    wxLIST_FORMAT_LEFT, list->FromDIP(140)) == column);
    list->SetItemCount(100000);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    REQUIRE(peer);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto table = peer.GetPattern(A::Peers::PatternInterface::Table).as<A::Provider::ITableProvider>();
    list->readCalls = 0;
    list->readRows.clear();
    const auto distant = ListProviderPeer(peer, grid.GetItem(90000, 11));
    const auto position = distant.GetPattern(A::Peers::PatternInterface::GridItem).as<A::Provider::IGridItemProvider>();
    CHECK(grid.RowCount() == 100000);
    CHECK(grid.ColumnCount() == 12);
    CHECK(position.Row() == 90000);
    CHECK(position.Column() == 11);
    CHECK(distant.IsOffscreen());
    CHECK(ListProviderPeer(peer, grid.GetItem(90000, 11)) == distant);
    const auto children = peer.GetChildren();
    CHECK(children.Size() < 128);
    CHECK(table.GetColumnHeaders().size() == 12);
    CHECK(list->readCalls == 0); // Metadata/tree queries cannot enumerate values.
    CHECK(distant.GetName() == L"Row 90000/11");
    CHECK(list->readCalls == 1);
    CHECK(list->readRows == std::set<long>{90000});
    list->readCalls = 0;
    list->readRows.clear();
    distant.GetPattern(A::Peers::PatternInterface::VirtualizedItem)
        .as<A::Provider::IVirtualizedItemProvider>().Realize();
    REQUIRE(WaitList([&] { return ListCell(list, 90000, 11) && !distant.IsOffscreen(); }));
    CHECK(A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(ListCell(list, 90000, 11)) == distant);
    CHECK(ListProviderPeer(peer, grid.GetItem(90000, 11)) == distant);
    CHECK(list->readRows.size() < 128);
    CHECK(list->readCalls < 2048);
    CHECK(CountListCells(ListNativeTree(list)) < 512);
    CHECK_FALSE(ListCell(list, 0, 0));
    const auto selection = ListSelection(ListReportRow(list, 90000));
    REQUIRE(selection);
    selection.Select();
    CHECK(list->GetItemState(90000, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    list->Disable();
    CHECK_FALSE(distant.IsEnabled());
    CHECK(distant.GetName() == L"Row 90000/11");
    CHECK_THROWS_AS(distant.GetPattern(A::Peers::PatternInterface::ScrollItem)
        .as<A::Provider::IScrollItemProvider>().ScrollIntoView(), winrt::hresult_error);
}

TEST_CASE("WinUIListCtrl::ReportGridRetiresSchemaAndDestroyedOwnerProviders",
          "[winui-listctrl][winui-listctrl-uia]")
{
    NativeListFixture fixture;
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 180) == 0);
    REQUIRE(list->InsertColumn(1, "Value", wxLIST_FORMAT_LEFT, 180) == 1);
    REQUIRE(list->InsertItem(0, "Original") == 0);
    REQUIRE(list->InsertItem(1, "Second") == 1);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto table = peer.GetPattern(A::Peers::PatternInterface::Table).as<A::Provider::ITableProvider>();
    const auto cell = ListProviderPeer(peer, grid.GetItem(0, 0));
    const auto item = cell.GetPattern(A::Peers::PatternInterface::GridItem).as<A::Provider::IGridItemProvider>();
    const auto header = ListProviderPeer(peer, table.GetColumnHeaders()[0]);
    const auto invoke = header.GetPattern(A::Peers::PatternInterface::Invoke).as<A::Provider::IInvokeProvider>();
    int clicks = 0;
    list->Bind(wxEVT_LIST_COL_CLICK, [&](wxListEvent&) { ++clicks; });
    REQUIRE(list->DeleteItem(0));
    ListUnavailable([&] { (void)item.Row(); });
    ListUnavailable([&] { (void)cell.GetName(); });
    const auto replacement = ListProviderPeer(peer, grid.GetItem(0, 0));
    CHECK(replacement != cell);
    CHECK(replacement.GetName() == L"Second");
    REQUIRE(list->DeleteColumn(0));
    invoke.Invoke();
    CHECK(clicks == 0);
    ListUnavailable([&] { (void)replacement.GetName(); });
    CHECK(grid.ColumnCount() == 1);
    fixture.frame->GetSizer()->Detach(list);
    delete list;
    fixture.list = nullptr;
    ListUnavailable([&] { (void)grid.RowCount(); });
    ListUnavailable([&] { (void)table.GetColumnHeaders(); });
    ListUnavailable([&] { (void)item.ContainingGrid(); });
}

TEST_CASE("WinUIListCtrl::ReportGridVirtualNameRejectsReentrantReplacement",
          "[winui-listctrl][winui-listctrl-uia]")
{
    const bool destroy = GENERATE(false, true);
    NativeListFixture fixture;
    fixture.frame->GetSizer()->Detach(fixture.list);
    delete fixture.list;
    auto* const list = new VirtualNativeList(fixture.frame);
    fixture.list = list;
    fixture.frame->GetSizer()->Add(list, 1, wxEXPAND);
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 200) == 0);
    list->SetItemCount(100);
    fixture.Show();
    const auto peer = ListGridPeer(list);
    const auto grid = peer.GetPattern(A::Peers::PatternInterface::Grid).as<A::Provider::IGridProvider>();
    const auto cell = ListProviderPeer(peer, grid.GetItem(90, 0));
    list->onRead = [&]
    {
        if ( destroy )
        {
            fixture.frame->GetSizer()->Detach(list);
            fixture.list = nullptr;
            delete list;
        }
        else list->SetItemCount(10);
    };
    ListUnavailable([&] { (void)cell.GetName(); });
    if ( destroy ) ListUnavailable([&] { (void)grid.RowCount(); });
    else CHECK(grid.RowCount() == 10);
}

TEST_CASE("WinUIListCtrl::AllPublicViewsRenderRealNativeCells",
          "[winui-listctrl][native-listctrl]")
{
    const long mode = GENERATE(wxLC_REPORT, wxLC_LIST, wxLC_ICON, wxLC_SMALL_ICON);
    const bool rtl = GENERATE(false, true);
    CAPTURE(mode, rtl);
    NativeListFixture fixture(mode | wxLC_SINGLE_SEL, rtl);
    auto* const list = fixture.list;
    if ( mode == wxLC_REPORT )
    {
        REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, list->FromDIP(220)) == 0);
        REQUIRE(list->InsertColumn(1, "Value", wxLIST_FORMAT_RIGHT, list->FromDIP(130)) == 1);
    }
    auto* const smallImages = new wxImageList(16, 16);
    REQUIRE(smallImages->Add(ListTestBitmap(16)) == 0);
    list->AssignImageList(smallImages, wxIMAGE_LIST_SMALL);
    auto* const normalImages = new wxImageList(32, 32);
    REQUIRE(normalImages->Add(ListTestBitmap(32)) == 0);
    list->AssignImageList(normalImages, wxIMAGE_LIST_NORMAL);
    for ( long row = 0; row < 4; ++row )
    {
        REQUIRE(list->InsertItem(row, wxString::Format("Item %ld", row), 0) == row);
        if ( mode == wxLC_REPORT )
            REQUIRE(list->SetItem(row, 1, wxString::Format("Value %ld", row)));
        REQUIRE(list->SetItemPtrData(row, static_cast<wxUIntPtr>(9000 + row)));
    }
    fixture.Show();
    const auto cell = ListDisplayedCell(list, 0);
    REQUIRE(cell);
    REQUIRE(WaitList([&]() { return ListCellText(cell) == "Item 0"; }));
    C::Image image{nullptr};
    REQUIRE(WaitList([&]()
    {
        image = FindListElement<C::Image>(cell);
        return image && image.IsLoaded() && image.ActualWidth() > 0;
    }));
    CHECK(image.Source() != nullptr);
    const auto imageSource = image.Source();
    CHECK(image.IsLoaded());
    CHECK(image.ActualWidth() > 0);
    CHECK(cell.FlowDirection() == (rtl ? X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight));
    if ( mode == wxLC_REPORT || mode == wxLC_LIST )
        CHECK(cell.ActualHeight() >= 32.0);
    CHECK(list->GetItemCount() == 4);
    CHECK(list->GetItemData(2) == 9002);
    if ( mode == wxLC_REPORT )
    {
        REQUIRE(WaitList([&]()
        {
            const auto header = ListHeader(list, 0);
            const auto value = ListCell(list, 0, 1);
            return header && header.IsLoaded() && value && value.IsLoaded();
        }));
        CHECK(ListCellText(ListHeader(list, 0)) == "Name");
        CHECK(ListCellText(ListCell(list, 0, 1)) == "Value 0");
        const auto row = ListReportRow(list, 0);
        REQUIRE(row);
        CHECK(row.IsLoaded());
        CHECK(ListReportAncestor(cell) == row);
        CHECK(ListReportAncestor(ListCell(list, 0, 1)) == row);
        CHECK_FALSE(cell.try_as<CP::ToggleButton>());
        REQUIRE(ListSelection(row));
        CHECK(ListSelection(row).SelectionContainer() != nullptr);
        CHECK(std::abs(cell.ActualWidth() -
              list->GetColumnWidth(0) / list->GetDPIScaleFactor()) <= 1.0);
    }
    else
    {
        CHECK_FALSE(ListHeader(list, 0));
        CHECK_FALSE(ListDisplayedCell(list, 0, 1));
    }
    list->SetItemText(0, "Updated in place");
    REQUIRE(WaitList([&]() { return ListCellText(ListDisplayedCell(list, 0)) == "Updated in place"; }));
    CHECK(ListDisplayedCell(list, 0) == cell);
    const auto updatedImage = FindListElement<C::Image>(ListDisplayedCell(list, 0));
    REQUIRE(updatedImage);
    CHECK(updatedImage.Source() == imageSource);
    CHECK(list->GetItemData(0) == 9000);
}

TEST_CASE("WinUIListCtrl::NativeSelectionHeaderAndCheckboxUsePublicEvents",
          "[winui-listctrl][native-listctrl][native-events]")
{
    NativeListFixture fixture(wxLC_REPORT | wxLC_SINGLE_SEL);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 240) == 0);
    REQUIRE(list->InsertColumn(1, "Data", wxLIST_FORMAT_LEFT, 140) == 1);
    REQUIRE(list->InsertItem(0, "First") == 0);
    REQUIRE(list->InsertItem(1, "Second") == 1);
    REQUIRE(list->SetItemData(1, 12345));
    REQUIRE(list->EnableCheckBoxes());
    fixture.Show();
    REQUIRE(WaitList([&]() { return ListCell(list, 1) && ListCheck(list, 1) && ListHeader(list, 1); }));
    unsigned selected = 0, checked = 0, headers = 0;
    list->Bind(wxEVT_LIST_ITEM_SELECTED, [&](wxListEvent& event)
    {
        ++selected;
        CHECK(event.GetIndex() == 1);
        CHECK(event.GetId() == list->GetId());
        CHECK(event.GetEventObject() == list);
        CHECK(event.GetData() == 12345);
    });
    list->Bind(wxEVT_LIST_ITEM_CHECKED, [&](wxListEvent& event)
    {
        ++checked;
        CHECK(event.GetIndex() == 1);
        CHECK(event.GetEventObject() == list);
    });
    list->Bind(wxEVT_LIST_COL_CLICK, [&](wxListEvent& event)
    {
        ++headers;
        CHECK(event.GetColumn() == 1);
        CHECK(event.GetEventObject() == list);
    });
    const auto selection = ListSelection(ListReportRow(list, 1));
    const auto checkbox = ListToggle(ListCheck(list, 1));
    const auto header = ListInvoke(ListHeader(list, 1));
    REQUIRE(selection);
    REQUIRE(checkbox);
    REQUIRE(header);
    selection.Select();
    REQUIRE(WaitList([&]() { return selected == 1; }));
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK(list->GetItemState(1, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    CHECK(ListReportRow(list, 1).IsSelected());
    checkbox.Toggle();
    REQUIRE(WaitList([&]() { return checked == 1 && list->IsItemChecked(1); }));
    header.Invoke();
    REQUIRE(WaitList([&]() { return headers == 1; }));
    CHECK(selected == 1);
    CHECK(checked == 1);
}

TEST_CASE("WinUIListCtrl::ListAndIconModesUseNativeItemContainersAndFitText",
          "[winui-listctrl][native-item-container][native-text-layout]")
{
    const long mode = GENERATE(wxLC_LIST, wxLC_ICON, wxLC_SMALL_ICON);
    const bool rtl = GENERATE(false, true);
    CAPTURE(mode, rtl);
    NativeListFixture fixture(mode | wxLC_SINGLE_SEL, rtl);
    auto* const list = fixture.list;
    auto* const images = new wxImageList(16, 16);
    REQUIRE(images->Add(ListTestBitmap(16)) == 0);
    list->AssignImageList(images, wxIMAGE_LIST_SMALL);
    auto* const large = new wxImageList(32, 32);
    REQUIRE(large->Add(ListTestBitmap(32)) == 0);
    list->AssignImageList(large, wxIMAGE_LIST_NORMAL);
    const wxString label = "Quarterly planning notes - approved.txt";
    REQUIRE(list->InsertItem(0, label, 0) == 0);
    REQUIRE(list->InsertItem(1, "Other file.txt", 0) == 1);
    fixture.frame->Layout();
    fixture.frame->ShowWithoutActivating();
    REQUIRE(WaitList([&]
    {
        const auto cell = ListSelectorCell(list, 0);
        const auto text = FindListElement<C::TextBlock>(cell);
        return cell && cell.IsLoaded() && cell.ActualWidth() > 0 &&
            text && text.IsLoaded() && text.ActualWidth() > 0 && ListCellText(cell) == label;
    }));
    const auto cell = ListSelectorCell(list, 0);
    CHECK_FALSE(cell.try_as<CP::ToggleButton>());
    if ( mode == wxLC_LIST ) REQUIRE(cell.try_as<C::ListViewItem>());
    else REQUIRE(cell.try_as<C::GridViewItem>());
    X::DependencyObject ancestor = X::Media::VisualTreeHelper::GetParent(cell);
    C::ListViewBase selector{nullptr};
    while ( ancestor && !selector )
    {
        selector = ancestor.try_as<C::ListViewBase>();
        ancestor = X::Media::VisualTreeHelper::GetParent(ancestor);
    }
    REQUIRE(selector);
    if ( mode == wxLC_LIST ) CHECK(selector.try_as<C::ListView>() != nullptr);
    else CHECK(selector.try_as<C::GridView>() != nullptr);
    REQUIRE(ListSelection(cell));
    CHECK(ListSelection(cell).SelectionContainer() != nullptr);

    const auto checkText = [&]
    {
        const auto text = FindListElement<C::TextBlock>(cell);
        const auto image = FindListElement<C::Image>(cell);
        REQUIRE(text);
        REQUIRE(image);
        REQUIRE(text.IsLoaded());
        const wxRect cellRect = ListScreenRect(list, cell);
        const wxRect textRect = ListScreenRect(list, text);
        CHECK(textRect.GetLeft() >= cellRect.GetLeft() - 1);
        CHECK(textRect.GetRight() <= cellRect.GetRight() + 1);
        CHECK(textRect.GetTop() >= cellRect.GetTop() - 1);
        CHECK(textRect.GetBottom() <= cellRect.GetBottom() + 1);
        if ( mode == wxLC_LIST )
        {
            INFO("native text width=" << text.ActualWidth()
                 << " natural=" << ListNaturalTextWidth(text));
            CHECK(text.ActualWidth() + 0.5 >= ListNaturalTextWidth(text));
            CHECK_FALSE(text.IsTextTrimmed());
        }
        int flags = 0;
        CHECK(list->HitTest(list->ScreenToClient(wxPoint(
            textRect.x + textRect.width / 2, textRect.y + textRect.height / 2)), flags) == 0);
    };
    checkText();
    const wxString longer = "Quarterly planning notes - final and approved.txt";
    list->SetItemText(0, longer);
    REQUIRE(WaitList([&]
    {
        wxRect expected;
        return ListCellText(ListSelectorCell(list, 0)) == longer &&
            list->GetItemRect(0, expected) &&
            std::abs(cell.ActualWidth() * list->GetDPIScaleFactor() - expected.width) <= 1;
    }));
    CHECK(ListSelectorCell(list, 0) == cell);
    checkText();
    fixture.frame->SetClientSize(fixture.frame->FromDIP(wxSize(770, 410)));
    REQUIRE(WaitList([&]
    {
        wxRect expected;
        return ListSelectorCell(list, 0) == cell && list->GetItemRect(0, expected) &&
            std::abs(ListScreenRect(list, cell).width - expected.width) <= 2;
    }));
    checkText();
}

TEST_CASE("WinUIListCtrl::NativeSelectionItemsFollowWxSingleMultiAndKeyboardState",
          "[winui-listctrl][native-item-container][native-selection]")
{
    const bool single = GENERATE(false, true);
    const long mode = GENERATE(wxLC_LIST, wxLC_REPORT);
    CAPTURE(single, mode);
    NativeListFixture fixture(mode | (single ? wxLC_SINGLE_SEL : 0));
    auto* const list = fixture.list;
    if ( mode == wxLC_REPORT )
        REQUIRE(list->InsertColumn(0, "Native report selection", wxLIST_FORMAT_LEFT, 300) == 0);
    for ( long row = 0; row < 4; ++row )
        REQUIRE(list->InsertItem(row, wxString::Format("Item %ld", row)) == row);
    fixture.frame->Layout();
    fixture.frame->ShowWithoutActivating();
    REQUIRE(WaitList([&] { return ListNativeItem(list, 3) && ListNativeItem(list, 3).IsLoaded(); }));
    unsigned selected = 0;
    list->Bind(wxEVT_LIST_ITEM_SELECTED, [&](wxListEvent& event)
    {
        CHECK(event.GetEventObject() == list);
        CHECK(event.GetId() == list->GetId());
        ++selected;
    });
    const auto first = ListSelection(ListNativeItem(list, 0));
    const auto second = ListSelection(ListNativeItem(list, 1));
    REQUIRE(first);
    REQUIRE(second);
    first.Select();
    REQUIRE(WaitList([&] { return list->GetSelectedItemCount() == 1 && first.IsSelected(); }));
    CHECK(selected == 1);
    if ( single ) second.Select();
    else second.AddToSelection();
    REQUIRE(WaitList([&]
    {
        return second.IsSelected() && list->GetSelectedItemCount() == (single ? 1 : 2);
    }));
    CHECK(selected == 2);
    CHECK(first.IsSelected() == !single);

    // Exercise the existing wx keyboard state machine, with no physical
    // keyboard injection. Native realization must mirror its Ctrl/Shift
    // semantics instead of replacing them with a second selection model.
    REQUIRE(list->SetItemState(1, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED));
    auto* const target = list->GetTargetWindow();
    REQUIRE(target);
    wxKeyEvent controlDown(wxEVT_CHAR);
    controlDown.m_keyCode = WXK_DOWN;
    controlDown.SetControlDown(true);
    controlDown.SetEventObject(target);
    target->GetEventHandler()->ProcessEvent(controlDown);
    REQUIRE(WaitList([&]
    {
        return list->GetItemState(2, wxLIST_STATE_FOCUSED) == wxLIST_STATE_FOCUSED &&
            ListNativeItem(list, 2).IsSelected() == single;
    }));
    CHECK(list->GetSelectedItemCount() == (single ? 1 : 2));
    wxKeyEvent shiftDown(wxEVT_CHAR);
    shiftDown.m_keyCode = WXK_DOWN;
    shiftDown.SetShiftDown(true);
    shiftDown.SetEventObject(target);
    target->GetEventHandler()->ProcessEvent(shiftDown);
    REQUIRE(WaitList([&] { return ListNativeItem(list, 3).IsSelected(); }));
    CHECK(list->GetItemState(3, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    for ( long row = 0; row < 4; ++row )
        CHECK(ListNativeItem(list, row).IsSelected() ==
              (list->GetItemState(row, wxLIST_STATE_SELECTED) != 0));
    if ( single ) CHECK(list->GetSelectedItemCount() == 1);
}

TEST_CASE("WinUIListCtrl::NativeSelectionItemRetirementRejectsReplacements",
          "[winui-listctrl][native-item-container][lifetime]")
{
    NativeListFixture fixture(wxLC_LIST | wxLC_SINGLE_SEL);
    auto* const list = fixture.list;
    REQUIRE(list->InsertItem(0, "Old item") == 0);
    fixture.frame->Layout();
    fixture.frame->ShowWithoutActivating();
    REQUIRE(WaitList([&] { return ListSelectorCell(list, 0) && ListSelectorCell(list, 0).IsLoaded(); }));
    const auto oldCell = ListSelectorCell(list, 0);
    const auto oldProvider = ListSelection(oldCell);
    REQUIRE(oldProvider);
    REQUIRE(list->DeleteItem(0));
    REQUIRE(list->InsertItem(0, "Replacement") == 0);
    REQUIRE(WaitList([&] { return ListCellText(ListSelectorCell(list, 0)) == "Replacement"; }));
    CHECK(ListSelectorCell(list, 0) != oldCell);
    const auto invokeRetired = [](const A::Provider::ISelectionItemProvider& provider)
    {
        try { provider.Select(); }
        catch ( const winrt::hresult_error& error )
        {
            const auto code = static_cast<int32_t>(error.code());
            CHECK((code == static_cast<int32_t>(0x80040200u) ||
                   code == static_cast<int32_t>(0x80040201u)));
        }
    };
    invokeRetired(oldProvider);
    CHECK(list->GetSelectedItemCount() == 0);
    const auto current = ListSelection(ListSelectorCell(list, 0));
    REQUIRE(current);
    const wxWeakRef<wxListCtrl> lifetime(list);
    unsigned events = 0;
    list->Bind(wxEVT_LIST_ITEM_SELECTED, [list, &events](wxListEvent&)
    {
        ++events;
        delete list;
    });
    fixture.list = nullptr;
    current.Select();
    REQUIRE(WaitList([&] { return !lifetime; }));
    CHECK(events == 1);
    invokeRetired(current);
    CHECK(events == 1);
}

TEST_CASE("WinUIListCtrl::NativeSelectionItemSelectClearsWxRowsOutsideTheViewport",
          "[winui-listctrl][native-item-container][native-selection][virtualization][lifetime]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeListFixture fixture(wxLC_LIST, rtl);
    auto* const list = fixture.list;
    for ( long row = 0; row < 100; ++row )
        REQUIRE(list->InsertItem(row, wxString::Format("File number %03ld.txt", row)) == row);
    fixture.Show();
    const auto initial = ListSelectorCell(list, 0);
    REQUIRE(initial);
    const auto initialProvider = ListSelection(initial);
    REQUIRE(initialProvider);
    initialProvider.Select();
    REQUIRE(WaitList([&] { return initial.IsSelected() && list->GetSelectedItemCount() == 1; }));

    // Make the native disable/focus retirement boundary deterministically
    // invalidate a projection snapshot. The old item must still leave the
    // native Items collection before the next snapshot is published.
    unsigned retirementInvalidations = 0;
    const wxWeakRef<wxListCtrl> liveList(list);
    const auto enabledToken = initial.RegisterPropertyChangedCallback(
        C::Control::IsEnabledProperty(),
        [liveList, &retirementInvalidations](const X::DependencyObject& sender, const X::DependencyProperty&)
        {
            if ( liveList && !sender.as<C::Control>().IsEnabled() )
            {
                ++retirementInvalidations;
                liveList->Refresh(false);
            }
        });
    const auto revokeEnabled = wxMakeGuard([&]
    { initial.UnregisterPropertyChangedCallback(C::Control::IsEnabledProperty(), enabledToken); });

    REQUIRE(list->EnsureVisible(90));
    const bool realized = WaitList([&]
    {
        const auto target = ListSelectorCell(list, 90);
        // IsLoaded can remain true on a retained native container after it
        // leaves Items. The contract is removal from the live visual tree,
        // not a particular timing of WinUI's cached Loaded state.
        return target && target.IsLoaded() && target.ActualWidth() > 0 &&
            ListSelectorCell(list, 0) != initial;
    });
    int viewX = 0, viewY = 0;
    list->GetViewStart(&viewX, &viewY);
    wxRect publicRect;
    REQUIRE(list->GetItemRect(90, publicRect));
    const auto diagnosticItem = ListSelectorCell(list, 90);
    const auto diagnosticSelector = FindListElement<C::ListView>(ListNativeTree(list));
    const auto diagnosticScroll = FindListElement<C::ScrollViewer>(diagnosticSelector);
    const auto initialParent = X::Media::VisualTreeHelper::GetParent(initial);
    INFO("view=" << viewX << ',' << viewY
         << " wxRect90=" << publicRect.x << ',' << publicRect.y << ',' << publicRect.width << ',' << publicRect.height
         << " client=" << list->GetClientSize().x << ',' << list->GetClientSize().y
         << " wxRange=" << list->GetScrollRange(wxHORIZONTAL)
         << " initialLoaded=" << initial.IsLoaded()
         << " initialInVisualTree=" << (ListSelectorCell(list, 0) == initial)
         << " initialParent=" << (initialParent
                ? wxString(winrt::get_class_name(initialParent).c_str()).ToStdString() : "none")
         << " target=" << static_cast<bool>(diagnosticItem)
         << " targetLoaded=" << (diagnosticItem && diagnosticItem.IsLoaded())
         << " targetWidth=" << (diagnosticItem ? diagnosticItem.ActualWidth() : -1)
         << " targetCanvas=" << (diagnosticItem ? C::Canvas::GetLeft(diagnosticItem) : -1)
         << " nativeItems=" << (diagnosticSelector ? diagnosticSelector.Items().Size() : 0)
         << " nativeOffset=" << (diagnosticScroll ? diagnosticScroll.HorizontalOffset() : -1)
         << " nativeViewport=" << (diagnosticScroll ? diagnosticScroll.ViewportWidth() : -1));
    REQUIRE(diagnosticSelector);
    uint32_t retiredIndex = 0;
    CHECK_FALSE(diagnosticSelector.Items().IndexOf(initial, retiredIndex));
    // Native coercion can notify both local and inherited effective enabled
    // values. The regression requires reentry, not an SDK callback count.
    CHECK(retirementInvalidations >= 1u);
    REQUIRE(realized);
    CHECK(ListSelectorCell(list, 0) != initial);
    wxRect initialRect;
    REQUIRE(list->GetItemRect(0, initialRect));
    CHECK_FALSE(initialRect.Intersects(wxRect(wxPoint(0, 0), list->GetClientSize())));
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    try { initialProvider.RemoveFromSelection(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) ||
               code == static_cast<int32_t>(0x80040201u)));
    }
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    const auto target = ListSelectorCell(list, 90);
    const auto provider = ListSelection(target);
    REQUIRE(provider);
    provider.AddToSelection();
    REQUIRE(WaitList([&] { return target.IsSelected() && list->GetSelectedItemCount() == 2; }));
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    provider.Select();
    REQUIRE(WaitList([&] { return target.IsSelected() && list->GetSelectedItemCount() == 1; }));
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == 0);
    CHECK(ListSelectorCell(list, 90) == target);
    CHECK(provider.SelectionContainer() != nullptr);

    REQUIRE(list->EnsureVisible(0));
    REQUIRE(WaitList([&]
    {
        const auto first = ListSelectorCell(list, 0);
        return first && first.IsLoaded() && !first.IsSelected();
    }));
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK(list->GetItemState(90, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
}

TEST_CASE("WinUIListCtrl::ColumnOrderMovesNativePeersNotLogicalValues",
          "[winui-listctrl][native-listctrl][native-column-order]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeListFixture fixture(wxLC_REPORT | wxLC_SINGLE_SEL, rtl);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Label", wxLIST_FORMAT_LEFT, list->FromDIP(180)) == 0);
    REQUIRE(list->InsertColumn(1, "Middle", wxLIST_FORMAT_LEFT, list->FromDIP(140)) == 1);
    REQUIRE(list->InsertColumn(2, "Last", wxLIST_FORMAT_LEFT, list->FromDIP(120)) == 2);
    REQUIRE(list->InsertItem(0, "Logical zero") == 0);
    REQUIRE(list->SetItem(0, 1, "Logical one"));
    REQUIRE(list->SetItem(0, 2, "Logical two"));
    REQUIRE(list->SetItemData(0, 34567));
    fixture.Show();
    CheckListOrder(list, ListOrder({0, 1, 2}), rtl);
    const auto nativeRow = ListReportRow(list, 0);
    REQUIRE(nativeRow);
    std::vector<C::ContentControl> cells;
    std::vector<C::Button> headers;
    std::vector<A::Provider::IInvokeProvider> headerProviders;
    for ( int column = 0; column < 3; ++column )
    {
        cells.push_back(ListCell(list, 0, column));
        headers.push_back(ListHeader(list, column));
        headerProviders.push_back(ListInvoke(headers.back()));
        REQUIRE(headerProviders.back());
    }
    unsigned clicked = 0;
    int expectedColumn = wxNOT_FOUND;
    list->Bind(wxEVT_LIST_COL_CLICK, [&](wxListEvent& event)
    {
        ++clicked;
        CHECK(event.GetColumn() == expectedColumn);
        CHECK(event.GetEventObject() == list);
    });

    for ( const auto& order : {ListOrder({2, 0, 1}), ListOrder({1, 2, 0})} )
    {
        REQUIRE(list->SetColumnsOrder(order));
        CheckListOrder(list, order, rtl);
        CHECK(ListReportRow(list, 0) == nativeRow);
        CHECK(list->GetItemText(0, 0) == "Logical zero");
        CHECK(list->GetItemText(0, 1) == "Logical one");
        CHECK(list->GetItemText(0, 2) == "Logical two");
        CHECK(list->GetItemData(0) == 34567);
        for ( int column = 0; column < 3; ++column )
        {
            CAPTURE(column);
            CHECK(ListCell(list, 0, column) == cells[column]);
            CHECK(ListHeader(list, column) == headers[column]);
            CHECK(ListCellText(cells[column]) == list->GetItemText(0, column));
            wxListItem info;
            REQUIRE(list->GetColumn(column, info));
            CHECK(ListCellText(headers[column]) == info.GetText());
            expectedColumn = column;
            const unsigned previous = clicked;
            // A provider retained before reordering must still identify its
            // logical column, not the column now occupying its old position.
            headerProviders[column].Invoke();
            REQUIRE(WaitList([&]() { return clicked == previous + 1; }));
        }
    }
}

TEST_CASE("WinUIListCtrl::ColumnOrderValidatesAndSurvivesTopologyChanges",
          "[winui-listctrl][native-listctrl][native-column-order]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeListFixture fixture(wxLC_REPORT, rtl);
    auto* const list = fixture.list;
    for ( int column = 0; column < 3; ++column )
        REQUIRE(list->InsertColumn(column, wxString::Format("Column %d", column),
                                  wxLIST_FORMAT_LEFT, list->FromDIP(110)) == column);
    REQUIRE(list->InsertItem(0, "Original zero") == 0);
    REQUIRE(list->SetItem(0, 1, "Original one"));
    REQUIRE(list->SetItem(0, 2, "Original two"));
    const auto order = ListOrder({2, 0, 1});
    REQUIRE(list->SetColumnsOrder(order));
    fixture.Show();
    CheckListOrder(list, order, rtl);
    const auto cell = ListCell(list, 0);
    const auto header = ListHeader(list, 0);
    for ( const auto& invalid : {ListOrder({}), ListOrder({0, 1}),
                                ListOrder({0, 1, 2, 3}), ListOrder({0, 0, 2}),
                                ListOrder({-1, 0, 1}), ListOrder({3, 0, 1})} )
    {
        CAPTURE(invalid.size());
        CHECK_FALSE(list->SetColumnsOrder(invalid));
        CheckListOrder(list, order, rtl);
        CHECK(ListCell(list, 0) == cell);
        CHECK(ListHeader(list, 0) == header);
        CHECK(list->GetItemText(0, 0) == "Original zero");
        CHECK(list->GetItemText(0, 1) == "Original one");
        CHECK(list->GetItemText(0, 2) == "Original two");
    }

    REQUIRE(list->InsertColumn(1, "Inserted", wxLIST_FORMAT_LEFT, list->FromDIP(90)) == 1);
    REQUIRE(list->SetItem(0, 1, "New column"));
    CheckListOrder(list, ListOrder({3, 1, 0, 2}), rtl);
    CHECK(list->GetItemText(0, 0) == "Original zero");
    CHECK(list->GetItemText(0, 1) == "New column");
    CHECK(list->GetItemText(0, 2) == "Original one");
    CHECK(list->GetItemText(0, 3) == "Original two");
    for ( int column = 0; column < 4; ++column )
        CHECK(ListCellText(ListCell(list, 0, column)) == list->GetItemText(0, column));
    REQUIRE(list->DeleteColumn(1));
    CheckListOrder(list, order, rtl);
    CHECK(list->GetItemText(0, 1) == "Original one");
    CHECK(list->GetItemText(0, 2) == "Original two");

    REQUIRE(list->DeleteAllItems());
    CHECK(list->GetColumnsOrder() == order);
    REQUIRE(list->InsertItem(0, "Replacement row") == 0);
    CheckListOrder(list, order, rtl);
    list->ClearAll();
    CHECK(list->GetColumnCount() == 0);
    CHECK(list->GetItemCount() == 0);
    CHECK(list->GetColumnsOrder().empty());
    REQUIRE(list->InsertColumn(0, "Fresh zero", wxLIST_FORMAT_LEFT, list->FromDIP(110)) == 0);
    REQUIRE(list->InsertColumn(1, "Fresh one", wxLIST_FORMAT_LEFT, list->FromDIP(120)) == 1);
    REQUIRE(list->InsertItem(0, "Fresh row") == 0);
    CheckListOrder(list, ListOrder({0, 1}), rtl);
    REQUIRE(list->SetColumnsOrder(ListOrder({1, 0})));
    REQUIRE(list->DeleteColumn(0));
    CheckListOrder(list, ListOrder({0}), rtl);
    REQUIRE(list->DeleteColumn(0));
    CHECK(list->GetColumnsOrder().empty());
    REQUIRE(list->InsertColumn(0, "After last deletion", wxLIST_FORMAT_LEFT, list->FromDIP(110)) == 0);
    CheckListOrder(list, ListOrder({0}), rtl);
}

#if wxUSE_TEXTCTRL
TEST_CASE("WinUIListCtrl::NativeListContainerKeepsWxLabelEditorAndCommit",
          "[winui-listctrl][native-item-container][native-editor]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeListFixture fixture(wxLC_LIST | wxLC_SINGLE_SEL | wxLC_EDIT_LABELS, rtl);
    auto* const list = fixture.list;
    REQUIRE(list->InsertItem(0, "Editable file.txt") == 0);
    fixture.Show();
    const auto cell = ListSelectorCell(list, 0);
    REQUIRE(cell);
    unsigned commits = 0;
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent& event)
    {
        CHECK(event.GetIndex() == 0);
        CHECK_FALSE(event.IsEditCancelled());
        ++commits;
        event.Skip();
    });
    auto* const editor = list->EditLabel(0);
    REQUIRE(editor);
    C::RichEditBox nativeEditor{nullptr};
    REQUIRE(WaitList([&]
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editor);
        auto* const slot = host ? host->FindSlot(editor) : nullptr;
        nativeEditor = slot ? slot->GetSemanticTarget().try_as<C::RichEditBox>() : nullptr;
        return nativeEditor && nativeEditor.IsLoaded() && nativeEditor.ActualWidth() > 0;
    }));
    const auto editing = ListScreenRect(list, nativeEditor);
    const auto item = ListScreenRect(list, cell);
    CHECK(item.Contains(editing.x + editing.width / 2, editing.y + editing.height / 2));
    editor->SetValue("Edited filename with a longer label.txt");
    REQUIRE(list->EndEditLabel(false));
    REQUIRE(WaitList([&]
    {
        return commits == 1 && ListCellText(ListSelectorCell(list, 0)) ==
            "Edited filename with a longer label.txt" && !nativeEditor.IsLoaded();
    }));
    CHECK(ListSelectorCell(list, 0) == cell);
    CHECK(list->GetItemText(0) == "Edited filename with a longer label.txt");
}

TEST_CASE("WinUIListCtrl::ReorderedLabelEditorFollowsLogicalColumnZero",
          "[winui-listctrl][native-listctrl][native-column-order][native-editor]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeListFixture fixture(wxLC_REPORT | wxLC_EDIT_LABELS, rtl);
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Label", wxLIST_FORMAT_LEFT, list->FromDIP(180)) == 0);
    REQUIRE(list->InsertColumn(1, "One", wxLIST_FORMAT_LEFT, list->FromDIP(120)) == 1);
    REQUIRE(list->InsertColumn(2, "Two", wxLIST_FORMAT_LEFT, list->FromDIP(120)) == 2);
    REQUIRE(list->InsertItem(0, "Editable label") == 0);
    REQUIRE(list->SetItem(0, 1, "Never edited one"));
    REQUIRE(list->SetItem(0, 2, "Never edited two"));
    REQUIRE(list->SetColumnsOrder(ListOrder({2, 0, 1})));
    fixture.Show();
    CheckListOrder(list, ListOrder({2, 0, 1}), rtl);
    unsigned begins = 0, ends = 0;
    list->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++begins;
        CHECK(event.GetIndex() == 0);
        event.Skip();
    });
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++ends;
        CHECK(event.GetIndex() == 0);
        CHECK_FALSE(event.IsEditCancelled());
        event.Skip();
    });
    auto* const editor = list->EditLabel(0);
    REQUIRE(editor);
    C::RichEditBox nativeEditor{nullptr};
    REQUIRE(WaitList([&]()
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editor);
        auto* const slot = host ? host->FindSlot(editor) : nullptr;
        nativeEditor = slot ? slot->GetSemanticTarget().try_as<C::RichEditBox>() : nullptr;
        return nativeEditor && nativeEditor.IsLoaded() && nativeEditor.ActualWidth() > 0;
    }));
    const auto editorInColumnZero = [&]()
    {
        const wxRect label = ListScreenRect(list, ListCell(list, 0, 0));
        const wxRect editing = ListScreenRect(list, nativeEditor);
        if ( label.IsEmpty() || editing.IsEmpty() ) return false;
        // The common wx label editor deliberately extends a few pixels past
        // the label rectangle for its border; it must still follow this cell.
        return editing.GetLeft() >= label.GetLeft() - 12 &&
               editing.GetRight() <= label.GetRight() + 12 &&
               label.Contains(editing.x + editing.width / 2,
                              editing.y + editing.height / 2);
    };
    REQUIRE(WaitList(editorInColumnZero));
    const wxRect before = ListScreenRect(list, nativeEditor);
    editor->SetValue("Edited logical zero");
    REQUIRE(list->SetColumnsOrder(ListOrder({1, 2, 0})));
    CheckListOrder(list, ListOrder({1, 2, 0}), rtl);
    REQUIRE(WaitList(editorInColumnZero));
    CHECK(list->GetEditControl() == editor);
    CHECK(std::abs(ListScreenRect(list, nativeEditor).x - before.x) > list->FromDIP(50));
    CHECK(editor->GetValue() == "Edited logical zero");
    CHECK(begins == 1);
    CHECK(ends == 0);
    REQUIRE(list->EndEditLabel(false));
    REQUIRE(WaitList([&]()
    {
        return ends == 1 && ListCellText(ListCell(list, 0)) == "Edited logical zero";
    }));
    CHECK(list->GetItemText(0, 0) == "Edited logical zero");
    CHECK(list->GetItemText(0, 1) == "Never edited one");
    CHECK(list->GetItemText(0, 2) == "Never edited two");
}
#endif // wxUSE_TEXTCTRL

TEST_CASE("WinUIListCtrl::VirtualViewportAndRefreshStayBounded",
          "[winui-listctrl][native-listctrl][virtual]")
{
    std::unique_ptr<wxFrame> frame(new wxFrame(nullptr, wxID_ANY, "virtual native list",
        ListPosition(), wxSize(600, 320)));
    auto* const list = new VirtualNativeList(frame.get());
    list->InsertColumn(0, "Row", wxLIST_FORMAT_LEFT, 220);
    list->InsertColumn(1, "Value", wxLIST_FORMAT_LEFT, 160);
    auto* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(list, 1, wxEXPAND);
    frame->SetSizer(sizer);
    list->SetItemCount(100000);
    frame->ShowWithoutActivating();
    frame->Layout();
    REQUIRE(WaitList([&]()
    {
        const auto cell = ListCell(list, 0);
        return cell && cell.IsLoaded() && ListCellText(cell) == "Row 0/0";
    }));
    CHECK(list->readRows.size() < 512);
    CHECK(CountListCells(ListNativeTree(list)) < 512);
    const auto initialSelection = ListSelection(ListReportRow(list, 0));
    REQUIRE(initialSelection);
    initialSelection.Select();
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    REQUIRE(list->EnsureVisible(50000));
    REQUIRE(WaitList([&]()
    {
        const auto cell = ListCell(list, 50000);
        return cell && cell.IsLoaded() && cell.ActualHeight() > 0 &&
               ListCellText(cell) == "Row 50000/0";
    }));
    CHECK(list->GetTopItem() <= 50000);
    CHECK(list->IsVisible(50000));
    CHECK(list->readRows.size() < 512);
    CHECK(CountListCells(ListNativeTree(list)) < 512);
    const auto cell = ListCell(list, 50000);
    const auto otherColumn = ListCell(list, 50000, 1);
    const auto nativeRow = ListReportRow(list, 50000);
    REQUIRE(nativeRow);
    CHECK(ListReportAncestor(cell) == nativeRow);
    CHECK(ListReportAncestor(otherColumn) == nativeRow);
    const auto nativeSelection = ListSelection(nativeRow);
    REQUIRE(nativeSelection);
    nativeSelection.Select();
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == 0);
    CHECK(list->GetItemState(50000, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    list->changedRow = 50000;
    list->RefreshItem(50000);
    REQUIRE(WaitList([&]() { return ListCellText(ListCell(list, 50000)) == "Changed 50000/0"; }));
    CHECK(ListCell(list, 50000) == cell);
    CHECK(ListCell(list, 50000, 1) == otherColumn);
    CHECK(ListReportRow(list, 50000) == nativeRow);
    CHECK(list->readRows.size() < 512);
    CHECK(list->GetItemCount() == 100000);
}

TEST_CASE("WinUIListCtrl::RetiredProvidersCannotSelectReplacementRows",
          "[winui-listctrl][native-listctrl][lifetime]")
{
    NativeListFixture fixture(wxLC_REPORT | wxLC_SINGLE_SEL);
    auto* const list = fixture.list;
    list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 220);
    list->InsertItem(0, "Old row");
    fixture.Show();
    const auto oldCell = ListCell(list, 0);
    const auto oldProvider = ListSelection(ListReportRow(list, 0));
    REQUIRE(oldProvider);
    REQUIRE(list->DeleteItem(0));
    REQUIRE(list->InsertItem(0, "Replacement") == 0);
    REQUIRE(WaitList([&]() { return ListCellText(ListCell(list, 0)) == "Replacement"; }));
    CHECK(ListCell(list, 0) != oldCell);
    unsigned events = 0;
    list->Bind(wxEVT_LIST_ITEM_SELECTED, [&](wxListEvent&) { ++events; });
    SelectRetiredListPeer(oldProvider);
    wxTheApp->ProcessPendingEvents();
    CHECK(events == 0);
    CHECK(list->GetSelectedItemCount() == 0);

    const auto current = ListSelection(ListReportRow(list, 0));
    REQUIRE(current);
    wxWeakRef<wxListCtrl> lifetime(list);
    list->Bind(wxEVT_LIST_ITEM_SELECTED, [list, &events](wxListEvent& event)
    {
        ++events;
        CHECK(event.GetEventObject() == list);
        delete list;
    });
    fixture.list = nullptr;
    current.Select();
    REQUIRE(WaitList([&]() { return !lifetime; }));
    CHECK(events == 1);
    SelectRetiredListPeer(current);
    CHECK(events == 1);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
}

TEST_CASE("WinUIListCtrl::OrdinaryDestructionNotifiesDeleteAllOnce",
          "[winui-listctrl][native-listctrl][lifetime]")
{
    unsigned notifications = 0;
    wxObject* eventObject = nullptr;
    NativeListFixture fixture;
    auto* const list = fixture.list;
    REQUIRE(list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 220) == 0);
    REQUIRE(list->InsertItem(0, "First") == 0);
    REQUIRE(list->InsertItem(1, "Second") == 1);
    fixture.Show();
    const wxWeakRef<wxListCtrl> lifetime(list);
    list->Bind(wxEVT_LIST_DELETE_ALL_ITEMS, [&](wxListEvent& event)
    {
        ++notifications;
        eventObject = event.GetEventObject();
        // Observe the documented destructor notification without reentering
        // deletion or invoking methods on the object being destroyed.
    });
    fixture.list = nullptr;
    delete list;
    CHECK_FALSE(lifetime);
    CHECK(notifications == 1);
    CHECK(eventObject == list);
    wxTheApp->ProcessPendingEvents();
    CHECK(notifications == 1);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
}

#if wxUSE_EDITABLELISTBOX && wxUSE_BMPBUTTON && wxUSE_TEXTCTRL
TEST_CASE("WinUIEditableListBox::NativeButtonsAndListEditThePublicStrings",
          "[winui-listctrl][winui-editable-listbox][native-listctrl]")
{
    std::unique_ptr<wxFrame> frame(new wxFrame(nullptr, wxID_ANY, "native editable list",
        ListPosition(), wxSize(640, 360)));
    auto* const box = new wxEditableListBox(frame.get(), wxID_ANY, "Items");
    const wxArrayString original{"First", "Second", "Third"};
    box->SetStrings(original);
    auto* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(box, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->ShowWithoutActivating();
    frame->Layout();
    auto* const list = box->GetListCtrl();
    REQUIRE(list);
    REQUIRE(WaitList([&]()
    {
        const auto cell = ListCell(list, 1);
        return cell && cell.IsLoaded() && PublicListButton(box->GetNewButton());
    }));
    CHECK(list->GetItemCount() == 4); // EditableListBox owns a terminal empty row.
    CHECK_FALSE(ListHeader(list, 0));
    for ( auto* button : {box->GetUpButton(), box->GetDownButton(), box->GetDelButton(),
                         box->GetNewButton(), box->GetEditButton()} )
    {
        REQUIRE(button);
        const auto peer = PublicListButton(button);
        REQUIRE(peer);
        CHECK(peer.IsLoaded());
        REQUIRE(ListInvoke(peer));
    }
    auto selection = ListSelection(ListReportRow(list, 1));
    REQUIRE(selection);
    selection.Select();
    REQUIRE(WaitList([&]() { return box->GetUpButton()->IsEnabled(); }));
    ListInvoke(PublicListButton(box->GetUpButton())).Invoke();
    wxArrayString strings;
    REQUIRE(WaitList([&]() { box->GetStrings(strings); return strings.size() == 3 && strings[0] == "Second"; }));
    CHECK((strings == wxArrayString{"Second", "First", "Third"}));
    ListInvoke(PublicListButton(box->GetDownButton())).Invoke();
    REQUIRE(WaitList([&]() { box->GetStrings(strings); return strings == original; }));
    ListInvoke(PublicListButton(box->GetDelButton())).Invoke();
    REQUIRE(WaitList([&]() { box->GetStrings(strings); return strings.size() == 2; }));
    CHECK((strings == wxArrayString{"First", "Third"}));
    ListInvoke(PublicListButton(box->GetNewButton())).Invoke();
    REQUIRE(WaitList([&]() { return list->GetEditControl() != nullptr; }));
    auto* const editor = list->GetEditControl();
    REQUIRE(WaitList([&]()
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editor);
        auto* const slot = host ? host->FindSlot(editor) : nullptr;
        // Production wxTextCtrl uses RichEditBox for plain single-line text,
        // too: TextBox is only the explicitly requested legacy test peer.
        const auto text = slot ? slot->GetSemanticTarget().try_as<C::RichEditBox>() : nullptr;
        return text && text.IsLoaded();
    }));
    editor->SetValue("Added through native editor");
    const wxString editorValueBeforeCommit = editor->GetValue();
    CAPTURE(editorValueBeforeCommit);
    REQUIRE(list->EndEditLabel(false));
    const bool committed = WaitList([&]()
    {
        box->GetStrings(strings);
        return strings.size() == 3 && strings[2] == "Added through native editor" &&
               ListCellText(ListCell(list, 2)) == strings[2];
    });
    wxString stringsAfterCommit;
    for ( const auto& value : strings )
        stringsAfterCommit += "[" + value + "] ";
    const auto nativeCellAfterCommit = ListCell(list, 2);
    CAPTURE(strings.size(), stringsAfterCommit, list->GetItemCount(),
            ListCellText(nativeCellAfterCommit));
    INFO("edited native cell present=" << static_cast<bool>(nativeCellAfterCommit)
         << ", loaded=" << (nativeCellAfterCommit && nativeCellAfterCommit.IsLoaded()));
    const auto nativeTreeAfterCommit = ListNativeTree(list);
    const wxSize clientAfterCommit = list->GetClientSize();
    wxRect itemRectAfterCommit;
    const bool hasItemRectAfterCommit = list->GetItemRect(2, itemRectAfterCommit);
    CAPTURE(CountListCells(nativeTreeAfterCommit),
            static_cast<bool>(ListCell(list, 0)),
            static_cast<bool>(ListCell(list, 1)),
            static_cast<bool>(ListCell(list, 3)),
            clientAfterCommit.x, clientAfterCommit.y,
            list->GetTopItem(), list->IsVisible(2),
            hasItemRectAfterCommit, itemRectAfterCommit.x, itemRectAfterCommit.y,
            itemRectAfterCommit.width, itemRectAfterCommit.height,
            static_cast<bool>(nativeTreeAfterCommit));
    REQUIRE(committed);
    CHECK(list->GetItemText(3).empty());
    CHECK(list->GetItemCount() == 4);
}

TEST_CASE("WinUIEditableListBox::PublicCallbacksCanDestroyTheirComposite",
          "[winui-listctrl][winui-editable-listbox][native-listctrl][lifetime]")
{
    enum class Action { SetStringsDelete, SetStringsInsert, Delete, New, CommitNew };
    const auto action = GENERATE(Action::SetStringsDelete, Action::SetStringsInsert,
                                 Action::Delete, Action::New, Action::CommitNew);
    CAPTURE(static_cast<int>(action));
    std::unique_ptr<wxFrame> frame(new wxFrame(nullptr, wxID_ANY, "editable list reentrance",
        ListPosition(), wxSize(640, 360)));
    auto* const box = new wxEditableListBox(frame.get(), wxID_ANY, "Items");
    box->SetStrings(wxArrayString{"First", "Second"});
    auto* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(box, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->ShowWithoutActivating();
    frame->Layout();
    auto* const list = box->GetListCtrl();
    REQUIRE(WaitList([&]()
    {
        const auto cell = ListCell(list, 0);
        const auto button = PublicListButton(box->GetNewButton());
        return cell && cell.IsLoaded() && button && button.IsLoaded();
    }));

    const wxWeakRef<wxEditableListBox> lifetime(box);
    const wxWeakRef<wxListCtrl> listLifetime(list);
    const auto calls = std::make_shared<unsigned>(0);
    const auto destroy = [box, list, calls](wxListEvent& event)
    {
        CHECK(event.GetEventObject() == list);
        ++*calls;
        delete box;
        // Deliberately consume this event: the composite's own handler must
        // not be dispatched through an already destroyed object.
    };

    if ( action == Action::SetStringsDelete )
    {
        list->Bind(wxEVT_LIST_DELETE_ALL_ITEMS, destroy);
        box->SetStrings(wxArrayString{"Replacement"});
    }
    else if ( action == Action::SetStringsInsert )
    {
        list->Bind(wxEVT_LIST_INSERT_ITEM, destroy);
        box->SetStrings(wxArrayString{"Replacement", "Never append this"});
    }
    else if ( action == Action::Delete )
    {
        list->Bind(wxEVT_LIST_DELETE_ITEM, destroy);
        const auto provider = ListInvoke(PublicListButton(box->GetDelButton()));
        REQUIRE(provider);
        provider.Invoke();
    }
    else if ( action == Action::New )
    {
        list->Bind(wxEVT_LIST_ITEM_SELECTED, destroy);
        const auto provider = ListInvoke(PublicListButton(box->GetNewButton()));
        REQUIRE(provider);
        provider.Invoke();
    }
    else
    {
        const auto provider = ListInvoke(PublicListButton(box->GetNewButton()));
        REQUIRE(provider);
        provider.Invoke();
        REQUIRE(WaitList([&]() { return list->GetEditControl() != nullptr; }));
        list->GetEditControl()->SetValue("Accepted before destroying owner");
        list->Bind(wxEVT_LIST_INSERT_ITEM, destroy);
        // The real editor completion enters the composite's EndLabelEdit
        // handler, which appends its terminal empty row and re-enters here.
        list->EndEditLabel(false);
    }
    REQUIRE(WaitList([&]() { return !lifetime && !listLifetime; }));
    CHECK(*calls == 1);
    CHECK_FALSE(frame->IsBeingDeleted());
    wxTheApp->ProcessPendingEvents();
    CHECK(*calls == 1);
}
#endif // wxUSE_EDITABLELISTBOX && wxUSE_BMPBUTTON && wxUSE_TEXTCTRL

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_LISTCTRL
