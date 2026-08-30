/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/tlwhost.cpp
// Purpose:     one shared XAML island per top-level window
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/titlebar.h"
#include "wx/winui/xamlhost.h"
#if wxUSE_RICHTOOLTIP
    #include "wx/winui/private/richtooltip.h"
#endif
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/winui/private/dropbroker.h"
#endif

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/toplevel.h"
#include "wx/control.h"
#include "wx/dialog.h"
#include "wx/scopeguard.h"
#include "wx/thread.h"
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif
#include "wx/msw/private.h"
#if wxUSE_DRAG_AND_DROP
    #include "wx/msw/private/dropfiles.h"
    #include <shellapi.h>
#endif

#include <windowsx.h>
#include <commctrl.h>
#include <UIAutomation.h>
#include <UIAutomationCoreApi.h>
#include <wrl/implements.h>

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.System.h>

#include <cmath>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <exception>
#include <functional>
#include <limits>
#include <set>
#include <utility>
#include <string>
#include <vector>

void wxWinUILogException(const char *what, const winrt::hresult_error& e);

namespace
{

// ABI contract from Microsoft.UI.Input.InputCursor.Interop.h. Declaring the
// tiny COM seam locally avoids mixing the package's raw generated namespace
// header with our C++/WinRT projection headers; the returned pointer is the
// default IInputCursor interface and can be attached directly to InputCursor.
struct __declspec(uuid("ac6f5065-90c4-46ce-beb7-05e138e54117"))
    __declspec(novtable) IWxInputCursorStaticsInterop
        : winrt::impl::inspectable_abi
{
    virtual HRESULT STDMETHODCALLTYPE CreateFromHCursor(
        HCURSOR cursor,
        winrt::impl::inspectable_abi **result) = 0;
};

// ----------------------------------------------------------------------------
// Input black box: set WX_WINUI_INPUT_LOG=<file> to trace the island input
// pipeline (root pointer events, forwards, capture states).  This exists to
// DIAGNOSE the input-freeze family with facts instead of theories: when a
// window freezes, the tail of this log says exactly where the flow stopped
// (island no longer receiving? receiving but not forwarding? forwarding but
// the target ignoring?).  No-op when the variable is unset.
// ----------------------------------------------------------------------------

FILE *wxWinUIInputLogFile()
{
    static FILE *s_file = []() -> FILE *
    {
        const DWORD required =
            ::GetEnvironmentVariableW(
                L"WX_WINUI_INPUT_LOG", nullptr, 0);
        if ( !required )
            return nullptr;

        std::vector<wchar_t> path(required);
        const DWORD copied =
            ::GetEnvironmentVariableW(
                L"WX_WINUI_INPUT_LOG", path.data(), required);
        if ( !copied || copied >= required || !path[0] )
            return nullptr;

        FILE * const f = _wfopen(path.data(), L"a");
        if ( f )
        {
            fprintf(f, "\n==== session pid=%lu ====\n",
                    ::GetCurrentProcessId());
            fflush(f);
        }
        return f;
    }();

    return s_file;
}

void wxWinUIInputLog(const char *fmt, ...)
{
    FILE * const f = wxWinUIInputLogFile();
    if ( !f )
        return;

    // Every line carries the tick, the current Win32 capture window and the
    // foreground window: the three state values the freeze family hinges on.
    fprintf(f, "[%9llu cap=%p fg=%p] ",
            static_cast<unsigned long long>(::GetTickCount64()),
            static_cast<void *>(::GetCapture()),
            static_cast<void *>(::GetForegroundWindow()));

    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);

    fputc('\n', f);
    fflush(f);
}

bool wxWinUIInputLogEnabled() { return wxWinUIInputLogFile() != nullptr; }
// ----------------------------------------------------------------------------
// Pointer routing profile: set WX_WINUI_INPUT_PROFILE=<file> to get the
// per-phase cost of routing one pointer sample.  Moving the mouse is the most
// frequent input an application receives -- a mouse reports hundreds of times
// per second -- so a few hundred microseconds spent per sample is felt by the
// user as a window that stops following the pointer.  Unlike the input log
// above this writes one aggregated line every kProfileBatch samples, so
// measuring does not change what is measured.  No-op when the variable is
// unset.
// ----------------------------------------------------------------------------

FILE *wxWinUIInputProfileFile()
{
    static FILE *s_file = []() -> FILE *
    {
        const DWORD required =
            ::GetEnvironmentVariableW(
                L"WX_WINUI_INPUT_PROFILE", nullptr, 0);
        if ( !required )
            return nullptr;

        std::vector<wchar_t> path(required);
        const DWORD copied =
            ::GetEnvironmentVariableW(
                L"WX_WINUI_INPUT_PROFILE", path.data(), required);
        if ( !copied || copied >= required || !path[0] )
            return nullptr;

        FILE * const f = _wfopen(path.data(), L"a");
        if ( f )
        {
            fprintf(f, "\n==== profile pid=%lu (us per pointer sample) ====\n",
                    ::GetCurrentProcessId());
            fflush(f);
        }
        return f;
    }();

    return s_file;
}

bool wxWinUIInputProfileEnabled()
{
    static const bool s_enabled = wxWinUIInputProfileFile() != nullptr;
    return s_enabled;
}

// The phases of one pointer sample, in the order they run.
enum class wxWinUIProfilePhase
{
    Total,
    Point,        // args.GetCurrentPoint() and the properties it exposes
    XamlHit,      // VisualTreeHelper hit test against the island content
    NativeHit,    // resolving the child window under the pointer
    Dispatch,     // the state machine and the SendMessage it ends with
    Mirror,       // mirroring the native cursor for the hit target
    MirrorSend,   // the WM_SETCURSOR the target runs
    MirrorCheck,  // re-proving the target still owns the point
    MirrorApply,  // converting and applying the island cursor
    Refresh,      // re-proving the native hit before delivery
    Send,         // the WM_MOUSEMOVE the target finally receives
    Cursor,       // observing and re-applying the XAML pointer cursor
    Max
};

long long gs_profileTicks[static_cast<size_t>(wxWinUIProfilePhase::Max)] = {};

// Host-wide invalidations seen while routing pointer input: one of these
// per pointer move means every hosted control is re-synchronised for
// every movement of the mouse.
unsigned gs_profileCursorPolicy = 0;
unsigned gs_profileLayoutNotify = 0;
unsigned gs_profileLayoutSuppressed = 0;
unsigned gs_profileMarkAllDirty = 0;
unsigned gs_profileMarkDirty = 0;
unsigned gs_profileFlushRuns = 0;
unsigned gs_profileSamples = 0;

long long wxWinUIProfileNow()
{
    LARGE_INTEGER now;
    ::QueryPerformanceCounter(&now);
    return now.QuadPart;
}

void wxWinUIProfileAdd(wxWinUIProfilePhase phase, long long started)
{
    gs_profileTicks[static_cast<size_t>(phase)] +=
        wxWinUIProfileNow() - started;
}

// Times one phase for as long as it is in scope.
class wxWinUIProfileScope
{
public:
    explicit wxWinUIProfileScope(wxWinUIProfilePhase phase)
        : m_phase(phase),
          m_started(wxWinUIInputProfileEnabled() ? wxWinUIProfileNow() : 0)
    {
    }

    ~wxWinUIProfileScope()
    {
        if ( m_started )
            wxWinUIProfileAdd(m_phase, m_started);
    }

private:
    const wxWinUIProfilePhase m_phase;
    const long long m_started;

    wxDECLARE_NO_COPY_CLASS(wxWinUIProfileScope);
};

// Called once per routed pointer sample; writes an average line per batch.
void wxWinUIProfileSample()
{
    FILE * const f = wxWinUIInputProfileFile();
    if ( !f )
        return;

    constexpr unsigned kProfileBatch = 20;
    if ( ++gs_profileSamples < kProfileBatch )
        return;

    LARGE_INTEGER frequency;
    ::QueryPerformanceFrequency(&frequency);
    const double perSample =
        1000000.0 / (double(frequency.QuadPart) * gs_profileSamples);

    const auto us = [perSample](wxWinUIProfilePhase phase)
    {
        return gs_profileTicks[static_cast<size_t>(phase)] * perSample;
    };

    fprintf(f,
            "samples=%u total=%.0f point=%.0f xamlhit=%.0f nativehit=%.0f "
            "dispatch=%.0f mirror=%.0f (send=%.0f check=%.0f apply=%.0f) "
            "refresh=%.0f send=%.0f cursor=%.0f policy=%u flush=%u\n",
            gs_profileSamples,
            us(wxWinUIProfilePhase::Total),
            us(wxWinUIProfilePhase::Point),
            us(wxWinUIProfilePhase::XamlHit),
            us(wxWinUIProfilePhase::NativeHit),
            us(wxWinUIProfilePhase::Dispatch),
            us(wxWinUIProfilePhase::Mirror),
            us(wxWinUIProfilePhase::MirrorSend),
            us(wxWinUIProfilePhase::MirrorCheck),
            us(wxWinUIProfilePhase::MirrorApply),
            us(wxWinUIProfilePhase::Refresh),
            us(wxWinUIProfilePhase::Send),
            us(wxWinUIProfilePhase::Cursor),
            gs_profileCursorPolicy,
            gs_profileFlushRuns);
    fflush(f);

    gs_profileSamples = 0;
    gs_profileCursorPolicy = 0;
    gs_profileFlushRuns = 0;
    for ( auto& ticks : gs_profileTicks )
        ticks = 0;
}


void wxWinUIInputLogTLWMoveStart(wxMoveEvent& event)
{
    wxWinUIInputLog("tlw ENTER-SIZEMOVE");
    event.Skip();
}

void wxWinUIInputLogTLWMoveEnd(wxMoveEvent& event)
{
    wxWinUIInputLog("tlw EXIT-SIZEMOVE");
    event.Skip();
}

void wxWinUIInputLogTLWActivate(wxActivateEvent& event)
{
    wxWinUIInputLog("tlw ACTIVATE=%d", event.GetActive());
    event.Skip();
}

UINT wxWinUIInternalCancelMessage()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.InternalCancelMode");
    return message;
}

UINT wxWinUINativeResizeMessage()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.NativeResize");
    return message;
}

bool (*gs_nativeResizeContactReader)(void *) = nullptr;
void *gs_nativeResizeContactContext = nullptr;
bool gs_failNextNativeResizePost = false;
std::set<wxWinUITopLevelHost *> gs_deferredNativeResizeWakes;

// All live hosts, keyed by their top-level window.
std::map<const wxWindow *, wxWinUITopLevelHost *> gs_tlwHosts;
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
// Native OLE coverage is intentionally independent from gs_tlwHosts. Logical
// host publication is removed before some callback-bearing teardown phases,
// while either exact broker registration can remain until the dual pair is
// revoked or retained fail-closed after a native cleanup failure.
using wxWinUIOleDropCoverageKey =
    std::pair<WXHWND, unsigned long long>;
std::map<wxWinUIOleDropCoverageKey,
         std::set<wxWinUIOleDropRegistrationCoverageToken>>
    gs_oleDropRegistrationCoverage;
wxWinUIOleDropRegistrationCoverageToken gs_nextOleDropCoverageToken = 0;
#endif
// Authoritative owner of each slot. During a cross-TLW transfer both slot
// allocations coexist briefly for rollback, so scanning host maps is
// inherently ambiguous; this registry is switched transactionally to the
// destination as soon as its slot becomes callback-safe.
std::map<const wxWindow *, wxWinUITopLevelHost *> gs_slotOwners;

// A TLW whose physical-disconnect gates are still pending transfers its
// source/root graph to one retirement. Exact gate completion can consume that
// ticket during normal dispatcher operation; XamlShutdownCompletedOnThread is
// the fail-closed fallback when causal completion never arrives.
std::atomic<unsigned> gs_pendingTLWTerminalRetirements{ 0 };
std::atomic<unsigned> gs_pendingPhysicalDisconnectGates{ 0 };
std::uint64_t gs_nextPhysicalDisconnectRetirementId = 0;

class wxWinUITLWTerminalRetirement final
{
public:
    void Promote() noexcept
    {
        if ( m_completed.load(std::memory_order_acquire) )
            return;
        bool expected = false;
        if ( m_counted.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            gs_pendingTLWTerminalRetirements.fetch_add(
                1, std::memory_order_relaxed);
        }
    }

    ~wxWinUITLWTerminalRetirement()
    {
        Complete();
    }

    void RequestComplete() noexcept
    {
        m_completionRequested.store(true, std::memory_order_release);
        if ( m_completionArmed.load(std::memory_order_acquire) )
            Complete();
    }

    void AdoptSource(
        winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource&& source)
        noexcept
    {
        m_source = std::move(source);
    }

    void AdoptRoot(
        winrt::Microsoft::UI::Xaml::Controls::Canvas&& root) noexcept
    {
        m_root = std::move(root);
    }

    void SetRetirementId(wxWinUIFrameworkRetirementId id) noexcept
    {
        m_retirementId.store(id, std::memory_order_release);
    }

    bool AdoptGates(
        std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>>&& gates)
        noexcept
    {
        try
        {
            m_gates = std::move(gates);
            return true;
        }
        catch ( ... )
        {
            return false;
        }
    }

    const std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>>&
    GetGatesForRetarget() const noexcept
    {
        return m_gates;
    }

    void GateCompleted() noexcept
    {
        const unsigned previous =
            m_pendingGates.fetch_sub(1, std::memory_order_acq_rel);
        if ( previous == 0 )
        {
            // A completion destination is single-shot. Reaching zero twice
            // means a gate violated that contract, and wrapping the counter
            // would hide a terminal graph forever.
            std::terminate();
        }
        if ( previous == 1 )
        {
            // Every child already performed its own normal handler cleanup,
            // so these aliases no longer need to retain the control adapters.
            m_gates.clear();
            const wxWinUIFrameworkRetirementId id =
                m_retirementId.load(std::memory_order_acquire);
            if ( id )
                wxWinUICompleteFrameworkRetirement(id);
        }
    }

    void SetPendingGateCount(unsigned count) noexcept
    {
        m_pendingGates.store(count, std::memory_order_release);
    }

    unsigned GetPendingGateCount() const noexcept
    {
        return m_pendingGates.load(std::memory_order_acquire);
    }

    void ArmForCompletion() noexcept
    {
        m_completionArmed.store(true, std::memory_order_release);
        if ( m_completionRequested.load(std::memory_order_acquire) )
            Complete();
    }

private:
    void Complete() noexcept
    {
        bool expected = false;
        if ( !m_completed.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            return;
        }

        // Any gates still present were frozen by dispatcher rundown: revoke
        // their projection handlers first while the XAML terminal deferral is
        // active. DesktopWindowXamlSource is then the outer graph lifetime;
        // release it before the still-complete root graph.
        for ( const auto& gate : m_gates )
        {
            if ( gate )
                gate->CompleteAtXamlBoundary();
        }
        m_gates.clear();
        m_source = nullptr;
        m_root = nullptr;

        if ( m_counted.load(std::memory_order_acquire) )
        {
            gs_pendingTLWTerminalRetirements.fetch_sub(
                1, std::memory_order_relaxed);
        }
    }

    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource
        m_source{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Canvas m_root{ nullptr };
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> m_gates;
    std::atomic<bool> m_completionArmed{false};
    std::atomic<bool> m_completionRequested{false};
    std::atomic<bool> m_completed{false};
    std::atomic<bool> m_counted{false};
    std::atomic<unsigned> m_pendingGates{0};
    std::atomic<wxWinUIFrameworkRetirementId> m_retirementId{0};
};

// Cumulative counters for the lifecycle unit tests: the add/revoke balance
// of the per-slot XAML handlers (every add must be matched by a revocation
// on every removal path) and the schedule/run balance of the coalesced
// flush (a Freeze() must not turn a MarkDirty() storm into a CallAfter
// storm).
unsigned gs_slotHandlerAdds = 0;
unsigned gs_slotHandlerRevokes = 0;
unsigned gs_rootHandlerAdds = 0;
unsigned gs_rootHandlerRevokes = 0;
unsigned gs_flushSchedules = 0;
unsigned gs_flushRuns = 0;
unsigned gs_flushCallbackAttempts = 0;
unsigned gs_liveSubclassContexts = 0;
unsigned gs_impossibleZOrderOverlaps = 0;
unsigned gs_accessibilityShellProviderRetires = 0;
std::atomic<unsigned> gs_liveInvisibleShellProviders{ 0 };

// WinUI delegates can synchronously reparent a slot from host A to host B
// before application code asks to destroy B. A per-host operation depth is
// therefore insufficient: keep one GUI-thread transaction depth and a weak,
// deduplicated queue of TLWs whose Destroy() must be replayed only after every
// nested WinUI/wx event dispatch has unwound.
unsigned gs_winuiOperationDepth = 0;
std::set<wxWinUITopLevelHost *> gs_hostsPendingDelete;

// A window event can arrive through a retained XAML peer after logical host
// shutdown removed the TLW registry entry. Such a dispatch still owns a
// process-wide destruction transaction, but no host-local operation.
void * const gs_globalOnlyWindowEventCookie =
    reinterpret_cast<void *>(static_cast<uintptr_t>(1));

enum class wxWinUIDeferredTLWDestroyMode
{
    // wxTopLevelWindowMSW::Destroy() reached the WinUI hook before the common
    // delayed-destruction mechanics. One Hide() is still owed.
    CompleteBaseDestroy,

    // The TLW was already in wxPendingDelete when a WinUI transaction entered.
    // Its ordinary Destroy()/Hide() path has run; only re-arming is owed.
    RearmPendingDelete,

    // wxDialog::Destroy() was intercepted before its modal wxEVT_DESTROY
    // preamble. The event and the common delayed destruction are both owed.
    CompleteModalDestroy,

    // wxPopupWindow and wxPopupTransientWindow are top-level wx windows but
    // not wxTopLevelWindowMSW objects. Their completion paths are therefore
    // kept explicit and separately typed.
    CompletePopupImmediateDestroy,
    CompletePopupPendingDestroy
};

struct wxWinUIDeferredTLWDestroy
{
    wxWinUIDeferredTLWDestroy(
        wxWindow *target,
        wxWinUIDeferredTLWDestroyMode requestedMode)
        : window(target),
          mode(requestedMode)
    {
    }

    wxWeakRef<wxWindow> window;
    wxWinUIDeferredTLWDestroyMode mode;
};

std::vector<wxWinUIDeferredTLWDestroy> gs_deferredTLWDestroyQueue;
bool gs_deferredTLWDestroyReplayScheduled = false;

void wxWinUIBeginGlobalOperation();
void wxWinUIEndGlobalOperation();
void wxWinUIQueueDeferredTLWDestroy(
    wxWindow *window,
    wxWinUIDeferredTLWDestroyMode mode);
bool wxWinUIHasDeferredTLWDestroy(const wxWindow *window);
void wxWinUIScheduleDeferredTLWDestroyReplay();

// Cumulative count of per-slot cursor applications, for the state tests:
// proves the wxWindow::SetCursor() plumbing reaches the slot seam.
unsigned gs_slotCursorSets = 0;

// Every application-wide cursor-policy mutation advances this generation.
// Per-slot and island-level caches include it, so nested busy-cursor
// transitions and wxSetCursor() can never be hidden by an unchanged window
// identity or HCURSOR value.
unsigned long long gs_cursorPolicyGeneration = 1;

unsigned gs_failInputPointerSourceSetAt = 0;

// Registration-rollback fault injection (see TestFailHandlerAdd): countdown
// to the handler subscription that must throw, 0 = disabled.
unsigned gs_failHandlerAddAt = 0;

void wxWinUIMaybeInjectHandlerFault()
{
    if ( gs_failHandlerAddAt && --gs_failHandlerAddAt == 0 )
    {
        throw winrt::hresult_error(E_FAIL,
                                   L"wxWinUI test: injected handler fault");
    }
}

// Content-transaction fault injection (see TestFailContentSwap): a mask of
// TestContentFault_* bits; each phase consumes its own bit when it fires.
unsigned gs_failContentMask = 0;
unsigned gs_failToolTipObserverAdds = 0;
unsigned gs_failContentLoadedObserverAdds = 0;
unsigned gs_liveAutomationNameStylePropertyStates = 0;
unsigned gs_liveContentLoadedStates = 0;
std::function<void (wxWindow *)> gs_testSlotAttachedHook;
std::function<void (wxWindow *)> gs_testSlotSyncedHook;
std::function<void (wxWindow *)> gs_testContentCarrierSetHook;
std::function<void (wxWindow *)> gs_testContentTransactionStartedHook;
std::function<void ()> gs_testZOrderSetHook;
std::function<void (wxWindow *)> gs_testWindowAtPointSlotStateHook;
wxWinUITopLevelHost::InitializationHookForTest
    gs_testHostInitializationHook = nullptr;
unsigned gs_testHostInitializationFaultMask =
    wxWinUITopLevelHost::TestInitializationFault_None;
bool gs_testHostInitializationHookInProgress = false;

void wxWinUIMaybeInjectContentFault(unsigned phaseBit)
{
    if ( gs_failContentMask & phaseBit )
    {
        gs_failContentMask &= ~phaseBit;
        throw winrt::hresult_error(E_FAIL,
                                   L"wxWinUI test: injected content fault");
    }
}

void wxWinUIMaybeInjectToolTipObserverFault()
{
    if ( gs_failToolTipObserverAdds )
    {
        --gs_failToolTipObserverAdds;
        throw winrt::hresult_error(
            E_FAIL,
            L"wxWinUI test: injected tooltip observer fault");
    }
}

void wxWinUIMaybeInjectContentLoadedObserverFault()
{
    if ( gs_failContentLoadedObserverAdds )
    {
        --gs_failContentLoadedObserverAdds;
        throw winrt::hresult_error(
            E_FAIL,
            L"wxWinUI test: injected content Loaded observer fault");
    }
}

constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_BRIDGE = 1;
constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_INNER = 2;

// SetWindowSubclass() may outlive its owning wx host when removal fails.
// Never give USER32 a raw host pointer: this context can be deactivated
// independently and then either deleted after a successful removal or left
// for WM_NCDESTROY to delete. The weak lifetime makes every late callback a
// harmless DefSubclassProc() even after the wx host has gone away.
struct wxWinUISubclassContext
{
    explicit wxWinUISubclassContext(
        const std::shared_ptr<wxWinUIHostLifetime>& hostState)
        : host(hostState)
    {
        ++gs_liveSubclassContexts;
    }
    ~wxWinUISubclassContext() { --gs_liveSubclassContexts; }

    std::weak_ptr<wxWinUIHostLifetime> host;
    bool active = true;
};

class wxWinUISyntheticMouseDispatch
{
public:
    wxWinUISyntheticMouseDispatch()
    {
        wxWinUIMSWBeginSyntheticMouseDispatch();
    }

    ~wxWinUISyntheticMouseDispatch()
    {
        wxWinUIMSWEndSyntheticMouseDispatch();
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxWinUISyntheticMouseDispatch);
};

bool wxWinUIStartsSystemModalLoop(LRESULT hitTest)
{
    switch ( hitTest )
    {
        case HTCAPTION:
        case HTSYSMENU:
        case HTLEFT:
        case HTRIGHT:
        case HTTOP:
        case HTBOTTOM:
        case HTTOPLEFT:
        case HTTOPRIGHT:
        case HTBOTTOMLEFT:
        case HTBOTTOMRIGHT:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
    }

    return false;
}

wxWinUIClickSettings wxWinUIGetClickSettings()
{
    wxWinUIClickSettings settings;
    settings.maxInterval = ::GetDoubleClickTime();
    settings.rectangleWidth = ::GetSystemMetrics(SM_CXDOUBLECLK);
    settings.rectangleHeight = ::GetSystemMetrics(SM_CYDOUBLECLK);
    return settings;
}

wxWinUIInputDevice wxWinUIGetInputDevice(
    winrt::Microsoft::UI::Input::PointerDeviceType device)
{
    using Device = winrt::Microsoft::UI::Input::PointerDeviceType;
    switch ( device )
    {
        case Device::Touch:
            return wxWinUIInputDevice::Touch;
        case Device::Pen:
            return wxWinUIInputDevice::Pen;
        case Device::Mouse:
        case Device::Touchpad:
            return wxWinUIInputDevice::Mouse;
    }

    return wxWinUIInputDevice::Mouse;
}

wxWinUIInputUpdateKind wxWinUIGetInputUpdateKind(
    winrt::Microsoft::UI::Input::PointerUpdateKind update)
{
    using Update = winrt::Microsoft::UI::Input::PointerUpdateKind;
    switch ( update )
    {
        case Update::LeftButtonPressed:
            return wxWinUIInputUpdateKind::LeftPressed;
        case Update::LeftButtonReleased:
            return wxWinUIInputUpdateKind::LeftReleased;
        case Update::RightButtonPressed:
            return wxWinUIInputUpdateKind::RightPressed;
        case Update::RightButtonReleased:
            return wxWinUIInputUpdateKind::RightReleased;
        case Update::MiddleButtonPressed:
            return wxWinUIInputUpdateKind::MiddlePressed;
        case Update::MiddleButtonReleased:
            return wxWinUIInputUpdateKind::MiddleReleased;
        case Update::XButton1Pressed:
            return wxWinUIInputUpdateKind::X1Pressed;
        case Update::XButton1Released:
            return wxWinUIInputUpdateKind::X1Released;
        case Update::XButton2Pressed:
            return wxWinUIInputUpdateKind::X2Pressed;
        case Update::XButton2Released:
            return wxWinUIInputUpdateKind::X2Released;
        case Update::Other:
            break;
    }

    return wxWinUIInputUpdateKind::Other;
}

wxWinUIInputTargetKey wxWinUIGetInputTargetKey(
    const wxWinUINativeTarget& target)
{
    wxWinUIInputTargetKey key;
    key.leaf = reinterpret_cast<std::uintptr_t>(target.GetLeafHwnd());
    key.generation = target.GetLeafGeneration();
    key.owner = reinterpret_cast<std::uintptr_t>(target.GetShellHwnd());
    key.ownerGeneration = target.GetShellGeneration();
    return key;
}

WORD wxWinUIGetWParamButtonMask(wxWinUIInputButton button)
{
    switch ( button )
    {
        case wxWinUIInputButton::Left:   return MK_LBUTTON;
        case wxWinUIInputButton::Right:  return MK_RBUTTON;
        case wxWinUIInputButton::Middle: return MK_MBUTTON;
        case wxWinUIInputButton::X1:     return MK_XBUTTON1;
        case wxWinUIInputButton::X2:     return MK_XBUTTON2;
        case wxWinUIInputButton::None:   return 0;
    }

    return 0;
}

// MK_* occupies the low word used by native mouse messages. Keep the Alt
// snapshot outside it: inputstate treats modifiers as opaque and the slot
// adapter consumes this bit when constructing the wxMouseEvent, while a
// native wParam naturally truncates it away.
constexpr unsigned wxWINUI_INPUT_MOD_ALT = 0x00010000u;

wxEventType wxWinUIGetSlotMouseEventType(
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::HoverEnter )
        return wxEVT_ENTER_WINDOW;
    if ( action.action == wxWinUIInputActionKind::HoverLeave )
        return wxEVT_LEAVE_WINDOW;
    if ( action.action != wxWinUIInputActionKind::Dispatch )
        return wxEVT_NULL;

    if ( action.kind == wxWinUIInputKind::Move )
        return wxEVT_MOTION;

    const bool isDown = action.kind == wxWinUIInputKind::Press;
    const bool isUp = action.kind == wxWinUIInputKind::Release;
    const bool isDouble = action.kind == wxWinUIInputKind::DoubleClick;
    if ( !isDown && !isUp && !isDouble )
        return wxEVT_NULL;

    switch ( action.button )
    {
        case wxWinUIInputButton::Left:
            return isDown ? wxEVT_LEFT_DOWN
                          : isUp ? wxEVT_LEFT_UP : wxEVT_LEFT_DCLICK;
        case wxWinUIInputButton::Right:
            return isDown ? wxEVT_RIGHT_DOWN
                          : isUp ? wxEVT_RIGHT_UP : wxEVT_RIGHT_DCLICK;
        case wxWinUIInputButton::Middle:
            return isDown ? wxEVT_MIDDLE_DOWN
                          : isUp ? wxEVT_MIDDLE_UP : wxEVT_MIDDLE_DCLICK;
        case wxWinUIInputButton::X1:
            return isDown ? wxEVT_AUX1_DOWN
                          : isUp ? wxEVT_AUX1_UP : wxEVT_AUX1_DCLICK;
        case wxWinUIInputButton::X2:
            return isDown ? wxEVT_AUX2_DOWN
                          : isUp ? wxEVT_AUX2_UP : wxEVT_AUX2_DCLICK;
        case wxWinUIInputButton::None:
            break;
    }

    return wxEVT_NULL;
}

UINT wxWinUIGetDoubleClickMessage(UINT message)
{
    switch ( message )
    {
        case WM_LBUTTONDOWN:   return WM_LBUTTONDBLCLK;
        case WM_RBUTTONDOWN:   return WM_RBUTTONDBLCLK;
        case WM_MBUTTONDOWN:   return WM_MBUTTONDBLCLK;
        case WM_XBUTTONDOWN:   return WM_XBUTTONDBLCLK;
        case WM_NCLBUTTONDOWN: return WM_NCLBUTTONDBLCLK;
        case WM_NCRBUTTONDOWN: return WM_NCRBUTTONDBLCLK;
        case WM_NCMBUTTONDOWN: return WM_NCMBUTTONDBLCLK;
        case WM_NCXBUTTONDOWN: return WM_NCXBUTTONDBLCLK;
    }

    return message;
}

bool wxWinUIIsCompatibilityMouseEvent()
{
    constexpr LONG_PTR signature = 0xFF515700;
    constexpr LONG_PTR signatureMask = 0xFFFFFF00;
    return (::GetMessageExtraInfo() & signatureMask) == signature;
}

struct wxWinUIButtonMessages
{
    UINT client = 0;
    UINT nonClient = 0;
    WORD xButton = 0;
    int virtualKey = 0;
};

bool wxWinUIGetButtonMessages(wxWinUIInputButton button,
                              wxWinUIInputKind kind,
                              wxWinUIButtonMessages *messages)
{
    if ( !messages )
        return false;

    wxWinUIButtonMessages result;
    switch ( button )
    {
        case wxWinUIInputButton::Left:
            result.client = WM_LBUTTONDOWN;
            result.nonClient = WM_NCLBUTTONDOWN;
            result.virtualKey = VK_LBUTTON;
            break;
        case wxWinUIInputButton::Right:
            result.client = WM_RBUTTONDOWN;
            result.nonClient = WM_NCRBUTTONDOWN;
            result.virtualKey = VK_RBUTTON;
            break;
        case wxWinUIInputButton::Middle:
            result.client = WM_MBUTTONDOWN;
            result.nonClient = WM_NCMBUTTONDOWN;
            result.virtualKey = VK_MBUTTON;
            break;
        case wxWinUIInputButton::X1:
            result.client = WM_XBUTTONDOWN;
            result.nonClient = WM_NCXBUTTONDOWN;
            result.xButton = XBUTTON1;
            result.virtualKey = VK_XBUTTON1;
            break;
        case wxWinUIInputButton::X2:
            result.client = WM_XBUTTONDOWN;
            result.nonClient = WM_NCXBUTTONDOWN;
            result.xButton = XBUTTON2;
            result.virtualKey = VK_XBUTTON2;
            break;
        case wxWinUIInputButton::None:
            return false;
    }

    if ( kind == wxWinUIInputKind::Release )
    {
        ++result.client;
        ++result.nonClient;
    }
    else if ( kind == wxWinUIInputKind::DoubleClick )
    {
        result.client = wxWinUIGetDoubleClickMessage(result.client);
        result.nonClient = wxWinUIGetDoubleClickMessage(result.nonClient);
    }
    else if ( kind != wxWinUIInputKind::Press )
    {
        return false;
    }

    *messages = result;
    return true;
}

bool wxWinUIIsPointerContactSnapshotActive(
    wxWinUIInputDevice device,
    int virtualKey,
    POINTER_INPUT_TYPE pointerType,
    UINT32 pointerFlags,
    UINT32 penFlags)
{
    if ( (pointerFlags & POINTER_FLAG_UP) != 0 )
        return false;

    if ( device == wxWinUIInputDevice::Touch )
    {
        return pointerType == PT_TOUCH &&
               virtualKey == VK_LBUTTON &&
               (pointerFlags & POINTER_FLAG_INCONTACT) != 0;
    }

    if ( device != wxWinUIInputDevice::Pen ||
         pointerType != PT_PEN )
    {
        return false;
    }

    if ( virtualKey == VK_LBUTTON )
        return (pointerFlags & POINTER_FLAG_INCONTACT) != 0;
    if ( virtualKey == VK_RBUTTON )
        return (penFlags & PEN_FLAG_BARREL) != 0;
    return false;
}

bool wxWinUIIsPointerContactActive(wxWinUIInputDevice device,
                                   std::uint32_t pointerId,
                                   int virtualKey)
{
    if ( device == wxWinUIInputDevice::Mouse )
    {
        return virtualKey &&
               (::GetAsyncKeyState(virtualKey) & 0x8000) != 0;
    }

    if ( !pointerId )
        return false;

    if ( device == wxWinUIInputDevice::Touch )
    {
        POINTER_INFO info = {};
        return ::GetPointerInfo(pointerId, &info) &&
               wxWinUIIsPointerContactSnapshotActive(
                   device, virtualKey, info.pointerType,
                   info.pointerFlags, 0);
    }

    if ( device == wxWinUIInputDevice::Pen )
    {
        POINTER_PEN_INFO info = {};
        return ::GetPointerPenInfo(pointerId, &info) &&
               wxWinUIIsPointerContactSnapshotActive(
                   device, virtualKey, info.pointerInfo.pointerType,
                   info.pointerInfo.pointerFlags, info.penFlags);
    }

    return false;
}

void wxWinUIRetireSubclass(HWND hwnd,
                           SUBCLASSPROC proc,
                           UINT_PTR subclassId,
                           void *&opaqueContext,
                           const char *description)
{
    wxWinUISubclassContext * const context =
        static_cast<wxWinUISubclassContext *>(opaqueContext);
    if ( !context )
        return;

    context->active = false;

    DWORD_PTR installedRefData = 0;
    const bool installed =
        hwnd && ::IsWindow(hwnd) &&
        ::GetWindowSubclass(hwnd, proc, subclassId, &installedRefData) &&
        installedRefData == reinterpret_cast<DWORD_PTR>(context);

    if ( !installed || ::RemoveWindowSubclass(hwnd, proc, subclassId) )
    {
        delete context;
    }
    else
    {
        // Keep the inactive context alive: USER32 still owns its pointer and
        // WM_NCDESTROY will reclaim it. Dropping it here would turn a benign
        // API failure into a deterministic use-after-free.
        wxLogWarning("wxWinUI: failed to remove the %s window subclass "
                     "(the API exposes no extended error); it was safely "
                     "deactivated", description);
    }

    // On removal failure ownership has transferred to WM_NCDESTROY.
    opaqueContext = nullptr;
}

void wxWinUITransferSubclassToHwnd(void *&opaqueContext)
{
    wxWinUISubclassContext * const context =
        static_cast<wxWinUISubclassContext *>(opaqueContext);
    if ( context )
        context->active = false;

    // The HWND now exclusively owns this inactive allocation. Its
    // WM_NCDESTROY path removes the subclass and deletes the context; keeping
    // a second raw owner in a host finalized after TLW destruction would be a
    // deterministic UAF/double-delete.
    opaqueContext = nullptr;
}

class wxWinUIOwnedRegion
{
public:
    explicit wxWinUIOwnedRegion(HRGN region = nullptr) : m_region(region) {}
    ~wxWinUIOwnedRegion()
    {
        if ( m_region )
            ::DeleteObject(m_region);
    }

    explicit operator bool() const { return m_region != nullptr; }
    HRGN Get() const { return m_region; }
    HRGN Release()
    {
        const HRGN region = m_region;
        m_region = nullptr;
        return region;
    }

private:
    HRGN m_region;

    wxDECLARE_NO_COPY_CLASS(wxWinUIOwnedRegion);
};

// The slot containers are Grids extended with cursor support: ProtectedCursor
// is only reachable from a derived element, and it is how a per-window
// wxWindow::SetCursor() (e.g. a wait cursor on one control) is reflected over
// the island, which otherwise insists on its own arrow.
struct wxWinUISlotGrid
    : winrt::Microsoft::UI::Xaml::Controls::GridT<wxWinUISlotGrid>
{
    bool SetWindowCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            // A null cursor means "use the default arrow".
            ProtectedCursor(cursor);
            return true;
        }
        catch ( const winrt::hresult_error& )
        {
            // Not being able to set the cursor is never fatal.
            return false;
        }
    }

    winrt::Microsoft::UI::Input::InputCursor GetWindowCursor() const
    {
        try
        {
            return ProtectedCursor();
        }
        catch ( const winrt::hresult_error& )
        {
            return nullptr;
        }
    }
};

// The implementation object of each slot's container, for cursor access.
std::map<const wxWinUISlot *, winrt::com_ptr<wxWinUISlotGrid>> gs_slotGrids;

// Root canvas subclass with cursor access, so the island can mirror the
// native cursor of whatever wx window the pointer hovers.
struct wxWinUIRootCanvas
    : winrt::Microsoft::UI::Xaml::Controls::CanvasT<wxWinUIRootCanvas>
{
    bool SetPointerCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            ProtectedCursor(cursor);
            return true;
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
    }
};

// The implementation object of each host's root canvas.
std::map<const wxWinUITopLevelHost *,
         winrt::com_ptr<wxWinUIRootCanvas>> gs_rootCanvases;

// Derive the UIA name wx may contribute when the XAML application didn't
// author one. wxWindow::GetName() is a technical/resource identifier (used by
// XRC, persistence and FindWindowByName), not the accessible name: the common
// wxWindowAccessible contract exposes only the stripped label/title. Keeping
// the same rule here also avoids leaking defaults such as "button" or
// "activityindicator" into narration.
wxString wxWinUIGetAutomationName(wxWindow *window)
{
    if ( !window )
        return wxString();

    return wxStripMenuCodes(window->GetLabel(), wxStrip_Mnemonics);
}

// Read whether a Style (or any BasedOn ancestor) authors
// AutomationProperties.Name. ReadLocalValue() cannot see Style setters, and an
// empty setter is otherwise indistinguishable from the dependency property's
// empty default. A malformed/cyclic chain is treated conservatively as
// authored: accessibility state must never be overwritten because inspection
// could not prove it was absent.
bool wxWinUIStyleChainAuthorsAutomationName(
    winrt::Microsoft::UI::Xaml::Style style)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

    std::vector<MUX::Style> visited;
    while ( style )
    {
        if ( std::find(visited.begin(), visited.end(), style) !=
                visited.end() )
        {
            return true;
        }
        visited.push_back(style);

        for ( const auto& setterBase : style.Setters() )
        {
            const auto setter = setterBase.try_as<MUX::Setter>();
            if ( setter &&
                 setter.Property() == MUXA::AutomationProperties::NameProperty() )
            {
                return true;
            }
        }

        style = style.BasedOn();
    }

    return false;
}

struct wxWinUIImplicitStyleLookup
{
    bool found = false;
    bool authorsAutomationName = false;
};

// Read only the entry owned by this dictionary. HasKey(), unlike Lookup(),
// doesn't walk MergedDictionaries, which lets the caller reproduce XAML's
// reverse merged-dictionary precedence and omit wx-installed framework
// dictionaries from the Application scope.
wxWinUIImplicitStyleLookup wxWinUIReadImplicitStyleEntry(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key)
{
    if ( !resources )
        return {};

    try
    {
        if ( !resources.HasKey(key) )
            return {};

        const auto value = resources.Lookup(key);
        // ResourceDictionary lookup is permitted to report a miss as null
        // instead of E_BOUNDS. A valid implicit Style can never be null.
        if ( !value )
            return {};

        const auto style =
            value.try_as<winrt::Microsoft::UI::Xaml::Style>();
        if ( !style )
        {
            // A resource occupying the implicit-style key but having the
            // wrong type is malformed application state. Fail closed for UIA
            // ownership instead of publishing narration over it.
            return { true, true };
        }

        return { true,
                 wxWinUIStyleChainAuthorsAutomationName(style) };
    }
    catch ( const winrt::hresult_out_of_bounds& )
    {
        return {};
    }
    catch ( const winrt::hresult_error& e )
    {
        // A deferred resource may fail while being materialized. Accessibility
        // ownership is fail-closed: log it and don't publish a wx fallback
        // over application state which couldn't be inspected.
        wxWinUILogException("resolve implicit automation Style", e);
        return { true, true };
    }
}

bool wxWinUIIsFrameworkResourceDictionary(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    if ( !resources )
        return false;

    try
    {
        if ( resources.try_as<MUXC::XamlControlsResources>() )
            return true;

        // wxWinUIEnsureApplicationResources() has a loose-XAML fallback for
        // machines where XamlControlsResources construction fails. Those
        // dictionaries are ordinary ResourceDictionary instances, but their
        // source still identifies the WinUI framework payload unambiguously.
        const auto source = resources.Source();
        if ( !source )
            return false;

        const wxString uri(source.AbsoluteUri().c_str());
        return uri.Lower().Find("/microsoft.ui.xaml/themes/") != wxNOT_FOUND;
    }
    catch ( const winrt::hresult_error& )
    {
        // Unknown provenance is application-owned for accessibility purposes:
        // fail closed instead of skipping a dictionary we couldn't identify.
        return false;
    }
}

wxWinUIImplicitStyleLookup wxWinUILookupImplicitStyleImpl(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key,
    bool skipFrameworkResources,
    std::vector<winrt::Microsoft::UI::Xaml::ResourceDictionary>& stack)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    if ( !resources )
        return {};

    if ( std::find(stack.begin(), stack.end(), resources) != stack.end() )
    {
        // A merged-dictionary cycle is malformed and makes ownership
        // unknowable. Preserve application accessibility state.
        return { true, true };
    }

    stack.push_back(resources);
    wxScopeGuard popStack = wxMakeGuard([&stack]()
    {
        stack.pop_back();
    });
    wxUnusedVar(popStack);

    const wxWinUIImplicitStyleLookup local =
        wxWinUIReadImplicitStyleEntry(resources, key);
    if ( local.found )
        return local;

    try
    {
        const auto merged = resources.MergedDictionaries();
        for ( uint32_t i = merged.Size(); i > 0; --i )
        {
            const MUX::ResourceDictionary dictionary = merged.GetAt(i - 1);
            if ( skipFrameworkResources &&
                 wxWinUIIsFrameworkResourceDictionary(dictionary) )
            {
                continue;
            }

            const wxWinUIImplicitStyleLookup nested =
                wxWinUILookupImplicitStyleImpl(
                    dictionary,
                    key,
                    skipFrameworkResources,
                    stack);
            if ( nested.found )
                return nested;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("walk implicit automation Style resources", e);
        return { true, true };
    }

    return {};
}

// WinUI's implicit-style resolver uses the element's TypeName as the resource
// key. Returning "found" independently from the Style result is important:
// the nearest implicit Style wins even if it doesn't set Name, so a farther
// application resource must not be inspected in that case.
wxWinUIImplicitStyleLookup wxWinUILookupImplicitStyle(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key,
    bool skipFrameworkResources = false)
{
    std::vector<winrt::Microsoft::UI::Xaml::ResourceDictionary> stack;
    return wxWinUILookupImplicitStyleImpl(
        resources, key, skipFrameworkResources, stack);
}

// FrameworkElement::Style() deliberately exposes only an explicitly assigned
// Style. Implicit styles have to be resolved through the same resource scopes
// as XAML: the element/parent chain first, then application resources. This
// matters especially for an authored empty Name, whose effective value is
// otherwise identical to the dependency property's default.
bool wxWinUIStyleAuthorsAutomationName(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    bool inspectImplicitStyle,
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& slotResources =
        nullptr)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    auto framework = content.try_as<MUX::FrameworkElement>();
    if ( !framework )
        return false;

    if ( const MUX::Style explicitStyle = framework.Style() )
    {
        return wxWinUIStyleChainAuthorsAutomationName(explicitStyle);
    }

    const auto localStyle =
        framework.ReadLocalValue(MUX::FrameworkElement::StyleProperty());
    if ( localStyle != MUX::DependencyProperty::UnsetValue() )
    {
        // Style={x:Null} is a deliberate opt-out from implicit styling. A
        // non-null local value with a null effective Style is inconsistent,
        // so keep ownership fail-closed in that case.
        return localStyle != nullptr;
    }

    // ResourceDictionary mutations don't necessarily invalidate an implicit
    // Style already resolved by WinUI. Native peers therefore inspect only at
    // creation and known re-resolution boundaries. Arbitrary wxWinUIXamlHost
    // content can also be re-inspected conservatively during an ordinary dirty
    // pass: application-owned XAML may have performed a silent x:Null ->
    // UnsetValue transition, and preserving application accessibility state is
    // safer than recreating a wx local Name over it.
    if ( !inspectImplicitStyle )
        return false;

    const winrt::Windows::UI::Xaml::Interop::TypeName contentType
    {
        winrt::get_class_name(content),
        winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
    };
    const auto implicitStyleKey =
        winrt::box_value(contentType);
    MUX::DependencyObject current = framework;
    std::vector<MUX::DependencyObject> visited;
    while ( current )
    {
        if ( std::find(visited.begin(), visited.end(), current) !=
                visited.end() )
        {
            return true;
        }
        visited.push_back(current);

        if ( const auto currentFramework =
                 current.try_as<MUX::FrameworkElement>() )
        {
            const wxWinUIImplicitStyleLookup lookup =
                wxWinUILookupImplicitStyle(
                    currentFramework.Resources(), implicitStyleKey);
            if ( lookup.found )
                return lookup.authorsAutomationName;
        }

        current = MUX::Media::VisualTreeHelper::GetParent(current);
    }

    // ContentPresenter can keep an arbitrary content root outside the visual
    // parent chain exposed by VisualTreeHelper. The slot container is still
    // an authoritative resource scope, so inspect its dictionary explicitly
    // before falling back to application resources.
    if ( slotResources )
    {
        const wxWinUIImplicitStyleLookup lookup =
            wxWinUILookupImplicitStyle(slotResources, implicitStyleKey);
        if ( lookup.found )
            return lookup.authorsAutomationName;
    }

    const MUX::Application application = MUX::Application::Current();
    if ( application )
    {
        const wxWinUIImplicitStyleLookup lookup =
            wxWinUILookupImplicitStyle(
                application.Resources(), implicitStyleKey, true);
        if ( lookup.found )
            return lookup.authorsAutomationName;
    }

    return false;
}

bool wxWinUITryGetLocalStringValue(
    const winrt::Windows::Foundation::IInspectable& value,
    wxString *result)
{
    if ( !value || !result )
        return false;

    const auto propertyValue =
        value.try_as<winrt::Windows::Foundation::IPropertyValue>();
    if ( !propertyValue ||
         propertyValue.Type() !=
             winrt::Windows::Foundation::PropertyType::String )
    {
        return false;
    }

    *result = wxString(propertyValue.GetString().c_str());
    return true;
}

bool wxWinUIHasBinding(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Microsoft::UI::Xaml::DependencyProperty& property)
{
    const auto framework =
        content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    return framework && framework.GetBindingExpression(property);
}

#if wxUSE_TOOLTIPS
winrt::Windows::Foundation::IInspectable
wxWinUIReadToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content)
{
    return content.ReadLocalValue(
        winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
            ToolTipProperty());
}

void wxWinUIRestoreToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Windows::Foundation::IInspectable& localValue)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const auto property = MUXC::ToolTipService::ToolTipProperty();
    if ( localValue == MUX::DependencyProperty::UnsetValue() )
        content.ClearValue(property);
    else
        content.SetValue(property, localValue);
}
#endif

// Is the CURRENT XAML focus inside this container's subtree?  Used as the
// stale-guard of the focus arbiter: Got/LostFocus events can arrive one
// dispatch generation late, after the focus was already moved elsewhere,
// and must not overwrite the ownership the newer move established.
bool wxWinUIFocusIsInside(
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
    const winrt::Microsoft::UI::Xaml::XamlRoot& root)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    try
    {
        if ( !container || !root )
            return false;

        const auto containerDO = container.as<MUX::DependencyObject>();
        auto node = MUX::Input::FocusManager::GetFocusedElement(root)
                        .try_as<MUX::DependencyObject>();
        while ( node )
        {
            if ( node == containerDO )
                return true;
            node = MUX::Media::VisualTreeHelper::GetParent(node);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    return false;
}

// Is element part of root's current visual subtree?  Preferred focus targets
// are accepted and used only while this remains true: a failed content swap
// must not leave a weak reference to a detached replacement tree.
bool wxWinUIElementIsInSubtree(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& root)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    try
    {
        if ( !element || !root )
            return false;

        const auto rootDO = root.as<MUX::DependencyObject>();
        auto node = element.as<MUX::DependencyObject>();
        while ( node )
        {
            if ( node == rootDO )
                return true;

            // Parent() covers the logical tree once WinUI has materialized
            // it; templated/visual-only descendants require
            // VisualTreeHelper instead.
            MUX::DependencyObject parent{ nullptr };
            if ( const auto framework =
                     node.try_as<MUX::FrameworkElement>() )
            {
                parent = framework.Parent();
            }
            if ( !parent )
                parent = MUX::Media::VisualTreeHelper::GetParent(node);
            node = parent;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // A candidate assembled in memory has no parent chain until it is placed
    // below a live XamlRoot: FrameworkElement::Parent() and
    // VisualTreeHelper::GetParent() both return null for e.g. a TextBlock
    // already present in Border::Child. Walk the logical content collections
    // downwards as the detached-tree fallback. These are the primitive
    // composition shapes accepted by the shared host (Panel, Border and
    // ContentControl); once realized, the upward path above remains the
    // authoritative route for template descendants.
    try
    {
        std::function<bool (const MUX::UIElement&, unsigned)> contains;
        contains =
            [&](const MUX::UIElement& candidate, unsigned depth)
            {
                if ( !candidate || depth > 256 )
                    return false;
                if ( candidate == element )
                    return true;

                if ( const auto panel = candidate.try_as<MUXC::Panel>() )
                {
                    const auto children = panel.Children();
                    for ( uint32_t i = 0; i < children.Size(); ++i )
                    {
                        if ( contains(children.GetAt(i), depth + 1) )
                            return true;
                    }
                }
                else if ( const auto border =
                              candidate.try_as<MUXC::Border>() )
                {
                    if ( contains(border.Child(), depth + 1) )
                        return true;
                }
                else if ( const auto contentControl =
                              candidate.try_as<MUXC::ContentControl>() )
                {
                    if ( contains(
                             contentControl.Content().try_as<MUX::UIElement>(),
                             depth + 1) )
                    {
                        return true;
                    }
                }

                return false;
            };

        return contains(root, 0);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

// The visual root and the element carrying the logical wx/UIA identity are a
// single generation.  Both may be null, otherwise the semantic element must
// still belong to the root's current visual subtree.  Keep this check in one
// place because every XAML attachment is a re-entrant boundary: Loaded or an
// application callback can detach a descendant after the caller's initial
// validation but before the slot publishes the pair.
bool wxWinUISemanticPairIsValid(
    const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget)
{
    if ( static_cast<bool>(visualRoot) !=
             static_cast<bool>(semanticTarget) )
    {
        return false;
    }

    return !visualRoot ||
           wxWinUIElementIsInSubtree(semanticTarget, visualRoot);
}

// Preserve the distinction between stock system cursors and application-
// created bitmap cursors. The WinAppSDK interop factory copies an HCURSOR into
// an InputCustomCursor even when it denotes a stock cursor, so recognize the
// latter first and keep the factory as the custom-cursor fallback. A null
// HCURSOR is the valid XAML "default cursor"; false is reserved for a real
// conversion failure so the caller can leave its cache uncommitted and retry.
bool wxWinUICursorFromHCURSOR(
    HCURSOR hcursor,
    winrt::Microsoft::UI::Input::InputCursor& result)
{
    namespace MUI = winrt::Microsoft::UI::Input;

    result = nullptr;
    if ( !hcursor )
        return true;

    try
    {
        static const struct
        {
            const wchar_t *id;
            MUI::InputSystemCursorShape shape;
        } stockCursors[] =
        {
            { IDC_ARROW,       MUI::InputSystemCursorShape::Arrow },
            { IDC_IBEAM,       MUI::InputSystemCursorShape::IBeam },
            { IDC_WAIT,        MUI::InputSystemCursorShape::Wait },
            { IDC_APPSTARTING, MUI::InputSystemCursorShape::AppStarting },
            { IDC_HAND,        MUI::InputSystemCursorShape::Hand },
            { IDC_CROSS,       MUI::InputSystemCursorShape::Cross },
            { IDC_SIZEALL,     MUI::InputSystemCursorShape::SizeAll },
            { IDC_SIZENWSE,
              MUI::InputSystemCursorShape::SizeNorthwestSoutheast },
            { IDC_SIZENESW,
              MUI::InputSystemCursorShape::SizeNortheastSouthwest },
            { IDC_SIZEWE,      MUI::InputSystemCursorShape::SizeWestEast },
            { IDC_SIZENS,      MUI::InputSystemCursorShape::SizeNorthSouth },
            { IDC_NO,          MUI::InputSystemCursorShape::UniversalNo },
            { IDC_HELP,        MUI::InputSystemCursorShape::Help },
            { IDC_UPARROW,     MUI::InputSystemCursorShape::UpArrow },
        };

        for ( const auto& stockCursor : stockCursors )
        {
            if ( hcursor == ::LoadCursorW(nullptr, stockCursor.id) )
            {
                result = MUI::InputSystemCursor::Create(stockCursor.shape);
                return result != nullptr;
            }
        }

        const auto factory =
            winrt::get_activation_factory<MUI::InputCursor>();
        winrt::com_ptr<IWxInputCursorStaticsInterop> interop;
        winrt::check_hresult(
            winrt::get_unknown(factory)->QueryInterface(
                __uuidof(IWxInputCursorStaticsInterop),
                interop.put_void()));
        winrt::check_hresult(interop->CreateFromHCursor(
            hcursor,
            reinterpret_cast<winrt::impl::inspectable_abi **>(
                winrt::put_abi(result))));
        return result != nullptr;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("InputCursor::CreateFromHCursor", e);
        result = nullptr;
        return false;
    }
}

bool wxWinUIShouldEmitSetCursor(wxWinUIInputKind kind)
{
    // WM_SETCURSOR is a movement/hit-test protocol. Button, wheel and
    // capture transitions at the current pointer point re-use its verdict.
    return kind == wxWinUIInputKind::Move;
}

} // anonymous namespace

struct wxWinUITopLevelHost::NativeResizeRequest
{
    wxWeakRef<wxWindow> source;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> grip;
    wxWinUINativeTarget target;
    wxWinUIPointerSample sample;
    std::function<bool ()> isCurrent;
    std::uint64_t ticket = 0;
    std::uint32_t inputSiteGeneration = 0;
    int hitTest = HTNOWHERE;
    void *subclassContext = nullptr;
    bool preparing = false;
    bool accepted = false;
    bool dispatching = false;
    bool entered = false;
    bool exited = false;
    bool cancelled = false;
};

void wxWinUISetNativeResizeContactReaderForTesting(
    bool (*reader)(void *), void *context)
{
    gs_nativeResizeContactReader = reader;
    gs_nativeResizeContactContext = reader ? context : nullptr;
}

void wxWinUIFailNextNativeResizePostForTesting()
{
    gs_failNextNativeResizePost = true;
}

bool wxWinUIGetNativeResizeSnapshotForTesting(
    wxWindow *window, wxWinUINativeResizeSnapshot *snapshot)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, false);
    if ( !host || !snapshot )
        return false;
    *snapshot = host->m_nativeResizeSnapshot;
    return true;
}

// ----------------------------------------------------------------------------
// Optimisation kill switch: set WX_WINUI_DISABLE to a comma separated list of
// "paint", "coalesce", "hittest", "cursor" to take the matching pointer-path
// optimisation out of the picture at startup.  Each of them trades an
// invariant of the naive implementation for speed, so when a rendering or
// input problem appears this is how the responsible one is identified without
// rebuilding.  Unset -- the normal case -- everything is enabled.
// ----------------------------------------------------------------------------

bool wxWinUIOptimisationDisabled(const char *name)
{
    static const std::string s_disabled = []() -> std::string
    {
        const DWORD required =
            ::GetEnvironmentVariableA("WX_WINUI_DISABLE", nullptr, 0);
        if ( !required )
            return std::string();

        std::vector<char> value(required);
        const DWORD copied =
            ::GetEnvironmentVariableA("WX_WINUI_DISABLE",
                                      value.data(), required);
        if ( !copied || copied >= required )
            return std::string();

        return std::string(value.data());
    }();

    if ( s_disabled.empty() )
        return false;

    return s_disabled.find(name) != std::string::npos;
}


// ============================================================================
// wxWinUISlot
// ============================================================================

class wxWinUIPhysicalDisconnectGateForTesting final :
    public wxWinUIPhysicalDisconnectGate
{
public:
    wxWinUIPhysicalDisconnectGateForTesting(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        bool satisfied,
        bool degraded)
        : m_queue(queue), m_satisfied(satisfied), m_degraded(degraded)
    {
    }

    bool IsSatisfied() const noexcept override
        { return m_satisfied && !m_degraded && !m_sealed; }
    bool IsDegraded() const noexcept override { return m_degraded; }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_sealed; }
    winrt::Microsoft::UI::Dispatching::DispatcherQueue
    GetDispatcherQueue() const noexcept override { return m_queue; }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        if ( m_terminal || !complete )
            return false;
        try
        {
            m_completion = std::move(complete);
            InvokeIfReady();
            return true;
        }
        catch ( ... )
        {
            return false;
        }
    }

    void SealForHostShutdown() noexcept override
    {
        m_hadDebtAtSeal = !m_satisfied || m_degraded;
        m_sealed = true;
    }

    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        if ( !m_openBeforeSourceClose )
            return;
        m_openBeforeSourceClose = false;
        m_satisfied = false;
        m_degraded = true;
        wxWinUINotePhysicalDisconnectGateDegraded();
    }

    bool CompleteSealedWithoutPopup() noexcept override
    {
        if ( m_sealedNoDebtCompleted )
            return true;
        if ( m_openAtSealedCompletion )
        {
            m_openAtSealedCompletion = false;
            m_satisfied = false;
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return false;
        }
        if ( !m_sealed || m_hadDebtAtSeal || m_degraded || !m_satisfied )
            return false;
        m_sealedNoDebtCompleted = true;
        InvokeCompletionEvenIfSealed();
        return true;
    }

    void CompleteAtXamlBoundary() noexcept override
    {
        m_terminal = true;
        m_completion = {};
        m_degraded = false;
        m_satisfied = true;
        m_queue = nullptr;
    }

    void SetState(bool satisfied, bool degraded) noexcept
    {
        if ( m_sealed && !satisfied )
            degraded = true;
        m_satisfied = satisfied;
        m_degraded = degraded;
        if ( degraded )
            wxWinUINotePhysicalDisconnectGateDegraded();
        if ( m_sealed && satisfied && !degraded )
            InvokeCompletionEvenIfSealed();
        else
            InvokeIfReady();
    }

    void OpenAtSealedCompletion() noexcept
    {
        m_openAtSealedCompletion = true;
    }

    void OpenBeforeSourceClose() noexcept
    {
        m_openBeforeSourceClose = true;
    }

private:
    void InvokeIfReady() noexcept
    {
        if ( !IsSatisfied() || !m_completion )
            return;
        InvokeCompletionEvenIfSealed();
    }

    void InvokeCompletionEvenIfSealed() noexcept
    {
        if ( !m_satisfied || m_degraded || !m_completion )
            return;
        std::function<void ()> completion = std::move(m_completion);
        try
        {
            completion();
        }
        catch ( ... )
        {
            m_degraded = true;
            m_satisfied = false;
            wxWinUINotePhysicalDisconnectGateDegraded();
        }
    }

    winrt::Microsoft::UI::Dispatching::DispatcherQueue m_queue{ nullptr };
    std::function<void ()> m_completion;
    bool m_satisfied = true;
    bool m_degraded = false;
    bool m_terminal = false;
    bool m_sealed = false;
    bool m_hadDebtAtSeal = false;
    bool m_sealedNoDebtCompleted = false;
    bool m_openAtSealedCompletion = false;
    bool m_openBeforeSourceClose = false;
};

class wxWinUIToolTipPropertyState
{
public:
    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    bool active = true;
    bool inCallback = false;
};

class wxWinUIAutomationNameStylePropertyState
{
public:
    wxWinUIAutomationNameStylePropertyState()
    {
        ++gs_liveAutomationNameStylePropertyStates;
    }

    ~wxWinUIAutomationNameStylePropertyState()
    {
        wxASSERT(gs_liveAutomationNameStylePropertyStates > 0);
        --gs_liveAutomationNameStylePropertyStates;
    }

    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    winrt::Windows::Foundation::IInspectable localStyleSource{ nullptr };
    bool active = true;
    bool inCallback = false;
    bool localStyleSourceInitialized = false;
    bool localStyleSourceIsSet = false;
};

class wxWinUIContentLoadedState
{
public:
    wxWinUIContentLoadedState() { ++gs_liveContentLoadedStates; }
    ~wxWinUIContentLoadedState() { --gs_liveContentLoadedStates; }

    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> content;
    unsigned long long contentGeneration = 0;
    bool active = true;
};

static bool wxWinUIUpdateLocalStyleSourceSnapshot(
    const std::shared_ptr<wxWinUIAutomationNameStylePropertyState>& state,
    const winrt::Microsoft::UI::Xaml::FrameworkElement& framework)
{
    const auto localStyle =
        framework.ReadLocalValue(
            winrt::Microsoft::UI::Xaml::FrameworkElement::StyleProperty());
    const bool isSet =
        localStyle !=
            winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue();
    const bool changed =
        state->localStyleSourceInitialized &&
        (state->localStyleSourceIsSet != isSet ||
         (isSet && state->localStyleSource != localStyle));
    state->localStyleSource = isSet ? localStyle : nullptr;
    state->localStyleSourceIsSet = isSet;
    state->localStyleSourceInitialized = true;
    return changed;
}

wxWinUISlot::wxWinUISlot()
    : m_inputState(wxWinUIGetClickSettings())
{
}

void wxWinUISlot::RevokeContentLoadedObserver()
{
    if ( m_contentLoadedState )
        m_contentLoadedState->active = false;

    if ( m_contentLoadedToken && m_contentLoadedObservedElement )
    {
        try
        {
            m_contentLoadedObservedElement.Loaded(m_contentLoadedToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    m_contentLoadedObservedElement = nullptr;
    m_contentLoadedToken = {};
    m_contentLoadedState.reset();
    m_contentLoadedObserverPending = false;
    m_contentLoadedObserverRetryFailures = 0;
    m_contentLoadedDuringTransaction = false;
    m_contentLoadedTransactionEpoch = 0;
    m_contentLoadedTransactionGeneration = 0;
}

bool wxWinUISlot::InstallContentLoadedObserver()
{
    constexpr unsigned AutomaticRetryLimit = 3;

    const unsigned previousRetryFailures =
        m_contentLoadedObserverRetryFailures;
    RevokeContentLoadedObserver();
    m_contentLoadedObserverRetryFailures = previousRetryFailures;

    wxWinUITopLevelHost * const host =
        m_lifetime ? m_lifetime->GetHost() : nullptr;
    if ( !host || m_deletePending || m_poisoned )
        return false;
    if ( !m_content )
    {
        m_contentLoadedObserverRetryFailures = 0;
        return true;
    }

    const auto framework =
        m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if ( !framework )
    {
        // UIElement has no Loaded event of its own. Once its carrier commit is
        // published there is no later template boundary to await.
        m_contentLoaded = true;
        m_contentLoadedObserverRetryFailures = 0;
        host->NotifySlotContentLoaded(
            m_window, m_content, m_contentGeneration);
        return m_lifetime && m_lifetime->GetHost() == host &&
               !m_deletePending && !m_poisoned;
    }

    const auto expectedLifetime = m_lifetime;
    const auto expectedContent = m_content;
    const unsigned long long expectedGeneration = m_contentGeneration;
    const unsigned long long expectedTransactionEpoch =
        m_contentTransactionEpoch;
    const auto operationIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime == expectedLifetime &&
               expectedLifetime &&
               expectedLifetime->GetHost() == host &&
               m_content == expectedContent &&
               m_contentGeneration == expectedGeneration &&
               m_contentTransactionEpoch == expectedTransactionEpoch &&
               !m_contentTransactionInProgress;
    };
    const auto state = std::make_shared<wxWinUIContentLoadedState>();
    state->lifetime = expectedLifetime;
    state->content = winrt::make_weak(expectedContent);
    state->contentGeneration = expectedGeneration;

    winrt::event_token token{};
    try
    {
        wxWinUIMaybeInjectContentLoadedObserverFault();
        token = framework.Loaded(
            [state](const winrt::Windows::Foundation::IInspectable&,
                    const winrt::Microsoft::UI::Xaml::RoutedEventArgs&)
            {
                if ( !state->active )
                    return;

                const auto lifetime = state->lifetime.lock();
                wxWinUITopLevelHost * const currentHost =
                    lifetime ? lifetime->GetHost() : nullptr;
                wxWindow * const window =
                    lifetime ? lifetime->GetWindow() : nullptr;
                wxWinUISlot * const slot =
                    currentHost && window
                        ? currentHost->FindSlot(window)
                        : nullptr;
                if ( slot )
                    slot->HandleContentLoadedObserver(state);
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot Loaded registration", e);
        state->active = false;
        if ( operationIsCurrent() )
        {
            m_contentLoadedObserverPending = true;
            m_contentLoadedObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_contentLoadedObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueContentLoadedObserverRetry();
            }
        }
        return false;
    }

    if ( !operationIsCurrent() )
    {
        state->active = false;
        try
        {
            framework.Loaded(token);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        return false;
    }

    m_contentLoadedObservedElement = framework;
    m_contentLoadedToken = token;
    m_contentLoadedState = state;
    m_contentLoadedObserverPending = false;

    try
    {
        if ( framework.IsLoaded() )
            HandleContentLoadedObserver(state);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot IsLoaded query", e);
        if ( operationIsCurrent() && m_contentLoadedState == state )
        {
            m_contentLoadedObserverPending = true;
            m_contentLoadedObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_contentLoadedObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueContentLoadedObserverRetry();
            }
        }
        return false;
    }

    if ( !operationIsCurrent() || m_contentLoadedState != state )
        return false;

    m_contentLoadedObserverRetryFailures = 0;
    return true;
}

void wxWinUISlot::QueueContentLoadedObserverRetry()
{
    // MarkDirty()/CallAfter is already coalesced by the owning host. The
    // counter maintained by InstallContentLoadedObserver() keeps this retry
    // finite; an unrelated future dirty pass may still recover a persistent
    // registration failure without an autonomous callback storm.
    if ( m_contentLoadedObserverPending &&
         !m_deletePending &&
         !m_poisoned &&
         m_content &&
         m_lifetime &&
         m_lifetime->GetHost() )
    {
        NudgeDirty();
    }
}

void wxWinUISlot::HandleContentLoadedObserver(
    const std::shared_ptr<wxWinUIContentLoadedState>& state)
{
    if ( !state || !state->active || m_contentLoadedState != state )
        return;

    const auto lifetime = state->lifetime.lock();
    const auto content = state->content.get();
    wxWinUITopLevelHost * const host =
        lifetime ? lifetime->GetHost() : nullptr;
    wxWindow * const window = lifetime ? lifetime->GetWindow() : nullptr;
    if ( !host || !window || !content ||
         m_lifetime != lifetime ||
         host->FindSlot(window) != this ||
         m_content != content ||
         m_contentGeneration != state->contentGeneration )
    {
        return;
    }

    OperationGuard slotOperation(this);
    host->NotifySlotContentLoaded(
        window, content, state->contentGeneration);
}

void wxWinUISlot::TestInvokeContentLoadedObserver()
{
    HandleContentLoadedObserver(m_contentLoadedState);
}

wxWinUISlot::OperationGuard::OperationGuard(wxWinUISlot *slot)
    : m_slot(slot)
{
    if ( m_slot )
    {
        ++m_slot->m_operationDepth;
        wxWinUIBeginGlobalOperation();
    }
}

wxWinUISlot::OperationGuard::~OperationGuard()
{
    if ( m_slot )
        m_slot->EndOperation();
}

void wxWinUISlot::EndOperation()
{
    wxASSERT_MSG(m_operationDepth > 0, "unbalanced WinUI slot operation");
    wxASSERT_MSG(gs_winuiOperationDepth > 0,
                 "unbalanced global WinUI operation");
    if ( m_operationDepth == 0 || gs_winuiOperationDepth == 0 )
        return;

    --m_operationDepth;
    bool deleteNow = false;
    if ( m_operationDepth == 0 )
    {
        if ( m_disconnectPending )
        {
            if ( m_deferredDisconnectHost )
                m_deferredDisconnectHost->ScheduleDeferredSlotDisconnects();
        }
        else
        {
            deleteNow = m_deletePending;
        }
    }

    // Keep the global transaction active through the disconnect scheduling
    // above: it can cross a wx/XAML callback boundary of its own. The common
    // tail also re-arms any physical host deletion whose CallAfter observed
    // this slot operation during a nested wxYield().
    wxWinUIEndGlobalOperation();

    if ( deleteNow )
        delete this;
}

void wxWinUISlot::RetireExposedAccessibilityShellProvider(bool terminal)
{
    if ( m_accessibilityShellRetirementTerminal )
        return;

    // Publish terminal state before the external COM boundary. Logical slot
    // retirement can be followed by a delayed physical disconnect after its
    // HWND value has been recycled; that later phase must never purge the new
    // native lifetime. Non-terminal content/authority changes remain
    // repeatable while this slot is live.
    if ( terminal )
        m_accessibilityShellRetirementTerminal = true;

    wxWinUITLWHostRetireAccessibilityShellProvider(m_hwnd);
}

void wxWinUISlot::AdvanceContentGeneration()
{
    if ( ++m_contentGeneration == 0 )
        ++m_contentGeneration;

    // Increment first: UiaReturnRawElementProvider(..., nullptr) is an
    // external COM boundary. If it causes a new WM_GETOBJECT synchronously,
    // that request must already bind to the replacement identity.
    RetireExposedAccessibilityShellProvider();
}

bool wxWinUISlot::InstallToolTipPropertyObserver()
{
#if wxUSE_TOOLTIPS
    constexpr unsigned AutomaticRetryLimit = 3;

    const unsigned previousRetryFailures =
        m_toolTipPropertyObserverRetryFailures;
    RevokeToolTipPropertyObserver();
    m_toolTipPropertyObserverRetryFailures = previousRetryFailures;
    if ( !m_content || !m_lifetime )
        return true;

    const auto expectedContent = m_content;
    const auto expectedLifetime = m_lifetime;
    const unsigned long long expectedGeneration = m_contentGeneration;
    const unsigned long long expectedTransactionEpoch =
        m_contentTransactionEpoch;
    const auto operationIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime == expectedLifetime &&
               expectedLifetime->GetHost() != nullptr &&
               m_content == expectedContent &&
               m_contentGeneration == expectedGeneration &&
               m_contentTransactionEpoch == expectedTransactionEpoch &&
               !m_contentTransactionInProgress;
    };

    try
    {
        auto state = std::make_shared<wxWinUIToolTipPropertyState>();
        state->lifetime = expectedLifetime;
        wxWinUIMaybeInjectToolTipObserverFault();
        wxWinUIMaybeInjectContentFault(
            wxWinUITopLevelHost::TestContentFault_ToolTipObserver);
        const int64_t token = expectedContent.RegisterPropertyChangedCallback(
            winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                ToolTipProperty(),
            [state](
                const winrt::Microsoft::UI::Xaml::DependencyObject& sender,
                const winrt::Microsoft::UI::Xaml::DependencyProperty&)
            {
                if ( !state->active || state->inCallback )
                    return;

                const auto lifetime = state->lifetime.lock();
                wxWinUITopLevelHost * const host =
                    lifetime ? lifetime->GetHost() : nullptr;
                wxWindow * const window =
                    lifetime ? lifetime->GetWindow() : nullptr;
                wxWinUISlot * const slot =
                    host && window ? host->FindSlot(window) : nullptr;
                const auto element =
                    sender.try_as<
                        winrt::Microsoft::UI::Xaml::UIElement>();
                if ( !slot || slot->m_lifetime != lifetime ||
                     slot->m_toolTipPropertyState != state ||
                     slot->m_content != element )
                {
                    return;
                }

                struct CallbackGuard
                {
                    explicit CallbackGuard(
                        const std::shared_ptr<
                            wxWinUIToolTipPropertyState>& value)
                        : state(value)
                    {
                        state->inCallback = true;
                    }
                    ~CallbackGuard() { state->inCallback = false; }
                    std::shared_ptr<wxWinUIToolTipPropertyState> state;
                } guard(state);

                // A direct XAML write has no wx geometry notification. Dirty
                // the slot so ownership/baseline is reconciled. Under global
                // suppression, close a ToolTip object immediately.
                host->MarkDirty(window);
                if ( !wxWinUIAreToolTipsEnabled() )
                {
                    try
                    {
                        const auto actual =
                            winrt::Microsoft::UI::Xaml::Controls::
                                ToolTipService::GetToolTip(element);
                        if ( const auto toolTip =
                                 actual.try_as<
                                     winrt::Microsoft::UI::Xaml::Controls::
                                         ToolTip>() )
                        {
                            toolTip.IsOpen(false);
                        }

                        // ToolTip::Closed is delivered from Popup::Closed and
                        // can therefore publish a replacement after the
                        // original wxToolTip::Enable(false) transaction has
                        // returned. Re-resolve every lifetime boundary crossed
                        // by IsOpen(false), then run the policy-only adapter
                        // immediately so even a frozen host never exposes that
                        // late replacement. Writes made by either host tooltip
                        // transaction are already covered by their own bounded
                        // re-read loop and must not recursively enter it.
                        if ( wxWinUIAreToolTipsEnabled() )
                            return;

                        const auto currentLifetime = state->lifetime.lock();
                        wxWinUITopLevelHost * const currentHost =
                            currentLifetime
                                ? currentLifetime->GetHost()
                                : nullptr;
                        wxWindow * const currentWindow =
                            currentLifetime
                                ? currentLifetime->GetWindow()
                                : nullptr;
                        wxWinUISlot * const currentSlot =
                            currentHost && currentWindow
                                ? currentHost->FindSlot(currentWindow)
                                : nullptr;
                        if ( !state->active ||
                             !currentSlot ||
                             currentSlot->m_lifetime != currentLifetime ||
                             currentSlot->m_toolTipPropertyState != state ||
                             currentSlot->m_content != element ||
                             currentSlot->m_toolTipPolicySyncDepth != 0 )
                        {
                            return;
                        }

                        const auto remaining =
                            winrt::Microsoft::UI::Xaml::Controls::
                                ToolTipService::GetToolTip(element);
                        if ( remaining )
                        {
                            // Last operation: this transaction may invoke
                            // application code which destroys or reparents the
                            // slot.
                            currentHost->SynchronizeSlotToolTipPolicy(
                                *currentSlot);
                        }
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "suppress tooltip under disabled policy", e);
                    }
                }
            });

        // RegisterPropertyChangedCallback() is a WinRT boundary. If it
        // synchronously destroyed/replaced the slot, revoke the just-created
        // callback instead of publishing it onto the obsolete generation.
        if ( !operationIsCurrent() )
        {
            state->active = false;
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                        ToolTipProperty(),
                    token);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke stale tooltip property observer", e);
            }
            return false;
        }

        m_toolTipObservedContent = expectedContent;
        m_toolTipPropertyCallbackToken = token;
        m_toolTipPropertyState = std::move(state);
        m_toolTipPropertyObserverPending = false;
        m_toolTipPropertyObserverRetryFailures = 0;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("register tooltip property observer", e);
        // A registration HRESULT on the still-exact content is a degraded
        // auxiliary state, not a failed content transaction. A stale result
        // (destroy/replacement/nested swap) belongs to an obsolete operation
        // and must not overwrite the newer observer's fields.
        if ( operationIsCurrent() )
        {
            m_toolTipPropertyObserverPending = true;
            m_toolTipPropertyObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_toolTipPropertyObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueToolTipPropertyObserverRetry();
            }
        }
        return false;
    }
#else
    return true;
#endif
}

void wxWinUISlot::RevokeToolTipPropertyObserver()
{
#if wxUSE_TOOLTIPS
    const auto state = m_toolTipPropertyState;
    if ( state )
        state->active = false;

    const auto content = m_toolTipObservedContent;
    const int64_t token = m_toolTipPropertyCallbackToken;
    m_toolTipObservedContent = nullptr;
    m_toolTipPropertyCallbackToken = 0;
    m_toolTipPropertyState.reset();
    m_toolTipPropertyObserverPending = false;
    m_toolTipPropertyObserverRetryFailures = 0;

    if ( content && token )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                    ToolTipProperty(),
                token);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("revoke tooltip property observer", e);
        }
    }
#endif
}

void wxWinUISlot::QueueToolTipPropertyObserverRetry()
{
#if wxUSE_TOOLTIPS
    // MarkDirty/CallAfter is already coalesced by the host. The finite retry
    // counter is maintained by InstallToolTipPropertyObserver(); this helper
    // deliberately does not spin or post an independent callback.
    if ( m_toolTipPropertyObserverPending &&
         !m_deletePending &&
         !m_poisoned &&
         m_content &&
         m_lifetime &&
         m_lifetime->GetHost() )
    {
        NudgeDirty();
    }
#endif
}

bool wxWinUISlot::InstallAutomationNameStylePropertyObserver()
{
    constexpr unsigned AutomaticRetryLimit = 3;

    const unsigned previousRetryFailures =
        m_automationNameStylePropertyObserverRetryFailures;
    RevokeAutomationNameStylePropertyObserver();
    m_automationNameStylePropertyObserverRetryFailures =
        previousRetryFailures;

    const auto expectedContent =
        m_semanticTarget.try_as<
            winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if ( !expectedContent || !m_lifetime )
        return true;

    const auto expectedLifetime = m_lifetime;
    const unsigned long long expectedGeneration = m_contentGeneration;
    const unsigned long long expectedTransactionEpoch =
        m_contentTransactionEpoch;
    const auto operationIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime == expectedLifetime &&
               expectedLifetime->GetHost() != nullptr &&
               m_semanticTarget == expectedContent &&
               m_contentGeneration == expectedGeneration &&
               m_contentTransactionEpoch == expectedTransactionEpoch &&
               !m_contentTransactionInProgress;
    };

    auto state =
        std::make_shared<wxWinUIAutomationNameStylePropertyState>();
    state->lifetime = expectedLifetime;
    int64_t styleToken = 0;
    int64_t nameToken = 0;
    int64_t labeledByToken = 0;
    winrt::event_token themeToken{};
    winrt::event_token layoutUpdatedToken{};

    const auto notifyStyleContextChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::FrameworkElement& framework,
            bool implicitStyleWasReresolved)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_automationNameStyleObservedContent != framework ||
                 slot->m_semanticTarget != framework )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // Record every explicit Style mutation for reentrancy validation.
            // Style changes and ActualThemeChanged are both boundaries where
            // WinUI can resolve a different implicit Style.
            if ( ++slot->m_automationNameStyleRevision == 0 )
                ++slot->m_automationNameStyleRevision;
            if ( implicitStyleWasReresolved )
            {
                if ( ++slot->m_automationNameImplicitStyleRevision == 0 )
                    ++slot->m_automationNameImplicitStyleRevision;
            }
            host->MarkDirty(window);
        };

    const auto notifyAutomationNameChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::DependencyObject& content)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_semanticTarget != content )
            {
                return;
            }

            if ( slot->m_contentTransactionInProgress )
            {
                // RestoreAutomationName() has already removed wx's exact
                // scalar before the carrier transaction starts. Any later
                // Name edge in this interval therefore comes from application
                // code (including SetName followed by ClearValue) and must
                // survive a migration even though its final DP value is unset.
                slot->m_contentTransactionAutomationNameRelinquished = true;
                return;
            }

            if ( !slot->m_uiaNameSynced ||
                 slot->m_uiaNameRelinquished )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // Name mutations don't re-resolve Style and must not advance its
            // revision. They only ask the common ownership pass to compare
            // local, binding, animated/effective and wx-published values.
            host->MarkDirty(window);
        };

    const auto notifyAutomationLabeledByChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::DependencyObject& content)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_semanticTarget != content )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // LabeledBy takes semantic precedence over wx's fallback Name,
            // but it is reversible. Advance the revision so a relation
            // mutation occurring inside another XAML setter invalidates that
            // outer SyncSlot pass instead of publishing stale ownership.
            if ( ++slot->m_automationNameStyleRevision == 0 )
                ++slot->m_automationNameStyleRevision;
            host->MarkDirty(window);
        };

    const auto updateLocalStyleSourceSnapshot =
        [state](
            const winrt::Microsoft::UI::Xaml::FrameworkElement& framework)
        {
            return wxWinUIUpdateLocalStyleSourceSnapshot(state, framework);
        };

    const auto revokeProvisional = [&]()
    {
        state->active = false;
        if ( layoutUpdatedToken.value )
            expectedContent.LayoutUpdated(layoutUpdatedToken);
        if ( themeToken.value )
            expectedContent.ActualThemeChanged(themeToken);
        if ( styleToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::FrameworkElement::
                        StyleProperty(),
                    styleToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation Style observer", e);
            }
        }
        if ( nameToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Automation::
                        AutomationProperties::NameProperty(),
                    nameToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation Name observer", e);
            }
        }
        if ( labeledByToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Automation::
                        AutomationProperties::LabeledByProperty(),
                    labeledByToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation LabeledBy observer", e);
            }
        }
    };

    try
    {
        const auto styleProperty =
            winrt::Microsoft::UI::Xaml::FrameworkElement::StyleProperty();
        updateLocalStyleSourceSnapshot(expectedContent);

        styleToken =
            expectedContent.RegisterPropertyChangedCallback(
                styleProperty,
                [state,
                 updateLocalStyleSourceSnapshot,
                 notifyStyleContextChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    const auto framework =
                        sender.try_as<
                            winrt::Microsoft::UI::Xaml::FrameworkElement>();
                    if ( framework )
                    {
                        try
                        {
                            updateLocalStyleSourceSnapshot(framework);
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "inspect changed Style source", e);
                        }
                    }
                    notifyStyleContextChanged(framework, true);
                });
        nameToken =
            expectedContent.RegisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::NameProperty(),
                [notifyAutomationNameChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    notifyAutomationNameChanged(sender);
                });
        labeledByToken =
            expectedContent.RegisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::LabeledByProperty(),
                [notifyAutomationLabeledByChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    notifyAutomationLabeledByChanged(sender);
                });
        themeToken = expectedContent.ActualThemeChanged(
            [notifyStyleContextChanged](
                const winrt::Microsoft::UI::Xaml::FrameworkElement& sender,
                const winrt::Windows::Foundation::IInspectable&)
            {
                notifyStyleContextChanged(sender, true);
            });

        if ( wxDynamicCast(m_window, wxWinUIXamlHost) )
        {
            const winrt::weak_ref<
                winrt::Microsoft::UI::Xaml::FrameworkElement> weakContent =
                    expectedContent;
            layoutUpdatedToken = expectedContent.LayoutUpdated(
                [state,
                 weakContent,
                 updateLocalStyleSourceSnapshot,
                 notifyStyleContextChanged](
                    const winrt::Windows::Foundation::IInspectable&,
                    const winrt::Windows::Foundation::IInspectable&)
                {
                    if ( !state->active || state->inCallback )
                    {
                        return;
                    }

                    const auto framework = weakContent.get();
                    if ( !framework )
                        return;

                    try
                    {
                        if ( !updateLocalStyleSourceSnapshot(framework) )
                            return;

                        // RegisterPropertyChangedCallback reports effective
                        // value changes. x:Null <-> UnsetValue can therefore
                        // be silent while the effective Style stays null.
                        // Arbitrary XAML hosts keep this source snapshot for
                        // their lifetime so layout-affecting late application
                        // mutations converge without imposing an observer on
                        // every native wx peer.
                        notifyStyleContextChanged(framework, true);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "inspect local Style source", e);
                    }
                });
        }

        if ( !operationIsCurrent() )
        {
            revokeProvisional();
            return false;
        }

        m_automationNameStyleObservedContent = expectedContent;
        m_automationNameStylePropertyCallbackToken = styleToken;
        m_automationNamePropertyCallbackToken = nameToken;
        m_automationLabeledByPropertyCallbackToken = labeledByToken;
        m_automationNameActualThemeChangedToken = themeToken;
        m_automationNameStyleLayoutUpdatedToken = layoutUpdatedToken;
        m_automationNameStylePropertyState = std::move(state);
        m_automationNameStylePropertyObserverPending = false;
        if ( previousRetryFailures )
        {
            // A Style/RequestedTheme/relationship mutation may have occurred
            // while no observer was installed. Force the catch-up pass to
            // re-inspect the complete naming context instead of treating the
            // last successful snapshot as current.
            if ( ++m_automationNameStyleRevision == 0 )
                ++m_automationNameStyleRevision;
            if ( ++m_automationNameImplicitStyleRevision == 0 )
                ++m_automationNameImplicitStyleRevision;
        }
        m_automationNameStylePropertyObserverRetryFailures = 0;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        revokeProvisional();
        wxWinUILogException(
            "register automation Name/style/relation observer", e);
        if ( operationIsCurrent() )
        {
            m_automationNameStylePropertyObserverPending = true;
            m_automationNameStylePropertyObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_automationNameStylePropertyObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueAutomationNameStylePropertyObserverRetry();
            }
        }
        return false;
    }
}

void wxWinUISlot::RevokeAutomationNameStylePropertyObserver()
{
    const auto state = m_automationNameStylePropertyState;
    if ( state )
        state->active = false;

    const auto content = m_automationNameStyleObservedContent;
    const int64_t token =
        m_automationNameStylePropertyCallbackToken;
    const int64_t nameToken =
        m_automationNamePropertyCallbackToken;
    const int64_t labeledByToken =
        m_automationLabeledByPropertyCallbackToken;
    const winrt::event_token themeToken =
        m_automationNameActualThemeChangedToken;
    const winrt::event_token layoutUpdatedToken =
        m_automationNameStyleLayoutUpdatedToken;
    m_automationNameStyleObservedContent = nullptr;
    m_automationNameStylePropertyCallbackToken = 0;
    m_automationNamePropertyCallbackToken = 0;
    m_automationLabeledByPropertyCallbackToken = 0;
    m_automationNameActualThemeChangedToken = {};
    m_automationNameStyleLayoutUpdatedToken = {};
    m_automationNameStylePropertyState.reset();
    m_automationNameStylePropertyObserverPending = false;
    m_automationNameStylePropertyObserverRetryFailures = 0;

    if ( content && token )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::FrameworkElement::
                    StyleProperty(),
                token);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation Style observer", e);
        }
    }

    if ( content && nameToken )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::NameProperty(),
                nameToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation Name observer", e);
        }
    }

    if ( content && labeledByToken )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::LabeledByProperty(),
                labeledByToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation LabeledBy observer", e);
        }
    }

    // C++/WinRT projects event removal as noexcept; state was invalidated
    // above first, so even an ABI-side removal failure cannot call live wx
    // objects through the retained delegate.
    if ( content && themeToken.value )
        content.ActualThemeChanged(themeToken);
    if ( content && layoutUpdatedToken.value )
        content.LayoutUpdated(layoutUpdatedToken);
}

void wxWinUISlot::QueueAutomationNameStylePropertyObserverRetry()
{
    if ( m_automationNameStylePropertyObserverPending &&
         !m_deletePending &&
         !m_poisoned &&
         m_semanticTarget &&
         m_lifetime &&
         m_lifetime->GetHost() )
    {
        NudgeDirty();
    }
}

bool wxWinUISlot::SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    return SetContent(element, element);
}

bool wxWinUISlot::SetContent(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget)
{
    wxWinUITopLevelHost * const host =
        m_lifetime ? m_lifetime->GetHost() : nullptr;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    OperationGuard slotOperation(this);

    if ( !host || !m_carrier )
        return false;

    // The pair is one model generation. A semantic target is meaningful only
    // while its visual root is attached, and must never escape that subtree.
    if ( !wxWinUISemanticPairIsValid(element, semanticTarget) )
    {
        return false;
    }

    // A nested content mutation cannot reason truthfully about the slot while
    // an outer transaction has already changed the carrier but has not yet
    // published the corresponding model. Refuse it and preserve the outer
    // transaction's coalesced catch-up instead of guessing which element is
    // attached.
    if ( m_contentTransactionInProgress )
    {
        NudgeDirty();
        return false;
    }

    if ( element == m_content &&
         semanticTarget == m_semanticTarget )
    {
        const auto lifetime = m_lifetime;
        const unsigned long long generation = m_contentGeneration;
        const bool toolTipObserverInstalled =
            !m_toolTipPropertyObserverPending ||
            InstallToolTipPropertyObserver();
        const bool automationStyleObserverInstalled =
            !m_automationNameStylePropertyObserverPending ||
            InstallAutomationNameStylePropertyObserver();
        const bool loadedObserverInstalled =
            !m_contentLoadedObserverPending ||
            InstallContentLoadedObserver();

#if wxUSE_TOOLTIPS
        // Re-attaching an already-published element can happen while the
        // destination TLW is frozen (notably during migration rollback).
        // Global tooltip suppression is process policy, so it must not wait
        // for the ordinary geometry flush: an authored baseline would
        // otherwise remain exposed indefinitely on a frozen host.
        if ( element && !wxWinUIAreToolTipsEnabled() )
        {
            host->SynchronizeSlotToolTipPolicy(*this);
            if ( m_deletePending || m_poisoned ||
                 m_lifetime != lifetime ||
                 !lifetime || lifetime->GetHost() != host ||
                 m_content != element ||
                 m_contentGeneration != generation )
            {
                return false;
            }
        }
#endif

        if ( !toolTipObserverInstalled ||
             !automationStyleObserverInstalled ||
             (!loadedObserverInstalled &&
              !m_contentLoadedObserverPending) )
        {
            NudgeDirty();
            return false;
        }

        NudgeDirty();
        return true;
    }

    // Content swaps are mechanically transactional, phase by phase: the old
    // element and the per-content caches survive until the new element is
    // actually installed.
    const auto oldContent = m_content;
    const auto oldSemanticTarget = m_semanticTarget;
    const unsigned long long generation = m_contentGeneration;
    const auto oldLifetime = m_lifetime;
    unsigned long long automationNameStateRevision =
        m_uiaNameStateRevision;
    const unsigned long long automationNameStyleRevision =
        m_automationNameStyleRevision;

    const auto preTransactionIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime == oldLifetime &&
               oldLifetime &&
               oldLifetime->GetHost() == host &&
               m_content == oldContent &&
               m_semanticTarget == oldSemanticTarget &&
               m_contentGeneration == generation &&
               m_uiaNameStateRevision ==
                   automationNameStateRevision &&
               m_automationNameStyleRevision ==
                   automationNameStyleRevision &&
               !m_contentTransactionInProgress;
    };

    const bool preserveOldAutomationNameRelinquishment =
        ShouldPreserveAutomationNameRelinquishment();
    // Implicit resource lookup may materialize deferred XAML and invoke
    // application code. Never let an obsolete outer swap restore or detach a
    // content generation superseded by such a callback.
    if ( !preTransactionIsCurrent() )
        return false;

    const wxString oldDesiredAutomationName = m_lastUIAName;
    bool automationNameRestoreCompleted = false;

    // Host-owned attached values belong to the slot, not to the application
    // element. Restore them before detaching the old content. Every setter can
    // synchronously mutate or remove this slot, so validate the exact content
    // generation after each restoration.
    const auto rollbackRestoredState = [&]()
    {
        if ( preTransactionIsCurrent() )
        {
            if ( automationNameRestoreCompleted &&
                 preserveOldAutomationNameRelinquishment )
            {
                SeedAutomationNameRelinquishment(
                    oldDesiredAutomationName);
            }
            NudgeDirty();
            host->SyncSlot(*this);
        }
        return false;
    };

    if ( !RestoreAutomationName() )
        return rollbackRestoredState();
    automationNameStateRevision = m_uiaNameStateRevision;
    if ( !preTransactionIsCurrent() )
        return rollbackRestoredState();
    automationNameRestoreCompleted = true;
    if ( !RestoreHelpText() || !preTransactionIsCurrent() )
        return rollbackRestoredState();
    if ( !RestoreToolTip() || !preTransactionIsCurrent() )
        return rollbackRestoredState();
    if ( !RestoreAccessibilityView() || !preTransactionIsCurrent() )
    {
        // Earlier restorations may already have committed before the later
        // one failed. Re-publish current wx state on the still-authoritative
        // old content before reporting the rollback.
        return rollbackRestoredState();
    }

    // Publish the transaction state BEFORE the first carrier setter. Loaded,
    // property callbacks and test seams may synchronously call FlushSync();
    // such a pass must requeue and leave without syncing old model A against
    // a carrier which already contains B (or is temporarily empty).
    if ( ++m_contentTransactionEpoch == 0 )
        ++m_contentTransactionEpoch;
    const unsigned long long transactionEpoch = m_contentTransactionEpoch;
    m_contentLoadedDuringTransaction = false;
    m_contentLoadedTransactionEpoch = 0;
    m_contentLoadedTransactionGeneration = 0;
    m_contentTransactionAutomationNameRelinquished =
        preserveOldAutomationNameRelinquishment;
    m_lastContentTransactionAutomationNameRelinquished = false;
    m_contentTransactionInProgress = true;

    const auto transactionIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime &&
               m_lifetime->GetHost() == host &&
               m_contentTransactionInProgress &&
               m_contentTransactionEpoch == transactionEpoch &&
               m_content == oldContent &&
               m_semanticTarget == oldSemanticTarget &&
               m_contentGeneration == generation;
    };

    const auto transactionObjectIsCurrent = [&]()
    {
        return !m_deletePending &&
               m_lifetime &&
               m_lifetime->GetHost() == host &&
               m_contentTransactionInProgress &&
               m_contentTransactionEpoch == transactionEpoch;
    };

    const auto finishTransaction = [&]()
    {
        if ( !transactionObjectIsCurrent() )
            return false;

        m_contentTransactionInProgress = false;
        m_lastContentTransactionAutomationNameRelinquished =
            m_contentTransactionAutomationNameRelinquished;
        m_contentTransactionAutomationNameRelinquished = false;
        m_contentLoadedDuringTransaction = false;
        m_contentLoadedTransactionEpoch = 0;
        m_contentLoadedTransactionGeneration = 0;
        if ( ++m_contentTransactionEpoch == 0 )
            ++m_contentTransactionEpoch;
        // Dirty only after the carrier/model pair is truthful. Any nested
        // flush that saw the in-progress flag also marked this same next pass.
        NudgeDirty();
        return true;
    };

    // Deterministic equivalent of an old-content Loaded callback entering
    // immediately after the transaction became observable. Consume the hook
    // before invocation so nested content changes cannot recurse through it.
    std::function<void (wxWindow *)> transactionStartedHook =
        std::move(gs_testContentTransactionStartedHook);
    if ( transactionStartedHook )
    {
        wxWindow * const currentWindow =
            m_lifetime ? m_lifetime->GetWindow() : nullptr;
        transactionStartedHook(currentWindow);
        if ( !transactionIsCurrent() )
            return false;
    }

    const auto publishedContentIdentityIsCurrent =
        [&](const winrt::Microsoft::UI::Xaml::UIElement& expected,
            const winrt::Microsoft::UI::Xaml::UIElement&
                expectedSemanticTarget,
            unsigned long long expectedGeneration)
        {
            return !m_deletePending &&
                   !m_poisoned &&
                   m_lifetime &&
                   m_lifetime->GetHost() == host &&
                   !m_contentTransactionInProgress &&
                   m_content == expected &&
                   m_semanticTarget == expectedSemanticTarget &&
                   m_contentGeneration == expectedGeneration;
        };
    const auto publishedContentIsCurrent =
        [&](const winrt::Microsoft::UI::Xaml::UIElement& expected,
            const winrt::Microsoft::UI::Xaml::UIElement&
                expectedSemanticTarget,
            unsigned long long expectedGeneration)
        {
            return publishedContentIdentityIsCurrent(
                       expected,
                       expectedSemanticTarget,
                       expectedGeneration) &&
                   wxWinUISemanticPairIsValid(
                       expected, expectedSemanticTarget);
        };
    const auto ensurePublishedPairIsUsable =
        [&](const winrt::Microsoft::UI::Xaml::UIElement& expected,
            const winrt::Microsoft::UI::Xaml::UIElement&
                expectedSemanticTarget,
            unsigned long long expectedGeneration)
        {
            if ( publishedContentIsCurrent(
                     expected,
                     expectedSemanticTarget,
                     expectedGeneration) )
            {
                return true;
            }

            // Observer registration and attached-property projection are
            // synchronous application-code boundaries. If one detached the
            // semantic descendant without changing object identities, repair
            // the already-published carrier to a truthful root/root pair.
            // The outer request still reports false because its requested
            // semantic target was not retained.
            if ( expected &&
                 publishedContentIdentityIsCurrent(
                     expected,
                     expectedSemanticTarget,
                     expectedGeneration) &&
                 !wxWinUISemanticPairIsValid(
                     expected, expectedSemanticTarget) )
            {
                if ( !SetContent(expected, expected) &&
                     !m_deletePending &&
                     !m_poisoned &&
                     m_lifetime &&
                     m_lifetime->GetHost() == host &&
                     m_content == expected &&
                     !wxWinUISemanticPairIsValid(
                         m_content, m_semanticTarget) )
                {
                    // A restoration HRESULT can stop the nested transaction
                    // before its ordinary root fallback. Mirror the readable
                    // carrier as the last-resort repair; this also poisons an
                    // unreadable carrier instead of publishing a lie.
                    SyncContentFromCarrier();
                }
            }
            return false;
        };

    const auto commitRequested = [&]()
    {
        if ( !transactionIsCurrent() )
            return false;

        RevokeContentLoadedObserver();
        if ( !transactionIsCurrent() )
            return false;
        m_content = element;
        m_semanticTarget = semanticTarget;
        AdvanceContentGeneration();
        const unsigned long long committedGeneration = m_contentGeneration;
        RevokeAutomationNameStylePropertyObserver();
        ResetContentCaches();
        if ( !finishTransaction() )
            return false;

        // Content placement is already committed and the model truthfully
        // describes it. An observer registration failure is represented by
        // m_toolTipPropertyObserverPending and retried by SyncSlot; do not
        // report a fictitious content rollback to the caller.
        const bool observerInstalled = InstallToolTipPropertyObserver();
        const bool automationStyleObserverInstalled =
            InstallAutomationNameStylePropertyObserver();
        const bool loadedObserverInstalled =
            InstallContentLoadedObserver();

        // Registration itself is a WinRT boundary. Always revalidate,
        // including the no-wxAccessible case: false+pending on this exact
        // content is a recoverable HRESULT, while false after a synchronous
        // destroy/replacement belongs to an obsolete operation.
        if ( !ensurePublishedPairIsUsable(
                 element, semanticTarget, committedGeneration) )
            return false;
        if ( !observerInstalled &&
             !m_toolTipPropertyObserverPending )
        {
            return false;
        }
        if ( !automationStyleObserverInstalled &&
             !m_automationNameStylePropertyObserverPending )
        {
            return false;
        }
        if ( !loadedObserverInstalled &&
             !m_contentLoadedObserverPending )
        {
            return false;
        }

        // A replacement installed while native accessibility is
        // authoritative must be Raw before SetContent() returns; otherwise
        // the old native and new XAML trees coexist until the deferred flush.
        if ( element && m_hasWxAccessible )
        {
            host->SyncSlotAccessibilityAuthority(*this, true);
            if ( !ensurePublishedPairIsUsable(
                     element, semanticTarget, committedGeneration) )
                return false;
        }

#if wxUSE_TOOLTIPS
        // A freshly-published element may carry an authored tooltip. Policy
        // suppression is synchronous even when the owning TLW is frozen, so
        // never leave that baseline visible until the coalesced full sync.
        if ( element && !wxWinUIAreToolTipsEnabled() )
        {
            host->SynchronizeSlotToolTipPolicy(*this);
            if ( !ensurePublishedPairIsUsable(
                     element, semanticTarget, committedGeneration) )
            {
                return false;
            }
        }
#endif

        return ensurePublishedPairIsUsable(
            element, semanticTarget, committedGeneration);
    };

    const auto noteOldRestored = [&](bool implicitStyleWasReresolved)
    {
        if ( !transactionIsCurrent() )
            return false;

        const auto restoredSemanticTarget =
            wxWinUISemanticPairIsValid(oldContent, oldSemanticTarget)
                ? oldSemanticTarget
                : oldContent;
        const bool semanticTargetChanged =
            restoredSemanticTarget != oldSemanticTarget;

        // The Style/Name observer belongs to the semantic target, unlike
        // Loaded and tooltip state which belong to the visual root. If
        // application code detached the old target while the candidate was
        // being installed, retire that observer before publishing the
        // root-as-semantic fallback. Do not ResetContentCaches(): the visual
        // root and all root-owned state are still the restored generation.
        if ( semanticTargetChanged )
        {
            RevokeAutomationNameStylePropertyObserver();
            if ( !transactionIsCurrent() )
                return false;

            m_automationNameStyleRevision = 0;
            m_automationNameImplicitStyleRevision = 0;
            // The inspection cursor belongs to the semantic target too.
            // Publish the complete provenance tuple instead of resetting one
            // field behind a potentially re-entrant observer boundary.
            PublishAutomationNameState(
                m_uiaNameSynced,
                m_uiaNameOwned,
                m_uiaNameAppOwned,
                m_uiaNameRelinquished,
                m_uiaNameSuppressedByLabeledBy,
                m_lastUIAName,
                m_lastAppliedUIAName,
                m_originalUIANameLocalValue,
                0);
        }

        // The detached old tree is application-visible and may itself have
        // been edited by the callback that invalidated the candidate. Keep
        // the carrier/model usable by falling back to the visual root instead
        // of republishing a semantic pointer outside that tree.
        m_semanticTarget = restoredSemanticTarget;

        const bool restoredContentWasLoaded =
            m_contentLoaded ||
            (m_contentLoadedDuringTransaction &&
             m_contentLoadedTransactionEpoch == transactionEpoch &&
             m_contentLoadedTransactionGeneration == generation);
        const bool remapDeferredFocus =
            host->m_deferredFocusRequest.target == m_lifetime &&
            host->m_deferredFocusRequest.intentGeneration ==
                host->m_focusIntentGeneration &&
            host->m_deferredFocusContentGeneration == generation;

        // Even with the same logical element, detach/restore invalidates an
        // outer SyncSlot generation. Restore the logical wx state immediately
        // because the failed transaction must be observationally atomic.
        AdvanceContentGeneration();
        if ( implicitStyleWasReresolved )
        {
            // Reattaching an element is one of the boundaries at which WinUI
            // can resolve an implicit Style again. Keep this distinct from a
            // plain StyleProperty ClearValue on an already-realized element.
            if ( ++m_automationNameStyleRevision == 0 )
                ++m_automationNameStyleRevision;
            if ( ++m_automationNameImplicitStyleRevision == 0 )
                ++m_automationNameImplicitStyleRevision;
        }
        const unsigned long long restoredGeneration = m_contentGeneration;
        if ( !finishTransaction() )
            return false;
        // finishTransaction() publishes the observer verdict accumulated while
        // the carrier was detached. Snapshot it immediately: installing the
        // restored observers below is itself re-entrant and may start another
        // content transaction before we seed the restored semantic target.
        const bool preserveRestoredAutomationNameRelinquishment =
            m_lastContentTransactionAutomationNameRelinquished;

        // The old observer captured the pre-transaction generation and is now
        // deliberately stale. Rebind it to restoredGeneration before any
        // later Loaded edge. A focus request waiting on A is part of the same
        // failed transaction and follows A to its restored generation.
        if ( remapDeferredFocus )
            host->m_deferredFocusContentGeneration = restoredGeneration;

        RevokeContentLoadedObserver();
        if ( !ensurePublishedPairIsUsable(
                 oldContent, restoredSemanticTarget, restoredGeneration) )
            return false;

        const bool automationStyleObserverInstalled =
            !semanticTargetChanged ||
            InstallAutomationNameStylePropertyObserver();
        if ( !ensurePublishedPairIsUsable(
                 oldContent, restoredSemanticTarget, restoredGeneration) )
        {
            return false;
        }
        if ( !automationStyleObserverInstalled &&
             !m_automationNameStylePropertyObserverPending )
        {
            return false;
        }

        m_contentLoaded = false;
        const bool loadedObserverInstalled =
            InstallContentLoadedObserver();
        if ( !ensurePublishedPairIsUsable(
                 oldContent, restoredSemanticTarget, restoredGeneration) )
            return false;
        if ( !loadedObserverInstalled &&
             !m_contentLoadedObserverPending )
        {
            return false;
        }

        if ( restoredContentWasLoaded && !m_contentLoaded )
        {
            host->NotifySlotContentLoaded(
                m_window, oldContent, restoredGeneration);
            if ( !ensurePublishedPairIsUsable(
                     oldContent,
                     restoredSemanticTarget,
                     restoredGeneration) )
            {
                return false;
            }
        }

        if ( preserveRestoredAutomationNameRelinquishment &&
             restoredSemanticTarget == oldSemanticTarget )
        {
            SeedAutomationNameRelinquishment(
                oldDesiredAutomationName);
        }
        NudgeDirty();
        if ( ensurePublishedPairIsUsable(
                 oldContent, restoredSemanticTarget, restoredGeneration) &&
             m_content && m_hasWxAccessible )
        {
            host->SyncSlotAccessibilityAuthority(*this, true);
            if ( !ensurePublishedPairIsUsable(
                     oldContent,
                     restoredSemanticTarget,
                     restoredGeneration) )
            {
                return false;
            }
        }
        if ( ensurePublishedPairIsUsable(
                 oldContent, restoredSemanticTarget, restoredGeneration) &&
             m_content )
        {
            host->SyncSlot(*this);
            (void)ensurePublishedPairIsUsable(
                oldContent,
                restoredSemanticTarget,
                restoredGeneration);
        }
        return false;
    };

    const auto poisonWithActual =
        [&](const winrt::Microsoft::UI::Xaml::UIElement& actual)
        {
            if ( !transactionIsCurrent() )
                return;

            m_content = actual;
            // An unexpected carrier value has no validated semantic
            // descendant. The slot is poisoned below, but keep its diagnostic
            // model internally coherent until teardown.
            m_semanticTarget = actual;
            AdvanceContentGeneration();
            ResetContentCaches();
            finishTransaction();
            // The slot is deliberately unrecoverable. Retire the old
            // observer rather than installing a new callback onto an element
            // whose carrier state cannot be trusted.
            RevokeToolTipPropertyObserver();
            RevokeAutomationNameStylePropertyObserver();
            RevokeContentLoadedObserver();
            m_poisoned = true;
            if ( m_lifetime )
                m_lifetime->Invalidate();
        };

    // Phase 1 -- detach the old content.  On failure nothing was provably
    // unchanged: WinRT setters may mutate before throwing, so read the
    // carrier and continue/commit only from the state it actually holds.
    try
    {
        wxWinUIMaybeInjectContentFault(
            wxWinUITopLevelHost::TestContentFault_Detach);
        m_carrier.Content(nullptr);
        if ( !transactionIsCurrent() )
            return false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot content detach", e);

        winrt::Microsoft::UI::Xaml::UIElement actual{ nullptr };
        if ( !TryGetCarrierContent(actual) || !transactionIsCurrent() )
        {
            if ( transactionIsCurrent() )
                poisonWithActual(nullptr);
            return false;
        }
        if ( actual == oldContent )
            return noteOldRestored(false);
        if ( actual == element &&
             wxWinUISemanticPairIsValid(element, semanticTarget) )
            return commitRequested();
        if ( actual )
        {
            poisonWithActual(actual);
            return false;
        }
        // Readably empty means the detach did commit: proceed to install.
    }

    // Phase 2 -- install the new one.
    try
    {
        if ( element )
        {
            wxWinUIMaybeInjectContentFault(
                wxWinUITopLevelHost::TestContentFault_Install);
            m_carrier.Content(element);
            if ( !transactionIsCurrent() )
                return false;
        }

        // The actual XAML Loaded/property boundary can occur inside the
        // Content() call. This one-shot seam deterministically exercises the
        // same state immediately afterwards, while the carrier is B and the
        // published slot model is still A.
        std::function<void (wxWindow *)> carrierSetHook =
            std::move(gs_testContentCarrierSetHook);
        if ( carrierSetHook )
        {
            wxWindow * const window =
                m_lifetime ? m_lifetime->GetWindow() : nullptr;
            carrierSetHook(window);
            if ( !transactionIsCurrent() )
                return false;
        }

        // Content() and the deterministic boundary above can synchronously
        // run Loaded/application code. Never publish a descendant which that
        // code has already detached from the requested visual root: route the
        // operation through the ordinary phase-3 restoration below.
        if ( !wxWinUISemanticPairIsValid(element, semanticTarget) )
            winrt::throw_hresult(E_FAIL);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot content swap", e);
        if ( !transactionIsCurrent() )
            return false;

        winrt::Microsoft::UI::Xaml::UIElement actual{ nullptr };
        if ( !TryGetCarrierContent(actual) || !transactionIsCurrent() )
        {
            if ( transactionIsCurrent() )
                poisonWithActual(nullptr);
            return false;
        }
        if ( actual == element &&
             wxWinUISemanticPairIsValid(element, semanticTarget) )
            return commitRequested();
        if ( actual == oldContent )
            return noteOldRestored(true);

        // Phase 3 -- restore the old element: a failed swap must leave the
        // slot as it was (m_content and the caches still describe it).
        try
        {
            wxWinUIMaybeInjectContentFault(
                wxWinUITopLevelHost::TestContentFault_Restore);
            m_carrier.Content(oldContent);
            if ( !transactionIsCurrent() )
                return false;
            return noteOldRestored(true);
        }
        catch ( const winrt::hresult_error& restoreError )
        {
            wxWinUILogException("slot content restore", restoreError);

            if ( !transactionIsCurrent() )
                return false;

            actual = nullptr;
            if ( !TryGetCarrierContent(actual) ||
                 !transactionIsCurrent() )
            {
                if ( transactionIsCurrent() )
                    poisonWithActual(nullptr);
                return false;
            }
            if ( actual == oldContent )
                return noteOldRestored(true);
            if ( actual == element &&
                 wxWinUISemanticPairIsValid(element, semanticTarget) )
                return commitRequested();

            // Neither contract state is true. Preserve what is really in the
            // carrier for diagnostics, invalidate callbacks and force
            // RegisterSlot() to rebuild the slot instead of guessing.
            poisonWithActual(actual);
            return false;
        }
    }

    // A null replacement has no phase-2 setter, so expose the same
    // deterministic reentrancy boundary after the successful detach.
    if ( !element )
    {
        std::function<void (wxWindow *)> carrierSetHook =
            std::move(gs_testContentCarrierSetHook);
        if ( carrierSetHook )
        {
            wxWindow * const window =
                m_lifetime ? m_lifetime->GetWindow() : nullptr;
            carrierSetHook(window);
            if ( !transactionIsCurrent() )
                return false;
        }
    }

    return commitRequested();
}

bool wxWinUISlot::TryGetCarrierContent(
    winrt::Microsoft::UI::Xaml::UIElement& content) const
{
    content = nullptr;
    if ( !m_carrier )
        return false;

    try
    {
        wxWinUIMaybeInjectContentFault(
            wxWinUITopLevelHost::TestContentFault_Read);
        content = m_carrier.Content().
            try_as<winrt::Microsoft::UI::Xaml::UIElement>();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxWinUISlot::SyncContentFromCarrier()
{
    const auto previousContent = m_content;
    const auto previousSemanticTarget = m_semanticTarget;
    winrt::Microsoft::UI::Xaml::UIElement actual{ nullptr };
    if ( !TryGetCarrierContent(actual) )
    {
        // The carrier cannot even be READ: its XAML state is now unknowable.
        // Poison the slot -- invalidate its lifetime so no callback can
        // reach wx through it any more, and flag it so the next
        // RegisterSlot() disposes of it and builds a fresh one rather than
        // reusing this corpse (a plain SetContent() on it would leave every
        // handler bound to the dead lifetime).
        if ( m_lifetime )
            m_lifetime->Invalidate();
        m_poisoned = true;
        actual = nullptr;
    }
    RevokeAutomationNameStylePropertyObserver();
    RevokeContentLoadedObserver();
    m_content = actual;
    m_semanticTarget =
        actual &&
        actual == previousContent &&
        wxWinUIElementIsInSubtree(previousSemanticTarget, actual)
            ? previousSemanticTarget
            : actual;
    AdvanceContentGeneration();
    ResetContentCaches();
    InstallToolTipPropertyObserver();
    InstallAutomationNameStylePropertyObserver();
    InstallContentLoadedObserver();
}

void wxWinUISlot::PublishAutomationNameState(
    bool synced,
    bool owned,
    bool appOwned,
    bool relinquished,
    bool suppressedByLabeledBy,
    const wxString& lastDesired,
    const wxString& lastApplied,
    const winrt::Windows::Foundation::IInspectable& originalLocalValue,
    unsigned long long implicitStyleInspectedRevision)
{
    m_uiaNameSynced = synced;
    m_uiaNameOwned = owned;
    m_uiaNameAppOwned = appOwned;
    m_uiaNameRelinquished = relinquished;
    m_uiaNameSuppressedByLabeledBy = suppressedByLabeledBy;
    m_lastUIAName = lastDesired;
    m_lastAppliedUIAName = lastApplied;
    m_originalUIANameLocalValue = originalLocalValue;
    m_automationNameImplicitStyleInspectedRevision =
        implicitStyleInspectedRevision;
    if ( ++m_uiaNameStateRevision == 0 )
        ++m_uiaNameStateRevision;
}

void wxWinUISlot::ResetContentCaches()
{
    m_contentLoaded = false;
    m_contentLoadedObserverRetryFailures = 0;
    m_contentLoadedDuringTransaction = false;
    m_contentLoadedTransactionEpoch = 0;
    m_contentLoadedTransactionGeneration = 0;
    m_toolTipPropertyObserverRetryFailures = 0;
    m_automationNameStylePropertyObserverRetryFailures = 0;
    m_automationNameStyleRevision = 0;
    m_automationNameImplicitStyleRevision = 0;
    m_automationNameImplicitStyleInspectedRevision = 0;
    m_lastEnabled = -1;
    m_toolTipSynced = false;
    m_toolTipOwned = false;
    m_toolTipSuppressed = false;
    m_lastToolTipPresent = false;
    m_lastToolTip.clear();
    m_lastToolTipMaximumWidth = 0;
    m_lastToolTipDPI = 0;
    m_lastAppliedToolTip = nullptr;
    m_originalToolTip = nullptr;
    m_originalToolTipLocalValue = nullptr;
    m_helpTextSynced = false;
    m_helpTextOwned = false;
    m_lastHelpTextPresent = false;
    m_lastHelpText.clear();
    m_lastAppliedHelpText.clear();
    m_originalHelpTextLocalValue = nullptr;
    PublishAutomationNameState(
        false, false, false, false, false,
        wxString(), wxString(), nullptr, 0);
    m_accessibilityViewSynced = false;
    m_accessibilityViewOwned = false;
    m_originalAccessibilityViewLocalValue = nullptr;
    m_pointerCursorVerdictValid = false;
    m_pointerCursorVerdict = nullptr;
    m_pointerCursorVerdictHandle = nullptr;
    m_pointerCursorVerdictKeepAlive = wxNullCursor;
    // A preferred target belongs to one concrete content tree.  Successful
    // replacement invalidates it; failed replacement doesn't call this and
    // therefore preserves the old target transactionally.
    m_preferredFocus = nullptr;
}

bool wxWinUISlot::RestoreToolTip()
{
#if wxUSE_TOOLTIPS
    if ( !m_toolTipSynced || !m_content )
        return true;

    // Restoring the application baseline is part of the host's content
    // transaction, not a new application-authored ToolTipProperty write.
    // In particular, cross-TLW migration while global suppression is active
    // must leave the exact baseline attached long enough for the destination
    // slot to adopt it. Prevent the property observer from recursively
    // suppressing this transient host write before the detach completes.
    ++m_toolTipPolicySyncDepth;
    const auto toolTipPolicySyncGuard = wxMakeGuard([this]()
    {
        wxASSERT_MSG(m_toolTipPolicySyncDepth != 0,
                     "unbalanced WinUI tooltip restore transaction");
        --m_toolTipPolicySyncDepth;
    });
    wxUnusedVar(toolTipPolicySyncGuard);

    const auto content = m_content;
    const unsigned long long generation = m_contentGeneration;
    bool ok = true;

    if ( m_toolTipOwned && m_lastAppliedToolTip )
    {
        ok = wxWinUIRestoreToolTip(
            content,
            m_originalToolTipLocalValue,
            m_lastAppliedToolTip);
    }
    else if ( m_toolTipOwned )
    {
        try
        {
            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;
            // Explicit SetToolTip("") owns the attached null. Do not restore
            // an older baseline over a direct application replacement.
            if ( !ToolTipService::GetToolTip(content) &&
                 !wxWinUIReadToolTipLocalValue(content) )
            {
                wxWinUIRestoreToolTipLocalValue(
                    content, m_originalToolTipLocalValue);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("restore slot empty tooltip mask", e);
            ok = false;
        }
    }
    else if ( m_toolTipSuppressed )
    {
        // An application-authored tooltip was detached by the global disable
        // policy. Once the element leaves the host, restore that exact object:
        // wx must not leak a process-policy null onto application-owned XAML.
        try
        {
            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;
            if ( !ToolTipService::GetToolTip(content) &&
                 !wxWinUIReadToolTipLocalValue(content) )
            {
                wxWinUIRestoreToolTipLocalValue(
                    content, m_originalToolTipLocalValue);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("restore slot authored tooltip", e);
            ok = false;
        }
    }

    if ( !ok || m_content != content || m_contentGeneration != generation )
        return false;

    m_toolTipSynced = false;
    m_toolTipOwned = false;
    m_toolTipSuppressed = false;
    m_lastToolTipPresent = false;
    m_lastToolTip.clear();
    m_lastToolTipMaximumWidth = 0;
    m_lastToolTipDPI = 0;
    m_lastAppliedToolTip = nullptr;
    m_originalToolTip = nullptr;
    m_originalToolTipLocalValue = nullptr;
#endif
    return true;
}

bool wxWinUISlot::RestoreHelpText()
{
    if ( !m_helpTextSynced || !m_semanticTarget )
        return true;

    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace MUX = winrt::Microsoft::UI::Xaml;

    const auto content = m_semanticTarget;
    const unsigned long long generation = m_contentGeneration;
    try
    {
        if ( m_helpTextOwned )
        {
            const auto property =
                Automation::AutomationProperties::HelpTextProperty();
            const auto actualLocal = content.ReadLocalValue(property);
            const bool actualHasLocal =
                actualLocal != MUX::DependencyProperty::UnsetValue();
            const wxString actual(
                Automation::AutomationProperties::GetHelpText(content).c_str());

            // Only the exact value last published by wx is ours to restore.
            if ( actualHasLocal && actual == m_lastAppliedHelpText )
            {
                if ( !m_originalHelpTextLocalValue ||
                     m_originalHelpTextLocalValue ==
                         MUX::DependencyProperty::UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(
                        property, m_originalHelpTextLocalValue);
                }
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot HelpText", e);
        return false;
    }

    if ( m_semanticTarget != content ||
         m_contentGeneration != generation )
        return false;

    m_helpTextSynced = false;
    m_helpTextOwned = false;
    m_lastHelpTextPresent = false;
    m_lastHelpText.clear();
    m_lastAppliedHelpText.clear();
    m_originalHelpTextLocalValue = nullptr;
    return true;
}

bool wxWinUISlot::RestoreAutomationName()
{
    if ( !m_uiaNameSynced || !m_semanticTarget )
        return true;

    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace MUX = winrt::Microsoft::UI::Xaml;

    const auto content = m_semanticTarget;
    const auto lifetime = m_lifetime;
    const unsigned long long generation = m_contentGeneration;
    unsigned long long expectedStateRevision = m_uiaNameStateRevision;
    const bool previousSynced = m_uiaNameSynced;
    const bool previousOwned = m_uiaNameOwned;
    const bool previousAppOwned = m_uiaNameAppOwned;
    const bool previousRelinquished = m_uiaNameRelinquished;
    const bool previousSuppressedByLabeledBy =
        m_uiaNameSuppressedByLabeledBy;
    const wxString previousDesired = m_lastUIAName;
    const wxString previousApplied = m_lastAppliedUIAName;
    const auto previousOriginalLocalValue =
        m_originalUIANameLocalValue;
    const unsigned long long previousInspectedRevision =
        m_automationNameImplicitStyleInspectedRevision;
    winrt::Windows::Foundation::IInspectable preWriteLocal{ nullptr };
    bool setterAttempted = false;

    const auto operationIsCurrent = [&]()
    {
        return !m_poisoned &&
               m_lifetime == lifetime &&
               lifetime &&
               m_semanticTarget == content &&
               m_contentGeneration == generation &&
               m_uiaNameStateRevision == expectedStateRevision;
    };

    try
    {
        if ( m_uiaNameOwned )
        {
            const auto property =
                Automation::AutomationProperties::NameProperty();
            wxWinUIMaybeInjectContentFault(
                wxWinUITopLevelHost::
                    TestContentFault_AutomationNameRestoreRead);
            const auto actualLocal =
                content.ReadLocalValue(property);
            wxString actualLocalString;
            const bool hasBinding =
                wxWinUIHasBinding(content, property);
            if ( !operationIsCurrent() )
                return false;

            // Compare the LOCAL scalar, not GetName()'s effective value: an
            // animation may temporarily override the effective name while the
            // local value is still exactly the one supplied by wx. Leaving
            // that local behind would make it leak back after detach when the
            // animation ends. A direct application replacement (including a
            // local empty string or Binding) is preserved.
            if ( !hasBinding &&
                 actualLocal != MUX::DependencyProperty::UnsetValue() &&
                 wxWinUITryGetLocalStringValue(
                     actualLocal, &actualLocalString) &&
                 actualLocalString == m_lastAppliedUIAName )
            {
                preWriteLocal = actualLocal;
                setterAttempted = true;
                // Publish the logical result before crossing the setter. A
                // property callback may call FlushSync() synchronously: it
                // must see that the old scalar is already relinquished while
                // retaining the current desired-name cursor, otherwise it can
                // either classify wx's transient value as application-owned
                // or immediately reclaim the value being restored.
                PublishAutomationNameState(
                    true, false, false, false, false,
                    previousDesired, wxString(), nullptr,
                    previousInspectedRevision);
                expectedStateRevision = m_uiaNameStateRevision;

                if ( !previousOriginalLocalValue ||
                     previousOriginalLocalValue ==
                         MUX::DependencyProperty::UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(
                        property, previousOriginalLocalValue);
                }

                // A nested pass is the newer writer. Leave its complete tuple
                // intact and let the surrounding content transaction retry
                // from that state instead of publishing an obsolete reset.
                if ( !operationIsCurrent() )
                    return false;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot automation Name", e);

        // WinRT setters are allowed to throw before or after mutation. Restore
        // the old provenance only when the exact slot and exact pre-write
        // local object are still present; otherwise the last writer wins.
        if ( operationIsCurrent() )
        {
            try
            {
                const auto property =
                    Automation::AutomationProperties::NameProperty();
                const auto actualLocal = content.ReadLocalValue(property);
                if ( operationIsCurrent() &&
                     setterAttempted &&
                     actualLocal == preWriteLocal &&
                     !wxWinUIHasBinding(content, property) )
                {
                    wxString actualLocalString;
                    if ( wxWinUITryGetLocalStringValue(
                             actualLocal, &actualLocalString) &&
                         actualLocalString == previousApplied )
                    {
                        PublishAutomationNameState(
                            previousSynced,
                            previousOwned,
                            previousAppOwned,
                            previousRelinquished,
                            previousSuppressedByLabeledBy,
                            previousDesired,
                            previousApplied,
                            previousOriginalLocalValue,
                            previousInspectedRevision);
                        expectedStateRevision = m_uiaNameStateRevision;
                    }
                }
            }
            catch ( const winrt::hresult_error& nested )
            {
                wxWinUILogException(
                    "inspect failed automation Name restore", nested);
            }
        }
        if ( operationIsCurrent() )
            NudgeDirty();
        return false;
    }

    if ( !operationIsCurrent() )
        return false;

    PublishAutomationNameState(
        false, false, false, false, false,
        wxString(), wxString(), nullptr,
        m_automationNameImplicitStyleInspectedRevision);
    return true;
}

bool wxWinUISlot::ShouldPreserveAutomationNameRelinquishment() const
{
    if ( m_uiaNameRelinquished )
        return true;

    // A direct ClearValue() is intentionally indistinguishable from the
    // dependency property's empty default once the slot is rebuilt. Detect
    // it while the old ownership record still tells us which exact local
    // scalar wx supplied. This also covers a direct replacement performed
    // immediately before a rollback or migration, without requiring a prior
    // coalesced SyncSlot pass.
    if ( !m_uiaNameSynced || !m_uiaNameOwned || !m_semanticTarget )
        return false;

    try
    {
        namespace Automation =
            winrt::Microsoft::UI::Xaml::Automation;
        namespace MUX = winrt::Microsoft::UI::Xaml;

        wxWinUIMaybeInjectContentFault(
            wxWinUITopLevelHost::
                TestContentFault_AutomationNameTransferRead);
        const auto local = m_semanticTarget.ReadLocalValue(
            Automation::AutomationProperties::NameProperty());
        wxString localString;
        const bool hasBinding = wxWinUIHasBinding(
            m_semanticTarget,
            Automation::AutomationProperties::NameProperty());
        const bool localStillExact =
            !hasBinding &&
            local != MUX::DependencyProperty::UnsetValue() &&
            wxWinUITryGetLocalStringValue(local, &localString) &&
            localString == m_lastAppliedUIAName;
        const wxString effective(
            Automation::AutomationProperties::GetName(
                m_semanticTarget).c_str());
        const bool implicitStyleNeedsInspection =
            m_automationNameImplicitStyleRevision !=
                m_automationNameImplicitStyleInspectedRevision;
        return !localStillExact ||
               effective != m_lastAppliedUIAName ||
               wxWinUIStyleAuthorsAutomationName(
                   m_semanticTarget,
                   implicitStyleNeedsInspection,
                   m_container.Resources());
    }
    catch ( const winrt::hresult_error& e )
    {
        // Accessibility suppression must fail closed: if the current
        // application/wx ownership cannot be distinguished, never recreate
        // narration merely because the slot is being moved or rolled back.
        wxWinUILogException(
            "read automation Name transfer state", e);
        return true;
    }
}

void wxWinUISlot::SeedAutomationNameRelinquishment(
    const wxString& lastDesiredName)
{
    PublishAutomationNameState(
        true, false, true, true, false,
        lastDesiredName, wxString(), nullptr,
        m_automationNameImplicitStyleInspectedRevision);
}

bool wxWinUISlot::RestoreAccessibilityView()
{
    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace Peers =
        winrt::Microsoft::UI::Xaml::Automation::Peers;

    if ( !m_accessibilityViewSynced || !m_accessibilityViewOwned ||
            !m_semanticTarget )
    {
        m_accessibilityViewSynced = false;
        m_accessibilityViewOwned = false;
        m_originalAccessibilityViewLocalValue = nullptr;
        return true;
    }

    const auto content = m_semanticTarget;
    const unsigned long long generation = m_contentGeneration;
    try
    {
        const auto actual =
            Automation::AutomationProperties::GetAccessibilityView(content);

        // A non-Raw value was written directly by the application while the
        // native tree was authoritative. It already is the value to preserve,
        // so do not overwrite it with our older snapshot.
        if ( actual == Peers::AccessibilityView::Raw )
        {
            const auto property =
                Automation::AutomationProperties::AccessibilityViewProperty();
            if ( m_originalAccessibilityViewLocalValue ==
                    winrt::Microsoft::UI::Xaml::DependencyProperty::
                        UnsetValue() )
            {
                content.ClearValue(property);
            }
            else
            {
                content.SetValue(
                    property, m_originalAccessibilityViewLocalValue);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot AccessibilityView", e);
        return false;
    }

    // Any XAML property callback is arbitrary application code. Only retire
    // the cache if it still describes the same attached element.
    if ( m_semanticTarget != content ||
         m_contentGeneration != generation )
        return false;

    m_accessibilityViewSynced = false;
    m_accessibilityViewOwned = false;
    m_originalAccessibilityViewLocalValue = nullptr;
    return true;
}

void wxWinUISlot::NudgeDirty()
{
    // One coalesced re-sync pushes geometry and state to the content.
    if ( m_lifetime && m_lifetime->GetHost() && m_window )
        m_lifetime->GetHost()->MarkDirty(m_window);
}

// ============================================================================
// fractional, identity-checked coordinate mapping
// ============================================================================

struct wxWinUIVisualCoordinates::Snapshot
{
    wxWeakRef<wxWindow> anchor;
    wxWeakRef<wxWindow> tlw;
    wxWindow *anchorIdentity = nullptr;
    wxWindow *tlwIdentity = nullptr;
    wxWinUITopLevelHost *host = nullptr;
    wxWinUISlot *slot = nullptr;
    std::shared_ptr<wxWinUIHostLifetime> hostLifetime;
    std::shared_ptr<wxWinUISlotLifetime> slotLifetime;
    winrt::Microsoft::UI::Xaml::Controls::Canvas root{ nullptr };
    winrt::Microsoft::UI::Xaml::XamlRoot xamlRoot{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement content{ nullptr };
    winrt::Windows::Foundation::Rect rootScreenPixels{};
    winrt::Windows::Foundation::Rect anchorScreenPixels{};
    HWND anchorHwnd = nullptr;
    HWND tlwHwnd = nullptr;
    unsigned long long anchorHwndGeneration = 0;
    unsigned long long tlwHwndGeneration = 0;
    unsigned long long contentGeneration = 0;
    unsigned long long syncEpoch = 0;
    unsigned long long structureGeneration = 0;
    double scale = 0.0;
    bool anchorMirrored = false;
};

wxWinUITopLevelHost *
wxWinUIVisualCoordinates::ResolveHost(wxWindow *anchor)
{
    if ( !anchor )
        return nullptr;

    if ( wxWinUITopLevelHost * const owner =
             wxWinUITopLevelHost::FindSlotOwner(anchor) )
    {
        return owner;
    }

    wxWindow * const tlw =
        anchor->IsTopLevel() ? anchor : wxGetTopLevelParent(anchor);
    return tlw ? wxWinUITopLevelHost::FindForTLW(tlw) : nullptr;
}

bool wxWinUIVisualCoordinates::TryGetPhysicalClientRect(
    wxWindow *window,
    winrt::Windows::Foundation::Rect *screenPixels,
    bool *mirrored)
{
    if ( !window || !screenPixels || !mirrored )
        return false;

    const HWND hwnd = GetHwndOf(window);
    if ( !hwnd )
        return false;

    RECT rect{};
    if ( !::GetClientRect(hwnd, &rect) )
        return false;

    ::SetLastError(ERROR_SUCCESS);
    const int mapped = ::MapWindowPoints(
        hwnd, HWND_DESKTOP, reinterpret_cast<POINT *>(&rect), 2);
    if ( mapped == 0 && ::GetLastError() != ERROR_SUCCESS )
        return false;

    const LONG left = (std::min)(rect.left, rect.right);
    const LONG right = (std::max)(rect.left, rect.right);
    const LONG top = (std::min)(rect.top, rect.bottom);
    const LONG bottom = (std::max)(rect.top, rect.bottom);

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR exStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ( exStyle == 0 && ::GetLastError() != ERROR_SUCCESS )
        return false;

    *screenPixels = {
        static_cast<float>(left),
        static_cast<float>(top),
        static_cast<float>(right - left),
        static_cast<float>(bottom - top)
    };
    *mirrored = (exStyle & WS_EX_LAYOUTRTL) != 0;
    return true;
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::Capture(
    wxWindow *anchor,
    wxWinUITopLevelHost *host,
    wxWinUISlot *slot,
    bool requireSlot,
    Snapshot *snapshot)
{
    if ( !anchor || !host || !snapshot || (requireSlot && !slot) )
        return wxWinUICoordinateResult::Unavailable;

    Snapshot current;
    current.anchor = anchor;
    current.anchorIdentity = anchor;
    current.host = host;
    current.slot = slot;
    current.hostLifetime = host->m_hostLifetime;
    current.tlwIdentity = host->m_tlw;
    current.tlw = current.tlwIdentity;
    current.root = host->m_root;
    current.structureGeneration = host->m_structureGeneration;

    if ( !current.hostLifetime ||
         current.hostLifetime->GetHost() != host ||
         host->m_shuttingDown || !current.tlwIdentity || !current.root )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    current.anchorHwnd = GetHwndOf(anchor);
    current.tlwHwnd = GetHwndOf(current.tlwIdentity);
    if ( !current.anchorHwnd || !current.tlwHwnd )
        return wxWinUICoordinateResult::Unavailable;

    current.anchorHwndGeneration =
        wxWinUIMSWGetHwndGeneration(
            anchor, reinterpret_cast<WXHWND>(current.anchorHwnd));
    current.tlwHwndGeneration =
        wxWinUIMSWGetHwndGeneration(
            current.tlwIdentity,
            reinterpret_cast<WXHWND>(current.tlwHwnd));
    if ( !current.anchorHwndGeneration || !current.tlwHwndGeneration )
        return wxWinUICoordinateResult::Unavailable;

    if ( slot )
    {
        current.slotLifetime = slot->m_lifetime;
        current.content = slot->m_content;
        current.contentGeneration = slot->m_contentGeneration;
        current.syncEpoch = slot->m_syncEpoch;
        if ( !current.slotLifetime ||
             current.slotLifetime->GetHost() != host ||
             current.slotLifetime->GetWindow() != anchor ||
             (requireSlot && !current.content) )
        {
            return wxWinUICoordinateResult::Unavailable;
        }
    }

    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        current.xamlRoot = current.root.XamlRoot();
        if ( !current.xamlRoot || !IsCurrent(current) )
            return wxWinUICoordinateResult::Superseded;

        current.scale = current.xamlRoot.RasterizationScale();
        if ( !(current.scale > 0.0) || !std::isfinite(current.scale) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(current) )
            return wxWinUICoordinateResult::Superseded;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }

    bool rootMirrored = false;
    if ( !TryGetPhysicalClientRect(
             current.tlwIdentity,
             &current.rootScreenPixels,
             &rootMirrored) )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    wxUnusedVar(rootMirrored); // XAML root space is always physically LTR.
    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    if ( !TryGetPhysicalClientRect(
             anchor,
             &current.anchorScreenPixels,
             &current.anchorMirrored) )
    {
        return IsCurrent(current)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(current) )
        return wxWinUICoordinateResult::Superseded;

    *snapshot = current;
    return wxWinUICoordinateResult::Mapped;
}

bool wxWinUIVisualCoordinates::IsCurrent(const Snapshot& snapshot)
{
    wxWinUITopLevelHost * const host = snapshot.host;
    const auto baseIsCurrent = [&]() -> bool
    {
        wxWindow * const anchor = snapshot.anchor.get();
        wxWindow * const tlw = snapshot.tlw.get();
        if ( !anchor || anchor != snapshot.anchorIdentity ||
             !tlw || tlw != snapshot.tlwIdentity ||
             !host || !snapshot.hostLifetime ||
             snapshot.hostLifetime->GetHost() != host ||
             host->m_shuttingDown || host->m_tlw != tlw ||
             host->m_root != snapshot.root ||
             host->m_structureGeneration !=
                 snapshot.structureGeneration ||
             GetHwndOf(anchor) != snapshot.anchorHwnd ||
             GetHwndOf(tlw) != snapshot.tlwHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 anchor,
                 reinterpret_cast<WXHWND>(snapshot.anchorHwnd)) !=
                 snapshot.anchorHwndGeneration ||
             wxWinUIMSWGetHwndGeneration(
                 tlw,
                 reinterpret_cast<WXHWND>(snapshot.tlwHwnd)) !=
                 snapshot.tlwHwndGeneration )
        {
            return false;
        }

        if ( snapshot.slot )
        {
            if ( host->FindSlot(anchor) != snapshot.slot ||
                 snapshot.slot->m_lifetime != snapshot.slotLifetime ||
                 !snapshot.slotLifetime ||
                 snapshot.slotLifetime->GetHost() != host ||
                 snapshot.slotLifetime->GetWindow() != anchor ||
                 snapshot.slot->m_content != snapshot.content ||
                 snapshot.slot->m_contentGeneration !=
                     snapshot.contentGeneration ||
                 snapshot.slot->m_syncEpoch != snapshot.syncEpoch )
            {
                return false;
            }
        }
        else if ( host->FindSlot(anchor) )
        {
            return false;
        }

        return wxGetTopLevelParent(anchor) == tlw ||
               (anchor->IsTopLevel() && anchor == tlw);
    };

    if ( !baseIsCurrent() )
        return false;

    // RasterizationScale and XamlRoot can change without a wx geometry event
    // while a window crosses monitors. When they have already been captured,
    // validate them as part of every publication ticket too.
    if ( snapshot.xamlRoot )
    {
        try
        {
            if ( snapshot.root.XamlRoot() != snapshot.xamlRoot )
                return false;
            if ( snapshot.scale > 0.0 )
            {
                const double currentScale =
                    snapshot.xamlRoot.RasterizationScale();
                if ( !std::isfinite(currentScale) ||
                     std::abs(currentScale - snapshot.scale) >
                         0.0000001 )
                {
                    return false;
                }
            }
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
    }

    // XamlRoot/RasterizationScale access is a callback boundary too.
    return baseIsCurrent();
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ClientToScreen(
    const Snapshot& snapshot,
    const Point& clientPixels)
{
    const Rect& rect = snapshot.anchorScreenPixels;
    return {
        snapshot.anchorMirrored
            ? rect.X + rect.Width - clientPixels.X
            : rect.X + clientPixels.X,
        rect.Y + clientPixels.Y
    };
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ScreenToClient(
    const Snapshot& snapshot,
    const Point& screenPixels)
{
    const Rect& rect = snapshot.anchorScreenPixels;
    return {
        snapshot.anchorMirrored
            ? rect.X + rect.Width - screenPixels.X
            : screenPixels.X - rect.X,
        screenPixels.Y - rect.Y
    };
}

wxWinUIVisualCoordinates::Point
wxWinUIVisualCoordinates::ScreenToRoot(
    const Snapshot& snapshot,
    const Point& screenPixels)
{
    return {
        static_cast<float>(
            (screenPixels.X - snapshot.rootScreenPixels.X) /
            snapshot.scale),
        static_cast<float>(
            (screenPixels.Y - snapshot.rootScreenPixels.Y) /
            snapshot.scale)
    };
}

namespace
{

bool wxWinUIIsFinitePoint(
    const winrt::Windows::Foundation::Point& point)
{
    return std::isfinite(point.X) && std::isfinite(point.Y);
}

bool wxWinUIIsFiniteRect(
    const winrt::Windows::Foundation::Rect& rect)
{
    return std::isfinite(rect.X) && std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) && std::isfinite(rect.Height);
}

template<typename Attempt>
wxWinUICoordinateResult wxWinUIRunCoordinateAttempts(Attempt&& attempt)
{
    wxWinUICoordinateResult result =
        wxWinUICoordinateResult::Unavailable;
    for ( unsigned pass = 0; pass < 2; ++pass )
    {
        result = attempt();
        if ( result != wxWinUICoordinateResult::Superseded )
            break;
    }
    return result;
}

} // anonymous namespace

wxWinUICoordinateResult wxWinUIVisualCoordinates::ClientPointToRoot(
    wxWindow *anchor,
    const Point& clientPixels,
    Point *rootDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ClientPointToRootOnce(
                anchor, clientPixels, rootDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ScreenPointToRoot(
    wxWindow *anchor,
    const Point& screenPixels,
    Point *rootDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ScreenPointToRootOnce(
                anchor, screenPixels, rootDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ClientPointToElement(
    wxWindow *anchor,
    const Point& clientPixels,
    const UIElement& element,
    Point *elementDips)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ClientPointToElementOnce(
                anchor, clientPixels, element, elementDips);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ElementPointToClient(
    wxWindow *anchor,
    const UIElement& element,
    const Point& elementDips,
    Point *clientPixels)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ElementPointToClientOnce(
                anchor, element, elementDips, clientPixels);
        });
}

wxWinUICoordinateResult wxWinUIVisualCoordinates::ElementBoundsToClient(
    wxWindow *anchor,
    const UIElement& element,
    const Rect& elementBoundsDips,
    Rect *clientBoundsPixels)
{
    return wxWinUIRunCoordinateAttempts(
        [=]()
        {
            return ElementBoundsToClientOnce(
                anchor, element, elementBoundsDips,
                clientBoundsPixels);
        });
}

bool wxWinUIVisualCoordinates::TestRoundTripPhysicalSpaces(
    const Rect& rootScreenPixels,
    const Rect& anchorScreenPixels,
    bool anchorMirrored,
    double scale,
    const Point& clientPixels,
    Point *rootDips,
    Point *roundTrippedClientPixels)
{
    if ( !rootDips || !roundTrippedClientPixels ||
         !wxWinUIIsFiniteRect(rootScreenPixels) ||
         !wxWinUIIsFiniteRect(anchorScreenPixels) ||
         !wxWinUIIsFinitePoint(clientPixels) ||
         !(scale > 0.0) || !std::isfinite(scale) )
    {
        return false;
    }

    Snapshot snapshot;
    snapshot.rootScreenPixels = rootScreenPixels;
    snapshot.anchorScreenPixels = anchorScreenPixels;
    snapshot.anchorMirrored = anchorMirrored;
    snapshot.scale = scale;

    const Point screen = ClientToScreen(snapshot, clientPixels);
    const Point root = ScreenToRoot(snapshot, screen);
    const Point screenAgain{
        rootScreenPixels.X +
            static_cast<float>(root.X * scale),
        rootScreenPixels.Y +
            static_cast<float>(root.Y * scale)
    };
    const Point roundTrip = ScreenToClient(snapshot, screenAgain);
    if ( !wxWinUIIsFinitePoint(root) ||
         !wxWinUIIsFinitePoint(roundTrip) )
    {
        return false;
    }

    *rootDips = root;
    *roundTrippedClientPixels = roundTrip;
    return true;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ClientPointToRootOnce(
    wxWindow *anchor,
    const Point& clientPixels,
    Point *rootDips)
{
    if ( !rootDips || !wxWinUIIsFinitePoint(clientPixels) )
        return wxWinUICoordinateResult::Unavailable;

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, false, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;

    const Point mapped =
        ScreenToRoot(snapshot, ClientToScreen(snapshot, clientPixels));
    if ( !wxWinUIIsFinitePoint(mapped) )
        return wxWinUICoordinateResult::Unavailable;
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    *rootDips = mapped;
    return wxWinUICoordinateResult::Mapped;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ScreenPointToRootOnce(
    wxWindow *anchor,
    const Point& screenPixels,
    Point *rootDips)
{
    if ( !rootDips || !wxWinUIIsFinitePoint(screenPixels) )
        return wxWinUICoordinateResult::Unavailable;

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, false, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;

    const Point mapped = ScreenToRoot(snapshot, screenPixels);
    if ( !wxWinUIIsFinitePoint(mapped) )
        return wxWinUICoordinateResult::Unavailable;
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    *rootDips = mapped;
    return wxWinUICoordinateResult::Mapped;
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ClientPointToElementOnce(
    wxWindow *anchor,
    const Point& clientPixels,
    const UIElement& element,
    Point *elementDips)
{
    if ( !elementDips || !element ||
         !wxWinUIIsFinitePoint(clientPixels) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const Point rootPoint =
            ScreenToRoot(snapshot, ClientToScreen(snapshot, clientPixels));
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const auto rootToElement = elementToRoot.Inverse();
        if ( !rootToElement )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Point mapped = rootToElement.TransformPoint(rootPoint);
        if ( !wxWinUIIsFinitePoint(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *elementDips = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ElementPointToClientOnce(
    wxWindow *anchor,
    const UIElement& element,
    const Point& elementDips,
    Point *clientPixels)
{
    if ( !clientPixels || !element ||
         !wxWinUIIsFinitePoint(elementDips) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Point rootPoint = elementToRoot.TransformPoint(elementDips);
        if ( !wxWinUIIsFinitePoint(rootPoint) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        const Point screenPoint{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootPoint.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootPoint.Y * snapshot.scale)
        };
        const Point mapped = ScreenToClient(snapshot, screenPoint);
        if ( !wxWinUIIsFinitePoint(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *clientPixels = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

wxWinUICoordinateResult
wxWinUIVisualCoordinates::ElementBoundsToClientOnce(
    wxWindow *anchor,
    const UIElement& element,
    const Rect& elementBoundsDips,
    Rect *clientBoundsPixels)
{
    if ( !clientBoundsPixels || !element ||
         !wxWinUIIsFiniteRect(elementBoundsDips) )
    {
        return wxWinUICoordinateResult::Unavailable;
    }

    wxWinUITopLevelHost * const host = ResolveHost(anchor);
    if ( !host )
        return wxWinUICoordinateResult::Unavailable;
    wxWinUITopLevelHost::OperationGuard hostOperation(host);
    wxWinUISlot * const slot = host->FindSlot(anchor);
    wxWinUISlot::OperationGuard slotOperation(slot);

    Snapshot snapshot;
    const wxWinUICoordinateResult captured =
        Capture(anchor, host, slot, true, &snapshot);
    if ( captured != wxWinUICoordinateResult::Mapped )
        return captured;
    if ( !wxWinUIElementIsInSubtree(element, snapshot.content) )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
    if ( !IsCurrent(snapshot) )
        return wxWinUICoordinateResult::Superseded;

    try
    {
        const auto elementToRoot = element.TransformToVisual(snapshot.root);
        if ( !elementToRoot )
        {
            return IsCurrent(snapshot)
                ? wxWinUICoordinateResult::Unavailable
                : wxWinUICoordinateResult::Superseded;
        }
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;
        const Rect rootBounds =
            elementToRoot.TransformBounds(elementBoundsDips);
        if ( !wxWinUIIsFiniteRect(rootBounds) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        const Point rootTopLeft{rootBounds.X, rootBounds.Y};
        const Point rootBottomRight{
            rootBounds.X + rootBounds.Width,
            rootBounds.Y + rootBounds.Height
        };
        const Point screenTopLeft{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootTopLeft.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootTopLeft.Y * snapshot.scale)
        };
        const Point screenBottomRight{
            snapshot.rootScreenPixels.X +
                static_cast<float>(rootBottomRight.X * snapshot.scale),
            snapshot.rootScreenPixels.Y +
                static_cast<float>(rootBottomRight.Y * snapshot.scale)
        };
        const Point clientA =
            ScreenToClient(snapshot, screenTopLeft);
        const Point clientB =
            ScreenToClient(snapshot, screenBottomRight);

        const float left = (std::min)(clientA.X, clientB.X);
        const float top = (std::min)(clientA.Y, clientB.Y);
        const float right = (std::max)(clientA.X, clientB.X);
        const float bottom = (std::max)(clientA.Y, clientB.Y);
        const Rect mapped{left, top, right - left, bottom - top};
        if ( !wxWinUIIsFiniteRect(mapped) )
            return wxWinUICoordinateResult::Unavailable;
        if ( !IsCurrent(snapshot) )
            return wxWinUICoordinateResult::Superseded;

        *clientBoundsPixels = mapped;
        return wxWinUICoordinateResult::Mapped;
    }
    catch ( const winrt::hresult_error& )
    {
        return IsCurrent(snapshot)
            ? wxWinUICoordinateResult::Unavailable
            : wxWinUICoordinateResult::Superseded;
    }
}

// ============================================================================
// wxWinUITopLevelHost: lifetime
// ============================================================================

wxWinUITopLevelHost::OperationGuard::OperationGuard(
    wxWinUITopLevelHost *host)
    : m_host(host)
{
    if ( m_host )
    {
        ++m_host->m_operationDepth;
        wxWinUIBeginGlobalOperation();
    }
}

wxWinUITopLevelHost::OperationGuard::~OperationGuard()
{
    if ( m_host )
        m_host->EndOperation();
}

void wxWinUITopLevelHost::EndOperation()
{
    wxASSERT_MSG(m_operationDepth > 0, "unbalanced WinUI host operation");
    if ( m_operationDepth == 0 )
        return;

    --m_operationDepth;
    wxWinUIEndGlobalOperation();
}

void
wxWinUITopLevelHost::SchedulePendingDeletesAfterGlobalOperation()
{
    if ( gs_winuiOperationDepth != 0 || gs_hostsPendingDelete.empty() )
        return;

    // ScheduleDeferredDelete() can synchronously diagnose a missing wxApp,
    // and a host may disappear while another scheduled teardown is drained.
    // Iterate a snapshot and revalidate membership before dereferencing.
    const std::vector<wxWinUITopLevelHost *> pending(
        gs_hostsPendingDelete.begin(), gs_hostsPendingDelete.end());
    for ( wxWinUITopLevelHost * const host : pending )
    {
        if ( gs_hostsPendingDelete.find(host) !=
                 gs_hostsPendingDelete.end() &&
             host->m_deleteRequested )
        {
            host->ScheduleDeferredDelete();
        }
    }
}

namespace
{

auto wxWinUIFindDeferredTLWDestroy(const wxWindow *window)
{
    return std::find_if(
        gs_deferredTLWDestroyQueue.begin(),
        gs_deferredTLWDestroyQueue.end(),
        [window](const wxWinUIDeferredTLWDestroy& queued)
        {
            return queued.window.get() == window;
        });
}

bool wxWinUIHasDeferredTLWDestroy(const wxWindow *window)
{
    return window &&
           wxWinUIFindDeferredTLWDestroy(window) !=
               gs_deferredTLWDestroyQueue.end();
}

void wxWinUIQueueDeferredTLWDestroy(
    wxWindow *window,
    wxWinUIDeferredTLWDestroyMode mode)
{
    if ( !window )
        return;

    const auto alreadyQueued = wxWinUIFindDeferredTLWDestroy(window);
    if ( alreadyQueued == gs_deferredTLWDestroyQueue.end() )
    {
        gs_deferredTLWDestroyQueue.emplace_back(window, mode);
        return;
    }

    // The modal preamble is the only request with an additional obligation.
    // Otherwise the first request is authoritative: in particular, a TLW
    // shielded after its ordinary Destroy()/Hide() must not be hidden again.
    if ( mode == wxWinUIDeferredTLWDestroyMode::CompleteModalDestroy )
        alreadyQueued->mode = mode;
}

void wxWinUIProtectPendingTopLevelDestroys()
{
    // wxPendingDelete is process-wide. A nested wxYield() in any WinUI
    // callback can drain another top-level object too. wxTopLevelWindows is
    // not authoritative here: popup windows are deliberately absent from it,
    // and a modal dialog can have removed its logical host already. Snapshot
    // the deletion list itself, then temporarily disarm every top-level wx
    // window before arbitrary nested dispatch can run.
    std::vector<wxWeakRef<wxWindow>> pendingWindows;
    for ( wxObjectList::iterator it = wxPendingDelete.begin();
          it != wxPendingDelete.end();
          ++it )
    {
        wxWindow * const window = wxDynamicCast(*it, wxWindow);
        if ( window && window->IsTopLevel() )
            pendingWindows.emplace_back(window);
    }

    for ( const wxWeakRef<wxWindow>& weakWindow : pendingWindows )
    {
        wxWindow * const window = weakWindow.get();
        if ( !window || !wxPendingDelete.Member(window) )
            continue;

        wxPendingDelete.DeleteObject(window);
        wxWinUIQueueDeferredTLWDestroy(
            window,
            wxWinUIDeferredTLWDestroyMode::RearmPendingDelete);
    }
}

void wxWinUIBeginGlobalOperation()
{
    ++gs_winuiOperationDepth;
    if ( gs_winuiOperationDepth == 1 )
        wxWinUIProtectPendingTopLevelDestroys();
}

void wxWinUIEndGlobalOperation()
{
    wxASSERT_MSG(gs_winuiOperationDepth > 0,
                 "unbalanced global WinUI host operation");
    if ( gs_winuiOperationDepth == 0 )
        return;

    --gs_winuiOperationDepth;
    if ( gs_winuiOperationDepth != 0 )
        return;

    wxWinUITopLevelHost::SchedulePendingDeletesAfterGlobalOperation();
    wxWinUITopLevelHost::ScheduleNativeResizeWakesAfterGlobalOperation();
    wxWinUIScheduleDeferredTLWDestroyReplay();
}

void wxWinUIScheduleDeferredTLWDestroyReplay()
{
    if ( gs_winuiOperationDepth != 0 ||
         gs_deferredTLWDestroyReplayScheduled ||
         gs_deferredTLWDestroyQueue.empty() )
    {
        return;
    }

    wxCHECK_RET(
        wxTheApp,
        "live WinUI host requires a wxApp for deferred TLW destroy");
    gs_deferredTLWDestroyReplayScheduled = true;
    wxTheApp->CallAfter(
        []()
        {
            if ( gs_winuiOperationDepth != 0 )
            {
                // The matching transaction tail owns the next scheduling
                // edge. Reposting here would create a CallAfter spin under a
                // nested wxYield() while the retained XAML callback remains
                // on the stack.
                gs_deferredTLWDestroyReplayScheduled = false;
                return;
            }

            // Keep the scheduled latch set throughout the batch. Completion
            // can synchronously dispatch wxEVT_SHOW/DESTROY; if that callback
            // queues another TLW, the final reschedule below owns it once.
            std::vector<wxWinUIDeferredTLWDestroy> pending =
                std::move(gs_deferredTLWDestroyQueue);
            gs_deferredTLWDestroyQueue.clear();
            for ( const wxWinUIDeferredTLWDestroy& request : pending )
            {
                wxWindow *window = request.window.get();
                if ( !window )
                    continue;

                if ( request.mode ==
                         wxWinUIDeferredTLWDestroyMode::RearmPendingDelete )
                {
                    // Popup parents can enter synchronous destruction while
                    // the global host operation is retained. Their delayed
                    // delete must then become immediate, using the same safe
                    // replay as an explicitly deferred popup request.
                    if ( wxWinUIMSWCompleteDeferredPopupDestroy(
                             window,
                             wxWinUIPopupDestroySemantics::PendingDelete) )
                    {
                        continue;
                    }

                    // IsBeingDeleted() is expected for a modal preamble and
                    // must not suppress re-arming the authoritative deletion
                    // list after the retained callback has unwound.
                    if ( !wxPendingDelete.Member(window) )
                        wxPendingDelete.Append(window);
                    wxWakeUpIdle();
                    continue;
                }

                const bool modal =
                    request.mode ==
                    wxWinUIDeferredTLWDestroyMode::CompleteModalDestroy;
                if ( modal )
                {
                    // wxDialog owns its protected most-derived destroy-event
                    // preamble and then enters the same no-virtual-replay MSW
                    // completion helper.
                    if ( wxDialog * const dialog =
                             wxDynamicCast(window, wxDialog) )
                    {
                        dialog->WinUICompleteDeferredModalDestroy();
                    }
                    continue;
                }

                if ( request.mode ==
                        wxWinUIDeferredTLWDestroyMode::
                            CompletePopupImmediateDestroy ||
                     request.mode ==
                        wxWinUIDeferredTLWDestroyMode::
                            CompletePopupPendingDestroy )
                {
                    const wxWinUIPopupDestroySemantics semantics =
                        request.mode ==
                            wxWinUIDeferredTLWDestroyMode::
                                CompletePopupImmediateDestroy
                            ? wxWinUIPopupDestroySemantics::Immediate
                            : wxWinUIPopupDestroySemantics::PendingDelete;
                    wxWinUIMSWCompleteDeferredPopupDestroy(window, semantics);
                    continue;
                }

                if ( window->IsBeingDeleted() )
                    continue;

                wxWinUIMSWCompleteDeferredTopLevelDestroy(
                    window, true, false);
            }

            gs_deferredTLWDestroyReplayScheduled = false;
            wxWinUIScheduleDeferredTLWDestroyReplay();
        });
}

} // anonymous namespace

void wxWinUITopLevelHost::RequestDeferredDelete()
{
    m_deleteRequested = true;
    gs_hostsPendingDelete.insert(this);
    ScheduleDeferredDelete();
}

void wxWinUITopLevelHost::ScheduleDeferredDelete()
{
    if ( !m_deleteRequested || m_deleteScheduled ||
         m_operationDepth != 0 || gs_winuiOperationDepth != 0 )
        return;

    wxCHECK_RET(
        wxTheApp,
        "live WinUI host requires a wxApp for deferred deletion");
    m_deleteScheduled = true;
    wxWinUITopLevelHost * const self = this;
    wxTheApp->CallAfter(
        [self]()
        {
            if ( self->m_operationDepth != 0 ||
                 gs_winuiOperationDepth != 0 )
            {
                // Do not form a self-reposting loop under wxYield(). The
                // outermost global operation tail revisits the pending-host
                // set exactly once.
                self->m_deleteScheduled = false;
                return;
            }
            // Keep the latch set throughout destruction. Attached-property
            // restoration can enter/leave another OperationGuard; it must not
            // enqueue a second delete for the object already being destroyed.
            gs_hostsPendingDelete.erase(self);
            delete self;
        });
}

wxWinUITopLevelHost *
wxWinUITopLevelHost::ForWindow(wxWindow *window, bool createIfNeeded)
{
    if ( !window || window->IsBeingDeleted() ||
         wxWinUITLWHostIsDestroyScheduled(window) )
        return nullptr;

    wxWindow * const tlw = wxGetTopLevelParent(window);
    if ( !tlw || tlw->IsBeingDeleted() ||
         wxWinUITLWHostIsDestroyScheduled(tlw) || !tlw->GetHandle() )
        return nullptr;

    const auto it = gs_tlwHosts.find(tlw);
    if ( it != gs_tlwHosts.end() )
        return it->second->IsReadyForLookup() ? it->second : nullptr;

    if ( !createIfNeeded )
        return nullptr;

    wxWinUITopLevelHost * const host = new wxWinUITopLevelHost(tlw);
    bool committed = false;
    const auto rollback = wxMakeGuard(
        [host, &committed]()
        {
            if ( committed )
                return;

            // Keep both the host allocation and every pending TLW deletion
            // protected while the logical rollback crosses title-bar/OLE/
            // XAML boundaries. The destructor performs the physical phase
            // only after this transaction has unwound. If OnTLWDestroy()
            // already requested ownership, its deferred callback is the sole
            // deleter and this guard must not race it.
            {
                OperationGuard operation(host);
                host->Shutdown();
            }
            if ( !host->m_deleteRequested )
                delete host;
        });

    {
        OperationGuard operation(host);

        if ( !host->CaptureInitializationIdentity() )
            return nullptr;

        const auto insertion = gs_tlwHosts.emplace(tlw, host);
        if ( !insertion.second )
            return nullptr;

        // Publish the teardown owner and bind destruction before the first
        // runtime/log/XAML boundary. Lookups still fail closed until the
        // final m_initializationComplete commit below.
        wxWindow * const liveTLW = host->m_tlwLifetime.get();
        if ( !liveTLW || !host->HasInitializationContext() )
            return nullptr;
        liveTLW->Bind(
            wxEVT_DESTROY, &wxWinUITopLevelHost::OnTLWDestroy, host);
        host->m_tlwDestroyBound = true;

        host->InvokeInitializationHookForTest(
            InitializationHookPointForTest::AfterProvisionalPublication);
        if ( !host->HasInitializationContext() || !host->Initialize() ||
             !host->HasInitializationContext() )
        {
            host->m_initializing = false;
            return nullptr;
        }

        if ( host->m_initializationDeferred )
        {
            // XAML initialization is complete even though the independent
            // OLE service still owns an exact PendingFixed claim. Publish the
            // host now so the control which triggered creation can register
            // its slot/content instead of losing that request. The posted OLE
            // continuation only finishes (or rolls back) drop integration.
            host->m_initializationComplete = true;
            host->m_initializing = false;
            committed = true;
            rollback.Dismiss();
            return host;
        }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        if ( host->m_dropBroker )
            host->m_dropBroker->CommitHostInitialization();
#endif
        host->m_initializationComplete = true;
        host->m_initializing = false;
        committed = true;
    }

    rollback.Dismiss();
    return host;
}

wxWinUITopLevelHost *wxWinUITopLevelHost::FindForTLW(const wxWindow *tlw)
{
    const auto it = gs_tlwHosts.find(tlw);
    return it != gs_tlwHosts.end() && it->second->IsReadyForLookup()
        ? it->second
        : nullptr;
}

wxWinUITopLevelHost *
wxWinUITopLevelHost::FindSlotOwner(const wxWindow *window)
{
    if ( !window )
        return nullptr;

    const auto authoritative = gs_slotOwners.find(window);
    if ( authoritative != gs_slotOwners.end() )
    {
        if ( authoritative->second &&
             authoritative->second->FindSlot(window) )
        {
            return authoritative->second;
        }

        // Repair a stale entry defensively; normal disconnect/shutdown paths
        // remove it synchronously.
        gs_slotOwners.erase(authoritative);
    }

    wxWinUITopLevelHost *found = nullptr;
    for ( const auto& kv : gs_tlwHosts )
    {
        if ( kv.second->IsReadyForLookup() &&
             kv.second->FindSlot(window) )
        {
            if ( found )
            {
                wxLogError("wxWinUI: duplicate unowned slots detected for "
                           "window %p", static_cast<const void *>(window));
                return nullptr;
            }
            found = kv.second;
        }
    }

    if ( found )
        gs_slotOwners[window] = found;
    return found;
}

wxWinUITopLevelHost *
wxWinUITopLevelHost::ReconcileSlotOwner(wxWindow *window,
                                        bool createIfNeeded)
{
    wxWinUITopLevelHost * const target =
        ForWindow(window, createIfNeeded);
    wxWinUITopLevelHost * const owner = FindSlotOwner(window);

    if ( !owner )
        return target;
    if ( owner == target )
        return owner;

    // Without a current TLW host there is nowhere truthful to migrate to.
    // Read/teardown callers can still use FindSlotOwner() explicitly.
    if ( !target || !owner->MigrateSlotToCurrentTLW(window) )
        return nullptr;

    return FindSlotOwner(window) == target ? target : nullptr;
}

wxWinUITopLevelHost *wxWinUITopLevelHost::FindForBridge(HWND hwnd)
{
    if ( !hwnd )
        return nullptr;

    for ( const auto& kv : gs_tlwHosts )
    {
        if ( kv.second->IsReadyForLookup() &&
             (kv.second->m_bridge == hwnd || kv.second->m_inner == hwnd) )
            return kv.second;
    }

    return nullptr;
}

wxWinUITopLevelHost::wxWinUITopLevelHost(wxWindow *tlw)
    : m_tlw(tlw),
      m_tlwLifetime(tlw),
      m_hostLifetime(std::make_shared<wxWinUIHostLifetime>(this)),
      m_inputState(wxWinUIGetClickSettings())
{
}

wxWinUITopLevelHost::~wxWinUITopLevelHost()
{
    gs_hostsPendingDelete.erase(this);
    if ( !m_shutdownComplete )
        Shutdown();
}

bool wxWinUITopLevelHost::CaptureInitializationIdentity()
{
    wxWindow * const tlw = m_tlwLifetime.get();
    if ( !tlw || tlw != m_tlw || tlw->IsBeingDeleted() ||
         wxWinUITLWHostIsDestroyScheduled(tlw) )
    {
        return false;
    }

    const HWND hwnd = GetHwndOf(tlw);
    if ( !hwnd || !::IsWindow(hwnd) ||
         ::GetWindowThreadProcessId(hwnd, nullptr) !=
             ::GetCurrentThreadId() ||
         wxFindWinFromHandle(hwnd) != tlw )
    {
        return false;
    }

    m_tlwHwnd = reinterpret_cast<WXHWND>(hwnd);
    m_tlwHwndGeneration =
        wxWinUIMSWGetHwndGeneration(tlw, m_tlwHwnd);
    m_tlwNativeHwndGeneration =
        wxWinUIMSWGetNativeHwndGeneration(m_tlwHwnd);
    return m_tlwHwndGeneration != 0 &&
           m_tlwNativeHwndGeneration != 0 &&
           HasInitializationContext();
}

bool wxWinUITopLevelHost::HasInitializationContext() const
{
    if ( m_shuttingDown || m_deleteRequested || !m_tlw || !m_tlwHwnd ||
         !m_tlwHwndGeneration || !m_tlwNativeHwndGeneration )
    {
        return false;
    }

    wxWindow * const tlw = m_tlwLifetime.get();
    const HWND hwnd = static_cast<HWND>(m_tlwHwnd);
    return tlw && tlw == m_tlw && !tlw->IsBeingDeleted() &&
           !wxWinUITLWHostIsDestroyScheduled(tlw) &&
           GetHwndOf(tlw) == hwnd && ::IsWindow(hwnd) &&
           ::GetWindowThreadProcessId(hwnd, nullptr) ==
               ::GetCurrentThreadId() &&
           wxFindWinFromHandle(hwnd) == tlw &&
           wxWinUIMSWGetHwndGeneration(tlw, m_tlwHwnd) ==
               m_tlwHwndGeneration &&
           wxWinUIMSWGetNativeHwndGeneration(m_tlwHwnd) ==
               m_tlwNativeHwndGeneration;
}

bool wxWinUITopLevelHost::IsReadyForLookup() const
{
    // This is a publication-state predicate, not an Attached-policy check.
    // A fully initialized host must remain discoverable during its own
    // wxEVT_DESTROY dispatch so the window-event OperationGuard can defer the
    // physical XAML teardown. Destruction-scheduled rejection belongs in the
    // individual public operation (notably TitleBar Attached); only a
    // provisional or already logically shut-down host is hidden here.
    return m_initializationComplete && !m_initializing && !m_shuttingDown &&
           !m_shutdownComplete && m_tlw && m_tlwLifetime.get() == m_tlw;
}

void wxWinUITopLevelHost::InvokeInitializationHookForTest(
    InitializationHookPointForTest point)
{
    const InitializationHookForTest callback =
        gs_testHostInitializationHook;
    if ( !callback || gs_testHostInitializationHookInProgress )
        return;

    wxWindow * const tlw = m_tlwLifetime.get();
    if ( !tlw )
        return;

    gs_testHostInitializationHookInProgress = true;
    const auto finish = wxMakeGuard(
        []() { gs_testHostInitializationHookInProgress = false; });
    try
    {
        callback(tlw, point);
    }
    catch ( ... )
    {
        // A test seam must never make production rollback throwing.
    }
}

void wxWinUITopLevelHost::MaybeThrowInitializationFault(unsigned fault)
{
    if ( gs_testHostInitializationFaultMask & fault )
    {
        gs_testHostInitializationFaultMask &= ~fault;
        throw winrt::hresult_error(
            E_FAIL, L"wxWinUI test: injected host initialization fault");
    }
}

void wxWinUITopLevelHost::TestSetInitializationHook(
    InitializationHookForTest callback)
{
    gs_testHostInitializationHook = callback;
}

void wxWinUITopLevelHost::TestFailInitialization(unsigned mask)
{
    gs_testHostInitializationFaultMask = mask;
}

bool wxWinUITopLevelHost::Initialize()
{
    if ( !HasInitializationContext() )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI;
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        if ( !wxWinUI3Initialize() || !HasInitializationContext() )
            return false;

        const HWND hwndTLW = static_cast<HWND>(m_tlwHwnd);

        m_source = DesktopWindowXamlSource();
        if ( !HasInitializationContext() )
            return false;
        const auto tlwWindowId =
            winrt::Microsoft::UI::GetWindowIdFromWindow(hwndTLW);
        if ( !HasInitializationContext() )
            return false;
        m_source.Initialize(tlwWindowId);
        InvokeInitializationHookForTest(
            InitializationHookPointForTest::AfterSourceInitialization);
        if ( !HasInitializationContext() )
            return false;

        const auto siteBridge = m_source.SiteBridge();
        if ( !HasInitializationContext() )
            return false;
        siteBridge.ResizePolicy(
            ContentSizePolicy::ResizeContentToParentWindow);
        if ( !HasInitializationContext() )
            return false;

        const auto bridgeWindowId = siteBridge.WindowId();
        if ( !HasInitializationContext() )
            return false;
        m_bridge = GetWindowFromWindowId(bridgeWindowId);
        if ( !HasInitializationContext() )
            return false;
        if ( !m_bridge || !::IsWindow(m_bridge) ||
             ::GetWindowThreadProcessId(m_bridge, nullptr) !=
                ::GetCurrentThreadId() )
        {
            throw winrt::hresult_error(
                E_HANDLE, L"Invalid or cross-thread island bridge HWND");
        }

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR oldStyle = ::GetWindowLongPtr(m_bridge, GWL_STYLE);
        DWORD styleError = ::GetLastError();
        if ( !HasInitializationContext() )
            return false;
        if ( !oldStyle && styleError != ERROR_SUCCESS )
        {
            throw winrt::hresult_error(
                HRESULT_FROM_WIN32(styleError),
                L"GetWindowLongPtr(GWL_STYLE) failed");
        }

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR previousStyle = ::SetWindowLongPtr(
            m_bridge,
            GWL_STYLE,
            oldStyle |
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        styleError = ::GetLastError();
        if ( !HasInitializationContext() )
            return false;
        if ( !previousStyle && styleError != ERROR_SUCCESS )
        {
            throw winrt::hresult_error(
                HRESULT_FROM_WIN32(styleError),
                L"SetWindowLongPtr(GWL_STYLE) failed");
        }

        // The composition band renders relative to the siblings' GDI in HWND
        // z-order: the bridge must live at the top to be visible at all.
        if ( !::SetWindowPos(m_bridge, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW | SWP_FRAMECHANGED) )
        {
            const DWORD error = ::GetLastError();
            throw winrt::hresult_error(
                error ? HRESULT_FROM_WIN32(error) : E_FAIL,
                L"Initial island bridge pinning failed");
        }
        if ( !HasInitializationContext() )
            return false;

        auto *bridgeContext = new wxWinUISubclassContext(m_hostLifetime);
        if ( !::SetWindowSubclass(m_bridge, BridgeSubclassProc,
                                  wxWINUI_TLW_SUBCLASS_BRIDGE,
                                  reinterpret_cast<DWORD_PTR>(bridgeContext)) )
        {
            delete bridgeContext;
            throw winrt::hresult_error(
                E_FAIL,
                L"Bridge subclass installation failed");
        }
        else
        {
            m_bridgeSubclassContext = bridgeContext;
        }
        if ( !HasInitializationContext() )
            return false;

        auto rootImpl = winrt::make_self<wxWinUIRootCanvas>();
        gs_rootCanvases[this] = rootImpl;
        m_root = rootImpl.as<Canvas>();
        if ( !HasInitializationContext() )
            return false;
        // The island coordinate space is always physical left-to-right.
        // Individual slot containers mirror their content as needed, but root
        // pointer coordinates must never inherit that semantic direction.
        m_root.FlowDirection(
            winrt::Microsoft::UI::Xaml::FlowDirection::LeftToRight);
        if ( !HasInitializationContext() )
            return false;
        m_root.RequestedTheme(wxWinUIGetCurrentElementTheme());
        if ( !HasInitializationContext() )
            return false;

        // The root paints nothing (Transparent) so the wx children's GDI
        // shows through wherever XAML draws nothing -- but unlike a null
        // background it IS hit-testable, so the island receives ALL the
        // input.  The root router below then forwards real native messages
        // wherever no actual XAML content is hit: relying on WM_NCHITTEST
        // pass-through instead proved non-deterministic, the pointer
        // pipeline does not reliably continue to the windows below.
        const auto transparentBrush =
            winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
                winrt::Microsoft::UI::Colors::Transparent());
        if ( !HasInitializationContext() )
            return false;
        m_root.Background(transparentBrush);
        if ( !HasInitializationContext() )
            return false;
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        // DesktopWindowXamlSource doesn't expose its HWND-registered OLE
        // target to the XAML island unless the visual tree opts in to drop
        // routing. UIElement::AllowDrop defaults to false; without this the
        // broker remains correctly registered on the TLW but never receives
        // DragEnter while the pointer is over the normal XAML surface.
        m_root.AllowDrop(true);
        if ( !HasInitializationContext() )
            return false;
#endif
        // No SystemBackdrop.

        m_source.Content(m_root);
        if ( !HasInitializationContext() )
            return false;

        const auto xamlRoot = m_root.XamlRoot();
        if ( !xamlRoot )
        {
            wxWinUIDebugLog("wxWinUITopLevelHost: no XamlRoot after Content()");
            return false;
        }
        if ( !HasInitializationContext() )
            return false;

        const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

        m_takeFocusToken = m_source.TakeFocusRequested(
            [hostState](DesktopWindowXamlSource const&,
                        DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args)
            {
                wxWinUITopLevelHost * const host = hostState->GetHost();
                if ( !host )
                    return;

                const bool previous =
                    args.Request().Reason() ==
                        XamlSourceFocusNavigationReason::Last;
                host->OnTakeFocusRequested(previous);
            });
        if ( m_takeFocusToken.value )
            ++gs_rootHandlerAdds;
        if ( !HasInitializationContext() )
            return false;

        if ( !BindRootRouter() )
            return false;

        InvokeInitializationHookForTest(
            InitializationHookPointForTest::AfterRootRouter);
        if ( !HasInitializationContext() )
            return false;

        EnsureInnerSubclass();
        if ( !HasInitializationContext() )
            return false;

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        wxWindow * const dropTLW = m_tlwLifetime.get();
        if ( !dropTLW || !HasInitializationContext() )
            return false;
        m_dropBroker = wxWinUIDropBroker::Create(
            dropTLW, m_bridge, m_hostLifetime);
        if ( !HasInitializationContext() )
            return false;
        if ( !m_dropBroker )
        {
            wxLogError("wxWinUI: unable to create the OLE drop broker; "
                       "continuing with XAML hosting and OLE drops disabled");
        }
        else
        {
            // DesktopWindowXamlSource publishes its final native bridge
            // routing during first presentation. Construction is not a
            // presentation boundary, even when the TLW and bridge happen to
            // be visible already (for example when the first WinUI control is
            // added dynamically to a shown frame). Existing logical
            // wxDropTargets remain registered until the first visible
            // FlushSync() has published geometry and z-order and atomically
            // adopts them into the broker.
            m_dropBrokerAwaitingPresentation = true;
        }

#endif
        return FinishInitializationAfterDropBroker();
    }
    catch ( const winrt::hresult_error& e )
    {
        InvokeInitializationHookForTest(
            InitializationHookPointForTest::BeforeExceptionLog);
        wxWinUILogException("TLW island initialization", e);
        return false;
    }
}

bool wxWinUITopLevelHost::FinishInitializationAfterDropBroker()
{
#if wxUSE_DRAG_AND_DROP
    UpdateBridgeFileAcceptance();
    if ( !HasInitializationContext() )
        return false;
#endif

    // Every TLW that hosts WinUI content gets its backdrop/theme applied at
    // host creation. OLE PendingFixed completion is an independent service
    // continuation and never repeats this XAML/window initialization tail.
    wxWindow *liveTLW = m_tlwLifetime.get();
    if ( !liveTLW || !HasInitializationContext() )
        return false;
    wxWinUIApplyWindowBackdrop(liveTLW);
    if ( !HasInitializationContext() )
        return false;

    wxWinUITitleBarAttachSystemPolicy(
        this, m_hostLifetime, liveTLW, m_tlwHwnd);
    InvokeInitializationHookForTest(
        InitializationHookPointForTest::AfterTitleBarPolicy);
    if ( !HasInitializationContext() )
        return false;

    liveTLW = m_tlwLifetime.get();
    if ( !liveTLW || !HasInitializationContext() )
        return false;
    liveTLW->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnTLWSize, this);
    m_tlwSizeBound = true;
    if ( !HasInitializationContext() )
        return false;
    liveTLW->Bind(
        wxEVT_DPI_CHANGED, &wxWinUITopLevelHost::OnTLWDpiChanged, this);
    m_tlwDpiBound = true;
    if ( !HasInitializationContext() )
        return false;
    liveTLW->Bind(
        wxEVT_SHOW, &wxWinUITopLevelHost::OnTLWVisibility, this);
    m_tlwShowBound = true;
    if ( !HasInitializationContext() )
        return false;
    liveTLW->Bind(
        wxEVT_ICONIZE, &wxWinUITopLevelHost::OnTLWVisibility, this);
    m_tlwIconizeBound = true;
    if ( !HasInitializationContext() )
        return false;

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    // This also covers a host created for an already-visible TLW whose bridge
    // had not obtained non-empty presentation geometry during Initialize().
    if ( m_dropBrokerAwaitingPresentation )
        MarkAllDirty();
#endif

    InvokeInitializationHookForTest(
        InitializationHookPointForTest::AfterTLWBindings);
    MaybeThrowInitializationFault(TestInitializationFault_AfterTLWBindings);
    if ( !HasInitializationContext() )
        return false;

    wxWinUIDebugLog(
        "wxWinUITopLevelHost: island created for TLW %p (bridge %p)",
        static_cast<void *>(liveTLW), static_cast<void *>(m_bridge));
    return HasInitializationContext();
}

void wxWinUITopLevelHost::ResumeDeferredInitialization(
    std::uint64_t pendingId)
{
    // A posted registered message can be duplicated or outlive the pending
    // claim which created it. An already committed host must never be torn
    // down by such a stale wake.
    if ( !m_initializationDeferred || m_deferredResumeInProgress )
        return;

    m_deferredResumeInProgress = true;
    struct ResumeGuard
    {
        explicit ResumeGuard(bool& active_) : active(active_) { }
        ~ResumeGuard() { active = false; }
        bool& active;
    } resumeGuard(m_deferredResumeInProgress);

    if ( !HasInitializationContext() )
    {
        m_initializationDeferred = false;
        if ( !m_shuttingDown )
        {
            Shutdown();
            RequestDeferredDelete();
        }
        return;
    }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    if ( !m_dropBroker )
    {
        m_initializationDeferred = false;
        return;
    }

    // Claiming the posted wake does not consume the broker's PendingFixed
    // reservation. Route every continuation back through the same
    // presentation/epoch gate as first acquisition, so this path cannot
    // silently weaken the visible-FlushSync contract.
    if ( !m_dropBroker->IsPendingContinuation(pendingId) )
        return;

    m_initializationDeferred = false;
    m_dropBrokerAwaitingPresentation = true;
    if ( !IsDropBrokerPresentationReady() ||
         m_structureAppliedGeneration != m_structureGeneration )
    {
        MarkAllDirty();
        return;
    }

    TryInitializeDropBrokerAfterPresentation();
    return;
#else
    wxUnusedVar(pendingId);
    m_initializationDeferred = false;
    return;
#endif
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP

bool wxWinUITopLevelHost::IsDropBrokerPresentationReady() const
{
    if ( !m_tlwHwnd || !m_bridge ||
         !::IsWindowVisible(m_tlwHwnd) || ::IsIconic(m_tlwHwnd) ||
         !::IsWindowVisible(m_bridge) )
    {
        return false;
    }

    RECT bridgeRect{};
    return ::GetWindowRect(m_bridge, &bridgeRect) &&
           bridgeRect.right > bridgeRect.left &&
           bridgeRect.bottom > bridgeRect.top;
}

bool wxWinUITopLevelHost::IsDropBrokerPresentationStable(
    unsigned long long expectedGeneration) const
{
    return IsDropBrokerPresentationReady() &&
           m_structureGeneration == expectedGeneration &&
           m_structureAppliedGeneration == expectedGeneration;
}

void wxWinUITopLevelHost::RestartDropBrokerAfterPresentationInvalidation()
{
    if ( !m_dropBroker )
        return;

    // Ready is still provisional here: CommitHostInitialization() has not
    // cleared the broker's rollback latch. Exact shutdown therefore restores
    // every adopted logical target before a new adapter pair is considered.
    m_dropBroker->Shutdown();
    if ( !HasInitializationContext() )
        return;

    const wxWinUIDropBrokerSnapshot stopped =
        m_dropBroker->GetSnapshotForTest();
    if ( stopped.ownsRegistration || stopped.locked )
    {
        // Native cleanup could not prove that both old identities are gone.
        // Retain this fail-closed broker and never create a concurrent pair.
        wxLogError(
            "wxWinUI: OLE presentation changed during first acquisition "
            "and exact rollback retained native ownership; OLE drops remain "
            "disabled for this host");
        m_dropBrokerAwaitingPresentation = false;
        return;
    }

    m_dropBroker.reset();
    wxWindow * const dropTLW = m_tlwLifetime.get();
    if ( !dropTLW || !HasInitializationContext() )
        return;

    m_dropBroker = wxWinUIDropBroker::Create(
        dropTLW, m_bridge, m_hostLifetime);
    if ( !HasInitializationContext() )
        return;
    if ( !m_dropBroker )
    {
        wxLogError("wxWinUI: unable to recreate the OLE drop broker after "
                   "a presentation change; OLE drops remain disabled");
        m_dropBrokerAwaitingPresentation = false;
        return;
    }

    m_dropBrokerAwaitingPresentation = true;
    MarkAllDirty();
}

void wxWinUITopLevelHost::TryInitializeDropBrokerAfterPresentation()
{
    if ( !m_dropBrokerAwaitingPresentation ||
         m_dropBrokerAcquireInProgress || m_initializationDeferred ||
         !m_initializationComplete || !m_dropBroker ||
         !HasInitializationContext() ||
         !IsDropBrokerPresentationReady() ||
         m_structureAppliedGeneration != m_structureGeneration )
    {
        return;
    }

    const unsigned long long presentationGeneration =
        m_structureGeneration;
    m_dropBrokerAwaitingPresentation = false;
    m_dropBrokerAcquireInProgress = true;
    struct AcquireGuard
    {
        explicit AcquireGuard(bool& active_) : active(active_) { }
        ~AcquireGuard() { active = false; }
        bool& active;
    } acquireGuard(m_dropBrokerAcquireInProgress);

    const wxWinUIDropBrokerInitResult result =
        m_dropBroker->InitializeAndAdoptShellDropTargets();
    if ( !HasInitializationContext() )
        return;

    if ( result.status ==
            wxWinUIDropBrokerInitStatus::PendingRegistration )
    {
        m_initializationDeferred = true;
        return;
    }

    if ( !result.IsReady() )
    {
        wxLogError(
            "wxWinUI: OLE drop broker visible acquisition failed "
            "(HRESULT 0x%08lx); continuing with XAML hosting and OLE "
            "drops disabled",
            static_cast<unsigned long>(result.hresult));
        m_dropBroker->Shutdown();
        m_dropBroker.reset();
        return;
    }

    InvokeInitializationHookForTest(
        InitializationHookPointForTest::AfterDropBrokerReadyBeforeCommit);
    if ( !IsDropBrokerPresentationStable(presentationGeneration) )
    {
        RestartDropBrokerAfterPresentationInvalidation();
        return;
    }

    // This path necessarily runs after ForWindow() committed the host: a
    // scheduled visible flush cannot execute inside the construction stack.
    m_dropBroker->CommitHostInitialization();
}

#endif // wxUSE_OLE && wxUSE_DRAG_AND_DROP

void wxWinUITopLevelHost::Shutdown()
{
    if ( m_shutdownComplete || m_shutdownFinalizing )
        return;

    if ( !m_shuttingDown )
    {
        // Logical phase: fail every callback closed and publish an empty slot
        // table without touching the XAML tree. If Shutdown() was requested
        // from a routed callback, this is all that may happen on that stack.
        m_shuttingDown = true;

        // Seal and retain every physical gate before the first operation
        // below which can enter application code. Include already-deferred
        // slots and currently satisfied/never-opened peers: a shutdown
        // callback can attempt a late publication, and its sealed adapter
        // must remain discoverable when the terminal decision is made.
        // Revoke host authority first: neither gate discovery nor sealing
        // calls application code, while every operation after this block may.
        if ( m_hostLifetime )
            m_hostLifetime->Invalidate();

        gs_deferredNativeResizeWakes.erase(this);
        const auto nativeResize = m_nativeResize;
        if ( nativeResize )
        {
            nativeResize->cancelled = true;
            if ( nativeResize->dispatching && nativeResize->target.IsValid() )
            {
                ::SendMessageW(
                    reinterpret_cast<HWND>(nativeResize->target.GetLeafHwnd()),
                    WM_CANCELMODE, 0, 0);
            }
            FinishNativeResize(nativeResize, true);
        }

        try
        {
            m_shutdownPhysicalDisconnectGates.reserve(
                m_slots.size() + m_deferredDisconnectSlots.size());
            const auto sealGate = [&](const std::shared_ptr<
                                          wxWinUIPhysicalDisconnectGate>& gate)
            {
                if ( !gate ||
                     std::find(
                         m_shutdownPhysicalDisconnectGates.begin(),
                         m_shutdownPhysicalDisconnectGates.end(),
                         gate) !=
                         m_shutdownPhysicalDisconnectGates.end() )
                {
                    return;
                }
                gate->SealForHostShutdown();
                m_shutdownPhysicalDisconnectGates.push_back(gate);
            };
            for ( const auto& entry : m_slots )
            {
                if ( entry.second )
                    sealGate(entry.second->m_disconnectGate);
            }
            for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
            {
                if ( slot )
                    sealGate(slot->m_disconnectGate);
            }
        }
        catch ( ... )
        {
            std::terminate();
        }

        if ( m_focusMigration )
        {
            // Every gate is sealed before this callback-bearing rollback.
            AbortFocusMigration(m_focusMigration, true);
        }

        // Relinquish a test-only extended title bar only after every gate is
        // sealed. SetWindowLong/USER32 policy cleanup is callback-rich too.
        wxWinUITitleBarDetachSystemPolicy(this);
        m_nativeFocusTransferTarget.reset();
        m_nativeFocusDepartureSource.reset();
        m_nativeFocusDepartureDestination = nullptr;
        m_nativeFocusDepartureDestinationGeneration = 0;
        m_nativeFocusEntryAuthority = nullptr;
        m_nativeFocusEntryAuthorityGeneration = 0;
        m_nativeFocusEntryTarget.reset();
        m_nativeFocusEntryEpoch = 0;
        m_nativeFocusRollbackTarget.reset();
        m_nativeFocusRollbackShell = nullptr;
        m_nativeFocusRollbackShellGeneration = 0;
        m_nativeFocusRollbackEpoch = 0;

        // The host lifetime is already invalid before crossing any re-entrant
        // native or application boundary below. m_dropBroker->Shutdown() can
        // balance a live drag with wxDropTarget::OnLeave(); that callback may
        // pump messages. Every XAML route must already fail closed then.
        m_deferredDisconnectScheduled = false;

        try
        {
            m_shutdownSlots.reserve(m_slots.size());
        }
        catch ( ... )
        {
            // No source/root transfer has happened yet. Losing a live popup
            // gate here would make later physical release unobservable.
            std::terminate();
        }
        for ( const auto& kv : m_slots )
        {
            wxWinUISlot * const slot = kv.second;
            m_shutdownSlots.push_back(slot);
            if ( slot )
            {
                if ( slot->m_lifetime )
                    slot->m_lifetime->Invalidate();
                slot->RetireExposedAccessibilityShellProvider(true);
            }

            const auto owner = gs_slotOwners.find(kv.first);
            if ( owner != gs_slotOwners.end() && owner->second == this )
                gs_slotOwners.erase(owner);
        }
        m_slots.clear();
        m_dirty.clear();

        // Retire every raw wx pointer before the broker crosses into a
        // balanced wxDropTarget::OnLeave(). That callback may destroy and
        // pump the whole hierarchy; after this loop the deferred physical
        // phase owns only invalid lifetimes and XAML/WinRT values.
        for ( wxWinUISlot *slot : m_shutdownSlots )
        {
            if ( !slot )
                continue;

            UnbindSlotWindow(*slot);
            slot->m_window = nullptr;
        }

        // Likewise retire TLW bindings and the registry's raw key before the
        // broker can re-enter application code. When this is its own
        // wxEVT_DESTROY dispatch the dynamic table is already dying and must
        // not be mutated.
        wxWindow * const retiredTLW = m_tlwLifetime.get();
        const wxWindow * const registryKey = m_tlw;
        if ( retiredTLW && !retiredTLW->IsBeingDeleted() )
        {
            if ( m_tlwSizeBound )
            {
                retiredTLW->Unbind(
                    wxEVT_SIZE, &wxWinUITopLevelHost::OnTLWSize, this);
            }
            if ( m_tlwDpiBound )
            {
                retiredTLW->Unbind(
                    wxEVT_DPI_CHANGED,
                    &wxWinUITopLevelHost::OnTLWDpiChanged, this);
            }
            if ( m_tlwShowBound )
            {
                retiredTLW->Unbind(
                    wxEVT_SHOW,
                    &wxWinUITopLevelHost::OnTLWVisibility, this);
            }
            if ( m_tlwIconizeBound )
            {
                retiredTLW->Unbind(
                    wxEVT_ICONIZE,
                    &wxWinUITopLevelHost::OnTLWVisibility, this);
            }
            if ( m_tlwDestroyBound )
            {
                retiredTLW->Unbind(
                    wxEVT_DESTROY,
                    &wxWinUITopLevelHost::OnTLWDestroy, this);
            }
            if ( m_tlwMoveStartLogBound )
            {
                retiredTLW->Unbind(
                    wxEVT_MOVE_START, &wxWinUIInputLogTLWMoveStart);
            }
            if ( m_tlwMoveEndLogBound )
            {
                retiredTLW->Unbind(
                    wxEVT_MOVE_END, &wxWinUIInputLogTLWMoveEnd);
            }
            if ( m_tlwActivateLogBound )
            {
                retiredTLW->Unbind(
                    wxEVT_ACTIVATE, &wxWinUIInputLogTLWActivate);
            }
        }
        m_tlwSizeBound = false;
        m_tlwDpiBound = false;
        m_tlwShowBound = false;
        m_tlwIconizeBound = false;
        m_tlwDestroyBound = false;
        m_tlwMoveStartLogBound = false;
        m_tlwMoveEndLogBound = false;
        m_tlwActivateLogBound = false;

        const auto hostEntry = gs_tlwHosts.find(registryKey);
        if ( hostEntry != gs_tlwHosts.end() && hostEntry->second == this )
            gs_tlwHosts.erase(hostEntry);
        m_tlw = nullptr;
        m_tlwLifetime.Release();
        m_tlwHwnd = nullptr;
        m_tlwHwndGeneration = 0;
        m_tlwNativeHwndGeneration = 0;
        m_initializing = false;
        m_initializationComplete = false;
        m_initializationDeferred = false;
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        m_dropBrokerAwaitingPresentation = false;
        m_dropBrokerAcquireInProgress = false;
#endif

        // Transfer the native subclass allocations before the first
        // application callback boundary. A balanced OLE OnLeave() below may
        // destroy the bridge and run WM_NCDESTROY; from that point the HWND,
        // not this deferred host, must be their only raw owner.
        wxWinUITransferSubclassToHwnd(m_innerSubclassContext);
        wxWinUITransferSubclassToHwnd(m_bridgeSubclassContext);

#if wxUSE_DRAG_AND_DROP
        // Disable the shell drop path while the current subclass generation
        // still proves m_bridge. The broker shutdown which follows can call
        // application code and invalidate that HWND synchronously.
        if ( m_bridge && m_bridgeAcceptsFiles )
        {
            ::DragAcceptFiles(m_bridge, FALSE);
            m_bridgeAcceptsFiles = false;
        }
#endif

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        // The broker state is already fail-closed through the host lifetime
        // invalidation above. Its shutdown revalidates the exact TLW
        // generation after balancing any live drag before it calls
        // RevokeDragDrop().
        if ( m_dropBroker )
        {
            // Native revoke/unlock and a balanced wxDropTarget::OnLeave()
            // can all pump a nested host Shutdown(). Hold a host operation
            // lease across the complete broker call so the nested invocation
            // cannot enter physical finalization and destroy this broker from
            // under its own stack frame.
            {
                OperationGuard brokerOperation(this);
                m_dropBroker->Shutdown();
            }
            // InitializeAndAdoptShellDropTargets() can be below us on this
            // same stack when a virtual wxWindow query or native OLE call
            // pumps destruction. Keep its object alive until the enclosing
            // host OperationGuard has unwound; the host destructor then owns
            // the deferred reset.
            if ( m_operationDepth == 0 )
                m_dropBroker.reset();
        }
#endif

    }

    // Removing a routed handler or closing DesktopWindowXamlSource from inside
    // its dispatch can crash the focus/input manager. OnTLWDestroy() already
    // requests deferred host deletion; its destructor re-enters here after the
    // outermost OperationGuard has unwound.
    if ( m_operationDepth != 0 )
        return;

    m_shutdownFinalizing = true;

    // Implicit deferrals exist only to leave a currently executing slot
    // callback. At this point every host/global operation is unwound, so they
    // can be disconnected synchronously. A satisfied popup gate has crossed
    // its causal tail and no longer needs its delegate adapter. Unsatisfied
    // gates transfer the graph to a retirement until their last exact
    // completion, with the XAML terminal callback as the rundown fallback.
    std::vector<wxWinUISlot *> pendingTerminalSlots;
    try
    {
        pendingTerminalSlots.reserve(m_deferredDisconnectSlots.size());
    }
    catch ( ... )
    {
        // This allocation precedes Source.Close() and the graph transfer.
        // Continuing would lose a logically retired slot without creating a
        // truthful retention owner.
        std::terminate();
    }
    for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
    {
        if ( !slot )
            continue;
        const auto& gate = slot->m_disconnectGate;
        if ( slot->m_operationDepth == 0 &&
             (!gate || gate->IsSatisfied()) )
        {
            if ( !CompleteDeferredSlotDisconnect(*slot) )
                std::terminate();
        }
        else
        {
            pendingTerminalSlots.push_back(slot);
        }
    }
    m_deferredDisconnectSlots.swap(pendingTerminalSlots);

    std::shared_ptr<wxWinUITLWTerminalRetirement> terminalRetirement;
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> gates;
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>>
        sealedGateAliases;
    winrt::Microsoft::UI::Dispatching::DispatcherQueue
        retirementQueue{ nullptr };
    try
    {
        gates.reserve(
            m_shutdownPhysicalDisconnectGates.size() +
            m_deferredDisconnectSlots.size());
        const auto addGate = [&](const std::shared_ptr<
                                     wxWinUIPhysicalDisconnectGate>& gate)
        {
            if ( !gate )
                return;
            if ( std::find(gates.begin(), gates.end(), gate) != gates.end() )
                return;

            if ( gate->IsDegraded() )
                wxWinUINotePhysicalDisconnectGateDegraded();
            const winrt::Microsoft::UI::Dispatching::DispatcherQueue queue =
                gate->GetDispatcherQueue();
            if ( !queue )
                std::terminate();
            if ( retirementQueue && retirementQueue != queue )
            {
                // All slots of one TLW must be bound to its one UI queue.
                std::terminate();
            }
            retirementQueue = queue;
            gates.push_back(gate);
        };

        for ( const auto& gate : m_shutdownPhysicalDisconnectGates )
            addGate(gate);
        for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
        {
            // After the synchronous drain above, every remaining retired slot
            // must have a genuinely unsatisfied causal gate. A callback-only
            // slot cannot remain once the host operation depth reached zero.
            if ( !slot || !slot->m_disconnectGate ||
                 slot->m_disconnectGate->IsSatisfied() )
            {
                std::terminate();
            }
            addGate(slot->m_disconnectGate);
        }
    }
    catch ( ... )
    {
        std::terminate();
    }
    if ( !gates.empty() )
    {

        // Register before the first physical XAML teardown boundary, with the
        // actual queue generation. Exact gate completion consumes the ticket
        // normally; the deferral-backed XAML terminal callback remains the
        // fail-closed fallback.
        wxWinUIFrameworkRetirementId retirementId = 0;
        try
        {
            terminalRetirement =
                std::make_shared<wxWinUITLWTerminalRetirement>();
            terminalRetirement->SetPendingGateCount(
                static_cast<unsigned>(gates.size()));
            // Keep a stable snapshot for the post-Close sealed-no-popup
            // sweep. Gate completion can synchronously clear the terminal's
            // own vector, so no iterator into that vector may cross a gate
            // callback.
            sealedGateAliases = gates;
            if ( !terminalRetirement->AdoptGates(std::move(gates)) )
                std::terminate();
            retirementId = wxWinUIRegisterFrameworkRetirement(
                retirementQueue,
                [terminalRetirement]() noexcept
                {
                    terminalRetirement->RequestComplete();
                });
        }
        catch ( ... )
        {
            // std::function construction happens at the call site and can
            // still fail before the noexcept registration function is entered.
        }

        if ( !terminalRetirement )
        {
            // OOM before a retention owner exists cannot be recovered without
            // releasing the graph on the unsafe stack. Termination is the only
            // deterministic fail-closed outcome.
            std::terminate();
        }

        if ( !retirementId )
            std::terminate();

        terminalRetirement->SetRetirementId(retirementId);

        // SetCompletion() may invoke synchronously for an already-complete
        // gate. The last such callback clears terminalRetirement->m_gates,
        // so iterate the independent strong snapshot rather than the vector
        // which the callback is allowed to mutate.
        for ( const auto& gate : sealedGateAliases )
        {
            if ( !gate->SetCompletion(
                     [terminalRetirement]() noexcept
                     {
                         terminalRetirement->GateCompleted();
                     }) )
            {
                std::terminate();
            }
        }
    }
    else if ( !m_deferredDisconnectSlots.empty() )
    {
        std::terminate();
    }

    // TakeFocusRequested is a host callback too.  Revoke it in phase 1,
    // before Close(), just like the root-router callbacks.
    if ( m_source && m_takeFocusToken.value )
    {
        try
        {
            m_source.TakeFocusRequested(m_takeFocusToken);
            ++gs_rootHandlerRevokes;
        }
        catch ( const winrt::hresult_error& )
        {
        }
        m_takeFocusToken = {};
    }

    if ( m_root )
    {
        for ( auto& handler : m_rootHandlers )
        {
            try
            {
                m_root.RemoveHandler(handler.first, handler.second);
                ++gs_rootHandlerRevokes;
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }
    m_rootHandlers.clear();

    // Phase 1 -- before Close(): cut every path from a XAML callback back
    // to wx.  Closing the source fires teardown events (Unloaded,
    // LostFocus...) on the tree it still holds, and those must find revoked
    // handlers and invalidated state, never a window already in
    // destruction.  The containers deliberately stay attached: the source
    // must be closed with its tree in place (see below), so the visual
    // release only happens in phase 2.
    // Every attached-property restoration below is an arbitrary application
    // callback boundary. The logical phase already detached the complete map;
    // this stable snapshot remains the sole teardown owner.
    for ( wxWinUISlot *slot : m_shutdownSlots )
    {
        wxWinUISlot::OperationGuard slotOperation(slot);

        RevokeSlotCallbacks(*slot);

        // The content may be retained and reused by application code after
        // the TLW dies. Revoke its property observer first, then return every
        // attached value owned by wx while the XAML source and its complete
        // visual tree are still alive. Source.Close() must remain the first
        // operation which tears that tree down.
        slot->RevokeContentLoadedObserver();
        slot->RevokeToolTipPropertyObserver();
        slot->RevokeAutomationNameStylePropertyObserver();
        slot->RestoreAutomationName();
        slot->RestoreHelpText();
        slot->RestoreToolTip();
        slot->RestoreAccessibilityView();
    }
    for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
    {
        if ( !slot || slot->m_operationDepth != 0 )
            continue;

        // Logically retired slots are no longer in m_slots, but their visual
        // source can still be attached while a deferred disconnect waits for
        // the dispatcher. Revoke it before Close() whenever no callback is
        // active. An active callback already sees an invalid lifetime; its
        // event source, delegate and complete visual ancestry remain owned by
        // the terminally retained source/root graph after the slot wrapper
        // eventually unwinds.
        RevokeSlotCallbacks(*slot);
        slot->RevokeContentLoadedObserver();
        slot->RevokeToolTipPropertyObserver();
        slot->RevokeAutomationNameStylePropertyObserver();
        slot->RestoreAutomationName();
        slot->RestoreHelpText();
        slot->RestoreToolTip();
        slot->RestoreAccessibilityView();
    }
    m_dirty.clear();
    ClearDeferredFocusRequest();
    m_pendingNativeInput.clear();
    m_nativeInputDispatchScheduled = false;
    m_nativeInputDispatchInFlight = false;
    m_nativeInputDispatchCommitted = false;
    m_inFlightNativeInput = PendingNativeDispatch();
    m_modalAwaitingReleases.fill(wxWinUIInputAction());
    m_islandCancelDeferred = false;
    m_deferredIslandCancelGeneration = 0;
    m_postedIslandCancelGeneration = 0;
    m_postedIslandCancelMessages = 0;

    // Every per-slot reference was released above; whatever remains (there
    // should be nothing) is dropped without unbinding dead windows.
    m_ancestorRefs.clear();

    wxWinUIRetireSubclass(m_inner, BridgeSubclassProc,
                          wxWINUI_TLW_SUBCLASS_INNER,
                          m_innerSubclassContext, "inner-island");
    m_inner = nullptr;

    wxWinUIRetireSubclass(m_bridge, BridgeSubclassProc,
                          wxWINUI_TLW_SUBCLASS_BRIDGE,
                          m_bridgeSubclassContext, "bridge");
    m_bridge = nullptr;

    // Release the island input object before closing the source it belongs
    // to. It carries no callbacks, but retaining it across Close() would make
    // later diagnostic reads address a disconnected ContentIsland.
    m_inputPointerSource = nullptr;
    m_lastIslandPointerCursor = nullptr;
    m_lastIslandPointerHandle = nullptr;
    m_islandPointerCursorApplied = false;
    m_inputPointerSourceAuthoritative = false;
    m_rootPointerCursorAuthoritative = false;

    // This is the last observation while the source still exposes live popup
    // properties. Source.Close()/Unloaded is allowed to clear them before a
    // deferred Opened event arrives; latch that debt into the already
    // registered/retargeted TLW ledger first.
    for ( const auto& gate : m_shutdownPhysicalDisconnectGates )
    {
        if ( gate )
            gate->LatchPublishedPopupDebtBeforeSourceClose();
    }

    if ( m_source )
    {
        try
        {
            // Deliberately don't clear the Content first: closing the source
            // with the content still attached is the teardown order that
            // doesn't crash inside XAML (same rule as wxWinUIControlHost).
            m_source.Close();
        }
        catch ( const winrt::hresult_error& )
        {
            // Close() can fail after partially starting island teardown. Keep
            // the still-live source and attached root indistinguishable from
            // the successful-close case below: both are transferred intact
            // and neither owning projection is released before the XAML
            // terminal boundary.
        }

    }

    if ( terminalRetirement )
        terminalRetirement->AdoptSource(std::move(m_source));
    else
        m_source = nullptr;
    m_shutdownPhysicalDisconnectGates.clear();

    // Phase 2 -- after Close(): release callback/bookkeeping references. Every
    // slot route was either revoked above or retains only an invalid lifetime.
    // The root still owns every Grid, each Grid owns its carrier, and each
    // carrier owns its content (including control-owned popup/template state);
    // erasing gs_slotGrids and deleting the C++ slot wrappers therefore drops
    // only duplicate projections and inert caches. The source plus this
    // complete graph remain strongly owned by terminalRetirement until the
    // last exact gate completion or the terminal XAML deferral.
    for ( wxWinUISlot *slot : m_shutdownSlots )
    {
        gs_slotGrids.erase(slot);
        slot->MarkForDeletion();
        if ( slot->m_operationDepth == 0 )
            delete slot;
    }
    m_shutdownSlots.clear();
    for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
    {
        if ( !slot )
            continue;

        gs_slotGrids.erase(slot);
        slot->m_disconnectPending = false;
        slot->m_deferredDisconnectHost = nullptr;
        if ( slot->m_disconnectGate )
        {
            gs_pendingPhysicalDisconnectGates.fetch_sub(
                1, std::memory_order_relaxed);
            slot->m_disconnectGate.reset();
        }
        slot->m_disconnectRetirementId = 0;
        slot->MarkForDeletion();
        if ( slot->m_operationDepth == 0 )
            delete slot;
    }
    m_deferredDisconnectSlots.clear();

    if ( terminalRetirement )
        terminalRetirement->AdoptRoot(std::move(m_root));
    else
        m_root = nullptr;
    gs_rootCanvases.erase(this);
    m_hostLifetime.reset();

    // Registration can complete reentrantly when shutdown is already
    // terminal. Arm only after Close(), the complete graph transfer, and the
    // release of every other host-owned strong root reference.
    if ( terminalRetirement )
    {
        terminalRetirement->ArmForCompletion();

        // Only after the ledger owns both outer graph roots and is armed may
        // sealed peers with no popup debt complete their logical gates.
        for ( const auto& gate : sealedGateAliases )
        {
            if ( gate )
                gate->CompleteSealedWithoutPopup();
        }
        if ( terminalRetirement->GetPendingGateCount() != 0 )
            terminalRetirement->Promote();
    }
    sealedGateAliases.clear();

    m_shutdownComplete = true;
    m_shutdownFinalizing = false;
}

// ============================================================================
// slots
// ============================================================================

unsigned wxWinUIHostLifetime::ms_liveCount = 0;
unsigned wxWinUISlotLifetime::ms_liveCount = 0;
std::map<wxWindow *,
         std::shared_ptr<wxWinUITopLevelHost::FocusMigration>>
    wxWinUITopLevelHost::ms_focusMigrations;

unsigned wxWinUITopLevelHost::GetLiveHostCount()
{
    return static_cast<unsigned>(gs_tlwHosts.size());
}

unsigned wxWinUITopLevelHost::GetLiveSlotCount()
{
    unsigned count = 0;
    for ( const auto& kv : gs_tlwHosts )
        count += static_cast<unsigned>(kv.second->m_slots.size());
    return count;
}

unsigned
wxWinUITopLevelHost::GetPendingPhysicalDisconnectGateCountForTest()
{
    return gs_pendingPhysicalDisconnectGates.load(std::memory_order_acquire);
}

unsigned
wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest()
{
    return gs_pendingTLWTerminalRetirements.load(std::memory_order_acquire);
}

unsigned
wxWinUITopLevelHost::GetLiveAutomationNameStyleObserverCountForTest()
{
    return gs_liveAutomationNameStylePropertyStates;
}

unsigned
wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest()
{
    return gs_liveContentLoadedStates;
}

unsigned wxWinUITopLevelHost::GetSlotHandlerAddCount()
{
    return gs_slotHandlerAdds;
}

unsigned wxWinUITopLevelHost::GetSlotHandlerRevokeCount()
{
    return gs_slotHandlerRevokes;
}

unsigned wxWinUITopLevelHost::GetRootHandlerAddCount()
{
    return gs_rootHandlerAdds;
}

unsigned wxWinUITopLevelHost::GetRootHandlerRevokeCount()
{
    return gs_rootHandlerRevokes;
}

unsigned wxWinUITopLevelHost::GetLiveSubclassContextCount()
{
    return gs_liveSubclassContexts;
}

unsigned wxWinUITopLevelHost::GetFlushScheduleCount()
{
    return gs_flushSchedules;
}

unsigned wxWinUITopLevelHost::GetFlushRunCount()
{
    return gs_flushRuns;
}

unsigned wxWinUITopLevelHost::GetFlushCallbackAttemptCount()
{
    return gs_flushCallbackAttempts;
}

unsigned wxWinUITopLevelHost::GetImpossibleZOrderCount()
{
    return gs_impossibleZOrderOverlaps;
}

void wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest()
{
    gs_impossibleZOrderOverlaps = 0;
}

void wxWinUITopLevelHost::TestFailHandlerAdd(unsigned nth)
{
    gs_failHandlerAddAt = nth;
}

void wxWinUITopLevelHost::TestFailContentSwap(unsigned mask)
{
    gs_failContentMask = mask;
}

void wxWinUITopLevelHost::TestFailToolTipObserverAdds(unsigned count)
{
    gs_failToolTipObserverAdds = count;
}

void wxWinUITopLevelHost::TestFailContentLoadedObserverAdds(unsigned count)
{
    gs_failContentLoadedObserverAdds = count;
}

void wxWinUITopLevelHost::TestFailInputPointerSourceSet(unsigned nth)
{
    gs_failInputPointerSourceSetAt = nth;
}

void wxWinUITopLevelHost::TestOnNextSlotAttached(
    std::function<void (wxWindow *)> callback)
{
    gs_testSlotAttachedHook = std::move(callback);
}

unsigned wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest()
{
    return static_cast<unsigned>(ms_focusMigrations.size());
}

void wxWinUITopLevelHost::TestOnNextSlotSynced(
    std::function<void (wxWindow *)> callback)
{
    gs_testSlotSyncedHook = std::move(callback);
}

void wxWinUITopLevelHost::TestOnNextContentCarrierSet(
    std::function<void (wxWindow *)> callback)
{
    gs_testContentCarrierSetHook = std::move(callback);
}

void wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
    std::function<void (wxWindow *)> callback)
{
    gs_testContentTransactionStartedHook = std::move(callback);
}

void wxWinUITopLevelHost::TestOnNextZOrderSet(
    std::function<void ()> callback)
{
    gs_testZOrderSetHook = std::move(callback);
}

void wxWinUITopLevelHost::TestOnNextWindowAtPointSlotStateRead(
    std::function<void (wxWindow *)> callback)
{
    gs_testWindowAtPointSlotStateHook = std::move(callback);
}

bool wxWinUITopLevelHost::TestPointerContactActiveSnapshot(
    wxWinUIInputDevice device,
    int virtualKey,
    unsigned pointerType,
    unsigned pointerFlags,
    unsigned penFlags)
{
    return wxWinUIIsPointerContactSnapshotActive(
        device, virtualKey,
        static_cast<POINTER_INPUT_TYPE>(pointerType),
        static_cast<UINT32>(pointerFlags),
        static_cast<UINT32>(penFlags));
}

unsigned wxWinUITopLevelHost::GetSlotCursorSetCount()
{
    return gs_slotCursorSets;
}

winrt::Microsoft::UI::Input::InputCursor
wxWinUITopLevelHost::GetSlotCursorForTest(const wxWindow *window) const
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return nullptr;

    const auto it = gs_slotGrids.find(slot);
    return it != gs_slotGrids.end()
             ? it->second->GetWindowCursor()
             : nullptr;
}

winrt::Microsoft::UI::Input::InputCursor
wxWinUITopLevelHost::GetIslandCursorForTest() const
{
    if ( m_rootPointerCursorAuthoritative )
        return m_lastIslandPointerCursor;

    if ( !m_inputPointerSource )
        return m_islandPointerCursorApplied
             ? m_lastIslandPointerCursor
             : nullptr;

    try
    {
        return m_inputPointerSource.Cursor();
    }
    catch ( const winrt::hresult_error& )
    {
        return nullptr;
    }
}

bool wxWinUITopLevelHost::TestActivateSlotCursor(
    wxWindow *window,
    bool emitSetCursorEvent)
{
    wxWinUISlot * const slot = FindSlot(window);
    return slot &&
           ApplySlotPointerCursor(slot->m_lifetime,
                                  emitSetCursorEvent,
                                  0,
                                  !emitSetCursorEvent);
}

bool wxWinUITopLevelHost::TestApplySlotPointerCursor(
    wxWindow *window,
    const wxPoint& screenPoint,
    wxWinUIInputKind kind)
{
    m_lastPointerScreen.x = screenPoint.x;
    m_lastPointerScreen.y = screenPoint.y;
    return TestActivateSlotCursor(
        window, wxWinUIShouldEmitSetCursor(kind));
}

bool wxWinUITopLevelHost::TestSimulateXamlCursorSelection(
    const winrt::Microsoft::UI::Input::InputCursor& cursor)
{
    if ( !m_inputPointerSource )
        return false;

    try
    {
        // This writes only the runtime property, deliberately leaving the
        // host's ownership bookkeeping untouched. ObserveXamlPointerCursor()
        // must discover and yield to the external/XAML mutation exactly as
        // the production routed-event path does.
        m_inputPointerSource.Cursor(cursor);
        ObserveXamlPointerCursor();
        return !m_inputPointerSourceAuthoritative;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxWinUISlot *
wxWinUITopLevelHost::RegisterSlot(wxWindow *window,
                                  const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    return RegisterSlot(window, element, element);
}

wxWinUISlot *
wxWinUITopLevelHost::RegisterSlot(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>&
        physicalDisconnectGate)
{
    return RegisterSlotInternal(
        window,
        element,
        semanticTarget,
        nullptr,
        false,
        false,
        wxString(),
        physicalDisconnectGate);
}

wxWinUISlot *
wxWinUITopLevelHost::RegisterSlotInternal(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
    wxWinUITopLevelHost *transferFrom,
    bool accessibilityFactoryAlreadyResolved,
    bool preserveAutomationNameRelinquishment,
    const wxString& lastDesiredAutomationName,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>&
        physicalDisconnectGate)
{
    OperationGuard hostOperation(this);

    if ( !window || !m_root || m_shuttingDown ||
         !wxWinUISemanticPairIsValid(element, semanticTarget) )
        return nullptr;

    wxWinUITopLevelHost * const authoritative = FindSlotOwner(window);
    if ( authoritative && authoritative != this &&
         authoritative != transferFrom )
    {
        // A normal registration must never manufacture a second slot. Only
        // MigrateSlotToCurrentTLW() may open the bounded rollback window.
        return nullptr;
    }
    if ( transferFrom &&
         (authoritative != transferFrom ||
          !transferFrom->FindSlot(window) ||
          FindSlot(window)) )
    {
        return nullptr;
    }

    const auto findMigratedSlot = [&]() -> wxWinUISlot *
    {
        wxWinUITopLevelHost * const owner = FindSlotOwner(window);
        if ( !owner || owner == this )
            return nullptr;

        wxWinUISlot * const migrated = owner->FindSlot(window);
        return migrated &&
                   migrated->GetContent() == element &&
                   migrated->GetSemanticTarget() == semanticTarget &&
                   (!physicalDisconnectGate ||
                    migrated->m_disconnectGate ==
                        physicalDisconnectGate) &&
                   wxWinUISemanticPairIsValid(
                       migrated->GetContent(),
                       migrated->GetSemanticTarget())
                 ? migrated
                 : nullptr;
    };

    const auto it = m_slots.find(window);
    if ( it != m_slots.end() )
    {
        wxWinUISlot * const existing = it->second;
        wxWinUISlot::OperationGuard existingOperation(existing);

        // A poisoned slot (its carrier went unreadable, its lifetime
        // invalidated) can never work again: dispose of it and fall through
        // to build a brand-new one, rather than SetContent()-ing a corpse
        // whose handlers are all bound to the dead lifetime.
        if ( existing->m_poisoned ||
                (existing->m_lifetime && !existing->m_lifetime->IsValid()) )
        {
            UnregisterSlot(window);
            // fall through to fresh creation below
        }
        else
        {
            // A control host may install its popup gate before its very first
            // content publication. Preserve that ordering on replacement too:
            // SetContent() can synchronously fire Loaded/application code.
            const std::shared_ptr<wxWinUIPhysicalDisconnectGate>
                previousPhysicalDisconnectGate =
                    existing->m_disconnectGate;
            if ( physicalDisconnectGate )
                existing->m_disconnectGate = physicalDisconnectGate;

            // Re-registration just swaps the content -- and a failed swap is
            // a failed registration: the slot keeps its previous content and
            // the caller must not believe the new element is installed.
            const auto previousContent = existing->GetContent();
            const auto previousSemanticTarget =
                existing->GetSemanticTarget();
            if ( !existing->SetContent(element, semanticTarget) )
            {
                if ( !existing->m_deletePending &&
                     FindSlot(window) == existing )
                {
                    existing->m_disconnectGate =
                        previousPhysicalDisconnectGate;
                }
                // An unreadable carrier poisons the slot and invalidates all
                // of its callbacks. Rebuild it immediately instead of
                // returning a corpse to a later caller. If the requested
                // registration still fails, make a best-effort restoration
                // of the previous logical content; the return value remains
                // null because the requested element was not installed.
                if ( existing->m_poisoned )
                {
                    UnregisterSlot(window);
                    if ( wxWinUISlot * const replacement =
                             RegisterSlot(
                                 window,
                                 element,
                                 semanticTarget,
                                 physicalDisconnectGate) )
                    {
                        return replacement;
                    }

                    if ( previousContent )
                    {
                        RegisterSlot(
                            window,
                            previousContent,
                            previousSemanticTarget);
                    }
                }
                return nullptr;
            }
            const unsigned long long requestedContentGeneration =
                existing->m_contentGeneration;
            if ( existing->m_deletePending ||
                 FindSlot(window) != existing ||
                 !existing->m_lifetime ||
                 existing->m_lifetime->GetHost() != this ||
                 existing->GetContent() != element ||
                 existing->GetSemanticTarget() != semanticTarget )
            {
                return findMigratedSlot();
            }
            MarkDirty(window);
#if wxUSE_TOOLTIPS
            // A tooltip may have been assigned before Create(), when this
            // window had no slot and was therefore classified as native.
            // Re-associate it only after registration is real so later
            // wxToolTip::SetTip() mutations dirty the XAML state adapter.
            if ( wxToolTip * const tip = window->GetToolTip() )
                tip->SetWindow(window);
#endif
            if ( existing->m_deletePending ||
                 FindSlot(window) != existing ||
                 !existing->m_lifetime ||
                 existing->m_lifetime->GetHost() != this ||
                 existing->GetContent() != element ||
                 existing->GetSemanticTarget() != semanticTarget ||
                 existing->m_contentGeneration !=
                     requestedContentGeneration )
            {
                return findMigratedSlot();
            }
            return existing;
        }
    }

    bool initialHasWxAccessible = false;
    bool accessibilityFactoryResolved =
        accessibilityFactoryAlreadyResolved;
#if wxUSE_ACCESSIBILITY
    if ( window != m_tlw && !window->IsTopLevel() )
    {
        // CreateAccessible() is virtual and may be lazy. Ask exactly once
        // before publishing the first slot so the initial authority decision
        // doesn't briefly expose both the native and XAML trees. The factory
        // is arbitrary application code: validate the HWND association,
        // generation, topology and slot ownership before touching `window`
        // again.
        const WXHWND accessibleHwnd = window->GetHWND();
        const unsigned long long accessibleHwndGeneration =
            wxWinUIMSWGetHwndGeneration(window, accessibleHwnd);
        if ( !accessibleHwnd || !accessibleHwndGeneration )
            return nullptr;

        if ( !accessibilityFactoryResolved )
        {
            window->GetOrCreateAccessible();
            accessibilityFactoryResolved = true;
        }

        if ( wxFindWinFromHandle(
                 reinterpret_cast<HWND>(accessibleHwnd)) != window ||
             wxWinUIMSWGetHwndGeneration(
                 window, accessibleHwnd) != accessibleHwndGeneration )
        {
            return findMigratedSlot();
        }

        if ( m_shuttingDown || window->IsBeingDeleted() ||
             wxGetTopLevelParent(window) != m_tlw )
        {
            return findMigratedSlot();
        }

        // A re-entrant factory may have completed this same registration.
        // Re-enter the already-existing branch instead of manufacturing a
        // duplicate slot.
        if ( FindSlot(window) )
        {
            return RegisterSlotInternal(
                window,
                element,
                semanticTarget,
                transferFrom,
                accessibilityFactoryResolved,
                preserveAutomationNameRelinquishment,
                lastDesiredAutomationName,
                physicalDisconnectGate);
        }

        wxWinUITopLevelHost * const ownerAfterFactory =
            FindSlotOwner(window);
        if ( ownerAfterFactory && ownerAfterFactory != transferFrom )
            return findMigratedSlot();

        initialHasWxAccessible = window->GetAccessible() != nullptr;
    }
#endif // wxUSE_ACCESSIBILITY

    wxWinUISlot * const slot = new wxWinUISlot;
    wxWinUISlot::OperationGuard slotOperation(slot);
    // A migrating popup gate is part of the slot's physical identity. Install
    // it before the carrier, content, Loaded handlers, or ownership registry
    // can publish this destination and re-enter application code.
    slot->m_disconnectGate = physicalDisconnectGate;
    slot->m_window = window;
    slot->m_hwnd = window->GetHWND();
    slot->m_hasWxAccessible = initialHasWxAccessible;
    slot->m_lifetime = std::make_shared<wxWinUISlotLifetime>(window, this);
    unsigned long long requestedContentGeneration = 0;
    bool requestedContentGenerationCaptured = false;
    bool requestedContentSuperseded = false;

    const auto rollback = [&]()
    {
        const auto current = m_slots.find(window);
        if ( current != m_slots.end() && current->second == slot )
        {
            UnbindSlotWindow(*slot);
            m_dirty.erase(window);
            m_slots.erase(current);
        }

        const auto owner = gs_slotOwners.find(window);
        if ( owner != gs_slotOwners.end() && owner->second == this )
        {
            if ( transferFrom && transferFrom->FindSlot(window) )
                owner->second = transferFrom;
            else
                gs_slotOwners.erase(owner);
        }

        if ( m_focusMigration &&
             m_focusMigration->destinationHost == m_hostLifetime &&
             m_focusMigration->destinationSlot == slot->m_lifetime )
        {
            m_focusMigration->destinationSlot.reset();
            m_focusMigration->destinationContentGeneration = 0;
            m_focusMigration.reset();
        }

        if ( !slot->m_deletePending )
        {
            DisconnectSlot(*slot);
            slot->MarkForDeletion();
        }
    };

    const auto slotIdentityIsCurrent = [&]()
    {
        return !slot->m_deletePending &&
               !slot->m_poisoned &&
               FindSlot(window) == slot &&
               slot->m_lifetime &&
               slot->m_lifetime->GetHost() == this &&
               slot->m_lifetime->GetWindow() == window;
    };

    // Observe, but never undo, a newer content generation published by a
    // synchronous Loaded/application callback. The fresh slot itself is
    // already authoritative and still needs the normal structural finalizing
    // below; only the OUTER request must eventually report that it lost.
    const auto observeRequestedContent = [&]()
    {
        if ( !slotIdentityIsCurrent() )
            return false;

        // Append(), Loaded and every observer registration above may run
        // arbitrary application code. The slot must never survive one of
        // those boundaries with UIA authority pointing outside its visual
        // root, even when a newer re-entrant content generation won.
        if ( !wxWinUISemanticPairIsValid(
                 slot->GetContent(), slot->GetSemanticTarget()) )
        {
            return false;
        }

        if ( !requestedContentGenerationCaptured ||
             slot->GetContent() != element ||
             slot->GetSemanticTarget() != semanticTarget ||
             slot->m_contentGeneration != requestedContentGeneration )
        {
            requestedContentSuperseded = true;
        }
        return true;
    };

    const auto abandonInvalidFreshSlot = [&]() -> wxWinUISlot *
    {
        // If this exact provisional slot is still registered but structurally
        // invalid, it belongs to this transaction and must be disconnected.
        // A valid slot whose CONTENT was superseded never reaches this path:
        // observeRequestedContent() keeps it alive and merely latches loss of
        // the outer generation.
        if ( FindSlot(window) == slot )
            rollback();

        if ( requestedContentSuperseded )
            return nullptr;
        return findMigratedSlot();
    };

    try
    {
        auto gridImpl = winrt::make_self<wxWinUISlotGrid>();
        gs_slotGrids[slot] = gridImpl;
        slot->m_container =
            gridImpl.as<winrt::Microsoft::UI::Xaml::Controls::Grid>();

        // The host-owned state carrier between the container and the
        // content (see wxWinUISlot::m_carrier): not a tab stop, hidden
        // from the UIA content tree (it must not add a narration level),
        // and stretching its content -- the ContentControl defaults are
        // Left/Top, which would leave a size-less element hugging the slot
        // corner instead of filling it.
        slot->m_carrier = winrt::Microsoft::UI::Xaml::Controls::ContentControl();
        slot->m_carrier.IsTabStop(false);
        slot->m_carrier.HorizontalContentAlignment(
            winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);
        slot->m_carrier.VerticalContentAlignment(
            winrt::Microsoft::UI::Xaml::VerticalAlignment::Stretch);
        winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::
            SetAccessibilityView(
                slot->m_carrier,
                winrt::Microsoft::UI::Xaml::Automation::Peers::
                    AccessibilityView::Raw);
        slot->m_container.Children().Append(slot->m_carrier);

        if ( element )
        {
            slot->m_carrier.Content(element);
            slot->m_content = element;
            slot->m_semanticTarget = semanticTarget;
        }
        if ( preserveAutomationNameRelinquishment && element )
        {
            // Seed this before the container is appended to the XAML root:
            // Loaded/application callbacks may synchronously flush the fresh
            // slot, and must already see the permanent application decision
            // made on the source slot.
            slot->SeedAutomationNameRelinquishment(
                lastDesiredAutomationName);
        }
        requestedContentGeneration = slot->m_contentGeneration;
        requestedContentGenerationCaptured = true;

        // Until the first sync runs the container must not cover the island
        // origin, where it would eat input meant for whatever lives there.
        slot->m_container.Visibility(
            winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

        // Slot positions are physical (already mirrored by wx for RTL), so
        // the root Canvas stays LTR; only the hosted element's own content
        // mirrors, per window.
        if ( window->GetLayoutDirection() == wxLayout_RightToLeft )
        {
            slot->m_container.FlowDirection(
                winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft);
        }

        // Registration and event binding belong to the same transaction:
        // AddHandler()/GotFocus()/LostFocus() can throw halfway through and
        // a slot must never stay half-registered.
        // Eight pointer handlers plus GettingFocus. Keep one spare so a
        // successful registration cannot allocate while publishing its
        // final callback.
        slot->m_routedHandlers.reserve(10);
        m_slots.emplace(window, slot);
        gs_slotOwners[window] = this;

        // A focus migration is published before Append(): Loaded, focus and
        // application callbacks raised by the destination carrier must
        // already see the shared A->B (or reentrant A->B->C) identity.
        const auto focusMigrationIt =
            ms_focusMigrations.find(window);
        if ( focusMigrationIt != ms_focusMigrations.end() )
        {
            const std::shared_ptr<FocusMigration> focusMigration =
                focusMigrationIt->second;
            if ( focusMigration && !focusMigration->committed &&
                 focusMigration->destinationHost == m_hostLifetime &&
                 focusMigration->window.get() == window &&
                 window->GetHWND() == focusMigration->hwnd &&
                  wxWinUIMSWGetHwndGeneration(
                      window, focusMigration->hwnd) ==
                      focusMigration->hwndGeneration )
            {
                // A host can carry only the one transaction owning its native
                // focus arbiter. Never orphan an earlier ticket by replacing
                // this alias silently: retire it first, then revalidate the
                // incoming identity after the resulting wx callbacks.
                if ( m_focusMigration != focusMigration &&
                     m_focusMigration &&
                     !m_focusMigration->committed )
                {
                    const std::shared_ptr<FocusMigration> incumbent =
                        m_focusMigration;
                    AbortFocusMigration(incumbent, true);
                }

                const auto current =
                    ms_focusMigrations.find(window);
                if ( (!m_focusMigration ||
                      m_focusMigration == focusMigration) &&
                     current != ms_focusMigrations.end() &&
                     current->second == focusMigration &&
                     !focusMigration->committed &&
                     focusMigration->destinationHost == m_hostLifetime &&
                     FindSlot(window) == slot &&
                     slot->m_lifetime->GetHost() == this &&
                     slot->m_lifetime->GetWindow() == window &&
                     window->GetHWND() == focusMigration->hwnd &&
                     wxWinUIMSWGetHwndGeneration(
                         window, focusMigration->hwnd) ==
                         focusMigration->hwndGeneration )
                {
                    focusMigration->destinationSlot =
                        slot->m_lifetime;
                    focusMigration->destinationContentGeneration =
                        slot->m_contentGeneration;
                    m_focusMigration = focusMigration;
                }
                else if ( current != ms_focusMigrations.end() &&
                          current->second == focusMigration &&
                          !focusMigration->committed )
                {
                    // A re-entrant focus transaction won while the incumbent
                    // was being retired. The incoming ticket can no longer
                    // acquire a host alias and must not survive only in the
                    // global table.
                    AbortFocusMigration(focusMigration, true);
                }
            }
        }

        BindSlotWindow(*slot);
        BindSlotEvents(*slot);
        const bool toolTipObserverInstalled =
            slot->InstallToolTipPropertyObserver();
        if ( !toolTipObserverInstalled &&
             !slot->m_toolTipPropertyObserverPending )
        {
            winrt::throw_hresult(E_FAIL);
        }
        const bool automationStyleObserverInstalled =
            slot->InstallAutomationNameStylePropertyObserver();
        if ( !automationStyleObserverInstalled &&
             !slot->m_automationNameStylePropertyObserverPending )
        {
            winrt::throw_hresult(E_FAIL);
        }
        const bool loadedObserverInstalled =
            slot->InstallContentLoadedObserver();
        if ( !loadedObserverInstalled &&
             !slot->m_contentLoadedObserverPending )
        {
            winrt::throw_hresult(E_FAIL);
        }

        // A fresh registration writes Content() before the slot can safely
        // publish callbacks. Expose the same deterministic post-write
        // boundary as wxWinUISlot::SetContent(), but only after lifetime,
        // ownership and destroy handlers are authoritative. Application code
        // reached here may destroy, reparent or supersede this generation.
        std::function<void (wxWindow *)> carrierSetHook =
            std::move(gs_testContentCarrierSetHook);
        if ( carrierSetHook )
        {
            wxWindow * const currentWindow =
                slot->m_lifetime
                    ? slot->m_lifetime->GetWindow()
                    : nullptr;
            carrierSetHook(currentWindow);
        }

        if ( !observeRequestedContent() )
            return abandonInvalidFreshSlot();

        // Attach only after the slot is discoverable and its destroy handler
        // is bound: Append() can synchronously raise Loaded, and application
        // code is allowed to destroy/reparent the wx window from that event.
        m_root.Children().Append(slot->m_container);

        // Test the same arbitrary-callback boundary deterministically. The
        // hook is consumed before invocation so a nested migration cannot
        // recurse through the test mechanism unless it explicitly rearms it.
        std::function<void (wxWindow *)> attachedHook =
            std::move(gs_testSlotAttachedHook);
        if ( attachedHook )
            attachedHook(window);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot registration", e);
        rollback();
        return nullptr;
    }
    catch ( ... )
    {
        rollback();
        throw;
    }

    if ( m_shuttingDown || !observeRequestedContent() )
    {
        // A synchronous Loaded handler already performed the truthful
        // unregister/migration. A nested content replacement in this same
        // valid slot is NOT rolled back: it remains the authoritative model.
        return abandonInvalidFreshSlot();
    }

    InvalidateStructure();
    MarkDirty(window);

    // Accessibility authority is not allowed to wait for the scheduled
    // geometry flush: a caller can issue WM_GETOBJECT immediately after
    // Create(). The helper revalidates the exact slot/content generation
    // around any attached-property callback.
    SyncSlotAccessibilityAuthority(*slot, initialHasWxAccessible);
    if ( !observeRequestedContent() )
        return abandonInvalidFreshSlot();

#if wxUSE_TOOLTIPS
    if ( wxToolTip * const tip = window->GetToolTip() )
        tip->SetWindow(window);

    if ( !observeRequestedContent() )
        return abandonInvalidFreshSlot();

    // Initial registration bypasses wxWinUISlot::SetContent(). Apply the
    // process tooltip policy here as part of the same publication boundary:
    // a frozen destination host must never expose an authored tooltip merely
    // because its deferred geometry flush cannot run yet.
    if ( element && !wxWinUIAreToolTipsEnabled() )
        SynchronizeSlotToolTipPolicy(*slot);
#endif

    if ( !observeRequestedContent() )
        return abandonInvalidFreshSlot();

    // A newer B->C publication deliberately survives and completes all common
    // slot finalization above, but the OUTER caller must not publish B's model
    // or Loaded hook after C has won.
    return requestedContentSuperseded ? nullptr : slot;
}

void wxWinUITopLevelHost::RevokeSlotCallbacks(wxWinUISlot& slot)
{
    // 1. No callback may reach wx through this slot any more, even one
    //    already queued in the dispatcher.
    if ( slot.m_lifetime )
        slot.m_lifetime->Invalidate();
    slot.RetireExposedAccessibilityShellProvider(true);

    if ( !slot.m_container )
        return;

    // 2. Revoke every handler so the container stops referencing the
    //    lambdas (and through them the shared state).
    for ( auto& handler : slot.m_routedHandlers )
    {
        try
        {
            slot.m_container.RemoveHandler(handler.first, handler.second);
            ++gs_slotHandlerRevokes;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    slot.m_routedHandlers.clear();

    if ( slot.m_gotFocusToken )
    {
        try
        {
            slot.m_container.GotFocus(slot.m_gotFocusToken);
            ++gs_slotHandlerRevokes;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    if ( slot.m_lostFocusToken )
    {
        try
        {
            slot.m_container.LostFocus(slot.m_lostFocusToken);
            ++gs_slotHandlerRevokes;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    slot.m_gotFocusToken = {};
    slot.m_lostFocusToken = {};
}

void wxWinUITopLevelHost::DisconnectSlot(wxWinUISlot& slot)
{
    const bool ownedActiveCursor =
        m_activeCursorSurface == ActiveCursorSurface::Slot &&
        m_activeCursorSlot.lock() == slot.m_lifetime;

    if ( m_deferredFocusRequest.target == slot.m_lifetime )
    {
        ClearDeferredFocusRequest(
            m_deferredFocusRequest.intentGeneration);
    }

    ForgetSlotPointerOwners(slot.m_lifetime);
    RevokeSlotCallbacks(slot);
    slot.RevokeToolTipPropertyObserver();
    slot.RevokeAutomationNameStylePropertyObserver();
    slot.RevokeContentLoadedObserver();

    // Drop the preferred-focus reference with the slot's connections.
    slot.m_preferredFocus = nullptr;

    // The content outlives its slot and may be re-used by the application.
    // Return every host-owned attached value before detaching it. Failure is
    // logged by the helpers and is best-effort during teardown (there is no
    // truthful rollback here).
    slot.RestoreAutomationName();
    slot.RestoreHelpText();
    slot.RestoreToolTip();
    slot.RestoreAccessibilityView();

    if ( slot.m_container )
    {
        // 3. Detach the hosted element (it belongs to the control, not to
        //    the slot) ...
        try
        {
            if ( slot.m_carrier )
                slot.m_carrier.Content(nullptr);
            slot.m_container.Children().Clear();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        slot.m_content = nullptr;
        slot.m_semanticTarget = nullptr;

        // 4. ... and drop the container from the shared tree.
        if ( m_root )
        {
            try
            {
                uint32_t index = 0;
                if ( m_root.Children().IndexOf(slot.m_container, index) )
                    m_root.Children().RemoveAt(index);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("slot removal", e);
            }
        }
    }

    gs_slotGrids.erase(&slot);

    // Never leave an island-level custom cursor owned by a slot whose
    // generation has just been retired.
    if ( ownedActiveCursor && !m_shuttingDown )
        RefreshPointerCursorAtLastPoint();
}

void wxWinUITopLevelHost::SetSlotPhysicalDisconnectGate(
    wxWindow *window,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    OperationGuard hostOperation(this);
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_disconnectPending )
        return;
    slot->m_disconnectGate = gate;
}

void wxWinUITopLevelHost::UnregisterSlot(
    wxWindow *window,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& requestedGate)
{
    OperationGuard hostOperation(this);

    if ( m_nativeResize && m_nativeResize->source.get() == window )
    {
        wxWinUIPointerSample cancel;
        cancel.kind = wxWinUIInputKind::Cancel;
        cancel.interruptAll = true;
        CancelNativeResize(cancel);
        if ( m_shuttingDown )
            return;
    }

    const auto it = m_slots.find(window);
    if ( it == m_slots.end() )
        return;

    wxWinUISlot * const slot = it->second;
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> gate =
        requestedGate ? requestedGate : slot->m_disconnectGate;
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> deferredGate =
        gate && !gate->IsSatisfied() ? gate : nullptr;
    const bool mustDeferPhysicalDisconnect =
        slot->m_operationDepth != 0 || deferredGate;

    if ( mustDeferPhysicalDisconnect )
    {
        try
        {
            // Reserve the stable raw-slot owner before invalidating/removing
            // any published route. Allocation failure after logical detach
            // would otherwise leave neither a normal completion nor a
            // terminal-retirement owner.
            m_deferredDisconnectSlots.reserve(
                m_deferredDisconnectSlots.size() + 1);
        }
        catch ( ... )
        {
            std::terminate();
        }

    }

    if ( deferredGate )
    {
        std::uint64_t retirementId =
            ++gs_nextPhysicalDisconnectRetirementId;
        if ( !retirementId )
            retirementId = ++gs_nextPhysicalDisconnectRetirementId;
        slot->m_disconnectRetirementId = retirementId;
        slot->m_disconnectGate = deferredGate;
        const std::shared_ptr<wxWinUIHostLifetime> hostState =
            m_hostLifetime;
        if ( !hostState )
            std::terminate();
        if ( !deferredGate->SetCompletion(
                 [hostState, retirementId]() noexcept
                 {
                     wxWinUITopLevelHost * const host =
                         hostState ? hostState->GetHost() : nullptr;
                     if ( host )
                     {
                         host->CompleteDeferredSlotDisconnect(
                             retirementId);
                     }
                 }) )
        {
            // The control graph is still attached and the logical slot is
            // still published. Losing the only causal completion destination
            // here cannot be repaired after detach.
            std::terminate();
        }
    }
    else
    {
        slot->m_disconnectGate.reset();
        slot->m_disconnectRetirementId = 0;
    }
    wxWinUISlot::OperationGuard slotOperation(slot);

    UnbindSlotWindow(*slot);
    m_dirty.erase(window);
    m_slots.erase(it);

    const auto owner = gs_slotOwners.find(window);
    if ( owner != gs_slotOwners.end() && owner->second == this )
        gs_slotOwners.erase(owner);

    if ( mustDeferPhysicalDisconnect )
    {
        // The routed callback which caused this removal still has the slot's
        // visual source on its WinUI stack. Retire every logical route now,
        // but leave RemoveHandler()/tree detachment to a single dispatcher
        // turn after the outer slot operation has unwound.
        if ( slot->m_lifetime )
            slot->m_lifetime->Invalidate();
        slot->RetireExposedAccessibilityShellProvider(true);
        if ( m_deferredFocusRequest.target == slot->m_lifetime )
        {
            ClearDeferredFocusRequest(
                m_deferredFocusRequest.intentGeneration);
        }
        ForgetSlotPointerOwners(slot->m_lifetime);
        slot->m_disconnectPending = true;
        slot->m_deferredDisconnectHost = this;
        m_deferredDisconnectSlots.push_back(slot);
        if ( deferredGate )
        {
            gs_pendingPhysicalDisconnectGates.fetch_add(
                1, std::memory_order_relaxed);
        }
    }
    else
    {
        slot->m_disconnectGate.reset();
        slot->m_disconnectRetirementId = 0;
        DisconnectSlot(*slot);
    }
    slot->MarkForDeletion();

    // Removing a sibling changes the mirrored paint order of every slot
    // after it. Keep this in the same coalesced generation as the removal.
    if ( !m_shuttingDown )
    {
        InvalidateStructure();
        ScheduleFlush();
    }
}

void wxWinUITopLevelHost::ScheduleDeferredSlotDisconnects()
{
    if ( m_shuttingDown || m_deferredDisconnectSlots.empty() ||
         m_deferredDisconnectScheduled || !wxTheApp )
    {
        return;
    }

    m_deferredDisconnectScheduled = true;
    const std::shared_ptr<wxWinUIHostLifetime> state = m_hostLifetime;
    wxTheApp->CallAfter(
        [state]()
        {
            wxWinUITopLevelHost * const host =
                state ? state->GetHost() : nullptr;
            if ( host )
                host->FlushDeferredSlotDisconnects();
        });
}

bool wxWinUITopLevelHost::CompleteDeferredSlotDisconnect(
    wxWinUISlot& slot)
{
    if ( !slot.m_deletePending || !slot.m_disconnectPending ||
         slot.m_deferredDisconnectHost != this ||
         slot.m_operationDepth != 0 ||
         (slot.m_disconnectGate &&
          !slot.m_disconnectGate->IsSatisfied()) )
    {
        return false;
    }

    wxWinUISlot::OperationGuard slotOperation(&slot);
    const HWND entryAuthority =
        static_cast<HWND>(m_nativeFocusEntryAuthority);
    const unsigned long long entryAuthorityGeneration =
        m_nativeFocusEntryAuthorityGeneration;
    const unsigned long long entryEpoch =
        m_nativeFocusEntryEpoch;
    slot.m_disconnectPending = false;
    slot.m_deferredDisconnectHost = nullptr;
    if ( slot.m_disconnectGate )
    {
        gs_pendingPhysicalDisconnectGates.fetch_sub(
            1, std::memory_order_relaxed);
        slot.m_disconnectGate.reset();
    }
    slot.m_disconnectRetirementId = 0;
    DisconnectSlot(slot);

    // WinUI may have returned true from UIElement::Focus() after the
    // GettingFocus predicate retired this slot. GotFocus then sees the
    // invalid lifetime and correctly publishes no owner, but the native
    // InputSite can still be left focused. Restore exactly the external
    // authority displaced on entry; a newer owner/native redirection wins.
    const bool ownsEntryTransaction =
        entryEpoch != 0 &&
        slot.m_nativeFocusEntryEpoch == entryEpoch &&
        m_nativeFocusEntryEpoch == entryEpoch &&
        m_nativeFocusEntryTarget.lock() == slot.m_lifetime;
    if ( ownsEntryTransaction && !m_shuttingDown && !m_focusOwner &&
         HasNativeFocusAuthority() )
    {
        m_nativeFocusEntryAuthority = nullptr;
        m_nativeFocusEntryAuthorityGeneration = 0;
        m_nativeFocusEntryTarget.reset();
        m_nativeFocusEntryEpoch = 0;
        slot.m_nativeFocusEntryEpoch = 0;
        const bool entryIsCurrent =
            entryAuthority && ::IsWindow(entryAuthority) &&
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(entryAuthority)) ==
                entryAuthorityGeneration;
        ::SetFocus(entryIsCurrent ? entryAuthority : nullptr);
    }

    // MarkForDeletion() was published together with logical removal.
    // slotOperation deletes the slot after this transaction returns.
    return true;
}

void wxWinUITopLevelHost::CompleteDeferredSlotDisconnect(
    std::uint64_t retirementId) noexcept
{
    if ( !retirementId )
        return;

    OperationGuard hostOperation(this);
    if ( m_shuttingDown )
        return;

    const auto pending = std::find_if(
        m_deferredDisconnectSlots.begin(),
        m_deferredDisconnectSlots.end(),
        [this, retirementId](wxWinUISlot *slot)
        {
            return slot && slot->m_disconnectPending &&
                   slot->m_deferredDisconnectHost == this &&
                   slot->m_disconnectRetirementId == retirementId;
        });
    if ( pending == m_deferredDisconnectSlots.end() )
        return;

    wxWinUISlot * const slot = *pending;
    if ( slot->m_operationDepth != 0 )
        return;

    // Remove the vector's raw pointer before the common transaction can
    // delete the slot allocation at its OperationGuard tail.
    m_deferredDisconnectSlots.erase(pending);
    if ( !CompleteDeferredSlotDisconnect(*slot) )
        std::terminate();
}

wxWinUITopLevelHost::DeferredTransferDisconnectResult
wxWinUITopLevelHost::CompleteDeferredTransferDisconnect(
    wxWindow *window,
    WXHWND hwnd,
    const winrt::Microsoft::UI::Xaml::UIElement& content)
{
    OperationGuard hostOperation(this);

    // A failed destination registration can retire its slot from a Loaded
    // or application callback while the registration operation still owns
    // the allocation. Once that callback has unwound and registration has
    // returned, release only this exact provisional carrier synchronously:
    // the source rollback cannot reattach the same UIElement while the
    // retired destination still parents it.
    const auto pending = std::find_if(
        m_deferredDisconnectSlots.begin(),
        m_deferredDisconnectSlots.end(),
        [this, window, hwnd, &content](wxWinUISlot *slot)
        {
            return slot &&
                   slot->m_window == window &&
                   slot->m_hwnd == hwnd &&
                   slot->m_content == content &&
                   slot->m_deletePending &&
                   slot->m_disconnectPending &&
                   slot->m_deferredDisconnectHost == this;
        });
    if ( pending == m_deferredDisconnectSlots.end() )
        return DeferredTransferDisconnectResult::NotFound;

    wxWinUISlot * const slot = *pending;
    if ( m_shuttingDown || slot->m_operationDepth != 0 )
    {
        // Never attempt to parent the transferred element elsewhere while
        // the retired carrier can still be on a XAML callback stack.
        return DeferredTransferDisconnectResult::StillActive;
    }

    m_deferredDisconnectSlots.erase(pending);
    if ( CompleteDeferredSlotDisconnect(*slot) )
        return DeferredTransferDisconnectResult::Completed;

    // No callback boundary exists between the readiness check and the common
    // completion helper, but preserve ownership defensively if a future
    // implementation adds one.
    m_deferredDisconnectSlots.push_back(slot);
    return DeferredTransferDisconnectResult::StillActive;
}

void wxWinUITopLevelHost::FlushDeferredSlotDisconnects()
{
    OperationGuard hostOperation(this);
    m_deferredDisconnectScheduled = false;
    if ( m_shuttingDown || m_deferredDisconnectSlots.empty() )
        return;

    std::vector<wxWinUISlot *> pending;
    pending.swap(m_deferredDisconnectSlots);
    for ( wxWinUISlot *slot : pending )
    {
        if ( !slot || !slot->m_disconnectPending ||
             slot->m_deferredDisconnectHost != this )
            continue;

        if ( !CompleteDeferredSlotDisconnect(*slot) &&
             slot->m_disconnectPending &&
             slot->m_deferredDisconnectHost == this )
        {
            m_deferredDisconnectSlots.push_back(slot);
        }
    }

    // A nested slot operation can still be active when an unrelated slot
    // schedules this pass. Its own EndOperation() will schedule the next
    // single turn; never repost from here in a loop.
}

wxWinUISlot *wxWinUITopLevelHost::FindSlot(const wxWindow *window) const
{
    const auto it = m_slots.find(const_cast<wxWindow *>(window));
    return it != m_slots.end() ? it->second : nullptr;
}

bool wxWinUITopLevelHost::TryGetAccessibilityShellState(
    const wxWindow *window,
    std::shared_ptr<wxWinUISlotLifetime> *lifetime,
    WXHWND *hwnd,
    unsigned long long *hwndGeneration,
    unsigned long long *contentGeneration,
    bool *hasWxAccessible,
    unsigned long long *authorityGeneration) const
{
    if ( !window || !lifetime || !hwnd || !hwndGeneration ||
            !contentGeneration || !hasWxAccessible ||
            !authorityGeneration ||
            window == m_tlw || window->IsTopLevel() ||
            window->IsBeingDeleted() || m_shuttingDown )
    {
        return false;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_poisoned || slot->m_deletePending ||
            !slot->m_lifetime ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return false;
    }

    const WXHWND currentHwnd = window->GetHWND();
    if ( !currentHwnd || slot->m_hwnd != currentHwnd )
        return false;

    const unsigned long long currentContentGeneration =
        slot->m_contentGeneration;
    const unsigned long long generation =
        wxWinUIMSWGetHwndGeneration(
            const_cast<wxWindow *>(window), currentHwnd);
    if ( !generation )
        return false;

    // Revalidate after the generation query: it crosses into USER32 and a
    // subclass may synchronously destroy or rebind the window.
    if ( FindSlot(window) != slot ||
            slot->m_deletePending || slot->m_poisoned ||
            slot->m_hwnd != window->GetHWND() ||
            slot->m_contentGeneration != currentContentGeneration ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return false;
    }

    *lifetime = slot->m_lifetime;
    *hwnd = currentHwnd;
    *hwndGeneration = generation;
    *contentGeneration = currentContentGeneration;
    *hasWxAccessible = slot->m_hasWxAccessible;
    *authorityGeneration = slot->m_accessibilityAuthorityGeneration;
    return true;
}

unsigned long long wxWinUITopLevelHost::PrepareFocusReparent(
    wxWindow *subtreeRoot,
    wxWindow *oldTLW,
    wxWindow *newTLW)
{
    if ( !subtreeRoot || !oldTLW || !newTLW ||
         oldTLW == newTLW )
        return 0;

    const wxWeakRef<wxWindow> subtreeIdentity(subtreeRoot);
    const wxWeakRef<wxWindow> sourceTLWIdentity(oldTLW);
    const WXHWND sourceTLWHwnd = oldTLW->GetHWND();
    const unsigned long long sourceTLWHwndGeneration =
        wxWinUIMSWGetHwndGeneration(oldTLW, sourceTLWHwnd);
    const wxWeakRef<wxWindow> destinationTLWIdentity(newTLW);
    const WXHWND destinationTLWHwnd = newTLW->GetHWND();
    const unsigned long long destinationTLWHwndGeneration =
        wxWinUIMSWGetHwndGeneration(newTLW, destinationTLWHwnd);
    if ( !sourceTLWHwnd || !sourceTLWHwndGeneration ||
         !destinationTLWHwnd ||
         !destinationTLWHwndGeneration )
    {
        return 0;
    }
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());

    wxWinUITopLevelHost * const oldHost = FindForTLW(oldTLW);
    if ( oldHost )
        hosts.push_back(oldHost->m_hostLifetime);
    for ( const auto& entry : gs_tlwHosts )
    {
        if ( entry.second != oldHost &&
             entry.second->IsReadyForLookup() )
        {
            hosts.push_back(entry.second->m_hostLifetime);
        }
    }

    for ( const auto& hostState : hosts )
    {
        wxWindow *root = subtreeIdentity.get();
        wxWindow *sourceTLW = sourceTLWIdentity.get();
        wxWindow *destinationTLW = destinationTLWIdentity.get();
        wxWinUITopLevelHost * const host =
            hostState ? hostState->GetHost() : nullptr;
        if ( !root || root->IsBeingDeleted() ||
             !sourceTLW ||
             sourceTLW->IsBeingDeleted() ||
             sourceTLW->GetHWND() != sourceTLWHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 sourceTLW, sourceTLWHwnd) !=
                 sourceTLWHwndGeneration ||
             !destinationTLW ||
             destinationTLW->IsBeingDeleted() ||
             destinationTLW->GetHWND() != destinationTLWHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 destinationTLW, destinationTLWHwnd) !=
                 destinationTLWHwndGeneration ||
             !host || host->m_shuttingDown )
        {
            continue;
        }

        OperationGuard hostOperation(host);
        wxWindow * const window = host->m_focusOwner;
        if ( !window ||
             (window != root && !root->IsDescendant(window)) )
        {
            continue;
        }

        // A nested B->C reparent reuses the preparation already owned by
        // the outer A->B operation. It receives no second token: whichever
        // physical operation finally consumes the shared migration makes the
        // outer token stale, while a failed nested SetParent leaves it intact.
        const auto existing = ms_focusMigrations.find(window);
        if ( existing != ms_focusMigrations.end() ||
             (host->m_focusMigration &&
              !host->m_focusMigration->committed) )
        {
            return 0;
        }

        wxWinUISlot *slot = host->FindSlot(window);
        if ( !slot || slot->m_deletePending || !slot->m_lifetime ||
             slot->m_lifetime->GetHost() != host ||
             slot->m_lifetime->GetWindow() != window )
        {
            return 0;
        }

        const auto slotState = slot->m_lifetime;
        const unsigned long long contentGeneration =
            slot->m_contentGeneration;
        const WXHWND shell = window->GetHWND();
        const unsigned long long shellGeneration =
            wxWinUIMSWGetHwndGeneration(window, shell);
        const unsigned long long shellNativeGeneration =
            wxWinUIMSWGetNativeHwndGeneration(shell);
        const unsigned long long focusIntent =
            host->m_focusIntentGeneration;
        const HWND nativeAuthority = ::GetFocus();
        const unsigned long long nativeAuthorityGeneration =
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(nativeAuthority));
        if ( !shell || !shellGeneration ||
             !shellNativeGeneration ||
             !nativeAuthority || !nativeAuthorityGeneration ||
             !host->HasNativeFocusAuthority() )
        {
            return 0;
        }

        const auto container = slot->m_container;
        const auto xamlRoot = host->GetXamlRoot();
        const bool logicalFocusWasActive =
            wxWinUIFocusIsInside(container, xamlRoot);

        // FocusManager/XamlRoot reads above are arbitrary callback
        // boundaries. Reacquire the entire source identity before publishing
        // a ticket or moving native focus.
        root = subtreeIdentity.get();
        sourceTLW = sourceTLWIdentity.get();
        destinationTLW = destinationTLWIdentity.get();
        slot = host->FindSlot(window);
        if ( !logicalFocusWasActive ||
             !root || root->IsBeingDeleted() ||
             !sourceTLW ||
             sourceTLW->IsBeingDeleted() ||
             sourceTLW->GetHWND() != sourceTLWHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 sourceTLW, sourceTLWHwnd) !=
                 sourceTLWHwndGeneration ||
             !destinationTLW ||
             destinationTLW->IsBeingDeleted() ||
             destinationTLW->GetHWND() != destinationTLWHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 destinationTLW, destinationTLWHwnd) !=
                 destinationTLWHwndGeneration ||
             (window != root && !root->IsDescendant(window)) ||
             !hostState || hostState->GetHost() != host ||
             host->m_shuttingDown ||
             host->m_focusOwner != window ||
             host->m_focusIntentGeneration != focusIntent ||
             ::GetFocus() != nativeAuthority ||
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(nativeAuthority)) !=
                 nativeAuthorityGeneration ||
             !slot || slot->m_lifetime != slotState ||
             slot->m_contentGeneration != contentGeneration ||
             slot->m_container != container ||
             slotState->GetHost() != host ||
             slotState->GetWindow() != window ||
             window->GetHWND() != shell ||
             wxWinUIMSWGetHwndGeneration(window, shell) !=
                 shellGeneration ||
             ms_focusMigrations.find(window) !=
                 ms_focusMigrations.end() ||
             host->m_focusMigration )
        {
            return 0;
        }

        static unsigned long long nextPreparedReparentId = 0;
        if ( ++nextPreparedReparentId == 0 )
            ++nextPreparedReparentId;

        const auto migration = std::make_shared<FocusMigration>();
        migration->sourceHost = host->m_hostLifetime;
        migration->destinationHost = host->m_hostLifetime;
        migration->sourceSlot = slotState;
        migration->destinationSlot = slotState;
        migration->window = window;
        migration->windowIdentity = window;
        migration->hwnd = shell;
        migration->hwndGeneration = shellGeneration;
        migration->transportNativeAuthority = shell;
        migration->transportNativeAuthorityGeneration =
            shellNativeGeneration;
        migration->preparedDestinationTLW = destinationTLW;
        migration->preparedDestinationTLWHwnd =
            destinationTLWHwnd;
        migration->preparedDestinationTLWHwndGeneration =
            destinationTLWHwndGeneration;
        migration->preparedSourceTLW = sourceTLW;
        migration->preparedSourceTLWHwnd = sourceTLWHwnd;
        migration->preparedSourceTLWHwndGeneration =
            sourceTLWHwndGeneration;
        migration->sourceContentGeneration = contentGeneration;
        migration->destinationContentGeneration = contentGeneration;
        migration->sourceFocusIntentGeneration = focusIntent;
        migration->logicalFocusWasActive = true;
        migration->preparedReparentId = nextPreparedReparentId;
        migration->shellFocusParking = true;

        // Publish before SetFocus(): even this implementation-only parking
        // edge can synchronously activate windows, reparent again or destroy
        // either endpoint.
        host->m_focusMigration = migration;
        ms_focusMigrations[window] = migration;
        ::SetFocus(static_cast<HWND>(shell));

        root = subtreeIdentity.get();
        sourceTLW = sourceTLWIdentity.get();
        destinationTLW = destinationTLWIdentity.get();
        slot = hostState && hostState->GetHost() == host
            ? host->FindSlot(window)
            : nullptr;
        const auto pending = ms_focusMigrations.find(window);
        const bool parkingSucceeded =
            root && !root->IsBeingDeleted() &&
            sourceTLW &&
            !sourceTLW->IsBeingDeleted() &&
            sourceTLW->GetHWND() == sourceTLWHwnd &&
            wxWinUIMSWGetHwndGeneration(
                sourceTLW, sourceTLWHwnd) ==
                sourceTLWHwndGeneration &&
            destinationTLW &&
            !destinationTLW->IsBeingDeleted() &&
            destinationTLW->GetHWND() == destinationTLWHwnd &&
            wxWinUIMSWGetHwndGeneration(
                destinationTLW, destinationTLWHwnd) ==
                destinationTLWHwndGeneration &&
            (window == root || root->IsDescendant(window)) &&
            pending != ms_focusMigrations.end() &&
            pending->second == migration &&
            !migration->committed &&
            migration->preparedReparentId ==
                nextPreparedReparentId &&
            host->m_focusMigration == migration &&
            host->m_focusOwner == window &&
            host->m_focusIntentGeneration == focusIntent &&
            slot && slot->m_lifetime == slotState &&
            slot->m_contentGeneration == contentGeneration &&
            slot->m_container == container &&
            slotState->GetHost() == host &&
            slotState->GetWindow() == window &&
            window->GetHWND() == shell &&
            wxWinUIMSWGetHwndGeneration(window, shell) ==
                shellGeneration &&
            migration->shellFocusConsumed &&
            !migration->shellFocusParking &&
            migration->transportNativeAuthority == shell &&
            migration->transportNativeAuthorityGeneration != 0 &&
            wxWinUIMSWGetNativeHwndGeneration(shell) ==
                migration->transportNativeAuthorityGeneration &&
            ::GetFocus() == static_cast<HWND>(shell);
        if ( !parkingSucceeded )
        {
            migration->shellFocusParking = false;
            if ( !migration->committed )
            {
                const bool sourceStillFocused =
                    hostState && hostState->GetHost() == host &&
                    host->HasNativeFocusAuthority() &&
                    slot && slot->m_lifetime == slotState &&
                    wxWinUIFocusIsInside(container, host->GetXamlRoot());
                wxWinUITopLevelHost * const liveHost =
                    hostState ? hostState->GetHost() : nullptr;
                if ( liveHost && !migration->committed )
                {
                    liveHost->AbortFocusMigration(
                        migration, !sourceStillFocused);
                }
            }
            return 0;
        }

        if ( wxWinUIInputLogEnabled() )
        {
            wxWinUIInputLog(
                "FocusMigration prepared before SetParent "
                "token=%llu window=%p shell=%p",
                nextPreparedReparentId,
                window,
                static_cast<void *>(static_cast<HWND>(shell)));
        }
        return nextPreparedReparentId;
    }

    return 0;
}

void wxWinUITopLevelHost::SetPreparedFocusReparentNativeBoundary(
    unsigned long long token,
    bool active)
{
    if ( !token )
        return;

    for ( const auto& pending : ms_focusMigrations )
    {
        const std::shared_ptr<FocusMigration>& migration =
            pending.second;
        if ( migration && !migration->committed &&
             migration->preparedReparentId == token )
        {
            migration->preparedNativeReparentBoundaryActive =
                active;
            return;
        }
    }
}

void wxWinUITopLevelHost::CancelPreparedFocusReparent(
    unsigned long long token)
{
    if ( !token )
        return;

    std::shared_ptr<FocusMigration> migration;
    for ( const auto& pending : ms_focusMigrations )
    {
        if ( pending.second &&
             !pending.second->committed &&
             pending.second->preparedReparentId == token )
        {
            migration = pending.second;
            break;
        }
    }
    if ( !migration )
        return;

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWindow * const window = migration->window.get();
    wxWinUISlot * const slot =
        source && window ? source->FindSlot(window) : nullptr;

    // Consume the caller's lease before any focus call. A nested reparent
    // can now either carry this transport ticket forward or create a new
    // preparation; the stale outer scope guard cannot match either.
    migration->preparedNativeReparentBoundaryActive = false;
    migration->preparedReparentId = 0;

    const bool exactSource =
        source && !source->m_shuttingDown &&
        source->m_focusMigration == migration &&
        window && !window->IsBeingDeleted() &&
        wxGetTopLevelParent(window) == source->m_tlw &&
        slot && slot->m_lifetime == migration->sourceSlot &&
        slot->m_contentGeneration ==
            migration->sourceContentGeneration &&
        migration->sourceSlot->GetHost() == source &&
        migration->sourceSlot->GetWindow() == window &&
        source->m_focusOwner == window &&
        source->m_focusIntentGeneration ==
            migration->sourceFocusIntentGeneration &&
        window->GetHWND() == migration->hwnd &&
        wxWinUIMSWGetHwndGeneration(
            window, migration->hwnd) ==
            migration->hwndGeneration &&
        migration->shellFocusConsumed &&
        !migration->shellFocusParking &&
        ::GetFocus() == static_cast<HWND>(migration->hwnd);
    if ( !exactSource )
    {
        if ( source )
            source->AbortFocusMigration(migration, true);
        return;
    }

    migration->destinationHost = migration->sourceHost;
    migration->destinationSlot = migration->sourceSlot;
    migration->destinationContentGeneration =
        migration->sourceContentGeneration;
    source->FocusSlotImpl(
        window,
        FocusOrigin::MigrationContinuation,
        migration->previous.get(),
        migration->sourceFocusIntentGeneration,
        true);

    if ( !migration->committed )
    {
        const FocusRequest& deferred =
            source->m_deferredFocusRequest;
        const bool exactDeferred =
            source->m_focusMigration == migration &&
            deferred.origin == FocusOrigin::MigrationContinuation &&
            deferred.target == migration->sourceSlot &&
            deferred.intentGeneration ==
                source->m_focusIntentGeneration &&
            source->m_deferredFocusContentGeneration ==
                migration->sourceContentGeneration;
        if ( !exactDeferred )
            source->AbortFocusMigration(migration, true);
    }
}

bool wxWinUITopLevelHost::MigrateSlotToCurrentTLW(wxWindow *window)
{
    OperationGuard hostOperation(this);

    wxWinUISlot * const oldSlot = FindSlot(window);
    if ( !oldSlot )
        return false;
    wxWinUISlot::OperationGuard oldSlotOperation(oldSlot);
    const std::shared_ptr<wxWinUISlotLifetime> oldState =
        oldSlot->m_lifetime;

    wxWinUITopLevelHost *target = ForWindow(window, true);
    if ( !target )
        return false;
    if ( target == this )
        return true;
    if ( !oldState ||
         oldState->GetHost() != this ||
         oldState->GetWindow() != window ||
         oldSlot->m_deletePending ||
         FindSlot(window) != oldSlot )
    {
        return FindSlotOwner(window) == target;
    }

    std::shared_ptr<wxWinUIPhysicalDisconnectGate>
        physicalDisconnectGate = oldSlot->m_disconnectGate;
    if ( physicalDisconnectGate &&
         (!physicalDisconnectGate->IsSatisfied() ||
          physicalDisconnectGate->IsDegraded()) )
    {
        // The MSW preflight normally refuses this reparent before any logical
        // or native mutation. Keep this last line of defence before any XAML
        // detach for direct/internal reconciliation callers.
        return false;
    }

    bool accessibilityFactoryResolvedForTransfer = false;
#if wxUSE_ACCESSIBILITY
    if ( window != m_tlw && !window->IsTopLevel() )
    {
        // Resolve the lazy factory while the source slot still owns its full
        // content and provenance. CreateAccessible() is arbitrary application
        // code and may synchronously reparent A->B->C. In that case the
        // nested migration must observe the intact A payload, not the empty
        // source left after our detach phase.
        const WXHWND accessibleHwnd = window->GetHWND();
        const unsigned long long accessibleHwndGeneration =
            wxWinUIMSWGetHwndGeneration(window, accessibleHwnd);
        if ( !accessibleHwnd || !accessibleHwndGeneration )
            return false;

        window->GetOrCreateAccessible();
        accessibilityFactoryResolvedForTransfer = true;

        // Validate the raw identity before dereferencing window again: the
        // virtual factory may have destroyed or rebound it.
        if ( wxFindWinFromHandle(
                 reinterpret_cast<HWND>(accessibleHwnd)) != window ||
             wxWinUIMSWGetHwndGeneration(
                 window, accessibleHwnd) != accessibleHwndGeneration )
        {
            return false;
        }

        // The factory may have changed the wx topology. Recompute the target
        // and accept a completed nested migration only when its owner is the
        // truthful host of the current TLW.
        target = ForWindow(window, true);
        if ( !target )
            return false;
        if ( target == this )
            return true;
        if ( window->IsBeingDeleted() ||
             !oldState ||
             oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             oldSlot->m_deletePending ||
             FindSlot(window) != oldSlot )
        {
            return !window->IsBeingDeleted() &&
                   FindSlotOwner(window) == target;
        }
    }
#endif // wxUSE_ACCESSIBILITY

    // Create the target host before touching the old slot, then detach the
    // content transactionally.  If registration in the target fails, restore
    // it in the still-live old slot instead of losing the control entirely.
    const WXHWND transferHwnd = window->GetHWND();
    const unsigned long long transferHwndGeneration =
        wxWinUIMSWGetHwndGeneration(window, transferHwnd);
    if ( !transferHwnd || !transferHwndGeneration )
        return false;

    const auto content = oldSlot->m_content;
    const auto semanticTarget = oldSlot->m_semanticTarget;
    const unsigned long long contentGeneration =
        oldSlot->m_contentGeneration;
    const unsigned long long automationNameStateRevision =
        oldSlot->m_uiaNameStateRevision;
    const unsigned long long automationNameStyleRevision =
        oldSlot->m_automationNameStyleRevision;
    const auto preferredFocus = oldSlot->m_preferredFocus;
    wxWinUIClipRectDIP clipRectDIP = oldSlot->m_clipRectDIP;
    bool hasClipRect = oldSlot->m_hasClipRect;
    unsigned long long clipRevision = oldSlot->m_clipRevision;
    const bool hasAutomationNameOverride =
        oldSlot->m_hasAutomationNameOverride;
    const wxString automationNameOverride =
        oldSlot->m_automationNameOverride;
    const unsigned long long automationNameOverrideRevision =
        oldSlot->m_automationNameOverrideRevision;
    bool preserveAutomationNameRelinquishment =
        oldSlot->ShouldPreserveAutomationNameRelinquishment();

    // Reading explicit Style/effective Name crosses WinRT and can materialize
    // deferred application state. Revalidate both the HWND and exact source
    // content generation before any detach. If a callback completed a nested
    // migration, accept only the owner matching the recomputed current TLW.
    if ( wxFindWinFromHandle(
             reinterpret_cast<HWND>(transferHwnd)) != window ||
         wxWinUIMSWGetHwndGeneration(
             window, transferHwnd) != transferHwndGeneration )
    {
        return false;
    }

    target = ForWindow(window, true);
    if ( !target )
        return false;
    if ( target == this )
        return true;
    if ( oldState->GetHost() != this ||
         oldState->GetWindow() != window ||
         oldSlot->m_deletePending ||
         FindSlot(window) != oldSlot ||
         oldSlot->m_content != content ||
         oldSlot->m_semanticTarget != semanticTarget ||
         oldSlot->m_disconnectGate != physicalDisconnectGate ||
         !wxWinUISemanticPairIsValid(content, semanticTarget) ||
         oldSlot->m_contentGeneration != contentGeneration ||
         oldSlot->m_uiaNameStateRevision !=
             automationNameStateRevision ||
         oldSlot->m_automationNameStyleRevision !=
             automationNameStyleRevision ||
         oldSlot->m_clipRevision != clipRevision )
    {
        return FindSlotOwner(window) == target;
    }
    const std::shared_ptr<wxWinUIHostLifetime> transferTargetState =
        target->m_hostLifetime;
    if ( !transferTargetState ||
         transferTargetState->GetHost() != target ||
         target->m_shuttingDown )
    {
        return false;
    }

    // Native focus is single-owner per host. A different in-flight migration
    // must be closed explicitly before this window can snapshot or publish a
    // new ticket; overwriting m_focusMigration would orphan its global entry
    // and, more importantly, its logical focus owner.
    if ( m_focusMigration && !m_focusMigration->committed &&
         m_focusMigration->window.get() != window )
    {
        const std::shared_ptr<FocusMigration> incumbent =
            m_focusMigration;
        AbortFocusMigration(incumbent, true);
        if ( oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             oldSlot->m_deletePending ||
             FindSlot(window) != oldSlot ||
             oldSlot->m_content != content ||
             oldSlot->m_contentGeneration != contentGeneration ||
             oldSlot->m_disconnectGate != physicalDisconnectGate ||
             oldSlot->m_uiaNameStateRevision !=
                  automationNameStateRevision ||
              oldSlot->m_automationNameStyleRevision !=
                  automationNameStyleRevision ||
              oldSlot->m_clipRevision != clipRevision ||
              window->GetHWND() != transferHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 window, transferHwnd) != transferHwndGeneration ||
             transferTargetState->GetHost() != target ||
             ForWindow(window, false) != target )
        {
            return FindSlotOwner(window) == target;
        }
    }

    // Focus is a logical wx property, not a property of either XamlRoot.
    // Capture it immediately before detach, while the exact source slot and
    // content generation are still authoritative. A nested A->B->C transfer
    // reuses the original ticket: only the final carrier may commit it.
    std::shared_ptr<FocusMigration> focusMigration;
    bool createdFocusMigration = false;
    bool consumingPreparedReparent = false;
    const auto pendingFocusMigration =
        ms_focusMigrations.find(window);
    if ( pendingFocusMigration != ms_focusMigrations.end() )
    {
        const std::shared_ptr<FocusMigration> pending =
            pendingFocusMigration->second;
        wxWinUISlot * const publishedDestination =
            target->FindSlot(window);
        const FocusRequest& publishedDeferred =
            target->m_deferredFocusRequest;
        const bool alreadyForwardedFromThisSource =
            pending && !pending->committed &&
            pending->logicalFocusWasActive &&
            pending->window.get() == window &&
            pending->windowIdentity == window &&
            pending->sourceHost == m_hostLifetime &&
            pending->sourceSlot == oldState &&
            m_focusMigration == pending &&
            oldSlot->m_operationDepth > 1 &&
            !oldSlot->m_content &&
            !oldSlot->m_semanticTarget &&
            oldSlot->m_contentGeneration !=
                pending->sourceContentGeneration &&
            m_focusOwner == window &&
            m_focusIntentGeneration ==
                pending->sourceFocusIntentGeneration &&
            pending->destinationHost == transferTargetState &&
            target->m_focusMigration == pending &&
            pending->destinationSlot &&
            publishedDestination &&
            publishedDestination->m_lifetime ==
                pending->destinationSlot &&
            !publishedDestination->m_deletePending &&
            pending->destinationSlot->GetHost() == target &&
            pending->destinationSlot->GetWindow() == window &&
            publishedDestination->m_contentGeneration ==
                pending->destinationContentGeneration &&
            publishedDeferred.origin ==
                FocusOrigin::MigrationContinuation &&
            publishedDeferred.target ==
                pending->destinationSlot &&
            publishedDeferred.intentGeneration ==
                target->m_focusIntentGeneration &&
            target->m_deferredFocusContentGeneration ==
                pending->destinationContentGeneration &&
            FindSlotOwner(window) == target &&
            window->GetHWND() == pending->hwnd &&
            wxWinUIMSWGetHwndGeneration(
                window, pending->hwnd) ==
                    pending->hwndGeneration &&
            pending->shellFocusConsumed &&
            !pending->shellFocusParking &&
            pending->preparedReparentId == 0 &&
            !pending->preparedNativeReparentBoundaryActive &&
            HasFocusMigrationNativeAuthority(pending, target);
        if ( alreadyForwardedFromThisSource )
        {
            // HandleReparentAcrossHosts() snapshots every host. During a
            // re-entrant A->B->C attach, its B->C pass can therefore revisit
            // the still-suspended A source after C has already accepted the
            // same ticket. A remains the logical arbiter until C commits, but
            // it is no longer the carrier and must not retire C's exact
            // bounded continuation. The outer A->B transaction will remove
            // A's detached slot when its registration stack unwinds.
            return true;
        }

        if ( pending && !pending->committed &&
             pending->window.get() == window &&
             pending->destinationHost == m_hostLifetime &&
             pending->destinationSlot == oldState &&
             m_focusMigration == pending &&
             window->GetHWND() == pending->hwnd &&
             wxWinUIMSWGetHwndGeneration(
                 window, pending->hwnd) ==
                 pending->hwndGeneration )
        {
            focusMigration = pending;
            consumingPreparedReparent =
                pending->preparedReparentId != 0 &&
                pending->sourceHost == m_hostLifetime &&
                pending->sourceSlot == oldState;
        }
        else
        {
            // A map entry which cannot identify this exact carrier is stale
            // or was superseded by a re-entrant focus redirect. Retire it
            // before creating any newer transaction for the same wxWindow.
            AbortFocusMigration(pending, true);
            if ( !pending )
                ms_focusMigrations.erase(pendingFocusMigration);
            return FindSlotOwner(window) == target;
        }
    }

    bool logicalFocusWasActive = false;
    bool deferredFocusWasPending = false;
    FocusRequest deferredFocus;
    if ( !focusMigration )
    {
        const unsigned long long focusIntentSnapshot =
            m_focusIntentGeneration;
        const HWND nativeAuthoritySnapshot = ::GetFocus();
        const unsigned long long nativeAuthorityGeneration =
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(nativeAuthoritySnapshot));
        const bool logicalFocusWasCandidate =
            m_focusOwner == window && HasNativeFocusAuthority();
        logicalFocusWasActive =
            logicalFocusWasCandidate &&
            wxWinUIFocusIsInside(
                oldSlot->m_container, GetXamlRoot());
        deferredFocusWasPending =
            m_deferredFocusRequest.target == oldState &&
            m_deferredFocusRequest.intentGeneration ==
                m_focusIntentGeneration &&
            m_deferredFocusContentGeneration == contentGeneration;
        if ( deferredFocusWasPending )
            deferredFocus = m_deferredFocusRequest;

        // FocusManager and every WinRT property read are re-entrant
        // boundaries. Never park or detach an identity that ceased to be the
        // source while the focus snapshot was being computed.
        if ( oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             oldSlot->m_deletePending ||
             FindSlot(window) != oldSlot ||
             oldSlot->m_content != content ||
             oldSlot->m_contentGeneration != contentGeneration ||
             oldSlot->m_disconnectGate != physicalDisconnectGate ||
             oldSlot->m_uiaNameStateRevision !=
                  automationNameStateRevision ||
              oldSlot->m_automationNameStyleRevision !=
                  automationNameStyleRevision ||
              oldSlot->m_clipRevision != clipRevision ||
              window->GetHWND() != transferHwnd ||
             wxWinUIMSWGetHwndGeneration(
                  window, transferHwnd) !=
                  transferHwndGeneration ||
             (logicalFocusWasCandidate &&
              (m_focusOwner != window ||
               m_focusIntentGeneration != focusIntentSnapshot ||
               ::GetFocus() != nativeAuthoritySnapshot ||
               wxWinUIMSWGetNativeHwndGeneration(
                   reinterpret_cast<WXHWND>(
                       nativeAuthoritySnapshot)) !=
                   nativeAuthorityGeneration)) ||
             transferTargetState->GetHost() != target ||
             ForWindow(window, false) != target )
        {
            return FindSlotOwner(window) == target;
        }

        if ( logicalFocusWasActive || deferredFocusWasPending )
        {
            focusMigration = std::make_shared<FocusMigration>();
            createdFocusMigration = true;
            focusMigration->sourceHost = m_hostLifetime;
            focusMigration->sourceSlot = oldState;
            focusMigration->window = window;
            focusMigration->windowIdentity = window;
            focusMigration->previous =
                deferredFocusWasPending
                    ? deferredFocus.previous
                    : wxWeakRef<wxWindow>();
            focusMigration->hwnd = transferHwnd;
            focusMigration->hwndGeneration =
                transferHwndGeneration;
            focusMigration->transportNativeAuthority =
                transferHwnd;
            focusMigration->transportNativeAuthorityGeneration =
                wxWinUIMSWGetNativeHwndGeneration(transferHwnd);
            focusMigration->sourceContentGeneration =
                contentGeneration;
            focusMigration->sourceFocusIntentGeneration =
                m_focusIntentGeneration;
            focusMigration->logicalFocusWasActive =
                logicalFocusWasActive;
            focusMigration->deferredFocusWasPending =
                deferredFocusWasPending;
            if ( deferredFocusWasPending )
            {
                focusMigration->deferredOrigin =
                    deferredFocus.origin;
                focusMigration->deferredNativeAuthority =
                    deferredFocus.nativeAuthority;
                focusMigration->deferredNativeAuthorityGeneration =
                    deferredFocus.nativeAuthorityGeneration;
            }

            m_focusMigration = focusMigration;
            ms_focusMigrations[window] = focusMigration;
        }
    }

    if ( focusMigration )
    {
        // Publish the next host before any detach callback. For a re-entrant
        // B->C hop, retire B's host-local alias while preserving A as the
        // sole logical source.
        if ( !createdFocusMigration &&
             m_focusMigration == focusMigration &&
             focusMigration->sourceHost != m_hostLifetime )
        {
            m_focusMigration.reset();
        }
        focusMigration->destinationHost = transferTargetState;
        focusMigration->destinationSlot.reset();
        focusMigration->destinationContentGeneration = 0;

        if ( createdFocusMigration &&
             focusMigration->logicalFocusWasActive )
        {
            // Move source XAML focus to the already-reparented shell. Its
            // WM_SETFOCUS is consumed by the generation-bound hook, and the
            // source LostFocus is suppressed by this ticket, so no public
            // KILL/SET pair is manufactured for an implementation move.
            focusMigration->shellFocusParking = true;
            focusMigration->shellFocusConsumed = false;
            ::SetFocus(static_cast<HWND>(transferHwnd));

            const bool parkingSucceeded =
                !focusMigration->committed &&
                focusMigration->shellFocusConsumed &&
                !focusMigration->shellFocusParking &&
                ::GetFocus() == static_cast<HWND>(transferHwnd) &&
                focusMigration->transportNativeAuthority ==
                    transferHwnd &&
                focusMigration->
                    transportNativeAuthorityGeneration != 0 &&
                wxWinUIMSWGetNativeHwndGeneration(transferHwnd) ==
                    focusMigration->
                        transportNativeAuthorityGeneration &&
                oldState->GetHost() == this &&
                oldState->GetWindow() == window &&
                FindSlot(window) == oldSlot &&
                !oldSlot->m_deletePending &&
                oldSlot->m_content == content &&
                oldSlot->m_contentGeneration == contentGeneration &&
                oldSlot->m_disconnectGate == physicalDisconnectGate &&
                oldSlot->m_uiaNameStateRevision ==
                    automationNameStateRevision &&
                oldSlot->m_automationNameStyleRevision ==
                    automationNameStyleRevision &&
                oldSlot->m_clipRevision == clipRevision &&
                window->GetHWND() == transferHwnd &&
                wxWinUIMSWGetHwndGeneration(
                    window, transferHwnd) ==
                    transferHwndGeneration &&
                transferTargetState->GetHost() == target &&
                ForWindow(window, false) == target;
            if ( !parkingSucceeded )
            {
                focusMigration->shellFocusParking = false;
                const bool sourceStillFocused =
                    HasNativeFocusAuthority() &&
                    wxWinUIFocusIsInside(
                        oldSlot->m_container, GetXamlRoot());
                AbortFocusMigration(
                    focusMigration, !sourceStillFocused);
                return FindSlotOwner(window) == target;
            }
        }

        // Cancel only the source host's old retry generation. The ticket now
        // owns its semantics, and the destination will allocate a fresh,
        // bounded intent after publication. In a B->C hop this also retires
        // B's MigrationContinuation request without aborting A's ticket.
        if ( ++m_focusIntentGeneration == 0 )
            ++m_focusIntentGeneration;
        ClearDeferredFocusRequest();
        if ( createdFocusMigration ||
             (consumingPreparedReparent &&
              focusMigration->sourceHost == m_hostLifetime) )
        {
            focusMigration->sourceFocusIntentGeneration =
                m_focusIntentGeneration;
        }
    }

    const wxString lastDesiredAutomationName =
        oldSlot->m_lastUIAName;
    unsigned long long detachedSourceGeneration = 0;

    // Clip is desired slot state, independent of the XAML content generation.
    // SetContent(nullptr) is itself an application callback boundary, so the
    // initial migration snapshot above is not necessarily the last writer.
    // Refresh it only while this exact detached source generation remains the
    // authoritative source; once a destination slot has a local revision, its
    // later writer wins instead.
    const auto refreshDetachedClipSnapshot = [&]()
    {
        if ( !detachedSourceGeneration || oldSlot->m_deletePending ||
             oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             FindSlot(window) != oldSlot || oldSlot->m_content ||
             oldSlot->m_semanticTarget ||
             oldSlot->m_contentGeneration != detachedSourceGeneration )
        {
            return false;
        }

        clipRectDIP = oldSlot->m_clipRectDIP;
        hasClipRect = oldSlot->m_hasClipRect;
        clipRevision = oldSlot->m_clipRevision;
        return true;
    };

    const auto restoreContentState = [&](wxWinUISlot& destination)
    {
        destination.m_preferredFocus = preferredFocus;
    };
    const auto restoreOldContent = [&]()
    {
        if ( oldSlot->m_deletePending ||
             oldState->GetHost() != this ||
             FindSlot(window) != oldSlot ||
             oldSlot->m_content ||
             oldSlot->m_semanticTarget ||
             !detachedSourceGeneration ||
             oldSlot->m_contentGeneration !=
                 detachedSourceGeneration )
        {
            return false;
        }

        // Application code can edit the detached visual tree while a
        // destination registration is in flight. Preserve the visual root
        // and fall back to it as the semantic target instead of retrying a
        // pair which is no longer structurally true.
        const auto restoredSemanticTarget =
            wxWinUISemanticPairIsValid(content, semanticTarget)
                ? semanticTarget
                : content;
        if ( !oldSlot->SetContent(content, restoredSemanticTarget) )
        {
            if ( !oldSlot->m_deletePending &&
                 oldState->GetHost() == this &&
                 FindSlot(window) == oldSlot )
            {
                wxLogWarning("wxWinUI: failed to restore a slot after a "
                             "cross-TLW migration failure");
            }
            return false;
        }

        if ( oldSlot->m_deletePending ||
             oldState->GetHost() != this ||
             FindSlot(window) != oldSlot )
        {
            return false;
        }

        if ( preserveAutomationNameRelinquishment &&
             restoredSemanticTarget == semanticTarget )
        {
            oldSlot->SeedAutomationNameRelinquishment(
                lastDesiredAutomationName);
        }
        restoreContentState(*oldSlot);
        MarkDirty(window);
        return true;
    };
    const auto resumeMigratedFocus =
        [&](wxWinUITopLevelHost *owner,
            wxWinUISlot *destination) -> bool
        {
            const auto consumePreparedReparentLease = [&]()
            {
                focusMigration->preparedNativeReparentBoundaryActive = false;
                focusMigration->preparedReparentId = 0;
            };

            if ( !focusMigration || focusMigration->committed )
                return true;
            if ( !owner || !destination ||
                 destination->m_deletePending ||
                 owner->FindSlot(window) != destination ||
                 !destination->m_lifetime ||
                 destination->m_lifetime->GetHost() != owner ||
                 destination->m_lifetime->GetWindow() != window )
            {
                AbortFocusMigration(
                    focusMigration,
                    focusMigration->logicalFocusWasActive);
                return false;
            }

            if ( focusMigration->logicalFocusWasActive )
            {
                wxWinUITopLevelHost * const source =
                    focusMigration->sourceHost
                        ? focusMigration->sourceHost->GetHost()
                        : nullptr;
                const auto pending =
                    ms_focusMigrations.find(window);
                const bool exactTransportAuthority =
                    source &&
                    source->m_focusMigration == focusMigration &&
                    source->m_focusOwner == window &&
                    source->m_focusIntentGeneration ==
                        focusMigration->sourceFocusIntentGeneration &&
                    pending != ms_focusMigrations.end() &&
                    pending->second == focusMigration &&
                    focusMigration->shellFocusConsumed &&
                    !focusMigration->shellFocusParking &&
                    window->GetHWND() == focusMigration->hwnd &&
                    wxWinUIMSWGetHwndGeneration(
                        window, focusMigration->hwnd) ==
                        focusMigration->hwndGeneration &&
                    HasFocusMigrationNativeAuthority(
                        focusMigration, owner);
                if ( !exactTransportAuthority )
                {
                    AbortFocusMigration(focusMigration, true);
                    return false;
                }

                // SetParent can bounce focus through the source TLW and its
                // InputSite before the destination island exists. Carry the
                // exact current native generation into the continuation;
                // retaining an earlier intermediate TLW HWND would make the
                // next authority check reject this valid transaction.
                const HWND currentAuthority = ::GetFocus();
                const unsigned long long currentAuthorityGeneration =
                    wxWinUIMSWGetNativeHwndGeneration(
                        reinterpret_cast<WXHWND>(currentAuthority));
                if ( !currentAuthority ||
                     !currentAuthorityGeneration )
                {
                    AbortFocusMigration(focusMigration, true);
                    return false;
                }
                focusMigration->transportNativeAuthority =
                    reinterpret_cast<WXHWND>(currentAuthority);
                focusMigration->transportNativeAuthorityGeneration =
                    currentAuthorityGeneration;
            }

            // A reentrant A->B->C migration may already have handed this
            // exact ticket to C's bounded Loaded continuation before the
            // outer A->B RegisterSlotInternal() unwinds. Re-entering
            // FocusSlotImpl() would increment C's intent and
            // ClearDeferredFocusRequest() would correctly abort the older
            // request -- but it is the same winning migration, not stale.
            const FocusRequest& existingDeferred =
                owner->m_deferredFocusRequest;
            const bool alreadyHandedToDestination =
                owner->m_focusMigration == focusMigration &&
                focusMigration->destinationHost ==
                    owner->m_hostLifetime &&
                focusMigration->destinationSlot ==
                    destination->m_lifetime &&
                focusMigration->destinationContentGeneration ==
                    destination->m_contentGeneration &&
                existingDeferred.origin ==
                    FocusOrigin::MigrationContinuation &&
                existingDeferred.target ==
                    destination->m_lifetime &&
                existingDeferred.intentGeneration ==
                    owner->m_focusIntentGeneration &&
                owner->m_deferredFocusContentGeneration ==
                    destination->m_contentGeneration;
            if ( wxWinUIInputLogEnabled() )
            {
                wxWinUIInputLog(
                    "FocusMigration resume preflight owner=%p "
                    "ticket=%d destination=%d deferred=%d "
                    "intent=%llu/%llu content=%llu/%llu",
                    owner,
                    owner->m_focusMigration == focusMigration,
                    focusMigration->destinationSlot ==
                        destination->m_lifetime,
                    existingDeferred.origin ==
                            FocusOrigin::MigrationContinuation &&
                        existingDeferred.target ==
                            destination->m_lifetime,
                    existingDeferred.intentGeneration,
                    owner->m_focusIntentGeneration,
                    owner->m_deferredFocusContentGeneration,
                    destination->m_contentGeneration);
            }
            if ( alreadyHandedToDestination )
            {
                consumePreparedReparentLease();
                return true;
            }

            focusMigration->destinationHost =
                owner->m_hostLifetime;
            focusMigration->destinationSlot =
                destination->m_lifetime;
            focusMigration->destinationContentGeneration =
                destination->m_contentGeneration;
            owner->m_focusMigration = focusMigration;

            wxWindow * const previous =
                focusMigration->previous.get();
            if ( !focusMigration->logicalFocusWasActive )
            {
                // No wx focus owner existed yet: the ticket only carried an
                // unloaded/deferred request. Once the final slot is
                // published, hand that request to the destination's normal
                // bounded arbiter and retire the transport identity.
                const HWND expectedAuthority =
                    static_cast<HWND>(
                        focusMigration->deferredNativeAuthority);
                const bool authorityIsCurrent =
                    ::GetFocus() == expectedAuthority &&
                    wxWinUIMSWGetNativeHwndGeneration(
                        reinterpret_cast<WXHWND>(
                            expectedAuthority)) ==
                        focusMigration->
                            deferredNativeAuthorityGeneration;
                if ( focusMigration->deferredFocusWasPending &&
                     !authorityIsCurrent )
                {
                    AbortFocusMigration(focusMigration, false);
                    return false;
                }

                const FocusOrigin origin =
                    focusMigration->deferredFocusWasPending
                        ? focusMigration->deferredOrigin
                        : FocusOrigin::LogicalRequest;
                AbortFocusMigration(focusMigration, false);
                owner->FocusSlotImpl(
                    window, origin, previous, 0, true);
                return true;
            }

            unsigned long long continuationGeneration = 0;
            if ( focusMigration->sourceHost ==
                 owner->m_hostLifetime )
            {
                // Rollback resumes in the original arbiter. Reserve its new
                // intent explicitly so the same-host commit validates the
                // post-capture generation instead of comparing against the
                // generation retired before detach.
                if ( ++owner->m_focusIntentGeneration == 0 )
                    ++owner->m_focusIntentGeneration;
                owner->ClearDeferredFocusRequest();
                continuationGeneration =
                    owner->m_focusIntentGeneration;
                focusMigration->sourceFocusIntentGeneration =
                    continuationGeneration;
            }

            owner->FocusSlotImpl(
                window,
                FocusOrigin::MigrationContinuation,
                previous,
                continuationGeneration,
                true);
            if ( focusMigration->committed )
            {
                const bool committed =
                    focusMigration->committedSuccessfully &&
                    owner->m_focusOwner == window &&
                    owner->HasNativeFocusAuthority();
                if ( committed )
                    consumePreparedReparentLease();
                return committed;
            }

            const FocusRequest& deferred =
                owner->m_deferredFocusRequest;
            const bool handedToDeferred =
                owner->m_focusMigration == focusMigration &&
                deferred.origin ==
                    FocusOrigin::MigrationContinuation &&
                deferred.target == destination->m_lifetime &&
                deferred.intentGeneration ==
                    owner->m_focusIntentGeneration &&
                owner->m_deferredFocusContentGeneration ==
                    destination->m_contentGeneration;
            if ( wxWinUIInputLogEnabled() )
            {
                wxWinUIInputLog(
                    "FocusMigration resume result committed=%d/%d "
                    "ticket=%d deferred=%d intent=%llu/%llu "
                    "content=%llu/%llu",
                    focusMigration->committed,
                    focusMigration->committedSuccessfully,
                    owner->m_focusMigration == focusMigration,
                    deferred.origin ==
                            FocusOrigin::MigrationContinuation &&
                        deferred.target == destination->m_lifetime,
                    deferred.intentGeneration,
                    owner->m_focusIntentGeneration,
                    owner->m_deferredFocusContentGeneration,
                    destination->m_contentGeneration);
            }
            if ( handedToDeferred )
            {
                consumePreparedReparentLease();
                return true;
            }

            AbortFocusMigration(focusMigration, true);
            return false;
        };

    // Focus parking and accessibility callbacks can still replace/open the
    // gate after the MSW preflight. Refuse before SetContent(nullptr): the
    // source carrier and XamlRoot remain physically unchanged.
    if ( oldSlot->m_disconnectGate != physicalDisconnectGate ||
         (physicalDisconnectGate &&
          (!physicalDisconnectGate->IsSatisfied() ||
           physicalDisconnectGate->IsDegraded())) )
    {
        if ( focusMigration )
            resumeMigratedFocus(this, oldSlot);
        return false;
    }

    if ( !oldSlot->SetContent(nullptr) )
    {
        if ( focusMigration )
            resumeMigratedFocus(this, oldSlot);
        return false;
    }
    preserveAutomationNameRelinquishment =
        oldSlot->m_lastContentTransactionAutomationNameRelinquished;
    if ( oldSlot->m_deletePending ||
         oldState->GetHost() != this ||
         FindSlot(window) != oldSlot )
    {
        const bool installedByNested =
            FindSlotOwner(window) == target;
        if ( !installedByNested && focusMigration &&
             !focusMigration->committed &&
             focusMigration->destinationHost ==
                 transferTargetState )
        {
            AbortFocusMigration(
                focusMigration,
                focusMigration->logicalFocusWasActive);
        }
        return installedByNested;
    }
    detachedSourceGeneration = oldSlot->m_contentGeneration;
    if ( !refreshDetachedClipSnapshot() )
    {
        wxWinUITopLevelHost * const nestedOwner = FindSlotOwner(window);
        if ( focusMigration && !focusMigration->committed &&
             (!nestedOwner || nestedOwner == this) )
        {
            AbortFocusMigration(
                focusMigration,
                focusMigration->logicalFocusWasActive);
        }
        return nestedOwner && nestedOwner != this;
    }

    if ( transferTargetState->GetHost() != target ||
         ForWindow(window, false) != target )
    {
        if ( restoreOldContent() )
            resumeMigratedFocus(this, oldSlot);
        else if ( focusMigration && !focusMigration->committed )
            AbortFocusMigration(focusMigration, true);
        return false;
    }

    // SetContent(nullptr) is a XAML/application callback boundary. The
    // detached tree remains application-owned and its former semantic
    // descendant can disappear before the destination sees it. Transfer the
    // still-valid pair when possible, otherwise preserve the root with
    // root-as-semantic fallback.
    const auto effectiveSemanticTarget =
        wxWinUISemanticPairIsValid(content, semanticTarget)
            ? semanticTarget
            : content;

    // Semantic-target inspection also crosses WinRT. Capture once more at the
    // last point before destination registration, so a clip authored from that
    // boundary is not replaced by the pre-detach value.
    if ( !refreshDetachedClipSnapshot() )
    {
        wxWinUITopLevelHost * const nestedOwner = FindSlotOwner(window);
        if ( focusMigration && !focusMigration->committed &&
             (!nestedOwner || nestedOwner == this) )
        {
            AbortFocusMigration(
                focusMigration,
                focusMigration->logicalFocusWasActive);
        }
        return nestedOwner && nestedOwner != this;
    }

    wxWinUISlot * const newSlot =
        target->RegisterSlotInternal(
            window,
            content,
            effectiveSemanticTarget,
            this,
            accessibilityFactoryResolvedForTransfer,
            preserveAutomationNameRelinquishment &&
                effectiveSemanticTarget == semanticTarget,
            lastDesiredAutomationName,
            physicalDisconnectGate);

    // RegisterSlotInternal() may have been re-entered from Loaded and moved
    // the just-created B slot on again to C before returning. If that newer
    // operation also replaced the content, its content-filtered return value
    // is null even though C is now the truthful owner. Resolve ownership
    // independently of the return value.
    wxWinUITopLevelHost * const installedOwner = FindSlotOwner(window);
    wxWinUISlot * const installedSlot =
        installedOwner ? installedOwner->FindSlot(window) : nullptr;
    if ( !installedOwner || !installedSlot || installedOwner == this )
    {
        // The requested content was not installed anywhere authoritative.
        // A logically retired provisional destination can still physically
        // own the transferred UIElement until its scheduled disconnect.
        // Complete only that exact disconnect now, after its callback and
        // RegisterSlotInternal() operation have unwound, so the old carrier
        // can truthfully reattach the element.
        const DeferredTransferDisconnectResult disconnectResult =
            target->CompleteDeferredTransferDisconnect(
                window, transferHwnd, content);
        if ( disconnectResult ==
                 DeferredTransferDisconnectResult::StillActive )
        {
            wxLogWarning(
                "wxWinUI: a retired migration destination still has an "
                "active callback; source rollback cannot proceed safely");
            if ( focusMigration && !focusMigration->committed )
                AbortFocusMigration(focusMigration, true);
            return false;
        }

        // The disconnect restores attached properties and can therefore run
        // application callbacks. A newer migration or destruction wins.
        if ( oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             oldSlot->m_deletePending ||
             FindSlot(window) != oldSlot )
        {
            wxWinUITopLevelHost * const nestedOwner =
                FindSlotOwner(window);
            if ( focusMigration &&
                 !focusMigration->committed &&
                 (!nestedOwner ||
                  focusMigration->destinationHost !=
                      nestedOwner->m_hostLifetime) )
            {
                AbortFocusMigration(
                    focusMigration,
                    focusMigration->logicalFocusWasActive);
            }
            return true;
        }

        if ( window->IsBeingDeleted() ||
             window->GetHWND() != transferHwnd ||
             wxFindWinFromHandle(
                 reinterpret_cast<HWND>(transferHwnd)) != window ||
             wxWinUIMSWGetHwndGeneration(
                 window, transferHwnd) != transferHwndGeneration )
        {
            if ( focusMigration && !focusMigration->committed )
                AbortFocusMigration(focusMigration, false);
            return true;
        }

        wxWinUITopLevelHost * const currentTarget =
            ForWindow(window, true);
        if ( !currentTarget )
        {
            if ( focusMigration && !focusMigration->committed )
            {
                AbortFocusMigration(
                    focusMigration,
                    focusMigration->logicalFocusWasActive);
            }
            return false;
        }
        if ( oldState->GetHost() != this ||
             oldState->GetWindow() != window ||
             oldSlot->m_deletePending ||
             FindSlot(window) != oldSlot ||
             window->IsBeingDeleted() ||
             window->GetHWND() != transferHwnd ||
             wxWinUIMSWGetHwndGeneration(
                 window, transferHwnd) != transferHwndGeneration ||
             ForWindow(window, false) != currentTarget )
        {
            wxWinUITopLevelHost * const nestedOwner =
                FindSlotOwner(window);
            if ( focusMigration &&
                 !focusMigration->committed &&
                 (!nestedOwner ||
                  focusMigration->destinationHost !=
                      nestedOwner->m_hostLifetime) )
            {
                AbortFocusMigration(
                    focusMigration,
                    focusMigration->logicalFocusWasActive);
            }
            return true;
        }
        wxWinUITopLevelHost * const ownerAfterRelease =
            FindSlotOwner(window);
        if ( ownerAfterRelease && ownerAfterRelease != this )
            return ownerAfterRelease == currentTarget;

        // A callback can also replace the detached source generation without
        // moving it. That newer source model wins and will be reconciled by
        // its own dirty pass.
        if ( oldSlot->m_content ||
             oldSlot->m_semanticTarget ||
             oldSlot->m_contentGeneration !=
                 detachedSourceGeneration )
        {
            if ( focusMigration && !focusMigration->committed )
                resumeMigratedFocus(this, oldSlot);
            return false;
        }

        // Preserve the old slot when it is still recoverable.
        if ( restoreOldContent() )
        {
            resumeMigratedFocus(this, oldSlot);
        }
        else if ( focusMigration && !focusMigration->committed )
        {
            AbortFocusMigration(
                focusMigration,
                focusMigration->logicalFocusWasActive);
        }
        return false;
    }

    // RegisterSlotInternal() installs this alias before it publishes the
    // destination. A missing/replaced transferred gate would make removing
    // the old carrier detach an open popup without any authoritative owner.
    if ( physicalDisconnectGate &&
         installedSlot->m_disconnectGate != physicalDisconnectGate )
    {
        std::terminate();
    }

    const bool installedExpectedContent =
        newSlot &&
        installedSlot == newSlot &&
        installedSlot->GetContent() == content &&
        installedSlot->GetSemanticTarget() == effectiveSemanticTarget &&
        wxWinUISemanticPairIsValid(
            installedSlot->GetContent(),
            installedSlot->GetSemanticTarget());
    if ( installedExpectedContent )
    {
        restoreContentState(*installedSlot);
    }
    // Otherwise a newer reentrant operation legitimately replaced or cleared
    // the content while the destination was attaching. Its authoritative
    // per-content state wins; restoring the outer generation's caches would
    // corrupt it. The migration itself still succeeded, so the empty old slot
    // must be removed instead of becoming a non-authoritative ghost.

    // The destination can be synchronously modified while its content is
    // attached. Preserve such a newer local writer; otherwise transfer the
    // exact scale-independent clip contract and its generation.
    if ( installedSlot->m_clipRevision == 0 )
    {
        // RegisterSlotInternal() is callback-bearing too. A control-host write
        // made before the destination became discoverable still landed on the
        // detached source; refresh it now. If the destination already owns a
        // revision we never enter this branch, so its later writer wins.
        (void)refreshDetachedClipSnapshot();
        installedSlot->m_clipRectDIP = clipRectDIP;
        installedSlot->m_hasClipRect = hasClipRect;
        installedSlot->m_clipRevision = clipRevision;
    }
    // This desired state is independent of the content generation. Preserve
    // a reentrant override authored on the destination while it was attaching;
    // otherwise transfer the old logical slot's exact set/reset revision.
    if ( installedSlot->m_automationNameOverrideRevision == 0 )
    {
        installedSlot->m_hasAutomationNameOverride =
            hasAutomationNameOverride;
        installedSlot->m_automationNameOverride =
            automationNameOverride;
        installedSlot->m_automationNameOverrideRevision =
            automationNameOverrideRevision;
    }
    installedOwner->MarkDirty(window);

    // Resume only after every destination cache has been restored and the
    // slot is authoritative. An immediate XAML focus commits atomically; an
    // unloaded peer retains exactly one bounded continuation request.
    resumeMigratedFocus(installedOwner, installedSlot);

    // Focus continuation is callback-bearing and can migrate B on again to C.
    // Resolve the final authoritative slot, and retire the old gate alias only
    // after that final destination proves it owns the exact shared identity.
    wxWinUITopLevelHost * const finalOwner = FindSlotOwner(window);
    wxWinUISlot * const finalSlot =
        finalOwner ? finalOwner->FindSlot(window) : nullptr;
    if ( physicalDisconnectGate &&
         (!finalOwner || finalOwner == this || !finalSlot ||
          finalSlot->m_disconnectGate != physicalDisconnectGate) )
    {
        std::terminate();
    }

    if ( !oldSlot->m_deletePending &&
         oldState->GetHost() == this &&
         FindSlot(window) == oldSlot )
    {
        // The destination is now the sole physical-retirement authority. The
        // empty source slot may still need its ordinary callback-stack
        // CallAfter, but it must never retarget the popup gate away from the
        // destination.
        oldSlot->m_disconnectGate.reset();
        UnregisterSlot(window);
    }
    return true;
}

void wxWinUITopLevelHost::HandleReparent(wxWindow *subtreeRoot)
{
    OperationGuard operation(this);

    if ( !subtreeRoot || m_shuttingDown )
        return;

    // Reparenting a native container moves all of its slotted descendants.
    // Snapshot first because a cross-TLW migration mutates m_slots.
    std::vector<std::shared_ptr<wxWinUISlotLifetime>> affected;
    for ( const auto& kv : m_slots )
    {
        wxWindow * const window = kv.first;
        if ( window == subtreeRoot || subtreeRoot->IsDescendant(window) )
            affected.push_back(kv.second->m_lifetime);
    }

    for ( const auto& state : affected )
    {
        if ( !state || state->GetHost() != this )
            continue;
        wxWindow * const window = state->GetWindow();
        if ( !window )
            continue;

        wxWinUISlot * const slot = FindSlot(window);
        if ( !slot || slot->m_lifetime != state )
            continue;

        if ( wxGetTopLevelParent(window) != m_tlw )
        {
            MigrateSlotToCurrentTLW(window);
            continue;
        }

        // Same-TLW reparent: keep the direct window subscriptions and logical
        // focus ownership intact. Only the ancestor chain changed.
        UnbindSlotAncestors(*slot);
        BindSlotAncestors(*slot);
        InvalidateStructure();
        MarkDirty(window);
    }
}

void wxWinUITopLevelHost::SetSlotClipRect(
    wxWindow *window,
    const wxRect& rectDIP)
{
    SetSlotClipRect(window,
                    rectDIP.x,
                    rectDIP.y,
                    rectDIP.width,
                    rectDIP.height);
}

void wxWinUITopLevelHost::SetSlotClipRect(
    wxWindow *window,
    double xDIP,
    double yDIP,
    double widthDIP,
    double heightDIP)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // SetSlotClipRect() is the exact API: normalize negative extents to an
    // intentionally empty clip rather than overloading them as "unbounded".
    // Only the legacy height wrapper below authors its private -1 width
    // sentinel.
    const wxWinUIClipRectDIP clip{
        std::isfinite(xDIP) ? xDIP : 0.0,
        std::isfinite(yDIP) ? yDIP : 0.0,
        std::isfinite(widthDIP) ? wxMax(0.0, widthDIP) : 0.0,
        std::isfinite(heightDIP) ? wxMax(0.0, heightDIP) : 0.0};
    if ( slot->m_hasClipRect && slot->m_clipRectDIP == clip )
        return;

    slot->m_clipRectDIP = clip;
    slot->m_hasClipRect = true;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

void wxWinUITopLevelHost::ClearSlotClip(wxWindow *window)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // Clear is an authored state even when the freshly-created destination
    // slot is already unbounded. During cross-TLW attachment this write must
    // supersede the source slot's older clip instead of being mistaken for
    // untouched revision zero.
    slot->m_clipRectDIP = {};
    slot->m_hasClipRect = false;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

void wxWinUITopLevelHost::SetSlotClipHeight(
    wxWindow *window,
    double heightDIP)
{
    if ( !std::isfinite(heightDIP) || heightDIP <= 0.0 )
    {
        ClearSlotClip(window);
        return;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const wxWinUIClipRectDIP clip{0.0, 0.0, -1.0, heightDIP};
    if ( slot->m_hasClipRect && slot->m_clipRectDIP == clip )
        return;

    slot->m_clipRectDIP = clip;
    slot->m_hasClipRect = true;
    if ( ++slot->m_clipRevision == 0 )
        ++slot->m_clipRevision;
    InvalidateStructure();
    MarkDirty(window);
}

wxWinUIClipRectDIP wxWinUITopLevelHost::PhysicalClipRectToDIP(
    const wxRect& rectPx,
    double scale)
{
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;

    return wxWinUIClipRectDIP{
        rectPx.x / scale,
        rectPx.y / scale,
        wxMax(0, rectPx.width) / scale,
        wxMax(0, rectPx.height) / scale};
}

wxRect wxWinUITopLevelHost::ClipRectDIPToPhysical(
    const wxWinUIClipRectDIP& rectDIP,
    double scale,
    const wxSize& slotSizePx)
{
    if ( !std::isfinite(scale) || scale <= 0.0 ||
         !std::isfinite(rectDIP.x) ||
         !std::isfinite(rectDIP.y) ||
         !std::isfinite(rectDIP.width) ||
         !std::isfinite(rectDIP.height) )
    {
        return wxRect();
    }

    const auto roundToInt = [](double value)
    {
        const double minimum =
            static_cast<double>((std::numeric_limits<int>::min)());
        const double maximum =
            static_cast<double>((std::numeric_limits<int>::max)());
        return static_cast<int>(std::lround(
            wxMax(minimum, wxMin(maximum, value))));
    };
    const int x = roundToInt(rectDIP.x * scale);
    const int y = roundToInt(rectDIP.y * scale);
    const int width = rectDIP.width < 0.0
        ? wxMax(0, slotSizePx.x - x)
        : wxMax(0, roundToInt(rectDIP.width * scale));
    const int height = wxMax(
        0, roundToInt(rectDIP.height * scale));
    return wxRect(x, y, width, height);
}

void wxWinUITopLevelHost::SetSlotAutomationNameOverride(
    wxWindow *window,
    const wxString& name)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const bool hasOverride = !name.empty();
    if ( slot->m_hasAutomationNameOverride == hasOverride &&
         slot->m_automationNameOverride == name )
    {
        return;
    }

    slot->m_hasAutomationNameOverride = hasOverride;
    slot->m_automationNameOverride = hasOverride ? name : wxString();
    if ( ++slot->m_automationNameOverrideRevision == 0 )
        ++slot->m_automationNameOverrideRevision;
    MarkDirty(window);
}

bool wxWinUITopLevelHost::MirrorNativeCursor(
    const wxWinUINativeTarget& targetIdentity,
    LRESULT hitTest,
    unsigned long long expectedEpoch)
{
    const unsigned long long cursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    const unsigned long long selectionGeneration =
        m_cursorSelectionGeneration;
    const std::shared_ptr<wxWinUIHostLifetime> hostState =
        m_hostLifetime;
    if ( !targetIdentity.IsValid() )
        return false;

    const HWND target =
        reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd());
    POINT screen = m_lastPointerScreen;
    if ( screen.x < -50000 || screen.y < -50000 )
        (void)::GetCursorPos(&screen);
    const wxPoint verdictPoint(screen.x, screen.y);
    const auto targetStillOwnsPoint =
        [this, targetIdentity, hitTest, verdictPoint]()
        {
            const POINT point = { verdictPoint.x, verdictPoint.y };
            wxWinUINativeHit refreshed;
            return
                wxWinUIResolveNativeHit(
                    m_tlw, point, m_bridge, m_inner, &refreshed) ==
                    wxWinUIHitResolution::Hit &&
                refreshed.GetTarget().Matches(targetIdentity) &&
                refreshed.GetHitTest() == hitTest;
        };

    const auto captureGeometry =
        [target, verdictPoint](
            wxPoint *clientPoint,
            wxSize *clientSize,
            std::vector<WXHWND> *chain)
        {
            if ( !clientPoint || !clientSize || !chain ||
                 !target || !::IsWindow(target) )
            {
                return false;
            }

            POINT client = { verdictPoint.x, verdictPoint.y };
            if ( !::ScreenToClient(target, &client) )
                return false;
            RECT rect;
            if ( !::GetClientRect(target, &rect) )
                return false;

            chain->clear();
            HWND current = target;
            for ( unsigned depth = 0; current && depth != 128; ++depth )
            {
                chain->push_back(reinterpret_cast<WXHWND>(current));
                current = ::GetParent(current);
            }
            if ( current )
                return false; // malformed/cyclic hierarchy

            *clientPoint = wxPoint(client.x, client.y);
            *clientSize = wxSize(rect.right - rect.left,
                                 rect.bottom - rect.top);
            return true;
        };

    wxPoint verdictClientPoint;
    wxSize verdictClientSize;
    std::vector<WXHWND> verdictChain;
    if ( !captureGeometry(&verdictClientPoint,
                          &verdictClientSize,
                          &verdictChain) )
    {
        return false;
    }

    // Snapshot USER32's verdict immediately after WM_SETCURSOR. The exact
    // hit revalidation below sends WM_NCHITTEST and application code is free
    // to call SetCursor() from that message too; that later side effect must
    // not silently replace the verdict we are validating.
    HCURSOR nativeCursorVerdict = nullptr;
    unsigned long long nativeCursorVerdictPolicyGeneration =
        cursorPolicyGeneration;
    bool nativeCursorVerdictValid = false;

    ++m_cursorResolutionDepth;
    const auto resolutionGuard = wxMakeGuard(
        [this, targetIdentity, hitTest, expectedEpoch,
         cursorPolicyGeneration, selectionGeneration, hostState,
         verdictPoint, verdictClientPoint, verdictClientSize,
         verdictChain, captureGeometry, targetStillOwnsPoint,
         &nativeCursorVerdict, &nativeCursorVerdictPolicyGeneration,
        &nativeCursorVerdictValid]()
        {
            wxASSERT(m_cursorResolutionDepth > 0);
            if ( !m_cursorPolicyReplayPending )
            {
                --m_cursorResolutionDepth;
                return;
            }

            // Keep the resolution boundary active while revalidating the
            // point: wxWinUIResolveNativeHit() sends WM_NCHITTEST and can run
            // application code. Any mutation it triggers is replayed only
            // after this exact transaction leaves the boundary.
            m_cursorPolicyReplayPending = false;
            bool refresh = false;
            const unsigned long long replaySelectionGeneration =
                m_cursorSelectionGeneration;
            const unsigned long long replayPolicyGeneration =
                gs_cursorPolicyGeneration;
            const unsigned long long replayEpoch =
                m_inputTransitionEpoch;

            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown )
            {
                --m_cursorResolutionDepth;
                return;
            }

            if ( replaySelectionGeneration != selectionGeneration )
            {
                // Preserve a genuine nested topology winner. It only needs a
                // new hit-test if its committed policy is already obsolete.
                refresh =
                    m_activeCursorPolicyGeneration != replayPolicyGeneration;
            }
            else if ( (expectedEpoch &&
                       replayEpoch != expectedEpoch) ||
                      !nativeCursorVerdictValid ||
                      replayPolicyGeneration !=
                        nativeCursorVerdictPolicyGeneration ||
                      wxPoint(m_lastPointerScreen.x,
                              m_lastPointerScreen.y) != verdictPoint )
            {
                refresh = true;
            }
            else
            {
                wxPoint currentClientPoint;
                wxSize currentClientSize;
                std::vector<WXHWND> currentChain;
                if ( !captureGeometry(&currentClientPoint,
                                      &currentClientSize,
                                      &currentChain) ||
                     currentClientPoint != verdictClientPoint ||
                     currentClientSize != verdictClientSize ||
                     currentChain != verdictChain ||
                     !targetStillOwnsPoint() ||
                     !hostState || hostState->GetHost() != this ||
                     m_shuttingDown ||
                     m_cursorSelectionGeneration !=
                        replaySelectionGeneration ||
                     gs_cursorPolicyGeneration !=
                        replayPolicyGeneration ||
                     m_inputTransitionEpoch != replayEpoch )
                {
                    refresh = true;
                }
                else
                {
                    // WM_SETCURSOR already ran on this exact current native
                    // target. If it changed wx cursor policy re-entrantly,
                    // that new policy outranks the event's transient
                    // SetCursor() result just as it does for a XAML slot.
                    // Resolve it callback-free instead of sending a duplicate
                    // WM_SETCURSOR. For non-client zones, retain the USER32
                    // verdict unless there is an explicit wx/busy override.
                    winrt::Microsoft::UI::Input::InputCursor cursor{ nullptr };
                    WXHCURSOR sourceHandle =
                        reinterpret_cast<WXHCURSOR>(
                            nativeCursorVerdict);
                    bool usePolicyVerdict =
                        nativeCursorVerdictPolicyGeneration !=
                            cursorPolicyGeneration;
                    if ( usePolicyVerdict && hitTest != HTCLIENT )
                    {
                        // Static/global cursors do not participate in the
                        // native non-client WM_SETCURSOR protocol. Only the
                        // effective busy cursor may override resize/title
                        // cursors (with the same modal-dialog exception as
                        // ResolveEffectivePointerCursor()).
                        bool useBusy = wxIsBusy();
                        if ( useBusy )
                        {
                            wxDialog * const dialog =
                                wxDynamicCast(m_tlw, wxDialog);
                            if ( dialog && dialog->IsModal() )
                                useBusy = false;
                        }
                        usePolicyVerdict = useBusy;
                    }

                    wxCursor policyKeepAlive;
                    if ( usePolicyVerdict )
                    {
                        bool hasWxOverride = false;
                        if ( !ResolveEffectivePointerCursor(
                                 targetIdentity.GetWindow(), &cursor,
                                 &policyKeepAlive, &sourceHandle,
                                 &hasWxOverride) )
                        {
                            refresh = true;
                        }
                    }
                    else if ( !wxWinUICursorFromHCURSOR(
                                  nativeCursorVerdict, cursor) )
                    {
                        refresh = true;
                    }
                    unsigned long long expectedSelectionGeneration =
                        replaySelectionGeneration + 1;
                    if ( !expectedSelectionGeneration )
                        ++expectedSelectionGeneration;

                    if ( refresh ||
                         !targetIdentity.IsValid() ||
                         !SetIslandPointerCursor(cursor, sourceHandle) ||
                         !hostState || hostState->GetHost() != this ||
                         m_shuttingDown ||
                         m_cursorSelectionGeneration !=
                            expectedSelectionGeneration ||
                         gs_cursorPolicyGeneration !=
                            replayPolicyGeneration ||
                         m_inputTransitionEpoch != replayEpoch ||
                         !targetIdentity.IsValid() )
                    {
                        refresh = true;
                    }
                    else
                    {
                        m_cursorTarget = targetIdentity;
                        m_cursorHit = hitTest;
                        m_cursorMirrored = true;
                        m_nativeCursorVerdictPoint = verdictPoint;
                        m_nativeCursorVerdictClientPoint =
                            verdictClientPoint;
                        m_nativeCursorVerdictClientSize =
                            verdictClientSize;
                        m_nativeCursorVerdictChain = verdictChain;
                        m_activeCursorSurface =
                            ActiveCursorSurface::Native;
                        m_activeCursorSlot.reset();
                        m_activeCursorPolicyGeneration =
                            replayPolicyGeneration;
                    }
                }
            }

            --m_cursorResolutionDepth;
            if ( refresh || m_cursorPolicyReplayPending )
            {
                if ( m_cursorResolutionDepth )
                {
                    // A containing resolution owns the unwind. Preserve the
                    // request for it only when this exact, newer descriptor
                    // could not be committed.
                    m_cursorPolicyReplayPending = true;
                }
                else
                {
                    m_cursorPolicyReplayPending = false;
                    if ( hostState && hostState->GetHost() == this &&
                         !m_shuttingDown )
                    {
                        (void)RefreshPointerCursorAtLastPoint(false);
                    }
                }
            }
        });
    wxUnusedVar(resolutionGuard);

    if ( targetIdentity.Matches(m_cursorTarget) &&
         hitTest == m_cursorHit &&
         m_cursorMirrored &&
         m_activeCursorSurface == ActiveCursorSurface::Native &&
         m_inputPointerSourceAuthoritative &&
         m_activeCursorPolicyGeneration == gs_cursorPolicyGeneration &&
         m_nativeCursorVerdictPoint == verdictPoint &&
         m_nativeCursorVerdictClientPoint == verdictClientPoint &&
         m_nativeCursorVerdictClientSize == verdictClientSize &&
         m_nativeCursorVerdictChain == verdictChain )
    {
        return true;
    }

    // Let the target run its WM_SETCURSOR protocol (wx windows call
    // ::SetCursor from it), then mirror whatever cursor it installed.
    {
        const wxWinUIProfileScope profileSetCursor(
            wxWinUIProfilePhase::MirrorSend);
        ::SendMessage(target, WM_SETCURSOR,
                      reinterpret_cast<WPARAM>(target),
                      MAKELPARAM(hitTest, WM_MOUSEMOVE));
    }
    nativeCursorVerdict = ::GetCursor();
    nativeCursorVerdictPolicyGeneration = gs_cursorPolicyGeneration;
    nativeCursorVerdictValid = true;
    if ( m_inputTransitionEpoch != expectedEpoch ||
         gs_cursorPolicyGeneration != cursorPolicyGeneration ||
         m_cursorSelectionGeneration != selectionGeneration ||
         !targetIdentity.IsValid() )
    {
        return false;
    }

    // Nothing to mirror when the target left the cursor untouched: the
    // island already shows exactly this handle.  Re-proving the
    // geometry and building a new InputCursor would answer a question
    // nobody asked -- and this is the common case, because a pointer
    // moving inside one window keeps the same cursor for hundreds of
    // consecutive samples.  Every check above still applies: a policy
    // change, a new epoch or a different target falls through to the
    // full mirror below.
    if ( nativeCursorVerdict &&
         m_cursorMirrored &&
         m_islandPointerCursorApplied &&
         m_inputPointerSourceAuthoritative &&
         m_activeCursorSurface == ActiveCursorSurface::Native &&
         m_activeCursorPolicyGeneration == cursorPolicyGeneration &&
         m_lastIslandPointerHandle ==
             reinterpret_cast<WXHCURSOR>(nativeCursorVerdict) &&
         targetIdentity.Matches(m_cursorTarget) &&
         hitTest == m_cursorHit )
    {
        m_nativeCursorVerdictPoint = verdictPoint;
        m_nativeCursorVerdictClientPoint = verdictClientPoint;
        m_nativeCursorVerdictClientSize = verdictClientSize;
        m_nativeCursorVerdictChain = verdictChain;
        return true;
    }

    wxPoint currentClientPoint;
    wxSize currentClientSize;
    std::vector<WXHWND> currentChain;
    const wxWinUIProfileScope profileCheck(
        wxWinUIProfilePhase::MirrorCheck);
    if ( wxPoint(m_lastPointerScreen.x, m_lastPointerScreen.y) !=
                verdictPoint ||
         !captureGeometry(&currentClientPoint,
                          &currentClientSize,
                          &currentChain) ||
         currentClientPoint != verdictClientPoint ||
         currentClientSize != verdictClientSize ||
         currentChain != verdictChain ||
         !targetStillOwnsPoint() ||
         !hostState || hostState->GetHost() != this ||
         m_shuttingDown ||
         m_inputTransitionEpoch != expectedEpoch ||
         gs_cursorPolicyGeneration != cursorPolicyGeneration ||
         m_cursorSelectionGeneration != selectionGeneration ||
         !targetIdentity.IsValid() )
    {
        return false;
    }

    winrt::Microsoft::UI::Input::InputCursor cursor{ nullptr };
    const HCURSOR nativeCursor = nativeCursorVerdict;
    unsigned long long expectedSelectionGeneration =
        selectionGeneration + 1;
    if ( !expectedSelectionGeneration )
        ++expectedSelectionGeneration;
    const wxWinUIProfileScope profileApply(
        wxWinUIProfilePhase::MirrorApply);
    if ( !wxWinUICursorFromHCURSOR(nativeCursor, cursor) ||
         !SetIslandPointerCursor(
             cursor, reinterpret_cast<WXHCURSOR>(nativeCursor)) ||
         !hostState || hostState->GetHost() != this ||
         m_shuttingDown ||
         m_inputTransitionEpoch != expectedEpoch ||
         gs_cursorPolicyGeneration != cursorPolicyGeneration ||
         m_cursorSelectionGeneration != expectedSelectionGeneration ||
         !targetIdentity.IsValid() )
    {
        return false;
    }

    m_cursorTarget = targetIdentity;
    m_cursorHit = hitTest;
    m_cursorMirrored = true;
    m_nativeCursorVerdictPoint = verdictPoint;
    m_nativeCursorVerdictClientPoint = verdictClientPoint;
    m_nativeCursorVerdictClientSize = verdictClientSize;
    m_nativeCursorVerdictChain = verdictChain;
    m_activeCursorSurface = ActiveCursorSurface::Native;
    m_activeCursorSlot.reset();
    m_activeCursorPolicyGeneration = cursorPolicyGeneration;
    return true;
}

bool wxWinUITopLevelHost::ResolveEffectivePointerCursor(
    wxWindow *window,
    winrt::Microsoft::UI::Input::InputCursor *cursor,
    wxCursor *keepAlive,
    WXHCURSOR *sourceHandle,
    bool *hasWxOverride) const
{
    if ( !cursor )
        return false;

    HCURSOR handle = nullptr;
    wxCursor owner;

    // Match wxWindowMSW::HandleSetCursor(): the process-wide busy cursor
    // dominates every ordinary cursor, except inside an active modal dialog.
    bool useBusy = wxIsBusy();
    if ( useBusy )
    {
        wxDialog * const dialog = wxDynamicCast(m_tlw, wxDialog);
        if ( dialog && dialog->IsModal() )
            useBusy = false;
    }

    if ( useBusy )
    {
        handle = wxGetCurrentBusyCursor();
    }
    else
    {
        for ( wxWindow *candidate = window;
              candidate;
              candidate = candidate->GetParent() )
        {
            const wxCursor& explicitCursor = candidate->GetCursor();
            if ( explicitCursor.IsOk() )
            {
                owner = explicitCursor;
                handle = GetHcursorOf(owner);
                break;
            }

            if ( candidate == m_tlw )
                break;
        }

        if ( !handle )
        {
            const wxCursor * const global = wxGetGlobalCursor();
            if ( global && global->IsOk() )
            {
                owner = *global;
                handle = GetHcursorOf(owner);
            }
        }
    }

    if ( keepAlive )
        *keepAlive = owner;
    if ( sourceHandle )
        *sourceHandle = reinterpret_cast<WXHCURSOR>(handle);
    if ( hasWxOverride )
        *hasWxOverride = handle != nullptr;
    return wxWinUICursorFromHCURSOR(handle, *cursor);
}

bool wxWinUITopLevelHost::SetIslandPointerCursor(
    const winrt::Microsoft::UI::Input::InputCursor& cursor,
    WXHCURSOR sourceHandle)
{
    OperationGuard operation(this);
    if ( m_shuttingDown )
        return false;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const unsigned long long selectionGeneration =
        m_cursorSelectionGeneration;
    winrt::Microsoft::UI::Input::InputPointerSource attemptedSource{ nullptr };
    bool oldSourceOverrideNeedsNeutralizing = false;

    try
    {
        if ( !m_inputPointerSource )
        {
            const auto xamlRoot = GetXamlRoot();
            if ( xamlRoot )
            {
                const auto island = xamlRoot.ContentIsland();
                if ( island )
                {
                    m_inputPointerSource =
                        winrt::Microsoft::UI::Input::InputPointerSource::
                            GetForIsland(island);
                }
            }
        }

        if ( m_inputPointerSource )
        {
            const auto source = m_inputPointerSource;
            attemptedSource = source;
            oldSourceOverrideNeedsNeutralizing =
                m_inputPointerSourceAuthoritative;
            if ( m_rootPointerCursorAuthoritative )
            {
                const auto root = gs_rootCanvases.find(this);
                if ( root == gs_rootCanvases.end() ||
                     !root->second->SetPointerCursor(nullptr) )
                {
                    return false;
                }
                if ( !hostState || hostState->GetHost() != this ||
                     m_shuttingDown ||
                     m_cursorSelectionGeneration != selectionGeneration )
                {
                    return false;
                }
                m_rootPointerCursorAuthoritative = false;
            }
            if ( gs_failInputPointerSourceSetAt &&
                 --gs_failInputPointerSourceSetAt == 0 )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"wxWinUI test: injected InputPointerSource failure");
            }
            source.Cursor(cursor);
            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown ||
                 m_cursorSelectionGeneration != selectionGeneration )
            {
                return false;
            }
            m_inputPointerSource = source;
            m_lastIslandPointerCursor = cursor;
            m_lastIslandPointerHandle = sourceHandle;
            m_islandPointerCursorApplied = true;
            m_inputPointerSourceAuthoritative = true;
            m_rootPointerCursorAuthoritative = false;
            if ( ++m_cursorSelectionGeneration == 0 )
                ++m_cursorSelectionGeneration;
            return true;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("InputPointerSource::Cursor", e);

        if ( m_cursorSelectionGeneration != selectionGeneration )
            return false;

        // InputPointerSource has precedence over FrameworkElement's protected
        // cursor. If changing the source failed while an older source-level
        // override was still active, a root fallback would be visually inert.
        // Neutralize that old override first; if even this fails, keep the
        // truthful old authority and let the caller retry instead of claiming
        // that the root won.
        if ( oldSourceOverrideNeedsNeutralizing )
        {
            try
            {
                if ( !attemptedSource )
                    return false;
                attemptedSource.Cursor(nullptr);
                if ( !hostState || hostState->GetHost() != this ||
                     m_shuttingDown ||
                     m_cursorSelectionGeneration != selectionGeneration )
                {
                    return false;
                }
            }
            catch ( const winrt::hresult_error& clearError )
            {
                wxWinUILogException(
                    "InputPointerSource::Cursor fallback neutralize",
                    clearError);
                return false;
            }
        }

        // Keep a valid projection for the next retry. Only its wx-owned
        // override was cleared above.
        m_inputPointerSource = attemptedSource;
        m_inputPointerSourceAuthoritative = false;
        if ( attemptedSource )
            return false;
    }

    // Old/failing input-source projections still get the root protected
    // cursor. Do not commit a cache when even this fallback cannot apply.
    const auto root = gs_rootCanvases.find(this);
    if ( root == gs_rootCanvases.end() ||
         !root->second->SetPointerCursor(cursor) )
    {
        return false;
    }
    if ( !hostState || hostState->GetHost() != this || m_shuttingDown ||
         m_cursorSelectionGeneration != selectionGeneration )
        return false;

    m_lastIslandPointerCursor = cursor;
    m_lastIslandPointerHandle = sourceHandle;
    m_islandPointerCursorApplied = true;
    m_inputPointerSourceAuthoritative = false;
    m_rootPointerCursorAuthoritative = true;
    if ( ++m_cursorSelectionGeneration == 0 )
        ++m_cursorSelectionGeneration;
    return true;
}

bool wxWinUITopLevelHost::RelinquishIslandPointerCursor()
{
    OperationGuard operation(this);
    if ( m_shuttingDown )
        return false;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const unsigned long long selectionGeneration =
        m_cursorSelectionGeneration;

    try
    {
        if ( m_inputPointerSourceAuthoritative && m_inputPointerSource )
        {
            const auto source = m_inputPointerSource;
            source.Cursor(nullptr);
            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown ||
                 m_cursorSelectionGeneration != selectionGeneration )
            {
                return false;
            }
        }

        const auto root = gs_rootCanvases.find(this);
        if ( m_rootPointerCursorAuthoritative &&
             (root == gs_rootCanvases.end() ||
              !root->second->SetPointerCursor(nullptr)) )
        {
            return false;
        }
        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown ||
             m_cursorSelectionGeneration != selectionGeneration )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("InputPointerSource cursor relinquish", e);
        return false;
    }

    m_lastIslandPointerCursor = nullptr;
    m_lastIslandPointerHandle = nullptr;
    m_islandPointerCursorApplied = false;
    m_inputPointerSourceAuthoritative = false;
    m_rootPointerCursorAuthoritative = false;
    if ( ++m_cursorSelectionGeneration == 0 )
        ++m_cursorSelectionGeneration;
    return true;
}

bool wxWinUITopLevelHost::ApplyGenericPointerCursor()
{
    const unsigned long long cursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    winrt::Microsoft::UI::Input::InputCursor cursor{ nullptr };
    WXHCURSOR sourceHandle = nullptr;
    bool hasWxOverride = false;
    if ( !ResolveEffectivePointerCursor(
             m_tlw, &cursor, nullptr, &sourceHandle, &hasWxOverride) ||
         (hasWxOverride
              ? !SetIslandPointerCursor(cursor, sourceHandle)
              : !RelinquishIslandPointerCursor()) )
    {
        return false;
    }
    if ( gs_cursorPolicyGeneration != cursorPolicyGeneration )
        return false;

    m_cursorMirrored = false;
    m_cursorTarget = wxWinUINativeTarget();
    m_cursorHit = 0;
    m_activeCursorSurface = ActiveCursorSurface::GenericXaml;
    m_activeCursorSlot.reset();
    m_activeCursorPolicyGeneration = cursorPolicyGeneration;
    return true;
}

bool wxWinUITopLevelHost::ApplySlotPointerCursor(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    bool emitSetCursorEvent,
    unsigned long long expectedEpoch,
    bool reuseCurrentVerdict)
{
    OperationGuard hostOperation(this);
    if ( !state || state->GetHost() != this )
        return false;

    const unsigned long long selectionGeneration =
        m_cursorSelectionGeneration;
    ++m_cursorResolutionDepth;
    const auto resolutionGuard = wxMakeGuard(
        [this, state, expectedEpoch, selectionGeneration]()
        {
            wxASSERT(m_cursorResolutionDepth > 0);
            if ( --m_cursorResolutionDepth != 0 ||
                 !m_cursorPolicyReplayPending )
            {
                return;
            }

            m_cursorPolicyReplayPending = false;
            if ( m_shuttingDown )
                return;
            if ( m_cursorSelectionGeneration != selectionGeneration )
            {
                if ( m_activeCursorPolicyGeneration !=
                        gs_cursorPolicyGeneration )
                {
                    (void)RefreshPointerCursorAtLastPoint(false);
                }
                return; // preserve the nested topology/pointer winner
            }

            // A cursor handler changed policy while the active-surface fields
            // still described the previous routed point. Re-evaluate the
            // exact in-flight slot without invoking that handler twice. If it
            // disappeared, re-hit-test the newly exposed surface instead.
            wxWindow * const target =
                state ? state->GetWindow() : nullptr;
            wxWinUISlot * const current =
                target ? FindSlot(target) : nullptr;
            if ( expectedEpoch &&
                 m_inputTransitionEpoch != expectedEpoch )
            {
                // A nested pointer route superseded the outer slot. The last
                // point/epoch identifies the newer surface; never replay the
                // stale outer state merely because its allocation survived.
                (void)RefreshPointerCursorAtLastPoint(false);
            }
            else if ( state && state->GetHost() == this && current &&
                 current->m_lifetime == state )
            {
                (void)ApplySlotPointerCursor(
                    state, false, expectedEpoch, true);
            }
            else
            {
                (void)RefreshPointerCursorAtLastPoint(false);
            }
        });
    wxUnusedVar(resolutionGuard);

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return false;
    wxWinUISlot::OperationGuard slotOperation(slot);

    const unsigned long long cursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    const unsigned long long contentGeneration =
        slot->m_contentGeneration;
    const unsigned long long topologyGeneration =
        slot->m_cursorTopologyGeneration;

    winrt::Microsoft::UI::Input::InputCursor cursor{ nullptr };
    wxCursor keepAlive;
    WXHCURSOR sourceHandle = nullptr;
    bool hasWxOverride = false;
    bool resolved = false;
    bool resolutionDecided = false;

    bool useBusy = wxIsBusy();
    if ( useBusy )
    {
        wxDialog * const dialog = wxDynamicCast(m_tlw, wxDialog);
        if ( dialog && dialog->IsModal() )
            useBusy = false;
    }

    POINT screen = m_lastPointerScreen;
    if ( screen.x < -50000 || screen.y < -50000 )
        (void)::GetCursorPos(&screen);
    const wxPoint verdictPoint(screen.x, screen.y);
    const wxPoint verdictClientPoint =
        window->ScreenToClient(verdictPoint);
    const wxSize verdictClientSize = window->GetClientSize();

    const auto operationIdentityIsCurrent =
        [this, &state, window, slot,
         cursorPolicyGeneration,
         contentGeneration, topologyGeneration,
         expectedEpoch]()
        {
            return
                !m_shuttingDown &&
                state->GetHost() == this &&
                state->GetWindow() == window &&
                FindSlot(window) == slot &&
                slot->m_lifetime == state &&
                slot->m_contentGeneration == contentGeneration &&
                slot->m_cursorTopologyGeneration == topologyGeneration &&
                wxGetTopLevelParent(window) == m_tlw &&
                gs_cursorPolicyGeneration == cursorPolicyGeneration &&
                (!expectedEpoch ||
                  m_inputTransitionEpoch == expectedEpoch);
        };
    const auto operationIsCurrent =
        [&]()
        {
            return operationIdentityIsCurrent() &&
                   m_cursorSelectionGeneration == selectionGeneration;
        };

    if ( reuseCurrentVerdict &&
         slot->m_pointerCursorVerdictValid &&
         slot->m_pointerCursorVerdictPoint == verdictPoint &&
         slot->m_pointerCursorVerdictClientPoint == verdictClientPoint &&
         slot->m_pointerCursorVerdictClientSize == verdictClientSize &&
         slot->m_pointerCursorVerdictPolicyGeneration ==
            cursorPolicyGeneration &&
         slot->m_pointerCursorVerdictContentGeneration ==
            contentGeneration &&
         slot->m_pointerCursorVerdictTopologyGeneration ==
            topologyGeneration )
    {
        cursor = slot->m_pointerCursorVerdict;
        keepAlive = slot->m_pointerCursorVerdictKeepAlive;
        sourceHandle = slot->m_pointerCursorVerdictHandle;
        hasWxOverride =
            slot->m_pointerCursorVerdictHasWxOverride;
        resolved = true;
        resolutionDecided = true;
    }
    else if ( emitSetCursorEvent && !useBusy )
    {
        // WM_SETCURSOR is first offered to the leaf wx window. If its event
        // is handled without a cursor, DefWindowProc continues with the
        // parent HWND: that parent's handler must run before its static
        // cursor, and so on to the TLW/global fallback. wxSetCursorEvent is
        // not a propagating command event, so model this walk explicitly.
        for ( wxWindow *candidate = window;
              candidate;
              candidate = candidate->GetParent() )
        {
            const WXHWND candidateHwnd = GetHwndOf(candidate);
            const unsigned long long candidateGeneration =
                wxWinUIMSWGetHwndGeneration(
                    candidate, candidateHwnd);
            if ( !candidateHwnd || !candidateGeneration )
                return false;

            const wxPoint client =
                candidate->ScreenToClient(verdictPoint);
            wxSetCursorEvent event(client.x, client.y);
            event.SetId(candidate->GetId());
            event.SetEventObject(candidate);
            const bool processed =
                candidate->HandleWindowEvent(event);

            // The callback can destroy/reparent any ancestor, replace the
            // slot content, pump a newer pointer event, or mutate cursor
            // policy. Resolve the exact HWND association before touching the
            // candidate again; an address/handle reuse is not continuity.
            if ( !operationIsCurrent() ||
                 wxFindWinFromHandle(
                    reinterpret_cast<HWND>(candidateHwnd)) != candidate ||
                 wxWinUIMSWGetHwndGeneration(
                    candidate, candidateHwnd) != candidateGeneration )
            {
                return false;
            }

            if ( processed && event.HasCursor() )
            {
                keepAlive = event.GetCursor();
                sourceHandle = keepAlive.GetHCURSOR();
                hasWxOverride = sourceHandle != nullptr;
                resolved = wxWinUICursorFromHCURSOR(
                    reinterpret_cast<HCURSOR>(sourceHandle), cursor);
                resolutionDecided = true;
                break;
            }

            if ( !processed )
            {
                const wxCursor& staticCursor =
                    candidate->GetCursor();
                if ( staticCursor.IsOk() )
                {
                    keepAlive = staticCursor;
                    sourceHandle = keepAlive.GetHCURSOR();
                    hasWxOverride = sourceHandle != nullptr;
                    resolved = wxWinUICursorFromHCURSOR(
                        reinterpret_cast<HCURSOR>(sourceHandle),
                        cursor);
                    resolutionDecided = true;
                    break;
                }
            }

            // A handled event without a cursor suppresses only this
            // candidate's static cursor. The parent gets a fresh event, just
            // as it does in the native DefWindowProc chain.
            if ( candidate == m_tlw )
                break;
        }

        if ( !resolutionDecided )
        {
            const wxCursor * const global = wxGetGlobalCursor();
            if ( global && global->IsOk() )
            {
                keepAlive = *global;
                sourceHandle = keepAlive.GetHCURSOR();
                hasWxOverride = sourceHandle != nullptr;
            }
            resolved = wxWinUICursorFromHCURSOR(
                reinterpret_cast<HCURSOR>(sourceHandle), cursor);
            resolutionDecided = true;
        }
    }

    if ( !resolutionDecided )
    {
        resolved = ResolveEffectivePointerCursor(
            window, &cursor, &keepAlive,
            &sourceHandle, &hasWxOverride);
    }

    if ( !resolved || !operationIsCurrent() )
        return false;

    if ( emitSetCursorEvent && !useBusy &&
         !reuseCurrentVerdict )
    {
        // Commit the callback verdict before touching the WinRT cursor
        // setter. A transient setter failure must be retryable by a capture
        // or topology resync without invoking user code for the same Move a
        // second time. Re-entrant policy/content/input changes above already
        // failed the complete key and cannot publish stale state here.
        slot->m_pointerCursorVerdictValid = true;
        slot->m_pointerCursorVerdictPoint = verdictPoint;
        slot->m_pointerCursorVerdictClientPoint = verdictClientPoint;
        slot->m_pointerCursorVerdictClientSize = verdictClientSize;
        slot->m_pointerCursorVerdictPolicyGeneration =
            cursorPolicyGeneration;
        slot->m_pointerCursorVerdictContentGeneration =
            contentGeneration;
        slot->m_pointerCursorVerdictTopologyGeneration =
            topologyGeneration;
        slot->m_pointerCursorVerdictHasWxOverride =
            hasWxOverride;
        slot->m_pointerCursorVerdict = cursor;
        slot->m_pointerCursorVerdictHandle = sourceHandle;
        slot->m_pointerCursorVerdictKeepAlive = keepAlive;
    }

    unsigned long long expectedSelectionGeneration =
        selectionGeneration + 1;
    if ( !expectedSelectionGeneration )
        ++expectedSelectionGeneration;

    if ( (hasWxOverride
              ? !SetIslandPointerCursor(cursor, sourceHandle)
              : !RelinquishIslandPointerCursor()) ||
         !operationIdentityIsCurrent() ||
         m_cursorSelectionGeneration != expectedSelectionGeneration )
    {
        return false;
    }

    m_cursorMirrored = false;
    m_cursorTarget = wxWinUINativeTarget();
    m_cursorHit = 0;
    m_activeCursorSurface = ActiveCursorSurface::Slot;
    m_activeCursorSlot = state;
    m_activeCursorPolicyGeneration = cursorPolicyGeneration;
    return true;
}

void wxWinUITopLevelHost::ObserveXamlPointerCursor()
{
    if ( !m_inputPointerSource )
        return;

    try
    {
        const auto observed = m_inputPointerSource.Cursor();
        if ( m_inputPointerSourceAuthoritative &&
             observed != m_lastIslandPointerCursor )
        {
            // XAML selected the cursor for the new OriginalSource before
            // raising the routed event. Yield ownership instead of clearing
            // its TextBox I-beam/template-part cursor below.
            m_inputPointerSourceAuthoritative = false;
            m_islandPointerCursorApplied = false;
            if ( ++m_cursorSelectionGeneration == 0 )
                ++m_cursorSelectionGeneration;
        }

        if ( !m_inputPointerSourceAuthoritative )
        {
            m_lastIslandPointerCursor = observed;
            m_lastIslandPointerHandle = nullptr;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // The eventual setter/retry path remains fail-safe.
    }
}

void wxWinUITopLevelHost::ApplyXamlPointerCursor(
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    bool emitSetCursorEvent,
    unsigned long long expectedEpoch)
{
    const unsigned long long cursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    const unsigned long long cursorSelectionGeneration =
        m_cursorSelectionGeneration;
    try
    {
        using winrt::Microsoft::UI::Xaml::DependencyObject;
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        DependencyObject node =
            args.OriginalSource().try_as<DependencyObject>();
        while ( node )
        {
            for ( const auto& item : m_slots )
            {
                wxWinUISlot * const slot = item.second;
                if ( slot && slot->m_container &&
                     node == slot->m_container )
                {
                    if ( ApplySlotPointerCursor(
                             slot->m_lifetime, emitSetCursorEvent,
                             expectedEpoch,
                             !emitSetCursorEvent) )
                        return;
                    if ( m_cursorSelectionGeneration !=
                            cursorSelectionGeneration )
                    {
                        // Slot teardown or another callback selected the
                        // newly exposed surface while this route was
                        // unwinding. Preserve that nested winner.
                        return;
                    }
                    if ( !emitSetCursorEvent )
                    {
                        // A terminal/capture resync never replaces a valid
                        // Move verdict with generic policy just because its
                        // WinRT re-assertion transiently failed.
                        return;
                    }
                    if ( gs_cursorPolicyGeneration !=
                            cursorPolicyGeneration ||
                         (expectedEpoch &&
                          m_inputTransitionEpoch != expectedEpoch) )
                    {
                        return;
                    }
                    ApplyGenericPointerCursor();
                    return;
                }
            }

            node = VisualTreeHelper::GetParent(node);
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // A re-template can detach the old OriginalSource while the routed
        // event is unwinding. Generic XAML policy is the safe fallback.
    }

    if ( emitSetCursorEvent &&
         m_cursorSelectionGeneration == cursorSelectionGeneration &&
         gs_cursorPolicyGeneration == cursorPolicyGeneration &&
         (!expectedEpoch || m_inputTransitionEpoch == expectedEpoch) )
    {
        ApplyGenericPointerCursor();
    }
}

bool wxWinUITopLevelHost::RefreshPointerCursorAtLastPoint(
    bool emitSetCursorEvent)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw || !m_root )
        return false;

    const RECT client = GetClientScreenRect();
    const POINT screen = m_lastPointerScreen;
    if ( screen.x < client.left || screen.x >= client.right ||
         screen.y < client.top || screen.y >= client.bottom )
    {
        return ApplyGenericPointerCursor();
    }

    const unsigned long long epoch = m_inputTransitionEpoch;
    const unsigned long long selectionGeneration =
        m_cursorSelectionGeneration;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    try
    {
        using winrt::Microsoft::UI::Xaml::DependencyObject;
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const auto xamlRoot = GetXamlRoot();
        if ( !xamlRoot )
            return false;

        const double scale = GetScale();
        const winrt::Windows::Foundation::Point point
        {
            float((screen.x - client.left) / scale),
            float((screen.y - client.top) / scale)
        };

        // PopupRoot is a separate visual tree: xamlRoot.Content() cannot see
        // an open ComboBox/menu/flyout. Hit-test each popup child explicitly
        // in the same app-window coordinate space. An unrelated non-modal
        // popup elsewhere must not make the whole island generic.
        const auto popups =
            VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot);
        for ( const auto& popup : popups )
        {
            const auto child = popup.Child();
            if ( !child )
                continue;

            const auto popupHits =
                VisualTreeHelper::FindElementsInHostCoordinates(
                    point, child);
            for ( const auto& popupHit : popupHits )
            {
                wxUnusedVar(popupHit);
                return ApplyGenericPointerCursor();
            }
        }

        auto scope = xamlRoot.Content();
        if ( !scope )
            scope = m_root;

        // The public wx API includes disabled children, so request the full
        // visual hit stack instead of XAML's input-only stack.  Ordering is
        // still front-to-back and resolveSlot() rejects hidden/stale slots.
        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(point, scope);
        for ( const auto& element : hits )
        {
            if ( element == m_root )
                continue;

            DependencyObject node = element;
            while ( node )
            {
                for ( const auto& item : m_slots )
                {
                    wxWinUISlot * const slot = item.second;
                    if ( slot && slot->m_container &&
                         node == slot->m_container )
                    {
                        const bool applied = ApplySlotPointerCursor(
                            slot->m_lifetime, emitSetCursorEvent, epoch, true);
                        return applied ||
                               m_cursorSelectionGeneration !=
                                   selectionGeneration;
                    }
                }
                node = VisualTreeHelper::GetParent(node);
            }

            // A real XAML surface (menu, popup, dialog smoke layer...) owns
            // the point even when it isn't a wx slot.
            return ApplyGenericPointerCursor();
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch ||
         m_cursorSelectionGeneration != selectionGeneration )
    {
        return false;
    }

    wxWinUINativeHit hit;
    const wxWinUIHitResolution resolution =
        wxWinUIResolveNativeHit(m_tlw, screen, m_bridge, m_inner, &hit);
    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch ||
         m_cursorSelectionGeneration != selectionGeneration )
    {
        // A nested callback has already selected the newer cursor.
        return true;
    }
    if ( resolution == wxWinUIHitResolution::Hit )
    {
        if ( MirrorNativeCursor(
                 hit.GetTarget(), hit.GetHitTest(), epoch) )
        {
            return true;
        }

        // MirrorNativeCursor can itself re-enter the root route. Preserve the
        // nested winner instead of overwriting it with a generic fallback.
        return m_inputTransitionEpoch != epoch ||
               m_cursorSelectionGeneration != selectionGeneration;
    }

    if ( m_cursorSelectionGeneration != selectionGeneration )
        return true;
    return ApplyGenericPointerCursor();
}

void wxWinUITopLevelHost::RefreshActivePointerCursor(
    bool emitSetCursorEvent)
{
    OperationGuard operation(this);
    if ( m_shuttingDown )
        return;

    if ( m_refreshingPointerCursor )
    {
        m_pointerCursorRefreshPending = true;
        m_pointerCursorRefreshNeedsSetCursorEvent |= emitSetCursorEvent;
        return;
    }

    m_refreshingPointerCursor = true;
    const auto finish = wxMakeGuard(
        [this]()
        {
            m_refreshingPointerCursor = false;
        });

    bool emitForPass = emitSetCursorEvent;

    // One replay is enough for a re-entrant SetCursor() mutation: it leaves
    // the latest policy generation pending and the second pass observes it.
    for ( unsigned pass = 0; pass != 2; ++pass )
    {
        m_pointerCursorRefreshPending = false;
        m_pointerCursorRefreshNeedsSetCursorEvent = false;
        const unsigned long long epochBefore = m_inputTransitionEpoch;
        const unsigned long long policyBefore =
            gs_cursorPolicyGeneration;
        const unsigned long long selectionBefore =
            m_cursorSelectionGeneration;

        bool applied = false;
        if ( m_activeCursorSurface == ActiveCursorSurface::Slot )
        {
            applied = ApplySlotPointerCursor(
                m_activeCursorSlot.lock(), emitForPass,
                m_inputTransitionEpoch, true);
        }
        else if ( m_activeCursorSurface == ActiveCursorSurface::Native )
        {
            const wxWinUINativeTarget target = m_cursorTarget;
            const LRESULT hit = m_cursorHit;
            m_cursorMirrored = false;
            applied = MirrorNativeCursor(
                target, hit, m_inputTransitionEpoch);
        }
        else
        {
            applied = ApplyGenericPointerCursor();
        }

        if ( !applied )
        {
            if ( m_inputTransitionEpoch != epochBefore )
                break; // a nested pointer transition is authoritative
            if ( m_cursorSelectionGeneration != selectionBefore )
                break; // a nested topology refresh already chose the winner
            if ( m_pointerCursorRefreshPending ||
                 gs_cursorPolicyGeneration != policyBefore )
            {
                emitForPass |=
                    m_pointerCursorRefreshNeedsSetCursorEvent;
                continue; // replay the latest cursor policy
            }
            ApplyGenericPointerCursor();
        }
        if ( !m_pointerCursorRefreshPending )
            break;
        emitForPass |= m_pointerCursorRefreshNeedsSetCursorEvent;
    }
}

void wxWinUITopLevelHost::ResetRootCursor()
{
    ApplyGenericPointerCursor();
}

void wxWinUITopLevelHost::CloseNativeHover()
{
    m_hasUndeliveredNativeHoverEnter = false;
    m_undeliveredNativeHoverEnter = wxWinUIInputAction();

    if ( !m_lastNativeTarget.IsOk() )
        return;

    // Clear our authoritative state before invoking application/native code:
    // a re-entrant move starts a fresh hover instead of closing it twice.
    const wxWinUINativeTarget oldTarget = m_lastNativeTarget;
    const wxWinUINativeArea oldArea = m_lastNativeArea;
    m_lastNativeTarget = wxWinUINativeTarget();
    m_lastNativeArea = wxWinUINativeArea::Client;

    if ( !oldTarget.IsValid() )
        return;

    const HWND hwnd =
        reinterpret_cast<HWND>(oldTarget.GetLeafHwnd());
    ::SendMessage(hwnd,
                  oldArea == wxWinUINativeArea::Client
                      ? WM_MOUSELEAVE
                      : WM_NCMOUSELEAVE,
                  0, 0);
}

bool wxWinUITopLevelHost::AbortUndeliveredNativeHover(
    const wxWinUIInputAction& nextAction)
{
    if ( !m_hasUndeliveredNativeHoverEnter )
        return false;

    const wxWinUIInputAction pending = m_undeliveredNativeHoverEnter;
    if ( pending.target != nextAction.target ||
         pending.area != nextAction.area )
    {
        return false;
    }

    m_hasUndeliveredNativeHoverEnter = false;
    m_undeliveredNativeHoverEnter = wxWinUIInputAction();
    if ( !m_inputState.AbortCommittedHoverEnter(pending) )
        return false;

    if ( m_lastNativeTarget.IsOk() &&
         wxWinUIGetInputTargetKey(m_lastNativeTarget) == pending.target &&
         ((pending.area == wxWinUIInputArea::Client) ==
          (m_lastNativeArea == wxWinUINativeArea::Client)) )
    {
        // No native wndproc observed this hover. Retire only the host-side
        // identity; sending WM_(NC)MOUSELEAVE would invent an unmatched Leave.
        m_lastNativeTarget = wxWinUINativeTarget();
        m_lastNativeArea = wxWinUINativeArea::Client;
    }
    return true;
}

void wxWinUITopLevelHost::MarkNativeHoverDispatchStarted(
    const wxWinUIInputAction& action)
{
    if ( !m_hasUndeliveredNativeHoverEnter ||
         action.kind != wxWinUIInputKind::Move )
        return;

    const wxWinUIInputAction& pending = m_undeliveredNativeHoverEnter;
    if ( pending.target == action.target &&
         pending.area == action.area )
    {
        // Clear before SendMessage: its wndproc can synchronously route newer
        // input, which must observe that this hover is already published.
        m_hasUndeliveredNativeHoverEnter = false;
        m_undeliveredNativeHoverEnter = wxWinUIInputAction();
    }
}

bool wxWinUITopLevelHost::ResolveInputTarget(
    const wxWinUIInputTargetKey& key,
    wxWinUINativeTarget *target) const
{
    if ( !target )
        return false;

    *target = wxWinUINativeTarget();
    if ( !m_tlw || !key.IsOk() ||
         !wxWinUIGetNativeTarget(
             m_tlw, reinterpret_cast<WXHWND>(key.leaf), target) ||
         wxWinUIGetInputTargetKey(*target) != key )
    {
        *target = wxWinUINativeTarget();
        return false;
    }

    return true;
}

bool wxWinUITopLevelHost::SyncNativeCapture(
    const wxWinUIPointerSample& sample,
    bool *hasNativeCapture,
    bool *executedActions,
    bool *interruptedPress,
    bool *superseded)
{
    if ( hasNativeCapture )
        *hasNativeCapture = false;
    if ( executedActions )
        *executedActions = false;
    if ( interruptedPress )
        *interruptedPress = false;
    if ( superseded )
        *superseded = false;

    const auto observeCapture =
        [this](wxWinUIInputTargetKey *owner,
               bool *isNative,
               bool *isForeign)
        {
            *owner = wxWinUIInputTargetKey();
            *isNative = false;
            *isForeign = false;

            const HWND capture = ::GetCapture();
            if ( !capture ||
                 capture == m_bridge ||
                 capture == m_inner ||
                 (m_bridge && ::IsChild(m_bridge, capture)) )
            {
                return;
            }

            wxWinUINativeTarget target;
            if ( wxWinUIGetNativeTarget(
                     m_tlw, reinterpret_cast<WXHWND>(capture), &target) )
            {
                *owner = wxWinUIGetInputTargetKey(target);
                *isNative = true;
            }
            else
            {
                // A capture belonging to another TLW/process is opaque. It
                // invalidates any previous local capture, but the current
                // routed event must fail closed instead of hit-testing below.
                *isForeign = true;
            }
        };

    wxWinUIInputTargetKey owner;
    bool isNative = false;
    bool isForeign = false;
    observeCapture(&owner, &isNative, &isForeign);

    // Cancellation callbacks may themselves release or transfer capture.
    // Converge on the observed USER32 owner, with a hard retry bound.
    for ( unsigned attempt = 0; attempt != 3; ++attempt )
    {
        const wxWinUIInputTransition transition =
            m_inputState.NativeCaptureChanged(owner, sample);
        if ( executedActions && !transition.actions.empty() )
            *executedActions = true;
        if ( interruptedPress &&
             (transition.HasAction(wxWinUIInputActionKind::CancelPress) ||
              transition.HasAction(
                  wxWinUIInputActionKind::ReleaseCapture)) )
        {
            *interruptedPress = true;
        }

        bool transitionSuperseded = false;
        if ( !transition.actions.empty() &&
             !ExecuteInputTransition(
                 transition, nullptr, &transitionSuperseded) )
        {
            if ( superseded )
                *superseded = transitionSuperseded;
            return false;
        }

        wxWinUIInputTargetKey nextOwner;
        bool nextNative = false;
        bool nextForeign = false;
        observeCapture(&nextOwner, &nextNative, &nextForeign);
        if ( nextOwner == owner &&
             nextNative == isNative &&
             nextForeign == isForeign )
        {
            if ( hasNativeCapture )
                *hasNativeCapture = isNative;
            return !isForeign;
        }

        owner = nextOwner;
        isNative = nextNative;
        isForeign = nextForeign;
    }

    // A capture owner that changes three times during its own cancellation
    // callbacks is unstable; stop this source event rather than guessing.
    return false;
}

bool wxWinUITopLevelHost::ExecuteInputTransition(
    const wxWinUIInputTransition& transition,
    const wxWinUINativeHit *currentHit,
    bool *superseded)
{
    if ( superseded )
        *superseded = false;
    if ( transition.actions.empty() )
        return true;

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });
    const auto cleanupRemaining =
        [this, &transition](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                ExecuteEmergencyInputCleanup(transition.actions[i]);
            }
        };

    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( action.action == wxWinUIInputActionKind::BreakStorm )
        {
            if ( transition.stormDecision ==
                    wxWinUIInputStormDecision::BreakCycle )
            {
                wxWinUIInputLog("pointer storm cycle period=%u: breaking",
                                action.stormPeriod);
                CancelIslandPointerState();
            }
        }
        else if ( !ExecuteInputAction(action, currentHit, epoch) )
        {
            cleanupRemaining(i + 1);
            if ( superseded )
            {
                *superseded =
                    !hostState ||
                    hostState->GetHost() != this ||
                    m_shuttingDown ||
                    m_inputTransitionEpoch != epoch;
            }
            return false;
        }

        if ( !hostState || hostState->GetHost() != this || m_shuttingDown )
        {
            cleanupRemaining(i + 1);
            if ( superseded )
                *superseded = true;
            return false;
        }
        if ( m_inputTransitionEpoch != epoch )
        {
            // One of the callbacks above synchronously routed a newer input
            // transition. The remainder of this snapshot is obsolete.
            cleanupRemaining(i + 1);
            if ( superseded )
                *superseded = true;
            return false;
        }
    }

    return true;
}

void wxWinUITopLevelHost::ExecuteEmergencyInputCleanup(
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::HoverLeave )
    {
        // A balancing release is deliberately ordered before hover cleanup.
        // If its callback aborts the outer snapshot, re-prove that the old
        // target is no longer under the sample before retiring it. A nested
        // transition or indeterminate geometry wins; the next real sample
        // will converge without inventing a Leave.
        if ( m_shuttingDown || !m_tlw )
            return;

        const unsigned long long epoch = m_inputTransitionEpoch;
        const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
        const RECT rootScreen = GetClientScreenRect();
        const wxPoint pointClient(action.screenX - rootScreen.left,
                                  action.screenY - rootScreen.top);
        const POINT pointScreen = { action.screenX, action.screenY };
        wxWinUINativeHit currentHit;
        wxWinUIRouteObservation observation;
        bool hasNativeHit = false;
        if ( !ClassifyRootPointer(
                 pointClient, pointScreen, nullptr, false,
                 &currentHit, &observation, &hasNativeHit) ||
             !hostState || hostState->GetHost() != this ||
             m_shuttingDown || m_inputTransitionEpoch != epoch ||
             observation.surface == wxWinUIInputSurface::Indeterminate )
        {
            return;
        }

        const bool stillSameTarget =
            observation.surface == wxWinUIInputSurface::Native &&
            observation.target == action.target &&
            observation.area == action.area;
        if ( !stillSameTarget )
            (void)ExecuteInputAction(action, nullptr, epoch);
        return;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        if ( !m_inputState.CanDeliverCancellation(action) )
            return;

        wxWinUINativeTarget target;
        if ( !m_shuttingDown &&
             ResolveInputTarget(action.target, &target) )
        {
            ::SendMessage(
                reinterpret_cast<HWND>(target.GetLeafHwnd()),
                WM_CANCELMODE, 0, 0);
        }
        m_inputState.AcknowledgeCancellation(action);
        return;
    }

    // ReleaseCapture is always the first action of a cancellation
    // transition. It can therefore never be an undelivered remainder after
    // another callback; releasing a same-HWND capture here could steal one
    // established by the nested gesture.
}

// Windows delivers WM_PAINT only when nothing else is waiting in the queue,
// which is what lets a window that invalidates itself on every mouse movement
// still appear to follow the pointer: the moves coalesce in the queue and the
// paint gets its turn. Routed pointer input is *sent*, not posted, so it never
// takes that turn and the paint can wait indefinitely -- measured at 300 ms of
// waiting for 1.7 ms of painting, i.e. a canvas frozen for as long as the hand
// keeps moving. Give the target its turn explicitly instead, bounded so that
// painting cannot starve the input either.
void wxWinUITopLevelHost::LetPointerTargetPaint(WXHWND targetHandle)
{
    const HWND target = reinterpret_cast<HWND>(targetHandle);
    if ( !target || !::IsWindow(target) || m_shuttingDown )
        return;

    if ( wxWinUIOptimisationDisabled("paint") )
        return;

    // ~120 Hz: fast enough that no display shows the difference, rare enough
    // that a slow paint cannot consume the whole event loop.
    constexpr unsigned long kMinPaintIntervalMs = 8;
    const unsigned long now = ::GetTickCount();
    if ( now - m_lastPointerPaintTick < kMinPaintIntervalMs )
        return;

    RECT update;
    if ( !::GetUpdateRect(target, &update, FALSE) )
        return;

    m_lastPointerPaintTick = now;
    ::RedrawWindow(target, nullptr, nullptr,
                   RDW_UPDATENOW | RDW_ALLCHILDREN);
}

bool wxWinUITopLevelHost::ExecuteInputAction(
    const wxWinUIInputAction& action,
    const wxWinUINativeHit *currentHit,
    unsigned long long expectedEpoch)
{
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto hostIsLive = [this, &hostState]()
    {
        return hostState &&
               hostState->GetHost() == this &&
               !m_shuttingDown;
    };
    const auto makeCaptureSample = [&action]()
    {
        wxWinUIPointerSample sample;
        sample.device = action.device;
        sample.kind = action.kind;
        sample.button = action.button;
        sample.pointerId = action.pointerId;
        sample.screenX = action.screenX;
        sample.screenY = action.screenY;
        sample.timestamp = ::GetTickCount64();
        sample.modifiers = action.modifiers;
        sample.buttonMask = action.buttonMask;
        return sample;
    };
    const auto failUndeliverableRelease =
        [this, &action, &hostIsLive, &makeCaptureSample]()
        {
            if ( AbortUndeliveredNativeHover(action) && hostIsLive() )
                ResetRootCursor();

            wxWinUIInputAction cancellation;
            if ( !m_inputState.AbortUndeliverableRelease(
                     action, &cancellation) )
            {
                return false;
            }

            // Deliver/acknowledge the exact serial first. WM_CANCELMODE is
            // callback-bearing; remember the capture transaction before it
            // runs so a handler can deliberately replace/renew capture
            // without the old cleanup stealing it afterward.
            const HWND captureBeforeCleanup = ::GetCapture();
            const unsigned long long captureGeneration =
                m_nativeCaptureMutationGeneration;
            ExecuteEmergencyInputCleanup(cancellation);
            if ( !hostIsLive() )
                return false;

            const HWND capture = ::GetCapture();
            wxWinUINativeTarget captureTarget;
            if ( capture && capture == captureBeforeCleanup &&
                 m_nativeCaptureMutationGeneration == captureGeneration &&
                 wxWinUIGetNativeTarget(
                     m_tlw, reinterpret_cast<WXHWND>(capture),
                     &captureTarget) &&
                 wxWinUIGetInputTargetKey(captureTarget)
                    .HasSameOwner(cancellation.target) &&
                 !m_inputState.HasActivePressForOwner(
                      cancellation.target) )
            {
                NotifyNativeCaptureMutation();
                (void)::ReleaseCapture();
            }

            if ( hostIsLive() )
                (void)SyncNativeCapture(makeCaptureSample());
            return false;
        };
    wxWinUINativeTarget targetIdentity;

    if ( action.action == wxWinUIInputActionKind::ReleaseCapture )
    {
        wxWinUIPointerSample sample = makeCaptureSample();
        sample.kind = wxWinUIInputKind::Cancel;
        const HWND capture = ::GetCapture();
        wxWinUINativeTarget captureIdentity;
        const bool matches =
            capture &&
            wxWinUIGetNativeTarget(
                  m_tlw, reinterpret_cast<WXHWND>(capture),
                  &captureIdentity) &&
            wxWinUIGetInputTargetKey(captureIdentity) == action.target;
        if ( !matches )
        {
            const bool converged =
                hostIsLive() && SyncNativeCapture(sample);
            return converged &&
                   hostIsLive() &&
                   m_inputTransitionEpoch == expectedEpoch &&
                   ::GetCapture() == nullptr;
        }

        NotifyNativeCaptureMutation();
        if ( !::ReleaseCapture() )
        {
            wxLogWarning("wxWinUI: failed to release native pointer "
                         "capture from %p", static_cast<void *>(capture));
            return false;
        }
        const bool converged =
            hostIsLive() && SyncNativeCapture(sample);
        return converged &&
               m_inputTransitionEpoch == expectedEpoch &&
               ::GetCapture() == nullptr;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress &&
         !m_inputState.CanDeliverCancellation(action) )
    {
        return true;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave &&
         AbortUndeliveredNativeHover(action) )
    {
        if ( !m_shuttingDown )
            ResetRootCursor();
        return !m_shuttingDown;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave &&
         !m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    if ( !ResolveInputTarget(action.target, &targetIdentity) )
    {
        if ( action.action == wxWinUIInputActionKind::CancelPress )
        {
            m_inputState.AcknowledgeCancellation(action);
            return true;
        }
        if ( action.action == wxWinUIInputActionKind::HoverLeave )
        {
            if ( m_lastNativeTarget.IsOk() &&
                 wxWinUIGetInputTargetKey(m_lastNativeTarget) ==
                    action.target &&
                 ((action.area == wxWinUIInputArea::Client) ==
                  (m_lastNativeArea == wxWinUINativeArea::Client)) )
            {
                // CloseNativeHover clears its host-side identity before it
                // checks whether the retired HWND can still receive Leave.
                CloseNativeHover();
            }
            if ( !m_shuttingDown )
                ResetRootCursor();
            return !m_shuttingDown;
        }
        if ( action.action == wxWinUIInputActionKind::Dispatch )
            return failUndeliverableRelease();
        // A destroyed/reparented generation is an expected fail-closed
        // outcome after arbitrary application code ran.
        return false;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        ::SendMessage(
            reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd()),
            WM_CANCELMODE, 0, 0);
        m_inputState.AcknowledgeCancellation(action);
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave )
    {
        if ( targetIdentity.Matches(m_lastNativeTarget) &&
             ((action.area == wxWinUIInputArea::Client) ==
              (m_lastNativeArea == wxWinUINativeArea::Client)) )
        {
            CloseNativeHover();
        }
        else
        {
            ::SendMessage(
                reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd()),
                action.area == wxWinUIInputArea::Client
                    ? WM_MOUSELEAVE
                    : WM_NCMOUSELEAVE,
                0, 0);
        }
        if ( m_inputTransitionEpoch != expectedEpoch )
            return false;
        if ( !m_shuttingDown )
            ResetRootCursor();
        return !m_shuttingDown;
    }

    if ( action.action == wxWinUIInputActionKind::HoverEnter )
    {
        if ( !m_inputState.CommitHoverAction(action) )
            return false;

        m_lastNativeTarget = targetIdentity;
        m_lastNativeArea =
            action.area == wxWinUIInputArea::Client
                ? wxWinUINativeArea::Client
                : wxWinUINativeArea::NonClient;
        m_undeliveredNativeHoverEnter = action;
        m_hasUndeliveredNativeHoverEnter = true;
        return true;
    }

    if ( action.action != wxWinUIInputActionKind::Dispatch )
        return true;

    wxWinUINativeHit refreshedHit;
    const wxWinUINativeHit *verifiedHit = nullptr;
    if ( currentHit &&
         wxWinUIGetInputTargetKey(currentHit->GetTarget()) ==
            action.target &&
         ((currentHit->GetArea() == wxWinUINativeArea::Client) ==
            (action.area == wxWinUIInputArea::Client)) &&
         currentHit->GetHitTest() == action.zone )
    {
        // Hover-leave and cursor callbacks above are re-entrant application
        // boundaries. Prove the same geometric hit again before delivery.
        if ( action.kind == wxWinUIInputKind::Move )
        {
            const long long mirrorStarted = wxWinUIProfileNow();
            const bool mirrored = MirrorNativeCursor(
                     targetIdentity, action.zone, expectedEpoch);
            wxWinUIProfileAdd(wxWinUIProfilePhase::Mirror,
                              mirrorStarted);
            if ( !mirrored || m_shuttingDown )
                return failUndeliverableRelease();
        }

        const long long refreshStarted = wxWinUIProfileNow();
        const bool refreshed = wxWinUIRefreshNativeHit(
            m_tlw, *currentHit, m_bridge, m_inner, &refreshedHit,
            m_hitLayoutGeneration == m_structureGeneration);
        wxWinUIProfileAdd(wxWinUIProfilePhase::Refresh, refreshStarted);
        if ( m_inputTransitionEpoch != expectedEpoch )
            return failUndeliverableRelease();
        if ( !refreshed )
        {
            // The refresh can fail after cursor mirroring ran application
            // code.  Do not emit Leave here: no matching HoverLeave action
            // has been committed, so doing so would leave m_inputState
            // hovering this target and make the next authoritative sample
            // emit a duplicate Leave.  Keep the native and semantic hover
            // prefixes aligned and let that next sample converge them.
            if ( !m_shuttingDown )
                ResetRootCursor();
            return failUndeliverableRelease();
        }

        verifiedHit = &refreshedHit;
        targetIdentity = refreshedHit.GetTarget();
    }

    if ( !targetIdentity.IsValid() )
        return failUndeliverableRelease();

    const HWND target =
        reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd());
    POINT screen = { action.screenX, action.screenY };
    POINT client = screen;
    ::SetLastError(ERROR_SUCCESS);
    if ( !::MapWindowPoints(HWND_DESKTOP, target, &client, 1) &&
         ::GetLastError() != ERROR_SUCCESS )
    {
        return failUndeliverableRelease();
    }

    const LPARAM lpClient =
        MAKELPARAM(static_cast<short>(client.x),
                   static_cast<short>(client.y));
    const LPARAM lpScreen =
        MAKELPARAM(static_cast<short>(screen.x),
                   static_cast<short>(screen.y));
    const WORD keys = static_cast<WORD>(
        action.modifiers | action.buttonMask);

    if ( action.kind == wxWinUIInputKind::Move )
    {
        MarkNativeHoverDispatchStarted(action);
        if ( action.area == wxWinUIInputArea::Client )
        {
            const wxWinUIProfileScope profileSend(
                wxWinUIProfilePhase::Send);
            const wxWinUISyntheticMouseDispatch syntheticDispatch;
            ::SendMessage(target, WM_MOUSEMOVE, keys, lpClient);
            LetPointerTargetPaint(target);
        }
        else
        {
            ::SendMessage(target, WM_NCMOUSEMOVE,
                          static_cast<WPARAM>(action.zone), lpScreen);
        }
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    if ( action.kind == wxWinUIInputKind::Wheel )
    {
        if ( action.wheelDelta )
        {
            ::SendMessage(
                target,
                action.horizontalWheel ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
                MAKEWPARAM(keys,
                    static_cast<WORD>(
                        static_cast<short>(action.wheelDelta))),
                lpScreen);
        }
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    wxWinUIButtonMessages messages;
    if ( !wxWinUIGetButtonMessages(action.button, action.kind, &messages) )
    {
        return failUndeliverableRelease();
    }

    const bool isDown =
        action.kind == wxWinUIInputKind::Press ||
        action.kind == wxWinUIInputKind::DoubleClick;
    const UINT message =
        action.area == wxWinUIInputArea::Client
            ? messages.client
            : messages.nonClient;
    const WPARAM buttonParams =
        action.area == wxWinUIInputArea::Client
            ? (messages.xButton
                ? MAKEWPARAM(keys, messages.xButton)
                : static_cast<WPARAM>(keys))
            : (messages.xButton
                ? MAKEWPARAM(static_cast<WORD>(action.zone),
                             messages.xButton)
                : static_cast<WPARAM>(action.zone));

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("fwd %s target=%p ht=%d %s",
                        isDown ? "DOWN" : "UP",
                        static_cast<void *>(target),
                        action.zone,
                        action.area == wxWinUIInputArea::Client
                            ? "client" : "nc");
    }

    // DefWindowProc runs these hit zones inside a system-modal loop. Queue
    // their DOWN outside the XAML callback and retire the bridge gesture
    // first; all other messages remain synchronous.
    if ( isDown &&
         action.button == wxWinUIInputButton::Left &&
         action.area == wxWinUIInputArea::NonClient &&
         wxWinUIStartsSystemModalLoop(action.zone) )
    {
        bool mayQueue =
            verifiedHit &&
            verifiedHit->GetTarget().Matches(targetIdentity);
        const HWND capture = ::GetCapture();
        const bool islandOwnsCapture =
            !capture ||
            capture == m_bridge ||
            capture == m_inner ||
            (m_bridge && ::IsChild(m_bridge, capture));

        if ( !islandOwnsCapture )
        {
            mayQueue = false;
            wxWinUIInputLog(
                "modal-ht %d: foreign capture %p; forward suppressed",
                action.zone, static_cast<void *>(capture));
        }
        else if ( capture )
        {
            NotifyNativeCaptureMutation();
            if ( !::ReleaseCapture() )
            {
                mayQueue = false;
                wxLogWarning("wxWinUI: failed to release island pointer "
                             "capture from %p",
                             static_cast<void *>(capture));
            }
        }

        if ( !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return failUndeliverableRelease();
        }

        wxWinUINativeHit afterCapture;
        if ( mayQueue )
        {
            const bool refreshed = wxWinUIRefreshNativeHit(
                m_tlw, *verifiedHit, m_bridge, m_inner, &afterCapture);
            if ( m_inputTransitionEpoch != expectedEpoch )
                return failUndeliverableRelease();
            if ( !refreshed || ::GetCapture() )
                mayQueue = false;
        }

        // The modal DOWN remains prepared while it is queued and is committed
        // only immediately before the eventual SendMessage. Reset the XAML
        // input site first, then revalidate before publishing the deferred
        // request; the dispatcher will either retire the gesture consumed by
        // the system loop or preserve it for a real routed UP.
        const std::uint32_t resetGeneration =
            m_inputSiteGeneration;
        bool resetTransferredToPending = false;
        const auto finishUnqueuedReset = wxMakeGuard(
            [this, &hostIsLive, &resetTransferredToPending,
             resetGeneration]()
            {
                if ( !resetTransferredToPending && hostIsLive() )
                    PostIslandCancelMode(resetGeneration);
            });

        if ( hostIsLive() )
            CancelIslandPointerState(false);

        if ( !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return failUndeliverableRelease();
        }

        if ( mayQueue )
        {
            wxWinUINativeHit finalHit;
            const bool refreshed = wxWinUIRefreshNativeHit(
                m_tlw, afterCapture, m_bridge, m_inner, &finalHit);
            if ( m_inputTransitionEpoch != expectedEpoch ||
                 !refreshed || ::GetCapture() )
            {
                mayQueue = false;
            }
            else
            {
                mayQueue = QueueModalNativeDispatch(
                    finalHit, action, message, messages.virtualKey,
                    messages.xButton, action.device, action.pointerId);
                resetTransferredToPending = mayQueue;
            }
        }

        if ( !mayQueue )
        {
            if ( AbortUndeliveredNativeHover(action) &&
                 !m_shuttingDown )
            {
                ResetRootCursor();
            }
            wxLogWarning("wxWinUI: suppressed unsafe queued non-client "
                         "input");
        }
        return mayQueue && !m_shuttingDown;
    }

    // This is the transaction's commit point. All HWND/generation, geometry,
    // cursor and modal-loop preflights above are complete, and SendMessage is
    // the very next callback-bearing operation.
    if ( !hostIsLive() ||
         m_inputTransitionEpoch != expectedEpoch ||
         !m_inputState.CommitBeforeDispatch(action) )
    {
        return failUndeliverableRelease();
    }
    MarkNativeHoverDispatchStarted(action);

    const ULONGLONG started = ::GetTickCount64();
    if ( action.area == wxWinUIInputArea::Client )
    {
        const wxWinUISyntheticMouseDispatch syntheticDispatch;
        ::SendMessage(target, message, buttonParams, lpClient);
    }
    else
    {
        ::SendMessage(target, message, buttonParams, lpScreen);
    }
    if ( m_shuttingDown )
        return false;
    const wxWinUIPointerSample captureSample = makeCaptureSample();
    const bool captureSynchronized = SyncNativeCapture(captureSample);
    if ( !hostIsLive() )
        return false;
    const bool outerTransitionIsCurrent =
        m_inputTransitionEpoch == expectedEpoch;

    // A synchronous native tracking loop may consume the matching physical
    // release before returning. It then cannot arrive through XAML, so clear
    // the semantic press under the same bounded evidence used by the legacy
    // implementation. Do this before abandoning a re-entered outer
    // transition: MOVE/ENTER/EXIT can advance the epoch without retiring this
    // exact gesture. FinishSynchronousDown() is serial-checked, so a nested UP
    // or replacement DOWN makes this a harmless no-op.
    if ( isDown )
    {
        const ULONGLONG elapsed = ::GetTickCount64() - started;
        const bool contactActive = wxWinUIIsPointerContactActive(
            action.device, action.pointerId, messages.virtualKey);
        const wxWinUIInputTransition cancelTransition =
            m_inputState.FinishSynchronousDown(
                action, captureSample, elapsed, contactActive);
        if ( !cancelTransition.actions.empty() )
        {
            if ( !ExecuteInputTransition(cancelTransition) )
                return false;
            CancelIslandPointerState();
        }
    }

    return captureSynchronized &&
           outerTransitionIsCurrent &&
           !m_shuttingDown;
}

bool wxWinUITopLevelHost::BuildRootPointerSample(
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    wxWinUIInputKind kind,
    wxWinUIPointerSample *sample) const
{
    if ( !sample || !m_root )
        return false;

    *sample = wxWinUIPointerSample();
    sample->kind = kind;
    sample->timestamp = ::GetTickCount64();

    bool havePointerIdentity = false;
    try
    {
        const auto pointer = args.Pointer();
        sample->device =
            wxWinUIGetInputDevice(pointer.PointerDeviceType());
        sample->pointerId = pointer.PointerId();
        havePointerIdentity = true;
    }
    catch ( const winrt::hresult_error& )
    {
    }

    try
    {
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        const auto position = point.Position();
        const double scale = GetScale();
        const RECT clientScreen = GetClientScreenRect();

        sample->device =
            wxWinUIGetInputDevice(point.PointerDeviceType());
        sample->pointerId = point.PointerId();
        sample->isPrimary = props.IsPrimary();
        sample->isCompatibilityMouse =
            sample->device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample->hasRootDips = true;
        sample->rootXDIP = position.X;
        sample->rootYDIP = position.Y;
        sample->screenX =
            clientScreen.left +
            static_cast<int>(std::lround(position.X * scale));
        sample->screenY =
            clientScreen.top +
            static_cast<int>(std::lround(position.Y * scale));
        sample->timestamp = point.Timestamp() / 1000;

        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            sample->modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            sample->modifiers |= MK_CONTROL;
        if ( props.IsLeftButtonPressed() )
            sample->buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            sample->buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            sample->buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            sample->buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            sample->buttonMask |= MK_XBUTTON2;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        if ( !havePointerIdentity ||
             (kind != wxWinUIInputKind::Cancel &&
              kind != wxWinUIInputKind::CaptureLost) )
        {
            return false;
        }

        // PointerCanceled/CaptureLost may no longer have a visual point.
        // Their stable pointer identity is sufficient for scoped cleanup.
        sample->isPrimary = true;
        sample->isCompatibilityMouse = false;
        POINT screen = { 0, 0 };
        if ( ::GetCursorPos(&screen) )
        {
            sample->screenX = screen.x;
            sample->screenY = screen.y;
        }
        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            sample->modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            sample->modifiers |= MK_CONTROL;
        return true;
    }
}

bool wxWinUITopLevelHost::ClassifyRootPointer(
    const wxPoint& pointClientPx,
    const POINT& pointScreen,
    const winrt::Windows::Foundation::Point *exactRootDips,
    bool sourceAlreadyHandled,
    wxWinUINativeHit *nativeHit,
    wxWinUIRouteObservation *observation,
    bool *hasNativeHit)
{
    if ( !nativeHit || !observation || !hasNativeHit ||
         m_shuttingDown || !m_tlw )
    {
        return false;
    }

    *nativeHit = wxWinUINativeHit();
    *observation = wxWinUIRouteObservation();
    *hasNativeHit = false;

    // handledEventsToo lets the root observe events consumed by a
    // Button/TextBox/etc. They remain XAML-owned even if the pointer is over
    // a transparent/native region. Native capture, applied by the state
    // machine after this geometric classification, is the sole override.
    if ( sourceAlreadyHandled )
    {
        observation->surface = wxWinUIInputSurface::Xaml;
        return true;
    }

    const long long xamlStarted = wxWinUIProfileNow();
    const XamlHitResolution xamlHit =
        PointOverXamlContent(pointClientPx, exactRootDips);
    wxWinUIProfileAdd(wxWinUIProfilePhase::XamlHit, xamlStarted);
    if ( m_shuttingDown )
        return false;

    if ( xamlHit == XamlHitResolution::Hit )
    {
        observation->surface = wxWinUIInputSurface::Xaml;
        return true;
    }
    if ( xamlHit == XamlHitResolution::Unavailable )
    {
        observation->surface = wxWinUIInputSurface::Indeterminate;
        return true;
    }

    const long long nativeStarted = wxWinUIProfileNow();
    const wxWinUIHitResolution nativeResolution =
        wxWinUIResolveNativeHitReusing(
            m_tlw, pointScreen, m_bridge, m_inner,
            m_lastResolvedHit,
            m_lastResolvedLayoutGeneration == m_structureGeneration &&
                !wxWinUIOptimisationDisabled("hittest"),
            nativeHit);
    wxWinUIProfileAdd(wxWinUIProfilePhase::NativeHit, nativeStarted);
    m_hitLayoutGeneration = m_structureGeneration;
    if ( nativeResolution == wxWinUIHitResolution::Hit )
    {
        m_lastResolvedHit = *nativeHit;
        m_lastResolvedLayoutGeneration = m_structureGeneration;
    }
    else
    {
        m_lastResolvedHit = wxWinUINativeHit();
    }
    if ( nativeResolution == wxWinUIHitResolution::Unstable )
    {
        observation->surface = wxWinUIInputSurface::Indeterminate;
        return true;
    }
    if ( nativeResolution == wxWinUIHitResolution::Miss )
    {
        observation->surface = wxWinUIInputSurface::Outside;
        return true;
    }

    *hasNativeHit = true;
    observation->surface = wxWinUIInputSurface::Native;
    observation->target =
        wxWinUIGetInputTargetKey(nativeHit->GetTarget());
    observation->area =
        nativeHit->GetArea() == wxWinUINativeArea::Client
            ? wxWinUIInputArea::Client
            : wxWinUIInputArea::NonClient;
    observation->zone =
        static_cast<int>(nativeHit->GetHitTest());
    observation->clientDoubleClicks =
        (::GetClassLongPtr(
            reinterpret_cast<HWND>(
                nativeHit->GetTarget().GetLeafHwnd()),
            GCL_STYLE) & CS_DBLCLKS) != 0;
    return true;
}

void wxWinUITopLevelHost::OnRootPointerBoundary(
    bool entered,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;

    try
    {
        const auto source =
            args.OriginalSource().try_as<
                winrt::Microsoft::UI::Xaml::UIElement>();
        if ( !source || source != m_root )
            return;
    }
    catch ( const winrt::hresult_error& )
    {
        // A descendant boundary is more common than a failed root query.
        // Without positive root ownership, fail closed instead of retiring a
        // native hover because a child XAML element was crossed.
        return;
    }

    wxWinUIPointerSample sample;
    if ( !BuildRootPointerSample(
             args,
             entered ? wxWinUIInputKind::Enter
                     : wxWinUIInputKind::Leave,
             &sample) ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return;
    }

    // Descendant boundaries and unrouteable/duplicate pointer samples are
    // deliberately ignored. Publish a new epoch only after proving this is
    // an authoritative root boundary; otherwise an ignored XAML callback
    // could supersede a valid native transition already in flight.
    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long eventEpoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    (void)RouteRootPointerBoundaryAtEpoch(
        entered, sample, eventEpoch);
}

bool wxWinUITopLevelHost::TestRouteRootPointerBoundary(
    bool entered,
    const wxWinUIPointerSample& originalSample)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return false;

    wxWinUIPointerSample sample = originalSample;
    sample.kind = entered
        ? wxWinUIInputKind::Enter
        : wxWinUIInputKind::Leave;
    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return false;
    }

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    return RouteRootPointerBoundaryAtEpoch(
        entered, sample, m_inputTransitionEpoch);
}

bool wxWinUITopLevelHost::RouteRootPointerBoundaryAtEpoch(
    bool entered,
    const wxWinUIPointerSample& sample,
    unsigned long long eventEpoch)
{
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    if ( m_nativeInputDispatchInFlight &&
         m_nativeInputDispatchCommitted &&
         m_inFlightNativeInput.device == sample.device &&
         m_inFlightNativeInput.pointerId == sample.pointerId )
    {
        // System-modal SendMessage pumps routed boundary events. Preserve the
        // latest boundary state on the published in-flight identity so its
        // return path cannot arm an already-outside release wait.
        m_inFlightNativeInput.pointerInsideRoot = entered;
    }

    if ( !entered )
    {
        CancelPendingNativeDispatch(sample);
        CancelSlotPointerOwners(sample, false);
        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown || m_inputTransitionEpoch != eventEpoch )
        {
            return false;
        }

        std::array<wxWinUIInputAction,
                   ModalAwaitingReleaseCapacity> modalOwners;
        std::size_t modalOwnerCount = 0;
        for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
        {
            if ( current.gestureSerial &&
                 current.device == sample.device &&
                 current.pointerId == sample.pointerId )
            {
                modalOwners[modalOwnerCount++] = current;
                // Retire the broker entry before callback-bearing
                // cancellation. A nested modal DOWN then gets a fresh slot
                // and cannot be cleared by this outer boundary snapshot.
                current = wxWinUIInputAction();
            }
        }

        // No XAML capture remains for modal DOWNs. Once their pointer leaves
        // the island, a physical UP outside is not guaranteed to return
        // through the root router. Retire every exact button serial in the
        // semantic state before invoking the first callback; the combined
        // executor will emergency-acknowledge any remaining CancelPress
        // tombstones if the first WM_CANCELMODE re-enters.
        wxWinUIInputTransition captureCleanup;
        wxWinUIInputTransition pressCancellations;
        for ( std::size_t i = 0; i < modalOwnerCount; ++i )
        {
            const wxWinUIInputAction& modalOwner = modalOwners[i];
            if ( m_inputState.HasActiveGesture(modalOwner) )
            {
                wxWinUIPointerSample cancel = sample;
                cancel.kind = wxWinUIInputKind::Cancel;
                cancel.button = modalOwner.button;
                cancel.buttonMask &=
                    ~wxWinUIGetWParamButtonMask(modalOwner.button);
                const wxWinUIInputTransition cancellation =
                    m_inputState.CancelActiveGesture(modalOwner, cancel);
                for ( const wxWinUIInputAction& action :
                      cancellation.actions )
                {
                    if ( action.action ==
                            wxWinUIInputActionKind::ReleaseCapture )
                    {
                        captureCleanup.actions.push_back(action);
                    }
                    else
                    {
                        pressCancellations.actions.push_back(action);
                    }
                }
            }
        }

        // Preserve the executor's cleanup invariant: ReleaseCapture is always
        // first. If the first callback re-enters, it has already run; every
        // remaining CancelPress can then be completed independently.
        wxWinUIInputTransition allCancellations;
        for ( const wxWinUIInputAction& action : captureCleanup.actions )
            allCancellations.actions.push_back(action);
        for ( const wxWinUIInputAction& action : pressCancellations.actions )
            allCancellations.actions.push_back(action);

        unsigned long long expectedEpoch = eventEpoch;
        if ( !allCancellations.actions.empty() )
        {
            if ( ++expectedEpoch == 0 )
                ++expectedEpoch;
        }
        if ( !ExecuteInputTransition(allCancellations) ||
             !hostState || hostState->GetHost() != this ||
             m_shuttingDown ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return false;
        }
        eventEpoch = m_inputTransitionEpoch;
    }

    const wxWinUIInputTransition transition =
        m_inputState.RouteBoundary(sample);
    if ( !ExecuteInputTransition(transition) )
        return false;
    RetryDeferredIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        // Sample transition storms without making healthy traces unreadable.
        static unsigned s_count = 0;
        ++s_count;
        if ( (s_count % 64) == 1 )
        {
            POINT pt = { 0, 0 };
            ::GetCursorPos(&pt);
            const HWND under = ::WindowFromPoint(pt);
            wchar_t cls[64] = L"";
            if ( under )
                ::GetClassNameW(under, cls, WXSIZEOF(cls));
            wxWinUIInputLog("root %s #%u under=%p '%S' pt=(%ld,%ld)",
                            entered ? "ENTER" : "EXIT",
                            s_count, static_cast<void *>(under),
                            cls, pt.x, pt.y);
        }
    }
    return true;
}

void wxWinUITopLevelHost::OnRootPointerInterrupted(
    bool captureLost,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWinUIPointerSample sample;
    if ( !BuildRootPointerSample(
             args,
             captureLost ? wxWinUIInputKind::CaptureLost
                         : wxWinUIInputKind::Cancel,
             &sample) ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
        return;

    // A pointer-specific interruption can bubble from a different XAML
    // descendant than the slot that received DOWN. Retire host-wide slot
    // ownership before filtering the event out of the independent root
    // native state machine.
    CancelSlotPointerOwners(sample, false);
    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch )
    {
        return;
    }
    RetryDeferredIslandCancelMode();

    bool sourceIsRoot = false;
    bool sourceKnown = false;
    try
    {
        const auto source =
            args.OriginalSource().try_as<
                winrt::Microsoft::UI::Xaml::UIElement>();
        sourceKnown = static_cast<bool>(source);
        sourceIsRoot = source && source == m_root;
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // CaptureLost/Canceled bubble from every hosted XAML slot. Only the
    // island root may terminate root-native state. If source inspection
    // itself failed, require positive ownership of this pointer as fallback.
    if ( sourceKnown && !sourceIsRoot )
    {
        // XAML may rewrite InputPointerSource as its capture is torn down.
        // This is not a new hit-test: preserve the last Move verdict without
        // re-entering wxEVT_SET_CURSOR while retiring the slot gesture.
        ApplyXamlPointerCursor(args, false, epoch);
        return;
    }
    if ( !sourceKnown &&
         !m_inputState.HasActivePress(
             sample.device, sample.pointerId) &&
         !HasPendingNativeDispatch(sample) )
    {
        return;
    }

    CancelPendingNativeDispatch(sample);
    HandleRootPointerInterrupted(captureLost, sample);
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::OnRootNativeInterrupted(bool captureLost)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    wxWinUIPointerSample sample;
    sample.kind = captureLost
        ? wxWinUIInputKind::CaptureLost
        : wxWinUIInputKind::Cancel;
    sample.interruptAll = true;
    sample.timestamp = ::GetTickCount64();
    POINT screen = { 0, 0 };
    if ( ::GetCursorPos(&screen) )
    {
        sample.screenX = screen.x;
        sample.screenY = screen.y;
    }
    CancelPendingNativeDispatch(sample);
    CancelSlotPointerOwners(sample, true);
    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch )
    {
        return;
    }
    HandleRootPointerInterrupted(captureLost, sample);
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::HandleRootPointerInterrupted(
    bool captureLost,
    const wxWinUIPointerSample& sample)
{
    OperationGuard operation(this);
    if ( m_shuttingDown )
        return;

    bool hasNativeCapture = false;
    bool captureInterruptedPress = false;
    if ( !SyncNativeCapture(sample, &hasNativeCapture, nullptr,
                            &captureInterruptedPress) )
        return;

    if ( captureInterruptedPress )
        m_inputState.ArmStormGuard(16, 3, 4);

    // PointerCaptureLost is the normal XAML-side consequence of a native
    // target taking USER32 capture during the forwarded DOWN. The explicit
    // capture observation above is authoritative; do not cancel that healthy
    // transfer or arm the emergency breaker.
    if ( captureLost && hasNativeCapture )
    {
        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("root CAPTURE-TRANSFER");
        return;
    }

    wxWinUIRouteObservation observation;
    observation.surface = wxWinUIInputSurface::Indeterminate;
    const wxWinUIInputTransition transition =
        m_inputState.Route(sample, observation);

    // Only a real unfinished gesture is evidence for the bounded guard.
    // Idle capture notifications leave ordinary input completely untouched.
    if ( !captureInterruptedPress &&
         (transition.HasAction(wxWinUIInputActionKind::CancelPress) ||
         transition.HasAction(wxWinUIInputActionKind::ReleaseCapture) )
       )
    {
        m_inputState.ArmStormGuard(16, 3, 4);
    }

    if ( !ExecuteInputTransition(transition) )
        return;

    if ( wxWinUIInputLogEnabled() )
        wxWinUIInputLog("root %s",
                        captureLost ? "CAPTURE-LOST"
                                    : "POINTER-CANCELED");
}

bool wxWinUITopLevelHost::ValidateNativeResize(
    const std::shared_ptr<NativeResizeRequest>& request)
{
    if ( !request || m_nativeResize != request || m_shuttingDown ||
         !m_hostLifetime || m_hostLifetime->GetHost() != this ||
         request->cancelled || !request->target.IsValid() ||
         !request->isCurrent || !request->grip.get() ||
         request->inputSiteGeneration != m_inputSiteGeneration )
    {
        return false;
    }

    wxWindow * const source = request->source.get();
    wxWindow * const tlw = m_tlwLifetime.get();
    if ( !source || !tlw || source->IsBeingDeleted() ||
         tlw->IsBeingDeleted() || wxGetTopLevelParent(source) != tlw ||
         FindSlotOwner(source) != this || !source->IsShownOnScreen() ||
         !source->IsEnabled() || !tlw->HasFlag(wxRESIZE_BORDER) )
    {
        return false;
    }

    const HWND hwnd =
        reinterpret_cast<HWND>(request->target.GetLeafHwnd());
    if ( hwnd != GetHwndOf(tlw) || !::IsWindowVisible(hwnd) ||
         !::IsWindowEnabled(hwnd) || ::IsZoomed(hwnd) || ::IsIconic(hwnd) ||
         !(::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_THICKFRAME) ||
         (request->hitTest != HTBOTTOMLEFT &&
          request->hitTest != HTBOTTOMRIGHT) )
    {
        return false;
    }

    // Peer validation may cross wx/XAML callbacks. Never use its verdict
    // without checking the host, request and native identity again.
    if ( !request->isCurrent() )
        return false;
    return !m_shuttingDown && m_nativeResize == request &&
           m_hostLifetime && m_hostLifetime->GetHost() == this &&
           !request->cancelled && request->source.get() == source &&
           request->target.IsValid() &&
           request->inputSiteGeneration == m_inputSiteGeneration &&
           !source->IsBeingDeleted() && !tlw->IsBeingDeleted() &&
           wxGetTopLevelParent(source) == tlw && FindSlotOwner(source) == this &&
           source->IsShownOnScreen() && source->IsEnabled() &&
           tlw->HasFlag(wxRESIZE_BORDER) && ::IsWindowVisible(hwnd) &&
           ::IsWindowEnabled(hwnd) && !::IsZoomed(hwnd) && !::IsIconic(hwnd) &&
           (::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_THICKFRAME);
}

bool wxWinUITopLevelHost::RequestNativeResize(
    wxWindow *source,
    const winrt::Microsoft::UI::Xaml::UIElement& grip,
    int hitTest,
    const wxWinUIPointerSample& sample,
    std::function<bool ()> isCurrent)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || m_nativeResize || !source || !grip ||
         !isCurrent || !m_tlw || !m_bridge ||
         sample.device != wxWinUIInputDevice::Mouse ||
         sample.kind != wxWinUIInputKind::Press ||
         sample.button != wxWinUIInputButton::Left || !sample.isPrimary ||
         sample.isCompatibilityMouse || !(sample.buttonMask & MK_LBUTTON) ||
         m_nativeInputDispatchInFlight || !m_pendingNativeInput.empty() ||
         m_inputState.HasAnyActivePress() ||
         std::any_of(m_slotPointerOwners.begin(), m_slotPointerOwners.end(),
                     [](const SlotPointerOwner& owner)
                     {
                         return owner.IsActive();
                     }) )
    {
        return false;
    }

    const HWND capture = ::GetCapture();
    if ( capture && capture != m_bridge && capture != m_inner &&
         !::IsChild(m_bridge, capture) )
    {
        return false;
    }

    auto request = std::make_shared<NativeResizeRequest>();
    request->source = source;
    request->grip = winrt::make_weak(grip);
    request->sample = sample;
    request->hitTest = hitTest;
    request->isCurrent = std::move(isCurrent);
    if ( !wxWinUIGetNativeTarget(m_tlw, m_tlw->GetHandle(),
                                 &request->target) )
    {
        return false;
    }
    if ( ++m_nextNativeInputSequence == 0 )
        ++m_nextNativeInputSequence;
    request->ticket = m_nextNativeInputSequence;
    AdvanceInputSiteGeneration();
    request->inputSiteGeneration = m_inputSiteGeneration;
    m_nativeResize = request;

    try
    {
        if ( !ValidateNativeResize(request) || !PostNativeResizeWake() )
        {
            FinishNativeResize(request, true);
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI native resize request", e);
        FinishNativeResize(request, true);
        return false;
    }

    request->accepted = true;
    m_nativeResizeSnapshot.phase = wxWinUINativeResizePhase::Pending;
    m_nativeResizeSnapshot.ticket = request->ticket;
    m_nativeResizeSnapshot.hitTest = hitTest;
    m_nativeResizeSnapshot.screenPoint = { sample.screenX, sample.screenY };
    ++m_nativeResizeSnapshot.scheduled;
    return true;
}

bool wxWinUITopLevelHost::PostNativeResizeWake()
{
    if ( m_nativeResizeWakePosted )
        return true;
    if ( !m_nativeResize || m_shuttingDown || !m_bridge )
        return false;
    if ( gs_failNextNativeResizePost )
    {
        gs_failNextNativeResizePost = false;
        return false;
    }
    const UINT message = wxWinUINativeResizeMessage();
    const std::uint64_t ticket = m_nativeResize->ticket;
    if ( !message ||
         !::PostMessageW(m_bridge, message,
                         static_cast<WPARAM>(ticket & 0xffffffffu),
                         static_cast<LPARAM>(ticket >> 32)) )
    {
        return false;
    }
    m_nativeResizeWakePosted = true;
    return true;
}

void wxWinUITopLevelHost::ScheduleNativeResizeWakesAfterGlobalOperation()
{
    if ( gs_winuiOperationDepth || gs_deferredNativeResizeWakes.empty() )
        return;
    const std::set<wxWinUITopLevelHost *> pending =
        std::move(gs_deferredNativeResizeWakes);
    gs_deferredNativeResizeWakes.clear();
    for ( wxWinUITopLevelHost *host : pending )
    {
        if ( !host->PostNativeResizeWake() )
            host->FinishNativeResize(host->m_nativeResize, true);
    }
}

void wxWinUITopLevelHost::FinishNativeResize(
    std::shared_ptr<NativeResizeRequest> request,
    bool cancelled)
{
    if ( !request || m_nativeResize != request )
        return;

    // Publish idle before retiring the native observer. A failed subclass
    // removal becomes an inert weak-lifetime context, as for the island.
    m_nativeResize.reset();
    m_nativeResizeWakePosted = false;
    gs_deferredNativeResizeWakes.erase(this);
    m_nativeResizeSnapshot.phase = wxWinUINativeResizePhase::Idle;
    if ( cancelled || request->cancelled || !request->entered )
        ++m_nativeResizeSnapshot.cancelled;
    wxWinUIRetireSubclass(
        reinterpret_cast<HWND>(request->target.GetLeafHwnd()),
        NativeResizeSubclassProc, static_cast<UINT_PTR>(request->ticket),
        request->subclassContext, "native-resize");
    if ( !m_shuttingDown && request->accepted )
        PostIslandCancelMode(request->inputSiteGeneration);
}

void wxWinUITopLevelHost::CancelNativeResize(
    const wxWinUIPointerSample& sample)
{
    const auto request = m_nativeResize;
    if ( !request ||
         (!sample.interruptAll &&
          (sample.device != request->sample.device ||
           sample.pointerId != request->sample.pointerId)) )
    {
        return;
    }
    switch ( sample.kind )
    {
        case wxWinUIInputKind::Press:
            if ( sample.button != wxWinUIInputButton::Left ||
                 sample.timestamp == request->sample.timestamp )
                return;
            break;
        case wxWinUIInputKind::Release:
            if ( sample.button != wxWinUIInputButton::Left )
                return;
            break;
        case wxWinUIInputKind::CaptureLost:
            // Losing island capture is expected both during our release and
            // when USER32 takes over. Other native capture changes cancel.
            if ( request->preparing ||
                 (request->dispatching && ::GetCapture() ==
                    reinterpret_cast<HWND>(request->target.GetLeafHwnd())) )
                return;
            break;
        case wxWinUIInputKind::Leave:
            if ( request->dispatching || request->preparing )
                return;
            break;
        case wxWinUIInputKind::Cancel:
            break;
        default:
            return;
    }

    request->cancelled = true;
    if ( request->dispatching )
    {
        if ( request->target.IsValid() )
        {
            ::SendMessageW(
                reinterpret_cast<HWND>(request->target.GetLeafHwnd()),
                WM_CANCELMODE, 0, 0);
        }
    }
    else
    {
        FinishNativeResize(request, true);
    }
}

void wxWinUITopLevelHost::DispatchNativeResize(std::uint64_t ticket)
{
    const auto request = m_nativeResize;
    if ( !request || request->ticket != ticket )
        return;
    m_nativeResizeWakePosted = false;
    if ( gs_winuiOperationDepth )
    {
        // A nested wxYield may pump this wake before the routed callback has
        // unwound. The common outer-operation tail, not idle CallAfter or a
        // repost spin, owns the next asynchronous wake.
        gs_deferredNativeResizeWakes.insert(this);
        return;
    }

    OperationGuard operation(this);
    const auto finish = wxMakeGuard([this, request]()
    {
        FinishNativeResize(request, request->cancelled);
    });
    const auto contactActive = []()
    {
        return gs_nativeResizeContactReader
            ? gs_nativeResizeContactReader(gs_nativeResizeContactContext)
            : wxWinUIIsPointerContactActive(
                  wxWinUIInputDevice::Mouse, 0, VK_LBUTTON);
    };

    try
    {
        if ( !contactActive() || !ValidateNativeResize(request) )
            return;

        request->preparing = true;
        const auto prepared = wxMakeGuard([request]()
        {
            request->preparing = false;
        });

        // The request already owns this exact pointer before XAML capture
        // release can re-enter. Never release a native control's capture or
        // a host/slot gesture that appeared while the wake was pending.
        if ( m_inputState.HasAnyActivePress() ||
             m_nativeInputDispatchInFlight || !m_pendingNativeInput.empty() ||
             std::any_of(m_slotPointerOwners.begin(), m_slotPointerOwners.end(),
                         [](const SlotPointerOwner& owner)
                         {
                             return owner.IsActive();
                         }) )
        {
            return;
        }
        const HWND capture = ::GetCapture();
        const unsigned long long captureGeneration =
            m_nativeCaptureMutationGeneration;
        if ( capture && capture != m_bridge && capture != m_inner &&
             !::IsChild(m_bridge, capture) )
        {
            return;
        }

        const auto grip = request->grip.get();
        if ( !grip )
            return;
        grip.ReleasePointerCaptures();
        if ( !ValidateNativeResize(request) )
            return;
        CancelIslandPointerState(false);
        if ( !ValidateNativeResize(request) )
            return;

        const HWND remainingCapture = ::GetCapture();
        if ( remainingCapture )
        {
            // An application callback may have replaced capture while XAML
            // released its pointer. Only the same island-owned capture can
            // still belong to this handoff; a new owner wins.
            if ( remainingCapture != capture ||
                 m_nativeCaptureMutationGeneration != captureGeneration ||
                 (remainingCapture != m_bridge && remainingCapture != m_inner &&
                  !::IsChild(m_bridge, remainingCapture)) )
            {
                return;
            }
            NotifyNativeCaptureMutation();
            if ( !::ReleaseCapture() )
                return;
        }
        if ( !ValidateNativeResize(request) || ::GetCapture() )
            return;

        const HWND hwnd =
            reinterpret_cast<HWND>(request->target.GetLeafHwnd());
        auto * const context = new wxWinUISubclassContext(m_hostLifetime);
        if ( !::SetWindowSubclass(
                 hwnd, NativeResizeSubclassProc,
                 static_cast<UINT_PTR>(request->ticket),
                 reinterpret_cast<DWORD_PTR>(context)) )
        {
            delete context;
            return;
        }
        request->subclassContext = context;

        // WM_NCLBUTTONDOWN has signed 16-bit coordinates. Keep the original
        // full-width point throughout validation, and fail closed instead of
        // silently starting from a wrapped point on an oversized desktop.
        if ( request->sample.screenX < (std::numeric_limits<short>::min)() ||
             request->sample.screenX > (std::numeric_limits<short>::max)() ||
             request->sample.screenY < (std::numeric_limits<short>::min)() ||
             request->sample.screenY > (std::numeric_limits<short>::max)() )
        {
            return;
        }
        // The physical check follows the callback-bearing peer validation:
        // a release processed during that validation must not start a late
        // tracking loop. No application callback follows this final check.
        if ( !ValidateNativeResize(request) || ::GetCapture() ||
             !contactActive() || m_nativeResize != request ||
             request->cancelled || m_shuttingDown )
        {
            return;
        }
        request->preparing = false;
        request->dispatching = true;
        wxWinUIInputLog("grip resize #%llu dispatch ht=%d point=(%d,%d)",
                        request->ticket, request->hitTest,
                        request->sample.screenX, request->sample.screenY);
        ::SendMessageW(hwnd, WM_NCLBUTTONDOWN, request->hitTest,
                       MAKELPARAM(request->sample.screenX,
                                  request->sample.screenY));
        request->dispatching = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI native resize dispatch", e);
        request->cancelled = true;
    }
}

LRESULT CALLBACK wxWinUITopLevelHost::NativeResizeSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
    UINT_PTR subclassId, DWORD_PTR refData)
{
    auto * const context = reinterpret_cast<wxWinUISubclassContext *>(refData);
    const auto state = context && context->active ? context->host.lock() : nullptr;
    wxWinUITopLevelHost * const host = state ? state->GetHost() : nullptr;
    const auto request = host ? host->m_nativeResize : nullptr;
    const bool matches = request && request->ticket == subclassId &&
        request->target.GetLeafHwnd() == reinterpret_cast<WXHWND>(hwnd);
    if ( matches )
    {
        if ( msg == WM_ENTERSIZEMOVE && request->dispatching &&
             !request->entered )
        {
            request->entered = true;
            host->m_nativeResizeSnapshot.phase = wxWinUINativeResizePhase::Entered;
            ++host->m_nativeResizeSnapshot.entered;
            wxWinUIInputLog("grip resize #%llu entered", request->ticket);
        }
        else if ( msg == WM_EXITSIZEMOVE && request->entered &&
                  !request->exited )
        {
            request->exited = true;
            host->m_nativeResizeSnapshot.phase = wxWinUINativeResizePhase::Idle;
            ++host->m_nativeResizeSnapshot.exited;
            wxWinUIInputLog("grip resize #%llu exited", request->ticket);
        }
        else if ( msg == WM_CANCELMODE || msg == WM_NCDESTROY )
        {
            request->cancelled = true;
        }
    }

    if ( msg == WM_NCDESTROY )
    {
        if ( matches )
            request->subclassContext = nullptr;
        if ( context )
            context->active = false;
        (void)::RemoveWindowSubclass(hwnd, NativeResizeSubclassProc, subclassId);
        const LRESULT result = ::DefSubclassProc(hwnd, msg, wParam, lParam);
        delete context;
        return result;
    }
    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

bool wxWinUITopLevelHost::QueueModalNativeDispatch(
    const wxWinUINativeHit& hit,
    const wxWinUIInputAction& action,
    UINT message,
    int virtualKey,
    WORD xButton,
    wxWinUIInputDevice device,
    std::uint32_t pointerId)
{
    if ( m_shuttingDown || m_nativeResize || !m_hostLifetime || !wxTheApp ||
         !hit.IsValid() ||
         hit.GetArea() != wxWinUINativeArea::NonClient ||
         !wxWinUIStartsSystemModalLoop(hit.GetHitTest()) )
    {
        return false;
    }

    PendingNativeDispatch pending;
    pending.hit = hit;
    pending.action = action;
    pending.message = message;
    pending.virtualKey = virtualKey;
    pending.xButton = xButton;
    pending.device = device;
    pending.pointerId = pointerId;
    if ( ++m_nextNativeInputSequence == 0 )
        ++m_nextNativeInputSequence;
    pending.sequence = m_nextNativeInputSequence;
    pending.cancellationGeneration =
        m_nativeInputCancellationGeneration;
    pending.inputSiteGeneration = m_inputSiteGeneration;
    m_pendingNativeInput.push_back(pending);

    if ( m_nativeInputDispatchScheduled )
        return true;

    m_nativeInputDispatchScheduled = true;
    const std::weak_ptr<wxWinUIHostLifetime> weakState = m_hostLifetime;
    wxTheApp->CallAfter(
        [weakState]()
        {
            const std::shared_ptr<wxWinUIHostLifetime> state =
                weakState.lock();
            wxWinUITopLevelHost * const host =
                state ? state->GetHost() : nullptr;
            if ( host )
                host->DispatchPendingNativeInput();
        });
    return true;
}

void wxWinUITopLevelHost::CancelPendingNativeDispatch(
    const wxWinUIPointerSample& sample)
{
    CancelNativeResize(sample);
    const bool cancelsPointer =
        sample.kind == wxWinUIInputKind::Press ||
        sample.kind == wxWinUIInputKind::Release ||
        sample.kind == wxWinUIInputKind::Cancel ||
        sample.kind == wxWinUIInputKind::CaptureLost ||
        sample.kind == wxWinUIInputKind::Leave;
    if ( !cancelsPointer )
        return;

    const auto matches =
        [&sample](const PendingNativeDispatch& pending)
        {
            const bool samePointer =
                sample.interruptAll ||
                (pending.device == sample.device &&
                 pending.pointerId == sample.pointerId);
            const bool sameButton =
                (sample.kind != wxWinUIInputKind::Press &&
                 sample.kind != wxWinUIInputKind::Release) ||
                sample.button == pending.action.button;
            return samePointer && sameButton;
        };

    bool canceled = false;
    std::vector<std::uint32_t> canceledResetGenerations;
    for ( auto it = m_pendingNativeInput.begin();
          it != m_pendingNativeInput.end(); )
    {
        if ( matches(*it) )
        {
            wxWinUIInputLog("modal dispatch #%llu canceled by kind=%d",
                            it->sequence,
                            static_cast<int>(sample.kind));
            canceledResetGenerations.push_back(
                it->inputSiteGeneration);
            it = m_pendingNativeInput.erase(it);
            canceled = true;
        }
        else
        {
            ++it;
        }
    }

    // The deque entry is popped before callback-bearing target refresh. Keep
    // the in-flight identity cancelable until its DOWN is committed. Once the
    // system loop is running, Leave alone is not termination; Release/Cancel
    // and a superseding Press remain authoritative and will update the exact
    // serial in inputstate.
    if ( m_nativeInputDispatchInFlight &&
         matches(m_inFlightNativeInput) &&
         (!m_nativeInputDispatchCommitted ||
          sample.kind != wxWinUIInputKind::Leave) )
    {
        canceled = true;
    }

    if ( canceled )
    {
        if ( ++m_nativeInputCancellationGeneration == 0 )
            ++m_nativeInputCancellationGeneration;
    }

    for ( const std::uint32_t generation :
          canceledResetGenerations )
    {
        PostIslandCancelMode(generation);
    }
}

bool wxWinUITopLevelHost::HasPendingNativeDispatch(
    const wxWinUIPointerSample& sample) const
{
    if ( m_nativeResize &&
         (sample.interruptAll ||
          (m_nativeResize->sample.device == sample.device &&
           m_nativeResize->sample.pointerId == sample.pointerId)) &&
         (sample.button == wxWinUIInputButton::None ||
          sample.button == wxWinUIInputButton::Left) )
    {
        return true;
    }
    const auto matches =
        [&sample](const PendingNativeDispatch& pending)
        {
            return (sample.interruptAll ||
                    (pending.device == sample.device &&
                     pending.pointerId == sample.pointerId)) &&
                   (sample.button == wxWinUIInputButton::None ||
                    pending.action.button == sample.button);
        };

    if ( m_nativeInputDispatchInFlight &&
         matches(m_inFlightNativeInput) )
    {
        return true;
    }

    return std::any_of(
        m_pendingNativeInput.begin(), m_pendingNativeInput.end(),
        matches);
}

bool wxWinUITopLevelHost::RememberModalAwaitingRelease(
    const wxWinUIInputAction& action)
{
    if ( !action.gestureSerial ||
         action.action != wxWinUIInputActionKind::Dispatch ||
         (action.kind != wxWinUIInputKind::Press &&
          action.kind != wxWinUIInputKind::DoubleClick) )
    {
        return false;
    }

    wxWinUIInputAction *freeEntry = nullptr;
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( !current.gestureSerial )
        {
            if ( !freeEntry )
                freeEntry = &current;
            continue;
        }

        if ( current.device == action.device &&
             current.pointerId == action.pointerId &&
             current.button == action.button )
        {
            if ( current.gestureSerial == action.gestureSerial )
            {
                current = action;
                return true;
            }

            // A newer DOWN for this exact physical button must explicitly
            // retire the old serial before it can replace this entry.
            return false;
        }
    }

    if ( !freeEntry )
        return false;

    *freeEntry = action;
    return true;
}

wxWinUIInputAction wxWinUITopLevelHost::TakeModalAwaitingRelease(
    wxWinUIInputDevice device,
    std::uint32_t pointerId,
    wxWinUIInputButton button)
{
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial &&
             current.device == device &&
             current.pointerId == pointerId &&
             current.button == button )
        {
            const wxWinUIInputAction result = current;
            current = wxWinUIInputAction();
            return result;
        }
    }
    return wxWinUIInputAction();
}

void wxWinUITopLevelHost::ForgetModalAwaitingRelease(
    std::uint64_t gestureSerial)
{
    if ( !gestureSerial )
        return;

    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial == gestureSerial )
            current = wxWinUIInputAction();
    }
}

void wxWinUITopLevelHost::PruneModalAwaitingReleases()
{
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial &&
             !m_inputState.HasActiveGesture(current) )
        {
            current = wxWinUIInputAction();
        }
    }
}

std::size_t
wxWinUITopLevelHost::GetModalAwaitingReleaseCountForTest() const
{
    return static_cast<std::size_t>(std::count_if(
        m_modalAwaitingReleases.begin(), m_modalAwaitingReleases.end(),
        [](const wxWinUIInputAction& action)
        {
            return action.gestureSerial != 0;
        }));
}

void wxWinUITopLevelHost::DispatchPendingNativeInput()
{
    OperationGuard operation(this);
    m_nativeInputDispatchScheduled = false;

    // A system-modal SendMessage pumps the dispatcher. A CallAfter posted by
    // nested input may therefore re-enter this method; the outer dispatcher
    // owns the single in-flight identity and will drain the newly queued
    // request when the native loop returns.
    if ( m_nativeInputDispatchInFlight )
        return;

    while ( !m_shuttingDown && !m_pendingNativeInput.empty() )
    {
        const PendingNativeDispatch pending =
            m_pendingNativeInput.front();
        m_pendingNativeInput.pop_front();
        m_inFlightNativeInput = pending;
        m_nativeInputDispatchInFlight = true;
        m_nativeInputDispatchCommitted = false;
        const auto clearInFlight = wxMakeGuard(
            [this]()
            {
                m_nativeInputDispatchInFlight = false;
                m_nativeInputDispatchCommitted = false;
                m_inFlightNativeInput = PendingNativeDispatch();
                RetryDeferredIslandCancelMode();
            });
        // CancelIslandPointerState(false) transferred one generation-bound
        // reset obligation to every queued request. Fulfil it on every
        // terminal path: pre-commit drop, cancellation, modal return or
        // normal completion.
        const auto finishIslandReset = wxMakeGuard(
            [this, generation = pending.inputSiteGeneration]()
            {
                if ( !m_shuttingDown )
                    PostIslandCancelMode(generation);
            });
        bool nativeDispatchStarted = false;
        const auto abortUnstartedHover = wxMakeGuard(
            [this, &pending, &nativeDispatchStarted]()
            {
                if ( !nativeDispatchStarted &&
                     AbortUndeliveredNativeHover(pending.action) &&
                     !m_shuttingDown )
                {
                    ResetRootCursor();
                }
            });

        // Starting a system-modal loop after the physical press has already
        // ended would leave it waiting for a release that can never arrive.
        if ( !wxWinUIIsPointerContactActive(
                 pending.device, pending.pointerId, pending.virtualKey) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: button released",
                            pending.sequence);
            continue;
        }

        const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
        const unsigned long long epoch = m_inputTransitionEpoch;
        wxWinUINativeHit current;
        const bool refreshed = wxWinUIRefreshNativeHit(
            m_tlw, pending.hit, m_bridge, m_inner, &current);
        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown ||
             m_inputTransitionEpoch != epoch ||
             pending.cancellationGeneration !=
                m_nativeInputCancellationGeneration ||
             !refreshed ||
             current.GetArea() != wxWinUINativeArea::NonClient ||
             !wxWinUIStartsSystemModalLoop(current.GetHitTest()) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: target changed",
                            pending.sequence);
            continue;
        }

        // Do not steal a capture established while this request was queued.
        if ( ::GetCapture() )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: capture=%p",
                            pending.sequence,
                            static_cast<void *>(::GetCapture()));
            continue;
        }

        const wxWinUINativeTarget targetIdentity = current.GetTarget();
        if ( !targetIdentity.IsValid() )
            continue;

        const HWND target =
            reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd());
        const POINT pendingScreen = pending.hit.GetScreenPoint();
        const LPARAM screen =
            MAKELPARAM(static_cast<short>(pendingScreen.x),
                       static_cast<short>(pendingScreen.y));

        // This SendMessage is deliberately outside the XAML routed-event
        // callback. DefWindowProc may run the complete move/size/caption
        // system-modal loop before it returns.
        const WPARAM hitAndButton =
            pending.xButton
                ? MAKEWPARAM(static_cast<WORD>(current.GetHitTest()),
                             pending.xButton)
                : static_cast<WPARAM>(current.GetHitTest());

        // The queued action itself is the gesture identity. If another input
        // transition changed this button while CallAfter was pending, the
        // two-phase commit rejects the stale DOWN even if a newer physical
        // press currently makes GetAsyncKeyState() look active.
        if ( !m_inputState.CommitBeforeDispatch(pending.action) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: stale gesture",
                            pending.sequence);
            continue;
        }
        m_nativeInputDispatchCommitted = true;
        MarkNativeHoverDispatchStarted(pending.action);
        nativeDispatchStarted = true;

        ::SendMessage(target, pending.message, hitAndButton, screen);

        if ( m_shuttingDown )
            return;

        // Not every non-client DOWN consumes its matching UP. In particular,
        // a caption double-click can return immediately after maximize while
        // the second contact is still held. Preserve the exact PressState so
        // the real routed UP can balance it, but do not let an aborted modal
        // loop seed a third click.
        if ( wxWinUIIsPointerContactActive(
                 pending.device, pending.pointerId, pending.virtualKey) )
        {
            const bool hasPublishedBoundaryState =
                m_inFlightNativeInput.sequence == pending.sequence;
            const bool leaveObserved =
                hasPublishedBoundaryState &&
                !m_inFlightNativeInput.pointerInsideRoot;
            bool pointerInsideRoot =
                hasPublishedBoundaryState &&
                m_inFlightNativeInput.pointerInsideRoot;
            bool geometryOutsideWithoutLeave = false;
            if ( pointerInsideRoot &&
                 pending.device == wxWinUIInputDevice::Mouse )
            {
                POINT currentPointer = { 0, 0 };
                const RECT rootScreen = GetClientScreenRect();
                pointerInsideRoot =
                    ::GetCursorPos(&currentPointer) &&
                    ::PtInRect(&rootScreen, currentPointer);
                geometryOutsideWithoutLeave =
                    !pointerInsideRoot && !leaveObserved;
            }

            if ( !pointerInsideRoot &&
                 m_inputState.HasActiveGesture(pending.action) )
            {
                wxWinUIPointerSample cancel;
                cancel.device = pending.action.device;
                cancel.kind = wxWinUIInputKind::Cancel;
                cancel.button = pending.action.button;
                cancel.pointerId = pending.action.pointerId;
                cancel.timestamp = ::GetTickCount64();
                cancel.modifiers = pending.action.modifiers;
                cancel.screenX = pending.action.screenX;
                cancel.screenY = pending.action.screenY;
                cancel.buttonMask =
                    pending.action.buttonMask &
                    ~wxWinUIGetWParamButtonMask(
                        pending.action.button);
                const wxWinUIInputTransition cancellation =
                    m_inputState.CancelActiveGesture(
                        pending.action, cancel);
                unsigned long long expectedEpoch =
                    m_inputTransitionEpoch;
                if ( !cancellation.actions.empty() )
                {
                    if ( ++expectedEpoch == 0 )
                        ++expectedEpoch;
                }
                const bool cancellationDelivered =
                    ExecuteInputTransition(cancellation);

                // A routed Leave already closed hover while SendMessage was
                // pumping. Only synthesize the boundary when geometry proves
                // that no such event reached us.
                if ( cancellationDelivered &&
                     geometryOutsideWithoutLeave &&
                     hostState && hostState->GetHost() == this &&
                     !m_shuttingDown &&
                     m_inputTransitionEpoch == expectedEpoch )
                {
                    wxWinUIPointerSample leave = cancel;
                    leave.kind = wxWinUIInputKind::Leave;
                    const wxWinUIInputTransition boundary =
                        m_inputState.RouteBoundary(leave);
                    (void)ExecuteInputTransition(boundary);
                }
            }
            else if ( m_inputState.SuppressClickForGesture(
                          pending.action) )
            {
                if ( !RememberModalAwaitingRelease(pending.action) )
                {
                    // Never overwrite an unrelated held button. The fixed
                    // table is deliberately larger than the V0 device policy,
                    // but if its invariant is ever exceeded, fail closed by
                    // retiring this exact serial instead of stranding it.
                    wxWinUIPointerSample cancel;
                    cancel.device = pending.action.device;
                    cancel.kind = wxWinUIInputKind::Cancel;
                    cancel.button = pending.action.button;
                    cancel.pointerId = pending.action.pointerId;
                    cancel.timestamp = ::GetTickCount64();
                    cancel.screenX = pending.action.screenX;
                    cancel.screenY = pending.action.screenY;
                    cancel.buttonMask =
                        pending.action.buttonMask &
                        ~wxWinUIGetWParamButtonMask(
                            pending.action.button);
                    const wxWinUIInputTransition cancellation =
                        m_inputState.CancelActiveGesture(
                            pending.action, cancel);
                    (void)ExecuteInputTransition(cancellation);
                    wxLogWarning("wxWinUI: modal release table exhausted; "
                                 "canceled gesture #%llu",
                                 pending.action.gestureSerial);
                }
            }
            wxWinUIInputLog(
                "modal dispatch #%llu returned with contact active (%s)",
                pending.sequence,
                pointerInsideRoot ? "inside" : "outside");
            continue;
        }

        // DefWindowProc consumed the physical release inside its system
        // tracking loop. Retire the committed semantic press without sending
        // a second UP. A click is recorded only if the final geometry still
        // proves the same non-client target/zone, enabling the next queued
        // DOWN to become WM_NC*BUTTONDBLCLK.
        wxWinUIPointerSample release;
        release.device = pending.action.device;
        release.kind = wxWinUIInputKind::Release;
        release.button = pending.action.button;
        release.pointerId = pending.action.pointerId;
        release.timestamp = ::GetTickCount64();
        release.modifiers = pending.action.modifiers;
        release.buttonMask =
            pending.action.buttonMask &
            ~wxWinUIGetWParamButtonMask(pending.action.button);

        POINT finalScreen = { 0, 0 };
        if ( ::GetCursorPos(&finalScreen) )
        {
            release.screenX = finalScreen.x;
            release.screenY = finalScreen.y;
        }

        wxWinUIRouteObservation observation;
        wxWinUINativeHit finalHit;
        const wxWinUIHitResolution resolution =
            wxWinUIResolveNativeHit(
                m_tlw, finalScreen, m_bridge, m_inner, &finalHit);
        if ( resolution == wxWinUIHitResolution::Hit )
        {
            observation.surface = wxWinUIInputSurface::Native;
            observation.target =
                wxWinUIGetInputTargetKey(finalHit.GetTarget());
            observation.area =
                finalHit.GetArea() == wxWinUINativeArea::Client
                    ? wxWinUIInputArea::Client
                    : wxWinUIInputArea::NonClient;
            observation.zone = finalHit.GetHitTest();
            observation.clientDoubleClicks =
                (::GetClassLongPtr(
                    reinterpret_cast<HWND>(
                        finalHit.GetTarget().GetLeafHwnd()),
                    GCL_STYLE) & CS_DBLCLKS) != 0;
        }
        else
        {
            observation.surface =
                resolution == wxWinUIHitResolution::Unstable
                    ? wxWinUIInputSurface::Indeterminate
                    : wxWinUIInputSurface::Outside;
        }

        // The modal loop can dispatch a release/cancel and even a complete
        // newer gesture before SendMessage returns. Route the synthetic
        // balancing release only while this exact committed serial still
        // owns the button; Route() itself mutates hover/click state.
        if ( !m_inputState.HasActiveGesture(pending.action) )
        {
            wxWinUIInputLog(
                "modal dispatch #%llu returned after gesture retired",
                pending.sequence);
            continue;
        }

        const wxWinUIInputTransition completion =
            m_inputState.Route(release, observation);
        const wxWinUIInputAction *balancingRelease = nullptr;
        for ( const wxWinUIInputAction& action : completion.actions )
        {
            if ( action.action == wxWinUIInputActionKind::Dispatch &&
                 action.kind == wxWinUIInputKind::Release &&
                 action.gestureSerial ==
                    pending.action.gestureSerial )
            {
                balancingRelease = &action;
                break;
            }
        }

        wxWinUIInputTransition cleanup;
        if ( balancingRelease )
        {
            if ( !m_inputState.CommitBeforeDispatch(*balancingRelease) )
            {
                wxWinUIInputAction cancellation;
                if ( m_inputState.AbortUndeliverableRelease(
                         *balancingRelease, &cancellation) )
                {
                    ExecuteEmergencyInputCleanup(cancellation);
                }
            }

            for ( const wxWinUIInputAction& action : completion.actions )
            {
                if ( action.action != wxWinUIInputActionKind::Dispatch )
                    cleanup.actions.push_back(action);
            }
        }
        if ( !cleanup.actions.empty() )
            (void)ExecuteInputTransition(cleanup);

        if ( !m_inputState.HasActiveGesture(pending.action) )
            ForgetModalAwaitingRelease(pending.action.gestureSerial);

    }
}

void wxWinUITopLevelHost::CancelIslandPointerState(bool postCancelMode)
{
    OperationGuard operation(this);
    if ( m_cancelingIslandPointerState )
        return;

    m_cancelingIslandPointerState = true;
    const auto resetCancelGuard = wxMakeGuard(
        [this]()
        {
            m_cancelingIslandPointerState = false;
        });

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto root = m_root;

    // This function is reserved for a proven interrupted/native-transferred
    // gesture. Only that evidence arms the otherwise inert state guard.
    m_inputState.ArmStormGuard(4096, 3, 4);

    // Release any XAML pointer capture the root grabbed for this gesture...
    if ( root )
    {
        try
        {
            root.ReleasePointerCaptures();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    // ReleasePointerCaptures() synchronously raises pointer/capture events.
    // Teardown from an application handler invalidates hostState before any
    // native HWND below may be touched.
    if ( !hostState || hostState->GetHost() != this || m_shuttingDown )
        return;

    // Modal non-client forwarding defers this until its native loop has
    // actually returned; otherwise a posted cancel can race and terminate
    // the very loop being started.
    if ( postCancelMode )
        PostIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("island reset post-cancel=%d "
                        "(inner=%p bridge=%p)",
                        postCancelMode,
                        static_cast<void *>(m_inner),
                        static_cast<void *>(m_bridge));
    }
}

void wxWinUITopLevelHost::PostIslandCancelMode()
{
    PostIslandCancelMode(m_inputSiteGeneration);
}

bool wxWinUITopLevelHost::CanPostIslandCancelMode(
    std::uint32_t expectedGeneration) const
{
    return expectedGeneration == m_inputSiteGeneration &&
           !m_nativeResize &&
           !m_inputState.HasAnyActivePress() &&
           m_pendingNativeInput.empty() &&
           std::none_of(
               m_slotPointerOwners.begin(), m_slotPointerOwners.end(),
               [](const SlotPointerOwner& owner)
               {
                   return owner.IsActive();
               });
}

void wxWinUITopLevelHost::AcknowledgeIslandCancelMessage(
    std::uint32_t generation)
{
    if ( m_postedIslandCancelMessages &&
         m_postedIslandCancelGeneration == generation )
    {
        --m_postedIslandCancelMessages;
        if ( !m_postedIslandCancelMessages )
            m_postedIslandCancelGeneration = 0;
    }
}

void wxWinUITopLevelHost::RetireIslandCancelTarget(HWND retiredTarget)
{
    if ( !retiredTarget || m_shuttingDown )
        return;

    const bool postedForCurrentSite =
        m_postedIslandCancelMessages &&
        m_postedIslandCancelGeneration == m_inputSiteGeneration;
    const bool deferredForCurrentSite =
        m_islandCancelDeferred &&
        m_deferredIslandCancelGeneration == m_inputSiteGeneration;

    // A stale batch can never coalesce the current generation, but one of its
    // target HWNDs has now disappeared and can never acknowledge it either.
    if ( m_postedIslandCancelMessages && !postedForCurrentSite )
    {
        m_postedIslandCancelMessages = 0;
        m_postedIslandCancelGeneration = 0;
    }

    if ( !postedForCurrentSite && !deferredForCurrentSite )
        return;

    // A posted batch identifies both its generation and the concrete input
    // site HWNDs. If either target retires, invalidate every old message and
    // transfer the still-required reset to the replacement generation. This
    // prevents a lost acknowledgement from coalescing resets forever.
    m_postedIslandCancelMessages = 0;
    m_postedIslandCancelGeneration = 0;
    m_islandCancelDeferred = false;
    m_deferredIslandCancelGeneration = 0;
    if ( ++m_inputSiteGeneration == 0 )
        ++m_inputSiteGeneration;
    m_islandCancelDeferred = true;
    m_deferredIslandCancelGeneration = m_inputSiteGeneration;
}

void wxWinUITopLevelHost::RetryDeferredIslandCancelMode()
{
    if ( !m_islandCancelDeferred )
        return;

    if ( m_shuttingDown ||
         m_deferredIslandCancelGeneration != m_inputSiteGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
        return;
    }

    if ( !CanPostIslandCancelMode(
             m_deferredIslandCancelGeneration) )
    {
        return;
    }

    const std::uint32_t generation =
        m_deferredIslandCancelGeneration;
    m_islandCancelDeferred = false;
    m_deferredIslandCancelGeneration = 0;
    PostIslandCancelMode(generation);
}

void wxWinUITopLevelHost::AdvanceInputSiteGeneration()
{
    if ( ++m_inputSiteGeneration == 0 )
        ++m_inputSiteGeneration;

    // A newer physical DOWN proves that an older deferred input-site reset
    // must never be applied to the new gesture.
    if ( m_islandCancelDeferred &&
         m_deferredIslandCancelGeneration != m_inputSiteGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
    }
}

void wxWinUITopLevelHost::NotifyNativeCaptureMutation()
{
    if ( ++m_nativeCaptureMutationGeneration == 0 )
        ++m_nativeCaptureMutationGeneration;

    // A capture callback can run inside wxEVT_SET_CURSOR/WM_SETCURSOR. The
    // in-flight resolution owns the point and will publish its freshly
    // validated verdict after the callback; reasserting the previous
    // surface here would advance the selection generation and incorrectly
    // make that stale cursor look like a newer nested winner.
    if ( m_cursorResolutionDepth )
        return;

    // USER32 capture transitions can cause WinUI to re-evaluate the island
    // cursor. Reapply the already-resolved value without invoking a native
    // WM_SETCURSOR callback in the middle of the capture transaction.
    if ( m_islandPointerCursorApplied )
        (void)SetIslandPointerCursor(m_lastIslandPointerCursor,
                                     m_lastIslandPointerHandle);
}

void wxWinUITopLevelHost::PostIslandCancelMode(
    std::uint32_t expectedGeneration)
{
    // A native modal loop pumps arbitrary application and XAML callbacks.
    // Never let its eventual cleanup cancel a newer physical press, a live
    // slot gesture or a later modal request.
    if ( m_shuttingDown ||
         expectedGeneration != m_inputSiteGeneration )
    {
        if ( m_islandCancelDeferred &&
             m_deferredIslandCancelGeneration == expectedGeneration )
        {
            m_islandCancelDeferred = false;
            m_deferredIslandCancelGeneration = 0;
        }
        return;
    }

    if ( !CanPostIslandCancelMode(expectedGeneration) )
    {
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
        return;
    }

    if ( m_islandCancelDeferred &&
         m_deferredIslandCancelGeneration == expectedGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
    }

    // Coalesce repeated terminal paths for the same modal generation while
    // the two distinct host-window resets are still in the USER32 queue.
    if ( m_postedIslandCancelMessages &&
         m_postedIslandCancelGeneration == expectedGeneration )
    {
        return;
    }

    const UINT message = wxWinUIInternalCancelMessage();
    if ( !message )
    {
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
        return;
    }

    // The subclass translates this private registered message to
    // WM_CANCELMODE only below itself. It therefore reaches the XAML input
    // site without being mistaken for an external USER32 interruption by
    // BridgeSubclassProc.
    unsigned posted = 0;
    if ( m_inner && ::IsWindow(m_inner) )
    {
        if ( ::PostMessage(
                 m_inner, message,
                 static_cast<WPARAM>(expectedGeneration), 0) )
        {
            ++posted;
        }
    }
    if ( m_bridge && m_bridge != m_inner && ::IsWindow(m_bridge) )
    {
        if ( ::PostMessage(
                 m_bridge, message,
                 static_cast<WPARAM>(expectedGeneration), 0) )
        {
            ++posted;
        }
    }

    if ( posted )
    {
        m_postedIslandCancelGeneration = expectedGeneration;
        m_postedIslandCancelMessages = posted;
    }
    else
    {
        // A transient teardown/recreation of the inner window can make both
        // posts fail. Keep the obligation retryable until the host advances
        // to a newer physical press or another terminal path becomes idle.
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
    }
}

static wxWindow *wxWinUIResolveCurrentNativeFocusWindow()
{
    for ( HWND hwnd = ::GetFocus(); hwnd; hwnd = ::GetParent(hwnd) )
    {
        if ( wxWindow * const window = wxFindWinFromHandle(hwnd) )
            return window;
    }

    return nullptr;
}

void wxWinUITopLevelHost::FocusSlot(wxWindow *window)
{
    wxWindow *previous =
        ResolveFocusHwnd(reinterpret_cast<WXHWND>(::GetFocus()));
    if ( !previous )
        previous = wxWinUIResolveCurrentNativeFocusWindow();
    FocusSlotImpl(window, FocusOrigin::LogicalRequest, previous);
}

void wxWinUITopLevelHost::HandleNativeShellFocus(wxWindow *window,
                                                  wxWindow *previous)
{
    if ( m_shuttingDown || !window || !FindSlot(window) )
        return;

    FocusSlotImpl(window, FocusOrigin::WxSetAlreadyDelivered, previous);
}

bool wxWinUITopLevelHost::ConsumeNativeFocusRollback(wxWindow *window)
{
    if ( !m_nativeFocusRollbackEpoch )
        return false;

    const auto rollback = m_nativeFocusRollbackTarget.lock();
    wxWinUISlot * const slot = FindSlot(window);
    const HWND shell = GetHwndOf(window);
    const bool consume =
        !m_shuttingDown && window && slot && rollback &&
        slot->m_lifetime == rollback &&
        rollback->GetHost() == this &&
        rollback->GetWindow() == window &&
        shell && ::GetFocus() == shell &&
        reinterpret_cast<WXHWND>(shell) ==
            m_nativeFocusRollbackShell &&
        wxWinUIMSWGetHwndGeneration(
            window, reinterpret_cast<WXHWND>(shell)) ==
            m_nativeFocusRollbackShellGeneration;

    // Every incoming shell focus supersedes this one-shot marker. A mismatch
    // is not allowed to survive and consume a later recycled HWND.
    m_nativeFocusRollbackTarget.reset();
    m_nativeFocusRollbackShell = nullptr;
    m_nativeFocusRollbackShellGeneration = 0;
    m_nativeFocusRollbackEpoch = 0;
    return consume;
}

bool wxWinUITopLevelHost::ConsumeMigrationShellFocus(wxWindow *window)
{
    const auto pending = ms_focusMigrations.find(window);
    const std::shared_ptr<FocusMigration> migration =
        pending != ms_focusMigrations.end() ? pending->second : nullptr;
    const HWND shell = window ? GetHwndOf(window) : nullptr;
    if ( !migration || migration->committed ||
         migration->sourceHost != m_hostLifetime ||
         m_focusMigration != migration ||
         !migration->shellFocusParking ||
         migration->shellFocusConsumed ||
         migration->window.get() != window ||
         migration->sourceSlot == nullptr ||
         migration->sourceSlot->GetHost() != this ||
         migration->sourceSlot->GetWindow() != window ||
         FindSlot(window) == nullptr ||
         FindSlot(window)->m_lifetime != migration->sourceSlot ||
         shell != static_cast<HWND>(migration->hwnd) ||
         ::GetFocus() != shell ||
         wxWinUIMSWGetHwndGeneration(
             window, reinterpret_cast<WXHWND>(shell)) !=
             migration->hwndGeneration )
    {
        return false;
    }

    migration->shellFocusConsumed = true;
    migration->shellFocusParking = false;
    return true;
}

bool wxWinUITopLevelHost::HasFocusMigrationNativeAuthority(
    const std::shared_ptr<FocusMigration>& migration,
    wxWinUITopLevelHost *destination)
{
    if ( !migration || migration->committed )
        return false;

    wxWindow * const window = migration->window.get();
    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    if ( source &&
         source->m_focusMigration == migration &&
         source->m_focusOwner == window &&
         source->HasNativeFocusAuthority() )
    {
        return true;
    }

    if ( destination &&
         migration->destinationHost == destination->m_hostLifetime &&
         destination->HasNativeFocusAuthority() &&
         (!destination->m_focusOwner ||
          destination->m_focusOwner == window) )
    {
        return true;
    }

    const HWND expected =
        static_cast<HWND>(migration->transportNativeAuthority);
    return expected &&
           migration->transportNativeAuthorityGeneration &&
           ::GetFocus() == expected &&
           wxWinUIMSWGetNativeHwndGeneration(
               reinterpret_cast<WXHWND>(expected)) ==
               migration->transportNativeAuthorityGeneration;
}

bool wxWinUITopLevelHost::NoteMigrationShellFocusDeparture(
    wxWindow *window,
    WXHWND destination)
{
    const auto pending = ms_focusMigrations.find(window);
    const std::shared_ptr<FocusMigration> migration =
        pending != ms_focusMigrations.end() ? pending->second : nullptr;
    if ( !window || !migration || migration->committed ||
         !migration->logicalFocusWasActive ||
         !migration->shellFocusConsumed ||
         migration->shellFocusParking ||
         migration->window.get() != window ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         ::GetFocus() == static_cast<HWND>(migration->hwnd) )
    {
        return false;
    }

    // SetParent() may synchronously activate the exact destination TLW before
    // MigrateSlotToCurrentTLW() has had a chance to create/find its island.
    // This is still the same implementation transport, not a user focus
    // decision. Carry its generation-bound native authority forward and
    // suppress the shell's public KILL_FOCUS; an arbitrary HWND outside this
    // prepared TLW still takes the ordinary terminal-abort path below.
    if ( migration->preparedReparentId &&
         migration->preparedNativeReparentBoundaryActive &&
         destination )
    {
        wxWindow * const sourceTLW =
            migration->preparedSourceTLW.get();
        wxWindow * const destinationTLW =
            migration->preparedDestinationTLW.get();
        const HWND sourceTLWHwnd =
            static_cast<HWND>(
                migration->preparedSourceTLWHwnd);
        const HWND destinationTLWHwnd =
            static_cast<HWND>(
                migration->preparedDestinationTLWHwnd);
        const HWND destinationHwnd =
            static_cast<HWND>(destination);
        const unsigned long long destinationGeneration =
            wxWinUIMSWGetNativeHwndGeneration(destination);
        const auto belongsToPreparedTLW =
            [destinationHwnd](
                wxWindow *tlw,
                WXHWND expectedHwnd,
                unsigned long long expectedGeneration)
            {
                const HWND hwnd = static_cast<HWND>(expectedHwnd);
                return tlw && !tlw->IsBeingDeleted() &&
                       tlw->GetHWND() == expectedHwnd &&
                       wxWinUIMSWGetHwndGeneration(
                           tlw, expectedHwnd) ==
                           expectedGeneration &&
                       hwnd && destinationHwnd &&
                       (destinationHwnd == hwnd ||
                        ::IsChild(hwnd, destinationHwnd));
            };
        const bool belongsToSource =
            belongsToPreparedTLW(
                sourceTLW,
                migration->preparedSourceTLWHwnd,
                migration->preparedSourceTLWHwndGeneration);
        const bool belongsToDestination =
            belongsToPreparedTLW(
                destinationTLW,
                migration->preparedDestinationTLWHwnd,
                migration->
                    preparedDestinationTLWHwndGeneration);
        if ( wxWinUIInputLogEnabled() )
        {
            wxWinUIInputLog(
                "FocusMigration SetParent departure token=%llu "
                "focus=%p root=%p source=%p target=%p "
                "sourceMatch=%d targetMatch=%d targetLive=%d "
                "targetGeneration=%llu/%llu",
                migration->preparedReparentId,
                static_cast<void *>(destinationHwnd),
                static_cast<void *>(
                    destinationHwnd
                        ? ::GetAncestor(destinationHwnd, GA_ROOT)
                        : nullptr),
                static_cast<void *>(sourceTLWHwnd),
                static_cast<void *>(destinationTLWHwnd),
                belongsToSource,
                belongsToDestination,
                destinationTLW &&
                    !destinationTLW->IsBeingDeleted() &&
                    destinationTLW->GetHWND() ==
                        migration->preparedDestinationTLWHwnd,
                destinationTLW
                    ? wxWinUIMSWGetHwndGeneration(
                          destinationTLW,
                          migration->preparedDestinationTLWHwnd)
                    : 0,
                migration->preparedDestinationTLWHwndGeneration);
        }
        if ( (belongsToSource || belongsToDestination) &&
             destinationGeneration &&
             ::GetFocus() == destinationHwnd )
        {
            migration->transportNativeAuthority = destination;
            migration->transportNativeAuthorityGeneration =
                destinationGeneration;
            if ( wxWinUIInputLogEnabled() )
            {
                wxWinUIInputLog(
                    "FocusMigration carried SetParent authority "
                    "token=%llu focus=%p target=%p",
                    migration->preparedReparentId,
                    static_cast<void *>(destinationHwnd),
                    static_cast<void *>(destinationTLWHwnd));
            }
            return true;
        }
    }

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const destinationHost =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const arbiter =
        source ? source : destinationHost;
    if ( !arbiter ||
         (source && source->m_focusMigration != migration) )
    {
        return false;
    }

    // HandleKillFocus() is about to publish the one real wx KILL_FOCUS for
    // this shell departure. Retire all migration state without dispatching a
    // second event; AbortFocusMigration() clears the old logical owner when
    // this marker is present.
    migration->shellLogicalLossDelivered = true;
    arbiter->AbortFocusMigration(migration, false);
    return false;
}

void wxWinUITopLevelHost::ClearDeferredFocusRequest(
    unsigned long long expectedGeneration)
{
    if ( expectedGeneration &&
         m_deferredFocusRequest.intentGeneration != expectedGeneration )
    {
        if ( m_deferredFocusFlushRetryGeneration == expectedGeneration )
            m_deferredFocusFlushRetryGeneration = 0;
        return;
    }

    const FocusRequest retired = m_deferredFocusRequest;
    m_deferredFocusRequest = FocusRequest();
    m_deferredFocusContentGeneration = 0;
    if ( !expectedGeneration ||
         m_deferredFocusFlushRetryGeneration == expectedGeneration )
    {
        m_deferredFocusFlushRetryGeneration = 0;
    }

    const std::shared_ptr<FocusMigration> migration =
        m_focusMigration;
    if ( migration && !migration->committed &&
         retired.origin == FocusOrigin::MigrationContinuation &&
         retired.target &&
         retired.target == migration->destinationSlot )
    {
        if ( wxWinUIInputLogEnabled() )
        {
            wxWinUIInputLog(
                "FocusMigration deferred continuation cleared "
                "expected=%llu retired=%llu current=%llu",
                expectedGeneration,
                retired.intentGeneration,
                m_focusIntentGeneration);
        }
        AbortFocusMigration(migration, true);
    }
}

bool wxWinUITopLevelHost::IsSuppressedMigrationLoss(
    const std::shared_ptr<wxWinUISlotLifetime>& source)
{
    const std::shared_ptr<FocusMigration> migration =
        m_focusMigration;
    if ( !migration || migration->committed ||
         !migration->logicalFocusWasActive ||
         migration->sourceHost != m_hostLifetime ||
         migration->sourceSlot != source ||
         !source || source->GetHost() != this ||
         m_focusOwner != source->GetWindow() )
    {
        return false;
    }

    wxWindow * const window = migration->window.get();
    if ( !window || window != source->GetWindow() ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        AbortFocusMigration(migration, false);
        return false;
    }

    // A real navigation to another source slot wins over the migration
    // suspension. During the expected detach/bridge handoff native authority
    // belongs either to this host or to the ticketed destination host.
    if ( wxWindow * const focused = FindFocusedSlot() )
    {
        if ( focused != window )
        {
            AbortFocusMigration(migration, false);
            return false;
        }
    }

    wxWinUITopLevelHost * const destination =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    const bool ownsTransportAuthority =
        (migration->shellFocusParking ||
         migration->shellFocusConsumed) &&
        HasFocusMigrationNativeAuthority(
            migration, destination);
    if ( !HasNativeFocusAuthority() &&
         !ownsTransportAuthority &&
         (!destination || !destination->HasNativeFocusAuthority()) )
    {
        AbortFocusMigration(migration, false);
        return false;
    }

    migration->sourceLostFocusObserved = true;
    return true;
}

bool wxWinUITopLevelHost::CommitFocusMigration(
    const std::shared_ptr<FocusMigration>& migration,
    const std::shared_ptr<wxWinUISlotLifetime>& destination)
{
    if ( !migration || migration->committed ||
         m_focusMigration != migration ||
         migration->destinationHost != m_hostLifetime ||
         migration->destinationSlot != destination ||
         !destination || destination->GetHost() != this ||
         !HasNativeFocusAuthority() )
    {
        return false;
    }

    wxWindow *window = migration->window.get();
    wxWinUISlot *destinationSlot =
        window ? FindSlot(window) : nullptr;
    if ( !window || destination->GetWindow() != window ||
         !destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration )
    {
        return false;
    }

    const auto destinationContainer = destinationSlot->m_container;
    const auto xamlRoot = GetXamlRoot();
    if ( !wxWinUIFocusIsInside(destinationContainer, xamlRoot) )
        return false;

    // FocusManager/XamlRoot reads are arbitrary callback boundaries. Reacquire
    // every weak identity and generation before publishing either host owner;
    // the focused visual may have been destroyed, replaced or moved again.
    window = migration->window.get();
    destinationSlot = window ? FindSlot(window) : nullptr;
    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    const auto pendingTicket = window
        ? ms_focusMigrations.find(window)
        : ms_focusMigrations.end();
    if ( migration->committed ||
         m_focusMigration != migration ||
         migration->destinationHost != m_hostLifetime ||
         migration->destinationSlot != destination ||
         !destination || destination->GetHost() != this ||
         !window || destination->GetWindow() != window ||
         !destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_container != destinationContainer ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         pendingTicket == ms_focusMigrations.end() ||
         pendingTicket->second != migration ||
         !HasNativeFocusAuthority() ||
         !source ||
         source->m_focusMigration != migration ||
         source->m_focusOwner != window ||
         source->m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        return false;
    }

    OperationGuard sourceOperation(source);
    const auto pendingAfterGuard =
        ms_focusMigrations.find(window);
    if ( migration->sourceHost->GetHost() != source ||
         migration->window.get() != window ||
         m_focusMigration != migration ||
         migration->destinationSlot != destination ||
         destination->GetHost() != this ||
         destination->GetWindow() != window ||
         FindSlot(window) != destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_container != destinationContainer ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         pendingAfterGuard == ms_focusMigrations.end() ||
         pendingAfterGuard->second != migration ||
         !HasNativeFocusAuthority() ||
         source->m_focusMigration != migration ||
         source->m_focusOwner != window ||
         source->m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        return false;
    }

    // A rollback restores the original carrier and resumes focus through the
    // same continuation path. Its source and destination are deliberately
    // identical: no logical ownership changes and therefore no wx focus
    // event is published.
    if ( source == this )
    {
        if ( destination != migration->sourceSlot )
            return false;

        migration->committedSuccessfully = true;
        migration->committed = true;
        m_focusMigration.reset();
        const auto rollbackPending =
            ms_focusMigrations.find(window);
        if ( rollbackPending != ms_focusMigrations.end() &&
             rollbackPending->second == migration )
        {
            ms_focusMigrations.erase(rollbackPending);
        }
        return true;
    }

    // Commit both logical publications before any cleanup can re-enter focus.
    migration->committedSuccessfully = true;
    migration->committed = true;
    source->m_focusOwner = nullptr;
    m_focusOwner = window;
    source->m_focusMigration.reset();
    m_focusMigration.reset();
    const auto committedPending =
        ms_focusMigrations.find(window);
    if ( committedPending != ms_focusMigrations.end() &&
         committedPending->second == migration )
    {
        ms_focusMigrations.erase(committedPending);
    }
    source->InvalidateFocusIntent(
        migration->sourceFocusIntentGeneration);
    return true;
}

void wxWinUITopLevelHost::AbortFocusMigration(
    const std::shared_ptr<FocusMigration>& migration,
    bool dispatchLogicalLoss,
    wxWindow *replacement)
{
    if ( !migration || migration->committed )
        return;

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog(
            "FocusMigration abort dispatch=%d replacement=%p "
            "shellLoss=%d focus=%p shell=%p",
            dispatchLogicalLoss,
            replacement,
            migration->shellLogicalLossDelivered,
            static_cast<void *>(::GetFocus()),
            static_cast<void *>(static_cast<HWND>(migration->hwnd)));
    }

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const destination =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    wxWindow * const window = migration->window.get();
    wxWindow * const windowIdentity = migration->windowIdentity;

    const bool sourceOwnsTicket =
        source && source->m_focusMigration == migration;
    const bool sourceWasLogicalOwner =
        sourceOwnsTicket && windowIdentity &&
        source->m_focusOwner == windowIdentity &&
        source->m_focusIntentGeneration ==
            migration->sourceFocusIntentGeneration;
    const HWND shell = static_cast<HWND>(migration->hwnd);
    const bool exactShellOwnsNativeFocus =
        dispatchLogicalLoss && !replacement &&
        !migration->shellLogicalLossDelivered &&
        sourceWasLogicalOwner && window && shell &&
        ::GetFocus() == shell &&
        window->GetHWND() == migration->hwnd &&
        wxWinUIMSWGetHwndGeneration(
            window, migration->hwnd) ==
            migration->hwndGeneration;
    const bool migrationIslandOwnsNativeFocus =
        dispatchLogicalLoss && !replacement &&
        !migration->shellLogicalLossDelivered &&
        sourceWasLogicalOwner && !exactShellOwnsNativeFocus &&
        ((destination && destination->HasNativeFocusAuthority() &&
          (!destination->m_focusOwner ||
           destination->m_focusOwner == window)) ||
         (source && source->HasNativeFocusAuthority() &&
          (!source->m_focusOwner ||
           source->m_focusOwner == window)));

    migration->committed = true;
    if ( sourceOwnsTicket )
        source->m_focusMigration.reset();
    if ( destination &&
         destination->m_focusMigration == migration )
    {
        destination->m_focusMigration.reset();
    }

    // The weak wxWindow can already be dead after a synchronous Destroy().
    // Remove by ticket identity, not by the map key derived from that weak
    // reference, or the ticket would retain both host and slot lifetimes.
    for ( auto pending = ms_focusMigrations.begin();
          pending != ms_focusMigrations.end(); )
    {
        if ( pending->second == migration )
            pending = ms_focusMigrations.erase(pending);
        else
            ++pending;
    }

    if ( migration->shellLogicalLossDelivered )
    {
        // HandleKillFocus() owns the event currently in flight. Publish the
        // host state it must observe, without synthesizing a duplicate KILL.
        if ( sourceWasLogicalOwner &&
             source->m_focusOwner == windowIdentity )
        {
            source->m_focusOwner = nullptr;
            source->InvalidateFocusIntent();
        }
        return;
    }

    if ( exactShellOwnsNativeFocus )
    {
        // Let the real shell WM_KILLFOCUS publish the sole wx event. The
        // ticket is already terminal so the hook cannot recurse into us.
        // Publish the truthful null owner before the event, matching
        // TransitionFocus()'s contract for FindFocus() in KILL handlers.
        if ( source->m_focusOwner == window )
        {
            source->m_focusOwner = nullptr;
            source->InvalidateFocusIntent();
        }
        ::SetFocus(nullptr);
        wxWinUITopLevelHost * const liveSource =
            migration->sourceHost
                ? migration->sourceHost->GetHost()
                : nullptr;
        wxWindow * const liveWindow = migration->window.get();
        if ( ::GetFocus() == shell && liveSource && liveWindow &&
             !liveSource->m_focusOwner )
        {
            // USER32 refused the relinquish and no KILL was sent. Keep native
            // and logical truth aligned; a later explicit request can start a
            // fresh transaction.
            liveSource->m_focusOwner = liveWindow;
        }
        return;
    }

    if ( !window && sourceWasLogicalOwner &&
         migration->sourceHost &&
         migration->sourceHost->GetHost() == source &&
         source->m_focusOwner == windowIdentity )
    {
        // No wx event may target a destroyed object, but the host must still
        // release the exact raw identity it suspended for this ticket.
        source->m_focusOwner = nullptr;
        source->InvalidateFocusIntent();
        return;
    }

    if ( migrationIslandOwnsNativeFocus )
    {
        // A failed continuation must not leave the ownerless bridge focused:
        // FindFocus()/Tab would otherwise report the migrated shell after its
        // logical KILL. Bridge/InputSite HWNDs have no wxWindowMSW handler, so
        // the logical event is still emitted below.
        ::SetFocus(nullptr);
    }

    if ( dispatchLogicalLoss && sourceWasLogicalOwner &&
         migration->sourceHost &&
         migration->sourceHost->GetHost() == source &&
         migration->window.get() == window &&
         source->m_focusOwner == window )
    {
        source->TransitionFocus(
            nullptr,
            window,
            false,
            replacement
                ? replacement
                : wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::NotifySlotContentLoaded(wxWindow *window)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    NotifySlotContentLoaded(
        window, slot->m_content, slot->m_contentGeneration);
}

void wxWinUITopLevelHost::NotifySlotContentLoaded(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& expectedContent,
    unsigned long long expectedGeneration)
{
    OperationGuard operation(this);

    wxWinUISlot * const loadedSlot = FindSlot(window);
    if ( !loadedSlot ||
         loadedSlot->m_content != expectedContent ||
         (expectedGeneration &&
          loadedSlot->m_contentGeneration != expectedGeneration) )
    {
        return;
    }

    // A callback for model A can run after the carrier was detached or even
    // after B was attached, while SetContent() deliberately still publishes
    // A. Record the exact transaction/generation only. Commit discards this
    // latch; rollback may apply it to the restored A generation.
    if ( loadedSlot->m_contentTransactionInProgress )
    {
        loadedSlot->m_contentLoadedDuringTransaction = true;
        loadedSlot->m_contentLoadedTransactionEpoch =
            loadedSlot->m_contentTransactionEpoch;
        loadedSlot->m_contentLoadedTransactionGeneration =
            loadedSlot->m_contentGeneration;
        return;
    }

    loadedSlot->m_contentLoaded = true;

    // UpdateLayout() in an already-running bounded retry may synchronously
    // raise Loaded. The latch above is all that nested callback must do; the
    // outer retry will revalidate and finish the decision.
    if ( m_deferredFocusRetryInProgress )
        return;

    const FocusRequest retry = m_deferredFocusRequest;
    if ( m_shuttingDown || !retry.target ||
         retry.intentGeneration != m_focusIntentGeneration )
    {
        ClearDeferredFocusRequest();
        return;
    }

    // A different slot loading must not consume the pending target.
    if ( retry.target->GetHost() != this ||
         retry.target->GetWindow() != window )
    {
        return;
    }

    if ( loadedSlot->m_lifetime != retry.target ||
         loadedSlot->m_contentGeneration != m_deferredFocusContentGeneration )
    {
        ClearDeferredFocusRequest(retry.intentGeneration);
        return;
    }

    // Loaded before the coalesced flush is only a latch: the unique flush
    // retry is still the next opportunity. This also prevents repeated
    // unload/reload notifications from multiplying attempts for one intent.
    // Loaded performs its one final retry only when the flush edge has
    // already been consumed.
    if ( m_deferredFocusFlushRetryGeneration == retry.intentGeneration )
        return;

    // Focus() is a synchronous arbitrary-callback boundary. Keep the exact
    // request until success and suppress a nested Loaded retry while this one
    // is on the stack.
    m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryGuard = wxMakeGuard([this]()
    {
        m_deferredFocusRetryInProgress = false;
    });
    wxUnusedVar(retryGuard);
    FocusSlotImpl(window, retry.origin, retry.previous.get(),
                  retry.intentGeneration, false);

    // If the flush retry was already consumed, this Loaded callback was the
    // final bounded opportunity. Release the retained slot identity on
    // failure; a future explicit focus request starts a fresh generation.
    if ( m_deferredFocusRequest.intentGeneration == retry.intentGeneration &&
         m_deferredFocusFlushRetryGeneration != retry.intentGeneration )
    {
        ClearDeferredFocusRequest(retry.intentGeneration);
    }
}

void wxWinUITopLevelHost::RetryDeferredFocusAfterFlush()
{
    const unsigned long long intentGeneration =
        m_deferredFocusFlushRetryGeneration;
    if ( !intentGeneration )
        return;

    if ( wxWinUIInputLogEnabled() &&
         m_deferredFocusRequest.origin ==
             FocusOrigin::MigrationContinuation )
    {
        wxWinUIInputLog(
            "FocusMigration retry after flush intent=%llu "
            "request=%llu content=%llu",
            intentGeneration,
            m_deferredFocusRequest.intentGeneration,
            m_deferredFocusContentGeneration);
    }

    // Consume before the callback boundary. This pass is unique even if
    // Focus()/UpdateLayout() pumps another host flush.
    m_deferredFocusFlushRetryGeneration = 0;

    const FocusRequest retry = m_deferredFocusRequest;
    if ( m_shuttingDown || !retry.target ||
         retry.intentGeneration != intentGeneration ||
         intentGeneration != m_focusIntentGeneration )
    {
        ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    wxWindow * const window = retry.target->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( retry.target->GetHost() != this || !slot ||
         slot->m_lifetime != retry.target ||
         slot->m_contentGeneration != m_deferredFocusContentGeneration )
    {
        ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryGuard = wxMakeGuard([this]()
    {
        m_deferredFocusRetryInProgress = false;
    });
    wxUnusedVar(retryGuard);
    FocusSlotImpl(window, retry.origin, retry.previous.get(),
                  intentGeneration, false);

    // A Loaded notification seen before or during this flush means there is
    // no later lifecycle edge to wait for. If focus still failed, terminate
    // the bounded retry now. Otherwise FocusSlotImpl() already cleared it.
    if ( m_deferredFocusRequest.intentGeneration == intentGeneration )
    {
        wxWinUISlot * const current = FindSlot(window);
        if ( current && current->m_lifetime == retry.target &&
             current->m_contentGeneration ==
                 m_deferredFocusContentGeneration &&
             current->m_contentLoaded )
        {
            ClearDeferredFocusRequest(intentGeneration);
        }
    }
}

void wxWinUITopLevelHost::FocusSlotImpl(wxWindow *window,
                                        FocusOrigin origin,
                                        wxWindow *previous,
                                        unsigned long long intentGeneration,
                                        bool allowDeferredRetry)
{
    OperationGuard operation(this);

    // Shutdown publishes the host boundary before restoring any XAML-owned
    // properties. Those restorations can run application code, which must not
    // re-enter virtual focus predicates on a window/TLW being destroyed.
    if ( m_shuttingDown || !window )
        return;

    const wxWeakRef<wxWindow> targetAtEntry(window);
    if ( m_focusMigration && !m_focusMigration->committed &&
         origin != FocusOrigin::MigrationContinuation )
    {
        const std::shared_ptr<FocusMigration> incumbent =
            m_focusMigration;
        if ( incumbent->window.get() == window )
        {
            // The same logical control is already crossing hosts. Its exact
            // continuation owns the single native transfer; another public
            // SetFocus() is satisfied by that pending operation.
            return;
        }

        AbortFocusMigration(incumbent, true);
        window = targetAtEntry.get();
        if ( m_shuttingDown || !window )
            return;
    }

    const std::shared_ptr<FocusMigration> expectedMigration =
        origin == FocusOrigin::MigrationContinuation
            ? m_focusMigration
            : nullptr;
    if ( origin == FocusOrigin::MigrationContinuation &&
         (!expectedMigration || expectedMigration->committed ||
          expectedMigration->window.get() != window ||
          expectedMigration->destinationHost != m_hostLifetime) )
    {
        return;
    }
    wxScopeGuard migrationContinuationGuard = wxMakeGuard(
        [this, expectedMigration]()
        {
            if ( !expectedMigration || expectedMigration->committed )
                return;

            wxWindow * const target =
                expectedMigration->window.get();
            wxWinUITopLevelHost * const source =
                expectedMigration->sourceHost
                    ? expectedMigration->sourceHost->GetHost()
                    : nullptr;
            const auto pending =
                target ? ms_focusMigrations.find(target)
                       : ms_focusMigrations.end();
            const FocusRequest& deferred =
                m_deferredFocusRequest;
            const bool exactDeferredContinuation =
                target && source &&
                m_focusMigration == expectedMigration &&
                source->m_focusMigration == expectedMigration &&
                source->m_focusOwner == target &&
                source->m_focusIntentGeneration ==
                    expectedMigration->sourceFocusIntentGeneration &&
                expectedMigration->destinationHost == m_hostLifetime &&
                expectedMigration->destinationSlot &&
                expectedMigration->destinationSlot->GetHost() == this &&
                expectedMigration->destinationSlot->GetWindow() == target &&
                deferred.origin == FocusOrigin::MigrationContinuation &&
                deferred.target == expectedMigration->destinationSlot &&
                deferred.intentGeneration == m_focusIntentGeneration &&
                m_deferredFocusContentGeneration ==
                    expectedMigration->destinationContentGeneration &&
                pending != ms_focusMigrations.end() &&
                pending->second == expectedMigration &&
                target->GetHWND() == expectedMigration->hwnd &&
                wxWinUIMSWGetHwndGeneration(
                    target, expectedMigration->hwnd) ==
                    expectedMigration->hwndGeneration &&
                expectedMigration->shellFocusConsumed &&
                !expectedMigration->shellFocusParking &&
                HasFocusMigrationNativeAuthority(
                    expectedMigration, this);
            if ( !exactDeferredContinuation )
            {
                if ( wxWinUIInputLogEnabled() )
                {
                    wxWinUIInputLog(
                        "FocusMigration continuation guard abort "
                        "target=%p source=%p local=%d sourceTicket=%d "
                        "sourceOwner=%d sourceIntent=%llu/%llu "
                        "destination=%d slot=%d deferred=%d "
                        "deferredIntent=%llu/%llu generation=%llu/%llu "
                        "pending=%d shell=%d focus=%p expected=%p",
                        target,
                        source,
                        m_focusMigration == expectedMigration,
                        source &&
                            source->m_focusMigration == expectedMigration,
                        source && source->m_focusOwner == target,
                        source ? source->m_focusIntentGeneration : 0,
                        expectedMigration->sourceFocusIntentGeneration,
                        expectedMigration->destinationHost ==
                            m_hostLifetime,
                        expectedMigration->destinationSlot &&
                            expectedMigration->destinationSlot->GetHost() ==
                                this,
                        deferred.origin ==
                                FocusOrigin::MigrationContinuation &&
                            deferred.target ==
                                expectedMigration->destinationSlot,
                        deferred.intentGeneration,
                        m_focusIntentGeneration,
                        m_deferredFocusContentGeneration,
                        expectedMigration->destinationContentGeneration,
                        pending != ms_focusMigrations.end() &&
                            pending->second == expectedMigration,
                        expectedMigration->shellFocusConsumed &&
                            !expectedMigration->shellFocusParking,
                        static_cast<void *>(::GetFocus()),
                        static_cast<void *>(
                            static_cast<HWND>(expectedMigration->hwnd)));
                }
                AbortFocusMigration(expectedMigration, true);
            }
        });
    wxUnusedVar(migrationContinuationGuard);

    // The target's virtual focus predicates and every native/XAML focus call
    // below may synchronously destroy the previous owner. Convert the raw
    // entry hint before crossing the first such boundary.
    const wxWeakRef<wxWindow> previousWeak(previous);

    if ( !intentGeneration )
    {
        if ( ++m_focusIntentGeneration == 0 )
            ++m_focusIntentGeneration;
        intentGeneration = m_focusIntentGeneration;
        ClearDeferredFocusRequest();
    }
    else if ( intentGeneration != m_focusIntentGeneration )
    {
        return;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // Virtual focus predicates and labels are application re-entrancy
    // boundaries too. Snapshot the exact slot before the first one and reject
    // the outer intent after every call: a nested SetFocus() owns its newer
    // generation and its deferred retry must never be overwritten by this
    // older stack frame.
    const std::shared_ptr<wxWinUISlotLifetime> lifetime = slot->m_lifetime;
    const unsigned long long contentGeneration = slot->m_contentGeneration;
    const wxWeakRef<wxWindow> targetWeak(window);
    bool bridgeAcquiredForAttempt = false;
    const auto migrationAuthorityIsCurrent = [&]()
    {
        if ( !expectedMigration )
            return true;
        if ( expectedMigration->committed )
            return expectedMigration->committedSuccessfully;

        wxWindow * const target = targetWeak.get();
        wxWinUITopLevelHost * const source =
            expectedMigration->sourceHost
                ? expectedMigration->sourceHost->GetHost()
                : nullptr;
        const auto pending =
            target ? ms_focusMigrations.find(target)
                   : ms_focusMigrations.end();
        if ( !target || !source ||
             m_focusMigration != expectedMigration ||
             source->m_focusMigration != expectedMigration ||
             source->m_focusOwner != target ||
             source->m_focusIntentGeneration !=
                 expectedMigration->sourceFocusIntentGeneration ||
             expectedMigration->destinationHost != m_hostLifetime ||
             expectedMigration->destinationSlot != lifetime ||
             expectedMigration->destinationContentGeneration !=
                 contentGeneration ||
             pending == ms_focusMigrations.end() ||
             pending->second != expectedMigration ||
             target->GetHWND() != expectedMigration->hwnd ||
             wxWinUIMSWGetHwndGeneration(
                 target, expectedMigration->hwnd) !=
                 expectedMigration->hwndGeneration )
        {
            return false;
        }

        if ( bridgeAcquiredForAttempt )
            return HasNativeFocusAuthority();

        return expectedMigration->shellFocusConsumed &&
               !expectedMigration->shellFocusParking &&
               HasFocusMigrationNativeAuthority(
                   expectedMigration, this);
    };
    const auto getInitialLiveSlot = [&]() -> wxWinUISlot *
    {
        if ( intentGeneration != m_focusIntentGeneration ||
             !lifetime || lifetime->GetHost() != this ||
             !migrationAuthorityIsCurrent() )
        {
            return nullptr;
        }

        wxWindow * const target = targetWeak.get();
        if ( !target || lifetime->GetWindow() != target )
            return nullptr;

        wxWinUISlot * const current = FindSlot(target);
        return current &&
                       current->m_lifetime == lifetime &&
                       current->m_contentGeneration == contentGeneration
                 ? current
                 : nullptr;
    };

    const bool canAcceptFocus = window->CanAcceptFocus();
    window = targetWeak.get();
    if ( !canAcceptFocus || !window || !getInitialLiveSlot() )
        return;

    const wxString label = window->GetLabel();
    window = targetWeak.get();
    if ( !window || !getInitialLiveSlot() )
        return;

    // ::SetFocus() is a reentrant boundary: it synchronously dispatches
    // WM_KILLFOCUS/WM_SETFOCUS, whose handlers may destroy, reparent or
    // unregister the very control being focused. Hold the invalidatable
    // lifetime across every call and re-find the slot afterwards.
    const unsigned long long requestId = ++m_nextFocusRequestId;
    FocusRequest request;
    request.id = requestId;
    request.intentGeneration = intentGeneration;
    request.target = lifetime;
    request.previous = previousWeak;
    request.origin = origin;
    const FocusRequest * const retained =
        intentGeneration == m_deferredFocusRequest.intentGeneration
            ? &m_deferredFocusRequest
            : nullptr;
    const HWND nativeAuthority =
        expectedMigration
            ? static_cast<HWND>(
                  expectedMigration->transportNativeAuthority)
            : retained
            ? static_cast<HWND>(retained->nativeAuthority)
            : ::GetFocus();
    request.nativeAuthority =
        reinterpret_cast<WXHWND>(nativeAuthority);
    request.nativeAuthorityGeneration =
        expectedMigration
            ? expectedMigration->
                  transportNativeAuthorityGeneration
            : retained
            ? retained->nativeAuthorityGeneration
            : wxWinUIMSWGetNativeHwndGeneration(
                  reinterpret_cast<WXHWND>(nativeAuthority));
    m_focusRequests.push_back(std::move(request));

    if ( allowDeferredRetry )
    {
        m_deferredFocusRequest.id = requestId;
        m_deferredFocusRequest.intentGeneration = intentGeneration;
        m_deferredFocusRequest.target = lifetime;
        m_deferredFocusRequest.previous = previousWeak;
        m_deferredFocusRequest.origin = origin;
        m_deferredFocusRequest.nativeAuthority =
            reinterpret_cast<WXHWND>(nativeAuthority);
        m_deferredFocusRequest.nativeAuthorityGeneration =
            expectedMigration
                ? expectedMigration->
                      transportNativeAuthorityGeneration
                : wxWinUIMSWGetNativeHwndGeneration(
                      reinterpret_cast<WXHWND>(nativeAuthority));
        m_deferredFocusContentGeneration = contentGeneration;
        // Publish the flush obligation together with the request, before any
        // SetFocus/Focus callback can synchronously raise Loaded. MarkDirty()
        // is delayed until the immediate attempt actually fails.
        m_deferredFocusFlushRetryGeneration = intentGeneration;
    }

    // Loaded can be raised synchronously from any focus/layout boundary. The
    // current attempt owns the decision; a nested Loaded callback only latches
    // the content state and leaves the bounded retry intact.
    const bool retryWasInProgress = m_deferredFocusRetryInProgress;
    if ( allowDeferredRetry )
        m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryAttemptGuard = wxMakeGuard(
        [this, retryWasInProgress]()
        {
            m_deferredFocusRetryInProgress = retryWasInProgress;
        });
    wxUnusedVar(retryAttemptGuard);

    // Nested focus requests are legal from application focus handlers. Remove
    // exactly this request on every exit, even if a nested vector growth
    // invalidated all references or teardown changed the stack shape.
    wxScopeGuard requestGuard = wxMakeGuard([this, requestId]()
    {
        const auto it = std::find_if(
            m_focusRequests.begin(),
            m_focusRequests.end(),
            [requestId](const FocusRequest& candidate)
            {
                return candidate.id == requestId;
            });
        if ( it != m_focusRequests.end() )
            m_focusRequests.erase(it);
    });
    wxUnusedVar(requestGuard);

    const auto getLiveSlot = [&]() -> wxWinUISlot *
    {
        if ( intentGeneration != m_focusIntentGeneration ||
                 !lifetime || lifetime->GetHost() != this ||
                 lifetime->GetWindow() != window ||
                 !migrationAuthorityIsCurrent() )
            return nullptr;

        wxWinUISlot * const live = FindSlot(window);
        return live &&
                       live->m_lifetime == lifetime &&
                       live->m_contentGeneration == contentGeneration
                 ? live
                 : nullptr;
    };
    const auto abandonCurrentIntent = [&]()
    {
        // A nested focus request owns a newer generation and must never be
        // cleared by the outer request whose callback triggered it.
        if ( intentGeneration != m_focusIntentGeneration )
            return;

        InvalidateFocusIntent(intentGeneration);
        if ( m_focusOwner == window )
        {
            TransitionFocus(
                nullptr, nullptr, false,
                wxWinUIResolveCurrentNativeFocusWindow());
        }
    };

    // Only grab the native focus when it is OUTSIDE the island.  When the
    // island already holds it, it sits on the inner InputSiteWindow, and an
    // unconditional ::SetFocus(bridge) would DOWNGRADE it inner -> bridge:
    // the input site loses the focus, XAML fires stray LostFocus on the
    // currently focused element, and the programmatic Focus() below runs
    // against a focus-less input site.
    const HWND focusNow = ::GetFocus();
    const bool inIsland = HasNativeFocusAuthority();
    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("FocusSlot '%s' focusNow=%p inIsland=%d",
                        label.utf8_str().data(),
                        static_cast<void *>(focusNow), inIsland);
    }
    if ( (!allowDeferredRetry ||
          origin == FocusOrigin::WxSetAlreadyDelivered ||
          origin == FocusOrigin::MigrationContinuation) &&
         !inIsland )
    {
        FocusRequest * const active = FindFocusRequest(window);
        const HWND expectedAuthority =
            active
                ? static_cast<HWND>(active->nativeAuthority)
                : nullptr;
        const bool authorityUnchanged =
            active && focusNow == expectedAuthority &&
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(focusNow)) ==
                active->nativeAuthorityGeneration;
        if ( !authorityUnchanged )
        {
            // This one-shot retry was queued while this TLW owned focus, but
            // another TLW gained it before template/layout became ready.
            // Re-activating the stale TLW here makes two hosts' retries steal
            // focus from each other forever. A later explicit focus request
            // gets a new intent generation and may legitimately activate it.
            abandonCurrentIntent();
            return;
        }
    }
    wxWinUISlot *live = getLiveSlot();
    if ( !live )
        return;

    // Strong XAML references are safe across the following layout/focus
    // boundaries; the invalidatable slot lifetime remains the authority for
    // every wx access.
    const auto container = live->m_container;
    const auto content = live->m_content;
    const auto preferred = live->m_preferredFocus.get();

    // A content swap can detach the focused peer without delivering a usable
    // LostFocus notification. Reconcile the old logical owner before even the
    // Loaded gate; keep this request alive so a new focusable template can
    // still acquire focus on the existing bounded retry.
    if ( origin != FocusOrigin::MigrationContinuation &&
         m_focusOwner == window &&
         (!HasNativeFocusAuthority() ||
          !wxWinUIFocusIsInside(container, GetXamlRoot())) )
    {
        TransitionFocus(
            nullptr, window, false,
            wxWinUIResolveCurrentNativeFocusWindow());
        if ( m_shuttingDown ||
             intentGeneration != m_focusIntentGeneration ||
             !getLiveSlot() )
        {
            return;
        }
    }

    // Until the exact content is Loaded there may be no focusable template
    // part. Retain one bounded attempt, but don't move native focus to the
    // bridge and don't publish a logical owner which XAML has never focused.
    // A native shell path has already delivered its wx events and remains the
    // truthful native focus owner; a direct host request simply completes
    // when Loaded/flush makes the peer focusable.
    if ( !live->m_contentLoaded )
    {
        if ( wxWinUIInputLogEnabled() &&
             origin == FocusOrigin::MigrationContinuation )
        {
            wxWinUIInputLog(
                "FocusMigration continuation deferred until Loaded "
                "intent=%llu content=%llu",
                intentGeneration,
                contentGeneration);
        }
        if ( allowDeferredRetry && wxTheApp )
            MarkDirty(window);
        return;
    }

    if ( !allowDeferredRetry )
    {
        // The only deferred attempt runs after the coalesced host flush.
        // Force pending template/layout work before deciding whether there is
        // any target worth transferring native focus to.
        try
        {
            container.UpdateLayout();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        if ( !getLiveSlot() )
            return;
    }

    // Never move a native HWND's focus to the bridge merely to discover that
    // this loaded subtree has no focusable element. On the immediate pass the
    // template may still settle at the coalesced flush, so retain exactly the
    // existing bounded retry; the final pass simply terminates.
    bool hasFocusableCandidate = false;
    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

        if ( preferred && wxWinUIElementIsInSubtree(preferred, content) )
        {
            if ( const auto control = preferred.try_as<MUXC::Control>() )
            {
                hasFocusableCandidate =
                    control.IsEnabled() &&
                    control.Visibility() == MUX::Visibility::Visible &&
                    control.IsTabStop();
            }
        }

        if ( !hasFocusableCandidate )
        {
            hasFocusableCandidate =
                MUX::Input::FocusManager::FindFirstFocusableElement(container)
                    .try_as<MUXC::Control>() != nullptr;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    if ( !getLiveSlot() )
        return;
    if ( !hasFocusableCandidate )
    {
        if ( allowDeferredRetry && wxTheApp )
            MarkDirty(window);
        else
            ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    bool xamlFocusCommitted = false;
    wxScopeGuard nativeAuthorityRollback = wxMakeGuard(
        [&, this]()
        {
            if ( !bridgeAcquiredForAttempt || xamlFocusCommitted ||
                 m_shuttingDown ||
                 intentGeneration != m_focusIntentGeneration ||
                 !HasNativeFocusAuthority() ||
                 (m_focusOwner && m_focusOwner != window) )
            {
                return;
            }

            FocusRequest *active = FindFocusRequest(window);
            if ( !active || active->intentGeneration != intentGeneration )
                return;

            const HWND original =
                static_cast<HWND>(active->nativeAuthority);
            const bool originalIsCurrent =
                original && ::IsWindow(original) &&
                wxWinUIMSWGetNativeHwndGeneration(
                    reinterpret_cast<WXHWND>(original)) ==
                    active->nativeAuthorityGeneration;

            // This attempt made the bridge authoritative but never acquired
            // XAML focus. Restore only the exact authority it displaced. If
            // that HWND died, drop focus rather than leave an ownerless bridge
            // that FindFocus()/Tab would falsely interpret as the TLW.
            if ( originalIsCurrent )
            {
                const unsigned long long originalGeneration =
                    active->nativeAuthorityGeneration;
                m_nativeFocusDepartureSource = lifetime;
                m_nativeFocusDepartureDestination =
                    reinterpret_cast<WXHWND>(original);
                m_nativeFocusDepartureDestinationGeneration =
                    originalGeneration;

                unsigned long long rollbackEpoch = 0;
                wxWindow * const rollbackWindow = lifetime->GetWindow();
                wxWinUISlot * const rollbackSlot =
                    rollbackWindow ? FindSlot(rollbackWindow) : nullptr;
                const HWND rollbackShell =
                    rollbackWindow ? GetHwndOf(rollbackWindow) : nullptr;
                const unsigned long long rollbackShellGeneration =
                    rollbackWindow && rollbackShell
                        ? wxWinUIMSWGetHwndGeneration(
                              rollbackWindow,
                              reinterpret_cast<WXHWND>(rollbackShell))
                        : 0;
                if ( rollbackSlot &&
                     rollbackSlot->m_lifetime == lifetime &&
                     rollbackShell == original &&
                     rollbackShellGeneration )
                {
                    if ( ++m_nextNativeFocusRollbackEpoch == 0 )
                        ++m_nextNativeFocusRollbackEpoch;
                    rollbackEpoch = m_nextNativeFocusRollbackEpoch;
                    m_nativeFocusRollbackTarget = lifetime;
                    m_nativeFocusRollbackShell =
                        reinterpret_cast<WXHWND>(rollbackShell);
                    m_nativeFocusRollbackShellGeneration =
                        rollbackShellGeneration;
                    m_nativeFocusRollbackEpoch = rollbackEpoch;
                }

                ::SetFocus(original);

                // SetFocus dispatch is synchronous. If the destination's
                // normal wx focus path did not consume this departure latch,
                // it has no remaining authority (including a successful move
                // to another host's bridge). Retire only the exact latch armed
                // by this attempt; a nested transaction may have replaced it.
                if ( m_nativeFocusDepartureSource.lock() == lifetime &&
                     m_nativeFocusDepartureDestination ==
                         reinterpret_cast<WXHWND>(original) &&
                     m_nativeFocusDepartureDestinationGeneration ==
                         originalGeneration )
                {
                    m_nativeFocusDepartureSource.reset();
                    m_nativeFocusDepartureDestination = nullptr;
                    m_nativeFocusDepartureDestinationGeneration = 0;
                }

                // The nested shell WM_SETFOCUS normally consumes the latch.
                // Clear it here only when no newer nested failure superseded
                // this exact epoch.
                if ( rollbackEpoch &&
                     m_nativeFocusRollbackEpoch == rollbackEpoch )
                {
                    m_nativeFocusRollbackTarget.reset();
                    m_nativeFocusRollbackShell = nullptr;
                    m_nativeFocusRollbackShellGeneration = 0;
                    m_nativeFocusRollbackEpoch = 0;
                }
            }
            else if ( HasNativeFocusAuthority() )
                ::SetFocus(nullptr);
        });
    wxUnusedVar(nativeAuthorityRollback);

    if ( !inIsland )
    {
        // A native shell request has already delivered CHILD_FOCUS/SET_FOCUS;
        // a direct logical request publishes the same transition below.
        // Moving shell -> bridge is an implementation detail and must not emit
        // a contradictory KILL_FOCUS for the just-focused logical control.
        wxWinUISlot * const transferSlot = getLiveSlot();
        if ( !transferSlot )
            return;
        const std::weak_ptr<wxWinUISlotLifetime> previousTransfer =
            m_nativeFocusTransferTarget;
        m_nativeFocusTransferTarget = transferSlot->m_lifetime;
        wxScopeGuard transferGuard = wxMakeGuard([this, previousTransfer]()
        {
            m_nativeFocusTransferTarget = previousTransfer;
        });
        wxUnusedVar(transferGuard);
        ::SetFocus(m_bridge);

        // SetFocus synchronously runs arbitrary KILL/SET_FOCUS handlers. They
        // are allowed to redirect native focus to another HWND. XAML can keep
        // a stale focused element for a short time after such a redirect, so
        // never publish that element as wx focus ownership. Supersede this
        // intent as well, making any still-live request frame and its queued
        // retry invisible to Got/LostFocus.
        if ( !HasNativeFocusAuthority() )
        {
            // GotFocus may already have published a transient slot owner while
            // SetFocus() was dispatching. Native focus has since been
            // redirected outside the island, so retire that owner and balance
            // its wx focus notification before returning.
            abandonCurrentIntent();
            return;
        }
        bridgeAcquiredForAttempt = true;
    }

    live = getLiveSlot();
    if ( !live || !HasNativeFocusAuthority() )
        return;

    bool focusInside = false;
    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;

        bool focused = false;

        // The component's preferred target first: a wxRadioBox focuses its
        // SELECTED item, not whatever comes first.
        if ( preferred && wxWinUIElementIsInSubtree(preferred, content) )
        {
            if ( const auto control =
                    preferred.try_as<MUX::Controls::Control>() )
            {
                focused =
                    control.IsEnabled() &&
                    control.Visibility() == MUX::Visibility::Visible &&
                    control.IsTabStop() &&
                    control.Focus(MUX::FocusState::Programmatic);
            }
            if ( !getLiveSlot() )
                return;
        }

        if ( !focused )
        {
            const auto control =
                content.try_as<MUX::Controls::Control>();
            if ( control )
                focused = control.Focus(MUX::FocusState::Programmatic);
            if ( !getLiveSlot() )
                return;
        }

        if ( !focused )
        {
            // The content is not a focusable Control itself: fall back to
            // its first focusable descendant.
            const auto first = MUX::Input::FocusManager::
                FindFirstFocusableElement(container);
            if ( const auto firstControl =
                    first.try_as<MUX::Controls::Control>() )
                focused = firstControl.Focus(MUX::FocusState::Programmatic);
            if ( !getLiveSlot() )
                return;
        }

        // The boolean returned by UIElement::Focus() is not the focus
        // authority: it can be false when focus was already within a
        // templated descendant (notably ComboBox's editable TextBox). Always
        // reconcile from FocusManager's current element and the exact slot
        // content generation. GotFocus normally publishes this transition
        // synchronously; this fallback also covers a projection/template that
        // doesn't bubble it through the container.
        focusInside = wxWinUIFocusIsInside(container, GetXamlRoot());
        if ( focusInside )
        {
            // Control::Focus() is another reentrant boundary. A Got/Kill
            // handler may have moved Win32 focus outside the bridge even
            // though FocusManager still reports this XAML subtree.
            if ( !HasNativeFocusAuthority() )
            {
                // Control::Focus() may have synchronously published the slot
                // before an application callback redirected native focus.
                // Once the island has lost native authority, no logical slot
                // owner may remain visible.
                abandonCurrentIntent();
                return;
            }

            // From here XAML really owns focus in this exact subtree. Any
            // later application redirection is authoritative and must not be
            // undone by the failed-attempt rollback guard.
            xamlFocusCommitted = true;
            FocusRequest *active = FindFocusRequest(window);
            if ( active &&
                 active->origin ==
                     FocusOrigin::MigrationContinuation )
            {
                const std::shared_ptr<FocusMigration> migration =
                    m_focusMigration;
                if ( CommitFocusMigration(migration, lifetime) )
                {
                    ClearDeferredFocusRequest(intentGeneration);
                    return;
                }
                AbortFocusMigration(migration, true);
                if ( !getLiveSlot() ||
                     intentGeneration != m_focusIntentGeneration )
                {
                    return;
                }
                active = FindFocusRequest(window);
            }
            ClearDeferredFocusRequest(intentGeneration);
            const bool alreadyDelivered =
                active &&
                active->origin == FocusOrigin::WxSetAlreadyDelivered;
            wxWindow * const previousHint =
                active ? active->previous.get() : previousWeak.get();
            TransitionFocus(window, previousHint, alreadyDelivered);
            if ( !HasNativeFocusAuthority() )
            {
                abandonCurrentIntent();
                return;
            }
            if ( m_focusOwner != window )
                return;
        }

        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("FocusSlot '%s' XAML focused=%d inside=%d",
                            label.utf8_str().data(), focused, focusInside);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    if ( focusInside || !allowDeferredRetry || !getLiveSlot() || !wxTheApp )
        return;

    // Focus can legitimately be unavailable until the coalesced slot
    // geometry/template pass. Arm one (and only one) post-flush retry for this
    // exact content generation. Loaded is an independent final edge when it
    // occurs after that flush; neither path reposts itself.
    MarkDirty(window);
}

wxWinUITopLevelHost::FocusRequest *
wxWinUITopLevelHost::FindFocusRequest(wxWindow *target)
{
    for ( auto it = m_focusRequests.rbegin();
          it != m_focusRequests.rend();
          ++it )
    {
        if ( it->intentGeneration == m_focusIntentGeneration &&
             it->target &&
             it->target->GetHost() == this &&
             it->target->GetWindow() == target )
        {
            return &*it;
        }
    }

    return nullptr;
}

bool wxWinUITopLevelHost::HasNativeFocusAuthority() const
{
    const HWND focus = ::GetFocus();
    return focus && m_bridge &&
           (focus == m_bridge || ::IsChild(m_bridge, focus));
}

void wxWinUITopLevelHost::InvalidateFocusIntent(
    unsigned long long expectedGeneration)
{
    if ( expectedGeneration &&
         expectedGeneration != m_focusIntentGeneration )
    {
        return;
    }

    if ( ++m_focusIntentGeneration == 0 )
        ++m_focusIntentGeneration;
    ClearDeferredFocusRequest();
}

wxWindow *wxWinUITopLevelHost::FindFocusedSlot() const
{
    const auto root = GetXamlRoot();
    for ( const auto& entry : m_slots )
    {
        const wxWinUISlot * const slot = entry.second;
        if ( slot && slot->m_container &&
             wxWinUIFocusIsInside(slot->m_container, root) )
        {
            return entry.first;
        }
    }

    return nullptr;
}

void wxWinUITopLevelHost::TransitionFocus(wxWindow *newOwner,
                                          wxWindow *previousHint,
                                          bool wxSetAlreadyDelivered,
                                          wxWindow *lossDestination)
{
    OperationGuard operation(this);

    if ( m_shuttingDown )
        return;

    if ( newOwner )
    {
        wxWinUISlot * const newSlot = FindSlot(newOwner);
        if ( !newSlot || !newSlot->m_lifetime ||
             newSlot->m_lifetime->GetHost() != this )
        {
            return;
        }
    }

    wxWindow * const oldOwner = m_focusOwner;
    if ( oldOwner == newOwner )
        return;

    const unsigned long long intentGeneration = m_focusIntentGeneration;
    const std::shared_ptr<wxWinUISlotLifetime> newLifetime =
        newOwner && FindSlot(newOwner)
            ? FindSlot(newOwner)->m_lifetime
            : nullptr;
    const unsigned long long newContentGeneration =
        newOwner && FindSlot(newOwner)
            ? FindSlot(newOwner)->m_contentGeneration
            : 0;
    wxWeakRef<wxWindow> oldWeak(oldOwner ? oldOwner : previousHint);
    wxWeakRef<wxWindow> newWeak(newOwner);
    wxWeakRef<wxWindow> lossWeak(newOwner ? nullptr : lossDestination);
    const auto destinationIsAuthoritative = [&]()
    {
        if ( !newOwner )
            return true;
        if ( intentGeneration != m_focusIntentGeneration ||
             !HasNativeFocusAuthority() || !newLifetime ||
             newLifetime->GetHost() != this ||
             newLifetime->GetWindow() != newWeak.get() )
        {
            return false;
        }

        wxWindow * const current = newWeak.get();
        wxWinUISlot * const slot = current ? FindSlot(current) : nullptr;
        return slot && slot->m_lifetime == newLifetime &&
               slot->m_contentGeneration == newContentGeneration &&
               wxWinUIFocusIsInside(slot->m_container, GetXamlRoot());
    };

    // Publish the truthful logical destination before application callbacks:
    // FindFocus() called from KILL_FOCUS must already see where focus moved.
    m_focusOwner = newOwner;

    const auto dispatch = [](wxWindow *receiver, wxEvent& event)
    {
        receiver->HandleWindowEvent(event);
    };

    if ( oldOwner )
    {
        if ( wxWindow * const oldWindow = oldWeak.get() )
        {
            wxFocusEvent event(wxEVT_KILL_FOCUS, oldWindow->GetId());
            event.SetEventObject(oldWindow);
            event.SetWindow(newOwner ? newWeak.get() : lossWeak.get());
            dispatch(oldWindow, event);
        }

        // KILL_FOCUS handlers may destroy the target or redirect focus. Do
        // not synthesize events for the stale destination in either case.
        if ( m_shuttingDown || m_focusOwner != newOwner )
            return;
        if ( !destinationIsAuthoritative() )
        {
            if ( m_focusOwner == newOwner )
                m_focusOwner = nullptr;
            return;
        }
    }

    if ( !newOwner || wxSetAlreadyDelivered )
        return;

    wxWindow *newWindow = newWeak.get();
    if ( !newWindow || !FindSlot(newWindow) )
    {
        if ( m_focusOwner == newOwner )
            m_focusOwner = nullptr;
        return;
    }

    // Match wxWindowMSW::HandleSetFocus(): parent bookkeeping first, then
    // SET_FOCUS with the previous window as its counterpart.
    wxChildFocusEvent childEvent(newWindow);
    dispatch(newWindow, childEvent);

    newWindow = newWeak.get();
    if ( m_shuttingDown || !newWindow ||
         m_focusOwner != newOwner || !FindSlot(newWindow) ||
         !destinationIsAuthoritative() )
    {
        if ( m_focusOwner == newOwner )
            m_focusOwner = nullptr;
        return;
    }

    wxFocusEvent setEvent(wxEVT_SET_FOCUS, newWindow->GetId());
    setEvent.SetEventObject(newWindow);
    setEvent.SetWindow(oldWeak.get());
    dispatch(newWindow, setEvent);

    // SET handlers may redirect native focus while FocusManager still reports
    // the old XAML leaf. Balance the SET exactly once if no synchronous
    // LostFocus/nested transition already retired this provisional owner.
    if ( m_focusOwner == newOwner && !destinationIsAuthoritative() )
    {
        TransitionFocus(
            nullptr, newOwner, false,
            wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::OnXamlSlotGotFocus(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container)
{
    wxWindow *window = state ? state->GetWindow() : nullptr;
    wxWinUISlot *slot = window ? FindSlot(window) : nullptr;
    if ( !state || state->GetHost() != this || !window || !slot ||
         slot->m_lifetime != state || !container )
    {
        return;
    }

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(slot);

    // GotFocus bubbles once per inner element and stale notifications can
    // arrive after a newer transition. Only the currently focused subtree
    // is authoritative. Win32 focus must still belong to the bridge/InputSite:
    // FocusManager can lag behind a reentrant native redirection.
    if ( !HasNativeFocusAuthority() ||
         !wxWinUIFocusIsInside(container, GetXamlRoot()) )
        return;

    m_nativeFocusDepartureSource.reset();
    m_nativeFocusDepartureDestination = nullptr;
    m_nativeFocusDepartureDestinationGeneration = 0;
    const std::shared_ptr<wxWinUISlotLifetime> entryTarget =
        m_nativeFocusEntryTarget.lock();
    if ( !entryTarget ||
         (entryTarget == state &&
          m_nativeFocusEntryEpoch != 0 &&
          slot->m_nativeFocusEntryEpoch == m_nativeFocusEntryEpoch) )
    {
        m_nativeFocusEntryAuthority = nullptr;
        m_nativeFocusEntryAuthorityGeneration = 0;
        m_nativeFocusEntryTarget.reset();
        m_nativeFocusEntryEpoch = 0;
        slot->m_nativeFocusEntryEpoch = 0;
    }

    wxWeakRef<wxWindow> migrationPrevious;
    const std::shared_ptr<FocusMigration> migration = m_focusMigration;
    if ( migration && !migration->committed &&
         migration->destinationHost == m_hostLifetime )
    {
        migrationPrevious = migration->window;
        if ( migration->destinationSlot == state &&
             CommitFocusMigration(migration, state) )
        {
            FocusRequest * const request = FindFocusRequest(window);
            if ( request &&
                 request->origin ==
                     FocusOrigin::MigrationContinuation )
            {
                ClearDeferredFocusRequest(
                    request->intentGeneration);
            }
            else if ( m_deferredFocusRequest.origin ==
                          FocusOrigin::MigrationContinuation &&
                      m_deferredFocusRequest.target == state )
            {
                ClearDeferredFocusRequest(
                    m_deferredFocusRequest.intentGeneration);
            }
            return;
        }

        // Any other focused destination slot wins over the suspended
        // transaction. Keep its native authority, balance the migrant's one
        // logical owner, then let the ordinary arbiter publish this winner.
        // A failed exact commit is handled by the same fail-closed path.
        AbortFocusMigration(
            migration, true,
            migration->destinationSlot == state ? nullptr : window);

        window = state->GetWindow();
        slot = window ? FindSlot(window) : nullptr;
        if ( !window || !slot || slot->m_lifetime != state ||
             state->GetHost() != this ||
             !HasNativeFocusAuthority() ||
             !wxWinUIFocusIsInside(container, GetXamlRoot()) )
        {
            return;
        }
    }

    FocusRequest *request = FindFocusRequest(window);
    if ( request &&
         request->origin == FocusOrigin::MigrationContinuation )
    {
        // The request can outlive a ticket retired by a native/XAML redirect.
        // It has no authority to revive that migration as an ordinary focus
        // request.
        ClearDeferredFocusRequest(request->intentGeneration);
        request = nullptr;
    }
    if ( !request )
    {
        // A physical/XAML navigation result supersedes any older deferred
        // programmatic intent.
        InvalidateFocusIntent();
    }
    wxWindow * const migratedPrevious = migrationPrevious.get();
    wxWindow * const previous =
        request
            ? request->previous.get()
            : migratedPrevious
                ? migratedPrevious
                : m_focusOwner;
    const bool alreadyDelivered =
        request &&
        request->origin == FocusOrigin::WxSetAlreadyDelivered;
    TransitionFocus(window, previous, alreadyDelivered);
    if ( m_focusOwner == window && !HasNativeFocusAuthority() )
    {
        TransitionFocus(
            nullptr, window, false,
            wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::OnXamlSlotLostFocus(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container)
{
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !state || state->GetHost() != this || !window || !slot ||
         slot->m_lifetime != state || !container )
    {
        return;
    }

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(slot);

    const auto xamlRoot = GetXamlRoot();
    if ( wxWinUIFocusIsInside(container, xamlRoot) )
        return;

    // A late LostFocus from a slot superseded by a newer transition is a
    // complete no-op, not another KILL_FOCUS.
    if ( m_focusOwner != window )
        return;

    // Detaching a focused slot for a cross-TLW migration raises LostFocus
    // before the destination carrier can acquire focus. Keep the single
    // logical wx control focused across that implementation handoff; a real
    // redirect to another slot/native HWND cancels the ticket above.
    if ( IsSuppressedMigrationLoss(state) )
        return;

    // Only FocusManager may publish a destination. A pending request is useful
    // as a counterpart hint but is never evidence that XAML focused its slot.
    wxWindow *target = FindFocusedSlot();
    FocusRequest * const request =
        target ? FindFocusRequest(target) : nullptr;
    if ( !request )
    {
        InvalidateFocusIntent();
    }
    const bool alreadyDelivered =
        request &&
        request->origin == FocusOrigin::WxSetAlreadyDelivered;
    // A non-null destination is truthful only while the island owns native
    // focus. If focus has left the bridge, publish a loss instead of reviving
    // a stale XAML target.
    const bool nativeAuthority = HasNativeFocusAuthority();
    wxWindow * const lossDestination =
        nativeAuthority ? nullptr : wxWinUIResolveCurrentNativeFocusWindow();
    TransitionFocus(nativeAuthority ? target : nullptr,
                    window, alreadyDelivered,
                    lossDestination);
}

void wxWinUITopLevelHost::SetSlotPreferredFocus(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& target)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    if ( !target )
    {
        slot->m_preferredFocus = nullptr;
        return;
    }

    // Never replace a valid target with an element from a detached or
    // speculative tree.  This makes a failed control-host content swap leave
    // the old preferred focus target intact.
    if ( target.try_as<winrt::Microsoft::UI::Xaml::Controls::Control>() &&
            wxWinUIElementIsInSubtree(target, slot->m_content) )
    {
        slot->m_preferredFocus = winrt::make_weak(target);
    }
}

void wxWinUITopLevelHost::BindSlotWindow(wxWinUISlot& slot)
{
    wxWindow * const window = slot.m_window;

    window->Bind(wxEVT_MOVE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_SHOW, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnSlotWindowDestroy, this);
    BindSlotAncestors(slot);
}

void wxWinUITopLevelHost::BindSlotAncestors(wxWinUISlot& slot)
{
    wxASSERT_MSG(slot.m_ancestors.empty(),
                 "slot ancestor subscriptions must be cleared before binding");

    wxWindow * const window = slot.m_window;

    // A child window gets NO event when one of its ancestors moves, resizes
    // or is shown/hidden (a notebook page switch, a sash drag...): listen on
    // the whole parent chain, with reference counting since chains overlap.
    for ( wxWindow *p = window->GetParent();
          p && p != m_tlw;
          p = p->GetParent() )
    {
        slot.m_ancestors.push_back(p);
        AddAncestorRef(p);
    }

    // The effective wxEVT_SET_CURSOR chain is part of the cached verdict.
    // Reparenting can replace it without moving the leaf on screen.
    if ( ++slot.m_cursorTopologyGeneration == 0 )
        ++slot.m_cursorTopologyGeneration;
}

void wxWinUITopLevelHost::UnbindSlotAncestors(wxWinUISlot& slot)
{
    for ( wxWindow *ancestor : slot.m_ancestors )
        ReleaseAncestorRef(ancestor);
    slot.m_ancestors.clear();
}

void wxWinUITopLevelHost::UnbindSlotWindow(wxWinUISlot& slot)
{
    wxWindow * const window = slot.m_window;

    // wxEVT_DESTROY is dispatched from the window's dynamic-handler table.
    // Mutating that same table from OnSlotWindowDestroy() (or from Shutdown()
    // when the TLW itself is also a slot) can invalidate the iterator wx is
    // currently using.  The table is about to die with the window, so leave
    // its direct bindings alone in this one case.  The host stays alive until
    // the dispatch has returned.
    if ( !window->IsBeingDeleted() )
    {
        window->Unbind(wxEVT_MOVE,
                       &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
        window->Unbind(wxEVT_SIZE,
                       &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
        window->Unbind(wxEVT_SHOW,
                       &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
        window->Unbind(wxEVT_DESTROY,
                       &wxWinUITopLevelHost::OnSlotWindowDestroy, this);
    }

    UnbindSlotAncestors(slot);

    const bool retainedByMigration =
        m_focusMigration &&
        !m_focusMigration->committed &&
        m_focusMigration->logicalFocusWasActive &&
        m_focusMigration->sourceHost == m_hostLifetime &&
        m_focusMigration->sourceSlot == slot.m_lifetime &&
        m_focusMigration->window.get() == window;
    if ( m_focusOwner == window && !retainedByMigration )
        m_focusOwner = nullptr;

    if ( const auto departure = m_nativeFocusDepartureSource.lock();
         departure && departure == slot.m_lifetime )
    {
        m_nativeFocusDepartureSource.reset();
        m_nativeFocusDepartureDestination = nullptr;
        m_nativeFocusDepartureDestinationGeneration = 0;
    }
    if ( const auto rollback = m_nativeFocusRollbackTarget.lock();
         rollback && rollback == slot.m_lifetime )
    {
        m_nativeFocusRollbackTarget.reset();
        m_nativeFocusRollbackShell = nullptr;
        m_nativeFocusRollbackShellGeneration = 0;
        m_nativeFocusRollbackEpoch = 0;
    }
}

void wxWinUITopLevelHost::AddAncestorRef(wxWindow *ancestor)
{
    if ( ++m_ancestorRefs[ancestor] > 1 )
        return;

    ancestor->Bind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);
}

void wxWinUITopLevelHost::ReleaseAncestorRef(wxWindow *ancestor)
{
    const auto it = m_ancestorRefs.find(ancestor);
    if ( it == m_ancestorRefs.end() )
        return;     // already dropped by OnAncestorDestroy

    if ( --it->second > 0 )
        return;

    m_ancestorRefs.erase(it);

    ancestor->Unbind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);
}

bool wxWinUITopLevelHost::MarkSlotsUnderAncestor(wxWindow *ancestor)
{
    if ( !ancestor )
        return false;

    bool marked = false;
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( !slot )
            continue;
        if ( entry.first != ancestor &&
             std::find(slot->m_ancestors.begin(),
                       slot->m_ancestors.end(),
                       ancestor) == slot->m_ancestors.end() )
        {
            continue;
        }

        if ( ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
        MarkDirty(entry.first);
        marked = true;
    }
    return marked;
}

void wxWinUITopLevelHost::OnAncestorGeometry(wxEvent& event)
{
    event.Skip();

    wxWindow * const ancestor =
        wxDynamicCast(event.GetEventObject(), wxWindow);
    InvalidateStructure();
    if ( !ancestor )
    {
        // Without the window that moved there is nothing to narrow
        // down: resync everything.
        MarkAllDirty();
        return;
    }

    // Anything under it may have moved on screen or changed visibility, and
    // an ancestor is rarely the only thing which moved: the flush is
    // coalesced, so resync everything rather than guess.
    MarkSlotsUnderAncestor(ancestor);
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnAncestorDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    wxWindow * const window = event.GetWindow();
    if ( window && m_ancestorRefs.erase(window) )
    {
        // The window is going away, no point unbinding from it; the slots
        // that referenced it are being destroyed too and their release will
        // find the entry already gone.
    }
}

// ============================================================================
// input synthesis + focus arbiter
// ============================================================================

void wxWinUITopLevelHost::OnSlotPointer(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    wxWinUIInputKind kind)
{
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state ||
         !container || container != slot->m_container )
    {
        return;
    }

    // A wx handler invoked below may synchronously unregister the slot. Keep
    // its allocation alive for the state transition, but revalidate the
    // invalidatable lifetime before every subsequent action.
    wxWinUISlot::OperationGuard slotOperation(slot);
    if ( ++slot->m_inputTransitionEpoch == 0 )
        ++slot->m_inputTransitionEpoch;
    const unsigned long long transitionEpoch =
        slot->m_inputTransitionEpoch;

    wxWinUIPointerSample sample;
    sample.kind = kind;
    sample.timestamp = ::GetTickCount64();

    const bool isInterruption =
        kind == wxWinUIInputKind::Cancel ||
        kind == wxWinUIInputKind::CaptureLost;

    bool havePointerIdentity = false;
    try
    {
        const auto pointer = args.Pointer();
        sample.device = wxWinUIGetInputDevice(pointer.PointerDeviceType());
        sample.pointerId = pointer.PointerId();
        havePointerIdentity = true;

        const unsigned keyModifiers =
            static_cast<unsigned>(args.KeyModifiers());
        using VirtualKeyModifiers =
            winrt::Windows::System::VirtualKeyModifiers;
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Shift) )
        {
            sample.modifiers |= MK_SHIFT;
        }
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Control) )
        {
            sample.modifiers |= MK_CONTROL;
        }
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Menu) )
        {
            sample.modifiers |= wxWINUI_INPUT_MOD_ALT;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // Cancellation must not depend on a visual/input query succeeding:
        // abandoning the cleanup here would leave the slot permanently
        // pressed. Ordinary samples fail closed.
        if ( !isInterruption )
            return;
    }

    bool havePoint = false;
    bool pointInside = true;
    try
    {
        // Ask for the point in the explicitly-LTR root coordinate space.
        // A mirrored slot container has a mirrored local origin, so using it
        // here would require reconstructing the physical X coordinate.
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        sample.device = wxWinUIGetInputDevice(point.PointerDeviceType());
        sample.pointerId = point.PointerId();
        sample.isPrimary = props.IsPrimary();
        sample.isCompatibilityMouse =
            sample.device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample.timestamp = point.Timestamp() / 1000;

        unsigned buttonMask = 0;
        if ( props.IsLeftButtonPressed() )
            buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            buttonMask |= MK_XBUTTON2;

        if ( kind == wxWinUIInputKind::Move ||
             kind == wxWinUIInputKind::Press ||
             kind == wxWinUIInputKind::Release )
        {
            const wxWinUIInputMapping mapping =
                wxWinUIMapPointerUpdate(wxWinUIGetInputUpdateKind(
                    props.PointerUpdateKind()));
            if ( mapping.valid )
            {
                sample.kind = mapping.kind;
                sample.button = mapping.button;
            }
            else if ( kind != wxWinUIInputKind::Move &&
                      sample.device != wxWinUIInputDevice::Mouse )
            {
                // A normal primary touch/pen contact has no mouse update
                // kind. Map that contact to left once; keep an explicit pen
                // barrel/right update from the mapping above unchanged.
                sample.button = wxWinUIInputButton::Left;
            }
            else if ( kind != wxWinUIInputKind::Move )
            {
                return;
            }

            if ( sample.kind == wxWinUIInputKind::Press )
                buttonMask |= wxWinUIGetWParamButtonMask(sample.button);
            else if ( sample.kind == wxWinUIInputKind::Release )
                buttonMask &= ~wxWinUIGetWParamButtonMask(sample.button);
        }
        else if ( kind == wxWinUIInputKind::Wheel )
        {
            sample.wheelDelta = props.MouseWheelDelta();
            sample.horizontalWheel = props.IsHorizontalMouseWheel();
        }
        sample.buttonMask = buttonMask;

        const auto position = point.Position();
        POINT screen;
        if ( !MapRootPointerPointForSlot(
                 *slot, position.X, position.Y, &screen, &pointInside) )
        {
            if ( !isInterruption )
                return;
        }
        else
        {
            sample.screenX = screen.x;
            sample.screenY = screen.y;
            havePoint = true;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        if ( !isInterruption )
            return;
    }

    if ( !havePoint )
    {
        if ( isInterruption )
        {
            // A canceled point may no longer expose PointerPoint properties.
            // Only primary contacts can have entered this slot state; retain
            // the stable id when available, otherwise retire every gesture
            // local to this one slot.
            sample.isPrimary = true;
            sample.isCompatibilityMouse = false;
            sample.interruptAll = !havePointerIdentity;
        }
        POINT screen = { 0, 0 };
        if ( ::GetCursorPos(&screen) )
        {
            sample.screenX = screen.x;
            sample.screenY = screen.y;
        }
    }

    wxWinUIRouteObservation observation;
    if ( isInterruption )
    {
        // inputstate deliberately processes termination independently of
        // hit resolution and ignores this observation.
        observation.surface = wxWinUIInputSurface::Indeterminate;
    }
    else
    {
        const WXHWND hwnd = slot->m_hwnd;
        const unsigned long long generation =
            wxWinUIMSWGetHwndGeneration(window, hwnd);
        if ( !hwnd || !generation )
            return;

        observation.surface = pointInside
            ? wxWinUIInputSurface::Native
            : wxWinUIInputSurface::Outside;
        observation.target.leaf =
            reinterpret_cast<std::uintptr_t>(hwnd);
        observation.target.generation = generation;
        observation.target.owner = observation.target.leaf;
        observation.target.ownerGeneration = generation;
        observation.area = wxWinUIInputArea::Client;
        observation.zone = HTCLIENT;
        observation.clientDoubleClicks =
            (::GetClassLongPtr(reinterpret_cast<HWND>(hwnd),
                               GCL_STYLE) & CS_DBLCLKS) != 0;
    }

    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return;
    }

    if ( sample.kind == wxWinUIInputKind::Press &&
         HasSlotPointerOwner(sample) )
    {
        return;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    const wxWinUIInputTransition transition =
        slot->m_inputState.Route(sample, observation);
    const auto cleanupRemaining =
        [this, &transition, &state](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                const wxWinUIInputAction& pending = transition.actions[i];
                if ( pending.action ==
                         wxWinUIInputActionKind::HoverLeave ||
                     pending.action ==
                         wxWinUIInputActionKind::CancelPress )
                {
                    ExecuteSlotInputCleanup(state, pending);
                }
            }
        };

    // Never apply transition.cancelSource here. These events originate in
    // the hosted XAML control itself; setting Handled would prevent its
    // native visual/command behaviour. The state-machine flag is for the
    // root's native pass-through path only.
    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != transitionEpoch )
        {
            cleanupRemaining(i);
            return;
        }

        if ( !SendSlotMouseEvent(state, action) )
        {
            cleanupRemaining(i + 1);
            return;
        }

        if ( slot->m_inputTransitionEpoch != transitionEpoch )
        {
            cleanupRemaining(i + 1);
            return;
        }
    }
    RetryDeferredIslandCancelMode();
}

bool wxWinUITopLevelHost::MapRootPointerPointForSlot(
    const wxWinUISlot& slot,
    double xDIP,
    double yDIP,
    POINT *screen,
    bool *inside) const
{
    if ( !screen || !inside ||
         !std::isfinite(xDIP) || !std::isfinite(yDIP) )
    {
        return false;
    }

    const double scale = GetScale();
    if ( !(scale > 0.0) || !std::isfinite(scale) )
        return false;

    // xDIP/yDIP are already expressed relative to the explicitly-LTR root.
    // Calling window->ClientToScreen() here would mirror X a second time for
    // WS_EX_LAYOUTRTL. SendSlotMouseEvent() deliberately converts the final
    // physical screen point to the native client space only at delivery.
    const RECT rootScreen = GetClientScreenRect();
    const double xPx = xDIP * scale;
    const double yPx = yDIP * scale;
    const wxPoint pointInRoot(
        static_cast<int>(std::lround(xPx)),
        static_cast<int>(std::lround(yPx)));
    screen->x = rootScreen.left + pointInRoot.x;
    screen->y = rootScreen.top + pointInRoot.y;
    // Keep the hit predicate in the continuous physical coordinate space.
    // Rounding is part of the Win32 POINT contract only: using that rounded
    // value for containment shifts every edge by half a pixel at fractional
    // rasterization scales.
    const double left = slot.m_visibleRectPx.GetX();
    const double top = slot.m_visibleRectPx.GetY();
    const double right = left + slot.m_visibleRectPx.GetWidth();
    const double bottom = top + slot.m_visibleRectPx.GetHeight();
    *inside = slot.m_visible &&
              xPx >= left && xPx < right &&
              yPx >= top && yPx < bottom;
    return true;
}

bool wxWinUITopLevelHost::SendSlotMouseEvent(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return false;

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return false;

    // CancelPress is semantic bookkeeping for a XAML gesture. There is no
    // wx "button canceled" event and fabricating an UP here would duplicate
    // a later release. Slots never claim USER32 capture in inputstate, so a
    // ReleaseCapture action is likewise only a defensive no-op.
    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        UpdateSlotPointerOwner(state, action);
        if ( slot->m_inputState.CanDeliverCancellation(action) )
            slot->m_inputState.AcknowledgeCancellation(action);
        return true;
    }
    if ( action.action == wxWinUIInputActionKind::ReleaseCapture ||
         action.action == wxWinUIInputActionKind::BreakStorm )
    {
        return true;
    }

    const bool isHoverLeave =
        action.action == wxWinUIInputActionKind::HoverLeave;
    if ( isHoverLeave &&
         !slot->m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    const WXHWND hwnd = slot->m_hwnd;
    wxWinUIInputTargetKey currentTarget;
    currentTarget.leaf = reinterpret_cast<std::uintptr_t>(hwnd);
    currentTarget.generation =
        wxWinUIMSWGetHwndGeneration(window, hwnd);
    currentTarget.owner = currentTarget.leaf;
    currentTarget.ownerGeneration = currentTarget.generation;
    if ( !currentTarget.IsOk() || currentTarget != action.target )
        return isHoverLeave;

    if ( action.action == wxWinUIInputActionKind::HoverEnter &&
         !slot->m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    if ( action.action == wxWinUIInputActionKind::Dispatch &&
         action.kind == wxWinUIInputKind::Wheel )
    {
        if ( !action.wheelDelta )
            return true;

        const WORD keys = static_cast<WORD>(
            action.modifiers | action.buttonMask);
        if ( !slot->m_inputState.CommitBeforeDispatch(action) )
            return false;
        ::SendMessage(
            reinterpret_cast<HWND>(hwnd),
            action.horizontalWheel ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
            MAKEWPARAM(
                keys,
                static_cast<WORD>(
                    static_cast<short>(action.wheelDelta))),
            MAKELPARAM(static_cast<short>(action.screenX),
                       static_cast<short>(action.screenY)));
        return state->GetHost() == this &&
               state->GetWindow() == window &&
               !m_shuttingDown;
    }

    const wxEventType type = wxWinUIGetSlotMouseEventType(action);
    if ( type == wxEVT_NULL )
        return true;

    wxMouseEvent event(type);
    event.SetEventObject(window);
    event.SetId(window->GetId());
    event.SetTimestamp(static_cast<long>(action.timestamp));

    const wxPoint nativeClient = window->ScreenToClient(
        wxPoint(action.screenX, action.screenY));
    // Match wxWindowMSW::InitMouseEvent(): wx logical client coordinates
    // exclude a frame's toolbar/status/menu client-origin offset.
    const wxPoint client =
        nativeClient - window->GetClientAreaOrigin();
    event.m_x = client.x;
    event.m_y = client.y;
    event.m_shiftDown = (action.modifiers & MK_SHIFT) != 0;
    event.m_controlDown = (action.modifiers & MK_CONTROL) != 0;
    event.m_altDown =
        (action.modifiers & wxWINUI_INPUT_MOD_ALT) != 0;
    event.m_leftDown = (action.buttonMask & MK_LBUTTON) != 0;
    event.m_middleDown = (action.buttonMask & MK_MBUTTON) != 0;
    event.m_rightDown = (action.buttonMask & MK_RBUTTON) != 0;
    event.m_aux1Down = (action.buttonMask & MK_XBUTTON1) != 0;
    event.m_aux2Down = (action.buttonMask & MK_XBUTTON2) != 0;
    event.m_synthesized =
        action.device != wxWinUIInputDevice::Mouse;

    if ( action.action == wxWinUIInputActionKind::Dispatch &&
         !slot->m_inputState.CommitBeforeDispatch(action) )
    {
        return false;
    }

    UpdateSlotPointerOwner(state, action);
    window->HandleWindowEvent(event);
    return state->GetHost() == this &&
           state->GetWindow() == window &&
           !m_shuttingDown;
}

void wxWinUITopLevelHost::ExecuteSlotInputCleanup(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        (void)SendSlotMouseEvent(state, action);
        return;
    }

    if ( action.action != wxWinUIInputActionKind::HoverLeave ||
         m_shuttingDown || !state || state->GetHost() != this )
    {
        return;
    }

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return;

    // A wx UP/leave handler can route a newer gesture before the prepared
    // balancing HoverLeave runs. Only finish that old cleanup if the current
    // slot geometry still proves the sample is outside; otherwise preserve
    // hover and let the next real sample converge it.
    const RECT rootScreen = GetClientScreenRect();
    const wxPoint pointInRoot(action.screenX - rootScreen.left,
                              action.screenY - rootScreen.top);
    if ( slot->m_visible &&
         slot->m_visibleRectPx.Contains(pointInRoot) )
    {
        return;
    }

    (void)SendSlotMouseEvent(state, action);
}

void wxWinUITopLevelHost::UpdateSlotPointerOwner(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( !state || !action.gestureSerial ||
         action.button == wxWinUIInputButton::None )
    {
        return;
    }

    const bool starts =
        action.action == wxWinUIInputActionKind::Dispatch &&
        (action.kind == wxWinUIInputKind::Press ||
         action.kind == wxWinUIInputKind::DoubleClick);
    const bool ends =
        action.action == wxWinUIInputActionKind::CancelPress ||
        (action.action == wxWinUIInputActionKind::Dispatch &&
         action.kind == wxWinUIInputKind::Release);
    if ( !starts && !ends )
        return;

    SlotPointerOwner *freeOwner = nullptr;
    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() )
        {
            if ( !freeOwner )
                freeOwner = &owner;
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> current =
            owner.state.lock();
        if ( !current )
        {
            owner.Clear();
            if ( !freeOwner )
                freeOwner = &owner;
            continue;
        }

        if ( current == state &&
             owner.device == action.device &&
             owner.pointerId == action.pointerId &&
             owner.button == action.button &&
             owner.gestureSerial == action.gestureSerial )
        {
            if ( ends )
            {
                if ( action.action == wxWinUIInputActionKind::Dispatch &&
                     action.kind == wxWinUIInputKind::Release )
                {
                    RememberSlotRelease(action);
                }
                owner.Clear();
            }
            return;
        }
    }

    if ( starts )
    {
        wxASSERT_MSG(freeOwner,
                     "WinUI slot pointer owner table exhausted");
        if ( freeOwner )
        {
            freeOwner->state = state;
            freeOwner->device = action.device;
            freeOwner->pointerId = action.pointerId;
            freeOwner->button = action.button;
            freeOwner->gestureSerial = action.gestureSerial;
            freeOwner->downTimestamp = action.timestamp;
        }
    }
}

void wxWinUITopLevelHost::RememberSlotRelease(
    const wxWinUIInputAction& action)
{
    SlotReleaseReceipt *freeReceipt = nullptr;
    for ( SlotReleaseReceipt& receipt : m_slotReleaseReceipts )
    {
        if ( !receipt.IsActive() )
        {
            if ( !freeReceipt )
                freeReceipt = &receipt;
            continue;
        }

        if ( receipt.device == action.device &&
             receipt.pointerId == action.pointerId &&
             receipt.button == action.button )
        {
            freeReceipt = &receipt;
            break;
        }
    }

    wxASSERT_MSG(freeReceipt,
                 "WinUI slot release receipt table exhausted");
    if ( freeReceipt )
    {
        freeReceipt->device = action.device;
        freeReceipt->pointerId = action.pointerId;
        freeReceipt->button = action.button;
        freeReceipt->timestamp = action.timestamp;
        freeReceipt->gestureSerial = action.gestureSerial;
    }
}

bool wxWinUITopLevelHost::HasSlotPointerOwner(
    const wxWinUIPointerSample& sample)
{
    if ( sample.button == wxWinUIInputButton::None )
        return false;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() )
            continue;
        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            owner.Clear();
            continue;
        }
        if ( owner.device == sample.device &&
             owner.pointerId == sample.pointerId &&
             owner.button == sample.button )
        {
            return true;
        }
    }
    return false;
}

bool wxWinUITopLevelHost::ConsumeSlotRelease(
    const wxWinUIPointerSample& sample)
{
    if ( sample.kind != wxWinUIInputKind::Release )
        return false;

    for ( SlotReleaseReceipt& receipt : m_slotReleaseReceipts )
    {
        if ( receipt.IsActive() &&
             receipt.device == sample.device &&
             receipt.pointerId == sample.pointerId &&
             receipt.button == sample.button &&
             receipt.timestamp == sample.timestamp )
        {
            receipt.Clear();
            return true;
        }
    }
    return false;
}

void wxWinUITopLevelHost::ForgetSlotPointerOwners(
    const std::shared_ptr<wxWinUISlotLifetime>& state)
{
    if ( !state )
        return;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( owner.IsActive() && owner.state.lock() == state )
            owner.Clear();
    }
    RetryDeferredIslandCancelMode();
}

bool wxWinUITopLevelHost::DispatchSlotOwnedRelease(
    const wxWinUIPointerSample& sample)
{
    OperationGuard hostOperation(this);
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( sample.kind != wxWinUIInputKind::Release ||
         sample.button == wxWinUIInputButton::None ||
         sample.isCompatibilityMouse ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) )
    {
        return false;
    }

    if ( ConsumeSlotRelease(sample) )
        return true;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() ||
             owner.device != sample.device ||
             owner.pointerId != sample.pointerId ||
             owner.button != sample.button ||
             (sample.timestamp && owner.downTimestamp &&
              sample.timestamp < owner.downTimestamp) )
        {
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        const std::uint64_t gestureSerial = owner.gestureSerial;
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            owner.Clear();
            return true;
        }

        // This path exists only because the release did not traverse the
        // original slot. Route the balancing UP there, but classify the
        // physical release as outside that slot so it cannot manufacture a
        // click/double-click pair.
        (void)RouteSlotPointerSample(
            state, sample, wxWinUIInputSurface::Xaml);
        (void)ConsumeSlotRelease(sample);

        // A matching owner always consumes the physical release. If its
        // state rejected the action, forwarding it below the former slot
        // would create exactly the orphan UP this broker prevents.
        // Clear only the exact old serial: the wx UP handler is allowed to
        // pump a newer DOWN which may reuse this fixed-table entry.
        for ( SlotPointerOwner& current : m_slotPointerOwners )
        {
            if ( current.IsActive() &&
                 current.state.lock() == state &&
                 current.device == sample.device &&
                 current.pointerId == sample.pointerId &&
                 current.button == sample.button &&
                 current.gestureSerial == gestureSerial )
            {
                current.Clear();
                break;
            }
        }
        return true;
    }

    return false;
}

bool wxWinUITopLevelHost::RouteSlotPointerSample(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIPointerSample& sample,
    wxWinUIInputSurface physicalSurface)
{
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return false;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return false;

    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return false;
    }

    if ( sample.kind == wxWinUIInputKind::Press &&
         HasSlotPointerOwner(sample) )
    {
        return false;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    wxWinUISlot::OperationGuard slotOperation(slot);
    if ( ++slot->m_inputTransitionEpoch == 0 )
        ++slot->m_inputTransitionEpoch;
    const unsigned long long epoch = slot->m_inputTransitionEpoch;

    wxWinUIRouteObservation observation;
    if ( sample.kind == wxWinUIInputKind::Cancel ||
         sample.kind == wxWinUIInputKind::CaptureLost )
    {
        observation.surface = wxWinUIInputSurface::Indeterminate;
    }
    else
    {
        const WXHWND hwnd = slot->m_hwnd;
        const unsigned long long generation =
            wxWinUIMSWGetHwndGeneration(window, hwnd);
        if ( !hwnd || !generation )
            return false;

        observation.surface = physicalSurface;
        observation.target.leaf =
            reinterpret_cast<std::uintptr_t>(hwnd);
        observation.target.generation = generation;
        observation.target.owner = observation.target.leaf;
        observation.target.ownerGeneration = generation;
        observation.area = wxWinUIInputArea::Client;
        observation.zone = HTCLIENT;
        observation.clientDoubleClicks =
            (::GetClassLongPtr(
                reinterpret_cast<HWND>(hwnd),
                GCL_STYLE) & CS_DBLCLKS) != 0;
    }

    const wxWinUIInputTransition transition =
        slot->m_inputState.Route(sample, observation);
    const auto cleanupRemaining =
        [this, &transition, &state](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                const wxWinUIInputAction& pending = transition.actions[i];
                if ( pending.action ==
                         wxWinUIInputActionKind::HoverLeave ||
                     pending.action ==
                         wxWinUIInputActionKind::CancelPress )
                {
                    ExecuteSlotInputCleanup(state, pending);
                }
            }
        };
    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != epoch )
        {
            cleanupRemaining(i);
            return false;
        }
        if ( !SendSlotMouseEvent(state, action) )
        {
            cleanupRemaining(i + 1);
            return false;
        }
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != epoch )
        {
            cleanupRemaining(i + 1);
            // The current wx event was delivered successfully. Re-entrance
            // superseded only the remainder of this prepared snapshot, not
            // the physical sample that the test seam/broker just consumed.
            return true;
        }
    }
    RetryDeferredIslandCancelMode();
    return true;
}

bool wxWinUITopLevelHost::TestRouteSlotPointer(
    wxWindow *window,
    const wxWinUIPointerSample& sample,
    wxWinUIInputSurface physicalSurface)
{
    wxWinUISlot * const slot = FindSlot(window);
    return slot &&
           RouteSlotPointerSample(
               slot->m_lifetime, sample, physicalSurface);
}

bool wxWinUITopLevelHost::TestMapRootPointerPointForSlot(
    wxWindow *window,
    double xDIP,
    double yDIP,
    wxPoint *screen,
    bool *inside) const
{
    const wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || !screen )
        return false;

    POINT nativeScreen;
    if ( !MapRootPointerPointForSlot(
             *slot, xDIP, yDIP, &nativeScreen, inside) )
    {
        return false;
    }

    *screen = wxPoint(nativeScreen.x, nativeScreen.y);
    return true;
}

std::size_t
wxWinUITopLevelHost::GetSlotPointerOwnerCountForTest() const
{
    return static_cast<std::size_t>(std::count_if(
        m_slotPointerOwners.begin(), m_slotPointerOwners.end(),
        [](const SlotPointerOwner& owner)
        {
            return owner.IsActive();
          }));
}

void wxWinUITopLevelHost::TestRetireIslandCancelTarget()
{
    RetireIslandCancelTarget(m_inner ? m_inner : m_bridge);
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::CancelSlotPointerOwners(
    const wxWinUIPointerSample& sample,
    bool allPointers)
{
    OperationGuard hostOperation(this);
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    // Snapshot the entries present at the interruption boundary. wx handlers
    // below may unregister/recreate slots and start a newer gesture; this old
    // cleanup must neither visit nor clear such a reentrant owner.
    const auto owners = m_slotPointerOwners;
    for ( std::size_t ownerIndex = 0;
          ownerIndex < owners.size(); ++ownerIndex )
    {
        const SlotPointerOwner& owner = owners[ownerIndex];
        if ( !owner.IsActive() ||
             (!allPointers &&
              (owner.device != sample.device ||
               owner.pointerId != sample.pointerId)) )
        {
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        const auto clearExactOwner =
            [this, ownerIndex, &owner, &state]()
            {
                SlotPointerOwner& current =
                    m_slotPointerOwners[ownerIndex];
                if ( current.IsActive() &&
                     current.state.lock() == state &&
                     current.device == owner.device &&
                     current.pointerId == owner.pointerId &&
                     current.button == owner.button &&
                     current.gestureSerial == owner.gestureSerial )
                {
                    current.Clear();
                }
            };
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            clearExactOwner();
            continue;
        }

        wxWinUISlot::OperationGuard slotOperation(slot);
        if ( ++slot->m_inputTransitionEpoch == 0 )
            ++slot->m_inputTransitionEpoch;
        const unsigned long long epoch = slot->m_inputTransitionEpoch;

        wxWinUIPointerSample cancel = sample;
        cancel.kind = wxWinUIInputKind::Cancel;
        cancel.device = owner.device;
        cancel.pointerId = owner.pointerId;
        cancel.button = owner.button;
        cancel.isPrimary = true;
        cancel.isCompatibilityMouse = false;
        cancel.interruptAll = false;
        wxWinUIRouteObservation observation;
        observation.surface = wxWinUIInputSurface::Indeterminate;
        const wxWinUIInputTransition transition =
            slot->m_inputState.Route(cancel, observation);
        for ( const wxWinUIInputAction& action : transition.actions )
        {
            if ( state->GetHost() != this ||
                 state->GetWindow() != window ||
                 FindSlot(window) != slot ||
                 slot->m_inputTransitionEpoch != epoch ||
                 !SendSlotMouseEvent(state, action) )
            {
                break;
            }
        }
        clearExactOwner();
    }
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::BindSlotEvents(wxWinUISlot& slot)
{
    using namespace winrt::Microsoft::UI::Xaml;
    using winrt::Windows::Foundation::IInspectable;

    // Weak-capture discipline: every handler captures only the shared
    // lifetime state (never raw wxWindow/host pointers, which a late
    // callback could dereference after free, and never the container, which
    // would create a Grid -> handler -> Grid retention cycle).  The
    // container comes from the sender argument instead.
    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;

    // All the pointer handlers are attached with handledEventsToo=true: many
    // XAML controls (Button among them) mark the pointer events as handled,
    // which would starve a plain subscription -- and thus the wx application
    // -- of the very clicks it is most interested in.  None of the handlers
    // sets Handled itself, so the hosted control keeps reacting as before.
    // The boxed delegates are recorded on the slot so DisconnectSlot() can
    // revoke them (AddHandler has no token-based removal).
    const auto addPointerHandler =
        [&slot](const RoutedEvent& routedEvent,
                Input::PointerEventHandler handler)
        {
            wxWinUIMaybeInjectHandlerFault();
            const auto boxed = winrt::box_value(std::move(handler));
            slot.m_container.AddHandler(routedEvent, boxed,
                                        true /* handledEventsToo */);
            slot.m_routedHandlers.emplace_back(routedEvent, boxed);
            ++gs_slotHandlerAdds;
        };

    const auto routeFromSender =
        [state](IInspectable const& sender,
                Input::PointerRoutedEventArgs const& e,
                wxWinUIInputKind kind)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            const auto container =
                sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            if ( !host || !container )
                return;

            host->OnSlotPointer(state, container, e, kind);
        };

    addPointerHandler(
        UIElement::PointerEnteredEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Enter);
        });

    addPointerHandler(
        UIElement::PointerExitedEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Leave);
        });

    addPointerHandler(
        UIElement::PointerMovedEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Move);
        });

    addPointerHandler(
        UIElement::PointerWheelChangedEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Wheel);
        });

    addPointerHandler(
        UIElement::PointerPressedEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Press);
        });

    addPointerHandler(
        UIElement::PointerReleasedEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Release);
        });

    addPointerHandler(
        UIElement::PointerCaptureLostEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::CaptureLost);
        });

    addPointerHandler(
        UIElement::PointerCanceledEvent(),
        [routeFromSender](IInspectable const& sender,
                          Input::PointerRoutedEventArgs const& e)
        {
            routeFromSender(sender, e, wxWinUIInputKind::Cancel);
        });

    // Do not subscribe to DoubleTapped: it is raised after the second raw
    // press/release pair. The per-slot state machine converts that second
    // press into DCLICK, producing Win32's DOWN, UP, DCLICK, UP ordering for
    // every supported button instead of appending a fifth event.

    // Focusability is a live wx invariant, not a property we can safely stamp
    // onto arbitrary application content.  Cancel routed focus acquisition at
    // the slot boundary when wx currently refuses it; keyboard/game-controller
    // navigation also honours DisableFocusFromKeyboard().
    {
        using GettingFocusHandler =
            winrt::Windows::Foundation::TypedEventHandler<
                UIElement, Input::GettingFocusEventArgs>;

        wxWinUIMaybeInjectHandlerFault();
        const auto boxed = winrt::box_value(GettingFocusHandler(
            [state](UIElement const& sender,
                    Input::GettingFocusEventArgs const& e)
            {
                wxWinUITopLevelHost * const callbackHost =
                    state->GetHost();
                wxWindow * const callbackWindow = state->GetWindow();
                wxWinUISlot * const callbackSlot =
                    callbackHost && callbackWindow
                        ? callbackHost->FindSlot(callbackWindow)
                        : nullptr;
                if ( !callbackHost || !callbackWindow || !callbackSlot ||
                     callbackSlot->m_lifetime != state ||
                     callbackSlot->m_container != sender )
                {
                    e.TryCancel();
                    return;
                }

                OperationGuard hostOperation(callbackHost);
                wxWinUISlot::OperationGuard slotOperation(callbackSlot);
                const auto getCurrentWindow = [&]() -> wxWindow *
                {
                    wxWinUITopLevelHost * const host = state->GetHost();
                    wxWindow * const window = state->GetWindow();
                    wxWinUISlot * const slot =
                        host && window ? host->FindSlot(window) : nullptr;
                    return slot &&
                                   slot->m_lifetime == state &&
                                   slot->m_container == sender
                               ? window
                               : nullptr;
                };

                wxWindow *window = getCurrentWindow();
                if ( !window )
                {
                    e.TryCancel();
                    return;
                }

                // Bind the native authority displaced on entry to this exact
                // routed source before crossing either virtual predicate. A
                // second GettingFocus attempt publishes a newer epoch, so the
                // deferred teardown of the first can never restore its HWND.
                if ( !callbackHost->m_focusOwner &&
                     callbackHost->HasNativeFocusAuthority() )
                {
                    if ( ++callbackHost->m_nextNativeFocusEntryEpoch == 0 )
                        ++callbackHost->m_nextNativeFocusEntryEpoch;
                    callbackHost->m_nativeFocusEntryTarget = state;
                    callbackHost->m_nativeFocusEntryEpoch =
                        callbackHost->m_nextNativeFocusEntryEpoch;
                    callbackSlot->m_nativeFocusEntryEpoch =
                        callbackHost->m_nativeFocusEntryEpoch;
                }

                bool allowed = window->CanAcceptFocus();
                window = getCurrentWindow();
                if ( !window )
                {
                    // The logical slot was retired by the predicate, but its
                    // physical XAML source remains attached until this
                    // callback returns, so cancelling is both necessary and
                    // safe. UnregisterSlot() performs the deferred disconnect.
                    e.TryCancel();
                    return;
                }

                const auto device = e.InputDevice();
                if ( allowed &&
                        (device == Input::FocusInputDeviceKind::Keyboard ||
                         device == Input::FocusInputDeviceKind::GameController) )
                {
                    allowed = window->CanAcceptFocusFromKeyboard();
                    if ( !getCurrentWindow() )
                    {
                        e.TryCancel();
                        return;
                    }
                }

                if ( !allowed )
                    e.TryCancel();
            }));
        slot.m_container.AddHandler(UIElement::GettingFocusEvent(), boxed,
                                    true /* handledEventsToo */);
        slot.m_routedHandlers.emplace_back(UIElement::GettingFocusEvent(),
                                           boxed);
        ++gs_slotHandlerAdds;
    }

    // Focus arbiter, XAML -> wx side: the island holds the native focus, so
    // slotted windows never get WM_SETFOCUS; synthesize the wx focus events
    // and track the owner for FindFocus() resolution.
    wxWinUIMaybeInjectHandlerFault();
    slot.m_gotFocusToken = slot.m_container.GotFocus(
        [state](IInspectable const& sender, RoutedEventArgs const&)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( !host )
                return;

            const auto container = sender.
                try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            host->OnXamlSlotGotFocus(state, container);
        });
    ++gs_slotHandlerAdds;

    wxWinUIMaybeInjectHandlerFault();
    slot.m_lostFocusToken = slot.m_container.LostFocus(
        [state](IInspectable const& sender, RoutedEventArgs const&)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( !host )
                return;

            const auto container = sender.
                try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            host->OnXamlSlotLostFocus(state, container);
        });
    ++gs_slotHandlerAdds;
}

// ============================================================================
// root router: island-first input, native forwarding over non-XAML areas
// ============================================================================

bool wxWinUITopLevelHost::BindRootRouter()
{
    using namespace winrt::Microsoft::UI::Xaml;
    using winrt::Windows::Foundation::IInspectable;

    // Same weak-capture discipline as the slot handlers: the root handlers
    // must not capture the host raw -- a dispatch racing the teardown would
    // touch a dying object -- so they capture the invalidatable host state.
    // They are recorded in m_rootHandlers and revoked by Shutdown() before
    // Close(), which also releases these state copies synchronously.
    const std::shared_ptr<wxWinUIHostLifetime> state = m_hostLifetime;

    const auto addHandler =
        [this](const RoutedEvent& routedEvent,
               Input::PointerEventHandler handler) -> bool
        {
            if ( !HasInitializationContext() )
                return false;

            const auto boxed = winrt::box_value(std::move(handler));
            m_rootHandlers.emplace_back(routedEvent, boxed);
            try
            {
                m_root.AddHandler(routedEvent, boxed,
                                  true /* handledEventsToo */);
            }
            catch ( ... )
            {
                m_rootHandlers.pop_back();
                throw;
            }
            ++gs_rootHandlerAdds;
            return HasInitializationContext();
        };

    if ( !addHandler(
        UIElement::PointerMovedEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointer(WM_MOUSEMOVE, e);
        }) )
    {
        return false;
    }

    if ( !addHandler(
        UIElement::PointerPressedEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointer(WM_LBUTTONDOWN, e);
        }) )
    {
        return false;
    }

    if ( !addHandler(
        UIElement::PointerReleasedEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointer(WM_LBUTTONUP, e);
        }) )
    {
        return false;
    }

    if ( !addHandler(
        UIElement::PointerWheelChangedEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointer(WM_MOUSEWHEEL, e);
        }) )
    {
        return false;
    }

    // These are state-machine inputs, not diagnostics-only events. Missing a
    // capture loss/cancel or root exit leaves synthetic hover/press state
    // alive indefinitely. Logging remains optional inside the handlers.
    if ( !addHandler(
        UIElement::PointerCaptureLostEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointerInterrupted(true, e);
        }) )
    {
        return false;
    }
    if ( !addHandler(
        UIElement::PointerCanceledEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointerInterrupted(false, e);
        }) )
    {
        return false;
    }
    if ( !addHandler(
        UIElement::PointerEnteredEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointerBoundary(true, e);
        }) )
    {
        return false;
    }
    if ( !addHandler(
        UIElement::PointerExitedEvent(),
        [state](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( host )
                host->OnRootPointerBoundary(false, e);
        }) )
    {
        return false;
    }

    if ( wxWinUIInputLogEnabled() && HasInitializationContext() )
    {
        wxWindow * const tlw = m_tlwLifetime.get();
        if ( !tlw )
            return false;
        tlw->Bind(wxEVT_MOVE_START, &wxWinUIInputLogTLWMoveStart);
        m_tlwMoveStartLogBound = true;
        if ( !HasInitializationContext() )
            return false;
        tlw->Bind(wxEVT_MOVE_END, &wxWinUIInputLogTLWMoveEnd);
        m_tlwMoveEndLogBound = true;
        if ( !HasInitializationContext() )
            return false;
        tlw->Bind(wxEVT_ACTIVATE, &wxWinUIInputLogTLWActivate);
        m_tlwActivateLogBound = true;
        if ( !HasInitializationContext() )
            return false;
    }

    // Do not translate XAML DoubleTapped separately. It is raised after the
    // pointer press/release sequence and forwarding it here would produce
    // DOWN, UP, DOWN, UP, DBLCLK instead of Win32's
    // DOWN, UP, DBLCLK, UP. OnRootPointer() owns the click sequence and turns
    // the second press into the corresponding WM_*BUTTONDBLCLK message.
    return HasInitializationContext();
}

void wxWinUITopLevelHost::OnRootPointer(
    UINT message,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long eventEpoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    try
    {
        const wxWinUIProfileScope profileTotal(wxWinUIProfilePhase::Total);
        const long long pointStarted = wxWinUIProfileNow();
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        wxWinUIProfileAdd(wxWinUIProfilePhase::Point, pointStarted);
        bool sourceOwnedByXaml = true;
        try
        {
            const auto source =
                args.OriginalSource().try_as<
                    winrt::Microsoft::UI::Xaml::UIElement>();
            sourceOwnedByXaml = !source || source != m_root;
        }
        catch ( const winrt::hresult_error& )
        {
        }
        const bool sourceAlreadyHandled =
            args.Handled() || sourceOwnedByXaml;
        if ( sourceOwnedByXaml && message == WM_MOUSEMOVE )
        {
            const wxWinUIProfileScope profileObserve(
                wxWinUIProfilePhase::Cursor);
            ObserveXamlPointerCursor();
        }

        WORD modifiers = 0;
        WORD buttonMask = 0;
        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            modifiers |= MK_CONTROL;
        if ( props.IsLeftButtonPressed() )
            buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            buttonMask |= MK_XBUTTON2;

        const double scale = GetScale();
        const auto position = point.Position();
        const wxPoint pointPx(
            static_cast<int>(std::lround(position.X * scale)),
            static_cast<int>(std::lround(position.Y * scale)));
        const RECT clientScreen = GetClientScreenRect();
        const POINT screen =
        {
            clientScreen.left + pointPx.x,
            clientScreen.top + pointPx.y
        };

        wxWinUIPointerSample sample;
        sample.device = wxWinUIGetInputDevice(point.PointerDeviceType());
        sample.pointerId = point.PointerId();
        sample.isPrimary = props.IsPrimary();
        sample.isCompatibilityMouse =
            sample.device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample.screenX = screen.x;
        sample.screenY = screen.y;
        // Keep the projection's exact root-space position for XAML hit
        // classification. The integer screen coordinates above are still
        // required by the Win32 dispatch contract, but reconstructing DIPs
        // from them would shift fractional-scale boundaries by up to half a
        // physical pixel.
        sample.hasRootDips = true;
        sample.rootXDIP = position.X;
        sample.rootYDIP = position.Y;
        sample.timestamp = point.Timestamp() / 1000;
        sample.modifiers = modifiers;

        if ( message == WM_MOUSEWHEEL )
        {
            sample.kind = wxWinUIInputKind::Wheel;
            sample.wheelDelta = props.MouseWheelDelta();
            sample.horizontalWheel = props.IsHorizontalMouseWheel();
        }
        else
        {
            const wxWinUIInputMapping mapping =
                wxWinUIMapPointerUpdate(wxWinUIGetInputUpdateKind(
                    props.PointerUpdateKind()));
            if ( mapping.valid )
            {
                sample.kind = mapping.kind;
                sample.button = mapping.button;
            }
            else if ( message == WM_MOUSEMOVE )
            {
                sample.kind = wxWinUIInputKind::Move;
            }
            else if ( sample.device != wxWinUIInputDevice::Mouse )
            {
                // V0 policy: one primary touch/pen contact is represented
                // exactly once as a mouse-compatible left-button gesture.
                sample.kind = message == WM_LBUTTONDOWN
                    ? wxWinUIInputKind::Press
                    : wxWinUIInputKind::Release;
                sample.button = wxWinUIInputButton::Left;
            }
            else
            {
                return;
            }
        }

        if ( sample.kind == wxWinUIInputKind::Press )
            buttonMask |= wxWinUIGetWParamButtonMask(sample.button);
        else if ( sample.kind == wxWinUIInputKind::Release )
            buttonMask &= ~wxWinUIGetWParamButtonMask(sample.button);
        sample.buttonMask = buttonMask;

        const wxWinUIRootPointerOutcome outcome =
            RouteRootPointerSampleAtEpoch(
                sample, sourceAlreadyHandled, eventEpoch);
        if ( sourceOwnedByXaml &&
             hostState && hostState->GetHost() == this &&
             !m_shuttingDown &&
             m_inputTransitionEpoch == eventEpoch )
        {
            // Apply this after the route: a transition from native space
            // first executes its balancing HoverLeave, whose cleanup resets
            // the old native cursor. A Move computes one position-sensitive
            // wxEVT_SET_CURSOR verdict; Press/Release/Wheel only re-assert
            // that generation-bound verdict, because XAML/capture can reset
            // InputPointerSource while the pointer has not moved.
            const wxWinUIProfileScope profileCursor(
                wxWinUIProfilePhase::Cursor);
            ApplyXamlPointerCursor(
                args,
                wxWinUIShouldEmitSetCursor(sample.kind),
                eventEpoch);
        }
        if ( outcome.cancelSource )
            args.Handled(true);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    wxWinUIProfileSample();
}

wxWinUIRootPointerOutcome
wxWinUITopLevelHost::TestRouteRootPointerSample(
    const wxWinUIPointerSample& sample,
    bool sourceAlreadyHandled)
{
    OperationGuard operation(this);
    wxWinUIRootPointerOutcome outcome;
    if ( m_shuttingDown || !m_tlw )
        return outcome;

    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    return RouteRootPointerSampleAtEpoch(
        sample, sourceAlreadyHandled, m_inputTransitionEpoch);
}

wxWinUIRootPointerOutcome
wxWinUITopLevelHost::RouteRootPointerSampleAtEpoch(
    const wxWinUIPointerSample& sample,
    bool sourceAlreadyHandled,
    unsigned long long eventEpoch)
{
    wxWinUIRootPointerOutcome outcome;
    const auto resize = m_nativeResize;
    if ( resize && sample.device == resize->sample.device &&
         sample.pointerId == resize->sample.pointerId )
    {
        // The grip's accepted press bubbles to the root after its own
        // callback. It is the same gesture, not a superseding DOWN: do not
        // advance the InputSite generation or route a second owner for it.
        if ( (sample.kind == wxWinUIInputKind::Press &&
              sample.button == wxWinUIInputButton::Left &&
              sample.timestamp == resize->sample.timestamp) ||
             sample.kind == wxWinUIInputKind::Move )
        {
            outcome.status = wxWinUIRootPointerStatus::Completed;
            outcome.disposition = wxWinUIInputDisposition::Routed;
            outcome.cancelSource = true;
            return outcome;
        }
        if ( sample.kind == wxWinUIInputKind::Release &&
             sample.button == wxWinUIInputButton::Left )
        {
            CancelNativeResize(sample);
            outcome.status = wxWinUIRootPointerStatus::Completed;
            outcome.disposition = wxWinUIInputDisposition::Routed;
            outcome.cancelSource = true;
            return outcome;
        }
    }
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto hostIsLive = [this, &hostState]()
    {
        return hostState &&
               hostState->GetHost() == this &&
               !m_shuttingDown;
    };
    const auto abortStatus =
        [this, &hostIsLive](unsigned long long expectedEpoch)
        {
            return !hostIsLive() ||
                   m_inputTransitionEpoch != expectedEpoch
                ? wxWinUIRootPointerStatus::Superseded
                : wxWinUIRootPointerStatus::FailedClosed;
        };

    const POINT screen = { sample.screenX, sample.screenY };
    const RECT clientScreen = GetClientScreenRect();
    m_lastPointerScreen = screen;
    const auto eventRoot = m_root;
    const auto eventXamlRoot = GetXamlRoot();
    const double eventScale = GetScale();

    // Apply the device policy before any host-wide side effect. A promoted
    // compatibility UP must not cancel a queued real-mouse modal request or
    // release a slot owner.
    if ( sample.device != wxWinUIInputDevice::Mouse &&
         !sample.isPrimary )
    {
        outcome.status = wxWinUIRootPointerStatus::PolicyFiltered;
        outcome.disposition =
            wxWinUIInputDisposition::SecondaryPointer;
        return outcome;
    }
    if ( sample.isCompatibilityMouse )
    {
        outcome.status = wxWinUIRootPointerStatus::PolicyFiltered;
        outcome.disposition =
            wxWinUIInputDisposition::CompatibilityDuplicate;
        return outcome;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    wxWinUIInputAction superseded;
    if ( sample.kind == wxWinUIInputKind::Press )
    {
        superseded = TakeModalAwaitingRelease(
            sample.device, sample.pointerId, sample.button);
        if ( !superseded.gestureSerial &&
             m_nativeInputDispatchInFlight &&
             m_nativeInputDispatchCommitted &&
             m_inFlightNativeInput.action.gestureSerial &&
             m_inFlightNativeInput.device == sample.device &&
             m_inFlightNativeInput.pointerId == sample.pointerId &&
             m_inFlightNativeInput.action.button == sample.button )
        {
            // A system-modal loop consumed the old UP and pumped the next
            // DOWN before its SendMessage unwound.
            superseded = m_inFlightNativeInput.action;
        }
    }

    if ( superseded.gestureSerial &&
         m_inputState.HasActiveGesture(superseded) )
    {
        // Retire the exact old modal serial before admitting the
        // corresponding new physical DOWN.
        wxWinUIPointerSample cancel = sample;
        cancel.kind = wxWinUIInputKind::Cancel;
        cancel.button = superseded.button;
        cancel.buttonMask &=
            ~wxWinUIGetWParamButtonMask(superseded.button);
        const wxWinUIInputTransition cancellation =
            m_inputState.CancelActiveGesture(superseded, cancel);

        unsigned long long expectedEpoch = m_inputTransitionEpoch;
        if ( !cancellation.actions.empty() )
        {
            if ( ++expectedEpoch == 0 )
                ++expectedEpoch;
        }

        if ( !ExecuteInputTransition(cancellation) ||
             !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            outcome.status = abortStatus(expectedEpoch);
            return outcome;
        }
        eventEpoch = m_inputTransitionEpoch;
    }

    CancelPendingNativeDispatch(sample);
    if ( sample.kind == wxWinUIInputKind::Release &&
         DispatchSlotOwnedRelease(sample) )
    {
        outcome.status = abortStatus(eventEpoch);
        if ( outcome.status ==
                wxWinUIRootPointerStatus::FailedClosed )
        {
            outcome.status =
                wxWinUIRootPointerStatus::SlotReleaseConsumed;
        }
        outcome.disposition = wxWinUIInputDisposition::Routed;
        outcome.cancelSource = true;
        RetryDeferredIslandCancelMode();
        return outcome;
    }

    wxWinUINativeHit nativeHit;
    bool hasNativeHit = false;
    wxWinUIRouteObservation observation;
    const winrt::Windows::Foundation::Point exactRootDips{
        static_cast<float>(sample.rootXDIP),
        static_cast<float>(sample.rootYDIP)
    };
    const winrt::Windows::Foundation::Point * const exactRootDipsPtr =
        sample.hasRootDips ? &exactRootDips : nullptr;
    bool exactRootDipsRemainValid = exactRootDipsPtr != nullptr;
    const auto classifySurface =
        [this, &hostIsLive, &screen, &clientScreen, &eventRoot,
         &eventXamlRoot, eventScale, exactRootDipsPtr,
         &exactRootDipsRemainValid,
         sourceAlreadyHandled,
         &nativeHit, &hasNativeHit, &observation]()
        {
            const RECT currentClientScreen = GetClientScreenRect();
            const wxPoint currentPointPx(
                screen.x - currentClientScreen.left,
                screen.y - currentClientScreen.top);
            const bool exactIsCurrent =
                exactRootDipsRemainValid &&
                m_root == eventRoot &&
                GetXamlRoot() == eventXamlRoot &&
                GetScale() == eventScale &&
                currentClientScreen.left == clientScreen.left &&
                currentClientScreen.top == clientScreen.top &&
                currentClientScreen.right == clientScreen.right &&
                currentClientScreen.bottom == clientScreen.bottom;
            return ClassifyRootPointer(
                       currentPointPx, screen,
                       exactIsCurrent ? exactRootDipsPtr : nullptr,
                       sourceAlreadyHandled,
                       &nativeHit, &observation, &hasNativeHit) &&
                   hostIsLive();
        };

    // Visual classification is deliberately first: neither capture
    // bookkeeping nor the breaker may turn an event over real XAML into a
    // native hit by assumption.
    if ( !classifySurface() ||
         m_inputTransitionEpoch != eventEpoch )
    {
        outcome.status = abortStatus(eventEpoch);
        return outcome;
    }

    bool captureActionsRan = false;
    bool captureSuperseded = false;
    if ( !SyncNativeCapture(
             sample, nullptr, &captureActionsRan, nullptr,
             &captureSuperseded) )
    {
        outcome.status = captureSuperseded || !hostIsLive()
            ? wxWinUIRootPointerStatus::Superseded
            : wxWinUIRootPointerStatus::FailedClosed;
        return outcome;
    }
    eventEpoch = m_inputTransitionEpoch;

    // Capture loss/transfer can synchronously invoke application code.
    // Reclassify rather than continuing with pre-callback geometry.
    if ( captureActionsRan )
    {
        // CaptureChanged is an arbitrary application callback boundary. Even
        // when the XamlRoot object survives, layout/DPI may have advanced
        // without preserving the event-local DIP sample. Reproject the
        // authoritative screen point against the current host.
        exactRootDipsRemainValid = false;
        if ( !classifySurface() ||
             m_inputTransitionEpoch != eventEpoch )
        {
            outcome.status = abortStatus(eventEpoch);
            return outcome;
        }
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        m_inputState.DisarmStormGuard();

    const wxWinUIInputTransition transition =
        m_inputState.Route(sample, observation);
    outcome.disposition = transition.disposition;
    outcome.stormDecision = transition.stormDecision;
    outcome.preparedActionCount =
        static_cast<unsigned>(transition.actions.size());
    outcome.cancelSource = transition.cancelSource;
    for ( const wxWinUIInputAction& action : transition.actions )
    {
        if ( action.action == wxWinUIInputActionKind::Dispatch &&
             (action.kind == wxWinUIInputKind::Press ||
              action.kind == wxWinUIInputKind::DoubleClick ||
              action.kind == wxWinUIInputKind::Release) )
        {
            outcome.pointerDispatch = action;
            break;
        }
    }

    unsigned long long expectedEpoch = m_inputTransitionEpoch;
    if ( !transition.actions.empty() )
    {
        if ( ++expectedEpoch == 0 )
            ++expectedEpoch;
    }

    bool transitionSuperseded = false;
    const long long dispatchStarted = wxWinUIProfileNow();
    const bool executed = ExecuteInputTransition(
             transition,
             hasNativeHit ? &nativeHit : nullptr,
             &transitionSuperseded);
    wxWinUIProfileAdd(wxWinUIProfilePhase::Dispatch, dispatchStarted);
    if ( !executed )
    {
        outcome.status = transitionSuperseded
            ? wxWinUIRootPointerStatus::Superseded
            : abortStatus(expectedEpoch);
        return outcome;
    }

    outcome.status = wxWinUIRootPointerStatus::Completed;
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        static unsigned s_moveCount = 0;
        if ( sample.kind != wxWinUIInputKind::Move ||
             (++s_moveCount % 8) == 0 )
        {
            wxWinUIInputLog(
                "root kind=%d surface=%d pt=(%ld,%ld) actions=%u",
                static_cast<int>(sample.kind),
                static_cast<int>(observation.surface),
                screen.x, screen.y,
                static_cast<unsigned>(transition.actions.size()));
        }
    }
    return outcome;
}

wxWinUITopLevelHost::XamlHitResolution
wxWinUITopLevelHost::PointOverXamlContent(
    const wxPoint& ptClientPx,
    const winrt::Windows::Foundation::Point *exactRootDips)
{
    if ( !m_tlw || !m_root || !m_hostLifetime )
        return XamlHitResolution::Unavailable;

    const std::shared_ptr<wxWinUIHostLifetime> hostState =
        m_hostLifetime;
    const auto root = m_root;
    const unsigned long long structureGeneration =
        m_structureGeneration;
    const auto current = [this, &hostState, &root, structureGeneration]()
    {
        return hostState && hostState->GetHost() == this &&
               !m_shuttingDown && m_root == root &&
               m_structureGeneration == structureGeneration;
    };

    winrt::Windows::Foundation::Point dip{};
    if ( exactRootDips )
    {
        dip = *exactRootDips;
        if ( !wxWinUIIsFinitePoint(dip) || !current() )
            return XamlHitResolution::Unavailable;
    }
    else if ( wxWinUIVisualCoordinates::ClientPointToRoot(
                  m_tlw,
                  winrt::Windows::Foundation::Point{
                      static_cast<float>(ptClientPx.x),
                      static_cast<float>(ptClientPx.y)},
                  &dip) != wxWinUICoordinateResult::Mapped ||
              !current() )
    {
        return XamlHitResolution::Unavailable;
    }

    try
    {
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const auto xamlRoot = root.XamlRoot();
        if ( !xamlRoot || !current() )
            return XamlHitResolution::Unavailable;

        // Note: there is deliberately NO "any open popup makes the island
        // solid everywhere" shortcut here.  A ContentDialog's smoke layer
        // and a flyout's content ARE hit-testable elements found by the
        // query below, so modality works through the normal path -- while
        // the shortcut turned a single stale popup (e.g. a flyout whose
        // closing was interrupted by a modal loop) into a permanently
        // input-dead window.
        //
        // The root canvas itself is hit-testable by design (it must receive
        // all the input): only count REAL content above it.  The query runs
        // against the XamlRoot content so popup layers are part of the walk.
        auto scope = xamlRoot.Content();
        if ( !scope )
            scope = root;
        if ( !current() )
            return XamlHitResolution::Unavailable;
        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(dip, scope);
        if ( !current() )
            return XamlHitResolution::Unavailable;
        for ( const auto& element : hits )
        {
            if ( !current() )
                return XamlHitResolution::Unavailable;
            if ( element != root )
                return XamlHitResolution::Hit;
        }

        return XamlHitResolution::Miss;
    }
    catch ( const winrt::hresult_error& )
    {
        return XamlHitResolution::Unavailable;
    }
}

void wxWinUITopLevelHost::OnTakeFocusRequested(bool previous)
{
    OperationGuard operation(this);

    // The island's own Tab navigation ran off its first/last element: hand
    // the focus to the neighbouring wx tab stop -- through the same
    // focusability-aware walk as the Tab pre-translation, so a window that
    // refuses the keyboard focus is never re-entered from this exit either.
    wxWindow * const current = m_focusOwner;
    const HWND hwndCurrent = current ? GetHwndOf(current) : nullptr;
    const HWND hwndTLW = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    if ( !hwndCurrent || !hwndTLW )
        return;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const wxWeakRef<wxWindow> currentWeak(current);
    const wxWeakRef<wxWindow> tlwWeak(m_tlw);
    const unsigned long long currentGeneration =
        wxWinUIMSWGetHwndGeneration(
            current, reinterpret_cast<WXHWND>(hwndCurrent));
    const unsigned long long tlwGeneration =
        wxWinUIMSWGetHwndGeneration(
            m_tlw, reinterpret_cast<WXHWND>(hwndTLW));
    if ( !currentGeneration || !tlwGeneration )
        return;

    const HWND next = wxWinUIFindNextKeyboardFocusable(hwndTLW,
                                                       hwndCurrent, previous);
    wxWindow * const liveCurrent = currentWeak.get();
    wxWindow * const liveTLW = tlwWeak.get();
    if ( !hostState || hostState->GetHost() != this || m_shuttingDown ||
         !liveCurrent || !liveTLW || m_focusOwner != liveCurrent ||
         wxWinUIMSWGetHwndGeneration(
             liveCurrent, reinterpret_cast<WXHWND>(hwndCurrent)) !=
             currentGeneration ||
         wxWinUIMSWGetHwndGeneration(
             liveTLW, reinterpret_cast<WXHWND>(hwndTLW)) != tlwGeneration )
    {
        return;
    }

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("TakeFocus prev=%d from=%p -> %p",
                        previous, static_cast<void *>(hwndCurrent),
                        static_cast<void *>(next));
    }

    if ( !next )
        return;

    const unsigned long long nextGeneration =
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(next));
    if ( !nextGeneration )
        return;

    wxWindow * const nextWin = wxGetWindowFromHWND((WXHWND)next);
    if ( nextWin && FindSlot(nextWin) )
        FocusSlot(nextWin);
    else if ( wxWinUIMSWGetNativeHwndGeneration(
                  reinterpret_cast<WXHWND>(next)) == nextGeneration )
        ::SetFocus(next);
}

wxWindow *wxWinUITopLevelHost::ResolveFocusHwnd(WXHWND hwnd)
{
    const HWND focus = static_cast<HWND>(hwnd);
    if ( !focus )
        return nullptr;

    for ( const auto& kv : gs_tlwHosts )
    {
        wxWinUITopLevelHost * const host = kv.second;
        if ( !host->IsReadyForLookup() || !host->m_bridge )
            continue;

        if ( focus == host->m_bridge || focus == host->m_inner ||
                ::IsChild(host->m_bridge, focus) )
        {
            return host->m_focusOwner ? host->m_focusOwner : host->m_tlw;
        }
    }

    return nullptr;
}

wxWindow *wxWinUITopLevelHost::ResolveFocusCounterpartHwnd(WXHWND hwnd)
{
    const HWND focus = static_cast<HWND>(hwnd);
    if ( !focus )
        return nullptr;

    for ( const auto& kv : gs_tlwHosts )
    {
        wxWinUITopLevelHost * const host = kv.second;
        if ( !host->IsReadyForLookup() || !host->m_bridge )
            continue;
        if ( focus != host->m_bridge && focus != host->m_inner &&
             !::IsChild(host->m_bridge, focus) )
        {
            continue;
        }

        const auto transfer = host->m_nativeFocusTransferTarget.lock();
        wxWindow * const transferWindow =
            transfer ? transfer->GetWindow() : nullptr;
        if ( transfer && transfer->GetHost() == host && transferWindow )
        {
            wxWinUISlot * const slot = host->FindSlot(transferWindow);
            if ( slot && slot->m_lifetime == transfer )
                return transferWindow;
        }

        const auto departure = host->m_nativeFocusDepartureSource.lock();
        const HWND departureDestination =
            static_cast<HWND>(host->m_nativeFocusDepartureDestination);
        const unsigned long long departureGeneration =
            host->m_nativeFocusDepartureDestinationGeneration;
        // Consume this identity exactly once. Even a failed validation must
        // retire it: carrying an old slot into a later unrelated HWND focus
        // transition would be worse than returning the conservative TLW.
        host->m_nativeFocusDepartureSource.reset();
        host->m_nativeFocusDepartureDestination = nullptr;
        host->m_nativeFocusDepartureDestinationGeneration = 0;
        if ( departure && departure->GetHost() == host &&
             departureDestination &&
             ::GetFocus() == departureDestination &&
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(departureDestination)) ==
                 departureGeneration )
        {
            wxWindow * const departureWindow = departure->GetWindow();
            wxWinUISlot * const slot =
                departureWindow ? host->FindSlot(departureWindow) : nullptr;
            if ( slot && slot->m_lifetime == departure )
                return departureWindow;
        }

        return host->m_focusOwner ? host->m_focusOwner : host->m_tlw;
    }

    return nullptr;
}

bool wxWinUITopLevelHost::ShouldSuppressNativeKillFocus(
    wxWindow *window,
    WXHWND destination)
{
    wxWinUITopLevelHost * const host = FindSlotOwner(window);
    const HWND target = static_cast<HWND>(destination);
    const auto transfer =
        host ? host->m_nativeFocusTransferTarget.lock() : nullptr;
    return host &&
           transfer && transfer->GetHost() == host &&
           transfer->GetWindow() == window &&
           host->FindSlot(window) &&
           host->FindSlot(window)->m_lifetime == transfer &&
           target &&
           (target == host->m_bridge ||
            (host->m_bridge && ::IsChild(host->m_bridge, target)));
}

namespace
{

constexpr LONG wxWINUI_OBJID_CLIENT = -4;
constexpr LONG wxWINUI_UIA_ROOT_OBJECT_ID = -25;
constexpr wchar_t wxWINUI_UIA_SHELL_PROVIDER_MARKER[] =
    L"wxWidgets.WinUI.InvisibleShellProviderExposed";

class wxWinUIInvisibleShellProvider final
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::ClassicCom>,
          IRawElementProviderSimple>
{
public:
    wxWinUIInvisibleShellProvider(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        HWND hwnd,
        unsigned long long hwndGeneration,
        unsigned long long contentGeneration,
        unsigned long long authorityGeneration)
        : m_state(state),
          m_hwnd(hwnd),
          m_hwndGeneration(hwndGeneration),
          m_contentGeneration(contentGeneration),
          m_authorityGeneration(authorityGeneration)
    {
        gs_liveInvisibleShellProviders.fetch_add(
            1, std::memory_order_relaxed);
    }

    ~wxWinUIInvisibleShellProvider() override
    {
        gs_liveInvisibleShellProviders.fetch_sub(
            1, std::memory_order_relaxed);
    }

    IFACEMETHODIMP get_ProviderOptions(
        ProviderOptions *options) noexcept override
    {
        if ( !options )
            return E_POINTER;

        *options = static_cast<ProviderOptions>(
            ProviderOptions_ServerSideProvider |
            ProviderOptions_UseComThreading);
        return S_OK;
    }

    IFACEMETHODIMP GetPatternProvider(
        PATTERNID,
        IUnknown **provider) noexcept override
    {
        if ( !provider )
            return E_POINTER;
        *provider = nullptr;
        return IsCurrent() ? S_OK : UIA_E_ELEMENTNOTAVAILABLE;
    }

    IFACEMETHODIMP GetPropertyValue(
        PROPERTYID propertyId,
        VARIANT *propertyValue) noexcept override
    {
        if ( !propertyValue )
            return E_POINTER;
        ::VariantInit(propertyValue);

        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;

        switch ( propertyId )
        {
            case UIA_IsControlElementPropertyId:
            case UIA_IsContentElementPropertyId:
            case UIA_IsKeyboardFocusablePropertyId:
            case UIA_HasKeyboardFocusPropertyId:
                propertyValue->vt = VT_BOOL;
                propertyValue->boolVal = VARIANT_FALSE;
                break;

            case UIA_NamePropertyId:
                propertyValue->vt = VT_BSTR;
                propertyValue->bstrVal = ::SysAllocString(L"");
                if ( !propertyValue->bstrVal )
                {
                    propertyValue->vt = VT_EMPTY;
                    return E_OUTOFMEMORY;
                }
                break;

            default:
                // VT_EMPTY means that this provider does not supply the
                // property. The explicit Control/Content/Name values above
                // prevent UIA from promoting the HWND proxy into narration.
                break;
        }

        return S_OK;
    }

    IFACEMETHODIMP get_HostRawElementProvider(
        IRawElementProviderSimple **provider) noexcept override
    {
        if ( !provider )
            return E_POINTER;
        *provider = nullptr;

        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;

        Microsoft::WRL::ComPtr<IRawElementProviderSimple> hostProvider;
        const HRESULT hr = ::UiaHostProviderFromHwnd(
            m_hwnd, hostProvider.GetAddressOf());

        // UIA can synchronously consult provider state while resolving the
        // HWND proxy. Publish only if this exact shell/content/accessibility
        // generation still owns the result after that boundary.
        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;
        if ( FAILED(hr) )
            return hr;

        return hostProvider.CopyTo(provider);
    }

private:
    bool IsCurrent() const noexcept
    {
        const auto state = m_state.lock();
        if ( !state )
            return false;

        wxWindow * const window = state->GetWindow();
        wxWinUITopLevelHost * const host = state->GetHost();
        if ( !window || !host )
            return false;

        std::shared_ptr<wxWinUISlotLifetime> currentState;
        WXHWND currentHwnd = nullptr;
        unsigned long long currentGeneration = 0;
        unsigned long long contentGeneration = 0;
        bool hasWxAccessible = false;
        unsigned long long authorityGeneration = 0;
        return host->TryGetAccessibilityShellState(
                   window,
                   &currentState,
                   &currentHwnd,
                   &currentGeneration,
                   &contentGeneration,
                   &hasWxAccessible,
                   &authorityGeneration) &&
               currentState.get() == state.get() &&
               static_cast<HWND>(currentHwnd) == m_hwnd &&
               currentGeneration == m_hwndGeneration &&
               contentGeneration == m_contentGeneration &&
               !hasWxAccessible &&
               authorityGeneration == m_authorityGeneration;
    }

    std::weak_ptr<wxWinUISlotLifetime> m_state;
    HWND m_hwnd = nullptr;
    unsigned long long m_hwndGeneration = 0;
    unsigned long long m_contentGeneration = 0;
    unsigned long long m_authorityGeneration = 0;
};

bool wxWinUITryGetInvisibleShellState(
    wxWindow *window,
    std::shared_ptr<wxWinUISlotLifetime> *state,
    WXHWND *hwnd,
    unsigned long long *generation,
    unsigned long long *contentGeneration,
    bool *hasWxAccessible,
    unsigned long long *authorityGeneration)
{
    if ( !window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    return host &&
           host->TryGetAccessibilityShellState(
               window, state, hwnd, generation,
               contentGeneration, hasWxAccessible,
               authorityGeneration);
}

} // anonymous namespace

WXDLLIMPEXP_CORE HRESULT
wxWinUITestCreateInvisibleShellProvider(
    wxWindow *window,
    IRawElementProviderSimple **provider)
{
    if ( !provider )
        return E_POINTER;
    *provider = nullptr;

    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwndValue = nullptr;
    unsigned long long hwndGeneration = 0;
    unsigned long long contentGeneration = 0;
    bool hasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    if ( !wxWinUITryGetInvisibleShellState(
             window, &state, &hwndValue, &hwndGeneration,
             &contentGeneration, &hasWxAccessible,
             &authorityGeneration) ||
         hasWxAccessible )
    {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }

    auto shell = Microsoft::WRL::Make<
        wxWinUIInvisibleShellProvider>(
            state,
            static_cast<HWND>(hwndValue),
            hwndGeneration,
            contentGeneration,
            authorityGeneration);
    return shell ? shell.CopyTo(provider) : E_OUTOFMEMORY;
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostIsInvisibleAccessibilityShell(wxWindow *window)
{
    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwnd = nullptr;
    unsigned long long generation = 0;
    unsigned long long contentGeneration = 0;
    bool hasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    return wxWinUITryGetInvisibleShellState(
        window, &state, &hwnd, &generation,
        &contentGeneration, &hasWxAccessible,
        &authorityGeneration);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostHandleShellGetObject(wxWindow *window,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam,
                                   bool hasWxAccessible,
                                   WXLRESULT *result)
{
    if ( !result || hasWxAccessible )
        return false;

    const LONG objectId = static_cast<LONG>(lParam);
    if ( objectId != wxWINUI_OBJID_CLIENT &&
            objectId != wxWINUI_UIA_ROOT_OBJECT_ID )
    {
        return false;
    }

    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwndValue = nullptr;
    unsigned long long generation = 0;
    unsigned long long contentGeneration = 0;
    bool stateHasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    if ( !wxWinUITryGetInvisibleShellState(
             window, &state, &hwndValue, &generation,
             &contentGeneration, &stateHasWxAccessible,
             &authorityGeneration) )
    {
        return false;
    }
    if ( stateHasWxAccessible )
        return false;

    if ( objectId == wxWINUI_OBJID_CLIENT )
    {
        // The authoritative accessible object is the XAML peer. Returning a
        // native MSAA proxy here would add the covered HWND shell as a second
        // control; the UiaRoot path below supplies the explicit Raw-only host.
        *result = 0;
        return true;
    }

    const HWND hwnd = static_cast<HWND>(hwndValue);
    auto provider = Microsoft::WRL::Make<
        wxWinUIInvisibleShellProvider>(
            state, hwnd, generation, contentGeneration,
            authorityGeneration);
    if ( !provider )
    {
        *result = 0;
        return true;
    }

    // UI Automation retains per-HWND server-provider state after the return
    // below. Mark only exact shells that reached this path so WM_DESTROY can
    // explicitly retire that state before USER32 recycles the handle.
    if ( !::SetPropW(
             hwnd,
             wxWINUI_UIA_SHELL_PROVIDER_MARKER,
             reinterpret_cast<HANDLE>(static_cast<uintptr_t>(1))) )
    {
        *result = 0;
        return true;
    }

    *result = static_cast<WXLRESULT>(
        ::UiaReturnRawElementProvider(
            hwnd,
            static_cast<WPARAM>(wParam),
            static_cast<LPARAM>(lParam),
            provider.Get()));
    if ( !*result )
        ::RemovePropW(hwnd, wxWINUI_UIA_SHELL_PROVIDER_MARKER);
    return true;
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostRetireAccessibilityShellProvider(WXHWND hwndValue)
{
    const HWND hwnd = static_cast<HWND>(hwndValue);
    if ( !hwnd ||
         !::RemovePropW(hwnd, wxWINUI_UIA_SHELL_PROVIDER_MARKER) )
    {
        return;
    }

    ++gs_accessibilityShellProviderRetires;
    (void)::UiaReturnRawElementProvider(hwnd, 0, 0, nullptr);
}

WXDLLIMPEXP_CORE unsigned
wxWinUITestGetAccessibilityShellProviderRetireCount()
{
    return gs_accessibilityShellProviderRetires;
}

WXDLLIMPEXP_CORE unsigned
wxWinUITestGetLiveInvisibleShellProviderCount()
{
    return gs_liveInvisibleShellProviders.load(
        std::memory_order_relaxed);
}

WXDLLIMPEXP_CORE wxWindow *wxWinUITLWHostResolveFocus(WXHWND hwnd)
{
    return wxWinUITopLevelHost::ResolveFocusHwnd(hwnd);
}

WXDLLIMPEXP_CORE wxWindow *
wxWinUITLWHostResolveFocusCounterpart(WXHWND hwnd)
{
    return wxWinUITopLevelHost::ResolveFocusCounterpartHwnd(hwnd);
}

WXDLLIMPEXP_CORE wxWindow *
wxWinUITLWHostResolveWindowAtPoint(WXHWND hwndValue,
                                   const wxPoint& screenPoint)
{
    if ( !wxIsMainThread() )
        return nullptr;

    HWND hwnd = reinterpret_cast<HWND>(hwndValue);
    for ( unsigned depth = 0; hwnd && depth != 64; ++depth )
    {
        if ( wxWinUITopLevelHost * const host =
                 wxWinUITopLevelHost::FindForBridge(hwnd) )
        {
            return host->ResolveWindowAtScreenPoint(screenPoint);
        }

        const HWND parent = ::GetParent(hwnd);
        if ( !parent || parent == hwnd )
            break;
        hwnd = parent;
    }

    return nullptr;
}

WXDLLIMPEXP_CORE void *
wxWinUITLWHostBeginWindowEventDispatch(const wxWindowBase *windowBase)
{
    // wxThreadEvent may be processed synchronously by a worker with a window
    // as its event object. The island registries are GUI-thread confined.
    if ( !wxIsMainThread() )
        return nullptr;

    wxWindow * const window =
        wxDynamicCast(const_cast<wxWindowBase *>(windowBase), wxWindow);
    if ( !window )
        return nullptr;

    // Slot ownership is authoritative during a cross-TLW reparent. Fall back
    // to the TLW registry for events belonging to the frame itself or to a
    // native child which has no XAML slot.
    wxWindow * const tlw =
        window->IsTopLevel() ? window : wxGetTopLevelParent(window);
    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( !host )
        host = wxWinUITopLevelHost::FindForTLW(tlw ? tlw : window);

    if ( !host || host->m_shutdownComplete || host->m_shutdownFinalizing )
    {
        // A retained XAML peer can dispatch after wxEVT_DESTROY performed the
        // logical Shutdown() and removed gs_tlwHosts. The top-level object and
        // physical DesktopWindowXamlSource can still be pending. Preserve the
        // process-wide shield even though there is no longer a host-local
        // operation to increment.
        if ( tlw &&
             (tlw->IsBeingDeleted() ||
              wxPendingDelete.Member(tlw) ||
              wxWinUIHasDeferredTLWDestroy(tlw)) )
        {
            wxWinUIBeginGlobalOperation();
            return gs_globalOnlyWindowEventCookie;
        }
        return nullptr;
    }

    ++host->m_operationDepth;
    wxWinUIBeginGlobalOperation();
    return host;
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostEndWindowEventDispatch(void *cookie)
{
    if ( cookie == gs_globalOnlyWindowEventCookie )
    {
        wxWinUIEndGlobalOperation();
        return;
    }

    static_cast<wxWinUITopLevelHost *>(cookie)->EndOperation();
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostDeferTopLevelDestroy(wxWindow *window)
{
    if ( !window || !wxIsMainThread() )
        return false;

    // A modal dialog can already be logically destroyed and temporarily
    // removed from wxPendingDelete when application code calls Destroy()
    // again inside the retained callback. Treat the queued request as
    // authoritative before IsBeingDeleted()/host lookup, otherwise the base
    // path re-appends it and a nested wxYield() destroys it immediately.
    if ( gs_winuiOperationDepth != 0 &&
         wxWinUIHasDeferredTLWDestroy(window) )
    {
        return true;
    }

    if ( window->IsBeingDeleted() )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(window);
    if ( !host || host->m_shuttingDown )
        return false;

    if ( gs_winuiOperationDepth == 0 )
        return false;

    wxWinUIQueueDeferredTLWDestroy(
        window,
        wxWinUIDeferredTLWDestroyMode::CompleteBaseDestroy);

    return true;
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostDeferModalDialogDestroy(wxWindow *window)
{
    if ( !window || !wxIsMainThread() || window->IsBeingDeleted() )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(window);
    if ( !host || host->m_shuttingDown ||
         gs_winuiOperationDepth == 0 )
    {
        return false;
    }

    wxWinUIQueueDeferredTLWDestroy(
        window,
        wxWinUIDeferredTLWDestroyMode::CompleteModalDestroy);
    return true;
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostHasDeferredDestroyExact(const wxWindow *window)
{
    return wxWinUIHasDeferredTLWDestroy(window);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostIsDestroyScheduled(const wxWindow *window)
{
    if ( !window )
        return false;

    return window->IsBeingDeleted() ||
           wxPendingDelete.Member(const_cast<wxWindow *>(window)) ||
           wxWinUIHasDeferredTLWDestroy(window);
}

WXDLLIMPEXP_CORE wxWinUIDestroyDeferralResult
wxWinUITLWHostDeferPopupDestroy(
    wxWindow *window,
    wxWinUIPopupDestroySemantics semantics)
{
    if ( !window || !wxIsMainThread() )
        return wxWinUIDestroyDeferralResult::NotDeferred;

    // Detect the private queue before consulting IsBeingDeleted()/host state:
    // a transient popup's second public Destroy() must remain an error while
    // its first request is waiting outside wxPendingDelete.
    if ( wxWinUIHasDeferredTLWDestroy(window) )
        return wxWinUIDestroyDeferralResult::AlreadyDeferred;

    if ( window->IsBeingDeleted() || gs_winuiOperationDepth == 0 )
        return wxWinUIDestroyDeferralResult::NotDeferred;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(window);
    if ( !host || host->m_shuttingDown )
        return wxWinUIDestroyDeferralResult::NotDeferred;

    wxWinUIQueueDeferredTLWDestroy(
        window,
        semantics == wxWinUIPopupDestroySemantics::Immediate
            ? wxWinUIDeferredTLWDestroyMode::
                  CompletePopupImmediateDestroy
            : wxWinUIDeferredTLWDestroyMode::
                  CompletePopupPendingDestroy);
    return wxWinUIDestroyDeferralResult::Deferred;
}

WXDLLIMPEXP_CORE bool wxWinUITLWHostSetFocus(wxWindow *window)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    wxWinUISlot * const slot = host ? host->FindSlot(window) : nullptr;
    if ( !slot )
        return false;

    // A public SetFocus() on an unrealized child must retain the ordinary MSW
    // contract: its shell HWND becomes the truthful, synchronous focus owner.
    // The resulting WM_SETFOCUS path records the wx events and asks this host
    // to hand authority to XAML once the exact content generation is Loaded.
    // Swallowing the request here would leave both native and logical focus on
    // the previous control until an unrelated future flush.
    if ( !slot->IsContentLoaded() )
    {
        const HWND shell = GetHwndOf(window);
        if ( shell && ::IsWindow(shell) && ::GetFocus() == shell )
        {
            // Content A may have recorded a deferred intent and then been
            // replaced by an unrealized B while the same HWND kept native
            // focus. USER32 emits no second WM_SETFOCUS in that case, so
            // refresh the exact slot/content generation here without
            // duplicating the wx SET_FOCUS event already published for A.
            host->HandleNativeShellFocus(window, window);
            return true;
        }
        return false;
    }

    // Public wx focus is a logical request. Preflight focusability, Loaded and
    // the current XAML candidate before touching the invisible shell; native
    // dialog/navigation code which really focuses that HWND still enters via
    // HandleSetFocus() and the post-event hand-off below.
    host->FocusSlot(window);
    return true;
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostConsumeNativeFocusRollback(
    wxWindow *window,
    WXHWND shell,
    unsigned long long shellGeneration)
{
    if ( !window || !shell || !shellGeneration ||
         !::IsWindow(static_cast<HWND>(shell)) ||
         wxWinUIMSWGetHwndGeneration(window, shell) != shellGeneration ||
         wxFindWinFromHandle(static_cast<HWND>(shell)) != window ||
         ::GetFocus() != static_cast<HWND>(shell) ||
         window->IsBeingDeleted() )
    {
        return false;
    }

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    return host && host->FindSlot(window) &&
           host->ConsumeNativeFocusRollback(window);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostConsumeMigrationShellFocus(
    wxWindow *window,
    WXHWND shell,
    unsigned long long shellGeneration)
{
    if ( !window || !shell || !shellGeneration ||
         !::IsWindow(static_cast<HWND>(shell)) ||
         wxWinUIMSWGetHwndGeneration(window, shell) != shellGeneration ||
         wxFindWinFromHandle(static_cast<HWND>(shell)) != window ||
         window->GetHWND() != shell ||
         window->IsBeingDeleted() )
    {
        return false;
    }

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    return host && host->FindSlot(window) &&
           host->ConsumeMigrationShellFocus(window);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostNoteMigrationShellFocusDeparture(
    wxWindow *window,
    WXHWND destination)
{
    if ( !window || window->IsBeingDeleted() )
        return false;

    return wxWinUITopLevelHost::NoteMigrationShellFocusDeparture(
        window, destination);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostAfterNativeSetFocus(wxWindow *window,
                                  WXHWND shell,
                                  unsigned long long shellGeneration,
                                  wxWindow *previous)
{
    if ( !window || !shell || !shellGeneration ||
         !::IsWindow(static_cast<HWND>(shell)) ||
         wxWinUIMSWGetHwndGeneration(window, shell) != shellGeneration ||
         wxFindWinFromHandle(static_cast<HWND>(shell)) != window ||
         window->IsBeingDeleted() )
    {
        return;
    }

    const HWND nativeFocus = ::GetFocus();
    const HWND shellHwnd = static_cast<HWND>(shell);
    if ( !nativeFocus ||
         (nativeFocus != shellHwnd &&
          wxGetWindowFromHWND(
              reinterpret_cast<WXHWND>(nativeFocus)) != window) )
    {
        // An application SET_FOCUS handler redirected native focus. Its
        // decision is authoritative; never pull focus back into the island.
        return;
    }

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( !host || !host->FindSlot(window) )
        return;

    host->HandleNativeShellFocus(window, previous);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostShouldSuppressNativeKillFocus(wxWindow *window,
                                            WXHWND destination)
{
    return wxWinUITopLevelHost::ShouldSuppressNativeKillFocus(window,
                                                              destination);
}

// ============================================================================
// wx event plumbing
// ============================================================================

void wxWinUITopLevelHost::OnSlotWindowGeometry(wxEvent& event)
{
    event.Skip();

    wxWindow * const window = wxDynamicCast(event.GetEventObject(), wxWindow);
    if ( window && m_slots.count(window) )
    {
        wxWinUISlot * const slot = FindSlot(window);
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
        InvalidateStructure();
        MarkDirty(window);
    }
}

void wxWinUITopLevelHost::OnSlotWindowDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    // wxWindowDestroyEvent propagates upwards, so this may be about a child
    // of the window the handler is bound to: only act on tracked windows.
    wxWindow * const window = event.GetWindow();
    // A frame-class dialog registers the TLW itself as a slot.  Its tree must
    // remain attached until DesktopWindowXamlSource::Close(); OnTLWDestroy()
    // owns that teardown exclusively.
    if ( window && window != m_tlw && m_slots.count(window) )
        UnregisterSlot(window);
}

void wxWinUITopLevelHost::OnTLWSize(wxSizeEvent& event)
{
    event.Skip();
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
    }
    InvalidateStructure();
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWDpiChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && ++slot->m_cursorTopologyGeneration == 0 )
            ++slot->m_cursorTopologyGeneration;
    }

    // The island's RasterizationScale follows the monitor by itself; the
    // slot geometry (positions, sizes, clips, scrollbar cutouts) has to be
    // recomputed with the new scale.
    InvalidateStructure();
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWVisibility(wxEvent& event)
{
    event.Skip();

    // Show/iconize events can propagate in port-specific code. Only the TLW
    // controls whether all of this host's slots must be collapsed together.
    if ( event.GetEventObject() == m_tlw )
    {
        InvalidateStructure();
        MarkAllDirty();
    }
}

void wxWinUITopLevelHost::OnTLWDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    if ( event.GetWindow() != m_tlw )
        return;

    Shutdown();

    // Never delete the host from inside the dispatch of its own TLW's
    // wxEVT_DESTROY. Also don't let a nested wxYield() run that deferred
    // delete while RegisterSlot()/SetContent() is still on the stack.
    RequestDeferredDelete();
}

// ============================================================================
// geometry sync
// ============================================================================

// Called from wxTopLevelWindowMSW::DoThaw(): run the flush the freeze held
// back.  The notification must come from the TLW override, not from
// wxWindowMSW::DoThaw(): the freeze is detected at the TLW level, and a TLW
// may thaw without any child running the base DoThaw().
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyThaw(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));
    if ( host )
        host->NotifyThaw();
}

void wxWinUITopLevelHost::NotifyThaw()
{
    if ( !m_frozenDirty )
        return;

    m_frozenDirty = false;
    MarkAllDirty();
}

// Called from wxWindowMSW::MSWWindowProc() on WM_ENABLE: EnableWindow() only
// messages the window it was called on, but the effective enabled state of
// every hosted descendant changed with it.  One coalesced re-sync covers
// them all (the per-slot caches make the no-change case cheap).
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyEnable(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost::MarkAllHostsDirty();
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyNativeLayout(wxWindow *window,
                                 bool zOrderMayHaveChanged)
{
    if ( !window )
        return;

    // Both lookups are read-only. In particular, never call ForWindow():
    // moving an ordinary/native window or changing its scrollbar must not
    // instantiate WinUI infrastructure as a side effect.
    wxWinUITopLevelHost * const slotHost =
        wxWinUITopLevelHost::FindSlotOwner(window);
    wxWinUITopLevelHost * const tlwHost =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));

    if ( slotHost )
        slotHost->NotifyNativeLayoutMutation(zOrderMayHaveChanged, window);
    if ( tlwHost && tlwHost != slotHost )
        tlwHost->NotifyNativeLayoutMutation(zOrderMayHaveChanged, window);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyCaptureMutation(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const slotHost =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( slotHost )
        slotHost->NotifyNativeCaptureMutation();

    wxWinUITopLevelHost * const tlwHost =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));
    if ( tlwHost && tlwHost != slotHost )
        tlwHost->NotifyNativeCaptureMutation();
}

// Called on the slot-state mutations that produce no geometry event
// (DoSetToolTip, WM_SETTEXT feeding the UIA name): one coalesced re-sync of
// the one window.  No-op for windows without a slot.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifySlotState(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( host )
        host->MarkDirty(window);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyAccessibilityAuthority(wxWindow *window,
                                           bool hasWxAccessible)
{
    if ( !window )
        return;

    // FindSlotOwner() is intentionally the only lookup here. In particular,
    // do not call ForWindow(): SetAccessible() on an ordinary/native window
    // must not instantiate an island as a side effect.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( host )
        host->AccessibilityAuthorityChanged(window, hasWxAccessible);
}

WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyReparent(wxWindow *window,
                                                   wxWindow *oldTLW)
{
#if wxUSE_RICHTOOLTIP
    // Close or retarget transient rich tips while their original island root
    // is still alive. The notifier snapshots only weak session state and is
    // safe if closing a tip destroys windows reentrantly.
    wxWinUINotifyTeachingTipReparent(window);
#endif
    wxWinUITopLevelHost::HandleReparentAcrossHosts(window, oldTLW);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostCanReparentSubtreeNow(wxWindow *window)
{
    return wxWinUITopLevelHost::CanReparentSubtreeNow(window);
}

WXDLLIMPEXP_CORE unsigned long long
wxWinUITLWHostPrepareFocusReparent(wxWindow *window,
                                   wxWindow *oldTLW,
                                   wxWindow *newTLW)
{
    return wxWinUITopLevelHost::PrepareFocusReparent(
        window, oldTLW, newTLW);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostSetPreparedFocusReparentNativeBoundary(
    unsigned long long token,
    bool active)
{
    wxWinUITopLevelHost::
        SetPreparedFocusReparentNativeBoundary(token, active);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostCancelPreparedFocusReparent(unsigned long long token)
{
    wxWinUITopLevelHost::CancelPreparedFocusReparent(token);
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
WXDLLIMPEXP_CORE wxWinUIOleDropRegistrationCoverageToken
wxWinUITLWHostAcquireOleDropRegistrationCoverage(
    WXHWND hwnd, unsigned long long generation) noexcept
{
    if ( !hwnd || generation == 0 )
        return 0;

    const auto identity = std::make_pair(hwnd, generation);
#if wxUSE_EXCEPTIONS
    try
    {
#endif
        wxWinUIOleDropRegistrationCoverageToken token =
            ++gs_nextOleDropCoverageToken;
        if ( token == 0 )
            token = ++gs_nextOleDropCoverageToken;

        auto& owners = gs_oleDropRegistrationCoverage[identity];
        while ( !owners.insert(token).second )
        {
            token = ++gs_nextOleDropCoverageToken;
            if ( token == 0 )
                token = ++gs_nextOleDropCoverageToken;
        }
        return token;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        // Acquisition must fail before the broker crosses a native OLE
        // boundary if its exact coverage cannot be made observable.
        return 0;
    }
#endif
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostReleaseOleDropRegistrationCoverage(
    WXHWND hwnd,
    unsigned long long generation,
    wxWinUIOleDropRegistrationCoverageToken token) noexcept
{
    if ( !hwnd || generation == 0 || token == 0 )
        return;

    const auto identity = std::make_pair(hwnd, generation);
    const auto coverage = gs_oleDropRegistrationCoverage.find(identity);
    if ( coverage == gs_oleDropRegistrationCoverage.end() )
        return;

    coverage->second.erase(token);
    if ( coverage->second.empty() )
        gs_oleDropRegistrationCoverage.erase(coverage);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostOwnsOleDropRegistration(wxWindow *window)
{
    if ( !window )
        return false;

    wxWindow* const tlw = wxGetTopLevelParent(window);
    const WXHWND hwnd = tlw ? tlw->GetHandle() : nullptr;
    const unsigned long long generation =
        hwnd ? wxWinUIMSWGetNativeHwndGeneration(hwnd) : 0;
    const auto coverage =
        hwnd && generation != 0
            ? gs_oleDropRegistrationCoverage.find(
                  std::make_pair(hwnd, generation))
            : gs_oleDropRegistrationCoverage.end();
    if ( coverage != gs_oleDropRegistrationCoverage.end() &&
         !coverage->second.empty() )
    {
        return true;
    }

    // This exact-generation registry is authoritative, including while a
    // host is only provisionally published. Falling back to the host's broker
    // bit would let a token for a destroyed/recreated HWND generation suppress
    // a new generation's independent shell registration.
    return false;
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyOleDropTopology(wxWindow *subtreeRoot)
{
    wxWinUIDropBroker::ReconcileShellDropTargets(subtreeRoot);
}
#endif

#if wxUSE_DRAG_AND_DROP
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyDragAcceptFiles(wxWindow *WXUNUSED(window))
{
    wxWinUITopLevelHost::UpdateFileAcceptanceForAll();
}

void wxWinUITopLevelHost::UpdateFileAcceptanceForAll()
{
    // A reparent changes both the old and new bridge's aggregate bit.
    // Updating all live hosts is conservative, synchronous and rare.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());
    for ( const auto& entry : gs_tlwHosts )
    {
        if ( entry.second->IsReadyForLookup() )
            hosts.push_back(entry.second->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        if ( state )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
                host->UpdateBridgeFileAcceptance();
        }
    }
}
#endif

void wxWinUITopLevelHost::HandleReparentAcrossHosts(wxWindow *subtreeRoot,
                                                    wxWindow *oldTLW)
{
    if ( !subtreeRoot )
        return;

    wxWeakRef<wxWindow> subtree(subtreeRoot);

    // A previous failed migration can truthfully leave a descendant slot in a
    // host other than the subtree's immediately previous wx TLW. Snapshot all
    // invalidatable host states, with the common old-host path first, so an
    // immediate second reparent reconciles every partial owner synchronously.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());

    wxWinUITopLevelHost * const oldHost =
        oldTLW ? FindForTLW(oldTLW) : nullptr;
    if ( oldHost )
        hosts.push_back(oldHost->m_hostLifetime);

    for ( const auto& entry : gs_tlwHosts )
    {
        wxWinUITopLevelHost * const host = entry.second;
        if ( host != oldHost && host->IsReadyForLookup() )
            hosts.push_back(host->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        wxWindow * const liveSubtree = subtree.get();
        if ( !liveSubtree || liveSubtree->IsBeingDeleted() )
            return;

        if ( state )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
                host->HandleReparent(liveSubtree);
        }
    }
}

bool wxWinUITopLevelHost::CanReparentSubtreeNow(wxWindow *subtreeRoot)
{
    if ( !subtreeRoot )
        return false;

    // This is deliberately allocation-free and callback-free: the MSW shim
    // repeats it at each mutation boundary. A slot can remain in an older
    // host after a previously aborted transfer, so inspect every live host
    // rather than only the subtree root's nominal TLW.
    try
    {
        for ( const auto& hostEntry : gs_tlwHosts )
        {
            wxWinUITopLevelHost * const host = hostEntry.second;
            if ( !host || host->m_shuttingDown )
                continue;

            for ( const auto& slotEntry : host->m_slots )
            {
                wxWindow * const window = slotEntry.first;
                wxWinUISlot * const slot = slotEntry.second;
                if ( !window || !slot ||
                     (window != subtreeRoot &&
                      !subtreeRoot->IsDescendant(window)) )
                {
                    continue;
                }

                const auto& gate = slot->m_disconnectGate;
                if ( gate &&
                     (!gate->IsSatisfied() || gate->IsDegraded()) )
                {
                    return false;
                }
            }
        }
    }
    catch ( ... )
    {
        return false;
    }
    return true;
}

void wxWinUITopLevelHost::MarkDirty(wxWindow *window)
{
    if ( m_shuttingDown )
        return;

    ++gs_profileMarkDirty;
    m_dirty.insert(window);
    ScheduleFlush();
}

void wxWinUITopLevelHost::InvalidateStructure()
{
    // Zero is reserved for the never-applied initial state. More
    // importantly, this value is never cleared: a nested flush can advance
    // it but cannot hide that fact from an outer z-order snapshot.
    if ( ++m_structureGeneration == 0 )
        ++m_structureGeneration;
}

void wxWinUITopLevelHost::MarkAllDirty()
{
    if ( m_shuttingDown )
        return;

    ++gs_profileMarkAllDirty;

    m_allDirty = true;
    ScheduleFlush();
}

void wxWinUITopLevelHost::NotifyNativeLayoutMutation(
    bool zOrderMayHaveChanged,
    wxWindow *mutated)
{
    if ( m_shuttingDown )
        return;

    ++gs_profileLayoutNotify;

    // The same pass also diagnoses overlaps that the one composition band
    // cannot represent. Geometry/visibility changes can create or remove
    // such an overlap even when USER32's sibling order itself is unchanged.
    InvalidateStructure();
    wxUnusedVar(zOrderMayHaveChanged);
    wxUnusedVar(mutated);

    // Native scrollbars alter the bridge region and a native ancestor move
    // changes every descendant slot. The common coalesced pass is both
    // cheaper and safer than trying to infer the affected descendants here:
    // narrowing it to the slots under the window which moved left the others
    // holding stale geometry, i.e. tool bars which stopped drawing and
    // stopped taking clicks until the next full pass.
    MarkAllDirty();
}

void wxWinUITopLevelHost::MarkAllHostsDirty()
{
    // Snapshot the invalidatable host lifetimes: MarkAllDirty() only posts a
    // callback, but wx application hooks reached while doing so are allowed
    // to synchronously destroy a TLW and mutate gs_tlwHosts.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());
    for ( const auto& entry : gs_tlwHosts )
    {
        if ( entry.second->IsReadyForLookup() )
            hosts.push_back(entry.second->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        if ( state )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
            {
                // Effective enabled state is part of native hit testing. A
                // parent WM_ENABLE changes every descendant even though they
                // receive no native layout notification of their own.
                host->InvalidateStructure();
                host->MarkAllDirty();
            }
        }
    }
}

void wxWinUITopLevelHost::SynchronizeAllHosts()
{
    // A tooltip policy setter is synchronous, but every dependency-property
    // write below may destroy or reparent a TLW. Snapshot invalidatable
    // lifetimes and resolve the host afresh before each operation.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());
    for ( const auto& entry : gs_tlwHosts )
    {
        if ( entry.second->IsReadyForLookup() )
            hosts.push_back(entry.second->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        if ( !state )
            continue;

        wxWinUITopLevelHost *host = state->GetHost();
        if ( !host )
            continue;

        // Preserve the complete geometry catch-up independently of the
        // synchronous tooltip policy transaction.
        host->m_allDirty = true;
        if ( host->m_tlw && host->m_tlw->IsFrozen() )
        {
            host->m_frozenDirty = true;

            std::vector<std::shared_ptr<wxWinUISlotLifetime>> slots;
            slots.reserve(host->m_slots.size());
            for ( const auto& entry : host->m_slots )
                slots.push_back(entry.second->m_lifetime);

            for ( const auto& slotState : slots )
            {
                wxWinUITopLevelHost * const currentHost =
                    state->GetHost();
                if ( !currentHost || currentHost != host )
                    break;
                wxWindow * const window =
                    slotState ? slotState->GetWindow() : nullptr;
                wxWinUISlot * const slot =
                    slotState && slotState->GetHost() == host && window
                        ? host->FindSlot(window)
                        : nullptr;
                if ( slot && slot->m_lifetime == slotState )
                    host->SynchronizeSlotToolTipPolicy(*slot);
            }
        }
        else
        {
            // Do not post a redundant CallAfter only to consume it
            // immediately on a non-frozen host.
            host->FlushSync();
        }
    }
}

#if wxUSE_TOOLTIPS

namespace
{

enum class wxWinUIToolTipSuppressionResult
{
    Suppressed,
    Superseded,
    Failed,
    BudgetExhausted
};

// ToolTip::IsOpen(false) and ToolTipService::SetToolTip() are both
// synchronous dependency-property boundaries. An authored IsOpen/property
// callback is allowed to install another object at either boundary. Keep
// consuming those replacements in the same policy call so Enable(false)
// remains synchronous, but put a strict ceiling on hostile replacement
// chains. The real ToolTip::Closed event is delivered asynchronously by the
// Popup; the ToolTipProperty observer starts a fresh guarded transaction for
// replacements published from it.
template <typename IsCurrent>
wxWinUIToolTipSuppressionResult wxWinUISuppressSlotToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const IsCurrent& isCurrent,
    bool& owned,
    winrt::Windows::Foundation::IInspectable& applied,
    winrt::Windows::Foundation::IInspectable& original,
    winrt::Windows::Foundation::IInspectable& originalLocal)
{
    using winrt::Microsoft::UI::Xaml::Controls::ToolTip;
    using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

    constexpr unsigned MaxReplacementPasses = 8;
    for ( unsigned pass = 0; pass < MaxReplacementPasses; ++pass )
    {
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        const auto visible = ToolTipService::GetToolTip(content);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        if ( !visible )
            return wxWinUIToolTipSuppressionResult::Suppressed;

        const auto visibleLocal =
            wxWinUIReadToolTipLocalValue(content);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        const bool exactManagedValue =
            owned && applied && visible == applied &&
            visibleLocal == applied &&
            wxWinUIIsManagedToolTipCurrent(content, applied);
        if ( !exactManagedValue )
        {
            // This is a direct application replacement, including one
            // published by Closed or by the preceding detach setter. It is
            // the baseline that Enable(true) must restore by exact identity.
            const auto previouslyApplied = applied;
            original = visible;
            originalLocal = wxWinUIReadToolTipLocalValue(content);
            owned = false;
            applied = nullptr;

            if ( previouslyApplied )
            {
                // Retire only the registry entry carrying the identity we
                // used to own. A nested wxWinUISetToolTip() may already have
                // installed a newer entry for this element, and the
                // element-only Forget helper would incorrectly erase it.
                if ( !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         previouslyApplied) )
                {
                    return wxWinUIToolTipSuppressionResult::Failed;
                }
                if ( !isCurrent() )
                    return wxWinUIToolTipSuppressionResult::Superseded;

                const auto afterForget =
                    ToolTipService::GetToolTip(content);
                if ( !isCurrent() )
                    return wxWinUIToolTipSuppressionResult::Superseded;
                if ( afterForget != visible )
                {
                    original = afterForget;
                    if ( !afterForget )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Suppressed;
                    }
                    continue;
                }
            }
        }

        if ( const auto object = visible.try_as<ToolTip>() )
        {
            object.IsOpen(false);
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;

            // A synchronous IsOpen dependency-property callback may have
            // installed a replacement. Do not detach it using the stale
            // identity: the next pass adopts and closes it.
            const auto afterClose =
                ToolTipService::GetToolTip(content);
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;
            if ( afterClose != visible )
            {
                const auto previouslyApplied = applied;
                original = afterClose;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                owned = false;
                applied = nullptr;
                if ( previouslyApplied )
                {
                    if ( !wxWinUIRestoreToolTip(
                             content,
                             winrt::Microsoft::UI::Xaml::
                                 DependencyProperty::UnsetValue(),
                             previouslyApplied) )
                    {
                        return wxWinUIToolTipSuppressionResult::Failed;
                    }
                    if ( !isCurrent() )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Superseded;
                    }

                    const auto afterForget =
                        ToolTipService::GetToolTip(content);
                    if ( !isCurrent() )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Superseded;
                    }
                    if ( afterForget != afterClose )
                        original = afterForget;
                    if ( !afterForget )
                    {
                        return
                            wxWinUIToolTipSuppressionResult::Suppressed;
                    }
                }
                else if ( !afterClose )
                {
                    return wxWinUIToolTipSuppressionResult::Suppressed;
                }
                continue;
            }
        }

        ToolTipService::SetToolTip(content, nullptr);
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;

        // Null is authoritative only after re-reading the property. A
        // property callback can publish yet another authored value from the
        // setter above; that object becomes the next pass's baseline.
        if ( !ToolTipService::GetToolTip(content) )
        {
            if ( !isCurrent() )
                return wxWinUIToolTipSuppressionResult::Superseded;
            return wxWinUIToolTipSuppressionResult::Suppressed;
        }
        if ( !isCurrent() )
            return wxWinUIToolTipSuppressionResult::Superseded;
    }

    return wxWinUIToolTipSuppressionResult::BudgetExhausted;
}

} // anonymous namespace

#endif // wxUSE_TOOLTIPS

void wxWinUITopLevelHost::SynchronizeSlotToolTipPolicy(wxWinUISlot& slot)
{
#if wxUSE_TOOLTIPS
    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(&slot);
    ++slot.m_toolTipPolicySyncDepth;
    const auto toolTipPolicySyncGuard = wxMakeGuard([&slot]()
    {
        wxASSERT_MSG(slot.m_toolTipPolicySyncDepth != 0,
                     "unbalanced WinUI tooltip policy transaction");
        --slot.m_toolTipPolicySyncDepth;
    });
    wxUnusedVar(toolTipPolicySyncGuard);

    if ( ++slot.m_syncEpoch == 0 )
        ++slot.m_syncEpoch;
    const unsigned long long syncEpoch = slot.m_syncEpoch;

    if ( slot.m_contentTransactionInProgress )
    {
        slot.NudgeDirty();
        return;
    }

    const auto state = slot.m_lifetime;
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    const auto content = slot.m_content;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    const bool enabled = wxWinUIAreToolTipsEnabled();
    if ( !window || !content || !state || state->GetHost() != this )
        return;

    wxToolTip * const tip = window->GetToolTip();
    const bool tipPresent = tip != nullptr;
    const wxString tipText = tip ? tip->GetTip() : wxString();
    const int maximumWidth =
        tip ? tip->GetWinUIMaxWidthAtCreation() : 0;
    const int toolTipDPI =
        maximumWidth > 0 ? window->GetDPI().GetWidth() : 0;

    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        if ( !state || state->GetHost() != this ||
             state->GetWindow() != window ||
             wxWinUIAreToolTipsEnabled() != enabled )
        {
            return nullptr;
        }
        wxWinUISlot * const current = FindSlot(window);
        if ( !current ||
             current->m_lifetime != state ||
             current->m_content != content ||
             current->m_contentGeneration != contentGeneration ||
             current->m_syncEpoch != syncEpoch ||
             current->m_contentTransactionInProgress )
        {
            return nullptr;
        }

        wxToolTip * const currentTip = window->GetToolTip();
        if ( currentTip != tip )
            return nullptr;
        if ( currentTip )
        {
            const int currentMaximumWidth =
                currentTip->GetWinUIMaxWidthAtCreation();
            const int currentDPI =
                currentMaximumWidth > 0
                    ? window->GetDPI().GetWidth()
                    : 0;
            if ( currentTip->GetTip() != tipText ||
                 currentMaximumWidth != maximumWidth ||
                 currentDPI != toolTipDPI )
            {
                return nullptr;
            }
        }
        return current;
    };

    bool owned = slot.m_toolTipOwned;
    bool suppressed = slot.m_toolTipSuppressed;
    auto applied = slot.m_lastAppliedToolTip;
    auto original = slot.m_originalToolTip;
    auto originalLocal = slot.m_originalToolTipLocalValue;
    const bool wxChanged =
        slot.m_lastToolTipPresent != tipPresent ||
        slot.m_lastToolTip != tipText ||
        slot.m_lastToolTipMaximumWidth != maximumWidth ||
        slot.m_lastToolTipDPI != toolTipDPI;

    using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

    try
    {
        auto actual = ToolTipService::GetToolTip(content);
        auto actualLocal =
            wxWinUIReadToolTipLocalValue(content);
        if ( !getCurrentSlot() )
            return;

        const auto applyWxOverride = [&]()
        {
            winrt::Windows::Foundation::IInspectable candidate{ nullptr };
            if ( !wxWinUISetToolTip(
                     content, tipText, window, &candidate, maximumWidth) )
            {
                return false;
            }
            if ( !getCurrentSlot() )
                return false;

            const auto post = ToolTipService::GetToolTip(content);
            if ( !getCurrentSlot() )
                return false;

            const bool stillManaged =
                candidate &&
                (post == candidate ||
                 wxWinUIIsManagedToolTipSuppressed(content, candidate) ||
                 wxWinUIIsManagedToolTipCurrent(content, candidate));
            if ( tipText.empty() ? !post : stillManaged )
            {
                owned = true;
                applied = candidate;
            }
            else
            {
                owned = false;
                applied = nullptr;
                if ( candidate &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         candidate) )
                {
                    return false;
                }
                if ( !getCurrentSlot() )
                    return false;
                original = ToolTipService::GetToolTip(content);
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }
            actual = ToolTipService::GetToolTip(content);
            return getCurrentSlot() != nullptr;
        };

        const auto relinquishWxOverride = [&]()
        {
            if ( applied )
            {
                if ( !wxWinUIRestoreToolTip(
                         content, originalLocal, applied) )
                {
                    return false;
                }
            }
            else
            {
                const auto current = ToolTipService::GetToolTip(content);
                if ( !getCurrentSlot() )
                    return false;
                if ( !current &&
                     !wxWinUIReadToolTipLocalValue(content) )
                {
                    wxWinUIRestoreToolTipLocalValue(
                        content, originalLocal);
                }
            }
            if ( !getCurrentSlot() )
                return false;
            actual = ToolTipService::GetToolTip(content);
            actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return false;
            owned = false;
            applied = nullptr;
            original = actual;
            originalLocal =
                wxWinUIReadToolTipLocalValue(content);
            return true;
        };

        if ( !slot.m_toolTipSynced )
        {
            original = actual;
            originalLocal =
                wxWinUIReadToolTipLocalValue(content);
            owned = false;
            applied = nullptr;
            if ( tipPresent && !applyWxOverride() )
                return;
        }
        else
        {
            const bool applicationClearedSuppressedNull =
                suppressed &&
                actualLocal ==
                    winrt::Microsoft::UI::Xaml::
                        DependencyProperty::UnsetValue();
            if ( applicationClearedSuppressedNull )
            {
                // Effective null alone cannot prove that wx still owns the
                // suppression mask. ClearValue() is an application mutation
                // which deliberately removes our local null, even if no
                // effective-value callback fires.
                if ( applied &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         applied) )
                {
                    return;
                }
                if ( !getCurrentSlot() )
                    return;
                owned = false;
                applied = nullptr;
                original = actual;
                originalLocal = actualLocal;
            }

            const bool managedSuppressed =
                owned && applied &&
                wxWinUIIsManagedToolTipSuppressed(content, applied);
            if ( !applicationClearedSuppressedNull &&
                 owned &&
                 actual != applied &&
                 !managedSuppressed )
            {
                // The exact wx object no longer owns the property.
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                if ( applied &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         applied) )
                {
                    return;
                }
                if ( !getCurrentSlot() )
                    return;
                owned = false;
                applied = nullptr;
            }
            else if ( !owned && suppressed && actual )
            {
                // A direct write while disabled becomes the authored
                // baseline. It is suppressed below if the policy remains off.
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }
            else if ( !owned && !suppressed && !wxChanged )
            {
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }

            if ( owned )
            {
                if ( !tipPresent )
                {
                    if ( !relinquishWxOverride() )
                        return;
                }
                else if ( wxChanged && !applyWxOverride() )
                    return;
            }
            else if ( tipPresent && wxChanged )
            {
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                if ( !applyWxOverride() )
                    return;
            }
        }

        if ( !enabled )
        {
            const wxWinUIToolTipSuppressionResult result =
                wxWinUISuppressSlotToolTip(
                    content,
                    getCurrentSlot,
                    owned,
                    applied,
                    original,
                    originalLocal);
            if ( result != wxWinUIToolTipSuppressionResult::Suppressed )
            {
                if ( result ==
                        wxWinUIToolTipSuppressionResult::BudgetExhausted )
                {
                    wxLogWarning(
                        "wxWinUI: synchronous slot tooltip suppression "
                        "replacement budget exhausted");
                    if ( wxWinUISlot * const current = getCurrentSlot() )
                        current->NudgeDirty();
                }
                else if ( result ==
                          wxWinUIToolTipSuppressionResult::Failed )
                {
                    if ( wxWinUISlot * const current = getCurrentSlot() )
                        current->NudgeDirty();
                }
                return;
            }
            suppressed = true;
        }
        else
        {
            actual = ToolTipService::GetToolTip(content);
            actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return;
            if ( suppressed )
            {
                if ( owned && tipPresent )
                {
                    if ( actual != applied && !applyWxOverride() )
                        return;
                }
                else if ( !actual && !actualLocal )
                {
                    wxWinUIRestoreToolTipLocalValue(
                        content, originalLocal);
                    if ( !getCurrentSlot() )
                        return;
                }
                else
                {
                    // A non-null effective value or an application-cleared
                    // local mask during suppression is the newer baseline.
                    original = actual;
                    originalLocal = actualLocal;
                    owned = false;
                    applied = nullptr;
                }
            }
            suppressed = false;
        }

        wxWinUISlot * const current = getCurrentSlot();
        if ( !current )
            return;
        current->m_toolTipSynced = true;
        current->m_toolTipOwned = owned;
        current->m_toolTipSuppressed = suppressed;
        current->m_lastToolTipPresent = tipPresent;
        current->m_lastToolTip = tipText;
        current->m_lastToolTipMaximumWidth = maximumWidth;
        current->m_lastToolTipDPI = toolTipDPI;
        current->m_lastAppliedToolTip = applied;
        current->m_originalToolTip = original;
        current->m_originalToolTipLocalValue = originalLocal;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("synchronize slot tooltip policy", e);
        slot.NudgeDirty();
    }
#else
    wxUnusedVar(slot);
#endif
}

void wxWinUITopLevelHost::NotifyCursorPolicyChanged(
    bool emitSetCursorEvents)
{
    ++gs_profileCursorPolicy;
    if ( ++gs_cursorPolicyGeneration == 0 )
        ++gs_cursorPolicyGeneration;

    // Snapshot lifetimes before crossing any WM_SETCURSOR/application
    // callback. A cursor handler is allowed to destroy a top-level window.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());
    for ( const auto& entry : gs_tlwHosts )
    {
        if ( entry.second->IsReadyForLookup() )
            hosts.push_back(entry.second->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        wxWinUITopLevelHost * const host =
            state ? state->GetHost() : nullptr;
        if ( !host )
            continue;

        for ( const auto& slotEntry : host->m_slots )
        {
            if ( slotEntry.second )
                slotEntry.second->m_cursorSynced = false;
        }
        host->MarkAllDirty();
        if ( host->m_cursorResolutionDepth )
        {
            // RefreshActivePointerCursor() would still target the previous
            // surface while a new slot's wxEVT_SET_CURSOR is in flight.
            // Its outer operation guard performs an exact, callback-free
            // replay once that resolution has unwound.
            host->m_cursorPolicyReplayPending = true;
            continue;
        }
        host->RefreshActivePointerCursor(emitSetCursorEvents);
    }
}

WXDLLIMPEXP_CORE void wxWinUINotifyGlobalCursorChanged()
{
    wxWinUITopLevelHost::NotifyCursorPolicyChanged();
}

WXDLLIMPEXP_CORE void wxWinUINotifyModalCursorChanged()
{
    // Modality only changes effective cursor policy by suppressing the
    // process-wide busy cursor inside the active dialog. Without a busy
    // scope, invalidating the position-sensitive slot verdict would replace
    // it with static policy until the next physical Move.
    if ( wxIsBusy() )
        wxWinUITopLevelHost::NotifyCursorPolicyChanged(false);
}

void wxWinUITopLevelHost::ScheduleFlush()
{
    // While the TLW is frozen, schedule nothing at all: each queued flush
    // would just see the freeze and bail out, so a Move/Size storm under
    // Freeze() would keep posting one CallAfter per event.  Note that a
    // catch-up flush is owed instead; wxWinUITLWHostNotifyThaw() -- called
    // from wxTopLevelWindowMSW::DoThaw() -- schedules that single flush at
    // thaw time.
    if ( m_tlw && m_tlw->IsFrozen() )
    {
        m_frozenDirty = true;
        return;
    }

    if ( m_flushScheduled )
        return;

    m_flushScheduled = true;
    ++gs_flushSchedules;
    CallAfter(&wxWinUITopLevelHost::FlushSync);
}

void wxWinUITopLevelHost::FlushSync()
{
    // Every XAML setter and every test/application hook below may destroy the
    // TLW and pump the deferred-delete callback. Keep this allocation alive
    // until the complete outermost flush stack has unwound.
    OperationGuard hostOperation(this);

    ++gs_flushCallbackAttempts;
    m_flushScheduled = false;

    if ( m_shuttingDown || !m_source )
        return;

    if ( wxWinUIInputLogEnabled() )
    {
        // Sampled: a flush STORM (self-rescheduling loop) must be visible in
        // the black box without drowning it.
        static unsigned s_flushCount = 0;
        if ( (++s_flushCount % 32) == 1 )
            wxWinUIInputLog("flush #%u", s_flushCount);
    }

    if ( m_tlw->IsFrozen() )
    {
        // This flush was scheduled before the freeze started (new ones are
        // not posted while frozen, see ScheduleFlush): remember that a
        // catch-up is owed and let wxWinUITLWHostNotifyThaw() schedule it.
        m_frozenDirty = true;
        return;
    }

    ++gs_flushRuns;
    ++gs_profileFlushRuns;

    // A flush re-synchronises every slot of this window. One per input
    // event is a storm, and the profile has to show it: the pointer
    // samples it happens between are far rarer than the flushes.
    if ( wxWinUIInputProfileEnabled() )
    {
        static unsigned s_runs = 0;
        static long s_since = 0;
        const long now = static_cast<long>(::GetTickCount());
        if ( !s_since )
            s_since = now;
        if ( ++s_runs == 50 )
        {
            if ( FILE * const f = wxWinUIInputProfileFile() )
            {
                fprintf(f, "flush: 50 full slot syncs in %ld ms, "
                           "%u slots, layoutnotify=%u (suppressed %u) "
                           "markall=%u markone=%u policy=%u relayout=%u\n",
                        now - s_since,
                        static_cast<unsigned>(m_slots.size()),
                        gs_profileLayoutNotify,
                        gs_profileLayoutSuppressed,
                        gs_profileMarkAllDirty,
                        gs_profileMarkDirty,
                        gs_profileCursorPolicy,
                        wxWinUIGetContentRelayoutCount());
                gs_profileLayoutNotify = 0;
                gs_profileLayoutSuppressed = 0;
                gs_profileMarkAllDirty = 0;
                gs_profileMarkDirty = 0;
                gs_profileCursorPolicy = 0;
                fflush(f);
            }
            s_runs = 0;
            s_since = now;
        }
    }
    ++m_flushRunsForTest;

    EnsureInnerSubclass();
    EnsureBridgeOnTop();
    RebuildBridgeRegion();
    if ( m_shuttingDown )
        return;

    // Snapshot invalidatable callback states, not map iterators or raw window
    // pointers.  XAML property setters below may synchronously emit LostFocus;
    // application code handling the resulting wx event is allowed to destroy
    // any slot and mutate m_slots/m_dirty.
    std::vector<std::shared_ptr<wxWinUISlotLifetime>> pending;
    if ( m_allDirty )
    {
        pending.reserve(m_slots.size());
        for ( const auto& kv : m_slots )
            pending.push_back(kv.second->m_lifetime);
    }
    else
    {
        pending.reserve(m_dirty.size());
        for ( wxWindow *window : m_dirty )
        {
            if ( wxWinUISlot * const slot = FindSlot(window) )
                pending.push_back(slot->m_lifetime);
        }
    }

    // Clear the generation before applying XAML state.  A reentrant dirty
    // notification then belongs to the next generation and must not be
    // erased when this one finishes.
    m_allDirty = false;
    m_dirty.clear();

    // Snapshot the structural epoch before crossing any slot setter. A
    // setter can synchronously mutate a slot already processed (or outside
    // this pending set), advancing the epoch while that slot's cached
    // visible region is still stale. Such a frame must never publish the
    // newer epoch; the dirty notification it raised owns the next pass.
    const unsigned long long syncStructureGeneration =
        m_structureGeneration;

    // Windows reparented into another TLW migrate after the sweep.
    std::vector<std::shared_ptr<wxWinUISlotLifetime>> migrating;
    for ( const auto& state : pending )
    {
        if ( !state || state->GetHost() != this )
            continue;

        wxWindow * const window = state->GetWindow();
        if ( !window || !window->GetHandle() )
            continue;

        wxWinUISlot * const slot = FindSlot(window);
        if ( !slot || slot->m_lifetime != state )
            continue;

        if ( wxGetTopLevelParent(window) != m_tlw )
        {
            migrating.push_back(state);
            continue;
        }

        SyncSlot(*slot);
        if ( m_shuttingDown )
            return;
    }

    if ( m_structureGeneration == syncStructureGeneration &&
         m_structureAppliedGeneration != syncStructureGeneration )
    {
        // Publish only a pass which completed against this exact epoch. A
        // nested FlushSync() is free to publish a NEWER generation; the outer
        // pass must then fail without overwriting it.
        if ( RecomputeZOrder(syncStructureGeneration) )
            m_structureAppliedGeneration = syncStructureGeneration;
        if ( m_shuttingDown )
            return;
    }

    for ( const auto& state : migrating )
    {
        if ( !state || state->GetHost() != this )
            continue;

        wxWindow * const window = state->GetWindow();
        if ( window )
            MigrateSlotToCurrentTLW(window);
        if ( m_shuttingDown )
            return;
    }

    RetryDeferredFocusAfterFlush();
    if ( m_shuttingDown )
        return;

    // Re-hit-test the last real point after layout/re-template/animation:
    // the former slot may now expose another slot or a native child. This is
    // skipped until the root has observed an actual point.
    if ( m_lastPointerScreen.x > -50000 &&
         m_lastPointerScreen.y > -50000 )
    {
        RefreshPointerCursorAtLastPoint();
    }
    else if ( m_islandPointerCursorApplied ||
              m_activeCursorSurface != ActiveCursorSurface::GenericXaml )
    {
        // A test seam can establish the active surface before the first real
        // routed point, and an InputPointerSource override may be reset by a
        // XAML re-template even while the pointer is stationary. Reassert the
        // already-selected surface after the sync instead of silently leaving
        // the runtime's default Arrow in force.
        RefreshActivePointerCursor();
    }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    // Native OLE ownership is acquired only after the first visible XAML
    // presentation and this pass's bridge geometry/z-order publication.
    if ( !m_shuttingDown )
        TryInitializeDropBrokerAfterPresentation();
#endif
}

void wxWinUITopLevelHost::AccessibilityAuthorityChanged(
    wxWindow *window,
    bool hasWxAccessible)
{
    OperationGuard hostOperation(this);

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_deletePending || slot->m_poisoned ||
            !slot->m_lifetime ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return;
    }

    SyncSlotAccessibilityAuthority(*slot, hasWxAccessible);
}

void wxWinUITopLevelHost::SyncSlotAccessibilityAuthority(
    wxWinUISlot& slot,
    bool hasWxAccessible)
{
    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace Peers =
        winrt::Microsoft::UI::Xaml::Automation::Peers;

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(&slot);

    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    const auto content = slot.m_semanticTarget;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    if ( !window || slot.m_deletePending || slot.m_poisoned ||
            !state || state->GetHost() != this )
    {
        return;
    }

    if ( slot.m_hasWxAccessible != hasWxAccessible )
    {
        slot.m_hasWxAccessible = hasWxAccessible;
        if ( ++slot.m_accessibilityAuthorityGeneration == 0 )
            ++slot.m_accessibilityAuthorityGeneration;

        // UIA caches the provider by HWND, independently of our provider
        // object's generation check. Make the newly-selected native/XAML
        // authority observable on the next WM_GETOBJECT for this same shell.
        slot.RetireExposedAccessibilityShellProvider();
    }
    const unsigned long long authorityGeneration =
        slot.m_accessibilityAuthorityGeneration;

    if ( !content )
    {
        slot.m_accessibilityViewSynced = false;
        slot.m_accessibilityViewOwned = false;
        slot.m_originalAccessibilityViewLocalValue = nullptr;
        return;
    }

    const bool previousSynced = slot.m_accessibilityViewSynced;
    const bool previousOwned = slot.m_accessibilityViewOwned;
    auto nextOriginal = slot.m_originalAccessibilityViewLocalValue;
    bool nextOwned = previousOwned;

    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        if ( !state || state->GetHost() != this ||
                state->GetWindow() != window )
        {
            return nullptr;
        }

        wxWinUISlot * const current = FindSlot(window);
        return current &&
                   current == &slot &&
                   current->m_lifetime == state &&
                   current->m_semanticTarget == content &&
                   current->m_contentGeneration == contentGeneration &&
                   current->m_hasWxAccessible == hasWxAccessible &&
                   current->m_accessibilityAuthorityGeneration ==
                       authorityGeneration
             ? current
             : nullptr;
    };

    try
    {
        const auto actual =
            Automation::AutomationProperties::GetAccessibilityView(content);
        if ( !getCurrentSlot() )
            return;

        if ( hasWxAccessible )
        {
            if ( actual != Peers::AccessibilityView::Raw )
            {
                // This is either the initial authored value or a newer direct
                // application mutation. Remember it, then keep the native
                // wxAccessible tree singular by hiding the XAML peer.
                nextOriginal = content.ReadLocalValue(
                    Automation::AutomationProperties::
                        AccessibilityViewProperty());

                // Publish the restoration snapshot before the setter. Its
                // property-changed callback may synchronously remove
                // wxAccessible; that nested authority transition must see
                // enough state to restore this exact authored value.
                wxWinUISlot * const current = getCurrentSlot();
                if ( !current )
                    return;
                current->m_accessibilityViewSynced = true;
                current->m_accessibilityViewOwned = true;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;

                Automation::AutomationProperties::SetAccessibilityView(
                    content, Peers::AccessibilityView::Raw);
                nextOwned = true;
            }
            else if ( !previousSynced )
            {
                // Raw was already authored by the application; there is
                // nothing for the host to restore later.
                nextOriginal = content.ReadLocalValue(
                    Automation::AutomationProperties::
                        AccessibilityViewProperty());
                nextOwned = false;
            }
        }
        else
        {
            if ( previousOwned &&
                 actual == Peers::AccessibilityView::Raw )
            {
                const auto property =
                    Automation::AutomationProperties::
                        AccessibilityViewProperty();

                // Conversely, retire ownership before restoring. A callback
                // which re-adds wxAccessible then treats the just-restored
                // application value as the new baseline instead of trying to
                // restore it a second time.
                wxWinUISlot * const current = getCurrentSlot();
                if ( !current )
                    return;
                current->m_accessibilityViewSynced = true;
                current->m_accessibilityViewOwned = false;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;

                if ( nextOriginal ==
                        winrt::Microsoft::UI::Xaml::DependencyProperty::
                            UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(property, nextOriginal);
                }
            }

            // If `actual` was already non-Raw, the application changed it
            // while native authority was active. Leave that newer authored
            // value untouched rather than restoring the older snapshot.
            nextOwned = false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        // If restoration itself failed, retain ownership so a later sync can
        // retry. This is safe even for a setter that mutated before throwing:
        // the no-authority path observes a non-Raw value and simply retires
        // ownership without overwriting it.
        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            if ( !hasWxAccessible && previousOwned )
            {
                current->m_accessibilityViewSynced = previousSynced;
                current->m_accessibilityViewOwned = true;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;
            }
        }
        wxWinUILogException("sync slot accessibility authority", e);
        return;
    }

    // Every attached-property setter can synchronously run application code.
    // Commit only onto the same live slot/content/authority generation.
    if ( wxWinUISlot * const current = getCurrentSlot() )
    {
        current->m_accessibilityViewSynced = true;
        current->m_accessibilityViewOwned = nextOwned;
        current->m_originalAccessibilityViewLocalValue = nextOriginal;
    }
}

void wxWinUITopLevelHost::SyncSlot(wxWinUISlot& slot)
{
    wxWinUISlot::OperationGuard slotOperation(&slot);

    // This epoch is per-entry, not per-content. A nested FlushSync() over the
    // same slot always supersedes its outer caller, even when both observe the
    // same element generation.
    if ( ++slot.m_syncEpoch == 0 )
        ++slot.m_syncEpoch;
    const unsigned long long syncEpoch = slot.m_syncEpoch;

    // SetContent() has already crossed (or is about to cross) a carrier
    // setter, but has not yet published the matching model. Never sync that
    // deliberately unobservable state. Requeue one coalesced pass for after
    // the transaction publishes commit or rollback.
    if ( slot.m_contentTransactionInProgress )
    {
        slot.NudgeDirty();
        return;
    }

    if ( slot.m_contentLoadedObserverPending )
    {
        const bool observerInstalled =
            slot.InstallContentLoadedObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_contentLoadedObserverPending )
        {
            slot.NudgeDirty();
            return;
        }
    }

    if ( slot.m_toolTipPropertyObserverPending )
    {
        const bool observerInstalled =
            slot.InstallToolTipPropertyObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_toolTipPropertyObserverPending )
        {
            // A nested replacement made this registration attempt obsolete.
            // Let the new generation's own observer and dirty pass win.
            slot.NudgeDirty();
            return;
        }
    }

    if ( slot.m_automationNameStylePropertyObserverPending )
    {
        const bool observerInstalled =
            slot.InstallAutomationNameStylePropertyObserver();
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_contentTransactionInProgress ||
             slot.m_syncEpoch != syncEpoch )
        {
            return;
        }
        if ( !observerInstalled &&
             !slot.m_automationNameStylePropertyObserverPending )
        {
            // A nested replacement made this registration attempt obsolete.
            slot.NudgeDirty();
            return;
        }
    }

    // RegisterPropertyChangedCallback observes effective values, not their
    // source. In particular, local x:Null -> UnsetValue can be silent when
    // FrameworkElement::Style() remains null while an implicit Style starts
    // contributing setters. Re-snapshot the source whenever the host already
    // has another reason to synchronize this slot. This is bounded by normal
    // wx dirty work and avoids a permanent Rendering/timer subscription.
    if ( const auto observerState =
             slot.m_automationNameStylePropertyState )
    {
        const auto framework =
            slot.m_automationNameStyleObservedContent;
        if ( observerState->active && framework )
        {
            try
            {
                if ( wxWinUIUpdateLocalStyleSourceSnapshot(
                         observerState, framework) )
                {
                    if ( ++slot.m_automationNameStyleRevision == 0 )
                        ++slot.m_automationNameStyleRevision;
                    if ( ++slot.m_automationNameImplicitStyleRevision == 0 )
                        ++slot.m_automationNameImplicitStyleRevision;
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                // Leave the old snapshot intact so a later ordinary sync can
                // retry instead of falsely declaring the source current.
                wxWinUILogException(
                    "snapshot automation Style source during sync", e);
            }
        }
    }

    // Snapshot every wx/slot value before crossing into a XAML setter.  Any
    // setter may synchronously raise LostFocus; application code handling the
    // synthesized wx event may unregister this slot.  From the first setter
    // until final revalidation, use only strong local XAML references and the
    // invalidatable lifetime state -- never the slot reference again.
    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;
    wxWindow * const w = state ? state->GetWindow() : nullptr;
    const HWND hwnd = GetHwndOf(w);
    if ( !hwnd || !slot.m_container )
        return;

    const WXHWND wxHwnd = w->GetHWND();
    const auto container = slot.m_container;
    const auto carrier = slot.m_carrier;
    const auto content = slot.m_content;
    const auto semanticTarget = slot.m_semanticTarget;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    const wxWinUIClipRectDIP clipRectDIP = slot.m_clipRectDIP;
    const bool hasClipRect = slot.m_hasClipRect;
    const unsigned long long clipRevision = slot.m_clipRevision;
    const int previousEnabled = slot.m_lastEnabled;
#if wxUSE_TOOLTIPS
    const bool previousToolTipSynced = slot.m_toolTipSynced;
    const bool previousToolTipOwned = slot.m_toolTipOwned;
    const bool previousToolTipSuppressed = slot.m_toolTipSuppressed;
    const bool previousToolTipPresent = slot.m_lastToolTipPresent;
    const wxString previousToolTip = slot.m_lastToolTip;
    const int previousToolTipMaximumWidth =
        slot.m_lastToolTipMaximumWidth;
    const int previousToolTipDPI = slot.m_lastToolTipDPI;
    const auto previousAppliedToolTip = slot.m_lastAppliedToolTip;
    const auto previousOriginalToolTip = slot.m_originalToolTip;
    const auto previousOriginalToolTipLocalValue =
        slot.m_originalToolTipLocalValue;
#endif
    const bool previousHelpTextSynced = slot.m_helpTextSynced;
    const bool previousHelpTextOwned = slot.m_helpTextOwned;
    const bool previousHelpTextPresent =
        slot.m_lastHelpTextPresent;
    const wxString previousHelpText = slot.m_lastHelpText;
    const wxString previousAppliedHelpText =
        slot.m_lastAppliedHelpText;
    const auto previousOriginalHelpTextLocalValue =
        slot.m_originalHelpTextLocalValue;
    const bool previousUIANameSynced = slot.m_uiaNameSynced;
    const bool previousUIANameOwned = slot.m_uiaNameOwned;
    const bool previousUIANameAppOwned = slot.m_uiaNameAppOwned;
    const bool previousUIANameRelinquished =
        slot.m_uiaNameRelinquished;
    const bool previousUIANameSuppressedByLabeledBy =
        slot.m_uiaNameSuppressedByLabeledBy;
    const wxString previousUIAName = slot.m_lastUIAName;
    const wxString previousAppliedUIAName = slot.m_lastAppliedUIAName;
    const auto previousOriginalUIANameLocalValue =
        slot.m_originalUIANameLocalValue;
    unsigned long long automationNameStateRevision =
        slot.m_uiaNameStateRevision;
    const unsigned long long automationNameStyleRevision =
        slot.m_automationNameStyleRevision;
    const unsigned long long automationNameImplicitStyleRevision =
        slot.m_automationNameImplicitStyleRevision;
    const unsigned long long
        previousAutomationNameImplicitStyleInspectedRevision =
            slot.m_automationNameImplicitStyleInspectedRevision;
    const bool hasAutomationNameOverride =
        slot.m_hasAutomationNameOverride;
    const wxString automationNameOverride =
        slot.m_automationNameOverride;
    const unsigned long long automationNameOverrideRevision =
        slot.m_automationNameOverrideRevision;
    const bool hasApplicationAuthoredXaml =
        wxDynamicCast(w, wxWinUIXamlHost) != nullptr;
#if wxUSE_ACCESSIBILITY
    const bool hasWxAccessible = w->GetAccessible() != nullptr;
#else
    const bool hasWxAccessible = false;
#endif
    const bool previousCursorSynced = slot.m_cursorSynced;
    const wxCursor previousCursor = slot.m_lastCursor;
    const unsigned long long previousCursorPolicyGeneration =
        slot.m_lastCursorPolicyGeneration;
    winrt::com_ptr<wxWinUISlotGrid> slotGrid;
    if ( const auto it = gs_slotGrids.find(&slot);
         it != gs_slotGrids.end() )
    {
        slotGrid = it->second;
    }

    const HWND hwndTLW = GetHwndOf(m_tlw);

    // Control rect in TLW client coordinates (physical px) -- the island
    // covers exactly the TLW client area.  The TLW itself can be slotted too
    // (the Window-presentation dialog shell hosts its whole content this
    // way): its slot simply covers the client area.
    wxRect rect;
    if ( w == m_tlw )
    {
        RECT rc;
        ::GetClientRect(hwndTLW, &rc);
        rect = wxRect(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    }
    else
    {
        // All island geometry is physical-LTR: window rects are physical
        // already, so anchor them on the physical client origin.  (See
        // GetClientScreenRect for why ScreenToClient must not be used.)
        const RECT rcCS = GetClientScreenRect();
        RECT wr;
        ::GetWindowRect(hwnd, &wr);
        rect = wxRect(wr.left - rcCS.left, wr.top - rcCS.top,
                      wr.right - wr.left, wr.bottom - wr.top);
    }

    // Clip against every ancestor's client area, like HWND clipping does.
    wxRect vis = rect;

    const double scale = GetScale();
    if ( hasClipRect )
    {
        const wxRect localLimit = ClipRectDIPToPhysical(
            clipRectDIP, scale, rect.GetSize());
        const wxRect limit(rect.x + localLimit.x,
                           rect.y + localLimit.y,
                           localLimit.width,
                           localLimit.height);
        vis.Intersect(limit);
    }
    if ( w != m_tlw )
    {
        const RECT rcCS = GetClientScreenRect();
        for ( wxWindow *p = w->GetParent();
              p && p != m_tlw && !vis.IsEmpty();
              p = p->GetParent() )
        {
            const HWND hp = GetHwndOf(p);
            if ( !hp )
                break;

            RECT rc;
            ::GetClientRect(hp, &rc);
            ::MapWindowPoints(hp, nullptr, reinterpret_cast<POINT *>(&rc), 2);
            if ( rc.left > rc.right )
            {
                const LONG swap = rc.left;
                rc.left = rc.right;
                rc.right = swap;
            }

            vis.Intersect(wxRect(rc.left - rcCS.left, rc.top - rcCS.top,
                                 rc.right - rc.left, rc.bottom - rc.top));
        }
    }

    const auto direction =
        w->GetLayoutDirection() == wxLayout_RightToLeft
            ? winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft
            : winrt::Microsoft::UI::Xaml::FlowDirection::LeftToRight;
    // IsShownOnScreen() does not make minimization an explicit part of its
    // contract. The XAML tree must not remain live/hit-testable while its TLW
    // is hidden or iconic, even if every wx child still has its shown bit.
    const bool tlwVisible =
        hwndTLW && ::IsWindowVisible(hwndTLW) && !::IsIconic(hwndTLW);
    const bool effectiveVisible =
        tlwVisible && w->IsShownOnScreen() && !vis.IsEmpty();
    const bool enabled = w->IsEnabled();

#if wxUSE_TOOLTIPS
    wxToolTip * const tip = w->GetToolTip();
    const bool toolTipPresent = tip != nullptr;
    const wxString toolTipText = tip ? tip->GetTip() : wxString();
    const int toolTipMaximumWidth =
        tip ? tip->GetWinUIMaxWidthAtCreation() : 0;
    const int toolTipDPI =
        toolTipMaximumWidth > 0 ? w->GetDPI().GetWidth() : 0;
    const bool wxToolTipChanged =
        previousToolTipPresent != toolTipPresent ||
        previousToolTip != toolTipText ||
        previousToolTipMaximumWidth != toolTipMaximumWidth ||
        previousToolTipDPI != toolTipDPI;
    const bool toolTipPolicyEnabled = wxWinUIAreToolTipsEnabled();
    bool nextToolTipOwned = previousToolTipOwned;
    auto nextAppliedToolTip = previousAppliedToolTip;
    auto nextOriginalToolTip = previousOriginalToolTip;
    auto nextOriginalToolTipLocalValue =
        previousOriginalToolTipLocalValue;
#endif

    wxString helpText;
#if wxUSE_HELP
    helpText = w->GetHelpText();
#endif
    bool helpTextPresent = !helpText.empty();
#if wxUSE_TOOLTIPS
    if ( !helpTextPresent && toolTipPresent )
    {
        helpText = toolTipText;
        helpTextPresent = true;
    }
#endif
    const bool wxHelpTextChanged =
        previousHelpTextPresent != helpTextPresent ||
        previousHelpText != helpText;
    bool nextHelpTextOwned = previousHelpTextOwned;
    wxString nextAppliedHelpText = previousAppliedHelpText;
    auto nextOriginalHelpTextLocalValue =
        previousOriginalHelpTextLocalValue;

    const wxString uiaLabel =
        hasAutomationNameOverride
            ? wxStripMenuCodes(automationNameOverride, wxStrip_Mnemonics)
            : wxWinUIGetAutomationName(w);
    bool nextUIANameOwned = previousUIANameOwned;
    bool nextUIANameAppOwned = previousUIANameAppOwned;
    bool nextUIANameRelinquished = previousUIANameRelinquished;
    bool nextUIANameSuppressedByLabeledBy =
        previousUIANameSuppressedByLabeledBy;
    wxString nextAppliedUIAName = previousAppliedUIAName;
    auto nextOriginalUIANameLocalValue =
        previousOriginalUIANameLocalValue;

    // Native WM_SETCURSOR walks up the parent chain until a window supplies
    // a cursor. A XAML slot is a separate visual tree and never gets that
    // walk, so resolve the same inherited value here. Parent cursor changes
    // mark the whole host dirty (see wxWinUISetWindowCursor()).
    wxCursor effectiveCursor;
    winrt::Microsoft::UI::Input::InputCursor effectiveInputCursor{ nullptr };
    const unsigned long long resolvedCursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    const bool effectiveCursorResolved =
        ResolveEffectivePointerCursor(
            w, &effectiveInputCursor, &effectiveCursor);
    const bool cursorChanged =
        !previousCursorSynced ||
        previousCursorPolicyGeneration !=
            resolvedCursorPolicyGeneration ||
        !previousCursor.IsSameAs(effectiveCursor);

    // Safe after any reentrant XAML callback: the shared state is invalidated
    // before a slot is removed.  Only return a slot whose generation is still
    // exactly the one snapshotted above.
    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        if ( !state || state->GetHost() != this || state->GetWindow() != w )
            return nullptr;

        wxWinUISlot * const current = FindSlot(w);
        if ( !current ||
             current->m_lifetime != state ||
             current->m_content != content ||
             current->m_semanticTarget != semanticTarget ||
              current->m_contentGeneration != contentGeneration ||
              current->m_syncEpoch != syncEpoch ||
              current->m_uiaNameStateRevision !=
                  automationNameStateRevision ||
              current->m_automationNameStyleRevision !=
                  automationNameStyleRevision ||
              current->m_automationNameImplicitStyleRevision !=
                  automationNameImplicitStyleRevision ||
              current->m_clipRevision != clipRevision ||
              current->m_contentTransactionInProgress ||
             current->m_automationNameOverrideRevision !=
                 automationNameOverrideRevision )
        {
            return nullptr;
        }

#if wxUSE_TOOLTIPS
        if ( wxWinUIAreToolTipsEnabled() != toolTipPolicyEnabled )
            return nullptr;

        wxToolTip * const currentTip = w->GetToolTip();
        if ( currentTip != tip )
            return nullptr;
        if ( currentTip )
        {
            const int currentMaximumWidth =
                currentTip->GetWinUIMaxWidthAtCreation();
            const int currentDPI =
                currentMaximumWidth > 0 ? w->GetDPI().GetWidth() : 0;
            if ( currentTip->GetTip() != toolTipText ||
                 currentMaximumWidth != toolTipMaximumWidth ||
                 currentDPI != toolTipDPI )
            {
                return nullptr;
            }
        }
#endif
        return current;
    };

    SyncSlotAccessibilityAuthority(slot, hasWxAccessible);
    if ( !getCurrentSlot() )
        return;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using winrt::Microsoft::UI::Xaml::Controls::Canvas;
        using winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;

        Canvas::SetLeft(container, rect.x / scale);
        Canvas::SetTop(container, rect.y / scale);
        container.Width(rect.width / scale);
        container.Height(rect.height / scale);
        if ( !getCurrentSlot() )
            return;

        // Follow dynamic direction changes (SetLayoutDirection after
        // creation): the hosted content mirrors, the positions above stay
        // physical.
        if ( container.FlowDirection() != direction )
            container.FlowDirection(direction);

        if ( vis == rect || vis.IsEmpty() )
        {
            container.Clip(nullptr);
        }
        else
        {
            winrt::Microsoft::UI::Xaml::Media::RectangleGeometry geometry;
            geometry.Rect(winrt::Windows::Foundation::Rect
            {
                float((vis.x - rect.x) / scale),
                float((vis.y - rect.y) / scale),
                float(vis.width / scale),
                float(vis.height / scale)
            });
            container.Clip(geometry);
        }
        if ( !getCurrentSlot() )
            return;

        container.Visibility(effectiveVisible ? Visibility::Visible
                                              : Visibility::Collapsed);
        if ( !getCurrentSlot() )
            return;

        // ----- state adapter -----
        // The flush is the one common seam every slot goes through: re-assert
        // the wx-side state here instead of overriding 30 components.  The
        // caches keep the no-change case to a few comparisons.

        // Effective enabled: IsEnabled() folds the ancestors in, which the
        // per-control implementations cannot see (a disabled panel sends no
        // message to its children).  The state goes on the host-owned
        // CARRIER, never on the content itself: Control::IsEnabled(false)
        // is inherited by every descendant control (keyboard focus and UIA
        // state included, composite contents too), while the content's own
        // IsEnabled -- possibly intrinsically false -- is left alone.
        const bool enabledStateNeedsSync =
            previousEnabled != static_cast<int>(enabled) ||
            !carrier ||
            carrier.IsEnabled() != enabled ||
            container.IsHitTestVisible() != enabled;
        if ( enabledStateNeedsSync )
        {
            if ( !carrier )
                return;

            carrier.IsEnabled(enabled);
            if ( !getCurrentSlot() )
                return;

            // IsEnabled callbacks may change the wx effective state or enter
            // a nested flush. Do not apply/cache the old hit-test verdict.
            if ( w->IsEnabled() != enabled ||
                 carrier.IsEnabled() != enabled )
            {
                MarkDirty(w);
                return;
            }

            container.IsHitTestVisible(enabled);
            if ( !getCurrentSlot() )
                return;
            if ( w->IsEnabled() != enabled ||
                 carrier.IsEnabled() != enabled ||
                 container.IsHitTestVisible() != enabled )
            {
                MarkDirty(w);
                return;
            }
        }

#if wxUSE_TOOLTIPS
        // Tooltip ownership is explicit. A tooltip authored in the XAML
        // survives the initial sync untouched. Once wx SetToolTip() supplies
        // an override we remember the exact previous IInspectable (which may
        // be a ToolTip object) and restore it on UnsetToolTip(). A direct
        // application mutation while wx owns the property relinquishes that
        // ownership instead of being overwritten by the next dirty flush.
        if ( content )
        {
            ++slot.m_toolTipPolicySyncDepth;
            const auto toolTipPolicySyncGuard = wxMakeGuard([&slot]()
            {
                wxASSERT_MSG(slot.m_toolTipPolicySyncDepth != 0,
                             "unbalanced WinUI tooltip slot transaction");
                --slot.m_toolTipPolicySyncDepth;
            });
            wxUnusedVar(toolTipPolicySyncGuard);

            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

            const auto actual = ToolTipService::GetToolTip(content);
            const auto actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return;

            if ( !previousToolTipSynced )
            {
                nextOriginalToolTip = actual;
                nextOriginalToolTipLocalValue = actualLocal;
                nextToolTipOwned = false;
                nextAppliedToolTip = nullptr;
                if ( toolTipPresent )
                {
                    if ( !wxWinUISetToolTip(content,
                                            toolTipText,
                                            w,
                                            &nextAppliedToolTip,
                                            toolTipMaximumWidth) )
                        return;
                    nextToolTipOwned =
                        toolTipText.empty() ||
                        static_cast<bool>(nextAppliedToolTip);
                }
            }
            else
            {
                const bool applicationClearedSuppressedNull =
                    previousToolTipSuppressed &&
                    actualLocal ==
                        DependencyProperty::UnsetValue();
                if ( applicationClearedSuppressedNull )
                {
                    if ( previousToolTipOwned &&
                         previousAppliedToolTip &&
                         !wxWinUIRestoreToolTip(
                             content,
                             DependencyProperty::UnsetValue(),
                             previousAppliedToolTip) )
                    {
                        return;
                    }
                    if ( !getCurrentSlot() )
                        return;
                    nextToolTipOwned = false;
                    nextAppliedToolTip = nullptr;
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                }

                if ( previousToolTipSuppressed &&
                     !applicationClearedSuppressedNull &&
                     actual )
                {
                    const bool restoredManagedValue =
                        previousToolTipOwned &&
                        actual == previousAppliedToolTip &&
                        wxWinUIIsManagedToolTipCurrent(
                            content, previousAppliedToolTip);
                    if ( !restoredManagedValue )
                    {
                        // A non-null value appearing while our global policy
                        // owns the null is a direct application mutation.
                        // Preserve it as the new baseline. When policy is
                        // still disabled it is detached again below; during
                        // re-enable it stays visible.
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                        nextOriginalToolTip = actual;
                        nextOriginalToolTipLocalValue = actualLocal;
                        if ( previousToolTipOwned &&
                             previousAppliedToolTip &&
                             !wxWinUIRestoreToolTip(
                                 content,
                                 winrt::Microsoft::UI::Xaml::
                                     DependencyProperty::UnsetValue(),
                                 previousAppliedToolTip) )
                        {
                            return;
                        }
                        if ( !getCurrentSlot() )
                            return;
                    }
                }

                if ( !previousToolTipSuppressed &&
                     nextToolTipOwned &&
                     actual != previousAppliedToolTip &&
                     !wxWinUIIsManagedToolTipSuppressed(
                         content, previousAppliedToolTip) )
                {
                    // The application replaced our exact object directly.
                    // Text equality is irrelevant to attached-property
                    // ownership.
                    nextToolTipOwned = false;
                    nextAppliedToolTip = nullptr;
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                    if ( previousAppliedToolTip &&
                         !wxWinUIRestoreToolTip(
                             content,
                             winrt::Microsoft::UI::Xaml::
                                 DependencyProperty::UnsetValue(),
                             previousAppliedToolTip) )
                        return;
                    if ( !getCurrentSlot() )
                        return;
                }
                else if ( !nextToolTipOwned &&
                          !previousToolTipSuppressed &&
                          !wxToolTipChanged )
                {
                    // Track direct authored-XAML changes even when no wx-side
                    // value changed. This baseline is what a later global
                    // disable/enable transaction must restore.
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                }

                if ( previousToolTipSuppressed &&
                     toolTipPolicyEnabled )
                {
                    // Re-enable is a restore transaction, not an application
                    // mutation: null is the value installed by the previous
                    // global-disable pass.
                    if ( nextToolTipOwned && toolTipPresent )
                    {
                        if ( actual != nextAppliedToolTip ||
                             wxToolTipChanged )
                        {
                            if ( !wxWinUISetToolTip(content,
                                                   toolTipText,
                                                   w,
                                                   &nextAppliedToolTip,
                                                   toolTipMaximumWidth) )
                                return;
                            nextToolTipOwned =
                                toolTipText.empty() ||
                                static_cast<bool>(nextAppliedToolTip);
                        }
                    }
                    else
                    {
                        if ( !actualLocal )
                        {
                            wxWinUIRestoreToolTipLocalValue(
                                content,
                                nextOriginalToolTipLocalValue);
                        }
                        else
                        {
                            nextOriginalToolTip = actual;
                            nextOriginalToolTipLocalValue =
                                actualLocal;
                        }
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                    }
                }
                else if ( nextToolTipOwned )
                {
                    if ( !toolTipPresent )
                    {
                        bool restored = true;
                        if ( nextAppliedToolTip )
                        {
                            restored = wxWinUIRestoreToolTip(
                                content,
                                nextOriginalToolTipLocalValue,
                                nextAppliedToolTip);
                        }
                        else
                        {
                            // An explicit empty wx tooltip owns null without a
                            // registry object. Restore only while null is still
                            // authoritative; a direct XAML object wins.
                            const auto actualNow =
                                ToolTipService::GetToolTip(content);
                            if ( !actualNow &&
                                 !wxWinUIReadToolTipLocalValue(content) )
                            {
                                wxWinUIRestoreToolTipLocalValue(
                                    content,
                                    nextOriginalToolTipLocalValue);
                            }
                        }
                        if ( !restored )
                        {
                            return;
                        }
                        nextToolTipOwned = false;
                        nextAppliedToolTip = nullptr;
                    }
                    else if ( wxToolTipChanged )
                    {
                        if ( !wxWinUISetToolTip(content,
                                                toolTipText,
                                                w,
                                                &nextAppliedToolTip,
                                                toolTipMaximumWidth) )
                            return;
                        nextToolTipOwned =
                            toolTipText.empty() ||
                            static_cast<bool>(nextAppliedToolTip);
                    }
                }
                else if ( toolTipPresent && wxToolTipChanged )
                {
                    // A new explicit wx-side value reclaims ownership,
                    // preserving whatever XAML value currently exists.
                    nextOriginalToolTip = actual;
                    nextOriginalToolTipLocalValue = actualLocal;
                    if ( !wxWinUISetToolTip(content,
                                            toolTipText,
                                            w,
                                            &nextAppliedToolTip,
                                            toolTipMaximumWidth) )
                        return;
                    nextToolTipOwned =
                        toolTipText.empty() ||
                        static_cast<bool>(nextAppliedToolTip);
                }
            }

            if ( !getCurrentSlot() )
                return;

            // ToolTipService normally reacts to target state itself, but an
            // explicitly-open ToolTip can otherwise outlive a wx Hide() or
            // Disable() transition. Close the current object without
            // detaching it, so showing/enabling the slot preserves ownership
            // and lets WinUI resume its normal pointer/focus activation.
            if ( toolTipPolicyEnabled &&
                 (!enabled || !effectiveVisible) )
            {
                const auto visible =
                    ToolTipService::GetToolTip(content);
                if ( const auto object =
                         visible.try_as<
                             winrt::Microsoft::UI::Xaml::Controls::ToolTip>();
                     object && object.IsOpen() )
                {
                    object.IsOpen(false);
                    if ( !getCurrentSlot() )
                        return;

                    // IsOpen is a re-entrant dependency property. If a Closed
                    // handler replaced the attached value, abandon this stale
                    // snapshot and let the property observer resynchronize it.
                    if ( ToolTipService::GetToolTip(content) != visible )
                    {
                        MarkDirty(w);
                        return;
                    }
                }
            }

            bool nextToolTipSuppressed = false;
            if ( !toolTipPolicyEnabled )
            {
                // WinUI exposes no process-wide ToolTipService switch.
                // Close and detach every synchronous application replacement
                // in this same call. Null is ours only after an exact
                // post-setter re-read.
                const wxWinUIToolTipSuppressionResult result =
                    wxWinUISuppressSlotToolTip(
                        content, getCurrentSlot,
                        nextToolTipOwned,
                        nextAppliedToolTip,
                        nextOriginalToolTip,
                        nextOriginalToolTipLocalValue);
                if ( result !=
                        wxWinUIToolTipSuppressionResult::Suppressed )
                {
                    if ( result ==
                            wxWinUIToolTipSuppressionResult::BudgetExhausted )
                    {
                        wxLogWarning(
                            "wxWinUI: synchronous tooltip suppression "
                            "replacement budget exhausted");
                        if ( wxWinUISlot * const current = getCurrentSlot() )
                            current->NudgeDirty();
                    }
                    else if ( result ==
                              wxWinUIToolTipSuppressionResult::Failed )
                    {
                        if ( wxWinUISlot * const current = getCurrentSlot() )
                            current->NudgeDirty();
                    }
                    return;
                }
                nextToolTipSuppressed = true;
            }

            if ( wxWinUISlot * const current = getCurrentSlot() )
                current->m_toolTipSuppressed = nextToolTipSuppressed;
            else
                return;
        }

        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            current->m_toolTipSynced = true;
            current->m_toolTipOwned = nextToolTipOwned;
            current->m_lastToolTipPresent = toolTipPresent;
            current->m_lastToolTip = toolTipText;
            current->m_lastToolTipMaximumWidth =
                toolTipMaximumWidth;
            current->m_lastToolTipDPI = toolTipDPI;
            current->m_lastAppliedToolTip = nextAppliedToolTip;
            current->m_originalToolTip = nextOriginalToolTip;
            current->m_originalToolTipLocalValue =
                nextOriginalToolTipLocalValue;
        }
        else
            return;
#endif // wxUSE_TOOLTIPS

        // UIA HelpText is the semantic counterpart of wx context help and,
        // when that is absent, of the visible tooltip. ToolTipService alone
        // is not consulted by FrameworkElementAutomationPeer::GetHelpText().
        // Preserve local-value ownership independently so an application can
        // author either property without the host silently clobbering it.
        if ( semanticTarget )
        {
            const auto property =
                AutomationProperties::HelpTextProperty();
            auto actualLocal =
                semanticTarget.ReadLocalValue(property);
            const bool actualHasLocal =
                actualLocal != DependencyProperty::UnsetValue();
            const wxString actual(
                AutomationProperties::GetHelpText(
                    semanticTarget).c_str());
            if ( !getCurrentSlot() )
                return;

            const auto readPostSetterHelpText =
                [&](winrt::Windows::Foundation::IInspectable *local,
                    bool *hasLocal,
                    wxString *value)
                {
                    if ( !getCurrentSlot() )
                        return false;
                    *local =
                        semanticTarget.ReadLocalValue(property);
                    *hasLocal =
                        *local != DependencyProperty::UnsetValue();
                    *value = wxString(
                        AutomationProperties::GetHelpText(
                            semanticTarget).c_str());
                    return getCurrentSlot() != nullptr;
                };

            const auto publishWxHelpText =
                [&](const wxString& desired)
                {
                    AutomationProperties::SetHelpText(
                        semanticTarget,
                        winrt::hstring(desired.ToStdWstring()));

                    winrt::Windows::Foundation::IInspectable postLocal{
                        nullptr
                    };
                    bool postHasLocal = false;
                    wxString postValue;
                    if ( !readPostSetterHelpText(
                             &postLocal, &postHasLocal, &postValue) )
                    {
                        return false;
                    }

                    // A dependency-property callback may replace or clear the
                    // scalar synchronously. Re-read it before claiming
                    // ownership; the last application write wins.
                    if ( postHasLocal && postValue == desired )
                    {
                        nextHelpTextOwned = true;
                        nextAppliedHelpText = desired;
                    }
                    else
                    {
                        nextHelpTextOwned = false;
                        nextOriginalHelpTextLocalValue = postLocal;
                        nextAppliedHelpText.clear();
                    }
                    return true;
                };

            const auto restoreHelpTextBaseline = [&]()
            {
                if ( !nextOriginalHelpTextLocalValue ||
                     nextOriginalHelpTextLocalValue ==
                         DependencyProperty::UnsetValue() )
                {
                    semanticTarget.ClearValue(property);
                }
                else
                {
                    semanticTarget.SetValue(
                        property,
                        nextOriginalHelpTextLocalValue);
                }

                winrt::Windows::Foundation::IInspectable postLocal{
                    nullptr
                };
                bool postHasLocal = false;
                wxString postValue;
                if ( !readPostSetterHelpText(
                         &postLocal, &postHasLocal, &postValue) )
                {
                    return false;
                }
                wxUnusedVar(postHasLocal);
                wxUnusedVar(postValue);

                nextHelpTextOwned = false;
                nextOriginalHelpTextLocalValue = postLocal;
                nextAppliedHelpText.clear();
                return true;
            };

            if ( !previousHelpTextSynced )
            {
                nextOriginalHelpTextLocalValue = actualLocal;
                nextHelpTextOwned = false;
                if ( helpTextPresent )
                {
                    if ( !publishWxHelpText(helpText) )
                        return;
                }
            }
            else
            {
                if ( nextHelpTextOwned &&
                     (!actualHasLocal ||
                      actual != previousAppliedHelpText) )
                {
                    // A direct XAML mutation (including ClearValue()) wins
                    // until the wx-side help value changes again.
                    nextHelpTextOwned = false;
                    nextOriginalHelpTextLocalValue = actualLocal;
                    nextAppliedHelpText.clear();
                }

                if ( nextHelpTextOwned )
                {
                    if ( !helpTextPresent )
                    {
                        if ( !restoreHelpTextBaseline() )
                            return;
                    }
                    else if ( wxHelpTextChanged )
                    {
                        if ( !publishWxHelpText(helpText) )
                            return;
                    }
                }
                else if ( helpTextPresent && wxHelpTextChanged )
                {
                    nextOriginalHelpTextLocalValue = actualLocal;
                    if ( !publishWxHelpText(helpText) )
                        return;
                }
            }

            if ( !getCurrentSlot() )
                return;
        }

        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            current->m_helpTextSynced = true;
            current->m_helpTextOwned = nextHelpTextOwned;
            current->m_lastHelpTextPresent = helpTextPresent;
            current->m_lastHelpText = helpText;
            current->m_lastAppliedHelpText = nextAppliedHelpText;
            current->m_originalHelpTextLocalValue =
                nextOriginalHelpTextLocalValue;
        }
        else
            return;

        // Automation name: claim only a property for which neither a local
        // value nor an effective/style-authored value exists. ReadLocalValue
        // alone is insufficient: Style setters live at a lower DP precedence,
        // and a local wx value can mask a Style assigned later. An authored
        // LabeledBy relation suspends this scalar fallback without becoming a
        // permanent Name relinquishment.
        if ( semanticTarget )
        {
            const auto nameProperty =
                AutomationProperties::NameProperty();

            struct AutomationNameObservation
            {
                winrt::Windows::Foundation::IInspectable
                    local{ nullptr };
                wxString localValue;
                wxString effective;
                bool hasLocal = false;
                bool localIsString = false;
                bool hasBinding = false;
                bool styleAuthors = false;
                bool hasLabeledBy = false;

                bool IsAuthored() const
                {
                    return hasLocal ||
                           hasBinding ||
                           styleAuthors ||
                           !effective.empty();
                }
            };

            const auto observeAutomationName = [&]()
            {
                AutomationNameObservation observation;
                observation.local =
                    semanticTarget.ReadLocalValue(nameProperty);
                observation.hasLocal =
                    observation.local !=
                        DependencyProperty::UnsetValue();
                observation.hasBinding =
                    wxWinUIHasBinding(
                        semanticTarget, nameProperty);
                if ( observation.hasLocal )
                {
                    observation.localIsString =
                        wxWinUITryGetLocalStringValue(
                            observation.local,
                            &observation.localValue);
                }
                observation.effective = wxString(
                    AutomationProperties::GetName(
                        semanticTarget).c_str());
                observation.styleAuthors =
                    wxWinUIStyleAuthorsAutomationName(
                        semanticTarget,
                        !previousUIANameSynced ||
                        automationNameImplicitStyleRevision !=
                            previousAutomationNameImplicitStyleInspectedRevision ||
                        (hasApplicationAuthoredXaml &&
                         !previousUIANameRelinquished &&
                         (previousUIANameOwned || !uiaLabel.empty())),
                        slot.GetContainer().Resources());
                observation.hasLabeledBy =
                    AutomationProperties::GetLabeledBy(
                        semanticTarget) != nullptr;
                return observation;
            };

            AutomationNameObservation observed =
                observeAutomationName();
            if ( !getCurrentSlot() )
                return;

            struct AutomationNameProvenance
            {
                bool synced = true;
                bool owned = false;
                bool appOwned = false;
                bool relinquished = false;
                bool suppressedByLabeledBy = false;
                wxString lastDesired;
                wxString lastApplied;
                winrt::Windows::Foundation::IInspectable
                    originalLocalValue{ nullptr };
                unsigned long long implicitStyleInspectedRevision = 0;
            };

            const auto captureNextAutomationNameState = [&]()
            {
                AutomationNameProvenance value;
                value.owned = nextUIANameOwned;
                value.appOwned = nextUIANameAppOwned;
                value.relinquished = nextUIANameRelinquished;
                value.suppressedByLabeledBy =
                    nextUIANameSuppressedByLabeledBy;
                value.lastDesired = uiaLabel;
                value.lastApplied = nextAppliedUIAName;
                value.originalLocalValue =
                    nextOriginalUIANameLocalValue;
                value.implicitStyleInspectedRevision =
                    automationNameImplicitStyleRevision;
                return value;
            };
            const auto assignNextAutomationNameState =
                [&](const AutomationNameProvenance& value)
                {
                    nextUIANameOwned = value.owned;
                    nextUIANameAppOwned = value.appOwned;
                    nextUIANameRelinquished = value.relinquished;
                    nextUIANameSuppressedByLabeledBy =
                        value.suppressedByLabeledBy;
                    nextAppliedUIAName = value.lastApplied;
                    nextOriginalUIANameLocalValue =
                        value.originalLocalValue;
                };
            const auto publishAutomationNameState =
                [&](const AutomationNameProvenance& value)
                {
                    wxWinUISlot * const current = getCurrentSlot();
                    if ( !current )
                        return false;
                    current->PublishAutomationNameState(
                        value.synced,
                        value.owned,
                        value.appOwned,
                        value.relinquished,
                        value.suppressedByLabeledBy,
                        value.lastDesired,
                        value.lastApplied,
                        value.originalLocalValue,
                        value.implicitStyleInspectedRevision);
                    automationNameStateRevision =
                        current->m_uiaNameStateRevision;
                    return true;
                };
            const auto sameAutomationNameSource =
                [](const AutomationNameObservation& lhs,
                   const AutomationNameObservation& rhs)
                {
                    return lhs.hasLocal == rhs.hasLocal &&
                           lhs.localIsString == rhs.localIsString &&
                           lhs.localValue == rhs.localValue &&
                           lhs.hasBinding == rhs.hasBinding &&
                           lhs.effective == rhs.effective;
                };

            // Remove the exact local scalar previously supplied by wx, but
            // never overwrite a direct application replacement. Re-read after
            // the setter because dependency-property callbacks may destroy,
            // replace or mutate this same content synchronously.
            const auto restoreWxAutomationName =
                [&](const wxString& expectedLocal,
                    const AutomationNameProvenance& restoredState)
                {
                    if ( observed.hasBinding ||
                         !observed.hasLocal ||
                         !observed.localIsString ||
                         observed.localValue != expectedLocal )
                    {
                        return true;
                    }

                    const AutomationNameObservation beforeWrite =
                        observed;
                    const AutomationNameProvenance beforeState =
                        captureNextAutomationNameState();
                    assignNextAutomationNameState(restoredState);
                    if ( !publishAutomationNameState(restoredState) )
                        return false;

                    try
                    {
                        if ( !beforeState.originalLocalValue ||
                             beforeState.originalLocalValue ==
                             DependencyProperty::UnsetValue() )
                        {
                            semanticTarget.ClearValue(nameProperty);
                        }
                        else
                        {
                            semanticTarget.SetValue(
                                nameProperty,
                                beforeState.originalLocalValue);
                        }
                    }
                    catch ( ... )
                    {
                        // A WinRT setter may throw before or after mutating.
                        // Roll provenance back only when the exact slot and
                        // exact physical source are demonstrably unchanged.
                        if ( getCurrentSlot() )
                        {
                            try
                            {
                                const AutomationNameObservation after =
                                    observeAutomationName();
                                if ( getCurrentSlot() &&
                                     sameAutomationNameSource(
                                         beforeWrite, after) )
                                {
                                    assignNextAutomationNameState(
                                        beforeState);
                                    publishAutomationNameState(beforeState);
                                    if ( wxWinUISlot * const current =
                                             getCurrentSlot() )
                                    {
                                        current->NudgeDirty();
                                    }
                                }
                                else if ( wxWinUISlot * const current =
                                              getCurrentSlot() )
                                {
                                    current->NudgeDirty();
                                }
                            }
                            catch ( ... )
                            {
                                if ( wxWinUISlot * const current =
                                         getCurrentSlot() )
                                {
                                    current->NudgeDirty();
                                }
                            }
                        }
                        throw;
                    }
                    if ( !getCurrentSlot() )
                        return false;

                    observed = observeAutomationName();
                    return getCurrentSlot() != nullptr;
                };

            const auto publishWxAutomationName =
                [&](const wxString& desired)
                {
                    const AutomationNameObservation beforeWrite =
                        observed;
                    const AutomationNameProvenance beforeState =
                        captureNextAutomationNameState();
                    nextUIANameOwned = true;
                    nextUIANameAppOwned = false;
                    nextUIANameRelinquished = false;
                    nextUIANameSuppressedByLabeledBy = false;
                    nextAppliedUIAName = desired;
                    const AutomationNameProvenance publishedState =
                        captureNextAutomationNameState();
                    if ( !publishAutomationNameState(publishedState) )
                        return false;

                    try
                    {
                        AutomationProperties::SetName(
                            semanticTarget,
                            winrt::hstring(desired.ToStdWstring()));
                    }
                    catch ( ... )
                    {
                        if ( getCurrentSlot() )
                        {
                            try
                            {
                                const AutomationNameObservation after =
                                    observeAutomationName();
                                if ( getCurrentSlot() &&
                                     sameAutomationNameSource(
                                         beforeWrite, after) )
                                {
                                    assignNextAutomationNameState(
                                        beforeState);
                                    publishAutomationNameState(beforeState);
                                    if ( wxWinUISlot * const current =
                                             getCurrentSlot() )
                                    {
                                        current->NudgeDirty();
                                    }
                                }
                                else if ( wxWinUISlot * const current =
                                              getCurrentSlot() )
                                {
                                    current->NudgeDirty();
                                }
                            }
                            catch ( ... )
                            {
                                if ( wxWinUISlot * const current =
                                         getCurrentSlot() )
                                {
                                    current->NudgeDirty();
                                }
                            }
                        }
                        throw;
                    }
                    if ( !getCurrentSlot() )
                        return false;

                    observed = observeAutomationName();
                    if ( !getCurrentSlot() )
                        return false;

                    const bool wxLocalStillExact =
                        !observed.hasBinding &&
                        observed.hasLocal &&
                        observed.localIsString &&
                        observed.localValue == desired;
                    if ( wxLocalStillExact &&
                         !observed.styleAuthors &&
                         !observed.hasLabeledBy &&
                         observed.effective == desired )
                    {
                        nextUIANameOwned = true;
                        nextUIANameAppOwned = false;
                        nextUIANameRelinquished = false;
                        nextUIANameSuppressedByLabeledBy = false;
                        nextAppliedUIAName = desired;
                    }
                    else
                    {
                        // A setter callback, animation or newly-assigned Style
                        // superseded the value. If our local scalar still
                        // exists, remove it now so it cannot mask/leak after
                        // the external source goes away or the element detaches.
                        AutomationNameProvenance restoredState =
                            captureNextAutomationNameState();
                        restoredState.owned = false;
                        restoredState.lastApplied.clear();
                        restoredState.originalLocalValue = nullptr;
                        if ( observed.hasLabeledBy &&
                             !observed.styleAuthors )
                        {
                            restoredState.appOwned = false;
                            restoredState.relinquished = false;
                            restoredState.suppressedByLabeledBy = true;
                        }
                        else
                        {
                            restoredState.appOwned = true;
                            restoredState.relinquished = true;
                            restoredState.suppressedByLabeledBy = false;
                        }
                        if ( wxLocalStillExact &&
                             !restoreWxAutomationName(
                                 desired, restoredState) )
                        {
                            return false;
                        }
                        nextUIANameOwned = false;
                        nextAppliedUIAName.clear();
                        nextOriginalUIANameLocalValue = nullptr;
                        if ( !observed.IsAuthored() &&
                             observed.hasLabeledBy )
                        {
                            nextUIANameAppOwned = false;
                            nextUIANameRelinquished = false;
                            nextUIANameSuppressedByLabeledBy = true;
                        }
                        else
                        {
                            nextUIANameAppOwned = true;
                            nextUIANameRelinquished = true;
                            nextUIANameSuppressedByLabeledBy = false;
                        }
                    }
                    return true;
                };

            if ( !previousUIANameSynced )
            {
                nextUIANameOwned = false;
                // A local or Style-authored empty string is meaningful:
                // applications use it to keep a visual out of narration.
                nextUIANameAppOwned = observed.IsAuthored();
                nextUIANameRelinquished = nextUIANameAppOwned;
                nextUIANameSuppressedByLabeledBy = false;
                if ( !nextUIANameRelinquished &&
                     observed.hasLabeledBy )
                {
                    // A typed relation is already the application's naming
                    // source. Keep Name completely unset so its scalar
                    // fallback cannot mask LabeledBy in the UIA peer.
                    nextUIANameSuppressedByLabeledBy = true;
                }
                else if ( !nextUIANameRelinquished &&
                          !uiaLabel.empty() )
                {
                    nextOriginalUIANameLocalValue = observed.local;
                    if ( !publishWxAutomationName(uiaLabel) )
                        return;
                }
            }
            else
            {
                if ( nextUIANameOwned && observed.hasLabeledBy )
                {
                    // Relinquish only the exact local scalar wx supplied.
                    // A simultaneous direct Name replacement remains
                    // application-owned and is never cleared.
                    const bool wxLocalStillExact =
                        !observed.hasBinding &&
                        observed.hasLocal &&
                        observed.localIsString &&
                        observed.localValue ==
                            previousAppliedUIAName;
                    AutomationNameProvenance suppressedState =
                        captureNextAutomationNameState();
                    suppressedState.owned = false;
                    suppressedState.appOwned = false;
                    suppressedState.relinquished = false;
                    suppressedState.suppressedByLabeledBy = true;
                    suppressedState.lastApplied.clear();
                    suppressedState.originalLocalValue = nullptr;
                    if ( !restoreWxAutomationName(
                             previousAppliedUIAName,
                             suppressedState) )
                    {
                        return;
                    }

                    nextUIANameOwned = false;
                    nextAppliedUIAName.clear();
                    nextOriginalUIANameLocalValue = nullptr;
                    if ( !wxLocalStillExact ||
                         observed.IsAuthored() )
                    {
                        // Missing/different local state before our restore is
                        // a direct application mutation (ClearValue included),
                        // not a reversible consequence of LabeledBy.
                        nextUIANameAppOwned = true;
                        nextUIANameRelinquished = true;
                        nextUIANameSuppressedByLabeledBy = false;
                    }
                    else
                    {
                        nextUIANameAppOwned = false;
                        nextUIANameRelinquished = false;
                        nextUIANameSuppressedByLabeledBy = true;
                    }
                }

                if ( nextUIANameOwned )
                {
                    const bool wxLocalStillExact =
                        !observed.hasBinding &&
                        observed.hasLocal &&
                        observed.localIsString &&
                        observed.localValue ==
                            previousAppliedUIAName;
                    if ( !wxLocalStillExact )
                    {
                        // ClearValue() is an authored decision too. Permanent
                        // relinquishment prevents wx from reclaiming the
                        // effective empty default on the next label change.
                        nextUIANameOwned = false;
                        nextUIANameAppOwned = true;
                        nextUIANameRelinquished = true;
                        nextUIANameSuppressedByLabeledBy = false;
                        nextAppliedUIAName.clear();
                        nextOriginalUIANameLocalValue = nullptr;
                    }
                    else if ( observed.styleAuthors ||
                              observed.effective !=
                                  previousAppliedUIAName )
                    {
                        // Style has lower precedence than our local value, and
                        // an animation has higher precedence. Both are direct
                        // application authorship: restore our baseline so the
                        // external source is the sole authority.
                        AutomationNameProvenance relinquishedState =
                            captureNextAutomationNameState();
                        relinquishedState.owned = false;
                        relinquishedState.appOwned = true;
                        relinquishedState.relinquished = true;
                        relinquishedState.suppressedByLabeledBy = false;
                        relinquishedState.lastApplied.clear();
                        relinquishedState.originalLocalValue = nullptr;
                        if ( !restoreWxAutomationName(
                                 previousAppliedUIAName,
                                 relinquishedState) )
                        {
                            return;
                        }
                        nextUIANameOwned = false;
                        nextUIANameAppOwned = true;
                        nextUIANameRelinquished = true;
                        nextUIANameSuppressedByLabeledBy = false;
                        nextAppliedUIAName.clear();
                        nextOriginalUIANameLocalValue = nullptr;
                    }
                    else if ( uiaLabel.empty() )
                    {
                        // wx voluntarily stopped supplying a semantic label.
                        // Reveal the exact pre-wx baseline; unlike an
                        // application mutation, an actually-unset result may
                        // be claimed again by a later non-empty label.
                        AutomationNameProvenance freeState =
                            captureNextAutomationNameState();
                        freeState.owned = false;
                        freeState.appOwned = false;
                        freeState.relinquished = false;
                        freeState.suppressedByLabeledBy = false;
                        freeState.lastApplied.clear();
                        freeState.originalLocalValue = nullptr;
                        if ( !restoreWxAutomationName(
                                 previousAppliedUIAName,
                                 freeState) )
                        {
                            return;
                        }
                        nextUIANameOwned = false;
                        nextUIANameAppOwned = observed.IsAuthored();
                        nextUIANameRelinquished =
                            nextUIANameAppOwned;
                        nextUIANameSuppressedByLabeledBy = false;
                        nextAppliedUIAName.clear();
                        nextOriginalUIANameLocalValue = nullptr;
                    }
                    else if ( uiaLabel != previousUIAName )
                    {
                        if ( !publishWxAutomationName(uiaLabel) )
                            return;
                    }
                }

                if ( !nextUIANameOwned && nextUIANameRelinquished )
                {
                    // Keep this diagnostic bit truthful without weakening the
                    // permanent relinquishment: empty is an intentional
                    // application value too, not permission for wx to reclaim.
                    nextUIANameAppOwned = observed.IsAuthored();
                    nextUIANameSuppressedByLabeledBy = false;
                }
                else if ( !nextUIANameOwned )
                {
                    if ( observed.IsAuthored() )
                    {
                        nextUIANameAppOwned = true;
                        nextUIANameRelinquished = true;
                        nextUIANameSuppressedByLabeledBy = false;
                    }
                    else if ( observed.hasLabeledBy )
                    {
                        nextUIANameAppOwned = false;
                        nextUIANameRelinquished = false;
                        nextUIANameSuppressedByLabeledBy = true;
                    }
                    else
                    {
                        nextUIANameAppOwned = false;
                        nextUIANameSuppressedByLabeledBy = false;
                        if ( !uiaLabel.empty() &&
                             (uiaLabel != previousUIAName ||
                              previousUIANameOwned ||
                              previousUIANameAppOwned ||
                              previousUIANameSuppressedByLabeledBy) )
                        {
                            nextOriginalUIANameLocalValue =
                                observed.local;
                            if ( !publishWxAutomationName(uiaLabel) )
                                return;
                        }
                    }
                }
            }

            if ( !getCurrentSlot() )
                return;
        }

        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            current->PublishAutomationNameState(
                true,
                nextUIANameOwned,
                nextUIANameAppOwned,
                nextUIANameRelinquished,
                nextUIANameSuppressedByLabeledBy,
                uiaLabel,
                nextAppliedUIAName,
                nextOriginalUIANameLocalValue,
                automationNameImplicitStyleRevision);
            automationNameStateRevision =
                current->m_uiaNameStateRevision;
        }
        else
            return;

        if ( cursorChanged )
        {
            // A missing implementation object is a transient registration
            // failure: leave the cache uncommitted so the next flush retries.
            if ( !effectiveCursorResolved ||
                 gs_cursorPolicyGeneration !=
                    resolvedCursorPolicyGeneration ||
                 !slotGrid ||
                 !slotGrid->SetWindowCursor(effectiveInputCursor) )
            {
                return;
            }
            ++gs_slotCursorSets;
            if ( gs_cursorPolicyGeneration !=
                    resolvedCursorPolicyGeneration ||
                 !getCurrentSlot() )
                return;

            wxWinUISlot * const current = getCurrentSlot();
            if ( !current )
                return;
            current->m_cursorSynced = true;
            current->m_lastCursor = effectiveCursor;
            current->m_lastCursorPolicyGeneration =
                resolvedCursorPolicyGeneration;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot geometry sync", e);
        return;
    }

    // Commit caches only after every setter succeeded and the same slot
    // generation is still registered.  An exception or reentrant removal
    // leaves the old cache values, forcing a retry on the next dirty flush.
    wxWinUISlot * const current = getCurrentSlot();
    if ( !current )
        return;
    try
    {
        if ( !carrier ||
             w->IsEnabled() != enabled ||
             carrier.IsEnabled() != enabled ||
             container.IsHitTestVisible() != enabled )
        {
            MarkDirty(w);
            return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("verify slot enabled state", e);
        MarkDirty(w);
        return;
    }

    current->m_hwnd = wxHwnd;
    current->m_rectPx = rect;
    current->m_visibleRectPx = vis;
    current->m_visible = effectiveVisible;
    current->m_lastEnabled = static_cast<int>(enabled);

    // Consume first: the callback may synchronously run another FlushSync().
    // The outer frame has already snapshotted its structural generation and
    // will refuse to publish if this hook changes it.
    std::function<void (wxWindow *)> slotSyncedHook =
        std::move(gs_testSlotSyncedHook);
    if ( slotSyncedHook )
        slotSyncedHook(w);
}

bool wxWinUITopLevelHost::RecomputeZOrder(
    unsigned long long expectedGeneration)
{
    // Recompute can itself be entered by a nested FlushSync() from a XAML
    // callback. Both frames must keep the host alive independently until
    // their local snapshots have been abandoned.
    OperationGuard hostOperation(this);
    if ( m_shuttingDown ||
         m_structureGeneration != expectedGeneration )
    {
        return false;
    }

    // USER32, not wxWindowList insertion order, is the authority after
    // Raise(), Lower() or a direct SetWindowPos(). Snapshot the physical HWND
    // tree bottom-to-top before touching XAML: SetZIndex can synchronously
    // run application code, so the application phase below uses only
    // invalidatable slot lifetimes and revalidates every native identity.
    struct SlotOrderSnapshot
    {
        std::shared_ptr<wxWinUISlotLifetime> state;
        winrt::Microsoft::UI::Xaml::Controls::Grid container{ nullptr };
        HWND hwnd = nullptr;
        unsigned long long hwndGeneration = 0;
        RECT visibleRectScreen = {};
        bool visible = false;
    };

    const HWND hwndTLW = GetHwndOf(m_tlw);
    const RECT clientScreen = GetClientScreenRect();
    std::vector<SlotOrderSnapshot> slots;
    std::map<HWND, std::size_t> slotByHwnd;
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        const std::shared_ptr<wxWinUISlotLifetime> state =
            slot ? slot->m_lifetime : nullptr;
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        const WXHWND wxHwnd = window ? window->GetHWND() : nullptr;
        const HWND hwnd = static_cast<HWND>(wxHwnd);
        const unsigned long long generation =
            hwnd ? wxWinUIMSWGetHwndGeneration(window, wxHwnd) : 0;
        if ( !slot || !slot->m_container || !state ||
             state->GetHost() != this || !hwnd || !generation )
        {
            continue;
        }

        SlotOrderSnapshot snapshot;
        snapshot.state = state;
        snapshot.container = slot->m_container;
        snapshot.hwnd = hwnd;
        snapshot.hwndGeneration = generation;
        snapshot.visible =
            slot->m_visible && !slot->m_visibleRectPx.IsEmpty();
        if ( snapshot.visible )
        {
            const wxRect& visible = slot->m_visibleRectPx;
            snapshot.visibleRectScreen =
            {
                clientScreen.left + visible.x,
                clientScreen.top + visible.y,
                clientScreen.left + visible.x + visible.width,
                clientScreen.top + visible.y + visible.height
            };
        }

        const std::size_t index = slots.size();
        slots.push_back(std::move(snapshot));
        if ( !slotByHwnd.emplace(hwnd, index).second )
        {
            wxWinUIDebugLog(
                "wxWinUI: duplicate slot HWND in z-order snapshot: %p",
                static_cast<void *>(hwnd));
        }
    }

    // Identify exact wx-owned HWNDs separately from implementation HWNDs.
    // Native implementation children of a control don't constitute an
    // application-requested native/XAML interleave.
    std::set<HWND> wxHwnds;
    std::function<void (wxWindow *)> collectWxHwnds =
        [&](wxWindow *parent)
        {
            if ( !parent )
                return;
            for ( wxWindow *child : parent->GetChildren() )
            {
                const HWND hwnd = GetHwndOf(child);
                if ( hwnd )
                    wxHwnds.insert(hwnd);
                collectWxHwnds(child);
            }
        };
    collectWxHwnds(m_tlw);

    struct NativeSurface
    {
        RECT bounds = {};
        // Exact visible region in screen coordinates. A shared handle keeps
        // SurfaceSummary cheap to concatenate while guaranteeing DeleteObject
        // on every early return and reentrant-abort path.
        std::shared_ptr<void> region;
    };

    struct SurfaceSummary
    {
        std::vector<RECT> xaml;
        std::vector<NativeSurface> native;
    };

    const auto overlaps =
        [](const std::vector<RECT>& below,
           const std::vector<NativeSurface>& above) -> bool
        {
            for ( const RECT& lhs : below )
            {
                for ( const NativeSurface& rhs : above )
                {
                    RECT intersection = {};
                    if ( ::IntersectRect(
                             &intersection, &lhs, &rhs.bounds) &&
                         intersection.right > intersection.left &&
                         intersection.bottom > intersection.top &&
                         rhs.region &&
                         ::RectInRegion(
                             static_cast<HRGN>(rhs.region.get()),
                             &intersection) )
                    {
                        return true;
                    }
                }
            }
            return false;
        };

    const auto makeRegion =
        [](HRGN region) -> std::shared_ptr<void>
        {
            return std::shared_ptr<void>(
                region,
                [](void *value)
                {
                    if ( value )
                        ::DeleteObject(static_cast<HRGN>(value));
                });
        };

    const auto makeNativeSurface =
        [&](HWND hwnd, NativeSurface *surface) -> bool
        {
            if ( !surface || !hwnd || !::IsWindowVisible(hwnd) )
                return false;

            RECT bounds = {};
            if ( !::GetWindowRect(hwnd, &bounds) ||
                 bounds.right <= bounds.left ||
                 bounds.bottom <= bounds.top )
            {
                return false;
            }

            // Child HWNDs are clipped by every ancestor client area. Apply
            // this first so a native surface outside a scrolled/paged
            // viewport cannot create a false impossible-overlap diagnostic.
            for ( HWND ancestor = ::GetParent(hwnd);
                  ancestor;
                  ancestor = ::GetParent(ancestor) )
            {
                RECT client = {};
                if ( !::GetClientRect(ancestor, &client) )
                    return false;
                ::MapWindowPoints(
                    ancestor, nullptr,
                    reinterpret_cast<POINT *>(&client), 2);
                if ( client.left > client.right )
                    std::swap(client.left, client.right);
                if ( client.top > client.bottom )
                    std::swap(client.top, client.bottom);

                RECT clipped = {};
                if ( !::IntersectRect(&clipped, &bounds, &client) )
                    return false;
                bounds = clipped;

                if ( ancestor == hwndTLW )
                    break;
            }

            HRGN visible = ::CreateRectRgn(
                bounds.left, bounds.top, bounds.right, bounds.bottom);
            if ( !visible )
                return false;
            std::shared_ptr<void> visibleOwner = makeRegion(visible);

            // Respect explicit SetWindowRgn shapes on the window and every
            // ancestor. Holes and rounded/non-rectangular windows otherwise
            // turn bounding-box contact into a false positive.
            for ( HWND shaped = hwnd;
                  shaped;
                  shaped = ::GetParent(shaped) )
            {
                HRGN local = ::CreateRectRgn(0, 0, 0, 0);
                if ( !local )
                    return false;
                std::shared_ptr<void> localOwner = makeRegion(local);

                const int type = ::GetWindowRgn(shaped, local);
                if ( type != ERROR )
                {
                    RECT windowRect = {};
                    if ( !::GetWindowRect(shaped, &windowRect) ||
                         ::OffsetRgn(local,
                                     windowRect.left,
                                     windowRect.top) == ERROR ||
                         ::CombineRgn(visible, visible, local,
                                      RGN_AND) == ERROR )
                    {
                        return false;
                    }
                }

                if ( shaped == hwndTLW )
                    break;
            }

            RECT exactBounds = {};
            if ( ::GetRgnBox(visible, &exactBounds) == ERROR ||
                 exactBounds.right <= exactBounds.left ||
                 exactBounds.bottom <= exactBounds.top )
            {
                return false;
            }

            surface->bounds = exactBounds;
            surface->region = std::move(visibleOwner);
            return true;
        };

    const auto appendSummary =
        [](SurfaceSummary& destination, const SurfaceSummary& source)
        {
            destination.xaml.insert(destination.xaml.end(),
                                    source.xaml.begin(), source.xaml.end());
            destination.native.insert(destination.native.end(),
                                      source.native.begin(),
                                      source.native.end());
        };

    const auto reportImpossible =
        [](HWND parent, const char *kind)
        {
            ++gs_impossibleZOrderOverlaps;
            wxWinUIDebugLog(
                "wxWinUI: impossible native/XAML z-order overlap "
                "(%s, parent=%p, count=%u); retaining one shared bridge",
                kind, static_cast<void *>(parent),
                gs_impossibleZOrderOverlaps);
        };

    std::vector<std::size_t> order;
    order.reserve(slots.size());
    std::set<std::size_t> orderedSlots;
    std::set<HWND> visitedHwnds;

    const auto appendSlot =
        [&](HWND hwnd)
        {
            const auto found = slotByHwnd.find(hwnd);
            if ( found != slotByHwnd.end() &&
                 orderedSlots.insert(found->second).second )
            {
                order.push_back(found->second);
            }
        };

    const bool tlwVisible =
        hwndTLW && ::IsWindowVisible(hwndTLW) && !::IsIconic(hwndTLW);

    std::function<SurfaceSummary (HWND)> walkHwndTree =
        [&](HWND parent) -> SurfaceSummary
        {
            SurfaceSummary combined;
            std::vector<HWND> topToBottom;
            for ( HWND child = ::GetWindow(parent, GW_CHILD);
                  child;
                  child = ::GetWindow(child, GW_HWNDNEXT) )
            {
                topToBottom.push_back(child);
            }

            // GetWindow(GW_CHILD/GW_HWNDNEXT) enumerates top-to-bottom.
            // Canvas.ZIndex grows in the opposite direction.
            std::vector<RECT> lowerXaml;
            bool siblingLimitReported = false;
            for ( auto it = topToBottom.rbegin();
                  it != topToBottom.rend();
                  ++it )
            {
                const HWND child = *it;
                if ( child == m_bridge ||
                     !visitedHwnds.insert(child).second )
                {
                    continue;
                }

                appendSlot(child);

                SurfaceSummary current;
                const auto ownSlot = slotByHwnd.find(child);
                const bool ownXaml = ownSlot != slotByHwnd.end();
                bool ownXamlVisible = false;
                if ( ownXaml )
                {
                    const SlotOrderSnapshot& snapshot =
                        slots[ownSlot->second];
                    if ( snapshot.visible )
                    {
                        current.xaml.push_back(
                            snapshot.visibleRectScreen);
                        ownXamlVisible = true;
                    }
                }
                else if ( wxHwnds.count(child) != 0 && tlwVisible )
                {
                    NativeSurface native;
                    if ( makeNativeSurface(child, &native) )
                        current.native.push_back(std::move(native));
                }

                SurfaceSummary descendants = walkHwndTree(child);

                // A native logical child is painted above its parent. If the
                // parent itself is XAML-backed and they overlap, the single
                // topmost bridge cannot honour that parent/child request.
                if ( ownXamlVisible &&
                     overlaps(current.xaml, descendants.native) )
                {
                    reportImpossible(child, "parent-child");
                }
                appendSummary(current, descendants);

                // `current` is above everything accumulated in lowerXaml.
                // A native surface in it therefore asks to sit above an
                // overlapping XAML surface, which a topmost shared bridge
                // cannot represent. Report once for this sibling group.
                if ( !siblingLimitReported &&
                     overlaps(lowerXaml, current.native) )
                {
                    reportImpossible(parent, "siblings");
                    siblingLimitReported = true;
                }

                lowerXaml.insert(lowerXaml.end(),
                                 current.xaml.begin(), current.xaml.end());
                appendSummary(combined, current);
            }
            return combined;
        };

    // A frame-class dialog may itself own the first/bottom slot.
    if ( hwndTLW )
        appendSlot(hwndTLW);

    SurfaceSummary tlwDescendants;
    if ( hwndTLW )
        tlwDescendants = walkHwndTree(hwndTLW);

    // Unlike ordinary slots, the TLW is the root passed to walkHwndTree(),
    // not one of the visited children. Its own XAML surface is therefore
    // ordered correctly above but must be joined explicitly to the overlap
    // analysis: a native child is painted above its parent HWND yet cannot
    // rise above the single topmost island bridge.
    const auto tlwSlot = slotByHwnd.find(hwndTLW);
    if ( tlwSlot != slotByHwnd.end() )
    {
        const SlotOrderSnapshot& snapshot = slots[tlwSlot->second];
        if ( snapshot.visible )
        {
            const std::vector<RECT> tlwXaml
            {
                snapshot.visibleRectScreen
            };
            if ( overlaps(tlwXaml, tlwDescendants.native) )
                reportImpossible(hwndTLW, "parent-child");
        }
    }

    // A slot can transiently remain in its old host during a failed/cascaded
    // cross-TLW migration. Do not drop it from the XAML ordering pass merely
    // because its native HWND no longer appears under this TLW; migration
    // immediately after this pass will reconcile it.
    for ( std::size_t i = 0; i < slots.size(); ++i )
    {
        if ( orderedSlots.insert(i).second )
        {
            order.push_back(i);
            wxWinUIDebugLog(
                "wxWinUI: slot HWND %p absent from TLW z-order snapshot",
                static_cast<void *>(slots[i].hwnd));
        }
    }

    int z = 0;
    for ( const std::size_t index : order )
    {
        if ( m_shuttingDown ||
             m_structureGeneration != expectedGeneration )
        {
            return false;
        }

        const SlotOrderSnapshot& snapshot = slots[index];
        const int desiredZ = ++z;
        wxWindow * const window =
            snapshot.state ? snapshot.state->GetWindow() : nullptr;
        wxWinUISlot * const current =
            window ? FindSlot(window) : nullptr;
        if ( !window || !current ||
             snapshot.state->GetHost() != this ||
             current->m_lifetime != snapshot.state ||
             current->m_container != snapshot.container ||
             static_cast<HWND>(window->GetHWND()) != snapshot.hwnd ||
             wxWinUIMSWGetHwndGeneration(
                 window, window->GetHWND()) != snapshot.hwndGeneration )
        {
            continue;
        }

        try
        {
            winrt::Microsoft::UI::Xaml::Controls::Canvas::SetZIndex(
                snapshot.container, desiredZ);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("slot z-order sync", e);
        }

        // Deterministic seam at the same post-setter boundary. Consume it
        // first so its nested flush cannot recursively invoke itself.
        std::function<void ()> zOrderHook =
            std::move(gs_testZOrderSetHook);
        if ( zOrderHook )
            zOrderHook();

        // A bool is insufficient here: a nested FlushSync() can consume and
        // clear it before returning. The monotone epoch permanently records
        // that this outer snapshot became stale.
        if ( m_shuttingDown ||
             m_structureGeneration != expectedGeneration )
        {
            return false;
        }
    }

    return !m_shuttingDown &&
           m_structureGeneration == expectedGeneration;
}

// ============================================================================
// bridge plumbing
// ============================================================================

void wxWinUITopLevelHost::EnsureInnerSubclass()
{
    if ( !m_bridge )
        return;

    const HWND currentInner = ::GetWindow(m_bridge, GW_CHILD);
    if ( currentInner == m_inner && m_innerSubclassContext )
        return;

    if ( currentInner != m_innerSubclassRetryWindow )
    {
        m_innerSubclassRetryWindow = currentInner;
        m_innerSubclassRetryCount = 0;
        m_innerSubclassFailureLogged = false;
    }

    // The island may recreate its InputSiteWindow while the bridge remains.
    // Retire the old callback before tracking the replacement.
    if ( m_innerSubclassContext )
    {
        RetireIslandCancelTarget(m_inner);
        wxWinUIRetireSubclass(m_inner, BridgeSubclassProc,
                              wxWINUI_TLW_SUBCLASS_INNER,
                              m_innerSubclassContext, "inner-island");
    }
    m_inner = nullptr;

    if ( currentInner && ::IsWindow(currentInner) &&
         ::GetWindowThreadProcessId(currentInner, nullptr) ==
            ::GetCurrentThreadId() )
    {
        auto *innerContext = new wxWinUISubclassContext(m_hostLifetime);
        if ( !::SetWindowSubclass(currentInner, BridgeSubclassProc,
                                  wxWINUI_TLW_SUBCLASS_INNER,
                                  reinterpret_cast<DWORD_PTR>(innerContext)) )
        {
            delete innerContext;
            ++m_innerSubclassRetryCount;
            if ( !m_innerSubclassFailureLogged ||
                 m_innerSubclassRetryCount == 3 )
            {
                wxLogWarning(
                    "wxWinUI: inner-island subclass installation failed "
                    "(the API exposes no extended error), attempt %u",
                    m_innerSubclassRetryCount);
                m_innerSubclassFailureLogged = true;
            }

            // Retry automatically only a bounded number of times. Later
            // real dirty flushes may still recover without a self-posting
            // failure loop.
            if ( m_innerSubclassRetryCount < 3 )
                ScheduleFlush();
            RetryDeferredIslandCancelMode();
            return;
        }
        m_inner = currentInner;
        m_innerSubclassContext = innerContext;
        m_innerSubclassRetryCount = 0;
        m_innerSubclassFailureLogged = false;

        // Null out the CLASS cursor of both island windows.  With a class
        // cursor installed, USER32 re-applies it directly (no WM_SETCURSOR
        // involved) whenever any window repositions under a resting pointer
        // -- our coalesced slot sync, a sizer relayout... -- wiping the
        // per-element cursor the island installed (the I-beam over a
        // TextBox held only while the mouse was moving).  A null class
        // cursor means "the application manages the cursor": the island
        // sets it on real pointer activity and nothing resets it in
        // between.
        ::SetClassLongPtrW(m_inner, GCLP_HCURSOR, 0);
        ::SetClassLongPtrW(m_bridge, GCLP_HCURSOR, 0);
    }
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::EnsureBridgeOnTop()
{
    if ( !m_bridge || !m_tlw )
        return;

    // wx children created after the bridge stack above it; re-pin (cheap,
    // and only when actually needed).
    const HWND top = ::GetWindow(GetHwndOf(m_tlw), GW_CHILD);
    if ( top == m_bridge )
        return;

    // A transient island (popup menu, overlay dialog, rich tooltip) pins its
    // own bridge on top for its lifetime: never leapfrog those.
    if ( top )
    {
        wchar_t cls[96] = L"";
        ::GetClassNameW(top, cls, WXSIZEOF(cls));
        if ( wxString(cls).Contains(wxS("DesktopChildSiteBridge")) )
            return;
    }

    if ( wxWinUIInputLogEnabled() )
    {
        wchar_t cls[64] = L"";
        if ( top )
            ::GetClassNameW(top, cls, WXSIZEOF(cls));
        wxWinUIInputLog("bridge REPIN over %p '%S'",
                        static_cast<void *>(top), cls);
    }

    if ( !::SetWindowPos(m_bridge, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) )
    {
        const DWORD error = ::GetLastError();
        if ( !m_bridgePinFailureLogged )
        {
            wxLogWarning("wxWinUI: bridge re-pinning failed (error %lu)",
                         error);
            m_bridgePinFailureLogged = true;
        }
    }
    else
        m_bridgePinFailureLogged = false;
}

void wxWinUITopLevelHost::RebuildBridgeRegion()
{
    OperationGuard operation(this);

    if ( !m_bridge || !m_tlw )
        return;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const HWND bridge = m_bridge;
    const HWND hwndTLW = GetHwndOf(m_tlw);
    if ( !hwndTLW || !::IsWindow(bridge) )
        return;

    RECT rcClient;
    if ( !::GetClientRect(bridge, &rcClient) )
        return;
    const bool rtl =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;

    // Collect the scrollbar bands first, so an unchanged set skips the
    // (redraw-forcing) SetWindowRgn call entirely.
    std::vector<RECT> cuts;
    cuts.push_back(rcClient);   // client size participates in the signature

    // The exact physical rect comes from GetScrollBarInfo: correct wherever
    // the bar actually is (left side under RTL layouts), correct under any
    // per-monitor DPI, and absent when the bar is currently hidden -- the
    // style bit alone says none of that. Cuts are collected in physical-LTR
    // coordinates. SetWindowRgn interprets an RTL target from its upper-right
    // corner, so they are mirrored only when building the native region
    // below; the physical signature remains stable and intelligible.
    const RECT rcCS = GetClientScreenRect();
    const auto addBarCut = [&](HWND hwnd, LONG objId)
    {
        SCROLLBARINFO sbi = { sizeof(SCROLLBARINFO) };
        if ( !::GetScrollBarInfo(hwnd, objId, &sbi) )
            return;
        if ( sbi.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN) )
            return;

        const RECT cut = { sbi.rcScrollBar.left - rcCS.left,
                           sbi.rcScrollBar.top - rcCS.top,
                           sbi.rcScrollBar.right - rcCS.left,
                           sbi.rcScrollBar.bottom - rcCS.top };
        cuts.push_back(cut);
    };

    std::function<void (wxWindow *)> walk = [&](wxWindow *win)
    {
        for ( wxWindow *child : win->GetChildren() )
        {
            const HWND hwnd = GetHwndOf(child);
            if ( hwnd && ::IsWindowVisible(hwnd) )
            {
                const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
                if ( style & WS_VSCROLL )
                    addBarCut(hwnd, OBJID_VSCROLL);
                if ( style & WS_HSCROLL )
                    addBarCut(hwnd, OBJID_HSCROLL);
            }

            walk(child);
        }
    };
    walk(m_tlw);

    if ( m_regionValid && rtl == m_regionRTL &&
         cuts.size() == m_regionCuts.size() &&
             memcmp(cuts.data(), m_regionCuts.data(),
                    cuts.size() * sizeof(RECT)) == 0 )
        return;

    wxWinUIOwnedRegion region(
        ::CreateRectRgn(0, 0, rcClient.right, rcClient.bottom));
    if ( !region )
    {
        if ( !m_regionFailureLogged )
        {
            wxLogWarning("wxWinUI: failed to allocate the bridge window "
                         "region");
            m_regionFailureLogged = true;
        }
        return;
    }

    for ( size_t i = 1; i < cuts.size(); ++i )
    {
        RECT nativeCut = cuts[i];
        if ( rtl )
        {
            const LONG oldLeft = nativeCut.left;
            nativeCut.left = rcClient.right - nativeCut.right;
            nativeCut.right = rcClient.right - oldLeft;
        }

        wxWinUIOwnedRegion cut(
            ::CreateRectRgn(nativeCut.left, nativeCut.top,
                            nativeCut.right, nativeCut.bottom));
        if ( !cut )
        {
            if ( !m_regionFailureLogged )
            {
                wxLogWarning("wxWinUI: failed to allocate a bridge cut-out "
                             "region");
                m_regionFailureLogged = true;
            }
            return;
        }

        if ( ::CombineRgn(region.Get(), region.Get(), cut.Get(), RGN_DIFF)
                == ERROR )
        {
            if ( !m_regionFailureLogged )
            {
                wxLogWarning("wxWinUI: failed to combine a bridge cut-out "
                             "region");
                m_regionFailureLogged = true;
            }
            return;
        }
    }

    // USER32 owns the region after a successful call only. Commit the
    // signature at that same point: until then m_regionCuts continues to
    // describe the region that is actually installed, and the next flush
    // retries this transaction.
    if ( !::SetWindowRgn(bridge, region.Get(), TRUE) )
    {
        if ( !m_regionFailureLogged )
        {
            wxLogWarning("wxWinUI: SetWindowRgn failed");
            m_regionFailureLogged = true;
        }
        return;
    }

    // USER32 took ownership at the successful return, even if one of the
    // synchronous window-position callbacks destroyed or replaced the host.
    region.Release();

    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_bridge != bridge || !::IsWindow(bridge) )
    {
        return;
    }

    const bool currentRTL =
        (::GetWindowLongPtr(bridge, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
    if ( currentRTL != rtl )
    {
        // The installed region used the old coordinate convention. Keep the
        // cache invalid and schedule one bounded coalesced correction.
        m_regionValid = false;
        ScheduleFlush();
        return;
    }

    m_regionCuts = std::move(cuts);
    m_regionRTL = rtl;
    m_regionValid = true;
    m_regionFailureLogged = false;
}

LRESULT CALLBACK
wxWinUITopLevelHost::BridgeSubclassProc(HWND hwnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam,
                                        UINT_PTR subclassId, DWORD_PTR refData)
{
    wxWinUISubclassContext * const context =
        reinterpret_cast<wxWinUISubclassContext *>(refData);
    const std::shared_ptr<wxWinUIHostLifetime> hostState =
        context && context->active ? context->host.lock() : nullptr;
    wxWinUITopLevelHost * const host =
        hostState ? hostState->GetHost() : nullptr;

    const UINT resizeMessage = wxWinUINativeResizeMessage();
    if ( resizeMessage && msg == resizeMessage )
    {
        if ( host )
        {
            const std::uint64_t ticket = static_cast<std::uint32_t>(wParam) |
                (static_cast<std::uint64_t>(
                    static_cast<std::uint32_t>(lParam)) << 32);
            host->DispatchNativeResize(ticket);
        }
        return 0;
    }

    if ( msg == WM_ERASEBKGND )
    {
        // The bridge covers the entire client area of its top-level window,
        // which is WS_CLIPCHILDREN, so the wx window below never paints the
        // pixels the XAML content leaves untouched -- and the XAML root is
        // deliberately transparent. Without this, those pixels keep the
        // initial (white) content of the redirection surface, which is what
        // shows through around the controls of any window whose children do
        // not cover it completely.
        //
        // Erase exactly like the wx windows below: black under a DWM
        // material, so that it is substituted by Mica, and the window's own
        // background colour otherwise.
        if ( wxWinUIEraseIslandBackground(hwnd, reinterpret_cast<HDC>(wParam)) )
            return TRUE;
    }

    const UINT internalCancelMessage = wxWinUIInternalCancelMessage();
    if ( internalCancelMessage && msg == internalCancelMessage )
    {
        const std::uint32_t generation =
            static_cast<std::uint32_t>(wParam);
        if ( !host )
            return 0;

        host->AcknowledgeIslandCancelMessage(generation);
        if ( !host->CanPostIslandCancelMode(generation) )
        {
            host->PostIslandCancelMode(generation);
            return 0;
        }

        return ::DefSubclassProc(hwnd, WM_CANCELMODE, 0, 0);
    }


#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    const UINT oleContinuationMessage =
        wxMSWOleGetExternalShellContinuationMessage();
    if ( host && (!oleContinuationMessage || msg != oleContinuationMessage) )
    {
        // A failed PostMessage (normally only a saturated UI queue) retains
        // its exact ledger state. Ordinary bridge activity retries once,
        // asynchronously, instead of spinning or stranding the host.
        (void)wxMSWOleRetryFailedExternalShellContinuation(
            reinterpret_cast<WXHWND>(hwnd));
    }
    if ( oleContinuationMessage && msg == oleContinuationMessage )
    {
        const std::uint64_t pendingId =
            static_cast<std::uint32_t>(wParam) |
            (static_cast<std::uint64_t>(
                 static_cast<std::uint32_t>(lParam)) << 32);
        if ( wxMSWOleIsFixedShellCallInProgress() )
        {
            // This queued wake was pumped from a later fixed Register/Revoke
            // boundary. Put the exact id back behind that operation's outer
            // tail instead of re-entering broker adoption on its stack.
            (void)wxMSWOleDeferExternalShellContinuation(
                pendingId, reinterpret_cast<WXHWND>(hwnd));
            return 0;
        }
        if ( host && wxMSWOleClaimExternalShellContinuation(
                         pendingId, reinterpret_cast<WXHWND>(hwnd)) )
        {
            OperationGuard operation(host);
            host->ResumeDeferredInitialization(pendingId);
        }
        return 0;
    }
#endif

    switch ( msg )
    {
        // Note: WM_NCHITTEST is deliberately NOT answered with HTTRANSPARENT
        // any more.  The island is the single input receiver (its root is
        // hit-testable everywhere) and the root router forwards real native
        // messages to the wx windows below: hit-test pass-through through
        // the pointer pipeline proved non-deterministic.

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        case WM_SHOWWINDOW:
        case WM_WINDOWPOSCHANGED:
        case WM_SIZE:
            if ( host && subclassId == wxWINUI_TLW_SUBCLASS_BRIDGE &&
                 hwnd == host->m_bridge &&
                 (host->m_dropBrokerAwaitingPresentation ||
                  host->m_dropBrokerAcquireInProgress ||
                  host->m_initializationDeferred) )
            {
                // Presentation readiness is event-driven. A first flush can
                // legitimately observe the bridge hidden or with an empty
                // rect while DesktopWindowXamlSource is still publishing it;
                // the next real bridge show/geometry transition schedules
                // one coalesced retry without a CallAfter polling loop.
                host->InvalidateStructure();
                host->MarkAllDirty();
            }
            break;
#endif

        case WM_SETFOCUS:
            if ( host )
            {
                const HWND previous = reinterpret_cast<HWND>(wParam);
                const bool previousInsideIsland =
                    previous &&
                    (previous == host->m_bridge ||
                     (host->m_bridge &&
                      ::IsChild(host->m_bridge, previous)));
                const unsigned long long previousGeneration =
                    previous && !previousInsideIsland
                        ? wxWinUIMSWGetNativeHwndGeneration(
                              reinterpret_cast<WXHWND>(previous))
                        : 0;
                // The bridge and its InputSite can each receive WM_SETFOCUS
                // during one external -> XAML transfer. Preserve the original
                // external authority across those intra-island hops; it is
                // consumed only by a confirmed GotFocus, an exit from the
                // island, or shutdown.
                if ( !previousInsideIsland )
                {
                    host->m_nativeFocusEntryAuthority =
                        previousGeneration
                            ? reinterpret_cast<WXHWND>(previous)
                            : nullptr;
                    host->m_nativeFocusEntryAuthorityGeneration =
                        previousGeneration;
                    host->m_nativeFocusEntryTarget.reset();
                    host->m_nativeFocusEntryEpoch = 0;
                }

                // A departure latch is meaningful only until its native
                // destination acquires focus. Re-entering this island by any
                // other route supersedes it.
                host->m_nativeFocusDepartureSource.reset();
                host->m_nativeFocusDepartureDestination = nullptr;
                host->m_nativeFocusDepartureDestinationGeneration = 0;
            }
            break;

        case WM_KILLFOCUS:
        {
            if ( !host )
                break;

            OperationGuard operation(host);
            const HWND destination = reinterpret_cast<HWND>(wParam);
            const bool destinationInsideIsland =
                destination &&
                (destination == host->m_bridge ||
                 (host->m_bridge &&
                  ::IsChild(host->m_bridge, destination)));
            const bool destinationOutsideIsland =
                destination && !destinationInsideIsland;
            const unsigned long long destinationGeneration =
                destinationOutsideIsland
                    ? wxWinUIMSWGetNativeHwndGeneration(
                          reinterpret_cast<WXHWND>(destination))
                    : 0;

            wxWinUISlot * const sourceSlot =
                host->m_focusOwner
                    ? host->FindSlot(host->m_focusOwner)
                    : nullptr;
            const std::shared_ptr<wxWinUISlotLifetime> sourceState =
                sourceSlot ? sourceSlot->m_lifetime : nullptr;
            const wxWeakRef<wxWindow> lossDestination(
                destinationOutsideIsland
                    ? wxGetWindowFromHWND(
                          reinterpret_cast<WXHWND>(destination))
                    : nullptr);

            if ( sourceState && destinationGeneration )
            {
                host->m_nativeFocusDepartureSource = sourceState;
                host->m_nativeFocusDepartureDestination =
                    reinterpret_cast<WXHWND>(destination);
                host->m_nativeFocusDepartureDestinationGeneration =
                    destinationGeneration;
            }
            else
            {
                // A failed XAML focus attempt may pre-arm the exact target
                // before restoring its old HWND even though no logical owner
                // was published. Preserve only that validated destination.
                const bool preservePrearmed =
                    destinationGeneration &&
                    static_cast<HWND>(
                        host->m_nativeFocusDepartureDestination) ==
                        destination &&
                    host->m_nativeFocusDepartureDestinationGeneration ==
                        destinationGeneration &&
                    !host->m_nativeFocusDepartureSource.expired();
                if ( !preservePrearmed )
                {
                    host->m_nativeFocusDepartureSource.reset();
                    host->m_nativeFocusDepartureDestination = nullptr;
                    host->m_nativeFocusDepartureDestinationGeneration = 0;
                }
            }

            if ( !destinationInsideIsland )
            {
                host->m_nativeFocusEntryAuthority = nullptr;
                host->m_nativeFocusEntryAuthorityGeneration = 0;
                host->m_nativeFocusEntryTarget.reset();
                host->m_nativeFocusEntryEpoch = 0;
            }

            // Let the framework complete its native focus bookkeeping first.
            // It can synchronously raise the ordinary XAML LostFocus, which
            // must win over this fallback and prevents duplicate wx losses.
            const LRESULT result =
                ::DefSubclassProc(hwnd, msg, wParam, lParam);

            wxWinUITopLevelHost * const liveHost =
                hostState ? hostState->GetHost() : nullptr;
            if ( liveHost != host || host->m_shuttingDown ||
                 destinationInsideIsland || !sourceState ||
                 host->HasNativeFocusAuthority() )
            {
                return result;
            }

            wxWindow * const source = sourceState->GetWindow();
            wxWinUISlot * const current =
                source ? host->FindSlot(source) : nullptr;
            if ( !source || !current ||
                 current->m_lifetime != sourceState ||
                 host->m_focusOwner != source )
            {
                return result;
            }

            // Cross-host migration deliberately parks native focus on the
            // control's shell while its XAML peer moves between roots.
            // DefSubclassProc() has already given the ordinary XAML
            // LostFocus path a chance to suppress this exact ticketed edge;
            // apply the same transaction verdict to the bridge fallback.
            // Otherwise this fallback publishes a spurious KILL_FOCUS,
            // invalidates the ticket generation and makes the destination
            // continuation fail before it can acquire XAML focus.
            if ( host->IsSuppressedMigrationLoss(sourceState) )
                return result;

            // XAML LostFocus is not guaranteed if its focused element was
            // detached just before native focus left. Retire the still-live
            // logical owner only after the normal framework path had a chance
            // to do so; a later routed notification is then a no-op.
            host->InvalidateFocusIntent();
            host->TransitionFocus(
                nullptr, source, false, lossDestination.get());
            return result;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            if ( host && wxWinUIInputLogEnabled() )
            {
                POINT pt = { 0, 0 };
                ::GetCursorPos(&pt);
                wxWinUIInputLog("bridge raw mouse msg=0x%04x pt=(%ld,%ld)",
                                msg, pt.x, pt.y);
            }
            break;

        case WM_SETCURSOR:
        {
            // A window repositioned under a resting pointer (a sizer
            // relayout, a child move, the coalesced slot sync...) makes the
            // system re-send WM_SETCURSOR without any pointer motion.  The
            // island answers those with its default arrow, wiping the
            // per-element cursor (the I-beam over a TextBox...) until the
            // next real move re-applies it -- visibly, hover cursors only
            // held while the mouse was moving.  Swallow them: returning
            // TRUE keeps whatever cursor is currently on screen.  (The
            // position comparison is tolerant: the island reports pointer
            // positions in DIPs and the round-trip can be off by a pixel.)
            POINT pt;
            const bool still =
                host && ::GetCursorPos(&pt) &&
                    std::abs(pt.x - host->m_lastPointerScreen.x) <= 2 &&
                        std::abs(pt.y - host->m_lastPointerScreen.y) <= 2;

            if ( host && wxWinUIInputLogEnabled() )
            {
                POINT ptLog = { -1, -1 };
                ::GetCursorPos(&ptLog);
                wxWinUIInputLog("bridge WM_SETCURSOR cur=(%ld,%ld) "
                                "last=(%ld,%ld) ht=%d => %s",
                                ptLog.x, ptLog.y,
                                host->m_lastPointerScreen.x,
                                host->m_lastPointerScreen.y,
                                (int)LOWORD(lParam),
                                still ? "SWALLOW" : "pass");
            }

            if ( still )
                return TRUE;
            break;
        }

#if wxUSE_DRAG_AND_DROP
        case WM_DROPFILES:
            if ( host )
            {
                host->RouteDropFiles(wParam);
            }
            else
            {
                // The bridge retained the shell allocation but its host was
                // already invalidated. Consume it without dispatching.
                ::DragFinish(reinterpret_cast<HDROP>(wParam));
            }
            return 0;
#endif

        case WM_CAPTURECHANGED:
            if ( host )
            {
                host->NotifyNativeCaptureMutation();
                host->OnRootNativeInterrupted(true);
            }
            break;

        case WM_CANCELMODE:
            if ( host )
                host->OnRootNativeInterrupted(false);
            break;

        case WM_NCDESTROY:
        {
            if ( context )
                context->active = false;

            if ( host )
            {
                if ( subclassId == wxWINUI_TLW_SUBCLASS_INNER &&
                     host->m_inner == hwnd )
                {
                    host->RetireIslandCancelTarget(hwnd);
                    host->m_inner = nullptr;
                    host->m_innerSubclassContext = nullptr;
                    host->RetryDeferredIslandCancelMode();
                }
                else if ( subclassId == wxWINUI_TLW_SUBCLASS_BRIDGE &&
                          host->m_bridge == hwnd )
                {
                    host->RetireIslandCancelTarget(hwnd);
                    host->m_bridge = nullptr;
                    host->m_bridgeSubclassContext = nullptr;
                    host->RetryDeferredIslandCancelMode();
                }
            }

            if ( !::RemoveWindowSubclass(hwnd, BridgeSubclassProc,
                                         subclassId) )
            {
                wxLogWarning("wxWinUI: failed to remove a subclass during "
                             "WM_NCDESTROY (error %lu)", ::GetLastError());
            }

            const LRESULT result =
                ::DefSubclassProc(hwnd, msg, wParam, lParam);
            delete context;
            return result;
        }
    }

    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// hit test
// ============================================================================

#if wxUSE_DRAG_AND_DROP

wxWinUITopLevelHost::DropSurfaceResolution
wxWinUITopLevelHost::ResolveDropSurface(const POINT& screen, wxWindow **seed)
{
    if ( seed )
        *seed = nullptr;
    if ( !seed || !m_root || m_shuttingDown )
        return DropSurfaceResolution::Unstable;

    const RECT clientScreen = GetClientScreenRect();
    const POINT client =
    {
        screen.x - clientScreen.left,
        screen.y - clientScreen.top
    };
    const RECT local =
    {
        0, 0,
        clientScreen.right - clientScreen.left,
        clientScreen.bottom - clientScreen.top
    };
    if ( !::PtInRect(&local, client) )
        return DropSurfaceResolution::Native;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const auto xamlRoot = GetXamlRoot();
        if ( !xamlRoot )
            return DropSurfaceResolution::Unstable;

        // Popups and flyouts live in a separate visual tree, so the ordinary
        // Content hit-test below cannot see them. Treat every open popup as an
        // opaque modal surface: routing a drop to content hidden below it is
        // always worse than rejecting the drop conservatively.
        if ( VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot).Size() )
            return DropSurfaceResolution::Blocking;

        const double scale = GetScale();
        const winrt::Windows::Foundation::Point dip
        {
            static_cast<float>(client.x / scale),
            static_cast<float>(client.y / scale)
        };

        auto scope = xamlRoot.Content();
        if ( !scope )
            scope = m_root;

        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(dip, scope);
        for ( const auto& hit : hits )
        {
            DependencyObject current = hit;
            if ( current == m_root )
                continue;

            while ( current )
            {
                for ( const auto& entry : m_slots )
                {
                    wxWinUISlot* const slot = entry.second;
                    if ( slot && slot->m_container &&
                         current == slot->m_container )
                    {
                        wxWindow* const window = slot->m_window;
                        if ( !window || window->IsBeingDeleted() ||
                             !slot->m_visible ||
                             !slot->m_visibleRectPx.Contains(
                                 client.x, client.y) )
                        {
                            return DropSurfaceResolution::Unstable;
                        }

                        *seed = window;
                        return DropSurfaceResolution::Slot;
                    }
                }

                current = VisualTreeHelper::GetParent(current);
            }

            // Free XAML (popup, menu, dialog smoke layer...) is opaque. A
            // drop must never tunnel through it to an unrelated HWND.
            return DropSurfaceResolution::Blocking;
        }

        return DropSurfaceResolution::Native;
    }
    catch ( const winrt::hresult_error& )
    {
        return DropSurfaceResolution::Unstable;
    }
}

void wxWinUITopLevelHost::UpdateBridgeFileAcceptance()
{
    if ( !m_bridge || !m_tlw || m_shuttingDown )
        return;

    bool accepts = false;
    std::vector<wxWeakRef<wxWindow>> pending;
    pending.emplace_back(m_tlw);
    while ( !pending.empty() && !accepts )
    {
        const wxWeakRef<wxWindow> lifetime = pending.back();
        pending.pop_back();
        wxWindow* const window = lifetime.get();
        if ( !window || window->IsBeingDeleted() )
            continue;
        if ( wxGetTopLevelParent(window) != m_tlw )
            continue;

        const HWND hwnd = GetHwndOf(window);
        if ( hwnd && ::IsWindow(hwnd) &&
             (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_ACCEPTFILES) )
        {
            accepts = true;
            break;
        }

        for ( wxWindow* const child : window->GetChildren() )
        {
            if ( wxGetTopLevelParent(child) == m_tlw )
                pending.emplace_back(child);
        }
    }

    if ( accepts == m_bridgeAcceptsFiles )
        return;

    ::DragAcceptFiles(m_bridge, accepts ? TRUE : FALSE);
    m_bridgeAcceptsFiles = accepts;
}

bool wxWinUITopLevelHost::RouteDropFiles(WXWPARAM drop)
{
    const HDROP handle = reinterpret_cast<HDROP>(drop);
    if ( !handle )
        return false;

    class DropFilesGuard
    {
    public:
        explicit DropFilesGuard(HDROP value) : m_value(value) {}
        ~DropFilesGuard()
        {
            if ( m_value )
                ::DragFinish(m_value);
        }

        HDROP Release()
        {
            const HDROP value = m_value;
            m_value = nullptr;
            return value;
        }

    private:
        HDROP m_value;
    } finish(handle);

    const auto reject = []()
    {
        return true;
    };

    if ( m_shuttingDown || !m_tlw || !m_bridge )
        return reject();

    POINT screen = { 0, 0 };
    if ( ::DragQueryPoint(handle, &screen) )
    {
        ::SetLastError(ERROR_SUCCESS);
        const int mappedToScreen =
            ::MapWindowPoints(m_bridge, HWND_DESKTOP, &screen, 1);
        if ( mappedToScreen == 0 && ::GetLastError() != ERROR_SUCCESS )
            return reject();
    }
    // For a non-client drop DragQueryPoint() returns FALSE and has already
    // supplied screen coordinates. Do not map them a second time.

    wxWindow* seed = nullptr;
    const DropSurfaceResolution surface =
        ResolveDropSurface(screen, &seed);
    if ( surface == DropSurfaceResolution::Blocking ||
         surface == DropSurfaceResolution::Unstable )
    {
        return reject();
    }

    if ( surface == DropSurfaceResolution::Native )
    {
        wxWinUINativeHit hit;
        const wxWinUIHitResolution resolution = wxWinUIResolveNativeHit(
            m_tlw, screen,
            reinterpret_cast<WXHWND>(m_bridge),
            reinterpret_cast<WXHWND>(m_inner),
            &hit);
        if ( resolution != wxWinUIHitResolution::Hit )
            return reject();
        seed = hit.GetTarget().GetWindow();
    }

    wxWindow* receiver = nullptr;
    HWND receiverHwnd = nullptr;
    for ( wxWindow* window = seed; window; window = window->GetParent() )
    {
        if ( wxGetTopLevelParent(window) != m_tlw )
            return reject();

        const HWND hwnd = GetHwndOf(window);
        if ( hwnd && ::IsWindow(hwnd) &&
             window->IsShownOnScreen() && window->IsEnabled() &&
             (::GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_ACCEPTFILES) )
        {
            receiver = window;
            receiverHwnd = hwnd;
            break;
        }

        if ( window == m_tlw )
            break;
    }

    if ( !receiver )
        return reject();

    POINT client = screen;
    ::SetLastError(ERROR_SUCCESS);
    const int mappedToClient =
        ::MapWindowPoints(HWND_DESKTOP, receiverHwnd, &client, 1);
    if ( mappedToClient == 0 && ::GetLastError() != ERROR_SUCCESS )
        return reject();

    // wxMSWDispatchDropFiles consumes the HDROP exactly once.
    const wxPoint clientPoint(client.x, client.y);
    return wxMSWDispatchDropFiles(
        receiver,
        reinterpret_cast<WXWPARAM>(finish.Release()),
        &clientPoint);
}

#endif // wxUSE_DRAG_AND_DROP

wxWindow *wxWinUITopLevelHost::ResolveWindowAtScreenPoint(
    const wxPoint& ptScreen)
{
    OperationGuard operation(this);
    if ( !m_tlw || !m_root || !m_hostLifetime )
        return nullptr;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const wxWeakRef<wxWindow> tlwLifetime(m_tlw);
    const auto root = m_root;
    const unsigned long long structureGeneration = m_structureGeneration;
    const auto current =
        [this, &hostState, &tlwLifetime, &root, structureGeneration]()
        {
            return hostState && hostState->GetHost() == this &&
                   tlwLifetime.get() == m_tlw && !m_shuttingDown &&
                   m_root == root &&
                   m_structureGeneration == structureGeneration;
        };

    winrt::Windows::Foundation::Point rootDips{};
    if ( wxWinUIVisualCoordinates::ScreenPointToRoot(
             m_tlw,
             winrt::Windows::Foundation::Point{
                 static_cast<float>(ptScreen.x),
                 static_cast<float>(ptScreen.y)},
             &rootDips) != wxWinUICoordinateResult::Mapped ||
         !current() )
    {
        return nullptr;
    }

    const RECT clientScreen = GetClientScreenRect();
    if ( !current() )
        return nullptr;
    const wxPoint clientPoint(ptScreen.x - clientScreen.left,
                              ptScreen.y - clientScreen.top);
    const RECT local =
    {
        0, 0,
        clientScreen.right - clientScreen.left,
        clientScreen.bottom - clientScreen.top
    };
    const POINT clientNative = { clientPoint.x, clientPoint.y };
    if ( !::PtInRect(&local, clientNative) )
        return nullptr;

    // No WinRT getter is crossed while m_slots is being iterated. Keep only
    // invalidatable lifetime/container identities; OperationGuard keeps the
    // host object alive and current() rejects a structurally newer snapshot.
    struct SlotSnapshot final
    {
        std::shared_ptr<wxWinUISlotLifetime> lifetime;
        winrt::Microsoft::UI::Xaml::Controls::Grid container{ nullptr };
    };

    std::vector<SlotSnapshot> slotSnapshots;
    slotSnapshots.reserve(m_slots.size());
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && slot->m_lifetime && slot->m_container )
        {
            slotSnapshots.push_back(
                { slot->m_lifetime, slot->m_container });
        }
    }

    const auto resolveSlot =
        [this, &current, &clientPoint](const SlotSnapshot& snapshot)
            -> wxWindow *
        {
            if ( !current() || !snapshot.lifetime || !snapshot.container )
                return nullptr;

            wxWindow * const window = snapshot.lifetime->GetWindow();
            wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
            if ( !window || !slot ||
                 snapshot.lifetime->GetHost() != this ||
                 slot->m_lifetime != snapshot.lifetime ||
                 slot->m_container != snapshot.container ||
                 window->IsBeingDeleted() ||
                 !slot->m_visible ||
                 !slot->m_visibleRectPx.Contains(clientPoint) )
            {
                return nullptr;
            }
            return window;
        };

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const auto xamlRoot = root.XamlRoot();
        if ( !xamlRoot || !current() )
            return tlwLifetime.get();

        auto scope = xamlRoot.Content();
        if ( !scope )
            scope = root;
        if ( !current() )
            return nullptr;

        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(
                rootDips, scope, true);
        if ( !current() )
            return nullptr;

        struct SlotState final
        {
            std::size_t index = 0;
            int z = 0;
            bool hitTestVisible = true;
            bool valid = false;
        };

        // Read one candidate exclusively through its stable snapshot. Every
        // callback-bearing WinRT boundary is followed immediately by both the
        // host-generation check and exact lifetime/container revalidation.
        const auto readSlotState =
            [&current, &resolveSlot, &slotSnapshots](std::size_t index)
                -> SlotState
            {
                SlotState state;
                if ( index >= slotSnapshots.size() )
                    return state;

                const SlotSnapshot& snapshot = slotSnapshots[index];
                wxWindow *window = resolveSlot(snapshot);
                if ( !window )
                    return state;

                const bool hitTestVisible =
                    snapshot.container.IsHitTestVisible();
                if ( !current() )
                    return state;
                window = resolveSlot(snapshot);
                if ( !window )
                    return state;

                std::function<void (wxWindow *)> boundaryHook =
                    std::move(gs_testWindowAtPointSlotStateHook);
                if ( boundaryHook )
                {
                    boundaryHook(window);
                    if ( !current() )
                        return state;
                    window = resolveSlot(snapshot);
                    if ( !window )
                        return state;
                }

                const int z =
                    winrt::Microsoft::UI::Xaml::Controls::Canvas::
                        GetZIndex(snapshot.container);
                if ( !current() )
                    return state;
                if ( !resolveSlot(snapshot) )
                    return state;

                state.index = index;
                state.z = z;
                state.hitTestVisible = hitTestVisible;
                state.valid = true;
                return state;
            };

        SlotState topNonHitTestable;
        for ( std::size_t i = 0; i < slotSnapshots.size(); ++i )
        {
            const SlotState state = readSlotState(i);
            if ( !current() )
                return nullptr;
            if ( state.valid && !state.hitTestVisible &&
                 (!topNonHitTestable.valid || state.z > topNonHitTestable.z) )
            {
                topNonHitTestable = state;
            }
        }

        const auto refreshTopNonHitTestable =
            [&readSlotState, &topNonHitTestable]() -> SlotState
            {
                if ( !topNonHitTestable.valid )
                    return SlotState{};

                const SlotState state =
                    readSlotState(topNonHitTestable.index);
                return state.valid && !state.hitTestVisible
                    ? state
                    : SlotState{};
            };

        // FindElementsInHostCoordinates() is ordered front-to-back. WinUI may
        // omit a non-hit-testable slot even from the requested full stack, so
        // merge that candidate by root-Canvas ZIndex with the first returned
        // branch. Equal Z keeps the actual visual ordering authoritative.
        for ( const auto& hit : hits )
        {
            if ( !current() )
                return nullptr;
            if ( hit == root )
                continue;

            DependencyObject element = hit;
            DependencyObject rootBranch{ nullptr };
            while ( element )
            {
                if ( !current() )
                    return nullptr;

                for ( std::size_t i = 0; i < slotSnapshots.size(); ++i )
                {
                    if ( element == slotSnapshots[i].container )
                    {
                        const SlotState hitState = readSlotState(i);
                        if ( !current() )
                            return nullptr;
                        if ( !hitState.valid )
                            return tlwLifetime.get();

                        const SlotState disabled =
                            refreshTopNonHitTestable();
                        if ( !current() )
                            return nullptr;

                        if ( disabled.valid && disabled.z > hitState.z )
                        {
                            wxWindow * const window = resolveSlot(
                                slotSnapshots[disabled.index]);
                            return window ? window : tlwLifetime.get();
                        }

                        wxWindow * const window = resolveSlot(slotSnapshots[i]);
                        if ( !window )
                            return tlwLifetime.get();
                        return window;
                    }
                }

                if ( element == root )
                    break;
                rootBranch = element;
                element = VisualTreeHelper::GetParent(element);
                if ( !current() )
                    return nullptr;
            }

            // A free XAML branch is opaque only when it is at or above the
            // omitted disabled slot. A disabled wx child with a strictly
            // higher root-Canvas ZIndex still owns the public wx hit.
            const UIElement freeBranch = rootBranch.try_as<UIElement>();
            if ( !current() )
                return nullptr;
            if ( !freeBranch )
                return tlwLifetime.get();

            const int freeZ =
                winrt::Microsoft::UI::Xaml::Controls::Canvas::
                    GetZIndex(freeBranch);
            if ( !current() )
                return nullptr;

            DependencyObject freeParent =
                VisualTreeHelper::GetParent(rootBranch);
            if ( !current() )
                return nullptr;
            if ( freeParent != root )
                return tlwLifetime.get();

            const SlotState disabled = refreshTopNonHitTestable();
            if ( !current() )
                return nullptr;

            // The disabled-slot getters above are another callback boundary;
            // verify that the compared free branch is still rooted here.
            freeParent = VisualTreeHelper::GetParent(rootBranch);
            if ( !current() )
                return nullptr;
            if ( freeParent != root )
                return tlwLifetime.get();

            if ( disabled.valid && disabled.z > freeZ )
            {
                wxWindow * const window = resolveSlot(
                    slotSnapshots[disabled.index]);
                return window ? window : tlwLifetime.get();
            }
            return tlwLifetime.get();
        }

        // Defensive fallback for a template/runtime that still omits a
        // non-hit-testable carrier from the requested full stack.
        const SlotState disabled = refreshTopNonHitTestable();
        if ( !current() )
            return nullptr;
        if ( disabled.valid )
        {
            wxWindow * const window = resolveSlot(
                slotSnapshots[disabled.index]);
            return window ? window : tlwLifetime.get();
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // An unreadable visual tree is opaque: never tunnel through it to a
        // native sibling whose visibility is no longer provable.
        return current() ? tlwLifetime.get() : nullptr;
    }

    if ( !current() )
        return nullptr;

    wxWindow * const tlw = tlwLifetime.get();
    if ( !tlw )
        return nullptr;

    wxWinUINativeTarget nativeTarget;
    const POINT screen = { ptScreen.x, ptScreen.y };
    const wxWinUIHitResolution nativeResolution =
        wxWinUIResolveNativeWindowAtPoint(
            tlw, screen,
            reinterpret_cast<WXHWND>(m_bridge),
            reinterpret_cast<WXHWND>(m_inner),
            &nativeTarget);
    if ( nativeResolution == wxWinUIHitResolution::Hit &&
         nativeTarget.IsValid() )
    {
        return nativeTarget.GetWindow();
    }

    // Miss and unstable are both represented conservatively by the TLW: the
    // point is physically on this island, but no deeper logical identity can
    // be proved for the current generation.
    return tlwLifetime.get();
}

bool wxWinUITopLevelHost::HitTestContent(const POINT& ptScreen)
{
    OperationGuard operation(this);
    if ( !m_tlw || !m_root || !m_hostLifetime )
        return false;

    const std::shared_ptr<wxWinUIHostLifetime> hostState =
        m_hostLifetime;
    const auto root = m_root;
    const unsigned long long structureGeneration =
        m_structureGeneration;
    const auto current = [this, &hostState, &root, structureGeneration]()
    {
        return hostState && hostState->GetHost() == this &&
               !m_shuttingDown && m_root == root &&
               m_structureGeneration == structureGeneration;
    };

    winrt::Windows::Foundation::Point dip{};
    if ( wxWinUIVisualCoordinates::ScreenPointToRoot(
             m_tlw,
             winrt::Windows::Foundation::Point{
                 static_cast<float>(ptScreen.x),
                 static_cast<float>(ptScreen.y)},
             &dip) != wxWinUICoordinateResult::Mapped ||
         !current() )
    {
        return false;
    }

    const RECT rcCS = GetClientScreenRect();
    if ( !current() )
        return false;
    const POINT pt = {
        ptScreen.x - rcCS.left,
        ptScreen.y - rcCS.top
    };

    try
    {
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        // Fast rejection on the synced slot rects; the precise (and more
        // expensive) visual-tree query only runs when a rect matches or when
        // free elements (transients, menu bar...) live on the root.
        bool inSlot = false;
        for ( const auto& kv : m_slots )
        {
            const wxWinUISlot * const slot = kv.second;
            if ( slot->m_visible &&
                 slot->m_visibleRectPx.Contains(pt.x, pt.y) )
            {
                inSlot = true;
                break;
            }
        }

        const bool hasFreeContent =
            root.Children().Size() > m_slots.size();
        if ( !current() )
            return false;
        if ( !inSlot && !hasFreeContent )
            return false;

        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(dip, root);
        if ( !current() )
            return false;
        const auto iterator = hits.First();
        if ( !current() )
            return false;
        const bool hasCurrent = iterator.HasCurrent();
        return current() && hasCurrent;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

// ============================================================================
// misc
// ============================================================================

winrt::Microsoft::UI::Xaml::XamlRoot wxWinUITopLevelHost::GetXamlRoot() const
{
    return m_root ? m_root.XamlRoot()
                  : winrt::Microsoft::UI::Xaml::XamlRoot{ nullptr };
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
bool wxWinUITopLevelHost::OwnsOleDropRegistration() const
{
    return m_dropBroker && m_dropBroker->CoversShellDropTargets();
}
#endif

// Physical screen rect of the TLW client area, normalized left<right even
// for mirrored (RTL) windows.  Island coordinates are physically LTR while
// ClientToScreen/ScreenToClient on a WS_EX_LAYOUTRTL window flip the X
// axis, so every island<->screen conversion must go through this rect and
// never through those two calls (with the TLW handle) directly.
RECT wxWinUITopLevelHost::GetClientScreenRect() const
{
    RECT rc = { 0, 0, 0, 0 };
    const HWND hwndTLW = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    if ( !hwndTLW )
        return rc;

    ::GetClientRect(hwndTLW, &rc);
    ::MapWindowPoints(hwndTLW, nullptr, reinterpret_cast<POINT *>(&rc), 2);

    if ( rc.left > rc.right )
    {
        const LONG swap = rc.left;
        rc.left = rc.right;
        rc.right = swap;
    }

    return rc;
}

wxPoint wxWinUITopLevelHost::ScreenToRootDIP(const wxPoint& screenPt) const
{
    if ( !m_tlw )
        return screenPt;

    const RECT rcCS = GetClientScreenRect();
    const POINT pt = { screenPt.x - rcCS.left, screenPt.y - rcCS.top };

    const double scale = GetScale();
    return wxPoint(static_cast<int>(pt.x / scale),
                   static_cast<int>(pt.y / scale));
}

double wxWinUITopLevelHost::GetScale() const
{
    try
    {
        const auto xamlRoot = GetXamlRoot();
        if ( xamlRoot )
        {
            const double scale = xamlRoot.RasterizationScale();
            if ( scale > 0 )
                return scale;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return m_tlw ? m_tlw->GetDPIScaleFactor() : 1.0;
}

void wxWinUITopLevelHost::ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    OperationGuard operation(this);
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto root = m_root;
    if ( !root || !hostState || hostState->GetHost() != this )
        return;

    try
    {
        const auto previousRequestedTheme = root.RequestedTheme();
        const auto previousActualTheme = root.ActualTheme();
        if ( hostState->GetHost() != this ||
             m_shuttingDown || m_root != root )
        {
            return;
        }

        root.RequestedTheme(theme);
        if ( hostState->GetHost() != this ||
             m_shuttingDown || m_root != root )
        {
            return;
        }

        const auto currentRequestedTheme = root.RequestedTheme();
        const auto currentActualTheme = root.ActualTheme();
        if ( hostState->GetHost() != this ||
             m_shuttingDown || m_root != root )
        {
            return;
        }

        // ActualThemeChanged is the semantic signal for realized content, but
        // arm every current slot as a defensive catch-up for unloaded elements
        // which may not raise it until a later attach. Do this only for an
        // actual RequestedTheme/ActualTheme transition: assigning the same
        // theme doesn't make WinUI re-resolve a late ResourceDictionary.
        if ( previousRequestedTheme != currentRequestedTheme ||
             previousActualTheme != currentActualTheme )
        {
            for ( const auto& entry : m_slots )
            {
                wxWinUISlot * const slot = entry.second;
                if ( slot && !slot->m_deletePending && !slot->m_poisoned )
                {
                    if ( ++slot->m_automationNameStyleRevision == 0 )
                        ++slot->m_automationNameStyleRevision;
                    if ( ++slot->m_automationNameImplicitStyleRevision == 0 )
                        ++slot->m_automationNameImplicitStyleRevision;
                }
            }
        }
        MarkAllDirty();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("TLW theme change", e);
    }
}

void wxWinUITopLevelHost::ApplyThemeToAll(
    winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    // RequestedTheme can synchronously raise application-visible theme
    // callbacks which destroy a TLW. Snapshot invalidatable host lifetimes so
    // such a callback never invalidates this iteration.
    std::vector<std::shared_ptr<wxWinUIHostLifetime>> hosts;
    hosts.reserve(gs_tlwHosts.size());
    for ( const auto& kv : gs_tlwHosts )
    {
        if ( kv.second->IsReadyForLookup() )
            hosts.push_back(kv.second->m_hostLifetime);
    }

    for ( const auto& state : hosts )
    {
        // RequestedTheme is an application-visible setter. A callback on an
        // earlier host may have installed a newer global theme across the
        // complete snapshot; never resume this older iteration on later hosts.
        if ( theme != wxWinUIGetCurrentElementTheme() )
            return;

        if ( state )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
                host->ApplyTheme(theme);
        }

        if ( theme != wxWinUIGetCurrentElementTheme() )
            return;
    }
}

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreatePhysicalDisconnectGateForTesting(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
    bool satisfied,
    bool degraded)
{
    try
    {
        return std::make_shared<wxWinUIPhysicalDisconnectGateForTesting>(
            queue, satisfied, degraded);
    }
    catch ( ... )
    {
        return {};
    }
}

bool wxWinUISetPhysicalDisconnectGateStateForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate,
    bool satisfied,
    bool degraded)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUIPhysicalDisconnectGateForTesting>(gate);
    if ( !probe )
        return false;
    probe->SetState(satisfied, degraded);
    return true;
}

bool wxWinUIIsPhysicalDisconnectGateSatisfiedForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    return gate && gate->IsSatisfied();
}

bool wxWinUIIsPhysicalDisconnectGateDegradedForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    return gate && gate->IsDegraded();
}

bool wxWinUIOpenPhysicalDisconnectGateAtSealedCompletionForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUIPhysicalDisconnectGateForTesting>(gate);
    if ( !probe )
        return false;
    probe->OpenAtSealedCompletion();
    return true;
}

bool wxWinUIOpenPhysicalDisconnectGateBeforeSourceCloseForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUIPhysicalDisconnectGateForTesting>(gate);
    if ( !probe )
        return false;
    probe->OpenBeforeSourceClose();
    return true;
}

#endif // wxUSE_WINUI3
