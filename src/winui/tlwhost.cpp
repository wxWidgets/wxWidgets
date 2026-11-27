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
#include "wx/winui/private/appearance.h"
#include "wx/winui/xamlhost.h"
#if wxUSE_RICHTOOLTIP
    #include "wx/winui/private/richtooltip.h"
#endif
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/winui/private/dropbroker.h"
#endif

#include "private.h"
#include "compositeretirement.h"
#ifdef WXWINUI_TEST_SUPPORT
#include "host-cursor-test-access.h"
#include "host-content-test-access.h"
#include "host-observation-test-access.h"
#include "physical-disconnect-test-access.h"
#endif
#include "hostnative.h"
#include "hostinput.h"
#include "hostfocus.h"
#include "hostprofile.h"
#include "hostresize.h"
#include "hostgeometry.h"
#include "scrollbarprojection.h"
#include "hostvisual.h"
#include "hostuia.h"
#include "visualcoordinates.h"

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
    #include "hosttooltip.h"
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
// What is on screen at a point: set WX_WINUI_DUMP_AT=<x>,<y>,<file> to have
// each window write to the file, five seconds after it got its island, the
// elements of its XAML tree which are under that point of its client area
// (in pixels), the front one first, with their place, their size and their
// fill. This tells which element something seen on screen is, instead of
// guessing it from a capture. No-op when the variable is unset.
// ----------------------------------------------------------------------------

void wxWinUIArmElementDump(
    const winrt::Microsoft::UI::Xaml::Controls::Canvas& root)
{
    namespace DX = winrt::Microsoft::UI::Xaml;

    wchar_t text[600];
    const DWORD length = ::GetEnvironmentVariableW(
        L"WX_WINUI_DUMP_AT", text, WXSIZEOF(text));
    if ( !length || length >= WXSIZEOF(text) )
        return;

    double x = 0, y = 0;
    wchar_t path[512] = L"";
    if ( swscanf(text, L"%lf,%lf,%511[^\n]", &x, &y, path) != 3 )
        return;

    try
    {
        const std::wstring file(path);
        DX::DispatcherTimer timer;
        timer.Interval(std::chrono::seconds(5));
        timer.Tick([timer, root, x, y, file](const auto&, const auto&)
        {
            timer.Stop();
            FILE * const f = _wfopen(file.c_str(), L"a");
            if ( !f )
                return;

            try
            {
                const auto xamlRoot = root.XamlRoot();
                const double scale =
                    xamlRoot ? xamlRoot.RasterizationScale() : 1.0;
                const winrt::Windows::Foundation::Point point
                {
                    static_cast<float>(x / scale),
                    static_cast<float>(y / scale)
                };
                fprintf(f, "==== pid %lu: elements at %g,%g px (scale %g) ====\n",
                        ::GetCurrentProcessId(), x, y, scale);

                const auto fill = [](const DX::Media::Brush& brush)
                {
                    std::string colour = "-";
                    if ( !brush )
                        return colour;

                    if ( const auto solid =
                            brush.try_as<DX::Media::SolidColorBrush>() )
                    {
                        const auto c = solid.Color();
                        char buffer[16];
                        snprintf(buffer, sizeof(buffer), "#%02X%02X%02X%02X",
                                 c.A, c.R, c.G, c.B);
                        colour = buffer;
                    }
                    else
                    {
                        colour = "brush";
                    }
                    return colour;
                };

                for ( const auto& hit :
                        DX::Media::VisualTreeHelper::
                            FindElementsInHostCoordinates(point, root, true) )
                {
                    const auto element = hit.try_as<DX::FrameworkElement>();
                    if ( !element )
                        continue;

                    DX::Media::Brush brush{ nullptr };
                    if ( const auto panel = element.try_as<DX::Controls::Panel>() )
                        brush = panel.Background();
                    else if ( const auto border = element.try_as<DX::Controls::Border>() )
                        brush = border.Background();
                    else if ( const auto control = element.try_as<DX::Controls::Control>() )
                        brush = control.Background();
                    else if ( const auto presenter = element.try_as<DX::Controls::ContentPresenter>() )
                        brush = presenter.Background();

                    const auto origin = element.TransformToVisual(root)
                                            .TransformPoint({ 0, 0 });
                    fprintf(f, "%ls name='%ls' id='%ls' at %.0f,%.0f size %.0fx%.0f px fill %s opacity %g\n",
                            winrt::get_class_name(element).c_str(),
                            element.Name().c_str(),
                            DX::Automation::AutomationProperties::
                                GetAutomationId(element).c_str(),
                            origin.X * scale, origin.Y * scale,
                            element.ActualWidth() * scale,
                            element.ActualHeight() * scale,
                            fill(brush).c_str(), element.Opacity());
                }
            }
            catch ( const winrt::hresult_error& )
            {
                fprintf(f, "(the tree could not be read)\n");
            }

            fclose(f);
        });
        timer.Start();
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

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

long long gs_profileTicks[static_cast<size_t>(wxWinUIProfilePhase::Max)] = {};


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
#ifdef WXWINUI_TEST_SUPPORT
std::atomic<unsigned> gs_pendingTLWTerminalRetirements{ 0 };
std::atomic<unsigned> gs_pendingPhysicalDisconnectGates{ 0 };
#endif
std::uint64_t gs_nextPhysicalDisconnectRetirementId = 0;

class wxWinUITLWTerminalRetirement final
{
public:
#ifdef WXWINUI_TEST_SUPPORT
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
#endif

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

#ifdef WXWINUI_TEST_SUPPORT
        if ( m_counted.load(std::memory_order_acquire) )
        {
            gs_pendingTLWTerminalRetirements.fetch_sub(
                1, std::memory_order_relaxed);
        }
#endif
    }

    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource
        m_source{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Canvas m_root{ nullptr };
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> m_gates;
    std::atomic<bool> m_completionArmed{false};
    std::atomic<bool> m_completionRequested{false};
    std::atomic<bool> m_completed{false};
#ifdef WXWINUI_TEST_SUPPORT
    std::atomic<bool> m_counted{false};
#endif
    std::atomic<unsigned> m_pendingGates{0};
    std::atomic<wxWinUIFrameworkRetirementId> m_retirementId{0};
};

// Cumulative counters for the lifecycle unit tests. Shipping builds retain
// the real registrations and lifetime protocol without this telemetry.
#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_liveHostLifetimes = 0;
unsigned gs_liveSlotLifetimes = 0;
unsigned gs_slotHandlerAdds = 0;
unsigned gs_slotHandlerRevokes = 0;
unsigned gs_rootHandlerAdds = 0;
unsigned gs_rootHandlerRevokes = 0;
unsigned gs_liveSubclassContexts = 0;
#endif

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
#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_slotCursorSets = 0;
#endif

// Every application-wide cursor-policy mutation advances this generation.
// Per-slot and island-level caches include it, so nested busy-cursor
// transitions and wxSetCursor() can never be hidden by an unchanged window
// identity or HCURSOR value.
unsigned long long gs_cursorPolicyGeneration = 1;

#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_failInputPointerSourceSetAt = 0;
#endif // WXWINUI_TEST_SUPPORT

// Registration-rollback fault injection (see TestFailHandlerAdd): countdown
// to the handler subscription that must throw, 0 = disabled.
#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_failHandlerAddAt = 0;

void wxWinUIMaybeInjectHandlerFault()
{
    if ( gs_failHandlerAddAt && --gs_failHandlerAddAt == 0 )
    {
        throw winrt::hresult_error(E_FAIL,
                                   L"wxWinUI test: injected handler fault");
    }
}
#endif // WXWINUI_TEST_SUPPORT

// Content-transaction fault injection (see TestFailContentSwap): a mask of
// TestContentFault_* bits; each phase consumes its own bit when it fires.
#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_failContentMask = 0;
unsigned gs_failContentLoadedObserverAdds = 0;
#endif // WXWINUI_TEST_SUPPORT
#ifdef WXWINUI_TEST_SUPPORT
unsigned gs_liveContentLoadedStates = 0;
std::function<void (wxWindow *)> gs_testSlotAttachedHook;
std::function<void (wxWindow *)> gs_testContentCarrierSetHook;
std::function<void (wxWindow *)> gs_testContentTransactionStartedHook;
std::function<void (wxWindow *)> gs_testWindowAtPointSlotStateHook;
wxWinUIHostContentTestAccess::InitializationHook
    gs_testHostInitializationHook = nullptr;
unsigned gs_testHostInitializationFaultMask =
    wxWinUIHostContentTestAccess::InitializationFault_None;
bool gs_testHostInitializationHookInProgress = false;
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIMaybeInjectContentFault(unsigned phaseBit)
{
    if ( gs_failContentMask & phaseBit )
    {
        gs_failContentMask &= ~phaseBit;
        throw winrt::hresult_error(E_FAIL,
                                   L"wxWinUI test: injected content fault");
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT

constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_BRIDGE = 1;
constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_INNER = 2;

// SetWindowSubclass() may outlive its owning wx host when removal fails.
// Never give USER32 a raw host pointer: this context can be deactivated
// independently and then either deleted after a successful removal or left
// for WM_NCDESTROY to delete. The weak lifetime makes every late callback a
// harmless DefSubclassProc() even after the wx host has gone away.
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

// The implementation object of each slot's container, for cursor access.
std::map<const wxWinUISlot *, winrt::com_ptr<wxWinUISlotGrid>> gs_slotGrids;

// The implementation object of each host's root canvas.
std::map<const wxWinUITopLevelHost *,
         winrt::com_ptr<wxWinUIRootCanvas>> gs_rootCanvases;

// The hosts whose root canvas carries the cursor of the native window under
// the pointer, see SetIslandPointerCursor(); and whether the cursor being
// set right now is such a cursor.
std::set<const wxWinUITopLevelHost *> gs_rootCursorMirrors;
thread_local bool gs_mirroringNativeCursor = false;

template <typename Setter>
bool wxWinUISetAsNativeMirror(Setter&& setter)
{
    const bool previous = gs_mirroringNativeCursor;
    gs_mirroringNativeCursor = true;
    const bool set = setter();
    gs_mirroringNativeCursor = previous;
    return set;
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

} // anonymous namespace

wxWinUIHostLifetime::wxWinUIHostLifetime(wxWinUITopLevelHost *host)
    : m_host(host)
{
#ifdef WXWINUI_TEST_SUPPORT
    ++gs_liveHostLifetimes;
#endif
}

wxWinUIHostLifetime::~wxWinUIHostLifetime()
{
#ifdef WXWINUI_TEST_SUPPORT
    --gs_liveHostLifetimes;
#endif
}

wxWinUISlotLifetime::wxWinUISlotLifetime(
    wxWindow *window,
    wxWinUITopLevelHost *host)
    : m_window(window),
      m_host(host)
{
#ifdef WXWINUI_TEST_SUPPORT
    ++gs_liveSlotLifetimes;
#endif
}

wxWinUISlotLifetime::~wxWinUISlotLifetime()
{
#ifdef WXWINUI_TEST_SUPPORT
    --gs_liveSlotLifetimes;
#endif
}

wxWinUISubclassContext::wxWinUISubclassContext(
    const std::shared_ptr<wxWinUIHostLifetime>& hostState)
    : host(hostState)
{
#ifdef WXWINUI_TEST_SUPPORT
    ++gs_liveSubclassContexts;
#endif
}

wxWinUISubclassContext::~wxWinUISubclassContext()
{
#ifdef WXWINUI_TEST_SUPPORT
    --gs_liveSubclassContexts;
#endif
}

winrt::com_ptr<wxWinUISlotGrid> wxWinUIGetSlotGrid(const wxWinUISlot *slot)
{
    const auto found = gs_slotGrids.find(slot);
    if ( found != gs_slotGrids.end() )
        return found->second;
    return nullptr;
}

winrt::com_ptr<wxWinUIRootCanvas>
wxWinUIGetRootCanvas(const wxWinUITopLevelHost *host)
{
    const auto found = gs_rootCanvases.find(host);
    if ( found != gs_rootCanvases.end() )
        return found->second;
    return nullptr;
}

unsigned long long wxWinUIGetCursorPolicyGeneration()
{
    return gs_cursorPolicyGeneration;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUINoteSlotCursorSet()
{
    ++gs_slotCursorSets;
}
#endif

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

bool wxWinUIIsGlobalOperationActive()
{
    return gs_winuiOperationDepth != 0;
}

bool wxWinUIInputLogEnabled() { return wxWinUIInputLogFile() != nullptr; }

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIMaybeInjectAutomationNameContentFault(unsigned phaseBit)
{
    wxWinUIMaybeInjectContentFault(phaseBit);
}
#endif // WXWINUI_TEST_SUPPORT

namespace
{

// 0: off, 1: on, 2: on and compared with the walk through the whole tree.
int wxWinUIInputNodesMode()
{
    static const int s_mode = []()
    {
        wchar_t text[8];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_INPUT_NODES", text, WXSIZEOF(text));
        if ( length == 1 && text[0] == L'0' )
            return 0;
        return length == 1 && text[0] == L'2' ? 2 : 1;
    }();

    return s_mode;
}

} // anonymous namespace

bool wxWinUIInputNodesEnabled()
{
    return wxWinUIInputNodesMode() != 0;
}

bool wxWinUIInputNodesCompared()
{
    return wxWinUIInputNodesMode() == 2;
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
        wxLogDebug("wxWinUI: failed to remove the %s window subclass "
                     "(the API exposes no extended error); it was safely "
                     "deactivated", description);
    }

    // On removal failure ownership has transferred to WM_NCDESTROY.
    opaqueContext = nullptr;
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

#ifdef WXWINUI_TEST_SUPPORT
class wxWinUITestPhysicalDisconnectGate final :
    public wxWinUIPhysicalDisconnectGate
{
public:
    wxWinUITestPhysicalDisconnectGate(
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
#endif


class wxWinUIContentLoadedState
{
public:
    wxWinUIContentLoadedState()
    {
#ifdef WXWINUI_TEST_SUPPORT
        ++gs_liveContentLoadedStates;
#endif
    }

    ~wxWinUIContentLoadedState()
    {
#ifdef WXWINUI_TEST_SUPPORT
        --gs_liveContentLoadedStates;
#endif
    }

    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> content;
    unsigned long long contentGeneration = 0;
    bool active = true;
};


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
#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIMaybeInjectContentLoadedObserverFault();
#endif // WXWINUI_TEST_SUPPORT
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

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUISlotTestAccess::InvokeContentLoadedObserver(wxWinUISlot& slot)
{
    slot.HandleContentLoadedObserver(slot.m_contentLoadedState);
}
#endif

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


void wxWinUISlot::AdvanceContentGeneration()
{
    if ( ++m_contentGeneration == 0 )
        ++m_contentGeneration;

    // Increment first: UiaReturnRawElementProvider(..., nullptr) is an
    // external COM boundary. If it causes a new WM_GETOBJECT synchronously,
    // that request must already bind to the replacement identity.
    RetireExposedAccessibilityShellProvider();
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

    if ( !host || (!m_carrier && !m_directRoot) || m_compositeRetiring ||
         host->m_compositeMigrationInProgress )
        return false;

    // An extension witnesses this exact parent publication, not a future
    // replacement. Retire it through Close(), even when no children remain.
    if ( m_compositeContentExtension &&
         (element != m_content || semanticTarget != m_semanticTarget) )
        return false;

    // Direct collection roots have stable identity. Their descendants may
    // mutate normally, but replacing the root must retire this slot first.
    if ( m_directRoot &&
         (element != m_content || semanticTarget != m_semanticTarget ||
          !host->IsCompositeMountCurrent(*this)) )
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

    // A composite publication owns the mounts of its child slots. Rebuilding
    // it requires detaching those children first; never strand a live logical
    // child in the old visual graph or quietly remount it on the root Canvas.
    if ( host->HasCompositeChildren(*this) )
        return false;

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
    // such a pass must leave without syncing old model A against
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
        // Dirty only after the carrier/model pair is truthful. Nested flushes
        // leave the in-progress slot alone; this boundary owns their catch-up.
        NudgeDirty();
        return true;
    };

    // Deterministic equivalent of an old-content Loaded callback entering
    // immediately after the transaction became observable. Consume the hook
    // before invocation so nested content changes cannot recurse through it.
#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT

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
#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIMaybeInjectContentFault(
            wxWinUIHostContentTestAccess::ContentFault_Detach);
#endif // WXWINUI_TEST_SUPPORT
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
#ifdef WXWINUI_TEST_SUPPORT
            wxWinUIMaybeInjectContentFault(
                wxWinUIHostContentTestAccess::ContentFault_Install);
#endif // WXWINUI_TEST_SUPPORT
            m_carrier.Content(element);
            if ( !transactionIsCurrent() )
                return false;
        }

        // The actual XAML Loaded/property boundary can occur inside the
        // Content() call. This one-shot seam deterministically exercises the
        // same state immediately afterwards, while the carrier is B and the
        // published slot model is still A.
#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT

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
#ifdef WXWINUI_TEST_SUPPORT
            wxWinUIMaybeInjectContentFault(
                wxWinUIHostContentTestAccess::ContentFault_Restore);
#endif // WXWINUI_TEST_SUPPORT
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
#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT
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
#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIMaybeInjectContentFault(
            wxWinUIHostContentTestAccess::ContentFault_Read);
#endif // WXWINUI_TEST_SUPPORT
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

#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIHostContentTestAccess::InvokeInitializationHook(*host,
            wxWinUIHostContentTestAccess::InitializationHookPoint::AfterProvisionalPublication);
#endif // WXWINUI_TEST_SUPPORT
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
                wxLogDebug("wxWinUI: duplicate unowned slots detected for "
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

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::InvokeInitializationHook(
    wxWinUITopLevelHost& host, InitializationHookPoint point)
{
    const InitializationHook callback =
        gs_testHostInitializationHook;
    if ( !callback || gs_testHostInitializationHookInProgress )
        return;

    wxWindow * const tlw = host.m_tlwLifetime.get();
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
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
static void wxWinUIMaybeInjectInitializationFault(unsigned fault)
{
    if ( gs_testHostInitializationFaultMask & fault )
    {
        gs_testHostInitializationFaultMask &= ~fault;
        throw winrt::hresult_error(
            E_FAIL, L"wxWinUI test: injected host initialization fault");
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::SetInitializationHook(
    InitializationHook callback)
{
    gs_testHostInitializationHook = callback;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::FailInitialization(unsigned mask)
{
    gs_testHostInitializationFaultMask = mask;
}
#endif // WXWINUI_TEST_SUPPORT

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
#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
            wxWinUIHostContentTestAccess::InitializationHookPoint::AfterSourceInitialization);
#endif // WXWINUI_TEST_SUPPORT
        if ( !HasInitializationContext() )
            return false;

        const auto siteBridge = m_source.SiteBridge();
        if ( !HasInitializationContext() )
            return false;
        siteBridge.ResizePolicy(
            ContentSizePolicy::ResizeContentToParentWindow);
        if ( !HasInitializationContext() )
            return false;
        if ( const int forcedDPI = wxWinUIGetForcedDPI() )
        {
            siteBridge.OverrideScale(forcedDPI / 96.0f);
            if ( !HasInitializationContext() )
                return false;
        }

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

        wxWinUIArmElementDump(m_root);

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
#ifdef WXWINUI_TEST_SUPPORT
        if ( m_takeFocusToken.value )
            ++gs_rootHandlerAdds;
#endif
        if ( !HasInitializationContext() )
            return false;

        if ( !BindRootRouter() )
            return false;

#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
            wxWinUIHostContentTestAccess::InitializationHookPoint::AfterRootRouter);
#endif // WXWINUI_TEST_SUPPORT
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
            wxLogDebug("wxWinUI: unable to create the OLE drop broker; "
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
#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
            wxWinUIHostContentTestAccess::InitializationHookPoint::BeforeExceptionLog);
#endif // WXWINUI_TEST_SUPPORT
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
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
        wxWinUIHostContentTestAccess::InitializationHookPoint::AfterTitleBarPolicy);
#endif // WXWINUI_TEST_SUPPORT
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

#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
        wxWinUIHostContentTestAccess::InitializationHookPoint::AfterTLWBindings);
#endif // WXWINUI_TEST_SUPPORT
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIMaybeInjectInitializationFault(wxWinUIHostContentTestAccess::InitializationFault_AfterTLWBindings);
#endif // WXWINUI_TEST_SUPPORT
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

    if ( m_dropBroker->HasNativeOwnership() )
    {
        // Native cleanup could not prove that both old identities are gone.
        // Retain this fail-closed broker and never create a concurrent pair.
        wxLogDebug(
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
        wxLogDebug("wxWinUI: unable to recreate the OLE drop broker after "
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
        wxLogDebug(
            "wxWinUI: OLE drop broker visible acquisition failed "
            "(HRESULT 0x%08lx); continuing with XAML hosting and OLE "
            "drops disabled",
            static_cast<unsigned long>(result.hresult));
        m_dropBroker->Shutdown();
        m_dropBroker.reset();
        return;
    }

#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIHostContentTestAccess::InvokeInitializationHook(*this,
        wxWinUIHostContentTestAccess::InitializationHookPoint::AfterDropBrokerReadyBeforeCommit);
#endif // WXWINUI_TEST_SUPPORT
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

        ShutdownNativeResize();
        if ( m_scrollbarProjection )
            m_scrollbarProjection->Shutdown();

        try
        {
            m_shutdownPhysicalDisconnectGates.reserve(
                m_slots.size() + m_deferredDisconnectSlots.size());
            const auto takeOwnGate = [](wxWinUISlot *slot)
            {
                if ( slot && slot->m_compositeDisconnectGate )
                {
                    // The terminal ledger owns the entire source/root graph
                    // and collects every child's own gate independently. It
                    // replaces the live parent's physical-receipt fan-in;
                    // deleting wrappers below must not fabricate receipts.
                    slot->m_disconnectGate =
                        slot->m_compositeDisconnectGate
                            ->TakeParentGateForGraphRetirement();
                    slot->m_compositeDisconnectGate.reset();
#ifdef WXWINUI_TEST_SUPPORT
                    if ( slot->m_disconnectPending && !slot->m_disconnectGate )
                    {
                        gs_pendingPhysicalDisconnectGates.fetch_sub(
                            1, std::memory_order_relaxed);
                    }
#endif
                }
            };
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
                takeOwnGate(entry.second);
                if ( entry.second )
                    sealGate(entry.second->m_disconnectGate);
            }
            for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
            {
                takeOwnGate(slot);
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
            const auto migration = m_focusMigration;
            wxWinUITopLevelHost * const destination = migration->destinationHost
                ? migration->destinationHost->GetHost() : nullptr;
            wxWindow * const window = migration->window.get();
            // A composite transfer keeps the SAME slot identity. Once it is
            // published in B, destroying A cannot retire B's focused child.
            // Hand the still-pending arbiter to B before releasing A. Ordinary
            // migrations have distinct source/destination slots and retain
            // their existing shutdown rollback policy.
            if ( m_compositeMigrationInProgress && destination &&
                 destination != this &&
                 destination->m_compositeMigrationInProgress &&
                 migration->logicalFocusWasActive && !migration->committed &&
                 migration->sourceHost == m_hostLifetime &&
                 migration->sourceSlot == migration->destinationSlot &&
                 migration->destinationSlot && window &&
                 migration->destinationSlot->GetHost() == destination &&
                 migration->destinationSlot->GetWindow() == window &&
                 destination->FindSlot(window) &&
                 destination->FindSlot(window)->m_lifetime ==
                     migration->destinationSlot &&
                 destination->m_focusMigration == migration &&
                 m_focusOwner == window &&
                 m_focusIntentGeneration == migration->sourceFocusIntentGeneration )
            {
                migration->sourceHost = destination->m_hostLifetime;
                migration->sourceFocusIntentGeneration =
                    destination->m_focusIntentGeneration;
                destination->m_focusOwner = window;
                m_focusOwner = nullptr;
                m_focusMigration.reset();

                // The shell has moved to B but USER32 may still consider A
                // active. Destroying A would then clear the parked native
                // focus before the migration's outer continuation runs.
                // Complete the same ticket on B now, while its exact native
                // authority is still live; never reacquire focus after an
                // application/native redirection has already won.
                if ( HasFocusMigrationNativeAuthority(migration, destination) )
                {
                    if ( ++destination->m_focusIntentGeneration == 0 )
                        ++destination->m_focusIntentGeneration;
                    const auto continuationGeneration =
                        destination->m_focusIntentGeneration;
                    destination->ClearDeferredFocusRequest();
                    migration->sourceFocusIntentGeneration =
                        continuationGeneration;
                    destination->FocusSlotImpl(
                        window, FocusOrigin::MigrationContinuation,
                        migration->previous.get(), continuationGeneration, true);
                }
            }
            else
            {
                // Every gate is sealed before this callback-bearing rollback.
                AbortFocusMigration(migration, true);
            }
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
        if ( !slot->m_compositeRetiring && slot->m_operationDepth == 0 &&
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
            // A composite parent stays attached until Source.Close even if
            // its own popup gate is empty: an independently retained child
            // may still need the complete visual ancestry. All other slots
            // must have genuinely unsatisfied causal gates after this drain.
            if ( !slot ||
                 (!slot->m_compositeRetiring &&
                  (!slot->m_disconnectGate ||
                   slot->m_disconnectGate->IsSatisfied())) )
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
    // With no popup gates, deferred composite parents need only the ordinary
    // Source.Close boundary below, exactly like the still-live shutdown slots.

    // TakeFocusRequested is a host callback too.  Revoke it in phase 1,
    // before Close(), just like the root-router callbacks.
    if ( m_source && m_takeFocusToken.value )
    {
        try
        {
            m_source.TakeFocusRequested(m_takeFocusToken);
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_rootHandlerRevokes;
#endif
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
#ifdef WXWINUI_TEST_SUPPORT
                ++gs_rootHandlerRevokes;
#endif
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
    m_rootPointerQueue.clear();
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
    m_parts.clear();
    m_interactiveParts = 0;
    m_nodes.clear();
    m_retiredCanvases.clear();

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
    m_islandPointerCursorSet = nullptr;
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
    m_compositeSlots.clear();
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
#ifdef WXWINUI_TEST_SUPPORT
            gs_pendingPhysicalDisconnectGates.fetch_sub(
                1, std::memory_order_relaxed);
#endif
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
    gs_rootCursorMirrors.erase(this);
    m_scrollbarProjection.reset();
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
#ifdef WXWINUI_TEST_SUPPORT
        if ( terminalRetirement->GetPendingGateCount() != 0 )
            terminalRetirement->Promote();
#endif
    }
    sealedGateAliases.clear();

    m_shutdownComplete = true;
    m_shutdownFinalizing = false;
}

// ============================================================================
// slots
// ============================================================================

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUIHostObservationTestAccess::GetLiveHostCount()
{
    return static_cast<unsigned>(gs_tlwHosts.size());
}

unsigned wxWinUIHostObservationTestAccess::GetLiveSlotCount()
{
    unsigned count = 0;
    for ( const auto& kv : gs_tlwHosts )
        count += static_cast<unsigned>(kv.second->m_slots.size());
    return count;
}

unsigned wxWinUIHostObservationTestAccess::GetLiveSlotLifetimeCount()
{
    return gs_liveSlotLifetimes;
}

unsigned wxWinUIHostObservationTestAccess::GetSlotHandlerAddCount()
{
    return gs_slotHandlerAdds;
}

unsigned wxWinUIHostObservationTestAccess::GetSlotHandlerRevokeCount()
{
    return gs_slotHandlerRevokes;
}

unsigned wxWinUIHostObservationTestAccess::GetLiveHostLifetimeCount()
{
    return gs_liveHostLifetimes;
}

unsigned wxWinUIHostObservationTestAccess::GetRootHandlerAddCount()
{
    return gs_rootHandlerAdds;
}

unsigned wxWinUIHostObservationTestAccess::GetRootHandlerRevokeCount()
{
    return gs_rootHandlerRevokes;
}

unsigned wxWinUIHostObservationTestAccess::GetLiveSubclassContextCount()
{
    return gs_liveSubclassContexts;
}

unsigned
wxWinUIHostObservationTestAccess::GetPendingPhysicalDisconnectGateCount()
{
    return gs_pendingPhysicalDisconnectGates.load(std::memory_order_acquire);
}

unsigned
wxWinUIHostObservationTestAccess::GetPendingTLWTerminalRetirementCount()
{
    return gs_pendingTLWTerminalRetirements.load(std::memory_order_acquire);
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
wxWinUIDropBroker *
wxWinUIHostObservationTestAccess::GetDropBroker(
    const wxWinUITopLevelHost& host)
{
    return host.m_dropBroker.get();
}
#endif
#endif


#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUIHostContentTestAccess::GetLiveContentLoadedObserverCount()
{
    return gs_liveContentLoadedStates;
}
#endif

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::FailHandlerAdd(unsigned nth)
{
    gs_failHandlerAddAt = nth;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::FailContentSwap(unsigned mask)
{
    gs_failContentMask = mask;
}
#endif // WXWINUI_TEST_SUPPORT

#if wxUSE_TOOLTIPS
// Keep the content-transaction fault mask in its existing authority. The
// extracted observer consumes exactly the same phase bit, at the same point.
#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIMaybeInjectToolTipContentFault()
{
    wxWinUIMaybeInjectContentFault(
        wxWinUIHostContentTestAccess::ContentFault_ToolTipObserver);
}
#endif // WXWINUI_TEST_SUPPORT
#endif // wxUSE_TOOLTIPS

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::FailContentLoadedObserverAdds(unsigned count)
{
    gs_failContentLoadedObserverAdds = count;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::FailInputPointerSourceSet(unsigned nth)
{
    gs_failInputPointerSourceSetAt = nth;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::OnNextSlotAttached(
    std::function<void (wxWindow *)> callback)
{
    gs_testSlotAttachedHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::OnNextContentCarrierSet(
    std::function<void (wxWindow *)> callback)
{
    gs_testContentCarrierSetHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::OnNextContentTransactionStarted(
    std::function<void (wxWindow *)> callback)
{
    gs_testContentTransactionStartedHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostContentTestAccess::OnNextWindowAtPointSlotStateRead(
    std::function<void (wxWindow *)> callback)
{
    gs_testWindowAtPointSlotStateHook = std::move(callback);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUIHostCursorTestAccess::GetSlotCursorSetCount()
{
    return gs_slotCursorSets;
}

winrt::Microsoft::UI::Input::InputCursor
wxWinUIHostCursorTestAccess::GetSlotCursor(
    const wxWinUITopLevelHost& host,
    const wxWindow *window)
{
    wxWinUISlot * const slot = host.FindSlot(window);
    if ( !slot )
        return nullptr;

    const auto it = gs_slotGrids.find(slot);
    return it != gs_slotGrids.end()
             ? it->second->GetWindowCursor()
             : nullptr;
}

winrt::Microsoft::UI::Input::InputCursor
wxWinUIHostCursorTestAccess::GetIslandCursor(
    const wxWinUITopLevelHost& host)
{
    if ( host.m_rootPointerCursorAuthoritative )
        return host.m_lastIslandPointerCursor;

    if ( !host.m_inputPointerSource )
        return host.m_islandPointerCursorApplied
             ? host.m_lastIslandPointerCursor
             : nullptr;

    try
    {
        return host.m_inputPointerSource.Cursor();
    }
    catch ( const winrt::hresult_error& )
    {
        return nullptr;
    }
}

bool
wxWinUIHostCursorTestAccess::ActivateSlotCursor(
    wxWinUITopLevelHost& host,
    wxWindow *window,
    bool emitSetCursorEvent)
{
    wxWinUISlot * const slot = host.FindSlot(window);
    return slot &&
           host.ApplySlotPointerCursor(slot->m_lifetime,
                                  emitSetCursorEvent,
                                  0,
                                  !emitSetCursorEvent);
}

bool
wxWinUIHostCursorTestAccess::ApplySlotPointerCursor(
    wxWinUITopLevelHost& host,
    wxWindow *window,
    const wxPoint& screenPoint,
    wxWinUIInputKind kind)
{
    host.m_lastPointerScreen.x = screenPoint.x;
    host.m_lastPointerScreen.y = screenPoint.y;
    return ActivateSlotCursor(host,
        window, wxWinUIShouldEmitSetCursor(kind));
}

bool
wxWinUIHostCursorTestAccess::SimulateXamlCursorSelection(
    wxWinUITopLevelHost& host,
    const winrt::Microsoft::UI::Input::InputCursor& cursor)
{
    if ( !host.m_inputPointerSource )
        return false;

    try
    {
        // This writes only the runtime property, deliberately leaving the
        // host's ownership bookkeeping untouched. ObserveXamlPointerCursor()
        // must discover and yield to the external/XAML mutation exactly as
        // the production routed-event path does.
        host.m_inputPointerSource.Cursor(cursor);
        host.ObserveXamlPointerCursor();
        return !host.m_inputPointerSourceAuthoritative;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool
wxWinUIHostCursorTestAccess::OverwriteIslandCursor(
    wxWinUITopLevelHost& host,
    const winrt::Microsoft::UI::Input::InputCursor& cursor)
{
    if ( !host.m_inputPointerSource )
        return false;

    try
    {
        // What XAML does before raising a pointer event on the root canvas:
        // the property changes and the host is told nothing.
        host.m_inputPointerSource.Cursor(cursor);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

WXHCURSOR
wxWinUIHostCursorTestAccess::GetIslandCursorHandle(
    const wxWinUITopLevelHost& host)
{
    return host.m_lastIslandPointerHandle;
}

bool
wxWinUIHostCursorTestAccess::IsInputPointerSourceAuthoritative(
    const wxWinUITopLevelHost& host)
{
    return host.m_inputPointerSourceAuthoritative;
}

bool
wxWinUIHostCursorTestAccess::RefreshPointerCursorAt(
    wxWinUITopLevelHost& host,
    const wxPoint& screenPoint)
{
    host.m_lastPointerScreen.x = screenPoint.x;
    host.m_lastPointerScreen.y = screenPoint.y;
    return host.RefreshPointerCursorAtLastPoint();
}
#endif // WXWINUI_TEST_SUPPORT

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
wxWinUITopLevelHost::RegisterSlotInParent(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
    wxWindow *compositeOwner,
    const winrt::Microsoft::UI::Xaml::Controls::ContentControl& contentMount,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    if ( !compositeOwner || !contentMount )
        return nullptr;
    return RegisterSlotInternal(window, element, semanticTarget, nullptr,
                                false, false, wxString(), gate,
                                compositeOwner, contentMount);
}

wxWinUISlot *
wxWinUITopLevelHost::RegisterSlotInParentDirect(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::Controls::Control& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
    wxWindow *compositeOwner,
    const std::shared_ptr<wxWinUIDirectCompositeMount>& mount,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    if ( !element || !compositeOwner || !mount || !mount->owner.get() ||
         !mount->insert || !mount->contains || !mount->remove )
        return nullptr;
    return RegisterSlotInternal(window, element, semanticTarget, nullptr,
                                false, false, wxString(), gate,
                                compositeOwner, nullptr, mount);
}

bool wxWinUITopLevelHost::HasCompositeChildren(const wxWinUISlot& parent) const
{
    for ( const auto child : m_compositeSlots )
        if ( child->m_compositeOwner.lock() == parent.m_lifetime )
            return true;
    return false;
}

bool wxWinUITopLevelHost::SetSlotCompositeContentExtension(
    wxWindow *window,
    const std::shared_ptr<wxWinUICompositeContentExtension>& extension)
{
    OperationGuard hostOperation(this);
    wxWinUISlot * const slot = FindSlot(window);
    wxWinUISlot::OperationGuard slotOperation(slot);
    if ( !slot || !extension || !extension->isCurrent ||
         m_shuttingDown || m_compositeMigrationInProgress ||
         slot->m_deletePending || slot->m_poisoned ||
         slot->m_compositeRetiring || slot->m_contentTransactionInProgress ||
         slot->m_compositeContentExtensionValidating || !slot->m_content ||
         (slot->m_compositeContentExtension &&
          slot->m_compositeContentExtension != extension) )
        return false;

    const auto state = slot->m_lifetime;
    const auto content = slot->m_content;
    const auto generation = slot->m_contentGeneration;
    const auto previous = slot->m_compositeContentExtension;
    const auto current = [&]()
    {
        return state && state->GetHost() == this &&
               state->GetWindow() == window && FindSlot(window) == slot &&
               !m_shuttingDown && !m_compositeMigrationInProgress &&
               !slot->m_deletePending && !slot->m_poisoned &&
               !slot->m_compositeRetiring &&
               !slot->m_contentTransactionInProgress &&
               slot->m_content == content &&
               slot->m_contentGeneration == generation &&
               slot->m_compositeContentExtension == previous;
    };
    try
    {
        const auto root = extension->root.get();
        if ( !root || !current() ||
             (previous &&
              (slot->m_compositeContentExtensionRoot.get() != root ||
               slot->m_compositeContentExtensionGeneration != generation)) )
            return false;

        slot->m_compositeContentExtensionValidating = true;
        const auto finish = wxMakeGuard([slot]()
        {
            slot->m_compositeContentExtensionValidating = false;
        });
        // The witness may cross an arbitrary native callback. OperationGuard
        // keeps allocations alive, but only this exact publication can win.
        // Initial registration can precede template materialization. It
        // grants no authority until IsCompositeParentMountCurrent witnesses
        // a current template; repeating a publication explicitly rechecks it.
        if ( (previous && !extension->isCurrent()) || !current() ||
             extension->root.get() != root || !current() )
            return false;

        slot->m_compositeContentExtension = extension;
        slot->m_compositeContentExtensionRoot = extension->root;
        slot->m_compositeContentExtensionGeneration = generation;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("validate composite content extension", e);
    }
    catch ( ... )
    {
        // A failed ownership witness is a local refusal, not a lost physical
        // popup receipt: do not poison unrelated control publication.
    }
    return false;
}

bool wxWinUITopLevelHost::IsCompositeParentMountCurrent(
    const wxWinUISlot& parent,
    const winrt::Microsoft::UI::Xaml::FrameworkElement& mount) const
{
    OperationGuard hostOperation(const_cast<wxWinUITopLevelHost *>(this));
    auto& mutableParent = const_cast<wxWinUISlot&>(parent);
    wxWinUISlot::OperationGuard parentOperation(&mutableParent);
    const auto state = parent.m_lifetime;
    const auto content = parent.m_content;
    const auto generation = parent.m_contentGeneration;
    const auto extension = parent.m_compositeContentExtension;
    const auto current = [&]()
    {
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        return window && state->GetHost() == this &&
               FindSlot(window) == &parent && !m_shuttingDown &&
               !parent.m_deletePending && !parent.m_poisoned &&
               !parent.m_compositeRetiring &&
               !parent.m_contentTransactionInProgress &&
               parent.m_content == content &&
               parent.m_contentGeneration == generation &&
               parent.m_compositeContentExtension == extension;
    };
    if ( !mount || !content || !current() )
        return false;
    try
    {
        const bool ordinary = wxWinUISemanticPairIsValid(content, mount);
        if ( !current() )
            return false;
        if ( ordinary )
            return true;
        if ( !extension || !extension->isCurrent ||
             parent.m_compositeContentExtensionGeneration != generation ||
             parent.m_compositeContentExtensionValidating )
            return false;

        const auto root = parent.m_compositeContentExtensionRoot.get();
        if ( !root || extension->root.get() != root || !current() )
            return false;
        mutableParent.m_compositeContentExtensionValidating = true;
        const auto finish = wxMakeGuard([&mutableParent]()
        {
            mutableParent.m_compositeContentExtensionValidating = false;
        });
        if ( !extension->isCurrent() || !current() ||
             extension->root.get() != root || !current() )
            return false;
        const bool contained = wxWinUISemanticPairIsValid(root, mount);
        return contained && current() && extension->isCurrent() && current() &&
               extension->root.get() == root && current();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    catch ( ... )
    {
    }
    return false;
}

bool wxWinUITopLevelHost::IsCompositeMountCurrent(const wxWinUISlot& slot) const
{
    if ( !slot.m_isCompositeChild )
        return true;
    OperationGuard hostOperation(const_cast<wxWinUITopLevelHost *>(this));
    wxWinUISlot::OperationGuard slotOperation(const_cast<wxWinUISlot *>(&slot));
    const auto childState = slot.m_lifetime;
    const auto childGeneration = slot.m_contentGeneration;
    const auto state = slot.m_compositeOwner.lock();
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    const wxWinUISlot * const parent = window ? FindSlot(window) : nullptr;
    wxWinUISlot::OperationGuard parentOperation(const_cast<wxWinUISlot *>(parent));
    if ( !parent || !state || state->GetHost() != this ||
         parent->m_lifetime != state || parent->m_compositeRetiring ||
         parent->m_contentTransactionInProgress ||
         parent->m_contentGeneration != slot.m_compositeOwnerGeneration )
        return false;
    const auto generation = slot.m_compositeOwnerGeneration;
    const auto current = [&]()
    {
        wxWindow * const childWindow = childState ? childState->GetWindow() : nullptr;
        return childWindow && childState->GetHost() == this &&
               FindSlot(childWindow) == &slot && !slot.m_deletePending &&
               !slot.m_poisoned && slot.m_contentGeneration == childGeneration &&
               slot.m_compositeOwner.lock() == state &&
               slot.m_compositeOwnerGeneration == generation &&
               state->GetHost() == this && state->GetWindow() == window &&
               FindSlot(window) == parent && !parent->m_compositeRetiring &&
               !parent->m_contentTransactionInProgress &&
               parent->m_contentGeneration == generation && !m_shuttingDown;
    };
    try
    {
        if ( slot.m_directMount )
        {
            const auto mount = slot.m_directMount;
            const auto root = slot.m_directRoot;
            const auto owner = mount->owner.get();
            if ( !owner || !root || !mount->contains(root) || !current() )
                return false;
            // Both collection callbacks and an extension witness can retire
            // either participant. Revalidate the child after parent admission.
            return IsCompositeParentMountCurrent(*parent, owner) && current() &&
                   mount->contains(root) && current();
        }
        const auto mount = slot.m_compositeMount.get();
        return mount && mount.Content() == slot.m_container &&
               current() && IsCompositeParentMountCurrent(*parent, mount) &&
               current() && mount.Content() == slot.m_container && current();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
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
        physicalDisconnectGate,
    wxWindow *compositeOwner,
    const winrt::Microsoft::UI::Xaml::Controls::ContentControl& contentMount,
    const std::shared_ptr<wxWinUIDirectCompositeMount>& directMount)
{
    OperationGuard hostOperation(this);

    if ( !window || !m_root || m_shuttingDown ||
         m_compositeMigrationInProgress ||
         !wxWinUISemanticPairIsValid(element, semanticTarget) )
        return nullptr;

    wxWinUISlot * const compositeParent = compositeOwner
        ? FindSlot(compositeOwner) : nullptr;
    wxWinUISlot::OperationGuard parentOperation(compositeParent);
    const auto parentState = compositeParent ? compositeParent->m_lifetime : nullptr;
    const auto parentGeneration = compositeParent
        ? compositeParent->m_contentGeneration : 0;
    const wxWeakRef<wxWindow> registeringWindow(window);
    const auto parentPublicationIsCurrent = [&]()
    {
        return registeringWindow.get() == window && !window->IsBeingDeleted() &&
               compositeParent && parentState &&
               parentState->GetHost() == this &&
               parentState->GetWindow() == compositeOwner &&
               FindSlot(compositeOwner) == compositeParent &&
               compositeParent->m_contentGeneration == parentGeneration &&
               !compositeParent->m_compositeRetiring &&
               !compositeParent->m_contentTransactionInProgress &&
               compositeOwner != window && compositeOwner->IsDescendant(window);
    };
    const auto parentIsCurrent = [&]()
    {
        if ( !compositeOwner )
            return true;
        if ( (!contentMount && !directMount) || !parentPublicationIsCurrent() )
            return false;
        const auto mountOwner = directMount ? directMount->owner.get()
            : contentMount.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
        return mountOwner &&
            IsCompositeParentMountCurrent(*compositeParent, mountOwner) &&
            parentPublicationIsCurrent();
    };
    try
    {
        if ( !parentIsCurrent() )
            return nullptr;
        if ( compositeOwner )
        {
            wxWinUISlot * const existing = FindSlot(window);
            if ( directMount )
            {
                if ( !element.try_as<winrt::Microsoft::UI::Xaml::Controls::Control>() ||
                     !directMount->insert || !directMount->contains ||
                     !directMount->remove ||
                     (existing && (!existing->m_directRoot ||
                        existing->m_directMount != directMount ||
                        existing->m_compositeOwner.lock() != parentState ||
                        existing->m_content != element ||
                        existing->m_semanticTarget != semanticTarget)) )
                    return nullptr;
            }
            else
            {
                const auto occupant = contentMount.Content();
                if ( (occupant && (!existing || occupant != existing->m_container)) ||
                     (existing && (!existing->m_isCompositeChild ||
                        existing->m_directRoot ||
                        existing->m_compositeOwner.lock() != parentState ||
                        existing->m_compositeMount.get() != contentMount)) )
                    return nullptr;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("validate composite slot mount", e);
        return nullptr;
    }

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
        if ( existing->m_compositeRetiring )
            return nullptr;

        // A poisoned slot (its carrier went unreadable, its lifetime
        // invalidated) can never work again: dispose of it and fall through
        // to build a brand-new one, rather than SetContent()-ing a corpse
        // whose handlers are all bound to the dead lifetime.
        if ( existing->m_poisoned ||
                (existing->m_lifetime && !existing->m_lifetime->IsValid()) )
        {
            const bool wasComposite = existing->m_isCompositeChild;
            UnregisterSlot(window);
            if ( wasComposite )
                return nullptr;
            // fall through to fresh creation below
        }
        else
        {
            if ( !IsCompositeMountCurrent(*existing) )
                return nullptr;
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
                    const bool wasComposite = existing->m_isCompositeChild;
                    UnregisterSlot(window);
                    if ( wasComposite )
                        return nullptr;
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
                physicalDisconnectGate, compositeOwner, contentMount, directMount);
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
    if ( compositeOwner )
    {
        slot->m_isCompositeChild = true;
        slot->m_compositeOwner = parentState;
        slot->m_compositeOwnerGeneration = parentGeneration;
        if ( contentMount )
            slot->m_compositeMount = winrt::make_weak(contentMount);
        slot->m_directMount = directMount;
    }
    unsigned long long requestedContentGeneration = 0;
    bool requestedContentGenerationCaptured = false;
    bool requestedContentSuperseded = false;

    const auto rollback = [&]()
    {
        const auto current = m_slots.find(window);
        if ( current != m_slots.end() && current->second == slot )
        {
            // A publication callback may already have mounted native child
            // peers. Roll back through their causal fan-in rather than
            // tearing a parent carrier away from still-running children.
            if ( slot->m_directMount || HasCompositeChildren(*slot) )
            {
                UnregisterSlot(window);
                return;
            }
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

        if ( !parentIsCurrent() )
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
        if ( directMount )
        {
            slot->m_directRoot =
                element.as<winrt::Microsoft::UI::Xaml::Controls::Control>();
            slot->m_content = element;
            slot->m_semanticTarget = semanticTarget;
        }
        else
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
        if ( !directMount )
            slot->m_container.Visibility(
                winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

        // Slot positions are physical (already mirrored by wx for RTL), so
        // the root Canvas stays LTR; only the hosted element's own content
        // mirrors, per window.
        if ( !directMount && window->GetLayoutDirection() == wxLayout_RightToLeft )
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
        if ( slot->m_isCompositeChild )
            m_compositeSlots.insert(slot);
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
#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT

        if ( !observeRequestedContent() )
            return abandonInvalidFreshSlot();

        // Attach only after the slot is discoverable and its destroy handler
        // is bound: Append() can synchronously raise Loaded, and application
        // code is allowed to destroy/reparent the wx window from that event.
        if ( compositeOwner )
        {
            if ( directMount )
            {
                if ( !directMount->contains(element) &&
                     (!observeRequestedContent() ||
                      !directMount->insert(element)) )
                    return abandonInvalidFreshSlot();
                if ( !observeRequestedContent() )
                    return abandonInvalidFreshSlot();
            }
            else
            {
                if ( contentMount.Content() )
                    winrt::throw_hresult(E_INVALIDARG);
                contentMount.Content(slot->m_container);
            }
            if ( !IsCompositeMountCurrent(*slot) )
                return abandonInvalidFreshSlot();
            const auto state = slot->m_lifetime;
            slot->m_compositeLayoutUpdatedToken = slot->GetPresentationRoot().LayoutUpdated(
                [state](const winrt::Windows::Foundation::IInspectable&,
                        const winrt::Windows::Foundation::IInspectable&)
                {
                    wxWinUITopLevelHost * const host = state->GetHost();
                    wxWindow * const window = state->GetWindow();
                    wxWinUISlot * const current = host && window
                        ? host->FindSlot(window) : nullptr;
                    if ( !current || current->m_lifetime != state )
                        return;
                    OperationGuard hostOperation(host);
                    wxWinUISlot::OperationGuard slotOperation(current);
                    const auto container = current->GetPresentationRoot();
                    wxRect rect;
                    if ( !host->GetCompositeSlotRect(container, rect) ||
                         state->GetHost() != host || !state->GetWindow() )
                        return;
                    wxWinUISlot * const after = host->FindSlot(state->GetWindow());
                    if ( after && after->m_lifetime == state &&
                         after->m_rectPx != rect )
                        host->MarkDirty(state->GetWindow());
                });
        }
        else
        {
            // In the canvas standing for the parent of the window, which
            // BindSlotWindow() made sure exists.
            MountSlotContainer(*slot);
        }

        // Test the same arbitrary-callback boundary deterministically. The
        // hook is consumed before invocation so a nested migration cannot
        // recurse through the test mechanism unless it explicitly rearms it.
#ifdef WXWINUI_TEST_SUPPORT
        std::function<void (wxWindow *)> attachedHook =
            std::move(gs_testSlotAttachedHook);
        if ( attachedHook )
            attachedHook(window);
#endif // WXWINUI_TEST_SUPPORT
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

    const auto presentation = slot.GetPresentationRoot();
    if ( !presentation )
        return;

    // 2. Revoke every handler so the container stops referencing the
    //    lambdas (and through them the shared state).
    for ( auto& handler : slot.m_routedHandlers )
    {
        try
        {
            presentation.RemoveHandler(handler.first, handler.second);
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_slotHandlerRevokes;
#endif
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
            presentation.GotFocus(slot.m_gotFocusToken);
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_slotHandlerRevokes;
#endif
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    if ( slot.m_lostFocusToken )
    {
        try
        {
            presentation.LostFocus(slot.m_lostFocusToken);
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_slotHandlerRevokes;
#endif
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    slot.m_gotFocusToken = {};
    slot.m_lostFocusToken = {};
    if ( slot.m_compositeLayoutUpdatedToken )
    {
        try { presentation.LayoutUpdated(slot.m_compositeLayoutUpdatedToken); }
        catch ( const winrt::hresult_error& ) {}
        slot.m_compositeLayoutUpdatedToken = {};
    }
}

void wxWinUITopLevelHost::DisconnectSlot(wxWinUISlot& slot)
{
    bool physicallyDisconnected = true;
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

    if ( slot.m_directRoot )
    {
        // Do not clear the peer's own content: it belongs to the adapter.
        // Removing this exact collection item is the physical disconnect.
        const auto root = slot.m_directRoot;
        const auto mount = slot.m_directMount;
        try
        {
            if ( !mount ||
                 (mount->contains(root) && !mount->remove(root)) ||
                 mount->contains(root) )
                physicallyDisconnected = false;
        }
        catch ( const winrt::hresult_error& e )
        {
            physicallyDisconnected = false;
            wxWinUILogException("direct composite slot removal", e);
        }
        catch ( ... )
        {
            physicallyDisconnected = false;
        }
        slot.m_content = nullptr;
        slot.m_semanticTarget = nullptr;
    }
    else if ( slot.m_container )
    {
        // 3. Detach the hosted element (it belongs to the control, not to
        //    the slot) ...
        try
        {
            if ( slot.m_carrier )
                slot.m_carrier.Content(nullptr);
            slot.m_container.Children().Clear();
        }
        catch ( const winrt::hresult_error& e )
        {
            physicallyDisconnected = false;
            wxWinUILogException("slot content removal", e);
        }
        slot.m_content = nullptr;
        slot.m_semanticTarget = nullptr;

        // 4. ... and drop the container from the shared tree.
        if ( slot.m_isCompositeChild )
        {
            try
            {
                if ( const auto mount = slot.m_compositeMount.get();
                     mount && mount.Content() == slot.m_container )
                    mount.Content(nullptr);
            }
            catch ( const winrt::hresult_error& e )
            {
                physicallyDisconnected = false;
                wxWinUILogException("composite slot removal", e);
            }
        }
        else
        {
            try
            {
                UnmountSlotContainer(slot);
            }
            catch ( const winrt::hresult_error& e )
            {
                physicallyDisconnected = false;
                wxWinUILogException("slot removal", e);
            }
        }
    }

    gs_slotGrids.erase(&slot);

    // Never leave an island-level custom cursor owned by a slot whose
    // generation has just been retired.
    if ( ownedActiveCursor && !m_shuttingDown )
        RefreshPointerCursorAtLastPoint();

    // Keep physical membership visible across every callback above: closing
    // the parent from Unloaded must still account for this child's carrier.
    m_compositeSlots.erase(&slot);
    const auto parentRetirement = slot.m_compositeRetirement.lock();
    const auto receipt = slot.m_compositeRetirementReceipt;
    slot.m_compositeRetirement.reset();
    slot.m_compositeRetirementReceipt = 0;
    if ( parentRetirement && physicallyDisconnected )
        parentRetirement->ChildDisconnected(receipt);
}

void wxWinUITopLevelHost::SetSlotPhysicalDisconnectGate(
    wxWindow *window,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    OperationGuard hostOperation(this);
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_disconnectPending || slot->m_compositeRetiring )
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

    auto it = m_slots.find(window);
    if ( it == m_slots.end() )
        return;

    wxWinUISlot * const slot = it->second;
    if ( slot->m_compositeRetiring )
        return;
    const bool alreadyInOperation = slot->m_operationDepth != 0;
    wxWinUISlot::OperationGuard slotOperation(slot);
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> gate =
        requestedGate ? requestedGate : slot->m_disconnectGate;

    if ( HasCompositeChildren(*slot) )
    {
        // A direct child's receipt includes its own descendants. Record the
        // whole fan-in before the first child Close can reenter application
        // code, including children already removed from the logical table.
        const auto retirement = wxWinUICompositeSlotRetirementGate::Make(
            gate, m_root.DispatcherQueue());
        if ( !retirement )
            std::terminate();
        std::vector<std::pair<wxWinUISlot *,
                              std::shared_ptr<wxWinUISlotLifetime>>> children;
        try
        {
            for ( wxWinUISlot *child : m_compositeSlots )
            {
                if ( child->m_compositeOwner.lock() != slot->m_lifetime )
                    continue;
                const auto receipt = retirement->AddChild();
                if ( !receipt )
                    std::terminate();
                children.emplace_back(child, child->m_lifetime);
                child->m_compositeRetirement = retirement;
                child->m_compositeRetirementReceipt = receipt;
            }
        }
        catch ( ... )
        {
            // No child has been removed yet. Never continue after dropping
            // an unrecorded physical ownership debt.
            std::terminate();
        }
        gate = retirement;
        slot->m_disconnectGate = gate;
        slot->m_compositeDisconnectGate = retirement;
        slot->m_compositeRetiring = true;
        for ( const auto& child : children )
        {
            if ( m_shuttingDown )
                return;
            // A previous child's callback may already have disconnected this
            // sibling. The lifetime identity also rejects address reuse.
            if ( m_compositeSlots.count(child.first) &&
                 child.first->m_lifetime == child.second )
            {
                wxWindow * const childWindow = child.second->GetWindow();
                if ( childWindow && FindSlot(childWindow) == child.first )
                    UnregisterSlot(childWindow);
            }
        }
        if ( m_shuttingDown )
            return;
        it = m_slots.find(window);
        if ( it == m_slots.end() || it->second != slot )
            return;
    }

    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> deferredGate =
        gate && !gate->IsSatisfied() ? gate : nullptr;
    const bool mustDeferPhysicalDisconnect =
        alreadyInOperation || deferredGate;

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
#ifdef WXWINUI_TEST_SUPPORT
        if ( deferredGate )
        {
            gs_pendingPhysicalDisconnectGates.fetch_add(
                1, std::memory_order_relaxed);
        }
#endif
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
#ifdef WXWINUI_TEST_SUPPORT
        gs_pendingPhysicalDisconnectGates.fetch_sub(
            1, std::memory_order_relaxed);
#endif
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

    // Keep the authoritative collection visible during callback-rich physical
    // removal: a reentrant parent Close or whole-host Shutdown must find every
    // remaining child and its gate. Only the exact current item leaves it.
    std::vector<std::pair<wxWinUISlot *,
                          std::shared_ptr<wxWinUISlotLifetime>>> pending;
    for ( wxWinUISlot *slot : m_deferredDisconnectSlots )
        if ( slot )
            pending.emplace_back(slot, slot->m_lifetime);
    for ( const auto& entry : pending )
    {
        if ( m_shuttingDown )
            break;
        const auto found = std::find(m_deferredDisconnectSlots.begin(),
                                     m_deferredDisconnectSlots.end(),
                                     entry.first);
        if ( found == m_deferredDisconnectSlots.end() ||
             (*found)->m_lifetime != entry.second )
            continue;
        wxWinUISlot * const slot = *found;
        m_deferredDisconnectSlots.erase(found);
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

        const auto container = slot->GetPresentationRoot();
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
             slot->GetPresentationRoot() != container ||
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
            slot->GetPresentationRoot() == container &&
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

bool wxWinUITopLevelHost::CanMigrateCompositeSubtreeNow(
    wxWindow *subtreeRoot) const
{
    if ( !subtreeRoot || m_shuttingDown || m_compositeMigrationInProgress )
        return false;
    const auto includes = [subtreeRoot](wxWindow *window)
    {
        return window && (window == subtreeRoot ||
                          subtreeRoot->IsDescendant(window));
    };
    for ( const auto& entry : m_slots )
    {
        const wxWinUISlot& slot = *entry.second;
        if ( !includes(entry.first) )
            continue;
        if ( slot.m_deletePending || slot.m_poisoned ||
             slot.m_disconnectPending || slot.m_compositeRetiring ||
             slot.m_contentTransactionInProgress ||
             slot.m_compositeContentExtensionValidating ||
             slot.m_inputState.HasAnyActivePress() ||
             (slot.m_disconnectGate &&
              (!slot.m_disconnectGate->IsSatisfied() ||
               slot.m_disconnectGate->IsDegraded())) )
            return false;
        if ( slot.m_isCompositeChild )
        {
            const auto parent = slot.m_compositeOwner.lock();
            if ( !parent || parent->GetHost() != this ||
                 !includes(parent->GetWindow()) )
                return false;
        }
    }
    // A logically removed child still owns its mount until its physical
    // receipt arrives. Never carry that debt to another XamlRoot.
    for ( const auto child : m_compositeSlots )
    {
        const auto parent = child->m_compositeOwner.lock();
        if ( parent && includes(parent->GetWindow()) &&
             (!child->m_lifetime ||
              !includes(child->m_lifetime->GetWindow()) ||
              child->m_disconnectPending || child->m_deletePending ||
              FindSlot(child->m_lifetime->GetWindow()) != child) )
            return false;
    }
    return !m_nativeResize;
}

bool wxWinUITopLevelHost::MigrateCompositeSubtree(wxWindow *subtreeRoot)
{
    OperationGuard sourceOperation(this);
    if ( !CanMigrateCompositeSubtreeNow(subtreeRoot) )
        return false;

    const wxWeakRef<wxWindow> subtree(subtreeRoot);
    const auto sourceState = m_hostLifetime;
    wxWinUITopLevelHost * const target = ForWindow(subtreeRoot, true);
    if ( !target || m_shuttingDown || !subtree.get() )
        return false;
    if ( target == this )
        return true;
    OperationGuard targetOperation(target);
    if ( target->m_shuttingDown || !target->m_root ||
         target->m_compositeMigrationInProgress ||
         !CanMigrateCompositeSubtreeNow(subtree.get()) )
        return false;
    const auto targetState = target->m_hostLifetime;

    struct Entry
    {
        wxWinUISlot *slot;
        std::shared_ptr<wxWinUISlotLifetime> state;
        wxWeakRef<wxWindow> parent;
        WXHWND hwnd;
        unsigned long long hwndGeneration;
        unsigned long long contentGeneration;
        unsigned sourceIndex = 0;
        bool detached = false;
        bool attached = false;
        std::unique_ptr<wxWinUISlot::OperationGuard> operation;
    };
    std::vector<Entry> entries;
    for ( const auto& entry : m_slots )
    {
        wxWindow * const window = entry.first;
        if ( window != subtreeRoot && !subtreeRoot->IsDescendant(window) )
            continue;
        wxWinUISlot * const slot = entry.second;
        if ( target->FindSlot(window) )
            return false;
        entries.push_back({slot, slot->m_lifetime, window->GetParent(),
                           window->GetHWND(),
                           wxWinUIMSWGetHwndGeneration(window, window->GetHWND()),
                           slot->m_contentGeneration, 0, false, false,
                           std::make_unique<wxWinUISlot::OperationGuard>(slot)});
    }
    if ( entries.empty() )
        return true;

    bool transferred = false;
    const auto current = [&](wxWinUITopLevelHost *owner)
    {
        wxWindow * const root = subtree.get();
        if ( !root || root->IsBeingDeleted() ||
             (owner == this &&
              (!sourceState || sourceState->GetHost() != this || m_shuttingDown)) ||
             !targetState || targetState->GetHost() != target ||
             target->m_shuttingDown ||
             wxGetTopLevelParent(root) != target->m_tlw )
            return false;
        for ( const auto& entry : entries )
        {
            wxWindow * const window = entry.state->GetWindow();
            if ( !window || entry.state->GetHost() != owner ||
                 owner->FindSlot(window) != entry.slot ||
                 entry.slot->m_deletePending || window->IsBeingDeleted() ||
                 entry.slot->m_contentGeneration != entry.contentGeneration ||
                 window->GetParent() != entry.parent.get() ||
                 window->GetHWND() != entry.hwnd ||
                 wxWinUIMSWGetHwndGeneration(window, entry.hwnd) !=
                     entry.hwndGeneration ||
                 wxGetTopLevelParent(window) != target->m_tlw )
                return false;
        }
        return true;
    };
    // Validate mounts before locking content sync. This invokes application
    // collection adapters, so each result is followed by a cohort identity
    // check; no child is detached by the transport.
    for ( auto& entry : entries )
    {
        if ( !IsCompositeMountCurrent(*entry.slot) || !current(this) )
            return false;
        if ( !entry.slot->m_isCompositeChild &&
             (!entry.slot->m_container ||
              !entry.slot->m_mountCanvas ||
              !entry.slot->m_mountCanvas.Children().IndexOf(
                  entry.slot->m_container, entry.sourceIndex)) )
            return false;
    }
    if ( !current(this) || !CanMigrateCompositeSubtreeNow(subtree.get()) )
        return false;

    // Keep the original focus ticket: it names the focused descendant, not
    // necessarily the root wx window passed to Reparent(). The MSW boundary
    // has already parked that descendant's focus before SetParent.
    std::shared_ptr<FocusMigration> focusMigration;
    for ( const auto& entry : entries )
    {
        const auto pending = ms_focusMigrations.find(entry.state->GetWindow());
        if ( pending != ms_focusMigrations.end() && pending->second &&
             !pending->second->committed &&
             pending->second->sourceHost == sourceState &&
             pending->second->sourceSlot == entry.state &&
             m_focusMigration == pending->second )
            focusMigration = pending->second;
    }
    if ( target->m_focusMigration &&
         target->m_focusMigration != focusMigration &&
         !target->m_focusMigration->committed )
        return false;

    m_compositeMigrationInProgress = true;
    target->m_compositeMigrationInProgress = true;
    for ( const auto& entry : entries )
    {
        entry.slot->m_contentTransactionInProgress = true;
        if ( ++entry.slot->m_contentTransactionEpoch == 0 )
            ++entry.slot->m_contentTransactionEpoch;
    }
    bool unlocked = false;
    const auto unlock = [&]()
    {
        if ( unlocked )
            return;
        unlocked = true;
        m_compositeMigrationInProgress = false;
        target->m_compositeMigrationInProgress = false;
        for ( const auto& entry : entries )
        {
            wxWinUISlot& slot = *entry.slot;
            slot.m_contentTransactionInProgress = false;
            // Content identity did not change. Loaded callbacks raised while
            // the root was moving still concern this exact generation.
            if ( slot.m_contentLoadedDuringTransaction )
                slot.m_contentLoaded = true;
            slot.m_contentLoadedDuringTransaction = false;
            slot.m_contentLoadedTransactionEpoch = 0;
            slot.m_contentLoadedTransactionGeneration = 0;
            if ( ++slot.m_contentTransactionEpoch == 0 )
                ++slot.m_contentTransactionEpoch;
        }
    };
    wxScopeGuard unlockGuard = wxMakeGuard(unlock);
    wxUnusedVar(unlockGuard);

    // Node extraction moves the already allocated registry entries. There
    // must be no WinRT/user boundary between maps and lifetime publication.
    const auto transferRegistry = [&](wxWinUITopLevelHost *from,
                                      wxWinUITopLevelHost *to)
    {
        for ( const auto& entry : entries )
        {
            wxWindow * const window = entry.state->GetWindow();
            if ( !window || entry.state->GetHost() != from ||
                 from->FindSlot(window) != entry.slot )
                continue; // destruction owns a retired identity
            from->UnbindSlotWindow(*entry.slot);
            from->ForgetSlotPointerOwners(entry.state);
            from->m_dirty.erase(window);
            to->m_slots.insert(from->m_slots.extract(window));
            if ( entry.slot->m_isCompositeChild )
                to->m_compositeSlots.insert(
                    from->m_compositeSlots.extract(entry.slot));
            gs_slotOwners.find(window)->second = to;
            entry.state->RebindHost(to);
        }
        for ( const auto& entry : entries )
        {
            if ( entry.state->GetHost() == to &&
                 entry.state->GetWindow() &&
                 to->FindSlot(entry.state->GetWindow()) == entry.slot )
                to->BindSlotWindow(*entry.slot);
        }
    };
    const auto destinationHasAuthoritativeRetirement = [&]()
    {
        if ( !targetState || targetState->GetHost() != target ||
             target->m_shuttingDown )
            return false;
        bool retired = false;
        for ( const auto& entry : entries )
        {
            wxWindow * const window = entry.state->GetWindow();
            if ( window && entry.state->GetHost() == target &&
                 target->FindSlot(window) == entry.slot &&
                 !entry.slot->m_deletePending &&
                 entry.slot->m_contentGeneration == entry.contentGeneration &&
                 window->GetParent() == entry.parent.get() &&
                 window->GetHWND() == entry.hwnd &&
                 wxWinUIMSWGetHwndGeneration(window, entry.hwnd) ==
                     entry.hwndGeneration &&
                 wxGetTopLevelParent(window) == target->m_tlw )
                continue;
            if ( !entry.state->IsValid() && entry.slot->m_deletePending &&
                 entry.slot->m_disconnectPending &&
                 entry.slot->m_deferredDisconnectHost == target )
            {
                retired = true;
                continue;
            }
            return false;
        }
        return retired;
    };
    const auto restore = [&]()
    {
        // Only independent Canvas roots cross XamlRoot. Direct Items and
        // ContentControl child mounts stay intact even during rollback.
        for ( auto& entry : entries )
        {
            if ( !entry.attached || entry.slot->m_isCompositeChild ||
                 entry.state->GetHost() != target ||
                 !entry.state->GetWindow() || entry.slot->m_deletePending ||
                 target->FindSlot(entry.state->GetWindow()) != entry.slot )
                continue;
            target->UnmountSlotContainer(*entry.slot);
            entry.attached = false;
        }
        if ( transferred && sourceState->GetHost() == this && !m_shuttingDown )
        {
            transferRegistry(target, this);
            transferred = false;
        }
        if ( focusMigration && !focusMigration->committed )
        {
            if ( target->m_focusMigration == focusMigration )
                target->m_focusMigration.reset();
            focusMigration->destinationHost = sourceState;
            focusMigration->destinationSlot = focusMigration->sourceSlot;
            focusMigration->destinationContentGeneration =
                focusMigration->sourceContentGeneration;
        }
        // Restore original sibling order, not pointer-map order.
        std::sort(entries.begin(), entries.end(),
                  [](const Entry& a, const Entry& b)
                  { return a.sourceIndex < b.sourceIndex; });
        for ( auto& entry : entries )
        {
            if ( !entry.detached || entry.slot->m_isCompositeChild ||
                 entry.state->GetHost() != this ||
                 !entry.state->GetWindow() || m_shuttingDown )
                continue;
            // Back in the canvas of its parent. The canvas may be a new one,
            // the registry round trip above having released and bound the
            // ancestors again; the order among siblings is the z-order
            // pass's to restore.
            MountSlotContainer(*entry.slot);
            entry.detached = false;
        }
    };

    bool succeeded = false;
    try
    {
        for ( auto& entry : entries )
        {
            if ( entry.slot->m_isCompositeChild )
                continue;
            if ( !entry.slot->m_mountCanvas )
                winrt::throw_hresult(E_UNEXPECTED);
            entry.detached = true;
            UnmountSlotContainer(*entry.slot);
            if ( !current(this) )
                winrt::throw_hresult(E_ABORT);
        }
        if ( focusMigration )
        {
            focusMigration->destinationHost = targetState;
            focusMigration->destinationSlot = focusMigration->sourceSlot;
            focusMigration->destinationContentGeneration =
                focusMigration->sourceContentGeneration;
            target->m_focusMigration = focusMigration;
        }
        transferred = true;
        transferRegistry(this, target);
        if ( !current(target) )
            winrt::throw_hresult(E_ABORT);
        for ( auto& entry : entries )
        {
            if ( entry.slot->m_isCompositeChild )
                continue;
            // Set before Append: a synchronous callback can throw after the
            // collection has already accepted this element.
            entry.attached = true;
            target->MountSlotContainer(*entry.slot);
#ifdef WXWINUI_TEST_SUPPORT
            auto attachedHook = std::move(gs_testSlotAttachedHook);
            if ( attachedHook )
                attachedHook(entry.state->GetWindow());
#endif
            if ( !current(target) &&
                 !destinationHasAuthoritativeRetirement() )
                winrt::throw_hresult(E_ABORT);
        }
        succeeded = true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("composite subtree migration", e);
    }
    catch ( ... )
    {
        // This boundary reports failure to wxWindowMSW::Reparent(), which
        // owns the matching wx/USER32 rollback. Rethrowing here would bypass
        // that half of the transaction. Retired destination graphs need the
        // same causal settlement as an HRESULT failure, not an early detach.
        wxLogDebug("wxWinUI: composite subtree migration callback raised "
                     "a C++ exception; settling the reparent transaction");
    }
    if ( !succeeded )
    {
        if ( transferred )
        {
            if ( destinationHasAuthoritativeRetirement() ||
                 ((!sourceState || sourceState->GetHost() != this) &&
                  current(target)) )
            {
                // B's retirement may already own a popup debt, or A may no
                // longer exist. Finish ALL independent roots before accepting
                // the newer topology. A Close from the first root's Loaded
                // must not strand later roots, including a logically retired
                // parent whose physical graph still awaits a child receipt.
                try
                {
                    for ( auto& entry : entries )
                    {
                        if ( entry.slot->m_isCompositeChild )
                            continue;
                        if ( !entry.slot->m_mountCanvas )
                        {
                            entry.attached = true;
                            target->MountSlotContainer(*entry.slot);
                        }
                        if ( !current(target) &&
                             !destinationHasAuthoritativeRetirement() )
                            winrt::throw_hresult(E_ABORT);
                    }
                    succeeded = true;
                }
                catch ( ... )
                {
                    // No rollback can relocate B's outstanding physical debt
                    // or recreate a destroyed source host. A native failure
                    // of this terminal publication is not partial success.
                    wxLogDebug("wxWinUI: failed to finish the authoritative "
                               "destination composite graph");
                    std::terminate();
                }
            }
        }
    }
    if ( !succeeded )
    {
        try { restore(); }
        catch ( ... )
        {
            // Keeping a graph with no authoritative native owner is not a
            // recoverable partial success.
            std::terminate();
        }
    }
    unlock();
    InvalidateStructure();
    target->InvalidateStructure();
    MarkAllDirty();
    target->MarkAllDirty();

    if ( succeeded && focusMigration && !focusMigration->committed )
    {
        wxWindow * const focused = focusMigration->window.get();
        if ( focused && target->FindSlot(focused) &&
             HasFocusMigrationNativeAuthority(focusMigration, target) )
        {
            const WXHWND authority = reinterpret_cast<WXHWND>(::GetFocus());
            focusMigration->transportNativeAuthority = authority;
            focusMigration->transportNativeAuthorityGeneration =
                wxWinUIMSWGetNativeHwndGeneration(authority);
            focusMigration->preparedNativeReparentBoundaryActive = false;
            focusMigration->preparedReparentId = 0;
            unsigned long long continuationGeneration = 0;
            if ( focusMigration->sourceHost == targetState )
            {
                // Source shutdown handed this exact arbiter to B. Reserve
                // its new intent before the same-host continuation commits.
                if ( ++target->m_focusIntentGeneration == 0 )
                    ++target->m_focusIntentGeneration;
                target->ClearDeferredFocusRequest();
                continuationGeneration = target->m_focusIntentGeneration;
                focusMigration->sourceFocusIntentGeneration =
                    continuationGeneration;
            }
            target->FocusSlotImpl(focused, FocusOrigin::MigrationContinuation,
                                 focusMigration->previous.get(),
                                 continuationGeneration, true);
        }
        else
            AbortFocusMigration(focusMigration, true);
    }
    return succeeded;
}

bool wxWinUITopLevelHost::MigrateSlotToCurrentTLW(wxWindow *window)
{
    OperationGuard hostOperation(this);

    if ( m_compositeMigrationInProgress )
        return false;

    wxWinUISlot * const oldSlot = FindSlot(window);
    if ( !oldSlot )
        return false;
    if ( oldSlot->m_isCompositeChild || oldSlot->m_compositeContentExtension ||
         HasCompositeChildren(*oldSlot) )
    {
        wxWinUISlot *root = oldSlot;
        while ( root->m_isCompositeChild )
        {
            const auto parent = root->m_compositeOwner.lock();
            root = parent && parent->GetHost() == this
                ? FindSlot(parent->GetWindow()) : nullptr;
            if ( !root )
                return false;
        }
        return MigrateCompositeSubtree(root->m_window);
    }
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
                oldSlot->GetPresentationRoot(), GetXamlRoot());
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
                        oldSlot->GetPresentationRoot(), GetXamlRoot());
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
                wxLogDebug("wxWinUI: failed to restore a slot after a "
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
            wxLogDebug(
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

bool wxWinUITopLevelHost::HandleReparent(wxWindow *subtreeRoot)
{
    OperationGuard operation(this);

    if ( !subtreeRoot || m_shuttingDown )
        return true;
    if ( m_compositeMigrationInProgress )
        return false;

    if ( wxGetTopLevelParent(subtreeRoot) != m_tlw )
    {
        for ( const auto& entry : m_slots )
        {
            if ( (entry.first == subtreeRoot ||
                  subtreeRoot->IsDescendant(entry.first)) &&
                 (entry.second->m_isCompositeChild ||
                  entry.second->m_compositeContentExtension ||
                  HasCompositeChildren(*entry.second)) )
                return MigrateCompositeSubtree(subtreeRoot);
        }
    }

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
        // focus ownership intact. Only the ancestor chain changed, and the
        // container follows the window to the canvas of its new parent. The
        // new chain is bound before the old one is released, so that the
        // nodes common to both are kept rather than torn down and rebuilt.
        std::vector<wxWindow *> formerAncestors;
        formerAncestors.swap(slot->m_ancestors);
        BindSlotAncestors(*slot);
        for ( wxWindow *ancestor : formerAncestors )
            ReleaseAncestorRef(ancestor);
        try
        {
            MountSlotContainer(*slot);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("slot remount after reparent", e);
        }
        m_zOrderDirty = true;
        InvalidateStructure();
        MarkDirty(window);
    }
    // Ordinary slots retain their historical deferred retry contract.
    return true;
}


bool wxWinUITopLevelHost::MirrorNativeCursor(
    const wxWinUINativeTarget& targetIdentity,
    LRESULT hitTest,
    unsigned long long expectedEpoch)
{
    // XAML picks a cursor of its own whenever it processes the pointer over
    // one of its elements, and the root canvas which stands for all native
    // space is one of them: over a window showing anything but the arrow,
    // what this host set is replaced at every move. Both shortcuts below rely
    // on the island still showing it, so look before taking them.
    ObserveXamlPointerCursor();

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
                         !wxWinUISetAsNativeMirror([&]
                            {
                                return SetIslandPointerCursor(
                                    cursor, sourceHandle);
                            }) ||
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
         !wxWinUISetAsNativeMirror([&]
            {
                return SetIslandPointerCursor(
                    cursor, reinterpret_cast<WXHCURSOR>(nativeCursor));
            }) ||
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

namespace
{

// A null InputCursor on an InputPointerSource is not "no override": like on
// its CoreWindow predecessor it hides the pointer, which then stays hidden
// until something sets another cursor. Over a window disabled by a modal
// dialog nothing ever does, so the pointer vanished there after leaving the
// dialog. Whenever the port has no cursor of its own to show, it therefore
// sets the standard arrow instead of nothing.
winrt::Microsoft::UI::Input::InputCursor wxWinUIDefaultIslandCursor()
{
    return winrt::Microsoft::UI::Input::InputSystemCursor::Create(
        winrt::Microsoft::UI::Input::InputSystemCursorShape::Arrow);
}

// Whether two cursors show the same thing: the same object, or two system
// cursors of the same shape. XAML makes its own object for the arrow it sets
// over an element without a cursor, which is no change when the island was
// already showing the arrow.
bool wxWinUISameIslandCursor(
    const winrt::Microsoft::UI::Input::InputCursor& first,
    const winrt::Microsoft::UI::Input::InputCursor& second)
{
    namespace MUI = winrt::Microsoft::UI::Input;

    if ( first == second )
        return true;
    if ( !first || !second )
        return false;

    const auto firstSystem = first.try_as<MUI::InputSystemCursor>();
    const auto secondSystem = second.try_as<MUI::InputSystemCursor>();
    return firstSystem && secondSystem &&
           firstSystem.CursorShape() == secondSystem.CursorShape();
}

} // anonymous namespace

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
            // XAML chooses a cursor by itself whenever it sees fit -- on a
            // pointer event, when the cursor of any element changes, and
            // with the pointer at rest too -- from the element under the
            // pointer and its ancestors. Over native space that element is
            // the root canvas, which had no cursor: XAML put the arrow back
            // over a splitter sash, and the cursor blinked between the two.
            // While the pointer is over a native window, the root canvas is
            // given the cursor of that window as well, so that what XAML
            // chooses is what is set here. (Over a control, its own element
            // carries its cursor.)
            //
            // The canvas comes first: XAML answers this change at once with
            // the cursor it believes is due, and the island is to have the
            // last word.
            const auto applied =
                cursor ? cursor : wxWinUIDefaultIslandCursor();
            const bool mirrorOnRoot =
                gs_mirroringNativeCursor ||
                m_activeCursorSurface == ActiveCursorSurface::Native;
            const auto rootCanvas = wxWinUIGetRootCanvas(this);
            if ( mirrorOnRoot )
            {
                if ( rootCanvas && rootCanvas->SetPointerCursor(applied) )
                    gs_rootCursorMirrors.insert(this);
            }
            else if ( gs_rootCursorMirrors.erase(this) ||
                      m_rootPointerCursorAuthoritative )
            {
                const bool cleared =
                    rootCanvas && rootCanvas->SetPointerCursor(nullptr);
                if ( !cleared && m_rootPointerCursorAuthoritative )
                    return false;
            }
            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown ||
                 m_cursorSelectionGeneration != selectionGeneration )
            {
                return false;
            }
            m_rootPointerCursorAuthoritative = false;
#ifdef WXWINUI_TEST_SUPPORT
            if ( gs_failInputPointerSourceSetAt &&
                 --gs_failInputPointerSourceSetAt == 0 )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"wxWinUI test: injected InputPointerSource failure");
            }
#endif // WXWINUI_TEST_SUPPORT
            source.Cursor(applied);
            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown ||
                 m_cursorSelectionGeneration != selectionGeneration )
            {
                return false;
            }
            m_inputPointerSource = source;
            m_lastIslandPointerCursor = cursor;
            m_lastIslandPointerHandle = sourceHandle;
            m_islandPointerCursorSet = applied;
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
                attemptedSource.Cursor(wxWinUIDefaultIslandCursor());
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
            source.Cursor(wxWinUIDefaultIslandCursor());
            if ( !hostState || hostState->GetHost() != this ||
                 m_shuttingDown ||
                 m_cursorSelectionGeneration != selectionGeneration )
            {
                return false;
            }
        }

        // The root canvas may carry the cursor of a native window too, see
        // SetIslandPointerCursor(): it has none any more. Only when it was
        // the sole carrier does a failure here leave a wrong cursor showing.
        const auto root = gs_rootCanvases.find(this);
        const bool mirrored = gs_rootCursorMirrors.erase(this) != 0;
        if ( mirrored || m_rootPointerCursorAuthoritative )
        {
            const bool rootCleared =
                root != gs_rootCanvases.end() &&
                root->second->SetPointerCursor(nullptr);
            if ( m_rootPointerCursorAuthoritative && !rootCleared )
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
    m_islandPointerCursorSet = nullptr;
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
             !wxWinUISameIslandCursor(observed, m_islandPointerCursorSet) )
        {
            // XAML selected the cursor for the new OriginalSource before
            // raising the routed event. Yield ownership instead of clearing
            // its TextBox I-beam/template-part cursor below.
            if ( wxWinUIInputLogEnabled() )
            {
                wxWinUIInputLog("cursor: the island no longer shows ours "
                                "(handle=%p)",
                                static_cast<void *>(
                                    m_lastIslandPointerHandle));
            }
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

void wxWinUITopLevelHost::ReassertIslandPointerCursor()
{
    if ( m_shuttingDown || !m_inputPointerSourceAuthoritative ||
         !m_islandPointerCursorApplied )
    {
        return;
    }

    // Keep what is being set again alive: observing can drop the last
    // reference the host holds.
    const auto cursor = m_lastIslandPointerCursor;
    const WXHCURSOR handle = m_lastIslandPointerHandle;

    ObserveXamlPointerCursor();
    if ( !m_inputPointerSourceAuthoritative && !m_shuttingDown )
        (void)wxWinUISetAsNativeMirror([&]
            {
                return SetIslandPointerCursor(cursor, handle);
            });
}

void wxWinUITopLevelHost::ApplyXamlPointerCursor(
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    bool emitSetCursorEvent,
    unsigned long long expectedEpoch)
{
    ApplyXamlPointerCursorForSlot(FindXamlPointerCursorSlot(args),
                                  emitSetCursorEvent, expectedEpoch);
}

std::shared_ptr<wxWinUISlotLifetime>
wxWinUITopLevelHost::FindXamlPointerCursorSlot(
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args) const
{
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
                if ( slot && slot->GetPresentationRoot() &&
                     node == slot->GetPresentationRoot() )
                {
                    return slot->m_lifetime;
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

    return nullptr;
}

void wxWinUITopLevelHost::ApplyXamlPointerCursorForSlot(
    const std::shared_ptr<wxWinUISlotLifetime>& cursorSlot,
    bool emitSetCursorEvent,
    unsigned long long expectedEpoch)
{
    const unsigned long long cursorPolicyGeneration =
        gs_cursorPolicyGeneration;
    const unsigned long long cursorSelectionGeneration =
        m_cursorSelectionGeneration;

    if ( cursorSlot )
    {
        if ( ApplySlotPointerCursor(
                 cursorSlot, emitSetCursorEvent, expectedEpoch,
                 !emitSetCursorEvent) )
            return;
        if ( m_cursorSelectionGeneration != cursorSelectionGeneration )
        {
            // Slot teardown or another callback selected the newly exposed
            // surface while this route was unwinding. Preserve that nested
            // winner.
            return;
        }
        if ( !emitSetCursorEvent )
        {
            // A terminal/capture resync never replaces a valid Move verdict
            // with generic policy just because its WinRT re-assertion
            // transiently failed.
            return;
        }
        if ( gs_cursorPolicyGeneration != cursorPolicyGeneration ||
             (expectedEpoch && m_inputTransitionEpoch != expectedEpoch) )
        {
            return;
        }
        ApplyGenericPointerCursor();
        return;
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
            // The full stack includes the canvases standing for windows,
            // which show nothing: only what is in them counts.
            if ( IsStructuralElement(element) )
                continue;

            DependencyObject node = element;
            while ( node )
            {
                for ( const auto& item : m_slots )
                {
                    wxWinUISlot * const slot = item.second;
                    if ( slot && slot->GetPresentationRoot() &&
                         node == slot->GetPresentationRoot() )
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
    }

    // From the top down: the canvas standing for a window is mounted in the
    // one standing for its parent, which has to exist first.
    for ( auto it = slot.m_ancestors.rbegin();
          it != slot.m_ancestors.rend();
          ++it )
    {
        AddAncestorRef(*it);
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
    // A window which is being destroyed has sent its wxEVT_DESTROY, which is
    // what takes its canvas away, and will not send another: one made for it
    // now, while its children go and what they show is put in order one last
    // time, would outlive it. Without a canvas of its own what is below it
    // goes in the nearest one above, see GetChildrenCanvas().
    if ( ancestor->IsBeingDeleted() && m_nodes.find(ancestor) == m_nodes.end() )
        return;

    WindowNode& node = m_nodes[ancestor];
    if ( ++node.refs > 1 )
    {
        // A window reparented inside its TLW keeps its node, and the node
        // follows it.
        try
        {
            MountNode(ancestor, node);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("window node remount", e);
        }
        return;
    }

    ancestor->Bind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;

        // No background: a canvas standing for a window shows nothing and
        // takes no input by itself, the pointer goes through it.
        node.canvas = Controls::Canvas();
        node.clip = Media::RectangleGeometry();
        node.canvas.Visibility(Visibility::Collapsed);
        MountNode(ancestor, node);
    }
    catch ( const winrt::hresult_error& e )
    {
        // Without its canvas the children of this window go to the nearest
        // canvas above, see GetChildrenCanvas().
        wxWinUILogException("window node creation", e);
        node.canvas = nullptr;
        node.mountCanvas = nullptr;
    }

    m_zOrderDirty = true;
    SyncNode(ancestor);
}

void wxWinUITopLevelHost::ReleaseAncestorRef(wxWindow *ancestor)
{
    const auto it = m_nodes.find(ancestor);
    if ( it == m_nodes.end() )
        return;     // already dropped by OnAncestorDestroy

    if ( --it->second.refs > 0 )
        return;

    UnmountNode(it->second);
    m_nodes.erase(it);

    ancestor->Unbind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);
}

void wxWinUITopLevelHost::OnAncestorDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    wxWindow * const window = event.GetWindow();
    m_inputNodes.erase(window);

    // Before the canvas of the window: emptied of its parts, it can leave
    // the tree for good.
    if ( window )
    {
        for ( const WindowPartKind kind : { WindowPartKind::PaintSurface,
                                            WindowPartKind::SplitterSash,
                                            WindowPartKind::Caret,
                                            WindowPartKind::Frame,
                                            WindowPartKind::HorizontalScrollBar,
                                            WindowPartKind::VerticalScrollBar,
                                            WindowPartKind::Ground,
                                            WindowPartKind::Smoke } )
        {
            RemoveWindowPart(window, kind, false);
        }
    }

    const auto it = window ? m_nodes.find(window) : m_nodes.end();
    if ( it != m_nodes.end() )
    {
        // The window is going away, no point unbinding from it; the slots
        // that referenced it are being destroyed too and their release will
        // find the entry already gone. Their containers leave the tree with
        // this canvas, and are taken out of it one by one afterwards.
        UnmountNode(it->second);
        m_nodes.erase(it);
    }
}

// ----------------------------------------------------------------------------
// window parts: what XAML draws in the canvas of a window, in its place
// ----------------------------------------------------------------------------

std::vector<wxWindow *>
wxWinUITopLevelHost::GetNodeChain(wxWindow *window) const
{
    std::vector<wxWindow *> chain;
    for ( wxWindow *p = window; p && p != m_tlw; p = p->GetParent() )
        chain.push_back(p);
    return chain;
}

void wxWinUITopLevelHost::SetWindowPart(
    wxWindow *window,
    WindowPartKind kind,
    const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    OperationGuard operation(this);
    if ( !window )
        return;

    // The window whose canvas the part goes in: its own, but for its border.
    wxWindow * const anchor =
        kind == WindowPartKind::Frame ? window->GetParent() : window;

    const auto existing = m_parts.find({ window, kind });
    if ( existing != m_parts.end() )
    {
        // Still the same element, under the same ancestors: nothing to do.
        // A window given another parent since takes its part along.
        if ( existing->second.element == element &&
             existing->second.chain == GetNodeChain(anchor) )
        {
            return;
        }
        RemoveWindowPart(window, kind, true);
    }

    if ( !element || m_shuttingDown || !m_root ||
         wxGetTopLevelParent(window) != m_tlw )
    {
        return;
    }

    WindowPart part;
    part.element = element;
    part.chain = GetNodeChain(anchor);

    // From the top down, see BindSlotAncestors().
    for ( auto it = part.chain.rbegin(); it != part.chain.rend(); ++it )
        AddAncestorRef(*it);

    try
    {
        using winrt::Microsoft::UI::Xaml::Controls::Canvas;

        part.canvas = GetChildrenCanvas(anchor);

        // The order of the containers of the children and of their canvases
        // starts at zero and cannot reach the million WinUI allows.
        Canvas::SetZIndex(
            element,
            kind == WindowPartKind::Ground ? -3
                : kind == WindowPartKind::PaintSurface ? -2
                : kind == WindowPartKind::SplitterSash ||
                  kind == WindowPartKind::Caret ||
                  kind == WindowPartKind::Frame ? -1
                : 1000000);
        part.canvas.Children().Append(element);
        if ( part.canvas == m_root )
            ++m_rootMounts;
        if ( IsInteractivePart(kind) )
            ++m_interactiveParts;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("window part mount", e);
        for ( wxWindow *w : part.chain )
            ReleaseAncestorRef(w);
        return;
    }

    m_parts.emplace(std::make_pair(window, kind), std::move(part));
}

void wxWinUITopLevelHost::SetWindowFrame(wxWindow *window,
                                         const wxRect *screenRect)
{
    OperationGuard operation(this);
    if ( !window )
        return;

    if ( !screenRect )
    {
        SetWindowPart(window, WindowPartKind::Frame, nullptr);
        return;
    }

    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;
        using MUX::Controls::Canvas;

        auto frame = GetWindowPart(window, WindowPartKind::Frame)
                        .try_as<MUX::Controls::Border>();
        if ( !frame )
        {
            // The line and the corners of a control, whatever the width
            // Win32 keeps for the border: one pixel at its outer edge.
            frame = wxWinUICreateThemeBrushBorder(
                "ControlStrokeColorDefaultBrush",
                wxWinUIThemeBrushProperty::BorderBrush);
            if ( !frame )
                return;
            frame.IsHitTestVisible(false);
            frame.BorderThickness(MUX::ThicknessHelper::FromUniformLength(1));
            frame.CornerRadius(MUX::CornerRadiusHelper::FromUniformRadius(4));
        }

        // Mounts it, or mounts it again under the new parent of the window.
        SetWindowPart(window, WindowPartKind::Frame, frame);

        const double scale = GetScale();
        if ( scale <= 0 || m_shuttingDown )
            return;

        const wxPoint origin = GetChildrenOriginOnScreen(window->GetParent());
        Canvas::SetLeft(frame, (screenRect->x - origin.x) / scale);
        Canvas::SetTop(frame, (screenRect->y - origin.y) / scale);
        frame.Width(screenRect->width / scale);
        frame.Height(screenRect->height / scale);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("window frame", e);
    }
}

winrt::Microsoft::UI::Xaml::UIElement
wxWinUITopLevelHost::GetWindowPart(wxWindow *window,
                                   WindowPartKind kind) const
{
    const auto it = m_parts.find({ window, kind });
    return it == m_parts.end() ? nullptr : it->second.element;
}

void wxWinUITopLevelHost::RemoveWindowPart(wxWindow *window,
                                           WindowPartKind kind,
                                           bool windowIsAlive)
{
    const auto it = m_parts.find({ window, kind });
    if ( it == m_parts.end() )
        return;

    const WindowPart part = std::move(it->second);
    m_parts.erase(it);
    if ( IsInteractivePart(kind) && m_interactiveParts )
        --m_interactiveParts;

    try
    {
        std::uint32_t index = 0;
        if ( part.canvas &&
             part.canvas.Children().IndexOf(part.element, index) )
        {
            part.canvas.Children().RemoveAt(index);
            if ( part.canvas == m_root && m_rootMounts )
                --m_rootMounts;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("window part unmount", e);
    }

    // The canvas of a window being destroyed is dropped by the caller, and
    // nothing can be unbound from the window any more.
    for ( wxWindow *w : part.chain )
    {
        if ( windowIsAlive || w != window )
            ReleaseAncestorRef(w);
    }
}

// ----------------------------------------------------------------------------
// nested tree: one canvas per window with slots below it
// ----------------------------------------------------------------------------

void wxWinUITopLevelHost::EnsureInputNodes(wxWindow *window)
{
    if ( m_shuttingDown || !window || window == m_tlw ||
         wxGetTopLevelParent(window) != m_tlw )
    {
        return;
    }

    // From the top down, so that each canvas is made in the one of its
    // parent.
    const std::vector<wxWindow *> chain = GetNodeChain(window);
    for ( auto it = chain.rbegin(); it != chain.rend(); ++it )
    {
        wxWindow * const w = *it;
        if ( m_inputNodes.count(w) )
            continue;
        if ( w->IsBeingDeleted() || w->IsTopLevel() )
            return;

        m_inputNodes.insert(w);
        AddAncestorRef(w);
        if ( m_shuttingDown )
            return;
    }
}

void wxWinUITopLevelHost::ForgetStrayInputNodes()
{
    // A window given to another top-level window takes nothing along: the
    // canvas it had here for the pointer goes.
    std::vector<wxWindow *> strays;
    for ( wxWindow *w : m_inputNodes )
    {
        if ( wxGetTopLevelParent(w) != m_tlw )
            strays.push_back(w);
    }
    for ( wxWindow *w : strays )
    {
        m_inputNodes.erase(w);
        ReleaseAncestorRef(w);
    }
}

wxWindow *wxWinUITopLevelHost::FindInputNodeWindow(
    const winrt::Microsoft::UI::Xaml::UIElement& element) const
{
    if ( !element )
        return nullptr;
    if ( element == m_root )
        return m_tlw;

    const auto canvas =
        element.try_as<winrt::Microsoft::UI::Xaml::Controls::Canvas>();
    if ( !canvas )
        return nullptr;
    for ( const auto& entry : m_nodes )
    {
        if ( entry.second.canvas == canvas )
            return entry.first;
    }
    return nullptr;
}

winrt::Microsoft::UI::Xaml::Controls::Canvas
wxWinUITopLevelHost::GetChildrenCanvas(wxWindow *parent) const
{
    // The nearest window above which has a canvas: its own parent normally,
    // further up only if creating a canvas failed.
    for ( wxWindow *p = parent; p && p != m_tlw; p = p->GetParent() )
    {
        const auto it = m_nodes.find(p);
        if ( it != m_nodes.end() && it->second.canvas )
            return it->second.canvas;
    }
    return m_root;
}

wxPoint wxWinUITopLevelHost::GetChildrenOriginOnScreen(wxWindow *parent) const
{
    // Must name the same window as GetChildrenCanvas().
    for ( wxWindow *p = parent; p && p != m_tlw; p = p->GetParent() )
    {
        const auto it = m_nodes.find(p);
        if ( it == m_nodes.end() || !it->second.canvas )
            continue;

        const HWND hwnd = GetHwndOf(p);
        if ( !hwnd )
            break;

        // Physical, like every position in the island: under a right-to-left
        // layout the client rectangle maps to the screen mirrored.
        RECT rc;
        ::GetClientRect(hwnd, &rc);
        ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT *>(&rc), 2);
        return wxPoint(wxMin(rc.left, rc.right), rc.top);
    }

    const RECT rcCS = GetClientScreenRect();
    return wxPoint(rcCS.left, rcCS.top);
}

bool wxWinUITopLevelHost::IsStructuralElement(
    const winrt::Microsoft::UI::Xaml::DependencyObject& element) const
{
    if ( !element )
        return false;
    if ( element == m_root )
        return true;

    // A part drawn in the place of a window, and what it is made of, a few
    // elements deep at most: they are the window. The parts which are
    // controls (scrollbars) are content like any other.
    if ( m_parts.size() > m_interactiveParts )
    {
        auto node = element;
        for ( int depth = 0; node && depth != 3; ++depth )
        {
            for ( const auto& part : m_parts )
            {
                if ( !IsInteractivePart(part.first.second) &&
                     part.second.element == node )
                {
                    return true;
                }
            }
            node = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                GetParent(node);
        }
    }

    const auto canvas =
        element.try_as<winrt::Microsoft::UI::Xaml::Controls::Canvas>();
    if ( !canvas )
        return false;
    for ( const auto& entry : m_nodes )
    {
        if ( entry.second.canvas == canvas )
            return true;
    }
    return std::find(m_retiredCanvases.begin(), m_retiredCanvases.end(),
                     canvas) != m_retiredCanvases.end();
}

winrt::Microsoft::UI::Xaml::UIElement
wxWinUITopLevelHost::GetRootBranch(const wxWinUISlot& slot) const
{
    using namespace winrt::Microsoft::UI::Xaml;

    DependencyObject branch = slot.GetPresentationRoot();
    while ( branch )
    {
        const DependencyObject parent =
            Media::VisualTreeHelper::GetParent(branch);
        if ( !parent || parent == m_root )
            break;
        branch = parent;
    }
    return branch.try_as<UIElement>();
}

void wxWinUITopLevelHost::MoveToCanvas(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::Controls::Canvas& from,
    const winrt::Microsoft::UI::Xaml::Controls::Canvas& to)
{
    using namespace winrt::Microsoft::UI::Xaml;

    if ( !element || from == to )
        return;

    // Leaving the tree drops the keyboard focus, even for the instant a move
    // inside the same tree takes.
    Controls::Control focused{ nullptr };
    if ( from && to )
    {
        const auto xamlRoot = GetXamlRoot();
        const auto frameworkElement = element.try_as<FrameworkElement>();
        if ( xamlRoot && frameworkElement &&
             wxWinUIFocusIsInside(frameworkElement, xamlRoot) )
        {
            focused = Input::FocusManager::GetFocusedElement(xamlRoot)
                          .try_as<Controls::Control>();
        }
    }

    if ( from )
    {
        uint32_t index = 0;
        if ( from.Children().IndexOf(element, index) )
        {
            from.Children().RemoveAt(index);
            if ( from == m_root && m_rootMounts )
                --m_rootMounts;
        }
        PruneRetiredCanvases(from);
    }
    if ( to )
    {
        to.Children().Append(element);
        if ( to == m_root )
            ++m_rootMounts;
    }

    if ( focused )
        focused.Focus(wxWinUIProgrammaticFocusState(focused));
}

void wxWinUITopLevelHost::PruneRetiredCanvases(
    winrt::Microsoft::UI::Xaml::Controls::Canvas canvas)
{
    using namespace winrt::Microsoft::UI::Xaml;

    // Up the tree: taking the last thing out of a retired canvas takes the
    // canvas out of its own parent, which may have been waiting for it too.
    while ( canvas && canvas != m_root )
    {
        const auto it = std::find(m_retiredCanvases.begin(),
                                  m_retiredCanvases.end(), canvas);
        if ( it == m_retiredCanvases.end() || canvas.Children().Size() != 0 )
            return;
        m_retiredCanvases.erase(it);

        const auto parent = Media::VisualTreeHelper::GetParent(canvas)
                                .try_as<Controls::Canvas>();
        if ( !parent )
            return;

        uint32_t index = 0;
        if ( parent.Children().IndexOf(canvas, index) )
        {
            parent.Children().RemoveAt(index);
            if ( parent == m_root && m_rootMounts )
                --m_rootMounts;
        }
        canvas = parent;
    }
}

void wxWinUITopLevelHost::MountNode(wxWindow *window, WindowNode& node)
{
    if ( !node.canvas )
        return;

    const auto target = GetChildrenCanvas(window->GetParent());
    if ( !target || node.mountCanvas == target )
        return;

    const auto from = node.mountCanvas;
    node.mountCanvas = target;
    node.synced = false;
    m_zOrderDirty = true;
    MoveToCanvas(node.canvas, from, target);
}

void wxWinUITopLevelHost::UnmountNode(WindowNode& node)
{
    if ( !node.canvas || !node.mountCanvas )
        return;

    const auto from = node.mountCanvas;
    node.mountCanvas = nullptr;
    try
    {
        // A canvas which still holds something stays in the tree until that
        // has been taken out of it. The slots below a window being destroyed
        // are disconnected after this, and some of them later still, once
        // the XAML callback they are being destroyed from has returned:
        // their content must not leave the tree before that either.
        if ( node.canvas.Children().Size() != 0 )
        {
            m_retiredCanvases.push_back(node.canvas);
            return;
        }

        MoveToCanvas(node.canvas, from, nullptr);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("window node removal", e);
    }
}

void wxWinUITopLevelHost::MountSlotContainer(wxWinUISlot& slot)
{
    if ( slot.m_isCompositeChild || !slot.m_container )
        return;

    wxWindow * const window = slot.m_window;
    const auto target = window == m_tlw
        ? m_root
        : GetChildrenCanvas(window ? window->GetParent() : nullptr);
    if ( !target || slot.m_mountCanvas == target )
        return;

    const auto from = slot.m_mountCanvas;
    slot.m_mountCanvas = target;
    slot.m_placementValid = false;
    m_zOrderDirty = true;
    MoveToCanvas(slot.m_container, from, target);
}

bool wxWinUITopLevelHost::UnmountSlotContainer(wxWinUISlot& slot)
{
    if ( !slot.m_container || !slot.m_mountCanvas )
        return false;

    const auto from = slot.m_mountCanvas;
    slot.m_mountCanvas = nullptr;
    slot.m_placementValid = false;
    MoveToCanvas(slot.m_container, from, nullptr);
    return true;
}

// ============================================================================
// input synthesis + focus arbiter
// ============================================================================

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
#ifdef WXWINUI_TEST_SUPPORT
            wxWinUIMaybeInjectHandlerFault();
#endif // WXWINUI_TEST_SUPPORT
            const auto boxed = winrt::box_value(std::move(handler));
            slot.GetPresentationRoot().AddHandler(routedEvent, boxed,
                                        true /* handledEventsToo */);
            slot.m_routedHandlers.emplace_back(routedEvent, boxed);
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_slotHandlerAdds;
#endif
        };

    const auto routeFromSender =
        [state](IInspectable const& sender,
                Input::PointerRoutedEventArgs const& e,
                wxWinUIInputKind kind)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            const auto container =
                sender.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
            if ( !host || !container )
                return;

            if ( !host->m_compositeSlots.empty() )
            {
                const auto source = e.OriginalSource().try_as<DependencyObject>();
                const wxWinUISlot * const owner = host->FindSlotForVisual(source);
                if ( owner && owner->m_lifetime != state )
                    return;
            }

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

#ifdef WXWINUI_TEST_SUPPORT
        wxWinUIMaybeInjectHandlerFault();
#endif // WXWINUI_TEST_SUPPORT
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
                     callbackSlot->GetPresentationRoot() != sender )
                {
                    e.TryCancel();
                    return;
                }

                OperationGuard hostOperation(callbackHost);
                wxWinUISlot::OperationGuard slotOperation(callbackSlot);
                const wxWinUISlot * const visualOwner =
                    callbackHost->FindSlotForVisual(e.NewFocusedElement());
                if ( visualOwner && visualOwner != callbackSlot )
                    return;
                const auto getCurrentWindow = [&]() -> wxWindow *
                {
                    wxWinUITopLevelHost * const host = state->GetHost();
                    wxWindow * const window = state->GetWindow();
                    wxWinUISlot * const slot =
                        host && window ? host->FindSlot(window) : nullptr;
                    return slot &&
                                   slot->m_lifetime == state &&
                                   slot->GetPresentationRoot() == sender
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
        slot.GetPresentationRoot().AddHandler(UIElement::GettingFocusEvent(), boxed,
                                    true /* handledEventsToo */);
        slot.m_routedHandlers.emplace_back(UIElement::GettingFocusEvent(),
                                           boxed);
#ifdef WXWINUI_TEST_SUPPORT
        ++gs_slotHandlerAdds;
#endif
    }

    // Focus arbiter, XAML -> wx side: the island holds the native focus, so
    // slotted windows never get WM_SETFOCUS; synthesize the wx focus events
    // and track the owner for FindFocus() resolution.
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIMaybeInjectHandlerFault();
#endif // WXWINUI_TEST_SUPPORT
    slot.m_gotFocusToken = slot.GetPresentationRoot().GotFocus(
        [state](IInspectable const& sender, RoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( !host )
                return;

            const auto container = sender.
                try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
            const wxWinUISlot * const owner = host->FindSlotForVisual(
                e.OriginalSource().try_as<DependencyObject>());
            if ( owner && owner->m_lifetime != state )
                return;
            host->OnXamlSlotGotFocus(state, container);
        });
#ifdef WXWINUI_TEST_SUPPORT
    ++gs_slotHandlerAdds;
#endif

#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIMaybeInjectHandlerFault();
#endif // WXWINUI_TEST_SUPPORT
    slot.m_lostFocusToken = slot.GetPresentationRoot().LostFocus(
        [state](IInspectable const& sender, RoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            if ( !host )
                return;

            const auto container = sender.
                try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
            const wxWinUISlot * const owner = host->FindSlotForVisual(
                e.OriginalSource().try_as<DependencyObject>());
            if ( owner && owner->m_lifetime != state )
                return;
            host->OnXamlSlotLostFocus(state, container);
        });
#ifdef WXWINUI_TEST_SUPPORT
    ++gs_slotHandlerAdds;
#endif
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
#ifdef WXWINUI_TEST_SUPPORT
            ++gs_rootHandlerAdds;
#endif
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

    // The request for a context menu, when nothing below has answered it.
    {
        const auto boxed = winrt::box_value(
            winrt::Windows::Foundation::TypedEventHandler<
                UIElement, Input::ContextRequestedEventArgs>(
                [state](UIElement const&,
                        Input::ContextRequestedEventArgs const& e)
                {
                    wxWinUITopLevelHost * const host = state->GetHost();
                    if ( host )
                        host->OnRootContextRequested(e);
                }));
        m_rootHandlers.emplace_back(UIElement::ContextRequestedEvent(), boxed);
        try
        {
            m_root.AddHandler(UIElement::ContextRequestedEvent(), boxed,
                              false /* only what nothing has handled */);
        }
        catch ( ... )
        {
            m_rootHandlers.pop_back();
            throw;
        }
#ifdef WXWINUI_TEST_SUPPORT
        ++gs_rootHandlerAdds;
#endif
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

wxWinUITopLevelHost::XamlHitResolution
wxWinUITopLevelHost::PointOverXamlContent(
    const wxPoint& ptClientPx,
    const winrt::Windows::Foundation::Point *exactRootDips,
    wxWindow **windowUnder)
{
    if ( windowUnder )
        *windowUnder = nullptr;
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
            if ( element != root && !IsStructuralElement(element) )
                return XamlHitResolution::Hit;
        }

        if ( windowUnder )
        {
            // The canvas of a window shows nothing and takes no input:
            // it is found among all the elements at the point, those hit
            // testing does not see included, the topmost first.
            const auto all = VisualTreeHelper::FindElementsInHostCoordinates(
                dip, scope, true);
            if ( !current() )
                return XamlHitResolution::Unavailable;
            for ( const auto& element : all )
            {
                if ( wxWindow * const window = FindInputNodeWindow(element) )
                {
                    *windowUnder = window;
                    break;
                }
            }
        }

        return XamlHitResolution::Miss;
    }
    catch ( const winrt::hresult_error& )
    {
        return XamlHitResolution::Unavailable;
    }
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

WXDLLIMPEXP_CORE bool
wxWinUITLWHostShouldPreserveFocusOnActivate(wxWindow *window)
{
    return wxWinUITopLevelHost::ShouldPreserveFocusOnActivate(window);
}

// ============================================================================
// wx event plumbing
// ============================================================================

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


WXDLLIMPEXP_CORE bool wxWinUITLWHostNotifyReparent(wxWindow *window,
                                                   wxWindow *oldTLW)
{
#if wxUSE_RICHTOOLTIP
    // Close or retarget transient rich tips while their original island root
    // is still alive. The notifier snapshots only weak session state and is
    // safe if closing a tip destroys windows reentrantly.
    wxWinUINotifyTeachingTipReparent(window);
#endif
    return wxWinUITopLevelHost::HandleReparentAcrossHosts(window, oldTLW);
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

bool wxWinUITopLevelHost::HandleReparentAcrossHosts(wxWindow *subtreeRoot,
                                                    wxWindow *oldTLW)
{
    if ( !subtreeRoot )
        return true;

    if ( wxWinUIInputNodesEnabled() )
    {
        std::vector<std::shared_ptr<wxWinUIHostLifetime>> states;
        for ( const auto& kv : gs_tlwHosts )
        {
            if ( kv.second && kv.second->m_hostLifetime )
                states.push_back(kv.second->m_hostLifetime);
        }
        for ( const auto& state : states )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
                host->ForgetStrayInputNodes();
        }
    }

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
            return true;

        if ( state )
        {
            if ( wxWinUITopLevelHost * const host = state->GetHost() )
            {
                if ( !host->HandleReparent(liveSubtree) )
                    return false;
            }
        }
    }
    return true;
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
                if ( (slot->m_isCompositeChild ||
                      slot->m_compositeContentExtension ||
                      host->HasCompositeChildren(*slot)) &&
                     !host->CanMigrateCompositeSubtreeNow(subtreeRoot) )
                    return false;
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

WXDLLIMPEXP_CORE int wxWinUIGetForcedDPI()
{
    static const int s_dpi = []()
    {
        wchar_t text[32];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_FORCE_SCALE", text, WXSIZEOF(text));
        if ( !length || length >= WXSIZEOF(text) )
            return 0;

        double scale = wcstod(text, nullptr);
        if ( scale > 10 )
            scale /= 100;   // a percentage
        if ( scale < 0.5 || scale > 5 )
            return 0;

        return static_cast<int>(std::lround(96 * scale));
    }();

    return s_dpi;
}

WXDLLIMPEXP_CORE bool wxWinUIIsRightToLeftForced()
{
    static const bool s_forced = []()
    {
        wchar_t text[8];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_FORCE_RTL", text, WXSIZEOF(text));
        return length && length < WXSIZEOF(text) && wcscmp(text, L"0") != 0;
    }();

    return s_forced;
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
                wxLogDebug(
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
                wxLogDebug("wxWinUI: failed to remove a subclass during "
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
            if ( IsStructuralElement(current) )
                continue;

            while ( current )
            {
                for ( const auto& entry : m_slots )
                {
                    wxWinUISlot* const slot = entry.second;
                    if ( slot && slot->GetPresentationRoot() &&
                         current == slot->GetPresentationRoot() )
                    {
                        wxWindow* const window = slot->m_window;
                        RefreshSlotRects(*slot);
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
        winrt::Microsoft::UI::Xaml::FrameworkElement container{ nullptr };
    };

    std::vector<SlotSnapshot> slotSnapshots;
    slotSnapshots.reserve(m_slots.size());
    for ( const auto& entry : m_slots )
    {
        wxWinUISlot * const slot = entry.second;
        if ( slot && slot->m_lifetime && slot->GetPresentationRoot() )
        {
            slotSnapshots.push_back(
                { slot->m_lifetime, slot->GetPresentationRoot() });
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
                 slot->GetPresentationRoot() != snapshot.container ||
                 window->IsBeingDeleted() ||
                 (RefreshSlotRects(*slot), !slot->m_visible) ||
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
            // Paint order among the slots: one counter runs over the whole
            // nested tree, see RecomputeZOrder().
            int z = 0;
            // Order, among the children of the root canvas, of the one this
            // slot is in: what a branch put directly on the root compares
            // with.
            int rootZ = 0;
            bool hitTestVisible = true;
            bool valid = false;
        };

        // Read one candidate exclusively through its stable snapshot. Every
        // callback-bearing WinRT boundary is followed immediately by both the
        // host-generation check and exact lifetime/container revalidation.
        const auto readSlotState =
            [&current, &resolveSlot, &slotSnapshots, &root](std::size_t index)
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

#ifdef WXWINUI_TEST_SUPPORT
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
#endif // WXWINUI_TEST_SUPPORT

                const int z =
                    winrt::Microsoft::UI::Xaml::Controls::Canvas::
                        GetZIndex(snapshot.container);
                if ( !current() )
                    return state;
                if ( !resolveSlot(snapshot) )
                    return state;

                int rootZ = z;
                DependencyObject branch = snapshot.container;
                for ( ;; )
                {
                    const DependencyObject parent =
                        VisualTreeHelper::GetParent(branch);
                    if ( !parent || parent == root )
                        break;
                    branch = parent;
                }
                if ( const auto branchElement = branch.try_as<UIElement>() )
                {
                    rootZ = winrt::Microsoft::UI::Xaml::Controls::Canvas::
                        GetZIndex(branchElement);
                }
                if ( !current() )
                    return state;
                if ( !resolveSlot(snapshot) )
                    return state;

                state.index = index;
                state.z = z;
                state.rootZ = rootZ;
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
            if ( hit == root || IsStructuralElement(hit) )
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

            if ( disabled.valid && disabled.rootZ > freeZ )
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
            RefreshSlotRects(*slot);
            if ( slot->m_visible &&
                 slot->m_visibleRectPx.Contains(pt.x, pt.y) )
            {
                inSlot = true;
                break;
            }
        }

        const bool hasFreeContent =
            root.Children().Size() > m_rootMounts || m_interactiveParts;
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

#ifdef WXWINUI_TEST_SUPPORT
std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIPhysicalDisconnectTestAccess::CreateGate(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
    bool satisfied,
    bool degraded)
{
    try
    {
        return std::make_shared<wxWinUITestPhysicalDisconnectGate>(
            queue, satisfied, degraded);
    }
    catch ( ... )
    {
        return {};
    }
}

bool wxWinUIPhysicalDisconnectTestAccess::SetGateState(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate,
    bool satisfied,
    bool degraded)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUITestPhysicalDisconnectGate>(gate);
    if ( !probe )
        return false;
    probe->SetState(satisfied, degraded);
    return true;
}

bool wxWinUIPhysicalDisconnectTestAccess::IsGateSatisfied(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    return gate && gate->IsSatisfied();
}

bool wxWinUIPhysicalDisconnectTestAccess::IsGateDegraded(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    return gate && gate->IsDegraded();
}

bool wxWinUIPhysicalDisconnectTestAccess::OpenGateAtSealedCompletion(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUITestPhysicalDisconnectGate>(gate);
    if ( !probe )
        return false;
    probe->OpenAtSealedCompletion();
    return true;
}

bool wxWinUIPhysicalDisconnectTestAccess::OpenGateBeforeSourceClose(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    const auto probe = std::dynamic_pointer_cast<
        wxWinUITestPhysicalDisconnectGate>(gate);
    if ( !probe )
        return false;
    probe->OpenBeforeSourceClose();
    return true;
}
#endif

#ifdef WXWINUI_TEST_SUPPORT
unsigned long long wxWinUISlotTestAccess::GetSyncEpoch(const wxWinUISlot& slot)
{ return slot.m_syncEpoch; }

bool wxWinUISlotTestAccess::IsContentTransactionInProgress(const wxWinUISlot& slot)
{ return slot.m_contentTransactionInProgress; }

bool wxWinUISlotTestAccess::IsToolTipPropertyObserverPending(const wxWinUISlot& slot)
{ return slot.m_toolTipPropertyObserverPending; }

unsigned wxWinUISlotTestAccess::GetToolTipPropertyObserverRetryFailures(const wxWinUISlot& slot)
{ return slot.m_toolTipPropertyObserverRetryFailures; }

bool wxWinUISlotTestAccess::IsContentLoaded(const wxWinUISlot& slot)
{ return slot.IsContentLoaded(); }

void wxWinUISlotTestAccess::SetContentLoaded(wxWinUISlot& slot, bool loaded)
{ slot.m_contentLoaded = loaded; }

bool wxWinUISlotTestAccess::IsContentLoadedObserverPending(const wxWinUISlot& slot)
{ return slot.m_contentLoadedObserverPending; }

bool wxWinUISlotTestAccess::HasContentLoadedObserver(const wxWinUISlot& slot)
{ return slot.m_contentLoadedState != nullptr; }

winrt::Microsoft::UI::Xaml::UIElement wxWinUISlotTestAccess::GetAutomationNameStyleObservedContent(const wxWinUISlot& slot)
{ return slot.m_automationNameStyleObservedContent; }

unsigned wxWinUISlotTestAccess::GetContentLoadedObserverRetryFailures(const wxWinUISlot& slot)
{ return slot.m_contentLoadedObserverRetryFailures; }

std::weak_ptr<wxWinUISlotLifetime> wxWinUISlotTestAccess::GetLifetime(const wxWinUISlot& slot)
{ return slot.m_lifetime; }

void wxWinUISlotTestAccess::SetLastToolTipDPI(wxWinUISlot& slot, int dpi)
{ slot.m_lastToolTipDPI = dpi; }

std::uintptr_t wxWinUISlotTestAccess::GetPhysicalDisconnectGateIdentity(const wxWinUISlot& slot)
{ return reinterpret_cast<std::uintptr_t>(slot.m_disconnectGate.get()); }

void wxWinUIHostContentTestAccess::Shutdown(wxWinUITopLevelHost& host)
{ host.Shutdown(); }

std::weak_ptr<wxWinUIHostLifetime> wxWinUIHostContentTestAccess::GetLifetime(const wxWinUITopLevelHost& host)
{ return host.m_hostLifetime; }

unsigned wxWinUIHostContentTestAccess::GetOwnFlushRunCount(const wxWinUITopLevelHost& host)
{ return host.m_flushRuns; }
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_WINUI3
