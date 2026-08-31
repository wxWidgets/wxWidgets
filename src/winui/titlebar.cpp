/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/titlebar.cpp
// Purpose:     WinAppSDK title-bar policy for an existing wx TLW
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "wx/winui/private/titlebar.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/weakref.h"

#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <array>
#include <map>
#include <memory>
#include <new>
#include <vector>

namespace
{

using winrt::Microsoft::UI::Windowing::AppWindow;
using winrt::Microsoft::UI::Windowing::AppWindowTitleBar;

enum class wxWinUITitleBarUse
{
    Attached,
    DetachedTeardown
};

struct wxWinUITitleBarPolicy
{
    wxWinUITopLevelHost *host = nullptr; // comparison key, never dereferenced
    std::weak_ptr<wxWinUIHostLifetime> hostLifetime;
    wxWeakRef<wxWindow> tlw;
    WXHWND hwnd = nullptr;
    unsigned long long publicationGeneration = 0;
    unsigned long long hwndGeneration = 0;
    unsigned long long nativeHwndGeneration = 0;
    AppWindow appWindow{ nullptr };
    AppWindowTitleBar titleBar{ nullptr };
    unsigned long long appWindowId = 0;
    bool attached = false;
    bool initializing = true;
    bool mutationInProgress = false;
    bool teardownResetPending = false;
    bool countTeardownResetPending = false;
    bool customizationSupported = false;
    bool systemFallback = false;
    bool extensionMayBeActive = true;
    bool dragRectangleWriteCommitted = false;
    wxWinUITitleBarMode mode = wxWinUITitleBarMode::Unknown;
    winrt::Windows::Graphics::RectInt32 dragRectangle{};
};

using wxWinUITitleBarPolicyPtr = std::shared_ptr<wxWinUITitleBarPolicy>;
using wxWinUITitleBarPolicies =
    std::map<wxWinUITopLevelHost *, wxWinUITitleBarPolicyPtr>;

wxWinUITitleBarPolicies gs_titleBarPolicies;
unsigned long long gs_titleBarPublicationGeneration = 0;
unsigned gs_titleBarFaultMask = wxWINUI_TITLEBAR_FAULT_NONE;
wxWinUITitleBarHookForTesting gs_titleBarHook = nullptr;
bool gs_titleBarHookActive = false;
unsigned gs_titleBarTeardownResets = 0;

unsigned long long wxWinUINextTitleBarPublicationGeneration()
{
    if ( ++gs_titleBarPublicationGeneration == 0 )
        ++gs_titleBarPublicationGeneration;
    return gs_titleBarPublicationGeneration;
}

bool wxWinUIConsumeTitleBarFault(unsigned fault)
{
    if ( !(gs_titleBarFaultMask & fault) )
        return false;

    gs_titleBarFaultMask &= ~fault;
    return true;
}

void wxWinUIThrowTitleBarFaultIfRequested(unsigned fault)
{
    if ( wxWinUIConsumeTitleBarFault(fault) )
    {
        throw winrt::hresult_error(
            E_FAIL, L"Injected AppWindowTitleBar transaction failure");
    }
}

void wxWinUIInvokeTitleBarHook(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarHookPointForTesting point)
{
    if ( !policy || !gs_titleBarHook || gs_titleBarHookActive )
        return;

    wxWindow * const window = policy->tlw.get();
    if ( !window )
        return;

    gs_titleBarHookActive = true;
    try
    {
        gs_titleBarHook(window, point);
    }
    catch ( ... )
    {
        gs_titleBarHookActive = false;
        return;
    }
    gs_titleBarHookActive = false;
}

void wxWinUILogTitleBarException(
    const wxWinUITitleBarPolicyPtr& policy,
    const char *where,
    const winrt::hresult_error& error)
{
    wxWinUIInvokeTitleBarHook(
        policy, wxWinUITitleBarHookPointForTesting::BeforeExceptionLog);
    wxWinUILogException(where, error);
    wxWinUIInvokeTitleBarHook(
        policy, wxWinUITitleBarHookPointForTesting::AfterExceptionLog);
}

bool wxWinUITitleBarHasNativeIdentity(
    const wxWinUITitleBarPolicyPtr& policy)
{
    if ( !policy )
        return false;

    wxWindow * const window = policy->tlw.get();
    return window && policy->hwnd &&
           reinterpret_cast<WXHWND>(window->GetHWND()) == policy->hwnd &&
           wxWinUIMSWGetHwndGeneration(window, policy->hwnd) ==
               policy->hwndGeneration &&
           wxWinUIMSWGetNativeHwndGeneration(policy->hwnd) ==
               policy->nativeHwndGeneration;
}

bool wxWinUITitleBarHasAttachedPublication(
    const wxWinUITitleBarPolicyPtr& policy)
{
    if ( !policy || !policy->attached )
        return false;

    wxWindow * const window = policy->tlw.get();
    if ( !window || wxWinUITLWHostIsDestroyScheduled(window) )
        return false;

    const std::shared_ptr<wxWinUIHostLifetime> hostLifetime =
        policy->hostLifetime.lock();
    if ( !hostLifetime || !policy->host ||
         hostLifetime->GetHost() != policy->host ||
         policy->host->IsShuttingDown() )
    {
        return false;
    }

    const auto found = gs_titleBarPolicies.find(policy->host);
    return found != gs_titleBarPolicies.end() &&
           found->second == policy &&
           found->second->publicationGeneration ==
               policy->publicationGeneration;
}

bool wxWinUITitleBarHasNewerOwner(
    const wxWinUITitleBarPolicyPtr& policy)
{
    wxWindow * const window = policy ? policy->tlw.get() : nullptr;
    if ( !window )
        return true;

    for ( const auto& entry : gs_titleBarPolicies )
    {
        const wxWinUITitleBarPolicyPtr& candidate = entry.second;
        if ( !candidate || candidate == policy || !candidate->attached ||
             candidate->tlw.get() != window )
        {
            continue;
        }

        if ( candidate->hwnd == policy->hwnd &&
             candidate->hwndGeneration == policy->hwndGeneration &&
             candidate->nativeHwndGeneration ==
                 policy->nativeHwndGeneration )
        {
            return true;
        }
    }

    return false;
}

bool wxWinUITitleBarHasContext(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use)
{
    if ( !wxWinUITitleBarHasNativeIdentity(policy) )
        return false;

    if ( use == wxWinUITitleBarUse::Attached )
    {
        return wxWinUITitleBarHasAttachedPublication(policy);
    }

    // Detach extracts the shared state and invalidates its publication before
    // crossing AppWindowTitleBar. It may still reset the exact old native
    // generation, but never after a re-entrant attach has published a newer
    // owner for that TLW/HWND.
    if ( policy->attached || wxWinUITitleBarHasNewerOwner(policy) )
        return false;

    const auto found = gs_titleBarPolicies.find(policy->host);
    return found == gs_titleBarPolicies.end() || found->second != policy;
}

bool wxWinUITitleBarHasAppWindowIdentity(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use)
{
    if ( !wxWinUITitleBarHasContext(policy, use) ||
         !policy->appWindow || !policy->appWindowId )
    {
        return false;
    }

    try
    {
        if ( !wxWinUITitleBarHasContext(policy, use) )
            return false;
        const unsigned long long currentId = policy->appWindow.Id().Value;
        return wxWinUITitleBarHasContext(policy, use) &&
               currentId == policy->appWindowId &&
               wxWinUITitleBarHasContext(policy, use);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxWinUITitleBarPolicyPtr wxWinUIFindTitleBarPolicy(wxWindow *tlw)
{
    if ( !tlw )
        return {};

    for ( const auto& entry : gs_titleBarPolicies )
    {
        const wxWinUITitleBarPolicyPtr& policy = entry.second;
        if ( policy && policy->attached && policy->tlw.get() == tlw )
            return policy;
    }

    return {};
}

bool wxWinUIReadExtended(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use,
    unsigned fault,
    bool *extended)
{
    if ( !extended ||
         !wxWinUITitleBarHasAppWindowIdentity(policy, use) ||
         !policy->titleBar )
    {
        return false;
    }

    wxWinUIThrowTitleBarFaultIfRequested(fault);
    const bool value = policy->titleBar.ExtendsContentIntoTitleBar();
    if ( !wxWinUITitleBarHasAppWindowIdentity(policy, use) )
        return false;

    *extended = value;
    return true;
}

void wxWinUIPublishUnknown(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use)
{
    if ( !wxWinUITitleBarHasContext(policy, use) )
        return;

    policy->mode = wxWinUITitleBarMode::Unknown;
    policy->systemFallback = false;
    policy->extensionMayBeActive = true;
    policy->dragRectangleWriteCommitted = false;
}

bool wxWinUIResetTitleBarToSystemCore(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use,
    bool fallback)
{
    if ( !wxWinUITitleBarHasContext(policy, use) )
        return false;

    // A title-less TLW or an unavailable/unsupported customization API has no
    // AppWindow extension authority at all: USER32/DWM is therefore the
    // observable system-caption authority without a speculative WinRT write.
    if ( !policy->titleBar || !policy->customizationSupported )
    {
        policy->mode = wxWinUITitleBarMode::System;
        policy->systemFallback = true;
        policy->extensionMayBeActive = false;
        policy->dragRectangleWriteCommitted = false;
        return true;
    }

    try
    {
        // Establish the exact pre-state before mutating it. Publication turns
        // Unknown before the first setter so a callback observing the policy
        // can never mistake an in-flight or failed write for system chrome.
        bool wasExtended = false;
        if ( !wxWinUIReadExtended(
                 policy, use,
                 wxWINUI_TITLEBAR_FAULT_READ_BEFORE_MUTATION,
                 &wasExtended) )
        {
            wxWinUIPublishUnknown(policy, use);
            return false;
        }

        if ( !wxWinUITitleBarHasContext(policy, use) )
            return false;
        policy->mode = wxWinUITitleBarMode::Unknown;
        policy->systemFallback = false;
        policy->extensionMayBeActive = wasExtended;
        policy->dragRectangleWriteCommitted = false;

        if ( !wxWinUITitleBarHasAppWindowIdentity(policy, use) )
            return false;
        // ResetToDefault() is the complete AppWindowTitleBar transition back
        // to the system defaults. Do not precede it with an explicit
        // ExtendsContentIntoTitleBar(false): that redundant two-write
        // sequence crosses the WinAppSDK title-bar teardown boundary twice
        // and can corrupt its internal state before ResetToDefault() runs.
        // From immediately before the call until the readback proves false,
        // teardown must conservatively assume that extension could be live.
        policy->extensionMayBeActive = true;
        wxWinUIThrowTitleBarFaultIfRequested(
            wxWINUI_TITLEBAR_FAULT_RESET_TO_DEFAULT);
        policy->titleBar.ResetToDefault();
        wxWinUIInvokeTitleBarHook(
            policy, wxWinUITitleBarHookPointForTesting::AfterResetToDefault);
        if ( !wxWinUITitleBarHasAppWindowIdentity(policy, use) )
            return false;

        bool extended = true;
        if ( !wxWinUIReadExtended(
                 policy, use,
                 wxWINUI_TITLEBAR_FAULT_READ_AFTER_MUTATION,
                 &extended) )
        {
            wxWinUIPublishUnknown(policy, use);
            return false;
        }

        if ( extended )
        {
            policy->mode = wxWinUITitleBarMode::ExtendedAppWindow;
            policy->systemFallback = false;
            policy->extensionMayBeActive = true;
            return false;
        }

        policy->mode = wxWinUITitleBarMode::System;
        policy->systemFallback = fallback;
        policy->extensionMayBeActive = false;
        return true;
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogTitleBarException(
            policy, "WinUI AppWindowTitleBar system fallback", error);
        if ( !wxWinUITitleBarHasContext(policy, use) )
            return false;

        // A write failure is not a verdict. Only an exact post-failure read
        // may publish System or Extended; a failed read remains Unknown and
        // all callers fail closed.
        try
        {
            bool extended = true;
            if ( wxWinUIReadExtended(
                     policy, use,
                     wxWINUI_TITLEBAR_FAULT_READ_AFTER_MUTATION,
                     &extended) )
            {
                policy->mode = extended
                    ? wxWinUITitleBarMode::ExtendedAppWindow
                    : wxWinUITitleBarMode::System;
                policy->systemFallback = !extended && fallback;
                policy->extensionMayBeActive = extended;
                policy->dragRectangleWriteCommitted = false;
                return !extended;
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }

        wxWinUIPublishUnknown(policy, use);
        return false;
    }
}

void wxWinUIFinishTitleBarMutation(
    const wxWinUITitleBarPolicyPtr& policy)
{
    if ( !policy )
        return;

    policy->mutationInProgress = false;
    if ( !policy->teardownResetPending )
        return;

    const bool countReset = policy->countTeardownResetPending;
    policy->teardownResetPending = false;
    policy->countTeardownResetPending = false;

    if ( !policy->extensionMayBeActive &&
         policy->mode == wxWinUITitleBarMode::System )
    {
        return;
    }

    policy->mutationInProgress = true;
    bool reset = false;
    try
    {
        reset = wxWinUIResetTitleBarToSystemCore(
            policy, wxWinUITitleBarUse::DetachedTeardown, true);
    }
    catch ( ... )
    {
        // This function is reached from a scope-guard destructor. An
        // implementation-only hook must never let teardown escape it.
    }
    policy->mutationInProgress = false;
    if ( reset && countReset )
        ++gs_titleBarTeardownResets;
}

class wxWinUITitleBarMutationGuard
{
public:
    explicit wxWinUITitleBarMutationGuard(
        const wxWinUITitleBarPolicyPtr& policy)
        : m_policy(policy)
    {
    }

    ~wxWinUITitleBarMutationGuard()
    {
        wxWinUIFinishTitleBarMutation(m_policy);
    }

private:
    wxWinUITitleBarPolicyPtr m_policy;
};

bool wxWinUIBeginTitleBarMutation(
    const wxWinUITitleBarPolicyPtr& policy,
    wxWinUITitleBarUse use)
{
    if ( !wxWinUITitleBarHasContext(policy, use) ||
         policy->mutationInProgress )
    {
        return false;
    }

    policy->mutationInProgress = true;
    return true;
}

bool wxWinUIExtendTitleBarForTest(
    const wxWinUITitleBarPolicyPtr& policy)
{
    if ( !policy || policy->mode == wxWinUITitleBarMode::Unknown ||
         !wxWinUITitleBarHasAppWindowIdentity(
             policy, wxWinUITitleBarUse::Attached) ||
         !policy->titleBar || !policy->customizationSupported ||
         !wxWinUIBeginTitleBarMutation(
             policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }

    wxWinUITitleBarMutationGuard finish(policy);
    try
    {
        policy->mode = wxWinUITitleBarMode::Unknown;
        policy->systemFallback = false;
        policy->extensionMayBeActive = true;
        policy->dragRectangleWriteCommitted = false;

        policy->titleBar.ExtendsContentIntoTitleBar(true);
        wxWinUIInvokeTitleBarHook(
            policy, wxWinUITitleBarHookPointForTesting::AfterExtensionWrite);
        wxWinUIThrowTitleBarFaultIfRequested(
            wxWINUI_TITLEBAR_FAULT_EXTENSION_WRITE_AFTER);
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }

        const int leftInset = policy->titleBar.LeftInset();
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }
        const int rightInset = policy->titleBar.RightInset();
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }
        const int height = policy->titleBar.Height();
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }

        RECT client = {};
        if ( !::GetClientRect(reinterpret_cast<HWND>(policy->hwnd), &client) ||
             height <= 0 ||
             client.right <= client.left + leftInset + rightInset )
        {
            wxWinUIResetTitleBarToSystemCore(
                policy, wxWinUITitleBarUse::Attached, true);
            return false;
        }

        const winrt::Windows::Graphics::RectInt32 dragRectangle =
        {
            leftInset,
            0,
            client.right - client.left - leftInset - rightInset,
            height
        };
        const std::array<winrt::Windows::Graphics::RectInt32, 1> dragRects =
            {{ dragRectangle }};
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }
        policy->titleBar.SetDragRectangles(dragRects);
        wxWinUIInvokeTitleBarHook(
            policy,
            wxWinUITitleBarHookPointForTesting::AfterDragRectanglesWrite);
        wxWinUIThrowTitleBarFaultIfRequested(
            wxWINUI_TITLEBAR_FAULT_DRAG_WRITE_AFTER);
        if ( !wxWinUITitleBarHasAppWindowIdentity(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }
        policy->dragRectangle = dragRectangle;
        policy->dragRectangleWriteCommitted = true;

        bool extended = false;
        if ( !wxWinUIReadExtended(
                 policy, wxWinUITitleBarUse::Attached,
                 wxWINUI_TITLEBAR_FAULT_READ_AFTER_MUTATION,
                 &extended) || !extended )
        {
            wxWinUIResetTitleBarToSystemCore(
                policy, wxWinUITitleBarUse::Attached, true);
            return false;
        }

        policy->mode = wxWinUITitleBarMode::ExtendedAppWindow;
        policy->systemFallback = false;
        policy->extensionMayBeActive = true;
        return true;
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogTitleBarException(
            policy, "WinUI AppWindowTitleBar test extension", error);
        if ( wxWinUITitleBarHasContext(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            wxWinUIResetTitleBarToSystemCore(
                policy, wxWinUITitleBarUse::Attached, true);
        }
        return false;
    }
}

bool wxWinUICountFirstPartyXamlTitleBars(
    const wxWinUITitleBarPolicyPtr& policy,
    unsigned *result)
{
    if ( !result )
        return false;
    *result = 0;

    if ( !wxWinUITitleBarHasContext(
             policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }

    const std::shared_ptr<wxWinUIHostLifetime> hostLifetime =
        policy->hostLifetime.lock();
    wxWinUITopLevelHost * const host =
        hostLifetime ? hostLifetime->GetHost() : nullptr;
    if ( !host || host != policy->host )
        return false;

    try
    {
        using winrt::Microsoft::UI::Xaml::DependencyObject;
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const auto root = host->Root();
        if ( !root || !wxWinUITitleBarHasContext(
                         policy, wxWinUITitleBarUse::Attached) )
        {
            return false;
        }

        unsigned count = 0;
        std::vector<DependencyObject> pending;
        pending.push_back(root);
        while ( !pending.empty() )
        {
            const DependencyObject current = pending.back();
            pending.pop_back();

            const winrt::hstring className = winrt::get_class_name(current);
            if ( !wxWinUITitleBarHasContext(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                return false;
            }
            if ( className == L"Microsoft.UI.Xaml.Controls.TitleBar" )
                ++count;

            const int childCount =
                VisualTreeHelper::GetChildrenCount(current);
            if ( !wxWinUITitleBarHasContext(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                return false;
            }
            for ( int child = 0; child < childCount; ++child )
            {
                pending.push_back(
                    VisualTreeHelper::GetChild(current, child));
                if ( !wxWinUITitleBarHasContext(
                         policy, wxWinUITitleBarUse::Attached) )
                {
                    return false;
                }
            }
        }

        *result = count;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
    catch ( const std::bad_alloc& )
    {
        return false;
    }
}

} // anonymous namespace

void wxWinUITitleBarAttachSystemPolicy(
    wxWinUITopLevelHost *host,
    const std::shared_ptr<wxWinUIHostLifetime>& hostLifetime,
    wxWindow *tlw,
    WXHWND hwnd)
{
    if ( !host || !hostLifetime || hostLifetime->GetHost() != host ||
         host->IsShuttingDown() ||
         !tlw || !hwnd )
    {
        return;
    }

    // A host address is one lifecycle identity. Extract any older policy
    // before publishing the new generation.
    wxWinUITitleBarDetachSystemPolicy(host);
    if ( hostLifetime->GetHost() != host )
        return;

    try
    {
        const wxWinUITitleBarPolicyPtr policy =
            std::make_shared<wxWinUITitleBarPolicy>();
        policy->host = host;
        policy->hostLifetime = hostLifetime;
        policy->tlw = wxWeakRef<wxWindow>(tlw);
        policy->hwnd = hwnd;
        policy->publicationGeneration =
            wxWinUINextTitleBarPublicationGeneration();
        policy->hwndGeneration = wxWinUIMSWGetHwndGeneration(tlw, hwnd);
        policy->nativeHwndGeneration =
            wxWinUIMSWGetNativeHwndGeneration(hwnd);
        if ( !policy->hwndGeneration || !policy->nativeHwndGeneration ||
             !wxWinUITitleBarHasNativeIdentity(policy) ||
             hostLifetime->GetHost() != host )
        {
            return;
        }

        // Publish a provisional Unknown policy before the first callback-rich
        // WinRT operation. Detach can now extract/invalidate this exact state;
        // the shared transaction below survives long enough to fail closed.
        policy->attached = true;
        policy->mutationInProgress = true;
        const auto insertion = gs_titleBarPolicies.emplace(host, policy);
        if ( !insertion.second )
            return;
        wxWinUITitleBarMutationGuard finish(policy);

        const LONG_PTR nativeStyle = ::GetWindowLongPtr(
            reinterpret_cast<HWND>(hwnd), GWL_STYLE);
        if ( !wxWinUITitleBarHasContext(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            return;
        }

        if ( !(nativeStyle & WS_CAPTION) )
        {
            policy->mode = wxWinUITitleBarMode::System;
            policy->systemFallback = true;
            policy->extensionMayBeActive = false;
            policy->initializing = false;
            return;
        }

        try
        {
            const auto windowId =
                winrt::Microsoft::UI::GetWindowIdFromWindow(
                reinterpret_cast<HWND>(hwnd));
            if ( !wxWinUITitleBarHasContext(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                return;
            }
            policy->appWindow = AppWindow::GetFromWindowId(windowId);
            if ( !wxWinUITitleBarHasContext(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                return;
            }

            if ( policy->appWindow )
            {
                policy->appWindowId = policy->appWindow.Id().Value;
                if ( !wxWinUITitleBarHasAppWindowIdentity(
                         policy, wxWinUITitleBarUse::Attached) )
                {
                    return;
                }
                policy->titleBar = policy->appWindow.TitleBar();
                if ( !wxWinUITitleBarHasAppWindowIdentity(
                         policy, wxWinUITitleBarUse::Attached) )
                {
                    return;
                }
                policy->customizationSupported =
                    AppWindowTitleBar::IsCustomizationSupported();
                if ( !wxWinUITitleBarHasAppWindowIdentity(
                         policy, wxWinUITitleBarUse::Attached) )
                {
                    return;
                }
            }
        }
        catch ( const winrt::hresult_error& error )
        {
            wxWinUILogTitleBarException(
                policy, "WinUI AppWindowTitleBar acquisition", error);
            if ( !wxWinUITitleBarHasContext(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                return;
            }

            // Acquisition failed before we could observe the AppWindow
            // extension property. Do not call this System merely because the
            // Win32 style still contains WS_CAPTION.
            policy->appWindow = nullptr;
            policy->titleBar = nullptr;
            policy->appWindowId = 0;
            policy->customizationSupported = false;
            policy->mode = wxWinUITitleBarMode::Unknown;
            policy->systemFallback = false;
            policy->extensionMayBeActive = true;
            policy->initializing = false;
            return;
        }

        if ( policy->titleBar && policy->customizationSupported )
        {
            wxWinUIResetTitleBarToSystemCore(
                policy, wxWinUITitleBarUse::Attached, false);
        }
        else
        {
            policy->mode = wxWinUITitleBarMode::System;
            policy->systemFallback = true;
            policy->extensionMayBeActive = false;
        }

        if ( wxWinUITitleBarHasContext(
                 policy, wxWinUITitleBarUse::Attached) )
        {
            policy->initializing = false;
        }
    }
    catch ( const std::bad_alloc& )
    {
        // Optional policy bookkeeping must not turn an otherwise usable
        // system-caption TLW into a failed WinUI host under memory pressure.
    }
}

void wxWinUITitleBarDetachSystemPolicy(wxWinUITopLevelHost *host)
{
    const auto found = gs_titleBarPolicies.find(host);
    if ( found == gs_titleBarPolicies.end() )
        return;

    const wxWinUITitleBarPolicyPtr policy = found->second;
    gs_titleBarPolicies.erase(found);
    if ( !policy )
        return;

    // Invalidate publication before any ResetToDefault/property setter/log.
    // A re-entrant attach may safely reuse the host key; the extracted shared
    // state can only touch its exact native generation and rejects a newer
    // owner of the same TLW/HWND.
    policy->attached = false;
    const bool resetRequired =
        policy->extensionMayBeActive ||
        policy->mode != wxWinUITitleBarMode::System;
    if ( !resetRequired )
        return;

    if ( policy->mutationInProgress )
    {
        policy->teardownResetPending = true;
        policy->countTeardownResetPending = true;
        return;
    }

    policy->mutationInProgress = true;
    bool reset = false;
    try
    {
        reset = wxWinUIResetTitleBarToSystemCore(
            policy, wxWinUITitleBarUse::DetachedTeardown, true);
    }
    catch ( ... )
    {
        // Shutdown must remain non-throwing even for a hostile private hook.
    }
    policy->mutationInProgress = false;
    if ( reset )
        ++gs_titleBarTeardownResets;
}

bool wxWinUITitleBarSetExtendedForTesting(wxWindow *tlw, bool extended)
{
    const wxWinUITitleBarPolicyPtr policy =
        wxWinUIFindTitleBarPolicy(tlw);
    if ( !policy || !wxWinUITitleBarHasContext(
                         policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }

    if ( extended )
        return wxWinUIExtendTitleBarForTest(policy);

    if ( !wxWinUIBeginTitleBarMutation(
             policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }
    wxWinUITitleBarMutationGuard finish(policy);
    return wxWinUIResetTitleBarToSystemCore(
        policy, wxWinUITitleBarUse::Attached, false);
}

bool wxWinUITitleBarSnapshotForTesting(
    wxWindow *tlw,
    wxWinUITitleBarSnapshot *snapshot)
{
    if ( !snapshot )
        return false;

    *snapshot = wxWinUITitleBarSnapshot();
    const wxWinUITitleBarPolicyPtr policy =
        wxWinUIFindTitleBarPolicy(tlw);
    if ( !policy )
        return false;

    snapshot->attached = policy->attached;
    snapshot->hwnd = policy->hwnd;
    snapshot->hwndGeneration = policy->hwndGeneration;
    wxWindow * const window = policy->tlw.get();
    snapshot->currentHwndGeneration = window
        ? wxWinUIMSWGetHwndGeneration(window, policy->hwnd)
        : 0;
    snapshot->nativeHwndGeneration = policy->nativeHwndGeneration;
    snapshot->currentNativeHwndGeneration =
        wxWinUIMSWGetNativeHwndGeneration(policy->hwnd);
    snapshot->hwndIdentityCurrent = wxWinUITitleBarHasContext(
        policy, wxWinUITitleBarUse::Attached);
    snapshot->appWindowAvailable = policy->appWindow != nullptr;
    snapshot->appWindowIdentityCurrent =
        wxWinUITitleBarHasAppWindowIdentity(
            policy, wxWinUITitleBarUse::Attached);
    if ( snapshot->appWindowIdentityCurrent )
    {
        try
        {
            const unsigned long long currentId =
                policy->appWindow.Id().Value;
            if ( wxWinUITitleBarHasAppWindowIdentity(
                     policy, wxWinUITitleBarUse::Attached) )
            {
                snapshot->currentAppWindowId = currentId;
            }
            else
            {
                snapshot->appWindowIdentityCurrent = false;
            }
        }
        catch ( const winrt::hresult_error& )
        {
            snapshot->appWindowIdentityCurrent = false;
        }
    }
    snapshot->customizationSupported = policy->customizationSupported;
    snapshot->systemFallback =
        policy->mode == wxWinUITitleBarMode::System &&
        policy->systemFallback;
    snapshot->mode = policy->mode;
    snapshot->appWindowId = policy->appWindowId;
    snapshot->dragRectangleWriteCommitted =
        policy->dragRectangleWriteCommitted &&
        policy->mode == wxWinUITitleBarMode::ExtendedAppWindow &&
        snapshot->appWindowIdentityCurrent;
    if ( snapshot->dragRectangleWriteCommitted )
    {
        snapshot->dragRectangleX = policy->dragRectangle.X;
        snapshot->dragRectangleY = policy->dragRectangle.Y;
        snapshot->dragRectangleWidth = policy->dragRectangle.Width;
        snapshot->dragRectangleHeight = policy->dragRectangle.Height;
    }

    snapshot->firstPartyXamlTitleBarScanSucceeded =
        wxWinUICountFirstPartyXamlTitleBars(
            policy, &snapshot->firstPartyXamlTitleBarCount);
    snapshot->firstPartyXamlTitleBarInstalled =
        snapshot->firstPartyXamlTitleBarScanSucceeded &&
        snapshot->firstPartyXamlTitleBarCount != 0;
    // A snapshot is also the deterministic observation seam for deliberately
    // stale native identities. Keep exposing the still-current publication,
    // while all mutation entry points continue to require HasContext(). A
    // scheduled destroy or detached/replaced publication remains unobservable.
    if ( !wxWinUITitleBarHasAttachedPublication(policy) )
    {
        *snapshot = wxWinUITitleBarSnapshot();
        return false;
    }
    return true;
}

void wxWinUITitleBarFailNextExtensionForTesting()
{
    gs_titleBarFaultMask |=
        wxWINUI_TITLEBAR_FAULT_EXTENSION_WRITE_AFTER;
}

void wxWinUITitleBarSetHookForTesting(wxWinUITitleBarHookForTesting hook)
{
    gs_titleBarHook = hook;
}

void wxWinUITitleBarSetFaultMaskForTesting(unsigned mask)
{
    gs_titleBarFaultMask = mask;
}

bool wxWinUITitleBarInvalidateNativeIdentityForTesting(wxWindow *tlw)
{
    const wxWinUITitleBarPolicyPtr policy =
        wxWinUIFindTitleBarPolicy(tlw);
    if ( !policy || !wxWinUITitleBarHasContext(
                         policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }

    if ( ++policy->nativeHwndGeneration == 0 )
        ++policy->nativeHwndGeneration;
    return !wxWinUITitleBarHasContext(
        policy, wxWinUITitleBarUse::Attached);
}

bool wxWinUITitleBarInvalidateAppWindowIdentityForTesting(wxWindow *tlw)
{
    const wxWinUITitleBarPolicyPtr policy =
        wxWinUIFindTitleBarPolicy(tlw);
    if ( !policy || !wxWinUITitleBarHasAppWindowIdentity(
                         policy, wxWinUITitleBarUse::Attached) )
    {
        return false;
    }

    if ( ++policy->appWindowId == 0 )
        ++policy->appWindowId;
    return !wxWinUITitleBarHasAppWindowIdentity(
        policy, wxWinUITitleBarUse::Attached);
}

unsigned wxWinUITitleBarGetLivePolicyCountForTesting()
{
    return static_cast<unsigned>(gs_titleBarPolicies.size());
}

unsigned wxWinUITitleBarGetTeardownResetCountForTesting()
{
    return gs_titleBarTeardownResets;
}

#endif // wxUSE_WINUI3
