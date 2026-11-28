/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidataviewlist.cpp
// Purpose:     Native WinUI presentation of the wx list and tree model adapters
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
#include "wx/dataview.h"
#ifdef wxHAS_WINUI_DATAVIEWCTRL

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/sizer.h"
#include "wx/treelist.h"
#include "wx/winui/private/tlwhost.h"
#include "waitfor.h"
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <memory>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = winrt::Microsoft::UI::Xaml::Controls;

wxPoint NativeDataViewListPosition()
{
    // CTest's private desktop provides a real viewport without displaying
    // these rendering tests on the user's input desktop. Outside it retain
    // the existing non-activating, off-screen fallback.
    wchar_t desktopName[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktopName, sizeof(desktopName), &needed) &&
         wxString(desktopName).StartsWith("wxWinUITest-") )
        return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

template <typename T> T FindListCell(const X::DependencyObject& element)
{
    if ( !element ) return nullptr;
    if ( auto found = element.try_as<T>() ) return found;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(element);
    for ( int n = 0; n < count; ++n )
    {
        if ( auto found = FindListCell<T>(
                X::Media::VisualTreeHelper::GetChild(element, n)) )
            return found;
    }
    return nullptr;
}

C::TextBlock FindListText(const X::DependencyObject& element,
                           const winrt::hstring& text)
{
    if ( !element ) return nullptr;
    if ( auto block = element.try_as<C::TextBlock>() )
    {
        if ( block.Text() == text ) return block;
    }
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(element);
    for ( int n = 0; n < count; ++n )
    {
        if ( auto block = FindListText(
                X::Media::VisualTreeHelper::GetChild(element, n), text) )
            return block;
    }
    return nullptr;
}

C::ListView GetListPeer(wxDataViewCtrl* view)
{
    auto host = wxWinUITopLevelHost::FindSlotOwner(view);
    if ( !host ) return nullptr;
    auto slot = host->FindSlot(view);
    return slot ? slot->GetSemanticTarget().try_as<C::ListView>() : nullptr;
}
}

TEST_CASE("WinUI DataViewListCtrl exposes real toggle text and progress cells",
          "[winui][winui-dataview-list][native-cells]")
{
    std::unique_ptr<wxFrame> frame(new wxFrame(nullptr, wxID_ANY,
        "Native data view list cells", NativeDataViewListPosition(), wxSize(660, 280)));
    auto view = new wxDataViewListCtrl(frame.get(), wxID_ANY);
    REQUIRE(view->AppendToggleColumn("Enabled", wxDATAVIEW_CELL_ACTIVATABLE, 100));
    REQUIRE(view->AppendTextColumn("Name", wxDATAVIEW_CELL_INERT, 180));
    REQUIRE(view->AppendProgressColumn("Progress", wxDATAVIEW_CELL_INERT, 140));
    wxVector<wxVariant> values;
    values.push_back(false);
    values.push_back(wxString("Native row"));
    values.push_back(40L);
    view->AppendItem(values);
    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(view, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->ShowWithoutActivating();
    frame->Layout();

    C::ListView list{nullptr};
    C::ListViewItem row{nullptr};
    REQUIRE(WaitFor("native DataViewListCtrl row", [&]
    {
        list = GetListPeer(view);
        if ( !list ) return false;
        row = list.ContainerFromIndex(0).try_as<C::ListViewItem>();
        return row && row.IsLoaded() && row.ActualHeight() > 0;
    }));
    REQUIRE(list.ItemsPanelRoot().try_as<C::ItemsStackPanel>());
    CHECK(list.Items().Size() == 1);
    C::CheckBox check{nullptr};
    C::TextBlock text{nullptr};
    C::ProgressBar progress{nullptr};
    const bool cellsReady = WaitFor("realized native data view list cells", [&]
    {
        check = FindListCell<C::CheckBox>(row);
        text = FindListText(row, L"Native row");
        progress = FindListCell<C::ProgressBar>(row);
        return check && text && progress && check.IsLoaded() &&
            check.ActualWidth() > 0 && text.ActualWidth() > 0;
    });
    CAPTURE(list.ActualWidth(), list.ActualHeight(), list.IsLoaded(),
            row.ActualWidth(), row.ActualHeight(), row.IsLoaded());
    auto host = wxWinUITopLevelHost::FindSlotOwner(view);
    REQUIRE(host);
    auto slot = host->FindSlot(view);
    REQUIRE(slot);
    auto root = slot->GetContent().as<X::FrameworkElement>();
    CAPTURE(root.ActualWidth(), root.ActualHeight(), root.IsLoaded());
    wxString visualTypes;
    std::vector<X::DependencyObject> nodes{row};
    for ( size_t n = 0; n < nodes.size() && n < 200; ++n )
    {
        visualTypes += wxString(winrt::get_class_name(nodes[n]).c_str()) + " ";
        if ( auto element = nodes[n].try_as<X::FrameworkElement>() )
            visualTypes += wxString::Format("%.0fx%.0f loaded=%d; ",
                element.ActualWidth(), element.ActualHeight(), element.IsLoaded());
        const int count = X::Media::VisualTreeHelper::GetChildrenCount(nodes[n]);
        for ( int child = 0; child < count; ++child )
            nodes.push_back(X::Media::VisualTreeHelper::GetChild(nodes[n], child));
    }
    INFO(visualTypes.ToStdString());
    REQUIRE(cellsReady);
    REQUIRE(check);
    REQUIRE(text);
    REQUIRE(progress);
    CHECK(text.Text() == L"Native row");
    CHECK(progress.Value() == Approx(40));
    CHECK_FALSE(check.IsChecked().Value());

    unsigned changed = 0;
    view->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, [&](wxDataViewEvent& event)
    {
        CHECK(event.GetItem() == view->RowToItem(0));
        CHECK(event.GetColumn() == 0);
        ++changed;
    });
    winrt::Microsoft::UI::Xaml::Automation::Peers::CheckBoxAutomationPeer peer(check);
    peer.Toggle();
    CHECK(changed == 1);
    CHECK(view->GetToggleValue(0, 0));
}

TEST_CASE("WinUI DataViewListCtrl column deletion preserves model schema",
          "[winui][winui-dataview-list][column]")
{
    wxDataViewListCtrl view(wxTheApp->GetTopWindow(), wxID_ANY);
    auto first = view.AppendTextColumn("First");
    auto middle = view.AppendTextColumn("Middle");
    auto last = view.AppendTextColumn("Last");
    REQUIRE(first);
    REQUIRE(middle);
    REQUIRE(last);
    wxVector<wxVariant> values;
    values.push_back(wxString("one"));
    values.push_back(wxString("two"));
    values.push_back(wxString("three"));
    view.AppendItem(values);
    REQUIRE(view.DeleteColumn(middle));
    CHECK(view.GetColumnCount() == 2);
    CHECK(view.GetColumn(0) == first);
    CHECK(view.GetColumn(1) == last);
    CHECK(last->GetModelColumn() == 1);
    CHECK(view.GetTextValue(0, 0) == "one");
    CHECK(view.GetTextValue(0, 1) == "three");
    REQUIRE(view.AppendTextColumn("Replacement"));
    view.SetTextValue("four", 0, 2);
    CHECK(view.GetTextValue(0, 1) == "three");
    CHECK(view.GetTextValue(0, 2) == "four");
    REQUIRE(view.ClearColumns());
    CHECK(view.GetColumnCount() == 0);
    REQUIRE(view.AppendTextColumn("Fresh"));
    CHECK(view.GetTextValue(0, 0).empty());
}

#if wxUSE_TREELISTCTRL
TEST_CASE("WinUI TreeList checkbox cells use native UIA and preserve state cycles",
          "[winui][winui-dataview-list][treelistctrl][native-cells]")
{
    const bool allowThird = GENERATE(false, true);
    CAPTURE(allowThird);
    std::unique_ptr<wxFrame> frame(new wxFrame(nullptr, wxID_ANY,
        "Native tree list checkbox", NativeDataViewListPosition(), wxSize(660, 280)));
    auto tree = new wxTreeListCtrl(frame.get(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxTL_CHECKBOX | (allowThird ? wxTL_USER_3STATE : 0));
    REQUIRE(tree->AppendColumn("Name", 240) == 0);
    const auto item = tree->AppendItem(tree->GetRootItem(), "Native tree row");
    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tree, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->ShowWithoutActivating();
    frame->Layout();
    unsigned events = 0;
    tree->Bind(wxEVT_TREELIST_ITEM_CHECKED, [&](wxTreeListEvent& event)
    {
        CHECK(event.GetItem() == item);
        ++events;
    });

    const auto loadCheck = [&]() -> C::CheckBox
    {
        C::CheckBox check{nullptr};
        REQUIRE(WaitFor("native TreeList checkbox cell", [&]
        {
            auto list = GetListPeer(tree->GetDataView());
            if ( !list ) return false;
            auto row = list.ContainerFromIndex(0).try_as<C::ListViewItem>();
            if ( !row ) return false;
            check = FindListCell<C::CheckBox>(row);
            auto text = FindListText(row, L"Native tree row");
            return check && text && check.IsLoaded() &&
                   check.ActualWidth() > 0 && text.ActualWidth() > 0;
        }));
        return check;
    };

    auto check = loadCheck();
    CHECK_FALSE(check.IsChecked().Value());
    winrt::Microsoft::UI::Xaml::Automation::Peers::CheckBoxAutomationPeer(check).Toggle();
    CHECK(tree->GetCheckedState(item) == wxCHK_CHECKED);
    CHECK(events == 1);
    check = loadCheck();
    REQUIRE(check.IsChecked());
    CHECK(check.IsChecked().Value());
    winrt::Microsoft::UI::Xaml::Automation::Peers::CheckBoxAutomationPeer(check).Toggle();
    CHECK(tree->GetCheckedState(item) ==
          (allowThird ? wxCHK_UNDETERMINED : wxCHK_UNCHECKED));
    CHECK(events == 2);
    check = loadCheck();
    if ( allowThird )
    {
        CHECK_FALSE(check.IsChecked());
        winrt::Microsoft::UI::Xaml::Automation::Peers::CheckBoxAutomationPeer(check).Toggle();
        CHECK(tree->GetCheckedState(item) == wxCHK_UNCHECKED);
        CHECK(events == 3);
    }
    else
    {
        REQUIRE(check.IsChecked());
        CHECK_FALSE(check.IsChecked().Value());
    }

    // Programmatic changes may display the indeterminate state even when the
    // user is restricted to a two-state cycle, and must not emit user events.
    const unsigned previousEvents = events;
    tree->CheckItem(item, wxCHK_UNDETERMINED);
    CHECK(events == previousEvents);
    check = loadCheck();
    CHECK_FALSE(check.IsChecked());
}

TEST_CASE("WinUI TreeList primary promotion preserves values and checkbox state",
          "[winui][winui-dataview-list][treelistctrl][column]")
{
    wxTreeListCtrl tree(wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE | wxTL_CHECKBOX);
    REQUIRE(tree.AppendColumn("Name") == 0);
    REQUIRE(tree.AppendColumn("Value") == 1);
    const auto item = tree.AppendItem(tree.GetRootItem(), "name");
    tree.SetItemText(item, 1, "promoted value");
    tree.CheckItem(item, wxCHK_CHECKED);
    auto view = tree.GetDataView();
    REQUIRE(view);
    REQUIRE(GetListPeer(view));
    view->GetColumn(1)->SetSortOrder(false);
    REQUIRE(tree.DeleteColumn(0));
    CHECK(tree.GetColumnCount() == 1);
    CHECK(tree.GetItemText(item) == "promoted value");
    CHECK(tree.GetCheckedState(item) == wxCHK_CHECKED);
    auto promoted = view->GetColumn(0);
    REQUIRE(promoted);
    CHECK(promoted->GetModelColumn() == 0);
    CHECK(promoted->IsSortKey());
    CHECK_FALSE(promoted->IsSortOrderAscending());
    CHECK(wxDynamicCast(promoted->GetRenderer(), wxDataViewCheckIconTextRenderer));
    REQUIRE(tree.AppendColumn("New") == 1);
    tree.SetItemText(item, 1, "new value");
    CHECK(tree.GetItemText(item) == "promoted value");
    CHECK(tree.GetItemText(item, 1) == "new value");
}
#endif // wxUSE_TREELISTCTRL

#endif // wxHAS_WINUI_DATAVIEWCTRL
#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
