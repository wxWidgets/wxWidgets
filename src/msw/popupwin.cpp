///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/popupwin.cpp
// Purpose:     implements wxPopupWindow for MSW
// Author:      Vadim Zeitlin
// Created:     08.05.02
// Copyright:   (c) 2002 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#if wxUSE_POPUPWIN

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/popupwin.h"

#include "wx/msw/private.h"     // for GetDesktopWindow()
#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/except.h"
    #include "wx/scopeguard.h"
    #include "wx/winui/private/dialogsession.h"
    #include "wx/winui/private/transient.h"

    #include <algorithm>
    #include <new>
    #include <unordered_map>
    #include <utility>
    #include <vector>
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/weakref.h"
#endif

// Set to the popup window currently being shown, if any.
//
// Note that this global variable is used in src/msw/window.cpp and so must be
// extern.
wxPopupWindow* wxCurrentPopupWindow = nullptr;

#if defined(__WXWINUI__) && wxUSE_WINUI3

// This implementation-only friend is the sole bridge from the cpp sidecar to
// the historical exported classes. It adds neither object state nor a vtable
// entry and keeps all WinUI policy out of the public API.
class wxWinUIPopupSidecarAccess final
{
public:
    static void SetOwner(wxPopupWindow *popup, wxWindow *owner)
    {
        popup->m_owner = owner;
    }

    static void NotifyDismiss(wxPopupTransientWindow *popup)
    {
        popup->OnDismiss();
    }

    static void DismissFromOutside(wxPopupTransientWindow *popup)
    {
        popup->DismissFromOutside();
    }
};

namespace
{

struct wxWinUIPopupCancelToken
{
    wxWeakRef<wxWindow> owner;
    WXHWND ownerHwnd = nullptr;
    // Owner publication validates the wxWindow/HWND association. Terminal
    // popup withdrawal additionally needs the independent native HWND
    // lifetime because it can call USER32 after arbitrary wx callbacks.
    unsigned long long ownerHwndGeneration = 0;
    WXHWND popupHwnd = nullptr;
    unsigned long long popupNativeHwndGeneration = 0;
    std::uint64_t registrationGeneration = 0;
    unsigned long long sessionEpoch = 0;
};

struct wxWinUIPopupState
{
    wxWeakRef<wxWindow> lifetime;
    std::unique_ptr<wxWinUITransientRegistration> registration;
    std::unique_ptr<wxWinUITransientOwnerRetirement> retirement;
    wxWeakRef<wxWindow> registeredOwner;
    wxWeakRef<wxWindow> focusBeforePopup;
    bool hadOwner = false;
    bool endingSession = false;
    bool destroying = false;
    bool dismissInProgress = false;
    bool managerCancelPending = false;
    std::shared_ptr<const wxWinUIPopupCancelToken> managerCancelToken;
    bool dismissAndNotifyInProgress = false;
    bool dismissNotificationDelivered = false;
    bool terminalCancellation = false;
    unsigned long long popupInvocationEpoch = 0;
    unsigned long long showRequestEpoch = 0;
    unsigned long long focusGeneration = 0;
    unsigned long long sessionEpoch = 0;
};

using wxWinUIPopupStates =
    std::unordered_map<const wxPopupWindow *, wxWinUIPopupState>;

wxWinUIPopupStates& wxWinUIGetPopupStates()
{
    static wxWinUIPopupStates * const states = new wxWinUIPopupStates;
    return *states;
}

wxWinUIPopupState& wxWinUIGetPopupState(wxPopupWindow *popup)
{
    wxWinUIPopupStates& states = wxWinUIGetPopupStates();
    wxWinUIPopupState& state = states[popup];
    if ( state.lifetime.get() != popup )
    {
        state = wxWinUIPopupState();
        state.lifetime = wxWeakRef<wxWindow>(popup);
    }

    if ( states.size() > 64 )
    {
        for ( wxWinUIPopupStates::iterator it = states.begin();
              it != states.end(); )
        {
            if ( it->first != popup && !it->second.lifetime.get() )
                it = states.erase(it);
            else
                ++it;
        }
    }

    return state;
}

wxWinUIPopupState *wxWinUIFindPopupState(wxPopupWindow *popup)
{
    wxWinUIPopupStates& states = wxWinUIGetPopupStates();
    const wxWinUIPopupStates::iterator it = states.find(popup);
    return it != states.end() && it->second.lifetime.get() == popup
               ? &it->second
               : nullptr;
}

void wxWinUIErasePopupState(wxPopupWindow *popup)
{
    wxWinUIGetPopupStates().erase(popup);
}

unsigned long long wxWinUIBumpEpoch(unsigned long long& epoch)
{
    if ( ++epoch == 0 )
        ++epoch;
    return epoch;
}

void wxWinUIBumpPopupEpoch(wxWinUIPopupState& state)
{
    (void)wxWinUIBumpEpoch(state.sessionEpoch);
}

bool wxWinUIBeginPopupTransientSession(wxPopupWindow *popup);
bool wxWinUIBeginPopupEndingSession(wxPopupWindow *popup);
void wxWinUIFinishPopupEndingSession(wxPopupWindow *popup);
void wxWinUIEndPopupTransientSession(wxPopupWindow *popup);
void wxWinUICancelPopupTransient(
    wxPopupWindow *popup,
    const std::shared_ptr<const wxWinUIPopupCancelToken>& token);
bool wxWinUIDismissAndNotify(wxPopupTransientWindow *popup);
bool wxWinUINotifyDismissOnce(wxPopupTransientWindow *popup);

wxWinUIPopupOwnerPublishHookForTesting
    gs_winuiPopupOwnerPublishHookForTesting = nullptr;
wxWinUIPopupCaptureReleaseHookForTesting
    gs_winuiPopupCaptureReleaseHookForTesting = nullptr;
bool gs_failNextPopupTerminalNativeIdentityForTesting = false;
wxWinUIPopupCreateHookForTesting
    gs_winuiPopupCreateHookForTesting = nullptr;
wxWinUIPopupOwnerDeactivateHookForTesting
    gs_winuiPopupOwnerDeactivateHookForTesting = nullptr;
bool gs_winuiFailNextPopupOwnerRecoveryForTesting = false;

template <typename T, typename... Args>
std::unique_ptr<T> wxWinUIMakePopupUnique(
    wxWinUITransientAllocationSite site,
    Args&&... args)
{
    wxWinUITransientAllocationPointForTesting(site);
    return std::make_unique<T>(std::forward<Args>(args)...);
}

bool wxWinUIPopupIsUnavailable(const wxWindow *window)
{
    return !window || window->IsBeingDeleted() ||
           wxWinUITLWHostIsDestroyScheduled(window);
}

wxPopupWindow *wxWinUIResolvePopup(
    const wxWeakRef<wxWindow>& identity,
    const wxPopupWindow *expected)
{
    wxPopupWindow * const popup =
        wxDynamicCast(identity.get(), wxPopupWindow);
    return popup == expected && !wxWinUIPopupIsUnavailable(popup)
               ? popup
               : nullptr;
}

unsigned long long wxWinUIGetPopupSessionEpoch(wxPopupWindow *popup)
{
    const wxWinUIPopupState * const state = wxWinUIFindPopupState(popup);
    return state ? state->sessionEpoch : 0;
}

bool wxWinUIPopupSessionIsCurrent(wxPopupWindow *popup,
                                  unsigned long long epoch)
{
    const wxWinUIPopupState * const state = wxWinUIFindPopupState(popup);
    return state && state->sessionEpoch == epoch &&
           !state->endingSession && !state->destroying;
}

wxWindow *wxWinUIGetPopupLogicalOwner(wxPopupWindow *popup)
{
    wxWindow * const parent = popup ? popup->GetParent() : nullptr;
    return parent ? wxGetTopLevelParent(parent) : nullptr;
}

void wxWinUIClearPopupFocusSnapshot(wxPopupWindow *popup,
                                    unsigned long long generation)
{
    wxWinUIPopupState * const state = wxWinUIFindPopupState(popup);
    if ( state && state->focusGeneration == generation )
        state->focusBeforePopup = nullptr;
}

struct wxWinUINativeHwndIdentity
{
    HWND hwnd = nullptr;
    unsigned long long generation = 0;
};

wxWinUINativeHwndIdentity wxWinUICaptureNativeIdentity(wxWindow *window)
{
    const WXHWND wxHwnd = window ? window->GetHWND() : nullptr;
    return {
        reinterpret_cast<HWND>(wxHwnd),
        wxHwnd ? wxWinUIMSWGetNativeHwndGeneration(wxHwnd) : 0
    };
}

bool wxWinUINativeIdentityIsCurrent(
    const wxWinUINativeHwndIdentity& identity)
{
    return identity.hwnd && identity.generation &&
           ::IsWindow(identity.hwnd) &&
           wxWinUIMSWGetNativeHwndGeneration(
               reinterpret_cast<WXHWND>(identity.hwnd)) ==
               identity.generation;
}

bool wxWinUINativeIdentityIsHiddenOrGone(
    const wxWinUINativeHwndIdentity& identity)
{
    return !wxWinUINativeIdentityIsCurrent(identity) ||
           !::IsWindowVisible(identity.hwnd);
}

bool wxWinUIHideExactNativeIdentity(
    const wxWinUINativeHwndIdentity& identity)
{
    if ( wxWinUINativeIdentityIsCurrent(identity) &&
            ::IsWindowVisible(identity.hwnd) )
    {
        (void)::ShowWindow(identity.hwnd, SW_HIDE);
    }

    return wxWinUINativeIdentityIsHiddenOrGone(identity);
}

bool wxWinUIGetExactPopupCapture(
    const wxWinUINativeHwndIdentity& popupIdentity,
    wxWinUINativeHwndIdentity *captureIdentity)
{
    if ( !captureIdentity ||
            !wxWinUINativeIdentityIsCurrent(popupIdentity) )
    {
        return false;
    }

    const HWND capture = ::GetCapture();
    if ( !capture ||
            (capture != popupIdentity.hwnd &&
             !::IsChild(popupIdentity.hwnd, capture)) )
    {
        return false;
    }

    const unsigned long long generation =
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(capture));
    if ( !generation )
        return false;

    captureIdentity->hwnd = capture;
    captureIdentity->generation = generation;
    return true;
}

bool wxWinUIReleaseExactPopupCapture(
    wxPopupWindow *popup,
    const wxWinUINativeHwndIdentity& popupIdentity,
    const wxWinUINativeHwndIdentity& captureIdentity)
{
    if ( wxWinUIPopupIsUnavailable(popup) ||
            !wxWinUINativeIdentityIsCurrent(popupIdentity) ||
            !wxWinUINativeIdentityIsCurrent(captureIdentity) ||
            ::GetCapture() != captureIdentity.hwnd ||
            (captureIdentity.hwnd != popupIdentity.hwnd &&
             !::IsChild(popupIdentity.hwnd, captureIdentity.hwnd)) )
    {
        return false;
    }

    wxWindow * const captureWindow =
        wxFindWinFromHandle(captureIdentity.hwnd);
    if ( captureWindow &&
            captureWindow->GetHWND() ==
                reinterpret_cast<WXHWND>(captureIdentity.hwnd) &&
            (captureWindow == popup || popup->IsDescendant(captureWindow)) )
    {
        return wxWinUIPopupReleaseCapture(captureWindow);
    }

    // The island bridge and its input-site descendants deliberately have no
    // wxWindow wrapper. Notify the owning popup host, then revalidate both raw
    // identities before releasing only this exact native capture authority.
    wxWinUITLWHostNotifyCaptureMutation(popup);
    if ( !wxWinUINativeIdentityIsCurrent(popupIdentity) ||
            !wxWinUINativeIdentityIsCurrent(captureIdentity) ||
            ::GetCapture() != captureIdentity.hwnd ||
            (captureIdentity.hwnd != popupIdentity.hwnd &&
             !::IsChild(popupIdentity.hwnd, captureIdentity.hwnd)) )
    {
        return false;
    }

    if ( !::ReleaseCapture() && ::GetCapture() == captureIdentity.hwnd )
        wxLogLastError(wxT("ReleaseCapture(popup native authority)"));
    return ::GetCapture() != captureIdentity.hwnd;
}

bool wxWinUIPopupCancelTokenMatchesGeneration(
    wxPopupWindow *popup,
    const std::shared_ptr<const wxWinUIPopupCancelToken>& token)
{
    const wxWinUIPopupState * const state =
        wxWinUIFindPopupState(popup);
    wxWindow * const owner = token ? token->owner.get() : nullptr;
    return token && owner && state && state->registration &&
           state->registration->GetGeneration() ==
               token->registrationGeneration &&
           state->registeredOwner.get() == owner &&
           owner->GetHWND() == token->ownerHwnd &&
           token->ownerHwndGeneration &&
           wxWinUIMSWGetHwndGeneration(owner, token->ownerHwnd) ==
               token->ownerHwndGeneration &&
           popup->GetHWND() == token->popupHwnd &&
           token->popupNativeHwndGeneration &&
           wxWinUIMSWGetNativeHwndGeneration(token->popupHwnd) ==
               token->popupNativeHwndGeneration &&
           wxWinUIPopupSessionIsCurrent(popup, token->sessionEpoch);
}

bool wxWinUIPopupCancelTokenIsCurrent(
    wxPopupWindow *popup,
    const std::shared_ptr<const wxWinUIPopupCancelToken>& token)
{
    return wxWinUIPopupCancelTokenMatchesGeneration(popup, token) &&
           wxWinUIGetPopupLogicalOwner(popup) == token->owner.get();
}

bool wxWinUIPopupRegistrationIsCurrent(wxPopupWindow *popup,
                                        wxWindow *owner,
                                        std::uint64_t generation)
{
    const wxWinUIPopupState * const state =
        wxWinUIFindPopupState(popup);
    return state && state->registration && *state->registration &&
           state->registration->GetGeneration() == generation &&
           state->registeredOwner.get() == owner;
}

wxPopupWindow *wxWinUIResolvePopupGeneration(
    const wxWeakRef<wxWindow>& identity,
    wxPopupWindow *expected,
    WXHWND expectedHwnd,
    unsigned long long expectedHwndGeneration,
    unsigned long long expectedEpoch)
{
    wxPopupWindow * const popup = wxWinUIResolvePopup(identity, expected);
    return popup && expectedHwnd && expectedHwndGeneration &&
                   popup->GetHWND() == expectedHwnd &&
                   wxWinUIMSWGetHwndGeneration(popup, expectedHwnd) ==
                       expectedHwndGeneration &&
                   wxWinUIPopupSessionIsCurrent(popup, expectedEpoch)
               ? popup
               : nullptr;
}

std::vector<wxWeakRef<wxWindow>> gs_winuiPopupStack;

void wxWinUIWithdrawCurrentPopup(wxPopupWindow *popup)
{
    gs_winuiPopupStack.erase(
        std::remove_if(
            gs_winuiPopupStack.begin(),
            gs_winuiPopupStack.end(),
            [popup](const wxWeakRef<wxWindow>& weak)
            {
                return !weak || weak.get() == popup;
            }),
        gs_winuiPopupStack.end());

    wxCurrentPopupWindow = nullptr;
    for ( auto it = gs_winuiPopupStack.rbegin();
          it != gs_winuiPopupStack.rend();
          ++it )
    {
        wxPopupWindow * const candidate =
            wxDynamicCast(it->get(), wxPopupWindow);
        if ( !wxWinUIPopupIsUnavailable(candidate) &&
                candidate->IsShown() )
        {
            wxCurrentPopupWindow = candidate;
            break;
        }
    }
}

void wxWinUIPublishCurrentPopup(wxPopupWindow *popup)
{
    wxWinUIWithdrawCurrentPopup(popup);
    gs_winuiPopupStack.emplace_back(popup);
    wxCurrentPopupWindow = popup;
}

enum class wxWinUIPopupOwnerPublishResult
{
    Published,
    WindowGone,
    Failed
};

wxWinUIPopupOwnerPublishResult
wxWinUIPublishPopupNativeOwner(
    wxPopupWindow *popup,
    wxWindow *owner,
    const wxWinUITransientRegistration *expectedRegistration)
{
    const auto registrationIsActive =
        [expectedRegistration]()
        {
            return !expectedRegistration || *expectedRegistration;
        };
    if ( !popup->HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        return registrationIsActive()
                   ? wxWinUIPopupOwnerPublishResult::Published
                   : wxWinUIPopupOwnerPublishResult::Failed;
    }

    const wxWeakRef<wxWindow> popupWeak(popup);
    const wxWeakRef<wxWindow> ownerWeak(owner);
    const WXHWND popupHwnd = popup->GetHWND();
    const WXHWND ownerHwnd = owner ? owner->GetHWND() : nullptr;
    const unsigned long long popupGeneration =
        wxWinUIMSWGetHwndGeneration(popup, popupHwnd);
    const unsigned long long popupEpoch =
        wxWinUIGetPopupSessionEpoch(popup);
    const unsigned long long ownerGeneration =
        owner ? wxWinUIMSWGetHwndGeneration(owner, ownerHwnd) : 0;
    if ( !popupHwnd || !popupGeneration ||
            (owner && (!ownerHwnd || !ownerGeneration)) )
    {
        return wxWinUIPopupOwnerPublishResult::Failed;
    }

    const HWND hwnd = reinterpret_cast<HWND>(popupHwnd);
    const HWND desiredOwner = reinterpret_cast<HWND>(ownerHwnd);
    const HWND oldOwner = ::GetWindow(hwnd, GW_OWNER);
    if ( oldOwner == desiredOwner )
    {
        return registrationIsActive()
                   ? wxWinUIPopupOwnerPublishResult::Published
                   : wxWinUIPopupOwnerPublishResult::Failed;
    }

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR previous = ::SetWindowLongPtr(
        hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(desiredOwner));
    const DWORD error = ::GetLastError();
    if ( !previous && error != ERROR_SUCCESS )
        return wxWinUIPopupOwnerPublishResult::Failed;
    if ( !registrationIsActive() )
        return wxWinUIPopupOwnerPublishResult::Failed;

    // SetWindowLongPtr() is a synchronous USER32 boundary in production. The
    // hook makes the otherwise timing-dependent nested-callback cases fully
    // deterministic without changing the production path.
    wxPopupWindow *live = wxWinUIResolvePopupGeneration(
        popupWeak, popup, popupHwnd, popupGeneration, popupEpoch);
    if ( !live )
        return wxWinUIPopupOwnerPublishResult::WindowGone;

    if ( gs_winuiPopupOwnerPublishHookForTesting )
        gs_winuiPopupOwnerPublishHookForTesting(live);
    if ( !registrationIsActive() )
        return wxWinUIPopupOwnerPublishResult::Failed;

    live = wxWinUIResolvePopupGeneration(
        popupWeak, popup, popupHwnd, popupGeneration, popupEpoch);
    if ( !live )
        return wxWinUIPopupOwnerPublishResult::WindowGone;

    wxWindow * const liveOwner = ownerWeak.get();
    wxWindow * const liveLogicalParent = live->GetParent();
    const bool targetStillCurrent =
        liveOwner == owner &&
        (!owner ||
            (!wxWinUIPopupIsUnavailable(owner) &&
             owner->GetHWND() == ownerHwnd &&
             wxWinUIMSWGetHwndGeneration(owner, ownerHwnd) ==
                 ownerGeneration)) &&
        (liveLogicalParent
             ? wxGetTopLevelParent(liveLogicalParent)
             : nullptr) == owner;
    if ( targetStillCurrent && ::GetWindow(hwnd, GW_OWNER) == desiredOwner )
        return wxWinUIPopupOwnerPublishResult::Published;

    // Converge the native owner on the logical topology which actually won the
    // callback. Only overwrite our own last still-observable native write: a
    // nested native publication is authoritative. Every recovery write and
    // postcondition is checked, including a deterministic injected failure.
    HWND nativeWriteOwnedByThisFrame = desiredOwner;
    constexpr unsigned MaxOwnerRecoveryPasses = 16;
    for ( unsigned pass = 0; pass < MaxOwnerRecoveryPasses; ++pass )
    {
        if ( !registrationIsActive() )
            return wxWinUIPopupOwnerPublishResult::Failed;

        live = wxWinUIResolvePopupGeneration(
            popupWeak, popup, popupHwnd, popupGeneration, popupEpoch);
        if ( !live )
            return wxWinUIPopupOwnerPublishResult::WindowGone;

        const HWND currentNativeOwner = ::GetWindow(hwnd, GW_OWNER);
        if ( currentNativeOwner != nativeWriteOwnedByThisFrame )
            return wxWinUIPopupOwnerPublishResult::Failed;

        wxWindow * const currentLogicalOwner =
            wxWinUIGetPopupLogicalOwner(live);
        const wxWeakRef<wxWindow> currentOwnerIdentity(currentLogicalOwner);
        const WXHWND currentOwnerHwnd =
            currentLogicalOwner ? currentLogicalOwner->GetHWND() : nullptr;
        const unsigned long long currentOwnerGeneration =
            currentLogicalOwner
                ? wxWinUIMSWGetHwndGeneration(
                      currentLogicalOwner, currentOwnerHwnd)
                : 0;
        if ( currentLogicalOwner &&
                (wxWinUIPopupIsUnavailable(currentLogicalOwner) ||
                 !currentOwnerHwnd || !currentOwnerGeneration) )
        {
            return wxWinUIPopupOwnerPublishResult::Failed;
        }

        const HWND recoveryOwner =
            reinterpret_cast<HWND>(currentOwnerHwnd);
        if ( currentNativeOwner == recoveryOwner )
            return wxWinUIPopupOwnerPublishResult::Failed;

        ::SetLastError(ERROR_SUCCESS);
        LONG_PTR previousRecovery = 0;
        DWORD recoveryError = ERROR_SUCCESS;
        if ( gs_winuiFailNextPopupOwnerRecoveryForTesting )
        {
            gs_winuiFailNextPopupOwnerRecoveryForTesting = false;
            recoveryError = ERROR_ACCESS_DENIED;
        }
        else
        {
            previousRecovery = ::SetWindowLongPtr(
                hwnd, GWLP_HWNDPARENT,
                reinterpret_cast<LONG_PTR>(recoveryOwner));
            recoveryError = ::GetLastError();
        }
        if ( !previousRecovery && recoveryError != ERROR_SUCCESS )
            continue;
        if ( !registrationIsActive() )
            return wxWinUIPopupOwnerPublishResult::Failed;

        nativeWriteOwnedByThisFrame = recoveryOwner;
        live = wxWinUIResolvePopupGeneration(
            popupWeak, popup, popupHwnd, popupGeneration, popupEpoch);
        if ( !live )
            return wxWinUIPopupOwnerPublishResult::WindowGone;

        if ( wxWinUIGetPopupLogicalOwner(live) == currentLogicalOwner &&
                (!currentLogicalOwner ||
                 (currentOwnerIdentity.get() == currentLogicalOwner &&
                  currentLogicalOwner->GetHWND() == currentOwnerHwnd &&
                  wxWinUIMSWGetHwndGeneration(
                      currentLogicalOwner, currentOwnerHwnd) ==
                      currentOwnerGeneration)) &&
                ::GetWindow(hwnd, GW_OWNER) == recoveryOwner )
        {
            wxWinUIPopupSidecarAccess::SetOwner(
                live, currentLogicalOwner);
            wxWinUIGetPopupState(live).hadOwner =
                live->GetParent() != nullptr;
            return wxWinUIPopupOwnerPublishResult::Failed;
        }
    }

    wxLogLastError(wxT("SetWindowLongPtr(GWLP_HWNDPARENT recovery)"));
    return wxWinUIPopupOwnerPublishResult::Failed;
}

} // anonymous namespace

void wxWinUISetPopupOwnerPublishHookForTesting(
    wxWinUIPopupOwnerPublishHookForTesting hook)
{
    gs_winuiPopupOwnerPublishHookForTesting = hook;
}

void wxWinUIResetPopupOwnerPublishHookForTesting()
{
    gs_winuiPopupOwnerPublishHookForTesting = nullptr;
}

void wxWinUIFailNextPopupOwnerRecoveryForTesting()
{
    gs_winuiFailNextPopupOwnerRecoveryForTesting = true;
}

void wxWinUIFailNextPopupAllocationForTesting()
{
    wxWinUIFailTransientAllocationAtForTesting(
        wxWinUITransientAllocationSite::Any);
}

void wxWinUISetPopupCreateHookForTesting(
    wxWinUIPopupCreateHookForTesting hook)
{
    gs_winuiPopupCreateHookForTesting = hook;
}

void wxWinUIResetPopupCreateHookForTesting()
{
    gs_winuiPopupCreateHookForTesting = nullptr;
}

void wxWinUISetPopupOwnerDeactivateHookForTesting(
    wxWinUIPopupOwnerDeactivateHookForTesting hook)
{
    gs_winuiPopupOwnerDeactivateHookForTesting = hook;
}

void wxWinUIResetPopupOwnerDeactivateHookForTesting()
{
    gs_winuiPopupOwnerDeactivateHookForTesting = nullptr;
}

void wxWinUISetPopupCaptureReleaseHookForTesting(
    wxWinUIPopupCaptureReleaseHookForTesting hook)
{
    gs_winuiPopupCaptureReleaseHookForTesting = hook;
}

void wxWinUIResetPopupCaptureReleaseHookForTesting()
{
    gs_winuiPopupCaptureReleaseHookForTesting = nullptr;
}

void wxWinUIFailNextPopupTerminalNativeIdentityForTesting()
{
    gs_failNextPopupTerminalNativeIdentityForTesting = true;
}

bool wxWinUIPopupBlocksCapture(wxWindow *window)
{
    wxPopupWindow * const popup = wxDynamicCast(
        window ? wxGetTopLevelParent(window) : nullptr,
        wxPopupWindow);
    const wxWinUIPopupState * const state =
        popup ? wxWinUIFindPopupState(popup) : nullptr;
    return state && state->terminalCancellation;
}
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    // popup windows are created hidden by default
    Hide();

    m_owner = wxGetTopLevelParent(parent);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWinUIGetPopupState(this).hadOwner = parent != nullptr;
#endif

    if ( !wxPopupWindowBase::Create(parent) )
        return false;

    if ( parent )
    {
        return wxWindow::Create(parent, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize,
                                flags);
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // wxWindow::Create() is a child-window helper and rejects a null logical
    // parent before MSWGetParent() can provide the desktop HWND. An ownerless
    // popup is nevertheless a supported non-owned window. Publish the same
    // wx base state and native styles directly, without inventing a wx parent
    // that would change GetParent(), lifetime, or transient-manager semantics.
    if ( !CreateBase(nullptr, wxID_ANY,
                     wxDefaultPosition, wxDefaultSize, flags,
                     wxASCII_STR(wxPanelNameStr)) )
    {
        return false;
    }

    WXDWORD exstyle;
    const WXDWORD msflags = MSWGetCreateWindowFlags(&exstyle);
    const wxWeakRef<wxWindow> weakThis(this);
    if ( !MSWCreate(GetMSWClassName(flags), nullptr,
                    wxDefaultPosition, wxDefaultSize, msflags, exstyle) )
    {
        return false;
    }

    wxPopupWindow *live = wxDynamicCast(weakThis.get(), wxPopupWindow);
    if ( wxWinUIPopupIsUnavailable(live) )
        return false;

    const WXHWND createdHwnd = live->GetHWND();
    const unsigned long long createdGeneration =
        wxWinUIMSWGetHwndGeneration(live, createdHwnd);
    if ( !createdHwnd || !createdGeneration )
        return false;

    if ( gs_winuiPopupCreateHookForTesting )
        gs_winuiPopupCreateHookForTesting(live);

    live = wxDynamicCast(weakThis.get(), wxPopupWindow);
    if ( wxWinUIPopupIsUnavailable(live) ||
            live->GetHWND() != createdHwnd ||
            wxWinUIMSWGetHwndGeneration(live, createdHwnd) !=
                createdGeneration )
    {
        return false;
    }

    live->InheritAttributes();
    live = wxDynamicCast(weakThis.get(), wxPopupWindow);
    return !wxWinUIPopupIsUnavailable(live) &&
           live->GetHWND() == createdHwnd &&
           wxWinUIMSWGetHwndGeneration(live, createdHwnd) ==
               createdGeneration;
#else
    return false;
#endif
}

wxPopupWindow::~wxPopupWindow()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWinUIWithdrawCurrentPopup(this);
    wxWinUIEndPopupTransientSession(this);
    wxWinUIErasePopupState(this);
#endif

    // If the popup is destroyed without being hidden first, ensure that we are
    // not left with a dangling pointer.
    if ( wxCurrentPopupWindow == this )
        wxCurrentPopupWindow = nullptr;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3
bool wxPopupWindow::Destroy()
{
    if ( wxWinUITLWHostIsDestroyScheduled(this) )
    {
        wxFAIL_MSG(wxS("Shouldn't destroy the popup twice."));
        return false;
    }

    if ( !wxWinUIPopupPrepareForDestroy(this) )
        return true;

    const wxWinUIDestroyDeferralResult deferred =
        wxWinUITLWHostDeferPopupDestroy(
            this, wxWinUIPopupDestroySemantics::Immediate);
    if ( deferred != wxWinUIDestroyDeferralResult::NotDeferred )
        return true;

    return wxPopupWindowBase::Destroy();
}
#endif

WXDWORD wxPopupWindow::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    // we only honour the border flags, the others don't make sense for us
    WXDWORD style = wxWindow::MSWGetStyle(flags & wxBORDER_MASK, exstyle);

    // wxMSW uses 2 rather different implementations of wxPopupWindow
    // internally, each one with its own limitations, so we allow specifying
    // wxPU_CONTAINS_CONTROLS flag to select which one is used. The default is
    // to use a child window of the desktop for the popup, which is compatible
    // with the previous wxWidgets versions and works well for simple popups,
    // but many standard controls can't work as children of such a window
    // because it doesn't accept focus. So an alternative implementation for
    // the popups that will contain such controls is available, but this one
    // has problems due to the fact that it does take focus and not only can
    // (and does) this break existing code, but it also prevents the parent
    // window from keeping focus while showing the popup, as must be done when
    // using auto-completion tooltips, for example. So neither implementation
    // can be used in all cases and you have to explicitly choose your poison.
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        style &= ~WS_CHILD;
        style |= WS_POPUP;
    }

    if ( exstyle )
    {
        // a popup window floats on top of everything
        *exstyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    }

    return style;
}

WXHWND wxPopupWindow::MSWGetParent() const
{
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        return wxPopupWindowBase::MSWGetParent();
    }
    else
    {
        // we must be a child of the desktop to be able to extend beyond the
        // parent window client area (like the comboboxes drop downs do)
        return (WXHWND)::GetDesktopWindow();
    }
}

void wxPopupWindow::SetFocus()
{
    // Focusing on a popup window does not work on MSW unless WS_POPUP style
    // is set. Since this is only the case if the style wxPU_CONTAINS_CONTROLS
    // is used, we'll handle the focus in that case and otherwise do nothing.
    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        wxPopupWindowBase::SetFocus();
    }
}

bool wxPopupWindow::Show(bool show)
{
    // Note that we're called from the ctor before the window is actually
    // created to hide the popup initially. This call doesn't really hide the
    // window, so don't do anything in this case, in particular don't change
    // wxCurrentPopupWindow value.
    if ( !GetHwnd() )
        return wxPopupWindowBase::Show(show);

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const bool wasShown = IsShown();
    const wxWeakRef<wxWindow> weakThis(this);
    wxWinUIPopupState& entryState = wxWinUIGetPopupState(this);
    const unsigned long long focusGeneration = show && !wasShown
        ? wxWinUIBumpEpoch(entryState.focusGeneration)
        : entryState.focusGeneration;
    if ( show )
    {
        (void)wxWinUIBumpEpoch(entryState.showRequestEpoch);
        if ( !wasShown )
        {
            wxWindow * const currentFocus = wxWindow::FindFocus();
            // Hide() can be followed synchronously by Popup() from CanDismiss
            // or another callback while focus is still parked on a now-hidden
            // popup child. Preserve the preceding external snapshot in that
            // case; a genuinely external focus starts a fresh provenance.
            if ( !currentFocus || !IsDescendant(currentFocus) )
            {
                entryState.focusBeforePopup = currentFocus;
            }
        }
    }
    wxScopeGuard focusSnapshotGuard = wxMakeGuard(
        [weakThis, show, wasShown, focusGeneration]()
        {
            wxPopupWindow * const live =
                wxDynamicCast(weakThis.get(), wxPopupWindow);
            if ( wxWinUIPopupIsUnavailable(live) )
                return;

            // A new Show(true) generation owns its own focus snapshot. Clear
            // only the generation this frame captured/retired and only when
            // no visible shell survived the operation. This guard starts
            // before registration/retirement allocation so bad_alloc obeys
            // the same rollback contract as an ordinary publication failure.
            if ( show && !wasShown && !live->IsShown() )
                wxWinUIClearPopupFocusSnapshot(live, focusGeneration);
        });
    wxUnusedVar(focusSnapshotGuard);
    const unsigned long long requestEpoch =
        wxWinUIGetPopupSessionEpoch(this);
    if ( show )
    {
        const auto retireFailedVisibleRequest =
            [&]()
            {
                if ( !wasShown )
                    return;

                if ( wxPopupWindow * const live =
                         wxDynamicCast(weakThis.get(), wxPopupWindow) )
                {
                    // A nested publication which committed a newer epoch is
                    // authoritative. Hide only the still-stale generation
                    // owned by this failed outer request.
                    if ( !wxWinUIPopupIsUnavailable(live) &&
                            live->IsShown() &&
                            wxWinUIGetPopupSessionEpoch(live) ==
                                requestEpoch )
                    {
                        (void)live->Show(false);
                    }
                }
            };

        // Refuse to publish a popup whose former owner is already gone, or
        // whose live owner is being retired by the central transient manager.
        bool sessionStarted = false;
        wxTRY
        {
            sessionStarted = wxWinUIBeginPopupTransientSession(this);
        }
        wxCATCH_ALL(
        {
            // Register() and every sidecar allocation provide the strong
            // rollback guarantee, but the old visible A generation is no
            // longer coherent after its logical topology moved to B. Retire
            // it before propagating the allocation failure.
            retireFailedVisibleRequest();
            throw;
        })
        if ( !sessionStarted )
        {
            // An already-visible popup must never remain published against a
            // stale owner after a failed migration. Hide it through the same
            // authoritative retirement transaction while the old token is
            // still intact.
            if ( wasShown )
            {
                retireFailedVisibleRequest();
            }
            else if ( wxPopupWindow * const live =
                          wxDynamicCast(weakThis.get(), wxPopupWindow) )
            {
                wxWinUIClearPopupFocusSnapshot(live, focusGeneration);
            }
            return false;
        }

        if ( !wasShown )
            wxWinUIBumpPopupEpoch(wxWinUIGetPopupState(this));
    }
    else
    {
        if ( !wxWinUIBeginPopupEndingSession(this) )
            return false;

        // Invalidate queued deactivation callbacks and withdraw from global
        // outside routing before any nested cancellation callback can run.
        wxWinUIBumpPopupEpoch(wxWinUIGetPopupState(this));
        wxWinUIWithdrawCurrentPopup(this);

        // Hiding an outer popup also closes every nested popup registered
        // against its shell. The manager removes entries before callbacks,
        // so a child OnDismiss() can safely destroy this outer object.
        wxWinUICancelTransientSessions(this);
        wxPopupWindow * const live =
            wxDynamicCast(weakThis.get(), wxPopupWindow);
        if ( wxWinUIPopupIsUnavailable(live) )
            return true;
        if ( wxWinUITLWHostIsDestroyScheduled(live) )
            return true;

        // Unregister before USER32 dispatch: a hide handler may destroy the
        // owner or re-show this popup with a fresh generation.
        wxWinUIEndPopupTransientSession(live);
    }

    const bool finishEndingSession = !show;
    wxScopeGuard endingSessionGuard = wxMakeGuard(
        [weakThis, finishEndingSession]()
        {
            if ( !finishEndingSession )
                return;

            if ( wxPopupWindow * const live =
                     wxDynamicCast(weakThis.get(), wxPopupWindow) )
            {
                wxWinUIFinishPopupEndingSession(live);
            }
        });
    wxUnusedVar(endingSessionGuard);

#endif

    // It's important to update wxCurrentPopupWindow before showing the window,
    // to ensure that it's already set by the time the owner gets WM_NCACTIVATE
    // from inside Show() so that it knows to remain [appearing] active, see
    // the WM_NCACTIVATE handler in wxWindow::MSWHandleMessage().
    if ( show )
    {
        // There could have been a previous popup window which hasn't been
        // hidden yet. This will happen now, when we show this one, as it will
        // result in activation loss for the other one, so it's ok to overwrite
        // the old pointer, even if it's non-null.
#if defined(__WXWINUI__) && wxUSE_WINUI3
        wxWinUIPublishCurrentPopup(this);
#else
        wxCurrentPopupWindow = this;
#endif
    }
    else
    {
        // Only reset the pointer if it points to this window, otherwise we
        // would lose the correct value in the situation described above.
#if defined(__WXWINUI__) && wxUSE_WINUI3
        wxWinUIWithdrawCurrentPopup(this);
#else
        if ( wxCurrentPopupWindow == this )
            wxCurrentPopupWindow = nullptr;
#endif
    }

    if ( HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        const bool shown = wxPopupWindowBase::Show(show);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        wxPopupWindow * const liveAfterShow =
            wxDynamicCast(weakThis.get(), wxPopupWindow);
        if ( show && !wasShown &&
                !wxWinUIPopupIsUnavailable(liveAfterShow) )
        {
            if ( liveAfterShow->IsShown() )
            {
                wxWinUIGetPopupState(liveAfterShow)
                    .dismissNotificationDelivered = false;
            }
            else
            {
                wxWinUIEndPopupTransientSession(liveAfterShow);
                wxWinUIWithdrawCurrentPopup(liveAfterShow);
            }
        }
#endif
        return shown;
    }
    else
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        const WXHWND showHwnd = show ? GetHWND() : nullptr;
        const unsigned long long showHwndGeneration =
            show ? wxWinUIMSWGetHwndGeneration(this, showHwnd) : 0;
        const unsigned long long showEpoch =
            show ? wxWinUIGetPopupSessionEpoch(this) : 0;
#endif
        if ( !wxWindowMSW::Show(show) )
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            if ( show && !wasShown && weakThis )
            {
                if ( wxPopupWindow * const live =
                         wxDynamicCast(weakThis.get(), wxPopupWindow) )
                {
                    if ( !wxWinUIPopupIsUnavailable(live) &&
                            !live->IsShown() )
                    {
                        wxWinUIEndPopupTransientSession(live);
                        wxWinUIWithdrawCurrentPopup(live);
                    }
                }
            }
#endif
            return false;
        }

        if ( show )
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            // ShowWindow() is a synchronous event boundary.
            wxPopupWindow * const liveAfterShow =
                wxWinUIResolvePopupGeneration(
                    weakThis, this, showHwnd, showHwndGeneration,
                    showEpoch);
            if ( !liveAfterShow || !liveAfterShow->IsShown() )
                return true;
            if ( !wasShown )
            {
                wxWinUIGetPopupState(liveAfterShow)
                    .dismissNotificationDelivered = false;
            }
            const HWND hwnd = GetHwndOf(liveAfterShow);
            if ( !hwnd )
                return true;
#else
            const HWND hwnd = GetHwnd();
#endif
            // raise to top of z order
            if ( !::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE) )
            {
                wxLogLastError(wxT("SetWindowPos"));
            }

#if defined(__WXWINUI__) && wxUSE_WINUI3
            // SetWindowPos() may synchronously run application callbacks too.
            wxPopupWindow * const liveAfterRaise =
                wxWinUIResolvePopupGeneration(
                    weakThis, this, showHwnd, showHwndGeneration,
                    showEpoch);
            if ( !liveAfterRaise || !liveAfterRaise->IsShown() ||
                    GetHwndOf(liveAfterRaise) != hwnd )
            {
                return true;
            }
#endif
            // and set it as the foreground window so the mouse can be captured
            ::SetForegroundWindow(hwnd);
        }

        return true;
    }
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

bool wxPopupWindow::Reparent(wxWindowBase *newParentBase)
{
    const wxWinUIPopupState * const entryState =
        wxWinUIFindPopupState(this);
    if ( entryState &&
            (entryState->endingSession || entryState->destroying) )
    {
        return false;
    }

    const auto isUsableOwner =
        [](wxWindow *owner)
        {
            if ( !owner || owner->IsBeingDeleted() ||
                    wxWinUITLWHostIsDestroyScheduled(owner) )
            {
                return false;
            }

            const WXHWND hwnd = owner->GetHWND();
            return hwnd && wxWinUIMSWGetHwndGeneration(owner, hwnd);
        };

    wxWindow * const requestedParent =
        static_cast<wxWindow *>(newParentBase);
    if ( requestedParent == GetParent() )
        return false;

    const wxWeakRef<wxWindow> requestedParentIdentity(requestedParent);
    wxWindow * const initialRequestedOwner =
        requestedParent ? wxGetTopLevelParent(requestedParent) : nullptr;
    if ( requestedParent && !isUsableOwner(initialRequestedOwner) )
    {
        return false;
    }

    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> oldParentIdentity(GetParent());
    const bool wasShown = IsShown();
    if ( wasShown )
    {
        (void)Hide();
        wxPopupWindow * const live =
            wxDynamicCast(weakThis.get(), wxPopupWindow);
        if ( wxWinUIPopupIsUnavailable(live) )
            return true;
        if ( live->IsShown() )
            return false;

        // A nested callback owns any topology change it completed first.
        if ( live->GetParent() != oldParentIdentity.get() )
            return true;
    }

    wxPopupWindow * const liveBeforeReparent =
        wxDynamicCast(weakThis.get(), wxPopupWindow);
    if ( wxWinUIPopupIsUnavailable(liveBeforeReparent) )
        return true;

    // Hide() is a synchronous application boundary: don't retain either the
    // requested logical parent or its TLW owner across it.
    wxWindow * const liveRequestedParent = requestedParentIdentity.get();
    if ( requestedParent && !liveRequestedParent )
    {
        if ( wasShown )
            (void)liveBeforeReparent->Show();
        return false;
    }
    wxWindow * const liveRequestedOwner =
        liveRequestedParent ? wxGetTopLevelParent(liveRequestedParent)
                            : nullptr;
    if ( liveRequestedParent && !isUsableOwner(liveRequestedOwner) )
    {
        if ( wasShown )
            (void)liveBeforeReparent->Show();
        return false;
    }

    // The default popup is deliberately a desktop child so it can extend
    // outside its logical owner. Keep that native topology; controlled
    // popups use WS_POPUP and must publish a new *owner*, never call
    // SetParent(). wxNonOwnedWindow::Reparent() provides the checked,
    // re-entrancy-safe GWLP_HWNDPARENT transaction used by all WinUI TLWs.
    const bool hasControlledContents =
        liveBeforeReparent->HasFlag(wxPU_CONTAINS_CONTROLS);
    const bool reparented = hasControlledContents
        ? liveBeforeReparent->wxNonOwnedWindow::Reparent(liveRequestedParent)
        : liveBeforeReparent->wxWindowBase::Reparent(liveRequestedParent);
    wxPopupWindow *live =
        wxDynamicCast(weakThis.get(), wxPopupWindow);
    if ( wxWinUIPopupIsUnavailable(live) )
        return reparented;

    // A default popup deliberately remains a desktop child, so its qualified
    // wxWindowBase::Reparent() changes only the logical parent. Unlike the
    // controlled/non-owned path above, it doesn't pass through
    // wxWindowMSW::Reparent() and therefore can't refresh an inherited
    // WS_EX_LAYOUTRTL projection itself.
    if ( !hasControlledContents && reparented )
    {
        live->MSWRefreshInheritedLayoutDirection();
        live = wxDynamicCast(weakThis.get(), wxPopupWindow);
        if ( wxWinUIPopupIsUnavailable(live) )
            return true;
    }

    // The qualified transaction deliberately lets a nested Reparent() win.
    // Mirror the topology it actually committed instead of republishing the
    // stale outer request in the transient registry.
    wxWindow * const actualParent = live->GetParent();
    wxWindow * const actualOwner =
        actualParent ? wxGetTopLevelParent(actualParent) : nullptr;
    live->m_owner = isUsableOwner(actualOwner) ? actualOwner : nullptr;
    wxWinUIGetPopupState(live).hadOwner = actualParent != nullptr;
    if ( wasShown && !live->IsBeingDeleted() && !live->IsShown() )
        (void)live->Show();
    return reparented;
}

namespace
{

bool wxWinUIOwnerAcceptsPopup(wxWindow *owner)
{
    if ( wxWinUIPopupIsUnavailable(owner) )
        return false;

    if ( wxPopupWindow * const ownerPopup =
             wxDynamicCast(owner, wxPopupWindow) )
    {
        const wxWinUIPopupState * const ownerState =
            wxWinUIFindPopupState(ownerPopup);
        if ( !ownerPopup->IsShown() ||
                (ownerState &&
                 (ownerState->endingSession || ownerState->destroying)) )
        {
            return false;
        }
    }

    return true;
}

wxPopupWindow *wxWinUIValidatePopupSession(
    wxPopupWindow *expectedPopup,
    const wxWeakRef<wxWindow>& popupIdentity,
    WXHWND popupHwnd,
    unsigned long long popupHwndGeneration,
    unsigned long long popupNativeHwndGeneration,
    unsigned long long sessionEpoch,
    bool expectedShown,
    wxWindow *expectedOwner,
    const wxWeakRef<wxWindow>& ownerIdentity,
    WXHWND ownerHwnd,
    unsigned long long ownerHwndGeneration)
{
    wxPopupWindow * const popup =
        wxWinUIResolvePopup(popupIdentity, expectedPopup);
    if ( !popup || popup->GetHWND() != popupHwnd ||
            !popupHwndGeneration ||
            wxWinUIMSWGetHwndGeneration(popup, popupHwnd) !=
                popupHwndGeneration ||
            !popupNativeHwndGeneration ||
            wxWinUIMSWGetNativeHwndGeneration(popupHwnd) !=
                popupNativeHwndGeneration ||
            popup->IsShown() != expectedShown )
    {
        return nullptr;
    }

    const wxWinUIPopupState * const state =
        wxWinUIFindPopupState(popup);
    if ( !state || state->sessionEpoch != sessionEpoch ||
            state->endingSession || state->destroying )
    {
        return nullptr;
    }

    wxWindow * const logicalParent = popup->GetParent();
    wxWindow * const currentOwner =
        logicalParent ? wxGetTopLevelParent(logicalParent) : nullptr;
    if ( currentOwner != expectedOwner )
        return nullptr;

    if ( expectedOwner )
    {
        if ( ownerIdentity.get() != expectedOwner ||
                !wxWinUIOwnerAcceptsPopup(expectedOwner) ||
                expectedOwner->GetHWND() != ownerHwnd ||
                !ownerHwndGeneration ||
                wxWinUIMSWGetHwndGeneration(expectedOwner, ownerHwnd) !=
                    ownerHwndGeneration )
        {
            return nullptr;
        }
    }

    return popup;
}

bool wxWinUIBeginPopupTransientSession(wxPopupWindow *popup)
{
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.endingSession || initialState.destroying ||
            initialState.terminalCancellation ||
            wxWinUIPopupIsUnavailable(popup) )
    {
        return false;
    }

    const unsigned long long expectedEpoch = initialState.sessionEpoch;
    const bool expectedShown = popup->IsShown();
    const wxWeakRef<wxWindow> popupIdentity(popup);
    const WXHWND popupHwnd = popup->GetHWND();
    const unsigned long long popupHwndGeneration =
        wxWinUIMSWGetHwndGeneration(popup, popupHwnd);
    const unsigned long long popupNativeHwndGeneration =
        wxWinUIMSWGetNativeHwndGeneration(popupHwnd);
    if ( !popupHwnd || !popupHwndGeneration ||
            !popupNativeHwndGeneration )
        return false;

    wxWindow * const logicalParent = popup->GetParent();
    wxWindow * const owner =
        logicalParent ? wxGetTopLevelParent(logicalParent) : nullptr;
    const wxWeakRef<wxWindow> ownerIdentity(owner);
    const WXHWND ownerHwnd = owner ? owner->GetHWND() : nullptr;
    const unsigned long long ownerHwndGeneration =
        owner ? wxWinUIMSWGetHwndGeneration(owner, ownerHwnd) : 0;

    if ( !owner )
    {
        // Ownerless-at-creation popups are intentionally caller-owned. A
        // popup which lost a former owner must not silently become ownerless.
        if ( initialState.hadOwner )
            return false;

        wxWinUIEndPopupTransientSession(popup);
        wxPopupWindow * const live =
            wxWinUIResolvePopup(popupIdentity, popup);
        if ( !live )
            return false;

        wxWinUIPopupSidecarAccess::SetOwner(live, nullptr);
        return true;
    }

    if ( !ownerHwnd || !ownerHwndGeneration ||
            !wxWinUIOwnerAcceptsPopup(owner) )
    {
        return false;
    }

    const wxWinUIPopupState *state = wxWinUIFindPopupState(popup);
    if ( state && state->registration && *state->registration &&
            state->registeredOwner.get() == owner )
    {
        const std::uint64_t registrationGeneration =
            state->registration->GetGeneration();
        const wxWinUIPopupOwnerPublishResult published =
            wxWinUIPublishPopupNativeOwner(
                popup, owner, state->registration.get());
        wxPopupWindow * const live = wxWinUIValidatePopupSession(
            popup, popupIdentity, popupHwnd, popupHwndGeneration,
            popupNativeHwndGeneration, expectedEpoch, expectedShown,
            owner, ownerIdentity,
            ownerHwnd, ownerHwndGeneration);
        if ( published != wxWinUIPopupOwnerPublishResult::Published || !live ||
                !wxWinUIPopupRegistrationIsCurrent(
                    live, owner, registrationGeneration) )
        {
            return false;
        }

        wxWinUIPopupSidecarAccess::SetOwner(live, owner);
        return true;
    }

    // Registering can allocate, bind an owner observer and synchronously cross
    // framework code. The sidecar epoch and both identities are revalidated
    // before native publication, not only after it.
    wxWinUITransientAllocationPointForTesting(
        wxWinUITransientAllocationSite::PopupCancelToken);
    const auto cancelToken =
        std::make_shared<wxWinUIPopupCancelToken>();
    cancelToken->owner = ownerIdentity;
    cancelToken->ownerHwnd = ownerHwnd;
    cancelToken->ownerHwndGeneration = ownerHwndGeneration;
    cancelToken->popupHwnd = popupHwnd;
    cancelToken->popupNativeHwndGeneration =
        popupNativeHwndGeneration;

    wxWinUITransientRegistration registration =
        wxWinUIRegisterTransient(
            owner,
            wxWinUITransientKind::Popup,
            [popupIdentity, popup, cancelToken]()
            {
                if ( wxPopupWindow * const live =
                         wxWinUIResolvePopup(popupIdentity, popup) )
                {
                    wxWinUICancelPopupTransient(live, cancelToken);
                }
            });
    if ( registration )
    {
        cancelToken->registrationGeneration =
            registration.GetGeneration();
    }
    if ( !registration ||
            !wxWinUIValidatePopupSession(
                popup, popupIdentity, popupHwnd, popupHwndGeneration,
                popupNativeHwndGeneration, expectedEpoch, expectedShown,
                owner, ownerIdentity,
                ownerHwnd, ownerHwndGeneration) )
    {
        return false;
    }

    std::unique_ptr<wxWinUITransientRegistration> newRegistration =
        wxWinUIMakePopupUnique<wxWinUITransientRegistration>(
            wxWinUITransientAllocationSite::PopupRegistrationHolder,
            std::move(registration));
    if ( !*newRegistration )
        return false;

    const wxWinUIPopupOwnerPublishResult published =
        wxWinUIPublishPopupNativeOwner(
            popup, owner, newRegistration.get());
    wxPopupWindow * const live = wxWinUIValidatePopupSession(
        popup, popupIdentity, popupHwnd, popupHwndGeneration,
        popupNativeHwndGeneration, expectedEpoch, expectedShown,
        owner, ownerIdentity,
        ownerHwnd, ownerHwndGeneration);
    if ( published != wxWinUIPopupOwnerPublishResult::Published || !live ||
            !*newRegistration )
    {
        // Native publication may synchronously cancel the destination owner
        // and invalidate this still-local registration. Keep the native and
        // cpp owner projections coherent, but never commit the inactive token.
        if ( live && wxWinUIGetPopupLogicalOwner(live) == owner &&
                ownerIdentity.get() == owner &&
                !wxWinUIPopupIsUnavailable(owner) &&
                owner->GetHWND() == ownerHwnd && ownerHwndGeneration &&
                wxWinUIMSWGetHwndGeneration(owner, ownerHwnd) ==
                    ownerHwndGeneration &&
                ::GetWindow(
                    reinterpret_cast<HWND>(popupHwnd), GW_OWNER) ==
                    reinterpret_cast<HWND>(ownerHwnd) )
        {
            wxWinUIPopupSidecarAccess::SetOwner(live, owner);
        }
        return false;
    }

    // Commit last. Never retain a reference into the unordered sidecar across
    // either registration or USER32 publication: nested popup construction may
    // rehash it, and a recycled address must receive a fresh weak identity.
    state = wxWinUIFindPopupState(live);
    if ( !state || state->sessionEpoch != expectedEpoch ||
            !*newRegistration )
        return false;

    wxWinUIPopupState& commitState = wxWinUIGetPopupState(live);
    if ( !*newRegistration )
        return false;
    newRegistration.swap(commitState.registration);
    commitState.registeredOwner = ownerIdentity;
    wxWinUIPopupSidecarAccess::SetOwner(live, owner);
    wxWinUIBumpPopupEpoch(commitState);
    cancelToken->sessionEpoch = commitState.sessionEpoch;
    if ( !expectedShown )
    {
        // Show(true) publishes one final visibility epoch immediately after
        // this function returns. Pre-authorize exactly that deterministic
        // commit so the manager token is current for the visible generation.
        if ( ++cancelToken->sessionEpoch == 0 )
            ++cancelToken->sessionEpoch;
    }
    return true;
}

bool wxWinUIBeginPopupEndingSession(wxPopupWindow *popup)
{
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.endingSession || initialState.destroying ||
            wxWinUIPopupIsUnavailable(popup) )
    {
        return false;
    }

    const unsigned long long expectedEpoch = initialState.sessionEpoch;
    const wxWeakRef<wxWindow> identity(popup);
    initialState.endingSession = true;
    bool committed = false;
    wxScopeGuard rollbackGuard = wxMakeGuard(
        [identity, popup, expectedEpoch, &committed]()
        {
            if ( committed )
                return;

            wxPopupWindow * const live =
                wxWinUIResolvePopup(identity, popup);
            wxWinUIPopupState * state =
                live ? wxWinUIFindPopupState(live) : nullptr;
            if ( state && state->sessionEpoch == expectedEpoch &&
                    !state->destroying )
            {
                state->retirement.reset();
                state = wxWinUIFindPopupState(live);
                if ( state && state->sessionEpoch == expectedEpoch &&
                        !state->destroying )
                {
                    state->endingSession = false;
                }
            }
        });
    wxUnusedVar(rollbackGuard);

    wxWinUITransientOwnerRetirement retirement =
        wxWinUIBeginTransientOwnerRetirement(popup);

    wxPopupWindow * const live = wxWinUIResolvePopup(identity, popup);
    wxWinUIPopupState * const state =
        live ? wxWinUIFindPopupState(live) : nullptr;
    if ( !retirement || !state || state->sessionEpoch != expectedEpoch ||
            !state->endingSession || state->destroying )
        return false;

    state->retirement =
        wxWinUIMakePopupUnique<wxWinUITransientOwnerRetirement>(
            wxWinUITransientAllocationSite::PopupRetirementHolder,
            std::move(retirement));
    committed = true;
    return true;
}

void wxWinUIFinishPopupEndingSession(wxPopupWindow *popup)
{
    wxWinUIPopupState *state = wxWinUIFindPopupState(popup);
    if ( !state || state->destroying )
        return;

    state->retirement.reset();
    state = wxWinUIFindPopupState(popup);
    if ( state && !state->destroying )
        state->endingSession = false;
}

void wxWinUIEndPopupTransientSession(wxPopupWindow *popup)
{
    wxWinUIPopupState *state = wxWinUIFindPopupState(popup);
    if ( !state )
        return;

    state->registration.reset();
    state = wxWinUIFindPopupState(popup);
    if ( !state )
        return;

    state->registeredOwner = nullptr;
    wxWinUIBumpPopupEpoch(*state);
}

bool wxWinUIPreparePopupForDestroy(wxPopupWindow *popup)
{
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.destroying )
        return false;

    const wxWeakRef<wxWindow> identity(popup);
    const unsigned long long originalEpoch = initialState.sessionEpoch;
    const bool originalEndingSession = initialState.endingSession;
    const bool hadRetirement = static_cast<bool>(initialState.retirement);
    initialState.destroying = true;
    initialState.endingSession = true;
    wxWinUIBumpPopupEpoch(initialState);
    const unsigned long long terminalEpoch = initialState.sessionEpoch;
    bool prepared = false;
    wxScopeGuard rollbackGuard = wxMakeGuard(
        [identity, popup, originalEpoch, originalEndingSession,
         terminalEpoch, hadRetirement, &prepared]()
        {
            if ( prepared )
                return;

            wxPopupWindow * const live =
                wxWinUIResolvePopup(identity, popup);
            wxWinUIPopupState *state =
                live ? wxWinUIFindPopupState(live) : nullptr;
            if ( !state || state->sessionEpoch != terminalEpoch ||
                    !state->destroying ||
                    wxWinUITLWHostIsDestroyScheduled(live) )
            {
                return;
            }

            if ( !hadRetirement )
                state->retirement.reset();
            state = wxWinUIFindPopupState(live);
            if ( state && state->sessionEpoch == terminalEpoch &&
                    state->destroying &&
                    !wxWinUITLWHostIsDestroyScheduled(live) )
            {
                state->sessionEpoch = originalEpoch;
                state->endingSession = originalEndingSession;
                state->destroying = false;
            }
        });
    wxUnusedVar(rollbackGuard);

    if ( !initialState.retirement )
    {
        wxWinUITransientOwnerRetirement retirement =
            wxWinUIBeginTransientOwnerRetirement(popup);
        wxPopupWindow * const live = wxWinUIResolvePopup(identity, popup);
        wxWinUIPopupState * const state =
            live ? wxWinUIFindPopupState(live) : nullptr;
        if ( !state || state->sessionEpoch != terminalEpoch ||
                !state->destroying )
        {
            return false;
        }
        if ( retirement )
        {
            state->retirement =
                wxWinUIMakePopupUnique<wxWinUITransientOwnerRetirement>(
                    wxWinUITransientAllocationSite::PopupRetirementHolder,
                    std::move(retirement));
        }
    }

    wxWinUIWithdrawCurrentPopup(popup);

    // A popup can own nested transients. Entries are withdrawn before their
    // callbacks, and the terminal epoch makes every outer continuation stale.
    wxWinUICancelTransientSessions(popup);
    wxPopupWindow * const live = wxWinUIResolvePopup(identity, popup);
    wxWinUIPopupState * const state =
        live ? wxWinUIFindPopupState(live) : nullptr;
    if ( !state || state->sessionEpoch != terminalEpoch ||
            !state->destroying )
    {
        return false;
    }

    prepared = true;
    wxWinUIEndPopupTransientSession(live);
    return wxWinUIResolvePopup(identity, popup) != nullptr;
}

} // anonymous namespace

bool wxWinUIPopupPrepareForDestroy(wxPopupWindow *popup)
{
    return popup && wxWinUIPreparePopupForDestroy(popup);
}

#endif // __WXWINUI__ && wxUSE_WINUI3

// ----------------------------------------------------------------------------
// wxPopupTransientWindow
// ----------------------------------------------------------------------------

void wxPopupTransientWindow::Popup(wxWindow* focus)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWinUIPopupState& entryState = wxWinUIGetPopupState(this);
    const unsigned long long popupInvocationEpoch =
        wxWinUIBumpEpoch(entryState.popupInvocationEpoch);
    const unsigned long long showRequestEpochBefore =
        entryState.showRequestEpoch;
    unsigned long long expectedShowRequestEpoch = showRequestEpochBefore;
    if ( ++expectedShowRequestEpoch == 0 )
        ++expectedShowRequestEpoch;
    const wxWeakRef<wxWindow> weakFocus(focus);
    const wxWeakRef<wxWindow> weakThis(this);
    (void)Show();

    wxPopupTransientWindow * const live =
        wxDynamicCast(weakThis.get(), wxPopupTransientWindow);
    const wxWinUIPopupState * const state =
        live ? wxWinUIFindPopupState(live) : nullptr;
    if ( wxWinUIPopupIsUnavailable(live) || !live->IsShown() || !state ||
            state->popupInvocationEpoch != popupInvocationEpoch ||
            state->showRequestEpoch != expectedShowRequestEpoch )
        return;
#else
    Show();
#endif

    // We can only set focus when using wxPU_CONTAINS_CONTROLS and then only to
    // one of our children as setting it to another window would result in an
    // immediate loss of activation and popup disappearance.
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWindow * const liveFocus = weakFocus.get();
#else
    wxWindow * const liveFocus = focus;
#endif
    if (
#if defined(__WXWINUI__) && wxUSE_WINUI3
         live->HasFlag(wxPU_CONTAINS_CONTROLS) && liveFocus &&
            live->IsDescendant(liveFocus)
#else
         HasFlag(wxPU_CONTAINS_CONTROLS) && liveFocus &&
            IsDescendant(liveFocus)
#endif
       )
    {
        liveFocus->SetFocus();
    }
}

void wxPopupTransientWindow::Dismiss()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(this);
    if ( initialState.dismissInProgress ||
            wxWinUIPopupIsUnavailable(this) || !IsShown() )
        return;

    // Never leave capture owned by an invisible popup descendant. Releasing
    // capture is itself an application callback boundary. A handler may move
    // capture to another descendant, so converge with a strict finite bound
    // and never hide while a popup child still owns it.
    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> focusBeforePopup(
        initialState.focusBeforePopup);
    const unsigned long long dismissFocusGeneration =
        initialState.focusGeneration;
    const WXHWND dismissHwnd = GetHWND();
    const unsigned long long dismissHwndGeneration =
        wxWinUIMSWGetHwndGeneration(this, dismissHwnd);
    const wxWinUINativeHwndIdentity dismissNativeIdentity =
        wxWinUICaptureNativeIdentity(this);
    const unsigned long long dismissEpoch = initialState.sessionEpoch;
    const auto resolveDismissGeneration = [&]()
        -> wxPopupTransientWindow*
    {
        return wxDynamicCast(
            wxWinUIResolvePopupGeneration(
                weakThis, this, dismissHwnd, dismissHwndGeneration,
                dismissEpoch),
            wxPopupTransientWindow);
    };
    // The association and native generations intentionally come from
    // different counters. Validate each against its own authority instead of
    // comparing their unrelated numeric values.
    if ( !resolveDismissGeneration() ||
            dismissNativeIdentity.hwnd !=
                reinterpret_cast<HWND>(dismissHwnd) ||
            !wxWinUINativeIdentityIsCurrent(dismissNativeIdentity) )
        return;

    initialState.dismissInProgress = true;
    wxScopeGuard dismissGuard = wxMakeGuard(
        [weakThis]()
        {
            if ( wxPopupTransientWindow * const live =
                     wxDynamicCast(weakThis.get(), wxPopupTransientWindow) )
            {
                wxWinUIPopupState * const state =
                    wxWinUIFindPopupState(live);
                if ( !state )
                    return;

                state->dismissInProgress = false;
                if ( state->managerCancelPending &&
                        !state->dismissAndNotifyInProgress )
                {
                    const auto cancelToken = state->managerCancelToken;
                    state->managerCancelPending = false;
                    state->managerCancelToken.reset();
                    wxTRY
                    {
                        wxWinUICancelPopupTransient(live, cancelToken);
                    }
                    wxCATCH_ALL(
                    {
                        // Scope-guard cleanup must not throw while another
                        // application callback is already unwinding. The
                        // authoritative cancellation path has still forced
                        // terminal retirement before propagating its error.
                    })
                }
            }
        });
    wxUnusedVar(dismissGuard);

    constexpr unsigned MaxCaptureReleasePasses = 16;
    for ( unsigned pass = 0; pass < MaxCaptureReleasePasses; ++pass )
    {
        wxPopupTransientWindow * const live = resolveDismissGeneration();
        if ( !live )
            return;

        wxWinUINativeHwndIdentity captureIdentity;
        if ( !wxWinUIGetExactPopupCapture(
                 dismissNativeIdentity, &captureIdentity) )
            break;

        (void)wxWinUIReleaseExactPopupCapture(
            live, dismissNativeIdentity, captureIdentity);

        wxPopupTransientWindow * const liveAfterRelease =
            resolveDismissGeneration();
        if ( !liveAfterRelease )
            return;

        if ( gs_winuiPopupCaptureReleaseHookForTesting )
            gs_winuiPopupCaptureReleaseHookForTesting(
                liveAfterRelease, pass);

        if ( !resolveDismissGeneration() )
            return;
    }

    wxPopupTransientWindow * const live = resolveDismissGeneration();
    if ( !live )
        return;

    wxWinUINativeHwndIdentity remainingCaptureIdentity;
    if ( wxWinUIGetExactPopupCapture(
             dismissNativeIdentity, &remainingCaptureIdentity) )
    {
        // A hostile or simply unusual handler can keep transferring capture.
        // Preserve the visible, registered generation instead of asserting or
        // reporting a dismissal that never happened.
        return;
    }

    live->Hide();
    wxPopupTransientWindow * const liveAfterHide =
        wxDynamicCast(weakThis.get(), wxPopupTransientWindow);
    if ( !wxWinUIPopupIsUnavailable(liveAfterHide) &&
            !liveAfterHide->IsShown() )
    {
        const wxWinUIPopupState * const stateAfterHide =
            wxWinUIFindPopupState(liveAfterHide);
        if ( !stateAfterHide ||
                stateAfterHide->focusGeneration != dismissFocusGeneration )
        {
            return;
        }

        // Consume exactly this focus provenance even if SetFocus() dispatches,
        // throws, destroys the popup or republishes a newer visibility
        // generation. Re-resolving the weak identity avoids clearing a new
        // object which happens to reuse the raw address.
        wxScopeGuard clearFocusSnapshot = wxMakeGuard(
            [weakThis, dismissFocusGeneration]()
            {
                wxPopupWindow * const current =
                    wxDynamicCast(weakThis.get(), wxPopupWindow);
                if ( current )
                {
                    wxWinUIClearPopupFocusSnapshot(
                        current, dismissFocusGeneration);
                }
            });
        wxUnusedVar(clearFocusSnapshot);

        wxWindow * const restoreFocus = focusBeforePopup.get();
        if ( restoreFocus && !restoreFocus->IsBeingDeleted() &&
                !wxWinUITLWHostIsDestroyScheduled(restoreFocus) &&
                restoreFocus != liveAfterHide &&
                !liveAfterHide->IsDescendant(restoreFocus) &&
                restoreFocus->GetHWND() &&
                wxWinUIMSWGetHwndGeneration(
                    restoreFocus, restoreFocus->GetHWND()) )
        {
            if ( wxWindow::FindFocus() != restoreFocus )
                restoreFocus->SetFocus();
        }

    }
#else
    Hide();
#endif
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

namespace
{

bool wxWinUINotifyDismissOnce(wxPopupTransientWindow *popup)
{
    const wxWeakRef<wxWindow> identity(popup);
    wxPopupTransientWindow * const live =
        wxDynamicCast(identity.get(), wxPopupTransientWindow);
    if ( wxWinUIPopupIsUnavailable(live) || live->IsShown() )
        return false;

    wxWinUIPopupState * const state = wxWinUIFindPopupState(live);
    if ( !state )
        return false;
    if ( state->dismissNotificationDelivered )
        return true;

    // Publish exactly-once before entering arbitrary application code. A
    // reentrant Show() starts a new visibility generation and clears this bit
    // only after the native show has actually succeeded.
    state->dismissNotificationDelivered = true;
    wxWinUIPopupSidecarAccess::NotifyDismiss(live);
    return true;
}

bool wxWinUIDismissAndNotify(wxPopupTransientWindow *popup)
{
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.dismissAndNotifyInProgress ||
            wxWinUIPopupIsUnavailable(popup) )
    {
        return false;
    }

    const wxWeakRef<wxWindow> identity(popup);
    const wxWinUINativeHwndIdentity nativeIdentity =
        wxWinUICaptureNativeIdentity(popup);
    if ( !wxWinUINativeIdentityIsCurrent(nativeIdentity) )
        return false;

    initialState.dismissAndNotifyInProgress = true;
    wxScopeGuard notifyGuard = wxMakeGuard(
        [identity]()
        {
            wxPopupTransientWindow * const live =
                wxDynamicCast(identity.get(), wxPopupTransientWindow);
            if ( !live )
                return;

            wxWinUIPopupState * const state =
                wxWinUIFindPopupState(live);
            if ( !state )
                return;

            state->dismissAndNotifyInProgress = false;
            if ( state->managerCancelPending &&
                    !state->dismissInProgress )
            {
                const auto cancelToken = state->managerCancelToken;
                state->managerCancelPending = false;
                state->managerCancelToken.reset();
                wxTRY
                {
                    wxWinUICancelPopupTransient(live, cancelToken);
                }
                wxCATCH_ALL(
                {
                    // A scope guard must not replace an exception already
                    // propagating from Dismiss() or OnDismiss().
                })
            }
        });
    wxUnusedVar(notifyGuard);

    popup->Dismiss();
    wxPopupTransientWindow *live =
        wxDynamicCast(identity.get(), wxPopupTransientWindow);
    if ( wxWinUIPopupIsUnavailable(live) )
    {
        // Dismiss() is an application override and may schedule destruction
        // without hiding the exact shell first. Never let a retired wrapper
        // leave that native generation visible while deletion is pending.
        (void)wxWinUIHideExactNativeIdentity(nativeIdentity);
        return false;
    }
    if ( live->IsShown() || !wxWinUIHideExactNativeIdentity(nativeIdentity) )
        return false;

    const bool notified = wxWinUINotifyDismissOnce(live);

    // OnDismiss() may legitimately start a new logical visibility generation;
    // the caller observes this as a failed old-generation commit and leaves the
    // new one alone. A raw SW_SHOWNA, however, changes only native visibility:
    // force that exact HWND generation hidden before reporting success.
    live = wxDynamicCast(identity.get(), wxPopupTransientWindow);
    if ( !wxWinUIPopupIsUnavailable(live) && live->IsShown() )
        return false;

    return notified && wxWinUIHideExactNativeIdentity(nativeIdentity);
}

bool wxWinUIDestroyPopupForTerminalRetirement(wxPopupWindow *popup)
{
    if ( wxWinUIPopupIsUnavailable(popup) )
        return false;

    if ( wxWinUITLWHostIsDestroyScheduled(popup) )
        return true;

    if ( !wxWinUIPopupPrepareForDestroy(popup) )
        return true;

    // wxPopupTransientWindowBase is public and directly subclassable, but it
    // deliberately has no wx class-info of its own. Consequently a custom
    // subclass cannot be distinguished from a plain wxPopupWindow with
    // wxDynamicCast(), and checking only the concrete MSW transient class
    // would bypass the base's delayed-destruction contract. Terminal fallback
    // is rare, so use the safer PendingDelete replay for every popup here.
    // This is type-independent, bypasses application Destroy() overrides and
    // preserves the ordinary-event lifetime guarantee of every transient base
    // subclass without changing public layout or RTTI.
    const auto mustDestroyImmediately =
        [popup]()
        {
            wxWindow * const parent = popup->GetParent();
            return (parent &&
                    (parent->IsBeingDeleted() ||
                     wxWinUITLWHostIsDestroyScheduled(parent))) ||
                   !popup->GetHandle();
        };

    const wxWinUIDestroyDeferralResult deferred =
        wxWinUITLWHostDeferPopupDestroy(
            popup,
            mustDestroyImmediately()
                ? wxWinUIPopupDestroySemantics::Immediate
                : wxWinUIPopupDestroySemantics::PendingDelete);
    if ( deferred != wxWinUIDestroyDeferralResult::NotDeferred )
        return true;

    // Re-read after PrepareForDestroy() and the host hand-off: owner teardown
    // must still delete children synchronously instead of leaving a dangling
    // entry in wxPendingDelete.
    if ( mustDestroyImmediately() )
        return popup->wxNonOwnedWindow::Destroy();

    if ( !wxPendingDelete.Member(popup) )
    {
        wxPendingDelete.Append(popup);
        wxWakeUpIdle();
    }
    return true;
}

bool wxWinUIRetirePopupTerminalImpl(
    wxPopupWindow *popup,
    const wxWinUINativeHwndIdentity& nativeIdentity)
{
    if ( wxWinUIPopupIsUnavailable(popup) ||
            !wxWinUINativeIdentityIsCurrent(nativeIdentity) )
    {
        return false;
    }

    const wxWeakRef<wxWindow> identity(popup);
    wxPopupTransientWindow * const transient =
        wxDynamicCast(popup, wxPopupTransientWindow);
    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.terminalCancellation )
    {
        // The outer transaction owns capture convergence and exact-HWND
        // retirement. Its caller must retain any external lifetime handle
        // until that transaction reaches its terminal postcondition.
        return false;
    }

    initialState.terminalCancellation = true;
    wxScopeGuard terminalGateGuard = wxMakeGuard(
        [identity, nativeIdentity]()
        {
            wxPopupWindow * const live =
                wxDynamicCast(identity.get(), wxPopupWindow);
            if ( wxWinUIPopupIsUnavailable(live) || live->IsShown() ||
                    !wxWinUINativeIdentityIsHiddenOrGone(nativeIdentity) )
            {
                return;
            }

            if ( wxWinUIPopupState * const state =
                     wxWinUIFindPopupState(live) )
            {
                state->terminalCancellation = false;
            }
        });
    wxUnusedVar(terminalGateGuard);

    const auto terminalPostcondition =
        [identity, nativeIdentity]()
        {
            wxPopupWindow * const live =
                wxDynamicCast(identity.get(), wxPopupWindow);
            return wxWinUIPopupIsUnavailable(live)
                       ? wxWinUINativeIdentityIsHiddenOrGone(nativeIdentity)
                       : !live->IsShown() &&
                         wxWinUINativeIdentityIsHiddenOrGone(nativeIdentity);
        };

    if ( !popup->IsShown() )
    {
        if ( !wxWinUIHideExactNativeIdentity(nativeIdentity) )
            (void)wxWinUIDestroyPopupForTerminalRetirement(popup);
        (void)wxWinUIHideExactNativeIdentity(nativeIdentity);
        wxPopupWindow * const live =
            wxDynamicCast(identity.get(), wxPopupWindow);
        if ( !wxWinUIPopupIsUnavailable(live) )
            wxWinUIEndPopupTransientSession(live);
        return terminalPostcondition();
    }

    const auto forceTerminalRetirement =
        [identity, popup, transient, nativeIdentity]()
        {
            const auto exactNativeHidden =
                [&]()
                {
                    return wxWinUIHideExactNativeIdentity(nativeIdentity);
                };
            const auto resolveLive =
                [&]() -> wxPopupWindow *
                {
                    wxPopupWindow * const live =
                        wxDynamicCast(identity.get(), wxPopupWindow);
                    return live == popup ? live : nullptr;
                };

            wxPopupWindow *live = resolveLive();
            if ( wxWinUIPopupIsUnavailable(live) )
                return exactNativeHidden();

            // terminalCancellation makes wx CaptureMouse() reject every new
            // descendant capture before touching the global capture stack.
            // Native island/input-site HWNDs can bypass that stack, so inspect
            // USER32 capture and release only an exact popup HWND descendant.
            constexpr unsigned MaxTerminalCaptureReleasePasses = 32;
            for ( unsigned pass = 0;
                  pass < MaxTerminalCaptureReleasePasses;
                  ++pass )
            {
                wxWinUINativeHwndIdentity captureIdentity;
                if ( !wxWinUIGetExactPopupCapture(
                         nativeIdentity, &captureIdentity) )
                {
                    break;
                }

                (void)wxWinUIReleaseExactPopupCapture(
                    live, nativeIdentity, captureIdentity);
                live = resolveLive();
                if ( wxWinUIPopupIsUnavailable(live) )
                    return exactNativeHidden();

                if ( gs_winuiPopupCaptureReleaseHookForTesting )
                {
                    if ( wxPopupTransientWindow * const liveTransient =
                             wxDynamicCast(live, wxPopupTransientWindow) )
                    {
                        gs_winuiPopupCaptureReleaseHookForTesting(
                            liveTransient, pass);
                    }
                }

                live = resolveLive();
                if ( wxWinUIPopupIsUnavailable(live) )
                    return exactNativeHidden();
            }

            wxWinUINativeHwndIdentity remainingCaptureIdentity;
            const bool captureReleaseExhausted =
                wxWinUIGetExactPopupCapture(
                    nativeIdentity, &remainingCaptureIdentity);
            if ( captureReleaseExhausted )
            {
                // The hostile seam runs after each bounded release. Perform
                // one final unobservable release before scheduling physical
                // destruction so a retained TLW guard cannot leave global
                // USER32 capture parked on the hidden popup meanwhile.
                (void)wxWinUIReleaseExactPopupCapture(
                    live, nativeIdentity, remainingCaptureIdentity);
                (void)exactNativeHidden();
            }

            live = resolveLive();
            if ( wxWinUIPopupIsUnavailable(live) )
                return exactNativeHidden();

            // Hide through the non-virtual base path to make the logical state
            // terminal, then independently verify the exact native generation.
            // A callback inside Show(false) is allowed to call SW_SHOWNA.
            if ( live->IsShown() )
                (void)live->wxPopupWindow::Show(false);
            (void)exactNativeHidden();

            live = resolveLive();
            if ( wxWinUIPopupIsUnavailable(live) )
                return exactNativeHidden();

            if ( transient && !live->IsShown() )
            {
                wxPopupTransientWindow * const liveTransient =
                    wxDynamicCast(live, wxPopupTransientWindow);
                if ( !wxWinUIPopupIsUnavailable(liveTransient) )
                    (void)wxWinUINotifyDismissOnce(liveTransient);

                // OnDismiss() is another arbitrary callback. Preserve a real
                // logical reopen only outside terminal cancellation; here the
                // cancelled generation must converge back to hidden.
                live = resolveLive();
                if ( !wxWinUIPopupIsUnavailable(live) && live->IsShown() )
                    (void)live->wxPopupWindow::Show(false);
                (void)exactNativeHidden();
            }

            live = resolveLive();
            bool logicalRetired =
                wxWinUIPopupIsUnavailable(live) || !live->IsShown();
            bool nativeRetired = exactNativeHidden();
            if ( !logicalRetired || !nativeRetired ||
                    captureReleaseExhausted )
            {
                // If hostile callbacks defeated logical, native-visibility or
                // capture convergence, retire the shell instead of returning a
                // visible registry-less HWND.
                (void)exactNativeHidden();
                if ( !wxWinUIPopupIsUnavailable(live) )
                    (void)wxWinUIDestroyPopupForTerminalRetirement(live);
                nativeRetired = exactNativeHidden();
                live = resolveLive();
                logicalRetired =
                    wxWinUIPopupIsUnavailable(live) || !live->IsShown();
            }

            return logicalRetired && nativeRetired;
        };

    bool committed = false;
    wxTRY
    {
        if ( transient )
            committed = wxWinUIDismissAndNotify(transient);
        else
        {
            (void)popup->Hide();
            wxPopupWindow * const live =
                wxDynamicCast(identity.get(), wxPopupWindow);
            committed = !wxWinUIPopupIsUnavailable(live) &&
                        !live->IsShown() &&
                        wxWinUINativeIdentityIsHiddenOrGone(nativeIdentity);
        }

        if ( !committed )
            committed = forceTerminalRetirement();
    }
    wxCATCH_ALL(
    {
        (void)forceTerminalRetirement();
        throw;
    })

    if ( committed && terminalPostcondition() )
    {
        wxPopupWindow * const live =
            wxDynamicCast(identity.get(), wxPopupWindow);
        if ( !wxWinUIPopupIsUnavailable(live) )
        {
            if ( wxWinUIPopupState * const state =
                     wxWinUIFindPopupState(live) )
            {
                state->terminalCancellation = false;
            }
        }
        return true;
    }

    return terminalPostcondition();
}

void wxWinUICancelPopupTransient(
    wxPopupWindow *popup,
    const std::shared_ptr<const wxWinUIPopupCancelToken>& token)
{
    if ( wxWinUIPopupIsUnavailable(popup) ||
            !wxWinUIPopupCancelTokenMatchesGeneration(popup, token) )
        return;

    const wxWeakRef<wxWindow> identity(popup);
    const wxWinUINativeHwndIdentity nativeIdentity = {
        reinterpret_cast<HWND>(token->popupHwnd),
        token->popupNativeHwndGeneration
    };
    wxWindow * const tokenOwner = token->owner.get();
    bool retireUnmigratedTopology = false;
    if ( wxWinUIGetPopupLogicalOwner(popup) != tokenOwner )
    {
        // A logical ancestor may have migrated to another TLW without calling
        // Reparent() on the popup itself. The old manager removes its entry
        // before this callback, so migrate synchronously to the current owner
        // instead of killing the now-foreign generation or leaving it
        // unregistered. If migration cannot commit, the obsolete generation
        // remains authoritative for terminal withdrawal below.
        wxWindow * const logicalOwner =
            wxWinUIGetPopupLogicalOwner(popup);
        bool migrated = false;
        if ( logicalOwner && !wxWinUIPopupIsUnavailable(logicalOwner) )
        {
            wxTRY
            {
                migrated = wxWinUIBeginPopupTransientSession(popup);
            }
            wxCATCH_ALL(
            {
                // The manager has already withdrawn the old generation. An
                // allocation/publication failure must therefore fall through
                // to terminal retirement instead of escaping and leaving a
                // visible popup without any authoritative registration.
            })
        }
        if ( migrated )
        {
            return;
        }

        popup = wxDynamicCast(identity.get(), wxPopupWindow);
        if ( wxWinUIPopupIsUnavailable(popup) ||
                !wxWinUIPopupCancelTokenMatchesGeneration(popup, token) )
        {
            return;
        }
        retireUnmigratedTopology = true;
    }

    if ( !retireUnmigratedTopology &&
            !wxWinUIPopupCancelTokenIsCurrent(popup, token) )
        return;

    wxWinUIPopupState& initialState = wxWinUIGetPopupState(popup);
    if ( initialState.dismissInProgress ||
            initialState.dismissAndNotifyInProgress )
    {
        // The active stack will consume this bit after all application
        // callbacks return. This retains an authoritative owner cancellation
        // instead of dropping it at a reentrant boundary.
        initialState.managerCancelPending = true;
        initialState.managerCancelToken = token;
        return;
    }
    initialState.managerCancelPending = false;
    initialState.managerCancelToken.reset();
    (void)wxWinUIRetirePopupTerminalImpl(popup, nativeIdentity);
}

} // anonymous namespace

bool wxWinUIRetirePopupTerminal(wxPopupWindow *popup)
{
    if ( wxWinUIPopupIsUnavailable(popup) )
        return false;

    wxWinUINativeHwndIdentity nativeIdentity =
        wxWinUICaptureNativeIdentity(popup);
    if ( gs_failNextPopupTerminalNativeIdentityForTesting )
    {
        gs_failNextPopupTerminalNativeIdentityForTesting = false;
        nativeIdentity = wxWinUINativeHwndIdentity();
    }

    if ( wxWinUINativeIdentityIsCurrent(nativeIdentity) )
        return wxWinUIRetirePopupTerminalImpl(popup, nativeIdentity);

    const wxWinUIPopupState * const state =
        wxWinUIFindPopupState(popup);
    if ( state && state->terminalCancellation )
    {
        // An outer exact-generation transaction remains authoritative.
        return false;
    }

    // There is no exact HWND which can be hidden safely, but keeping a live
    // Ref can no longer make progress either. Bypass application overrides and
    // transfer the wrapper to the central popup-destroy pipeline; its eventual
    // destructor is the sole authority which clears external lifetime handles.
    const wxWeakRef<wxWindow> identity(popup);
    const bool destroyAccepted =
        wxWinUIDestroyPopupForTerminalRetirement(popup);
    wxPopupWindow * const live =
        wxDynamicCast(identity.get(), wxPopupWindow);
    return destroyAccepted || wxWinUIPopupIsUnavailable(live) ||
           (live && wxWinUITLWHostIsDestroyScheduled(live));
}

void wxWinUITestPopupOutsideClick(wxPopupTransientWindow *popup)
{
    if ( popup )
        wxWinUIPopupSidecarAccess::DismissFromOutside(popup);
}

#endif // __WXWINUI__ && wxUSE_WINUI3

void wxPopupTransientWindow::DismissOnDeactivate()
{
    DismissFromOutside();
}

void wxPopupTransientWindow::DismissFromOutside()
{
    if ( !IsShown() )
        return;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const wxWeakRef<wxWindow> weakThis(this);
    const WXHWND outsideHwnd = GetHWND();
    const unsigned long long outsideHwndGeneration =
        wxWinUIMSWGetHwndGeneration(this, outsideHwnd);
    const unsigned long long outsideEpoch =
        wxWinUIGetPopupSessionEpoch(this);
    const wxWinUIPopupState * const outsideState =
        wxWinUIFindPopupState(this);
    const unsigned long long outsideShowRequestEpoch =
        outsideState ? outsideState->showRequestEpoch : 0;
    if ( !CanDismiss() )
        return;

    wxPopupTransientWindow * const live =
        wxDynamicCast(
            wxWinUIResolvePopupGeneration(
                weakThis, this, outsideHwnd, outsideHwndGeneration,
                outsideEpoch),
            wxPopupTransientWindow);
    if ( !live || !live->IsShown() )
        return;

    // OnDismiss() is an arbitrary application callback and may delete,
    // reparent or reopen this popup or its owner. Snapshot both generations;
    // the native tail below is allowed only if this exact hidden operation is
    // still authoritative after the callback.
    const wxWeakRef<wxWindow> ownerWeak(live->MSWGetOwner());
    wxWindow * const expectedOwner = live->MSWGetOwner();
    const WXHWND expectedOwnerHwnd =
        expectedOwner ? expectedOwner->GetHWND() : nullptr;
    const unsigned long long expectedOwnerGeneration =
        expectedOwner
            ? wxWinUIMSWGetHwndGeneration(
                  expectedOwner, expectedOwnerHwnd)
            : 0;
#else
    if ( !CanDismiss() )
        return;
    wxWindow * const ownerBeforeDismiss = MSWGetOwner();
#endif

    // Hide the window automatically when it loses activation.
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( !wxWinUIDismissAndNotify(live) )
        return;
#else
    DismissAndNotify();
#endif

    // Activation might have gone to a different window or maybe
    // even a different application, don't let our owner continue
    // to appear active in this case.
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxPopupTransientWindow * const liveAfterDismiss =
        wxDynamicCast(weakThis.get(), wxPopupTransientWindow);
    const wxWinUIPopupState * const stateAfterDismiss =
        liveAfterDismiss ? wxWinUIFindPopupState(liveAfterDismiss) : nullptr;
    wxWindow* const owner = ownerWeak.get();
    if ( !wxWinUIPopupIsUnavailable(liveAfterDismiss) &&
            !liveAfterDismiss->IsShown() && stateAfterDismiss &&
            stateAfterDismiss->showRequestEpoch ==
                outsideShowRequestEpoch &&
            liveAfterDismiss->MSWGetOwner() == expectedOwner &&
            wxWinUIGetPopupLogicalOwner(liveAfterDismiss) == expectedOwner &&
            owner == expectedOwner &&
            !wxWinUIPopupIsUnavailable(owner) && expectedOwnerHwnd &&
            owner->GetHWND() == expectedOwnerHwnd &&
            expectedOwnerGeneration &&
            wxWinUIMSWGetHwndGeneration(owner, expectedOwnerHwnd) ==
                expectedOwnerGeneration )
#else
    wxWindow* const owner = ownerBeforeDismiss;
    if ( owner )
#endif
    {
        if ( ::GetActiveWindow() != GetHwndOf(owner) )
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            if ( gs_winuiPopupOwnerDeactivateHookForTesting )
                gs_winuiPopupOwnerDeactivateHookForTesting(owner);
#endif
            ::SendMessage(GetHwndOf(owner), WM_NCACTIVATE, FALSE, 0);
        }
    }
}

void wxPopupTransientWindow::MSWDismissUnfocusedPopup()
{
    // When we use wxPU_CONTAINS_CONTROLS, we can react to the popup
    // deactivation in MSWHandleMessage(), but if we don't have focus, we don't
    // get any events ourselves, so we rely on wxWindow to forward them to us.
    if ( !HasFlag(wxPU_CONTAINS_CONTROLS) )
    {
        // It doesn't seem necessary to use CallAfter() here, as dismissing
        // this window shouldn't affect the focus, as it never has it anyhow.
        DismissFromOutside();
    }
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

bool wxWinUIMSWCompleteDeferredPopupDestroy(
    wxWindow *window,
    wxWinUIPopupDestroySemantics semantics)
{
    wxPopupWindow * const popup = wxDynamicCast(window, wxPopupWindow);
    if ( !popup )
        return false;

    if ( semantics == wxWinUIPopupDestroySemantics::PendingDelete )
    {
        wxWindow * const parent = popup->GetParent();
        if ( (parent &&
              (parent->IsBeingDeleted() ||
               wxWinUITLWHostIsDestroyScheduled(parent))) ||
                !popup->GetHandle() )
        {
            return popup->wxNonOwnedWindow::Destroy();
        }

        if ( !wxPendingDelete.Member(popup) )
            wxPendingDelete.Append(popup);
        wxWakeUpIdle();
        return true;
    }

    // Qualified non-virtual replay: application overrides and the transient
    // delayed-delete override must not observe a second Destroy() call.
    return popup->wxNonOwnedWindow::Destroy();
}

#endif // __WXWINUI__ && wxUSE_WINUI3

bool
wxPopupTransientWindow::MSWHandleMessage(WXLRESULT *result,
                                         WXUINT message,
                                         WXWPARAM wParam,
                                         WXLPARAM lParam)
{
    switch ( message )
    {
        case WM_ACTIVATE:
            // The high word reports the minimized state and is not part of
            // the activation code.
            if ( LOWORD(wParam) == WA_INACTIVE )
            {
                bool destructionScheduled =
                    !wxTheApp ||
                    wxTheApp->IsScheduledForDestruction(this);
#if defined(__WXWINUI__) && wxUSE_WINUI3
                destructionScheduled =
                    destructionScheduled ||
                    wxWinUITLWHostIsDestroyScheduled(this);
#endif
                if ( destructionScheduled )
                {
                    // It is possible that we get this message again after
                    // already getting it once, avoid scheduling the window for
                    // destruction again in this case.
                    break;
                }

                // We need to dismiss this window, however doing it directly
                // from here seems to confuse ::ShowWindow(), which ends up
                // calling this handler, and may result in losing activation
                // entirely, so postpone it slightly.
                //
                // Also note that the active window hasn't changed yet, so we
                // postpone calling it until DismissOnDeactivate() is executed.
#if defined(__WXWINUI__) && wxUSE_WINUI3
                const wxWeakRef<wxWindow> weakThis(this);
                const WXHWND popupHwnd = GetHWND();
                const unsigned long long popupHwndGeneration =
                    wxWinUIMSWGetHwndGeneration(this, popupHwnd);
                const unsigned long long sessionEpoch =
                    wxWinUIGetPopupSessionEpoch(this);
                CallAfter(
                    [weakThis, popupHwnd, popupHwndGeneration,
                     sessionEpoch]()
                    {
                        wxPopupTransientWindow * const popup =
                            wxDynamicCast(
                                wxWinUIResolvePopupGeneration(
                                    weakThis,
                                    wxDynamicCast(
                                        weakThis.get(), wxPopupWindow),
                                    popupHwnd, popupHwndGeneration,
                                    sessionEpoch),
                                wxPopupTransientWindow);
                        if ( popup && popup->IsShown() )
                        {
                            popup->DismissOnDeactivate();
                        }
                    });
#else
                CallAfter(&wxPopupTransientWindow::DismissOnDeactivate);
#endif
            }
            break;
    }

    return wxPopupTransientWindowBase::MSWHandleMessage(result, message,
                                                        wParam, lParam);
}

#endif // #if wxUSE_POPUPWIN

#if defined(__WXWINUI__) && wxUSE_WINUI3 && !wxUSE_POPUPWIN

bool wxWinUIMSWCompleteDeferredPopupDestroy(
    wxWindow *WXUNUSED(window),
    wxWinUIPopupDestroySemantics WXUNUSED(semantics))
{
    // No popup destruction request can be queued when the feature is absent,
    // but tlwhost.cpp keeps one feature-independent replay switch.
    return false;
}

#endif
