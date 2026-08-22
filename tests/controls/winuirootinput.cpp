///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuirootinput.cpp
// Purpose:     adapter-level tests of the WinUI root pointer router
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/window.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/winui/private/tlwhost.h"

#include <functional>
#include <memory>
#include <utility>

namespace
{

void DrainRootInputDispatch(int rounds = 8)
{
    for ( int i = 0; i != rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

class RootRouteWindow : public wxWindow
{
public:
    RootRouteWindow(wxWindow *parent,
                    const wxPoint& pos,
                    const wxSize& size)
        : wxWindow(parent, wxID_ANY, pos, size)
    {
    }

    void SetHitTest(LRESULT hitTest) { m_hitTest = hitTest; }
    void OnNextSetCursor(std::function<void ()> callback)
        { m_setCursorCallback = std::move(callback); }
    void OnNextCancelMode(std::function<void ()> callback)
        { m_cancelModeCallback = std::move(callback); }
    void ClearSetCursorCallback() { m_setCursorCallback = nullptr; }
    void ClearCancelModeCallback() { m_cancelModeCallback = nullptr; }
    bool HasSetCursorCallback() const
        { return static_cast<bool>(m_setCursorCallback); }
    bool HasCancelModeCallback() const
        { return static_cast<bool>(m_cancelModeCallback); }
    void CaptureOnButtonDown(bool enable = true)
        { m_captureOnButtonDown = enable; }

    unsigned GetMoveCount() const { return m_moveCount; }
    unsigned GetLeaveCount() const { return m_leaveCount; }
    unsigned GetSetCursorCount() const { return m_setCursorCount; }
    unsigned GetCancelModeCount() const { return m_cancelModeCount; }
    unsigned GetLeftDownCount() const { return m_leftDownCount; }
    unsigned GetRightDownCount() const { return m_rightDownCount; }
    unsigned GetMiddleDownCount() const { return m_middleDownCount; }
    unsigned GetLeftUpCount() const { return m_leftUpCount; }
    unsigned GetMiddleUpCount() const { return m_middleUpCount; }

    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        switch ( message )
        {
            case WM_NCHITTEST:
                return m_hitTest;

            case WM_SETCURSOR:
            {
                ++m_setCursorCount;
                std::function<void ()> callback =
                    std::move(m_setCursorCallback);
                m_setCursorCallback = nullptr;
                if ( callback )
                    callback();
                break;
            }

            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
                ++m_moveCount;
                break;

            case WM_MOUSELEAVE:
            case WM_NCMOUSELEAVE:
                ++m_leaveCount;
                break;

            case WM_LBUTTONDOWN:
                ++m_leftDownCount;
                if ( m_captureOnButtonDown )
                    ::SetCapture(GetHwndOf(this));
                return 0;

            case WM_RBUTTONDOWN:
                ++m_rightDownCount;
                if ( m_captureOnButtonDown )
                    ::SetCapture(GetHwndOf(this));
                return 0;

            case WM_MBUTTONDOWN:
                ++m_middleDownCount;
                if ( m_captureOnButtonDown )
                    ::SetCapture(GetHwndOf(this));
                return 0;

            case WM_LBUTTONUP:
                ++m_leftUpCount;
                if ( ::GetCapture() == GetHwndOf(this) )
                    ::ReleaseCapture();
                return 0;

            case WM_MBUTTONUP:
                ++m_middleUpCount;
                if ( ::GetCapture() == GetHwndOf(this) )
                    ::ReleaseCapture();
                return 0;

            case WM_CANCELMODE:
            {
                ++m_cancelModeCount;
                std::function<void ()> callback =
                    std::move(m_cancelModeCallback);
                m_cancelModeCallback = nullptr;
                if ( callback )
                    callback();
                // Do not let DefWindowProc release capture established by a
                // nested replacement gesture: the host owns that decision.
                return 0;
            }
        }

        return wxWindow::MSWWindowProc(message, wParam, lParam);
    }

private:
    LRESULT m_hitTest = HTCLIENT;
    std::function<void ()> m_setCursorCallback;
    std::function<void ()> m_cancelModeCallback;
    bool m_captureOnButtonDown = false;
    unsigned m_moveCount = 0;
    unsigned m_leaveCount = 0;
    unsigned m_setCursorCount = 0;
    unsigned m_cancelModeCount = 0;
    unsigned m_leftDownCount = 0;
    unsigned m_rightDownCount = 0;
    unsigned m_middleDownCount = 0;
    unsigned m_leftUpCount = 0;
    unsigned m_middleUpCount = 0;
};

class WinUIRootInputFixture
{
public:
    WinUIRootInputFixture()
    {
        m_frame = new wxFrame(nullptr, wxID_ANY, "root-input-test",
                              wxPoint(-20000, -20000),
                              wxSize(420, 320));
        m_frame->SetClientSize(380, 280);
        m_window = new RootRouteWindow(
            m_frame, wxPoint(40, 40), wxSize(220, 160));
        m_window->Show();
        // Keep the real HWND/XamlRoot visible but outside every monitor and
        // never activate it: these adapter tests must not race the user's
        // physical pointer or steal foreground focus.
        m_frame->ShowWithoutActivating();
        DrainRootInputDispatch();

        m_host = wxWinUITopLevelHost::ForWindow(m_frame, true);
        REQUIRE(m_host != nullptr);
        DrainRootInputDispatch();
    }

    ~WinUIRootInputFixture()
    {
        if ( m_window )
        {
            m_window->ClearSetCursorCallback();
            m_window->ClearCancelModeCallback();
        }
        if ( m_frame )
        {
            m_frame->Destroy();
            DrainRootInputDispatch();
        }
    }

    wxWinUIPointerSample Sample() const
    {
        RECT rect;
        REQUIRE(::GetWindowRect(GetHwndOf(m_window), &rect) != 0);

        wxWinUIPointerSample sample;
        sample.device = wxWinUIInputDevice::Mouse;
        sample.kind = wxWinUIInputKind::Move;
        sample.pointerId = 1;
        sample.isPrimary = true;
        sample.screenX = (rect.left + rect.right) / 2;
        sample.screenY = (rect.top + rect.bottom) / 2;
        sample.timestamp = ::GetTickCount64();
        return sample;
    }

protected:
    wxFrame *m_frame = nullptr;
    RootRouteWindow *m_window = nullptr;
    wxWinUITopLevelHost *m_host = nullptr;
};

} // anonymous namespace

TEST_CASE_METHOD(
    WinUIRootInputFixture,
    "WinUIRootInput::RefreshFailureRollsBackUnpublishedHover",
    "[winui-root-input]")
{
    wxWinUIPointerSample sample = Sample();
    m_window->OnNextSetCursor(
        [this]()
        {
            // MirrorNativeCursor() is callback-bearing. Change only the hit
            // zone while keeping the same live HWND and geometry so the
            // post-callback refresh, not identity validation, rejects it.
            m_window->SetHitTest(HTVSCROLL);
        });

    const wxWinUIRootPointerOutcome rejected =
        m_host->TestRouteRootPointerSample(sample);
    CHECK_FALSE(m_window->HasSetCursorCallback());
    CHECK(rejected.status == wxWinUIRootPointerStatus::FailedClosed);
    CHECK(rejected.disposition == wxWinUIInputDisposition::Routed);
    CHECK(rejected.preparedActionCount == 2);
    CHECK(rejected.cancelSource);
    CHECK(m_window->GetSetCursorCount() == 1);
    CHECK(m_window->GetMoveCount() == 0);
    CHECK(m_window->GetLeaveCount() == 0);

    // The failed dispatch must have rolled back both semantic and native
    // hover prefixes. A clean sample enters and publishes exactly once.
    m_window->SetHitTest(HTCLIENT);
    ++sample.timestamp;
    const wxWinUIRootPointerOutcome delivered =
        m_host->TestRouteRootPointerSample(sample);
    CHECK(delivered.status == wxWinUIRootPointerStatus::Completed);
    CHECK(delivered.disposition == wxWinUIInputDisposition::Routed);
    CHECK(delivered.preparedActionCount == 2);
    CHECK(m_window->GetMoveCount() == 1);
    CHECK(m_window->GetLeaveCount() == 0);

    // Moving to XAML retires the published native hover exactly once.
    ++sample.timestamp;
    const wxWinUIRootPointerOutcome xaml =
        m_host->TestRouteRootPointerSample(sample, true);
    CHECK(xaml.status == wxWinUIRootPointerStatus::Completed);
    CHECK(xaml.disposition == wxWinUIInputDisposition::Xaml);
    CHECK(xaml.preparedActionCount == 1);
    CHECK(m_window->GetMoveCount() == 1);
    CHECK(m_window->GetLeaveCount() == 1);

    ++sample.timestamp;
    const wxWinUIRootPointerOutcome repeatedXaml =
        m_host->TestRouteRootPointerSample(sample, true);
    CHECK(repeatedXaml.status == wxWinUIRootPointerStatus::Completed);
    CHECK(repeatedXaml.preparedActionCount == 0);
    CHECK(m_window->GetLeaveCount() == 1);
}

TEST_CASE_METHOD(
    WinUIRootInputFixture,
    "WinUIRootInput::SetCursorReentranceSupersedesOuter",
    "[winui-root-input]")
{
    wxWinUIPointerSample outer = Sample();
    struct ReentryState
    {
        wxWinUIPointerSample sample;
        wxWinUIRootPointerOutcome outcome;
        bool called = false;
    };
    const std::shared_ptr<ReentryState> reentry =
        std::make_shared<ReentryState>();
    reentry->sample = outer;
    ++reentry->sample.timestamp;

    m_window->OnNextSetCursor(
        [this, reentry]()
        {
            reentry->called = true;
            reentry->outcome =
                m_host->TestRouteRootPointerSample(
                    reentry->sample, true);
        });

    const wxWinUIRootPointerOutcome outerOutcome =
        m_host->TestRouteRootPointerSample(outer);
    CHECK_FALSE(m_window->HasSetCursorCallback());
    REQUIRE(reentry->called);
    CHECK(reentry->outcome.status ==
          wxWinUIRootPointerStatus::Completed);
    CHECK(reentry->outcome.disposition ==
          wxWinUIInputDisposition::Xaml);
    CHECK(outerOutcome.status == wxWinUIRootPointerStatus::Superseded);
    CHECK(outerOutcome.disposition == wxWinUIInputDisposition::Routed);
    CHECK(outerOutcome.cancelSource);
    CHECK(m_window->GetMoveCount() == 0);
    CHECK(m_window->GetLeaveCount() == 0);

    // The nested transition won and the stale outer dispatch stayed silent.
    // A later authoritative native sample can establish hover normally.
    outer.timestamp += 2;
    const wxWinUIRootPointerOutcome recovered =
        m_host->TestRouteRootPointerSample(outer);
    CHECK(recovered.status == wxWinUIRootPointerStatus::Completed);
    CHECK(recovered.disposition == wxWinUIInputDisposition::Routed);
    CHECK(recovered.preparedActionCount == 2);
    CHECK(m_window->GetMoveCount() == 1);
    CHECK(m_window->GetLeaveCount() == 0);
}

TEST_CASE_METHOD(
    WinUIRootInputFixture,
    "WinUIRootInput::ModalBoundaryCancelsEveryButtonBeforeReentry",
    "[winui-root-input]")
{
    m_window->CaptureOnButtonDown();

    wxWinUIPointerSample left = Sample();
    left.kind = wxWinUIInputKind::Press;
    left.button = wxWinUIInputButton::Left;
    left.buttonMask = MK_LBUTTON;
    const wxWinUIRootPointerOutcome leftDown =
        m_host->TestRouteRootPointerSample(left);
    REQUIRE(leftDown.status == wxWinUIRootPointerStatus::Completed);
    REQUIRE(leftDown.pointerDispatch.gestureSerial != 0);
    REQUIRE(leftDown.pointerDispatch.button ==
            wxWinUIInputButton::Left);
    CHECK(m_window->GetLeftDownCount() == 1);
    CHECK(::GetCapture() == GetHwndOf(m_window));

    wxWinUIPointerSample right = left;
    ++right.timestamp;
    right.button = wxWinUIInputButton::Right;
    right.buttonMask = MK_LBUTTON | MK_RBUTTON;
    const wxWinUIRootPointerOutcome rightDown =
        m_host->TestRouteRootPointerSample(right);
    REQUIRE(rightDown.status == wxWinUIRootPointerStatus::Completed);
    REQUIRE(rightDown.pointerDispatch.gestureSerial != 0);
    REQUIRE(rightDown.pointerDispatch.button ==
            wxWinUIInputButton::Right);
    CHECK(m_window->GetRightDownCount() == 1);

    // These are the exact committed actions returned by the production
    // state machine, including their serials; no synthetic PressState is
    // fabricated by this test.
    REQUIRE(m_host->TestRememberModalAwaitingRelease(
        leftDown.pointerDispatch));
    REQUIRE(m_host->TestRememberModalAwaitingRelease(
        rightDown.pointerDispatch));
    REQUIRE(m_host->GetModalAwaitingReleaseCountForTest() == 2);

    struct CancelReentryState
    {
        wxWinUIPointerSample press;
        wxWinUIRootPointerOutcome outcome;
        bool called = false;
    };
    const std::shared_ptr<CancelReentryState> reentry =
        std::make_shared<CancelReentryState>();
    reentry->press = left;
    reentry->press.timestamp = right.timestamp + 1;
    reentry->press.button = wxWinUIInputButton::Middle;
    reentry->press.buttonMask = MK_MBUTTON;

    // The first old WM_CANCELMODE starts an independent middle-button
    // gesture. (The exact old left button remains tombstoned until this
    // callback returns and is intentionally not reusable yet.) The outer
    // executor must emergency-acknowledge the second old tombstone without
    // releasing or canceling this nested capture/press.
    m_window->OnNextCancelMode(
        [this, reentry]()
        {
            reentry->called = true;
            reentry->outcome =
                m_host->TestRouteRootPointerSample(reentry->press);
        });

    wxWinUIPointerSample leave = right;
    ++leave.timestamp;
    leave.button = wxWinUIInputButton::None;
    const bool boundaryCompleted =
        m_host->TestRouteRootPointerBoundary(false, leave);
    CHECK_FALSE(boundaryCompleted);
    CHECK_FALSE(m_window->HasCancelModeCallback());
    REQUIRE(reentry->called);
    CHECK(reentry->outcome.status ==
          wxWinUIRootPointerStatus::Completed);
    CHECK(reentry->outcome.pointerDispatch.gestureSerial != 0);
    CHECK(reentry->outcome.pointerDispatch.gestureSerial !=
          leftDown.pointerDispatch.gestureSerial);
    CHECK(m_window->GetMiddleDownCount() == 1);
    CHECK(m_window->GetCancelModeCount() == 2);
    CHECK(m_host->GetModalAwaitingReleaseCountForTest() == 0);
    CHECK(::GetCapture() == GetHwndOf(m_window));

    wxWinUIPointerSample release = reentry->press;
    ++release.timestamp;
    release.kind = wxWinUIInputKind::Release;
    release.buttonMask = 0;
    const wxWinUIRootPointerOutcome released =
        m_host->TestRouteRootPointerSample(release);
    CHECK(released.status == wxWinUIRootPointerStatus::Completed);
    CHECK(released.pointerDispatch.balancesPress);
    CHECK(released.pointerDispatch.gestureSerial ==
          reentry->outcome.pointerDispatch.gestureSerial);
    CHECK(m_window->GetMiddleUpCount() == 1);
    CHECK(m_window->GetLeftUpCount() == 0);
    CHECK(m_window->GetCancelModeCount() == 2);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE_METHOD(
    WinUIRootInputFixture,
    "WinUIRootInput::CaptureLossAndReleaseOutside",
    "[winui-root-input]")
{
    m_window->CaptureOnButtonDown();

    wxWinUIPointerSample press = Sample();
    press.kind = wxWinUIInputKind::Press;
    press.button = wxWinUIInputButton::Left;
    press.buttonMask = MK_LBUTTON;
    const wxWinUIRootPointerOutcome down =
        m_host->TestRouteRootPointerSample(press);
    REQUIRE(down.status == wxWinUIRootPointerStatus::Completed);
    REQUIRE(down.pointerDispatch.gestureSerial != 0);
    REQUIRE(::GetCapture() == GetHwndOf(m_window));

    SECTION("release outside remains balanced through capture")
    {
        RECT frameRect;
        REQUIRE(::GetWindowRect(GetHwndOf(m_frame), &frameRect) != 0);

        wxWinUIPointerSample release = press;
        ++release.timestamp;
        release.kind = wxWinUIInputKind::Release;
        release.buttonMask = 0;
        release.screenX = frameRect.right + 40;
        release.screenY = frameRect.bottom + 40;
        const wxWinUIRootPointerOutcome outcome =
            m_host->TestRouteRootPointerSample(release);
        CHECK(outcome.status == wxWinUIRootPointerStatus::Completed);
        CHECK(outcome.pointerDispatch.balancesPress);
        CHECK(outcome.pointerDispatch.gestureSerial ==
              down.pointerDispatch.gestureSerial);
        CHECK(m_window->GetLeftUpCount() == 1);
        CHECK(m_window->GetCancelModeCount() == 0);
        CHECK(::GetCapture() == nullptr);
    }

    SECTION("lost native capture cancels the exact press once")
    {
        REQUIRE(::ReleaseCapture() != 0);

        wxWinUIPointerSample lost = press;
        ++lost.timestamp;
        lost.kind = wxWinUIInputKind::CaptureLost;
        lost.button = wxWinUIInputButton::None;
        lost.buttonMask = 0;
        m_host->TestHandleRootPointerInterrupted(true, lost);
        CHECK(m_window->GetCancelModeCount() == 1);
        CHECK(::GetCapture() == nullptr);

        wxWinUIPointerSample lateRelease = lost;
        ++lateRelease.timestamp;
        lateRelease.kind = wxWinUIInputKind::Release;
        lateRelease.button = wxWinUIInputButton::Left;
        const wxWinUIRootPointerOutcome outcome =
            m_host->TestRouteRootPointerSample(lateRelease);
        CHECK(outcome.status == wxWinUIRootPointerStatus::Completed);
        CHECK_FALSE(outcome.pointerDispatch.balancesPress);
        CHECK(m_window->GetLeftUpCount() == 0);
        CHECK(m_window->GetCancelModeCount() == 1);
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3
