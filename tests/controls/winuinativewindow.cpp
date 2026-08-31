///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuinativewindow.cpp
// Purpose:     deterministic WinUI wxNativeWindow hybrid qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/nativewin.h"

#if defined(wxHAS_NATIVE_WINDOW) && wxUSE_BUTTON && wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/cursor.h"
#include "wx/msw/private.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/dataobj.h"
    #include "wx/dnd.h"
    #include "wx/msw/private/dropsession.h"
    #include "wx/winui/private/dropbroker.h"
#endif

#include <UIAutomation.h>
#include <UIAutomationCoreApi.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace
{

constexpr wchar_t NativeProbeClassName[] =
    L"wxWidgets.WinUI.NativeWindow.Probe";
constexpr UINT NativeHandledMessage = WM_APP + 0x541;
constexpr UINT NativePassThroughMessage = WM_APP + 0x542;
constexpr LRESULT NativeHandledResult = 0x1541;
constexpr LRESULT NativePassThroughResult = 0x1542;

void DrainDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch(1);
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 60; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        const unsigned callbacks =
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() &&
             callbacks ==
                 wxWinUITopLevelHost::GetFlushCallbackAttemptCount() )
        {
            return true;
        }
    }

    return false;
}

bool SameTransientRegistry(const wxWinUITransientRegistrySnapshot& lhs,
                           const wxWinUITransientRegistrySnapshot& rhs)
{
    // The two retirement counters are cumulative diagnostics. Only the live
    // registry state is a teardown postcondition.
    return lhs.ownerCount == rhs.ownerCount &&
           lhs.transientCount == rhs.transientCount &&
           lhs.cancelCallbackCount == rhs.cancelCallbackCount &&
           lhs.modalCount == rhs.modalCount &&
           lhs.popupCount == rhs.popupCount &&
           lhs.teachingTipCount == rhs.teachingTipCount &&
           lhs.pendingOwnerRetireCount == rhs.pendingOwnerRetireCount &&
           lhs.activeOwnerRetirementCount ==
               rhs.activeOwnerRetirementCount;
}

struct RuntimeSnapshot
{
    static RuntimeSnapshot Capture()
    {
        RuntimeSnapshot snapshot;
        snapshot.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        snapshot.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        snapshot.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        snapshot.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        snapshot.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        snapshot.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        snapshot.automationObservers =
            wxWinUITopLevelHost::
                GetLiveAutomationNameStyleObserverCountForTest();
        snapshot.contentObservers =
            wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
        snapshot.invisibleShellProviders =
            wxWinUITestGetLiveInvisibleShellProviderCount();
        snapshot.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        snapshot.slotHandlerAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount();
        snapshot.slotHandlerRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        snapshot.rootHandlerAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount();
        snapshot.rootHandlerRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        snapshot.transients =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        snapshot.wxCapture = wxWindow::GetCapture();
        snapshot.nativeCapture = ::GetCapture();
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        snapshot.dropMutationGeneration =
            wxMSWOleGetDropTargetMutationGeneration();
#endif
        return snapshot;
    }

    bool IsRestored() const
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hosts &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   hostLifetimes &&
               wxWinUITopLevelHost::GetLiveSlotCount() == slots &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   slotLifetimes &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclasses &&
               wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks &&
               wxWinUITopLevelHost::
                       GetLiveAutomationNameStyleObserverCountForTest() ==
                   automationObservers &&
               wxWinUITopLevelHost::
                       GetLiveContentLoadedObserverCountForTest() ==
                   contentObservers &&
               wxWinUITestGetLiveInvisibleShellProviderCount() ==
                   invisibleShellProviders &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() -
                       slotHandlerAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotHandlerRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() -
                       rootHandlerAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootHandlerRevokes &&
               SameTransientRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   transients) &&
               wxWindow::GetCapture() == wxCapture &&
               ::GetCapture() == nativeCapture;
    }

    void CheckRestored(bool requireHostTraffic = true) const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::
                  GetLiveAutomationNameStyleObserverCountForTest() ==
              automationObservers);
        CHECK(wxWinUITopLevelHost::
                  GetLiveContentLoadedObserverCountForTest() ==
              contentObservers);
        CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
              invisibleShellProviders);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;
        if ( requireHostTraffic )
        {
            CHECK(slotAdds > 0);
            CHECK(rootAdds > 0);
        }
        CHECK(slotAdds == slotRevokes);
        CHECK(rootAdds == rootRevokes);

        CHECK(SameTransientRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), transients));
        CHECK(wxWindow::GetCapture() == wxCapture);
        CHECK(::GetCapture() == nativeCapture);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        CHECK(wxMSWOleGetDropTargetMutationGeneration() >=
              dropMutationGeneration);
#endif
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned automationObservers = 0;
    unsigned contentObservers = 0;
    unsigned invisibleShellProviders = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
    wxWinUITransientRegistrySnapshot transients;
    wxWindow *wxCapture = nullptr;
    HWND nativeCapture = nullptr;
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    std::uint64_t dropMutationGeneration = 0;
#endif
};

struct NativeState
{
    HWND hwnd = nullptr;
    unsigned passThroughMessages = 0;
    unsigned destroyMessages = 0;
    unsigned ncDestroyMessages = 0;
};

LRESULT CALLBACK NativeProbeWindowProc(HWND hwnd,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
    NativeState *state = reinterpret_cast<NativeState *>(
        ::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if ( message == WM_NCCREATE )
    {
        const CREATESTRUCTW * const create =
            reinterpret_cast<const CREATESTRUCTW *>(lParam);
        state = static_cast<NativeState *>(create->lpCreateParams);
        ::SetWindowLongPtrW(
            hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
        if ( state )
            state->hwnd = hwnd;
    }

    if ( state )
    {
        if ( message == NativePassThroughMessage )
        {
            ++state->passThroughMessages;
            return NativePassThroughResult;
        }
        if ( message == WM_DESTROY )
            ++state->destroyMessages;
    }

    const LRESULT result =
        ::DefWindowProcW(hwnd, message, wParam, lParam);

    if ( state && message == WM_NCDESTROY )
    {
        ++state->ncDestroyMessages;
        state->hwnd = nullptr;
        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
    }

    return result;
}

bool EnsureNativeProbeClass()
{
    static const bool registered = []()
    {
        WNDCLASSEXW windowClass = {};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = NativeProbeWindowProc;
        windowClass.hInstance = wxGetInstance();
        windowClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground =
            reinterpret_cast<HBRUSH>(
                static_cast<INT_PTR>(COLOR_WINDOW + 1));
        windowClass.lpszClassName = NativeProbeClassName;

        if ( ::RegisterClassExW(&windowClass) )
            return true;

        return ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }();

    return registered;
}

HWND CreateNativeProbe(wxWindow *parent,
                       int id,
                       NativeState *state,
                       const wxRect& rect = wxRect(24, 24, 180, 64))
{
    if ( !parent || !state || !EnsureNativeProbeClass() )
        return nullptr;

    return ::CreateWindowExW(
        0,
        NativeProbeClassName,
        L"native probe",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS,
        rect.x, rect.y, rect.width, rect.height,
        GetHwndOf(parent),
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        wxGetInstance(), state);
}

// SetWindowRgn() takes ownership only on success. Keep the test's failure path
// leak-free without obscuring the actual region/topology assertion.
bool InstallWindowRegion(HWND hwnd, HRGN region)
{
    if ( ::SetWindowRgn(hwnd, region, TRUE) )
        return true;

    if ( region )
        ::DeleteObject(region);
    return false;
}

class NativeFocusRestoreGuard final
{
public:
    NativeFocusRestoreGuard()
        : m_hwnd(::GetFocus()),
          m_generation(m_hwnd
              ? wxWinUIMSWGetNativeHwndGeneration(
                    reinterpret_cast<WXHWND>(m_hwnd))
              : 0)
    {
    }

    ~NativeFocusRestoreGuard()
    {
        Restore();
    }

    void Restore()
    {
        if ( m_restored )
            return;

        HWND target = nullptr;
        if ( m_hwnd && m_generation &&
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(m_hwnd)) == m_generation )
        {
            target = m_hwnd;
        }
        ::SetFocus(target);
        m_restored = true;
    }

private:
    HWND m_hwnd;
    unsigned long long m_generation;
    bool m_restored = false;

    wxDECLARE_NO_COPY_CLASS(NativeFocusRestoreGuard);
};

class NativeCursorRestoreGuard final
{
public:
    NativeCursorRestoreGuard() : m_cursor(::GetCursor()) {}

    ~NativeCursorRestoreGuard()
    {
        Restore();
    }

    void Restore()
    {
        if ( !m_restored )
        {
            ::SetCursor(m_cursor);
            m_restored = true;
        }
    }

private:
    HCURSOR m_cursor;
    bool m_restored = false;

    wxDECLARE_NO_COPY_CLASS(NativeCursorRestoreGuard);
};

struct RouteState
{
    unsigned handledMessages = 0;
    unsigned passThroughMessages = 0;
    std::function<void (class ProbeNativeWindow *)> action;
};

class ProbeNativeWindow final : public wxNativeWindow
{
public:
    explicit ProbeNativeWindow(const std::shared_ptr<RouteState>& route)
        : m_route(route)
    {
    }

protected:
    WXLRESULT MSWWindowProc(WXUINT message,
                            WXWPARAM wParam,
                            WXLPARAM lParam) override
    {
        // Hold callback state independently of this object. An action is
        // deliberately allowed to delete the wrapper or its external HWND.
        const std::shared_ptr<RouteState> route = m_route;
        if ( message == NativeHandledMessage )
        {
            ++route->handledMessages;
            const auto action = route->action;
            if ( action )
                action(this);
            return NativeHandledResult;
        }
        if ( message == NativePassThroughMessage )
            ++route->passThroughMessages;

        return wxNativeWindow::MSWWindowProc(message, wParam, lParam);
    }

private:
    std::shared_ptr<RouteState> m_route;
};

// Test failures may unwind from any REQUIRE below. Keep every external HWND
// and its wrapper terminal while the stack-backed NativeState still exists.
// WM_NCDESTROY clears NativeState::hwnd, so the exact state/user-data checks
// also reject a recycled HWND value. Normal paths null the wrapper and destroy
// their HWNDs explicitly; the guard is then a strict no-op.
class NativeLifetimeGuard final
{
public:
    explicit NativeLifetimeGuard(ProbeNativeWindow *&wrapper)
        : m_wrapper(wrapper)
    {
    }

    ~NativeLifetimeGuard()
    {
        if ( m_wrapper )
        {
            delete m_wrapper;
            m_wrapper = nullptr;
        }

        for ( const TrackedHandle& handle : m_handles )
        {
            if ( handle.state->hwnd == handle.hwnd &&
                 ::IsWindow(handle.hwnd) &&
                 reinterpret_cast<NativeState *>(::GetWindowLongPtrW(
                     handle.hwnd, GWLP_USERDATA)) == handle.state )
            {
                ::DestroyWindow(handle.hwnd);
            }
        }
    }

    void Track(HWND hwnd, NativeState *state)
    {
        if ( hwnd && state )
            m_handles.push_back({hwnd, state});
    }

private:
    struct TrackedHandle
    {
        HWND hwnd;
        NativeState *state;
    };

    ProbeNativeWindow *&m_wrapper;
    std::vector<TrackedHandle> m_handles;

    wxDECLARE_NO_COPY_CLASS(NativeLifetimeGuard);
};

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
class NativeDropTarget final : public wxTextDropTarget
{
public:
    wxDragResult OnEnter(wxCoord x,
                         wxCoord y,
                         wxDragResult def) override
    {
        ++enters;
        lastX = x;
        lastY = y;
        return def;
    }

    wxDragResult OnDragOver(wxCoord x,
                            wxCoord y,
                            wxDragResult def) override
    {
        ++overs;
        lastX = x;
        lastY = y;
        return def;
    }

    bool OnDropText(wxCoord x,
                    wxCoord y,
                    const wxString& value) override
    {
        ++drops;
        lastX = x;
        lastY = y;
        text = value;
        return true;
    }

    unsigned enters = 0;
    unsigned overs = 0;
    unsigned drops = 0;
    int lastX = 0;
    int lastY = 0;
    wxString text;
};

POINTL NativeCenterInScreen(HWND hwnd)
{
    RECT rect = {};
    REQUIRE(::GetWindowRect(hwnd, &rect));
    return {
        rect.left + (rect.right - rect.left) / 2,
        rect.top + (rect.bottom - rect.top) / 2
    };
}

void CheckTextDropRoute(IDropTarget *comTarget,
                        HWND hwnd,
                        NativeDropTarget *target,
                        const wxString& payload,
                        unsigned expectedCount)
{
    REQUIRE(comTarget != nullptr);
    REQUIRE(target != nullptr);
    const POINTL point = NativeCenterInScreen(hwnd);
    wxTextDataObject source(payload);

    DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(comTarget->DragEnter(
                source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(target->enters == expectedCount);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(comTarget->DragOver(MK_SHIFT, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);
    CHECK(target->overs == expectedCount);

    effect = DROPEFFECT_COPY;
    REQUIRE(comTarget->Drop(
                source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(target->drops == expectedCount);
    CHECK(target->text == payload);

    POINT expected = { point.x, point.y };
    ::SetLastError(ERROR_SUCCESS);
    const int mapped =
        ::MapWindowPoints(HWND_DESKTOP, hwnd, &expected, 1);
    REQUIRE((mapped != 0 || ::GetLastError() == ERROR_SUCCESS));
    CHECK(target->lastX == expected.x);
    CHECK(target->lastY == expected.y);
}
#endif

bool ShowOffscreenWithoutActivation(wxFrame *frame, unsigned ordinal)
{
    const HWND hwnd = frame ? static_cast<HWND>(frame->GetHWND()) : nullptr;
    if ( !hwnd )
        return false;

    const int left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const RECT desktop = {
        left,
        top,
        left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
        top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN)
    };
    frame->Move(wxPoint(left - 6000 - static_cast<int>(ordinal) * 400,
                        top - 6000));

    const auto isOutsideDesktop = [hwnd, &desktop]()
    {
        RECT windowRect = {};
        RECT intersection = {};
        return ::GetWindowRect(hwnd, &windowRect) &&
               !::IntersectRect(&intersection, &windowRect, &desktop);
    };
    if ( !isOutsideDesktop() )
        return false;

    frame->ShowWithoutActivating();
    if ( !isOutsideDesktop() )
    {
        frame->Hide();
        return false;
    }

    return true;
}

class TwoTLWFixture final
{
public:
    bool Create()
    {
        frameA = new wxFrame(nullptr, wxID_ANY, "native-window-a",
                             wxDefaultPosition, wxSize(440, 260),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        frameB = new wxFrame(nullptr, wxID_ANY, "native-window-b",
                             wxDefaultPosition, wxSize(440, 260),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        panelA = new wxPanel(frameA, wxID_ANY,
                             wxPoint(0, 0), wxSize(420, 220));
        panelB = new wxPanel(frameB, wxID_ANY,
                             wxPoint(0, 0), wxSize(420, 220));

        buttonA = new wxButton(panelA, wxID_ANY, "xaml-a",
                               wxPoint(250, 24), wxSize(130, 48));
        buttonB = new wxButton(panelB, wxID_ANY, "xaml-b",
                               wxPoint(250, 24), wxSize(130, 48));
        textA = new wxTextCtrl(panelA, wxID_ANY, "text-a",
                               wxPoint(24, 120), wxSize(180, 36));
        textB = new wxTextCtrl(panelB, wxID_ANY, "text-b",
                               wxPoint(24, 120), wxSize(180, 36));

        if ( !ShowOffscreenWithoutActivation(frameA, 0) ||
             !ShowOffscreenWithoutActivation(frameB, 1) ||
             !DrainToQuiescence() )
        {
            Destroy();
            return false;
        }

        hostA = wxWinUITopLevelHost::FindSlotOwner(buttonA);
        hostB = wxWinUITopLevelHost::FindSlotOwner(buttonB);
        if ( !hostA || !hostB || hostA == hostB ||
             wxWinUITopLevelHost::FindSlotOwner(textA) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(textB) != hostB )
        {
            Destroy();
            return false;
        }
        hostA->FlushSync();
        hostB->FlushSync();
        return true;
    }

    ~TwoTLWFixture()
    {
        Destroy();
    }

    void Destroy()
    {
        // All wxNativeWindow wrappers are explicit test-owned objects and are
        // removed before this point. Direct TLW deletion avoids depending on
        // a second idle pass while Catch itself is running from idle.
        if ( frameA )
        {
            delete frameA;
            frameA = nullptr;
        }
        if ( frameB )
        {
            delete frameB;
            frameB = nullptr;
        }

        panelA = nullptr;
        panelB = nullptr;
        buttonA = nullptr;
        buttonB = nullptr;
        textA = nullptr;
        textB = nullptr;
        hostA = nullptr;
        hostB = nullptr;
        DrainDispatch();
    }

    wxFrame *frameA = nullptr;
    wxFrame *frameB = nullptr;
    wxPanel *panelA = nullptr;
    wxPanel *panelB = nullptr;
    wxButton *buttonA = nullptr;
    wxButton *buttonB = nullptr;
    wxTextCtrl *textA = nullptr;
    wxTextCtrl *textB = nullptr;
    wxWinUITopLevelHost *hostA = nullptr;
    wxWinUITopLevelHost *hostB = nullptr;
};

ProbeNativeWindow *WrapNative(wxWindow *parent,
                              int id,
                              HWND hwnd,
                              const std::shared_ptr<RouteState>& route)
{
    ProbeNativeWindow * const wrapper = new ProbeNativeWindow(route);
    if ( !wrapper->Create(parent, id, hwnd) )
    {
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void CheckWrappedIdentity(ProbeNativeWindow *wrapper,
                          HWND hwnd,
                          LONG_PTR originalWindowProc)
{
    REQUIRE(wrapper != nullptr);
    REQUIRE(hwnd != nullptr);
    CHECK(wrapper->GetHWND() == hwnd);
    CHECK(wxFindWinFromHandle(hwnd) == wrapper);
    CHECK(wxWinUIMSWGetHwndGeneration(
              wrapper, reinterpret_cast<WXHWND>(hwnd)) != 0);
    CHECK(reinterpret_cast<LONG_PTR>(wrapper->MSWGetOldWndProc()) ==
          originalWindowProc);
    CHECK(::GetWindowLongPtrW(hwnd, GWLP_WNDPROC) != originalWindowProc);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(wrapper) == nullptr);
    CHECK_FALSE(wxWinUITLWHostIsInvisibleAccessibilityShell(wrapper));
}

void CheckMixedTabRoundTrip(ProbeNativeWindow *wrapper,
                            wxWinUITopLevelHost *host)
{
    REQUIRE(wrapper != nullptr);
    REQUIRE(host != nullptr);
    const HWND native = static_cast<HWND>(wrapper->GetHWND());
    const HWND tlw = static_cast<HWND>(host->GetTLW()->GetHWND());
    REQUIRE(native != nullptr);
    REQUIRE(tlw != nullptr);

    // Discover the adjacent XAML tab stop in the actual USER32 dialog order.
    // One of the two directions must cross the native/XAML boundary; using
    // that observed order keeps the oracle independent of sibling creation
    // quirks while still exercising both Tab and its Shift-Tab inverse.
    bool toSlotIsBackward = false;
    wxWindow *slotWindow = nullptr;
    for ( unsigned direction = 0; direction < 2; ++direction )
    {
        const bool backward = direction != 0;
        const HWND candidate = ::GetNextDlgTabItem(tlw, native, backward);
        wxWindow * const window = candidate
            ? wxGetWindowFromHWND(reinterpret_cast<WXHWND>(candidate))
            : nullptr;
        if ( window && host->FindSlot(window) )
        {
            toSlotIsBackward = backward;
            slotWindow = window;
            break;
        }
    }
    REQUIRE(slotWindow != nullptr);

    ::SetFocus(native);
    REQUIRE(::GetFocus() == native);
    REQUIRE(wxWindow::FindFocus() == wrapper);

    MSG toSlot = {};
    toSlot.hwnd = native;
    toSlot.message = WM_KEYDOWN;
    toSlot.wParam = VK_TAB;
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &toSlot, toSlotIsBackward, false, false));
    REQUIRE(DrainUntil(
        [host, slotWindow]()
        {
            return host->GetFocusOwner() == slotWindow &&
                   wxWindow::FindFocus() == slotWindow;
        }));

    MSG toNative = {};
    toNative.hwnd = ::GetFocus();
    toNative.message = WM_KEYDOWN;
    toNative.wParam = VK_TAB;
    REQUIRE(toNative.hwnd != nullptr);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &toNative, !toSlotIsBackward, false, false));
    REQUIRE(DrainUntil(
        [wrapper, native]()
        {
            return ::GetFocus() == native &&
                   wxWindow::FindFocus() == wrapper;
        }));
}

void CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
    ProbeNativeWindow *wrapper,
    unsigned expectedPendingFocusMigrations)
{
    REQUIRE(wrapper != nullptr);
    const HWND native = static_cast<HWND>(wrapper->GetHWND());
    REQUIRE(native != nullptr);

    // wxWindow::Reparent() promises a topology change, not preservation of
    // USER32 focus when moving into an inactive TLW. The WinUI migration
    // transaction deliberately covers focused XAML slots only, and both TLWs
    // in this fixture were shown without activation. Accept either legitimate
    // USER32 result, but reject a stale disagreement between native and wx
    // focus. Check both the synchronous boundary and the settled state: the
    // following mixed-Tab oracle deliberately reacquires the HWND itself.
    const HWND immediateNativeFocus = ::GetFocus();
    wxWindow * const immediateLogicalFocus = wxWindow::FindFocus();
    const unsigned immediatePendingFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    INFO("native focus immediately after cross-TLW Reparent: "
         << reinterpret_cast<std::uintptr_t>(immediateNativeFocus));
    INFO("logical focus immediately after cross-TLW Reparent: "
         << static_cast<const void *>(immediateLogicalFocus));
    INFO("pending WinUI focus migrations immediately after Reparent: "
         << immediatePendingFocusMigrations);
    CHECK((immediateNativeFocus == native) ==
          (immediateLogicalFocus == wrapper));
    CHECK(immediatePendingFocusMigrations ==
          expectedPendingFocusMigrations);

    DrainDispatch(4);
    REQUIRE(DrainToQuiescence());
    const HWND nativeFocus = ::GetFocus();
    wxWindow * const logicalFocus = wxWindow::FindFocus();
    const unsigned pendingFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    INFO("native focus after cross-TLW Reparent: "
         << reinterpret_cast<std::uintptr_t>(nativeFocus));
    INFO("logical focus after cross-TLW Reparent: "
         << static_cast<const void *>(logicalFocus));
    INFO("pending WinUI focus migrations after cross-TLW Reparent: "
         << pendingFocusMigrations);
    CHECK((nativeFocus == native) == (logicalFocus == wrapper));
    CHECK(pendingFocusMigrations == expectedPendingFocusMigrations);
}

void FinishFixture(TwoTLWFixture& fixture, const RuntimeSnapshot& before)
{
    fixture.Destroy();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

} // anonymous namespace

TEST_CASE("WinUINativeWindow::AttachDetachRewrapAndInfrastructure",
          "[winui-nativewindow][winui-015][HostLifecycle][generation]")
{
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWFixture fixture;
    REQUIRE(fixture.Create());

    NativeState firstState;
    NativeState secondState;
    const std::shared_ptr<RouteState> route =
        std::make_shared<RouteState>();
    ProbeNativeWindow *wrapper = nullptr;
    NativeLifetimeGuard lifetime(wrapper);
    const int id = 15001;
    HWND first = CreateNativeProbe(fixture.panelA, id, &firstState);
    lifetime.Track(first, &firstState);
    REQUIRE(first != nullptr);
    const LONG_PTR firstOriginalProc =
        ::GetWindowLongPtrW(first, GWLP_WNDPROC);
    REQUIRE(firstOriginalProc != 0);

    wrapper = WrapNative(fixture.panelA, id, first, route);
    CheckWrappedIdentity(wrapper, first, firstOriginalProc);
    CHECK(wxWinUITopLevelHost::FindForTLW(fixture.frameA) == fixture.hostA);

    CHECK(::SendMessageW(first, NativeHandledMessage, 0, 0) ==
          NativeHandledResult);
    CHECK(route->handledMessages == 1);
    CHECK(::SendMessageW(first, NativePassThroughMessage, 0, 0) ==
          NativePassThroughResult);
    CHECK(route->passThroughMessages == 1);
    CHECK(firstState.passThroughMessages == 1);

    // The external HWND has exactly its ordinary USER32 host provider. The
    // wrapper has no XAML slot and therefore no invisible duplicate shell.
    winrt::com_ptr<IRawElementProviderSimple> nativeProvider;
    REQUIRE(::UiaHostProviderFromHwnd(first, nativeProvider.put()) == S_OK);
    REQUIRE(nativeProvider != nullptr);
    ProviderOptions providerOptions{};
    REQUIRE(nativeProvider->get_ProviderOptions(&providerOptions) == S_OK);
    CHECK((providerOptions & ProviderOptions_ServerSideProvider) != 0);

    REQUIRE(wrapper->Hide());
    CHECK_FALSE(::IsWindowVisible(first));
    REQUIRE(wrapper->Show());
    CHECK(::IsWindowVisible(first));
    CHECK(wrapper->Enable(false));
    CHECK_FALSE(::IsWindowEnabled(first));
    CHECK(wrapper->Enable(true));
    CHECK(::IsWindowEnabled(first));
    wrapper->SetSize(wxRect(36, 32, 196, 72));
    RECT firstRect = {};
    REQUIRE(::GetClientRect(first, &firstRect));
    CHECK(firstRect.right - firstRect.left == 196);
    CHECK(firstRect.bottom - firstRect.top == 72);
    NativeCursorRestoreGuard cursorRestore;
    const wxCursor handCursor(wxCURSOR_HAND);
    REQUIRE(wrapper->SetCursor(handCursor));
    CHECK(wrapper->GetCursor().IsOk());
    REQUIRE(::SendMessageW(
                first, WM_SETCURSOR, reinterpret_cast<WPARAM>(first),
                MAKELPARAM(HTCLIENT, WM_MOUSEMOVE)) != 0);
    CHECK(::GetCursor() ==
          reinterpret_cast<HCURSOR>(handCursor.GetHCURSOR()));
    cursorRestore.Restore();

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    NativeDropTarget * const target = new NativeDropTarget;
    wrapper->SetDropTarget(target);
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(wrapper));
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, first));
    wxMSWOleDropTargetBinding firstBinding;
    REQUIRE(wxMSWOleLookupDropTarget(wrapper, &firstBinding) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(firstBinding.IsCurrent());
    CHECK(firstBinding.GetOwnerHwndIfCurrent() == first);
#endif

    const unsigned long long firstAssociationGeneration =
        wxWinUIMSWGetHwndGeneration(
            wrapper, reinterpret_cast<WXHWND>(first));
    REQUIRE(firstAssociationGeneration != 0);
    wrapper->DissociateHandle();
    CHECK(wrapper->GetHWND() == nullptr);
    CHECK(wxFindWinFromHandle(first) == nullptr);
    CHECK(::GetWindowLongPtrW(first, GWLP_WNDPROC) == firstOriginalProc);
    CHECK(::IsWindow(first));
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    CHECK_FALSE(firstBinding.IsCurrent());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, first));
#endif

    CHECK(::SendMessageW(first, NativePassThroughMessage, 0, 0) ==
          NativePassThroughResult);
    CHECK(firstState.passThroughMessages == 2);

    HWND second = CreateNativeProbe(fixture.panelA, id, &secondState,
                                    wxRect(40, 36, 188, 66));
    lifetime.Track(second, &secondState);
    REQUIRE(second != nullptr);
    const LONG_PTR secondOriginalProc =
        ::GetWindowLongPtrW(second, GWLP_WNDPROC);
    REQUIRE(secondOriginalProc != 0);
    wrapper->AssociateHandle(second);
    CheckWrappedIdentity(wrapper, second, secondOriginalProc);
    const unsigned long long secondAssociationGeneration =
        wxWinUIMSWGetHwndGeneration(
            wrapper, reinterpret_cast<WXHWND>(second));
    CHECK(secondAssociationGeneration != firstAssociationGeneration);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    wxMSWOleDropTargetBinding secondBinding;
    REQUIRE(wxMSWOleLookupDropTarget(wrapper, &secondBinding) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(secondBinding.IsCurrent());
    CHECK(secondBinding.GetOwnerHwndIfCurrent() == second);
    CHECK_FALSE(secondBinding.IsSameBinding(firstBinding));
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, second));
#endif

    delete wrapper;
    wrapper = nullptr;
    CHECK(::IsWindow(second));
    CHECK(::GetWindowLongPtrW(second, GWLP_WNDPROC) == secondOriginalProc);
    CHECK(wxFindWinFromHandle(second) == nullptr);
    nativeProvider = nullptr;
    REQUIRE(::DestroyWindow(first));
    REQUIRE(::DestroyWindow(second));
    CHECK(firstState.destroyMessages == 1);
    CHECK(firstState.ncDestroyMessages == 1);
    CHECK(secondState.destroyMessages == 1);
    CHECK(secondState.ncDestroyMessages == 1);

    FinishFixture(fixture, before);
}

TEST_CASE("WinUINativeWindow::CompositionFocusCaptureDropAndCrossTLWReparent",
           "[winui-nativewindow][winui-015][HostLifecycle][z-order]"
          "[focus][capture][reparent][winui-drop-broker]")
{
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWFixture fixture;
    REQUIRE(fixture.Create());

    NativeState state;
    const std::shared_ptr<RouteState> route =
        std::make_shared<RouteState>();
    ProbeNativeWindow *wrapper = nullptr;
    NativeLifetimeGuard lifetime(wrapper);
    const int id = 15002;
    HWND hwnd = CreateNativeProbe(fixture.panelA, id, &state);
    lifetime.Track(hwnd, &state);
    REQUIRE(hwnd != nullptr);
    const LONG_PTR originalProc = ::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
    wrapper = WrapNative(fixture.panelA, id, hwnd, route);
    CheckWrappedIdentity(wrapper, hwnd, originalProc);

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    NativeDropTarget * const target = new NativeDropTarget;
    wrapper->SetDropTarget(target);
    wxMSWOleDropTargetBinding bindingA;
    REQUIRE(wxMSWOleLookupDropTarget(wrapper, &bindingA) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(bindingA.IsCurrent());
    CHECK(bindingA.GetOwnerHwndIfCurrent() == hwnd);
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(wrapper));
    wxWinUIDropBroker * const brokerA =
        fixture.hostA->GetDropBrokerForTest();
    REQUIRE(brokerA != nullptr);
    winrt::com_ptr<IDropTarget> comA;
    comA.copy_from(brokerA->GetCOMTargetForTest());
    REQUIRE(comA != nullptr);
    CheckTextDropRoute(comA.get(), hwnd, target,
                       "native-window-host-a", 1);
#endif

    // Adjacent native and XAML surfaces require no impossible interleave.
    REQUIRE(DrainToQuiescence());
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // With overlap, XAML above native is representable by the one topmost
    // bridge. Put the native child below the XAML shell and prove that this
    // direction remains accepted by the production topology classifier.
    fixture.buttonA->SetSize(wxRect(48, 34, 132, 48));
    REQUIRE(::SetWindowPos(
        hwnd, static_cast<HWND>(fixture.buttonA->GetHWND()),
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // The reverse overlap is the plan's explicit one-island limitation: a
    // native child requested above overlapping XAML cannot overtake the
    // bridge. It must be diagnosed, never silently treated as S0 support.
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() >= 1);

    // Scrolled/paged parents clip native children at their client boundary.
    // Moving the wrapper fully outside that viewport must retire the overlap
    // instead of retaining a stale bounding-box diagnostic.
    wrapper->SetSize(wxRect(-220, 34, 180, 64));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // Restore the overlap, then prove that an explicit empty native clip is
    // also honoured. This exercises both ancestor and SetWindowRgn clipping.
    wrapper->SetSize(wxRect(24, 24, 180, 64));
    REQUIRE(::SetWindowPos(
        hwnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() >= 1);

    HRGN emptyClip = ::CreateRectRgn(0, 0, 0, 0);
    REQUIRE(emptyClip != nullptr);
    REQUIRE(InstallWindowRegion(hwnd, emptyClip));
    wxWinUITLWHostNotifyNativeLayout(wrapper, false);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);
    REQUIRE(InstallWindowRegion(hwnd, nullptr));
    REQUIRE(::SetWindowPos(
        hwnd, static_cast<HWND>(fixture.buttonA->GetHWND()),
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    wxWinUITLWHostNotifyNativeLayout(wrapper, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.hostA->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    // Programmatic mixed Tab/Shift-Tab, focus and capture exercise the actual
    // external HWND without physical input injection. The generation-bound
    // guard restores the pre-test focus on normal and assertion unwind paths.
    NativeFocusRestoreGuard focusRestore;
    CheckMixedTabRoundTrip(wrapper, fixture.hostA);
    REQUIRE(wxWindow::GetCapture() == before.wxCapture);
    REQUIRE(::GetCapture() == before.nativeCapture);
    wrapper->CaptureMouse();
    CHECK(wxWindow::GetCapture() == wrapper);
    CHECK(::GetCapture() == hwnd);
    wrapper->ReleaseMouse();
    CHECK(wxWindow::GetCapture() == before.wxCapture);
    CHECK(::GetCapture() == before.nativeCapture);

    REQUIRE(wrapper->Reparent(fixture.panelB));
    CHECK(wrapper->GetParent() == fixture.panelB);
    CHECK(::GetParent(hwnd) == GetHwndOf(fixture.panelB));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(wrapper) == nullptr);
    CHECK(wxWinUITopLevelHost::FindForTLW(fixture.frameB) == fixture.hostB);
    CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
        wrapper, before.pendingFocusMigrations);
    CheckMixedTabRoundTrip(wrapper, fixture.hostB);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    wxMSWOleDropTargetBinding bindingB;
    REQUIRE(wxMSWOleLookupDropTarget(wrapper, &bindingB) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(bindingB.IsCurrent());
    CHECK(bindingB.GetOwnerHwndIfCurrent() == hwnd);
    // Host ownership moves A -> B, but the logical drop target remains bound
    // to the same wrapper/HWND generation throughout the reparent.
    CHECK(bindingB.IsSameBinding(bindingA));
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(wrapper));
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, hwnd));

    fixture.hostB->FlushSync();
    const POINTL pointB = NativeCenterInScreen(hwnd);
    wxTextDataObject movedSource("native-window-host-b");
    DWORD oldEffect = DROPEFFECT_COPY;
    REQUIRE(comA->DragEnter(
                movedSource.GetInterface(), 0, pointB, &oldEffect) == S_OK);
    CHECK(oldEffect == DROPEFFECT_NONE);
    REQUIRE(comA->DragLeave() == S_OK);
    CHECK(target->enters == 1);
    CHECK(target->drops == 1);

    wxWinUIDropBroker * const brokerB =
        fixture.hostB->GetDropBrokerForTest();
    REQUIRE(brokerB != nullptr);
    winrt::com_ptr<IDropTarget> comB;
    comB.copy_from(brokerB->GetCOMTargetForTest());
    REQUIRE(comB != nullptr);
    CheckTextDropRoute(comB.get(), hwnd, target,
                       "native-window-host-b", 2);
#endif

    REQUIRE(wrapper->Reparent(fixture.panelA));
    CHECK(::GetParent(hwnd) == GetHwndOf(fixture.panelA));
    CHECK(wxWinUITopLevelHost::FindForTLW(fixture.frameA) == fixture.hostA);
    CheckFocusCoherenceAfterNoActivateCrossTLWReparent(
        wrapper, before.pendingFocusMigrations);
    CheckMixedTabRoundTrip(wrapper, fixture.hostA);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    comB = nullptr;
    comA = nullptr;
#endif
    focusRestore.Restore();
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();

    delete wrapper;
    wrapper = nullptr;
    CHECK(::IsWindow(hwnd));
    CHECK(::GetWindowLongPtrW(hwnd, GWLP_WNDPROC) == originalProc);
    REQUIRE(::DestroyWindow(hwnd));

    FinishFixture(fixture, before);
}

TEST_CASE("WinUINativeWindow::ReentrantMessagesAndBothDestroyOrders",
          "[winui-nativewindow][winui-015][HostLifecycle][reentrancy]"
          "[destroy]")
{
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWFixture fixture;
    REQUIRE(fixture.Create());

    SECTION("message callback reparents across top-level hosts")
    {
        NativeState state;
        const std::shared_ptr<RouteState> route =
            std::make_shared<RouteState>();
        ProbeNativeWindow *wrapper = nullptr;
        NativeLifetimeGuard lifetime(wrapper);
        const int id = 15003;
        HWND hwnd = CreateNativeProbe(fixture.panelA, id, &state);
        lifetime.Track(hwnd, &state);
        REQUIRE(hwnd != nullptr);
        const LONG_PTR originalProc =
            ::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
        wrapper = WrapNative(fixture.panelA, id, hwnd, route);
        REQUIRE(wrapper != nullptr);
        bool reparented = false;
        route->action = [&fixture, &reparented](ProbeNativeWindow *window)
        {
            reparented = window->Reparent(fixture.panelB);
        };

        CHECK(::SendMessageW(hwnd, NativeHandledMessage, 0, 0) ==
              NativeHandledResult);
        CHECK(reparented);
        CHECK(route->handledMessages == 1);
        CHECK(wrapper->GetParent() == fixture.panelB);
        CHECK(::GetParent(hwnd) == GetHwndOf(fixture.panelB));
        CHECK(wxWinUITopLevelHost::FindSlotOwner(wrapper) == nullptr);

        route->action = {};
        delete wrapper;
        wrapper = nullptr;
        CHECK(::GetWindowLongPtrW(hwnd, GWLP_WNDPROC) == originalProc);
        REQUIRE(::DestroyWindow(hwnd));
    }

    SECTION("message callback deletes wx wrapper before external HWND")
    {
        NativeState state;
        const std::shared_ptr<RouteState> route =
            std::make_shared<RouteState>();
        ProbeNativeWindow *wrapper = nullptr;
        NativeLifetimeGuard lifetime(wrapper);
        const int id = 15004;
        HWND hwnd = CreateNativeProbe(fixture.panelA, id, &state);
        lifetime.Track(hwnd, &state);
        REQUIRE(hwnd != nullptr);
        const LONG_PTR originalProc =
            ::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
        wrapper = WrapNative(fixture.panelA, id, hwnd, route);
        REQUIRE(wrapper != nullptr);
        const wxWeakRef<wxWindow> weakWrapper(wrapper);
        route->action = [&wrapper](ProbeNativeWindow *window)
        {
            wrapper = nullptr;
            delete window;
        };

        CHECK(::SendMessageW(hwnd, NativeHandledMessage, 0, 0) ==
              NativeHandledResult);
        CHECK(wrapper == nullptr);
        CHECK_FALSE(weakWrapper);
        CHECK(::IsWindow(hwnd));
        CHECK(wxFindWinFromHandle(hwnd) == nullptr);
        CHECK(::GetWindowLongPtrW(hwnd, GWLP_WNDPROC) == originalProc);
        CHECK(::SendMessageW(hwnd, NativePassThroughMessage, 0, 0) ==
              NativePassThroughResult);
        CHECK(state.passThroughMessages == 1);
        REQUIRE(::DestroyWindow(hwnd));
    }

    SECTION("message callback destroys external HWND before wx wrapper")
    {
        NativeState state;
        NativeState replacementState;
        const std::shared_ptr<RouteState> route =
            std::make_shared<RouteState>();
        ProbeNativeWindow *wrapper = nullptr;
        NativeLifetimeGuard lifetime(wrapper);
        const int id = 15005;
        HWND hwnd = CreateNativeProbe(fixture.panelA, id, &state);
        lifetime.Track(hwnd, &state);
        REQUIRE(hwnd != nullptr);
        wrapper = WrapNative(fixture.panelA, id, hwnd, route);
        REQUIRE(wrapper != nullptr);
        const unsigned long long destroyedGeneration =
            wxWinUIMSWGetHwndGeneration(
                wrapper, reinterpret_cast<WXHWND>(hwnd));
        REQUIRE(destroyedGeneration != 0);
        bool nativeDestroyed = false;
        route->action = [hwnd, &nativeDestroyed](ProbeNativeWindow *)
        {
            nativeDestroyed = ::DestroyWindow(hwnd) != 0;
        };

        CHECK(::SendMessageW(hwnd, NativeHandledMessage, 0, 0) ==
              NativeHandledResult);
        CHECK(nativeDestroyed);
        CHECK_FALSE(::IsWindow(hwnd));
        CHECK(state.destroyMessages == 1);
        CHECK(state.ncDestroyMessages == 1);
        CHECK(wrapper->GetHWND() == nullptr);
        CHECK(wxFindWinFromHandle(hwnd) == nullptr);
        CHECK(wxWinUIMSWGetHwndGeneration(
                  wrapper, reinterpret_cast<WXHWND>(hwnd)) == 0);

        // WM_DESTROY must have retired the exact association already, not
        // deferred it until dtor where the numeric HWND could be recycled.
        // Prove that this same logical wrapper can bind a new generation.
        route->action = {};
        HWND replacement = CreateNativeProbe(
            fixture.panelA, id, &replacementState,
            wxRect(34, 30, 184, 64));
        lifetime.Track(replacement, &replacementState);
        REQUIRE(replacement != nullptr);
        const LONG_PTR replacementOriginalProc =
            ::GetWindowLongPtrW(replacement, GWLP_WNDPROC);
        wrapper->AssociateHandle(replacement);
        CheckWrappedIdentity(wrapper, replacement, replacementOriginalProc);
        const unsigned long long replacementGeneration =
            wxWinUIMSWGetHwndGeneration(
                wrapper, reinterpret_cast<WXHWND>(replacement));
        REQUIRE(replacementGeneration != 0);
        CHECK(replacementGeneration != destroyedGeneration);

        delete wrapper;
        wrapper = nullptr;
        CHECK(::GetWindowLongPtrW(replacement, GWLP_WNDPROC) ==
              replacementOriginalProc);
        REQUIRE(::DestroyWindow(replacement));
    }

    SECTION("Disown transfers terminal HWND destruction to wx")
    {
        NativeState state;
        const std::shared_ptr<RouteState> route =
            std::make_shared<RouteState>();
        ProbeNativeWindow *wrapper = nullptr;
        NativeLifetimeGuard lifetime(wrapper);
        const int id = 15006;
        HWND hwnd = CreateNativeProbe(fixture.panelA, id, &state);
        lifetime.Track(hwnd, &state);
        REQUIRE(hwnd != nullptr);
        wrapper = WrapNative(fixture.panelA, id, hwnd, route);
        REQUIRE(wrapper != nullptr);
        wrapper->Disown();
        delete wrapper;
        wrapper = nullptr;
        CHECK_FALSE(::IsWindow(hwnd));
        CHECK(state.destroyMessages == 1);
        CHECK(state.ncDestroyMessages == 1);
    }

    FinishFixture(fixture, before);
}

TEST_CASE("WinUINativeWindow::HundredAttachReparentAndDestroyCycles",
          "[winui-nativewindow][winui-015][HostLifecycle][stress]"
          "[generation][reparent][destroy]")
{
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    TwoTLWFixture fixture;
    REQUIRE(fixture.Create());
    REQUIRE(DrainToQuiescence());

    const unsigned steadyHosts = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned steadyHostLifetimes =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned steadySlotLifetimes =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned steadySubclasses =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    const unsigned steadyFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    const wxWinUITransientRegistrySnapshot steadyTransients =
        wxWinUIGetTransientRegistrySnapshotForTesting();

    unsigned externalFirst = 0;
    unsigned wrapperFirst = 0;
    unsigned rewraps = 0;
    unsigned wxOwned = 0;

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        INFO("native-window lifecycle cycle " << cycle);
        wxPanel * const source = cycle % 2 ? fixture.panelB : fixture.panelA;
        wxPanel * const destination =
            cycle % 2 ? fixture.panelA : fixture.panelB;
        const int id = 15100 + static_cast<int>(cycle);

        NativeState firstState;
        NativeState replacementState;
        const std::shared_ptr<RouteState> route =
            std::make_shared<RouteState>();
        ProbeNativeWindow *wrapper = nullptr;
        NativeLifetimeGuard lifetime(wrapper);
        HWND first = CreateNativeProbe(source, id, &firstState);
        lifetime.Track(first, &firstState);
        REQUIRE(first != nullptr);
        const LONG_PTR firstOriginalProc =
            ::GetWindowLongPtrW(first, GWLP_WNDPROC);
        wrapper = WrapNative(source, id, first, route);
        REQUIRE(wrapper != nullptr);
        const unsigned long long firstGeneration =
            wxWinUIMSWGetHwndGeneration(
                wrapper, reinterpret_cast<WXHWND>(first));
        REQUIRE(firstGeneration != 0);

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        NativeDropTarget * const target = new NativeDropTarget;
        wrapper->SetDropTarget(target);
        wxMSWOleDropTargetBinding sourceBinding;
        REQUIRE(wxMSWOleLookupDropTarget(wrapper, &sourceBinding) ==
                wxMSWOleDropTargetLookup::Found);
        REQUIRE(sourceBinding.IsCurrent());
        CHECK(sourceBinding.GetOwnerHwndIfCurrent() == first);
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, first));
#endif

        CHECK(::SendMessageW(first, NativePassThroughMessage, 0, 0) ==
              NativePassThroughResult);
        CHECK(route->passThroughMessages == 1);
        CHECK(firstState.passThroughMessages == 1);
        wrapper->SetSize(28 + static_cast<int>(cycle % 7),
                         26 + static_cast<int>(cycle % 5),
                         176 + static_cast<int>(cycle % 9), 62);
        REQUIRE(wrapper->Hide());
        REQUIRE(wrapper->Show());
        CHECK(wrapper->Enable(false));
        CHECK(wrapper->Enable(true));

        REQUIRE(wrapper->Reparent(destination));
        CHECK(wrapper->GetParent() == destination);
        CHECK(::GetParent(first) == GetHwndOf(destination));
        CHECK(wxWinUITopLevelHost::FindSlotOwner(wrapper) == nullptr);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        wxMSWOleDropTargetBinding destinationBinding;
        REQUIRE(wxMSWOleLookupDropTarget(wrapper, &destinationBinding) ==
                wxMSWOleDropTargetLookup::Found);
        REQUIRE(destinationBinding.IsCurrent());
        CHECK(destinationBinding.GetOwnerHwndIfCurrent() == first);
        CHECK(destinationBinding.IsSameBinding(sourceBinding));
        CHECK(wxWinUITLWHostOwnsOleDropRegistration(wrapper));
#endif

        REQUIRE(wrapper->Reparent(source));
        CHECK(::GetParent(first) == GetHwndOf(source));

        switch ( cycle % 4 )
        {
            case 0:
                delete wrapper;
                wrapper = nullptr;
                CHECK(::IsWindow(first));
                CHECK(::GetWindowLongPtrW(first, GWLP_WNDPROC) ==
                      firstOriginalProc);
                REQUIRE(::DestroyWindow(first));
                ++wrapperFirst;
                break;

            case 1:
                REQUIRE(::DestroyWindow(first));
                CHECK(firstState.destroyMessages == 1);
                CHECK(wrapper->GetHWND() == nullptr);
                CHECK(wxFindWinFromHandle(first) == nullptr);
                CHECK(wxWinUIMSWGetHwndGeneration(
                          wrapper, reinterpret_cast<WXHWND>(first)) == 0);
                delete wrapper;
                wrapper = nullptr;
                ++externalFirst;
                break;

            case 2:
            {
                wrapper->DissociateHandle();
                CHECK(wrapper->GetHWND() == nullptr);
                CHECK(::GetWindowLongPtrW(first, GWLP_WNDPROC) ==
                      firstOriginalProc);

                HWND replacement = CreateNativeProbe(
                    source, id, &replacementState,
                    wxRect(32, 30, 184, 64));
                lifetime.Track(replacement, &replacementState);
                REQUIRE(replacement != nullptr);
                const LONG_PTR replacementOriginalProc =
                    ::GetWindowLongPtrW(replacement, GWLP_WNDPROC);
                wrapper->AssociateHandle(replacement);
                const unsigned long long replacementGeneration =
                    wxWinUIMSWGetHwndGeneration(
                        wrapper, reinterpret_cast<WXHWND>(replacement));
                CHECK(replacementGeneration != 0);
                CHECK(replacementGeneration != firstGeneration);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
                wxMSWOleDropTargetBinding replacementBinding;
                REQUIRE(wxMSWOleLookupDropTarget(
                            wrapper, &replacementBinding) ==
                        wxMSWOleDropTargetLookup::Found);
                CHECK(replacementBinding.GetOwnerHwndIfCurrent() ==
                      replacement);
#endif
                delete wrapper;
                wrapper = nullptr;
                CHECK(::GetWindowLongPtrW(replacement, GWLP_WNDPROC) ==
                      replacementOriginalProc);
                REQUIRE(::DestroyWindow(first));
                REQUIRE(::DestroyWindow(replacement));
                ++rewraps;
                break;
            }

            default:
                wrapper->Disown();
                delete wrapper;
                wrapper = nullptr;
                CHECK_FALSE(::IsWindow(first));
                ++wxOwned;
                break;
        }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        CHECK_FALSE(sourceBinding.IsCurrent());
#endif
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == steadyHosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              steadyHostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              steadySlotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              steadySubclasses);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              steadyFocusMigrations);
        CHECK(SameTransientRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            steadyTransients));
        CHECK(wxWindow::GetCapture() == before.wxCapture);
        CHECK(::GetCapture() == before.nativeCapture);

        if ( cycle % 10 == 9 )
            REQUIRE(DrainToQuiescence());
    }

    CHECK(wrapperFirst == 25);
    CHECK(externalFirst == 25);
    CHECK(rewraps == 25);
    CHECK(wxOwned == 25);

    FinishFixture(fixture, before);
}

#endif // wxHAS_NATIVE_WINDOW && wxUSE_BUTTON && wxUSE_TEXTCTRL
#endif // __WXWINUI__ && wxUSE_WINUI3
