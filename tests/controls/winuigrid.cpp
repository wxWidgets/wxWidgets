/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuigrid.cpp
// Purpose:     Native wxGrid projection of the real public model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_GRID && !defined(wxNO_RTTI)

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"
#include "wx/uilocale.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <algorithm>
#include <functional>
#include <utility>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitGrid(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1500 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

C::Grid NativeRoot(wxWindow* window)
{
    auto* host = window ? wxWinUITopLevelHost::FindSlotOwner(window) : nullptr;
    auto* slot = host ? host->FindSlot(window) : nullptr;
    return slot ? slot->GetContent().try_as<C::Grid>() : nullptr;
}

C::Canvas NativeCanvas(wxWindow* window)
{
    const auto root = NativeRoot(window);
    if ( !root || root.Children().Size() != 1 ) return nullptr;
    return root.Children().GetAt(0).try_as<C::Canvas>();
}

C::Control Cell(wxWindow* window, int row, int column)
{
    const auto canvas = NativeCanvas(window);
    if ( !canvas ) return nullptr;
    const auto id = winrt::hstring(wxString::Format("wxGrid.Cell.%d.%d", row, column).wc_str());
    for ( const auto& child : canvas.Children() )
        if ( A::AutomationProperties::GetAutomationId(child) == id )
            return child.try_as<C::Control>();
    return nullptr;
}

wxString CellText(wxWindow* window, int row, int column)
{
    const auto control = Cell(window, row, column);
    const auto button = control ? control.try_as<C::Button>() : nullptr;
    const auto text = button ? button.Content().try_as<C::TextBlock>() : nullptr;
    return text ? wxString(text.Text().c_str()) : wxString();
}

A::Provider::IInvokeProvider InvokeProvider(const C::Control& control)
{
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control);
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
        .try_as<A::Provider::IInvokeProvider>() : nullptr;
}

struct GridFixture
{
    explicit GridFixture(bool rtl = false)
    {
        wxPoint position(-30000, -30000);
        wchar_t desktopName[256]{};
        DWORD needed = 0;
        if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
                UOI_NAME, desktopName, sizeof(desktopName), &needed) &&
             wxString(desktopName).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
        frame = new wxFrame(nullptr, wxID_ANY, "native public wxGrid", position, wxSize(680, 420));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        grid = new wxGrid(frame, wxID_ANY, wxPoint(10, 10), wxSize(620, 340));
        frame->ShowWithoutActivating();
    }

    ~GridFixture()
    {
        delete frame;
        wxTheApp->ProcessPendingEvents();
    }

    void Ready()
    {
        REQUIRE(WaitGrid([&]()
        {
            const auto root = NativeRoot(grid->GetGridWindow());
            return root && root.IsLoaded() && root.ActualWidth() > 0 && NativeCanvas(grid->GetGridWindow());
        }));
    }

    wxFrame* frame = nullptr;
    wxGrid* grid = nullptr;
};

class VirtualTable : public wxGridTableBase
{
public:
    int GetNumberRows() override { return 100000; }
    int GetNumberCols() override { return 2000; }
    wxString GetValue(int row, int col) override
    {
        ++reads;
        return wxString::Format("%d:%d", row, col);
    }
    void SetValue(int, int, const wxString&) override {}
    unsigned reads = 0;
};

class CustomRenderer final : public wxGridCellStringRenderer
{
public:
    explicit CustomRenderer(unsigned* calls) : m_calls(calls) {}
    void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
              const wxRect& rect, int row, int col, bool selected) override
    {
        ++*m_calls;
        wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, selected);
    }
    wxGridCellRenderer* Clone() const override { return new CustomRenderer(m_calls); }
private:
    unsigned* m_calls;
};

class CallbackTable final : public wxGridStringTable
{
public:
    explicit CallbackTable(const wxString& value) : wxGridStringTable(2, 2)
    {
        for ( int row = 0; row != 2; ++row )
            for ( int col = 0; col != 2; ++col ) SetValue(row, col, value);
    }

    wxString GetValue(int row, int col) override
    {
        ++reads;
        // Keep the returned old-model value independent of a callback which
        // swaps the grid's table. This table is explicitly application-owned.
        const wxString value = wxGridStringTable::GetValue(row, col);
        const auto callback = std::move(onRead);
        onRead = nullptr;
        if ( callback ) callback();
        return value;
    }

    unsigned reads = 0;
    std::function<void()> onRead;
};

class CallbackRenderer final : public wxGridCellStringRenderer
{
public:
    explicit CallbackRenderer(std::function<void()> callback)
        : m_callback(std::move(callback)) {}

    void Draw(wxGrid&, wxGridCellAttr&, wxDC&, const wxRect&,
              int, int, bool) override
    {
        const auto callback = std::move(m_callback);
        m_callback = nullptr;
        if ( callback ) callback();
        // Deliberately do not touch the grid after application code may have
        // destroyed it. The projection must make the same lifetime check.
    }

    wxGridCellRenderer* Clone() const override { return new CallbackRenderer(m_callback); }

private:
    std::function<void()> m_callback;
};

class ChoiceTable final : public wxGridStringTable
{
public:
    ChoiceTable() : wxGridStringTable(3, 2) {}
    bool CanGetValueAs(int row, int col, const wxString& type) override
    {
        const bool typed = col == 1 && row != 1 && type == wxGRID_VALUE_NUMBER;
        if ( typed )
        {
            const auto callback = std::move(onCapability);
            onCapability = nullptr;
            if ( callback ) callback();
        }
        return typed;
    }
    long GetValueAsLong(int row, int) override
    {
        ++typedReads;
        const long value = row == 0 ? choice : 2;
        const auto callback = std::move(onTypedRead);
        onTypedRead = nullptr;
        if ( callback ) callback();
        return value;
    }
    long choice = 1;
    unsigned typedReads = 0;
    std::function<void()> onCapability, onTypedRead;
};

#if wxUSE_DATETIME
class DateTable final : public wxGridStringTable
{
public:
    DateTable() : wxGridStringTable(3, 4), value(4, wxDateTime::Oct, 2026, 13, 42, 7) {}
    bool CanGetValueAs(int, int col, const wxString& type) override
    {
        const bool typed = col == 2 && type == wxGRID_VALUE_DATETIME;
        if ( typed )
        {
            const auto callback = std::move(onCapability);
            onCapability = nullptr;
            if ( callback ) callback();
        }
        return typed;
    }
    void* GetValueAsCustom(int row, int, const wxString&) override
    {
        ++customReads;
        const auto callback = std::move(onTypedRead);
        onTypedRead = nullptr;
        if ( callback ) callback();
        return row == 1 ? nullptr : new wxDateTime(value);
    }
    wxDateTime value;
    unsigned customReads = 0;
    std::function<void()> onCapability, onTypedRead;
};
#endif

template<typename Base>
class CustomFormattedRenderer final : public Base
{
public:
    explicit CustomFormattedRenderer(unsigned* draws) : m_draws(draws) {}
    void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
              const wxRect& rect, int row, int col, bool selected) override
    {
        ++*m_draws;
        Base::Draw(grid, attr, dc, rect, row, col, selected);
    }
    wxGridCellRenderer* Clone() const override { return new CustomFormattedRenderer(*this); }
private:
    unsigned* m_draws;
};
}

TEST_CASE("WinUIGrid::NativeStandardCellsAndHeadersFollowPublicMutations",
          "[winui-grid][native-grid]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    GridFixture fixture(rtl);
    auto& grid = *fixture.grid;
    REQUIRE(grid.CreateGrid(8, 6));
    grid.SetCellValue(0, 0, "native text");
    grid.SetColFormatBool(1);
    grid.SetCellValue(0, 1, "1");
    grid.SetColFormatNumber(2);
    grid.SetCellValue(0, 2, "42");
    grid.SetColFormatFloat(3, -1, 2);
    grid.SetCellValue(0, 3, "12.5");
    grid.SetRowLabelValue(0, "first row");
    grid.SetColLabelValue(0, "first column");
    grid.ForceRefresh();
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 0) == "native text"; }));
    const auto text = Cell(grid.GetGridWindow(), 0, 0).as<C::Button>();
    REQUIRE(text.Content().try_as<C::TextBlock>());
    CHECK_FALSE(text.Content().try_as<C::Image>());
    const auto check = Cell(grid.GetGridWindow(), 0, 1).try_as<C::CheckBox>();
    REQUIRE(check);
    REQUIRE(check.IsChecked());
    CHECK(check.IsChecked().Value());
    CHECK(CellText(grid.GetGridWindow(), 0, 2) == "42");
    CHECK_FALSE(CellText(grid.GetGridWindow(), 0, 3).empty());
    CHECK(CellText(grid.GetGridRowLabelWindow(), 0, -1) == "first row");
    CHECK(CellText(grid.GetGridColLabelWindow(), -1, 0) == "first column");
    REQUIRE(NativeCanvas(grid.GetGridCornerLabelWindow()));

    grid.BeginBatch();
    grid.SetCellValue(0, 0, "updated native text");
    grid.SetRowLabelValue(0, "updated row");
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "native text");
    grid.EndBatch();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 0) == "updated native text"; }));
    CHECK(Cell(grid.GetGridWindow(), 0, 0) == text);
    CHECK(CellText(grid.GetGridRowLabelWindow(), 0, -1) == "updated row");
    CHECK(grid.GetCellValue(0, 0) == "updated native text");

    grid.EnableEditing(false);
    REQUIRE(WaitGrid([&]() { return !check.IsEnabled(); }));
    grid.EnableEditing(true);
    REQUIRE(WaitGrid([&]() { return check.IsEnabled(); }));
    grid.SetReadOnly(0, 1);
    REQUIRE(WaitGrid([&]() { return !check.IsEnabled(); }));
    grid.SetReadOnly(0, 1, false);
    REQUIRE(WaitGrid([&]() { return check.IsEnabled(); }));
}

TEST_CASE("WinUIGrid::ViewportVirtualizesBothAxesAndFrozenWindows",
          "[winui-grid][native-grid]")
{
    GridFixture fixture;
    auto& grid = *fixture.grid;
    auto* table = new VirtualTable;
    REQUIRE(grid.SetTable(table, true));
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 0) == "0:0"; }));
    REQUIRE(NativeCanvas(grid.GetGridWindow()));
    CHECK(NativeCanvas(grid.GetGridWindow()).Children().Size() < 400);
    CHECK(table->reads < 2000);
    table->reads = 0;
    grid.GoToCell(90000, 1900);
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 90000, 1900) == "90000:1900"; }));
    CHECK_FALSE(Cell(grid.GetGridWindow(), 0, 0));
    CHECK(NativeCanvas(grid.GetGridWindow()).Children().Size() < 400);
    CHECK(table->reads < 2000);

    grid.GoToCell(0, 0);
    REQUIRE(grid.FreezeTo(1, 1));
    REQUIRE(WaitGrid([&]()
    {
        return NativeCanvas(grid.GetFrozenCornerGridWindow()) &&
               CellText(grid.GetFrozenCornerGridWindow(), 0, 0) == "0:0";
    }));
    REQUIRE(NativeRoot(grid.GetFrozenRowGridWindow()));
    REQUIRE(NativeRoot(grid.GetFrozenColGridWindow()));
    const auto frozen = Cell(grid.GetFrozenCornerGridWindow(), 0, 0);
    grid.GoToCell(80000, 1800);
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 80000, 1800) == "80000:1800"; }));
    CHECK(Cell(grid.GetFrozenCornerGridWindow(), 0, 0) == frozen);
    CHECK(CellText(grid.GetFrozenCornerGridWindow(), 0, 0) == "0:0");
}

TEST_CASE("WinUIGrid::NativeProvidersRespectSelectionAndChangingVeto",
          "[winui-grid][native-grid]")
{
    GridFixture fixture;
    auto& grid = *fixture.grid;
    REQUIRE(grid.CreateGrid(5, 4));
    grid.SetColFormatBool(1);
    grid.SetCellValue(0, 1, "");
    grid.ForceRefresh();
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return Cell(grid.GetGridWindow(), 1, 0) != nullptr; }));
    auto invoke = InvokeProvider(Cell(grid.GetGridWindow(), 1, 0));
    REQUIRE(invoke);
    // Veto must preserve the exact existing cursor, not select the native
    // provider's target before the wx selection transaction has accepted it.
    const wxGridCellCoords initialCursor = grid.GetGridCursorCoords();
    bool vetoSelection = true;
    grid.Bind(wxEVT_GRID_SELECT_CELL, [&](wxGridEvent& event)
    {
        if ( vetoSelection && event.GetRow() == 1 ) event.Veto();
        else event.Skip();
    });
    invoke.Invoke();
    CHECK(grid.GetGridCursorCoords() == initialCursor);
    vetoSelection = false;
    invoke.Invoke();
    CHECK(grid.GetGridCursorRow() == 1);
    CHECK(grid.GetGridCursorCol() == 0);

    unsigned changing = 0, changed = 0;
    bool vetoChange = true;
    grid.Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
    {
        ++changing;
        if ( vetoChange ) event.Veto();
        else event.Skip();
    });
    grid.Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event) { ++changed; event.Skip(); });
    const auto check = Cell(grid.GetGridWindow(), 0, 1).as<C::CheckBox>();
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(check);
    REQUIRE(peer);
    const auto toggle = peer.GetPattern(A::Peers::PatternInterface::Toggle).try_as<A::Provider::IToggleProvider>();
    REQUIRE(toggle);
    toggle.Toggle();
    REQUIRE(WaitGrid([&]() { return check.IsChecked() && !check.IsChecked().Value(); }));
    CHECK(grid.GetCellValue(0, 1).empty());
    CHECK(changing == 1);
    CHECK(changed == 0);
    vetoChange = false;
    toggle.Toggle();
    REQUIRE(WaitGrid([&]() { return grid.GetCellValue(0, 1) == "1" && check.IsChecked().Value(); }));
    CHECK(changing == 2);
    CHECK(changed == 1);
}

TEST_CASE("WinUIGrid::PublicEditorUsesNativeTextAndCommonCommitProtocol",
          "[winui-grid][native-grid]")
{
    GridFixture fixture;
    auto& grid = *fixture.grid;
    REQUIRE(grid.CreateGrid(3, 3));
    grid.SetCellValue(0, 0, "before");
    fixture.Ready();
    wxTextCtrl* editor = nullptr;
    grid.Bind(wxEVT_GRID_EDITOR_CREATED, [&](wxGridEditorCreatedEvent& event)
    {
        editor = wxDynamicCast(event.GetWindow(), wxTextCtrl);
        event.Skip();
    });
    unsigned changed = 0;
    bool veto = true;
    grid.Bind(wxEVT_GRID_CELL_CHANGING, [&](wxGridEvent& event)
    {
        if ( veto ) event.Veto(); else event.Skip();
    });
    grid.Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event) { ++changed; event.Skip(); });
    grid.SetGridCursor(0, 0);
    grid.EnableCellEditControl();
    REQUIRE(editor);
    auto* host = wxWinUITopLevelHost::FindSlotOwner(editor);
    REQUIRE(host);
    auto* slot = host->FindSlot(editor);
    REQUIRE(slot);
    // Production wxTextCtrl uses RichEditBox even for plain single-line text;
    // TextBox is only its explicitly requested legacy test configuration.
    const auto nativeEditor = slot->GetSemanticTarget().try_as<C::RichEditBox>();
    REQUIRE(nativeEditor);
    REQUIRE(WaitGrid([&]()
    {
        return nativeEditor.IsLoaded() && nativeEditor.ActualWidth() > 0 &&
               nativeEditor.ActualHeight() > 0;
    }));
    const auto nativeText = [&]()
    {
        namespace T = winrt::Microsoft::UI::Text;
        winrt::hstring text;
        const auto options = static_cast<T::TextGetOptions>(
            static_cast<uint32_t>(T::TextGetOptions::UseLf) |
            static_cast<uint32_t>(T::TextGetOptions::AllowFinalEop));
        nativeEditor.Document().GetText(options, text);
        wxString value(text.c_str());
        // TOM includes its final paragraph terminator, distinct from the
        // contents of the single-line wx editor.
        if ( !value.empty() && (value.Last() == '\r' || value.Last() == '\n') )
            value.RemoveLast();
        return value;
    };
    REQUIRE(nativeText() == "before");
    editor->ChangeValue("vetoed");
    REQUIRE(nativeText() == "vetoed");
    grid.DisableCellEditControl();
    CHECK(grid.GetCellValue(0, 0) == "before");
    CHECK(changed == 0);
    veto = false;
    grid.EnableCellEditControl();
    editor->ChangeValue("committed");
    REQUIRE(nativeText() == "committed");
    grid.DisableCellEditControl();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 0) == "committed"; }));
    CHECK(grid.GetCellValue(0, 0) == "committed");
    CHECK(changed == 1);

    // Disable hides the reusable editor; explicitly retire its real native
    // control as well. The child's propagated destroy event must not close
    // the grid-window projection that contains it.
    const auto paneBefore = NativeRoot(grid.GetGridWindow());
    const auto cellBefore = Cell(grid.GetGridWindow(), 0, 0);
    REQUIRE(paneBefore); REQUIRE(cellBefore);
    const wxWeakRef<wxTextCtrl> retiredEditor(editor);
    const wxGridCellEditorPtr cellEditor(grid.GetCellEditor(0, 0));
    cellEditor->Destroy();
    editor = nullptr;
    REQUIRE(WaitGrid([&]() { return !retiredEditor; }));
    CHECK(NativeRoot(grid.GetGridWindow()) == paneBefore);
    CHECK(paneBefore.IsLoaded());
    CHECK(Cell(grid.GetGridWindow(), 0, 0) == cellBefore);
    CHECK(cellBefore.IsLoaded());
    grid.EnableCellEditControl();
    REQUIRE(editor);
    editor->ChangeValue("reopened after destruction");
    grid.DisableCellEditControl();
    REQUIRE(WaitGrid([&]()
        { return CellText(grid.GetGridWindow(), 0, 0) == "reopened after destruction"; }));
    CHECK(changed == 2);
    CHECK(NativeRoot(grid.GetGridWindow()) == paneBefore);
}

TEST_CASE("WinUIGrid::CustomRendererAndRetiredProviderKeepTheirContracts",
          "[winui-grid][native-grid]")
{
    unsigned draws = 0;
    GridFixture fixture;
    auto& grid = *fixture.grid;
    REQUIRE(grid.CreateGrid(5, 4));
    grid.SetCellValue(0, 0, "custom");
    grid.SetCellRenderer(0, 0, new CustomRenderer(&draws));
    grid.ForceRefresh();
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return draws > 0 && Cell(grid.GetGridWindow(), 0, 0) != nullptr; }));
    const auto custom = Cell(grid.GetGridWindow(), 0, 0).as<C::Button>();
    REQUIRE(custom.Content().try_as<C::Image>());
    const auto standard = Cell(grid.GetGridWindow(), 0, 1).as<C::Button>();
    REQUIRE(standard.Content().try_as<C::TextBlock>());
    auto stale = InvokeProvider(standard);
    REQUIRE(stale);
    REQUIRE(grid.DeleteRows(0));
    grid.SetGridCursor(2, 2);
    stale.Invoke(); // It must not reinterpret an old row index as a new item.
    CHECK(grid.GetGridCursorRow() == 2);
    CHECK(grid.GetGridCursorCol() == 2);
    REQUIRE(WaitGrid([&]()
    {
        const auto replacement = Cell(grid.GetGridWindow(), 0, 1);
        return replacement && replacement != standard && replacement.IsLoaded();
    }));
    CHECK(Cell(grid.GetGridWindow(), 0, 1) != standard);
    stale.Invoke();
    CHECK(grid.GetGridCursorRow() == 2);
    CHECK(grid.GetGridCursorCol() == 2);
}

TEST_CASE("WinUIGrid::FirstPresentationAndUpdateKeepSynchronousContracts",
          "[winui-grid][native-grid]")
{
    unsigned draws = 0, selections = 0;
    GridFixture fixture;
    auto& grid = *fixture.grid;
    grid.Bind(wxEVT_GRID_SELECT_CELL, [&](wxGridEvent& event)
    {
        ++selections;
        event.Skip();
    });
    CHECK(grid.GetGridCursorCoords() == wxGridNoCellCoords);
    REQUIRE(grid.CreateGrid(2, 2));
    CHECK(grid.GetGridCursorCoords() == wxGridNoCellCoords);

    // No event-loop pumping: Update() must publish the requested native cells
    // and complete the same first-paint state transition as the generic grid.
    grid.Update();
    REQUIRE(Cell(grid.GetGridWindow(), 0, 0));
    CHECK(grid.GetGridCursorCoords() == wxGridCellCoords(0, 0));
    CHECK(selections == 0);

    grid.SetCellRenderer(0, 0, new CustomRenderer(&draws));
    grid.Refresh();
    grid.Update();
    CHECK(draws == 1);
    REQUIRE(Cell(grid.GetGridWindow(), 0, 0).as<C::Button>().Content().try_as<C::Image>());

    // A repeated synchronous update must not replay the already consumed
    // asynchronous repaint ticket or redraw the custom cell a second time.
    grid.Update();
    CHECK(draws == 1);
    CHECK(selections == 0);
}

TEST_CASE("WinUIGrid::UpdateDoesNotReadOrPublishInsideBatch",
          "[winui-grid][native-grid][grid-update-reentrant]")
{
    CallbackTable table("before batch");
    GridFixture fixture;
    auto& grid = *fixture.grid;
    REQUIRE(grid.SetTable(&table));
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 0) == "before batch"; }));
    const auto original = Cell(grid.GetGridWindow(), 0, 0);

    grid.BeginBatch();
    grid.SetCellValue(0, 0, "after batch");
    // SetCellValue itself reads to suppress equal-value writes. Count only
    // presentation reads made after this public mutation has completed.
    table.reads = 0;
    grid.Refresh();
    grid.Update();
    wxTheApp->ProcessPendingEvents(); // Consume any previously queued repaint.
    grid.Update();
    CHECK(table.reads == 0);
    CHECK(Cell(grid.GetGridWindow(), 0, 0) == original);
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "before batch");

    grid.EndBatch();
    grid.Update();
    CHECK(table.reads > 0);
    CHECK(Cell(grid.GetGridWindow(), 0, 0) == original);
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "after batch");
}

TEST_CASE("WinUIGrid::UpdateRejectsReentrantModelAndOwnerRetirement",
          "[winui-grid][native-grid][grid-update-reentrant]")
{
    const bool fromRenderer = GENERATE(false, true);
    const bool destroyOwner = GENERATE(false, true);
    CAPTURE(fromRenderer, destroyOwner);
    CallbackTable originalTable("old published value"), replacementTable("new table value");
    GridFixture fixture;
    wxGrid* const updating = fixture.grid;
    REQUIRE(updating->SetTable(&originalTable));
    fixture.Ready();
    REQUIRE(WaitGrid([&]()
    {
        return CellText(updating->GetGridWindow(), 0, 0) == "old published value";
    }));
    wxWindow* const bodyAddress = updating->GetGridWindow();
    const wxWeakRef<wxGrid> weakGrid(updating);
    const wxWeakRef<wxWindow> weakBody(bodyAddress);
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(bodyAddress);
    REQUIRE(host);
    const auto oldControl = Cell(bodyAddress, 0, 0).as<C::Button>();
    const auto oldText = oldControl.Content().as<C::TextBlock>();
    const auto oldProvider = InvokeProvider(oldControl);
    REQUIRE(oldProvider);
    unsigned callbacks = 0;
    bool replacementAccepted = false;
    const auto mutate = [&]()
    {
        ++callbacks;
        if ( destroyOwner )
        {
            fixture.grid = nullptr;
            delete updating;
        }
        else
        {
            replacementAccepted = updating->SetTable(&replacementTable);
            // Recursive Update must not publish either the in-flight old
            // snapshot or a partially attached replacement table.
            updating->Update();
            CHECK(oldText.Text() == L"old published value");
        }
    };
    if ( fromRenderer ) updating->SetCellRenderer(0, 0, new CallbackRenderer(mutate));
    else originalTable.onRead = mutate;

    updating->Refresh();
    updating->Update();
    CHECK(callbacks == 1);
    CHECK(oldText.Text() == L"old published value");
    if ( destroyOwner )
    {
        CHECK_FALSE(weakGrid);
        CHECK_FALSE(weakBody);
        // FindSlot is an identity-map lookup; it does not dereference this
        // retired address. The top-level frame and its host are still alive.
        CHECK_FALSE(host->FindSlot(bodyAddress));
        CHECK_NOTHROW(oldProvider.Invoke());
        wxTheApp->ProcessPendingEvents();
        CHECK_FALSE(host->FindSlot(bodyAddress));
    }
    else
    {
        REQUIRE(replacementAccepted);
        REQUIRE(weakGrid);
        CHECK(updating->GetTable() == &replacementTable);
        REQUIRE(Cell(bodyAddress, 0, 0));
        CHECK(Cell(bodyAddress, 0, 0) != oldControl);
        CHECK(CellText(bodyAddress, 0, 0) == "new table value");
        updating->SetGridCursor(1, 1);
        oldProvider.Invoke();
        CHECK(updating->GetGridCursorCoords() == wxGridCellCoords(1, 1));
    }
}

TEST_CASE("WinUIGrid::NativeChoiceAndEnumPreserveTypedAndStringValues",
          "[winui-grid][native-grid][grid-formatted-renderers]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    ChoiceTable table;
    table.SetValue(0, 0, "Second");
    table.SetValue(1, 0, "not restricted by the choices");
    table.SetValue(1, 1, "untyped enum text");
    GridFixture fixture(rtl);
    auto& grid = *fixture.grid;
    REQUIRE(grid.SetTable(&table));
    auto* choice = new wxGridCellChoiceRenderer;
    choice->SetParameters("First,Second,Third");
    auto* choiceAttr = new wxGridCellAttr;
    choiceAttr->SetRenderer(choice);
    grid.SetColAttr(0, choiceAttr);
    auto* enumeration = new wxGridCellEnumRenderer;
    enumeration->SetParameters("First,Second,Third");
    auto* enumAttr = new wxGridCellAttr;
    enumAttr->SetRenderer(enumeration);
    grid.SetColAttr(1, enumAttr);
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 1) == "Second"; }));
    for ( int row = 0; row != 3; ++row )
    {
        for ( int col = 0; col != 2; ++col )
        {
            const auto button = Cell(grid.GetGridWindow(), row, col).as<C::Button>();
            REQUIRE(button.Content().try_as<C::TextBlock>());
            CHECK_FALSE(button.Content().try_as<C::Image>());
        }
    }
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "Second");
    CHECK(CellText(grid.GetGridWindow(), 1, 0) == "not restricted by the choices");
    CHECK(CellText(grid.GetGridWindow(), 2, 0).empty());
    CHECK(CellText(grid.GetGridWindow(), 1, 1) == "untyped enum text");
    CHECK(CellText(grid.GetGridWindow(), 2, 1) == "Third");
    const auto choiceText = Cell(grid.GetGridWindow(), 0, 0).as<C::Button>().Content().as<C::TextBlock>();
    const auto enumText = Cell(grid.GetGridWindow(), 0, 1).as<C::Button>().Content().as<C::TextBlock>();
    CHECK(choiceText.HorizontalAlignment() == X::HorizontalAlignment::Left);
    CHECK(enumText.HorizontalAlignment() == X::HorizontalAlignment::Right);

    // Mutating the renderer's parameters requires the same RefreshAttr as the
    // generic grid. The native text must use its new choices, not a side model.
    enumeration->SetParameters("Alpha,Beta,Gamma");
    table.choice = 2;
    grid.RefreshAttr(0, 1);
    grid.Update();
    CHECK(enumText.Text() == L"Gamma");
    grid.SetCellAlignment(0, 1, wxALIGN_LEFT, wxALIGN_BOTTOM);
    grid.Update();
    CHECK(enumText.HorizontalAlignment() == X::HorizontalAlignment::Left);
    CHECK(enumText.VerticalAlignment() == X::VerticalAlignment::Bottom);
    table.choice = -1;
    grid.RefreshAttr(0, 1);
    grid.Update();
    CHECK(enumText.Text().empty());
    table.choice = 1000;
    grid.RefreshAttr(0, 1);
    grid.Update();
    CHECK(enumText.Text().empty());
}

#if wxUSE_DATETIME
TEST_CASE("WinUIGrid::NativeDatesUseCommonParsingFormatsAndLocale",
          "[winui-grid][native-grid][grid-formatted-renderers]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    DateTable table;
    const wxString longFormat = "%A %d %B %Y";
    table.SetValue(0, 0, "04.10.2026");
    table.SetValue(1, 0, "today");
    table.SetValue(2, 0, "not a date");
    table.SetValue(0, 1, "04-10-2026 13:42:07");
    table.SetValue(1, 1, "today");
    table.SetValue(2, 1, "invalid date-time");
    table.SetValue(1, 2, table.value.Format(longFormat)); // Typed null falls back to text.
    for ( int row = 0; row != 3; ++row )
        table.SetValue(row, 3, table.value.Format(wxGetUIDateFormat()));
    GridFixture fixture(rtl);
    auto& grid = *fixture.grid;
    REQUIRE(grid.SetTable(&table));
    auto* date = new wxGridCellDateRenderer("%d.%m.%Y");
    auto* dateTime = new wxGridCellDateTimeRenderer("%Y/%m/%d %H:%M", "%d-%m-%Y %H:%M:%S");
    wxGridCellRenderer* renderers[] = {
        date, dateTime, new wxGridCellDateRenderer(longFormat), new wxGridCellDateRenderer
    };
    for ( int col = 0; col != 4; ++col )
    {
        auto* attr = new wxGridCellAttr;
        attr->SetRenderer(renderers[col]);
        grid.SetColAttr(col, attr);
    }
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return CellText(grid.GetGridWindow(), 0, 1) == "2026/10/04 13:42"; }));
    for ( int row = 0; row != 3; ++row )
    {
        for ( int col = 0; col != 4; ++col )
        {
            const auto button = Cell(grid.GetGridWindow(), row, col).as<C::Button>();
            const auto text = button.Content().try_as<C::TextBlock>();
            REQUIRE(text);
            CHECK_FALSE(button.Content().try_as<C::Image>());
            CHECK(text.HorizontalAlignment() == X::HorizontalAlignment::Right);
        }
        CHECK(CellText(grid.GetGridWindow(), row, 2) == table.value.Format(longFormat));
        CHECK(CellText(grid.GetGridWindow(), row, 3) == table.value.Format(wxGetUIDateFormat()));
    }
    CHECK(table.customReads > 0);
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "04.10.2026");
    CHECK(CellText(grid.GetGridWindow(), 1, 0) == wxDateTime::Today().Format("%d.%m.%Y"));
    CHECK(CellText(grid.GetGridWindow(), 2, 0).empty());
    CHECK(CellText(grid.GetGridWindow(), 1, 1).empty()); // No free-form fallback in DateTime.
    CHECK(CellText(grid.GetGridWindow(), 2, 1).empty());

    dateTime->SetParameters("%Hh%M");
    grid.RefreshAttr(0, 1);
    grid.Update();
    CHECK(CellText(grid.GetGridWindow(), 0, 1) == "13h42");
    date->SetParameters(wxString()); // Empty parameters preserve the output format.
    grid.RefreshAttr(0, 0);
    grid.Update();
    CHECK(CellText(grid.GetGridWindow(), 0, 0) == "04.10.2026");
}
#endif

TEST_CASE("WinUIGrid::FormattedRendererSubclassesKeepCustomDraw",
          "[winui-grid][native-grid][grid-formatted-renderers]")
{
    unsigned draws = 0;
    GridFixture fixture;
    auto& grid = *fixture.grid;
#if wxUSE_DATETIME
    constexpr int count = 4;
#else
    constexpr int count = 2;
#endif
    REQUIRE(grid.CreateGrid(1, count));
    wxGridCellRenderer* renderers[] = {
        new CustomFormattedRenderer<wxGridCellChoiceRenderer>(&draws),
        new CustomFormattedRenderer<wxGridCellEnumRenderer>(&draws),
#if wxUSE_DATETIME
        new CustomFormattedRenderer<wxGridCellDateRenderer>(&draws),
        new CustomFormattedRenderer<wxGridCellDateTimeRenderer>(&draws),
#endif
    };
    for ( int col = 0; col != count; ++col )
    {
        grid.SetCellValue(0, col, "custom cell");
        grid.SetCellRenderer(0, col, renderers[col]);
    }
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return draws >= static_cast<unsigned>(count) && Cell(grid.GetGridWindow(), 0, count - 1); }));
    for ( int col = 0; col != count; ++col )
    {
        const auto button = Cell(grid.GetGridWindow(), 0, col).as<C::Button>();
        CHECK(button.Content().try_as<C::Image>());
        CHECK_FALSE(button.Content().try_as<C::TextBlock>());
    }
}

TEST_CASE("WinUIGrid::NativeFormattedValuesRejectRetiredModelCallbacks",
          "[winui-grid][native-grid][grid-formatted-renderers][grid-update-reentrant]")
{
#if wxUSE_DATETIME
    const bool dates = GENERATE(false, true);
    DateTable dateTable;
#else
    const bool dates = false;
#endif
    const bool fromCapability = GENERATE(false, true);
    const bool destroyOwner = GENERATE(false, true);
    CAPTURE(dates, fromCapability, destroyOwner);
    ChoiceTable choiceTable;
    wxGridStringTable replacement(1, 1);
    replacement.SetValue(0, 0, "replacement model");
    GridFixture fixture;
    wxGrid* const updating = fixture.grid;
    const wxWeakRef<wxGrid> weakGrid(updating);
    int col = 1;
#if wxUSE_DATETIME
    if ( dates )
    {
        REQUIRE(updating->SetTable(&dateTable));
        col = 2;
        updating->SetCellRenderer(0, col, new wxGridCellDateRenderer("%Y-%m-%d"));
    }
    else
#endif
    {
        REQUIRE(updating->SetTable(&choiceTable));
        updating->SetCellRenderer(0, col, new wxGridCellEnumRenderer("First,Second,Third"));
    }
    fixture.Ready();
    REQUIRE(WaitGrid([&]() { return !CellText(updating->GetGridWindow(), 0, col).empty(); }));
    const auto oldControl = Cell(updating->GetGridWindow(), 0, col).as<C::Button>();
    const auto oldText = oldControl.Content().as<C::TextBlock>();
    const auto oldValue = oldText.Text();
    const auto oldProvider = InvokeProvider(oldControl);
    REQUIRE(oldProvider);
    unsigned callbacks = 0;
    bool accepted = false;
    const auto mutate = [&]()
    {
        ++callbacks;
        if ( destroyOwner )
        {
            fixture.grid = nullptr;
            delete updating;
        }
        else accepted = updating->SetTable(&replacement);
    };
    unsigned* reads = &choiceTable.typedReads;
    std::function<void()>* callback = fromCapability ? &choiceTable.onCapability : &choiceTable.onTypedRead;
#if wxUSE_DATETIME
    if ( dates )
    {
        reads = &dateTable.customReads;
        callback = fromCapability ? &dateTable.onCapability : &dateTable.onTypedRead;
    }
#endif
    *reads = 0;
    *callback = mutate;
    updating->Refresh();
    updating->Update();
    CHECK(callbacks == 1);
    CHECK(*reads == (fromCapability ? 0u : 1u));
    CHECK(oldText.Text() == oldValue);
    if ( destroyOwner )
    {
        CHECK_FALSE(weakGrid);
        CHECK_NOTHROW(oldProvider.Invoke());
    }
    else
    {
        REQUIRE(accepted);
        CHECK(updating->GetTable() == &replacement);
        CHECK(CellText(updating->GetGridWindow(), 0, 0) == "replacement model");
        CHECK_FALSE(Cell(updating->GetGridWindow(), 0, col));
        const auto cursor = updating->GetGridCursorCoords();
        oldProvider.Invoke();
        CHECK(updating->GetGridCursorCoords() == cursor);
    }
}

#endif
