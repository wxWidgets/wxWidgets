/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuimdi.cpp
// Purpose:     the tabbed MDI of wxWinUI through its real TabView
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_MDI

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/mdi.h"
#include "wx/menu.h"
#include "wx/notebook.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

namespace
{

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitMDI(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;

    wxStopWatch elapsed;
    while ( elapsed.Time() < 2000 )
    {
        if ( predicate() )
            return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        if ( loop->DispatchTimeout(5) == 0 )
            return false;
    }
    return predicate();
}

// On the desktop of the tests, or out of sight on the desktop of a person.
wxPoint MDIPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(
            ::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") )
    {
        return wxPoint(80, 80);
    }

    return wxPoint(-30000, -30000);
}

// The tab of the document of this title.
C::TabViewItem FindTab(const X::DependencyObject& node, const wxString& title)
{
    if ( !node )
        return nullptr;

    if ( const auto tab = node.try_as<C::TabViewItem>();
         tab &&
         wxString(A::AutomationProperties::GetName(tab).c_str()) == title )
    {
        return tab;
    }

    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
    {
        if ( const auto found = FindTab(
                X::Media::VisualTreeHelper::GetChild(node, i), title) )
        {
            return found;
        }
    }

    return nullptr;
}

// The close button which the template of a TabViewItem names so.
C::Button FindCloseButton(const X::DependencyObject& node)
{
    if ( !node )
        return nullptr;

    if ( const auto button = node.try_as<C::Button>();
         button && button.Name() == L"CloseButton" )
    {
        return button;
    }

    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
    {
        if ( const auto found = FindCloseButton(
                X::Media::VisualTreeHelper::GetChild(node, i)) )
        {
            return found;
        }
    }

    return nullptr;
}

C::Button FindTabCloseButton(wxWindow *book, const wxString& title)
{
    auto* const host = book ? wxWinUITopLevelHost::FindSlotOwner(book) : nullptr;
    auto* const slot = host ? host->FindSlot(book) : nullptr;
    const auto tab = slot ? FindTab(slot->GetContent(), title) : nullptr;
    return tab && tab.IsLoaded() ? FindCloseButton(tab) : nullptr;
}

// What a click on the button does, as a screen reader does it.
bool PressCloseButton(wxWindow *book, const wxString& title)
{
    const auto button = FindTabCloseButton(book, title);
    if ( !button )
        return false;

    const auto peer =
        A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(button);
    const auto invoke = peer
        ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
              .try_as<A::Provider::IInvokeProvider>()
        : nullptr;
    if ( !invoke )
        return false;

    invoke.Invoke();
    return true;
}

} // anonymous namespace

TEST_CASE("wxWinUI MDI documents are the closable tabs of a TabView",
          "[winui-mdi]")
{
    wxMDIParentFrame * const parent = new wxMDIParentFrame(
        nullptr, wxID_ANY, "MDI parent", MDIPosition(), wxSize(700, 500));

    wxMenuBar * const parentBar = new wxMenuBar;
    wxMenu * const fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT);
    parentBar->Append(fileMenu, "&File");
    parent->SetMenuBar(parentBar);

    // Programs give the standard menu back after changing its items: this
    // must neither delete it nor lose it.
    wxMenu * const windowMenu = parent->GetWindowMenu();
    REQUIRE(windowMenu);
    parent->SetWindowMenu(windowMenu);
    CHECK(parent->GetWindowMenu() == windowMenu);
    CHECK(windowMenu->FindItem(wxID_MDI_WINDOW_NEXT));
    CHECK(windowMenu->FindItem(wxID_MDI_WINDOW_PREV));

    wxMDIChildFrame * const first =
        new wxMDIChildFrame(parent, wxID_ANY, "First");
    wxMDIChildFrame * const second =
        new wxMDIChildFrame(parent, wxID_ANY, "Second");

    wxMenuBar * const childBar = new wxMenuBar;
    wxMenu * const editMenu = new wxMenu;
    editMenu->Append(wxID_COPY);
    childBar->Append(editMenu, "&Edit");
    second->SetMenuBar(childBar);

    parent->ShowWithoutActivating();

    wxNotebook * const book = wxDynamicCast(parent->GetBookCtrl(), wxNotebook);
    REQUIRE(book);
    CHECK(book->GetPageCount() == 2);
    CHECK(book->GetPageText(0) == "First");
    CHECK(book->GetPageText(1) == "Second");

    // The menu bar of the frame is the one of the active document, and its
    // own again for a document which has none.
    second->Activate();
    CHECK(parent->GetActiveChild() == second);
    CHECK(parent->GetMenuBar() == childBar);
    first->Activate();
    CHECK(parent->GetActiveChild() == first);
    CHECK(parent->GetMenuBar() == parentBar);

    // The title of a document is the text of its tab.
    first->SetTitle("Renamed");
    CHECK(book->GetPageText(0) == "Renamed");

    // The close button of a tab closes the document, which may refuse.
    REQUIRE(WaitMDI([&]()
    {
        return static_cast<bool>(FindTabCloseButton(book, "Renamed"));
    }));

    bool veto = true;
    int closes = 0;
    first->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event)
    {
        ++closes;
        if ( veto && event.CanVeto() )
            event.Veto();
        else
            event.Skip();
    });

    REQUIRE(PressCloseButton(book, "Renamed"));
    REQUIRE(WaitMDI([&]() { return closes == 1; }));
    CHECK(book->GetPageCount() == 2);

    veto = false;
    const wxWeakRef<wxMDIChildFrame> weakFirst(first);
    REQUIRE(PressCloseButton(book, "Renamed"));
    REQUIRE(WaitMDI([&]() { return book->GetPageCount() == 1; }));
    CHECK(closes == 2);
    CHECK(WaitMDI([&]() { return !weakFirst; }));
    CHECK(parent->GetActiveChild() == second);
    CHECK(parent->GetMenuBar() == childBar);

    delete parent;
    wxTheApp->ProcessPendingEvents();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_MDI
