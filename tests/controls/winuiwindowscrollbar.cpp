/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiwindowscrollbar.cpp
// Purpose:     Native WinUI projection of wxWindow HWND scrollbar models
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_BUTTON

#include "wx/app.h"
#include "wx/button.h"
#include "wx/dcclient.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/msw/private.h"
#include "wx/notebook.h"
#include "wx/splitter.h"
#include "wx/stopwatch.h"
#include "wx/scrolwin.h"
#include "wx/sizer.h"
#include "wx/sysopt.h"
#include "wx/window.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "appearance-test-access.h"
#include "host-input-test-access.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace A = X::Automation::Peers;
namespace Provider = X::Automation::Provider;

template <typename Predicate>
bool WaitForWindowScrollbars(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() ) return true;
        if ( elapsed.Time() >= 1000 ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        const long remaining = 1000 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
            return false;
    }
}

bool SettleWindowScrollbars()
{
    wxStopWatch elapsed;
    return WaitForWindowScrollbars([&]() { return elapsed.Time() >= 30; });
}

wxPoint WindowScrollbarTestPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") )
        return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

P::ScrollBar FindWindowScrollbar(const X::DependencyObject& node,
                                const winrt::hstring& id)
{
    if ( !node ) return nullptr;
    if ( auto bar = node.try_as<P::ScrollBar>() )
    {
        if ( X::Automation::AutomationProperties::GetAutomationId(bar) == id )
            return bar;
    }
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( auto bar = FindWindowScrollbar(
                X::Media::VisualTreeHelper::GetChild(node, i), id) )
            return bar;
    return nullptr;
}

X::FrameworkElement FindWindowScrollbarPart(const X::DependencyObject& node,
                                          const winrt::hstring& name)
{
    if ( !node ) return nullptr;
    if ( const auto element = node.try_as<X::FrameworkElement>() )
        if ( element.Name() == name ) return element;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
        if ( auto part = FindWindowScrollbarPart(
                X::Media::VisualTreeHelper::GetChild(node, i), name) )
            return part;
    return nullptr;
}

struct ObservedWindowScroll
{
    wxEventType type;
    int position;
    int orientation;
    wxObject* object;
};

class CountedScrollbarWindow : public wxWindow
{
public:
    using wxWindow::wxWindow;

    unsigned paintMessages = 0;
    unsigned clientPaints = 0;

protected:
    bool MSWHandleMessage(WXLRESULT* result, WXUINT message,
                          WXWPARAM wParam, WXLPARAM lParam) override
    {
        if ( message == WM_PAINT )
        {
            ++paintMessages;
            if ( ::GetUpdateRect(GetHwndOf(this), nullptr, FALSE) )
                ++clientPaints;
        }
        return wxWindow::MSWHandleMessage(result, message, wParam, lParam);
    }
};

// This observer deliberately runs on the actual wxScrolledWindow path, after
// its ScrollWindow() has moved pixels and before a queued host flush can hide
// damage. SetScrollPos() alone does not exercise this contract.
class CountedScrolledScrollbarWindow : public wxScrolledWindow
{
public:
    using wxScrolledWindow::wxScrolledWindow;
    unsigned scrolls = 0, paints = 0, nonClientPaints = 0, wheels = 0;
    std::function<void()> afterScroll;
    std::function<void(WXUINT)> afterNativeGeometry;

    void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr) override
    {
        wxScrolledWindow::ScrollWindow(dx, dy, rect);
        if ( dx || dy )
        {
            ++scrolls;
            if ( afterScroll ) afterScroll();
        }
    }

protected:
    WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override
    {
        const auto result = wxScrolledWindow::MSWWindowProc(message, wParam, lParam);
        if ( afterNativeGeometry &&
             (message == WM_SIZE || message == WM_WINDOWPOSCHANGED) )
            afterNativeGeometry(message);
        return result;
    }

    bool MSWHandleMessage(WXLRESULT* result, WXUINT message,
                          WXWPARAM wParam, WXLPARAM lParam) override
    {
        if ( message == WM_PAINT ) ++paints;
        if ( message == WM_NCPAINT ) ++nonClientPaints;
        if ( message == WM_MOUSEWHEEL || message == WM_MOUSEHWHEEL ) ++wheels;
        return wxScrolledWindow::MSWHandleMessage(result, message, wParam, lParam);
    }
};

std::vector<COLORREF> ReadScrollbarBand(HWND hwnd, bool vertical)
{
    SCROLLBARINFO bar{sizeof(SCROLLBARINFO)};
    REQUIRE(::GetScrollBarInfo(hwnd, vertical ? OBJID_VSCROLL : OBJID_HSCROLL, &bar));
    RECT window{};
    REQUIRE(::GetWindowRect(hwnd, &window));
    RECT band = bar.rcScrollBar;
    ::OffsetRect(&band, -window.left, -window.top);
    REQUIRE(band.right - band.left > 4);
    REQUIRE(band.bottom - band.top > 4);
    struct DC
    {
        explicit DC(HWND value) : window(value), dc(::GetWindowDC(value)),
            saved(dc ? ::SaveDC(dc) : 0) {}
        ~DC()
        {
            if ( saved ) ::RestoreDC(dc, saved);
            if ( dc ) ::ReleaseDC(window, dc);
        }
        HWND window;
        HDC dc;
        int saved;
    } observed(hwnd);
    REQUIRE(observed.dc);
    REQUIRE(observed.saved);
    REQUIRE(::SetLayout(observed.dc, 0) != GDI_ERROR);
    std::vector<COLORREF> pixels;
    for ( int y = band.top + 2; y < band.bottom - 2; y += 2 )
        for ( int x = band.left + 2; x < band.right - 2; x += 2 )
            pixels.push_back(::GetPixel(observed.dc, x, y));
    REQUIRE_FALSE(pixels.empty());
    REQUIRE(std::count(pixels.begin(), pixels.end(), CLR_INVALID) == 0);
    return pixels;
}

struct ScrollbarGeometrySample
{
    WXUINT message = 0;
    bool readable = false;
    bool projected = false;
    RECT band{};
    UINT page = 0;
    COLORREF first = CLR_INVALID;
    unsigned dirty = 0;
    unsigned invalid = 0;
};

// This observer also runs inside a native window procedure. Keep assertions
// outside that ABI boundary: a failed CHECK/REQUIRE must not unwind USER32.
ScrollbarGeometrySample SampleScrollbarGeometry(HWND hwnd, WXUINT message = 0)
{
    ScrollbarGeometrySample sample;
    sample.message = message;
    SCROLLBARINFO bar{sizeof(SCROLLBARINFO)};
    SCROLLINFO model{sizeof(SCROLLINFO), SIF_PAGE};
    RECT window{};
    if ( !::GetWindowRect(hwnd, &window) ||
         !::GetScrollBarInfo(hwnd, OBJID_VSCROLL, &bar) ||
         !::GetScrollInfo(hwnd, SB_VERT, &model) ||
         (bar.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) )
        return sample;
    sample.band = bar.rcScrollBar;
    sample.page = model.nPage;
    sample.projected = wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL);
    HDC dc = ::GetWindowDC(hwnd);
    if ( !dc ) return sample;
    const int saved = ::SaveDC(dc);
    if ( saved && ::SetLayout(dc, 0) != GDI_ERROR )
    {
        RECT band = bar.rcScrollBar;
        ::OffsetRect(&band, -window.left, -window.top);
        sample.first = ::GetPixel(dc, band.left + 2, band.top + 2);
        unsigned count = 0;
        for ( int y = band.top + 2; y < band.bottom - 2; y += 3 )
            for ( int x = band.left + 2; x < band.right - 2; x += 2 )
            {
                const COLORREF pixel = ::GetPixel(dc, x, y);
                ++count;
                if ( pixel == CLR_INVALID ) ++sample.invalid;
                else if ( pixel != RGB(0, 0, 0) ) ++sample.dirty;
            }
        sample.readable = count != 0 && sample.invalid == 0;
    }
    if ( saved ) ::RestoreDC(dc, saved);
    ::ReleaseDC(hwnd, dc);
    return sample;
}

class WindowScrollbarFixture
{
public:
    explicit WindowScrollbarFixture(bool rtl)
    {
        wxWinUIAppearanceTestAccess::SetHighContrastOverride(
            wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
            "window scrollbar projection", WindowScrollbarTestPosition(),
            wxSize(540, 380));
        window = std::make_unique<CountedScrollbarWindow>(frame.get(), wxID_ANY,
            wxPoint(20, 60), wxSize(340, 240));
        window->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        window->SetScrollbar(wxVERTICAL, 123, 120, 200000);
        window->SetScrollbar(wxHORIZONTAL, 234, 80, 150000);
        // Capture the HWND contract before creating the first native XAML
        // child. Projection must not remove styles or enlarge the client.
        nativeBefore[0] = ReadInfo(SB_VERT);
        nativeBefore[1] = ReadInfo(SB_HORZ);
        REQUIRE(::GetClientRect(GetHwndOf(window.get()), &clientBefore));
        const auto observe = [this](wxScrollWinEvent& event)
            {
                events.push_back({event.GetEventType(), event.GetPosition(),
                                  event.GetOrientation(), event.GetEventObject()});
                window->SetScrollPos(event.GetOrientation(), event.GetPosition());
            };
        window->Bind(wxEVT_SCROLLWIN_TOP, observe);
        window->Bind(wxEVT_SCROLLWIN_BOTTOM, observe);
        window->Bind(wxEVT_SCROLLWIN_LINEUP, observe);
        window->Bind(wxEVT_SCROLLWIN_LINEDOWN, observe);
        window->Bind(wxEVT_SCROLLWIN_PAGEUP, observe);
        window->Bind(wxEVT_SCROLLWIN_PAGEDOWN, observe);
        window->Bind(wxEVT_SCROLLWIN_THUMBTRACK, observe);
        window->Bind(wxEVT_SCROLLWIN_THUMBRELEASE, observe);
        // A sibling avoids the frame's automatic single-child sizing and
        // establishes the ordinary shared-island host without a test seam.
        new wxButton(frame.get(), wxID_ANY, "XAML host",
                     wxPoint(20, 15), wxSize(110, 30));
        frame->ShowWithoutActivating();
    }

    ~WindowScrollbarFixture()
    {
        window.reset();
        frame.reset();
        wxWinUIAppearanceTestAccess::SetHighContrastOverride(
            wxWinUIAppearanceTestAccess::HighContrastOverride::System);
    }

    SCROLLINFO ReadInfo(int nativeOrientation) const
    {
        SCROLLINFO info{sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS};
        REQUIRE(::GetScrollInfo(GetHwndOf(window.get()), nativeOrientation, &info));
        return info;
    }

    winrt::hstring Id(bool verticalAxis) const
    {
        const auto text = wxString::Format("wxWindow.Scrollbar.%llu.%s",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(
                GetHwndOf(window.get()))), verticalAxis ? "Vertical" : "Horizontal");
        return winrt::hstring(text.wc_str());
    }

    X::UIElement Root() const
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        return host && host->GetXamlRoot() ? host->GetXamlRoot().Content() : nullptr;
    }

    bool Ready()
    {
        auto root = Root();
        if ( !root ) return false;
        vertical = FindWindowScrollbar(root, Id(true));
        horizontal = FindWindowScrollbar(root, Id(false));
        return vertical && horizontal && vertical.IsLoaded() && horizontal.IsLoaded() &&
            vertical.ActualWidth() > 0 && vertical.ActualHeight() > 0 &&
            horizontal.ActualWidth() > 0 && horizontal.ActualHeight() > 0;
    }

    std::string DescribeReadiness() const
    {
        std::ostringstream out;
        const HWND hwnd = GetHwndOf(window.get());
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        const auto root = Root();
        RECT rect{};
        ::GetWindowRect(hwnd, &rect);
        out << "scrollbar readiness host=" << (host != nullptr)
            << " root=" << static_cast<bool>(root)
            << " rootChildren=" << (root ? X::Media::VisualTreeHelper::GetChildrenCount(root) : -1)
            << " canvasChildren=" << (host && host->Root() ? static_cast<int>(host->Root().Children().Size()) : -1)
            << " hwnd=" << static_cast<void*>(hwnd)
            << " visible=" << (::IsWindowVisible(hwnd) != FALSE)
            << " managed=" << (::GetPropW(hwnd, L"wxWinUI.ManagedWindowScrollbars") != nullptr)
            << " styles=0x" << std::hex << ::GetWindowLongPtr(hwnd, GWL_STYLE) << std::dec
            << " rect=" << rect.left << ',' << rect.top << ',' << rect.right << ',' << rect.bottom;
        for ( int i = 0; i != 2; ++i )
        {
            const auto bar = i == 0 ? vertical : horizontal;
            SCROLLBARINFO native{sizeof(SCROLLBARINFO)};
            const bool nativeFound = ::GetScrollBarInfo(hwnd,
                i == 0 ? OBJID_VSCROLL : OBJID_HSCROLL, &native) != FALSE;
            out << (i == 0 ? " vertical" : " horizontal")
                << " native=" << nativeFound
                << " state=0x" << std::hex << native.rgstate[0] << std::dec
                << " band=" << native.rcScrollBar.left << ',' << native.rcScrollBar.top
                << ',' << native.rcScrollBar.right << ',' << native.rcScrollBar.bottom
                << " peer=" << static_cast<bool>(bar);
            if ( bar )
                out << " loaded=" << bar.IsLoaded()
                    << " visibility=" << static_cast<int>(bar.Visibility())
                    << " size=" << bar.ActualWidth() << 'x' << bar.ActualHeight();
        }
        return out.str();
    }

    std::vector<ObservedWindowScroll> events;
    std::unique_ptr<wxFrame> frame;
    std::unique_ptr<CountedScrollbarWindow> window;
    P::ScrollBar vertical{nullptr}, horizontal{nullptr};
    SCROLLINFO nativeBefore[2]{};
    RECT clientBefore{};
};

void CheckWindowScrollbarModel(const P::ScrollBar& bar, const SCROLLINFO& info)
{
    CHECK(bar.Minimum() == info.nMin);
    CHECK(bar.Maximum() == info.nMax - static_cast<int>(info.nPage - 1));
    CHECK(bar.ViewportSize() == info.nPage);
    CHECK(bar.Value() == info.nPos);
}
}

TEST_CASE("wxWinUI window scrollbars preserve native model geometry and RTL",
          "[winui-window-scrollbar][winui-range-events][scrollbar][geometry]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    WindowScrollbarFixture fixture(rtl);
    const bool ready = WaitForWindowScrollbars([&]() { return fixture.Ready(); });
    INFO((ready ? "" : fixture.DescribeReadiness()));
    REQUIRE(ready);
    const auto vertical = fixture.vertical;
    const auto horizontal = fixture.horizontal;
    CHECK(vertical.Orientation() == C::Orientation::Vertical);
    CHECK(horizontal.Orientation() == C::Orientation::Horizontal);
    const auto flow = rtl ? X::FlowDirection::RightToLeft : X::FlowDirection::LeftToRight;
    CHECK(vertical.FlowDirection() == flow);
    CHECK(horizontal.FlowDirection() == flow);
    const HWND hwnd = GetHwndOf(fixture.window.get());
    RECT client{};
    REQUIRE(::GetClientRect(hwnd, &client));
    CHECK(::EqualRect(&client, &fixture.clientBefore));
    CHECK((::GetWindowLongPtr(hwnd, GWL_STYLE) & (WS_VSCROLL | WS_HSCROLL)) ==
          (WS_VSCROLL | WS_HSCROLL));
    for ( int i = 0; i != 2; ++i )
    {
        const auto current = fixture.ReadInfo(i == 0 ? SB_VERT : SB_HORZ);
        CHECK(current.nMin == fixture.nativeBefore[i].nMin);
        CHECK(current.nMax == fixture.nativeBefore[i].nMax);
        CHECK(current.nPage == fixture.nativeBefore[i].nPage);
        CHECK(current.nPos == fixture.nativeBefore[i].nPos);
        const auto bar = i == 0 ? vertical : horizontal;
        CheckWindowScrollbarModel(bar, current);
        SCROLLBARINFO band{sizeof(SCROLLBARINFO)};
        REQUIRE(::GetScrollBarInfo(hwnd, i == 0 ? OBJID_VSCROLL : OBJID_HSCROLL, &band));
        auto* const host = wxWinUITopLevelHost::ForWindow(fixture.frame.get(), false);
        REQUIRE(host);
        const auto screen = host->GetClientScreenRect();
        const double scale = host->GetScale();
        CHECK(std::abs(C::Canvas::GetLeft(bar) * scale + screen.left - band.rcScrollBar.left) < 1.1);
        CHECK(std::abs(C::Canvas::GetTop(bar) * scale + screen.top - band.rcScrollBar.top) < 1.1);
        CHECK(std::abs(bar.ActualWidth() * scale -
            (band.rcScrollBar.right - band.rcScrollBar.left)) < 1.1);
        CHECK(std::abs(bar.ActualHeight() * scale -
            (band.rcScrollBar.bottom - band.rcScrollBar.top)) < 1.1);
        // Use the same native palette/template as ScrollViewer/ListView:
        // transparent ordinary backing, native Acrylic track on expansion,
        // and the system's unmodified high-contrast resources.
        // HasKey/Lookup also resolve inherited theme resources. Enumeration
        // observes the local dictionary, which must not replace that palette.
        for ( const auto& entry : bar.Resources() )
        {
            const auto key = winrt::unbox_value_or<winrt::hstring>(entry.Key(), L"");
            for ( const auto* resource : {L"ScrollBarBackground", L"ScrollBarBackgroundPointerOver",
                L"ScrollBarBackgroundDisabled", L"ScrollBarTrackFill", L"ScrollBarTrackFillPointerOver",
                L"ScrollBarTrackFillDisabled", L"ScrollBarTrackStroke", L"ScrollBarTrackStrokePointerOver",
                L"ScrollBarTrackStrokeDisabled"} )
                CHECK(key != resource);
        }
        const auto background = bar.Background().try_as<X::Media::SolidColorBrush>();
        REQUIRE(background);
        CHECK(background.Color().A == 0);
        const auto templateRoot = FindWindowScrollbarPart(bar, L"Root").try_as<C::Grid>();
        REQUIRE(templateRoot);
        const auto rootBrush = templateRoot.Background();
        // Null paints no background, just like a transparent solid brush.
        if ( rootBrush )
        {
            const auto templateBackground = rootBrush.try_as<X::Media::SolidColorBrush>();
            REQUIRE(templateBackground);
            CHECK(templateBackground.Color().A == 0);
        }
    }
    fixture.window->SetScrollPos(wxVERTICAL, 70001);
    fixture.window->SetScrollPos(wxHORIZONTAL, 80002);
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        return vertical.Value() == 70001 && horizontal.Value() == 80002;
    }));
    REQUIRE(SettleWindowScrollbars());
    CHECK(fixture.events.empty());
    CHECK(fixture.ReadInfo(SB_VERT).nPos == 70001);
    CHECK(fixture.ReadInfo(SB_HORZ).nPos == 80002);
    CHECK(FindWindowScrollbar(fixture.Root(), fixture.Id(true)) == vertical);
    CHECK(FindWindowScrollbar(fixture.Root(), fixture.Id(false)) == horizontal);
    REQUIRE(::GetClientRect(hwnd, &client));
    CHECK(::EqualRect(&client, &fixture.clientBefore));
}

TEST_CASE("wxWinUI window scrollbar UIA delivers full positions and retires stale work",
          "[winui-window-scrollbar][winui-range-events][scrollbar][uia][lifetime]")
{
    const bool rtl = GENERATE(false, true);
    const bool verticalAxis = GENERATE(false, true);
    CAPTURE(rtl, verticalAxis);
    WindowScrollbarFixture fixture(rtl);
    const bool ready = WaitForWindowScrollbars([&]() { return fixture.Ready(); });
    INFO((ready ? "" : fixture.DescribeReadiness()));
    REQUIRE(ready);
    const auto bar = verticalAxis ? fixture.vertical : fixture.horizontal;
    const int orientation = verticalAxis ? wxVERTICAL : wxHORIZONTAL;
    const auto id = fixture.Id(verticalAxis);
    const auto peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(bar);
    REQUIRE(peer);
    const auto range = peer.GetPattern(A::PatternInterface::RangeValue)
        .try_as<Provider::IRangeValueProvider>();
    REQUIRE(range);
    CHECK_FALSE(range.IsReadOnly());
    range.SetValue(90001);
    REQUIRE(WaitForWindowScrollbars([&]() { return fixture.events.size() == 1; }));
    REQUIRE(SettleWindowScrollbars());
    REQUIRE(fixture.events.size() == 1);
    CHECK(fixture.events[0].type == wxEVT_SCROLLWIN_THUMBTRACK);
    CHECK(fixture.events[0].position == 90001);
    CHECK(fixture.events[0].orientation == orientation);
    CHECK(fixture.events[0].object == fixture.window.get());
    CHECK(fixture.window->GetScrollPos(orientation) == 90001);
    CHECK(bar.Value() == 90001);

    fixture.events.clear();
    range.SetValue(100002);
    fixture.window->SetScrollPos(orientation, 70003);
    REQUIRE(WaitForWindowScrollbars([&]() { return bar.Value() == 70003; }));
    REQUIRE(SettleWindowScrollbars());
    CHECK(fixture.events.empty());
    CHECK(fixture.window->GetScrollPos(orientation) == 70003);
    CHECK(FindWindowScrollbar(fixture.Root(), id) == bar);

    // Both an already queued UIA write and a retained provider are stale
    // after the HWND owner dies. A detached WinUI peer may reject SetValue;
    // it must never publish another wx event or access the deleted owner.
    range.SetValue(80004);
    fixture.window.reset();
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        return !FindWindowScrollbar(fixture.Root(), id);
    }));
    try { range.SetValue(85005); }
    catch ( const winrt::hresult_error& ) { }
    REQUIRE(SettleWindowScrollbars());
    CHECK(fixture.events.empty());
}

TEST_CASE("wxWinUI scrollbar projection does not invalidate the scrolling client",
          "[winui-window-scrollbar][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    WindowScrollbarFixture fixture(rtl);
    REQUIRE(WaitForWindowScrollbars([&]() { return fixture.Ready(); }));
    REQUIRE(SettleWindowScrollbars());
    const HWND hwnd = GetHwndOf(fixture.window.get());
    auto* const host = wxWinUITopLevelHost::ForWindow(fixture.frame.get(), false);
    REQUIRE(host);
    host->FlushSync();
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxBOTH));

    // Positive control: this observes real WM_PAINT and its native update
    // region, without replacing the window's handler or validating it away.
    const RECT content{8, 9, 28, 29};
    REQUIRE(::InvalidateRect(hwnd, &content, FALSE));
    REQUIRE(::UpdateWindow(hwnd));
    REQUIRE(fixture.window->clientPaints > 0);
    REQUIRE(SettleWindowScrollbars());
    REQUIRE_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));
    fixture.window->paintMessages = 0;
    fixture.window->clientPaints = 0;

    for ( int i = 0; i != 4; ++i )
    {
        fixture.window->SetScrollPos(wxVERTICAL, 50000 + i);
        fixture.window->SetScrollPos(wxHORIZONTAL, 60000 + i);
        host->FlushSync();
        CHECK_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));
        REQUIRE(::UpdateWindow(hwnd));
        REQUIRE(WaitForWindowScrollbars([&]()
        {
            return fixture.vertical.Value() == 50000 + i &&
                   fixture.horizontal.Value() == 60000 + i;
        }));
        // Repeated geometry synchronizations with identical bands must also
        // remain paint-neutral, rather than clearing backing every flush.
        wxWinUITLWHostNotifyNativeLayout(fixture.window.get(), false);
        host->FlushSync();
        CHECK_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));
        REQUIRE(SettleWindowScrollbars());
    }
    CHECK(fixture.window->paintMessages == 0);
    CHECK(fixture.window->clientPaints == 0);
    CHECK(fixture.events.empty());

    // Unlike a pure position write, USER32 can invalidate client pixels
    // while changing the range/page. Observe that native region immediately
    // after the setter: the projection may not add any client damage to it.
    fixture.window->SetScrollbar(wxVERTICAL, 50003, 130, 210000);
    AutoHRGN beforeFlush = ::CreateRectRgn(0, 0, 0, 0);
    AutoHRGN afterFlush = ::CreateRectRgn(0, 0, 0, 0);
    AutoHRGN added = ::CreateRectRgn(0, 0, 0, 0);
    REQUIRE(static_cast<HRGN>(beforeFlush));
    REQUIRE(static_cast<HRGN>(afterFlush));
    REQUIRE(static_cast<HRGN>(added));
    REQUIRE(::GetUpdateRgn(hwnd, beforeFlush, FALSE) != ERROR);
    host->FlushSync();
    REQUIRE(::GetUpdateRgn(hwnd, afterFlush, FALSE) != ERROR);
    CHECK(::CombineRgn(added, afterFlush, beforeFlush, RGN_DIFF) == NULLREGION);
    REQUIRE(::UpdateWindow(hwnd));
    REQUIRE(SettleWindowScrollbars());
    const unsigned afterRangePaint = fixture.window->clientPaints;
    wxWinUITLWHostNotifyNativeLayout(fixture.window.get(), false);
    host->FlushSync();
    REQUIRE(SettleWindowScrollbars());
    CHECK(fixture.window->clientPaints == afterRangePaint);
    CHECK_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));
    CHECK(fixture.vertical.ViewportSize() == 130);
    CHECK(fixture.vertical.Maximum() == 210000 - 130);
}

TEST_CASE("wxWinUI scrollbar setters do not expose classic non-client pixels",
          "[winui-window-scrollbar][winui-scrollbar-backing][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    const bool verticalAxis = GENERATE(false, true);
    CAPTURE(rtl, verticalAxis);
    WindowScrollbarFixture fixture(rtl);
    REQUIRE(WaitForWindowScrollbars([&]() { return fixture.Ready(); }));
    REQUIRE(SettleWindowScrollbars());
    auto* const host = wxWinUITopLevelHost::ForWindow(fixture.frame.get(), false);
    REQUIRE(host);
    host->FlushSync();
    const HWND hwnd = GetHwndOf(fixture.window.get());
    const int orientation = verticalAxis ? wxVERTICAL : wxHORIZONTAL;
    const int nativeOrientation = verticalAxis ? SB_VERT : SB_HORZ;
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxBOTH));
    const auto peer = verticalAxis ? fixture.vertical : fixture.horizontal;
    const auto initial = fixture.ReadInfo(nativeOrientation);
    RECT initialClient{};
    REQUIRE(::GetClientRect(hwnd, &initialClient));

    // Observe the HWND backing, not a synthetic rendering of the scrollbar.
    // All samples are read on the same stack, before a queued projection flush
    // or a later WM_NCPAINT can conceal a transient classic scrollbar.
    const auto readBand = [&]()
    {
        SCROLLBARINFO bar{sizeof(SCROLLBARINFO)};
        REQUIRE(::GetScrollBarInfo(hwnd,
            verticalAxis ? OBJID_VSCROLL : OBJID_HSCROLL, &bar));
        RECT window{};
        REQUIRE(::GetWindowRect(hwnd, &window));
        RECT band = bar.rcScrollBar;
        ::OffsetRect(&band, -window.left, -window.top);
        REQUIRE(band.right - band.left > 4);
        REQUIRE(band.bottom - band.top > 4);
        struct WindowDC
        {
            explicit WindowDC(HWND windowHandle)
                : window(windowHandle), dc(::GetWindowDC(windowHandle)),
                  saved(dc ? ::SaveDC(dc) : 0) { }
            ~WindowDC()
            {
                if ( saved ) ::RestoreDC(dc, saved);
                if ( dc ) ::ReleaseDC(window, dc);
            }
            HWND window;
            HDC dc;
            int saved;
        } dc(hwnd);
        REQUIRE(dc.dc);
        REQUIRE(dc.saved);
        REQUIRE(::SetLayout(dc.dc, 0) != GDI_ERROR);
        std::vector<COLORREF> pixels;
        for ( int y = band.top + 2; y < band.bottom - 2; y += 2 )
            for ( int x = band.left + 2; x < band.right - 2; x += 2 )
                pixels.push_back(::GetPixel(dc.dc, x, y));
        REQUIRE_FALSE(pixels.empty());
        const auto invalid = std::count(pixels.begin(), pixels.end(), CLR_INVALID);
        INFO("NC band " << band.left << ',' << band.top << ','
             << band.right << ',' << band.bottom << "; invalid pixels=" << invalid);
        REQUIRE(invalid == 0);
        return pixels;
    };

    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    const auto clean = readBand();
    // Positive control: bypass only the wx NC paint filter and demand that
    // this observer can actually see USER32's scrollbar. An unreadable or
    // composited-only surface is not accepted as evidence of no double paint.
    ::DefWindowProcW(hwnd, WM_NCPAINT, 1, 0);
    REQUIRE(readBand() != clean);
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    REQUIRE(readBand() == clean);

    fixture.window->SetScrollPos(orientation, 75000);
    CHECK(fixture.ReadInfo(nativeOrientation).nPos == 75000);
    CHECK(readBand() == clean);
    fixture.window->SetScrollbar(orientation, 65000,
        static_cast<int>(initial.nPage), initial.nMax + 1);
    CHECK(fixture.ReadInfo(nativeOrientation).nPos == 65000);
    CHECK(readBand() == clean);

    // Unlike enabling an already-enabled track, this pair exercises a real
    // USER32 availability transition while retaining its reserved NC band.
    fixture.window->SetScrollbar(orientation, 0, 0, -1);
    SCROLLBARINFO disabled{sizeof(SCROLLBARINFO)};
    REQUIRE(::GetScrollBarInfo(hwnd,
        verticalAxis ? OBJID_VSCROLL : OBJID_HSCROLL, &disabled));
    CHECK((disabled.rgstate[0] & STATE_SYSTEM_UNAVAILABLE) != 0);
    CHECK(readBand() == clean);
    REQUIRE(WaitForWindowScrollbars([&]() { return !peer.IsEnabled(); }));
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    REQUIRE(readBand() == clean);

    fixture.window->SetScrollbar(orientation, initial.nPos,
        static_cast<int>(initial.nPage), initial.nMax + 1);
    CHECK(readBand() == clean);
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        return peer.IsEnabled() && peer.Value() == initial.nPos;
    }));
    CHECK(FindWindowScrollbar(fixture.Root(), fixture.Id(verticalAxis)) == peer);
    CheckWindowScrollbarModel(peer, fixture.ReadInfo(nativeOrientation));
    RECT finalClient{};
    REQUIRE(::GetClientRect(hwnd, &finalClient));
    CHECK(::EqualRect(&initialClient, &finalClient));
    CHECK(fixture.events.empty());
}

TEST_CASE("wxWinUI pure GDI scrolled window bootstraps native scrollbars asynchronously",
          "[winui-window-scrollbar][scrollbar][bootstrap][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "GDI scrollbar bootstrap", WindowScrollbarTestPosition(), wxSize(420, 300));
    auto window = std::make_unique<wxScrolledWindow>(frame.get(), wxID_ANY);
    window->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    REQUIRE_FALSE(wxWinUITopLevelHost::ForWindow(frame.get(), false));
    unsigned paints = 0;
    window->Bind(wxEVT_PAINT, [&](wxPaintEvent&)
    {
        wxPaintDC dc(window.get());
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
        ++paints;
    });
    window->SetScrollbars(1, 1, 1800, 1600);
    window->SetScrollbars(1, 1, 1900, 1700);
    // No host creation may occur on SetScrollbar/constructor's native stack.
    REQUIRE_FALSE(wxWinUITopLevelHost::ForWindow(frame.get(), false));
    frame->ShowWithoutActivating();
    const HWND hwnd = GetHwndOf(window.get());
    const auto id = [&](bool vertical)
    {
        return winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.%s",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(hwnd)),
            vertical ? "Vertical" : "Horizontal").wc_str());
    };
    P::ScrollBar vertical{nullptr}, horizontal{nullptr};
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        if ( !host || !host->GetXamlRoot() ) return false;
        auto root = host->GetXamlRoot().Content();
        vertical = FindWindowScrollbar(root, id(true));
        horizontal = FindWindowScrollbar(root, id(false));
        return vertical && horizontal && vertical.IsLoaded() && horizontal.IsLoaded() &&
               vertical.ActualHeight() > 0 && horizontal.ActualWidth() > 0 &&
               wxWinUIWindowScrollbarsProjected(hwnd, wxBOTH);
    }));
    auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
    REQUIRE(host);
    CHECK_FALSE(host->FindSlot(window.get()));
    const wxPoint point = window->ClientToScreen(wxPoint(30, 30));
    // HitTestContent deliberately keeps the bridge eligible: the root
    // routes through to classic HWNDs. Exercise that exact router and the
    // resulting real wx motion event instead of treating eligibility as a
    // XAML-only hit. No OS pointer movement or injected input is involved.
    unsigned motions = 0;
    wxPoint received;
    window->Bind(wxEVT_MOTION, [&](wxMouseEvent& event)
    {
        ++motions;
        received = event.GetPosition();
        event.Skip();
    });
    wxWinUIPointerSample sample;
    sample.pointerId = 83;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    const auto routed = wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample);
    CHECK(routed.status == wxWinUIRootPointerStatus::Completed);
    CHECK(routed.disposition == wxWinUIInputDisposition::Routed);
    REQUIRE(WaitForWindowScrollbars([&]() { return motions == 1; }));
    CHECK(received == wxPoint(30, 30));
    // The new island represents only the NC bars. The existing canvas still
    // receives and paints its ordinary wxPaintDC content without a XAML slot.
    const unsigned before = paints;
    window->Refresh(false);
    window->Update();
    CHECK(paints > before);
    SCROLLINFO info{sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS};
    REQUIRE(::GetScrollInfo(hwnd, SB_VERT, &info));
    CheckWindowScrollbarModel(vertical, info);
    REQUIRE(::GetScrollInfo(hwnd, SB_HORZ, &info));
    CheckWindowScrollbarModel(horizontal, info);
}

TEST_CASE("wxWinUI scrollbar bootstrap ignores owners destroyed before dispatch",
          "[winui-window-scrollbar][scrollbar][bootstrap][lifetime]")
{
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "retired scrollbar bootstrap", WindowScrollbarTestPosition(), wxSize(420, 300));
    auto window = std::make_unique<wxScrolledWindow>(frame.get(), wxID_ANY);
    window->SetScrollbars(1, 1, 1800, 1600);
    window->SetScrollbars(1, 1, 1900, 1700);
    REQUIRE_FALSE(wxWinUITopLevelHost::ForWindow(frame.get(), false));
    const wxWindowIDRef id = window->GetId();
    window.reset();
    // A later, non-scrolling HWND with the same wx id is not that request's
    // owner, even if USER32 reuses the handle. Dispatch must remain inert.
    auto replacement = std::make_unique<wxWindow>(frame.get(), id);
    REQUIRE(SettleWindowScrollbars());
    CHECK_FALSE(wxWinUITopLevelHost::ForWindow(frame.get(), false));
}

TEST_CASE("wxWinUI scrolled window keeps native bands clean through real scrolling",
          "[winui-window-scrollbar][winui-scrollbar-live-scroll][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "scrolled window live NC backing", WindowScrollbarTestPosition(), wxSize(540, 380));
    auto window = std::make_unique<CountedScrolledScrollbarWindow>(
        frame.get(), wxID_ANY, wxPoint(20, 60), wxSize(340, 240));
    window->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    window->SetScrollbars(10, 10, 190, 170);
    new wxButton(frame.get(), wxID_ANY, "native sibling", wxPoint(20, 15), wxSize(110, 30));
    window->Bind(wxEVT_PAINT, [&](wxPaintEvent&)
    {
        wxPaintDC dc(window.get());
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
    });
    frame->ShowWithoutActivating();
    const HWND hwnd = GetHwndOf(window.get());
    P::ScrollBar vertical{nullptr};
    const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
        static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(hwnd))).wc_str());
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        if ( !host || !host->GetXamlRoot() ) return false;
        vertical = FindWindowScrollbar(host->GetXamlRoot().Content(), id);
        return vertical && vertical.IsLoaded() && vertical.ActualHeight() > 0 &&
               wxWinUIWindowScrollbarsProjected(hwnd, wxBOTH);
    }));
    REQUIRE(SettleWindowScrollbars());
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    window->Update();
    const auto cleanVertical = ReadScrollbarBand(hwnd, true);
    const auto cleanHorizontal = ReadScrollbarBand(hwnd, false);
    const auto checkBands = [&]()
    {
        INFO("scrolls=" << window->scrolls << "; WM_PAINT=" << window->paints
             << "; WM_NCPAINT=" << window->nonClientPaints << "; wheels=" << window->wheels);
        CHECK(ReadScrollbarBand(hwnd, true) == cleanVertical);
        CHECK(ReadScrollbarBand(hwnd, false) == cleanHorizontal);
    };
    // The observer must distinguish actual USER32 art from the projection's
    // backing: otherwise unchanged pixels would be a vacuous oracle.
    ::DefWindowProcW(hwnd, WM_NCPAINT, 1, 0);
    REQUIRE(ReadScrollbarBand(hwnd, true) != cleanVertical);
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    checkBands();

    window->afterScroll = checkBands;
    const auto style = ::GetWindowLongPtrW(hwnd, GWL_STYLE);
    RECT client{};
    REQUIRE(::GetClientRect(hwnd, &client));
    unsigned scrolls = window->scrolls;
    window->Scroll(0, 1);
    CHECK(window->GetViewStart() == wxPoint(0, 1));
    CHECK(window->scrolls > scrolls);
    RECT damage{};
    REQUIRE(::GetUpdateRect(hwnd, &damage, FALSE));
    // A ten-pixel vertical move needs only the newly exposed strip, not a
    // full-client repair on behalf of the native scrollbar.
    CHECK(damage.bottom - damage.top == 10);
    window->Update();
    checkBands();

    scrolls = window->scrolls;
    const wxPoint point = window->ClientToScreen(wxPoint(35, 35));
    ::SendMessageW(hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, static_cast<WORD>(-WHEEL_DELTA)),
                   MAKELPARAM(point.x, point.y));
    CHECK(window->wheels == 1u);
    CHECK(window->scrolls > scrolls);
    CHECK(window->GetViewStart().y > 1);
    checkBands();
    window->Update();
    checkBands();

    auto automation = A::FrameworkElementAutomationPeer::CreatePeerForElement(vertical);
    REQUIRE(automation);
    auto range = automation.GetPattern(A::PatternInterface::RangeValue)
        .try_as<Provider::IRangeValueProvider>();
    REQUIRE(range);
    scrolls = window->scrolls;
    range.SetValue(80);
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        return window->GetViewStart().y == 80 && window->scrolls > scrolls;
    }));
    checkBands();
    window->Update();
    checkBands();
    CHECK(::GetWindowLongPtrW(hwnd, GWL_STYLE) == style);
    RECT finalClient{};
    REQUIRE(::GetClientRect(hwnd, &finalClient));
    CHECK(::EqualRect(&client, &finalClient));
    auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
    REQUIRE(host);
    CHECK(FindWindowScrollbar(host->GetXamlRoot().Content(), id) == vertical);
    window->afterScroll = {};
}

#if wxUSE_NOTEBOOK && wxUSE_SPLITTER && wxUSE_SYSTEM_OPTIONS
TEST_CASE("wxWinUI notebook page scrollbars expose the same Mica backing after scrolling",
          "[winui-window-scrollbar][winui-scrollbar-nested-mica][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    struct RestoreAppearance
    {
        wxString backdrop = wxSystemOptions::GetOption("winui.backdrop");
        RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", 1);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);
        }
        ~RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", backdrop);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::System);
        }
    } appearance;
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "nested page scrollbar backing", WindowScrollbarTestPosition(), wxSize(640, 420));
    auto* const splitter = new wxSplitterWindow(frame.get(), wxID_ANY);
    auto* const navigation = new wxWindow(splitter, wxID_ANY);
    auto* const book = new wxNotebook(splitter, wxID_ANY);
    auto* const page = new CountedScrolledScrollbarWindow(book, wxID_ANY);
    page->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    page->SetScrollbars(10, 10, 190, 170);
    new wxButton(page, wxID_ANY, "native page content", wxPoint(15, 15), wxSize(160, 32));
    REQUIRE(book->AddPage(page, "Scrolling page", true));
    REQUIRE(book->AddPage(new wxWindow(book, wxID_ANY), "Other page"));
    REQUIRE(splitter->SplitVertically(navigation, book, 120));
    frame->ShowWithoutActivating();
    wxWinUIApplyWindowBackdrop(frame.get());
    // Demand actual successful material publication, not a marker manufactured
    // by the test. This regression is specifically the Showcase topology.
    REQUIRE(wxWinUIWindowUsesBackdrop(frame.get()));
    REQUIRE(wxWinUIWindowUsesBackdrop(page));
    const HWND hwnd = GetHwndOf(page);
    const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
        static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(hwnd))).wc_str());
    P::ScrollBar vertical{nullptr};
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        if ( !host || !host->GetXamlRoot() ) return false;
        vertical = FindWindowScrollbar(host->GetXamlRoot().Content(), id);
        return vertical && vertical.IsLoaded() && vertical.ActualHeight() > 0 &&
               wxWinUIWindowScrollbarsProjected(hwnd, wxBOTH) && page->nonClientPaints > 0;
    }));
    REQUIRE(SettleWindowScrollbars());
    const auto checkTransparentBacking = [&]()
    {
        INFO("nested NC paints=" << page->nonClientPaints << "; scrolls=" << page->scrolls);
        for ( bool axis : {false, true} )
        {
            const auto pixels = ReadScrollbarBand(hwnd, axis);
            CAPTURE(axis, pixels.front());
            // These are HWND backing pixels, not a screenshot of Mica. The
            // transparent XAML template must have the DWM zero-colour backing,
            // not a retained USER32 #171717 track under it.
            CHECK(std::all_of(pixels.begin(), pixels.end(), [](COLORREF pixel)
                { return pixel == RGB(0, 0, 0); }));
        }
    };
    checkTransparentBacking();
    page->afterScroll = checkTransparentBacking;
    page->Scroll(0, 3);
    CHECK(page->GetViewStart() == wxPoint(0, 3));
    checkTransparentBacking();
    page->Update();
    checkTransparentBacking();
    const auto previous = page->scrolls;
    const wxPoint point = page->ClientToScreen(wxPoint(45, 80));
    ::SendMessageW(hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, static_cast<WORD>(-WHEEL_DELTA)),
                   MAKELPARAM(point.x, point.y));
    CHECK(page->scrolls > previous);
    CHECK(page->wheels == 1u);
    page->Update();
    checkTransparentBacking();
    auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
    REQUIRE(host);
    REQUIRE(WaitForWindowScrollbars([&]()
        { return vertical.Value() == page->GetViewStart().y; }));
    CHECK(FindWindowScrollbar(host->GetXamlRoot().Content(), id) == vertical);
    page->afterScroll = {};
}
#endif // wxUSE_NOTEBOOK && wxUSE_SPLITTER && wxUSE_SYSTEM_OPTIONS

#if wxUSE_SYSTEM_OPTIONS
TEST_CASE("wxWinUI scrollbar bridge backing is observable independently of its owner",
          "[winui-window-scrollbar][winui-scrollbar-bridge-backing][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    struct RestoreAppearance
    {
        wxString backdrop = wxSystemOptions::GetOption("winui.backdrop");
        RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", 1);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);
        }
        ~RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", backdrop);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::System);
        }
    } appearance;
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "scrollbar bridge backing", WindowScrollbarTestPosition(), wxSize(540, 380));
    auto window = std::make_unique<CountedScrolledScrollbarWindow>(
        frame.get(), wxID_ANY, wxPoint(20, 60), wxSize(340, 240));
    window->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    window->SetScrollbars(10, 10, 190, 170);
    new wxButton(frame.get(), wxID_ANY, "native sibling", wxPoint(20, 15), wxSize(110, 30));
    frame->ShowWithoutActivating();
    wxWinUIApplyWindowBackdrop(frame.get());
    REQUIRE(wxWinUIWindowUsesBackdrop(window.get()));
    const HWND owner = GetHwndOf(window.get());
    const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
        static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(owner))).wc_str());
    P::ScrollBar peer{nullptr};
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        if ( !host || !host->GetXamlRoot() ) return false;
        peer = FindWindowScrollbar(host->GetXamlRoot().Content(), id);
        return peer && peer.IsLoaded() && peer.ActualHeight() > 0 &&
               wxWinUIWindowScrollbarsProjected(owner, wxBOTH) && window->nonClientPaints;
    }));
    REQUIRE(SettleWindowScrollbars());
    auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge);
    SCROLLBARINFO bar{sizeof(SCROLLBARINFO)};
    REQUIRE(::GetScrollBarInfo(owner, OBJID_VSCROLL, &bar));
    const POINT point{bar.rcScrollBar.left + 2, bar.rcScrollBar.top + 2};

    struct BackingDC
    {
        explicit BackingDC(HWND value) : hwnd(value), dc(::GetWindowDC(value)),
            saved(dc ? ::SaveDC(dc) : 0) {}
        ~BackingDC()
        {
            if ( saved ) ::RestoreDC(dc, saved);
            if ( dc ) ::ReleaseDC(hwnd, dc);
        }
        HWND hwnd;
        HDC dc;
        int saved;
    };
    const auto sample = [&](HWND hwnd)
    {
        RECT bounds{};
        REQUIRE(::GetWindowRect(hwnd, &bounds));
        BackingDC backing(hwnd);
        REQUIRE(backing.dc); REQUIRE(backing.saved);
        REQUIRE(::SetLayout(backing.dc, 0) != GDI_ERROR);
        const auto pixel = ::GetPixel(backing.dc, point.x - bounds.left, point.y - bounds.top);
        REQUIRE(pixel != CLR_INVALID);
        return pixel;
    };
    const COLORREF cleanOwner = sample(owner), cleanBridge = sample(bridge);
    CAPTURE(cleanOwner, cleanBridge);
    CHECK(cleanOwner == RGB(0, 0, 0));
    CHECK(cleanBridge == RGB(0, 0, 0));
    RECT bridgeBounds{};
    REQUIRE(::GetWindowRect(bridge, &bridgeBounds));
    const RECT sentinelRect{point.x - bridgeBounds.left, point.y - bridgeBounds.top,
                           point.x - bridgeBounds.left + 2, point.y - bridgeBounds.top + 2};
    const COLORREF sentinel = RGB(239, 37, 181);
    {
        BackingDC backing(bridge);
        REQUIRE(backing.dc); REQUIRE(backing.saved);
        REQUIRE(::SetLayout(backing.dc, 0) != GDI_ERROR);
        HBRUSH brush = ::CreateSolidBrush(sentinel);
        REQUIRE(brush);
        const bool painted = ::FillRect(backing.dc, &sentinelRect, brush) != 0;
        ::DeleteObject(brush);
        REQUIRE(painted);
    }
    // Positive control: observe the bridge's real GDI backing, not the
    // transparent XAML Background property or an unrelated owner DC.
    REQUIRE(sample(bridge) == sentinel);
    const COLORREF ownerAfterSentinel = sample(owner);
    window->Scroll(0, 1);
    REQUIRE(window->scrolls > 0);
    REQUIRE(WaitForWindowScrollbars([&]() { return peer.Value() == 1; }));
    peer.IndicatorMode(P::ScrollingIndicatorMode::None);
    REQUIRE(WaitForWindowScrollbars([&]()
    {
        const auto track = FindWindowScrollbarPart(peer, L"VerticalRoot");
        return track && track.Visibility() == X::Visibility::Collapsed;
    }));
    const COLORREF afterScroll = sample(bridge);
    // Deliberately do not require Scroll() to repair an arbitrary external
    // write: determine whether the NC cleanup acts on the same surface first.
    ::SendMessageW(owner, WM_NCPAINT, 1, 0);
    const COLORREF afterNC = sample(bridge);
    {
        BackingDC backing(bridge);
        REQUIRE(backing.dc); REQUIRE(backing.saved);
        REQUIRE(::SetLayout(backing.dc, 0) != GDI_ERROR);
        REQUIRE(::IntersectClipRect(backing.dc, sentinelRect.left, sentinelRect.top,
                                   sentinelRect.right, sentinelRect.bottom) != ERROR);
        REQUIRE(::SendMessageW(bridge, WM_ERASEBKGND,
                              reinterpret_cast<WPARAM>(backing.dc), 0));
    }
    const COLORREF afterBridgeErase = sample(bridge);
    WARN("bridge backing experiment rtl=" << rtl << " clean owner/bridge="
         << cleanOwner << '/' << cleanBridge << " owner after bridge sentinel="
         << ownerAfterSentinel << " bridge after Scroll/NC/bridge erase="
         << afterScroll << '/' << afterNC << '/' << afterBridgeErase);
    CHECK(afterBridgeErase == cleanBridge);
    CHECK(sample(owner) == cleanOwner);
    CHECK(FindWindowScrollbar(host->GetXamlRoot().Content(), id) == peer);
}
#endif // wxUSE_SYSTEM_OPTIONS

TEST_CASE("wxWinUI vertical-only scrollbar excludes direct native frame redraw paths",
          "[winui-window-scrollbar][winui-scrollbar-native-frame][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "short vertical scrollbar backing", WindowScrollbarTestPosition(), wxSize(620, 660));
    auto page = std::make_unique<CountedScrolledScrollbarWindow>(frame.get(), wxID_ANY,
        wxPoint(20, 50), wxSize(540, 552), wxBORDER_NONE);
    page->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    new wxButton(page.get(), wxID_ANY, "scrolling native child",
        wxPoint(10, 10), wxSize(180, 32));
    new wxButton(frame.get(), wxID_ANY, "native sibling", wxPoint(20, 10), wxSize(140, 30));
    frame->ShowWithoutActivating();
    // Creating the first native child can size the frame's then-only child
    // to its entire client. Establish the short range only after the sibling
    // and frame exist: otherwise 49*12 may fit, legitimately hiding the bar.
    page->SetSize(wxRect(20, 50, 540, 552));
    page->SetClientSize(540, 552);
    REQUIRE(page->GetClientSize().y == 552);
    page->SetScrollbars(0, 12, 0, 49);
    const HWND hwnd = GetHwndOf(page.get());
    REQUIRE((::GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_VSCROLL) != 0);
    SCROLLINFO initialInfo{sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS};
    REQUIRE(::GetScrollInfo(hwnd, SB_VERT, &initialInfo));
    CAPTURE(page->GetClientSize().y, initialInfo.nMax, initialInfo.nPage);
    REQUIRE(initialInfo.nMax == 48);
    REQUIRE(initialInfo.nPage == 46u);
    const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
        static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(hwnd))).wc_str());
    P::ScrollBar peer{nullptr};
    REQUIRE(WaitForWindowScrollbars([&]
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        if ( !host || !host->GetXamlRoot() ) return false;
        peer = FindWindowScrollbar(host->GetXamlRoot().Content(), id);
        return peer && peer.IsLoaded() && peer.ActualHeight() > 0 &&
               wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL);
    }));
    REQUIRE(SettleWindowScrollbars());
    CHECK((::GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_HSCROLL) == 0);
    SCROLLINFO info{sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS};
    REQUIRE(::GetScrollInfo(hwnd, SB_VERT, &info));
    CAPTURE(info.nMax, info.nPage);
    REQUIRE(info.nMax - static_cast<int>(info.nPage) + 1 > 0);
    REQUIRE(info.nMax - static_cast<int>(info.nPage) + 1 <= 4);
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    const auto clean = ReadScrollbarBand(hwnd, true);
    const auto check = [&](const char* stage)
    {
        const auto actual = ReadScrollbarBand(hwnd, true);
        WARN("vertical native redraw rtl=" << rtl << " stage=" << stage
             << " first=" << actual.front() << " clean=" << clean.front()
             << " changed=" << (actual != clean));
        CHECK((actual == clean));
    };
    const auto resetBacking = [&]
    {
        ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
        REQUIRE((ReadScrollbarBand(hwnd, true) == clean));
    };
    SCROLLBARINFO bar{sizeof(SCROLLBARINFO)};
    REQUIRE(::GetScrollBarInfo(hwnd, OBJID_VSCROLL, &bar));
    // Positive observer control, followed by restoration through the public
    // window procedure. No desktop input or USER32 tracking loop is entered.
    ::DefWindowProcW(hwnd, WM_NCPAINT, 1, 0);
    REQUIRE(ReadScrollbarBand(hwnd, true) != clean);
    ::SendMessageW(hwnd, WM_NCPAINT, 1, 0);
    check("full NC");

    HRGN band = ::CreateRectRgnIndirect(&bar.rcScrollBar);
    REQUIRE(band);
    ::SendMessageW(hwnd, WM_NCPAINT, reinterpret_cast<WPARAM>(band), 0);
    ::DeleteObject(band);
    check("band-only NC");
    resetBacking();
    const HWND activeWindow = ::GetActiveWindow();
    const HWND foregroundWindow = ::GetForegroundWindow();
    CHECK(::SendMessageW(hwnd, WM_NCACTIVATE, TRUE, 0) == TRUE);
    check("NC activation");
    CHECK(::GetActiveWindow() == activeWindow);
    CHECK(::GetForegroundWindow() == foregroundWindow);
    resetBacking();
    CHECK(::SendMessageW(hwnd, WM_NCACTIVATE, FALSE, 0) == TRUE);
    check("NC deactivation");
    resetBacking();
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    ::DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, -1);
    const auto directActivation = ReadScrollbarBand(hwnd, true);
    WARN("direct DefWindowProc NCACTIVATE(-1) rtl=" << rtl
         << " first=" << directActivation.front()
         << " changed=" << (directActivation != clean));
    resetBacking();
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    const auto paintsBeforeSuppressedActivation = page->nonClientPaints;
    CHECK(::SendMessageW(hwnd, WM_NCACTIVATE, TRUE, -1) == TRUE);
    CHECK(page->nonClientPaints == paintsBeforeSuppressedActivation);
    check("NC activation with drawing suppressed");
    resetBacking();
    ::SendMessageW(hwnd, WM_NCMOUSEMOVE, HTVSCROLL,
        MAKELPARAM(bar.rcScrollBar.left + 2, bar.rcScrollBar.top + 2));
    check("NC hover");
    resetBacking();
    ::SendMessageW(hwnd, WM_NCMOUSELEAVE, 0, 0);
    check("NC leave");

    resetBacking();
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    SCROLLINFO directPosition{sizeof(SCROLLINFO), SIF_POS};
    directPosition.nPos = 1;
    ::SetScrollInfo(hwnd, SB_VERT, &directPosition, FALSE);
    const auto directPositionBacking = ReadScrollbarBand(hwnd, true);
    WARN("direct SetScrollInfo(FALSE) rtl=" << rtl
         << " first=" << directPositionBacking.front()
         << " changed=" << (directPositionBacking != clean));
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    directPosition.nPos = 0;
    ::SetScrollInfo(hwnd, SB_VERT, &directPosition, FALSE);
    resetBacking();
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    page->SetScrollPos(wxVERTICAL, 1);
    REQUIRE(wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL));
    check("SetScrollPos");
    page->SetScrollPos(wxVERTICAL, 0);
    resetBacking();
    page->afterScroll = [&] { check("inside ScrollWindow return"); };
    page->Scroll(0, 3);
    CHECK(page->GetViewStart().y == 3);
    check("Scroll return");
    resetBacking();
    page->Update();
    check("client Update");
    resetBacking();
    REQUIRE(WaitForWindowScrollbars([&] { return peer.Value() == 3; }));
    check("native projection settled");
    page->afterScroll = {};
}

#if wxUSE_SYSTEM_OPTIONS
TEST_CASE("wxWinUI projected scrollbar backing stays clean during native resize and move",
          "[winui-window-scrollbar][winui-scrollbar-resize-frame][scrollbar][paint]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    struct RestoreAppearance
    {
        wxString backdrop = wxSystemOptions::GetOption("winui.backdrop");
        RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", 1);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);
        }
        ~RestoreAppearance()
        {
            wxSystemOptions::SetOption("winui.backdrop", backdrop);
            wxWinUIAppearanceTestAccess::SetHighContrastOverride(
                wxWinUIAppearanceTestAccess::HighContrastOverride::System);
        }
    } appearance;
    auto frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
        "native resize scrollbar backing", WindowScrollbarTestPosition(), wxSize(640, 650));
    auto* const sibling = new wxButton(frame.get(), wxID_ANY, "native sibling");
    sibling->SetMinSize(wxSize(-1, 48));
    auto* const page = new CountedScrolledScrollbarWindow(frame.get(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    page->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    new wxButton(page, wxID_ANY, "moving native content", wxPoint(10, 10), wxSize(180, 32));
    auto* const layout = new wxBoxSizer(wxVERTICAL);
    layout->Add(sibling, 0, wxEXPAND);
    layout->Add(page, 1, wxEXPAND);
    frame->SetSizer(layout);
    frame->SetClientSize(600, 600);
    frame->ShowWithoutActivating();
    frame->Layout();
    wxWinUIApplyWindowBackdrop(frame.get());
    REQUIRE(wxWinUIWindowUsesBackdrop(page));
    const int range = page->GetClientSize().y / 12 + 3;
    page->SetScrollbars(0, 12, 0, range);
    const HWND hwnd = GetHwndOf(page);
    const auto id = winrt::hstring(wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
        static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(hwnd))).wc_str());
    P::ScrollBar peer{nullptr};
    auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
    REQUIRE(host);
    const auto settled = [&]
    {
        if ( !host->GetXamlRoot() ) return false;
        peer = FindWindowScrollbar(host->GetXamlRoot().Content(), id);
        return peer && peer.IsLoaded() && peer.ActualHeight() > 0 &&
               wxWinUIWindowScrollbarsProjected(hwnd, wxVERTICAL);
    };
    REQUIRE(WaitForWindowScrollbars(settled));
    REQUIRE(SettleWindowScrollbars());
    const auto originalPeer = peer;
    const auto clean = SampleScrollbarGeometry(hwnd);
    REQUIRE(clean.readable);
    REQUIRE(clean.dirty == 0u);

    struct Observations
    {
        bool enabled = false;
        std::vector<ScrollbarGeometrySample> samples;
    };
    const auto observations = std::make_shared<Observations>();
    page->afterNativeGeometry = [observations, hwnd](WXUINT message)
    {
        if ( observations->enabled )
            observations->samples.push_back(SampleScrollbarGeometry(hwnd, message));
    };
    const auto check = [&](const char* stage, const ScrollbarGeometrySample& sample)
    {
        INFO("resize backing stage=" << stage << " message=" << sample.message
             << " projected=" << sample.projected << " first=" << sample.first
             << " dirty=" << sample.dirty << " invalid=" << sample.invalid
             << " page=" << sample.page << " band=" << sample.band.left << ','
             << sample.band.top << ',' << sample.band.right << ',' << sample.band.bottom);
        CHECK(sample.readable);
        CHECK(sample.dirty == 0u);
    };
    const auto exercise = [&](const char* stage, auto operation, bool resize)
    {
        const auto before = SampleScrollbarGeometry(hwnd);
        observations->samples.clear();
        observations->enabled = true;
        operation();
        observations->enabled = false;
        // No yield, Refresh, Update, layout call or projection flush between
        // the API return and this observation of the new physical band.
        const auto immediate = SampleScrollbarGeometry(hwnd);
        check(stage, immediate);
        for ( const auto& sample : observations->samples ) check(stage, sample);
        if ( resize )
        {
            CHECK_FALSE(observations->samples.empty());
            CHECK(immediate.page != before.page);
        }
        else
        {
            CHECK(immediate.page == before.page);
            CHECK(immediate.band.right - immediate.band.left ==
                  before.band.right - before.band.left);
            CHECK(immediate.band.bottom - immediate.band.top ==
                  before.band.bottom - before.band.top);
        }
        CHECK_FALSE(::EqualRect(&immediate.band, &before.band));
        REQUIRE(WaitForWindowScrollbars(settled));
        REQUIRE(SettleWindowScrollbars());
        check("settled", SampleScrollbarGeometry(hwnd));
        CHECK(peer == originalPeer);
    };
    exercise("wx frame resize", [&] { frame->SetClientSize(580, 576); }, true);
    exercise("native frame resize", [&]
    {
        const wxSize size = frame->GetSize();
        REQUIRE(::SetWindowPos(GetHwndOf(frame.get()), nullptr, 0, 0,
            size.x + 16, size.y + 12, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
    }, true);
    exercise("wx frame move", [&] { frame->Move(frame->GetPosition() + wxPoint(18, 12)); }, false);
    exercise("native frame move", [&]
    {
        const wxPoint position = frame->GetPosition();
        REQUIRE(::SetWindowPos(GetHwndOf(frame.get()), nullptr,
            position.x - 12, position.y - 6, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
    }, false);
    page->afterNativeGeometry = {};
}
#endif // wxUSE_SYSTEM_OPTIONS

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_BUTTON
