/////////////////////////////////////////////////////////////////////////
// File:        src/msw/taskbar.cpp
// Purpose:     Implements wxTaskBarIcon class for manipulating icons on
//              the Windows task bar.
// Author:      Julian Smart
// Modified by: Vaclav Slavik
// Created:     24/3/98
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TASKBARICON

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/frame.h"
    #include "wx/utils.h"
    #include "wx/menu.h"
    #include "wx/app.h"
#endif

#include "wx/msw/wrapshl.h"

#include <string.h>
#include "wx/taskbar.h"
#include "wx/platinfo.h"
#include "wx/msw/private.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/scopeguard.h"
    #include "wx/weakref.h"
    #include "wx/winui/private/taskbaricon.h"
    #include "wx/winui/private/tlwhostmsw.h"

    #include <memory>
    #include <unordered_map>
#endif

#ifndef NIN_BALLOONTIMEOUT
    #define NIN_BALLOONTIMEOUT      0x0404
    #define NIN_BALLOONUSERCLICK    0x0405
#endif

#ifndef NIIF_USER
    #define NIIF_USER       0x0004
#endif

#ifndef NIIF_LARGE_ICON
    #define NIIF_LARGE_ICON 0x0020
#endif

// initialized on demand
static UINT gs_msgTaskbar = 0;
static UINT gs_msgRestartTaskbar = 0;

#if defined(__WXWINUI__) && wxUSE_WINUI3

namespace
{

struct wxWinUITaskBarIconState
{
    wxWeakRef<wxEvtHandler> lifetime;
    wxWeakRef<wxWindow> helper;
    wxBitmapBundle desiredIcon;
    wxString desiredTooltip;
    wxIcon retainedIcon;
    wxWinUITaskBarIconAppliedState applied =
        wxWinUITaskBarIconAppliedState::Absent;
    bool desiredInstalled = false;
    bool versionApplied = false;
    bool reconciling = false;
    bool reconcilePending = false;
    bool destroying = false;
    bool helperDestroyPending = false;
    unsigned callbackDepth = 0;
    unsigned popupDepth = 0;
    unsigned long long desiredGeneration = 0;
    unsigned long long appliedGeneration = 0;
    unsigned long long explorerGeneration = 1;
    unsigned long long appliedExplorerGeneration = 1;
    unsigned nativeCallCount = 0;
};

using wxWinUITaskBarIconStatePtr =
    std::shared_ptr<wxWinUITaskBarIconState>;
using wxWinUITaskBarIconStates =
    std::unordered_map<const wxTaskBarIcon *, wxWinUITaskBarIconStatePtr>;

wxWinUITaskBarIconStates& wxWinUIGetTaskBarIconStates()
{
    static wxWinUITaskBarIconStates * const states =
        new wxWinUITaskBarIconStates;
    return *states;
}

wxWinUITaskBarIconStatePtr wxWinUIGetTaskBarIconState(wxTaskBarIcon *icon)
{
    wxWinUITaskBarIconStates& states = wxWinUIGetTaskBarIconStates();
    const auto existing = states.find(icon);
    if ( existing != states.end() && existing->second &&
            existing->second->lifetime.get() == icon )
    {
        return existing->second;
    }

    const auto state = std::make_shared<wxWinUITaskBarIconState>();
    state->lifetime = icon;
    states[icon] = state;
    return state;
}

wxWinUITaskBarIconStatePtr wxWinUIFindTaskBarIconState(
    const wxTaskBarIcon *icon)
{
    wxWinUITaskBarIconStates& states = wxWinUIGetTaskBarIconStates();
    const auto existing = states.find(icon);
    return existing != states.end() && existing->second &&
                   existing->second->lifetime.get() == icon
               ? existing->second
               : wxWinUITaskBarIconStatePtr();
}

void wxWinUITryEraseTaskBarIconState(
    const wxWinUITaskBarIconStatePtr& state,
    const wxTaskBarIcon *identity)
{
    if ( !state || !state->destroying || state->reconciling ||
            state->callbackDepth || state->popupDepth )
    {
        return;
    }

    wxWinUITaskBarIconStates& states = wxWinUIGetTaskBarIconStates();
    const auto existing = states.find(identity);
    if ( existing != states.end() && existing->second == state )
        states.erase(existing);
}

void wxWinUIFinalizeDeferredTaskBarIconHelper(
    const wxWinUITaskBarIconStatePtr& state)
{
    if ( !state || !state->destroying || state->reconciling ||
            !state->helperDestroyPending )
    {
        return;
    }

    state->helperDestroyPending = false;
    wxWindow * const helper = state->helper.get();
    if ( helper && !helper->IsBeingDeleted() )
        (void)helper->Destroy();
}

unsigned long long wxWinUIBumpTaskBarIconGeneration(
    unsigned long long& generation)
{
    if ( ++generation == 0 )
        ++generation;
    return generation;
}

wxWinUITaskBarIconShellNotifyHookForTesting
    gs_taskBarIconShellNotifyHookForTesting = nullptr;
wxWinUITaskBarIconPopupHookForTesting
    gs_taskBarIconPopupHookForTesting = nullptr;
unsigned gs_taskBarIconHelperCount = 0;
bool gs_taskBarIconPopupActive = false;

class wxWinUITaskBarIconCallbackLease final
{
public:
    wxWinUITaskBarIconCallbackLease(
        const wxWinUITaskBarIconStatePtr& state,
        const wxTaskBarIcon *identity)
        : m_state(state), m_identity(identity)
    {
        if ( m_state )
            ++m_state->callbackDepth;
    }

    ~wxWinUITaskBarIconCallbackLease()
    {
        if ( m_state )
        {
            wxASSERT(m_state->callbackDepth != 0);
            --m_state->callbackDepth;
            wxWinUITryEraseTaskBarIconState(m_state, m_identity);
        }
    }

private:
    wxWinUITaskBarIconStatePtr m_state;
    const wxTaskBarIcon *m_identity;
};

} // anonymous namespace

#endif // __WXWINUI__ && wxUSE_WINUI3


wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTaskBarIconWindow: helper window
// ----------------------------------------------------------------------------

// NB: this class serves two purposes:
//     1. win32 needs a HWND associated with taskbar icon, this provides it
//     2. we need wxTopLevelWindow so that the app doesn't exit when
//        last frame is closed but there still is a taskbar icon
class wxTaskBarIconWindow : public wxFrame
{
public:
    wxTaskBarIconWindow(wxTaskBarIcon *icon)
        : wxFrame(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0),
          m_icon(icon)
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        Bind(wxEVT_MENU, &wxTaskBarIconWindow::OnMenu, this);
        Bind(wxEVT_UPDATE_UI, &wxTaskBarIconWindow::OnUpdateUI, this);
        ++gs_taskBarIconHelperCount;
#endif
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    ~wxTaskBarIconWindow() override
    {
        wxTaskBarIcon * const icon =
            wxDynamicCast(m_icon.get(), wxTaskBarIcon);
        if ( icon )
            icon->MSWOnTaskBarIconWindowDestroyed(this);

        // Do not leave a weak owner association live while the wxFrame base
        // classes tear down and may dispatch final native notifications.
        m_icon = nullptr;

        wxASSERT(gs_taskBarIconHelperCount != 0);
        if ( gs_taskBarIconHelperCount != 0 )
            --gs_taskBarIconHelperCount;
    }

    void DetachIcon(wxTaskBarIcon *icon)
    {
        if ( m_icon.get() == icon )
            m_icon = nullptr;
    }
#endif

    WXLRESULT MSWWindowProc(WXUINT msg,
                            WXWPARAM wParam, WXLPARAM lParam) override
    {
        if (msg == gs_msgRestartTaskbar || msg == gs_msgTaskbar)
        {
#if defined(__WXWINUI__) && wxUSE_WINUI3
            wxTaskBarIcon * const icon =
                wxDynamicCast(m_icon.get(), wxTaskBarIcon);
            if ( !icon )
                return wxFrame::MSWWindowProc(msg, wParam, lParam);

            const auto state = wxWinUIFindTaskBarIconState(icon);
            wxWinUITaskBarIconCallbackLease lease(state, icon);
            return icon->WindowProc(msg, wParam, lParam);
#else
            return m_icon->WindowProc(msg, wParam, lParam);
#endif
        }
        else
        {
            return wxFrame::MSWWindowProc(msg, wParam, lParam);
        }
    }

private:
#if defined(__WXWINUI__) && wxUSE_WINUI3
    void ForwardEvent(wxEvent& event)
    {
        wxTaskBarIcon * const icon =
            wxDynamicCast(m_icon.get(), wxTaskBarIcon);
        if ( !icon )
        {
            event.Skip();
            return;
        }

        const auto state = wxWinUIFindTaskBarIconState(icon);
        wxWinUITaskBarIconCallbackLease lease(state, icon);
        if ( !icon->ProcessEvent(event) )
            event.Skip();
    }

    void OnMenu(wxCommandEvent& event) { ForwardEvent(event); }
    void OnUpdateUI(wxUpdateUIEvent& event) { ForwardEvent(event); }

    wxWeakRef<wxEvtHandler> m_icon;
#else
    wxTaskBarIcon *m_icon;
#endif
};


// ----------------------------------------------------------------------------
// NotifyIconData: wrapper around NOTIFYICONDATA
// ----------------------------------------------------------------------------

struct NotifyIconData : public WinStruct<NOTIFYICONDATA>
{
    NotifyIconData(WXHWND hwnd)
    {
        hWnd = (HWND) hwnd;
        uCallbackMessage = gs_msgTaskbar;
        uFlags = NIF_MESSAGE;

        // we use the same id for all taskbar icons as we don't need it to
        // distinguish between them
        uID = 99;
    }
};

#if defined(__WXWINUI__) && wxUSE_WINUI3

class wxWinUITaskBarIconSidecarAccess final
{
public:
    static void SetRealIcon(wxTaskBarIcon *icon, const wxIcon& realIcon)
    {
        icon->m_realIcon = realIcon;
    }

    static void SetIconAdded(wxTaskBarIcon *icon, bool added)
    {
        icon->m_iconAdded = added;
    }

    static bool DestroyHelper(wxTaskBarIcon *icon)
    {
        wxTaskBarIconWindow * const helper = icon ? icon->m_win : nullptr;
        if ( !helper )
            return false;

        const auto state = wxWinUIFindTaskBarIconState(icon);
        if ( state && (state->callbackDepth || state->popupDepth) )
        {
            // A taskbar notification or menu callback still has a member
            // function of this helper on its stack. wxWindow::Destroy()
            // defers the C++ deletion until that stack has unwound.
            return helper->Destroy();
        }

        const wxWeakRef<wxEvtHandler> weakIcon(icon);
        delete helper;

        wxTaskBarIcon * const liveIcon =
            wxDynamicCast(weakIcon.get(), wxTaskBarIcon);
        return liveIcon && liveIcon->m_win == nullptr;
    }
};

namespace
{

wxTaskBarIcon *wxWinUIResolveTaskBarIcon(
    const wxWinUITaskBarIconStatePtr& state)
{
    return state
               ? wxDynamicCast(state->lifetime.get(), wxTaskBarIcon)
               : nullptr;
}

wxWindow *wxWinUIResolveTaskBarIconHelper(
    const wxWinUITaskBarIconStatePtr& state,
    WXHWND expectedHwnd = nullptr,
    unsigned long long expectedGeneration = 0)
{
    wxWindow * const helper = state ? state->helper.get() : nullptr;
    if ( !helper || helper->IsBeingDeleted() )
        return nullptr;

    const WXHWND hwnd = helper->GetHWND();
    if ( !hwnd )
        return nullptr;
    if ( expectedHwnd && hwnd != expectedHwnd )
        return nullptr;

    const unsigned long long generation =
        wxWinUIMSWGetHwndGeneration(helper, hwnd);
    if ( !generation ||
            (expectedGeneration && generation != expectedGeneration) )
    {
        return nullptr;
    }

    return helper;
}

bool wxWinUITaskBarIconRequestIsCurrent(
    const wxWinUITaskBarIconStatePtr& state,
    unsigned long long desiredGeneration,
    unsigned long long explorerGeneration,
    WXHWND helperHwnd,
    unsigned long long helperGeneration)
{
    return state &&
           state->desiredGeneration == desiredGeneration &&
           state->explorerGeneration == explorerGeneration &&
           wxWinUIResolveTaskBarIconHelper(
               state, helperHwnd, helperGeneration) != nullptr;
}

bool wxWinUITaskBarIconPresentationIsCurrent(
    const wxWinUITaskBarIconStatePtr& state,
    unsigned long long desiredGeneration,
    unsigned long long explorerGeneration,
    WXHWND helperHwnd,
    unsigned long long helperGeneration)
{
    return wxWinUITaskBarIconRequestIsCurrent(
               state, desiredGeneration, explorerGeneration,
               helperHwnd, helperGeneration) &&
           state->desiredInstalled &&
           state->applied == wxWinUITaskBarIconAppliedState::Present &&
           state->appliedGeneration == desiredGeneration &&
           state->appliedExplorerGeneration == explorerGeneration &&
           state->versionApplied;
}

bool wxWinUITaskBarIconAbsenceIsCurrent(
    const wxWinUITaskBarIconStatePtr& state,
    unsigned long long desiredGeneration,
    unsigned long long explorerGeneration)
{
    return state &&
           state->desiredGeneration == desiredGeneration &&
           state->explorerGeneration == explorerGeneration &&
           !state->desiredInstalled &&
           state->applied == wxWinUITaskBarIconAppliedState::Absent &&
           state->appliedGeneration == desiredGeneration &&
           state->appliedExplorerGeneration == explorerGeneration &&
           !state->versionApplied;
}

bool wxWinUICallTaskBarIconShell(
    const wxWinUITaskBarIconStatePtr& state,
    DWORD message,
    NotifyIconData *data)
{
    if ( state )
        ++state->nativeCallCount;

    if ( gs_taskBarIconShellNotifyHookForTesting )
    {
        return gs_taskBarIconShellNotifyHookForTesting(
            static_cast<unsigned>(message), data);
    }

    return ::Shell_NotifyIcon(message, data) != 0;
}

void wxWinUILogTaskBarIconShellFailure(const wxString& operation)
{
    if ( !gs_taskBarIconShellNotifyHookForTesting )
        wxLogLastError(operation);
}

bool wxWinUIReconcileTaskBarIcon(
    const wxWinUITaskBarIconStatePtr& state)
{
    if ( !state )
        return false;

    if ( state->reconciling )
    {
        state->reconcilePending = true;
        return true;
    }

    const wxTaskBarIcon * const identity =
        wxWinUIResolveTaskBarIcon(state);
    state->reconciling = true;
    wxScopeGuard reconcileGuard = wxMakeGuard(
        [state, identity]()
        {
            state->reconciling = false;
            wxWinUIFinalizeDeferredTaskBarIconHelper(state);
            wxWinUITryEraseTaskBarIconState(state, identity);
        });
    wxUnusedVar(reconcileGuard);

    constexpr unsigned MaxReconcilePasses = 16;
    for ( unsigned pass = 0; pass < MaxReconcilePasses; ++pass )
    {
        state->reconcilePending = false;

        const bool desiredInstalled = state->desiredInstalled;
        const unsigned long long desiredGeneration =
            state->desiredGeneration;
        const unsigned long long explorerGeneration =
            state->explorerGeneration;
        wxWindow * const helper = wxWinUIResolveTaskBarIconHelper(state);
        if ( !helper )
        {
            if ( !desiredInstalled )
            {
                state->applied = wxWinUITaskBarIconAppliedState::Absent;
                state->appliedGeneration = desiredGeneration;
                state->appliedExplorerGeneration = explorerGeneration;
                state->versionApplied = false;
                return true;
            }

            state->applied = wxWinUITaskBarIconAppliedState::Unknown;
            state->versionApplied = false;
            return false;
        }

        const WXHWND helperHwnd = helper->GetHWND();
        const unsigned long long helperGeneration =
            wxWinUIMSWGetHwndGeneration(helper, helperHwnd);
        if ( !helperHwnd || !helperGeneration )
        {
            state->applied = wxWinUITaskBarIconAppliedState::Unknown;
            state->versionApplied = false;
            return false;
        }

        if ( desiredInstalled &&
                state->applied ==
                    wxWinUITaskBarIconAppliedState::Present &&
                state->appliedGeneration == desiredGeneration &&
                state->appliedExplorerGeneration == explorerGeneration &&
                state->versionApplied )
        {
            if ( wxTaskBarIcon * const live =
                     wxWinUIResolveTaskBarIcon(state) )
            {
                wxWinUITaskBarIconSidecarAccess::SetIconAdded(live, true);
            }
            return true;
        }

        if ( !desiredInstalled )
        {
            if ( state->applied ==
                     wxWinUITaskBarIconAppliedState::Absent &&
                    state->appliedExplorerGeneration == explorerGeneration )
            {
                state->appliedGeneration = desiredGeneration;
                state->versionApplied = false;
                return true;
            }

            NotifyIconData notifyData(helperHwnd);
            // Unknown is published before crossing the seam: an exception or
            // a nested desired-state mutation can never leave a stale
            // Present claim behind.
            state->applied = wxWinUITaskBarIconAppliedState::Unknown;
            state->versionApplied = false;
            const bool deleted = wxWinUICallTaskBarIconShell(
                state, NIM_DELETE, &notifyData);

            if ( !wxWinUITaskBarIconRequestIsCurrent(
                     state, desiredGeneration, explorerGeneration,
                     helperHwnd, helperGeneration) )
            {
                state->reconcilePending = true;
                continue;
            }

            if ( !deleted )
            {
                wxWinUILogTaskBarIconShellFailure(
                    wxT("Shell_NotifyIcon(NIM_DELETE)"));
                return false;
            }

            state->applied = wxWinUITaskBarIconAppliedState::Absent;
            state->appliedGeneration = desiredGeneration;
            state->appliedExplorerGeneration = explorerGeneration;
            return true;
        }

        const wxBitmapBundle desiredIcon = state->desiredIcon;
        const wxString desiredTooltip = state->desiredTooltip;
        wxIcon realIcon;
        if ( desiredIcon.IsOk() )
            realIcon = desiredIcon.GetIconFor(helper);

        if ( !wxWinUITaskBarIconRequestIsCurrent(
                 state, desiredGeneration, explorerGeneration,
                 helperHwnd, helperGeneration) )
        {
            state->reconcilePending = true;
            continue;
        }

        NotifyIconData notifyData(helperHwnd);
        if ( realIcon.IsOk() )
        {
            notifyData.uFlags |= NIF_ICON;
            notifyData.hIcon = GetHiconOf(realIcon);
        }
        notifyData.uFlags |= NIF_TIP;
        if ( !desiredTooltip.empty() )
        {
            wxStrlcpy(notifyData.szTip, desiredTooltip.t_str(),
                      WXSIZEOF(notifyData.szTip));
        }

        state->retainedIcon = realIcon;
        if ( wxTaskBarIcon * const live = wxWinUIResolveTaskBarIcon(state) )
            wxWinUITaskBarIconSidecarAccess::SetRealIcon(live, realIcon);

        const bool knownAbsent =
            state->applied == wxWinUITaskBarIconAppliedState::Absent &&
            state->appliedExplorerGeneration == explorerGeneration;
        const bool knownPresent =
            state->applied == wxWinUITaskBarIconAppliedState::Present &&
            state->appliedExplorerGeneration == explorerGeneration;

        state->applied = wxWinUITaskBarIconAppliedState::Unknown;
        state->versionApplied = false;

        bool iconApplied = false;
        if ( knownAbsent )
        {
            iconApplied = wxWinUICallTaskBarIconShell(
                state, NIM_ADD, &notifyData);
            if ( !iconApplied )
            {
                wxWinUILogTaskBarIconShellFailure(
                    wxT("Shell_NotifyIcon(NIM_ADD)"));
            }
        }
        else if ( knownPresent )
        {
            iconApplied = wxWinUICallTaskBarIconShell(
                state, NIM_MODIFY, &notifyData);
            if ( !iconApplied )
            {
                wxWinUILogTaskBarIconShellFailure(
                    wxT("Shell_NotifyIcon(NIM_MODIFY)"));
            }
        }
        else
        {
            // TaskbarCreated can mean either a new Explorer instance or a
            // refresh of the existing one. ADD first, then MODIFY only when
            // the icon already exists, without guessing from stale state.
            iconApplied = wxWinUICallTaskBarIconShell(
                state, NIM_ADD, &notifyData);
            if ( !wxWinUITaskBarIconRequestIsCurrent(
                     state, desiredGeneration, explorerGeneration,
                     helperHwnd, helperGeneration) )
            {
                state->reconcilePending = true;
                continue;
            }

            if ( !iconApplied )
            {
                iconApplied = wxWinUICallTaskBarIconShell(
                    state, NIM_MODIFY, &notifyData);
            }
            if ( !iconApplied )
            {
                wxWinUILogTaskBarIconShellFailure(
                    wxT("Shell_NotifyIcon(NIM_ADD/NIM_MODIFY)"));
            }
        }

        if ( !wxWinUITaskBarIconRequestIsCurrent(
                 state, desiredGeneration, explorerGeneration,
                 helperHwnd, helperGeneration) )
        {
            state->reconcilePending = true;
            continue;
        }
        if ( !iconApplied )
            return false;

        state->applied = wxWinUITaskBarIconAppliedState::Present;
        state->appliedGeneration = desiredGeneration;
        state->appliedExplorerGeneration = explorerGeneration;

        NotifyIconData versionData(helperHwnd);
        versionData.uFlags = 0;
        versionData.uVersion = 3;
        const bool versionApplied = wxWinUICallTaskBarIconShell(
            state, NIM_SETVERSION, &versionData);
        if ( !wxWinUITaskBarIconRequestIsCurrent(
                 state, desiredGeneration, explorerGeneration,
                 helperHwnd, helperGeneration) )
        {
            state->applied = wxWinUITaskBarIconAppliedState::Unknown;
            state->versionApplied = false;
            state->reconcilePending = true;
            continue;
        }

        state->versionApplied = versionApplied;
        if ( !versionApplied )
        {
            wxWinUILogTaskBarIconShellFailure(
                wxT("Shell_NotifyIcon(NIM_SETVERSION)"));
            return false;
        }

        // This is the only transition that makes the compatibility flag true:
        // both the icon operation and NIM_SETVERSION succeeded for the exact
        // desired/Explorer/helper generation still current after the seams.
        if ( wxTaskBarIcon * const live =
                 wxWinUIResolveTaskBarIcon(state) )
        {
            wxWinUITaskBarIconSidecarAccess::SetIconAdded(live, true);
        }

        if ( !state->reconcilePending )
            return true;
    }

    state->applied = wxWinUITaskBarIconAppliedState::Unknown;
    state->versionApplied = false;
    return false;
}

bool wxWinUIRestartTaskBarIcon(
    wxTaskBarIcon *icon,
    const wxWinUITaskBarIconStatePtr& state)
{
    if ( !icon || !state || state->destroying )
        return false;

    (void)wxWinUIBumpTaskBarIconGeneration(state->explorerGeneration);

    // A successfully removed icon is absent both after a real Explorer
    // restart and after the benign refresh broadcasts seen on some systems.
    // Keep this proven absence without issuing a noisy NIM_DELETE against a
    // fresh tray. Unknown (e.g. a failed RemoveIcon()) is still reconciled by
    // an exact DELETE below.
    if ( !state->desiredInstalled &&
            state->applied == wxWinUITaskBarIconAppliedState::Absent )
    {
        state->appliedGeneration = state->desiredGeneration;
        state->appliedExplorerGeneration = state->explorerGeneration;
        state->versionApplied = false;
        return true;
    }

    state->applied = wxWinUITaskBarIconAppliedState::Unknown;
    state->versionApplied = false;
    return wxWinUIReconcileTaskBarIcon(state);
}

} // anonymous namespace

#endif // __WXWINUI__ && wxUSE_WINUI3

// ----------------------------------------------------------------------------
// wxTaskBarIcon
// ----------------------------------------------------------------------------

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType WXUNUSED(iconType))
{
    m_win = nullptr;
    m_iconAdded = false;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    (void)wxWinUIGetTaskBarIconState(this);
#endif
    RegisterWindowMessages();
}

wxTaskBarIcon::~wxTaskBarIcon()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto state = wxWinUIFindTaskBarIconState(this);
    if ( state )
    {
        state->destroying = true;
        state->desiredInstalled = false;
        state->versionApplied = false;
        (void)wxWinUIBumpTaskBarIconGeneration(
            state->desiredGeneration);
        m_iconAdded = false;

        // If this destructor is reached from a native seam or a menu/taskbar
        // callback, the active transaction owns the final DELETE pass. The
        // desired-absent publication above happens first, so neither that pass
        // nor TaskbarCreated can ever resurrect the icon.
        (void)wxWinUIReconcileTaskBarIcon(state);
    }

    if ( m_win )
    {
        wxTaskBarIconWindow * const win = m_win;
        m_win = nullptr;
        win->DetachIcon(this);

        if ( state && state->reconciling )
        {
            // The outer generation still needs this HWND to compensate a
            // possibly successful stale ADD with the queued DELETE.
            state->helperDestroyPending = true;
        }
        else if ( state && (state->callbackDepth || state->popupDepth) )
        {
            // Destroy() is intentionally deferred while a helper method or
            // USER32 callback still has the frame on its stack.
            (void)win->Destroy();
        }
        else
        {
            // Preserve the historical synchronous shutdown guarantee when no
            // callback can still be executing on the helper.
            delete win;
        }
    }

    wxWinUITryEraseTaskBarIconState(state, this);
#else
    if ( m_iconAdded )
        RemoveIcon();

    if ( m_win )
    {
        // we must use delete and not Destroy() here because the latter will
        // only schedule the window to be deleted during the next idle event
        // processing but we may not get any idle events if there are no other
        // windows left in the program
        delete m_win;
    }
#endif
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

void wxTaskBarIcon::MSWOnTaskBarIconWindowDestroyed(
    wxTaskBarIconWindow *window)
{
    // An obsolete helper must not detach a replacement created for the same
    // taskbar object. This also makes the notification harmless when reached
    // from an unusual nested window-destruction path.
    if ( m_win != window )
        return;

    const WXHWND helperHwnd = window->GetHWND();
    m_win = nullptr;
    m_iconAdded = false;

    const auto state = wxWinUIFindTaskBarIconState(this);
    if ( !state )
        return;

    const bool deletePossiblyPresentIcon =
        state->desiredInstalled ||
        state->applied != wxWinUITaskBarIconAppliedState::Absent;

    // Invalidate every claim before invoking even the test seam: deleting the
    // old icon may re-enter user-controlled test code and create a replacement
    // helper, whose newer state must win without being overwritten here.
    state->helper = nullptr;
    state->applied = wxWinUITaskBarIconAppliedState::Unknown;
    state->versionApplied = false;

    // Force an in-flight native transaction to revalidate the helper before
    // publishing its result. The desired state intentionally survives: a
    // later SetIcon() can create a fresh helper and reconcile it.
    if ( state->reconciling )
        state->reconcilePending = true;

    // DeleteAllTLWs() enters this derived destructor while the HWND is still
    // valid. Remove a possibly present native icon now, rather than leaving a
    // ghost tray entry until Explorer notices the dead callback window. This
    // is best-effort and deliberately doesn't clear desiredInstalled: a later
    // SetIcon() is allowed to create and reconcile a replacement helper.
    if ( helperHwnd && deletePossiblyPresentIcon )
    {
        NotifyIconData notifyData(helperHwnd);
#if wxUSE_EXCEPTIONS
        try
        {
            (void)wxWinUICallTaskBarIconShell(
                state, NIM_DELETE, &notifyData);
        }
        catch ( ... )
        {
            // Destructors must not propagate failures from a test seam.
        }
#else
        (void)wxWinUICallTaskBarIconShell(
            state, NIM_DELETE, &notifyData);
#endif
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3

// Operations
bool wxTaskBarIcon::SetIcon(const wxBitmapBundle& icon, const wxString& tooltip)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return DoSetIcon(icon, tooltip,
                     m_iconAdded ? Operation_Modify : Operation_Add);
#else
    if ( !DoSetIcon(icon, tooltip,
                    m_iconAdded ? Operation_Modify : Operation_Add) )
    {
        return false;
    }

    // We surely have it now, after setting it successfully (we could also have
    // had it before, but it's harmless to set this flag again in this case).
    m_iconAdded = true;

    return true;
#endif
}

bool
wxTaskBarIcon::DoSetIcon(const wxBitmapBundle& icon,
                         const wxString& tooltip,
                         Operation operation)
{
    // NB: we have to create the window lazily because of backward compatibility,
    //     old applications may create a wxTaskBarIcon instance before wxApp
    //     is initialized (as samples/taskbar used to do)
    if (!m_win)
    {
        m_win = new wxTaskBarIconWindow(this);
    }

    m_icon = icon;
    m_strTooltip = tooltip;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxUnusedVar(operation);
    const auto state = wxWinUIGetTaskBarIconState(this);
    if ( state->destroying )
        return false;

    state->helper = m_win;
    state->desiredIcon = icon;
    state->desiredTooltip = tooltip;
    state->desiredInstalled = true;
    const unsigned long long desiredGeneration =
        wxWinUIBumpTaskBarIconGeneration(state->desiredGeneration);

    const unsigned long long explorerGeneration =
        state->explorerGeneration;
    const WXHWND helperHwnd = m_win->GetHWND();
    const unsigned long long helperGeneration =
        wxWinUIMSWGetHwndGeneration(m_win, helperHwnd);

    // Publish intent before the native seam. Recursive RemoveIcon(), Explorer
    // restart, SetIcon() or destruction therefore supersedes this exact
    // generation. The public compatibility flag is published only by the
    // reconciler after both the icon and its version are proven current.
    const bool reconciled = wxWinUIReconcileTaskBarIcon(state);
    return reconciled && helperHwnd && helperGeneration &&
           wxWinUITaskBarIconPresentationIsCurrent(
               state, desiredGeneration, explorerGeneration,
               helperHwnd, helperGeneration);
#else

    NotifyIconData notifyData(GetHwndOf(m_win));

    if (icon.IsOk())
    {
        notifyData.uFlags |= NIF_ICON;
        m_realIcon = icon.GetIconFor(m_win);
        notifyData.hIcon = GetHiconOf(m_realIcon);
    }

    // set NIF_TIP even for an empty tooltip: otherwise it would be impossible
    // to remove an existing tooltip using this function
    notifyData.uFlags |= NIF_TIP;
    if ( !tooltip.empty() )
    {
        wxStrlcpy(notifyData.szTip, tooltip.t_str(), WXSIZEOF(notifyData.szTip));
    }

    switch ( operation )
    {
        case Operation_Add:
            if ( !Shell_NotifyIcon(NIM_ADD, &notifyData) )
            {
                wxLogLastError("Shell_NotifyIcon(NIM_ADD)");
                return false;
            }
            break;

        case Operation_Modify:
            if ( !Shell_NotifyIcon(NIM_MODIFY, &notifyData) )
            {
                wxLogLastError("Shell_NotifyIcon(NIM_MODIFY)");
                return false;
            }
            break;

        case Operation_TryBoth:
            if ( !Shell_NotifyIcon(NIM_ADD, &notifyData) &&
                    !Shell_NotifyIcon(NIM_MODIFY, &notifyData) )
            {
                wxLogLastError("Shell_NotifyIcon(NIM_ADD/NIM_MODIFY)");
                return false;
            }
            break;
    }

    return true;
#endif
}

#if wxUSE_TASKBARICON_BALLOONS

bool
wxTaskBarIcon::ShowBalloon(const wxString& title,
                           const wxString& text,
                           unsigned msec,
                           int flags,
                           const wxBitmapBundle& icon)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // A failed initial SetIcon() leaves a desired intent for an explicit
    // SetIcon()/TaskbarCreated retry, but it is not an installed icon and a
    // balloon must not implicitly turn it into one.
    if ( !m_iconAdded )
        return false;
#else
    wxCHECK_MSG( m_iconAdded, false,
                    wxT("can't be used before the icon is created") );
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto state = wxWinUIFindTaskBarIconState(this);
    if ( !state || !state->desiredInstalled ||
            !wxWinUIReconcileTaskBarIcon(state) )
    {
        return false;
    }

    wxWindow * const helper = wxWinUIResolveTaskBarIconHelper(state);
    if ( !helper )
        return false;
    const unsigned long long desiredGeneration =
        state->desiredGeneration;
    const unsigned long long explorerGeneration =
        state->explorerGeneration;
    const WXHWND helperHwnd = helper->GetHWND();
    const unsigned long long helperGeneration =
        wxWinUIMSWGetHwndGeneration(helper, helperHwnd);
    if ( !helperHwnd || !helperGeneration ||
         !wxWinUITaskBarIconPresentationIsCurrent(
             state, desiredGeneration, explorerGeneration,
             helperHwnd, helperGeneration) )
    {
        return false;
    }
    const HWND hwnd = static_cast<HWND>(helperHwnd);
#else
    const HWND hwnd = GetHwndOf(m_win);

    // we need to enable version 5.0 behaviour to receive notifications about
    // the balloon disappearance
    NotifyIconData notifyData(hwnd);
    notifyData.uFlags = 0;
    notifyData.uVersion = 3 /* NOTIFYICON_VERSION for Windows 2000/XP */;

    if ( !Shell_NotifyIcon(NIM_SETVERSION, &notifyData) )
    {
        wxLogLastError(wxT("Shell_NotifyIcon(NIM_SETVERSION)"));
    }
#endif

    // do show the balloon now
#if defined(__WXWINUI__) && wxUSE_WINUI3
    NotifyIconData notifyData(hwnd);
#else
    notifyData = NotifyIconData(hwnd);
#endif
    notifyData.uFlags |= NIF_INFO;
    notifyData.uTimeout = msec;
    wxStrlcpy(notifyData.szInfo, text.t_str(), WXSIZEOF(notifyData.szInfo));
    wxStrlcpy(notifyData.szInfoTitle, title.t_str(),
                WXSIZEOF(notifyData.szInfoTitle));

    wxUnusedVar(icon); // It's only unused if not supported actually.

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // This lease must outlive the block below: NOTIFYICONDATA only borrows the
    // HICON and the test seam is allowed to destroy both public owner and
    // hidden helper before the synchronous native call returns.
    wxIcon balloonIcon;
#endif

    if ( icon.IsOk() )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        balloonIcon = icon.GetIconFor(helper);
        if ( !wxWinUITaskBarIconPresentationIsCurrent(
                 state, desiredGeneration, explorerGeneration,
                 helperHwnd, helperGeneration) )
        {
            return false;
        }
        notifyData.hBalloonIcon = GetHiconOf(balloonIcon);
#else
        m_balloonIcon = icon.GetIconFor(m_win);
        notifyData.hBalloonIcon = GetHiconOf(m_balloonIcon);
#endif
        notifyData.dwInfoFlags |= NIIF_USER | NIIF_LARGE_ICON;
    }
    else if ( flags & wxICON_INFORMATION )
        notifyData.dwInfoFlags |= NIIF_INFO;
    else if ( flags & wxICON_WARNING )
        notifyData.dwInfoFlags |= NIIF_WARNING;
    else if ( flags & wxICON_ERROR )
        notifyData.dwInfoFlags |= NIIF_ERROR;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const bool ok =
        wxWinUICallTaskBarIconShell(state, NIM_MODIFY, &notifyData);

    // The native seam may synchronously replace/remove the desired icon,
    // recreate its helper, dispatch TaskbarCreated or destroy this object.
    // Never let the older balloon result overwrite that newer generation.
    if ( !wxWinUITaskBarIconPresentationIsCurrent(
             state, desiredGeneration, explorerGeneration,
             helperHwnd, helperGeneration) )
    {
        return false;
    }

    if ( !ok )
    {
        state->applied = wxWinUITaskBarIconAppliedState::Unknown;
        state->versionApplied = false;
        wxWinUILogTaskBarIconShellFailure(
            wxT("Shell_NotifyIcon(NIM_MODIFY)"));
    }

    return ok;
#else
    const bool ok = Shell_NotifyIcon(NIM_MODIFY, &notifyData) != 0;
    if ( !ok )
    {
        wxLogLastError(wxT("Shell_NotifyIcon(NIM_MODIFY)"));
    }

    return ok;
#endif
}

#endif // wxUSE_TASKBARICON_BALLOONS

bool wxTaskBarIcon::RemoveIcon()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto state = wxWinUIFindTaskBarIconState(this);
    // desiredInstalled, not the historical public flag, owns the operation:
    // this also cancels an initial SetIcon() intent whose ADD or SETVERSION
    // failed before m_iconAdded could legitimately become true.
    if ( !state || state->destroying || !state->desiredInstalled )
        return false;

    state->desiredInstalled = false;
    state->versionApplied = false;
    const unsigned long long desiredGeneration =
        wxWinUIBumpTaskBarIconGeneration(state->desiredGeneration);
    const unsigned long long explorerGeneration =
        state->explorerGeneration;
    m_iconAdded = false;

    const bool reconciled = wxWinUIReconcileTaskBarIcon(state);
    return reconciled && wxWinUITaskBarIconAbsenceIsCurrent(
                             state, desiredGeneration,
                             explorerGeneration);
#else
    if (!m_iconAdded)
        return false;

    m_iconAdded = false;

    NotifyIconData notifyData(GetHwndOf(m_win));

    bool ok = Shell_NotifyIcon(NIM_DELETE, &notifyData) != 0;
    if ( !ok )
    {
        wxLogLastError(wxT("Shell_NotifyIcon(NIM_DELETE)"));
    }

    return ok;
#endif
}

#if wxUSE_MENUS
bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxCHECK_MSG( menu, false, wxT("can't popup a null taskbar menu") );
    if ( !m_win || gs_taskBarIconPopupActive )
        return false;

    gs_taskBarIconPopupActive = true;
    wxScopeGuard popupActiveGuard = wxMakeGuard(
        []() { gs_taskBarIconPopupActive = false; });
    wxUnusedVar(popupActiveGuard);

    const wxWeakRef<wxEvtHandler> weakThis(this);
    const wxWeakRef<wxWindow> weakHelper(m_win);
    const auto state = wxWinUIFindTaskBarIconState(this);
    if ( !state )
        return false;

    const wxTaskBarIcon * const identity = this;
    ++state->popupDepth;
    wxScopeGuard popupDepthGuard = wxMakeGuard(
        [state, identity]()
        {
            wxASSERT(state->popupDepth != 0);
            --state->popupDepth;
            // identity is used only as an opaque map key: the taskbar object
            // is explicitly allowed to destroy itself from the menu command.
            wxWinUITryEraseTaskBarIconState(state, identity);
        });
    wxUnusedVar(popupDepthGuard);

    wxTaskBarIconWindow * const helper = m_win;
    int x, y;
    wxGetMousePosition(&x, &y);
    helper->Move(x, y);

    if ( weakThis.get() != this || weakHelper.get() != helper ||
            helper->IsBeingDeleted() )
    {
        return false;
    }

    // wxWindowBase::PopupMenu() owns the single UpdateUI pass. The helper's
    // weak forwarding handlers route both update and command events to the
    // taskbar object without linking its lifetime into a raw handler chain.
    ::SetForegroundWindow(GetHwndOf(helper));
    if ( weakThis.get() != this || weakHelper.get() != helper ||
            helper->IsBeingDeleted() )
    {
        return false;
    }

    const bool result = gs_taskBarIconPopupHookForTesting
        ? gs_taskBarIconPopupHookForTesting(helper, menu)
        : helper->PopupMenu(menu, 0, 0);

    wxWindow * const liveHelper = weakHelper.get();
    if ( liveHelper && !liveHelper->IsBeingDeleted() &&
            liveHelper->GetHWND() )
    {
        // Required by the Win32 notification-area popup-menu contract.
        (void)::PostMessage(GetHwndOf(liveHelper), WM_NULL, 0, 0L);
    }

    return result;
#else
    wxASSERT_MSG( m_win != nullptr, wxT("taskbar icon not initialized") );

    static bool s_inPopup = false;

    if (s_inPopup)
        return false;

    s_inPopup = true;

    int         x, y;
    wxGetMousePosition(&x, &y);

    m_win->Move(x, y);

    m_win->PushEventHandler(this);

    menu->UpdateUI();

    // the SetForegroundWindow() and PostMessage() calls are needed to work
    // around Win32 bug with the popup menus shown for the notifications as
    // documented at http://support.microsoft.com/kb/q135788/
    ::SetForegroundWindow(GetHwndOf(m_win));

    bool rval = m_win->PopupMenu(menu, 0, 0);

    ::PostMessage(GetHwndOf(m_win), WM_NULL, 0, 0L);

    m_win->PopEventHandler(false);

    s_inPopup = false;

    return rval;
#endif
}
#endif // wxUSE_MENUS

void wxTaskBarIcon::RegisterWindowMessages()
{
    static bool s_registered = false;

    if ( !s_registered )
    {
        // Taskbar restart msg will be sent to us if the icon needs to be redrawn
        gs_msgRestartTaskbar = RegisterWindowMessage(wxT("TaskbarCreated"));

        // Also register the taskbar message here
        gs_msgTaskbar = ::RegisterWindowMessage(wxT("wxTaskBarIconMessage"));

        s_registered = true;
    }
}

// ----------------------------------------------------------------------------
// wxTaskBarIcon window proc
// ----------------------------------------------------------------------------

long wxTaskBarIcon::WindowProc(unsigned int msg,
                               unsigned int WXUNUSED(wParam),
                               long lParam)
{
    if ( msg == gs_msgRestartTaskbar )   // does the icon need to be redrawn?
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // TaskbarCreated invalidates only our knowledge of Explorer state.
        // Reconciliation is driven exclusively by the already-published
        // desired state, so an explicit RemoveIcon() can never be undone here.
        (void)wxWinUIRestartTaskBarIcon(
            this, wxWinUIFindTaskBarIconState(this));
#else
        // We can get this message after the taskbar has been really recreated,
        // in which case we need to add our icon anew, or if it just needs to
        // be refreshed, in which case the existing icon just needs to be
        // updated, so try doing both in DoSetIcon().
        DoSetIcon(m_icon, m_strTooltip, Operation_TryBoth);
#endif
        return 0;
    }

    // this function should only be called for gs_msg(Restart)Taskbar messages
    wxASSERT( msg == gs_msgTaskbar );

    wxEventType eventType = 0;
    switch ( lParam )
    {
        case WM_LBUTTONDOWN:
            eventType = wxEVT_TASKBAR_LEFT_DOWN;
            break;

        case WM_LBUTTONUP:
            eventType = wxEVT_TASKBAR_LEFT_UP;
            break;

        case WM_RBUTTONDOWN:
            eventType = wxEVT_TASKBAR_RIGHT_DOWN;
            break;

        case WM_RBUTTONUP:
            eventType = wxEVT_TASKBAR_RIGHT_UP;
            break;

        case WM_LBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_LEFT_DCLICK;
            break;

        case WM_RBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_RIGHT_DCLICK;
            break;

        case WM_MOUSEMOVE:
            eventType = wxEVT_TASKBAR_MOVE;
            break;

        case NIN_BALLOONTIMEOUT:
            eventType = wxEVT_TASKBAR_BALLOON_TIMEOUT;
            break;

        case NIN_BALLOONUSERCLICK:
            eventType = wxEVT_TASKBAR_BALLOON_CLICK;
            break;
    }

    if ( eventType )
    {
        wxTaskBarIconEvent event(eventType, this);

        ProcessEvent(event);
    }

    return 0;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

void wxWinUISetTaskBarIconShellNotifyHookForTesting(
    wxWinUITaskBarIconShellNotifyHookForTesting hook)
{
    gs_taskBarIconShellNotifyHookForTesting = hook;
}

void wxWinUIResetTaskBarIconShellNotifyHookForTesting()
{
    gs_taskBarIconShellNotifyHookForTesting = nullptr;
}

void wxWinUISetTaskBarIconPopupHookForTesting(
    wxWinUITaskBarIconPopupHookForTesting hook)
{
    gs_taskBarIconPopupHookForTesting = hook;
}

void wxWinUIResetTaskBarIconPopupHookForTesting()
{
    gs_taskBarIconPopupHookForTesting = nullptr;
}

bool wxWinUITaskBarIconRestartExplorerForTesting(wxTaskBarIcon *icon)
{
    return icon && wxWinUIRestartTaskBarIcon(
                       icon, wxWinUIFindTaskBarIconState(icon));
}

namespace
{

bool wxWinUIDispatchTaskBarIconHelperMessageForTesting(
    wxTaskBarIcon *icon,
    UINT message,
    LPARAM lParam)
{
    const auto state = wxWinUIFindTaskBarIconState(icon);
    wxWindow * const helper = wxWinUIResolveTaskBarIconHelper(state);
    if ( !helper || !message )
        return false;

    const WXHWND helperHwnd = helper->GetHWND();
    const unsigned long long helperGeneration =
        wxWinUIMSWGetHwndGeneration(helper, helperHwnd);
    if ( !helperHwnd || !helperGeneration ||
            !::IsWindow(static_cast<HWND>(helperHwnd)) )
    {
        return false;
    }

    // The message may synchronously destroy the public owner and request
    // destruction of this helper. Do not dereference either identity after
    // SendMessage() returns: successful dispatch, not survival, is the seam's
    // contract.
    (void)::SendMessage(static_cast<HWND>(helperHwnd), message, 0, lParam);
    return true;
}

} // anonymous namespace

bool wxWinUITaskBarIconDispatchTaskbarCreatedForTesting(
    wxTaskBarIcon *icon)
{
    return wxWinUIDispatchTaskBarIconHelperMessageForTesting(
        icon, gs_msgRestartTaskbar, 0);
}

bool wxWinUITaskBarIconDispatchEventForTesting(
    wxTaskBarIcon *icon,
    long nativeEvent)
{
    return wxWinUIDispatchTaskBarIconHelperMessageForTesting(
        icon, gs_msgTaskbar, static_cast<LPARAM>(nativeEvent));
}

bool wxWinUITaskBarIconDestroyHelperForTesting(wxTaskBarIcon *icon)
{
    return wxWinUITaskBarIconSidecarAccess::DestroyHelper(icon);
}

bool wxWinUIGetTaskBarIconSnapshotForTesting(
    wxTaskBarIcon *icon,
    wxWinUITaskBarIconSnapshotForTesting *snapshot)
{
    if ( !icon || !snapshot )
        return false;

    const auto state = wxWinUIFindTaskBarIconState(icon);
    if ( !state )
        return false;

    snapshot->desiredInstalled = state->desiredInstalled;
    snapshot->applied = state->applied;
    snapshot->versionApplied = state->versionApplied;
    snapshot->reconciling = state->reconciling;
    snapshot->reconcilePending = state->reconcilePending;
    snapshot->destroying = state->destroying;
    snapshot->desiredGeneration = state->desiredGeneration;
    snapshot->appliedGeneration = state->appliedGeneration;
    snapshot->explorerGeneration = state->explorerGeneration;
    snapshot->appliedExplorerGeneration =
        state->appliedExplorerGeneration;
    snapshot->nativeCallCount = state->nativeCallCount;
    return true;
}

unsigned wxWinUIGetTaskBarIconStateCountForTesting()
{
    return static_cast<unsigned>(wxWinUIGetTaskBarIconStates().size());
}

unsigned wxWinUIGetTaskBarIconHelperCountForTesting()
{
    return gs_taskBarIconHelperCount;
}

#endif // __WXWINUI__ && wxUSE_WINUI3

#endif // wxUSE_TASKBARICON

