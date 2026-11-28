///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuichromefeedback.cpp
// Purpose:     deterministic WinUI status/hyperlink/activity contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
#include "feedback-test-access.h"
#include "statusbar-test-access.h"
#include "../../src/winui/hostresize.h"

#include "wx/app.h"
#include "wx/dcclient.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/msw/private.h"
#include "wx/scopeguard.h"
#include "wx/winui/private/appearance.h"
#include "appearance-test-access.h"
#include "host-resize-test-access.h"
#include "host-input-test-access.h"
#include "keyboard-test-access.h"
#include "wx/winui/private/tlwhost.h"
#include "tooltip-policy-test-access.h"
#include "host-content-test-access.h"
#include "wx/winui/winui.h"

#if wxUSE_STATUSBAR
    #include "wx/display.h"
    #include "wx/statusbr.h"
#endif
#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
#endif
#if wxUSE_ACTIVITYINDICATOR
    #include "wx/activityindicator.h"
#endif
#if wxUSE_BUTTON
    #include "wx/button.h"
#endif

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUI = winrt::Microsoft::UI::Input;

namespace
{

class YieldOnceLogTarget final : public wxLog
{
public:
    YieldOnceLogTarget()
        : m_previous(wxLog::SetActiveTarget(this))
    {
    }

    ~YieldOnceLogTarget() override
    {
        wxLog::SetActiveTarget(m_previous);
    }

    bool DidYield() const { return m_didYield; }

protected:
    void DoLogText(const wxString&) override
    {
        if ( m_didYield )
            return;

        m_didYield = true;
        wxYield();
    }

private:
    wxLog* m_previous;
    bool m_didYield = false;
};

wxFont MakeChromeTestFont()
{
    return wxFont(wxFontInfo(12).FaceName("Segoe UI").Bold().Italic());
}

void CheckChromeFontLocals(const wxWinUIAppearanceSnapshot& snapshot,
                           bool expected)
{
    CHECK(snapshot.hasFontFamily == expected);
    CHECK(snapshot.hasFontSize == expected);
    CHECK(snapshot.hasFontWeight == expected);
    CHECK(snapshot.hasFontStyle == expected);
}

class HighContrastOverrideRestorer final
{
public:
    explicit HighContrastOverrideRestorer(
        wxWinUIAppearanceTestAccess::HighContrastOverride value)
    {
        Set(value);
    }

    ~HighContrastOverrideRestorer()
    {
        wxWinUIAppearanceTestAccess::SetHighContrastOverride(
            wxWinUIAppearanceTestAccess::HighContrastOverride::System);
    }

    void Set(wxWinUIAppearanceTestAccess::HighContrastOverride value)
    {
        wxWinUIAppearanceTestAccess::SetHighContrastOverride(value);
    }

private:
    wxDECLARE_NO_COPY_CLASS(HighContrastOverrideRestorer);
};

#if wxUSE_STATUSBAR

template <typename Predicate>
bool WaitForStatusBarDispatch(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 500 )
    {
        wxTheApp->ProcessPendingEvents();
        if ( predicate() )
            return true;
        if ( wxEventLoopBase::GetActive() != loop ||
             loop->DispatchTimeout(5) == 0 )
            return false;
    }
    return predicate();
}

wxPoint StatusBarResizeTestPosition()
{
    wchar_t desktop[128]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(
             ::GetThreadDesktop(::GetCurrentThreadId()),
             UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") )
        return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

// Only the physical-contact observation is substituted. USER32 receives the
// production non-client request and owns the real sizing loop. These tests run
// on the isolated desktop, never inject input, and cancel their own loop as
// soon as native entry is observed. Held-drag movement needs a physical test.
class NativeResizeContactOverride final
{
public:
    NativeResizeContactOverride()
    {
        wxWinUIHostResizeTestAccess::SetContactReader(
            [](void *context)
            {
                return static_cast<NativeResizeContactOverride *>(
                    context)->down;
            }, this);
    }

    ~NativeResizeContactOverride()
    {
        wxWinUIHostResizeTestAccess::SetContactReader(nullptr);
    }

    bool down = true;
};

class NativeResizeFrame final : public wxFrame
{
public:
    static constexpr UINT RoutedInputMessage = WM_APP + 73;

    NativeResizeFrame()
        : wxFrame(nullptr, wxID_ANY, "native grip transaction",
                  wxDefaultPosition, wxSize(420, 240))
    {
    }

    ~NativeResizeFrame() override
    {
        if ( m_watchdog && GetHandle() )
            ::KillTimer(GetHwndOf(this), m_watchdog);
    }

    bool ArmWatchdog()
    {
        m_watchdog = ::SetTimer(GetHwndOf(this), 0, 2000, nullptr);
        return m_watchdog != 0;
    }

    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        switch ( message )
        {
            case WM_NCLBUTTONDOWN:
                ++nativeDowns;
                lastHitTest = static_cast<int>(wParam);
                downBeforeUnwind = downBeforeUnwind || invoking;
                break;

            case WM_ENTERSIZEMOVE:
            {
                ++nativeEntries;
                wxWinUINativeResizeSnapshot snapshot;
                enteredPhaseObserved =
                    wxWinUIHostResizeTestAccess::GetSnapshot(this, &snapshot) &&
                    snapshot.phase == wxWinUINativeResizePhase::Entered;
                // A test can route MOVE/UP while USER32 is really tracking,
                // instead of ending every case immediately at native entry.
                // This message addresses only our own private-desktop HWND;
                // it never injects mouse/keyboard input into the desktop.
                if ( routeWhileSizing )
                    routedInputPosted = ::PostMessageW(
                        GetHwndOf(this), RoutedInputMessage, 0, 0) != FALSE;
                else
                    cancelPosted = ::PostMessageW(
                        GetHwndOf(this), WM_CANCELMODE, 0, 0) != FALSE;
                break;
            }

            case RoutedInputMessage:
                if ( routeWhileSizing )
                {
                    auto route = std::move(routeWhileSizing);
                    route();
                }
                return 0;

            case WM_EXITSIZEMOVE:
            {
                ++nativeExits;
                wxWinUINativeResizeSnapshot snapshot;
                idlePhaseObserved =
                    wxWinUIHostResizeTestAccess::GetSnapshot(this, &snapshot) &&
                    snapshot.phase == wxWinUINativeResizePhase::Idle;
                break;
            }

            case WM_SIZING:
                ++nativeSizing;
                lastSizingRect = *reinterpret_cast<const RECT*>(lParam);
                if ( observeSizing ) observeSizing();
                break;

            case WM_CANCELMODE:
                ++nativeCancels;
                break;

            case WM_TIMER:
                if ( m_watchdog && wParam == m_watchdog )
                {
                    watchdogFired = true;
                    // This is a failing-test escape hatch, not success. Only
                    // release capture if this exact test window owns it.
                    if ( ::GetCapture() == GetHwndOf(this) )
                        ::ReleaseCapture();
                    ::PostMessageW(GetHwndOf(this), WM_CANCELMODE, 0, 0);
                    ::PostMessageW(GetHwndOf(this), WM_KEYDOWN, VK_ESCAPE, 0);
                    return 0;
                }
                break;
        }

        return wxFrame::MSWWindowProc(message, wParam, lParam);
    }

    bool invoking = false;
    bool downBeforeUnwind = false;
    bool cancelPosted = false;
    bool watchdogFired = false;
    bool enteredPhaseObserved = false;
    bool idlePhaseObserved = false;
    bool routedInputPosted = false;
    std::function<void ()> routeWhileSizing;
    std::function<void ()> observeSizing;
    unsigned nativeDowns = 0;
    unsigned nativeEntries = 0;
    unsigned nativeExits = 0;
    unsigned nativeSizing = 0;
    unsigned nativeCancels = 0;
    RECT lastSizingRect{};
    int lastHitTest = HTNOWHERE;

private:
    UINT_PTR m_watchdog = 0;
};

class NativeResizeForeignCapture final
{
public:
    explicit NativeResizeForeignCapture(HWND hwnd) : m_hwnd(hwnd)
    {
        ::SetCapture(m_hwnd);
    }

    ~NativeResizeForeignCapture()
    {
        if ( ::GetCapture() == m_hwnd )
            ::ReleaseCapture();
    }

private:
    HWND m_hwnd;
};

// Observe what USER32 receives after the production WH_GETMESSAGE hook.
// The test supplies no replacement translator and never changes the packet.
class NativeSizingMessageObserver final
{
public:
    explicit NativeSizingMessageObserver(HWND target)
        : m_target(target), m_previous(s_current)
    {
        s_current = this;
        m_hook = ::SetWindowsHookExW(WH_GETMESSAGE, Hook, nullptr, ::GetCurrentThreadId());
    }
    ~NativeSizingMessageObserver()
    {
        if ( m_hook ) CHECK(::UnhookWindowsHookEx(m_hook));
        s_current = m_previous;
    }
    bool IsOk() const { return m_hook != nullptr; }
    std::vector<MSG> moves;
    unsigned releases = 0;

private:
    static LRESULT CALLBACK Hook(int code, WPARAM removal, LPARAM opaque)
    {
        const auto result = ::CallNextHookEx(nullptr, code, removal, opaque);
        auto* const observer = s_current;
        if ( observer && code >= 0 && removal == PM_REMOVE )
        {
            const auto& msg = *reinterpret_cast<const MSG*>(opaque);
            if ( msg.hwnd == observer->m_target && ::GetCapture() == observer->m_target )
            {
                if ( msg.message == WM_MOUSEMOVE && (msg.wParam & MK_LBUTTON) )
                    observer->moves.push_back(msg);
                else if ( msg.message == WM_LBUTTONUP )
                    ++observer->releases;
            }
        }
        return result;
    }
    static thread_local NativeSizingMessageObserver* s_current;
    HWND m_target;
    HHOOK m_hook = nullptr;
    NativeSizingMessageObserver* m_previous;
};
thread_local NativeSizingMessageObserver* NativeSizingMessageObserver::s_current = nullptr;

HWND FindResizeInputSite(HWND bridge)
{
    HWND inputSite = nullptr;
    ::EnumChildWindows(bridge,
        [](HWND child, LPARAM opaque) -> BOOL
        {
            wchar_t name[64]{};
            if ( ::GetClassNameW(child, name, WXSIZEOF(name)) &&
                 ::lstrcmpW(name, L"InputSiteWindowClass") == 0 )
            {
                *reinterpret_cast<HWND *>(opaque) = child;
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&inputSite));
    return inputSite;
}

struct StatusBarTextReentryContext
{
    wxString text;
    int calls = 0;
    bool observePeer = false;
    bool peerReadable = false;
    int observedStyle = -1;
};

void SetNestedStatusBarText(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarTextReentryContext *>(opaque);
    ++context->calls;
    if ( context->observePeer )
    {
        context->peerReadable =
            wxWinUIStatusBarTestAccess::GetFieldState(*statusBar,
                0, nullptr, nullptr, &context->observedStyle, nullptr,
                nullptr, nullptr, nullptr);
    }
    statusBar->SetStatusText(context->text);
}

struct StatusBarAppearanceReentryContext
{
    int calls = 0;
};

void ClearNestedStatusBarFont(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarAppearanceReentryContext *>(opaque);
    ++context->calls;
    statusBar->SetFont(wxNullFont);
}

struct StatusBarColourReentryContext
{
    wxColour colour;
    int calls = 0;
};

void SetNestedStatusBarForeground(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarColourReentryContext *>(opaque);
    ++context->calls;
    statusBar->SetForegroundColour(context->colour);
}

struct StatusBarDestructionReentryContext
{
    std::unique_ptr<wxStatusBar> *owned = nullptr;
    int calls = 0;
    bool ownerMatched = false;
};

struct StatusBarDPIReentryContext
{
    wxSize oldDPI;
    wxSize newDPI;
    int calls = 0;
};

void DeliverNestedStatusBarDPI(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarDPIReentryContext *>(opaque);
    ++context->calls;
    wxDPIChangedEvent nested(context->oldDPI, context->newDPI);
    nested.SetEventObject(statusBar);
    nested.SetId(statusBar->GetId());
    wxWinUIStatusBarTestAccess::DeliverDPIChanged(*statusBar, nested);
}

void DestroyStatusBarDuringRebuild(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarDestructionReentryContext *>(opaque);
    ++context->calls;
    context->ownerMatched =
        context->owned && context->owned->get() == statusBar;
    if ( context->ownerMatched )
        context->owned->reset();
}

struct StatusBarMinHeightReentryContext
{
    int height = 0;
    int calls = 0;
};

void SetNestedStatusBarMinHeight(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarMinHeightReentryContext *>(opaque);
    ++context->calls;
    statusBar->SetMinHeight(context->height);
}

struct StatusBarRebuildStormContext
{
    int calls = 0;
    int targetCalls = 0;
    int latestStyle = wxSB_NORMAL;
};

void ContinueStatusBarRebuildStorm(wxStatusBar *statusBar, void *opaque)
{
    auto * const context =
        static_cast<StatusBarRebuildStormContext *>(opaque);
    ++context->calls;
    context->latestStyle =
        context->calls % 2 ? wxSB_RAISED : wxSB_SUNKEN;
    if ( context->calls < context->targetCalls )
    {
        wxWinUIStatusBarTestAccess::SetNextReentryHook(*statusBar,
            wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
            &ContinueStatusBarRebuildStorm,
            context);
    }
    statusBar->SetStatusStyles(1, &context->latestStyle);
}

#endif // wxUSE_STATUSBAR

#if wxUSE_HYPERLINKCTRL

class InspectableGenericHyperlinkCtrl final
    : public wxGenericHyperlinkCtrl
{
public:
    wxRect GetLabelRectForTesting() const
    {
        return GetLabelRect();
    }
};

struct HyperlinkApplyStormContext
{
    wxHyperlinkCtrl *link = nullptr;
    int calls = 0;
    int targetCalls = 0;
    int destroyAtCall = 0;
    std::unique_ptr<wxHyperlinkCtrl> *owned = nullptr;
    wxHyperlinkCtrl **observed = nullptr;
    wxString latestLabel;
};

void ContinueHyperlinkApplyStorm(void *opaque)
{
    auto * const context =
        static_cast<HyperlinkApplyStormContext *>(opaque);
    ++context->calls;
    if ( context->destroyAtCall == context->calls )
    {
        wxHyperlinkCtrl * const dying = context->owned->release();
        *context->observed = nullptr;
        context->link = nullptr;
        delete dying;
        return;
    }

    const wxChar accessKey =
        static_cast<wxChar>('A' + (context->calls - 1) % 26);
    context->latestLabel = wxString::Format(
        "Deferred &%c %d", accessKey, context->calls);
    if ( context->calls < context->targetCalls ||
         (context->destroyAtCall &&
          context->calls < context->destroyAtCall) )
    {
        wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(*context->link,
            &ContinueHyperlinkApplyStorm, context);
    }
    context->link->SetLabel(context->latestLabel);
}

#endif // wxUSE_HYPERLINKCTRL

#if wxUSE_ACTIVITYINDICATOR

struct ActivityApplyStormContext
{
    wxActivityIndicator *activity = nullptr;
    int calls = 0;
    int targetCalls = 0;
    int destroyAtCall = 0;
    std::unique_ptr<wxActivityIndicator> *owned = nullptr;
    wxActivityIndicator **observed = nullptr;
};

void ContinueActivityApplyStorm(void *opaque)
{
    auto * const context =
        static_cast<ActivityApplyStormContext *>(opaque);
    ++context->calls;
    if ( context->destroyAtCall == context->calls )
    {
        wxActivityIndicator * const dying = context->owned->release();
        *context->observed = nullptr;
        context->activity = nullptr;
        delete dying;
        return;
    }

    if ( context->calls < context->targetCalls ||
         (context->destroyAtCall &&
          context->calls < context->destroyAtCall) )
    {
        wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(*context->activity,
            &ContinueActivityApplyStorm, context);
    }

    if ( context->calls % 2 )
        context->activity->Stop();
    else
        context->activity->Start();
}

#endif // wxUSE_ACTIVITYINDICATOR

} // namespace

TEST_CASE("wxWinUI native resize progress records entry and exit at most once",
          "[winui-native-resize-state]")
{
    wxWinUINativeResizeProgress progress;
    CHECK_FALSE(progress.entered);
    CHECK_FALSE(progress.exited);
    CHECK_FALSE(progress.Enter(false));
    CHECK_FALSE(progress.Exit());
    CHECK_FALSE(progress.entered);
    CHECK_FALSE(progress.exited);

    CHECK(progress.Enter(true));
    CHECK(progress.entered);
    CHECK_FALSE(progress.exited);
    CHECK_FALSE(progress.Enter(true));

    CHECK(progress.Exit());
    CHECK(progress.exited);
    CHECK_FALSE(progress.Exit());
    CHECK_FALSE(progress.Enter(true));
}

#if wxUSE_STATUSBAR

TEST_CASE("wxWinUI StatusBar resize retains native fields and active grip",
          "[winui-chrome-feedback][statusbar][winui-statusbar-resize]")
{
    const bool rtl = GENERATE(false, true);
    const long ellipsis = GENERATE(wxSTB_ELLIPSIZE_START,
                                  wxSTB_ELLIPSIZE_MIDDLE,
                                  wxSTB_ELLIPSIZE_END);
    CAPTURE(rtl, ellipsis);
    wxFrame frame(nullptr, wxID_ANY, "retained status-bar resize",
                  StatusBarResizeTestPosition(), wxSize(900, 240));
    frame.SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    wxStatusBar * const status = frame.CreateStatusBar(
        3, wxSTB_SIZEGRIP | wxSTB_SHOW_TIPS | ellipsis);
    REQUIRE(status);
    status->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    const int widths[] = {status->FromDIP(90), -1, status->FromDIP(70)};
    status->SetStatusWidths(3, widths);
    status->SetStatusText("Ready", 0);
    const wxString full = "A long status message that must track available width";
    status->SetStatusText(full, 1);
    status->SetStatusText("Last", 2);
    frame.ShowWithoutActivating();
    auto * const host = wxWinUITopLevelHost::FindSlotOwner(status);
    REQUIRE(host);
    auto * const slot = host->FindSlot(status);
    REQUIRE(slot);
    REQUIRE(WaitForStatusBarDispatch([&]()
    {
        const auto peer = slot->GetContent().try_as<MUX::FrameworkElement>();
        return peer && peer.IsLoaded() && peer.ActualWidth() > 0;
    }));
    const auto root = slot->GetContent().as<MUXC::Grid>();
    const auto content = root.Children().GetAt(0).as<MUXC::ContentControl>();
    const auto overlay = content.Content().as<MUXC::Grid>();
    REQUIRE(overlay.Children().Size() == 2);
    const auto grid = overlay.Children().GetAt(0).as<MUXC::Grid>();
    const auto grip = overlay.Children().GetAt(1).as<MUXC::Grid>();
    REQUIRE(grid.Children().Size() == 3);
    std::array<MUXC::Grid, 3> cells{nullptr, nullptr, nullptr};
    std::array<MUXC::TextBlock, 3> texts{nullptr, nullptr, nullptr};
    for ( size_t i = 0; i < cells.size(); ++i )
    {
        cells[i] = grid.Children().GetAt(static_cast<uint32_t>(i)).as<MUXC::Grid>();
        for ( const auto child : cells[i].Children() )
        {
            if ( const auto text = child.try_as<MUXC::TextBlock>() )
                texts[i] = text;
        }
        REQUIRE(texts[i]);
    }
    REQUIRE(WaitForStatusBarDispatch([&]()
    {
        return grip.IsLoaded() && texts[1].IsLoaded() &&
               cells[1].ActualWidth() > 0;
    }));
    auto unloads = std::make_shared<unsigned>(0);
    const auto observeUnload = [unloads](const auto&, const auto&) { ++*unloads; };
    const auto rootUnloaded = root.Unloaded(winrt::auto_revoke, observeUnload);
    const auto gripUnloaded = grip.Unloaded(winrt::auto_revoke, observeUnload);
    const auto firstUnloaded = texts[0].Unloaded(winrt::auto_revoke, observeUnload);
    const auto middleUnloaded = texts[1].Unloaded(winrt::auto_revoke, observeUnload);
    const auto lastUnloaded = texts[2].Unloaded(winrt::auto_revoke, observeUnload);

    for ( int step = 0; step < 24; ++step )
    {
        const bool narrow = step % 2 == 0;
        CAPTURE(step, narrow);
        frame.SetClientSize(frame.FromDIP(wxSize(
            (narrow ? 260 : 1100) + step, 200 + step)));
        REQUIRE(WaitForStatusBarDispatch([&]()
        {
            const double expected =
                status->GetClientSize().x / status->GetDPIScaleFactor();
            return std::abs(root.ActualWidth() - expected) <= 1.0 &&
                   texts[1].IsLoaded();
        }));
        CHECK(slot->GetContent() == root);
        CHECK(content.Content() == overlay);
        CHECK(overlay.Children().GetAt(0) == grid);
        CHECK(overlay.Children().GetAt(1) == grip);
        CHECK(*unloads == 0);
        for ( size_t i = 0; i < cells.size(); ++i )
        {
            CHECK(grid.Children().GetAt(static_cast<uint32_t>(i)) == cells[i]);
            MUXC::TextBlock currentText{nullptr};
            unsigned textCount = 0;
            for ( const auto child : cells[i].Children() )
            {
                if ( const auto text = child.try_as<MUXC::TextBlock>() )
                {
                    currentText = text;
                    ++textCount;
                }
            }
            CHECK(textCount == 1);
            CHECK(currentText == texts[i]);
            wxRect rect;
            REQUIRE(status->GetFieldRect(static_cast<int>(i), rect));
            CHECK(std::abs(cells[i].ActualWidth() -
                (rect.width + 2 * status->GetBorderX()) /
                    status->GetDPIScaleFactor()) <= 1.0);
        }
        wxString rendered;
        bool tip = false;
        REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(
            *status, 1, &rendered, nullptr, nullptr, &tip,
            nullptr, nullptr, nullptr));
        CHECK(status->GetField(1).IsEllipsized() == narrow);
#if wxUSE_TOOLTIPS
        CHECK(tip == narrow);
#endif
        CHECK((rendered != full) ==
              (narrow && ellipsis != wxSTB_ELLIPSIZE_END));
        wxWinUIStatusBarSizeGripSnapshot gripState;
        REQUIRE(wxWinUIStatusBarTestAccess::GetSizeGripState(*status, &gripState));
        CHECK(gripState.nativeHitTest == (rtl ? HTBOTTOMLEFT : HTBOTTOMRIGHT));
        CHECK(gripState.reservesFieldSpace);
    }
    CHECK(*unloads == 0);
}

TEST_CASE("wxWinUI StatusBar keeps pixel geometry and XAML DIPs distinct",
          "[winui-chrome-feedback][statusbar][geometry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status;
    REQUIRE(status.Create(parent));
    status.SetClientSize(parent->FromDIP(wxSize(420, 32)));

    const int widths[] = { parent->FromDIP(90), -1, -2 };
    status.SetFieldsCount(3, widths);

    const int available =
        status.GetClientSize().x - 2 * status.GetBorderX() -
        status.FromDIP(18);
    const int weighted = std::max(0, available - widths[0]);
    const int expectedSecond = weighted / 3;
    const int expectedThird = weighted - expectedSecond;

    wxRect first;
    wxRect second;
    wxRect third;
    REQUIRE(status.GetFieldRect(0, first));
    REQUIRE(status.GetFieldRect(1, second));
    REQUIRE(status.GetFieldRect(2, third));
    CHECK(first.x == status.GetBorderX());
    CHECK(first.width == widths[0] - 2 * status.GetBorderX());
    CHECK(second.x == status.GetBorderX() + widths[0]);
    CHECK(second.width == expectedSecond - 2 * status.GetBorderX());
    CHECK(third.x ==
          status.GetBorderX() + widths[0] + expectedSecond);
    CHECK(third.width == expectedThird - 2 * status.GetBorderX());

    double columnValue = 0.0;
    int columnUnit = -1;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, nullptr, nullptr, nullptr, nullptr,
        &columnValue, &columnUnit, nullptr));
    CHECK(columnUnit == static_cast<int>(MUX::GridUnitType::Pixel));
    CHECK(std::abs(
              columnValue -
              widths[0] / status.GetDPIScaleFactor()) < 0.01);

    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        1, nullptr, nullptr, nullptr, nullptr,
        &columnValue, &columnUnit, nullptr));
    CHECK(columnUnit == static_cast<int>(MUX::GridUnitType::Star));
    CHECK(columnValue == 1.0);
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        2, nullptr, nullptr, nullptr, nullptr,
        &columnValue, &columnUnit, nullptr));
    CHECK(columnUnit == static_cast<int>(MUX::GridUnitType::Star));
    CHECK(columnValue == 2.0);
    CHECK(wxWinUIStatusBarTestAccess::HasSizeGrip(status));

    wxStatusBar noGrip(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    CHECK_FALSE(wxWinUIStatusBarTestAccess::HasSizeGrip(noGrip));
}

TEST_CASE("wxWinUI StatusBar size grip has localized overlay geometry",
          "[winui-chrome-feedback][statusbar][geometry][uia][input]")
{
    wxFrame frame(
        nullptr, wxID_ANY, "status-grip-contract-owner",
        wxDefaultPosition, wxSize(420, 180),
        wxDEFAULT_FRAME_STYLE);
    wxStatusBar status(&frame);
    status.SetClientSize(frame.FromDIP(wxSize(360, 28)));
    const int widths[] = { -1, -2, status.FromDIP(61) };
    status.SetFieldsCount(3, widths);

    wxWinUIStatusBarSizeGripSnapshot snapshot;
    REQUIRE(wxWinUIStatusBarTestAccess::GetSizeGripState(status, &snapshot));
    CHECK(snapshot.widthDips == 18.0);
    CHECK(snapshot.heightDips == 18.0);
    CHECK(snapshot.horizontalAlignment ==
          static_cast<int>(MUX::HorizontalAlignment::Right));
    CHECK(snapshot.cursorShape ==
          static_cast<int>(
              MUI::InputSystemCursorShape::SizeNorthwestSoutheast));
    CHECK(snapshot.nativeHitTest == HTBOTTOMRIGHT);
    CHECK(snapshot.automationName == _("Resize"));
    CHECK(snapshot.localizedControlType == _("resize grip"));
    CHECK_FALSE(snapshot.automationName.empty());
    CHECK_FALSE(snapshot.localizedControlType.empty());
    CHECK(snapshot.overlaysFields);
    CHECK(snapshot.reservesFieldSpace);
    CHECK(std::abs(
              snapshot.fieldReservationDips -
              status.FromDIP(18) / status.GetDPIScaleFactor()) < 0.01);

    const wxPoint actionPoint(173, 281);

    CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(status,
        false, true, true, actionPoint));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(status,
        true, false, true, actionPoint));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(status,
        true, true, false, actionPoint));

    wxRect restoredFirst;
    wxRect restoredLast;
    REQUIRE(status.GetFieldRect(0, restoredFirst));
    REQUIRE(status.GetFieldRect(2, restoredLast));
    CHECK(restoredFirst.width != restoredLast.width);
    wxWinUIStatusBarTestAccess::SetTopLevelMaximized(status, true);
    CHECK_FALSE(wxWinUIStatusBarTestAccess::HasSizeGrip(status));
    CHECK_FALSE(
        wxWinUIStatusBarTestAccess::GetSizeGripState(status, &snapshot));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(status,
        true, true, true, actionPoint));
    wxRect maximizedFirst;
    REQUIRE(status.GetFieldRect(0, maximizedFirst));
    CHECK(maximizedFirst.width > restoredFirst.width);

    wxWinUIStatusBarTestAccess::SetTopLevelMaximized(status, false);
    REQUIRE(wxWinUIStatusBarTestAccess::GetSizeGripState(status, &snapshot));
    wxRect restoredAgainFirst;
    REQUIRE(status.GetFieldRect(0, restoredAgainFirst));
    CHECK(restoredAgainFirst == restoredFirst);

    status.SetLayoutDirection(wxLayout_RightToLeft);
    status.SendSizeEvent();
    REQUIRE(wxWinUIStatusBarTestAccess::GetSizeGripState(status, &snapshot));
    CHECK(snapshot.horizontalAlignment ==
          static_cast<int>(MUX::HorizontalAlignment::Left));
    CHECK(snapshot.cursorShape ==
          static_cast<int>(
              MUI::InputSystemCursorShape::SizeNortheastSouthwest));
    CHECK(snapshot.nativeHitTest == HTBOTTOMLEFT);
    CHECK(snapshot.overlaysFields);
    CHECK(snapshot.reservesFieldSpace);
    CHECK(std::abs(
              snapshot.fieldReservationDips -
              status.FromDIP(18) / status.GetDPIScaleFactor()) < 0.01);
    wxRect rtlFirst;
    wxRect rtlLast;
    REQUIRE(status.GetFieldRect(0, rtlFirst));
    REQUIRE(status.GetFieldRect(2, rtlLast));
    CHECK(rtlFirst == restoredFirst);
    CHECK(rtlLast == restoredLast);
    CHECK(rtlFirst.x == status.GetBorderX());
    CHECK(rtlLast.x + rtlLast.width + 2 * status.GetBorderX() ==
          status.GetClientSize().x - status.GetBorderX() -
              status.FromDIP(18));

    wxFrame fixedFrame(
        nullptr, wxID_ANY, "status-grip-fixed-owner",
        wxDefaultPosition, wxSize(420, 180),
        wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER);
    wxStatusBar fixedStatus;
    REQUIRE(fixedStatus.Create(&fixedFrame));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::HasSizeGrip(fixedStatus));
    CHECK_FALSE(
        wxWinUIStatusBarTestAccess::GetSizeGripState(fixedStatus, &snapshot));
}

TEST_CASE("wxWinUI grip request enters and exits the real native sizing loop once",
          "[winui-native-resize][winui-v0-supported][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar * const status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    int expectedHit = HTBOTTOMRIGHT;
    bool islandOwnsCapture = false;

    SECTION("LTR") { }
    SECTION("island capture is handed off")
    {
        islandOwnsCapture = true;
    }
    SECTION("RTL")
    {
        status->SetLayoutDirection(wxLayout_RightToLeft);
        expectedHit = HTBOTTOMLEFT;
    }

    frame.Show();
    YieldForAWhile(30);
    REQUIRE(wxWinUIStatusBarTestAccess::HasSizeGrip(*status));
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    std::unique_ptr<NativeResizeForeignCapture> islandCapture;
    if ( islandOwnsCapture )
    {
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(&frame, false);
        REQUIRE(host);
        REQUIRE(host->GetBridgeHwnd());
        islandCapture = std::make_unique<NativeResizeForeignCapture>(
            host->GetBridgeHwnd());
        REQUIRE(::GetCapture() == host->GetBridgeHwnd());
    }
    const wxPoint point = status->GetScreenRect().GetBottomRight() -
        status->FromDIP(wxPoint(8, 8));

    frame.invoking = true;
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point));
    wxWinUINativeResizeSnapshot pending;
    REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &pending));
    CHECK(pending.phase == wxWinUINativeResizePhase::Pending);
    CHECK(pending.scheduled == 1);
    CHECK(pending.entered == 0);
    CHECK(pending.ticket != 0);
    CHECK(pending.hitTest == expectedHit);
    CHECK(pending.screenPoint.x == point.x);
    CHECK(pending.screenPoint.y == point.y);
    CHECK(frame.nativeDowns == 0);
    frame.invoking = false;

    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("native sizing loop completion", [&]()
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    CHECK(finished.ticket == pending.ticket);
    CHECK(finished.entered == 1);
    CHECK(finished.exited == 1);
    CHECK(frame.nativeDowns == 1);
    CHECK(frame.nativeEntries == 1);
    CHECK(frame.nativeExits == 1);
    CHECK(frame.lastHitTest == expectedHit);
    CHECK_FALSE(frame.downBeforeUnwind);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(frame.cancelPosted);
    CHECK(frame.enteredPhaseObserved);
    CHECK(frame.idlePhaseObserved);
    CHECK(::GetCapture() == nullptr);
    YieldForAWhile(20);
    CHECK(frame.nativeEntries == 1);
}

TEST_CASE("wxWinUI grip native sizing packets bypass only the XAML get-message translator",
          "[winui-native-resize][winui-native-resize-pump][statusbar][input]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeResizeFrame frame;
    frame.SetPosition(StatusBarResizeTestPosition());
    auto* const status = frame.CreateStatusBar();
    REQUIRE(status);
    status->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    NativeResizeContactOverride contact;
    frame.Show();
    REQUIRE(WaitFor("native sizing pump grip ready", [&]
    { return wxWinUIStatusBarTestAccess::HasSizeGrip(*status); }));
    auto* const host = wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    REQUIRE(host->GetBridgeHwnd());
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    unsigned translated = 0, probes = 0;
    const auto translator = [](WXMSG*, void* context)
    { ++*static_cast<unsigned*>(context); return true; };
    const auto probe = [&](HWND destination, UINT id, WPARAM keys, bool expectedNative)
    {
        MSG message{};
        message.hwnd = destination;
        message.message = id;
        message.wParam = keys;
        message.lParam = MAKELPARAM(47, 29);
        message.pt = { 470, 290 };
        const auto before = translated;
        const bool consumed = wxWinUIKeyboardTestAccess::ProcessGetMessageHook(
            HC_ACTION, PM_REMOVE, reinterpret_cast<WXMSG*>(&message),
            wxWinUIKeyboardModifiers(), translator, &translated);
        CAPTURE(id, expectedNative);
        CHECK(consumed == !expectedNative);
        CHECK(translated == before + (expectedNative ? 0 : 1));
        CHECK(message.message == (expectedNative ? id : WM_NULL));
        if ( expectedNative )
        {
            CHECK(message.wParam == keys);
            CHECK(message.lParam == MAKELPARAM(47, 29));
            CHECK(message.pt.x == 470);
            CHECK(message.pt.y == 290);
        }
        ++probes;
    };
    probe(GetHwndOf(&frame), WM_MOUSEMOVE, MK_LBUTTON, false);
    bool ranInsideNativeLoop = false;
    frame.routeWhileSizing = [&]
    {
        // This callback is dispatched by USER32's actual sizing loop, after
        // WM_ENTERSIZEMOVE and native capture acquisition. Only the existing
        // translator seam is substituted; no mouse motion or HWND is faked.
        // Always queue our own loop cancellation before checking assertions.
        frame.cancelPosted = ::PostMessageW(GetHwndOf(&frame), WM_CANCELMODE, 0, 0) != FALSE;
        ranInsideNativeLoop = true;
        CHECK(frame.nativeEntries == 1);
        CHECK(::GetCapture() == GetHwndOf(&frame));
        probe(GetHwndOf(&frame), WM_MOUSEMOVE, MK_LBUTTON, true);
        probe(GetHwndOf(&frame), WM_NCMOUSEMOVE, HTBOTTOMRIGHT, true);
        probe(GetHwndOf(&frame), WM_LBUTTONUP, 0, true);
        probe(GetHwndOf(&frame), WM_NCLBUTTONUP, HTBOTTOMRIGHT, true);
        probe(GetHwndOf(&frame), WM_KEYDOWN, VK_ESCAPE, true);
        probe(GetHwndOf(&frame), WM_KEYUP, VK_RIGHT, true);
        probe(GetHwndOf(&frame), WM_CANCELMODE, 0, true);
        // Foreign HWNDs and the presentation pump keep their ordinary
        // translation path; this is not a global suspension of XAML.
        probe(host->GetBridgeHwnd(), WM_MOUSEMOVE, MK_LBUTTON, false);
        probe(GetHwndOf(&frame), WM_TIMER, 0x1234, false);
        probe(GetHwndOf(&frame), WM_MOUSEWHEEL, 0, false);
    };
    const auto point = rtl
        ? status->GetScreenRect().GetBottomLeft() + status->FromDIP(wxPoint(8, -8))
        : status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(*status, true, true, true, point));
    // Merely scheduling a resize must not steal any island messages.
    probe(GetHwndOf(&frame), WM_MOUSEMOVE, MK_LBUTTON, false);
    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("native resize get-message ownership", [&]
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    REQUIRE(ranInsideNativeLoop);
    CHECK(finished.entered == 1);
    CHECK(finished.exited == 1);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(frame.cancelPosted);
    CHECK(::GetCapture() == nullptr);
    // The scope expires with SendMessage, even though the HWND is still live.
    probe(GetHwndOf(&frame), WM_MOUSEMOVE, MK_LBUTTON, false);
    CHECK(probes == 13);
}

TEST_CASE("wxWinUI grip routes coalesced native movement and commits on release",
          "[winui-native-resize][winui-native-resize-motion][statusbar][input]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    NativeResizeFrame frame;
    frame.SetPosition(StatusBarResizeTestPosition());
    auto* const status = frame.CreateStatusBar();
    REQUIRE(status);
    status->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    NativeResizeContactOverride contact;
    frame.Show();
    REQUIRE(WaitFor("native sizing motion grip ready", [&]
    { return wxWinUIStatusBarTestAccess::HasSizeGrip(*status); }));
    auto* const host = wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    NativeSizingMessageObserver messages(GetHwndOf(&frame));
    REQUIRE(messages.IsOk());
    const auto point = rtl
        ? status->GetScreenRect().GetBottomLeft() + status->FromDIP(wxPoint(8, -8))
        : status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    wxWinUIPointerSample sample;
    sample.pointerId = 77;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    sample.buttonMask = MK_LBUTTON;
    bool movementRouted = false, checkedBeforeRelease = false, releaseRouted = false;
    size_t messagesAtMove = 0;
    frame.routeWhileSizing = [&]
    {
        CHECK(frame.nativeEntries == 1);
        CHECK(::GetCapture() == GetHwndOf(&frame));
        messagesAtMove = messages.moves.size();
        sample.kind = wxWinUIInputKind::Move;
        sample.screenX += rtl ? -64 : 64;
        sample.screenY += 48;
        ++sample.timestamp;
        movementRouted = wxWinUIHostInputTestAccess::RouteRootPointerSample(
            *host, sample, true).cancelSource;
        sample.screenX += rtl ? -32 : 32;
        sample.screenY += 24;
        ++sample.timestamp;
        CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true).cancelSource);
        CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true).cancelSource);
        auto late = sample;
        --late.timestamp;
        late.screenX += rtl ? -1000 : 1000;
        late.screenY += 1000;
        CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, late, true).cancelSource);

        // Queue a second native-loop turn after the routed move. Observe
        // the actual packet delivered after the production hook, before UP.
        // USER32's physical cursor/queue state is deliberately untouched:
        // the earlier 68/69 WM_SIZING oracle was red on a private desktop
        // while the human candidate69 drag proved real live tracking. The
        // physical geometry contract remains a separate human qualification.
        frame.routeWhileSizing = [&]
        {
            checkedBeforeRelease = true;
            CHECK(contact.down);
            CHECK(messages.moves.size() == messagesAtMove + 1);
            if ( messages.moves.size() > messagesAtMove )
            {
                const auto& delivered = messages.moves.back();
                CHECK(delivered.pt.x == sample.screenX);
                CHECK(delivered.pt.y == sample.screenY);
                CHECK(delivered.time == static_cast<DWORD>(sample.timestamp));
                CHECK(delivered.wParam == MK_LBUTTON);
            }
            sample.kind = wxWinUIInputKind::Release;
            sample.button = wxWinUIInputButton::Left;
            sample.buttonMask = 0;
            ++sample.timestamp;
            contact.down = false;
            releaseRouted = wxWinUIHostInputTestAccess::RouteRootPointerSample(
                *host, sample, true).cancelSource;
            // The same routed UP can bubble more than once. It commits only
            // once, and a late MOVE cannot replace the pending release.
            CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true).cancelSource);
            auto afterRelease = sample;
            afterRelease.kind = wxWinUIInputKind::Move;
            afterRelease.button = wxWinUIInputButton::None;
            afterRelease.buttonMask = MK_LBUTTON;
            ++afterRelease.timestamp;
            afterRelease.screenX += 400;
            CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, afterRelease, true).cancelSource);
        };
        CHECK(::PostMessageW(GetHwndOf(&frame), NativeResizeFrame::RoutedInputMessage, 0, 0));
    };
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(*status, true, true, true,
        point, sample.pointerId, sample.timestamp));
    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("routed motion native sizing completion", [&]
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    CHECK(movementRouted);
    CHECK(checkedBeforeRelease);
    CHECK(releaseRouted);
    CHECK(finished.entered == 1);
    CHECK(finished.exited == 1);
    CHECK(finished.cancelled == 0);
    CHECK(messages.releases == 1);
    CHECK(frame.nativeCancels == 0);
    CHECK(messages.moves.size() == messagesAtMove + 1);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI grip queued movement expires on release or foreign capture",
          "[winui-native-resize][winui-native-resize-motion][statusbar][input]")
{
    const bool loseCapture = GENERATE(false, true);
    CAPTURE(loseCapture);
    NativeResizeFrame frame;
    frame.SetPosition(StatusBarResizeTestPosition());
    auto* const status = frame.CreateStatusBar();
    REQUIRE(status);
    wxWindow foreign(&frame, wxID_ANY);
    NativeResizeContactOverride contact;
    frame.Show();
    REQUIRE(WaitFor("native sizing cancelled motion grip ready", [&]
    { return wxWinUIStatusBarTestAccess::HasSizeGrip(*status); }));
    // The sizing loop of USER32 follows the real cursor, wherever the pointer
    // samples of the test are: put the grip under it, for the loop to find no
    // movement of its own to apply. The cursor of the CI machine stays in the
    // middle of its screen. On a desktop without a cursor nothing moves.
    POINT cursor{};
    if ( ::GetCursorPos(&cursor) )
    {
        const wxPoint grip = status->GetScreenRect().GetBottomRight() -
                             status->FromDIP(wxPoint(8, 8));
        frame.Move(frame.GetPosition() + wxPoint(cursor.x, cursor.y) - grip);
        REQUIRE(WaitFor("native sizing cancelled motion grip under the cursor", [&]
        {
            const wxPoint moved = status->GetScreenRect().GetBottomRight() -
                                  status->FromDIP(wxPoint(8, 8));
            return moved == wxPoint(cursor.x, cursor.y);
        }));
    }
    auto* const host = wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    NativeSizingMessageObserver messages(GetHwndOf(&frame));
    REQUIRE(messages.IsOk());
    const auto point = status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    wxWinUIPointerSample sample;
    sample.pointerId = 78;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    sample.buttonMask = MK_LBUTTON;
    bool routed = false, checkedAfterInvalidation = false;
    unsigned sizingBefore = 0;
    const wxRect initialRect = frame.GetScreenRect();
    wxRect rectangleAtSizing;
    unsigned releasesAtSizing = 0;
    size_t movesAtSizing = 0;
    DWORD messagePositionAtSizing = 0;
    frame.observeSizing = [&]
    {
        rectangleAtSizing = wxRect(frame.lastSizingRect.left, frame.lastSizingRect.top,
            frame.lastSizingRect.right - frame.lastSizingRect.left,
            frame.lastSizingRect.bottom - frame.lastSizingRect.top);
        releasesAtSizing = messages.releases;
        movesAtSizing = messages.moves.size();
        messagePositionAtSizing = ::GetMessagePos();
    };
    const auto clearSizingObserver = wxMakeGuard([&] { frame.observeSizing = {}; });
    std::unique_ptr<NativeResizeForeignCapture> foreignCapture;
    frame.routeWhileSizing = [&]
    {
        CHECK(::GetCapture() == GetHwndOf(&frame));
        sizingBefore = frame.nativeSizing;
        sample.kind = wxWinUIInputKind::Move;
        sample.screenX += 80;
        sample.screenY += 60;
        ++sample.timestamp;
        routed = wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true).cancelSource;
        if ( loseCapture )
            foreignCapture = std::make_unique<NativeResizeForeignCapture>(GetHwndOf(&foreign));
        else
        {
            sample.kind = wxWinUIInputKind::Release;
            sample.button = wxWinUIInputButton::Left;
            sample.buttonMask = 0;
            ++sample.timestamp;
            contact.down = false;
            CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true).cancelSource);
        }
        frame.routeWhileSizing = [&]
        {
            checkedAfterInvalidation = true;
            CAPTURE(initialRect, rectangleAtSizing, releasesAtSizing, movesAtSizing,
                    messagePositionAtSizing, frame.GetScreenRect());
            CHECK(frame.nativeSizing == sizingBefore);
            if ( loseCapture )
            {
                sample.kind = wxWinUIInputKind::Release;
                sample.button = wxWinUIInputButton::Left;
                sample.buttonMask = 0;
                ++sample.timestamp;
                contact.down = false;
                wxWinUIHostInputTestAccess::RouteRootPointerSample(*host, sample, true);
                foreignCapture.reset();
                ::PostMessageW(GetHwndOf(&frame), WM_CANCELMODE, 0, 0);
            }
        };
        CHECK(::PostMessageW(GetHwndOf(&frame), NativeResizeFrame::RoutedInputMessage, 0, 0));
    };
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(*status, true, true, true,
        point, sample.pointerId, sample.timestamp));
    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("invalidated motion native sizing completion", [&]
    {
        return checkedAfterInvalidation &&
               wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    CHECK(routed);
    CAPTURE(initialRect, rectangleAtSizing, releasesAtSizing, movesAtSizing,
            messagePositionAtSizing, frame.GetScreenRect());
    CHECK(frame.nativeSizing == sizingBefore);
    CHECK(finished.entered == 1);
    CHECK(finished.exited == 1);
    CHECK(messages.moves.empty());
    CHECK(messages.releases == (loseCapture ? 0u : 1u));
    if ( !loseCapture )
    {
        CHECK(finished.cancelled == 0);
        CHECK(frame.nativeCancels == 0);
    }
    CHECK_FALSE(frame.watchdogFired);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI grip keeps native ownership while input bubbles through its slot",
          "[winui-native-resize][winui-v0-supported][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar * const status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    const bool rightToLeft = GENERATE(false, true);
    if ( rightToLeft )
        status->SetLayoutDirection(wxLayout_RightToLeft);
    frame.Show();
    YieldForAWhile(30);
    REQUIRE(wxWinUIStatusBarTestAccess::HasSizeGrip(*status));
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);

    unsigned downs = 0, moves = 0, ups = 0;
    status->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent&) { ++downs; });
    status->Bind(wxEVT_MOTION, [&](wxMouseEvent&) { ++moves; });
    status->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent&) { ++ups; });
    const wxPoint point = rightToLeft
        ? status->GetScreenRect().GetBottomLeft() + status->FromDIP(wxPoint(8, -8))
        : status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    frame.invoking = true;
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point));
    const auto generation = wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host);

    // The real grip's accepted PointerPressed then bubbles through the
    // statusbar slot (handledEventsToo) before reaching the root. Exercise
    // that existing slot route: invoking only the grip misses this handoff.
    wxWinUIPointerSample sample;
    sample.kind = wxWinUIInputKind::Press;
    sample.button = wxWinUIInputButton::Left;
    sample.buttonMask = MK_LBUTTON;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    sample.kind = wxWinUIInputKind::Move;
    sample.button = wxWinUIInputButton::None;
    ++sample.timestamp;
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    CHECK(wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host) == generation);
    CHECK(wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(*host) == 0);
    CHECK(downs == 0);
    CHECK(moves == 0);
    CHECK(frame.nativeDowns == 0);
    frame.invoking = false;

    wxWinUINativeResizeSnapshot finished;
    SECTION("the accepted request still enters the native sizing loop once")
    {
        REQUIRE(WaitFor("bubbled grip native sizing completion", [&]()
        {
            return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
                   finished.phase == wxWinUINativeResizePhase::Idle;
        }, 3000));
        CHECK(finished.entered == 1);
        CHECK(finished.exited == 1);
        CHECK(frame.nativeDowns == 1);
        CHECK(frame.nativeEntries == 1);
        CHECK(frame.nativeExits == 1);
        CHECK(frame.lastHitTest == (rightToLeft ? HTBOTTOMLEFT : HTBOTTOMRIGHT));
        CHECK(frame.cancelPosted);
    }
    SECTION("release bubbling cancels before deferred native entry")
    {
        sample.kind = wxWinUIInputKind::Release;
        sample.button = wxWinUIInputButton::Left;
        sample.buttonMask = 0;
        ++sample.timestamp;
        REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
        const auto routed = wxWinUIHostInputTestAccess::RouteRootPointerSample(
            *host, sample, true);
        CHECK(routed.cancelSource);
        REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished));
        CHECK(finished.phase == wxWinUINativeResizePhase::Idle);
        CHECK(finished.cancelled == 1);
        CHECK(finished.entered == 0);
        YieldForAWhile(20);
        CHECK(frame.nativeDowns == 0);
    }
    CHECK(finished.scheduled == 1);
    CHECK(wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(*host) == 0);
    CHECK(ups == 0);
    CHECK_FALSE(frame.downBeforeUnwind);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI grip survives same-island capture transfer and routed release",
          "[winui-native-resize][winui-native-resize-capture][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar * const status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    const bool rightToLeft = GENERATE(false, true);
    const bool captureToBridge = GENERATE(false, true);
    if ( rightToLeft )
        status->SetLayoutDirection(wxLayout_RightToLeft);
    frame.Show();
    YieldForAWhile(30);
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    const HWND inputSite = FindResizeInputSite(bridge);
    REQUIRE(inputSite);
    REQUIRE(::IsChild(bridge, inputSite));
    const HWND firstCapture = captureToBridge ? inputSite : bridge;
    const HWND secondCapture = captureToBridge ? bridge : inputSite;
    NativeResizeForeignCapture firstCaptureGuard(firstCapture);
    REQUIRE(::GetCapture() == firstCapture);

    unsigned downs = 0, moves = 0, ups = 0;
    status->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent&) { ++downs; });
    status->Bind(wxEVT_MOTION, [&](wxMouseEvent&) { ++moves; });
    status->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent&) { ++ups; });
    const wxPoint point = rightToLeft
        ? status->GetScreenRect().GetBottomLeft() + status->FromDIP(wxPoint(8, -8))
        : status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    wxWinUIPointerSample sample;
    sample.kind = wxWinUIInputKind::Press;
    sample.button = wxWinUIInputButton::Left;
    sample.buttonMask = MK_LBUTTON;
    sample.pointerId = 71;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    frame.invoking = true;
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point, sample.pointerId, sample.timestamp));
    const auto generation = wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host);
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(
        *host, sample, true).cancelSource);

    // SetCapture delivers the real WM_CAPTURECHANGED to the old island
    // HWND, through its production subclass. This is not a direct call of
    // CancelNativeResize and does not substitute capture validation.
    NativeResizeForeignCapture secondCaptureGuard(secondCapture);
    REQUIRE(::GetCapture() == secondCapture);
    wxWinUINativeResizeSnapshot pending;
    REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &pending));
    REQUIRE(pending.phase == wxWinUINativeResizePhase::Pending);
    CHECK(pending.scheduled == 1);
    CHECK(pending.cancelled == 0);
    CHECK(wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host) == generation);
    CHECK(wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(*host) == 0);

    sample.kind = wxWinUIInputKind::Move;
    sample.button = wxWinUIInputButton::None;
    ++sample.timestamp;
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(
        *host, sample, true).cancelSource);
    CHECK(frame.nativeDowns == 0);
    frame.invoking = false;

    bool callbackRan = false, entered = false, nativeCapture = false;
    bool moveAtSlot = false, moveAtRoot = false;
    bool releaseAtSlot = false, releaseAtRoot = false;
    frame.routeWhileSizing = [&]()
    {
        callbackRan = true;
        wxWinUINativeResizeSnapshot active;
        entered = wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &active) &&
                  active.phase == wxWinUINativeResizePhase::Entered;
        nativeCapture = ::GetCapture() == GetHwndOf(&frame);
        ++sample.timestamp;
        ++sample.screenX;
        ++sample.screenY;
        moveAtSlot = wxWinUIHostInputTestAccess::RouteSlotPointer(
            *host, status, sample);
        moveAtRoot = wxWinUIHostInputTestAccess::RouteRootPointerSample(
            *host, sample, true).cancelSource;
        sample.kind = wxWinUIInputKind::Release;
        sample.button = wxWinUIInputButton::Left;
        sample.buttonMask = 0;
        ++sample.timestamp;
        contact.down = false;
        releaseAtSlot = wxWinUIHostInputTestAccess::RouteSlotPointer(
            *host, status, sample);
        releaseAtRoot = wxWinUIHostInputTestAccess::RouteRootPointerSample(
            *host, sample, true).cancelSource;
    };

    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("capture transfer and routed native resize completion", [&]()
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    CHECK(callbackRan);
    CHECK(entered);
    CHECK(nativeCapture);
    CHECK(moveAtSlot);
    CHECK(moveAtRoot);
    CHECK(releaseAtSlot);
    CHECK(releaseAtRoot);
    CHECK(frame.routedInputPosted);
    CHECK(finished.ticket == pending.ticket);
    CHECK(finished.entered == 1);
    CHECK(finished.exited == 1);
    CHECK(frame.nativeDowns == 1);
    CHECK(frame.nativeEntries == 1);
    CHECK(frame.nativeExits == 1);
    CHECK(frame.lastHitTest == (rightToLeft ? HTBOTTOMLEFT : HTBOTTOMRIGHT));
    CHECK_FALSE(frame.downBeforeUnwind);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(downs == 0);
    CHECK(moves == 0);
    CHECK(ups == 0);
    CHECK(wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(*host) == 0);
    CHECK(::GetCapture() == nullptr);
    YieldForAWhile(20);
    CHECK(frame.nativeDowns == 1);
}

TEST_CASE("wxWinUI grip capture handoff does not hide real pending interruptions",
          "[winui-native-resize][winui-native-resize-capture][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar * const status = frame.CreateStatusBar();
    REQUIRE(status);
    wxWindow foreign(&frame, wxID_ANY);
    NativeResizeContactOverride contact;
    frame.Show();
    YieldForAWhile(30);
    REQUIRE(::GetCapture() == nullptr);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    const HWND bridge = host->GetBridgeHwnd();
    const HWND inputSite = FindResizeInputSite(bridge);
    REQUIRE(inputSite);
    NativeResizeForeignCapture bridgeCapture(bridge);
    REQUIRE(::GetCapture() == bridge);
    const wxPoint point = status->GetScreenRect().GetBottomRight() -
        status->FromDIP(wxPoint(8, 8));
    wxWinUIPointerSample sample;
    sample.kind = wxWinUIInputKind::Press;
    sample.button = wxWinUIInputButton::Left;
    sample.buttonMask = MK_LBUTTON;
    sample.pointerId = 83;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point, sample.pointerId, sample.timestamp));
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(
        *host, sample, true).cancelSource);
    NativeResizeForeignCapture inputCapture(inputSite);
    REQUIRE(::GetCapture() == inputSite);
    wxWinUINativeResizeSnapshot state;
    REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &state));
    REQUIRE(state.phase == wxWinUINativeResizePhase::Pending);

    std::unique_ptr<NativeResizeForeignCapture> foreignCapture;
    bool deferredContactCheck = false;
    SECTION("capture is really released")
    {
        REQUIRE(::ReleaseCapture());
        CHECK(::GetCapture() == nullptr);
    }
    SECTION("capture moves outside the island")
    {
        foreignCapture = std::make_unique<NativeResizeForeignCapture>(
            GetHwndOf(&foreign));
        REQUIRE(::GetCapture() == GetHwndOf(&foreign));
    }
    SECTION("the island receives WM_CANCELMODE")
    {
        ::SendMessageW(inputSite, WM_CANCELMODE, 0, 0);
    }
    SECTION("the exact pointer releases before the wake")
    {
        sample.kind = wxWinUIInputKind::Release;
        sample.buttonMask = 0;
        ++sample.timestamp;
        contact.down = false;
        REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
        CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(
            *host, sample, true).cancelSource);
    }
    SECTION("physical contact ends without a routed release")
    {
        contact.down = false;
        deferredContactCheck = true;
    }
    REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &state));
    CHECK(state.phase == (deferredContactCheck
        ? wxWinUINativeResizePhase::Pending : wxWinUINativeResizePhase::Idle));
    REQUIRE(WaitFor("pending resize cancellation after an internal handoff", [&]()
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &state) &&
               state.phase == wxWinUINativeResizePhase::Idle;
    }));
    CHECK(state.scheduled == 1);
    CHECK(state.cancelled == 1);
    CHECK(state.entered == 0);
    CHECK(state.exited == 0);
    CHECK(frame.nativeDowns == 0);
    CHECK(frame.nativeEntries == 0);
    CHECK(frame.nativeExits == 0);
    if ( foreignCapture )
        CHECK(::GetCapture() == GetHwndOf(&foreign));
}

TEST_CASE("wxWinUI grip keeps a held drag when it leaves before the native wake",
          "[winui-native-resize][winui-native-resize-leave][statusbar][input]")
{
    const bool rightToLeft = GENERATE(false, true);
    // Keep both terminal paths: an observed release at the boundary, and a
    // release outside the island after the final routed sample was still down.
    const int release = GENERATE(0, 1, 2);
    CAPTURE(rightToLeft, release);
    NativeResizeFrame frame;
    wxStatusBar* const status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    if ( rightToLeft ) status->SetLayoutDirection(wxLayout_RightToLeft);
    frame.Show();
    YieldForAWhile(30);
    REQUIRE(frame.ArmWatchdog());
    REQUIRE(::GetCapture() == nullptr);
    auto* const host = wxWinUITopLevelHost::ForWindow(&frame, false);
    REQUIRE(host);
    const wxPoint point = rightToLeft
        ? status->GetScreenRect().GetBottomLeft() + status->FromDIP(wxPoint(8, -8))
        : status->GetScreenRect().GetBottomRight() - status->FromDIP(wxPoint(8, 8));
    wxWinUIPointerSample sample;
    sample.kind = wxWinUIInputKind::Press;
    sample.button = wxWinUIInputButton::Left;
    sample.buttonMask = MK_LBUTTON;
    sample.pointerId = 89;
    sample.timestamp = ::GetTickCount64();
    sample.screenX = point.x;
    sample.screenY = point.y;
    frame.invoking = true;
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point, sample.pointerId, sample.timestamp));
    const auto generation = wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host);
    REQUIRE(wxWinUIHostInputTestAccess::RouteSlotPointer(*host, status, sample));
    CHECK(wxWinUIHostInputTestAccess::RouteRootPointerSample(
        *host, sample, true).cancelSource);

    // Route the same boundary path used by XAML PointerExited, before pumping
    // the posted resize request. No physical cursor or button is modified.
    ++sample.timestamp;
    sample.screenX = rightToLeft ? frame.GetScreenRect().GetLeft() - 20
                                 : frame.GetScreenRect().GetRight() + 20;
    sample.screenY = frame.GetScreenRect().GetBottom() + 20;
    if ( release == 1 )
    {
        sample.buttonMask = 0;
        contact.down = false;
    }
    REQUIRE(wxWinUIHostInputTestAccess::RouteRootPointerBoundary(*host, false, sample));
    wxWinUINativeResizeSnapshot pending;
    REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &pending));
    CHECK(pending.scheduled == 1);
    CHECK(pending.phase == (release == 1 ? wxWinUINativeResizePhase::Idle
                                        : wxWinUINativeResizePhase::Pending));
    CHECK(wxWinUIHostInputTestAccess::GetInputSiteGeneration(*host) == generation);
    CHECK(wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(*host) == 0);
    CHECK(frame.nativeDowns == 0);
    if ( release == 2 ) contact.down = false;
    frame.invoking = false;

    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("held resize crossing the root boundary", [&]
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }, 3000));
    const unsigned expectedEntries = release == 0 ? 1 : 0;
    CHECK(finished.entered == expectedEntries);
    CHECK(finished.exited == expectedEntries);
    CHECK(frame.nativeDowns == expectedEntries);
    CHECK(frame.nativeEntries == expectedEntries);
    CHECK(frame.nativeExits == expectedEntries);
    if ( release == 0 )
    {
        CHECK(frame.lastHitTest == (rightToLeft ? HTBOTTOMLEFT : HTBOTTOMRIGHT));
        CHECK(frame.enteredPhaseObserved);
        CHECK(frame.idlePhaseObserved);
        CHECK(frame.cancelPosted);
    }
    else CHECK(finished.cancelled == 1);
    CHECK_FALSE(frame.downBeforeUnwind);
    CHECK_FALSE(frame.watchdogFired);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI grip cancels stale requests before native dispatch",
          "[winui-native-resize][winui-v0-supported][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar *status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    frame.Show();
    YieldForAWhile(30);
    REQUIRE(wxWinUIStatusBarTestAccess::HasSizeGrip(*status));
    const wxPoint point = status->GetScreenRect().GetBottomRight() -
        status->FromDIP(wxPoint(8, 8));
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point));

    SECTION("released before the private message is processed")
    {
        contact.down = false;
    }
    SECTION("window disabled while pending")
    {
        frame.Enable(false);
    }
    SECTION("window hidden while pending")
    {
        frame.Hide();
    }
    SECTION("resize border removed while pending")
    {
        frame.SetWindowStyleFlag(frame.GetWindowStyleFlag() & ~wxRESIZE_BORDER);
    }
    SECTION("direction changes while pending")
    {
        status->SetLayoutDirection(wxLayout_RightToLeft);
    }
    SECTION("peer rebuilt while pending")
    {
        const int styles[] = { wxSB_RAISED };
        status->SetStatusStyles(1, styles);
    }
    SECTION("source destroyed while pending")
    {
        frame.SetStatusBar(nullptr);
        delete status;
        status = nullptr;
    }

    wxWinUINativeResizeSnapshot finished;
    REQUIRE(WaitFor("stale native resize cancellation", [&]()
    {
        return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
               finished.phase == wxWinUINativeResizePhase::Idle;
    }));
    CHECK(finished.scheduled == 1);
    CHECK(finished.cancelled == 1);
    CHECK(finished.entered == 0);
    CHECK(finished.exited == 0);
    CHECK(frame.nativeDowns == 0);
    CHECK(frame.nativeEntries == 0);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI grip preserves foreign capture and rolls back post failure",
          "[winui-native-resize][winui-v0-supported][statusbar][input]")
{
    NativeResizeFrame frame;
    wxStatusBar * const status = frame.CreateStatusBar();
    REQUIRE(status);
    NativeResizeContactOverride contact;
    frame.Show();
    YieldForAWhile(30);
    const wxPoint point = status->GetScreenRect().GetBottomRight() -
        status->FromDIP(wxPoint(8, 8));

    SECTION("foreign capture acquired after request")
    {
        wxWindow foreign(&frame, wxID_ANY);
        REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
            *status, true, true, true, point));
        NativeResizeForeignCapture capture(GetHwndOf(&foreign));
        REQUIRE(::GetCapture() == GetHwndOf(&foreign));
        wxWinUINativeResizeSnapshot finished;
        REQUIRE(WaitFor("foreign capture cancellation", [&]()
        {
            return wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &finished) &&
                   finished.phase == wxWinUINativeResizePhase::Idle;
        }));
        CHECK(finished.cancelled == 1);
        CHECK(::GetCapture() == GetHwndOf(&foreign));
    }
    SECTION("failed post leaves no accepted transaction")
    {
        wxWinUIHostResizeTestAccess::FailNextPost();
        CHECK_FALSE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(*status,
            true, true, true, point));
        wxWinUINativeResizeSnapshot state;
        REQUIRE(wxWinUIHostResizeTestAccess::GetSnapshot(&frame, &state));
        CHECK(state.phase == wxWinUINativeResizePhase::Idle);
        CHECK(state.scheduled == 0);
        YieldForAWhile(20);
    }
    CHECK(frame.nativeDowns == 0);
    CHECK(frame.nativeEntries == 0);
    CHECK(frame.nativeExits == 0);
}

TEST_CASE("wxWinUI queued grip cannot outlive its top-level window",
          "[winui-native-resize][winui-v0-supported][statusbar][input]")
{
    NativeResizeContactOverride contact;
    auto frame = std::make_unique<NativeResizeFrame>();
    wxStatusBar * const status = frame->CreateStatusBar();
    REQUIRE(status);
    frame->Show();
    YieldForAWhile(30);
    const wxPoint point = status->GetScreenRect().GetBottomRight() -
        status->FromDIP(wxPoint(8, 8));
    REQUIRE(wxWinUIStatusBarTestAccess::InvokeSizeGrip(
        *status, true, true, true, point));
    frame.reset();
    YieldForAWhile(20);
    CHECK(::GetCapture() == nullptr);
}

TEST_CASE("wxWinUI StatusBar mirrors logical fields and controls in RTL",
          "[winui-chrome-feedback][statusbar][geometry][rtl]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    status.SetClientSize(parent->FromDIP(wxSize(420, 34)));
    const int widths[] =
    {
        parent->FromDIP(71),
        -1,
        parent->FromDIP(137)
    };
    status.SetFieldsCount(3, widths);

#if wxUSE_BUTTON
    auto * const firstControl =
        new wxButton(&status, wxID_ANY, "first logical field");
    auto * const lastControl =
        new wxButton(&status, wxID_ANY, "last logical field");
    REQUIRE(status.AddFieldControl(0, firstControl));
    REQUIRE(status.AddFieldControl(2, lastControl));
#endif

    status.SendSizeEvent();
    wxRect ltrFields[3];
    for ( int i = 0; i < 3; ++i )
        REQUIRE(status.GetFieldRect(i, ltrFields[i]));
    CHECK(ltrFields[0].width != ltrFields[2].width);
#if wxUSE_BUTTON
    CHECK(firstControl->GetRect() == ltrFields[0]);
    CHECK(lastControl->GetRect() == ltrFields[2]);
    const wxRect ltrFirstScreen = firstControl->GetScreenRect();
    const wxRect ltrLastScreen = lastControl->GetScreenRect();
    CHECK(ltrFirstScreen.GetPosition() ==
          status.ClientToScreen(ltrFields[0].GetPosition()));
    CHECK(ltrLastScreen.GetPosition() ==
          status.ClientToScreen(ltrFields[2].GetPosition()));
    CHECK(ltrFirstScreen.x < ltrLastScreen.x);
#endif

    int gridFlow = -1;
    int textFlow = -1;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, &gridFlow, &textFlow));
    CHECK(gridFlow == static_cast<int>(MUX::FlowDirection::LeftToRight));
    CHECK(textFlow == static_cast<int>(MUX::FlowDirection::LeftToRight));

    status.SetLayoutDirection(wxLayout_RightToLeft);
    status.SendSizeEvent();
    for ( int i = 0; i < 3; ++i )
    {
        wxRect rtlField;
        REQUIRE(status.GetFieldRect(i, rtlField));
        CHECK(rtlField == ltrFields[i]);
    }
#if wxUSE_BUTTON
    CHECK(firstControl->GetRect() == ltrFields[0]);
    CHECK(lastControl->GetRect() == ltrFields[2]);
    const wxRect rtlFirstScreen = firstControl->GetScreenRect();
    const wxRect rtlLastScreen = lastControl->GetScreenRect();
    CHECK(rtlFirstScreen.x > rtlLastScreen.x);
    CHECK(rtlFirstScreen.GetPosition() ==
          status.ClientToScreen(
              wxPoint(ltrFields[0].x + ltrFields[0].width,
                      ltrFields[0].y)));
    CHECK(rtlLastScreen.GetPosition() ==
          status.ClientToScreen(
              wxPoint(ltrFields[2].x + ltrFields[2].width,
                      ltrFields[2].y)));
    CHECK(rtlFirstScreen.GetSize() == ltrFirstScreen.GetSize());
    CHECK(rtlLastScreen.GetSize() == ltrLastScreen.GetSize());
#endif
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, &gridFlow, &textFlow));
    CHECK(gridFlow == static_cast<int>(MUX::FlowDirection::RightToLeft));
    CHECK(textFlow == static_cast<int>(MUX::FlowDirection::RightToLeft));

    status.SetLayoutDirection(wxLayout_LeftToRight);
    status.SendSizeEvent();
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        2, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, &gridFlow, &textFlow));
    CHECK(gridFlow == static_cast<int>(MUX::FlowDirection::LeftToRight));
    CHECK(textFlow == static_cast<int>(MUX::FlowDirection::LeftToRight));
#if wxUSE_BUTTON
    CHECK(firstControl->GetRect() == ltrFields[0]);
    CHECK(lastControl->GetRect() == ltrFields[2]);
#endif
}

TEST_CASE("wxWinUI StatusBar distinguishes field borders without fixed colours",
          "[winui-chrome-feedback][statusbar][styles]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    status.SetFieldsCount(3);
    const int styles[] = { wxSB_FLAT, wxSB_RAISED, wxSB_SUNKEN };
    status.SetStatusStyles(3, styles);

    for ( int i = 0; i < 3; ++i )
    {
        int observed = -1;
        int borderElements = -1;
        REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
            i, nullptr, nullptr, &observed, nullptr,
            nullptr, nullptr, nullptr, &borderElements));
        CHECK(observed == styles[i]);
        CHECK(borderElements == (i == 0 ? 0 : 4));
    }
    CHECK(wxWinUIStatusBarTestAccess::UsesThemeBorders(status));
}

TEST_CASE("wxWinUI StatusBar implements all ellipsis modes and SHOW_TIPS",
          "[winui-chrome-feedback][statusbar][ellipsis]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxString full =
        "A deliberately long status message for deterministic clipping";

    struct EllipsisCase
    {
        long style;
        MUX::TextTrimming trimming;
        bool manual;
    };
    const EllipsisCase cases[] =
    {
        { wxSTB_ELLIPSIZE_START, MUX::TextTrimming::None, true },
        { wxSTB_ELLIPSIZE_MIDDLE, MUX::TextTrimming::None, true },
        { wxSTB_ELLIPSIZE_END,
          MUX::TextTrimming::CharacterEllipsis, false },
    };

    for ( const auto& test : cases )
    {
        wxStatusBar status(
            parent, wxID_ANY, test.style | wxSTB_SHOW_TIPS);
        status.SetClientSize(parent->FromDIP(wxSize(88, 28)));
        const int width[] = { parent->FromDIP(80) };
        status.SetStatusWidths(1, width);
        status.SetStatusText(full);

        wxString rendered;
        int trimming = -1;
        bool hasToolTip = false;
        wxString automationName;
        REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
            0, &rendered, &trimming, nullptr, &hasToolTip,
            nullptr, nullptr, &automationName));
        CHECK(trimming == static_cast<int>(test.trimming));
        CHECK((rendered != full) == test.manual);
        CHECK(status.GetField(0).IsEllipsized());
#if wxUSE_TOOLTIPS
        CHECK(hasToolTip);
#else
        CHECK_FALSE(hasToolTip);
#endif
        CHECK(automationName == full);
    }
}

TEST_CASE("wxWinUI StatusBar applies and clears appearance and places controls",
          "[winui-chrome-feedback][statusbar][appearance]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent);
    status.SetClientSize(parent->FromDIP(wxSize(360, 32)));
    status.SetFieldsCount(2);
    status.SetStatusText("Ready", 0);
    status.SetStatusText("Selection details", 1);
    status.PushStatusText("Menu help", 0);
    wxString rendered;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr));
    CHECK(rendered == "Menu help");
    status.PopStatusText(0);
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr));
    CHECK(rendered == "Ready");
    status.SetStatusText("R&D ready", 1);
    wxString fieldName;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        1, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, &fieldName));
    CHECK(fieldName == "R&D ready");

    wxWinUIAppearanceSnapshot snapshot;
    wxYield();
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CHECK(snapshot.automationName.empty());
    CHECK_FALSE(snapshot.localizedControlType.empty());

    status.SetFont(MakeChromeTestFont());
    status.SetForegroundColour(wxColour(14, 61, 117));
    status.SetBackgroundColour(wxColour(31, 37, 43));
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CheckChromeFontLocals(snapshot, true);
    CHECK(snapshot.hasForeground);
    CHECK(snapshot.hasBackground);

    status.SetFont(wxNullFont);
    status.SetForegroundColour(wxNullColour);
    status.SetBackgroundColour(wxNullColour);
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CheckChromeFontLocals(snapshot, false);
    CHECK_FALSE(snapshot.hasForeground);
    CHECK_FALSE(snapshot.hasBackground);

#if wxUSE_BUTTON
    auto * const child =
        new wxButton(&status, wxID_ANY, "field control");
    REQUIRE(status.AddFieldControl(1, child));
    status.SendSizeEvent();
    wxYield();
    wxRect field;
    REQUIRE(status.GetFieldRect(1, field));
    CHECK(child->GetRect() == field);
#endif
}

TEST_CASE("wxWinUI StatusBar imposes minimum height immediately and safely",
          "[winui-chrome-feedback][statusbar][geometry][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto expectedHeight =
        [](const wxStatusBar& status, int requested)
        {
            const int natural =
                std::max(
                    (11 * status.GetCharHeight()) / 10 +
                        2 * status.GetBorderY(),
                    status.FromDIP(26));
            return std::max(
                natural, requested + 2 * status.GetBorderY());
        };

    wxStatusBar status(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    const int requested = parent->FromDIP(48);
    const int expected = expectedHeight(status, requested);
    status.SetMinHeight(requested);
    CHECK(status.GetMinSize().y == expected);
    CHECK(status.GetSize().y == expected);
    CHECK(status.GetBestSize().y >= expected);

    // On fractional scales, find a physical height that would change under an
    // integer DIP round-trip. Reapplying the stored request for an unrelated
    // font update must retain the exact caller-visible pixel height.
    wxStatusBar exactStatus(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    const int exactNaturalHeight = expectedHeight(exactStatus, 0);
    int inexactRoundTripHeight = exactNaturalHeight + 1;
    const int inexactSearchLimit = exactNaturalHeight + 256;
    for ( ; inexactRoundTripHeight <= inexactSearchLimit;
          ++inexactRoundTripHeight )
    {
        const int integerDIPs =
            exactStatus.ToDIP(
                wxSize(wxDefaultCoord, inexactRoundTripHeight)).y;
        if ( exactStatus.FromDIP(
                 wxSize(wxDefaultCoord, integerDIPs)).y !=
             inexactRoundTripHeight )
        {
            break;
        }
    }
    if ( inexactRoundTripHeight <= inexactSearchLimit )
    {
        const int exactExpected =
            expectedHeight(exactStatus, inexactRoundTripHeight);
        exactStatus.SetMinHeight(inexactRoundTripHeight);
        exactStatus.SetFont(exactStatus.GetFont());
        CHECK(exactStatus.GetMinSize().y == exactExpected);
        CHECK(exactStatus.GetSize().y == exactExpected);
    }

    StatusBarMinHeightReentryContext nestedContext;
    nestedContext.height = parent->FromDIP(67);
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::MinHeightBeforeResize,
        &SetNestedStatusBarMinHeight,
        &nestedContext);
    status.SetMinHeight(parent->FromDIP(53));
    const int nestedExpected =
        expectedHeight(status, nestedContext.height);
    CHECK(nestedContext.calls == 1);
    CHECK(status.GetMinSize().y == nestedExpected);
    CHECK(status.GetSize().y == nestedExpected);
    CHECK(status.GetBestSize().y >= nestedExpected);

    std::unique_ptr<wxStatusBar> owned(
        new wxStatusBar(parent, wxID_ANY, wxSTB_SHOW_TIPS));
    StatusBarDestructionReentryContext destructionContext{
        &owned
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(*owned,
        wxWinUIStatusBarReentryPointForTesting::MinHeightBeforeResize,
        &DestroyStatusBarDuringRebuild,
        &destructionContext);
    wxStatusBar * const invoking = owned.get();
    invoking->SetMinHeight(parent->FromDIP(58));
    CHECK(destructionContext.calls == 1);
    CHECK(destructionContext.ownerMatched);
    CHECK(owned == nullptr);

    wxStatusBar twoStage;
    const int preCreateHeight = parent->FromDIP(61);
    twoStage.SetMinHeight(preCreateHeight);
    REQUIRE(twoStage.Create(parent, wxID_ANY, wxSTB_SHOW_TIPS));
    const int preCreateExpected =
        expectedHeight(twoStage, preCreateHeight);
    CHECK(twoStage.GetMinSize().y == preCreateExpected);
    CHECK(twoStage.GetSize().y == preCreateExpected);

    // Deliver an explicit synthetic DPI transition so this remains
    // deterministic even when the test machine has only one monitor. The
    // handler must materialize the stored logical request at newDPI instead
    // of treating the original physical pixels as a fresh DIP request.
    wxStatusBar dpiStatus(parent, wxID_ANY, wxSTB_SHOW_TIPS);
#if wxUSE_BUTTON
    auto * const dpiControl =
        new wxButton(&dpiStatus, wxID_ANY, "DPI-height field control");
    REQUIRE(dpiStatus.AddFieldControl(0, dpiControl));
#endif
    const int logicalHeightDIPs = 64;
    const int dpiRequest =
        dpiStatus.FromDIP(
            wxSize(wxDefaultCoord, logicalHeightDIPs)).y;
    dpiStatus.SetMinHeight(dpiRequest);
    const wxSize oldDPI = dpiStatus.GetDPI();
    REQUIRE(oldDPI.x > 0);
    REQUIRE(oldDPI.y > 0);
    const wxSize newDPI(2 * oldDPI.x, 2 * oldDPI.y);
    wxDPIChangedEvent dpiEvent(oldDPI, newDPI);
    dpiEvent.SetEventObject(&dpiStatus);
    dpiEvent.SetId(dpiStatus.GetId());
    wxWinUIStatusBarTestAccess::DeliverDPIChanged(dpiStatus, dpiEvent);
    const wxDPIChangedEvent logicalScaleEvent(
        wxDisplay::GetStdPPI(), newDPI);
    const int scaledLogicalHeight =
        logicalScaleEvent.ScaleY(logicalHeightDIPs);
    const int dpiExpected =
        expectedHeight(dpiStatus, scaledLogicalHeight);
    CHECK(dpiStatus.GetMinSize().y == dpiExpected);
    CHECK(dpiStatus.GetSize().y == dpiExpected);
    CHECK(dpiStatus.GetBestSize().y >= dpiExpected);
    wxRect dpiField;
    REQUIRE(dpiStatus.GetFieldRect(0, dpiField));
    CHECK(dpiField.height == scaledLogicalHeight);
#if wxUSE_BUTTON
    CHECK(dpiControl->GetRect() == dpiField);
#endif
}

TEST_CASE("wxWinUI StatusBar nested peer writes preserve the last wx mutation",
          "[winui-chrome-feedback][statusbar][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent);
    status.SetClientSize(parent->FromDIP(wxSize(360, 32)));

    StatusBarTextReentryContext textContext{
        "nested text wins"
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::TextValue,
        &SetNestedStatusBarText,
        &textContext);
    status.SetStatusText("stale outer text");
    CHECK(textContext.calls == 1);
    CHECK(status.GetStatusText() == textContext.text);

    wxString rendered;
    wxString automationName;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, &automationName));
    CHECK(rendered == textContext.text);
    CHECK(automationName == textContext.text);

    status.SetFont(wxNullFont);
    StatusBarAppearanceReentryContext appearanceContext;
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::AppearanceRootFont,
        &ClearNestedStatusBarFont,
        &appearanceContext);
    status.SetFont(MakeChromeTestFont());
    CHECK(appearanceContext.calls == 1);

    wxWinUIAppearanceSnapshot snapshot;
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CheckChromeFontLocals(snapshot, false);

    // Cross-domain convergence: an appearance transaction re-entered by a
    // text mutation must publish both the outer colour and the nested text.
    status.SetForegroundColour(wxNullColour);
    StatusBarTextReentryContext crossTextContext{
        "nested text during appearance"
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::TextValue,
        &SetNestedStatusBarText,
        &crossTextContext);
    status.SetForegroundColour(wxColour(22, 73, 141));
    CHECK(crossTextContext.calls == 1);
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, &automationName));
    CHECK(rendered == crossTextContext.text);
    CHECK(automationName == crossTextContext.text);
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CHECK(snapshot.hasForeground);

    // And the inverse: a text transaction re-entered by an appearance
    // mutation must retain the text/UIA pair while projecting the new colour.
    status.SetForegroundColour(wxNullColour);
    StatusBarColourReentryContext crossColourContext{
        wxColour(127, 35, 91)
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::AppearanceRootFont,
        &SetNestedStatusBarForeground,
        &crossColourContext);
    const wxString crossDomainText = "text survives nested appearance";
    status.SetStatusText(crossDomainText);
    CHECK(crossColourContext.calls == 1);
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, &automationName));
    CHECK(rendered == crossDomainText);
    CHECK(automationName == crossDomainText);
    REQUIRE(wxWinUIStatusBarTestAccess::GetAppearance(status, &snapshot));
    CHECK(snapshot.hasForeground);
}

TEST_CASE("wxWinUI StatusBar rebuild publishes only its latest revision",
          "[winui-chrome-feedback][statusbar][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent);
    status.SetStatusText("initial");
    const unsigned long long revisionBefore =
        wxWinUIStatusBarTestAccess::GetModelRevision(status);
    StatusBarTextReentryContext textContext{
        "text changed from Loaded",
        0,
        true
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
        &SetNestedStatusBarText,
        &textContext);
    const int raised = wxSB_RAISED;
    status.SetStatusStyles(1, &raised);

    CHECK(textContext.calls == 1);
    CHECK(textContext.peerReadable);
    CHECK(textContext.observedStyle == wxSB_RAISED);
    CHECK(wxWinUIStatusBarTestAccess::GetModelRevision(status) >=
          revisionBefore + 2);
    CHECK(status.GetStatusText() == textContext.text);
    wxString rendered;
    int observedStyle = -1;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, &rendered, nullptr, &observedStyle, nullptr,
        nullptr, nullptr, nullptr));
    CHECK(rendered == textContext.text);
    CHECK(observedStyle == wxSB_RAISED);

    std::unique_ptr<wxStatusBar> owned(
        new wxStatusBar(parent, wxID_ANY, wxSTB_SHOW_TIPS));
    REQUIRE(owned);
    StatusBarDestructionReentryContext destructionContext{
        &owned
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(*owned,
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
        &DestroyStatusBarDuringRebuild,
        &destructionContext);
    wxStatusBar * const invoking = owned.get();
    const int flat = wxSB_FLAT;
    invoking->SetStatusStyles(1, &flat);
    CHECK(destructionContext.calls == 1);
    CHECK(destructionContext.ownerMatched);
    CHECK(owned == nullptr);

    // Destruction is equally valid from a detached candidate DP boundary,
    // before SetContent() has had a chance to publish it.
    std::unique_ptr<wxStatusBar> textOwned(
        new wxStatusBar(parent, wxID_ANY, wxSTB_SHOW_TIPS));
    REQUIRE(textOwned);
    StatusBarDestructionReentryContext textDestructionContext{
        &textOwned
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(*textOwned,
        wxWinUIStatusBarReentryPointForTesting::TextValue,
        &DestroyStatusBarDuringRebuild,
        &textDestructionContext);
    wxStatusBar * const textInvoking = textOwned.get();
    textInvoking->SetStatusText("destroy from candidate text write");
    CHECK(textDestructionContext.calls == 1);
    CHECK(textDestructionContext.ownerMatched);
    CHECK(textOwned == nullptr);

    wxStatusBar dpiReentrant(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    StatusBarTextReentryContext dpiTextContext{
        "nested text during DPI conversion"
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(dpiReentrant,
        wxWinUIStatusBarReentryPointForTesting::DPIBorderX,
        &SetNestedStatusBarText,
        &dpiTextContext);
    const unsigned long long dpiRevisionBefore =
        wxWinUIStatusBarTestAccess::GetModelRevision(dpiReentrant);
    wxDPIChangedEvent reentrantDpiEvent(
        wxSize(96, 96), wxSize(144, 144));
    reentrantDpiEvent.SetEventObject(&dpiReentrant);
    reentrantDpiEvent.SetId(dpiReentrant.GetId());
    dpiReentrant.ProcessWindowEvent(reentrantDpiEvent);
    CHECK(dpiTextContext.calls == 1);
    CHECK(dpiReentrant.GetStatusText() == dpiTextContext.text);
    wxString dpiRendered;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(dpiReentrant,
        0, &dpiRendered, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr));
    CHECK(dpiRendered == dpiTextContext.text);
    CHECK(wxWinUIStatusBarTestAccess::GetModelRevision(dpiReentrant) >=
          dpiRevisionBefore + 2);

    // A nested DPI delivery is a newer writer even when the outer handler
    // resumes afterwards. Its physical height must not be overwritten by the
    // stale outer event.
    wxStatusBar dpiLastWriter(parent, wxID_ANY, wxSTB_SHOW_TIPS);
    const wxSize currentDPI = dpiLastWriter.GetDPI();
    REQUIRE(currentDPI.x > 0);
    REQUIRE(currentDPI.y > 0);
    const int logicalHeight = 80;
    const int requestedHeight =
        dpiLastWriter.FromDIP(
            wxSize(wxDefaultCoord, logicalHeight)).y;
    dpiLastWriter.SetMinHeight(requestedHeight);
    StatusBarDPIReentryContext nestedDPI{
        currentDPI,
        wxSize(currentDPI.x * 3 / 2, currentDPI.y * 3 / 2)
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(dpiLastWriter,
        wxWinUIStatusBarReentryPointForTesting::DPIBorderX,
        &DeliverNestedStatusBarDPI,
        &nestedDPI);
    wxDPIChangedEvent staleOuterDPI(
        currentDPI,
        wxSize(currentDPI.x * 2, currentDPI.y * 2));
    staleOuterDPI.SetEventObject(&dpiLastWriter);
    staleOuterDPI.SetId(dpiLastWriter.GetId());
    wxWinUIStatusBarTestAccess::DeliverDPIChanged(dpiLastWriter, staleOuterDPI);

    const int nestedPixels = wxRound(
        static_cast<double>(requestedHeight) *
        nestedDPI.newDPI.y / currentDPI.y);
    const int nestedExpected =
        nestedPixels +
        2 * dpiLastWriter.FromDIP(
                wxSize(wxDefaultCoord, 2)).y;
    CHECK(nestedDPI.calls == 1);
    CHECK(dpiLastWriter.GetMinSize().y == nestedExpected);
    CHECK(dpiLastWriter.GetSize().y == nestedExpected);

    // The DPI handler must also treat each DIP conversion as observable:
    // deletion after the first conversion may not reach the second conversion
    // or touch the retired implementation.
    std::unique_ptr<wxStatusBar> dpiOwned(
        new wxStatusBar(parent, wxID_ANY, wxSTB_SHOW_TIPS));
    REQUIRE(dpiOwned);
    StatusBarDestructionReentryContext dpiDestructionContext{
        &dpiOwned
    };
    wxWinUIStatusBarTestAccess::SetNextReentryHook(*dpiOwned,
        wxWinUIStatusBarReentryPointForTesting::DPIBorderX,
        &DestroyStatusBarDuringRebuild,
        &dpiDestructionContext);
    wxStatusBar * const dpiInvoking = dpiOwned.get();
    wxDPIChangedEvent dpiEvent(wxSize(96, 96), wxSize(144, 144));
    dpiEvent.SetEventObject(dpiInvoking);
    dpiEvent.SetId(dpiInvoking->GetId());
    // Direct handler delivery is intentional: synchronously deleting an
    // wxEvtHandler while ProcessWindowEvent() is traversing its dynamic table
    // is outside the event-system contract. This seam isolates the StatusBar
    // post-conversion lifetime guarantee that is under test.
    wxWinUIStatusBarTestAccess::DeliverDPIChanged(*dpiInvoking, dpiEvent);
    CHECK(dpiDestructionContext.calls == 1);
    CHECK(dpiDestructionContext.ownerMatched);
    CHECK(dpiOwned == nullptr);
}

TEST_CASE("wxWinUI StatusBar bounds a perpetually reentrant rebuild",
          "[winui-chrome-feedback][statusbar][reentrancy][deferred]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent);
    StatusBarRebuildStormContext context;
    context.targetCalls = 12;
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
        &ContinueStatusBarRebuildStorm,
        &context);
    const int initialStyle = wxSB_FLAT;
    status.SetStatusStyles(1, &initialStyle);

    CHECK(context.calls > 1);
    CHECK(wxWinUIStatusBarTestAccess::HasDeferredRebuild(status));

    REQUIRE(WaitFor("deferred status-bar rebuild", [&]()
    {
        int observedStyle = -1;
        return !wxWinUIStatusBarTestAccess::HasDeferredRebuild(status) &&
               context.calls == context.targetCalls &&
               wxWinUIStatusBarTestAccess::GetFieldState(status,
                   0, nullptr, nullptr, &observedStyle, nullptr,
                   nullptr, nullptr, nullptr) &&
               observedStyle == context.latestStyle;
    }));
}

TEST_CASE("wxWinUI StatusBar quarantines an unbounded rebuild storm and "
          "rearms on a fresh mutation",
          "[winui-chrome-feedback][statusbar][reentrancy][quarantine]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxStatusBar status(parent);
    StatusBarRebuildStormContext context;
    context.targetCalls = 1000000;
    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
        &ContinueStatusBarRebuildStorm,
        &context);

    YieldOnceLogTarget yieldingLog;
    const int initialStyle = wxSB_FLAT;
    status.SetStatusStyles(1, &initialStyle);

    // The overrun is reported as a debug diagnostic: whether it reaches this
    // log target depends on the build, so it is not required to yield.
    CHECK(context.calls == 8);
    CHECK(wxWinUIStatusBarTestAccess::HasDeferredRebuild(status));
    REQUIRE(WaitFor("bounded status-bar rebuild quarantine", [&]()
    {
        return context.calls == 16 &&
               !wxWinUIStatusBarTestAccess::HasDeferredRebuild(status) &&
               wxWinUIStatusBarTestAccess::IsRebuildQuarantined(status);
    }));

    const int callsAtQuarantine = context.calls;
    wxYield();
    wxYield();
    CHECK(context.calls == callsAtQuarantine);

    wxWinUIStatusBarTestAccess::SetNextReentryHook(status,
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded,
        nullptr,
        nullptr);
    const int recoveredStyle = wxSB_NORMAL;
    status.SetStatusStyles(1, &recoveredStyle);
    CHECK_FALSE(wxWinUIStatusBarTestAccess::HasDeferredRebuild(status));
    CHECK_FALSE(wxWinUIStatusBarTestAccess::IsRebuildQuarantined(status));

    int observedStyle = -1;
    REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
        0, nullptr, nullptr, &observedStyle, nullptr,
        nullptr, nullptr, nullptr));
    CHECK(observedStyle == recoveredStyle);
}

#if wxUSE_TOOLTIPS

TEST_CASE("wxWinUI StatusBar rebuild and teardown retire managed tooltips",
          "[winui-chrome-feedback][statusbar][tooltip][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIToolTipPolicyTestAccess::GetManagedToolTipCount();

    {
        wxStatusBar status(
            parent, wxID_ANY,
            wxSTB_SHOW_TIPS | wxSTB_ELLIPSIZE_START);
        status.SetClientSize(parent->FromDIP(wxSize(96, 28)));
        const int width[] = { parent->FromDIP(88) };
        status.SetStatusWidths(1, width);
        status.SetStatusText(
            "A deliberately long status message with a managed tooltip");

        bool hasToolTip = false;
        REQUIRE(wxWinUIStatusBarTestAccess::GetFieldState(status,
            0, nullptr, nullptr, nullptr, &hasToolTip,
            nullptr, nullptr, nullptr));
        REQUIRE(hasToolTip);
        CHECK(wxWinUIToolTipPolicyTestAccess::GetManagedToolTipCount() ==
              baseline + 1);

        for ( int i = 0; i < 20; ++i )
        {
            const int style =
                i % 2 ? wxSB_RAISED : wxSB_SUNKEN;
            status.SetStatusStyles(1, &style);
            CHECK(wxWinUIToolTipPolicyTestAccess::GetManagedToolTipCount() ==
                  baseline + 1);
        }
    }

    CHECK(wxWinUIToolTipPolicyTestAccess::GetManagedToolTipCount() == baseline);
}

#endif // wxUSE_TOOLTIPS

#endif // wxUSE_STATUSBAR

#if wxUSE_HYPERLINKCTRL

TEST_CASE("wxWinUI Hyperlink Create revalidates host and size callbacks",
          "[winui-chrome-feedback][hyperlink][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount();

    SECTION("shared-host attachment")
    {
        wxHyperlinkCtrl *link = new wxHyperlinkCtrl;
        wxHyperlinkCtrl * const invoking = link;
        bool attached = false;
        wxWinUIHostContentTestAccess::OnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxHyperlinkCtrl * const doomed = link;
                link = nullptr;
                delete doomed;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, "Doomed", "https://example.invalid");
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});

        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(link == nullptr);
        CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
              baseline);
        delete link;
    }

    SECTION("final-size callback")
    {
        wxHyperlinkCtrl *link = new wxHyperlinkCtrl;
        wxHyperlinkCtrl * const invoking = link;
        bool attached = false;
        bool sized = false;
        link->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent& event)
            {
                if ( !attached )
                {
                    event.Skip();
                    return;
                }

                sized = true;
                wxHyperlinkCtrl * const doomed = link;
                link = nullptr;
                delete doomed;
            });
        wxWinUIHostContentTestAccess::OnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window == invoking )
                    attached = true;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, "Sized", "https://example.invalid",
            wxDefaultPosition, wxDefaultSize);
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});

        CHECK(attached);
        CHECK(sized);
        CHECK_FALSE(created);
        CHECK(link == nullptr);
        CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
              baseline);
        delete link;
    }
}

TEST_CASE("wxWinUI Hyperlink owns alignment, appearance and refreshed UIA name",
          "[winui-chrome-feedback][hyperlink][appearance]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    struct ThemeRestorer
    {
        wxWinUIAppTheme saved = wxWinUIGetAppTheme();
        ~ThemeRestorer() { wxWinUISetAppTheme(saved); }
    } restoreTheme;
    wxWinUISetAppTheme(wxWinUIAppTheme::System);

    wxHyperlinkCtrl link;
    wxGenericHyperlinkCtrl genericDefaults;
    CHECK(link.GetHoverColour() == genericDefaults.GetHoverColour());
    CHECK(link.GetNormalColour() == genericDefaults.GetNormalColour());
    CHECK(link.GetVisitedColour() == genericDefaults.GetVisitedColour());
    REQUIRE(link.Create(
        parent, wxID_ANY, "wx&Widgets", "https://wxwidgets.org"));
    CHECK(link.GetHoverColour() == genericDefaults.GetHoverColour());
    CHECK(link.GetNormalColour() == genericDefaults.GetNormalColour());
    CHECK(link.GetVisitedColour() == genericDefaults.GetVisitedColour());
    CHECK(link.GetDefaultAttributes().colFg ==
          genericDefaults.GetDefaultAttributes().colFg);
    CHECK(wxHyperlinkCtrl::GetClassDefaultAttributes().colFg ==
          wxGenericHyperlinkCtrl::GetClassDefaultAttributes().colFg);

    bool pointerOver = true;
    int alignment = -1;
    bool context = false;
    wxColour effective;
    wxWinUIAppearanceSnapshot appearance;
    wxYield();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        &pointerOver, &alignment, &context, &effective, &appearance));
    CHECK_FALSE(pointerOver);
    CHECK(alignment ==
          static_cast<int>(MUX::HorizontalAlignment::Center));
#if wxUSE_MENUS
    CHECK(context);
#else
    CHECK_FALSE(context);
#endif
    CHECK(appearance.automationName == "wxWidgets");
    CHECK(appearance.accessKey.CmpNoCase("W") == 0);
    CHECK(appearance.hasForeground);
    CHECK(effective == link.GetNormalColour());

    bool peerEnabled = true;
    link.Enable(false);
    REQUIRE(WaitFor("disabled hyperlink host state", [&]()
    {
        return wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, nullptr, nullptr,
                   &peerEnabled) &&
               !peerEnabled;
    }));
    CHECK_FALSE(peerEnabled);
    link.Enable();
    REQUIRE(WaitFor("enabled hyperlink host state", [&]()
    {
        return wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, nullptr, nullptr,
                   &peerEnabled) &&
               peerEnabled;
    }));
    CHECK(peerEnabled);

    link.SetLabel("Updated &link");
    wxYield();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK(appearance.automationName == "Updated link");
    CHECK(appearance.accessKey.CmpNoCase("l") == 0);

    link.SetFont(MakeChromeTestFont());
    link.SetForegroundColour(wxColour(19, 83, 149));
    link.SetBackgroundColour(wxColour(41, 47, 53));
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CheckChromeFontLocals(appearance, true);
    CHECK(appearance.hasForeground);
    CHECK(appearance.hasBackground);

    link.SetFont(wxNullFont);
    link.SetForegroundColour(wxNullColour);
    link.SetBackgroundColour(wxNullColour);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CheckChromeFontLocals(appearance, false);
    CHECK(appearance.hasForeground);
    CHECK(effective == link.GetNormalColour());
    CHECK_FALSE(appearance.hasBackground);

    // wxNullColour restores the documented generic defaults instead of
    // leaving the getter at one value while XAML displays a theme resource
    // with unrelated colour semantics.
    link.SetHoverColour(wxColour(1, 2, 3));
    link.SetVisitedColour(wxColour(4, 5, 6));
    link.SetHoverColour(wxNullColour);
    link.SetNormalColour(wxNullColour);
    link.SetVisitedColour(wxNullColour);
    CHECK(link.GetHoverColour() == genericDefaults.GetHoverColour());
    CHECK(link.GetNormalColour() == genericDefaults.GetNormalColour());
    CHECK(link.GetVisitedColour() == genericDefaults.GetVisitedColour());

    struct ThemeProjectionContext
    {
        bool called = false;
    } themeContext;
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link,
        [](void *opaque)
        {
            static_cast<ThemeProjectionContext *>(opaque)->called = true;
        },
        &themeContext);
    wxSysColourChangedEvent themeChanged;
    themeChanged.SetEventObject(&link);
    link.ProcessWindowEvent(themeChanged);
    CHECK(themeContext.called);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == link.GetNormalColour());

    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    wxColour lightDefault;
    REQUIRE(WaitFor("light hyperlink default projection", [&]()
    {
        return wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, &lightDefault, nullptr) &&
               lightDefault == link.GetNormalColour();
    }));
    CHECK(lightDefault == wxColour(0x00, 0x00, 0xee));

    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    wxColour darkDefault;
    REQUIRE(WaitFor("dark hyperlink default projection", [&]()
    {
        return wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, &darkDefault, nullptr) &&
               darkDefault == link.GetNormalColour();
    }));
    CHECK(darkDefault == wxColour(0x87, 0xce, 0xfa));
    CHECK(darkDefault != lightDefault);
}

TEST_CASE("wxWinUI Hyperlink native hit area follows its rendered label",
          "[winui-chrome-feedback][hyperlink][hit-test][dpi][uia]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    struct AlignmentCase
    {
        long style;
        MUX::HorizontalAlignment alignment;
    };
    const AlignmentCase cases[] =
    {
        { wxHL_ALIGN_LEFT, MUX::HorizontalAlignment::Left },
        { wxHL_ALIGN_CENTRE, MUX::HorizontalAlignment::Center },
        { wxHL_ALIGN_RIGHT, MUX::HorizontalAlignment::Right }
    };

    for ( const AlignmentCase& current : cases )
    {
        INFO("alignment " << static_cast<int>(current.alignment));

        wxHyperlinkCtrl link;
        REQUIRE(link.Create(
            parent, wxID_ANY, "Link", "https://example.invalid",
            wxDefaultPosition, wxDefaultSize, current.style));

        // Express the requested client area in DIPs, but compare the exposed
        // hit rectangle in wx physical client pixels. This catches accidental
        // DIP/pixel mixing without relying on physical input injection.
        link.SetClientSize(parent->FromDIP(wxSize(480, 72)));

        wxRect interactive;
        REQUIRE(WaitFor("content-sized hyperlink hit area", [&]()
        {
            return wxWinUIHyperlinkTestAccess::GetInteractiveRect(link, &interactive) &&
                   interactive.width > 0 &&
                   interactive.width < link.GetClientSize().x / 2;
        }));

        wxWinUIAppearanceSnapshot appearance;
        REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
            nullptr, nullptr, nullptr, nullptr, &appearance));
        CHECK(appearance.automationName == "Link");
        CHECK(appearance.hasAutomationPeer);
        CHECK(appearance.automationControlType ==
              static_cast<int>(
                  MUX::Automation::Peers::AutomationControlType::Hyperlink));
        // HyperlinkButtonAutomationPeer intentionally exposes the UIA class
        // name "Hyperlink" together with AutomationControlType::Hyperlink.
        CHECK(appearance.automationClassName == "Hyperlink");
        CHECK(appearance.peerName == "Link");

        const wxSize client = link.GetClientSize();
        const int tolerance = wxMax(2, parent->FromDIP(2));
        CHECK(std::abs(2 * interactive.y + interactive.height -
                       client.y) <= 2 * tolerance);
        switch ( current.alignment )
        {
            case MUX::HorizontalAlignment::Left:
                CHECK(std::abs(interactive.x) <= tolerance);
                break;

            case MUX::HorizontalAlignment::Center:
                CHECK(std::abs(2 * interactive.x + interactive.width -
                               client.x) <= 2 * tolerance);
                break;

            case MUX::HorizontalAlignment::Right:
                CHECK(std::abs(interactive.GetRight() + 1 -
                               client.x) <= tolerance);
                break;

            default:
                FAIL("unexpected hyperlink alignment");
        }

        int events = 0;
        link.Bind(
            wxEVT_HYPERLINK,
            [&events](wxHyperlinkEvent&)
            {
                ++events;
            });

        wxPoint outside(0, client.y / 2);
        if ( interactive.Contains(outside) )
            outside.x = client.x - 1;
        REQUIRE_FALSE(interactive.Contains(outside));
        CHECK_FALSE(wxWinUIHyperlinkTestAccess::HitTest(link, outside));
        CHECK_FALSE(wxWinUIHyperlinkTestAccess::InvokeAt(link, outside));
        CHECK(events == 0);

        const wxPoint inside(interactive.x + interactive.width / 2,
                             interactive.y + interactive.height / 2);
        CHECK(wxWinUIHyperlinkTestAccess::HitTest(link, inside));
        REQUIRE(wxWinUIHyperlinkTestAccess::InvokeAt(link, inside));
        CHECK(events == 1);

        const int shortWidth = interactive.width;
        link.SetLabel("A considerably longer rendered hyperlink label");
        REQUIRE(WaitFor("updated hyperlink hit area", [&]()
        {
            return wxWinUIHyperlinkTestAccess::GetInteractiveRect(link, &interactive) &&
                   interactive.width > shortWidth;
        }));
    }
}

TEST_CASE("wxWinUI keeps wxGenericHyperlinkCtrl as a real generic control",
          "[winui-chrome-feedback][hyperlink][generic][hit-test][lifetime]")
{
    static_assert(
        !std::is_same<wxGenericHyperlinkCtrl, wxHyperlinkCtrl>::value,
        "the explicit generic hyperlink API must not alias the native peer");

    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    InspectableGenericHyperlinkCtrl link;
    REQUIRE(link.Create(
        parent, wxID_ANY, "Generic link", "https://generic.invalid",
        wxDefaultPosition, wxDefaultSize, wxHL_ALIGN_CENTRE));
    link.SetClientSize(parent->FromDIP(wxSize(360, 72)));

    const wxRect label = link.GetLabelRectForTesting();
    const wxSize client = link.GetClientSize();
    REQUIRE_FALSE(label.IsEmpty());
    CHECK(label.width < client.x);
    CHECK(std::abs(2 * label.x + label.width - client.x) <= 2);
    CHECK(std::abs(2 * label.y + label.height - client.y) <= 2);
    CHECK(link.GetURL() == "https://generic.invalid");

    int events = 0;
    link.Bind(
        wxEVT_HYPERLINK,
        [&events](wxHyperlinkEvent&)
        {
            ++events;
        });

    const auto clickAt =
        [&link](const wxPoint& point)
        {
            wxMouseEvent down(wxEVT_LEFT_DOWN);
            down.SetId(link.GetId());
            down.SetEventObject(&link);
            down.SetPosition(point);
            link.ProcessWindowEvent(down);

            wxMouseEvent up(wxEVT_LEFT_UP);
            up.SetId(link.GetId());
            up.SetEventObject(&link);
            up.SetPosition(point);
            link.ProcessWindowEvent(up);
        };

    wxPoint outside(0, client.y / 2);
    if ( label.Contains(outside) )
        outside.x = client.x - 1;
    REQUIRE_FALSE(label.Contains(outside));
    clickAt(outside);
    CHECK(events == 0);

    clickAt(wxPoint(label.x + label.width / 2,
                    label.y + label.height / 2));
    CHECK(events == 1);
    CHECK(link.GetVisited());
}

TEST_CASE("wxWinUI Hyperlink projects hover and visited visual states",
          "[winui-chrome-feedback][hyperlink][visual-state]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Link", "https://example.invalid",
        wxDefaultPosition, wxDefaultSize, wxHL_ALIGN_LEFT);
    link.SetNormalColour(wxColour(180, 20, 30));
    link.SetHoverColour(wxColour(20, 170, 40));
    link.SetVisitedColour(wxColour(60, 40, 190));

    wxColour effective;
    int alignment = -1;
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, &alignment, nullptr, &effective, nullptr));
    CHECK(alignment ==
          static_cast<int>(MUX::HorizontalAlignment::Left));
    CHECK(effective == wxColour(180, 20, 30));

    wxWinUIHyperlinkTestAccess::SetPointerOver(link, true);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == wxColour(20, 170, 40));

    wxWinUIHyperlinkTestAccess::SetPointerOver(link, false);
    link.SetVisited();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == wxColour(60, 40, 190));

    wxWinUIHyperlinkTestAccess::SetPointerOver(link, true);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == wxColour(20, 170, 40));

    wxGenericHyperlinkCtrl genericDefaults;
    link.SetHoverColour(wxNullColour);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(link.GetHoverColour() == genericDefaults.GetHoverColour());
    CHECK(effective == link.GetHoverColour());

    wxWinUIHyperlinkTestAccess::SetPointerOver(link, false);
    link.SetVisitedColour(wxNullColour);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(link.GetVisitedColour() == genericDefaults.GetVisitedColour());
    CHECK(effective == link.GetVisitedColour());

    link.SetVisited(false);
    link.SetNormalColour(wxNullColour);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(link.GetNormalColour() ==
          wxGenericHyperlinkCtrl::GetClassDefaultAttributes().colFg);
    CHECK(effective == link.GetNormalColour());
}

TEST_CASE("wxWinUI Hyperlink relinquishes only default colours in High "
          "Contrast",
          "[winui-chrome-feedback][hyperlink][appearance]"
          "[high-contrast][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    struct ThemeRestorer
    {
        wxWinUIAppTheme saved = wxWinUIGetAppTheme();
        ~ThemeRestorer() { wxWinUISetAppTheme(saved); }
    } restoreTheme;
    HighContrastOverrideRestorer highContrast(
        wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);

    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Link", "https://example.invalid");
    wxWinUIAppearanceSnapshot appearance;
    wxColour effective;
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == link.GetNormalColour());

    const auto notifySystemColourChange = [&]()
    {
        wxSysColourChangedEvent event;
        event.SetEventObject(&link);
        // OnSysColourChanged() deliberately calls Skip() so the normal wx
        // propagation contract is preserved. ProcessWindowEvent() may
        // consequently return false even though the projection ran.
        (void)link.ProcessWindowEvent(event);
    };

    highContrast.Set(
        wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOn);
    notifySystemColourChange();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);

    const wxColour customNormal(11, 73, 151);
    link.SetNormalColour(customNormal);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == customNormal);

    // A custom normal state must not keep a local brush alive when the active
    // hover state is still the generic default.
    wxWinUIHyperlinkTestAccess::SetPointerOver(link, true);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);

    const wxColour customHover(17, 139, 43);
    link.SetHoverColour(customHover);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == customHover);

    // Visited has independent provenance too.
    wxWinUIHyperlinkTestAccess::SetPointerOver(link, false);
    link.SetVisitedColour(wxNullColour);
    link.SetVisited();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);

    const wxColour customVisited(103, 47, 181);
    link.SetVisitedColour(customVisited);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == customVisited);

    // Leaving High Contrast republishes the concrete wx default. Getters stay
    // concrete in both modes; only local DependencyProperty ownership differs.
    highContrast.Set(
        wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff);
    link.SetVisitedColour(wxNullColour);
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(effective == link.GetVisitedColour());

    // Theme changes while High Contrast is active must not reintroduce a
    // local default brush.
    highContrast.Set(
        wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOn);
    link.SetVisited(false);
    link.SetNormalColour(wxNullColour);
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    wxYield();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);
    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    wxYield();
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);
}

TEST_CASE("wxWinUI Hyperlink URL publication resets visited atomically",
          "[winui-chrome-feedback][hyperlink][url][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxColour normal(17, 87, 151);
    const wxColour visited(91, 39, 143);
    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Link", "https://initial.invalid");
    link.SetNormalColour(normal);
    link.SetVisitedColour(visited);
    link.SetVisited();

    wxColour effective;
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == visited);

    link.SetURL("https://next.invalid");
    CHECK(link.GetURL() == "https://next.invalid");
    CHECK_FALSE(link.GetVisited());
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == normal);

    // MSW only resets the visited state when the URL actually changes.
    link.SetVisited();
    link.SetURL("https://next.invalid");
    CHECK(link.GetVisited());
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == visited);

    struct URLMutationContext
    {
        wxHyperlinkCtrl *link;
        bool called = false;
    } context{ &link };
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link,
        [](void *opaque)
        {
            auto * const current =
                static_cast<URLMutationContext *>(opaque);
            current->called = true;
            current->link->SetURL("https://inner.invalid");
            current->link->SetVisited();
        },
        &context);
    link.SetURL("https://outer.invalid");

    CHECK(context.called);
    CHECK(link.GetURL() == "https://inner.invalid");
    CHECK(link.GetVisited());
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, &effective, nullptr));
    CHECK(effective == visited);
}

TEST_CASE("wxWinUI Hyperlink peer projection is reentrant last-writer-wins",
          "[winui-chrome-feedback][hyperlink][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Initial &I", "https://example.invalid");
    struct MutationContext
    {
        wxHyperlinkCtrl *link;
        bool called = false;
    } context{ &link };

    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link,
        [](void *opaque)
        {
            MutationContext * const current =
                static_cast<MutationContext *>(opaque);
            current->called = true;
            current->link->SetLabel("Inner &N");
        },
        &context);
    link.SetLabel("Outer &O");

    CHECK(context.called);
    CHECK(link.GetLabel() == "Inner &N");
    wxWinUIAppearanceSnapshot appearance;
    REQUIRE(wxWinUIHyperlinkTestAccess::GetState(link,
        nullptr, nullptr, nullptr, nullptr, &appearance));
    CHECK(appearance.automationName == "Inner N");
    CHECK(appearance.accessKey.CmpNoCase("N") == 0);
}

TEST_CASE("wxWinUI Hyperlink defers and converges after its synchronous "
           "projection budget",
           "[winui-chrome-feedback][hyperlink][reentrancy][deferred]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Initial", "https://example.invalid");
    const unsigned long long revisionBefore =
        wxWinUIHyperlinkTestAccess::GetModelRevision(link);
    HyperlinkApplyStormContext context;
    context.link = &link;
    context.targetCalls = 12;
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link,
        &ContinueHyperlinkApplyStorm, &context);

    link.SetLabel("Begin deferred projection");

    CHECK(context.calls == 8);
    CHECK(wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(link));
    CHECK(wxWinUIHyperlinkTestAccess::GetModelRevision(link) >
          revisionBefore + 8);

    wxWinUIAppearanceSnapshot appearance;
    REQUIRE(WaitFor("deferred hyperlink projection", [&]()
    {
        return !wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(link) &&
               context.calls == context.targetCalls &&
               wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, nullptr, &appearance) &&
               appearance.automationName == "Deferred L 12" &&
               appearance.accessKey.CmpNoCase("L") == 0;
    }));
    CHECK(link.GetLabel() == context.latestLabel);
    CHECK(wxWinUIHyperlinkTestAccess::GetModelRevision(link) >=
          revisionBefore + 13);
}

TEST_CASE("wxWinUI Hyperlink quarantines an unbounded projection storm and "
          "a fresh mutation rearms it",
          "[winui-chrome-feedback][hyperlink][reentrancy][quarantine]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxHyperlinkCtrl link(
        parent, wxID_ANY, "Initial", "https://example.invalid");
    HyperlinkApplyStormContext context;
    context.link = &link;
    context.targetCalls = 1000000;
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link,
        &ContinueHyperlinkApplyStorm, &context);

    YieldOnceLogTarget yieldingLog;
    link.SetLabel("Begin bounded storm");

    // The overrun is reported as a debug diagnostic: whether it reaches this
    // log target depends on the build, so it is not required to yield.
    CHECK(context.calls == 8);
    CHECK(wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(link));
    REQUIRE(WaitFor("bounded hyperlink projection quarantine", [&]()
    {
        return context.calls == 16 &&
               !wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(link) &&
               wxWinUIHyperlinkTestAccess::IsPeerProjectionQuarantined(link);
    }));

    const int callsAtQuarantine = context.calls;
    wxYield();
    wxYield();
    CHECK(context.calls == callsAtQuarantine);

    // The storm deliberately leaves its next one-shot hook installed. Remove
    // it before proving that a genuinely new public mutation gets a fresh
    // budget and converges synchronously.
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(link, nullptr, nullptr);
    link.SetLabel("Recovered &R");
    CHECK_FALSE(wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(link));
    CHECK_FALSE(wxWinUIHyperlinkTestAccess::IsPeerProjectionQuarantined(link));

    // The control peer converges synchronously, while the shared slot owns
    // AutomationProperties.Name and publishes it through its coalesced host
    // pass. Wait for that independent adapter without weakening the fresh
    // mutation/quarantine assertions above.
    wxWinUIAppearanceSnapshot appearance;
    REQUIRE(WaitFor("rearmed hyperlink automation name", [&]()
    {
        return wxWinUIHyperlinkTestAccess::GetState(link,
                   nullptr, nullptr, nullptr, nullptr, &appearance) &&
               appearance.automationName == "Recovered R" &&
               appearance.accessKey.CmpNoCase("R") == 0;
    }));
    CHECK(appearance.automationName == "Recovered R");
    CHECK(appearance.accessKey.CmpNoCase("R") == 0);
}

TEST_CASE("wxWinUI Hyperlink deferred projection may destroy its owner",
          "[winui-chrome-feedback][hyperlink][reentrancy][deferred]"
          "[lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount();

    std::unique_ptr<wxHyperlinkCtrl> owned(new wxHyperlinkCtrl);
    REQUIRE(owned->Create(
        parent, wxID_ANY, "Deferred delete",
        "https://example.invalid"));
    wxHyperlinkCtrl *observed = owned.get();
    HyperlinkApplyStormContext context;
    context.link = observed;
    context.targetCalls = 20;
    context.destroyAtCall = 9;
    context.owned = &owned;
    context.observed = &observed;
    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(*observed,
        &ContinueHyperlinkApplyStorm, &context);

    observed->SetLabel("Begin deferred delete");

    REQUIRE(observed);
    CHECK(context.calls == 8);
    CHECK(wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(*observed));
    REQUIRE(WaitFor("deferred hyperlink destruction", [&]()
    {
        return observed == nullptr;
    }));
    CHECK(context.calls == context.destroyAtCall);
    CHECK_FALSE(owned);
    CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

TEST_CASE("wxWinUI Hyperlink peer write may destroy its owner",
          "[winui-chrome-feedback][hyperlink][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount();

    std::unique_ptr<wxHyperlinkCtrl> owned(new wxHyperlinkCtrl);
    REQUIRE(owned->Create(
        parent, wxID_ANY, "Delete during apply",
        "https://example.invalid"));
    wxHyperlinkCtrl *link = owned.get();
    struct DestructionContext
    {
        std::unique_ptr<wxHyperlinkCtrl> *owned;
        wxHyperlinkCtrl **link;
        bool called = false;
    } context{ &owned, &link };

    wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(*link,
        [](void *opaque)
        {
            DestructionContext * const current =
                static_cast<DestructionContext *>(opaque);
            current->called = true;
            wxHyperlinkCtrl * const dying = current->owned->release();
            *current->link = nullptr;
            delete dying;
        },
        &context);

    wxHyperlinkCtrl * const invoking = link;
    invoking->SetURL("https://outer.invalid");
    CHECK(context.called);
    CHECK(link == nullptr);
    CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

TEST_CASE("wxWinUI Hyperlink click may destroy its owner safely",
          "[winui-chrome-feedback][hyperlink][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount();

    std::unique_ptr<wxHyperlinkCtrl> owned(new wxHyperlinkCtrl);
    REQUIRE(owned->Create(
        parent, wxID_ANY, "Delete me", "https://example.invalid"));
    wxHyperlinkCtrl *link = owned.get();
    CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
          baseline + 1);

    bool handled = false;
    link->Bind(
        wxEVT_HYPERLINK,
        [&owned, &link, &handled](wxHyperlinkEvent& event)
        {
            handled = true;
            // Exercise the dangerous base-path shape too: an unhandled event
            // would normally fall through to browser launch after deletion.
            event.Skip();
            wxHyperlinkCtrl * const dying = owned.release();
            link = nullptr;
            delete dying;
        });

    wxHyperlinkCtrl * const invoking = link;
    REQUIRE(wxWinUIHyperlinkTestAccess::Invoke(*invoking));
    CHECK(handled);
    CHECK(link == nullptr);
    CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

#endif // wxUSE_HYPERLINKCTRL

#if wxUSE_ACTIVITYINDICATOR

TEST_CASE("wxWinUI ActivityIndicator Create revalidates host and size "
          "callbacks",
          "[winui-chrome-feedback][activity][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("shared-host attachment")
    {
        wxActivityIndicator *activity = new wxActivityIndicator;
        wxActivityIndicator * const invoking = activity;
        bool attached = false;
        wxWinUIHostContentTestAccess::OnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;

                attached = true;
                wxActivityIndicator * const doomed = activity;
                activity = nullptr;
                delete doomed;
            });

        const bool created = invoking->Create(parent);
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});

        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(activity == nullptr);
        delete activity;
    }

    SECTION("final-size callback")
    {
        wxActivityIndicator *activity = new wxActivityIndicator;
        wxActivityIndicator * const invoking = activity;
        bool attached = false;
        bool sized = false;
        activity->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent& event)
            {
                if ( !attached )
                {
                    event.Skip();
                    return;
                }

                sized = true;
                wxActivityIndicator * const doomed = activity;
                activity = nullptr;
                delete doomed;
            });
        wxWinUIHostContentTestAccess::OnNextSlotAttached(
            [&](wxWindow *window)
            {
                if ( window == invoking )
                    attached = true;
            });

        const bool created = invoking->Create(parent);
        wxWinUIHostContentTestAccess::OnNextSlotAttached({});

        CHECK(attached);
        CHECK(sized);
        CHECK_FALSE(created);
        CHECK(activity == nullptr);
        delete activity;
    }
}

TEST_CASE("wxWinUI ActivityIndicator state and UIA remain synchronized",
          "[winui-chrome-feedback][activity]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxActivityIndicator activity;
    REQUIRE(activity.Create(parent));

    bool active = true;
    bool tabStop = true;
    wxString idleStatus;
    wxWinUIAppearanceSnapshot appearance;
    wxYield();
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        &active, &tabStop, &idleStatus, &appearance));
    CHECK_FALSE(activity.IsRunning());
    CHECK_FALSE(active);
    CHECK_FALSE(tabStop);
    CHECK_FALSE(idleStatus.empty());
    CHECK_FALSE(appearance.hasForeground);
    CHECK_FALSE(appearance.hasBackground);
    // The default technical wx name is filtered by the common slot adapter.
    CHECK(appearance.automationName.empty());
    CHECK_FALSE(appearance.localizedControlType.empty());
    CHECK(activity.GetBestSize() ==
          parent->FromDIP(wxSize(40, 40)));

    activity.Hide();
    wxYield();
    CHECK_FALSE(activity.IsShown());
    activity.Show();
    wxYield();
    CHECK(activity.IsShown());

    activity.Start();
    wxString busyStatus;
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        &active, nullptr, &busyStatus, nullptr));
    CHECK(activity.IsRunning());
    CHECK(active);
    CHECK_FALSE(busyStatus.empty());
    CHECK(busyStatus != idleStatus);

    activity.Start();
    CHECK(activity.IsRunning());
    activity.Stop();
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        &active, nullptr, nullptr, nullptr));
    CHECK_FALSE(activity.IsRunning());
    CHECK_FALSE(active);
    activity.Stop();

    activity.SetForegroundColour(wxColour(21, 91, 151));
    activity.SetBackgroundColour(wxColour(42, 48, 54));
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        nullptr, nullptr, nullptr, &appearance));
    CHECK(appearance.hasForeground);
    CHECK(appearance.hasBackground);
    activity.SetForegroundColour(wxNullColour);
    activity.SetBackgroundColour(wxNullColour);
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        nullptr, nullptr, nullptr, &appearance));
    CHECK_FALSE(appearance.hasForeground);
    CHECK_FALSE(appearance.hasBackground);

    wxActivityIndicator named;
    REQUIRE(named.Create(
        parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0,
        "network activity"));
    wxYield();
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(named,
        nullptr, nullptr, nullptr, &appearance));
    // The Create() name is a technical wx resource identifier, just as on
    // classic MSW; only a visible/semantic label feeds UIA.
    CHECK(appearance.automationName.empty());
    named.SetLabel("network activity");
    wxYield();
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(named,
        nullptr, nullptr, nullptr, &appearance));
    CHECK(appearance.automationName == "network activity");
}

TEST_CASE("wxWinUI ActivityIndicator peer projection is reentrant "
          "last-writer-wins",
          "[winui-chrome-feedback][activity][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxActivityIndicator activity(parent);
    wxString idleStatus;
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        nullptr, nullptr, &idleStatus, nullptr));

    struct MutationContext
    {
        wxActivityIndicator *activity;
        bool called = false;
    } context{ &activity };
    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(activity,
        [](void *opaque)
        {
            MutationContext * const current =
                static_cast<MutationContext *>(opaque);
            current->called = true;
            current->activity->Stop();
        },
        &context);

    activity.Start();
    CHECK(context.called);
    CHECK_FALSE(activity.IsRunning());
    bool peerActive = true;
    wxString itemStatus;
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        &peerActive, nullptr, &itemStatus, nullptr));
    CHECK_FALSE(peerActive);
    CHECK(itemStatus == idleStatus);
}

TEST_CASE("wxWinUI ActivityIndicator defers and converges after its "
           "synchronous projection budget",
           "[winui-chrome-feedback][activity][reentrancy][deferred]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxActivityIndicator activity(parent);
    activity.Start();
    wxString busyStatus;
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        nullptr, nullptr, &busyStatus, nullptr));
    activity.Stop();

    const unsigned long long revisionBefore =
        wxWinUIActivityIndicatorTestAccess::GetModelRevision(activity);
    ActivityApplyStormContext context;
    context.activity = &activity;
    context.targetCalls = 12;
    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(activity,
        &ContinueActivityApplyStorm, &context);

    YieldOnceLogTarget yieldingLog;
    activity.Start();

    // The overrun is reported as a debug diagnostic: whether it reaches this
    // log target depends on the build, so it is not required to yield.
    CHECK(context.calls == 8);
    CHECK(wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(activity));
    CHECK(wxWinUIActivityIndicatorTestAccess::GetModelRevision(activity) >
          revisionBefore + 8);

    bool peerActive = false;
    wxString itemStatus;
    REQUIRE(WaitFor("deferred activity-indicator projection", [&]()
    {
        return !wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(activity) &&
               context.calls == context.targetCalls &&
               wxWinUIActivityIndicatorTestAccess::GetState(activity,
                   &peerActive, nullptr, &itemStatus, nullptr) &&
               peerActive && itemStatus == busyStatus;
    }));
    CHECK(activity.IsRunning());
    CHECK(wxWinUIActivityIndicatorTestAccess::GetModelRevision(activity) >=
          revisionBefore + 13);
}

TEST_CASE("wxWinUI ActivityIndicator quarantines an unbounded projection "
          "storm and a fresh mutation rearms it",
          "[winui-chrome-feedback][activity][reentrancy][quarantine]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxActivityIndicator activity(parent);
    ActivityApplyStormContext context;
    context.activity = &activity;
    context.targetCalls = 1000000;
    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(activity,
        &ContinueActivityApplyStorm, &context);

    activity.Start();

    CHECK(context.calls == 8);
    CHECK(wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(activity));
    REQUIRE(WaitFor("bounded activity projection quarantine", [&]()
    {
        return context.calls == 16 &&
               !wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(activity) &&
               wxWinUIActivityIndicatorTestAccess::IsPeerProjectionQuarantined(activity);
    }));

    const int callsAtQuarantine = context.calls;
    wxYield();
    wxYield();
    CHECK(context.calls == callsAtQuarantine);

    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(activity, nullptr, nullptr);
    activity.Stop();
    CHECK_FALSE(wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(activity));
    CHECK_FALSE(wxWinUIActivityIndicatorTestAccess::IsPeerProjectionQuarantined(activity));

    bool peerActive = true;
    REQUIRE(wxWinUIActivityIndicatorTestAccess::GetState(activity,
        &peerActive, nullptr, nullptr, nullptr));
    CHECK_FALSE(activity.IsRunning());
    CHECK_FALSE(peerActive);
}

TEST_CASE("wxWinUI ActivityIndicator deferred projection may destroy "
           "its owner",
          "[winui-chrome-feedback][activity][reentrancy][deferred]"
          "[lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    std::unique_ptr<wxActivityIndicator> owned(
        new wxActivityIndicator);
    REQUIRE(owned->Create(parent));
    wxActivityIndicator *observed = owned.get();
    ActivityApplyStormContext context;
    context.activity = observed;
    context.targetCalls = 20;
    context.destroyAtCall = 9;
    context.owned = &owned;
    context.observed = &observed;
    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(*observed,
        &ContinueActivityApplyStorm, &context);

    observed->Start();

    REQUIRE(observed);
    CHECK(context.calls == 8);
    CHECK(wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(*observed));
    REQUIRE(WaitFor("deferred activity-indicator destruction", [&]()
    {
        return observed == nullptr;
    }));
    CHECK(context.calls == context.destroyAtCall);
    CHECK_FALSE(owned);
}

TEST_CASE("wxWinUI ActivityIndicator peer write may destroy its owner",
          "[winui-chrome-feedback][activity][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    std::unique_ptr<wxActivityIndicator> owned(
        new wxActivityIndicator);
    REQUIRE(owned->Create(parent));
    wxActivityIndicator *activity = owned.get();
    struct DestructionContext
    {
        std::unique_ptr<wxActivityIndicator> *owned;
        wxActivityIndicator **activity;
        bool called = false;
    } context{ &owned, &activity };

    wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(*activity,
        [](void *opaque)
        {
            DestructionContext * const current =
                static_cast<DestructionContext *>(opaque);
            current->called = true;
            wxActivityIndicator * const dying =
                current->owned->release();
            *current->activity = nullptr;
            delete dying;
        },
        &context);

    wxActivityIndicator * const invoking = activity;
    invoking->Start();
    CHECK(context.called);
    CHECK(activity == nullptr);
}

#endif // wxUSE_ACTIVITYINDICATOR

#if wxUSE_STATUSBAR && wxUSE_HYPERLINKCTRL && wxUSE_ACTIVITYINDICATOR

TEST_CASE("wxWinUI chrome feedback controls survive repeated active teardown",
          "[winui-chrome-feedback][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const unsigned baseline =
        wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount();

    for ( int i = 0; i < 100; ++i )
    {
        wxStatusBar status;
        REQUIRE(status.Create(
            parent, wxID_ANY,
            wxSTB_SHOW_TIPS |
                (i % 2 ? wxSTB_ELLIPSIZE_START
                       : wxSTB_ELLIPSIZE_MIDDLE)));
        status.SetClientSize(parent->FromDIP(wxSize(180, 28)));
        status.SetStatusText(
            wxString::Format("iteration %d with a long message", i));

        wxHyperlinkCtrl link;
        REQUIRE(link.Create(
            parent, wxID_ANY, "&Link", "https://example.invalid",
            wxDefaultPosition, wxDefaultSize,
            i % 2 ? wxHL_ALIGN_LEFT : wxHL_ALIGN_RIGHT));
        link.SetVisited(i % 3 == 0);
        wxWinUIHyperlinkTestAccess::SetPointerOver(link, i % 4 == 0);

        wxActivityIndicator activity;
        REQUIRE(activity.Create(parent));
        activity.Start();
    }

    CHECK(wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

#endif

#endif // __WXWINUI__ && wxUSE_WINUI3
