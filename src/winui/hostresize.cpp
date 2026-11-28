/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostresize.cpp
// Purpose:     Host-owned native resize handoff and USER32 lifecycle
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "hostresize.h"
#include "hostnative.h"
#include "private.h"
#ifdef WXWINUI_TEST_SUPPORT
#include "host-resize-test-access.h"
#include <map>
#endif

#include "wx/toplevel.h"
#include "wx/scopeguard.h"
#include "wx/msw/private.h"

#include <windowsx.h>
#include <algorithm>
#include <limits>
#include <set>
#include <utility>

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
bool (*gs_nativeResizeContactReader)(void *) = nullptr;
void *gs_nativeResizeContactContext = nullptr;
bool gs_failNextNativeResizePost = false;

// Test-only observations, not transaction authority. Weak control-block
// identity rejects a later host at the same address without retaining it.
// All callers keep the existing UI-thread contract. No snapshot reference
// is used after a wx/XAML/native callback; ShutdownNativeResize erases this entry
// before the host releases its lifetime control block.
using NativeResizeSnapshots = std::map<
    std::weak_ptr<wxWinUIHostLifetime>, wxWinUINativeResizeSnapshot,
    std::owner_less<std::weak_ptr<wxWinUIHostLifetime>>>;
NativeResizeSnapshots gs_nativeResizeSnapshots;

wxWinUINativeResizeSnapshot& NativeResizeSnapshotFor(
    const std::shared_ptr<wxWinUIHostLifetime>& lifetime)
{
    return gs_nativeResizeSnapshots[lifetime];
}
#endif // WXWINUI_TEST_SUPPORT
std::set<wxWinUITopLevelHost *> gs_deferredNativeResizeWakes;

struct NativeSizingAdmission;
thread_local NativeSizingAdmission* gs_nativeSizingAdmission = nullptr;
UINT gs_nativeSizingMotionMessage = 0;

// This is only an admission scope around the real USER32 call, not a second
// resize state machine. The request's identities/entered state remain the
// authority; nesting restores the previous scope without keeping any wx
// object alive beyond its existing OperationGuard.
struct NativeSizingAdmission final
{
    NativeSizingAdmission(HWND window,
                          const std::shared_ptr<wxWinUIHostLifetime>& owner,
                          std::uint64_t requestTicket,
                          const wxWinUIPointerSample& press,
                          std::function<bool()> validate)
        : target(window), lifetime(owner), ticket(requestTicket), latest(press),
          current(std::move(validate)), previous(gs_nativeSizingAdmission)
    {
        // USER32 tracks from the press until a motion is delivered.
        deliveredPoint = {press.screenX, press.screenY};
        gs_nativeSizingAdmission = this;
    }
    ~NativeSizingAdmission() { gs_nativeSizingAdmission = previous; }

    HWND target;
    std::shared_ptr<wxWinUIHostLifetime> lifetime;
    std::uint64_t ticket;
    wxWinUIPointerSample latest;
    std::function<bool()> current;
    NativeSizingAdmission* previous;
    bool logged = false;
    bool motionPosted = false;
    bool motionDelivered = false;
    bool releaseAccepted = false;
    bool releasePending = false;
    POINT deliveredPoint{};
};

} // anonymous namespace

void wxWinUIQueueNativeSizingMove(
    const std::shared_ptr<wxWinUIHostLifetime>& lifetime,
    const wxWinUIPointerSample& sample)
{
    auto* const scope = gs_nativeSizingAdmission;
    if ( !scope || scope->lifetime != lifetime || !gs_nativeSizingMotionMessage ||
         scope->releaseAccepted ||
         ::GetCapture() != scope->target || !scope->current() ||
         sample.kind != wxWinUIInputKind::Move ||
         sample.device != wxWinUIInputDevice::Mouse || !sample.isPrimary ||
         sample.isCompatibilityMouse || !(sample.buttonMask & MK_LBUTTON) ||
         sample.pointerId != scope->latest.pointerId ||
         sample.timestamp < scope->latest.timestamp )
        return;

    // The request remains the sole sizing authority. This stack-bound
    // transport keeps only the most recent observed delta, with one queued
    // notification regardless of how many routed moves arrive together.
    scope->latest = sample;
    if ( scope->motionPosted ||
         (scope->motionDelivered && scope->deliveredPoint.x == sample.screenX &&
          scope->deliveredPoint.y == sample.screenY) )
        return;
    scope->motionPosted = ::PostMessageW(scope->target, gs_nativeSizingMotionMessage,
        static_cast<WPARAM>(scope->ticket & 0xffffffffu),
        static_cast<LPARAM>(scope->ticket >> 32)) != FALSE;
    if ( !scope->motionPosted )
        wxWinUIInputLog("grip resize #%llu motion post failed", scope->ticket);
}

namespace
{
bool QueueNativeSizingRelease(const std::shared_ptr<wxWinUIHostLifetime>& lifetime,
                             const wxWinUIPointerSample& sample)
{
    auto* const scope = gs_nativeSizingAdmission;
    if ( !scope || scope->lifetime != lifetime || !gs_nativeSizingMotionMessage ||
         ::GetCapture() != scope->target || !scope->current() ||
         sample.device != wxWinUIInputDevice::Mouse || !sample.isPrimary ||
         sample.isCompatibilityMouse || (sample.buttonMask & MK_LBUTTON) ||
         sample.pointerId != scope->latest.pointerId )
        return false;
    // A repeated/stale routed release must neither end a newer sample nor
    // turn the already accepted commit into WM_CANCELMODE on the bubble.
    if ( scope->releaseAccepted || sample.timestamp < scope->latest.timestamp )
        return true;
    scope->releaseAccepted = true;
    scope->releasePending = true;
    scope->latest = sample;
    // Retire an undelivered motion at the release boundary. USER32 commits
    // the geometry it has actually tracked, without applying a late move
    // after the physical contact ended.
    if ( !scope->motionPosted )
        scope->motionPosted = ::PostMessageW(scope->target, gs_nativeSizingMotionMessage,
            static_cast<WPARAM>(scope->ticket & 0xffffffffu),
            static_cast<LPARAM>(scope->ticket >> 32)) != FALSE;
    if ( !scope->motionPosted )
    {
        scope->releaseAccepted = false;
        scope->releasePending = false;
        return false;
    }
    wxWinUIInputLog("grip resize #%llu release queued for commit", scope->ticket);
    return true;
}
}

bool wxWinUIMessageBelongsToNativeSizing(MSG& message)
{
    auto* const scope = gs_nativeSizingAdmission;
    if ( gs_nativeSizingMotionMessage && message.message == gs_nativeSizingMotionMessage )
    {
        const std::uint64_t ticket = static_cast<std::uint32_t>(message.wParam) |
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(message.lParam)) << 32);
        // A packet left behind by release, capture loss, destruction or a
        // different request never becomes a mouse message in the outer pump.
        message.message = WM_NULL;
        message.wParam = 0;
        message.lParam = 0;
        if ( !scope || message.hwnd != scope->target || ticket != scope->ticket ||
             !scope->motionPosted )
            return true;
        scope->motionPosted = false;
        if ( ::GetCapture() != scope->target || !scope->current() )
            return true;
        const auto& sample = scope->latest;
        const bool release = scope->releasePending;
        // The release ends the tracking where USER32 has tracked it: at the
        // last delivered motion, or at the press. The USER32 of Windows 10,
        // and of Windows Server 2022, follows the position of WM_LBUTTONUP
        // before ending: it would apply the late undelivered motion after all.
        const POINT screen = release ? scope->deliveredPoint
                                     : POINT{sample.screenX, sample.screenY};
        if ( !release && scope->motionDelivered && scope->deliveredPoint.x == screen.x &&
             scope->deliveredPoint.y == screen.y )
            return true;
        POINT client = screen;
        ::SetLastError(ERROR_SUCCESS);
        if ( (!::MapWindowPoints(HWND_DESKTOP, scope->target, &client, 1) &&
              ::GetLastError() != ERROR_SUCCESS) ||
             client.x < (std::numeric_limits<short>::min)() ||
             client.x > (std::numeric_limits<short>::max)() ||
             client.y < (std::numeric_limits<short>::min)() ||
             client.y > (std::numeric_limits<short>::max)() )
            return true;
        scope->releasePending = false;
        message.message = release ? WM_LBUTTONUP : WM_MOUSEMOVE;
        message.wParam = sample.modifiers | sample.buttonMask;
        message.lParam = MAKELPARAM(static_cast<short>(client.x), static_cast<short>(client.y));
        // Preserve the observed XAML position in the queued message. This
        // does not alter GetCursorPos/GetMessagePos's physical queue state.
        message.pt = screen;
        message.time = static_cast<DWORD>(sample.timestamp);
        if ( !release )
        {
            scope->motionDelivered = true;
            scope->deliveredPoint = screen;
        }
    }
    if ( !scope || message.hwnd != scope->target ||
         ::GetCapture() != scope->target || !scope->current() ) return false;

    bool tracking = false;
    switch ( message.message )
    {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
            scope->motionDelivered = true;
            scope->deliveredPoint = message.pt;
            tracking = true;
            break;
        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
        case WM_CANCELMODE:
            tracking = true;
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
            tracking = message.wParam == VK_ESCAPE || message.wParam == VK_RETURN ||
                       message.wParam == VK_LEFT || message.wParam == VK_RIGHT ||
                       message.wParam == VK_UP || message.wParam == VK_DOWN;
            break;
        default:
            break;
    }
    if ( tracking && !scope->logged )
    {
        scope->logged = true;
        wxWinUIInputLog("grip USER32 tracking retained msg=0x%04x target=%p",
                       message.message, static_cast<void*>(scope->target));
    }
    return tracking;
}

UINT wxWinUINativeResizeMessage()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.NativeResize");
    return message;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostResizeTestAccess::SetContactReader(
    bool (*reader)(void *), void *context)
{
    gs_nativeResizeContactReader = reader;
    gs_nativeResizeContactContext = reader ? context : nullptr;
}

void wxWinUIHostResizeTestAccess::FailNextPost()
{
    gs_failNextNativeResizePost = true;
}

bool wxWinUIHostResizeTestAccess::GetSnapshot(
    wxWindow *window, wxWinUINativeResizeSnapshot *snapshot)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, false);
    if ( !host || !snapshot )
        return false;
    const auto it = gs_nativeResizeSnapshots.find(host->m_hostLifetime);
    *snapshot = it != gs_nativeResizeSnapshots.end()
        ? it->second
        : wxWinUINativeResizeSnapshot();
    return true;
}

#endif // WXWINUI_TEST_SUPPORT

void wxWinUITopLevelHost::ShutdownNativeResize()
{
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
#ifdef WXWINUI_TEST_SUPPORT
    gs_nativeResizeSnapshots.erase(m_hostLifetime);
#endif
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
    wxWinUIInputLog("grip resize #%llu accepted pointer=%u time=%llu ht=%d",
                    request->ticket, sample.pointerId, sample.timestamp, hitTest);
#ifdef WXWINUI_TEST_SUPPORT
    auto& snapshot = NativeResizeSnapshotFor(m_hostLifetime);
    snapshot.phase = wxWinUINativeResizePhase::Pending;
    snapshot.ticket = request->ticket;
    snapshot.hitTest = hitTest;
    snapshot.screenPoint = { sample.screenX, sample.screenY };
    ++snapshot.scheduled;
#endif
    return true;
}

bool wxWinUITopLevelHost::PostNativeResizeWake()
{
    if ( m_nativeResizeWakePosted )
        return true;
    if ( !m_nativeResize || m_shuttingDown || !m_bridge )
        return false;
#ifdef WXWINUI_TEST_SUPPORT
    if ( gs_failNextNativeResizePost )
    {
        gs_failNextNativeResizePost = false;
        return false;
    }
#endif
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
    if ( wxWinUIIsGlobalOperationActive() || gs_deferredNativeResizeWakes.empty() )
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
#ifdef WXWINUI_TEST_SUPPORT
    auto& snapshot = NativeResizeSnapshotFor(m_hostLifetime);
    snapshot.phase = wxWinUINativeResizePhase::Idle;
    if ( cancelled || request->cancelled || !request->entered )
        ++snapshot.cancelled;
#else
    wxUnusedVar(cancelled);
#endif
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
            // Release commits an entered USER32 sizing loop. WM_CANCELMODE
            // is reserved for interruptions and rolls its geometry back.
            // Before native entry, the old cancellation behavior remains.
            if ( QueueNativeSizingRelease(m_hostLifetime, sample) )
                return;
            break;
        case wxWinUIInputKind::CaptureLost:
        {
            // Losing island capture is expected both during our release and
            // when USER32 takes over. Other native capture changes cancel.
            if ( request->preparing ||
                 (request->dispatching && ::GetCapture() ==
                    reinterpret_cast<HWND>(request->target.GetLeafHwnd())) )
                return;

            // XAML can transfer native capture between the bridge and its
            // InputSite after PointerPressed, before our queued wake. The
            // old HWND receives WM_CAPTURECHANGED, but the island still owns
            // this gesture. Apply the same island boundary as at acceptance:
            // do not mistake this internal transfer for loss to another UI.
            // A null/outside owner still cancels, as do Cancel and Release;
            // the wake retains all identity and physical-contact checks.
            const HWND capture = ::GetCapture();
            if ( !request->dispatching && capture &&
                 (capture == m_bridge || capture == m_inner ||
                  (m_bridge && ::IsChild(m_bridge, capture))) )
            {
                wxWinUIInputLog("grip resize #%llu island capture transfer",
                                request->ticket);
                return;
            }
            break;
        }
        case wxWinUIInputKind::Leave:
            // Dragging the corner outward can cross the island boundary
            // before the queued native handoff runs. Leaving is not releasing
            // this accepted gesture while the exact pointer is still down.
            // The wake independently rechecks physical contact and all target,
            // generation and capture ownership before entering USER32 sizing.
            if ( request->dispatching || request->preparing ||
                 (sample.buttonMask & MK_LBUTTON) )
                return;
            break;
        case wxWinUIInputKind::Cancel:
            break;
        default:
            return;
    }

    wxWinUIInputLog("grip resize #%llu cancelled kind=%d preparing=%d dispatching=%d",
                    request->ticket, static_cast<int>(sample.kind),
                    request->preparing, request->dispatching);
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
    if ( wxWinUIIsGlobalOperationActive() )
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
#ifdef WXWINUI_TEST_SUPPORT
        if ( gs_nativeResizeContactReader )
            return gs_nativeResizeContactReader(gs_nativeResizeContactContext);
#endif
        return wxWinUIIsPointerContactActive(
            wxWinUIInputDevice::Mouse, 0, VK_LBUTTON);
    };

    try
    {
        const bool contactAtWake = contactActive();
        if ( !contactAtWake || !ValidateNativeResize(request) )
        {
            wxWinUIInputLog("grip resize #%llu wake rejected contact=%d",
                            request->ticket, contactAtWake);
            return;
        }

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
        const auto lifetime = m_hostLifetime;
        if ( !gs_nativeSizingMotionMessage )
            gs_nativeSizingMotionMessage = ::RegisterWindowMessageW(L"wxWidgets.WinUI.NativeResizeMotion");
        if ( !gs_nativeSizingMotionMessage )
            return;
        const NativeSizingAdmission nativeTracking(hwnd, lifetime, request->ticket,
            request->sample, [this, lifetime, request]()
        {
            // No callback-bearing peer query from WH_GETMESSAGE. The strong
            // request and invalidatable host/native identities suffice here;
            // the physical contact check above is unchanged.
            return lifetime && lifetime->GetHost() == this && !m_shuttingDown &&
                   m_nativeResize == request && request->dispatching &&
                   request->entered && !request->exited && !request->cancelled &&
                   request->inputSiteGeneration == m_inputSiteGeneration &&
                   request->target.IsValid();
        });
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
        if ( msg == WM_ENTERSIZEMOVE &&
             request->Enter(request->dispatching) )
        {
#ifdef WXWINUI_TEST_SUPPORT
            auto& snapshot = NativeResizeSnapshotFor(host->m_hostLifetime);
            snapshot.phase = wxWinUINativeResizePhase::Entered;
            ++snapshot.entered;
#endif
            wxWinUIInputLog("grip resize #%llu entered", request->ticket);
        }
        else if ( msg == WM_EXITSIZEMOVE && request->Exit() )
        {
#ifdef WXWINUI_TEST_SUPPORT
            auto& snapshot = NativeResizeSnapshotFor(host->m_hostLifetime);
            snapshot.phase = wxWinUINativeResizePhase::Idle;
            ++snapshot.exited;
#endif
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

#endif // wxUSE_WINUI3
