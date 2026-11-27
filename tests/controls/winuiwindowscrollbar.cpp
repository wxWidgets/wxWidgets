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
        // The WinUI bar is a strip of the client area along its edge, drawn
        // over the content: USER32 reserves no non-client band for these
        // windows, so there is no classic scrollbar underneath.
        RECT expected{};
        REQUIRE(::GetClientRect(hwnd, &expected));
        ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&expected), 2);
        if ( expected.left > expected.right )
            std::swap(expected.left, expected.right);
        const UINT dpi = ::GetDpiForWindow(hwnd);
        const int cx = ::GetSystemMetricsForDpi(SM_CXVSCROLL, dpi);
        const int cy = ::GetSystemMetricsForDpi(SM_CYHSCROLL, dpi);
        if ( i == 0 )
        {
            if ( rtl ) expected.right = expected.left + cx;
            else expected.left = expected.right - cx;
            expected.bottom -= cy;
        }
        else
        {
            expected.top = expected.bottom - cy;
            if ( rtl ) expected.left += cx;
            else expected.right -= cx;
        }
        auto* const host = wxWinUITopLevelHost::ForWindow(fixture.frame.get(), false);
        REQUIRE(host);
        const auto screen = host->GetClientScreenRect();
        const double scale = host->GetScale();
        // The bar is in the canvas standing for its window: what counts is
        // where that puts it in the island.
        const auto placed =
            bar.TransformToVisual(host->Root()).TransformBounds(
                {0, 0, static_cast<float>(bar.ActualWidth()),
                       static_cast<float>(bar.ActualHeight())});
        CHECK(std::abs(placed.X * scale + screen.left - expected.left) < 1.1);
        CHECK(std::abs(placed.Y * scale + screen.top - expected.top) < 1.1);
        CHECK(std::abs(bar.ActualWidth() * scale -
            (expected.right - expected.left)) < 1.1);
        CHECK(std::abs(bar.ActualHeight() * scale -
            (expected.bottom - expected.top)) < 1.1);
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


#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_BUTTON
