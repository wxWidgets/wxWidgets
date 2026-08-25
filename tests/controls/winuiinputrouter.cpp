///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiinputrouter.cpp
// Purpose:     deterministic native resolver tests for the WinUI input router
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/winui/private/inputrouter.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <thread>
#include <utility>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

class HitTestWindow : public wxWindow
{
public:
    HitTestWindow(wxWindow *parent,
                  const wxPoint& pos,
                  const wxSize& size,
                  LRESULT hitTest = HTCLIENT)
        : wxWindow(parent, wxID_ANY, pos, size),
          m_hitTest(hitTest)
    {
    }

    void SetHitTest(LRESULT hitTest) { m_hitTest = hitTest; }
    void SetHitAction(std::function<void ()> action)
        { m_hitAction = std::move(action); }
    unsigned GetHitCount() const { return m_hitCount; }

    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        if ( message == WM_NCHITTEST )
        {
            ++m_hitCount;
            if ( m_hitAction )
                m_hitAction();
            return m_hitTest;
        }

        return wxWindow::MSWWindowProc(message, wParam, lParam);
    }

private:
    LRESULT m_hitTest;
    std::function<void ()> m_hitAction;
    unsigned m_hitCount = 0;
};

class WinUIInputRouterFixture
{
public:
    WinUIInputRouterFixture()
    {
        m_frame = new wxFrame(nullptr, wxID_ANY, "input-router-test",
                              wxPoint(-18000, -18000),
                              wxSize(440, 340));
        m_frame->SetClientSize(400, 300);
        // Keep real HWND geometry while remaining isolated from the user's
        // pointer and foreground window.
        m_frame->ShowWithoutActivating();
        wxYield();
    }

    ~WinUIInputRouterFixture()
    {
        if ( m_otherFrame )
            m_otherFrame->Destroy();
        m_frame->Destroy();
        wxYield();
    }

    HitTestWindow *NewWindow(
        LRESULT hitTest = HTCLIENT,
        wxWindow *parent = nullptr,
        const wxPoint& pos = wxPoint(40, 40),
        const wxSize& size = wxSize(180, 140))
    {
        HitTestWindow * const window =
            new HitTestWindow(parent ? parent : m_frame,
                              pos, size, hitTest);
        window->Show();
        return window;
    }

    void PutOnTop(wxWindow *window)
    {
        REQUIRE(::SetWindowPos(GetHwndOf(window), HWND_TOP,
                               0, 0, 0, 0,
                               SWP_NOMOVE | SWP_NOSIZE |
                               SWP_NOACTIVATE) != 0);
    }

    POINT TestPoint(const wxPoint& client = wxPoint(90, 90)) const
    {
        const wxPoint screen = m_frame->ClientToScreen(client);
        return { screen.x, screen.y };
    }

    wxWinUIHitResolution Resolve(
        wxWinUINativeHit& hit,
        WXHWND bridge = nullptr,
        WXHWND inner = nullptr,
        const wxPoint& client = wxPoint(90, 90)) const
    {
        const POINT point = TestPoint(client);
        return wxWinUIResolveNativeHit(
            m_frame, point, bridge, inner, &hit);
    }

    wxFrame *OtherFrame()
    {
        if ( !m_otherFrame )
        {
            m_otherFrame =
                new wxFrame(nullptr, wxID_ANY, "input-router-other",
                            wxPoint(-17000, -18000),
                            wxSize(300, 240));
            m_otherFrame->ShowWithoutActivating();
            wxYield();
        }
        return m_otherFrame;
    }

protected:
    wxFrame *m_frame = nullptr;
    wxFrame *m_otherFrame = nullptr;
};

class CrossThreadChild
{
public:
    explicit CrossThreadChild(HWND parent)
    {
        m_ready = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        m_stop = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        m_done = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if ( !m_ready || !m_stop || !m_done )
            return;

        m_thread = std::thread(
            [this, parent]()
            {
                const HWND hwnd = ::CreateWindowExW(
                    0, L"STATIC", L"opaque-worker-child",
                    WS_CHILD | WS_VISIBLE,
                    40, 40, 180, 140,
                    parent, nullptr, nullptr, nullptr);
                m_hwnd.store(hwnd, std::memory_order_release);
                ::SetEvent(m_ready);
                ::WaitForSingleObject(m_stop, INFINITE);
                if ( hwnd )
                    ::DestroyWindow(hwnd);
                ::SetEvent(m_done);
            });

        for ( unsigned i = 0;
              i != 500 &&
              ::WaitForSingleObject(m_ready, 0) != WAIT_OBJECT_0;
              ++i )
        {
            wxYield();
            ::Sleep(1);
        }
    }

    ~CrossThreadChild()
    {
        if ( m_stop )
            ::SetEvent(m_stop);
        for ( unsigned i = 0;
              m_done && i != 500 &&
              ::WaitForSingleObject(m_done, 0) != WAIT_OBJECT_0;
              ++i )
        {
            wxYield();
            ::Sleep(1);
        }
        if ( m_thread.joinable() )
            m_thread.join();
        if ( m_ready )
            ::CloseHandle(m_ready);
        if ( m_stop )
            ::CloseHandle(m_stop);
        if ( m_done )
            ::CloseHandle(m_done);
    }

    HWND Get() const
        { return m_hwnd.load(std::memory_order_acquire); }

private:
    HANDLE m_ready = nullptr;
    HANDLE m_stop = nullptr;
    HANDLE m_done = nullptr;
    std::atomic<HWND> m_hwnd{nullptr};
    std::thread m_thread;

    wxDECLARE_NO_COPY_CLASS(CrossThreadChild);
};

} // anonymous namespace

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::TransparentSibling",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow(HTCLIENT);
    HitTestWindow * const upper = NewWindow(HTTRANSPARENT);
    PutOnTop(upper);

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    CHECK(hit.GetHitTest() == HTCLIENT);
    CHECK(hit.GetArea() == wxWinUINativeArea::Client);
    CHECK(upper->GetHitCount() == 1);
    CHECK(lower->GetHitCount() == 1);
    CHECK(wxWinUIGetInputRouterCounters().transparentCandidates == 1);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::PublicFindWindowAtPointResolvesSlots",
                 "[winui-input-router][winui-slot-hit-test]")
{
    HitTestWindow * const native =
        NewWindow(HTCLIENT, nullptr,
                  wxPoint(40, 40), wxSize(180, 140));
    wxButton * const first =
        new wxButton(m_frame, wxID_ANY, "first",
                     wxPoint(40, 40), wxSize(180, 140));
    wxButton * const second =
        new wxButton(m_frame, wxID_ANY, "second",
                     wxPoint(40, 40), wxSize(180, 140));
    first->Show();
    second->Show();
    second->Raise();
    wxYield();

    const wxPoint screen = m_frame->ClientToScreen(wxPoint(90, 90));
    CHECK(wxFindWindowAtPoint(screen) == second);

    // USER32's WindowFromPoint() ignores disabled children, while the public
    // wx contract deliberately includes them. The XAML resolver must retain
    // the top slotted identity even though its carrier no longer receives
    // pointer input.
    second->Disable();
    wxYield();
    CHECK(wxFindWindowAtPoint(screen) == second);

    second->Hide();
    wxYield();
    CHECK(wxFindWindowAtPoint(screen) == first);

    first->Hide();
    wxYield();
    CHECK(wxFindWindowAtPoint(screen) == native);
    native->Disable();
    CHECK(wxFindWindowAtPoint(screen) == native);
    native->Enable();

    // A migrated slot must disappear from the old host synchronously and be
    // found through the new island without confusing the covered shell HWND
    // with either TLW.
    wxFrame * const other = OtherFrame();
    REQUIRE(second->Reparent(other));
    second->SetPosition(wxPoint(30, 30));
    second->Enable();
    second->Show();
    wxYield();

    CHECK(wxFindWindowAtPoint(screen) == native);
    const wxPoint otherScreen =
        other->ClientToScreen(wxPoint(70, 70));
    CHECK(wxFindWindowAtPoint(otherScreen) == second);

    second->Destroy();
    wxYield();
    CHECK(wxFindWindowAtPoint(otherScreen) == other);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::DisabledSlotMergesWithFreeXamlByZOrder",
                 "[winui-input-router][winui-slot-hit-test]")
{
    wxButton * const first =
        new wxButton(m_frame, wxID_ANY, "first",
                     wxPoint(40, 40), wxSize(180, 140));
    wxButton * const second =
        new wxButton(m_frame, wxID_ANY, "second",
                     wxPoint(40, 40), wxSize(180, 140));
    first->Show();
    second->Show();
    second->Raise();
    wxYield();
    second->Disable();
    wxYield();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(m_frame);
    REQUIRE(host);
    wxWinUISlot * const firstSlot = host->FindSlot(first);
    wxWinUISlot * const secondSlot = host->FindSlot(second);
    REQUIRE(firstSlot);
    REQUIRE(secondSlot);

    const MUXC::Grid firstContainer = firstSlot->GetContainer();
    const MUXC::Grid secondContainer = secondSlot->GetContainer();
    const int firstZ = MUXC::Canvas::GetZIndex(firstContainer);
    const int secondZ = MUXC::Canvas::GetZIndex(secondContainer);
    REQUIRE(secondZ > firstZ);

    MUXC::Border overlay;
    overlay.Background(
        MUXM::SolidColorBrush(winrt::Microsoft::UI::Colors::Transparent()));
    const double scale = host->GetScale();
    MUXC::Canvas::SetLeft(overlay, 40.0 / scale);
    MUXC::Canvas::SetTop(overlay, 40.0 / scale);
    overlay.Width(180.0 / scale);
    overlay.Height(140.0 / scale);

    // Appending at the enabled slot's Z puts this free branch above `first`
    // at equal Z but still strictly below the omitted disabled `second`.
    MUXC::Canvas::SetZIndex(overlay, secondZ - 1);
    const MUXC::Canvas root = host->Root();
    root.Children().Append(overlay);
    wxYield();

    const wxPoint screen = m_frame->ClientToScreen(wxPoint(90, 90));
    const wxPoint rootPoint = host->ScreenToRootDIP(screen);
    const auto firstNonRoot =
        [&root](const auto& elements) -> MUX::DependencyObject
        {
            for ( const auto& element : elements )
            {
                if ( element != root )
                    return element;
            }
            return nullptr;
        };
    auto hits = MUXM::VisualTreeHelper::FindElementsInHostCoordinates(
        winrt::Windows::Foundation::Point{
            static_cast<float>(rootPoint.x),
            static_cast<float>(rootPoint.y)},
        root, true);
    REQUIRE(firstNonRoot(hits) == overlay);
    CHECK(wxFindWindowAtPoint(screen) == second);

    // The same real free branch becomes authoritative once it is strictly
    // above the disabled slot.
    MUXC::Canvas::SetZIndex(overlay, secondZ + 1);
    wxYield();
    hits = MUXM::VisualTreeHelper::FindElementsInHostCoordinates(
        winrt::Windows::Foundation::Point{
            static_cast<float>(rootPoint.x),
            static_cast<float>(rootPoint.y)},
        root, true);
    REQUIRE(firstNonRoot(hits) == overlay);
    CHECK(wxFindWindowAtPoint(screen) == m_frame);

    std::uint32_t overlayIndex = 0;
    REQUIRE(root.Children().IndexOf(overlay, overlayIndex));
    root.Children().RemoveAt(overlayIndex);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::WindowAtPointSlotSnapshotSurvivesDestruction",
                 "[winui-input-router][winui-slot-hit-test]")
{
    HitTestWindow * const native = NewWindow();
    wxButton * const doomed =
        new wxButton(m_frame, wxID_ANY, "doomed",
                     wxPoint(40, 40), wxSize(180, 140));
    doomed->Show();
    doomed->Disable();
    wxYield();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(m_frame);
    REQUIRE(host);
    REQUIRE(host->FindSlot(doomed));

    const wxPoint screen = m_frame->ClientToScreen(wxPoint(90, 90));
    wxWeakRef<wxWindow> doomedLifetime(doomed);
    wxWindow *callbackWindow = nullptr;
    bool callbackRan = false;
    wxWinUITopLevelHost::TestOnNextWindowAtPointSlotStateRead(
        [&](wxWindow *window)
        {
            callbackRan = true;
            callbackWindow = window;
            window->Destroy();
            wxYield();
        });

    // A structural change at the getter boundary invalidates this operation;
    // the next ordinary call resolves the newly exposed native child.
    CHECK(host->ResolveWindowAtScreenPoint(screen) == nullptr);
    CHECK(callbackRan);
    CHECK(callbackWindow == doomed);
    CHECK(doomedLifetime.get() == nullptr);
    CHECK(wxFindWindowAtPoint(screen) == native);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::HitTestErrorFailsClosed",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow(HTCLIENT);
    HitTestWindow * const upper = NewWindow(HTERROR);
    PutOnTop(upper);

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    CHECK(Resolve(hit) == wxWinUIHitResolution::Unstable);
    CHECK_FALSE(hit.IsOk());
    CHECK(upper->GetHitCount() == 1);
    CHECK(lower->GetHitCount() == 0);
    CHECK(wxWinUIGetInputRouterCounters().resolveUnstable == 1);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::ReentrantResolutionFailsClosed",
                 "[winui-input-router]")
{
    HitTestWindow * const window = NewWindow(HTCLIENT);
    const POINT point = TestPoint();
    wxWinUINativeHit knownHit;
    REQUIRE(Resolve(knownHit) == wxWinUIHitResolution::Hit);
    REQUIRE(knownHit.IsOk());
    const unsigned initialHitCount = window->GetHitCount();

    wxWinUIHitResolution nestedResolution = wxWinUIHitResolution::Miss;
    bool nestedOutputStartedValid = false;
    bool nestedOutputWasValid = true;
    bool callbackRan = false;

    SECTION("native hit")
    {
        window->SetHitAction(
            [this, point, &nestedResolution,
             &nestedOutputStartedValid, &nestedOutputWasValid,
             &callbackRan, &knownHit]()
            {
                callbackRan = true;
                wxWinUINativeHit nestedHit = knownHit;
                nestedOutputStartedValid = nestedHit.IsOk();
                nestedResolution = wxWinUIResolveNativeHit(
                    m_frame, point, nullptr, nullptr, &nestedHit);
                nestedOutputWasValid = nestedHit.IsOk();
            });
    }

    SECTION("window at point")
    {
        window->SetHitAction(
            [this, point, &nestedResolution,
             &nestedOutputStartedValid, &nestedOutputWasValid,
             &callbackRan, &knownHit]()
            {
                callbackRan = true;
                wxWinUINativeTarget nestedTarget = knownHit.GetTarget();
                nestedOutputStartedValid = nestedTarget.IsOk();
                nestedResolution = wxWinUIResolveNativeWindowAtPoint(
                    m_frame, point, nullptr, nullptr, &nestedTarget);
                nestedOutputWasValid = nestedTarget.IsOk();
            });
    }

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == window);
    CHECK(callbackRan);
    CHECK(nestedResolution == wxWinUIHitResolution::Unstable);
    CHECK(nestedOutputStartedValid);
    CHECK_FALSE(nestedOutputWasValid);
    CHECK(window->GetHitCount() == initialHitCount + 1);

    wxWinUIInputRouterCounters counters =
        wxWinUIGetInputRouterCounters();
    CHECK(counters.resolveAttempts == 1);
    CHECK(counters.resolveRestarts == 0);
    CHECK(counters.resolveUnstable == 1);
    CHECK(counters.transparentCandidates == 0);

    // Leaving the outer scope must reopen the resolver for ordinary
    // sequential work on this same UI thread.
    window->SetHitAction({});
    wxWinUINativeHit sequentialHit;
    REQUIRE(Resolve(sequentialHit) == wxWinUIHitResolution::Hit);
    CHECK(sequentialHit.GetTarget().GetWindow() == window);
    CHECK(window->GetHitCount() == initialHitCount + 2);

    counters = wxWinUIGetInputRouterCounters();
    CHECK(counters.resolveAttempts == 2);
    CHECK(counters.resolveRestarts == 0);
    CHECK(counters.resolveUnstable == 1);
    CHECK(counters.transparentCandidates == 0);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::NestedTransparentSibling",
                 "[winui-input-router]")
{
    HitTestWindow * const parent =
        NewWindow(HTCLIENT, nullptr, wxPoint(30, 30), wxSize(240, 190));
    HitTestWindow * const lower =
        NewWindow(HTCLIENT, parent, wxPoint(20, 20), wxSize(160, 120));
    HitTestWindow * const upper =
        NewWindow(HTTRANSPARENT, parent,
                  wxPoint(20, 20), wxSize(160, 120));
    PutOnTop(upper);

    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    CHECK(parent->GetHitCount() == 0);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::VisibilityEnableAndExclusion",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow(HTCLIENT);
    HitTestWindow * const upper = NewWindow(HTCLIENT);
    PutOnTop(upper);

    wxWinUINativeHit hit;

    upper->Hide();
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);

    upper->Show();
    upper->Disable();
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    const POINT framePoint = { 90, 90 };
    CHECK(reinterpret_cast<HWND>(hit.GetTarget().GetLeafHwnd()) ==
          ::ChildWindowFromPointEx(
              GetHwndOf(m_frame), framePoint,
              CWP_SKIPINVISIBLE | CWP_SKIPDISABLED));

    upper->Enable();
    REQUIRE(Resolve(hit, reinterpret_cast<WXHWND>(GetHwndOf(upper))) ==
            wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::TransparentExtendedStyleIsNotClickThrough",
                 "[winui-input-router]")
{
    NewWindow(HTCLIENT);
    HitTestWindow * const upper = NewWindow(HTCLIENT);
    PutOnTop(upper);
    const HWND hwnd = GetHwndOf(upper);
    ::SetWindowLongPtr(hwnd, GWL_EXSTYLE,
                       ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
                           WS_EX_TRANSPARENT);

    // WS_EX_TRANSPARENT controls painting order; unlike HTTRANSPARENT it is
    // not by itself a request to target an underlying sibling.
    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == upper);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::WindowRegionAndNowhereAreTransparent",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow(HTCLIENT);
    HitTestWindow * const upper = NewWindow(HTCLIENT);
    PutOnTop(upper);

    const HWND upperHwnd = GetHwndOf(upper);
    HRGN region = ::CreateRectRgn(0, 0, 180, 140);
    HRGN hole = ::CreateRectRgn(40, 40, 80, 80);
    REQUIRE(region != nullptr);
    REQUIRE(hole != nullptr);
    REQUIRE(::CombineRgn(region, region, hole, RGN_DIFF) != ERROR);
    ::DeleteObject(hole);
    REQUIRE(::SetWindowRgn(upperHwnd, region, TRUE) != 0);
    // USER32 owns `region` after successful SetWindowRgn().

    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    CHECK(upper->GetHitCount() == 0);

    REQUIRE(::SetWindowRgn(upperHwnd, nullptr, TRUE) != 0);
    upper->SetHitTest(HTNOWHERE);
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);

    // SetWindowRgn() interprets an RTL window's region from the upper-right
    // corner. Use an asymmetric hole around this physical point: treating the
    // returned HRGN as ordinary left-origin coordinates would hit `upper`.
    upper->SetHitTest(HTCLIENT);
    ::SetWindowLongPtr(upperHwnd, GWL_EXSTYLE,
                       ::GetWindowLongPtr(upperHwnd, GWL_EXSTYLE) |
                           WS_EX_LAYOUTRTL);
    REQUIRE(::SetWindowPos(upperHwnd, nullptr, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                           SWP_NOACTIVATE | SWP_FRAMECHANGED) != 0);

    RECT upperRect;
    REQUIRE(::GetWindowRect(upperHwnd, &upperRect) != 0);
    const POINT testPoint = TestPoint();
    const int rtlX = upperRect.right - 1 - testPoint.x;
    const int physicalLeftX = testPoint.x - upperRect.left;
    const int localY = testPoint.y - upperRect.top;
    REQUIRE(std::abs(rtlX - physicalLeftX) > 20);

    region = ::CreateRectRgn(
        0, 0,
        upperRect.right - upperRect.left,
        upperRect.bottom - upperRect.top);
    hole = ::CreateRectRgn(rtlX - 5, localY - 5,
                           rtlX + 5, localY + 5);
    REQUIRE(region != nullptr);
    REQUIRE(hole != nullptr);
    REQUIRE(::CombineRgn(region, region, hole, RGN_DIFF) != ERROR);
    ::DeleteObject(hole);
    REQUIRE(::SetWindowRgn(upperHwnd, region, TRUE) != 0);

    const unsigned hitCount = upper->GetHitCount();
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    CHECK(upper->GetHitCount() == hitCount);
    REQUIRE(::SetWindowRgn(upperHwnd, nullptr, TRUE) != 0);

    // Exercise the half-open horizontal boundary explicitly.  In an RTL
    // window GDI region x=0 is the physical rightmost pixel and x=width-1 is
    // the physical leftmost one.
    const int width = upperRect.right - upperRect.left;
    const int height = upperRect.bottom - upperRect.top;
    const POINT physicalLeft =
        { upperRect.left, upperRect.top + height / 2 };
    const POINT physicalRight =
        { upperRect.right - 1, upperRect.top + height / 2 };

    region = ::CreateRectRgn(0, 0, width, height);
    REQUIRE(region != nullptr);
    REQUIRE(::SetWindowRgn(upperHwnd, region, TRUE) != 0);
    REQUIRE(wxWinUIResolveNativeHit(
                m_frame, physicalLeft, nullptr, nullptr, &hit) ==
            wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == upper);
    REQUIRE(wxWinUIResolveNativeHit(
                m_frame, physicalRight, nullptr, nullptr, &hit) ==
            wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == upper);

    region = ::CreateRectRgn(0, 0, 1, height);
    REQUIRE(region != nullptr);
    REQUIRE(::SetWindowRgn(upperHwnd, region, TRUE) != 0);
    REQUIRE(wxWinUIResolveNativeHit(
                m_frame, physicalRight, nullptr, nullptr, &hit) ==
            wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == upper);
    REQUIRE(wxWinUIResolveNativeHit(
                m_frame, physicalLeft, nullptr, nullptr, &hit) ==
            wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    REQUIRE(::SetWindowRgn(upperHwnd, nullptr, TRUE) != 0);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::ClientAndNonClientCoordinates",
                 "[winui-input-router]")
{
    HitTestWindow * const window = NewWindow(HTVSCROLL);
    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == window);
    CHECK(hit.GetHitTest() == HTVSCROLL);
    CHECK(hit.GetArea() == wxWinUINativeArea::NonClient);

    POINT expected = hit.GetScreenPoint();
    ::SetLastError(ERROR_SUCCESS);
    const int mapped =
        ::MapWindowPoints(HWND_DESKTOP, GetHwndOf(window), &expected, 1);
    REQUIRE((mapped != 0 || ::GetLastError() == ERROR_SUCCESS));
    CHECK(hit.GetClientPoint().x == expected.x);
    CHECK(hit.GetClientPoint().y == expected.y);

    const HWND hwnd = GetHwndOf(window);
    ::SetWindowLongPtr(hwnd, GWL_EXSTYLE,
                       ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
                           WS_EX_LAYOUTRTL);
    REQUIRE(::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                          SWP_NOACTIVATE | SWP_FRAMECHANGED) != 0);

    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    expected = hit.GetScreenPoint();
    ::SetLastError(ERROR_SUCCESS);
    const int mappedRTL =
        ::MapWindowPoints(HWND_DESKTOP, hwnd, &expected, 1);
    REQUIRE((mappedRTL != 0 || ::GetLastError() == ERROR_SUCCESS));
    CHECK(hit.GetClientPoint().x == expected.x);
    CHECK(hit.GetClientPoint().y == expected.y);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::IdentityInvalidates",
                 "[winui-input-router]")
{
    SECTION("destroy")
    {
        HitTestWindow * const window = NewWindow();
        wxWinUINativeHit hit;
        REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
        REQUIRE(hit.IsValid());

        delete window;
        CHECK_FALSE(hit.IsValid());
    }

    SECTION("cross TLW reparent")
    {
        HitTestWindow * const window = NewWindow();
        wxWinUINativeHit hit;
        REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
        REQUIRE(hit.IsValid());

        REQUIRE(window->Reparent(OtherFrame()));
        CHECK_FALSE(hit.IsValid());
    }

    SECTION("internal leaf lifetime")
    {
        HitTestWindow * const window = NewWindow();
        const HWND internal = ::CreateWindowExW(
            0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 0, 120, 100, GetHwndOf(window), nullptr,
            wxGetInstance(), nullptr);
        REQUIRE(internal != nullptr);

        RECT internalRect;
        REQUIRE(::GetWindowRect(internal, &internalRect) != 0);
        const POINT internalPoint =
        {
            (internalRect.left + internalRect.right) / 2,
            (internalRect.top + internalRect.bottom) / 2
        };

        wxWinUINativeHit hit;
        REQUIRE(wxWinUIResolveNativeHit(
                    m_frame, internalPoint, nullptr, nullptr, &hit) ==
                wxWinUIHitResolution::Hit);
        CHECK(hit.GetTarget().GetWindow() == window);
        CHECK(reinterpret_cast<HWND>(hit.GetTarget().GetLeafHwnd()) ==
              internal);
        CHECK(hit.GetTarget().GetLeafGeneration() != 0);
        REQUIRE(hit.IsValid());
        const unsigned long long firstGeneration =
            hit.GetTarget().GetLeafGeneration();

        wxWinUINativeTarget captured;
        REQUIRE(wxWinUIGetNativeTarget(
            m_frame, reinterpret_cast<WXHWND>(internal), &captured));
        CHECK(captured.Matches(hit.GetTarget()));
        CHECK_FALSE(wxWinUIGetNativeTarget(
            OtherFrame(), reinterpret_cast<WXHWND>(internal), &captured));
        CHECK_FALSE(captured.IsOk());
        CHECK_FALSE(wxWinUIGetNativeTarget(
            m_frame, reinterpret_cast<WXHWND>(internal), nullptr));

        REQUIRE(::DestroyWindow(internal) != 0);
        CHECK_FALSE(hit.IsValid());

        const HWND replacement = ::CreateWindowExW(
            0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 0, 120, 100, GetHwndOf(window), nullptr,
            wxGetInstance(), nullptr);
        REQUIRE(replacement != nullptr);
        const unsigned long long replacementGeneration =
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(replacement));
        CHECK(replacementGeneration != 0);
        CHECK(replacementGeneration != firstGeneration);
        REQUIRE(::DestroyWindow(replacement) != 0);
    }
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::MutationDuringHitTestRestarts",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow();
    HitTestWindow * const upper = NewWindow();
    PutOnTop(upper);
    bool mutated = false;
    upper->SetHitAction([upper, &mutated]()
    {
        if ( !mutated )
        {
            mutated = true;
            upper->Hide();
        }
    });

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
    CHECK(hit.GetTarget().GetWindow() == lower);
    CHECK(wxWinUIGetInputRouterCounters().resolveRestarts == 1);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::NativeStateMutationRestarts",
                 "[winui-input-router]")
{
    SECTION("window geometry")
    {
        NewWindow();
        HitTestWindow * const upper = NewWindow();
        PutOnTop(upper);
        const HWND hwnd = GetHwndOf(upper);
        bool mutated = false;
        bool mutationSucceeded = false;
        upper->SetHitAction([hwnd, &mutated, &mutationSucceeded]()
        {
            if ( mutated )
                return;

            mutated = true;
            RECT rect;
            POINT origin;
            if ( !::GetWindowRect(hwnd, &rect) )
                return;
            origin = { rect.left, rect.top };
            ::SetLastError(ERROR_SUCCESS);
            if ( !::MapWindowPoints(
                     HWND_DESKTOP, ::GetParent(hwnd), &origin, 1) &&
                 ::GetLastError() != ERROR_SUCCESS )
            {
                return;
            }
            mutationSucceeded =
                ::SetWindowPos(hwnd, nullptr, origin.x + 1, origin.y,
                               0, 0,
                               SWP_NOSIZE | SWP_NOZORDER |
                               SWP_NOACTIVATE) != 0;
        });

        wxWinUIResetInputRouterCounters();
        wxWinUINativeHit hit;
        REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
        REQUIRE(mutationSucceeded);
        CHECK(hit.GetTarget().GetWindow() == upper);
        CHECK(upper->GetHitCount() == 2);
        CHECK(wxWinUIGetInputRouterCounters().resolveRestarts == 1);
    }

    SECTION("window style")
    {
        NewWindow();
        HitTestWindow * const upper = NewWindow();
        PutOnTop(upper);
        const HWND hwnd = GetHwndOf(upper);
        bool mutated = false;
        bool mutationSucceeded = false;
        upper->SetHitAction([hwnd, &mutated, &mutationSucceeded]()
        {
            if ( mutated )
                return;

            mutated = true;
            ::SetLastError(ERROR_SUCCESS);
            const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
            if ( !style && ::GetLastError() != ERROR_SUCCESS )
                return;

            ::SetLastError(ERROR_SUCCESS);
            const LONG_PTR previous = ::SetWindowLongPtr(
                hwnd, GWL_STYLE, style ^ WS_TABSTOP);
            mutationSucceeded =
                previous != 0 || ::GetLastError() == ERROR_SUCCESS;
        });

        wxWinUIResetInputRouterCounters();
        wxWinUINativeHit hit;
        REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
        REQUIRE(mutationSucceeded);
        CHECK(hit.GetTarget().GetWindow() == upper);
        CHECK(upper->GetHitCount() == 2);
        CHECK(wxWinUIGetInputRouterCounters().resolveRestarts == 1);
    }

    SECTION("window region")
    {
        NewWindow();
        HitTestWindow * const upper = NewWindow();
        PutOnTop(upper);
        const HWND hwnd = GetHwndOf(upper);
        bool mutated = false;
        bool mutationSucceeded = false;
        upper->SetHitAction([hwnd, &mutated, &mutationSucceeded]()
        {
            if ( mutated )
                return;

            mutated = true;
            RECT rect;
            if ( !::GetWindowRect(hwnd, &rect) )
                return;

            HRGN region = ::CreateRectRgn(
                0, 0, rect.right - rect.left, rect.bottom - rect.top);
            if ( !region )
                return;
            mutationSucceeded = ::SetWindowRgn(hwnd, region, FALSE) != 0;
            if ( !mutationSucceeded )
                ::DeleteObject(region);
            // USER32 owns `region` after successful SetWindowRgn().
        });

        wxWinUIResetInputRouterCounters();
        wxWinUINativeHit hit;
        REQUIRE(Resolve(hit) == wxWinUIHitResolution::Hit);
        REQUIRE(mutationSucceeded);
        CHECK(hit.GetTarget().GetWindow() == upper);
        CHECK(upper->GetHitCount() == 2);
        CHECK(wxWinUIGetInputRouterCounters().resolveRestarts == 1);
    }
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::RefreshRejectsGeometryAndZoneChanges",
                 "[winui-input-router]")
{
    HitTestWindow * const lower = NewWindow();
    HitTestWindow * const upper = NewWindow();
    PutOnTop(upper);

    wxWinUINativeHit expected;
    REQUIRE(Resolve(expected) == wxWinUIHitResolution::Hit);
    REQUIRE(expected.GetTarget().GetWindow() == upper);

    // A synchronous callback may change Z order while leaving both HWNDs and
    // all generations alive. Identity-only validation must reject it.
    PutOnTop(lower);
    wxWinUINativeHit refreshed;
    CHECK_FALSE(wxWinUIRefreshNativeHit(
        m_frame, expected, nullptr, nullptr, &refreshed, true));

    PutOnTop(upper);
    REQUIRE(Resolve(expected) == wxWinUIHitResolution::Hit);
    upper->SetHitTest(HTVSCROLL);
    CHECK_FALSE(wxWinUIRefreshNativeHit(
        m_frame, expected, nullptr, nullptr, &refreshed, true));
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::RefreshRejectsDestroyedExclusion",
                 "[winui-input-router]")
{
    HitTestWindow * const native = NewWindow();
    const HWND bridge = ::CreateWindowExW(
        0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        40, 40, 180, 140, GetHwndOf(m_frame), nullptr,
        wxGetInstance(), nullptr);
    REQUIRE(bridge != nullptr);
    PutOnTop(native);
    REQUIRE(::SetWindowPos(bridge, HWND_TOP, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) != 0);

    wxWinUINativeHit expected;
    REQUIRE(Resolve(expected, reinterpret_cast<WXHWND>(bridge)) ==
            wxWinUIHitResolution::Hit);
    CHECK(expected.GetTarget().GetWindow() == native);
    CHECK(expected.GetBridgeExclusionGeneration() != 0);

    REQUIRE(::DestroyWindow(bridge) != 0);
    wxWinUINativeHit refreshed;
    CHECK_FALSE(wxWinUIRefreshNativeHit(
        m_frame, expected, reinterpret_cast<WXHWND>(bridge), nullptr,
        &refreshed));
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::UnstableTopologyFailsClosed",
                 "[winui-input-router]")
{
    HitTestWindow * const toggled = NewWindow();
    HitTestWindow * const upper = NewWindow();
    PutOnTop(upper);
    upper->SetHitAction([toggled]()
    {
        toggled->Show(!toggled->IsShown());
    });

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    CHECK(Resolve(hit) == wxWinUIHitResolution::Unstable);
    CHECK_FALSE(hit.IsOk());

    const wxWinUIInputRouterCounters counters =
        wxWinUIGetInputRouterCounters();
    CHECK(counters.resolveAttempts == 3);
    CHECK(counters.resolveRestarts == 3);
    CHECK(counters.resolveUnstable == 1);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::CrossThreadChildFailsClosed",
                 "[winui-input-router]")
{
    NewWindow(HTCLIENT);
    CrossThreadChild opaque(GetHwndOf(m_frame));
    REQUIRE(opaque.Get() != nullptr);
    // The worker deliberately has no message pump. Avoid any synchronous
    // cross-thread window-position call here: it would wait forever for the
    // worker to process WM_WINDOWPOSCHANGING/WM_WINDOWPOSCHANGED and test the
    // harness rather than the resolver's fail-closed traversal.

    wxWinUIResetInputRouterCounters();
    wxWinUINativeHit hit;
    CHECK(Resolve(hit) == wxWinUIHitResolution::Unstable);
    CHECK_FALSE(hit.IsOk());
    CHECK(wxWinUIGetInputRouterCounters().resolveUnstable == 1);
}

TEST_CASE_METHOD(WinUIInputRouterFixture,
                 "WinUIInputRouter::CrossThreadTLWFailsClosed",
                 "[winui-input-router]")
{
    NewWindow(HTCLIENT);
    const POINT point = TestPoint();
    std::atomic<int> resolution{
        static_cast<int>(wxWinUIHitResolution::Miss)};
    std::atomic<bool> hitWasValid{true};

    wxWinUIResetInputRouterCounters();
    std::thread worker([this, point, &resolution, &hitWasValid]()
    {
        wxWinUINativeHit hit;
        resolution.store(
            static_cast<int>(wxWinUIResolveNativeHit(
                m_frame, point, nullptr, nullptr, &hit)),
            std::memory_order_release);
        hitWasValid.store(hit.IsOk(), std::memory_order_release);
    });
    worker.join();

    CHECK(static_cast<wxWinUIHitResolution>(
              resolution.load(std::memory_order_acquire)) ==
          wxWinUIHitResolution::Unstable);
    CHECK_FALSE(hitWasValid.load(std::memory_order_acquire));
    CHECK(wxWinUIGetInputRouterCounters().resolveUnstable == 1);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
