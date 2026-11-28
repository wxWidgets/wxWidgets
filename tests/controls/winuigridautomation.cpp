/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuigridautomation.cpp
// Purpose:     Native wxGrid Grid/Table providers, virtualization and lifetime
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_GRID

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <UIAutomation.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;

template<typename Predicate> bool WaitGridAutomation(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 2000 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

C::Grid GridAutomationRoot(wxWindow* window)
{
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(window);
    auto* const slot = host ? host->FindSlot(window) : nullptr;
    return slot ? slot->GetContent().try_as<C::Grid>() : nullptr;
}

C::Control GridAutomationCell(wxWindow* window, int row, int column)
{
    const auto root = GridAutomationRoot(window);
    if ( !root || root.Children().Size() != 1 ) return nullptr;
    const auto canvas = root.Children().GetAt(0).try_as<C::Canvas>();
    if ( !canvas ) return nullptr;
    const auto id = winrt::hstring(wxString::Format("wxGrid.Cell.%d.%d", row, column).wc_str());
    for ( const auto& child : canvas.Children() )
        if ( X::Automation::AutomationProperties::GetAutomationId(child) == id ) return child.try_as<C::Control>();
    return nullptr;
}

A::AutomationPeer GridItemPeer(const A::AutomationPeer& grid, const P::IRawElementProviderSimple& provider)
{
    return grid.as<A::IAutomationPeerProtected>().PeerFromProvider(provider);
}

// The native field of a text editor: wxTextCtrl uses a RichEditBox, and a
// TextBox only for a password.
C::Control GridEditorField(const X::DependencyObject& node)
{
    if ( const auto rich = node.try_as<C::RichEditBox>() ) return rich;
    if ( const auto text = node.try_as<C::TextBox>() ) return text;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(node); ++n )
        if ( const auto field = GridEditorField(X::Media::VisualTreeHelper::GetChild(node, n)) ) return field;
    return nullptr;
}

winrt::hstring GridEditorFieldText(const C::Control& field)
{
    if ( const auto text = field.try_as<C::TextBox>() ) return text.Text();
    winrt::hstring value;
    field.as<C::RichEditBox>().Document().GetText(
        winrt::Microsoft::UI::Text::TextGetOptions::None, value);
    // The document always ends with a paragraph mark.
    std::wstring plain(value.c_str());
    while ( !plain.empty() && (plain.back() == L'\r' ||
                               plain.back() == L'\n') )
        plain.pop_back();
    return winrt::hstring(plain);
}

template<typename Action> void CheckGridUnavailable(Action action)
{
    bool unavailable = false;
    try { action(); }
    catch ( const winrt::hresult_error& error ) { unavailable = error.code() == UIA_E_ELEMENTNOTAVAILABLE; }
    CHECK(unavailable);
}

struct GridAutomationFixture
{
    explicit GridAutomationFixture(bool rtl = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t desktop[256]{};
        DWORD needed = 0;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()), UOI_NAME,
                desktop, sizeof(desktop), &needed) && wxString(desktop).StartsWith("wxWinUITest-") )
            position = wxPoint(80, 80);
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "wxGrid native automation", position, wxSize(600, 350));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        grid = new wxGrid(frame.get(), wxID_ANY);
        frame->ShowWithoutActivating();
    }
    void Ready()
    {
        REQUIRE(WaitGridAutomation([&]
        {
            root = GridAutomationRoot(grid->GetGridWindow());
            return root && root.IsLoaded() && root.ActualWidth() > 0 && root.ActualHeight() > 0;
        }));
        peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(root);
        REQUIRE(peer);
        CHECK(peer.GetClassName() == L"wxGrid");
        CHECK(peer.GetAutomationControlType() == A::AutomationControlType::DataGrid);
        provider = peer.GetPattern(A::PatternInterface::Grid).as<P::IGridProvider>();
        table = peer.GetPattern(A::PatternInterface::Table).as<P::ITableProvider>();
        REQUIRE(provider); REQUIRE(table);
    }
    std::unique_ptr<wxFrame> frame;
    wxGrid* grid = nullptr;
    C::Grid root{nullptr};
    A::AutomationPeer peer{nullptr};
    P::IGridProvider provider{nullptr};
    P::ITableProvider table{nullptr};
};

class AutomationVirtualTable final : public wxGridTableBase
{
public:
    int GetNumberRows() override { return 100000; }
    int GetNumberCols() override { return 2000; }
    wxString GetValue(int row, int col) override
        { ++reads; return wxString::Format("%d:%d", row, col); }
    void SetValue(int, int, const wxString&) override { ++writes; }
    unsigned reads = 0, writes = 0;
};

class AutomationCallbackTable final : public wxGridStringTable
{
public:
    AutomationCallbackTable() : wxGridStringTable(8, 6) { SetValue(1, 1, "before callback"); }
    wxString GetValue(int row, int col) override
    {
        const auto value = wxGridStringTable::GetValue(row, col);
        const auto callback = std::move(onRead);
        onRead = nullptr;
        if ( callback ) callback();
        return value;
    }
    wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind) override
    {
        wxGridCellAttrPtr attr(wxGridStringTable::GetAttr(row, col, kind));
        const auto callback = std::move(onAttr);
        onAttr = nullptr;
        if ( callback ) callback();
        return attr.release();
    }
    wxString GetTypeName(int row, int col) override
    {
        const wxString type = wxGridStringTable::GetTypeName(row, col);
        const auto callback = std::move(onType);
        onType = nullptr;
        if ( callback ) callback();
        return type;
    }
    void SetValue(int row, int col, const wxString& value) override
    {
        wxGridStringTable::SetValue(row, col, value);
        const auto callback = std::move(onWrite);
        onWrite = nullptr;
        if ( callback ) callback();
    }
    std::function<void()> onRead, onAttr, onType, onWrite;
};
}

TEST_CASE("WinUIGrid::UIAGetItemPreservesVirtualCellIdentityWithoutReadingTheTable",
          "[winui-grid][winui-grid-uia]")
{
    AutomationVirtualTable model;
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->SetTable(&model));
    fixture.Ready();
    const unsigned before = model.reads;
    CHECK(fixture.provider.RowCount() == 100000);
    CHECK(fixture.provider.ColumnCount() == 2000);
    const auto nearPeer = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0));
    const auto farPeer = GridItemPeer(fixture.peer, fixture.provider.GetItem(90000, 1500));
    const auto item = farPeer.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    CHECK(item.Row() == 90000); CHECK(item.Column() == 1500);
    CHECK(item.RowSpan() == 1); CHECK(item.ColumnSpan() == 1);
    CHECK(farPeer.IsOffscreen());
    CHECK(farPeer.GetBoundingRectangle().Width == 0);
    CHECK(model.reads == before);
    CHECK(GridItemPeer(fixture.peer, item.ContainingGrid()) == fixture.peer);
    CHECK(GridItemPeer(fixture.peer, fixture.provider.GetItem(90000, 1500)) == farPeer);
    CHECK(farPeer.GetName() == L"90000:1500");
    CHECK(model.reads == before + 1);
#ifndef wxNO_RTTI
    const auto value = farPeer.GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    REQUIRE(value);
    CHECK(model.reads == before + 1); // Pattern discovery does not read values.
    CHECK(value.Value() == L"90000:1500");
    CHECK(model.reads == before + 2);
#else
    CHECK_FALSE(farPeer.GetPattern(A::PatternInterface::Value));
#endif
    CHECK_FALSE(farPeer.GetPattern(A::PatternInterface::SelectionItem));
    CHECK_THROWS_AS(fixture.provider.GetItem(-1, 0), winrt::hresult_error);
    CHECK_THROWS_AS(fixture.provider.GetItem(100000, 0), winrt::hresult_error);
    CHECK_THROWS_AS(fixture.provider.GetItem(0, 2000), winrt::hresult_error);
    farPeer.GetPattern(A::PatternInterface::VirtualizedItem).as<P::IVirtualizedItemProvider>().Realize();
    REQUIRE(WaitGridAutomation([&]
    {
        const auto cell = GridAutomationCell(fixture.grid->GetGridWindow(), 90000, 1500);
        return cell && cell.IsLoaded() && !farPeer.IsOffscreen();
    }));
    const auto visual = GridAutomationCell(fixture.grid->GetGridWindow(), 90000, 1500);
    CHECK(A::FrameworkElementAutomationPeer::CreatePeerForElement(visual) == farPeer);
    CHECK(GridItemPeer(fixture.peer, fixture.provider.GetItem(90000, 1500)) == farPeer);
    CHECK(nearPeer.IsOffscreen());
    CHECK(farPeer.GetBoundingRectangle().Width > 0);
    CHECK(model.reads - before < 1000);
    CHECK(model.writes == 0);
}

TEST_CASE("WinUIGrid::UIATableConnectsMergedAndFrozenCellsToTheirRealHeaders",
          "[winui-grid][winui-grid-uia]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    GridAutomationFixture fixture(rtl);
    REQUIRE(fixture.grid->CreateGrid(40, 12));
    fixture.grid->SetCellValue(2, 2, "merged anchor");
    fixture.grid->SetCellSize(2, 2, 2, 2);
    REQUIRE(fixture.grid->FreezeTo(1, 2));
    fixture.Ready();
    const auto anchor = GridItemPeer(fixture.peer, fixture.provider.GetItem(2, 2));
    CHECK(GridItemPeer(fixture.peer, fixture.provider.GetItem(3, 3)) == anchor);
    const auto item = anchor.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    CHECK(item.Row() == 2); CHECK(item.Column() == 2);
    CHECK(item.RowSpan() == 2); CHECK(item.ColumnSpan() == 2);
    CHECK(anchor.GetName() == L"merged anchor");
    const auto tableItem = anchor.GetPattern(A::PatternInterface::TableItem).as<P::ITableItemProvider>();
    const auto rowHeaders = fixture.table.GetRowHeaders();
    const auto columnHeaders = fixture.table.GetColumnHeaders();
    REQUIRE(rowHeaders.size() == 40); REQUIRE(columnHeaders.size() == 12);
    CHECK(fixture.table.RowOrColumnMajor() == X::Automation::RowOrColumnMajor::RowMajor);
    const auto rows = tableItem.GetRowHeaderItems();
    const auto columns = tableItem.GetColumnHeaderItems();
    REQUIRE(rows.size() == 2); REQUIRE(columns.size() == 2);
    CHECK(GridItemPeer(fixture.peer, rows[0]) == GridItemPeer(fixture.peer, rowHeaders[2]));
    CHECK(GridItemPeer(fixture.peer, columns[1]) == GridItemPeer(fixture.peer, columnHeaders[3]));
    CHECK(GridItemPeer(fixture.peer, rowHeaders[2]).GetName() == L"3");
    CHECK(GridItemPeer(fixture.peer, columnHeaders[3]).GetName() == L"D");

    const auto frozen = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0));
    REQUIRE(WaitGridAutomation([&] { return !frozen.IsOffscreen(); }));
    const auto frozenControl = GridAutomationCell(fixture.grid->GetFrozenCornerGridWindow(), 0, 0);
    REQUIRE(frozenControl);
    CHECK(A::FrameworkElementAutomationPeer::CreatePeerForElement(frozenControl) == frozen);
    const auto before = frozen.GetBoundingRectangle();
    auto distant = GridItemPeer(fixture.peer, fixture.provider.GetItem(35, 10));
    distant.GetPattern(A::PatternInterface::ScrollItem).as<P::IScrollItemProvider>().ScrollIntoView();
    REQUIRE(WaitGridAutomation([&] { return !distant.IsOffscreen(); }));
    CHECK_FALSE(frozen.IsOffscreen());
    const auto after = frozen.GetBoundingRectangle();
    CHECK(after.X == before.X); CHECK(after.Y == before.Y);
    CHECK(GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0)) == frozen);
    CHECK_FALSE(GridItemPeer(fixture.peer, columnHeaders[0]).IsOffscreen());
    CHECK_FALSE(GridItemPeer(fixture.peer, rowHeaders[0]).IsOffscreen());
}

TEST_CASE("WinUIGrid::UIADisplayOrderHiddenCellsAndStructuralRetirementAreExplicit",
          "[winui-grid][winui-grid-uia]")
{
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->CreateGrid(8, 6));
    fixture.grid->SetCellValue(1, 2, "logical one two");
    fixture.Ready();
    const auto old = GridItemPeer(fixture.peer, fixture.provider.GetItem(1, 2));
    const auto oldItem = old.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    fixture.grid->SetRowPos(1, 0);
    fixture.grid->SetColPos(2, 0);
    CheckGridUnavailable([&] { (void)oldItem.Row(); });
    const auto moved = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0));
    CHECK(moved.GetName() == L"logical one two");
    const auto movedItem = moved.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    CHECK(movedItem.Row() == 0); CHECK(movedItem.Column() == 0);
    const auto headers = fixture.table.GetColumnHeaders();
    CHECK(GridItemPeer(fixture.peer, headers[0]).GetName() == L"C");
    fixture.grid->HideRow(1);
    fixture.grid->HideCol(2);
    CHECK(fixture.provider.RowCount() == 8); CHECK(fixture.provider.ColumnCount() == 6);
    CHECK(moved.IsOffscreen());
    CHECK(moved.GetBoundingRectangle().Width == 0);
    CHECK_THROWS_AS(moved.GetPattern(A::PatternInterface::VirtualizedItem)
        .as<P::IVirtualizedItemProvider>().Realize(), winrt::hresult_error);
    fixture.grid->ShowRow(1); fixture.grid->ShowCol(2);
    fixture.grid->Enable(false);
    CHECK_FALSE(moved.IsEnabled());
    CHECK_THROWS_AS(moved.GetPattern(A::PatternInterface::VirtualizedItem)
        .as<P::IVirtualizedItemProvider>().Realize(), winrt::hresult_error);
    fixture.grid->Enable(true);
    REQUIRE(fixture.grid->DeleteRows(1));
    CheckGridUnavailable([&] { (void)movedItem.Column(); });
    CHECK(fixture.provider.RowCount() == 7);
    const auto retainedHeader = GridItemPeer(fixture.peer, headers[0]);
    CheckGridUnavailable([&] { (void)retainedHeader.GetName(); });
    fixture.frame.reset();
    CheckGridUnavailable([&] { (void)fixture.provider.ColumnCount(); });
}

TEST_CASE("WinUIGrid::UIAReadsCannotReturnRetiredModelValues",
          "[winui-grid][winui-grid-uia]")
{
    const int mutation = GENERATE(0, 1, 2);
    const bool attrRead = GENERATE(false, true);
    CAPTURE(mutation, attrRead);
    AutomationCallbackTable model;
    wxGridStringTable replacement(3, 3);
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->SetTable(&model));
    fixture.Ready();
    const auto cell = GridItemPeer(fixture.peer, fixture.provider.GetItem(1, 1));
    const auto item = cell.GetPattern(A::PatternInterface::GridItem).as<P::IGridItemProvider>();
    unsigned callbacks = 0;
    const auto mutate = [&]
    {
        ++callbacks;
        if ( mutation == 0 ) REQUIRE(fixture.grid->DeleteCols(1));
        else if ( mutation == 1 ) REQUIRE(fixture.grid->SetTable(&replacement));
        else { delete fixture.grid; fixture.grid = nullptr; }
    };
    if ( attrRead )
    {
        model.onAttr = mutate;
        CheckGridUnavailable([&] { (void)fixture.provider.GetItem(6, 5); });
    }
    else
    {
        model.onRead = mutate;
        CheckGridUnavailable([&] { (void)cell.GetName(); });
    }
    CHECK(callbacks == 1);
    CheckGridUnavailable([&] { (void)item.Row(); });
    if ( mutation == 0 ) CHECK(fixture.provider.ColumnCount() == 5);
    if ( mutation == 1 ) CHECK(fixture.provider.RowCount() == 3);
    if ( mutation == 2 ) CheckGridUnavailable([&] { (void)fixture.provider.RowCount(); });
}

#ifndef wxNO_RTTI
TEST_CASE("WinUIGrid::UIAValueUsesVisibleStandardEditorsAndPublicCommitEvents",
          "[winui-grid][winui-grid-uia][grid-uia-value]")
{
    const bool rtl = GENERATE(false, true);
    const int kind = GENERATE(0, 1, 2, 3);
    CAPTURE(rtl, kind);
    GridAutomationFixture fixture(rtl);
    REQUIRE(fixture.grid->CreateGrid(100, 4));
    if ( kind == 1 ) fixture.grid->SetCellEditor(88, 2, new wxGridCellNumberEditor);
    if ( kind == 2 ) fixture.grid->SetCellEditor(88, 2, new wxGridCellFloatEditor);
    if ( kind == 3 ) fixture.grid->SetCellEditor(88, 2, new wxGridCellAutoWrapStringEditor);
    fixture.Ready();
    const auto peer = GridItemPeer(fixture.peer, fixture.provider.GetItem(88, 2));
    const auto value = peer.GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    REQUIRE(value);
    CHECK_FALSE(value.IsReadOnly());
    CHECK(peer.IsOffscreen());
    std::vector<wxString> events;
    wxWeakRef<wxWindow> editorWindow;
    C::Control nativeEditor{nullptr};
    // What the slot of the editor held, when no text field was found there.
    wxString editorContent;
    fixture.grid->Bind(wxEVT_GRID_EDITOR_SHOWN, [&](wxGridEvent& event)
        { events.push_back("shown"); event.Skip(); });
    // No REQUIRE in the handlers: they run below the native automation call,
    // which an exception must not cross.
    fixture.grid->Bind(wxEVT_GRID_EDITOR_CREATED, [&](wxGridEditorCreatedEvent& event)
    {
        events.push_back("created");
        event.Skip();
        editorWindow = event.GetWindow();
        if ( !editorWindow )
            return;
        CHECK(editorWindow->GetParent() == fixture.grid->GetGridWindow());
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(editorWindow.get());
        auto* const slot = host ? host->FindSlot(editorWindow.get()) : nullptr;
        if ( !slot )
        {
            editorContent = "no slot";
            return;
        }
        const auto content = slot->GetContent();
        nativeEditor = content ? GridEditorField(content) : nullptr;
        if ( !nativeEditor )
        {
            editorContent = content
                ? wxString(winrt::get_class_name(content).c_str())
                : wxString("no content");
        }
    });
    fixture.grid->Bind(wxEVT_GRID_EDITOR_HIDDEN, [&](wxGridEvent& event)
        { events.push_back("hidden"); event.Skip(); });
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
        { events.push_back("changing"); CHECK(event.GetRow() == 88); CHECK(event.GetCol() == 2); event.Skip(); });
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event)
        { events.push_back("changed"); event.Skip(); });
    const winrt::hstring input = kind == 1 ? L"42" : kind == 2 ? L"1.25" : L"native value";
    value.SetValue(input);
    CHECK(events == std::vector<wxString>{"shown", "created", "hidden", "changing", "changed"});
    REQUIRE(editorWindow);
    INFO("editor content: " << editorContent);
    REQUIRE(nativeEditor);
    CHECK(value.Value() == input);
    CHECK(GridEditorFieldText(nativeEditor) == input);
    CHECK(fixture.grid->GetGridCursorRow() == 88);
    CHECK(fixture.grid->GetGridCursorCol() == 2);
    CHECK_FALSE(fixture.grid->IsCellEditControlEnabled());
    REQUIRE(editorWindow);
    CHECK_FALSE(editorWindow->IsShown());
    REQUIRE(WaitGridAutomation([&] { return !peer.IsOffscreen(); }));
    CHECK(GridItemPeer(fixture.peer, fixture.provider.GetItem(88, 2)) == peer);
}

TEST_CASE("WinUIGrid::UIAValueHonoursVetoReadOnlyAndEditorCompatibility",
          "[winui-grid][winui-grid-uia][grid-uia-value]")
{
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->CreateGrid(4, 3));
    fixture.grid->SetCellValue(0, 0, "original");
    fixture.Ready();
    const auto peer = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0));
    const auto value = peer.GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    REQUIRE(value);
    bool vetoShown = true, vetoChanging = false;
    fixture.grid->Bind(wxEVT_GRID_SELECT_CELL, [&](wxGridEvent& event)
        { if ( event.GetRow() == 1 ) event.Veto(); else event.Skip(); });
    const auto unselectable = GridItemPeer(fixture.peer, fixture.provider.GetItem(1, 0))
        .GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    CHECK_THROWS_AS(unselectable.SetValue(L"selection veto"), winrt::hresult_error);
    CHECK(fixture.grid->GetCellValue(1, 0).empty());
    unsigned changing = 0, changed = 0;
    fixture.grid->Bind(wxEVT_GRID_EDITOR_SHOWN, [&](wxGridEvent& event)
        { if ( vetoShown ) event.Veto(); else event.Skip(); });
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
        { ++changing; if ( vetoChanging ) event.Veto(); else event.Skip(); });
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event)
        { ++changed; event.Skip(); });
    CHECK_THROWS_AS(value.SetValue(L"shown veto"), winrt::hresult_error);
    CHECK(value.Value() == L"original"); CHECK(changing == 0);
    vetoShown = false; vetoChanging = true;
    value.SetValue(L"changing veto");
    CHECK(value.Value() == L"original"); CHECK(changing == 1); CHECK(changed == 0);
    vetoChanging = false;
    value.SetValue(L"accepted");
    CHECK(value.Value() == L"accepted"); CHECK(changing == 2); CHECK(changed == 1);
    fixture.grid->SetReadOnly(0, 0);
    CHECK(value.IsReadOnly());
    CHECK_THROWS_AS(value.SetValue(L"readonly"), winrt::hresult_error);
    fixture.grid->SetReadOnly(0, 0, false);
    fixture.grid->EnableEditing(false);
    CHECK(value.IsReadOnly());
    CHECK_THROWS_AS(value.SetValue(L"disabled editing"), winrt::hresult_error);
    fixture.grid->EnableEditing(true);
    fixture.grid->Enable(false);
    CHECK_THROWS_AS(value.SetValue(L"disabled window"), winrt::hresult_error);
    fixture.grid->Enable(true);
    CHECK(value.Value() == L"accepted"); CHECK(changed == 1);
    fixture.grid->SetCellEditor(0, 1, new wxGridCellNumberEditor);
    const auto number = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 1))
        .GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    REQUIRE(number);
    CHECK_THROWS_AS(number.SetValue(L"not a number"), winrt::hresult_error);
    CHECK(fixture.grid->GetCellValue(0, 1).empty());
    class CustomTextEditor : public wxGridCellTextEditor
    {
    public:
        wxGridCellEditor* Clone() const override { return new CustomTextEditor; }
    };
    fixture.grid->SetCellEditor(0, 0, new CustomTextEditor);
    CHECK_FALSE(peer.GetPattern(A::PatternInterface::Value));
    CHECK_THROWS_AS(value.SetValue(L"custom"), winrt::hresult_error);
    CHECK(fixture.grid->GetCellValue(0, 0) == "accepted");
}

TEST_CASE("WinUIGrid::UIAValueRetainsLogicalCoordinatesAndRejectsRetiredProviders",
          "[winui-grid][winui-grid-uia][grid-uia-value]")
{
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->CreateGrid(8, 6));
    fixture.Ready();
    const auto old = GridItemPeer(fixture.peer, fixture.provider.GetItem(1, 2))
        .GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    fixture.grid->SetRowPos(1, 0); fixture.grid->SetColPos(2, 0);
    CheckGridUnavailable([&] { old.SetValue(L"stale"); });
    const auto moved = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 0))
        .GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    moved.SetValue(L"logical 1,2");
    CHECK(fixture.grid->GetCellValue(1, 2) == "logical 1,2");
    fixture.grid->HideRow(1);
    CHECK(moved.Value() == L"logical 1,2");
    CHECK_THROWS_AS(moved.SetValue(L"hidden"), winrt::hresult_error);
    fixture.grid->ShowRow(1);
    moved.SetValue(L"shown again");
    CHECK(fixture.grid->GetCellValue(1, 2) == "shown again");
    REQUIRE(fixture.grid->DeleteRows(1));
    CheckGridUnavailable([&] { (void)moved.Value(); });
    CheckGridUnavailable([&] { moved.SetValue(L"removed"); });
}

TEST_CASE("WinUIGrid::UIAValueStopsAtDestructiveEditCallbacks",
          "[winui-grid][winui-grid-uia][grid-uia-value]")
{
    const int boundary = GENERATE(0, 1, 2, 3, 4, 5, 6, 7);
    CAPTURE(boundary);
    AutomationCallbackTable model;
    wxGridStringTable replacement(3, 3);
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->SetTable(&model));
    fixture.Ready();
    const auto value = GridItemPeer(fixture.peer, fixture.provider.GetItem(1, 1))
        .GetPattern(A::PatternInterface::Value).as<P::IValueProvider>();
    REQUIRE(value);
    unsigned callbacks = 0;
    const auto destroy = [&]
        { ++callbacks; delete fixture.grid; fixture.grid = nullptr; };
    if ( boundary == 0 ) fixture.grid->Bind(wxEVT_GRID_EDITOR_SHOWN, [&](wxGridEvent&) { destroy(); });
    if ( boundary == 1 ) fixture.grid->Bind(wxEVT_GRID_EDITOR_CREATED, [&](wxGridEditorCreatedEvent&)
    {
        ++callbacks;
        const wxGridCellEditorPtr editor(fixture.grid->GetCellEditor(1, 1));
        editor->Destroy();
    });
    if ( boundary == 2 ) fixture.grid->Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent&)
        { ++callbacks; REQUIRE(fixture.grid->SetTable(&replacement)); });
    if ( boundary == 3 ) fixture.grid->Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent&) { destroy(); });
    if ( boundary == 4 ) fixture.grid->Bind(wxEVT_GRID_EDITOR_HIDDEN, [&](wxGridEvent&) { destroy(); });
    if ( boundary == 5 ) fixture.grid->Bind(wxEVT_GRID_EDITOR_CREATED, [&](wxGridEditorCreatedEvent& event)
        { model.onRead = destroy; event.Skip(); });
    if ( boundary == 6 ) fixture.grid->Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
        { model.onWrite = destroy; event.Skip(); });
    if ( boundary == 7 ) fixture.grid->Bind(wxEVT_GRID_EDITOR_SHOWN, [&](wxGridEvent& event)
        { model.onType = destroy; event.Skip(); });
    CHECK_THROWS_AS(value.SetValue(L"attempted"), winrt::hresult_error);
    CHECK(callbacks == 1);
    CHECK(model.GetValue(1, 1) == (boundary == 3 || boundary == 6 ? "attempted" : "before callback"));
    if ( boundary == 2 ) CHECK(replacement.GetValue(1, 1).empty());
}

TEST_CASE("WinUIGrid::UIAToggleUsesTheExistingEditVetoAndReadOnlyAuthority",
          "[winui-grid][winui-grid-uia]")
{
    GridAutomationFixture fixture;
    REQUIRE(fixture.grid->CreateGrid(4, 2));
    fixture.grid->SetColFormatBool(1);
    fixture.Ready();
    const auto cell = GridItemPeer(fixture.peer, fixture.provider.GetItem(0, 1));
    REQUIRE(WaitGridAutomation([&]
    {
        const auto visual = GridAutomationCell(fixture.grid->GetGridWindow(), 0, 1);
        return visual && visual.IsLoaded();
    }));
    const auto toggle = cell.GetPattern(A::PatternInterface::Toggle).as<P::IToggleProvider>();
    REQUIRE(toggle);
    fixture.grid->SetReadOnly(0, 1);
    CHECK_FALSE(cell.IsEnabled());
    CHECK_THROWS_AS(toggle.Toggle(), winrt::hresult_error);
    CHECK(fixture.grid->GetCellValue(0, 1).empty());
    fixture.grid->SetReadOnly(0, 1, false);
    bool veto = true;
    unsigned changing = 0, changed = 0;
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
        { ++changing; if ( veto ) event.Veto(); else event.Skip(); });
    fixture.grid->Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event)
        { ++changed; event.Skip(); });
    toggle.Toggle();
    CHECK(changing == 1); CHECK(changed == 0);
    CHECK(fixture.grid->GetCellValue(0, 1).empty());
    veto = false;
    toggle.Toggle();
    CHECK(changing == 2); CHECK(changed == 1);
    CHECK(fixture.grid->GetCellValue(0, 1) == "1");
    fixture.grid->Enable(false);
    CHECK_THROWS_AS(toggle.Toggle(), winrt::hresult_error);
    CHECK(changed == 1);
}
#endif

#endif
