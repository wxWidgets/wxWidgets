/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitoolbook.cpp
// Purpose:     Public wxToolbook contracts through actual WinUI toolbar peers
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TOOLBOOK

#include "wx/app.h"
#include "wx/dcmemory.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/imaglist.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stopwatch.h"
#include "wx/toolbook.h"
#include "wx/toolbar.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <cmath>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitToolbook(Predicate predicate)
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

wxPoint ToolbookPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

template<typename T>
T FindBookElement(const X::DependencyObject& node, const wxString& name = {})
{
    if ( !node ) return nullptr;
    if ( const auto element = node.try_as<T>(); element &&
         (name.empty() || wxString(A::AutomationProperties::GetName(element).c_str()) == name) )
        return element;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( const auto found = FindBookElement<T>(
                X::Media::VisualTreeHelper::GetChild(node, i), name) ) return found;
    return nullptr;
}

// The button of the page of this name: the tool of a page is a radio tool,
// which is an AppBarToggleButton whose label is the name.
C::AppBarToggleButton FindBookTool(const X::DependencyObject& node,
                                   const wxString& name)
{
    if ( !node ) return nullptr;
    if ( const auto tool = node.try_as<C::AppBarToggleButton>();
         tool && wxString(tool.Label().c_str()) == name )
        return tool;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( const auto found = FindBookTool(
                X::Media::VisualTreeHelper::GetChild(node, i), name) ) return found;
    return nullptr;
}

C::AppBarToggleButton BookRadio(wxToolbook* book, const wxString& name)
{
    auto* const toolbar = book ? book->GetToolBar() : nullptr;
    auto* const host = toolbar ? wxWinUITopLevelHost::FindSlotOwner(toolbar) : nullptr;
    auto* const slot = host ? host->FindSlot(toolbar) : nullptr;
    return slot ? FindBookTool(slot->GetContent(), name) : nullptr;
}

bool RadioSelected(const C::AppBarToggleButton& radio)
{
    const auto checked = radio ? radio.IsChecked() : nullptr;
    return checked && checked.Value();
}

// What a screen reader uses to choose the page.
A::Provider::IToggleProvider BookSelection(const C::AppBarToggleButton& radio)
{
    const auto peer = radio
        ? A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(radio) : nullptr;
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Toggle)
        .try_as<A::Provider::IToggleProvider>() : nullptr;
}

wxRect BookNativeScreenRect(wxToolbook* book, const X::FrameworkElement& element)
{
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(book->GetToolBar());
    if ( !host || !element || !element.IsLoaded() ) return {};
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

void SelectRetiredBookPeer(const A::Provider::IToggleProvider& provider)
{
    try
    {
        provider.Toggle();
    }
    catch ( const winrt::hresult_error& error )
    {
        const auto code = static_cast<int32_t>(error.code());
        CHECK((code == static_cast<int32_t>(0x80040200u) ||
               code == static_cast<int32_t>(0x80040201u)));
    }
}

class NativeToolbook final : public wxToolbook
{
public:
    using wxToolbook::wxToolbook;
    using wxBookCtrlBase::GetPageRect;
};

struct NativeToolbookFixture
{
    explicit NativeToolbookFixture(long alignment = wxBK_TOP, bool rtl = false,
                                   bool duplicateIds = false)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "native public wxToolbook",
                            ToolbookPosition(), wxSize(640, 400));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        book = new NativeToolbook(frame, wxID_ANY, wxDefaultPosition,
                              wxDefaultSize, alignment);
        book->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        auto* const images = new wxImageList(16, 16);
        wxBitmap bitmap(16, 16, 32);
        wxMemoryDC dc(bitmap);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
        REQUIRE(images->Add(bitmap) == 0);
        book->AssignImageList(images);
        for ( int i = 0; i < 3; ++i )
            REQUIRE(book->AddPage(new wxPanel(book, duplicateIds ? 7841 : wxID_ANY),
                wxString::Format("Page %d", i), i == 0, 0));
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(book, 1, wxEXPAND);
        frame->SetSizer(sizer);
        frame->Layout();
        book->Realize();
        frame->ShowWithoutActivating();
        REQUIRE(WaitToolbook([&]() { return Ready(); }));
    }
    ~NativeToolbookFixture()
    {
        delete frame;
        wxTheApp->ProcessPendingEvents();
    }
    bool Ready() const
    {
        for ( int i = 0; i < 3; ++i )
        {
            const auto radio = BookRadio(book, wxString::Format("Page %d", i));
            if ( !radio || !radio.IsLoaded() || radio.ActualWidth() <= 0 ||
                 radio.ActualHeight() <= 0 ) return false;
        }
        // HWND layout changes synchronously, but the island arranges on its
        // dispatcher. IsLoaded alone still describes the previous position
        // after a bottom/right-aligned toolbar moves during frame resize.
        auto* const toolbar = book->GetToolBar();
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(toolbar);
        auto* const slot = host ? host->FindSlot(toolbar) : nullptr;
        const auto root = slot ? slot->GetContent().try_as<X::FrameworkElement>() : nullptr;
        if ( !root ) return false;
        const wxRect visual = BookNativeScreenRect(book, root);
        RECT client{};
        if ( !::GetClientRect(static_cast<HWND>(toolbar->GetHandle()), &client) ) return false;
        ::MapWindowPoints(static_cast<HWND>(toolbar->GetHandle()), nullptr,
                          reinterpret_cast<POINT*>(&client), 2);
        return std::abs(visual.x - client.left) <= 1 &&
               std::abs(visual.y - client.top) <= 1 &&
               std::abs(visual.width - (client.right - client.left)) <= 1 &&
               std::abs(visual.height - (client.bottom - client.top)) <= 1;
    }
    wxFrame* frame = nullptr;
    NativeToolbook* book = nullptr;
};
}

TEST_CASE("WinUIToolbook::NativeLayoutAndPositionalHitTest",
          "[toolbook][winui-toolbook]")
{
    const bool rtl = GENERATE(false, true);
    const long alignment = GENERATE(wxBK_TOP, wxBK_BOTTOM, wxBK_LEFT, wxBK_RIGHT);
    CAPTURE(rtl, alignment);
    NativeToolbookFixture fixture(alignment, rtl, true);
    auto* const book = fixture.book;
    auto* const toolbar = book->GetToolBar();
    const auto checkLayout = [&](const char* phase)
    {
        CAPTURE(phase);
        REQUIRE(fixture.Ready());
        const wxRect pageRect = book->GetPageRect();
        REQUIRE(!pageRect.IsEmpty());
        CHECK(book->GetPage(0)->GetRect() == pageRect);
        CHECK(!pageRect.Intersects(toolbar->GetRect()));
        for ( int i = 0; i < 3; ++i )
        {
            CAPTURE(i);
            const auto radio = BookRadio(book, wxString::Format("Page %d", i));
            REQUIRE(FindBookElement<C::TextBlock>(radio));
            const wxRect nativeRect = BookNativeScreenRect(book, radio);
            REQUIRE(!nativeRect.IsEmpty());
            const wxPoint screen(nativeRect.x + nativeRect.width / 2,
                                 nativeRect.y + nativeRect.height / 2);
            const wxPoint toolbarPoint = toolbar->ScreenToClient(screen);
            const wxPoint bookPoint = book->ScreenToClient(screen);
            const wxPoint roundTrip = toolbar->ScreenToClient(book->ClientToScreen(bookPoint));
            CAPTURE(nativeRect, toolbarPoint, bookPoint, roundTrip, toolbar->GetRect());
            CHECK(toolbar->FindToolForPosition(toolbarPoint.x, toolbarPoint.y) ==
                  toolbar->GetToolByPos(static_cast<size_t>(i)));
            long flags = 0;
            CHECK(book->HitTest(book->ScreenToClient(screen), &flags) == i);
            CHECK((flags & (wxBK_HITTEST_ONICON | wxBK_HITTEST_ONLABEL)) != 0);
        }
    };
    checkLayout("initial");
    const wxSize oldSize = book->GetSize();
    fixture.frame->SetClientSize(760, 470);
    REQUIRE(WaitToolbook([&]()
    {
        return book->GetSize() != oldSize && fixture.Ready() &&
               book->GetPage(0)->GetRect() == book->GetPageRect();
    }));
    checkLayout("resized");
}

TEST_CASE("WinUIToolbook::NativeSelectionVetoAndSilentUpdates",
          "[toolbook][winui-toolbook]")
{
    const bool rtl = GENERATE(false, true);
    const bool duplicateIds = GENERATE(false, true);
    CAPTURE(rtl, duplicateIds);
    NativeToolbookFixture fixture(wxBK_TOP, rtl, duplicateIds);
    auto* const book = fixture.book;
    int changing = 0;
    int changed = 0;
    bool veto = true;
    book->Bind(wxEVT_TOOLBOOK_PAGE_CHANGING, [&](wxBookCtrlEvent& event)
    {
        CHECK(event.GetEventObject() == book);
        CHECK(event.GetOldSelection() == 0);
        CHECK(event.GetSelection() == 1);
        ++changing;
        if ( veto ) event.Veto();
    });
    book->Bind(wxEVT_TOOLBOOK_PAGE_CHANGED, [&](wxBookCtrlEvent& event)
    {
        CHECK(event.GetEventObject() == book);
        CHECK(event.GetSelection() == 1);
        ++changed;
    });
    auto first = BookRadio(book, "Page 0");
    auto second = BookRadio(book, "Page 1");
    const auto select = BookSelection(second);
    REQUIRE(select);
    select.Toggle();
    REQUIRE(WaitToolbook([&]() { return changing == 1; }));
    CHECK(changed == 0);
    CHECK(book->GetSelection() == 0);
    CHECK(RadioSelected(first));
    CHECK(!RadioSelected(second));
    CHECK(book->GetPage(0)->IsShown());
    CHECK(!book->GetPage(1)->IsShown());

    veto = false;
    select.Toggle();
    REQUIRE(WaitToolbook([&]() { return changed == 1; }));
    CHECK(changing == 2);
    CHECK(book->GetSelection() == 1);
    CHECK(!RadioSelected(first));
    CHECK(RadioSelected(second));
    CHECK(!book->GetPage(0)->IsShown());
    CHECK(book->GetPage(1)->IsShown());

    book->ChangeSelection(2);
    REQUIRE(book->SetPageText(2, "Renamed native page"));
    REQUIRE(book->EnablePage(static_cast<size_t>(0), false));
    REQUIRE(WaitToolbook([&]()
    {
        const auto last = BookRadio(book, "Renamed native page");
        return last && last.IsLoaded() && RadioSelected(last);
    }));
    CHECK(changing == 2);
    CHECK(changed == 1);
    CHECK(!BookRadio(book, "Page 0").IsEnabled());
    CHECK(book->GetPage(2)->IsShown());
}

TEST_CASE("WinUIToolbook::NativeProvidersRetireAcrossDeleteAndCallback",
          "[toolbook][winui-toolbook]")
{
    const bool destroyDuringChanging = GENERATE(false, true);
    NativeToolbookFixture fixture;
    auto* const book = fixture.book;
    const auto stale = BookSelection(BookRadio(book, "Page 1"));
    REQUIRE(stale);
    int changed = 0;
    book->Bind(wxEVT_TOOLBOOK_PAGE_CHANGED, [&](wxBookCtrlEvent&) { ++changed; });
    REQUIRE(book->DeletePage(1));
    auto* const replacement = new wxPanel(book);
    REQUIRE(book->InsertPage(1, replacement, "Replacement", false, 0));
    book->Realize();
    REQUIRE(WaitToolbook([&]()
    {
        const auto peer = BookRadio(book, "Replacement");
        return peer && peer.IsLoaded() && peer.ActualWidth() > 0;
    }));
    changed = 0;
    SelectRetiredBookPeer(stale);
    CHECK(changed == 0);
    CHECK(book->GetSelection() == 0);

    const auto current = BookSelection(BookRadio(book, "Replacement"));
    REQUIRE(current);
    wxWeakRef<wxToolbook> weak(book);
    int callbacks = 0;
    book->Bind(destroyDuringChanging ? wxEVT_TOOLBOOK_PAGE_CHANGING
                                     : wxEVT_TOOLBOOK_PAGE_CHANGED,
        [&](wxBookCtrlEvent& event)
        {
            CHECK(event.GetSelection() == 1);
            ++callbacks;
            fixture.book = nullptr;
            delete book;
        });
    current.Toggle();
    REQUIRE(WaitToolbook([&]() { return callbacks == 1; }));
    CHECK(!weak);
    SelectRetiredBookPeer(current);
    CHECK(callbacks == 1);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TOOLBOOK
