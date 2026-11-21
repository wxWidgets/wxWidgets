/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuifilebrowsers.cpp
// Purpose:     Native presentation of the public file and directory browsers
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_FILECTRL && \
    wxUSE_LISTCTRL && wxUSE_TREECTRL && (wxUSE_FILEDLG || wxUSE_DIRDLG)

#include "wx/app.h"
#include "wx/dirctrl.h"
#include "wx/evtloop.h"
#include "wx/file.h"
#include "wx/filectrl.h"
#include "wx/filename.h"
#include "wx/frame.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/treectrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "testfile.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitBrowser(Predicate predicate)
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

wxPoint BrowserPosition()
{
    wchar_t name[256]{};
    DWORD required = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, name, sizeof(name), &required) &&
         wxString(name).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

X::DependencyObject BrowserNativeTree(wxWindow* window)
{
    auto* const host = wxWinUITopLevelHost::ForWindow(window, false);
    return host && host->GetXamlRoot() ? host->GetXamlRoot().Content() : nullptr;
}

template<typename T, typename Predicate>
T FindBrowserPeer(const X::DependencyObject& node, Predicate predicate)
{
    if ( !node ) return nullptr;
    if ( const auto result = node.try_as<T>(); result && predicate(result) )
        return result;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int n = 0; n < count; ++n )
        if ( const auto found = FindBrowserPeer<T>(
                X::Media::VisualTreeHelper::GetChild(node, n), predicate) )
            return found;
    return nullptr;
}

template<typename T>
T BrowserPeerWithId(wxWindow* window, const wxString& id)
{
    return FindBrowserPeer<T>(BrowserNativeTree(window), [&](const T& peer)
    {
        return wxString(A::AutomationProperties::GetAutomationId(peer).c_str()) == id;
    });
}

bool BrowserLoaded(const X::FrameworkElement& peer)
{
    return peer && peer.IsLoaded() && peer.ActualWidth() > 0 && peer.ActualHeight() > 0;
}

wxString BrowserText(const X::DependencyObject& node)
{
    const auto text = FindBrowserPeer<C::TextBlock>(node,
        [](const C::TextBlock&) { return true; });
    return text ? wxString(text.Text().c_str()) : wxString();
}

C::Control FileCell(wxFileCtrl* browser, const wxString& name)
{
    auto* const list = browser->GetFileList();
    const long row = list->FindItem(-1, name);
    if ( row == wxNOT_FOUND ) return nullptr;
    const auto peer = BrowserPeerWithId<C::Control>(browser,
        wxString::Format("wxList.Item.%ld.0", row));
    // An index can be reused by a filter/navigation refresh before its queued
    // native projection has completed. Require the current content as well.
    return peer && BrowserText(peer) == name ? peer : nullptr;
}

C::TreeViewItem DirectoryCell(wxGenericDirCtrl* browser, const wxString& name)
{
    return FindBrowserPeer<C::TreeViewItem>(BrowserNativeTree(browser),
        [&](const C::TreeViewItem& peer)
        {
            return wxString(A::AutomationProperties::GetName(peer).c_str()) == name;
        });
}

wxTreeItemId DirectoryChild(wxTreeCtrl* tree, wxTreeItemId parent,
                            const wxString& name)
{
    wxTreeItemIdValue cookie;
    for ( auto child = tree->GetFirstChild(parent, cookie); child.IsOk();
          child = tree->GetNextChild(parent, cookie) )
        if ( tree->GetItemText(child) == name ) return child;
    return {};
}

template<typename Provider>
Provider BrowserProvider(const C::Control& control, A::Peers::PatternInterface pattern)
{
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(control);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

template<typename Action>
void CallRetiredBrowserProvider(Action action)
{
    try { action(); }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) ||
               code == static_cast<int32_t>(0x80040201u)));
    }
}

struct BrowserFixture
{
    explicit BrowserFixture(bool rtl = false)
    {
        REQUIRE(files.IsOk());
        REQUIRE(wxMkdir(Path("child")));
        Write("alpha.txt");
        Write("beta.log");
        Write("child/nested.txt");
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
            "native filesystem browser", BrowserPosition(), wxSize(800, 520));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    }
    ~BrowserFixture()
    {
        frame.reset();
        wxTheApp->ProcessPendingEvents();
    }
    wxString Path(const wxString& relative) const
    {
        return files.GetName() + wxFILE_SEP_PATH + relative;
    }
    void Write(const wxString& relative)
    {
        wxFile file;
        REQUIRE(file.Create(Path(relative)));
        REQUIRE(file.Write("native file browser fixture"));
        REQUIRE(file.Close());
    }
    void Show(wxWindow* browser)
    {
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(browser, 1, wxEXPAND);
        frame->SetSizer(sizer);
        frame->Layout();
        frame->ShowWithoutActivating();
    }
    TempDir files{"wx-winui-browser-"};
    std::unique_ptr<wxFrame> frame;
};

struct NativeTreeSelectionFixture : BrowserFixture
{
    explicit NativeTreeSelectionFixture(bool rtl = false)
        : BrowserFixture(rtl)
    {
        tree = new wxTreeCtrl(frame.get(), wxID_ANY);
        root = tree->AddRoot("Root");
        first = tree->AppendItem(root, "First");
        second = tree->AppendItem(root, "Second");
        third = tree->AppendItem(root, "Third");
        tree->Expand(root);
        tree->SelectItem(first);
        Show(tree);
        tree->EnsureVisible(third);
        REQUIRE(WaitBrowser([&]
        {
            return BrowserLoaded(Cell("First")) &&
                   BrowserLoaded(Cell("Second")) &&
                   BrowserLoaded(Cell("Third"));
        }));
        native = FindBrowserPeer<C::TreeView>(BrowserNativeTree(tree),
            [](const C::TreeView&) { return true; });
        REQUIRE(native);
        REQUIRE(WaitBrowser([&]
        {
            return native.SelectedNode() == native.NodeFromContainer(Cell("First"));
        }));
    }

    C::TreeViewItem Cell(const wxString& label) const
    {
        return FindBrowserPeer<C::TreeViewItem>(BrowserNativeTree(tree),
            [&](const C::TreeViewItem& peer)
            {
                return wxString(A::AutomationProperties::GetName(peer).c_str()) == label;
            });
    }

    A::Provider::ISelectionItemProvider Provider(const wxString& label) const
    {
        const auto row = Cell(label);
        REQUIRE(row);
        const auto provider = BrowserProvider<A::Provider::ISelectionItemProvider>(
            row, A::Peers::PatternInterface::SelectionItem);
        REQUIRE(provider);
        return provider;
    }

    void DrainSelection() const
    {
        // A causal dispatcher fence, not a sleep: pending genuine removals and
        // selection corrections must have completed before inspecting counts.
        const auto done = std::make_shared<bool>(false);
        REQUIRE(native.DispatcherQueue().TryEnqueue([done] { *done = true; }));
        REQUIRE(WaitBrowser([&] { return *done; }));
    }

    wxTreeCtrl* tree = nullptr;
    wxTreeItemId root, first, second, third;
    C::TreeView native{nullptr};
};
}

TEST_CASE("wxWinUI FileCtrl native list items fit filenames and expose selection items",
          "[winui-file-browsers][winui-filectrl][native-item-container][native-text-layout]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    BrowserFixture fixture(rtl);
    const wxString filename = "Quarterly planning notes - approved.txt";
    fixture.Write(filename);
    auto* const browser = new wxFileCtrl;
    REQUIRE(browser->Create(fixture.frame.get(), wxID_ANY, fixture.files.GetName(),
        wxString(), "Text|*.txt|Logs|*.log", wxFC_OPEN));
    browser->ChangeToListMode();
    fixture.Show(browser);
    auto* const list = browser->GetFileList();
    REQUIRE(list);
    C::ListViewItem cell{nullptr};
    REQUIRE(WaitBrowser([&]
    {
        const long row = list->FindItem(-1, filename);
        if ( row == wxNOT_FOUND ) return false;
        cell = BrowserPeerWithId<C::ListViewItem>(browser,
            wxString::Format("wxList.Item.%ld.0", row));
        const auto label = FindBrowserPeer<C::TextBlock>(cell,
            [](const C::TextBlock&) { return true; });
        return BrowserLoaded(cell) && BrowserLoaded(label) && BrowserText(cell) == filename;
    }));
    CHECK_FALSE(cell.try_as<C::Primitives::ToggleButton>());
    const auto text = FindBrowserPeer<C::TextBlock>(cell,
        [](const C::TextBlock&) { return true; });
    REQUIRE(BrowserLoaded(text));
    C::TextBlock measured;
    measured.Text(text.Text());
    measured.FontFamily(text.FontFamily());
    measured.FontSize(text.FontSize());
    measured.FontWeight(text.FontWeight());
    measured.FontStyle(text.FontStyle());
    measured.FontStretch(text.FontStretch());
    measured.Measure({std::numeric_limits<float>::infinity(),
                      std::numeric_limits<float>::infinity()});
    INFO("filename native width=" << text.ActualWidth()
         << " natural=" << measured.DesiredSize().Width);
    CHECK(text.ActualWidth() + 0.5 >= measured.DesiredSize().Width);
    CHECK_FALSE(text.IsTextTrimmed());
    const auto selection = BrowserProvider<A::Provider::ISelectionItemProvider>(
        cell, A::Peers::PatternInterface::SelectionItem);
    REQUIRE(selection);
    REQUIRE(selection.SelectionContainer());
    unsigned selected = 0;
    browser->Bind(wxEVT_FILECTRL_SELECTIONCHANGED, [&](wxFileCtrlEvent& event)
    {
        CHECK(event.GetEventObject() == browser);
        ++selected;
    });
    selection.Select();
    REQUIRE(WaitBrowser([&] { return browser->GetFilename() == filename; }));
    CHECK(selected == 1);
    CHECK(selection.IsSelected());
    browser->SetFilterIndex(1);
    REQUIRE(WaitBrowser([&] { return list->FindItem(-1, filename) == wxNOT_FOUND && !cell.IsLoaded(); }));
    const unsigned eventsAfterFilter = selected;
    // Changing the filter preserves the editable filename (see the generic
    // DoSetFilterIndex contract). A retired item must not change that value
    // or select a replacement row occupying the same index.
    const wxString filenameAfterFilter = browser->GetFilename();
    const long selectionsAfterFilter = list->GetSelectedItemCount();
    CallRetiredBrowserProvider([&] { selection.Select(); });
    CHECK(selected == eventsAfterFilter);
    CHECK(browser->GetFilename() == filenameAfterFilter);
    CHECK(list->GetSelectedItemCount() == selectionsAfterFilter);
}

TEST_CASE("wxWinUI public FileCtrl presents real list peers through modes filters and resize",
          "[winui-file-browsers][winui-filectrl]")
{
    for ( const bool rtl : {false, true} )
    {
        CAPTURE(rtl);
        BrowserFixture fixture(rtl);
        auto* const browser = new wxFileCtrl;
        REQUIRE(browser->Create(fixture.frame.get(), wxID_ANY, fixture.files.GetName(),
            wxString(), "Text|*.txt|Logs|*.log", wxFC_OPEN));
        fixture.Show(browser);
        auto* const list = browser->GetFileList();
        REQUIRE(list);
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(FileCell(browser, "alpha.txt")); }));
        CHECK(list->FindItem(-1, "beta.log") == wxNOT_FOUND);
        CHECK(list->FindItem(-1, "child") != wxNOT_FOUND);

        const auto initial = FileCell(browser, "alpha.txt");
        REQUIRE(initial.try_as<C::ListViewItem>());
        const auto selection = BrowserProvider<A::Provider::ISelectionItemProvider>(
            initial, A::Peers::PatternInterface::SelectionItem);
        REQUIRE(selection);
        int selected = 0;
        browser->Bind(wxEVT_FILECTRL_SELECTIONCHANGED, [&](wxFileCtrlEvent& event)
        {
            CHECK(event.GetEventObject() == browser);
            ++selected;
        });
        selection.Select();
        REQUIRE(WaitBrowser([&] { return browser->GetFilename() == "alpha.txt"; }));
        CHECK(selected == 1);

        browser->ChangeToReportMode();
        REQUIRE(WaitBrowser([&]
        {
            const auto current = FileCell(browser, "alpha.txt");
            const long row = list->FindItem(-1, "alpha.txt");
            const auto nativeRow = BrowserPeerWithId<C::ListViewItem>(browser,
                wxString::Format("wxList.Row.%ld", row));
            return BrowserLoaded(current) && BrowserLoaded(nativeRow) &&
                   !current.try_as<C::Primitives::ToggleButton>() &&
                   BrowserLoaded(BrowserPeerWithId<C::Button>(browser, "wxList.Header.0"));
        }));
        const auto reportCell = FileCell(browser, "alpha.txt");
        for ( const wxSize size : {wxSize(920, 570), wxSize(760, 470)} )
        {
            fixture.frame->SetClientSize(size);
            REQUIRE(WaitBrowser([&]
            {
                const auto current = FileCell(browser, "alpha.txt");
                return BrowserLoaded(current) && current == reportCell &&
                    std::abs(current.ActualWidth() * browser->GetDPIScaleFactor() -
                             list->GetColumnWidth(0)) <= 2;
            }));
            CHECK(FileCell(browser, "alpha.txt") == reportCell);
            CHECK(list->GetClientSize().x > 0);
        }

        browser->SetFilterIndex(1);
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(FileCell(browser, "beta.log")); }));
        CHECK(list->FindItem(-1, "alpha.txt") == wxNOT_FOUND);
        CHECK(browser->GetFilterIndex() == 1);
        browser->SetFilterIndex(0);
        REQUIRE(browser->SetDirectory(fixture.Path("child")));
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(FileCell(browser, "nested.txt")); }));
        REQUIRE(browser->SetFilename("nested.txt"));
        CHECK(wxFileName(browser->GetPath()).SameAs(fixture.Path("child/nested.txt")));
        browser->GoToParentDir();
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(FileCell(browser, "alpha.txt")); }));
        CHECK(wxFileName(browser->GetDirectory()).SameAs(fixture.files.GetName()));
        browser->ChangeToListMode();
        REQUIRE(WaitBrowser([&]
        {
            const auto current = FileCell(browser, "alpha.txt");
            return BrowserLoaded(current) && current.try_as<C::ListViewItem>();
        }));
    }
}

TEST_CASE("wxWinUI public DirCtrl presents real tree peers through filters and resize",
          "[winui-file-browsers][winui-dirctrl]")
{
    for ( const bool rtl : {false, true} )
    {
        CAPTURE(rtl);
        BrowserFixture fixture(rtl);
        auto* const browser = new wxGenericDirCtrl;
        REQUIRE(browser->Create(fixture.frame.get(), wxID_ANY, fixture.files.GetName(),
            wxDefaultPosition, wxDefaultSize,
            wxDIRCTRL_DEFAULT_STYLE | wxDIRCTRL_SHOW_FILTERS,
            "Text|*.txt|Logs|*.log"));
        fixture.Show(browser);
        auto* const tree = browser->GetTreeCtrl();
        REQUIRE(tree);
        const auto directory = tree->GetSelection();
        REQUIRE(directory.IsOk());
        const auto alpha = DirectoryChild(tree, directory, "alpha.txt");
        REQUIRE(alpha.IsOk());
        CHECK_FALSE(DirectoryChild(tree, directory, "beta.log").IsOk());
        tree->EnsureVisible(alpha);
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(DirectoryCell(browser, "alpha.txt")); }));
        const auto nativeTree = FindBrowserPeer<C::TreeView>(BrowserNativeTree(browser),
            [](const C::TreeView&) { return true; });
        REQUIRE(BrowserLoaded(nativeTree));
        CHECK(nativeTree.FlowDirection() ==
              (rtl ? X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight));
        const auto row = DirectoryCell(browser, "alpha.txt");
        const auto selection = BrowserProvider<A::Provider::ISelectionItemProvider>(
            row, A::Peers::PatternInterface::SelectionItem);
        REQUIRE(selection);
        int selected = 0;
        wxString selectionTrace;
        browser->Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, [&](wxTreeEvent& event)
        {
            CHECK(event.GetEventObject() == browser);
            selectionTrace += wxString::Format("[%d item=%p old=%p path=%s current=%p] ",
                selected, event.GetItem().GetID(), event.GetOldItem().GetID(),
                event.GetItem().IsOk() ? browser->GetPath(event.GetItem()) : "<empty>",
                tree->GetSelection().GetID());
            ++selected;
        });
        selection.Select();
        REQUIRE(WaitBrowser([&] { return tree->GetSelection() == alpha; }));
        INFO(selectionTrace);
        CHECK(selected == 1);
        CHECK(wxFileName(browser->GetFilePath()).SameAs(fixture.Path("alpha.txt")));

        for ( const wxSize size : {wxSize(920, 570), wxSize(760, 470)} )
        {
            fixture.frame->SetClientSize(size);
            REQUIRE(WaitBrowser([&]
            {
                return BrowserLoaded(nativeTree) &&
                    std::abs(nativeTree.ActualWidth() * tree->GetDPIScaleFactor() -
                             tree->GetClientSize().x) <= 2 &&
                    std::abs(nativeTree.ActualHeight() * tree->GetDPIScaleFactor() -
                             tree->GetClientSize().y) <= 2;
            }));
            CHECK(DirectoryCell(browser, "alpha.txt") == row);
        }

        browser->SetPath(fixture.files.GetName());
        browser->SetFilterIndex(1);
        const auto logDirectory = tree->GetSelection();
        REQUIRE(logDirectory.IsOk());
        const auto beta = DirectoryChild(tree, logDirectory, "beta.log");
        REQUIRE(beta.IsOk());
        CHECK_FALSE(DirectoryChild(tree, logDirectory, "alpha.txt").IsOk());
        tree->EnsureVisible(beta);
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(DirectoryCell(browser, "beta.log")); }));
        CHECK(browser->GetFilterListCtrl()->GetSelection() == 1);
        browser->SetFilterIndex(0);
        REQUIRE(browser->ExpandPath(fixture.Path("child")));
        const auto nested = DirectoryChild(tree, tree->GetSelection(), "nested.txt");
        REQUIRE(nested.IsOk());
        tree->EnsureVisible(nested);
        REQUIRE(WaitBrowser([&] { return BrowserLoaded(DirectoryCell(browser, "nested.txt")); }));
    }
}

TEST_CASE("wxWinUI file browser native selection can destroy its composite",
          "[winui-file-browsers][winui-filectrl][lifetime]")
{
    BrowserFixture fixture;
    auto* const browser = new wxFileCtrl(fixture.frame.get(), wxID_ANY,
        fixture.files.GetName(), wxString(), "Text|*.txt", wxFC_OPEN);
    fixture.Show(browser);
    REQUIRE(WaitBrowser([&] { return BrowserLoaded(FileCell(browser, "alpha.txt")); }));
    const auto row = FileCell(browser, "alpha.txt");
    REQUIRE(row.try_as<C::ListViewItem>());
    const auto provider = BrowserProvider<A::Provider::ISelectionItemProvider>(
        row, A::Peers::PatternInterface::SelectionItem);
    REQUIRE(provider);
    wxWeakRef<wxFileCtrl> weak(browser);
    int selected = 0;
    browser->Bind(wxEVT_FILECTRL_SELECTIONCHANGED, [&](wxFileCtrlEvent&)
    {
        ++selected;
        delete browser;
    });
    provider.Select();
    REQUIRE(WaitBrowser([&] { return !weak.get() && !row.IsLoaded(); }));
    CHECK(selected == 1);
    CallRetiredBrowserProvider([&] { provider.Select(); });
    CHECK(selected == 1);
}

TEST_CASE("wxWinUI directory browser native selection can destroy its composite",
          "[winui-file-browsers][winui-dirctrl][lifetime]")
{
    BrowserFixture fixture;
    auto* const browser = new wxGenericDirCtrl(fixture.frame.get(), wxID_ANY,
        fixture.files.GetName(), wxDefaultPosition, wxDefaultSize,
        wxDIRCTRL_DEFAULT_STYLE, "Text|*.txt");
    fixture.Show(browser);
    auto* const tree = browser->GetTreeCtrl();
    const auto alpha = DirectoryChild(tree, tree->GetSelection(), "alpha.txt");
    REQUIRE(alpha.IsOk());
    tree->EnsureVisible(alpha);
    REQUIRE(WaitBrowser([&] { return BrowserLoaded(DirectoryCell(browser, "alpha.txt")); }));
    const auto row = DirectoryCell(browser, "alpha.txt");
    const auto provider = BrowserProvider<A::Provider::ISelectionItemProvider>(
        row, A::Peers::PatternInterface::SelectionItem);
    REQUIRE(provider);
    wxWeakRef<wxGenericDirCtrl> weak(browser);
    int selected = 0;
    browser->Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, [&](wxTreeEvent& event)
    {
        CHECK(event.GetItem() == alpha);
        ++selected;
        delete browser;
    });
    provider.Select();
    REQUIRE(WaitBrowser([&] { return !weak.get() && !row.IsLoaded(); }));
    CHECK(selected == 1);
    CallRetiredBrowserProvider([&] { provider.Select(); });
    CHECK(selected == 1);
}

TEST_CASE("wxWinUI native single tree selection transfers atomically and retains real removals",
          "[winui-file-browsers][winui-treemodel][native-single-selection]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeTreeSelectionFixture fixture(rtl);
    auto* const tree = fixture.tree;
    const auto firstNode = fixture.native.SelectedNode();
    const auto secondRow = fixture.Cell("Second");
    const auto secondNode = fixture.native.NodeFromContainer(secondRow);
    const auto secondProvider = fixture.Provider("Second");
    using Transition = std::pair<wxTreeItemId, wxTreeItemId>;
    std::vector<Transition> changing, changed;
    bool veto = false;
    bool pumpDuringChanging = false;
    tree->Bind(wxEVT_TREE_SEL_CHANGING, [&](wxTreeEvent& event)
    {
        CHECK(tree->GetSelection() == event.GetOldItem());
        changing.emplace_back(event.GetOldItem(), event.GetItem());
        if ( veto ) event.Veto();
        if ( pumpDuringChanging && event.GetItem() == fixture.third )
            fixture.DrainSelection();
    });
    tree->Bind(wxEVT_TREE_SEL_CHANGED, [&](wxTreeEvent& event)
    {
        CHECK(tree->GetSelection() == event.GetItem());
        changed.emplace_back(event.GetOldItem(), event.GetItem());
    });

    // Real UIA Select traverses WinUI's Clear()+Append() implementation.
    secondProvider.Select();
    REQUIRE(WaitBrowser([&] { return tree->GetSelection() == fixture.second; }));
    fixture.DrainSelection();
    REQUIRE(changing.size() == 1);
    REQUIRE(changed.size() == 1);
    CHECK(changing.front() == Transition(fixture.first, fixture.second));
    CHECK(changed.front() == changing.front());

    // Programmatic wx selection remains synchronous and emits its one pair.
    tree->SelectItem(fixture.first);
    REQUIRE(tree->GetSelection() == fixture.first);
    REQUIRE(changed.size() == 2);
    CHECK(changed.back() == Transition(fixture.second, fixture.first));

    veto = true;
    secondProvider.Select();
    REQUIRE(WaitBrowser([&] { return fixture.native.SelectedNode() == firstNode; }));
    fixture.DrainSelection();
    CHECK(tree->GetSelection() == fixture.first);
    REQUIRE(changing.size() == 3);
    CHECK(changing.back() == Transition(fixture.first, fixture.second));
    CHECK(changed.size() == 2);

    veto = false;
    secondProvider.Select();
    REQUIRE(WaitBrowser([&] { return tree->GetSelection() == fixture.second; }));
    fixture.DrainSelection();
    changing.clear();
    changed.clear();

    // A removal with no following append is not discarded, and can be vetoed.
    veto = true;
    secondProvider.RemoveFromSelection();
    REQUIRE(WaitBrowser([&]
    {
        return changing.size() == 1 && fixture.native.SelectedNode() == secondNode;
    }));
    CHECK(changing.front() == Transition(fixture.second, wxTreeItemId()));
    CHECK(tree->GetSelection() == fixture.second);
    CHECK(changed.empty());

    veto = false;
    secondProvider.RemoveFromSelection();
    REQUIRE(WaitBrowser([&] { return !tree->GetSelection().IsOk(); }));
    fixture.DrainSelection();
    REQUIRE(changing.size() == 2);
    REQUIRE(changed.size() == 1);
    CHECK(changed.front() == Transition(fixture.second, wxTreeItemId()));
    CHECK_FALSE(fixture.native.SelectedNode());

    // A newer wx writer supersedes the queued native removal, not vice versa.
    tree->SelectItem(fixture.second);
    changing.clear();
    changed.clear();
    secondProvider.RemoveFromSelection();
    pumpDuringChanging = true;
    tree->SelectItem(fixture.third);
    REQUIRE(tree->GetSelection() == fixture.third);
    fixture.DrainSelection();
    REQUIRE(changed.size() == 1);
    CHECK(changed.front() == Transition(fixture.second, fixture.third));
    CHECK(tree->GetSelection() == fixture.third);
}

TEST_CASE("wxWinUI native single tree selection survives retirement and reentrant writers",
          "[winui-file-browsers][winui-treemodel][native-single-selection][lifetime]")
{
    const int action = GENERATE(0, 1, 2, 3);
    CAPTURE(action);
    NativeTreeSelectionFixture fixture;
    auto* const tree = fixture.tree;
    const auto provider = fixture.Provider("Second");
    const auto row = fixture.Cell("Second");
    wxWeakRef<wxTreeCtrl> weak(tree);
    int changing = 0;
    int changed = 0;

    if ( action < 2 )
    {
        tree->Bind(wxEVT_TREE_SEL_CHANGING, [&](wxTreeEvent& event)
        {
            ++changing;
            CHECK(event.GetOldItem() == fixture.first);
            CHECK(event.GetItem() == fixture.second);
            if ( action == 0 )
                delete tree;
            else
                tree->Delete(fixture.second);
        });
        tree->Bind(wxEVT_TREE_SEL_CHANGED, [&](wxTreeEvent&) { ++changed; });
        provider.Select();
        fixture.DrainSelection();
        CHECK(changing == 1);
        CHECK(changed == 0);
        if ( action == 0 )
            CHECK_FALSE(weak.get());
        else
            CHECK(tree->GetSelection() == fixture.first);
    }
    else
    {
        provider.Select();
        REQUIRE(WaitBrowser([&] { return tree->GetSelection() == fixture.second; }));
        fixture.DrainSelection();
        tree->Bind(wxEVT_TREE_SEL_CHANGING, [&](wxTreeEvent&) { ++changing; });
        tree->Bind(wxEVT_TREE_SEL_CHANGED, [&](wxTreeEvent&) { ++changed; });
        provider.RemoveFromSelection();
        if ( action == 2 )
        {
            tree->Delete(fixture.second);
            const auto replacement = tree->AppendItem(fixture.root, "Second");
            REQUIRE(replacement != fixture.second);
            tree->SelectItem(replacement);
            const int changingAfterWrite = changing;
            const int changedAfterWrite = changed;
            fixture.DrainSelection();
            CHECK(tree->GetSelection() == replacement);
            CHECK(changing == changingAfterWrite);
            CHECK(changed == changedAfterWrite);
        }
        else
        {
            delete tree;
            fixture.DrainSelection();
            CHECK_FALSE(weak.get());
            CHECK(changing == 0);
            CHECK(changed == 0);
        }
    }

    REQUIRE(WaitBrowser([&] { return !row.IsLoaded(); }));
    const int finalChanging = changing;
    const int finalChanged = changed;
    CallRetiredBrowserProvider([&] { provider.Select(); });
    fixture.DrainSelection();
    CHECK(changing == finalChanging);
    CHECK(changed == finalChanged);
}

#endif // WinUI native file/directory browsers
