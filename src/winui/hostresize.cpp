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

bool (*gs_nativeResizeContactReader)(void *) = nullptr;
void *gs_nativeResizeContactContext = nullptr;
bool gs_failNextNativeResizePost = false;
std::set<wxWinUITopLevelHost *> gs_deferredNativeResizeWakes;

} // anonymous namespace

UINT wxWinUINativeResizeMessage()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.NativeResize");
    return message;
}

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
        if ( msg == WM_ENTERSIZEMOVE &&
             request->Enter(request->dispatching) )
        {
            host->m_nativeResizeSnapshot.phase = wxWinUINativeResizePhase::Entered;
            ++host->m_nativeResizeSnapshot.entered;
            wxWinUIInputLog("grip resize #%llu entered", request->ticket);
        }
        else if ( msg == WM_EXITSIZEMOVE && request->Exit() )
        {
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

#endif // wxUSE_WINUI3
