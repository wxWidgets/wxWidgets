/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidataviewcolumns.cpp
// Purpose:     Native DataView header resize and retained UIA contracts
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
#include "wx/dataview.h"
#ifdef wxHAS_WINUI_DATAVIEWCTRL
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/winui/private/tlwhost.h"
#include "waitfor.h"
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include "../../src/winui/dataviewheader.h"
#include <memory>
#include <limits>
#include <array>
#include <cmath>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;

template <typename T> T FindHeaderPart(const X::DependencyObject& root)
{
    if ( !root ) return nullptr;
    if ( auto result = root.try_as<T>() ) return result;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( auto result = FindHeaderPart<T>(X::Media::VisualTreeHelper::GetChild(root, n)) )
            return result;
    return nullptr;
}

struct HeaderFixture
{
    HeaderFixture(bool rtl, bool reorderable = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t name[256]{};
        DWORD needed;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
                UOI_NAME, name, sizeof(name), &needed) &&
             wxString(name).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
        frame.reset(new wxFrame(nullptr, wxID_ANY, "Native DataView headers", position, wxSize(640, 300)));
        auto candidate = std::make_unique<wxDataViewListCtrl>();
        REQUIRE(candidate->Create(frame.get(), wxID_ANY));
        view = candidate.release();
        if ( rtl ) view->SetLayoutDirection(wxLayout_RightToLeft);
        column = view->AppendTextColumn("Name", wxDATAVIEW_CELL_INERT, 180,
            wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE |
                (reorderable ? wxDATAVIEW_COL_REORDERABLE : 0));
        REQUIRE(column);
        REQUIRE(view->AppendTextColumn("Value", wxDATAVIEW_CELL_INERT, 160));
        wxVector<wxVariant> row;
        row.push_back(wxString("Alpha"));
        row.push_back(wxString("Beta"));
        view->AppendItem(row);
        auto sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(view, 1, wxEXPAND);
        frame->SetSizer(sizer);
        frame->ShowWithoutActivating();
        frame->Layout();
        REQUIRE(WaitFor("native DataView header layout", [&]
        {
            auto host = wxWinUITopLevelHost::FindSlotOwner(view);
            auto slot = host ? host->FindSlot(view) : nullptr;
            auto root = slot ? slot->GetContent().try_as<C::Grid>() : nullptr;
            if ( !root || root.Children().Size() < 1 ) return false;
            auto scroll = root.Children().GetAt(0).try_as<C::ScrollViewer>();
            auto canvas = scroll ? scroll.Content().try_as<C::Canvas>() : nullptr;
            if ( !canvas || canvas.Children().Size() < 1 ) return false;
            header = canvas.Children().GetAt(0).try_as<C::Grid>();
            return header && header.IsLoaded() && header.ActualWidth() > 0;
        }));
        peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(header);
        REQUIRE(peer);
        REQUIRE(peer.GetClassName() == L"wxDataViewColumnHeader");
        REQUIRE(peer.GetAutomationControlType() == A::AutomationControlType::HeaderItem);
        provider = peer.GetPattern(A::PatternInterface::Transform).try_as<P::ITransformProvider>();
        REQUIRE(provider);
    }
    std::unique_ptr<wxFrame> frame;
    wxDataViewListCtrl* view;
    wxDataViewColumn* column;
    C::Grid header{nullptr};
    A::AutomationPeer peer{nullptr};
    P::ITransformProvider provider{nullptr};
    C::Grid Root() const
    {
        auto host = wxWinUITopLevelHost::FindSlotOwner(view);
        auto slot = host ? host->FindSlot(view) : nullptr;
        return slot ? slot->GetContent().try_as<C::Grid>() : nullptr;
    }
    A::AutomationPeer GridPeer() const
    {
        return A::FrameworkElementAutomationPeer::CreatePeerForElement(FindHeaderPart<C::ListView>(Root()));
    }
};

A::AutomationPeer HeaderProviderPeer(const A::AutomationPeer& owner, const P::IRawElementProviderSimple& provider)
{
    return owner.as<A::IAutomationPeerProtected>().PeerFromProvider(provider);
}

C::Primitives::Thumb FindMoveThumb(const X::DependencyObject& root)
{
    if ( !root ) return nullptr;
    if ( const auto thumb = root.try_as<C::Primitives::Thumb>() )
        if ( X::Automation::AutomationProperties::GetName(thumb) == L"Reorder column" ) return thumb;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
        if ( auto result = FindMoveThumb(X::Media::VisualTreeHelper::GetChild(root, n)) ) return result;
    return nullptr;
}

class ResizeCountingModel final : public wxDataViewVirtualListModel
{
public:
    ResizeCountingModel() : wxDataViewVirtualListModel(4) {}
    unsigned GetCount() const override
    { ++counts; return wxDataViewVirtualListModel::GetCount(); }
    unsigned GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& children) const override
    { ++enumerations; return wxDataViewVirtualListModel::GetChildren(parent, children); }
    void GetValueByRow(wxVariant& value, unsigned row, unsigned column) const override
    {
        ++reads.at(row * 2 + column);
        value = wxString::Format("resize row %u column %u", row, column);
    }
    bool SetValueByRow(const wxVariant&, unsigned, unsigned) override { return false; }
    bool GetAttr(const wxDataViewItem& item, unsigned column, wxDataViewItemAttr&) const override
    { ++attrs.at(GetRow(item) * 2 + column); return false; }
    mutable std::array<unsigned, 8> reads{}, attrs{};
    mutable unsigned counts = 0, enumerations = 0;
};
}

TEST_CASE("WinUI DataView reorder midpoint rule preserves hidden logical positions",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-reorder][reorder-preview]")
{
    using namespace wxWinUIDataViewHeader;
    struct Example
    {
        std::vector<double> widths;
        unsigned source;
        double left;
        unsigned target;
        double edge;
    };
    const std::vector<Example> examples =
    {
        {{100, 200, 60}, 0, 0,      0, 100},
        {{100, 200, 60}, 0, 149.75, 0, 100},
        {{100, 200, 60}, 0, 150,    1, 300},
        {{100, 200, 60}, 0, 279.75, 1, 300},
        {{100, 200, 60}, 0, 280,    2, 360},
        {{100, 200, 60}, 1, -50.25, 0,   0},
        {{100, 200, 60}, 1, -50,    1, 300},
        {{100, 200, 60}, 1, 230,    2, 360},
        {{100, 0, 200, 60}, 0, 49.75, 0, 100},
        {{100, 0, 200, 60}, 0, 50,    1, 100},
        {{100, 0, 200, 60}, 0, 150,   2, 300},
        {{100, 0, 200, 60}, 0, 280,   3, 360},
        {{100, 0, 200, 60}, 1, 0,     0,   0},
        {{100, 0, 200, 60}, 1, 50,    1, 100},
        {{100, 0, 200, 60}, 1, 200,   2, 300},
        {{100, 0, 200, 60}, 2, 100,   2, 300},
        {{100, 0, 200, 60}, 2, 230,   3, 360}
    };
    for ( const auto& example : examples )
    {
        CAPTURE(example.widths, example.source, example.left);
        DropLocation location;
        REQUIRE(FindDropLocation(example.widths, example.source,
                                 example.left, location));
        CHECK(location.position == example.target);
        CHECK(location.edge == Approx(example.edge));
        CHECK(location.changesOrder == (example.target != example.source));
    }

    DropLocation location{42, 123, true};
    CHECK_FALSE(FindDropLocation({}, 0, 0, location));
    CHECK_FALSE(FindDropLocation({100}, 1, 0, location));
    CHECK_FALSE(FindDropLocation({100, -1}, 0, 0, location));
    CHECK_FALSE(FindDropLocation({100, std::numeric_limits<double>::infinity()}, 0, 0, location));
    CHECK_FALSE(FindDropLocation({100}, 0, std::numeric_limits<double>::quiet_NaN(), location));
    CHECK(location.position == 42);
    CHECK(location.edge == 123);
    CHECK(location.changesOrder);
}

TEST_CASE("WinUI DataView native move gesture previews a real marker before committing",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-reorder][reorder-preview]")
{
    namespace H = wxWinUIDataViewHeader;
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    HeaderFixture fixture(rtl, true);
    fixture.column->SetWidth(fixture.view->FromDIP(100));
    fixture.view->GetColumn(1)->SetWidth(fixture.view->FromDIP(200));
    REQUIRE(fixture.view->AppendTextColumn("Last", wxDATAVIEW_CELL_INERT,
                                          fixture.view->FromDIP(60)));
    // Keep the viewport narrower than the sum: the last real column otherwise
    // fills the remaining width, so its effective width would not be 60 DIP.
    fixture.frame->SetClientSize(fixture.frame->FromDIP(wxSize(340, 360)));
    fixture.frame->Layout();
    const auto root = fixture.Root();
    const auto scroll = root.Children().GetAt(0).as<C::ScrollViewer>();
    const auto canvas = scroll.Content().as<C::Canvas>();
    REQUIRE(WaitFor("unequal native header widths", [&]
        { return std::abs(canvas.ActualWidth() - 360) < 1; }));

    H::Header gesture;
    H::InsertionMarker marker;
    gesture.grid.Width(100);
    canvas.Children().Append(gesture.grid);
    canvas.Children().Append(marker.grid);
    bool current = true, previewAccepted = true;
    std::uint64_t version = 1;
    unsigned commits = 0, reordered = 0;
    H::DropLocation preview;
    const std::vector<double> widths{100, 200, 60};
    const auto binding = gesture.binding;
    binding->current = [&] { return current; };
    binding->movable = [] { return true; };
    binding->beginMove = [] { return true; };
    binding->left = [] { return 0.0; };
    binding->version = [&] { return version; };
    binding->rightToLeft = [rtl] { return rtl; };
    binding->previewMove = [&](double left)
    {
        if ( !H::FindDropLocation(widths, 0, left, preview) ) return false;
        marker.Update(preview.edge, preview.changesOrder);
        return previewAccepted;
    };
    binding->clearMove = [&] { marker.Clear(); };
    binding->moveToLeft = [&](double left)
    {
        H::DropLocation committed;
        REQUIRE(H::FindDropLocation(widths, 0, left, committed));
        CHECK(committed.position == preview.position);
        CHECK(committed.edge == preview.edge);
        CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);
        ++commits;
        fixture.view->ColumnMoved(fixture.column, committed.position);
        return true;
    };
    fixture.view->Bind(wxEVT_DATAVIEW_COLUMN_REORDERED,
                      [&](wxDataViewEvent&) { ++reordered; });
    const auto cleanup = wxMakeGuard([&]
    {
        gesture.Retire();
        std::uint32_t index;
        if ( canvas.Children().IndexOf(gesture.grid, index) ) canvas.Children().RemoveAt(index);
        if ( canvas.Children().IndexOf(marker.grid, index) ) canvas.Children().RemoveAt(index);
    });
    wxUnusedVar(cleanup);

    const auto delta = [rtl](double logical) { return rtl ? -logical : logical; };
    REQUIRE(binding->StartMoveGesture());
    CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);
    REQUIRE(binding->ContinueMoveGesture(delta(149.75)));
    CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);
    CHECK(fixture.view->GetColumnPosition(fixture.column) == 0);
    REQUIRE(binding->ContinueMoveGesture(delta(0.25)));
    CHECK(preview.position == 1);
    CHECK(commits == 0);
    CHECK(reordered == 0);
    REQUIRE(WaitFor("native themed insertion marker is arranged", [&]
    {
        return marker.grid.IsLoaded() &&
               marker.grid.Visibility() == X::Visibility::Visible &&
               marker.grid.ActualWidth() == 2 && marker.grid.ActualHeight() == 32;
    }));
    CHECK_FALSE(marker.grid.IsHitTestVisible());
    CHECK(X::Automation::AutomationProperties::GetAccessibilityView(marker.grid) == A::AccessibilityView::Raw);
    // A thin line in the accent colour, as the drop indicator of a WinUI
    // table: one element, no caps.
    REQUIRE(marker.grid.Children().Size() == 1);
    const auto stem = marker.grid.Children().GetAt(0).as<C::Border>();
    CHECK(stem.ActualWidth() == 2);
    CHECK(stem.ActualHeight() == 32);
    REQUIRE(stem.Background());
    // Compare in the island's coordinate system, not the RTL DataView root's
    // own logical coordinates (which undo that root's visual mirroring).
    const auto markerCentre = [&]
    {
        return marker.grid.TransformToVisual(nullptr).TransformPoint({1.0f, 16.0f});
    };
    const auto centreBefore = markerCentre();
    const auto expectedBefore = canvas.TransformToVisual(nullptr).TransformPoint({300.0f, 16.0f});
    CHECK(centreBefore.X == Approx(expectedBefore.X).margin(0.5));
    CHECK(centreBefore.Y == Approx(expectedBefore.Y).margin(0.5));

    REQUIRE(binding->ContinueMoveGesture(delta(130)));
    REQUIRE(WaitFor("insertion marker tracks the next native boundary", [&]
    {
        const auto actual = markerCentre();
        const auto expected = canvas.TransformToVisual(nullptr).TransformPoint({360.0f, 16.0f});
        return std::abs(actual.X - expected.X) < 0.5;
    }));
    CHECK((rtl ? markerCentre().X < centreBefore.X : markerCentre().X > centreBefore.X));
    CHECK(preview.position == 2);
    CHECK(commits == 0);
    CHECK(reordered == 0);
    CHECK(fixture.view->GetColumnPosition(fixture.column) == 0);
    binding->EndMoveGesture(false);
    CHECK(commits == 1);
    CHECK(reordered == 1);
    CHECK(fixture.view->GetColumnPosition(fixture.column) == 2);
    CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);

    fixture.view->ColumnMoved(fixture.column, 0);
    commits = reordered = 0;
    for ( const int reason : {0, 1, 2} )
    {
        CAPTURE(reason);
        REQUIRE(binding->StartMoveGesture());
        REQUIRE(binding->ContinueMoveGesture(delta(280)));
        CHECK(marker.grid.Visibility() == X::Visibility::Visible);
        if ( reason == 1 ) ++version;
        if ( reason == 2 ) current = false;
        if ( reason != 0 ) CHECK_FALSE(binding->ContinueMoveGesture(delta(1)));
        binding->EndMoveGesture(reason == 0);
        CHECK_FALSE(binding->moving);
        CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);
        CHECK(commits == 0);
        CHECK(reordered == 0);
        CHECK(fixture.view->GetColumnPosition(fixture.column) == 0);
        current = true;
    }
    previewAccepted = false;
    CHECK_FALSE(binding->StartMoveGesture());
    CHECK_FALSE(binding->moving);
    CHECK(marker.grid.Visibility() == X::Visibility::Collapsed);
    CHECK(commits == 0);
}

TEST_CASE("WinUI DataView native column resize retains realized cells without model rereads",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-churn][native-live-resize]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    wxPoint position(-30000, -30000);
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "DataView live resize",
                                         position, wxSize(640, 300));
    auto created = std::make_unique<wxDataViewCtrl>();
    REQUIRE(created->Create(frame.get(), wxID_ANY));
    auto* const view = created.release();
    if ( rtl ) view->SetLayoutDirection(wxLayout_RightToLeft);
    auto* const column = view->AppendTextColumn("Resize", 0, wxDATAVIEW_CELL_INERT, 180);
    REQUIRE(column);
    REQUIRE(view->AppendTextColumn("Neighbour", 1, wxDATAVIEW_CELL_INERT, 150));
    auto* const model = new ResizeCountingModel;
    REQUIRE(view->AssociateModel(model));
    model->DecRef();
    auto* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(view, 1, wxEXPAND);
    frame->SetSizer(sizer);
    frame->Layout();
    frame->ShowWithoutActivating();
    C::ListView list{nullptr};
    C::Grid root{nullptr}, header{nullptr};
    std::array<C::ListViewItem, 4> containers{nullptr, nullptr, nullptr, nullptr};
    std::array<C::Canvas, 4> rows{nullptr, nullptr, nullptr, nullptr};
    REQUIRE(WaitFor("all counted resize cells are really loaded", [&]
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(view);
        auto* const slot = host ? host->FindSlot(view) : nullptr;
        root = slot ? slot->GetContent().try_as<C::Grid>() : nullptr;
        list = slot ? slot->GetSemanticTarget().try_as<C::ListView>() : nullptr;
        if ( !root || !list || !list.IsLoaded() || root.Children().Size() < 1 ) return false;
        const auto scroll = root.Children().GetAt(0).try_as<C::ScrollViewer>();
        const auto canvas = scroll ? scroll.Content().try_as<C::Canvas>() : nullptr;
        header = canvas && canvas.Children().Size() ? canvas.Children().GetAt(0).try_as<C::Grid>() : nullptr;
        if ( !header || !header.IsLoaded() ) return false;
        for ( int row = 0; row < 4; ++row )
        {
            containers[row] = list.ContainerFromIndex(row).try_as<C::ListViewItem>();
            rows[row] = containers[row] ? containers[row].ContentTemplateRoot().try_as<C::Canvas>() : nullptr;
            if ( !rows[row] || rows[row].Children().Size() != 2 ) return false;
            for ( const auto& cell : rows[row].Children() )
                if ( !cell.as<X::FrameworkElement>().IsLoaded() ) return false;
        }
        return true;
    }));
    std::array<X::UIElement, 8> cells{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    for ( unsigned row = 0; row < 4; ++row )
        for ( unsigned col = 0; col < 2; ++col ) cells[row * 2 + col] = rows[row].Children().GetAt(col);
    unsigned unloaded = 0;
    const auto token = rows[0].Unloaded([&](const auto&, const auto&) { ++unloaded; });
    auto cleanup = wxMakeGuard([&] { rows[0].Unloaded(token); });
    wxUnusedVar(cleanup);
    const auto peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(header);
    const auto resize = peer.GetPattern(A::PatternInterface::Transform).as<P::ITransformProvider>();
    const auto initialReads = model->reads, initialAttrs = model->attrs;
    const unsigned counts = model->counts, enumerations = model->enumerations;
    const double scale = view->GetDPIScaleFactor();
    for ( const int width : {220, 260, 140, 180} )
    {
        CAPTURE(width);
        resize.Resize(width, header.ActualHeight() * scale);
        REQUIRE(WaitFor("live native cell width without a second interaction", [&]
        {
            const auto current = list.ContainerFromIndex(0).try_as<C::ListViewItem>();
            const auto canvas = current ? current.ContentTemplateRoot().try_as<C::Canvas>() : nullptr;
            return canvas && canvas.Children().Size() == 2 &&
                std::abs(canvas.Children().GetAt(0).as<X::FrameworkElement>().ActualWidth() - width / scale) < 1 &&
                std::abs(C::Canvas::GetLeft(canvas.Children().GetAt(1).as<X::FrameworkElement>()) - width / scale) < 1;
        }));
        CHECK(column->GetWidth() == width);
        CHECK(model->reads == initialReads);
        CHECK(model->attrs == initialAttrs);
        CHECK(model->counts == counts);
        CHECK(model->enumerations == enumerations);
        CHECK(unloaded == 0);
        for ( unsigned row = 0; row < 4; ++row )
        {
            CHECK(list.ContainerFromIndex(static_cast<int>(row)) == containers[row]);
            CHECK(containers[row].ContentTemplateRoot() == rows[row]);
            REQUIRE(rows[row].Children().Size() == 2);
            for ( unsigned col = 0; col < 2; ++col )
                CHECK(rows[row].Children().GetAt(col) == cells[row * 2 + col]);
        }
    }
}

TEST_CASE("WinUI DataView native header move preserves logical columns and retained providers",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-reorder]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    HeaderFixture fixture(rtl, true);
    const auto middle = fixture.view->GetColumn(1);
    const auto last = fixture.view->AppendTextColumn("Last", wxDATAVIEW_CELL_INERT, 140);
    REQUIRE(last);
    const auto moveThumb = FindMoveThumb(fixture.header);
    REQUIRE(moveThumb);
    CHECK(moveThumb.IsLoaded());
    CHECK(fixture.provider.CanMove());
    auto gridPeer = fixture.GridPeer();
    auto grid = gridPeer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
    auto table = gridPeer.GetPattern(A::PatternInterface::Table).as<P::ITableProvider>();
    auto cellPeer = HeaderProviderPeer(gridPeer, grid.GetItem(0, 0));
    auto cell = cellPeer.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    auto headers = table.GetColumnHeaders();
    REQUIRE(headers.size() == 3);
    auto lastHeader = HeaderProviderPeer(gridPeer, headers[2]);
    REQUIRE(WaitFor("reorder target geometry", [&] { return lastHeader.GetBoundingRectangle().Width > 0; }));
    const auto target = lastHeader.GetBoundingRectangle();
    const auto original = fixture.peer.GetBoundingRectangle();
    CHECK((rtl ? original.X > target.X : original.X < target.X));
    unsigned moved = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_COLUMN_REORDERED, [&](wxDataViewEvent& event)
    {
        ++moved;
        CHECK(event.GetDataViewColumn() == fixture.column);
        CHECK(event.GetColumn() == fixture.view->GetColumnPosition(fixture.column));
        CHECK(fixture.view->GetColumn(0) == fixture.column);
    });
    fixture.provider.Move(rtl ? target.X : target.X + target.Width - original.Width, target.Y);
    CHECK(moved == 1);
    CHECK(fixture.view->GetColumnPosition(fixture.column) == 2);
    CHECK(fixture.view->GetColumnAt(0) == middle);
    CHECK(fixture.view->GetColumnAt(1) == last);
    CHECK(fixture.view->GetColumnAt(2) == fixture.column);
    CHECK(fixture.view->GetColumn(0) == fixture.column);
    CHECK(fixture.view->GetColumn(1) == middle);
    CHECK(fixture.view->GetColumn(2) == last);
    CHECK(cell.Column() == 2);
    CHECK(cellPeer.GetName() == L"Alpha");
    CHECK(FindMoveThumb(fixture.header) == moveThumb);
    CHECK(A::FrameworkElementAutomationPeer::CreatePeerForElement(fixture.header) == fixture.peer);
    headers = table.GetColumnHeaders();
    CHECK(HeaderProviderPeer(gridPeer, headers[2]) == fixture.peer);
    CHECK(HeaderProviderPeer(gridPeer, headers[0]).GetName() == L"Value");
    fixture.view->SetTextValue("changed logical zero", 0, 0);
    CHECK(cellPeer.GetName() == L"changed logical zero");
    // Applying the current position is not a second user reorder event.
    fixture.view->ColumnMoved(fixture.column, 2);
    CHECK(moved == 1);
    REQUIRE(fixture.view->DeleteColumn(middle));
    CHECK(fixture.view->GetColumn(0) == fixture.column);
    CHECK(fixture.view->GetColumn(1) == last);
    CHECK(fixture.view->GetColumnAt(0) == last);
    CHECK(fixture.view->GetColumnAt(1) == fixture.column);
    CHECK(cell.Column() == 1);
    CHECK(cellPeer.GetName() == L"changed logical zero");
}

TEST_CASE("WinUI DataView native header move honours hidden flags and retirement",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-reorder]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    HeaderFixture fixture(rtl, true);
    const auto hidden = fixture.view->GetColumn(1);
    const auto last = fixture.view->AppendTextColumn("Last", wxDATAVIEW_CELL_INERT, 140);
    REQUIRE(last);
    hidden->SetHidden(true);
    auto gridPeer = fixture.GridPeer();
    auto table = gridPeer.GetPattern(A::PatternInterface::Table).as<P::ITableProvider>();
    auto headers = table.GetColumnHeaders();
    REQUIRE(headers.size() == 2);
    auto targetPeer = HeaderProviderPeer(gridPeer, headers[1]);
    REQUIRE(WaitFor("hidden reorder target geometry", [&] { return targetPeer.GetBoundingRectangle().Width > 0; }));
    const auto target = targetPeer.GetBoundingRectangle();
    const auto original = fixture.peer.GetBoundingRectangle();
    CHECK((rtl ? original.X > target.X : original.X < target.X));
    const double x = rtl ? target.X : target.X + target.Width - original.Width;
    fixture.column->SetReorderable(false);
    CHECK_FALSE(fixture.provider.CanMove());
    CHECK_THROWS_AS(fixture.provider.Move(x, target.Y), winrt::hresult_error);
    fixture.column->SetReorderable(true);
    fixture.view->Enable(false);
    CHECK_FALSE(fixture.provider.CanMove());
    CHECK_THROWS_AS(fixture.provider.Move(x, target.Y), winrt::hresult_error);
    fixture.view->Enable(true);
    CHECK_THROWS_AS(fixture.provider.Move(std::numeric_limits<double>::quiet_NaN(), target.Y), winrt::hresult_error);
    CHECK_THROWS_AS(fixture.provider.Move(x, target.Y + 100), winrt::hresult_error);
    fixture.provider.Move(x, target.Y);
    CHECK(fixture.view->GetColumnPosition(fixture.column) == 2);
    CHECK(fixture.view->GetColumnPosition(hidden) == 0);
    CHECK(fixture.view->GetColumnAt(1) == last);
    CHECK(fixture.view->GetColumn(1) == hidden);
    hidden->SetHidden(false);
    CHECK(table.GetColumnHeaders().size() == 3);
    fixture.column->SetHidden(true);
    CHECK_FALSE(fixture.provider.CanMove());
    CHECK_THROWS_AS(fixture.provider.Move(x, target.Y), winrt::hresult_error);
    fixture.column->SetHidden(false);
    CHECK_FALSE(fixture.provider.CanMove());
    fixture.frame.reset();
    CHECK_FALSE(fixture.provider.CanMove());
}

TEST_CASE("WinUI DataView native reorder respects editor veto and reentrant destruction",
          "[winui][winui-dataview][winui-dataview-columns][winui-dataview-reorder]")
{
    const bool destroy = GENERATE(false, true);
    HeaderFixture fixture(false, true);
    const auto second = fixture.view->GetColumn(1);
    auto tablePeer = fixture.GridPeer();
    auto table = tablePeer.GetPattern(A::PatternInterface::Table).as<P::ITableProvider>();
    auto headers = table.GetColumnHeaders();
    auto destination = HeaderProviderPeer(tablePeer, headers[1]);
    REQUIRE(WaitFor("editor reorder target geometry", [&] { return destination.GetBoundingRectangle().Width > 0; }));
    const auto target = destination.GetBoundingRectangle();
    const auto source = fixture.peer.GetBoundingRectangle();
    const double x = target.X + target.Width - source.Width;
    fixture.column->GetRenderer()->SetMode(wxDATAVIEW_CELL_EDITABLE);
    fixture.view->EditItem(fixture.view->RowToItem(0), fixture.column);
    C::TextBox editor{nullptr};
    REQUIRE(WaitFor("native reorder pending editor", [&]
        { editor = FindHeaderPart<C::TextBox>(fixture.Root()); return editor != nullptr; }));
    editor.Text(L"candidate");
    unsigned done = 0, moved = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_COLUMN_REORDERED, [&](wxDataViewEvent&) { ++moved; });
    fixture.view->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [&](wxDataViewEvent& event)
    {
        ++done;
        if ( destroy )
        {
            event.Skip(false);
            fixture.frame.reset();
        }
        else event.Veto();
    });
    CHECK_THROWS_AS(fixture.provider.Move(x, target.Y), winrt::hresult_error);
    CHECK(done == 1);
    CHECK(moved == 0);
    if ( !destroy )
    {
        CHECK(fixture.view->GetColumnAt(0) == fixture.column);
        CHECK(fixture.view->GetColumnAt(1) == second);
        CHECK(fixture.view->GetTextValue(0, 0) == "Alpha");
    }
    else CHECK_FALSE(fixture.provider.CanMove());
}

TEST_CASE("WinUI DataView retained header resize shares its native width authority with UIA",
          "[winui][winui-dataview][winui-dataview-columns]")
{
    const bool rtl = GENERATE(false, true);
    HeaderFixture fixture(rtl);
    auto& provider = fixture.provider;
    auto column = fixture.column;
    auto header = fixture.header;
    const double scale = fixture.view->GetDPIScaleFactor();
    CHECK(fixture.peer.GetAutomationControlType() == A::AutomationControlType::HeaderItem);
    CHECK(fixture.peer.GetName() == L"Name");
    CHECK(provider.CanResize());
    CHECK_FALSE(provider.CanMove());
    CHECK_FALSE(provider.CanRotate());
    const auto thumb = FindHeaderPart<C::Primitives::Thumb>(header);
    REQUIRE(thumb);
    column->SetMinWidth(100);
    provider.Resize(240, header.ActualHeight() * scale);
    CHECK(column->GetWidth() == 240);
    provider.Resize(280, header.ActualHeight() * scale);
    CHECK(column->GetWidth() == 280);
    CHECK(FindHeaderPart<C::Primitives::Thumb>(header) == thumb);
    CHECK(header.IsLoaded());
    CHECK(header.Width() == Approx(280 / scale));
    provider.Resize(20, header.ActualHeight() * scale);
    CHECK(column->GetWidth() == 100);
    CHECK_THROWS_AS(provider.Resize(-1, header.ActualHeight() * scale), winrt::hresult_error);
    CHECK(column->GetWidth() == 100);

    unsigned clicks = 0, sorts = 0;
    fixture.view->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK,
        [&](wxDataViewEvent& event) { ++clicks; CHECK(event.GetDataViewColumn() == column); });
    fixture.view->Bind(wxEVT_DATAVIEW_COLUMN_SORTED,
        [&](wxDataViewEvent& event) { ++sorts; CHECK(event.GetDataViewColumn() == column); });
    auto button = FindHeaderPart<C::Button>(header);
    REQUIRE(button);
    A::ButtonAutomationPeer(button).Invoke();
    CHECK(clicks == 1);
    CHECK(sorts == 1);
    CHECK(column->IsSortKey());
    CHECK(provider.CanResize());
    CHECK(FindHeaderPart<C::Primitives::Thumb>(header) == thumb);

    column->SetResizeable(false);
    CHECK_FALSE(provider.CanResize());
    CHECK_THROWS_AS(provider.Resize(240, header.ActualHeight() * scale), winrt::hresult_error);
    column->SetResizeable(true);
    CHECK(provider.CanResize());
    fixture.view->Enable(false);
    CHECK_FALSE(provider.CanResize());
    fixture.view->Enable(true);
    CHECK(provider.CanResize());
    column->SetHidden(true);
    CHECK_FALSE(provider.CanResize());
    CHECK_THROWS_AS(provider.Resize(240, 36), winrt::hresult_error);
    column->SetHidden(false);
    // A provider from a retired header must never attach to its replacement.
    CHECK_FALSE(provider.CanResize());
    REQUIRE(fixture.view->DeleteColumn(column));
    CHECK_FALSE(provider.CanResize());
}

TEST_CASE("WinUI DataView header providers fail closed after destruction",
          "[winui][winui-dataview][winui-dataview-columns]")
{
    HeaderFixture fixture(false);
    auto provider = fixture.provider;
    fixture.frame.reset();
    CHECK_FALSE(provider.CanResize());
    CHECK_THROWS_AS(provider.Resize(240, 36), winrt::hresult_error);
}

#endif // wxHAS_WINUI_DATAVIEWCTRL
#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_DATAVIEWCTRL
