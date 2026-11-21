/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuishowcaselayout.cpp
// Purpose:     Showcase scrolling after content publication and pane changes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_NOTEBOOK && \
    wxUSE_COLLPANE && wxUSE_STATBOX && wxUSE_BUTTON

#include "wx/app.h"
#include "wx/button.h"
#include "wx/collheaderctrl.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/sizer.h"
#include "wx/statbox.h"
#include "wx/stopwatch.h"
#include "wx/winui/private/tlwhost.h"
#include "../../samples/showcase/scrolledpage.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <cstdint>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace P = X::Controls::Primitives;

template<typename Predicate>
bool WaitShowcaseLayout(Predicate predicate)
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

wxPoint ShowcaseLayoutPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

P::ScrollBar FindShowcaseScrollBar(const X::DependencyObject& node,
                                  const winrt::hstring& id)
{
    if ( !node ) return nullptr;
    if ( const auto bar = node.try_as<P::ScrollBar>(); bar &&
         X::Automation::AutomationProperties::GetAutomationId(bar) == id ) return bar;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( const auto bar = FindShowcaseScrollBar(
                X::Media::VisualTreeHelper::GetChild(node, i), id) ) return bar;
    return nullptr;
}

struct ShowcaseLayoutFixture
{
    ShowcaseLayoutFixture()
    {
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
            "Showcase scrolling lifecycle", ShowcaseLayoutPosition(), requestedFrameSize);
        book = new wxNotebook(frame.get(), wxID_ANY);
        auto* const frameSizer = new wxBoxSizer(wxVERTICAL);
        frameSizer->Add(book, 1, wxEXPAND);
        frame->SetSizer(frameSizer);
        page = new ShowcaseScrolledPage(book);
        contents = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(contents);
    }

    void Show()
    {
        REQUIRE(book->AddPage(page, "Overview", true));
        frame->Layout();
        frame->ShowWithoutActivating();
        // The native host settles its non-client metrics on first load. Do
        // not capture the pre-Show provisional frame/client size as the
        // baseline for a subsequent content-only change. No resize is issued
        // here: the original constructor allocation must be reached itself.
        REQUIRE(WaitShowcaseLayout([&]
        {
            auto* const host = wxWinUITopLevelHost::ForWindow(page, false);
            const auto root = host ? host->GetXamlRoot() : nullptr;
            const auto content = root ? root.Content().try_as<X::FrameworkElement>() : nullptr;
            return frame->GetSize() == requestedFrameSize && content &&
                   content.IsLoaded() && content.ActualHeight() > 0 &&
                   page->GetClientSize().y > 100;
        }));
    }

    P::ScrollBar ScrollBar() const
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(page, false);
        if ( !host || !host->GetXamlRoot() ) return nullptr;
        const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(page->GetHandle()))).wc_str());
        return FindShowcaseScrollBar(host->GetXamlRoot().Content(), id);
    }

    bool HasNativeScrollRange() const
    {
        const auto bar = ScrollBar();
        return page->GetScrollRange(wxVERTICAL) > page->GetScrollThumb(wxVERTICAL) &&
               bar && bar.IsLoaded() && bar.ActualHeight() > 0 && bar.Maximum() > 0;
    }

    const wxSize requestedFrameSize{640, 400};
    std::unique_ptr<wxFrame> frame;
    wxNotebook* book = nullptr;
    ShowcaseScrolledPage* page = nullptr;
    wxBoxSizer* contents = nullptr;
};
}

TEST_CASE("Showcase first page publishes its scroll extent without a resize",
          "[winui-showcase-layout][winui-window-scrollbar]")
{
    const bool populateAfterShow = GENERATE(false, true);
    CAPTURE(populateAfterShow);
    ShowcaseLayoutFixture fixture;
    if ( populateAfterShow ) fixture.Show();
    const wxSize frameSize = fixture.requestedFrameSize;
    for ( int i = 0; i < 12; ++i )
    {
        auto* const button = new wxButton(fixture.page, wxID_ANY,
            wxString::Format("Overview control %d", i));
        button->SetMinSize(fixture.page->FromDIP(wxSize(180, 48)));
        fixture.contents->Add(button, 0, wxEXPAND | wxALL, 4);
    }
    fixture.page->FinalizeContent();
    if ( !populateAfterShow ) fixture.Show();
    REQUIRE(WaitShowcaseLayout([&] { return fixture.HasNativeScrollRange(); }));
    CHECK(fixture.frame->GetSize() == frameSize);
    CHECK(fixture.page->GetVirtualSize().y >= fixture.contents->CalcMin().y);
    CHECK(fixture.page->GetVirtualSize().y > fixture.page->GetClientSize().y);
}

TEST_CASE("Showcase collapsible content updates only the containing page extent",
          "[winui-showcase-layout][winui-window-scrollbar]")
{
    ShowcaseLayoutFixture fixture;
    auto* const group = new wxStaticBoxSizer(wxVERTICAL, fixture.page, "Collapsible pane");
    fixture.contents->Add(group, 0, wxEXPAND | wxALL, 8);
    auto* const pane = new wxCollapsiblePane(group->GetStaticBox(), wxID_ANY,
        "Expandable details", wxDefaultPosition, wxDefaultSize,
        wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE);
    group->Add(pane, 0, wxEXPAND | wxALL, 8);
    auto* const inside = new wxBoxSizer(wxVERTICAL);
    auto* const detail = new wxButton(pane->GetPane(), wxID_ANY, "Full expanded content");
    detail->SetMinSize(fixture.page->FromDIP(wxSize(200, 520)));
    inside->Add(detail, 0, wxEXPAND | wxALL, 8);
    pane->GetPane()->SetSizer(inside);
    auto* const footer = new wxButton(fixture.page, wxID_ANY, "After the pane");
    fixture.contents->Add(footer, 0, wxEXPAND | wxALL, 8);
    fixture.page->FinalizeContent();
    fixture.Show();
    REQUIRE(pane->IsCollapsed());
    const wxSize collapsedExtent = fixture.page->GetVirtualSize();
    const wxRect frameRect = fixture.frame->GetRect();
    const wxSize pageSize = fixture.page->GetSize();
    auto* const header = wxWindow::FindWindowByName(wxCollapsibleHeaderCtrlNameStr, pane);
    REQUIRE(header);
    int changes = 0;
    fixture.page->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [&](wxCollapsiblePaneEvent& event)
    {
        CHECK(event.GetEventObject() == pane);
        ++changes;
        event.Skip();
    });

    // Send a native character to the real generic header on the private
    // desktop. This runs OnChar->Collapse->CHANGED without physical input or
    // fabricating the notification after manually changing the pane state.
    ::SendMessageW(static_cast<HWND>(header->GetHandle()), WM_CHAR, VK_SPACE, 1);
    REQUIRE(pane->IsExpanded());
    REQUIRE(WaitShowcaseLayout([&] { return fixture.HasNativeScrollRange(); }));
    CHECK(changes == 1);
    CHECK(fixture.frame->GetRect() == frameRect);
    CHECK(fixture.page->GetSize() == pageSize);
    CHECK(fixture.page->GetVirtualSize().y > collapsedExtent.y);
    CHECK(fixture.page->GetVirtualSize().y >= fixture.contents->CalcMin().y);
    CHECK(pane->GetSize().y >= pane->GetBestSize().y);
    CHECK(fixture.page->CalcUnscrolledPosition(footer->GetPosition()).y +
          footer->GetSize().y <= fixture.page->GetVirtualSize().y);

    ::SendMessageW(static_cast<HWND>(header->GetHandle()), WM_CHAR, VK_SPACE, 1);
    REQUIRE(pane->IsCollapsed());
    const bool collapsedLayout = WaitShowcaseLayout([&]
    {
        const auto bar = fixture.ScrollBar();
        // MSW reports an inclusive range of one for nMin==nMax==0 even
        // after SetScrollbar(0, 0, 0) has removed the scrollbar. Inspect
        // actual native visibility instead of comparing that sentinel to
        // the zero-sized page. The same extent and allocation remain exact.
        return fixture.page->GetVirtualSize().y == collapsedExtent.y &&
               !(::GetWindowLongPtrW(static_cast<HWND>(fixture.page->GetHandle()),
                                     GWL_STYLE) & WS_VSCROLL) &&
               (!bar || !bar.IsLoaded() || bar.Visibility() == X::Visibility::Collapsed);
    });
    INFO("collapsed extent initial=" << collapsedExtent << " current=" << fixture.page->GetVirtualSize()
         << " client=" << fixture.page->GetClientSize() << " minimum=" << fixture.contents->CalcMin()
         << " range=" << fixture.page->GetScrollRange(wxVERTICAL)
         << " thumb=" << fixture.page->GetScrollThumb(wxVERTICAL)
         << " pane=" << pane->GetSize() << " pane best=" << pane->GetBestSize());
    REQUIRE(collapsedLayout);
    CHECK(changes == 2);
    CHECK(fixture.frame->GetRect() == frameRect);
    CHECK(fixture.page->GetSize() == pageSize);
}

#endif // WinUI Showcase scrolling
