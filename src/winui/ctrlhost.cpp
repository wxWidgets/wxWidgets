/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/ctrlhost.cpp
// Purpose:     private wxWinUI host implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"

#include "wx/winui/private/appearance.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/dialogcontracts.h"
#include "wx/winui/private/shelltheme.h"
#include "wx/winui/private/transient.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
    #include "wx/cursor.h"
    #include "wx/font.h"
#endif

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/module.h"
#include "wx/settings.h"
#include "wx/msw/private/darkmode.h"
#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/toplevel.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/msw/private.h"
#if wxUSE_TEXTCTRL
    #include "wx/textctrl.h"
#endif

#include <dwmapi.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.Input.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <cstdarg>
#include <clocale>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <vector>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
    #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
    #define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_MAINWINDOW
    #define DWMSBT_MAINWINDOW 2  // Mica
#endif
#ifndef DWMSBT_NONE
    #define DWMSBT_NONE 1
#endif
#ifndef DWMSBT_TRANSIENTWINDOW
    #define DWMSBT_TRANSIENTWINDOW 3  // Mica Alt, for popups/dialogs
#endif

void wxWinUILogException(const char *what, const winrt::hresult_error& e);

namespace
{

const wchar_t wxWinUIBackdropTransparentProp[] =
    L"wxWinUIBackdropTransparent";

wxWinUIAppTheme gs_winuiAppTheme = wxWinUIAppTheme::System;
winrt::Microsoft::UI::Xaml::ElementTheme gs_winuiElementTheme =
    winrt::Microsoft::UI::Xaml::ElementTheme::Default;
wxWinUIShellThemeNativeOps gs_winuiShellThemeTestOps;
bool gs_hasWinUIShellThemeTestOps = false;

// Read the system "apps use light theme" preference directly from the registry,
// which works regardless of whether the classic MSW dark mode support has been
// enabled (the WinUI port bypasses it).
bool wxWinUISystemUsesDarkMode()
{
    DWORD value = 1;
    DWORD size = sizeof(value);
    if ( ::RegGetValueW(
             HKEY_CURRENT_USER,
             L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
             L"AppsUseLightTheme",
             RRF_RT_REG_DWORD,
             nullptr,
             &value,
             &size) == ERROR_SUCCESS )
    {
        return value == 0;
    }

    return false;
}

wxWinUIShellThemePolicy
wxWinUIResolveShellThemePolicy(wxWinUIAppTheme appTheme,
                               bool systemDark,
                               bool highContrast)
{
    wxWinUIShellThemePolicy policy;
    policy.highContrast = highContrast;
    policy.allowBackdrop = !highContrast;

    if ( highContrast )
        return policy;

    switch ( appTheme )
    {
        case wxWinUIAppTheme::Light:
            policy.dark = false;
            break;

        case wxWinUIAppTheme::Dark:
            policy.dark = true;
            break;

        case wxWinUIAppTheme::System:
            policy.dark = systemDark;
            break;
    }

    return policy;
}

wxWinUIShellThemePolicy wxWinUIGetShellThemePolicy()
{
    return wxWinUIResolveShellThemePolicy(
        gs_winuiAppTheme,
        wxWinUISystemUsesDarkMode(),
        wxWinUIIsHighContrastActive());
}

// Applications paint the surfaces they own themselves using the colours
// returned by wxSystemSettings, and so does all the wxMSW code reused by this
// port. Those colours come from the classic dark mode support, which this port
// doesn't otherwise use, so it has to be told which appearance is really being
// shown -- without this, a dark WinUI window is filled with light colours by
// everything that doesn't go through a XAML peer.
//
// High Contrast resolves to a non-dark policy on purpose: the classic light
// path then returns the accessibility palette from the system, which is what
// High Contrast requires.
void wxWinUISyncClassicAppearance()
{
    wxMSWDarkMode::SyncWithWinUITheme(wxWinUIGetShellThemePolicy().dark);
}

// Once the WinUI runtime is in use, wxSetlocale() keeps LC_NUMERIC as "C"
// because the runtime formats the numbers of its own XAML markup with it (and
// terminates the process when the result doesn't parse back). An application
// calling the CRT setlocale() directly bypasses wxSetlocale(), so re-assert it
// wherever the port is about to hand work to XAML.
void wxWinUIEnsureCNumericLocale()
{
    const char * const current = ::setlocale(LC_NUMERIC, nullptr);
    if ( current && strcmp(current, "C") != 0 )
    {
        wxLogTrace("winui",
                   "LC_NUMERIC was \"%s\": resetting it to \"C\", the WinUI "
                   "runtime cannot parse its own XAML with a decimal comma",
                   current);
    }

    wxKeepCNumericLocale(true);
}

// WinUI-like solid background colours used only when the DWM Mica backdrop is
// not available.
wxColour wxWinUIBackgroundColour(const wxWinUIShellThemePolicy& policy)
{
    // Hard-coded Fluent neutrals are unreadable in several High Contrast
    // palettes. Let the active system palette choose this surface instead.
    if ( policy.highContrast )
        return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    return policy.dark ? wxColour(32, 32, 32)
                       : wxColour(243, 243, 243);
}

bool wxWinUISetImmersiveDarkModeNative(void *,
                                       WXHWND hwndArg,
                                       bool dark)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const BOOL useDark = dark;
    return hwnd &&
           SUCCEEDED(::DwmSetWindowAttribute(
               hwnd,
               DWMWA_USE_IMMERSIVE_DARK_MODE,
               &useDark,
               sizeof(useDark)));
}

bool wxWinUISetSystemBackdropNative(void *,
                                    WXHWND hwndArg,
                                    bool enabled)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( !hwnd )
        return false;

    int backdrop = enabled ? DWMSBT_MAINWINDOW : DWMSBT_NONE;
    const bool applied =
        SUCCEEDED(::DwmSetWindowAttribute(
            hwnd,
            DWMWA_SYSTEMBACKDROP_TYPE,
            &backdrop,
            sizeof(backdrop)));
    return enabled && applied;
}

bool wxWinUIExtendFrameNative(void *,
                              WXHWND hwndArg,
                              bool enabled)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( !hwnd )
        return false;

    const MARGINS margins = enabled
        ? MARGINS{ -1, -1, -1, -1 }
        : MARGINS{ 0, 0, 0, 0 };
    return SUCCEEDED(::DwmExtendFrameIntoClientArea(hwnd, &margins));
}

bool wxWinUISetBackdropMarkerNative(void *,
                                    WXHWND hwndArg,
                                    bool enabled)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( !hwnd )
        return false;

    if ( enabled )
    {
        return ::SetPropW(
                   hwnd,
                   wxWinUIBackdropTransparentProp,
                   reinterpret_cast<HANDLE>(1)) != FALSE;
    }

    ::RemovePropW(hwnd, wxWinUIBackdropTransparentProp);
    return ::GetPropW(hwnd, wxWinUIBackdropTransparentProp) == nullptr;
}

// Theme a window's native scrollbars (and other common controls) to match the
// resolved shell policy. SetWindowTheme is loaded dynamically to avoid pulling
// in <uxtheme.h> here (which conflicts with the C++/WinRT headers).
bool wxWinUISetControlThemeNative(void *,
                                  WXHWND hwndArg,
                                  bool dark,
                                  bool highContrast)
{
    typedef HRESULT (WINAPI *SetWindowTheme_t)(HWND, LPCWSTR, LPCWSTR);
    static SetWindowTheme_t s_setWindowTheme = []() -> SetWindowTheme_t
    {
        HMODULE module = ::GetModuleHandleW(L"uxtheme.dll");
        if ( !module )
            module = ::LoadLibraryW(L"uxtheme.dll");
        return module
            ? reinterpret_cast<SetWindowTheme_t>(
                  ::GetProcAddress(module, "SetWindowTheme"))
            : nullptr;
    }();

    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( !hwnd || !s_setWindowTheme )
        return false;

    // A null sub-app restores the system-selected control theme. In High
    // Contrast this is essential: forcing either Explorer theme would retain
    // decorative light/dark brushes instead of the accessibility palette.
    return SUCCEEDED(s_setWindowTheme(
        hwnd,
        highContrast ? nullptr
                     : (dark ? L"DarkMode_Explorer" : L"Explorer"),
        nullptr));
}

const wxWinUIShellThemeNativeOps& wxWinUIGetShellThemeOps()
{
    if ( gs_hasWinUIShellThemeTestOps )
        return gs_winuiShellThemeTestOps;

    static const wxWinUIShellThemeNativeOps operations =
    {
        nullptr,
        wxWinUISetImmersiveDarkModeNative,
        wxWinUISetSystemBackdropNative,
        wxWinUIExtendFrameNative,
        wxWinUISetBackdropMarkerNative,
        wxWinUISetControlThemeNative
    };
    return operations;
}

bool wxWinUIShellThemeOpsAreComplete(
    const wxWinUIShellThemeNativeOps& operations)
{
    return operations.setImmersiveDarkMode &&
           operations.setSystemBackdrop &&
           operations.extendFrameIntoClientArea &&
           operations.setBackdropMarker &&
           operations.setControlTheme;
}

struct wxWinUIShellWindowIdentity
{
    wxWeakRef<wxWindow> window;
    wxWindow *pointer = nullptr;
    WXHWND hwnd = nullptr;
    unsigned long long hwndGeneration = 0;
};

struct wxWinUIShellApplyEpoch
{
    unsigned long long hwndGeneration = 0;
    unsigned long long epoch = 0;
};

struct wxWinUIShellBackgroundState
{
    wxWeakRef<wxWindow> window;
    wxWindow *pointer = nullptr;
    WXHWND hwnd = nullptr;
    unsigned long long hwndGeneration = 0;
    bool hadBackground = false;
    bool inherited = false;
    wxColour background;
    wxColour applied;
};

struct wxWinUIShellApplyDriver
{
    wxWeakRef<wxWindow> window;
    wxWindow *pointer = nullptr;
    WXHWND hwnd = nullptr;
    unsigned long long hwndGeneration = 0;
    bool inProgress = false;
    bool rerunRequested = false;
};

std::map<HWND, wxWinUIShellApplyEpoch> gs_winuiShellApplyEpochs;
std::map<HWND, wxWinUIShellBackgroundState>
    gs_winuiShellBackgroundStates;
std::map<HWND, std::shared_ptr<wxWinUIShellApplyDriver>>
    gs_winuiShellApplyDrivers;
unsigned long long gs_nextWinUIShellApplyEpoch = 0;

bool wxWinUIShellIdentityIsCurrent(
    const wxWinUIShellWindowIdentity& identity)
{
    wxWindow * const window = identity.window.get();
    return window &&
           window == identity.pointer &&
           !window->IsBeingDeleted() &&
           !wxPendingDelete.Member(window) &&
           window->GetHWND() == identity.hwnd &&
           identity.hwndGeneration &&
           wxWinUIMSWGetHwndGeneration(window, identity.hwnd) ==
               identity.hwndGeneration;
}

unsigned long long wxWinUIPublishShellApplyEpoch(
    const wxWinUIShellWindowIdentity& identity)
{
    if ( !wxWinUIShellIdentityIsCurrent(identity) )
        return 0;

    if ( ++gs_nextWinUIShellApplyEpoch == 0 )
        ++gs_nextWinUIShellApplyEpoch;

    const HWND hwnd = reinterpret_cast<HWND>(identity.hwnd);
    gs_winuiShellApplyEpochs[hwnd] =
        { identity.hwndGeneration, gs_nextWinUIShellApplyEpoch };
    return gs_nextWinUIShellApplyEpoch;
}

bool wxWinUIShellApplyEpochIsCurrent(
    const wxWinUIShellWindowIdentity& identity,
    unsigned long long epoch)
{
    if ( !epoch || !wxWinUIShellIdentityIsCurrent(identity) )
        return false;

    const HWND hwnd = reinterpret_cast<HWND>(identity.hwnd);
    const auto current = gs_winuiShellApplyEpochs.find(hwnd);
    return current != gs_winuiShellApplyEpochs.end() &&
           current->second.hwndGeneration == identity.hwndGeneration &&
           current->second.epoch == epoch;
}

void wxWinUIRetireShellApplyEpoch(
    const wxWinUIShellWindowIdentity& identity,
    unsigned long long epoch)
{
    const HWND hwnd = reinterpret_cast<HWND>(identity.hwnd);
    const auto current = gs_winuiShellApplyEpochs.find(hwnd);
    if ( current != gs_winuiShellApplyEpochs.end() &&
         current->second.hwndGeneration == identity.hwndGeneration &&
         current->second.epoch == epoch )
    {
        gs_winuiShellApplyEpochs.erase(current);
    }
}

struct wxWinUIShellApplyEpochGuard
{
    wxWinUIShellApplyEpochGuard(
        const wxWinUIShellWindowIdentity& identity_,
        unsigned long long epoch_)
        : identity(identity_),
          epoch(epoch_)
    {
    }

    wxWinUIShellWindowIdentity identity;
    unsigned long long epoch = 0;

    ~wxWinUIShellApplyEpochGuard()
    {
        wxWinUIRetireShellApplyEpoch(identity, epoch);
    }
};

void wxWinUICaptureShellWindowTree(
    wxWindow *window,
    std::vector<wxWinUIShellWindowIdentity>& identities)
{
    if ( !window )
        return;

    wxWinUIShellWindowIdentity identity;
    identity.window = window;
    identity.pointer = window;
    identity.hwnd = window->GetHWND();
    identity.hwndGeneration =
        wxWinUIMSWGetHwndGeneration(window, identity.hwnd);
    if ( !wxWinUIShellIdentityIsCurrent(identity) )
        return;

    identities.push_back(identity);
    for ( wxWindowList::const_iterator i = window->GetChildren().begin();
          i != window->GetChildren().end();
          ++i )
    {
        // Owned TLWs (notably wxDialog) are present in their owner's child
        // list, but have an independent DWM surface and independent shell
        // transaction. Marking them from the owner's result can make their
        // client transparent without enabling a backdrop on their own HWND.
        if ( !(*i)->IsTopLevel() )
            wxWinUICaptureShellWindowTree(*i, identities);
    }
}

void wxWinUIApplyShellBackground(
    const wxWinUIShellWindowIdentity& identity,
    const wxWinUIShellThemePolicy& policy,
    bool materialEnabled)
{
    if ( !wxWinUIShellIdentityIsCurrent(identity) )
        return;

    wxWindow * const window = identity.window.get();
    if ( !window )
        return;

    const HWND hwnd = reinterpret_cast<HWND>(identity.hwnd);
    auto stateIt = gs_winuiShellBackgroundStates.find(hwnd);
    if ( stateIt != gs_winuiShellBackgroundStates.end() &&
         (stateIt->second.pointer != identity.pointer ||
          stateIt->second.hwndGeneration != identity.hwndGeneration ||
          stateIt->second.window.get() != identity.pointer) )
    {
        // Numeric HWND reuse or a retired wx identity: never restore state
        // captured for the previous association.
        gs_winuiShellBackgroundStates.erase(stateIt);
        stateIt = gs_winuiShellBackgroundStates.end();
    }

    const auto restoreOwnedFallback =
        [&]()
        {
            stateIt = gs_winuiShellBackgroundStates.find(hwnd);
            if ( stateIt == gs_winuiShellBackgroundStates.end() )
                return;

            const wxWinUIShellBackgroundState state = stateIt->second;
            gs_winuiShellBackgroundStates.erase(stateIt);
            stateIt = gs_winuiShellBackgroundStates.end();

            // An application mutation made while the fallback was active is
            // authoritative and must not be overwritten during restoration.
            if ( !wxWinUIShellIdentityIsCurrent(identity) ||
                 !window->UseBackgroundColour() ||
                 !window->InheritsBackgroundColour() ||
                 window->GetBackgroundColour() != state.applied )
            {
                return;
            }

            if ( !state.hadBackground )
            {
                window->SetBackgroundColour(wxNullColour);
            }
            else if ( state.inherited )
            {
                window->SetBackgroundColour(state.background);
            }
            else
            {
                window->SetOwnBackgroundColour(state.background);
            }
        };

    if ( materialEnabled || policy.highContrast )
    {
        // High Contrast must remain owned by the native control classes and
        // their role-specific system colours. In particular, applying
        // COLOR_WINDOW recursively would turn buttons, lists and panels into
        // custom-colour controls and defeat SetWindowTheme(nullptr).
        restoreOwnedFallback();
        return;
    }

    if ( stateIt != gs_winuiShellBackgroundStates.end() )
    {
        const wxWinUIShellBackgroundState& state = stateIt->second;
        const bool stillOwnsCurrent =
            window->UseBackgroundColour() &&
            window->InheritsBackgroundColour() &&
            window->GetBackgroundColour() == state.applied;
        if ( !stillOwnsCurrent )
        {
            // The application changed the colour while a fallback was active.
            gs_winuiShellBackgroundStates.erase(stateIt);
            stateIt = gs_winuiShellBackgroundStates.end();
        }
        else if ( state.hadBackground )
        {
            // Older/in-flight state may have captured an explicit baseline.
            // Restore it before deciding whether a non-HC fallback is needed:
            // both SetBackgroundColour() and SetOwnBackgroundColour() are
            // application-authored, regardless of the inheritance bit.
            restoreOwnedFallback();
            return;
        }
    }

    if ( stateIt == gs_winuiShellBackgroundStates.end() )
    {
        // Any explicit wx background is authoritative outside High Contrast.
        // InheritsBackgroundColour() says whether children inherit this
        // colour; it does not mean that this window inherited the colour from
        // its parent.
        if ( window->UseBackgroundColour() )
            return;

        wxWinUIShellBackgroundState state;
        state.window = window;
        state.pointer = identity.pointer;
        state.hwnd = identity.hwnd;
        state.hwndGeneration = identity.hwndGeneration;
        state.hadBackground = window->UseBackgroundColour();
        state.inherited = window->InheritsBackgroundColour();
        if ( state.hadBackground )
            state.background = window->GetBackgroundColour();
        state.applied = wxWinUIBackgroundColour(policy);
        gs_winuiShellBackgroundStates[hwnd] = state;
    }
    else
    {
        stateIt->second.applied = wxWinUIBackgroundColour(policy);
    }

    // Publish provenance before the virtual call: a synchronous nested theme
    // notification can then supersede this epoch without losing restoration.
    const wxColour applied =
        gs_winuiShellBackgroundStates[hwnd].applied;
    window->SetBackgroundColour(applied);
}

enum class wxWinUIShellPresentationResult
{
    Applied,
    Superseded,
    MarkerRejected
};

wxWinUIShellPresentationResult wxWinUIApplyMicaBackground(
    const std::vector<wxWinUIShellWindowIdentity>& identities,
    const wxWinUIShellWindowIdentity& root,
    unsigned long long epoch,
    const wxWinUIShellThemePolicy& policy,
    bool materialEnabled)
{
    const wxWinUIShellThemeNativeOps& operations =
        wxWinUIGetShellThemeOps();

    bool markerRejected = false;
    for ( const wxWinUIShellWindowIdentity& identity : identities )
    {
        if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
            return wxWinUIShellPresentationResult::Superseded;

        if ( !wxWinUIShellIdentityIsCurrent(identity) )
            continue;

        // The prop drives WM_ERASEBKGND in src/msw/window.cpp. The operation
        // table makes the High Contrast transition and failure path fully
        // deterministic without changing the user's desktop settings.
        const bool markerApplied =
            operations.setBackdropMarker(
                operations.context, identity.hwnd, materialEnabled);
        if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
            return wxWinUIShellPresentationResult::Superseded;
        if ( wxWinUIShellIdentityIsCurrent(identity) &&
             !markerApplied )
        {
            markerRejected = true;
        }
    }

    // Transparency is all-or-nothing for a DWM material tree. The caller
    // invokes us again with false first, retracting every marker and publishing
    // the solid surface, and only then rolls back DWM/the extended frame.
    if ( materialEnabled && markerRejected )
        return wxWinUIShellPresentationResult::MarkerRejected;

    for ( const wxWinUIShellWindowIdentity& identity : identities )
    {
        if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
            return wxWinUIShellPresentationResult::Superseded;
        if ( !wxWinUIShellIdentityIsCurrent(identity) )
            continue;

        operations.setControlTheme(
            operations.context,
            identity.hwnd,
            policy.dark,
            policy.highContrast);
        if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
            return wxWinUIShellPresentationResult::Superseded;

        // A solid fallback is a property of the top-level surface. Native
        // controls below it must retain their role-specific system palette,
        // especially in High Contrast.
        if ( identity.pointer == root.pointer )
        {
            wxWinUIApplyShellBackground(
                identity, policy, materialEnabled);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return wxWinUIShellPresentationResult::Superseded;
        }
    }

    return markerRejected
        ? wxWinUIShellPresentationResult::MarkerRejected
        : wxWinUIShellPresentationResult::Applied;
}

} // namespace

wxWinUIShellThemePolicy
wxWinUI3ResolveShellThemePolicyForTesting(wxWinUIAppTheme appTheme,
                                          bool systemDark,
                                          bool highContrast)
{
    return wxWinUIResolveShellThemePolicy(
        appTheme, systemDark, highContrast);
}

void wxWinUI3SetShellThemeNativeOpsForTesting(
    const wxWinUIShellThemeNativeOps& operations)
{
    wxCHECK_RET(wxIsMainThread(),
                "WinUI shell-theme seam is UI-thread only");
    wxCHECK_RET(wxWinUIShellThemeOpsAreComplete(operations),
                "incomplete WinUI shell-theme operation table");
    wxCHECK_RET(gs_winuiShellApplyEpochs.empty() &&
                    gs_winuiShellApplyDrivers.empty(),
                "cannot replace WinUI shell-theme operations in flight");

    gs_winuiShellThemeTestOps = operations;
    gs_hasWinUIShellThemeTestOps = true;
}

void wxWinUI3ResetShellThemeNativeOpsForTesting()
{
    wxCHECK_RET(wxIsMainThread(),
                "WinUI shell-theme seam is UI-thread only");
    wxCHECK_RET(gs_winuiShellApplyEpochs.empty() &&
                    gs_winuiShellApplyDrivers.empty(),
                "cannot reset WinUI shell-theme operations in flight");

    gs_winuiShellThemeTestOps = wxWinUIShellThemeNativeOps();
    gs_hasWinUIShellThemeTestOps = false;
}

void wxWinUIShellThemeWindowDestroyed(WXHWND hwndArg)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( hwnd )
    {
        gs_winuiShellApplyEpochs.erase(hwnd);
        gs_winuiShellBackgroundStates.erase(hwnd);
        gs_winuiShellApplyDrivers.erase(hwnd);
    }
}

void wxWinUILogException(const char *what, const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
}

void wxWinUIDebugLog(const char *format, ...)
{
#if !wxUSE_WINUI3_DEBUG_LOG
    wxUnusedVar(format);
#else
    va_list argptr;
    va_start(argptr, format);
    const wxString message = wxString::FormatV(wxString::FromAscii(format),
                                               argptr);
    va_end(argptr);

    const wxString line = wxString::Format(
        "[%llu pid=%lu tid=%lu] %s\r\n",
        static_cast<unsigned long long>(::GetTickCount64()),
        static_cast<unsigned long>(::GetCurrentProcessId()),
        static_cast<unsigned long>(::GetCurrentThreadId()),
        message.c_str());

    ::OutputDebugString(line.t_str());

    const DWORD required = ::GetEnvironmentVariableW(
        L"WX_WINUI_DIAGNOSTICS_DIR", nullptr, 0);
    if ( !required )
        return;

    std::vector<wchar_t> directory(required);
    const DWORD copied = ::GetEnvironmentVariableW(
        L"WX_WINUI_DIAGNOSTICS_DIR", directory.data(), required);
    if ( !copied || copied >= required || !directory[0] )
        return;

    wxString filename(directory.data());
    if ( !filename.EndsWith(wxS("\\")) &&
         !filename.EndsWith(wxS("/")) )
    {
        filename += wxS("\\");
    }
    filename += wxS("winui-debug.txt");

    HANDLE file = ::CreateFileW(
        filename.wc_str(),
        FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if ( file == INVALID_HANDLE_VALUE )
        return;

    const wxCharBuffer utf8 = line.utf8_str();
    const char *data = utf8.data();
    if ( data )
    {
        DWORD written = 0;
        ::WriteFile(file,
                    data,
                    static_cast<DWORD>(std::strlen(data)),
                    &written,
                    nullptr);
        ::FlushFileBuffers(file);
    }

    ::CloseHandle(file);
#endif // wxUSE_WINUI3_DEBUG_LOG
}

winrt::hstring wxWinUIToHString(const wxString& str)
{
    return winrt::hstring(str.ToStdWstring());
}

wxString wxWinUIFromHString(const winrt::hstring& str)
{
    return wxString(str.c_str());
}

wxString wxWinUIRemoveMnemonics(const wxString& label)
{
    wxString stripped;
    stripped.reserve(label.length());

    for ( size_t i = 0; i < label.length(); ++i )
    {
        if ( label[i] == '&' )
        {
            if ( i + 1 < label.length() && label[i + 1] == '&' )
            {
                stripped += '&';
                ++i;
            }
        }
        else
        {
            stripped += label[i];
        }
    }

    return stripped;
}

void wxWinUISetDialogText(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& text,
                          const wxString& value)
{
    using namespace winrt::Microsoft::UI::Xaml;

    text.Text(wxWinUIToHString(value));
    text.TextWrapping(TextWrapping::Wrap);
    text.IsTextSelectionEnabled(true);
}

bool wxWinUIIsHostWindow(wxWindow *win)
{
    if ( !win )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(win);
    return host && host->FindSlot(win) != nullptr;
}

namespace
{

// COM interface giving direct access to an IBuffer's bytes.
struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da"))
IWxHostBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall Buffer(uint8_t** value) = 0;
};

#if wxUSE_TOOLTIPS
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

struct wxWinUIManagedToolTip
{
    enum class PolicyPhase
    {
        Attached,
        SuppressingClose,
        SuppressingDisable,
        SuppressingDetach,
        Suppressed,
        RestoringEnable,
        RestoringAttach
    };

    winrt::weak_ref<MUX::UIElement> element;
    MUXC::ToolTip toolTip{ nullptr };
    bool suppressed = false;
    PolicyPhase policyPhase = PolicyPhase::Attached;
    unsigned long long revision = 0;
    int maximumWidth = 0;
    unsigned policyFailureCount = 0;
    bool policyQuarantined = false;
};

bool gs_winuiToolTipsEnabled = true;
int gs_winuiToolTipMaxWidth = 0;
unsigned long long gs_winuiToolTipPolicyGeneration = 1;
unsigned long long gs_winuiToolTipEntryRevision = 1;
bool gs_winuiToolTipPolicyActive = false;
bool gs_winuiToolTipPolicyReplayRequested = false;
bool gs_winuiToolTipPolicyReplayScheduled = false;
bool gs_winuiToolTipPolicyReplayCallbackActive = false;
bool gs_winuiToolTipPolicyShutdown = false;
bool gs_winuiToolTipPolicyTransactionQuarantined = false;
bool gs_winuiToolTipHostSyncQuarantined = false;
unsigned gs_winuiToolTipHostSyncFailureCount = 0;
unsigned gs_winuiToolTipDeferredReplayCount = 0;
unsigned long long gs_winuiToolTipReplayToken = 1;
std::vector<wxWinUIManagedToolTip> gs_winuiManagedToolTips;

constexpr unsigned wxWinUIToolTipMaxPersistentFailures = 12;
constexpr unsigned wxWinUIToolTipMaxDeferredReplays = 2;

unsigned gs_winuiToolTipSetBeforeCommitFaults = 0;
unsigned gs_winuiToolTipSetAfterCommitFaults = 0;
unsigned gs_winuiToolTipPolicyFaults = 0;
unsigned gs_winuiToolTipPolicyFaultAttempts = 0;
unsigned gs_winuiToolTipPolicyReplaySchedules = 0;
unsigned gs_winuiToolTipPolicyReplayExecutions = 0;

bool wxWinUIConsumeToolTipFault(unsigned& remaining)
{
    if ( !remaining )
        return false;

    if ( remaining != std::numeric_limits<unsigned>::max() )
        --remaining;
    return true;
}

unsigned long long wxWinUINextToolTipEntryRevision()
{
    if ( ++gs_winuiToolTipEntryRevision == 0 )
        ++gs_winuiToolTipEntryRevision;
    return gs_winuiToolTipEntryRevision;
}

void wxWinUIApplyToolTipMaximum(const MUXC::ToolTip& toolTip,
                                const MUX::UIElement& element,
                                wxWindow *owner,
                                int maximumWidth)
{
    if ( !toolTip )
        return;

    if ( maximumWidth == 0 )
    {
        toolTip.ClearValue(MUX::FrameworkElement::MaxWidthProperty());
    }
    else
    {
        // wxToolTip::SetMaxWidth() is specified in physical pixels, whereas
        // XAML measures MaxWidth in DIPs. Prefer the owning wx window because
        // a freshly-created element does not necessarily have a XamlRoot yet.
        double scale = 1.0;
        if ( owner )
        {
            const int dpi = owner->GetDPI().GetWidth();
            if ( dpi > 0 )
                scale = static_cast<double>(dpi) / 96.0;
        }
        else if ( const auto framework = element.try_as<MUX::FrameworkElement>() )
        {
            if ( const auto root = framework.XamlRoot() )
                scale = root.RasterizationScale();
        }

        // XAML has no "do not wrap" sentinel. A very large finite DIP value
        // preserves that contract without feeding infinity/DBL_MAX into its
        // layout arithmetic.
        toolTip.MaxWidth(maximumWidth < 0
                             ? 1000000.0
                             : static_cast<double>(maximumWidth) /
                                   std::max(scale, 0.01));
    }
}

void wxWinUIPurgeDeadManagedToolTips()
{
    gs_winuiManagedToolTips.erase(
        std::remove_if(
            gs_winuiManagedToolTips.begin(),
            gs_winuiManagedToolTips.end(),
            [](const wxWinUIManagedToolTip& managed)
            {
                return !managed.element.get() || !managed.toolTip;
            }),
        gs_winuiManagedToolTips.end());
}

auto wxWinUIFindManagedToolTip(const MUX::UIElement& element)
{
    return std::find_if(
        gs_winuiManagedToolTips.begin(),
        gs_winuiManagedToolTips.end(),
        [&element](const wxWinUIManagedToolTip& managed)
        {
            return managed.element.get() == element;
        });
}

bool wxWinUIManagedToolTipIsCurrent(
    const MUX::UIElement& element,
    const MUXC::ToolTip& toolTip,
    unsigned long long revision)
{
    const auto current = wxWinUIFindManagedToolTip(element);
    return current != gs_winuiManagedToolTips.end() &&
           current->toolTip == toolTip &&
           current->revision == revision;
}

bool wxWinUIManagedToolTipOwnsNull(
    const wxWinUIManagedToolTip& managed)
{
    using Phase = wxWinUIManagedToolTip::PolicyPhase;

    // SuppressingClose/SuppressingDisable have not written the attached
    // property yet: a null observed in either phase is an application clear,
    // not ours. From SuppressingDetach onwards, null is a value published by
    // the policy transaction (including a setter which committed and then
    // entered a callback before returning).
    return managed.suppressed ||
           managed.policyPhase == Phase::SuppressingDetach ||
           managed.policyPhase == Phase::RestoringEnable ||
           managed.policyPhase == Phase::RestoringAttach;
}

bool wxWinUISetManagedToolTipPhaseIfCurrent(
    const MUX::UIElement& element,
    const MUXC::ToolTip& toolTip,
    unsigned long long revision,
    wxWinUIManagedToolTip::PolicyPhase phase)
{
    const auto current = wxWinUIFindManagedToolTip(element);
    if ( current == gs_winuiManagedToolTips.end() ||
         current->toolTip != toolTip ||
         current->revision != revision )
    {
        return false;
    }

    current->policyPhase = phase;
    return true;
}

void wxWinUIEraseManagedToolTipIfCurrent(
    const MUX::UIElement& element,
    const MUXC::ToolTip& toolTip,
    unsigned long long revision)
{
    const auto current = wxWinUIFindManagedToolTip(element);
    if ( current != gs_winuiManagedToolTips.end() &&
         current->toolTip == toolTip &&
         current->revision == revision )
    {
        gs_winuiManagedToolTips.erase(current);
    }
}
#endif // wxUSE_TOOLTIPS

} // namespace

winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap
wxWinUIWriteableBitmapFromBitmap(const wxBitmap& bitmap)
{
    if ( !bitmap.IsOk() )
        return nullptr;

    wxImage image = bitmap.ConvertToImage();
    if ( !image.IsOk() )
        return nullptr;

    // Many icons (e.g. from wxArtProvider) carry a colour mask rather than a
    // real alpha channel; without this the masked (transparent) areas would
    // render as opaque black.  Convert the mask to alpha so they composite
    // transparently.
    if ( !image.HasAlpha() && image.HasMask() )
        image.InitAlpha();

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const unsigned char *rgb = image.GetData();
    const unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;

    winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap wb(w, h);

    uint8_t *dst = nullptr;
    auto access = wb.PixelBuffer().as<IWxHostBufferByteAccess>();
    if ( FAILED(access->Buffer(&dst)) || !dst )
        return nullptr;

    for ( int i = 0; i < w * h; ++i )
    {
        const unsigned char r = rgb[i * 3 + 0];
        const unsigned char g = rgb[i * 3 + 1];
        const unsigned char b = rgb[i * 3 + 2];
        const unsigned char a = alpha ? alpha[i] : 255;

        // WriteableBitmap expects premultiplied BGRA.
        dst[i * 4 + 0] = static_cast<uint8_t>(b * a / 255);
        dst[i * 4 + 1] = static_cast<uint8_t>(g * a / 255);
        dst[i * 4 + 2] = static_cast<uint8_t>(r * a / 255);
        dst[i * 4 + 3] = a;
    }

    wb.Invalidate();
    return wb;
}

bool wxWinUISetToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& tip,
    wxWindow *owner,
    winrt::Windows::Foundation::IInspectable *appliedValue,
    int maximumWidth)
{
    if ( appliedValue )
        *appliedValue = nullptr;

    if ( !element )
        return true;

#if wxUSE_TOOLTIPS
    if ( gs_winuiToolTipPolicyShutdown )
        return true;

    MUXC::ToolTip pendingToolTip{ nullptr };
    unsigned long long pendingRevision = 0;
    wxWinUIManagedToolTip previousManaged;
    bool hadPreviousManaged = false;

    try
    {
        wxWinUIPurgeDeadManagedToolTips();
        auto managed = wxWinUIFindManagedToolTip(element);

        if ( tip.empty() )
        {
            const MUXC::ToolTip expected =
                managed != gs_winuiManagedToolTips.end()
                    ? managed->toolTip
                    : nullptr;
            const unsigned long long expectedRevision =
                managed != gs_winuiManagedToolTips.end()
                    ? managed->revision
                    : 0;

            MUXC::ToolTipService::SetToolTip(element, nullptr);

            // The attached-property setter can synchronously enter application
            // code and mutate this vector. Never keep an iterator across it,
            // and never erase a replacement installed by that nested call.
            if ( expected )
            {
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, expected, expectedRevision);
            }
        }
        else
        {
            // Replacing the peer of an already-managed element is still the
            // same logical tooltip. Preserve its creation-time policy unless
            // the wxToolTip owner supplied an explicit snapshot.
            const int effectiveMaximumWidth =
                maximumWidth != -2
                    ? maximumWidth
                    : managed != gs_winuiManagedToolTips.end()
                          ? managed->maximumWidth
                          : gs_winuiToolTipMaxWidth;
            MUXC::ToolTip toolTip;
            toolTip.Content(winrt::box_value(wxWinUIToHString(tip)));
            wxWinUIApplyToolTipMaximum(
                toolTip, element, owner, effectiveMaximumWidth);
            toolTip.IsEnabled(gs_winuiToolTipsEnabled);
            const unsigned long long revision =
                wxWinUINextToolTipEntryRevision();
            pendingToolTip = toolTip;
            pendingRevision = revision;

            if ( managed == gs_winuiManagedToolTips.end() )
            {
                gs_winuiManagedToolTips.push_back(
                    { winrt::make_weak(element),
                      toolTip,
                      !gs_winuiToolTipsEnabled,
                      gs_winuiToolTipsEnabled
                          ? wxWinUIManagedToolTip::PolicyPhase::Attached
                          : wxWinUIManagedToolTip::PolicyPhase::Suppressed,
                      revision,
                      effectiveMaximumWidth });
            }
            else
            {
                previousManaged = *managed;
                hadPreviousManaged = true;
                managed->toolTip = toolTip;
                managed->suppressed = !gs_winuiToolTipsEnabled;
                managed->policyPhase =
                    gs_winuiToolTipsEnabled
                        ? wxWinUIManagedToolTip::PolicyPhase::Attached
                        : wxWinUIManagedToolTip::PolicyPhase::Suppressed;
                managed->revision = revision;
                managed->maximumWidth = effectiveMaximumWidth;
            }

            winrt::Windows::Foundation::IInspectable attached{ nullptr };
            if ( gs_winuiToolTipsEnabled )
                attached = toolTip;

            if ( wxWinUIConsumeToolTipFault(
                     gs_winuiToolTipSetBeforeCommitFaults) )
            {
                winrt::throw_hresult(E_FAIL);
            }
            MUXC::ToolTipService::SetToolTip(element, attached);
            if ( wxWinUIConsumeToolTipFault(
                     gs_winuiToolTipSetAfterCommitFaults) )
            {
                winrt::throw_hresult(E_FAIL);
            }

            if ( !wxWinUIManagedToolTipIsCurrent(element, toolTip, revision) )
                return true;

            const auto finalActual =
                MUXC::ToolTipService::GetToolTip(element);
            const auto finalToolTip = finalActual.try_as<MUXC::ToolTip>();
            const bool stillOwned = gs_winuiToolTipsEnabled
                                        ? finalToolTip == toolTip
                                        : !finalActual;
            if ( !stillOwned )
            {
                // A synchronous application callback replaced our value.
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, toolTip, revision);
                return true;
            }

            if ( appliedValue )
                *appliedValue = toolTip;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( pendingToolTip &&
             wxWinUIManagedToolTipIsCurrent(
                 element, pendingToolTip, pendingRevision) )
        {
            try
            {
                const auto current = wxWinUIFindManagedToolTip(element);
                const auto actual =
                    MUXC::ToolTipService::GetToolTip(element);
                const auto actualToolTip = actual.try_as<MUXC::ToolTip>();
                const bool mutationCommitted =
                    current != gs_winuiManagedToolTips.end() &&
                    (wxWinUIManagedToolTipOwnsNull(*current)
                         ? !actual
                         : actualToolTip == pendingToolTip);
                if ( mutationCommitted )
                {
                    // Some dependency properties can throw after publishing.
                    // Preserve the truthful applied state rather than rolling
                    // the registry back to an object no longer attached.
                    if ( appliedValue )
                        *appliedValue = pendingToolTip;
                    wxWinUILogException(
                        "WinUI tooltip (setter reported after commit)", e);
                    return true;
                }

                if ( hadPreviousManaged &&
                     (actualToolTip == previousManaged.toolTip ||
                      (wxWinUIManagedToolTipOwnsNull(previousManaged) &&
                       !actual)) )
                {
                    *current = previousManaged;
                }
                else
                {
                    wxWinUIEraseManagedToolTipIfCurrent(
                        element, pendingToolTip, pendingRevision);
                }
            }
            catch ( const winrt::hresult_error& )
            {
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, pendingToolTip, pendingRevision);
            }
        }
        wxWinUILogException("WinUI tooltip", e);
        return false;
    }
#else
    wxUnusedVar(tip);
    wxUnusedVar(owner);
    wxUnusedVar(appliedValue);
    wxUnusedVar(maximumWidth);
    return true;
#endif // wxUSE_TOOLTIPS
}

bool wxWinUIRestoreToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& originalLocalValue,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue)
{
    if ( !element )
        return true;

#if wxUSE_TOOLTIPS
    if ( gs_winuiToolTipPolicyShutdown )
        return true;

    try
    {
        wxWinUIPurgeDeadManagedToolTips();
        const auto managed = wxWinUIFindManagedToolTip(element);
        const auto expected = expectedWxValue.try_as<MUXC::ToolTip>();
        if ( managed == gs_winuiManagedToolTips.end() ||
             !expected ||
             managed->toolTip != expected )
        {
            return true;
        }

        const unsigned long long revision = managed->revision;
        const bool ownsNull = wxWinUIManagedToolTipOwnsNull(*managed);
        const auto actual = MUXC::ToolTipService::GetToolTip(element);
        const auto actualToolTip = actual.try_as<MUXC::ToolTip>();
        const auto property =
            MUXC::ToolTipService::ToolTipProperty();
        const auto actualLocal = element.ReadLocalValue(property);
        const auto actualLocalToolTip =
            actualLocal.try_as<MUXC::ToolTip>();

        // Restore only while the exact object installed by wx remains
        // authoritative as a LOCAL value. Merely seeing the same object as an
        // effective Style/resource value after application ClearValue() is
        // not ownership. During global suppression, effective null alone is
        // likewise insufficient: ClearValue() leaves the local value Unset
        // rather than wx's owned local null. Equal text is deliberately
        // irrelevant here.
        if ( actualLocalToolTip == expected ||
             (ownsNull && !actual && !actualLocal) )
        {
            if ( originalLocalValue ==
                    MUX::DependencyProperty::UnsetValue() )
            {
                element.ClearValue(property);
            }
            else
            {
                element.SetValue(property, originalLocalValue);
            }
        }

        wxWinUIEraseManagedToolTipIfCurrent(element, expected, revision);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tooltip restoration", e);
        return false;
    }
#else
    wxUnusedVar(originalLocalValue);
    wxUnusedVar(expectedWxValue);
    return true;
#endif
}

bool wxWinUIForgetManagedToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !element )
        return true;

#if wxUSE_TOOLTIPS
    wxWinUIPurgeDeadManagedToolTips();
    const auto managed = wxWinUIFindManagedToolTip(element);
    if ( managed == gs_winuiManagedToolTips.end() )
        return true;

    const winrt::Windows::Foundation::IInspectable expected =
        managed->toolTip;
    return wxWinUIRestoreToolTip(
        element, MUX::DependencyProperty::UnsetValue(), expected);
#else
    return true;
#endif
}

bool wxWinUIIsManagedToolTipSuppressed(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue)
{
#if wxUSE_TOOLTIPS
    if ( !element || !expectedWxValue )
        return false;

    wxWinUIPurgeDeadManagedToolTips();
    const auto expected = expectedWxValue.try_as<MUXC::ToolTip>();
    const auto managed = wxWinUIFindManagedToolTip(element);
    return expected &&
           managed != gs_winuiManagedToolTips.end() &&
           managed->toolTip == expected &&
           wxWinUIManagedToolTipOwnsNull(*managed);
#else
    wxUnusedVar(element);
    wxUnusedVar(expectedWxValue);
    return false;
#endif
}

bool wxWinUIIsManagedToolTipCurrent(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue)
{
#if wxUSE_TOOLTIPS
    if ( !element || !expectedWxValue )
        return false;

    wxWinUIPurgeDeadManagedToolTips();
    const auto expected = expectedWxValue.try_as<MUXC::ToolTip>();
    const auto managed = wxWinUIFindManagedToolTip(element);
    return expected &&
           managed != gs_winuiManagedToolTips.end() &&
           managed->toolTip == expected;
#else
    wxUnusedVar(element);
    wxUnusedVar(expectedWxValue);
    return false;
#endif
}

#if wxUSE_TOOLTIPS

unsigned wxWinUIGetManagedToolTipCountForTesting()
{
    wxWinUIPurgeDeadManagedToolTips();
    return static_cast<unsigned>(gs_winuiManagedToolTips.size());
}

void wxWinUISetToolTipFaultForTesting(unsigned faults, unsigned count)
{
    if ( faults & wxWinUIToolTipFault_SetBeforeCommit )
        gs_winuiToolTipSetBeforeCommitFaults = count;
    if ( faults & wxWinUIToolTipFault_SetAfterCommit )
        gs_winuiToolTipSetAfterCommitFaults = count;
    if ( faults & wxWinUIToolTipFault_Policy )
        gs_winuiToolTipPolicyFaults = count;

    if ( faults == wxWinUIToolTipFault_None )
    {
        gs_winuiToolTipSetBeforeCommitFaults = 0;
        gs_winuiToolTipSetAfterCommitFaults = 0;
        gs_winuiToolTipPolicyFaults = 0;
    }
}

void wxWinUIResetToolTipPolicyDiagnosticsForTesting()
{
    gs_winuiToolTipPolicyFaultAttempts = 0;
    gs_winuiToolTipPolicyReplaySchedules = 0;
    gs_winuiToolTipPolicyReplayExecutions = 0;
}

unsigned wxWinUIGetToolTipPolicyFaultAttemptCountForTesting()
{
    return gs_winuiToolTipPolicyFaultAttempts;
}

unsigned wxWinUIGetToolTipPolicyReplayScheduleCountForTesting()
{
    return gs_winuiToolTipPolicyReplaySchedules;
}

unsigned wxWinUIGetToolTipPolicyReplayExecutionCountForTesting()
{
    return gs_winuiToolTipPolicyReplayExecutions;
}

unsigned wxWinUIGetToolTipPolicyQuarantinedEntryCountForTesting()
{
    wxWinUIPurgeDeadManagedToolTips();
    return static_cast<unsigned>(std::count_if(
        gs_winuiManagedToolTips.begin(),
        gs_winuiManagedToolTips.end(),
        [](const wxWinUIManagedToolTip& managed)
        {
            return managed.policyQuarantined;
        }));
}

bool wxWinUIIsToolTipPolicyReplayScheduledForTesting()
{
    return gs_winuiToolTipPolicyReplayScheduled;
}

bool wxWinUIIsToolTipPolicyShutdownForTesting()
{
    return gs_winuiToolTipPolicyShutdown;
}

namespace
{

enum class wxWinUIToolTipPolicyEntryResult
{
    Complete,
    Interrupted,
    Retry
};

bool wxWinUIToolTipPolicyRequestIsCurrent(
    bool enabled,
    unsigned long long generation)
{
    return gs_winuiToolTipsEnabled == enabled &&
           gs_winuiToolTipPolicyGeneration == generation;
}

void wxWinUIFinishManagedToolTipPolicy(
    const MUX::UIElement& element,
    const MUXC::ToolTip& toolTip,
    unsigned long long revision,
    bool suppressed)
{
    const auto current = wxWinUIFindManagedToolTip(element);
    if ( current == gs_winuiManagedToolTips.end() ||
         current->toolTip != toolTip ||
         current->revision != revision )
    {
        return;
    }

    current->suppressed = suppressed;
    current->policyPhase =
        suppressed
            ? wxWinUIManagedToolTip::PolicyPhase::Suppressed
            : wxWinUIManagedToolTip::PolicyPhase::Attached;
    current->policyFailureCount = 0;
    current->policyQuarantined = false;
}

wxWinUIToolTipPolicyEntryResult wxWinUIReconcileManagedToolTipPolicy(
    const wxWinUIManagedToolTip& candidate,
    bool enabled,
    unsigned long long generation)
{
    using Phase = wxWinUIManagedToolTip::PolicyPhase;

    const auto element = candidate.element.get();
    if ( !element || !candidate.toolTip )
        return wxWinUIToolTipPolicyEntryResult::Complete;

    const auto transactionIsCurrent = [&]()
    {
        return wxWinUIToolTipPolicyRequestIsCurrent(enabled, generation) &&
               wxWinUIManagedToolTipIsCurrent(
                   element, candidate.toolTip, candidate.revision);
    };

    const auto interruptedOrRetired = [&]()
    {
        return wxWinUIToolTipPolicyRequestIsCurrent(enabled, generation)
                   ? wxWinUIToolTipPolicyEntryResult::Complete
                   : wxWinUIToolTipPolicyEntryResult::Interrupted;
    };

    try
    {
        if ( !transactionIsCurrent() )
            return interruptedOrRetired();

        {
            const auto current = wxWinUIFindManagedToolTip(element);
            if ( current == gs_winuiManagedToolTips.end() ||
                 current->toolTip != candidate.toolTip ||
                 current->revision != candidate.revision )
            {
                return interruptedOrRetired();
            }

            if ( current->policyQuarantined )
                return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        if ( wxWinUIConsumeToolTipFault(gs_winuiToolTipPolicyFaults) )
        {
            ++gs_winuiToolTipPolicyFaultAttempts;
            winrt::throw_hresult(E_FAIL);
        }

        auto actual = MUXC::ToolTipService::GetToolTip(element);
        auto actualToolTip = actual.try_as<MUXC::ToolTip>();
        if ( actual && actualToolTip != candidate.toolTip )
        {
            // A direct XAML replacement, including an object with identical
            // text, owns the property now.
            wxWinUIEraseManagedToolTipIfCurrent(
                element, candidate.toolTip, candidate.revision);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        if ( enabled )
        {
            if ( actualToolTip == candidate.toolTip )
            {
                if ( !candidate.toolTip.IsEnabled() )
                {
                    if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                             element,
                             candidate.toolTip,
                             candidate.revision,
                             Phase::RestoringEnable) )
                    {
                        return wxWinUIToolTipPolicyEntryResult::Complete;
                    }

                    candidate.toolTip.IsEnabled(true);
                    if ( !transactionIsCurrent() )
                        return interruptedOrRetired();

                    actual = MUXC::ToolTipService::GetToolTip(element);
                    actualToolTip = actual.try_as<MUXC::ToolTip>();
                    if ( actualToolTip != candidate.toolTip )
                    {
                        // We started with the object attached, so a callback
                        // clearing or replacing it owns the new value.
                        wxWinUIEraseManagedToolTipIfCurrent(
                            element,
                            candidate.toolTip,
                            candidate.revision);
                        return wxWinUIToolTipPolicyEntryResult::Complete;
                    }
                }

                wxWinUIFinishManagedToolTipPolicy(
                    element, candidate.toolTip, candidate.revision, false);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            // Null is ours only after the detach boundary has begun. A null
            // observed while an Attached entry is otherwise stable is a
            // direct application clear and must not be resurrected.
            const auto current = wxWinUIFindManagedToolTip(element);
            if ( current == gs_winuiManagedToolTips.end() ||
                 current->toolTip != candidate.toolTip ||
                 current->revision != candidate.revision )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }
            if ( !wxWinUIManagedToolTipOwnsNull(*current) )
            {
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, candidate.toolTip, candidate.revision);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                     element,
                     candidate.toolTip,
                     candidate.revision,
                     Phase::RestoringEnable) )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            if ( !candidate.toolTip.IsEnabled() )
                candidate.toolTip.IsEnabled(true);
            if ( !transactionIsCurrent() )
                return interruptedOrRetired();

            actual = MUXC::ToolTipService::GetToolTip(element);
            actualToolTip = actual.try_as<MUXC::ToolTip>();
            if ( actual )
            {
                if ( actualToolTip != candidate.toolTip )
                {
                    wxWinUIEraseManagedToolTipIfCurrent(
                        element,
                        candidate.toolTip,
                        candidate.revision);
                    return wxWinUIToolTipPolicyEntryResult::Complete;
                }

                wxWinUIFinishManagedToolTipPolicy(
                    element, candidate.toolTip, candidate.revision, false);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                     element,
                     candidate.toolTip,
                     candidate.revision,
                     Phase::RestoringAttach) )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            MUXC::ToolTipService::SetToolTip(element, candidate.toolTip);
            if ( !transactionIsCurrent() )
                return interruptedOrRetired();

            actual = MUXC::ToolTipService::GetToolTip(element);
            actualToolTip = actual.try_as<MUXC::ToolTip>();
            if ( actualToolTip != candidate.toolTip )
            {
                // The setter completed, so a callback's null or replacement
                // is authoritative. An opposite policy request would have
                // changed the generation and returned above instead.
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, candidate.toolTip, candidate.revision);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            wxWinUIFinishManagedToolTipPolicy(
                element, candidate.toolTip, candidate.revision, false);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        // Disable path.
        if ( actualToolTip == candidate.toolTip )
        {
            if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                     element,
                     candidate.toolTip,
                     candidate.revision,
                     Phase::SuppressingClose) )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            candidate.toolTip.IsOpen(false);
            if ( !transactionIsCurrent() )
                return interruptedOrRetired();

            actual = MUXC::ToolTipService::GetToolTip(element);
            actualToolTip = actual.try_as<MUXC::ToolTip>();
            if ( actualToolTip != candidate.toolTip )
            {
                // No policy detach has started yet. Null is therefore a
                // direct application mutation just like another object.
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, candidate.toolTip, candidate.revision);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                     element,
                     candidate.toolTip,
                     candidate.revision,
                     Phase::SuppressingDisable) )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            candidate.toolTip.IsEnabled(false);
            if ( !transactionIsCurrent() )
                return interruptedOrRetired();

            actual = MUXC::ToolTipService::GetToolTip(element);
            actualToolTip = actual.try_as<MUXC::ToolTip>();
            if ( actualToolTip != candidate.toolTip )
            {
                // We still haven't detached it ourselves.
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, candidate.toolTip, candidate.revision);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                     element,
                     candidate.toolTip,
                     candidate.revision,
                     Phase::SuppressingDetach) )
            {
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            MUXC::ToolTipService::SetToolTip(element, nullptr);
            if ( !transactionIsCurrent() )
                return interruptedOrRetired();

            actual = MUXC::ToolTipService::GetToolTip(element);
            if ( actual )
            {
                // A callback published a replacement after our detach.
                wxWinUIEraseManagedToolTipIfCurrent(
                    element, candidate.toolTip, candidate.revision);
                return wxWinUIToolTipPolicyEntryResult::Complete;
            }

            wxWinUIFinishManagedToolTipPolicy(
                element, candidate.toolTip, candidate.revision, true);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        const auto current = wxWinUIFindManagedToolTip(element);
        if ( current == gs_winuiManagedToolTips.end() ||
             current->toolTip != candidate.toolTip ||
             current->revision != candidate.revision )
        {
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }
        if ( !wxWinUIManagedToolTipOwnsNull(*current) )
        {
            wxWinUIEraseManagedToolTipIfCurrent(
                element, candidate.toolTip, candidate.revision);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        // A newer disable request can encounter an entry halfway through a
        // restore, with the candidate still detached but already re-enabled.
        // Finish the suppression from that truthful partial state.
        if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                 element,
                 candidate.toolTip,
                 candidate.revision,
                 Phase::SuppressingClose) )
        {
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        candidate.toolTip.IsOpen(false);
        if ( !transactionIsCurrent() )
            return interruptedOrRetired();

        if ( !wxWinUISetManagedToolTipPhaseIfCurrent(
                 element,
                 candidate.toolTip,
                 candidate.revision,
                 Phase::SuppressingDisable) )
        {
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        if ( candidate.toolTip.IsEnabled() )
            candidate.toolTip.IsEnabled(false);
        if ( !transactionIsCurrent() )
            return interruptedOrRetired();

        actual = MUXC::ToolTipService::GetToolTip(element);
        if ( actual )
        {
            // Detached policy state was replaced while callbacks ran.
            wxWinUIEraseManagedToolTipIfCurrent(
                element, candidate.toolTip, candidate.revision);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        wxWinUIFinishManagedToolTipPolicy(
            element, candidate.toolTip, candidate.revision, true);
        return wxWinUIToolTipPolicyEntryResult::Complete;
    }
    catch ( const winrt::hresult_error& e )
    {
        // Leave the exact phase in place. The bounded driver can retry from
        // the observable attached value without mistaking a committed null
        // for an application clear.
        wxWinUILogException("WinUI global tooltip policy", e);

        const auto current = wxWinUIFindManagedToolTip(element);
        if ( current == gs_winuiManagedToolTips.end() ||
             current->toolTip != candidate.toolTip ||
             current->revision != candidate.revision )
        {
            return interruptedOrRetired();
        }

        if ( current->policyFailureCount <
             wxWinUIToolTipMaxPersistentFailures )
        {
            ++current->policyFailureCount;
        }

        if ( current->policyFailureCount >=
             wxWinUIToolTipMaxPersistentFailures )
        {
            current->policyQuarantined = true;
            wxLogWarning(
                "wxWinUI: quarantining a tooltip policy entry after %u "
                "consecutive failures",
                current->policyFailureCount);
            return wxWinUIToolTipPolicyEntryResult::Complete;
        }

        return wxWinUIToolTipPolicyEntryResult::Retry;
    }
}

void wxWinUIResetToolTipPolicyEntryRetryBudget()
{
    gs_winuiToolTipHostSyncFailureCount = 0;
    gs_winuiToolTipHostSyncQuarantined = false;
    for ( auto& managed : gs_winuiManagedToolTips )
    {
        managed.policyFailureCount = 0;
        managed.policyQuarantined = false;
    }
}

void wxWinUIResetToolTipPolicyRetryBudget()
{
    gs_winuiToolTipDeferredReplayCount = 0;
    gs_winuiToolTipPolicyTransactionQuarantined = false;
    wxWinUIResetToolTipPolicyEntryRetryBudget();
}

void wxWinUICancelToolTipPolicyReplay()
{
    if ( ++gs_winuiToolTipReplayToken == 0 )
        ++gs_winuiToolTipReplayToken;
    gs_winuiToolTipPolicyReplayScheduled = false;
}

class wxWinUIToolTipReplayCallbackGuard final
{
public:
    wxWinUIToolTipReplayCallbackGuard()
    {
        gs_winuiToolTipPolicyReplayCallbackActive = true;
    }

    ~wxWinUIToolTipReplayCallbackGuard()
    {
        gs_winuiToolTipPolicyReplayCallbackActive = false;
    }
};

void wxWinUIScheduleToolTipPolicyReplay()
{
    if ( gs_winuiToolTipPolicyReplayScheduled ||
         gs_winuiToolTipPolicyShutdown ||
         gs_winuiToolTipPolicyTransactionQuarantined ||
         !wxTheApp )
    {
        return;
    }

    if ( gs_winuiToolTipDeferredReplayCount >=
         wxWinUIToolTipMaxDeferredReplays )
    {
        gs_winuiToolTipPolicyTransactionQuarantined = true;
        wxLogWarning(
            "wxWinUI: quarantining a global tooltip policy transaction "
            "after %u deferred replays",
            gs_winuiToolTipDeferredReplayCount);
        return;
    }

    ++gs_winuiToolTipDeferredReplayCount;
    ++gs_winuiToolTipPolicyReplaySchedules;
    gs_winuiToolTipPolicyReplayScheduled = true;
    if ( ++gs_winuiToolTipReplayToken == 0 )
        ++gs_winuiToolTipReplayToken;
    const unsigned long long replayToken = gs_winuiToolTipReplayToken;
    wxTheApp->CallAfter(
        [replayToken]()
        {
            if ( replayToken != gs_winuiToolTipReplayToken ||
                 gs_winuiToolTipPolicyShutdown )
            {
                return;
            }

            gs_winuiToolTipPolicyReplayScheduled = false;
            ++gs_winuiToolTipPolicyReplayExecutions;
            wxWinUIToolTipReplayCallbackGuard callbackGuard;
            const bool requested = gs_winuiToolTipsEnabled;
            (void)wxWinUISetToolTipsEnabled(requested);
        });
}

} // namespace

bool wxWinUISetToolTipsEnabled(bool enabled)
{
    const bool policyChanged = gs_winuiToolTipsEnabled != enabled;
    if ( gs_winuiToolTipsEnabled != enabled )
    {
        gs_winuiToolTipsEnabled = enabled;
        if ( ++gs_winuiToolTipPolicyGeneration == 0 )
            ++gs_winuiToolTipPolicyGeneration;

        // An opposite request nested in a dependency-property callback starts
        // a new entry phase, but it is still part of the same bounded driver:
        // do not replenish its deferred-replay budget on every alternation.
        if ( gs_winuiToolTipPolicyActive ||
             gs_winuiToolTipPolicyReplayCallbackActive )
        {
            wxWinUIResetToolTipPolicyEntryRetryBudget();
        }
        else
        {
            wxWinUIResetToolTipPolicyRetryBudget();
        }
    }

    const unsigned long long callerGeneration =
        gs_winuiToolTipPolicyGeneration;

    // During runtime teardown the wx-side process policy remains meaningful
    // for the native TTM control, but no XAML object may be touched or queued.
    if ( gs_winuiToolTipPolicyShutdown )
        return true;

    // A public, top-level same-value call is an explicit retry request. It
    // supersedes any queued replay and gets a fresh bounded budget. Nested
    // calls and the replay callback itself must preserve the current budget.
    if ( !gs_winuiToolTipPolicyActive &&
         !gs_winuiToolTipPolicyReplayCallbackActive )
    {
        wxWinUICancelToolTipPolicyReplay();
        if ( !policyChanged )
            wxWinUIResetToolTipPolicyRetryBudget();
    }

    gs_winuiToolTipPolicyReplayRequested = true;

    // Dependency-property callbacks can call wxToolTip::Enable() again.
    // Nested calls publish only the newest request and return to their caller;
    // the one outer driver resumes from the recorded entry phase. This avoids
    // recursive walks while still allowing the nested wxToolTip::Enable() to
    // publish its matching native TTM state.
    if ( gs_winuiToolTipPolicyActive )
    {
        return wxWinUIToolTipPolicyRequestIsCurrent(
            enabled, callerGeneration);
    }

    gs_winuiToolTipPolicyActive = true;
    bool converged = false;
    constexpr unsigned MaxImmediatePasses = 8;

    for ( unsigned pass = 0; pass < MaxImmediatePasses; ++pass )
    {
        if ( gs_winuiToolTipPolicyShutdown )
            break;

        gs_winuiToolTipPolicyReplayRequested = false;
        const bool requested = gs_winuiToolTipsEnabled;
        const unsigned long long generation =
            gs_winuiToolTipPolicyGeneration;
        bool retry = false;

        wxWinUIPurgeDeadManagedToolTips();
        const auto snapshot = gs_winuiManagedToolTips;
        for ( const auto& candidate : snapshot )
        {
            if ( !wxWinUIToolTipPolicyRequestIsCurrent(
                     requested, generation) )
            {
                break;
            }

            const wxWinUIToolTipPolicyEntryResult result =
                wxWinUIReconcileManagedToolTipPolicy(
                    candidate, requested, generation);
            if ( result == wxWinUIToolTipPolicyEntryResult::Interrupted )
                break;
            if ( result == wxWinUIToolTipPolicyEntryResult::Retry )
                retry = true;
        }

        if ( wxWinUIToolTipPolicyRequestIsCurrent(requested, generation) &&
             !gs_winuiToolTipHostSyncQuarantined )
        {
            try
            {
                // This also suppresses/restores application-authored XAML
                // tooltips which are not in the helper registry, through the
                // slot ownership adapter.
                wxWinUITopLevelHost::SynchronizeAllHosts();
                gs_winuiToolTipHostSyncFailureCount = 0;
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI global tooltip host synchronization", e);
                if ( gs_winuiToolTipHostSyncFailureCount <
                     wxWinUIToolTipMaxPersistentFailures )
                {
                    ++gs_winuiToolTipHostSyncFailureCount;
                }

                if ( gs_winuiToolTipHostSyncFailureCount >=
                     wxWinUIToolTipMaxPersistentFailures )
                {
                    gs_winuiToolTipHostSyncQuarantined = true;
                    wxLogWarning(
                        "wxWinUI: quarantining global tooltip host "
                        "synchronization after %u consecutive failures",
                        gs_winuiToolTipHostSyncFailureCount);
                }
                else
                {
                    retry = true;
                }
            }
        }

        if ( wxWinUIToolTipPolicyRequestIsCurrent(requested, generation) &&
             !gs_winuiToolTipPolicyReplayRequested &&
             !retry )
        {
            converged = true;
            break;
        }
    }

    gs_winuiToolTipPolicyActive = false;
    if ( !converged )
    {
        // A callback storm must never grow the C++ stack or monopolize the UI
        // thread. The latest request (and its native TTM state) is already
        // published; continue XAML convergence in one coalesced later turn.
        wxWinUIScheduleToolTipPolicyReplay();
    }
    else
    {
        gs_winuiToolTipDeferredReplayCount = 0;
        gs_winuiToolTipPolicyTransactionQuarantined = false;
    }

    // False means that this caller was superseded, not that the newest policy
    // failed. wxToolTip::Enable() uses it to prevent a stale outer frame from
    // overwriting the native state already published by a nested last writer.
    return wxWinUIToolTipPolicyRequestIsCurrent(
        enabled, callerGeneration);
}

bool wxWinUIAreToolTipsEnabled()
{
    return gs_winuiToolTipsEnabled;
}

void wxWinUISetToolTipMaxWidth(int width)
{
    // wxToolTip explicitly specifies that this setting affects only tooltips
    // created after the call. Existing XAML ToolTip instances are therefore
    // intentionally left untouched.
    gs_winuiToolTipMaxWidth = width;
}

int wxWinUIGetToolTipMaxWidth()
{
    return gs_winuiToolTipMaxWidth;
}

void wxWinUIInitializeToolTipPolicy()
{
    wxWinUICancelToolTipPolicyReplay();
    gs_winuiToolTipPolicyShutdown = false;
    gs_winuiToolTipPolicyReplayRequested = false;
    gs_winuiToolTipPolicyActive = false;
    wxWinUIResetToolTipPolicyRetryBudget();
}

void wxWinUIShutdownToolTipPolicy()
{
    // Invalidate a callback already present in wxApp's pending-event queue.
    // The lambda may still be invoked, but its token/shutdown checks make it a
    // callback-free no-op which cannot touch XAML or schedule another turn.
    gs_winuiToolTipPolicyShutdown = true;
    if ( ++gs_winuiToolTipPolicyGeneration == 0 )
        ++gs_winuiToolTipPolicyGeneration;
    wxWinUICancelToolTipPolicyReplay();
    gs_winuiToolTipPolicyReplayRequested = false;
    gs_winuiToolTipPolicyTransactionQuarantined = true;
    gs_winuiToolTipHostSyncQuarantined = true;

    gs_winuiToolTipSetBeforeCommitFaults = 0;
    gs_winuiToolTipSetAfterCommitFaults = 0;
    gs_winuiToolTipPolicyFaults = 0;

    // These strong C++/WinRT references must be released while the XAML
    // manager and apartment are still alive.
    gs_winuiManagedToolTips.clear();
}

#endif // wxUSE_TOOLTIPS

winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme()
{
    return gs_winuiElementTheme;
}

bool wxWinUIIsDarkTheme()
{
    return wxWinUIGetShellThemePolicy().dark;
}

wxWinUIControlHost::~wxWinUIControlHost()
{
    Close();
}

bool wxWinUIControlHost::Initialize(wxWindow *window)
{
    if ( m_window )
        return true;

    if ( !window || wxWinUIIsPhysicalDisconnectPublicationPoisoned() ||
         !wxWinUI3Initialize() )
        return false;

    // Find or create the shared island of this window's top-level parent;
    // the slot itself is only registered by SetContent().
    if ( !wxWinUITopLevelHost::ForWindow(window, true) )
        return false;

    m_window = window;
    return true;
}

bool wxWinUIControlHost::IsOk() const
{
    return m_window &&
           (wxWinUITopLevelHost::FindSlotOwner(m_window) != nullptr ||
            wxWinUITopLevelHost::ForWindow(m_window, false) != nullptr);
}

HWND wxWinUIControlHost::GetHostHWND() const
{
    return m_window ? static_cast<HWND>(m_window->GetHWND()) : nullptr;
}

HWND wxWinUIControlHost::GetBridgeHWND() const
{
    wxWinUITopLevelHost *host =
        m_window ? wxWinUITopLevelHost::FindSlotOwner(m_window) : nullptr;
    if ( !host && m_window )
        host = wxWinUITopLevelHost::ForWindow(m_window, false);
    return host ? host->GetBridgeHwnd() : nullptr;
}

namespace
{
// Live callback-state count across the control hosts. A state whose WinRT
// removal failed remains counted until the element finally releases its
// inactive callback; this is more truthful than decrementing on an attempted
// removal and proves that no raw host pointer survives.
unsigned gs_liveLoadedHooks = 0;
std::atomic<bool> gs_physicalDisconnectPublicationPoisoned{false};
} // anonymous namespace

void wxWinUINotePhysicalDisconnectGateDegraded() noexcept
{
    gs_physicalDisconnectPublicationPoisoned.store(
        true, std::memory_order_release);
}

bool wxWinUIIsPhysicalDisconnectPublicationPoisoned() noexcept
{
    return gs_physicalDisconnectPublicationPoisoned.load(
        std::memory_order_acquire);
}

void wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting() noexcept
{
    gs_physicalDisconnectPublicationPoisoned.store(
        false, std::memory_order_release);
}

void wxWinUIPoisonPhysicalDisconnectPublicationForTesting() noexcept
{
    wxWinUINotePhysicalDisconnectGateDegraded();
}

class wxWinUILoadedState
{
public:
    wxWinUILoadedState() { ++gs_liveLoadedHooks; }
    ~wxWinUILoadedState() { --gs_liveLoadedHooks; }

    wxWinUIControlHost *host = nullptr;
    bool fired = false;
    bool active = false;
};

unsigned wxWinUITopLevelHost::GetLiveLoadedHookCount()
{
    return gs_liveLoadedHooks;
}

void wxWinUIControlHost::RevokeLoadedHook()
{
    // Invalidate first. If WinRT refuses removal, its late callback still
    // owns a state object but can no longer dereference this host.
    if ( m_loadedState )
    {
        m_loadedState->active = false;
        m_loadedState->host = nullptr;
    }

    if ( m_loadedToken.value && m_content )
    {
        try
        {
            if ( auto framework = m_content
                     .try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
            {
                framework.Loaded(m_loadedToken);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("FrameworkElement::Loaded removal", e);
        }
    }

    m_loadedToken = {};
    m_loadedState.reset();
}

void wxWinUIControlHost::SetPhysicalDisconnectGate(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    m_physicalDisconnectGate = gate;
    wxWinUITopLevelHost * const host =
        m_window ? wxWinUITopLevelHost::FindSlotOwner(m_window) : nullptr;
    if ( host )
        host->SetSlotPhysicalDisconnectGate(m_window, gate);
}

void wxWinUIControlHost::Close(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate)
{
    // Drop a test hook before any revocation can drain a callback. Production
    // code never arms it, but its captured owner state must obey the same
    // lifetime rule as the real Loaded delegate.
    m_nextContentLoadedHookForTesting = {};

    if ( !m_window )
        return;

    RevokeLoadedHook();

    // The TLW host may already be gone (it dies with its top-level window,
    // taking every slot with it), hence the tolerant lookup.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
    {
        host->UnregisterSlot(
            m_window, gate ? gate : m_physicalDisconnectGate);
    }

    m_content = nullptr;
    m_physicalDisconnectGate.reset();
    m_window = nullptr;
}

bool wxWinUIControlHost::SetContent(
    const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    return SetContent(element, element);
}

bool wxWinUIControlHost::SetContent(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget)
{
    if ( !m_window || wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ReconcileSlotOwner(m_window, true);
    if ( !host )
        return false;

    // Caller-level transaction, matching the slot's: the old content, its
    // Loaded hook and its loaded state all survive until the slot swap
    // actually succeeded -- on failure the slot keeps (or restores) the old
    // element, and this model must not diverge from it.

    // The natural size of a XAML element is only known once its control
    // template has been applied, which happens when it is loaded into a live
    // visual tree -- i.e. some time after this call.  Until then Measure()
    // under-reports (typically it returns the bare glyph of a check box,
    // without its label), so the control would be laid out far too small and
    // its content clipped away.  Re-query the best size when the content is
    // actually loaded and let the layout catch up.  The NEW hook is
    // registered before the swap (so no Loaded can be missed) but only into
    // a local token: it is revoked again if the swap fails.
    winrt::event_token newLoadedToken{};
    std::shared_ptr<wxWinUILoadedState> newLoadedState;
    if ( auto framework =
             element.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
    {
        try
        {
            newLoadedState = std::make_shared<wxWinUILoadedState>();
            newLoadedToken = framework.Loaded(
                [newLoadedState](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                {
                    newLoadedState->fired = true;
                    if ( newLoadedState->active &&
                         newLoadedState->host )
                    {
                        newLoadedState->host->OnContentLoaded();
                    }
                });
            if ( !newLoadedToken.value )
                newLoadedState.reset();
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( newLoadedState )
            {
                newLoadedState->active = false;
                newLoadedState->host = nullptr;
            }
            wxWinUILogException("FrameworkElement::Loaded registration", e);
            return false;
        }
    }

    if ( !host->RegisterSlot(
             m_window,
             element,
             semanticTarget,
             m_physicalDisconnectGate) )
    {
        // Failed OR superseded swap. In the ordinary failure case the slot
        // still shows the old content; in the reentrant case a nested call
        // already published a newer content/model/hook. Drop only this
        // operation's candidate hook and leave whichever model is currently
        // authoritative exactly as it is.
        if ( newLoadedState )
        {
            newLoadedState->active = false;
            newLoadedState->host = nullptr;
        }
        if ( newLoadedToken.value )
        {
            try
            {
                if ( auto framework = element
                         .try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
                {
                    framework.Loaded(newLoadedToken);
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "candidate FrameworkElement::Loaded removal", e);
            }
        }
        return false;
    }

    // Success: retire the old hook, commit the new model, then arm the new
    // callback. Loaded may have fired synchronously during RegisterSlot();
    // the state latched it while unarmed so the notification is not lost.
    RevokeLoadedHook();
    m_loadedToken = newLoadedToken;
    m_loadedState = newLoadedState;
    m_content = element;
    m_contentLoaded = false;
    if ( m_loadedState )
    {
        m_loadedState->host = this;
        m_loadedState->active = true;
        if ( m_loadedState->fired )
            OnContentLoaded();
    }
    return true;
}

void wxWinUIControlHost::ApplyBackdropMaterial()
{
    // Nothing to do: the per-island Mica backdrop is gone, the DWM window
    // backdrop shows through the shared island's transparent pixels.
}

void wxWinUIControlHost::ClearContent()
{
    if ( !m_window )
        return;

    // The detach is the operation that can fail: try it first, and change
    // nothing at all if it does (the old element stays in the carrier, and
    // this model must keep describing it).
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
    {
        if ( wxWinUISlot * const slot = host->FindSlot(m_window) )
        {
            if ( !slot->SetContent(nullptr) )
                return;
        }
    }

    // Detach succeeded: invalidate before attempting removal so the old
    // element may safely outlive this proxy even if WinRT rejects removal.
    RevokeLoadedHook();

    m_content = nullptr;
    m_contentLoaded = false;
}

// ----------------------------------------------------------------------------
// wxWinUIControlHostProbe -- test seam for the ClearContent path
// ----------------------------------------------------------------------------

namespace
{
winrt::Microsoft::UI::Xaml::UIElement
wxWinUIMakeProbeButton(const wxString& label)
{
    winrt::Microsoft::UI::Xaml::Controls::Button button;
    button.Content(winrt::box_value(wxWinUIToHString(label)));
    return button;
}
} // anonymous namespace

wxWinUIControlHostProbe *
wxWinUIControlHostProbe::CreateEmpty(wxWindow *window)
{
    if ( !window )
        return nullptr;

    auto host = new wxWinUIControlHost;
    if ( !host->Initialize(window) )
    {
        delete host;
        return nullptr;
    }

    auto probe = new wxWinUIControlHostProbe;
    probe->m_host = host;
    return probe;
}

wxWinUIControlHostProbe *wxWinUIControlHostProbe::Create(wxWindow *window)
{
    wxWinUIControlHostProbe * const probe = CreateEmpty(window);
    if ( !probe )
        return nullptr;

    if ( !probe->SetContent() )
    {
        delete probe;
        return nullptr;
    }

    return probe;
}

wxWinUIControlHostProbe::~wxWinUIControlHostProbe()
{
    delete m_host;      // wxWinUIControlHost::~ closes itself
}

bool wxWinUIControlHostProbe::SetContent(const wxString& label)
{
    // Keep the pointee local: a deterministic slot-attachment hook is allowed
    // to delete the wxWinUIControlHost while its SetContent() is on the stack.
    // The call result is returned without dereferencing that pointee again.
    wxWinUIControlHost * const host = m_host;
    return host && host->SetContent(wxWinUIMakeProbeButton(label));
}

winrt::Microsoft::UI::Xaml::UIElement
wxWinUIControlHostProbe::GetContentForTesting() const
{
    return m_host ? m_host->GetContentForTesting() : nullptr;
}

void wxWinUIControlHostProbe::DestroyControlHostForTesting()
{
    wxWinUIControlHost * const host = m_host;
    m_host = nullptr;
    delete host;
}

void wxWinUIControlHostProbe::ClearContent()
{
    if ( m_host )
        m_host->ClearContent();
}

void wxWinUIControlHostProbe::SetBridgeClipRect(
    const wxRect& physicalRect)
{
    if ( m_host )
        m_host->SetBridgeClipRect(physicalRect);
}

void wxWinUIControlHostProbe::Close()
{
    if ( m_host )
        m_host->Close();
}

void wxWinUIControlHost::ApplyWxCursor(const wxCursor& cursor)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
        host->MarkDirty(m_window);
    wxUnusedVar(cursor);
}

bool wxWinUIWindowUsesBackdrop(const wxWindow *win)
{
    if ( !win )
        return false;

    // The mark is put on the top-level window when the backdrop is applied
    // and inherited downwards by the first background erase of each child,
    // so a window created later may not carry it yet: walk up, exactly like
    // that erase does, and stop at the first non-child window (::GetParent()
    // returns the OWNER of an owned top-level window, whose backdrop is not
    // this window's).
    for ( HWND hwnd = reinterpret_cast<HWND>(win->GetHandle()); hwnd; )
    {
        if ( ::GetPropW(hwnd, wxWinUIBackdropTransparentProp) )
            return true;
        if ( !(::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD) )
            break;
        hwnd = ::GetParent(hwnd);
    }

    return false;
}

void wxWinUISetWindowCursor(wxWindow *win, const wxCursor& cursor)
{
    if ( !win )
        return;

    // The coalesced state pass is authoritative. Applying the direct cursor
    // here without updating its inherited-value cache creates a race when an
    // explicit child cursor is set and cleared before the pending flush.
    // The window can also be a native parent of slots, including a slot that
    // truthfully remains in an old host after a failed migration, so
    // conservatively invalidate every live host.
    wxWinUITopLevelHost::NotifyCursorPolicyChanged();
    wxUnusedVar(cursor);
}

void wxWinUIControlHost::ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if ( !m_content )
        return;

    if ( auto element =
             m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
    {
        try
        {
            element.RequestedTheme(theme);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("FrameworkElement::RequestedTheme", e);
        }
    }
}

void wxWinUIControlHost::SetBridgeHeightLimit(int physicalHeight)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
    {
        // The legacy control-host seam receives wx physical pixels (the
        // notebook obtains them with FromDIP()). Store the invariant value in
        // the shared host instead: after a cross-monitor/TLW migration the
        // next sync converts these DIPs using the destination XamlRoot scale.
        double scale = m_window->GetDPIScaleFactor();
        if ( !std::isfinite(scale) || scale <= 0.0 )
            scale = 1.0;
        const double heightDIP =
            physicalHeight > 0
                ? physicalHeight / scale
                : physicalHeight;
        host->SetSlotClipHeight(m_window, heightDIP);
    }
}

void wxWinUIControlHost::SetBridgeClipRect(const wxRect& physicalRect)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( !host )
        return;

    // Preserve fractional DIPs until SyncSlot rounds back to physical pixels.
    // Integer ToDIP() would turn (for example) 1 px at 150% into 1 DIP and
    // expand it to 2 px on the way back. Both axes remain physical-LTR: the
    // slot's FlowDirection never mirrors its native composition/input area.
    double scale = m_window->GetDPIScaleFactor();
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;
    const wxWinUIClipRectDIP rectDIP =
        wxWinUITopLevelHost::PhysicalClipRectToDIP(
            physicalRect, scale);
    host->SetSlotClipRect(
        m_window,
        rectDIP.x,
        rectDIP.y,
        rectDIP.width,
        rectDIP.height);
}

void wxWinUIControlHost::ClearBridgeClipRect()
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
        host->ClearSlotClip(m_window);
}

wxSize wxWinUIControlHost::MeasureContent() const
{
    if ( !m_content || !m_window )
        return wxDefaultSize;

    auto element =
        m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if ( !element )
        return wxDefaultSize;

    try
    {
        // The element itself is no longer size-pinned (its slot container
        // is), so an unconstrained Measure() reports the natural size
        // directly.
        const float inf = std::numeric_limits<float>::infinity();
        element.Measure({ inf, inf });
        const auto desired = element.DesiredSize();

        if ( desired.Width > 0 && desired.Height > 0 )
        {
            // DesiredSize is expressed in DIPs.
            return m_window->FromDIP(
                wxSize(static_cast<int>(std::ceil(desired.Width)),
                       static_cast<int>(std::ceil(desired.Height))));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("wxWinUIControlHost::MeasureContent", e);
    }

    return wxDefaultSize;
}

namespace
{

// Pending relayout pass, coalesced across every control whose content loads
// in the same burst (at startup they all load together: one pass, not one
// per control).
bool gs_winuiRelayoutPending = false;
std::vector<wxWeakRef<wxWindow>> gs_winuiRelayoutTops;

} // anonymous namespace

bool wxWinUIRefitDialogGrowOnly(wxWindow *top)
{
    wxDialog *dialog = wxDynamicCast(top, wxDialog);
    if ( !dialog || dialog->IsBeingDeleted() )
        return false;

    wxSizer * const sizer = dialog->GetSizer();
    if ( !sizer )
        return false;

    // Computing best sizes can call control code. Do not retain the dialog or
    // sizer as authoritative across that boundary.
    const wxWeakRef<wxWindow> weakDialog(dialog);
    const wxSize desired = sizer->ComputeFittingClientSize(dialog);

    dialog = wxDynamicCast(weakDialog.get(), wxDialog);
    if ( !dialog || dialog->IsBeingDeleted() ||
            dialog->GetSizer() != sizer )
    {
        return false;
    }

    const wxSize current = dialog->GetClientSize();
    const wxSize grown =
        wxWinUIComputeGrowOnlyClientSize(current, desired);

    // Never shrink: generic Font/Find dialogs establish their initial hints
    // before WinUI templates have produced stable DesiredSize values. Update
    // the stable minimum too, otherwise a later user resize can return the
    // dialog to the pre-template cropped size.
    const wxSize oldMin = dialog->GetMinClientSize();
    const wxSize grownMin = wxWinUIComputeGrowOnlyClientSize(oldMin, desired);
    if ( grownMin != oldMin )
        dialog->SetMinClientSize(grownMin);

    if ( grown == current )
        return false;

    dialog->SetClientSize(grown);
    return true;
}

namespace
{

unsigned gs_contentRelayouts = 0;

void wxWinUIScheduleContentRelayout(wxWindow *top)
{
    ++gs_contentRelayouts;
    for ( const auto& weak : gs_winuiRelayoutTops )
    {
        if ( weak.get() == top )
        {
            top = nullptr;
            break;
        }
    }
    if ( top )
        gs_winuiRelayoutTops.push_back(wxWeakRef<wxWindow>(top));

    if ( gs_winuiRelayoutPending )
        return;
    gs_winuiRelayoutPending = true;

    wxTheApp->CallAfter([]()
    {
        gs_winuiRelayoutPending = false;

        std::vector<wxWeakRef<wxWindow>> tops;
        tops.swap(gs_winuiRelayoutTops);

        for ( const auto& weak : tops )
        {
            wxWindow *top = weak.get();
            if ( !top )
                continue;

            if ( top->GetSizer() )
                top->Layout();
            top = weak.get();
            if ( !top )
                continue;

            // A single top-level Layout() is NOT enough: when it hands an
            // intermediate panel the very size it already had, that panel's
            // own sizer never reruns and keeps the geometry computed with
            // the pre-loading best sizes (cropped labels in static boxes...)
            // until something resizes the window.  Relayout DEEP, parents
            // first, and let every scrolling window recompute its virtual
            // size too -- otherwise the bottom of a page that grew during
            // loading stays out of reach.
            std::function<void (const wxWeakRef<wxWindow>&)> walk =
                [&](const wxWeakRef<wxWindow>& weakParent)
            {
                wxWindow * const parent = weakParent.get();
                if ( !parent )
                    return;

                // Every Layout()/SendSizeEvent() below is application-code
                // reentrancy: it may destroy, reparent or reorder siblings.
                // Snapshot weak children first and revalidate both ends of
                // the edge after each callback; never keep a wxWindowList
                // iterator or raw child across one of those boundaries.
                std::vector<wxWeakRef<wxWindow>> children;
                children.reserve(parent->GetChildren().size());
                for ( wxWindow *child : parent->GetChildren() )
                    children.emplace_back(child);

                for ( const auto& weakChild : children )
                {
                    wxWindow *child = weakChild.get();
                    wxWindow *liveParent = weakParent.get();
                    if ( !child || !liveParent ||
                         child->GetParent() != liveParent )
                    {
                        continue;
                    }

                    if ( child->GetSizer() )
                        child->Layout();

                    child = weakChild.get();
                    liveParent = weakParent.get();
                    if ( !child || !liveParent ||
                         child->GetParent() != liveParent )
                    {
                        continue;
                    }

                    const HWND hwnd = static_cast<HWND>(child->GetHWND());
                    if ( hwnd )
                    {
                        const LONG_PTR style =
                            ::GetWindowLongPtr(hwnd, GWL_STYLE);
                        if ( style & (WS_VSCROLL | WS_HSCROLL) )
                            child->SendSizeEvent();
                    }

                    child = weakChild.get();
                    liveParent = weakParent.get();
                    if ( child && liveParent &&
                         child->GetParent() == liveParent )
                    {
                        walk(weakChild);
                    }
                }
            };
            walk(weak);

            top = weak.get();
            if ( top && wxWinUIRefitDialogGrowOnly(top) )
            {
                // SetClientSize() normally delivers wxEVT_SIZE immediately,
                // but explicitly laying out once makes the postcondition
                // independent of native delivery timing. Any Loaded burst
                // scheduled during this call becomes a later generation;
                // the monotonic size rule makes that pass idempotent.
                top = weak.get();
                if ( top && top->GetSizer() )
                    top->Layout();
            }
        }
    });
}

} // anonymous namespace

unsigned wxWinUIGetContentRelayoutCount()
{
    return gs_contentRelayouts;
}

void wxWinUIScheduleDialogRefitAfterShow(wxWindow *top)
{
    wxDialog * const dialog = wxDynamicCast(top, wxDialog);
    if ( !dialog || dialog->IsBeingDeleted() )
        return;

    // Reuse the exact Loaded-path transaction rather than maintaining a
    // second dialog-only queue. This gives Show() and Loaded one coalesced
    // pass when they happen in the same turn, while a dialog whose content
    // loaded earlier gets one new, bounded post-show pass.
    wxWinUIScheduleContentRelayout(dialog);
}

void wxWinUIControlHost::OnContentLoaded()
{
    if ( !m_window )
        return;

    // Keep this callback local before any operation below can destroy the
    // hosted wx control and, with it, this proxy. It is deliberately invoked
    // after the real Loaded work and nothing may dereference `this` after the
    // move.
    std::function<void()> testingHook =
        std::move(m_nextContentLoadedHookForTesting);
    wxWindow * const window = m_window;

    // Relayout is needed only for the first load of this content generation.
    // Loaded itself may repeat after an unload/reload, and every occurrence
    // remains relevant to a pending focus request below.
    if ( !m_contentLoaded )
    {
        m_contentLoaded = true;

        // Clearing the cached best size is safe from inside the XAML callback;
        // the actual relayout is deferred (and coalesced globally).
        //
        // Only a size that really changed is worth one, though. Loaded fires
        // again whenever an element re-enters the visual tree, which the
        // coalesced slot pass does routinely, and relaying out the whole
        // window resizes its children, which reports geometry mutations,
        // which schedules the next pass: a loop that measured four full
        // window layouts per second for as long as the mouse kept moving,
        // and left a CAD canvas painting once every five seconds.
        const wxSize sizeBeforeLoad = window->GetEffectiveMinSize();
        window->InvalidateBestSize();
        if ( window->GetEffectiveMinSize() != sizeBeforeLoad ||
             wxWinUIOptimisationDisabled("relayout") )
        {
            wxWindow * const top = wxGetTopLevelParent(window);
            if ( top )
                wxWinUIScheduleContentRelayout(top);
            else if ( wxWindow * const parent = window->GetParent() )
                wxWinUIScheduleContentRelayout(parent);
        }
    }

    // The retained focus request can synchronously deliver application focus
    // handlers which destroy `window`, this control host and even the TLW.
    // Only the self-contained test callback local may run afterwards.
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::FindSlotOwner(window) )
    {
        host->NotifySlotContentLoaded(window);
    }

    // Final operation. The hook itself is allowed to destroy this host.
    if ( testingHook )
        testingHook();
}

void wxWinUIControlHost::DispatchPendingContentLoadedHookForTesting()
{
    if ( m_window && m_nextContentLoadedHookForTesting )
        OnContentLoaded();
}

void wxWinUIControlHost::ForceRender()
{
    // Property changes invalidate the shared XAML tree by themselves; a slot
    // layout refresh is kept as belt and braces for the historical call
    // sites.
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    wxWinUISlot * const slot = host ? host->FindSlot(m_window) : nullptr;
    if ( !slot )
        return;

    // Keep the XAML object strong across the flush. FlushSync() can run
    // arbitrary application callbacks and destroy this wx control (and this
    // wxWinUIControlHost), so nothing below may dereference `this`.
    const wxWeakRef<wxWindow> alive(m_window);
    const auto container = slot->GetContainer();
    try
    {
        // A newly replaced content tree can otherwise be measured against
        // the slot's old zero-sized geometry until a later composition tick.
        // ForceRender is explicitly synchronous: publish current wx geometry
        // first, then run XAML measure/arrange on that exact container.
        host->FlushSync();
        wxWindow * const liveWindow = alive.get();
        wxWinUITopLevelHost * const currentHost =
            liveWindow
                ? wxWinUITopLevelHost::FindSlotOwner(liveWindow)
                : nullptr;
        wxWinUISlot * const current =
            currentHost ? currentHost->FindSlot(liveWindow) : nullptr;
        if ( !current || current->GetContainer() != container )
            return;
        container.InvalidateMeasure();
        container.InvalidateArrange();
        container.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("wxWinUIControlHost::ForceRender", e);
    }
}

bool wxWinUIControlHost::ContainsFocus(HWND hwnd) const
{
    if ( !hwnd || !m_window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( !host || host->GetFocusOwner() != m_window )
        return false;

    const HWND bridge = host->GetBridgeHwnd();
    return hwnd == bridge || (bridge && ::IsChild(bridge, hwnd));
}

bool wxWinUIControlHost::NavigateFocus(bool WXUNUSED(forward))
{
    if ( !m_window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( !host || !host->FindSlot(m_window) )
        return false;

    host->FocusSlot(m_window);
    return true;
}

void wxWinUIControlHost::SetPreferredFocus(
    const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !m_window )
        return;

    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::FindSlotOwner(m_window) )
    {
        host->SetSlotPreferredFocus(m_window, element);
    }
}

void wxWinUIControlHost::SetAutomationNameOverride(const wxString& name)
{
    if ( !m_window )
        return;

    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::FindSlotOwner(m_window) )
    {
        host->SetSlotAutomationNameOverride(m_window, name);
    }
}

void wxWinUIControlHost::SynchronizeForFocus()
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(m_window);
    if ( host )
    {
        // FlushSync() is an arbitrary reentrancy boundary and may destroy the
        // hosted wxWindow and this proxy. It is deliberately the final
        // operation in this method.
        host->FlushSync();
    }
}

HWND wxWinUIFindNextKeyboardFocusable(HWND hwndTLW, HWND hwndCurrent,
                                      bool previous)
{
    if ( !hwndTLW || !hwndCurrent )
        return nullptr;

    // GetNextDlgTabItem() walks every WS_TABSTOP child, including HWNDs
    // privately created by Windows App SDK (for example the top-level
    // InputNonClientPointerSource helper). wxGetWindowFromHWND() is not a
    // suitable ownership test here: it deliberately walks parent HWNDs, so
    // any such foreign direct child resolves to the wx top-level window and
    // can steal focus from the current slot.
    //
    // Anchor compound-control lookup in the logical wx tree instead. Exact wx
    // HWNDs are accepted directly; implementation subwindows are accepted
    // only when a wx control explicitly claims them through ContainsHWND().
    // Everything else is outside the wx tab order and must be walked past.
    wxWindow * const tlw = wxFindWinFromHandle(hwndTLW);
    if ( !tlw || static_cast<HWND>(tlw->GetHWND()) != hwndTLW )
        return nullptr;

    const wxWeakRef<wxWindow> tlwWeak(tlw);
    const WXHWND tlwShell = tlw->GetHWND();
    const unsigned long long tlwGeneration =
        wxWinUIMSWGetHwndGeneration(tlw, tlwShell);
    if ( !tlwGeneration )
        return nullptr;

    // Bound the walk by the FIRST candidate seen, not only by coming back
    // to hwndCurrent: when hwndCurrent itself is not part of the native tab
    // enumeration, an all-refused enumeration would otherwise cycle among
    // its members forever.
    HWND first = nullptr;
    HWND next = hwndCurrent;
    for ( ;; )
    {
        next = ::GetNextDlgTabItem(hwndTLW, next, previous);
        if ( !next || next == hwndCurrent || next == first )
            return nullptr;
        if ( !first )
            first = next;

        wxWindow *win = wxFindWinFromHandle(next);
        if ( !win )
        {
            wxWindow * const liveTLW = tlwWeak.get();
            if ( !liveTLW || liveTLW->GetHWND() != tlwShell ||
                 wxWinUIMSWGetHwndGeneration(liveTLW, tlwShell) !=
                     tlwGeneration )
            {
                return nullptr;
            }

            win = liveTLW->FindItemByHWND(
                reinterpret_cast<WXHWND>(next));
        }
        if ( !win )
            continue;

        // The virtual focus predicate may synchronously destroy/reparent the
        // candidate/TLW or replace either HWND association. Never return or
        // iterate from identities captured before that callback.
        const wxWeakRef<wxWindow> weak(win);
        const WXHWND shell = win->GetHWND();
        const unsigned long long shellGeneration =
            wxWinUIMSWGetHwndGeneration(win, shell);
        const unsigned long long candidateGeneration =
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(next));
        const bool accepts = win->CanAcceptFocusFromKeyboard();
        win = weak.get();
        wxWindow * const liveTLW = tlwWeak.get();
        if ( !win || !liveTLW ||
             liveTLW->GetHWND() != tlwShell ||
             wxWinUIMSWGetHwndGeneration(liveTLW, tlwShell) !=
                 tlwGeneration ||
             !shellGeneration || !candidateGeneration ||
             wxWinUIMSWGetHwndGeneration(win, shell) != shellGeneration ||
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(next)) != candidateGeneration )
        {
            return nullptr;
        }
        if ( accepts )
            return next;
    }
}

bool wxWinUI3ProcessTabNavigationWithModifiers(WXMSG *msg,
                                                bool shiftDown,
                                                bool controlDown,
                                                bool altDown)
{
    if ( !msg || msg->message != WM_KEYDOWN || msg->wParam != VK_TAB )
        return false;
    if ( controlDown || altDown )
        return false;

    // The destination of the removed message is authoritative. Looking at
    // GetFocus() here can steal Tab from a native popup/message loop merely
    // because focus still happens to be recorded inside an island.
    HWND target = msg->hwnd;
    if ( !target )
        return false;

    // Resolve the logical current control: with the focus inside an island,
    // it is the slot owner recorded by the arbiter.
    wxWindow *current =
        wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)target);
    const bool focusInIsland = current != nullptr;
    if ( !current )
    {
        // Only an exact wx HWND may enter the mixed-navigation domain.
        // wxGetWindowFromHWND() deliberately walks parents for native helper
        // controls, but doing that here lets any foreign WS_CHILD of a frame
        // steal Tab. Native compound controls keep their normal wx/MSW dialog
        // preprocessing when their implementation child owns the message.
        current = wxFindWinFromHandle(target);
    }
    if ( !current )
        return false;

    if ( !wxWinUI3AllowMessageProcessing(current) )
        return false;

    // Controls asking for character messages own Tab too (e.g. an editable
    // wxRichTextCtrl inserts it). Let their normal wx key pipeline decide.
    if ( current->HasFlag(wxWANTS_CHARS) )
        return false;

#if wxUSE_TEXTCTRL
    // wxTE_PROCESS_TAB has the same contract for a XAML-backed wxTextCtrl:
    // the editable peer owns Tab and inserts it. This test must live in the
    // mixed navigation arbiter because it runs before ContentPreTranslateMessage
    // can deliver KeyDown to the TextBox/RichEditBox. A read-only text control
    // still uses ordinary dialog traversal, matching native wxMSW.
    if ( wxTextCtrl * const text = wxDynamicCast(current, wxTextCtrl) )
    {
        if ( text->HasFlag(wxTE_PROCESS_TAB) && text->IsEditable() )
            return false;
    }
#endif // wxUSE_TEXTCTRL

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(current, false);
    if ( !host )
        return false;   // no island in this TLW: default wx processing

    const HWND hwndCurrent = static_cast<HWND>(current->GetHWND());
    const HWND hwndTLW = static_cast<HWND>(host->GetTLW()->GetHWND());
    if ( !hwndCurrent || !hwndTLW )
        return false;

    const HWND next =
        wxWinUIFindNextKeyboardFocusable(hwndTLW, hwndCurrent, shiftDown);
    if ( !next )
        return false;

    wxWindow * const nextWin = wxGetWindowFromHWND((WXHWND)next);
    if ( nextWin && host->FindSlot(nextWin) )
    {
        host->FocusSlot(nextWin);
        return true;
    }

    // Only take over the Tab when leaving an island: between two plain wx
    // windows the standard wx navigation must keep running.
    if ( focusInIsland )
    {
        ::SetFocus(next);
        return true;
    }

    return false;
}

bool wxWinUI3ProcessTabNavigation(WXMSG *msg)
{
    return wxWinUI3ProcessTabNavigationWithModifiers(
        msg,
        (::GetKeyState(VK_SHIFT) & 0x8000) != 0,
        (::GetKeyState(VK_CONTROL) & 0x8000) != 0,
        (::GetKeyState(VK_MENU) & 0x8000) != 0);
}

bool wxWinUI3DispatchIslandKeyboard(WXMSG *msg)
{
    if ( !msg )
        return false;

    switch ( msg->message )
    {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
        case WM_UNICHAR:
            break;
        default:
            return false;
    }

    // Only take over messages actually addressed to an island HWND. Testing
    // GetFocus() alone would steal keyboard messages for native popup/menu
    // windows while the logical focus still happened to be inside an island.
    if ( !wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)msg->hwnd) )
        return false;

    // The shared wx-accelerator -> XAML pipeline has already had its one
    // chance to consume the message (from the hook, or the event-loop
    // fallback). Since it did not, dispatch it straight to the island exactly
    // like the canonical XAML-island message loop does. Crucially, arbitrary
    // messages never enter wxWidgets' broad dialog preprocessing.
    ::TranslateMessage(reinterpret_cast<MSG *>(msg));
    ::DispatchMessage(reinterpret_cast<MSG *>(msg));
    return true;
}

static void wxWinUIApplyWindowBackdropPass(wxWindow *tlw)
{
    if ( !tlw || wxPendingDelete.Member(tlw) )
        return;

    std::vector<wxWinUIShellWindowIdentity> identities;
    wxWinUICaptureShellWindowTree(tlw, identities);
    if ( identities.empty() )
        return;

    const wxWinUIShellWindowIdentity root = identities.front();
    if ( !wxWinUIShellIdentityIsCurrent(root) )
        return;

    const unsigned long long epoch =
        wxWinUIPublishShellApplyEpoch(root);
    if ( !epoch )
        return;
    const wxWinUIShellApplyEpochGuard epochGuard{ root, epoch };

    const wxWinUIShellThemePolicy policy = wxWinUIGetShellThemePolicy();
    const wxWinUIShellThemeNativeOps& operations =
        wxWinUIGetShellThemeOps();
    if ( !wxWinUIShellThemeOpsAreComplete(operations) )
        return;

    // Match the title bar (and other non-client areas) to the resolved policy.
    // High Contrast never inherits an explicit application Dark preference.
    operations.setImmersiveDarkMode(
        operations.context, root.hwnd, policy.dark);
    if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
        return;

    const auto publishSolidAndDisableMaterial =
        [&]()
        {
            // Marker/background cleanup is the prepare phase. DWM and the
            // extended frame are the commit phase and must remain untouched if
            // even one transparent marker cannot be retired.
            const wxWinUIShellPresentationResult presentation =
                wxWinUIApplyMicaBackground(
                    identities, root, epoch, policy, false);
            if ( presentation ==
                 wxWinUIShellPresentationResult::Superseded )
            {
                return;
            }

            if ( presentation ==
                 wxWinUIShellPresentationResult::MarkerRejected )
            {
                wxLogWarning(
                    "wxWinUI: failed to clear one or more native backdrop "
                    "markers; DWM/frame rollback was skipped");
                if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                    return;

                wxWindow * const currentTLW = root.window.get();
                if ( currentTLW )
                    currentTLW->Refresh();
                return;
            }

            operations.setSystemBackdrop(
                operations.context, root.hwnd, false);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return;
            operations.extendFrameIntoClientArea(
                operations.context, root.hwnd, false);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return;

            wxWinUIInvalidateBackdropPrime(root.hwnd);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return;

            wxWindow * const currentTLW = root.window.get();
            if ( currentTLW )
                currentTLW->Refresh();
        };

    if ( !policy.allowBackdrop )
    {
        // This ordering is mandatory for a hot High Contrast transition.
        publishSolidAndDisableMaterial();
        return;
    }

    // Let DWM provide the same Mica backdrop used by normal Win32 apps.  The
    // client background is made transparent below so this is visible behind
    // wx panels without requiring a WinUI MicaController/DispatcherQueue.
    //
    // OWNED top-level windows (dialogs) get it too: an earlier conclusion
    // that DWM never composes the backdrop on them was wrong (re-verified
    // on the spike's modal dialog: with the marking + per-window priming
    // below the material composes fine, and the mismatched black control
    // patches disappear).
    //
    // High Contrast is a hard policy boundary, not a best-effort DWM hint:
    // explicitly request DWMSBT_NONE and retract the extended client frame.
    const bool nativeBackdropEnabled =
        operations.setSystemBackdrop(
            operations.context, root.hwnd, true);
    if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
        return;
    if ( !nativeBackdropEnabled )
    {
        // An enable failure may occur on a window carrying material from the
        // preceding policy. Retire its markers before forcing the fallback.
        publishSolidAndDisableMaterial();
        return;
    }

    const bool frameConfigured =
        operations.extendFrameIntoClientArea(
            operations.context, root.hwnd, true);
    if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
        return;
    if ( !frameConfigured )
    {
        // As above, an old transparent marker may still exist. Never roll DWM
        // back until the complete tree has accepted the solid presentation.
        publishSolidAndDisableMaterial();
        return;
    }

    bool materialEnabled = true;

    const auto rollBackMaterial =
        [&]()
        {
            operations.setSystemBackdrop(
                operations.context, root.hwnd, false);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return false;
            operations.extendFrameIntoClientArea(
                operations.context, root.hwnd, false);
            if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
                return false;
            wxWinUIInvalidateBackdropPrime(root.hwnd);
            return wxWinUIShellApplyEpochIsCurrent(root, epoch);
        };

    wxWinUIShellPresentationResult presentation =
        wxWinUIApplyMicaBackground(
            identities, root, epoch, policy, materialEnabled);
    if ( presentation == wxWinUIShellPresentationResult::Superseded )
        return;

    if ( presentation ==
             wxWinUIShellPresentationResult::MarkerRejected &&
         materialEnabled )
    {
        // A single missing transparent marker makes the material tree
        // inconsistent. First retract every possibly-written marker (including
        // the one whose SetProp result was ambiguous) and publish the solid
        // surface. Disable DWM only if this cleanup completed: retaining the
        // material is safer than creating a transparent client with no
        // backdrop when clearing a marker itself was rejected.
        presentation = wxWinUIApplyMicaBackground(
            identities, root, epoch, policy, false);
        if ( presentation ==
                wxWinUIShellPresentationResult::Superseded )
        {
            return;
        }
        if ( presentation == wxWinUIShellPresentationResult::Applied )
        {
            if ( !rollBackMaterial() )
                return;
            materialEnabled = false;
        }
    }

    if ( presentation ==
         wxWinUIShellPresentationResult::MarkerRejected )
    {
        wxLogWarning(
            "wxWinUI: failed to clear one or more native backdrop "
            "markers after a partial update; DWM material remains enabled");
    }

    if ( !wxWinUIShellApplyEpochIsCurrent(root, epoch) )
        return;

    wxWindow * const currentTLW = root.window.get();
    if ( !currentTLW )
        return;

    currentTLW->Refresh();
    if ( materialEnabled &&
         wxWinUIShellApplyEpochIsCurrent(root, epoch) )
    {
        wxWinUIPrimeBackdrop(root.hwnd);
    }
}

bool wxWinUIEraseIslandBackground(HWND island, HDC hdc)
{
    if ( !island || !hdc )
        return false;

    RECT rc;
    if ( !::GetClientRect(island, &rc) ||
         rc.right <= rc.left || rc.bottom <= rc.top )
    {
        return false;
    }

    HWND const tlwHwnd = ::GetAncestor(island, GA_ROOT);

    // Under a DWM material the wx windows fill black and let Windows
    // substitute the material for it: the island must do exactly the same or
    // its (transparent) content shows the uninitialized redirection surface.
    if ( tlwHwnd && ::GetPropW(tlwHwnd, wxWinUIBackdropTransparentProp) )
    {
        ::FillRect(hdc, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
        return true;
    }

    wxWindow * const tlw =
        tlwHwnd ? wxFindWinFromHandle((WXHWND)tlwHwnd) : nullptr;
    wxColour colour = tlw ? tlw->GetBackgroundColour() : wxColour();
    if ( !colour.IsOk() )
        colour = wxWinUIBackgroundColour(wxWinUIGetShellThemePolicy());

    const HBRUSH brush = ::CreateSolidBrush(
        RGB(colour.Red(), colour.Green(), colour.Blue()));
    if ( !brush )
        return false;

    ::FillRect(hdc, &rc, brush);
    ::DeleteObject(brush);
    return true;
}

void wxWinUIApplyWindowBackdrop(wxWindow *tlw)
{
    wxCHECK_RET(wxIsMainThread(),
                "WinUI shell appearance must be applied on the UI thread");

    // Applying the policy to a window is also when a system theme change
    // reaches us, so this is the natural place to keep wxSystemSettings in
    // sync with what is about to be shown.
    wxWinUISyncClassicAppearance();
    wxWinUIEnsureCNumericLocale();

    if ( !tlw || wxPendingDelete.Member(tlw) )
        return;

    wxWinUIShellWindowIdentity identity;
    identity.window = tlw;
    identity.pointer = tlw;
    identity.hwnd = tlw->GetHWND();
    identity.hwndGeneration =
        wxWinUIMSWGetHwndGeneration(tlw, identity.hwnd);
    if ( !wxWinUIShellIdentityIsCurrent(identity) )
        return;

    const HWND hwnd = reinterpret_cast<HWND>(identity.hwnd);
    std::shared_ptr<wxWinUIShellApplyDriver> driver;
    const auto existing = gs_winuiShellApplyDrivers.find(hwnd);
    if ( existing != gs_winuiShellApplyDrivers.end() &&
         existing->second &&
         existing->second->pointer == identity.pointer &&
         existing->second->hwndGeneration ==
             identity.hwndGeneration &&
         existing->second->window.get() == identity.pointer )
    {
        driver = existing->second;
    }
    else
    {
        driver = std::make_shared<wxWinUIShellApplyDriver>();
        driver->window = tlw;
        driver->pointer = tlw;
        driver->hwnd = identity.hwnd;
        driver->hwndGeneration = identity.hwndGeneration;
        gs_winuiShellApplyDrivers[hwnd] = driver;
    }

    if ( driver->inProgress )
    {
        // A native/theme/background setter can synchronously request a newer
        // policy while the old native call has not yet returned. Let that
        // outer call finish, then apply the current policy last.
        driver->rerunRequested = true;
        return;
    }

    driver->inProgress = true;
    constexpr unsigned MaxSynchronousPasses = 8;
    unsigned pass = 0;
    for ( ; pass < MaxSynchronousPasses; ++pass )
    {
        driver->rerunRequested = false;
        wxWindow * const current = driver->window.get();
        if ( !current || current != driver->pointer ||
             current->IsBeingDeleted() ||
             wxPendingDelete.Member(current) ||
             current->GetHWND() != driver->hwnd ||
             wxWinUIMSWGetHwndGeneration(current, driver->hwnd) !=
                 driver->hwndGeneration )
        {
            break;
        }

        wxWinUIApplyWindowBackdropPass(current);
        if ( !driver->rerunRequested )
            break;
    }

    const bool budgetExhausted =
        pass == MaxSynchronousPasses &&
        driver->rerunRequested;
    driver->inProgress = false;

    const auto currentDriver = gs_winuiShellApplyDrivers.find(hwnd);
    if ( currentDriver != gs_winuiShellApplyDrivers.end() &&
         currentDriver->second == driver )
    {
        gs_winuiShellApplyDrivers.erase(currentDriver);
    }

    if ( budgetExhausted )
    {
        wxLogWarning(
            "wxWinUI: shell theme re-entrancy did not converge after "
            "eight passes; waiting for the next system/theme notification");
    }
}

namespace
{

constexpr wchar_t wxWINUI_BACKDROP_PRIMED_PROP[] =
    L"wxWinUIBackdropPrimed";

struct wxWinUIBackdropPrimeState
{
    unsigned long long generation = 0;
    unsigned long long epoch = 0;
    wxWinUITransientRect originalRect;
    wxWinUITransientRect expectedShrinkRect;
    unsigned int restoreAttempts = 0;
    bool restorePending = false;
};

struct wxWinUIBackdropPrimeCallback
{
    HWND hwnd = nullptr;
    unsigned long long generation = 0;
    unsigned long long epoch = 0;
};

using wxWinUIBackdropPrimeStates =
    std::map<HWND, wxWinUIBackdropPrimeState>;
wxWinUIBackdropPrimeStates gs_winuiBackdropPrimeStates;
std::map<HWND, unsigned long long> gs_winuiBackdropRetryGenerations;
unsigned long long gs_nextWinUIBackdropPrimeEpoch = 0;
wxWinUIBackdropNativeOps gs_winuiBackdropTestOps;
bool gs_hasWinUIBackdropTestOps = false;

unsigned long long
wxWinUIBackdropGetGenerationNative(void *, WXHWND hwnd)
{
    return wxWinUIMSWGetNativeHwndGeneration(hwnd);
}

bool wxWinUIBackdropIsWindowNative(void *, WXHWND hwnd)
{
    return ::IsWindow(reinterpret_cast<HWND>(hwnd)) != FALSE;
}

bool
wxWinUIBackdropGetRectNative(void *,
                             WXHWND hwndArg,
                             wxWinUITransientRect *rectOut)
{
    if ( !rectOut )
        return false;

    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    RECT rect;
    if ( !::IsWindow(hwnd) || !::GetWindowRect(hwnd, &rect) )
        return false;

    rectOut->left = rect.left;
    rectOut->top = rect.top;
    rectOut->right = rect.right;
    rectOut->bottom = rect.bottom;
    return ::IsWindow(hwnd) != FALSE;
}

bool
wxWinUIBackdropSetSizeNative(void *,
                             WXHWND hwndArg,
                             int width,
                             int height)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                       SWP_NOOWNERZORDER;
    return ::IsWindow(hwnd) &&
           ::SetWindowPos(
               hwnd, nullptr, 0, 0, width, height, flags) != FALSE;
}

bool wxWinUIBackdropSetMarkerNative(void *, WXHWND hwndArg)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const HANDLE marker = reinterpret_cast<HANDLE>(1);
    return ::IsWindow(hwnd) &&
           ::SetPropW(hwnd, wxWINUI_BACKDROP_PRIMED_PROP, marker) &&
           ::GetPropW(hwnd, wxWINUI_BACKDROP_PRIMED_PROP) == marker;
}

bool wxWinUIBackdropHasMarkerNative(void *, WXHWND hwndArg)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    return ::IsWindow(hwnd) &&
           ::GetPropW(hwnd, wxWINUI_BACKDROP_PRIMED_PROP) != nullptr;
}

bool wxWinUIBackdropClearMarkerNative(void *, WXHWND hwndArg)
{
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    if ( !::IsWindow(hwnd) )
        return false;

    ::RemovePropW(hwnd, wxWINUI_BACKDROP_PRIMED_PROP);
    return ::IsWindow(hwnd) &&
           !::GetPropW(hwnd, wxWINUI_BACKDROP_PRIMED_PROP);
}

bool
wxWinUIBackdropPostNative(void *,
                          void (*callback)(void *),
                          void (*destroy)(void *),
                          void *callbackData)
{
    if ( !wxTheApp || !callback || !destroy )
        return false;

    const std::shared_ptr<void> keepAlive(callbackData, destroy);
    wxTheApp->CallAfter(
        [callback, keepAlive]()
        {
            callback(keepAlive.get());
        });
    return true;
}

const wxWinUIBackdropNativeOps& wxWinUIGetBackdropOps()
{
    if ( gs_hasWinUIBackdropTestOps )
        return gs_winuiBackdropTestOps;

    static const wxWinUIBackdropNativeOps operations =
    {
        nullptr,
        wxWinUIBackdropGetGenerationNative,
        wxWinUIBackdropIsWindowNative,
        wxWinUIBackdropGetRectNative,
        wxWinUIBackdropSetSizeNative,
        wxWinUIBackdropSetMarkerNative,
        wxWinUIBackdropHasMarkerNative,
        wxWinUIBackdropClearMarkerNative,
        wxWinUIBackdropPostNative
    };
    return operations;
}

bool
wxWinUIBackdropOpsAreComplete(
    const wxWinUIBackdropNativeOps& operations)
{
    return operations.getGeneration && operations.isWindow &&
           operations.getRect && operations.setSize &&
           operations.setPrimeMarker && operations.hasPrimeMarker &&
           operations.clearPrimeMarker && operations.post;
}

bool
wxWinUIBackdropIdentityIsCurrent(
    const wxWinUIBackdropNativeOps& operations,
    HWND hwnd,
    unsigned long long generation)
{
    return generation != 0 &&
           operations.isWindow(operations.context, hwnd) &&
           operations.getGeneration(operations.context, hwnd) == generation;
}

bool
wxWinUIGetBackdropDimensions(const wxWinUITransientRect& rect,
                             int *width,
                             int *height)
{
    const long long width64 =
        static_cast<long long>(rect.right) - rect.left;
    const long long height64 =
        static_cast<long long>(rect.bottom) - rect.top;
    if ( width64 <= 1 || height64 <= 1 ||
         width64 > (std::numeric_limits<int>::max)() ||
         height64 > (std::numeric_limits<int>::max)() )
    {
        return false;
    }

    *width = static_cast<int>(width64);
    *height = static_cast<int>(height64);
    return true;
}

void wxWinUIBackdropPrimeCallbackProc(void *callbackData);

void wxWinUIBackdropPrimeCallbackDestroy(void *callbackData)
{
    delete static_cast<wxWinUIBackdropPrimeCallback *>(callbackData);
}

bool
wxWinUIScheduleBackdropPrimeCallback(
    const wxWinUIBackdropNativeOps& operations,
    HWND hwnd,
    const wxWinUIBackdropPrimeState& state)
{
    std::unique_ptr<wxWinUIBackdropPrimeCallback> callback(
        new wxWinUIBackdropPrimeCallback);
    callback->hwnd = hwnd;
    callback->generation = state.generation;
    callback->epoch = state.epoch;
    if ( !operations.post(
             operations.context,
             wxWinUIBackdropPrimeCallbackProc,
             wxWinUIBackdropPrimeCallbackDestroy,
             callback.get()) )
    {
        return false;
    }

    callback.release();
    return true;
}

bool
wxWinUIRestoreExactBackdropShrink(
    const wxWinUIBackdropNativeOps& operations,
    HWND hwnd,
    const wxWinUIBackdropPrimeState& state)
{
    if ( !wxWinUIBackdropIdentityIsCurrent(
             operations, hwnd, state.generation) )
    {
        return false;
    }

    wxWinUITransientRect current;
    if ( !operations.getRect(operations.context, hwnd, &current) ||
         current != state.expectedShrinkRect )
    {
        return false;
    }

    int width = 0;
    int height = 0;
    if ( !wxWinUIGetBackdropDimensions(
             state.originalRect, &width, &height) ||
         !operations.setSize(
             operations.context, hwnd, width, height) )
    {
        return false;
    }

    return wxWinUIBackdropIdentityIsCurrent(
               operations, hwnd, state.generation) &&
           operations.getRect(operations.context, hwnd, &current) &&
           current == state.originalRect;
}

bool
wxWinUIPreserveBackdropRestorePending(
    const wxWinUIBackdropNativeOps& operations,
    HWND hwnd,
    const wxWinUIBackdropPrimeState& state,
    bool clearMarker)
{
    wxWinUIBackdropPrimeStates::iterator current =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( current == gs_winuiBackdropPrimeStates.end() ||
         current->second.epoch != state.epoch ||
         !wxWinUIBackdropIdentityIsCurrent(
             operations, hwnd, state.generation) )
    {
        return false;
    }

    wxWinUITransientRect rect;
    if ( !operations.getRect(operations.context, hwnd, &rect) ||
         rect != state.expectedShrinkRect )
    {
        return false;
    }

    // No new callback is posted: the next explicit prime/invalidation/owner
    // transition first repairs the exact one-pixel shrink. Keeping this state
    // is essential; otherwise the shortened RECT becomes the next "original".
    current->second.restorePending = true;

    if ( clearMarker )
    {
        if ( !operations.clearPrimeMarker(operations.context, hwnd) )
            gs_winuiBackdropRetryGenerations[hwnd] = state.generation;
        else
            gs_winuiBackdropRetryGenerations.erase(hwnd);
    }

    current = gs_winuiBackdropPrimeStates.find(hwnd);
    return current != gs_winuiBackdropPrimeStates.end() &&
           current->second.epoch == state.epoch &&
           current->second.restorePending;
}

void
wxWinUIRetireBackdropPrime(HWND hwnd,
                           const wxWinUIBackdropPrimeState& state,
                           bool clearMarker)
{
    const wxWinUIBackdropNativeOps& operations = wxWinUIGetBackdropOps();
    const wxWinUIBackdropPrimeStates::const_iterator current =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( current == gs_winuiBackdropPrimeStates.end() ||
         current->second.epoch != state.epoch )
    {
        return;
    }

    if ( clearMarker &&
         wxWinUIBackdropIdentityIsCurrent(
             operations, hwnd, state.generation) )
    {
        if ( !operations.clearPrimeMarker(operations.context, hwnd) )
            gs_winuiBackdropRetryGenerations[hwnd] = state.generation;
    }

    // RemoveProp is not expected to dispatch, but the injected boundary is
    // deliberately allowed to model reentrancy. Never erase a replacement
    // epoch installed while clearing the old marker.
    const wxWinUIBackdropPrimeStates::const_iterator afterClear =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( afterClear != gs_winuiBackdropPrimeStates.end() &&
         afterClear->second.epoch == state.epoch )
    {
        gs_winuiBackdropPrimeStates.erase(afterClear);
    }
}

void wxWinUIBackdropPrimeCallbackProc(void *callbackData)
{
    const wxWinUIBackdropPrimeCallback * const callback =
        static_cast<wxWinUIBackdropPrimeCallback *>(callbackData);
    if ( !callback )
        return;

    const wxWinUIBackdropNativeOps& operations = wxWinUIGetBackdropOps();
    if ( !wxWinUIBackdropOpsAreComplete(operations) )
        return;

    wxWinUIBackdropPrimeStates::iterator currentState =
        gs_winuiBackdropPrimeStates.find(callback->hwnd);
    if ( currentState == gs_winuiBackdropPrimeStates.end() ||
         currentState->second.generation != callback->generation ||
         currentState->second.epoch != callback->epoch )
    {
        return;
    }

    wxWinUIBackdropPrimeState state = currentState->second;
    if ( !wxWinUIBackdropIdentityIsCurrent(
             operations, callback->hwnd, state.generation) )
    {
        // A recycled HWND is a different identity. Do not inspect or clear
        // properties on it.
        gs_winuiBackdropPrimeStates.erase(currentState);
        return;
    }

    wxWinUITransientRect currentRect;
    if ( !operations.getRect(
             operations.context, callback->hwnd, &currentRect) )
    {
        wxWinUIRetireBackdropPrime(callback->hwnd, state, true);
        return;
    }

    if ( currentRect != state.expectedShrinkRect )
    {
        int currentWidth = 0;
        int currentHeight = 0;
        int shrinkWidth = 0;
        int shrinkHeight = 0;
        const bool currentDimensionsValid =
            wxWinUIGetBackdropDimensions(
                currentRect, &currentWidth, &currentHeight);
        const bool shrinkDimensionsValid =
            wxWinUIGetBackdropDimensions(
                state.expectedShrinkRect,
                &shrinkWidth,
                &shrinkHeight);

        if ( currentDimensionsValid && shrinkDimensionsValid &&
             currentWidth == shrinkWidth &&
             currentHeight == shrinkHeight )
        {
            // The window only moved while the one-pixel shrink was pending.
            // Re-sample its exact RECT and defer once more; the next callback
            // restores the original dimensions without writing stale
            // coordinates.
            int originalWidth = 0;
            int originalHeight = 0;
            if ( !wxWinUIGetBackdropDimensions(
                     state.originalRect,
                     &originalWidth,
                     &originalHeight) )
            {
                wxWinUIRetireBackdropPrime(
                    callback->hwnd, state, true);
                return;
            }

            state.expectedShrinkRect = currentRect;
            state.originalRect = currentRect;
            state.originalRect.right =
                state.originalRect.left + originalWidth;
            state.originalRect.bottom =
                state.originalRect.top + originalHeight;
            if ( ++gs_nextWinUIBackdropPrimeEpoch == 0 )
                ++gs_nextWinUIBackdropPrimeEpoch;
            state.epoch = gs_nextWinUIBackdropPrimeEpoch;
            currentState->second = state;

            if ( !wxWinUIScheduleBackdropPrimeCallback(
                     operations, callback->hwnd, state) )
            {
                (void)wxWinUIRestoreExactBackdropShrink(
                    operations, callback->hwnd, state);
                wxWinUIRetireBackdropPrime(
                    callback->hwnd, state, true);
            }
            return;
        }

        // A real external resize superseded our pending dimensions. It is
        // itself the DWM resize required for priming, so keep the success
        // marker but never restore the stale snapshot.
        gs_winuiBackdropRetryGenerations.erase(callback->hwnd);
        gs_winuiBackdropPrimeStates.erase(currentState);
        return;
    }

    int width = 0;
    int height = 0;
    if ( !wxWinUIGetBackdropDimensions(
             state.originalRect, &width, &height) )
    {
        wxWinUIRetireBackdropPrime(callback->hwnd, state, true);
        return;
    }

    const bool restoreCallSucceeded =
        operations.setSize(
            operations.context, callback->hwnd, width, height);

    currentState = gs_winuiBackdropPrimeStates.find(callback->hwnd);
    if ( currentState == gs_winuiBackdropPrimeStates.end() ||
         currentState->second.epoch != state.epoch )
    {
        return;
    }

    wxWinUITransientRect restoredRect;
    const bool restoreVerified =
        restoreCallSucceeded &&
        wxWinUIBackdropIdentityIsCurrent(
            operations, callback->hwnd, state.generation) &&
        operations.getRect(
            operations.context, callback->hwnd, &restoredRect) &&
        restoredRect == state.originalRect;
    if ( restoreVerified )
    {
        gs_winuiBackdropRetryGenerations.erase(callback->hwnd);
        gs_winuiBackdropPrimeStates.erase(currentState);
        return;
    }

    // A transient USER32 failure gets one bounded deferred retry, but only
    // while the exact shrink is still ours. No callback loop is possible.
    wxWinUITransientRect afterFailure;
    if ( state.restoreAttempts == 0 &&
         wxWinUIBackdropIdentityIsCurrent(
             operations, callback->hwnd, state.generation) &&
         operations.getRect(
             operations.context, callback->hwnd, &afterFailure) &&
         afterFailure == state.expectedShrinkRect )
    {
        ++state.restoreAttempts;
        if ( ++gs_nextWinUIBackdropPrimeEpoch == 0 )
            ++gs_nextWinUIBackdropPrimeEpoch;
        state.epoch = gs_nextWinUIBackdropPrimeEpoch;
        currentState->second = state;
        if ( wxWinUIScheduleBackdropPrimeCallback(
                 operations, callback->hwnd, state) )
        {
            return;
        }
    }

    if ( wxWinUIPreserveBackdropRestorePending(
             operations, callback->hwnd, state, false) )
    {
        return;
    }

    wxWinUIRetireBackdropPrime(callback->hwnd, state, true);
}

} // anonymous namespace

void wxWinUIPrimeBackdrop(WXHWND hwndArg)
{
    const wxWinUIBackdropNativeOps& operations = wxWinUIGetBackdropOps();
    if ( !wxWinUIBackdropOpsAreComplete(operations) )
        return;

    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const unsigned long long generation =
        operations.getGeneration(operations.context, hwndArg);
    if ( !hwnd || !generation ||
         !operations.isWindow(operations.context, hwndArg) )
    {
        return;
    }

    wxWinUIBackdropPrimeStates::iterator existing =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( existing != gs_winuiBackdropPrimeStates.end() )
    {
        if ( existing->second.generation == generation )
        {
            if ( !existing->second.restorePending )
                return; // one exact in-flight epoch coalesces every request

            const wxWinUIBackdropPrimeState pending = existing->second;
            // Publish restoration-in-progress before SetWindowPos, which can
            // re-enter this function. Nested requests then coalesce.
            existing->second.restorePending = false;
            const bool restored =
                wxWinUIRestoreExactBackdropShrink(
                    operations, hwnd, pending);

            existing = gs_winuiBackdropPrimeStates.find(hwnd);
            if ( existing == gs_winuiBackdropPrimeStates.end() ||
                 existing->second.epoch != pending.epoch )
            {
                return;
            }

            if ( !restored )
            {
                if ( wxWinUIPreserveBackdropRestorePending(
                         operations, hwnd, pending, false) )
                {
                    return;
                }

                // A different RECT superseded the exact shrink, or the
                // identity expired. Never write the stale original.
                gs_winuiBackdropPrimeStates.erase(existing);
                return;
            }

            gs_winuiBackdropPrimeStates.erase(existing);
            if ( operations.hasPrimeMarker(
                     operations.context, hwndArg) )
            {
                return;
            }
            // The failed attempt never published its marker. The RECT is now
            // exact again, so this same explicit request may start a fresh
            // bounded prime below.
        }
        else
        {
            // Numeric HWND reuse: forget the old epoch without touching the
            // new window's properties.
            gs_winuiBackdropPrimeStates.erase(existing);
        }
    }

    std::map<HWND, unsigned long long>::iterator retry =
        gs_winuiBackdropRetryGenerations.find(hwnd);
    if ( retry != gs_winuiBackdropRetryGenerations.end() &&
         retry->second != generation )
    {
        gs_winuiBackdropRetryGenerations.erase(retry);
        retry = gs_winuiBackdropRetryGenerations.end();
    }

    const bool hasMarker =
        operations.hasPrimeMarker(operations.context, hwndArg);
    if ( retry == gs_winuiBackdropRetryGenerations.end() )
    {
        if ( hasMarker )
            return;
    }
    else
    {
        // A verified invalidation whose RemoveProp failed must not be
        // mistaken for a completed prime forever. Retry the property boundary
        // first; if it still fails, a later request tries again.
        if ( hasMarker &&
             !operations.clearPrimeMarker(
                 operations.context, hwndArg) )
        {
            return;
        }
        gs_winuiBackdropRetryGenerations.erase(retry);
    }

    wxWinUITransientRect originalRect;
    int width = 0;
    int height = 0;
    if ( !operations.getRect(
             operations.context, hwndArg, &originalRect) ||
         !wxWinUIGetBackdropDimensions(
             originalRect, &width, &height) )
    {
        return; // not laid out yet; a later request retries
    }

    if ( ++gs_nextWinUIBackdropPrimeEpoch == 0 )
        ++gs_nextWinUIBackdropPrimeEpoch;

    wxWinUIBackdropPrimeState state;
    state.generation = generation;
    state.epoch = gs_nextWinUIBackdropPrimeEpoch;
    state.originalRect = originalRect;
    state.expectedShrinkRect = originalRect;
    --state.expectedShrinkRect.bottom;
    gs_winuiBackdropPrimeStates[hwnd] = state;

    // Publish state before USER32: SetWindowPos can dispatch nested messages,
    // which must coalesce into this epoch instead of starting another shrink.
    const bool shrinkCallSucceeded =
        operations.setSize(
            operations.context, hwndArg, width, height - 1);

    wxWinUIBackdropPrimeStates::iterator afterShrink =
        gs_winuiBackdropPrimeStates.find(hwnd);
    wxWinUITransientRect observedRect;
    const bool shrinkVerified =
        shrinkCallSucceeded &&
        afterShrink != gs_winuiBackdropPrimeStates.end() &&
        afterShrink->second.epoch == state.epoch &&
        wxWinUIBackdropIdentityIsCurrent(
            operations, hwnd, state.generation) &&
        operations.getRect(
            operations.context, hwndArg, &observedRect) &&
        observedRect == state.expectedShrinkRect;
    if ( !shrinkVerified )
    {
        if ( afterShrink != gs_winuiBackdropPrimeStates.end() &&
             afterShrink->second.epoch == state.epoch )
        {
            const bool restored =
                wxWinUIRestoreExactBackdropShrink(
                    operations, hwnd, state);
            if ( !restored &&
                 wxWinUIPreserveBackdropRestorePending(
                     operations, hwnd, state, true) )
            {
                return;
            }
            const wxWinUIBackdropPrimeStates::const_iterator current =
                gs_winuiBackdropPrimeStates.find(hwnd);
            if ( current != gs_winuiBackdropPrimeStates.end() &&
                 current->second.epoch == state.epoch )
            {
                gs_winuiBackdropPrimeStates.erase(current);
            }
        }
        return;
    }

    // The old code set this before checking SetWindowPos and could permanently
    // suppress every retry. Publish success only after the first real resize
    // and verify the property boundary too.
    const bool markerSet =
        operations.setPrimeMarker(operations.context, hwndArg);
    afterShrink = gs_winuiBackdropPrimeStates.find(hwnd);
    const bool stillOwnsEpoch =
        afterShrink != gs_winuiBackdropPrimeStates.end() &&
        afterShrink->second.epoch == state.epoch;
    if ( !markerSet ||
         !operations.hasPrimeMarker(operations.context, hwndArg) ||
         !stillOwnsEpoch ||
         !wxWinUIBackdropIdentityIsCurrent(
             operations, hwnd, state.generation) )
    {
        if ( stillOwnsEpoch )
        {
            const bool restored =
                wxWinUIRestoreExactBackdropShrink(
                    operations, hwnd, state);
            if ( !restored &&
                 wxWinUIPreserveBackdropRestorePending(
                     operations, hwnd, state, true) )
            {
                return;
            }
            const wxWinUIBackdropPrimeStates::const_iterator current =
                gs_winuiBackdropPrimeStates.find(hwnd);
            if ( current != gs_winuiBackdropPrimeStates.end() &&
                 current->second.epoch == state.epoch )
            {
                if ( wxWinUIBackdropIdentityIsCurrent(
                         operations, hwnd, state.generation) &&
                     !operations.clearPrimeMarker(
                         operations.context, hwndArg) )
                {
                    gs_winuiBackdropRetryGenerations[hwnd] =
                        state.generation;
                }

                const wxWinUIBackdropPrimeStates::const_iterator
                    afterClear =
                        gs_winuiBackdropPrimeStates.find(hwnd);
                if ( afterClear !=
                        gs_winuiBackdropPrimeStates.end() &&
                     afterClear->second.epoch == state.epoch )
                {
                    gs_winuiBackdropPrimeStates.erase(afterClear);
                }
            }
        }
        return;
    }

    if ( !wxWinUIScheduleBackdropPrimeCallback(
             operations, hwnd, state) )
    {
        const bool restored =
            wxWinUIRestoreExactBackdropShrink(
                operations, hwnd, state);
        if ( !restored &&
             wxWinUIPreserveBackdropRestorePending(
                 operations, hwnd, state, true) )
        {
            return;
        }
        const wxWinUIBackdropPrimeStates::const_iterator current =
            gs_winuiBackdropPrimeStates.find(hwnd);
        if ( current != gs_winuiBackdropPrimeStates.end() &&
             current->second.epoch == state.epoch )
        {
            if ( wxWinUIBackdropIdentityIsCurrent(
                     operations, hwnd, state.generation) &&
                 !operations.clearPrimeMarker(
                     operations.context, hwndArg) )
            {
                gs_winuiBackdropRetryGenerations[hwnd] =
                    state.generation;
            }

            const wxWinUIBackdropPrimeStates::const_iterator afterClear =
                gs_winuiBackdropPrimeStates.find(hwnd);
            if ( afterClear != gs_winuiBackdropPrimeStates.end() &&
                 afterClear->second.epoch == state.epoch )
            {
                gs_winuiBackdropPrimeStates.erase(afterClear);
            }
        }
    }
}

void wxWinUIInvalidateBackdropPrime(WXHWND hwndArg)
{
    const wxWinUIBackdropNativeOps& operations = wxWinUIGetBackdropOps();
    if ( !wxWinUIBackdropOpsAreComplete(operations) )
        return;

    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const wxWinUIBackdropPrimeStates::const_iterator state =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( state != gs_winuiBackdropPrimeStates.end() )
    {
        const wxWinUIBackdropPrimeState snapshot = state->second;
        if ( wxWinUIBackdropIdentityIsCurrent(
                 operations, hwnd, snapshot.generation) )
        {
            wxWinUIBackdropPrimeStates::iterator currentState =
                gs_winuiBackdropPrimeStates.find(hwnd);
            if ( currentState != gs_winuiBackdropPrimeStates.end() &&
                 currentState->second.epoch == snapshot.epoch )
            {
                currentState->second.restorePending = false;
            }

            const bool restored =
                wxWinUIRestoreExactBackdropShrink(
                    operations, hwnd, snapshot);
            if ( !operations.clearPrimeMarker(
                     operations.context, hwndArg) )
            {
                gs_winuiBackdropRetryGenerations[hwnd] =
                    snapshot.generation;
            }
            else
            {
                gs_winuiBackdropRetryGenerations.erase(hwnd);
            }

            if ( !restored &&
                 wxWinUIPreserveBackdropRestorePending(
                     operations, hwnd, snapshot, false) )
            {
                return;
            }
        }

        // Restoring the exact shrink calls SetWindowPos and can dispatch a
        // nested owner/prime transition. Erase only the epoch we inspected.
        const wxWinUIBackdropPrimeStates::const_iterator current =
            gs_winuiBackdropPrimeStates.find(hwnd);
        if ( current != gs_winuiBackdropPrimeStates.end() &&
             current->second.epoch == snapshot.epoch )
        {
            gs_winuiBackdropPrimeStates.erase(current);
        }
        return;
    }

    const unsigned long long generation =
        operations.getGeneration(operations.context, hwndArg);
    if ( generation &&
         wxWinUIBackdropIdentityIsCurrent(
             operations, hwnd, generation) )
    {
        if ( !operations.clearPrimeMarker(
                 operations.context, hwndArg) )
        {
            gs_winuiBackdropRetryGenerations[hwnd] = generation;
        }
        else
        {
            gs_winuiBackdropRetryGenerations.erase(hwnd);
        }
    }
}

void wxWinUIBackdropWindowDestroyed(WXHWND hwndArg)
{
    // WM_DESTROY is not an ordinary invalidation: never resize a dying
    // window. Retiring the exact epoch makes its eventual callback a no-op,
    // and native properties disappear with this HWND identity.
    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    gs_winuiBackdropPrimeStates.erase(hwnd);
    gs_winuiBackdropRetryGenerations.erase(hwnd);
}

void
wxWinUI3SetBackdropNativeOpsForTesting(
    const wxWinUIBackdropNativeOps& operations)
{
    wxCHECK_RET(wxWinUIBackdropOpsAreComplete(operations),
                "incomplete WinUI backdrop operation table");

    gs_winuiBackdropPrimeStates.clear();
    gs_winuiBackdropRetryGenerations.clear();
    gs_winuiBackdropTestOps = operations;
    gs_hasWinUIBackdropTestOps = true;
}

void wxWinUI3ResetBackdropNativeOpsForTesting()
{
    gs_winuiBackdropPrimeStates.clear();
    gs_winuiBackdropRetryGenerations.clear();
    gs_winuiBackdropTestOps = wxWinUIBackdropNativeOps();
    gs_hasWinUIBackdropTestOps = false;
}

wxWinUITransientSnapshot
wxWinUI3GetBackdropSnapshotForTesting(WXHWND hwndArg)
{
    wxWinUITransientSnapshot snapshot;
    snapshot.transactionCount = gs_winuiBackdropPrimeStates.size();

    const HWND hwnd = reinterpret_cast<HWND>(hwndArg);
    const wxWinUIBackdropPrimeStates::const_iterator state =
        gs_winuiBackdropPrimeStates.find(hwnd);
    if ( state != gs_winuiBackdropPrimeStates.end() )
    {
        snapshot.inFlight = !state->second.restorePending;
        snapshot.restorePending = state->second.restorePending;
        snapshot.generation = state->second.generation;
        snapshot.epoch = state->second.epoch;
    }
    return snapshot;
}

void wxWinUISetAppTheme(wxWinUIAppTheme theme)
{
    using winrt::Microsoft::UI::Xaml::ElementTheme;

    wxCHECK_RET(wxIsMainThread(),
                "WinUI application theme must be changed on the UI thread");

    gs_winuiAppTheme = theme;
    switch ( theme )
    {
        case wxWinUIAppTheme::Light:
            gs_winuiElementTheme = ElementTheme::Light;
            break;
        case wxWinUIAppTheme::Dark:
            gs_winuiElementTheme = ElementTheme::Dark;
            break;
        case wxWinUIAppTheme::System:
            gs_winuiElementTheme = ElementTheme::Default;
            break;
    }

    // Publish the new appearance to wxSystemSettings before any window is
    // updated: this also covers an application changing the theme while it has
    // no top-level window yet.
    wxWinUISyncClassicAppearance();

    wxWinUITopLevelHost::ApplyThemeToAll(gs_winuiElementTheme);

    // Notify all classic/generic surfaces as well as native controls. The
    // normal WM_SYSCOLORCHANGE path recursively updates children and refreshes
    // the WinUI backdrop/title bar of the TLW after application handlers have
    // observed the new palette. Every SendMessage() is a re-entrancy boundary,
    // so never retain a wxTopLevelWindows iterator across one notification.
    std::vector<wxWeakRef<wxWindow>> topLevels;
    topLevels.reserve(wxTopLevelWindows.size());
    for ( wxWindowList::const_iterator i = wxTopLevelWindows.begin();
          i != wxTopLevelWindows.end();
          ++i )
    {
        topLevels.emplace_back(*i);
    }
    for ( const wxWeakRef<wxWindow>& weakTopLevel : topLevels )
    {
        wxWindow * const topLevel = weakTopLevel.get();
        if ( topLevel && topLevel->IsTopLevel() &&
             !topLevel->IsBeingDeleted() &&
             !wxPendingDelete.Member(topLevel) )
        {
            const WXHWND hwnd = GetHwndOf(topLevel);
            if ( hwnd )
                ::SendMessage(hwnd, WM_SYSCOLORCHANGE, 0, 0);
        }
    }
}

wxWinUIAppTheme wxWinUIGetAppTheme()
{
    return gs_winuiAppTheme;
}

// An application may well ask wxSystemSettings for the colours to use before
// creating any window at all, so the appearance can't only be published when
// the first top-level window applies the shell policy.
class wxWinUIAppearanceModule : public wxModule
{
public:
    bool OnInit() override
    {
        wxWinUISyncClassicAppearance();
        return true;
    }

    void OnExit() override { }

private:
    wxDECLARE_DYNAMIC_CLASS(wxWinUIAppearanceModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUIAppearanceModule, wxModule);

#endif // wxUSE_WINUI3
