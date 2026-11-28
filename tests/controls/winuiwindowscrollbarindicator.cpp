/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiwindowscrollbarindicator.cpp
// Purpose:     Native scrollbar indicator visibility and retirement
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_BUTTON

#include "wx/app.h"
#include "wx/button.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/msw/private.h"
#include "wx/stopwatch.h"
#include "wx/window.h"
#include "wx/winui/private/tlwhost.h"
#include "../../src/winui/scrollbarprojection.h"
#include "../../src/winui/scrollbarindicator.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <algorithm>
#include <cstdint>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace P = X::Controls::Primitives;
using Mode = P::ScrollingIndicatorMode;

template<typename Predicate>
bool WaitIndicator(Predicate predicate, long timeout = 3000)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( !predicate() )
    {
        if ( elapsed.Time() >= timeout ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return true;
}

bool PumpIndicator(long milliseconds)
{
    wxStopWatch elapsed;
    return WaitIndicator([&] { return elapsed.Time() >= milliseconds; }, milliseconds + 1000);
}

wxPoint IndicatorPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

template<typename T>
T FindIndicatorPart(const X::DependencyObject& node, const winrt::hstring& id = {})
{
    if ( !node ) return nullptr;
    if ( auto candidate = node.try_as<T>() )
        if ( id.empty() || X::Automation::AutomationProperties::GetAutomationId(candidate) == id )
            return candidate;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int index = 0; index < count; ++index )
        if ( auto found = FindIndicatorPart<T>(X::Media::VisualTreeHelper::GetChild(node, index), id) )
            return found;
    return nullptr;
}

P::Thumb VisibleIndicatorThumb(const X::DependencyObject& node)
{
    if ( !node ) return nullptr;
    if ( auto element = node.try_as<X::UIElement>() )
        if ( element.Visibility() != X::Visibility::Visible ||
             element.Opacity() <= 0 || !element.IsHitTestVisible() ) return nullptr;
    if ( auto thumb = node.try_as<P::Thumb>(); thumb && thumb.IsLoaded() &&
         thumb.ActualWidth() > 0 && thumb.ActualHeight() > 0 && thumb.Opacity() > 0 ) return thumb;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int index = 0; index < count; ++index )
        if ( auto thumb = VisibleIndicatorThumb(X::Media::VisualTreeHelper::GetChild(node, index)) )
            return thumb;
    return nullptr;
}

struct IndicatorFixture
{
    IndicatorFixture()
    {
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "native scrollbar indicators",
            IndicatorPosition(), wxSize(540, 360));
        window = std::make_unique<wxWindow>(frame.get(), wxID_ANY,
            wxPoint(20, 60), wxSize(340, 240));
        window->SetScrollbar(wxVERTICAL, 20, 50, 1000);
        new wxButton(frame.get(), wxID_ANY, "native host", wxPoint(20, 15), wxSize(110, 30));
        frame->ShowWithoutActivating();
    }
    X::Controls::Canvas Root() const
    {
        auto* const host = wxWinUITopLevelHost::ForWindow(frame.get(), false);
        return host ? host->Root() : nullptr;
    }
    bool Ready()
    {
        const auto id = wxString::Format("wxWindow.Scrollbar.%llu.Vertical",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(GetHwndOf(window.get()))));
        bar = FindIndicatorPart<P::ScrollBar>(Root(), winrt::hstring(id.wc_str()));
        return bar && bar.IsLoaded() && bar.ActualWidth() > 0 && bar.ActualHeight() > 0;
    }
    std::unique_ptr<wxFrame> frame;
    std::unique_ptr<wxWindow> window;
    P::ScrollBar bar{nullptr};
};
}

TEST_CASE("wxWinUI projected scrollbar reveals its native thumb on content activity",
          "[winui-window-scrollbar][winui-scrollbar-indicator]")
{
    IndicatorFixture fixture;
    REQUIRE(WaitIndicator([&] { return fixture.Ready(); }));
    const bool autoHide = winrt::Windows::UI::ViewManagement::UISettings().AutoHideScrollBars();
    REQUIRE(fixture.bar.IndicatorMode() == (autoHide ? Mode::None : Mode::MouseIndicator));

    MSG movement{};
    movement.hwnd = GetHwndOf(fixture.window.get());
    movement.message = WM_MOUSEMOVE;
    const wxPoint screen = fixture.window->ClientToScreen(wxPoint(40, 40));
    movement.pt = {screen.x, screen.y};
    // Invoke the production observation hook, not SendInput/DispatchMessage.
    // It must neither consume nor manufacture any wx/native pointer event.
    // The projection takes the bar as ready in the synchronization which its
    // Loaded schedules, a little after it: observe the movement again until
    // then, as the moves of a real mouse would come again.
    CHECK(WaitIndicator([&]
    {
        wxWinUIWindowScrollbarsObserveMessage(movement);
        return fixture.bar.IndicatorMode() == Mode::MouseIndicator;
    }));
    REQUIRE(WaitIndicator([&] { return !!VisibleIndicatorThumb(fixture.bar); }));
    const auto identity = winrt::get_abi(fixture.bar);
    if ( autoHide ) REQUIRE(WaitIndicator([&] { return fixture.bar.IndicatorMode() == Mode::None; }));
    else { REQUIRE(PumpIndicator(2150)); CHECK(fixture.bar.IndicatorMode() == Mode::MouseIndicator); }

    fixture.window->SetScrollPos(wxVERTICAL, 80);
    REQUIRE(WaitIndicator([&] { return fixture.bar.Value() == 80; }));
    CHECK(fixture.bar.IndicatorMode() == Mode::MouseIndicator);
    CHECK(winrt::get_abi(fixture.bar) == identity);
    // HasKey also resolves inherited theme resources. Only a local override
    // would replace the native palette we want to preserve here.
    for ( const auto& entry : fixture.bar.Resources() )
        CHECK(winrt::unbox_value_or<winrt::hstring>(entry.Key(), L"") != L"ScrollBarTrackFill");

    fixture.window.reset();
    REQUIRE(WaitIndicator([&] { return !fixture.bar.IsLoaded(); }));
    fixture.bar.IndicatorMode(Mode::TouchIndicator);
    wxWinUIWindowScrollbarsObserveMessage(movement); // retired HWND incarnation
    REQUIRE(PumpIndicator(2150));
    CHECK(fixture.bar.IndicatorMode() == Mode::TouchIndicator);
}

TEST_CASE("wxWinUI native scrollbar indicator holds interaction and retires its timer",
          "[winui-window-scrollbar][winui-scrollbar-indicator]")
{
    IndicatorFixture fixture;
    REQUIRE(WaitIndicator([&] { return fixture.Ready(); }));
    auto root = fixture.Root();
    P::ScrollBar bar;
    bar.Orientation(X::Controls::Orientation::Vertical);
    bar.Width(16); bar.Height(180); bar.Maximum(100); bar.ViewportSize(10); bar.Value(20);
    X::Controls::Canvas::SetLeft(bar, 400);
    X::Controls::Canvas::SetTop(bar, 60);
    auto indicator = wxWinUIScrollBarIndicator::Create(bar);
    root.Children().Append(bar);
    REQUIRE(WaitIndicator([&] { return bar.IsLoaded() && bar.ActualHeight() > 0; }));
    const bool autoHide = winrt::Windows::UI::ViewManagement::UISettings().AutoHideScrollBars();
    indicator->Activity(true);
    CHECK(bar.IndicatorMode() == (autoHide ? Mode::TouchIndicator : Mode::MouseIndicator));
    indicator->PointerOver(true);
    REQUIRE(WaitIndicator([&] { return !!VisibleIndicatorThumb(bar); }));
    REQUIRE(PumpIndicator(2150));
    CHECK(bar.IndicatorMode() == Mode::MouseIndicator);
    indicator->Dragging(true);
    indicator->PointerOver(false);
    REQUIRE(PumpIndicator(2150));
    CHECK(bar.IndicatorMode() == Mode::MouseIndicator);
    indicator->Dragging(false);
    if ( autoHide ) REQUIRE(WaitIndicator([&] { return bar.IndicatorMode() == Mode::None; }));
    indicator->Activity();
    indicator->Retire();
    bar.IndicatorMode(Mode::TouchIndicator);
    indicator->Activity(); indicator->PointerOver(true); indicator->Dragging(true);
    REQUIRE(PumpIndicator(2150));
    CHECK(bar.IndicatorMode() == Mode::TouchIndicator);
    uint32_t index = 0;
    REQUIRE(root.Children().IndexOf(bar, index));
    root.Children().RemoveAt(index);
}

#endif
