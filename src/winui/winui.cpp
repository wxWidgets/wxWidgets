/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/winui.cpp
// Purpose:     wxWinUI bootstrap support
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/tlwhostmsw.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/module.h"
    #include "wx/string.h"
    #include "wx/window.h"
#endif

#include "wx/msw/wrapwin.h"

#include <objbase.h>
#include <tlhelp32.h>

#include <algorithm>
#include <exception>
#include <utility>
#include <vector>

#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>

namespace
{

bool gs_winuiBootstrapInitialized = false;
bool gs_winuiComInitialized = false;
std::atomic<DWORD> gs_winuiOwnerThreadId{0};
HMODULE gs_winuiPinnedXamlModule = nullptr;
HMODULE gs_winuiPinnedXamlControlsModule = nullptr;
HMODULE gs_winuiPinnedApplicationModelResourcesModule = nullptr;
HMODULE gs_winuiPinnedOwnerModule = nullptr;
const unsigned char gs_winuiOwnerModuleAnchor = 0;
wxString gs_winuiPinnedRuntimePackageFullName;
enum class wxWinUIRuntimeState : unsigned
{
    Idle,
    Initializing,
    Running,
    ShuttingDown,
    Quarantined
};
std::atomic<wxWinUIRuntimeState> gs_winuiRuntimeState{
    wxWinUIRuntimeState::Idle };
bool gs_winuiContentPreTranslateLogShown = false;
winrt::Microsoft::UI::Dispatching::DispatcherQueueController
    gs_winuiDispatcherQueueController{ nullptr };
winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager
    gs_winuiXamlManager{ nullptr };
winrt::Microsoft::UI::Xaml::Application gs_winuiApplication{ nullptr };
winrt::event_token gs_winuiXamlShutdownRetirementToken{};
bool gs_winuiXamlShutdownRetirementHookInstalled = false;
std::atomic<unsigned> gs_winuiFrameworkRetirementHookFaultForTesting{0};

// A quarantined XAML epoch must outlive both CRT static destruction and a
// possible shared-library unload. The holder itself is allocated only on the
// fail-closed path and is intentionally never deleted; moving the ordinary
// globals into it makes their own static destructors inert.
struct wxWinUIImmortalQuarantineEpoch final
{
    winrt::Microsoft::UI::Dispatching::DispatcherQueueController
        controller{ nullptr };
    winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager
        xamlManager{ nullptr };
    winrt::Microsoft::UI::Xaml::Application application{ nullptr };
    winrt::event_token xamlShutdownToken{};
    bool xamlShutdownHookInstalled = false;
    bool bootstrapOwned = false;
    bool comApartmentOwned = false;
    bool captured = false;
};

wxWinUIImmortalQuarantineEpoch& wxWinUIGetImmortalQuarantineEpoch()
{
    static wxWinUIImmortalQuarantineEpoch * const epoch =
        new wxWinUIImmortalQuarantineEpoch;
    return *epoch;
}

[[noreturn]] void wxWinUIFailFastQuarantine() noexcept
{
    // std::terminate() is user-replaceable and could start CRT teardown while
    // callback-bearing roots are still live. Fail-fast cannot be intercepted.
    ::RaiseFailFastException(nullptr, nullptr, 0);
    ::TerminateProcess(::GetCurrentProcess(), 0xC0000602u);
    for ( ;; )
    {
    }
}

void wxWinUIQuarantineRuntimeEpoch() noexcept;

bool wxWinUIPinOwnerModuleForQuarantine() noexcept
{
    if ( gs_winuiPinnedOwnerModule )
        return true;

    HMODULE module = nullptr;
    if ( !::GetModuleHandleExW(
             GET_MODULE_HANDLE_EX_FLAG_PIN |
                 GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
             reinterpret_cast<LPCWSTR>(&gs_winuiOwnerModuleAnchor),
             &module) ||
         !module )
    {
        return false;
    }

    gs_winuiPinnedOwnerModule = module;
    return true;
}

constexpr unsigned wxWinUIRejectFrameworkRetirementHook = 1u;
constexpr unsigned wxWinUIRejectShutdownRetirementHook = 2u;

// These faults are intentionally unreachable unless a dedicated subprocess
// supplies both exact environment variables. Consume and erase the guard at
// the very start of wxWinUI3Initialize(), before COM, the Windows App SDK or
// XAML can acquire any state. Keeping the enum private avoids turning failure
// injection into an installed-library ABI.
enum class wxWinUIRuntimeFaultForTesting
{
    None,
    AfterApplication,
    AfterXamlManager,
    RejectXamlShutdownHook,
    RejectRequiredQueueHook,
    ThrowShutdownQueue,
    SuppressFrameworkCompleted
};

wxWinUIRuntimeFaultForTesting wxWinUIConsumeRuntimeFaultForTesting() noexcept
{
    static std::atomic<bool> s_consumed{false};
    if ( s_consumed.exchange(true, std::memory_order_acq_rel) )
        return wxWinUIRuntimeFaultForTesting::None;

    constexpr wchar_t guardName[] = L"WX_WINUI_TEST_ONLY";
    constexpr wchar_t faultName[] =
        L"WX_WINUI_TEST_ONLY_RUNTIME_FAULT";

    wchar_t guard[4]{};
    wchar_t fault[64]{};
    const DWORD guardLength = ::GetEnvironmentVariableW(
        guardName, guard, static_cast<DWORD>(WXSIZEOF(guard)));
    const DWORD faultLength = ::GetEnvironmentVariableW(
        faultName, fault, static_cast<DWORD>(WXSIZEOF(fault)));

    // Erase both variables regardless of validity. An unknown or inherited
    // value is an inert one-shot and cannot affect a later initialization.
    const BOOL guardErased = ::SetEnvironmentVariableW(guardName, nullptr);
    const BOOL faultErased = ::SetEnvironmentVariableW(faultName, nullptr);

    if ( !guardErased || !faultErased ||
         guardLength != 1 || guard[0] != L'1' || !faultLength ||
         faultLength >= WXSIZEOF(fault) )
    {
        return wxWinUIRuntimeFaultForTesting::None;
    }

    if ( ::lstrcmpW(fault, L"after-application") == 0 )
        return wxWinUIRuntimeFaultForTesting::AfterApplication;
    if ( ::lstrcmpW(fault, L"after-xaml-manager") == 0 )
        return wxWinUIRuntimeFaultForTesting::AfterXamlManager;
    if ( ::lstrcmpW(fault, L"reject-xaml-hook") == 0 )
        return wxWinUIRuntimeFaultForTesting::RejectXamlShutdownHook;
    if ( ::lstrcmpW(fault, L"reject-required-queue-hook") == 0 )
        return wxWinUIRuntimeFaultForTesting::RejectRequiredQueueHook;
    if ( ::lstrcmpW(fault, L"throw-shutdown-queue") == 0 )
        return wxWinUIRuntimeFaultForTesting::ThrowShutdownQueue;
    if ( ::lstrcmpW(fault, L"suppress-framework-completed") == 0 )
        return wxWinUIRuntimeFaultForTesting::SuppressFrameworkCompleted;

    return wxWinUIRuntimeFaultForTesting::None;
}

bool wxWinUIFilesAreIdentical(
    const wxString& firstPath,
    const wxString& secondPath) noexcept
{
    const DWORD shareMode =
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    const HANDLE first = ::CreateFileW(
        firstPath.wc_str(),
        FILE_READ_ATTRIBUTES,
        shareMode,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if ( first == INVALID_HANDLE_VALUE )
        return false;

    const HANDLE second = ::CreateFileW(
        secondPath.wc_str(),
        FILE_READ_ATTRIBUTES,
        shareMode,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if ( second == INVALID_HANDLE_VALUE )
    {
        ::CloseHandle(first);
        return false;
    }

    BY_HANDLE_FILE_INFORMATION firstInfo{};
    BY_HANDLE_FILE_INFORMATION secondInfo{};
    const bool identical =
        ::GetFileInformationByHandle(first, &firstInfo) &&
        ::GetFileInformationByHandle(second, &secondInfo) &&
        firstInfo.dwVolumeSerialNumber == secondInfo.dwVolumeSerialNumber &&
        firstInfo.nFileIndexHigh == secondInfo.nFileIndexHigh &&
        firstInfo.nFileIndexLow == secondInfo.nFileIndexLow;

    ::CloseHandle(second);
    ::CloseHandle(first);
    return identical;
}

bool wxWinUIValidateLoadedRuntimeModules(
    const wxString& moduleName,
    const wxString& expectedModulePath) noexcept
{
    HANDLE snapshot = INVALID_HANDLE_VALUE;
    for ( unsigned attempt = 0; attempt < 3; ++attempt )
    {
        snapshot = ::CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            ::GetCurrentProcessId());
        if ( snapshot != INVALID_HANDLE_VALUE ||
             ::GetLastError() != ERROR_BAD_LENGTH )
        {
            break;
        }
    }
    if ( snapshot == INVALID_HANDLE_VALUE )
    {
        wxLogWarning("wxWinUI: failed to inspect loaded modules before "
                     "loading %s (error %lu)", moduleName,
                     ::GetLastError());
        return false;
    }

    bool valid = true;
    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if ( ::Module32FirstW(snapshot, &entry) )
    {
        bool reachedEnd = false;
        for ( ;; )
        {
            if ( wxString(entry.szModule).CmpNoCase(moduleName) == 0 &&
                 !wxWinUIFilesAreIdentical(
                     wxString(entry.szExePath), expectedModulePath) )
            {
                wxLogWarning("wxWinUI: refusing an already-loaded "
                             "%s outside the selected Windows App SDK "
                             "framework package", moduleName);
                valid = false;
                break;
            }

            if ( !::Module32NextW(snapshot, &entry) )
            {
                reachedEnd = ::GetLastError() == ERROR_NO_MORE_FILES;
                break;
            }
        }

        if ( valid && !reachedEnd )
        {
            wxLogWarning("wxWinUI: loaded-module enumeration ended "
                         "unexpectedly (error %lu)", ::GetLastError());
            valid = false;
        }
    }
    else
    {
        wxLogWarning("wxWinUI: failed to enumerate loaded modules before "
                     "loading %s (error %lu)", moduleName,
                     ::GetLastError());
        valid = false;
    }

    ::CloseHandle(snapshot);
    return valid;
}

bool wxWinUIPinRuntimeModule(
    const wxString& moduleName,
    const wxString& expectedModulePath,
    HMODULE& pinnedModule) noexcept
{
    if ( !wxWinUIValidateLoadedRuntimeModules(
             moduleName, expectedModulePath) )
    {
        return false;
    }

    HMODULE const module = ::LoadLibraryExW(
        expectedModulePath.wc_str(),
        nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
            LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if ( !module )
    {
        wxLogWarning("wxWinUI: failed to load %s for process-lifetime "
                     "pinning (error %lu)", moduleName, ::GetLastError());
        return false;
    }

    std::vector<wchar_t> loadedPath(32768);
    const DWORD loadedPathLength = ::GetModuleFileNameW(
        module,
        loadedPath.data(),
        static_cast<DWORD>(loadedPath.size()));
    const wxString loadedModulePath(loadedPath.data(), loadedPathLength);
    if ( !loadedPathLength || loadedPathLength == loadedPath.size() ||
         !wxWinUIFilesAreIdentical(
             loadedModulePath, expectedModulePath) )
    {
        ::FreeLibrary(module);
        wxLogWarning("wxWinUI: %s was resolved outside the selected Windows "
                     "App SDK framework package", moduleName);
        return false;
    }

    if ( pinnedModule )
    {
        const bool sameModule = module == pinnedModule;
        ::FreeLibrary(module);
        if ( !sameModule )
        {
            wxLogWarning("wxWinUI: a runtime epoch resolved a different %s "
                         "after it was pinned", moduleName);
        }
        return sameModule;
    }

    HMODULE pinned = nullptr;
    const BOOL didPin = ::GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_PIN |
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(module),
        &pinned);
    const DWORD pinError = didPin ? ERROR_SUCCESS : ::GetLastError();

    // Balance the explicit load. GET_MODULE_HANDLE_EX_FLAG_PIN makes the
    // selected module immune to this and all later FreeLibrary calls until
    // process termination.
    ::FreeLibrary(module);

    if ( !didPin || pinned != module )
    {
        wxLogWarning("wxWinUI: failed to pin %s (error %lu)",
                     moduleName, pinError);
        return false;
    }

    pinnedModule = pinned;
    return true;
}

bool wxWinUIPinXamlRuntimeModule() noexcept
{
    // MddBootstrapInitialize2() has added the selected framework package to
    // the process graph. Resolve that graph entry explicitly: a bare
    // LoadLibrary() name would still allow an app-directory/PATH DLL to win in
    // an unpackaged process.
    // Pinning the module, rather than leaving a COM apartment reference
    // unbalanced, keeps XAML Controls' process-global metadata destructors
    // from calling through vtables in an already-unloaded dependency when the
    // final UI apartment is closed.
    constexpr UINT32 packageInfoFlags =
        PACKAGE_FILTER_HEAD | PACKAGE_FILTER_DIRECT |
        PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC;
    UINT32 packageInfoBytes = 0;
    UINT32 packageCount = 0;
    LONG packageResult = ::GetCurrentPackageInfo(
        packageInfoFlags, &packageInfoBytes, nullptr, &packageCount);
    if ( packageResult != ERROR_INSUFFICIENT_BUFFER || !packageInfoBytes )
    {
        wxLogWarning("wxWinUI: failed to query the Windows App SDK package "
                     "graph (error %ld)", packageResult);
        return false;
    }

    std::vector<BYTE> packageInfoBuffer(packageInfoBytes);
    packageResult = ::GetCurrentPackageInfo(
        packageInfoFlags,
        &packageInfoBytes,
        packageInfoBuffer.data(),
        &packageCount);
    if ( packageResult != ERROR_SUCCESS )
    {
        wxLogWarning("wxWinUI: failed to read the Windows App SDK package "
                     "graph (error %ld)", packageResult);
        return false;
    }

    wxString expectedXamlPath;
    wxString expectedXamlControlsPath;
    wxString expectedApplicationModelResourcesPath;
    wxString expectedPackageFullName;
    const PACKAGE_INFO * const packages =
        reinterpret_cast<const PACKAGE_INFO *>(packageInfoBuffer.data());
    for ( UINT32 i = 0; i < packageCount; ++i )
    {
        const PACKAGE_INFO& package = packages[i];
        if ( !package.path || !package.packageFamilyName ||
             !package.packageFullName ||
             wxString(package.packageFamilyName).CmpNoCase(
                 WINDOWSAPPSDK_RUNTIME_PACKAGE_FRAMEWORK_PACKAGEFAMILYNAME_W) !=
                 0 )
        {
            continue;
        }

        expectedXamlPath = package.path;
        if ( !expectedXamlPath.EndsWith("\\") )
            expectedXamlPath += "\\";
        expectedXamlPath += "Microsoft.UI.Xaml.dll";
        expectedXamlControlsPath = package.path;
        if ( !expectedXamlControlsPath.EndsWith("\\") )
            expectedXamlControlsPath += "\\";
        expectedXamlControlsPath += "Microsoft.UI.Xaml.Controls.dll";
        expectedApplicationModelResourcesPath = package.path;
        if ( !expectedApplicationModelResourcesPath.EndsWith("\\") )
            expectedApplicationModelResourcesPath += "\\";
        expectedApplicationModelResourcesPath +=
            "Microsoft.Windows.ApplicationModel.Resources.dll";
        expectedPackageFullName = package.packageFullName;
        break;
    }

    if ( expectedXamlPath.empty() )
    {
        wxLogWarning("wxWinUI: the selected Windows App SDK framework "
                     "package is absent from the process graph");
        return false;
    }

    if ( (gs_winuiPinnedXamlModule ||
          gs_winuiPinnedXamlControlsModule ||
          gs_winuiPinnedApplicationModelResourcesModule) &&
         expectedPackageFullName.CmpNoCase(
             gs_winuiPinnedRuntimePackageFullName) != 0 )
    {
        wxLogWarning("wxWinUI: a runtime epoch selected a different "
                     "Windows App SDK package after XAML was pinned");
        return false;
    }

    // XamlControlsResources retains resource-manager COM objects until its
    // process-global destructors run. Load and pin their implementation before
    // XAML and Controls so MddBootstrapShutdown() cannot unmap their vtables
    // early and normal process detach runs Controls before this dependency.
    if ( !wxWinUIPinRuntimeModule(
             "Microsoft.Windows.ApplicationModel.Resources.dll",
             expectedApplicationModelResourcesPath,
             gs_winuiPinnedApplicationModelResourcesModule) )
    {
        return false;
    }

    if ( !wxWinUIPinRuntimeModule(
             "Microsoft.UI.Xaml.dll",
             expectedXamlPath,
             gs_winuiPinnedXamlModule) )
    {
        return false;
    }

    if ( gs_winuiPinnedRuntimePackageFullName.empty() )
        gs_winuiPinnedRuntimePackageFullName = expectedPackageFullName;

    // Controls registers custom metadata with Microsoft.UI.Xaml. If Controls
    // is released by the final COM rundown first, XAML's process destructor
    // later calls a stale Controls vtable. Keeping both modules alive lets
    // Controls unregister while XAML is still present during process rundown.
    if ( !wxWinUIPinRuntimeModule(
             "Microsoft.UI.Xaml.Controls.dll",
             expectedXamlControlsPath,
             gs_winuiPinnedXamlControlsModule) )
    {
        return false;
    }

    return true;
}

// ContentPreTranslateMessage() routes input (including character keys) to the
// focused WinUI island.  It lives in a Windows App SDK runtime DLL that is only
// on the search path after the bootstrapper has run, so it must be resolved
// dynamically rather than statically linked (a static import would make the
// process fail to start because the DLL is absent at load time).
using wxWinUIContentPreTranslateMessage = BOOL (WINAPI *)(const MSG *);
wxWinUIContentPreTranslateMessage gs_winuiContentPreTranslateMessage = nullptr;
bool gs_winuiForceEventLoopFallbackForTesting = false;
bool gs_winuiKeyboardModifiersOverriddenForTesting = false;
wxWinUIKeyboardModifiers gs_winuiKeyboardModifiersForTesting =
    { false, false, false, false };

bool wxWinUIIsKeyboardMessage(const MSG *msg)
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
            return true;
    }

    return false;
}

bool wxWinUIIsTabMessage(const MSG *msg)
{
    return msg &&
           msg->message == WM_KEYDOWN &&
           msg->wParam == VK_TAB;
}

wxWinUIKeyboardModifiers wxWinUIGetKeyboardModifiers()
{
    if ( gs_winuiKeyboardModifiersOverriddenForTesting )
        return gs_winuiKeyboardModifiersForTesting;

    return
    {
        (::GetKeyState(VK_SHIFT) & 0x8000) != 0,
        (::GetKeyState(VK_CONTROL) & 0x8000) != 0,
        (::GetKeyState(VK_LMENU) & 0x8000) != 0,
        (::GetKeyState(VK_RMENU) & 0x8000) != 0
    };
}

wxWinUIKeyboardPipelineResult wxWinUIRunKeyboardPipelineImpl(
    MSG *msg,
    wxWindow *logicalOwner,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    if ( !msg || !logicalOwner || !wxWinUIIsKeyboardMessage(msg) )
    {
        return wxWinUIKeyboardPipelineResult::NotApplicable;
    }

    // wxEVT_CHAR_HOOK normally suppresses the key at WH_KEYBOARD time. With
    // an active IME Windows must still receive it, so wxMSW records the key
    // here for deferred suppression. Honour that veto before either the wx
    // accelerator table or the island can observe the message.
    if ( wxWinUI3ConsumeBlockedKeyboardMessage(
            reinterpret_cast<WXMSG *>(msg)) )
    {
        return wxWinUIKeyboardPipelineResult::WxHandled;
    }

    // Match wxGUIEventLoop::PreProcessMessage(): keyboard input for a window
    // outside the current critical subtree must be eaten, not dispatched into
    // the island behind a modal/assert window.
    if ( !wxWinUI3AllowMessageProcessing(logicalOwner) )
        return wxWinUIKeyboardPipelineResult::WxHandled;

    const bool isKeyDown =
        msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;
    const bool isMnemonic = msg->message == WM_SYSCHAR;
    const bool isTab = wxWinUIIsTabMessage(msg);

    if ( (isKeyDown && !isTab) || isMnemonic )
    {
        // AltGr is reported by Windows as Ctrl+right-Alt. Running the wx
        // accelerator or mnemonic tables for the following key would steal
        // printable characters on layouts which use AltGr.
        const bool isAltGr =
            modifiers.controlDown && modifiers.rightAltDown;

        if ( !isAltGr &&
                logicalOwner->MSWShouldPreProcessMessage(
                    reinterpret_cast<WXMSG *>(msg)) )
        {
            wxWindow *window = logicalOwner;
            while ( window )
            {
                const bool handled =
                    isKeyDown
                        ? window->MSWTranslateMessage(
                              reinterpret_cast<WXMSG *>(msg))
                        // Keep mnemonic handling deliberately narrow. Calling
                        // MSWProcessMessage() for arbitrary island input would
                        // reintroduce IsDialogMessage() swallowing printable
                        // characters; WM_SYSCHAR is its mnemonic-only path.
                        : window->MSWProcessMessage(
                              reinterpret_cast<WXMSG *>(msg));

                // TranslateAccelerator() can synchronously dispatch a command
                // which destroys the frame/slot, so never touch the raw
                // window again after a successful translation.
                if ( handled )
                    return wxWinUIKeyboardPipelineResult::WxHandled;

                if ( window->IsTopNavigationDomain(
                        wxWindow::Navigation_Accel) )
                {
                    break;
                }

                window = window->GetParent();
            }
        }
    }

    if ( xamlPreTranslate &&
            xamlPreTranslate(reinterpret_cast<WXMSG *>(msg), xamlContext) )
    {
        return wxWinUIKeyboardPipelineResult::XamlHandled;
    }

    return wxWinUIKeyboardPipelineResult::NotHandled;
}

struct wxWinUIApplication :
    winrt::Microsoft::UI::Xaml::ApplicationT
    <
        wxWinUIApplication,
        winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider
    >
{
    wxWinUIApplication() = default;

    void OnLaunched(
        winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
    }

    winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(
        winrt::Windows::UI::Xaml::Interop::TypeName const& type)
    {
        return m_provider.GetXamlType(type);
    }

    winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(
        winrt::hstring const& fullName)
    {
        return m_provider.GetXamlType(fullName);
    }

    winrt::com_array<winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition>
    GetXmlnsDefinitions()
    {
        return m_provider.GetXmlnsDefinitions();
    }

private:
    winrt::Microsoft::UI::Xaml::XamlTypeInfo::XamlControlsXamlMetaDataProvider
        m_provider;
};

void wxWinUILogHresult(const char *what, HRESULT hr)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx.",
                 what,
                 static_cast<unsigned long>(hr));
}

void wxWinUIBootstrapLogException(
    const char *what,
    const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
}

// DispatcherQueue rundown has two distinct facts which controls must not
// conflate: TryEnqueue() may reject new work, while the framework can still be
// draining work already accepted by XAML. Before rundown, a normal dispatcher
// barrier may consume a ticket. From the synchronous rundown gate onwards,
// Complete() is deliberately inert: XamlShutdownCompletedOnThread is the sole
// release boundary and holds the queue's deferral while destroying every
// retained graph.
class wxWinUIFrameworkRetirementRuntime final
{
public:
    static wxWinUIFrameworkRetirementRuntime& Get()
    {
        // The ledger can own callbacks and WinRT queue projections after a
        // failed shutdown. It must never participate in CRT static teardown.
        static wxWinUIFrameworkRetirementRuntime * const runtime =
            new wxWinUIFrameworkRetirementRuntime;
        return *runtime;
    }

    wxWinUIFrameworkRetirementId Register(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        std::function<void ()> complete) noexcept
    {
        if ( !complete )
            return 0;

        std::uint64_t generation = 0;
        try
        {
            if ( queue && !m_runtimeTerminal && !m_xamlTerminal )
            {
                try
                {
                    generation = m_rundown
                        ? FindQueueGeneration(queue)
                        : EnsureQueueGeneration(queue);
                }
                catch ( const winrt::hresult_error& e )
                {
                    // Keep a generation-zero terminal quarantine even when
                    // the dispatcher is already too far into rundown to
                    // accept the framework hook.
                    wxWinUIBootstrapLogException(
                        "WinUI framework shutdown hook registration", e);
                }
                catch ( const std::exception& )
                {
                }
                catch ( ... )
                {
                }
            }

            const wxWinUIFrameworkRetirementId id = NextId();
            m_entries.push_back(
                RetirementEntry{id, generation, std::move(complete)});

            // Reentrant registration while the XAML terminal callback is
            // actively holding its deferral is still inside the only safe
            // release window. After that window, terminal registrations stay
            // quarantined and make post-shutdown validation fail closed.
            if ( m_xamlCompletionActive )
                Complete(id);

            return id;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUIBootstrapLogException(
                "WinUI framework retirement registration", e);
        }
        catch ( const std::exception& )
        {
        }
        catch ( ... )
        {
        }

        RemoveQueueGenerationIfIdle(generation);

        // Registration happens before the caller detaches its XAML graph, so
        // it can fail closed without releasing that graph in the detach stack.
        return 0;
    }

    void Complete(wxWinUIFrameworkRetirementId id) noexcept
    {
        if ( !id )
            return;

        if ( m_rundown && !m_xamlCompletionActive )
            return;

        std::function<void ()> complete;
        std::uint64_t generation = 0;
        for ( auto it = m_entries.begin(); it != m_entries.end(); ++it )
        {
            if ( it->id != id )
                continue;

            generation = it->generation;
            complete = std::move(it->complete);
            m_entries.erase(it);
            break;
        }

        Invoke(std::move(complete));
        RemoveQueueGenerationIfIdle(generation);
    }

    bool BeginRuntime(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        wxWinUIRuntimeFaultForTesting fault =
            wxWinUIRuntimeFaultForTesting::None)
        noexcept
    {
        if ( m_quarantined ||
             gs_winuiRuntimeState.load(std::memory_order_acquire) ==
                 wxWinUIRuntimeState::Quarantined )
        {
            return false;
        }

        // A later wx initialization may start only after the previous XAML
        // deferral consumed every graph. Never run a leftover callback after
        // DispatcherQueue shutdown has returned.
        if ( m_runtimeTerminal || m_xamlTerminal )
        {
            if ( !m_entries.empty() )
                return false;

            RevokeAndClearQueues();
            ResetEpochFlags();
        }

        if ( !queue )
            return false;

        try
        {
            const std::uint64_t generation =
                EnsureQueueGeneration(queue, true, fault);
            return HasAllQueueHooks(generation);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUIBootstrapLogException(
                "WinUI early framework retirement hook", e);
        }
        catch ( const std::exception& )
        {
        }
        catch ( ... )
        {
        }

        return false;
    }

    void BeginRundownBeforeShutdown() noexcept
    {
        // This synchronous gate closes the interval between wx publishing
        // ShuttingDown and DispatcherQueue raising ShutdownStarting.
        m_rundown = true;
    }

    void CompleteAtXamlBoundary(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr,
        bool releaseApplication = false) noexcept
    {
        if ( m_quarantined || m_xamlTerminal )
            return;

        m_rundown = true;
        m_xamlTerminal = true;
        m_xamlCompletionActive = true;

        for ( auto& state : m_queues )
        {
            if ( !state.shutdownStartingSeen )
                m_phaseOrderValid = false;
            state.xamlCompletedSeen = true;
        }

        // Publish the terminal phase before arbitrary graph destruction can
        // re-enter registration. Every release below is protected by the
        // XamlShutdownCompletedOnThread DispatcherQueue deferral owned by the
        // caller.
        InvokeHook(hook, context);
        CompleteAllEntries();

        // Application::Current() is itself a XAML root. Release it only after
        // all peer graphs, while the same deferral still keeps the queue live.
        if ( releaseApplication )
            gs_winuiApplication = nullptr;
        CompleteAllEntries();

        m_xamlCompletionActive = false;
    }

    void InvalidatePhaseOrder() noexcept
    {
        m_phaseOrderValid = false;
    }

    void Quarantine() noexcept
    {
        // Complete() and every later XAML boundary are permanently inert for
        // this epoch. Entries and queue handlers remain owned by the immortal
        // ledger until process termination.
        m_rundown = true;
        m_xamlCompletionActive = false;
        m_quarantined = true;
    }

    bool ValidateAfterShutdown() noexcept
    {
        if ( m_quarantined )
            return false;

        m_runtimeTerminal = true;

        if ( !m_phaseOrderValid || !m_rundown || !m_xamlTerminal ||
             m_xamlCompletionActive || !m_entries.empty() ||
             m_queues.empty() )
        {
            return false;
        }

        const bool valid = std::all_of(
            m_queues.begin(), m_queues.end(),
            [](const QueueGeneration& state)
            {
                return state.shutdownStartingSeen &&
                       state.frameworkStartingSeen &&
                       state.xamlCompletedSeen &&
                       state.frameworkCompletedSeen &&
                       state.shutdownCompletedSeen;
            });
        if ( valid )
        {
            // Drop the now-closed queue while COM and the Windows App SDK
            // bootstrap are still alive. Invalid state is retained strongly
            // for quarantine diagnostics and safety.
            RevokeAndClearQueues();
        }
        return valid;
    }

    bool IsTerminalLedgerEmpty() const noexcept
    {
        return m_runtimeTerminal && !m_xamlCompletionActive &&
               m_entries.empty();
    }

    void SimulateShutdownStartingForTesting(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        SimulatePhaseForTesting(
            QueuePhase::ShutdownStarting, hook, context);
    }

    void SimulateFrameworkShutdownStartingForTesting(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        SimulatePhaseForTesting(
            QueuePhase::FrameworkStarting, hook, context);
    }

    void SimulateFrameworkShutdownCompletedForTesting(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        SimulatePhaseForTesting(
            QueuePhase::FrameworkCompleted, hook, context);
    }

    void SimulateShutdownCompletedForTesting(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        SimulatePhaseForTesting(
            QueuePhase::ShutdownCompleted, hook, context);
    }

    void ResetForTesting(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue)
        noexcept
    {
        if ( m_quarantined )
            return;

        // Test-only cleanup models the XAML deferral before arming a fresh
        // generation on the still-live process dispatcher.
        m_rundown = true;
        m_xamlCompletionActive = true;
        CompleteAllEntries();
        m_xamlCompletionActive = false;
        RevokeAndClearQueues();
        ResetEpochFlags();

        if ( !queue )
            return;

        try
        {
            (void)EnsureQueueGeneration(queue, true);
        }
        catch ( ... )
        {
        }
    }

    std::size_t GetPendingCountForTesting() const noexcept
    {
        return m_entries.size();
    }

    wxWinUIFrameworkRetirementSnapshotForTesting
    GetSnapshotForTesting() const noexcept
    {
        wxWinUIFrameworkRetirementSnapshotForTesting snapshot;
        snapshot.entries = m_entries.size();
        snapshot.queueStates = m_queues.size();
        snapshot.rundown = m_rundown;
        snapshot.xamlTerminal = m_xamlTerminal;
        snapshot.xamlCompletionActive = m_xamlCompletionActive;
        snapshot.runtimeTerminal = m_runtimeTerminal;
        snapshot.phaseOrderValid = m_phaseOrderValid;

        for ( const auto& entry : m_entries )
        {
            if ( !entry.generation )
                ++snapshot.unboundEntries;
        }

        for ( const auto& state : m_queues )
        {
            if ( state.shutdownStartingHookInstalled )
                ++snapshot.shutdownStartingHooks;
            if ( state.frameworkStartingHookInstalled )
                ++snapshot.frameworkStartingHooks;
            if ( state.frameworkHookInstalled )
                ++snapshot.frameworkHooks;
            if ( state.shutdownHookInstalled )
                ++snapshot.shutdownHooks;
            if ( state.shutdownStartingSeen )
                ++snapshot.shutdownStartingStates;
            if ( state.frameworkStartingSeen )
                ++snapshot.frameworkStartingStates;
            if ( state.xamlCompletedSeen )
                ++snapshot.xamlCompletedStates;
            if ( state.frameworkCompletedSeen )
                ++snapshot.frameworkDoneStates;
            if ( state.shutdownCompletedSeen )
                ++snapshot.shutdownDoneStates;
            if ( !state.shutdownStartingSeen )
                ++snapshot.activeStates;
        }

        return snapshot;
    }

private:
    enum class QueuePhase
    {
        ShutdownStarting,
        FrameworkStarting,
        FrameworkCompleted,
        ShutdownCompleted
    };

    struct QueueGeneration
    {
        winrt::Microsoft::UI::Dispatching::DispatcherQueue queue{ nullptr };
        std::uint64_t generation = 0;
        winrt::event_token shutdownStartingToken{};
        winrt::event_token frameworkStartingToken{};
        winrt::event_token frameworkToken{};
        winrt::event_token shutdownToken{};
        bool shutdownStartingHookInstalled = false;
        bool frameworkStartingHookInstalled = false;
        bool frameworkHookInstalled = false;
        bool shutdownHookInstalled = false;
        bool persistent = false;
        bool shutdownStartingSeen = false;
        bool frameworkStartingSeen = false;
        bool xamlCompletedSeen = false;
        bool frameworkCompletedSeen = false;
        bool shutdownCompletedSeen = false;
    };

    struct RetirementEntry
    {
        wxWinUIFrameworkRetirementId id = 0;
        std::uint64_t generation = 0;
        std::function<void ()> complete;
    };

    wxWinUIFrameworkRetirementId NextId() noexcept
    {
        ++m_nextId;
        if ( !m_nextId )
            ++m_nextId;
        return m_nextId;
    }

    std::uint64_t EnsureQueueGeneration(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        bool persistent = false,
        wxWinUIRuntimeFaultForTesting runtimeFault =
            wxWinUIRuntimeFaultForTesting::None)
    {
        for ( auto& state : m_queues )
        {
            if ( state.queue == queue )
            {
                state.persistent = state.persistent || persistent;
                return state.generation;
            }
        }

        QueueGeneration state;
        state.queue = queue;
        state.persistent = persistent;
        state.generation = ++m_nextGeneration;
        if ( !state.generation )
            state.generation = ++m_nextGeneration;

        const std::uint64_t generation = state.generation;
        const unsigned hookFault =
            gs_winuiFrameworkRetirementHookFaultForTesting.load(
                std::memory_order_acquire);

        try
        {
            state.shutdownStartingToken = queue.ShutdownStarting(
                [generation](
                    const winrt::Microsoft::UI::Dispatching::DispatcherQueue&,
                    const winrt::Microsoft::UI::Dispatching::
                        DispatcherQueueShutdownStartingEventArgs&) noexcept
                {
                    wxWinUIFrameworkRetirementRuntime::Get().ObservePhase(
                        generation,
                        QueuePhase::ShutdownStarting,
                        false,
                        nullptr,
                        nullptr);
                });
            state.shutdownStartingHookInstalled = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUIBootstrapLogException(
                "WinUI ShutdownStarting retirement hook", e);
        }

        try
        {
            state.frameworkStartingToken = queue.FrameworkShutdownStarting(
                [generation](
                    const winrt::Microsoft::UI::Dispatching::DispatcherQueue&,
                    const winrt::Microsoft::UI::Dispatching::
                        DispatcherQueueShutdownStartingEventArgs&) noexcept
                {
                    wxWinUIFrameworkRetirementRuntime::Get().ObservePhase(
                        generation,
                        QueuePhase::FrameworkStarting,
                        false,
                        nullptr,
                        nullptr);
                });
            state.frameworkStartingHookInstalled = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUIBootstrapLogException(
                "WinUI FrameworkShutdownStarting retirement hook", e);
        }

        const bool rejectFrameworkHook =
            (hookFault & wxWinUIRejectFrameworkRetirementHook) != 0 ||
            runtimeFault ==
                wxWinUIRuntimeFaultForTesting::RejectRequiredQueueHook;
        const bool suppressFrameworkCompleted =
            runtimeFault ==
                wxWinUIRuntimeFaultForTesting::SuppressFrameworkCompleted;

        if ( !rejectFrameworkHook )
        {
            try
            {
                state.frameworkToken = queue.FrameworkShutdownCompleted(
                    [generation, suppressFrameworkCompleted](
                        const winrt::Microsoft::UI::Dispatching::
                            DispatcherQueue&,
                        const winrt::Windows::Foundation::IInspectable&)
                        noexcept
                    {
                        if ( suppressFrameworkCompleted )
                            return;
                        wxWinUIFrameworkRetirementRuntime::Get()
                            .ObservePhase(
                                generation,
                                QueuePhase::FrameworkCompleted,
                                false,
                                nullptr,
                                nullptr);
                    });
                state.frameworkHookInstalled = true;
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUIBootstrapLogException(
                    "WinUI FrameworkShutdownCompleted hook", e);
            }
        }

        if ( !(hookFault & wxWinUIRejectShutdownRetirementHook) )
        {
            try
            {
                state.shutdownToken = queue.ShutdownCompleted(
                    [generation](
                        const winrt::Microsoft::UI::Dispatching::
                            DispatcherQueue&,
                        const winrt::Windows::Foundation::IInspectable&)
                        noexcept
                    {
                        wxWinUIFrameworkRetirementRuntime::Get()
                            .ObservePhase(
                                generation,
                                QueuePhase::ShutdownCompleted,
                                false,
                                nullptr,
                                nullptr);
                    });
                state.shutdownHookInstalled = true;
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUIBootstrapLogException(
                    "WinUI ShutdownCompleted retirement hook", e);
            }
        }

        try
        {
            m_queues.push_back(state);
        }
        catch ( ... )
        {
            RevokeShutdownStartingHook(state);
            RevokeFrameworkStartingHook(state);
            RevokeFrameworkHook(state);
            RevokeShutdownHook(state);
            throw;
        }
        return generation;
    }

    std::uint64_t FindQueueGeneration(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue) const
        noexcept
    {
        const auto it = std::find_if(
            m_queues.begin(), m_queues.end(),
            [&queue](const QueueGeneration& state)
            {
                return state.queue == queue;
            });
        return it == m_queues.end() ? 0 : it->generation;
    }

    bool HasAllQueueHooks(std::uint64_t generation) const noexcept
    {
        const auto it = std::find_if(
            m_queues.begin(), m_queues.end(),
            [generation](const QueueGeneration& state)
            {
                return state.generation == generation;
            });
        return it != m_queues.end() &&
               it->shutdownStartingHookInstalled &&
               it->frameworkStartingHookInstalled &&
               it->frameworkHookInstalled &&
               it->shutdownHookInstalled;
    }

    void ObservePhase(
        std::uint64_t generation,
        QueuePhase phase,
        bool synthetic,
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        auto stateIt = std::find_if(
            m_queues.begin(), m_queues.end(),
            [generation](const QueueGeneration& state)
            {
                return state.generation == generation;
            });
        if ( stateIt == m_queues.end() )
            return;

        switch ( phase )
        {
            case QueuePhase::ShutdownStarting:
                if ( !stateIt->shutdownStartingHookInstalled ||
                     stateIt->shutdownStartingSeen )
                    return;
                stateIt->shutdownStartingSeen = true;
                m_rundown = true;
                if ( synthetic )
                    RevokeShutdownStartingHook(*stateIt);
                else
                {
                    stateIt->shutdownStartingHookInstalled = false;
                    stateIt->shutdownStartingToken = {};
                }
                break;

            case QueuePhase::FrameworkStarting:
                if ( !stateIt->frameworkStartingHookInstalled ||
                     stateIt->frameworkStartingSeen )
                    return;
                stateIt->frameworkStartingSeen = true;
                if ( !stateIt->shutdownStartingSeen )
                    m_phaseOrderValid = false;
                if ( synthetic )
                    RevokeFrameworkStartingHook(*stateIt);
                else
                {
                    stateIt->frameworkStartingHookInstalled = false;
                    stateIt->frameworkStartingToken = {};
                }
                break;

            case QueuePhase::FrameworkCompleted:
                if ( !stateIt->frameworkHookInstalled ||
                     stateIt->frameworkCompletedSeen )
                    return;
                stateIt->frameworkCompletedSeen = true;
                if ( !stateIt->shutdownStartingSeen ||
                     !stateIt->frameworkStartingSeen ||
                     !stateIt->xamlCompletedSeen )
                {
                    m_phaseOrderValid = false;
                }
                if ( synthetic )
                    RevokeFrameworkHook(*stateIt);
                else
                {
                    stateIt->frameworkHookInstalled = false;
                    stateIt->frameworkToken = {};
                }
                break;

            case QueuePhase::ShutdownCompleted:
                if ( !stateIt->shutdownHookInstalled ||
                     stateIt->shutdownCompletedSeen )
                    return;
                stateIt->shutdownCompletedSeen = true;
                if ( !stateIt->frameworkCompletedSeen )
                    m_phaseOrderValid = false;
                if ( synthetic )
                    RevokeShutdownHook(*stateIt);
                else
                {
                    stateIt->shutdownHookInstalled = false;
                    stateIt->shutdownToken = {};
                }
                break;
        }

        // Queue phases publish facts only. In particular, neither completion
        // event destroys callback-bearing XAML graphs.
        InvokeHook(hook, context);
    }

    void RemoveQueueGenerationIfIdle(std::uint64_t generation) noexcept
    {
        if ( !generation ||
             std::any_of(
                 m_entries.begin(), m_entries.end(),
                 [generation](const RetirementEntry& entry)
                 {
                     return entry.generation == generation;
                 }) )
        {
            return;
        }

        const auto it = std::find_if(
            m_queues.begin(), m_queues.end(),
            [generation](const QueueGeneration& state)
            {
                return state.generation == generation;
            });
        if ( it == m_queues.end() )
            return;

        if ( it->persistent || m_rundown )
            return;

        RevokeShutdownStartingHook(*it);
        RevokeFrameworkStartingHook(*it);
        RevokeFrameworkHook(*it);
        RevokeShutdownHook(*it);
        m_queues.erase(it);
    }

    void SimulatePhaseForTesting(
        QueuePhase phase,
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        try
        {
            std::vector<std::uint64_t> generations;
            generations.reserve(m_queues.size());
            for ( const auto& state : m_queues )
            {
                bool hasRequestedHook = false;
                switch ( phase )
                {
                    case QueuePhase::ShutdownStarting:
                        hasRequestedHook =
                            state.shutdownStartingHookInstalled &&
                            !state.shutdownStartingSeen;
                        break;
                    case QueuePhase::FrameworkStarting:
                        hasRequestedHook =
                            state.frameworkStartingHookInstalled &&
                            !state.frameworkStartingSeen;
                        break;
                    case QueuePhase::FrameworkCompleted:
                        hasRequestedHook = state.frameworkHookInstalled &&
                                           !state.frameworkCompletedSeen;
                        break;
                    case QueuePhase::ShutdownCompleted:
                        hasRequestedHook = state.shutdownHookInstalled &&
                                           !state.shutdownCompletedSeen;
                        break;
                }
                if ( hasRequestedHook )
                    generations.push_back(state.generation);
            }

            bool hookInvoked = false;
            for ( const std::uint64_t generation : generations )
            {
                ObservePhase(
                    generation,
                    phase,
                    true,
                    hookInvoked ? nullptr : hook,
                    context);
                hookInvoked = hookInvoked || hook != nullptr;
            }
        }
        catch ( ... )
        {
        }
    }

    void RevokeAndClearQueues() noexcept
    {
        for ( auto& state : m_queues )
        {
            RevokeShutdownStartingHook(state);
            RevokeFrameworkStartingHook(state);
            RevokeFrameworkHook(state);
            RevokeShutdownHook(state);
        }
        m_queues.clear();
    }

    static void RevokeShutdownStartingHook(QueueGeneration& state) noexcept
    {
        if ( !state.shutdownStartingHookInstalled )
            return;

        state.shutdownStartingHookInstalled = false;
        const winrt::event_token token = state.shutdownStartingToken;
        state.shutdownStartingToken = {};
        if ( !state.queue )
            return;

        try
        {
            state.queue.ShutdownStarting(token);
        }
        catch ( ... )
        {
        }
    }

    static void RevokeFrameworkStartingHook(QueueGeneration& state) noexcept
    {
        if ( !state.frameworkStartingHookInstalled )
            return;

        state.frameworkStartingHookInstalled = false;
        const winrt::event_token token = state.frameworkStartingToken;
        state.frameworkStartingToken = {};
        if ( !state.queue )
            return;

        try
        {
            state.queue.FrameworkShutdownStarting(token);
        }
        catch ( ... )
        {
        }
    }

    static void RevokeFrameworkHook(QueueGeneration& state) noexcept
    {
        if ( !state.frameworkHookInstalled )
            return;

        // Publish revocation before crossing the ABI. A queue already in
        // rundown can report ERROR_NOT_FOUND for a token whose terminal event
        // raced us; that is already the state we need and must never escape a
        // noexcept shutdown path.
        state.frameworkHookInstalled = false;
        const winrt::event_token token = state.frameworkToken;
        state.frameworkToken = {};
        if ( !state.queue )
            return;

        try
        {
            state.queue.FrameworkShutdownCompleted(token);
        }
        catch ( ... )
        {
        }
    }

    static void RevokeShutdownHook(QueueGeneration& state) noexcept
    {
        if ( !state.shutdownHookInstalled )
            return;

        state.shutdownHookInstalled = false;
        const winrt::event_token token = state.shutdownToken;
        state.shutdownToken = {};
        if ( !state.queue )
            return;

        try
        {
            state.queue.ShutdownCompleted(token);
        }
        catch ( ... )
        {
        }
    }

    void CompleteAllEntries() noexcept
    {
        while ( !m_entries.empty() )
        {
            std::function<void ()> complete =
                std::move(m_entries.back().complete);
            m_entries.pop_back();
            Invoke(std::move(complete));
        }
    }

    void ResetEpochFlags() noexcept
    {
        m_rundown = false;
        m_xamlTerminal = false;
        m_xamlCompletionActive = false;
        m_runtimeTerminal = false;
        m_phaseOrderValid = true;
    }

    static void Invoke(std::function<void ()> complete) noexcept
    {
        if ( !complete )
            return;

        try
        {
            complete();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        catch ( const std::exception& )
        {
        }
        catch ( ... )
        {
        }
    }

    static void InvokeHook(
        wxWinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context) noexcept
    {
        if ( !hook )
            return;

        try
        {
            hook(context);
        }
        catch ( ... )
        {
        }
    }

    std::vector<QueueGeneration> m_queues;
    std::vector<RetirementEntry> m_entries;
    wxWinUIFrameworkRetirementId m_nextId = 0;
    std::uint64_t m_nextGeneration = 0;
    bool m_rundown = false;
    bool m_xamlTerminal = false;
    bool m_xamlCompletionActive = false;
    bool m_runtimeTerminal = false;
    bool m_phaseOrderValid = true;
    bool m_quarantined = false;
};

bool wxWinUIPrepareImmortalRuntimeStorage() noexcept
{
    try
    {
        // Force both process-lifetime allocations before COM, MddBootstrap or
        // XAML can acquire external state. Quarantine itself then allocates
        // nothing and is safe to enter from a noexcept shutdown callback.
        (void)wxWinUIGetImmortalQuarantineEpoch();
        (void)wxWinUIFrameworkRetirementRuntime::Get();
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

void wxWinUIQuarantineRuntimeEpoch() noexcept
{
    // Queue and XAML handlers retain lambdas whose code lives in this module.
    // If the application unloads a shared wx library after a failed rundown,
    // retaining just the WinRT projections would leave dangling callbacks.
    // A pin failure therefore has no recoverable safe continuation.
    if ( !wxWinUIPinOwnerModuleForQuarantine() )
        wxWinUIFailFastQuarantine();

    // Every caller exits its initialization/shutdown path without balancing
    // any owned MddBootstrap or COM-apartment reference. Those ownership facts
    // are transferred into the same bounded immortal epoch as the projections.

    wxWinUIImmortalQuarantineEpoch& epoch =
        wxWinUIGetImmortalQuarantineEpoch();
    if ( epoch.captured )
    {
        // Runtime state rejects a second epoch. Seeing newly populated source
        // globals here would mean that invariant was violated; overwriting the
        // first immortal graph would release it and is never safe.
        if ( gs_winuiDispatcherQueueController || gs_winuiXamlManager ||
             gs_winuiApplication ||
             gs_winuiXamlShutdownRetirementHookInstalled ||
             gs_winuiXamlShutdownRetirementToken.value )
        {
            wxWinUIFailFastQuarantine();
        }
    }
    else
    {
        epoch.controller = std::move(gs_winuiDispatcherQueueController);
        epoch.xamlManager = std::move(gs_winuiXamlManager);
        epoch.application = std::move(gs_winuiApplication);
        epoch.xamlShutdownToken = std::exchange(
            gs_winuiXamlShutdownRetirementToken, winrt::event_token{});
        epoch.xamlShutdownHookInstalled = std::exchange(
            gs_winuiXamlShutdownRetirementHookInstalled, false);
        epoch.captured = true;
    }

    const bool publishedBootstrapOwned =
        std::exchange(gs_winuiBootstrapInitialized, false);
    const bool publishedComApartmentOwned =
        std::exchange(gs_winuiComInitialized, false);
    epoch.bootstrapOwned =
        epoch.bootstrapOwned || publishedBootstrapOwned;
    epoch.comApartmentOwned = epoch.comApartmentOwned ||
                              publishedComApartmentOwned;

    // Publish Quarantined only after the module pin and all moves are visible:
    // any observer of this state can rely on static destruction being inert.
    wxWinUIFrameworkRetirementRuntime::Get().Quarantine();
    gs_winuiRuntimeState.store(
        wxWinUIRuntimeState::Quarantined, std::memory_order_release);
}

bool wxWinUIEnsureXamlShutdownRetirementHook(
    wxWinUIRuntimeFaultForTesting fault =
        wxWinUIRuntimeFaultForTesting::None) noexcept
{
    if ( fault == wxWinUIRuntimeFaultForTesting::RejectXamlShutdownHook )
        return false;

    if ( gs_winuiXamlShutdownRetirementHookInstalled )
        return true;

    if ( !gs_winuiXamlManager )
        return false;

    try
    {
        gs_winuiXamlShutdownRetirementToken =
            gs_winuiXamlManager.XamlShutdownCompletedOnThread(
                [](const winrt::Microsoft::UI::Xaml::Hosting::
                       WindowsXamlManager&,
                   const winrt::Microsoft::UI::Xaml::Hosting::
                       XamlShutdownCompletedOnThreadEventArgs& args) noexcept
                {
                    try
                    {
                        const auto deferral =
                            args.GetDispatcherQueueDeferral();
                        if ( !deferral )
                        {
                            wxLogWarning("WinUI XAML shutdown completion did "
                                         "not provide a DispatcherQueue "
                                         "deferral");
                            wxWinUIFrameworkRetirementRuntime::Get()
                                .InvalidatePhaseOrder();
                            wxWinUIQuarantineRuntimeEpoch();
                            return;
                        }
                        wxWinUIFrameworkRetirementRuntime::Get()
                            .CompleteAtXamlBoundary(
                                nullptr, nullptr, true);
                        deferral.Complete();
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUIBootstrapLogException(
                            "WinUI XAML shutdown retirement deferral", e);
                        wxWinUIFrameworkRetirementRuntime::Get()
                            .InvalidatePhaseOrder();
                        wxWinUIQuarantineRuntimeEpoch();
                    }
                    catch ( ... )
                    {
                        wxLogWarning("WinUI XAML shutdown retirement deferral "
                                     "raised a non-WinRT exception");
                        wxWinUIFrameworkRetirementRuntime::Get()
                            .InvalidatePhaseOrder();
                        wxWinUIQuarantineRuntimeEpoch();
                    }
                });
        gs_winuiXamlShutdownRetirementHookInstalled = true;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException(
            "WinUI XamlShutdownCompletedOnThread retirement hook", e);
    }
    catch ( const std::exception& )
    {
    }
    catch ( ... )
    {
    }

    gs_winuiXamlShutdownRetirementToken = {};
    gs_winuiXamlShutdownRetirementHookInstalled = false;
    return false;
}

bool wxWinUIArmFrameworkRetirementRuntime(
    wxWinUIRuntimeFaultForTesting fault =
        wxWinUIRuntimeFaultForTesting::None) noexcept
{
    bool queueHooksInstalled = false;
    try
    {
        const auto queue =
            winrt::Microsoft::UI::Dispatching::DispatcherQueue::
                GetForCurrentThread();
        queueHooksInstalled =
            wxWinUIFrameworkRetirementRuntime::Get().BeginRuntime(
                queue, fault);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException(
            "WinUI early retirement DispatcherQueue lookup", e);
    }
    catch ( ... )
    {
    }

    // Still try to arm the XAML hook after a partial queue-hook failure: it is
    // required to release any graph retained by causal initialization cleanup.
    const bool xamlHookInstalled =
        wxWinUIEnsureXamlShutdownRetirementHook(fault);
    return queueHooksInstalled && xamlHookInstalled;
}

wxString wxWinUIGetModuleDirectory()
{
    wchar_t path[MAX_PATH];
    const DWORD len = ::GetModuleFileNameW(nullptr, path, WXSIZEOF(path));
    if ( !len || len == WXSIZEOF(path) )
        return wxString();

    wxString dir(path, len);
    const wxString::size_type slash = dir.find_last_of("\\/");
    if ( slash != wxString::npos )
        dir.erase(slash + 1);
    else
        dir.clear();

    return dir;
}

bool wxWinUIAppendLooseXamlResources(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources)
{
    using namespace winrt::Microsoft::UI::Xaml;
    using namespace winrt::Windows::Foundation;

    try
    {
        ResourceDictionary dictionary;
        dictionary.Source(Uri(L"ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xbf"));
        resources.MergedDictionaries().Append(dictionary);

        dictionary = ResourceDictionary();
        dictionary.Source(Uri(L"ms-appx:///Microsoft.UI.Xaml/Themes/generic.xbf"));
        resources.MergedDictionaries().Append(dictionary);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
    }

    try
    {
        wxString source = wxWinUIGetModuleDirectory();
        if ( source.empty() )
            return false;

        source += "Microsoft.UI\\Themes\\generic.xaml";
        if ( ::GetFileAttributesW(source.wc_str()) == INVALID_FILE_ATTRIBUTES )
            return false;

        source.Replace("\\", "/");
        source.Replace(" ", "%20");
        source.Prepend("file:///");

        ResourceDictionary dictionary;
        dictionary.Source(Uri(winrt::hstring(source.wc_str())));
        resources.MergedDictionaries().Append(dictionary);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException(
            "WinUI loose XAML resources initialization", e);
        return false;
    }
}

bool wxWinUIEnsureDispatcherQueue()
{
    try
    {
        using namespace winrt::Microsoft::UI::Dispatching;

        if ( const auto current = DispatcherQueue::GetForCurrentThread() )
        {
            // XAML runtime shutdown is tied to DispatcherQueue shutdown in
            // Windows App SDK 1.5+. wx cannot safely remove the bootstrap and
            // its COM apartment if another component owns this queue and its
            // controller, so reject that topology before creating XAML.
            if ( !gs_winuiDispatcherQueueController ||
                 gs_winuiDispatcherQueueController.DispatcherQueue() !=
                     current )
            {
                wxLogWarning("wxWinUI: the UI thread already has an "
                             "externally-owned DispatcherQueue");
                return false;
            }
            return true;
        }

        gs_winuiDispatcherQueueController =
            DispatcherQueueController::CreateOnCurrentThread();

        if ( gs_winuiDispatcherQueueController )
            return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException(
            "DispatcherQueueController initialization", e);
        return false;
    }
    catch ( ... )
    {
        wxLogWarning("DispatcherQueueController initialization raised a "
                     "non-WinRT exception");
        return false;
    }

    wxLogWarning("DispatcherQueueController initialization failed.");
    return false;
}

bool wxWinUIShutdownOwnedDispatcherQueue(
    const char *what,
    bool requireXamlBoundary = false,
    bool throwBeforeShutdownForTesting = false) noexcept
{
    if ( !gs_winuiDispatcherQueueController )
    {
        if ( requireXamlBoundary )
            wxWinUIQuarantineRuntimeEpoch();
        return !requireXamlBoundary;
    }

    if ( requireXamlBoundary )
    {
        wxWinUIFrameworkRetirementRuntime::Get()
            .BeginRundownBeforeShutdown();
    }

    try
    {
        if ( throwBeforeShutdownForTesting )
            throw winrt::hresult_error(E_FAIL);
        gs_winuiDispatcherQueueController.ShutdownQueue();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException(what, e);
        // Keep the controller, bootstrap, COM apartment and owner thread
        // quarantined together. Releasing any of them while XAML can still
        // dispatch would turn a recoverable shutdown failure into UAF.
        wxWinUIQuarantineRuntimeEpoch();
        return false;
    }

    catch ( ... )
    {
        wxLogWarning("%s failed with a non-WinRT exception", what);
        wxWinUIQuarantineRuntimeEpoch();
        return false;
    }

    if ( gs_winuiRuntimeState.load(std::memory_order_acquire) ==
             wxWinUIRuntimeState::Quarantined )
    {
        // The XAML callback may have entered quarantine while ShutdownQueue()
        // was on the stack. Its handler already transferred the epoch.
        return false;
    }

    if ( requireXamlBoundary )
    {
        wxWinUIFrameworkRetirementRuntime& runtime =
            wxWinUIFrameworkRetirementRuntime::Get();
        if ( !runtime.ValidateAfterShutdown() || gs_winuiApplication ||
             !runtime.IsTerminalLedgerEmpty() )
        {
            wxLogWarning("%s returned without the complete WinUI shutdown "
                         "sequence or with retained XAML graphs",
                         what);
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }
    }

    gs_winuiXamlShutdownRetirementHookInstalled = false;
    gs_winuiXamlShutdownRetirementToken = {};
    if ( requireXamlBoundary )
    {
        gs_winuiXamlManager = nullptr;
        if ( !wxWinUIFrameworkRetirementRuntime::Get()
                  .IsTerminalLedgerEmpty() )
        {
            wxLogWarning("%s released the closed runtime but a reentrant "
                         "WinUI graph entered terminal quarantine",
                         what);
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }
    }
    gs_winuiDispatcherQueueController = nullptr;
    if ( requireXamlBoundary &&
         !wxWinUIFrameworkRetirementRuntime::Get()
              .IsTerminalLedgerEmpty() )
    {
        wxLogWarning("%s released the closed DispatcherQueue controller but "
                     "a WinUI graph entered terminal quarantine",
                     what);
        wxWinUIQuarantineRuntimeEpoch();
        return false;
    }
    return true;
}

bool wxWinUIEnsureXamlManager()
{
    try
    {
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        gs_winuiXamlManager = WindowsXamlManager::GetForCurrentThread();
        if ( gs_winuiXamlManager )
            return true;

        gs_winuiXamlManager = WindowsXamlManager::InitializeForCurrentThread();
        return static_cast<bool>(gs_winuiXamlManager);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException("WindowsXamlManager initialization", e);
        return false;
    }
    catch ( ... )
    {
        wxLogWarning("WindowsXamlManager initialization raised a non-WinRT "
                     "exception");
        return false;
    }
}

// Instantiate our own application object so that it becomes
// Application::Current() and provides the XAML metadata provider before the
// XAML framework would otherwise create a default application of its own.
bool wxWinUIEnsureApplication()
{
    try
    {
        using namespace winrt::Microsoft::UI::Xaml;

        gs_winuiApplication = Application::Current();
        if ( gs_winuiApplication )
            return true;

        gs_winuiApplication =
            winrt::make<wxWinUIApplication>();
        return static_cast<bool>(gs_winuiApplication);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUIBootstrapLogException("WinUI application creation", e);
        return false;
    }
    catch ( ... )
    {
        wxLogWarning("WinUI application creation raised a non-WinRT "
                     "exception");
        return false;
    }
}

bool wxWinUIEnsureApplicationResources()
{
    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        auto app = Application::Current();
        if ( !app )
            return false;

        auto resources = app.Resources();
        if ( !resources )
        {
            resources = ResourceDictionary();
            app.Resources(resources);
        }

        resources.MergedDictionaries().Append(XamlControlsResources());
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        // The fallback crosses the same WinRT ABI as the primary path and can
        // fail independently. Never let that second failure escape after the
        // XAML manager and shutdown hooks have already been published: the
        // caller must get false and run the causal queue-shutdown path.
        try
        {
            auto app = winrt::Microsoft::UI::Xaml::Application::Current();
            auto resources = app ? app.Resources() : nullptr;
            if ( resources && wxWinUIAppendLooseXamlResources(resources) )
                return true;
        }
        catch ( const winrt::hresult_error& fallbackError )
        {
            wxWinUIBootstrapLogException(
                "WinUI application resource fallback", fallbackError);
        }
        catch ( ... )
        {
            wxLogWarning("WinUI application resource fallback raised a "
                         "non-WinRT exception");
        }

        wxWinUIBootstrapLogException(
            "WinUI application resources initialization", e);
        return false;
    }
    catch ( ... )
    {
        wxLogWarning("WinUI application resources initialization raised a "
                     "non-WinRT exception");
        return false;
    }
}

wxWinUIContentPreTranslateMessage wxWinUIGetContentPreTranslateMessage()
{
    if ( gs_winuiContentPreTranslateMessage )
        return gs_winuiContentPreTranslateMessage;

    static const wchar_t *dlls[] =
    {
        L"Microsoft.UI.Windowing.Core.dll",
        L"Microsoft.UI.Windowing.dll",
        L"Microsoft.ui.xaml.dll",
    };

    for ( const wchar_t *name : dlls )
    {
        HMODULE module = ::GetModuleHandleW(name);
        if ( !module )
            module = ::LoadLibraryW(name);
        if ( !module )
            continue;

        gs_winuiContentPreTranslateMessage =
            reinterpret_cast<wxWinUIContentPreTranslateMessage>(
                ::GetProcAddress(module, "ContentPreTranslateMessage"));
        if ( gs_winuiContentPreTranslateMessage )
            return gs_winuiContentPreTranslateMessage;
    }

    if ( !gs_winuiContentPreTranslateLogShown )
    {
        gs_winuiContentPreTranslateLogShown = true;
        wxLogWarning("ContentPreTranslateMessage export not found; "
                     "WinUI keyboard input may not work.");
    }

    return nullptr;
}

struct wxWinUIXamlPreTranslateContext
{
    wxWinUIContentPreTranslateMessage function;
};

bool wxWinUICallXamlPreTranslate(WXMSG *msg, void *context)
{
    const auto * const xaml =
        static_cast<const wxWinUIXamlPreTranslateContext *>(context);
    return xaml && xaml->function &&
           xaml->function(reinterpret_cast<const MSG *>(msg)) != FALSE;
}

wxWinUIKeyboardPipelineResult wxWinUIProcessKeyboardMessageImpl(
    MSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    if ( !msg || !wxWinUIIsKeyboardMessage(msg) )
        return wxWinUIKeyboardPipelineResult::NotApplicable;

    // This veto applies to the removed Windows message itself, even if its
    // HWND is a native IME/helper window rather than an island HWND.
    if ( wxWinUI3ConsumeBlockedKeyboardMessage(
            reinterpret_cast<WXMSG *>(msg)) )
    {
        return wxWinUIKeyboardPipelineResult::WxHandled;
    }

    // Tab has one arbiter in every message loop, including native nested
    // loops: first cross the wx/XAML slot boundary if needed, then let XAML
    // navigate inside the island. The arbiter itself is strictly scoped to
    // msg->hwnd and to the current critical subtree.
    if ( wxWinUIIsTabMessage(msg) &&
            wxWinUI3ProcessTabNavigationWithModifiers(
                reinterpret_cast<WXMSG *>(msg),
                modifiers.shiftDown,
                modifiers.controlDown,
                modifiers.leftAltDown || modifiers.rightAltDown) )
    {
        return wxWinUIKeyboardPipelineResult::WxHandled;
    }

    // Messages addressed to an island use the slot owner recorded by the
    // host. Both wx accelerators and XAML may consume them.
    wxWindow * const islandOwner =
        wxWinUITLWHostResolveFocus((WXHWND)msg->hwnd);
    if ( islandOwner )
    {
        return wxWinUIRunKeyboardPipelineImpl(
            msg,
            islandOwner,
            modifiers,
            xamlPreTranslate,
            xamlContext);
    }

    // A genuine wx native HWND still owns the normal wx preprocessing
    // contract. This matters in nested native loops, where there is no later
    // wxGUIEventLoop::PreProcessMessage() pass. Feed XAML afterwards as a
    // presentation pump, but do not let its global return value steal a
    // message addressed to the native HWND.
    if ( wxWindow * const nativeOwner =
            wxGetWindowFromHWND((WXHWND)msg->hwnd) )
    {
        const wxWinUIKeyboardPipelineResult wxResult =
            wxWinUIRunKeyboardPipelineImpl(
                msg, nativeOwner, modifiers, nullptr, nullptr);
        if ( wxResult == wxWinUIKeyboardPipelineResult::WxHandled )
            return wxResult;

        if ( xamlPreTranslate )
            xamlPreTranslate(
                reinterpret_cast<WXMSG *>(msg), xamlContext);
        return wxWinUIKeyboardPipelineResult::NotHandled;
    }

    // ContentPreTranslateMessage is also the presentation pump for XAML
    // islands. It must still see genuinely foreign messages, but neither its
    // return value nor wx accelerator tables may steal those messages.
    if ( xamlPreTranslate )
        xamlPreTranslate(reinterpret_cast<WXMSG *>(msg), xamlContext);

    return wxWinUIKeyboardPipelineResult::NotHandled;
}

bool wxWinUIProcessMessageWithCallback(
    MSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    if ( !msg )
        return false;

    if ( wxWinUIIsKeyboardMessage(msg) )
    {
        const wxWinUIKeyboardPipelineResult result =
            wxWinUIProcessKeyboardMessageImpl(
                msg, modifiers, xamlPreTranslate, xamlContext);
        return result == wxWinUIKeyboardPipelineResult::WxHandled ||
               result == wxWinUIKeyboardPipelineResult::XamlHandled;
    }

    return xamlPreTranslate &&
           xamlPreTranslate(
               reinterpret_cast<WXMSG *>(msg), xamlContext);
}

bool wxWinUIProcessMessageImpl(
    MSG *msg,
    wxWinUIContentPreTranslateMessage xamlPreTranslate)
{
    wxWinUIXamlPreTranslateContext xaml = { xamlPreTranslate };
    return wxWinUIProcessMessageWithCallback(
        msg,
        wxWinUIGetKeyboardModifiers(),
        wxWinUICallXamlPreTranslate,
        &xaml);
}

// ----------------------------------------------------------------------------
// Thread-wide GetMessage hook
// ----------------------------------------------------------------------------

// ContentPreTranslateMessage() must see every message the GUI thread pumps or
// the islands' input and presentation machinery starves.  Our own event loop
// calls it (wxGUIEventLoop::ProcessMessage), but the system runs plenty of
// message loops we do not control: the SC_MOVE/SC_SIZE loop while the user
// drags or resizes a window, MessageBox(), native menus, OLE waits...  While
// one of those pumps, XAML content degrades to ~1 Hz: hover states stop
// answering, hosted dialogs stall, and interactive resizes leave stale-pixel
// trails.  This is the documented island contract for exactly this case:
// hook WH_GETMESSAGE and pre-translate from there so nested native loops
// keep the islands alive.
HHOOK gs_winuiGetMsgHook = nullptr;
bool gs_winuiGetMsgHookEnabled = false;
enum class wxWinUIGetMsgHookState
{
    Absent,
    Active,
    Quarantined
};
wxWinUIGetMsgHookState gs_winuiGetMsgHookState =
    wxWinUIGetMsgHookState::Absent;
bool gs_winuiGetMsgHookFailureLogged = false;

LRESULT CALLBACK wxWinUIGetMsgHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if ( gs_winuiGetMsgHookEnabled &&
            !gs_winuiForceEventLoopFallbackForTesting &&
            code >= 0 && wParam == PM_REMOVE )
    {
        MSG * const msg = reinterpret_cast<MSG *>(lParam);

        // Use the cached pointer only: no library loading from a hook. Tab is
        // intentionally included: native nested loops do not pass through
        // wxGUIEventLoop, so the shared arbiter must run here too.
        const wxWinUIContentPreTranslateMessage fn =
            gs_winuiContentPreTranslateMessage;
        if ( wxWinUIProcessMessageImpl(msg, fn) )
        {
            // Handled by wx or an island: neutralize the message so whichever
            // loop retrieved it dispatches a no-op.
            msg->message = WM_NULL;
            msg->wParam = 0;
            msg->lParam = 0;
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

bool wxWinUIEnsureGetMessageHook()
{
    gs_winuiGetMsgHookEnabled = false;

    if ( gs_winuiGetMsgHookState ==
            wxWinUIGetMsgHookState::Quarantined )
    {
        // The old callback may still be installed after a failed unhook.
        // Prove it is gone before installing another one; never stack two
        // callbacks sharing the same global enable gate.
        if ( !gs_winuiGetMsgHook ||
             ::UnhookWindowsHookEx(gs_winuiGetMsgHook) )
        {
            gs_winuiGetMsgHook = nullptr;
            gs_winuiGetMsgHookState = wxWinUIGetMsgHookState::Absent;
        }
        else
        {
            const DWORD error = ::GetLastError();
            if ( !gs_winuiGetMsgHookFailureLogged )
            {
                wxLogWarning("wxWinUI: quarantined WH_GETMESSAGE hook could "
                             "not be removed (error %lu); it remains "
                             "disabled", error);
                gs_winuiGetMsgHookFailureLogged = true;
            }
            return false;
        }
    }

    if ( gs_winuiGetMsgHookState == wxWinUIGetMsgHookState::Active &&
         gs_winuiGetMsgHook )
    {
        gs_winuiGetMsgHookEnabled = true;
        return true;
    }

    gs_winuiGetMsgHook = ::SetWindowsHookExW(WH_GETMESSAGE,
                                             wxWinUIGetMsgHookProc,
                                             nullptr,
                                             ::GetCurrentThreadId());
    if ( !gs_winuiGetMsgHook )
    {
        const DWORD error = ::GetLastError();
        gs_winuiGetMsgHookState = wxWinUIGetMsgHookState::Absent;
        if ( !gs_winuiGetMsgHookFailureLogged )
        {
            // The wx event loop has a semantic fallback, but native nested
            // loops cannot use it. Keep initialization usable and make this
            // partial degradation observable.
            wxLogWarning("wxWinUI: WH_GETMESSAGE hook installation failed "
                         "(error %lu); nested native-loop WinUI input is "
                         "degraded", error);
            gs_winuiGetMsgHookFailureLogged = true;
        }
        return false;
    }

    gs_winuiGetMsgHookState = wxWinUIGetMsgHookState::Active;
    gs_winuiGetMsgHookEnabled = true;
    gs_winuiGetMsgHookFailureLogged = false;
    return true;
}

} // namespace

wxWinUIFrameworkRetirementId wxWinUIRegisterFrameworkRetirement(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
    std::function<void ()> complete) noexcept
{
    return wxWinUIFrameworkRetirementRuntime::Get().Register(
        queue, std::move(complete));
}

void wxWinUICompleteFrameworkRetirement(
    wxWinUIFrameworkRetirementId id) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get().Complete(id);
}

bool wxWinUIBeginFrameworkRetirementRuntime(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue) noexcept
{
    return wxWinUIFrameworkRetirementRuntime::Get().BeginRuntime(queue);
}

void wxWinUISimulateShutdownStartingForTesting(
    wxWinUIFrameworkRetirementPhaseHookForTesting hook,
    void *context) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get()
        .SimulateShutdownStartingForTesting(hook, context);
}

void wxWinUISimulateFrameworkShutdownStartingForTesting(
    wxWinUIFrameworkRetirementPhaseHookForTesting hook,
    void *context) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get()
        .SimulateFrameworkShutdownStartingForTesting(hook, context);
}

void wxWinUISimulateFrameworkShutdownCompletedForTesting(
    wxWinUIFrameworkRetirementPhaseHookForTesting hook,
    void *context) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get()
        .SimulateFrameworkShutdownCompletedForTesting(hook, context);
}

void wxWinUISimulateShutdownCompletedForTesting(
    wxWinUIFrameworkRetirementPhaseHookForTesting hook,
    void *context) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get()
        .SimulateShutdownCompletedForTesting(hook, context);
}

void wxWinUISimulateXamlShutdownCompletedForTesting(
    wxWinUIFrameworkRetirementPhaseHookForTesting hook,
    void *context) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get()
        .CompleteAtXamlBoundary(hook, context);
}

void wxWinUIResetFrameworkRetirementRuntimeForTesting(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue) noexcept
{
    wxWinUIFrameworkRetirementRuntime::Get().ResetForTesting(queue);
}

void wxWinUISetFrameworkRetirementHookFaultForTesting(
    unsigned faultMask) noexcept
{
    gs_winuiFrameworkRetirementHookFaultForTesting.store(
        faultMask, std::memory_order_release);
}

unsigned wxWinUIGetFrameworkRetirementHookFaultForTesting() noexcept
{
    return gs_winuiFrameworkRetirementHookFaultForTesting.load(
        std::memory_order_acquire);
}

wxWinUIFrameworkRetirementSnapshotForTesting
wxWinUIGetFrameworkRetirementSnapshotForTesting() noexcept
{
    wxWinUIFrameworkRetirementSnapshotForTesting snapshot =
        wxWinUIFrameworkRetirementRuntime::Get()
            .GetSnapshotForTesting();
    snapshot.xamlShutdownHookInstalled =
        gs_winuiXamlShutdownRetirementHookInstalled;
    return snapshot;
}

std::size_t wxWinUIGetFrameworkRetirementCountForTesting() noexcept
{
    return wxWinUIFrameworkRetirementRuntime::Get()
        .GetPendingCountForTesting();
}

wxWinUIKeyboardPipelineResult wxWinUI3RunKeyboardPipelineForTesting(
    WXMSG *msg,
    wxWindow *logicalOwner,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    // Make the same injected snapshot visible to the local text-entry filter,
    // which normally queries GetKeyState() itself.
    struct ModifierOverrideRestorer
    {
        ~ModifierOverrideRestorer()
        {
            gs_winuiKeyboardModifiersForTesting = previous;
            gs_winuiKeyboardModifiersOverriddenForTesting = hadOverride;
        }

        bool hadOverride;
        wxWinUIKeyboardModifiers previous;
    } restore =
    {
        gs_winuiKeyboardModifiersOverriddenForTesting,
        gs_winuiKeyboardModifiersForTesting
    };

    gs_winuiKeyboardModifiersForTesting = modifiers;
    gs_winuiKeyboardModifiersOverriddenForTesting = true;

    return wxWinUIRunKeyboardPipelineImpl(
        reinterpret_cast<MSG *>(msg),
        logicalOwner,
        modifiers,
        xamlPreTranslate,
        xamlContext);
}

wxWinUIKeyboardPipelineResult
wxWinUI3ProcessKeyboardMessageForTesting(
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    return wxWinUIProcessKeyboardMessageImpl(
        reinterpret_cast<MSG *>(msg),
        modifiers,
        xamlPreTranslate,
        xamlContext);
}

bool wxWinUI3ProcessGetMessageHookForTesting(
    int code,
    WXWPARAM removalMode,
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    if ( code < 0 || removalMode != PM_REMOVE || !msg )
        return false;

    MSG * const native = reinterpret_cast<MSG *>(msg);
    if ( !wxWinUIProcessMessageWithCallback(
            native,
            modifiers,
            xamlPreTranslate,
            xamlContext) )
    {
        return false;
    }

    native->message = WM_NULL;
    native->wParam = 0;
    native->lParam = 0;
    return true;
}

bool wxWinUI3ProcessEventLoopFallbackForTesting(
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext)
{
    return msg &&
           wxWinUIProcessMessageWithCallback(
               reinterpret_cast<MSG *>(msg),
               modifiers,
               xamlPreTranslate,
               xamlContext);
}

bool wxWinUI3ForceEventLoopFallbackForTesting(bool forceFallback)
{
    const bool wasForced = gs_winuiForceEventLoopFallbackForTesting;
    gs_winuiForceEventLoopFallbackForTesting = forceFallback;
    return wasForced;
}

void wxWinUI3SetKeyboardModifiersForTesting(
    const wxWinUIKeyboardModifiers *modifiers)
{
    if ( modifiers )
    {
        gs_winuiKeyboardModifiersForTesting = *modifiers;
        gs_winuiKeyboardModifiersOverriddenForTesting = true;
    }
    else
    {
        gs_winuiKeyboardModifiersOverriddenForTesting = false;
    }
}

bool wxWinUI3GetKeyboardModifiersOverrideForTesting(
    wxWinUIKeyboardModifiers *modifiers)
{
    if ( !gs_winuiKeyboardModifiersOverriddenForTesting )
        return false;

    if ( modifiers )
        *modifiers = gs_winuiKeyboardModifiersForTesting;
    return true;
}

bool wxWinUI3IsGetMessageHookActive()
{
    return gs_winuiGetMsgHookState == wxWinUIGetMsgHookState::Active &&
           gs_winuiGetMsgHookEnabled &&
           !gs_winuiForceEventLoopFallbackForTesting &&
           gs_winuiGetMsgHook != nullptr;
}

bool wxWinUI3Initialize()
{
    const wxWinUIRuntimeFaultForTesting runtimeFault =
        wxWinUIConsumeRuntimeFaultForTesting();

    // Allocate the two intentionally immortal safety stores before claiming
    // any process/thread runtime resource. A failure here is a clean ordinary
    // initialization failure, not a partially activated XAML epoch.
    if ( !wxWinUIPrepareImmortalRuntimeStorage() )
        return false;

    const DWORD currentThreadId = ::GetCurrentThreadId();
    DWORD expectedOwnerThreadId = 0;
    const bool claimedOwnerThread =
        gs_winuiOwnerThreadId.compare_exchange_strong(
            expectedOwnerThreadId,
            currentThreadId,
            std::memory_order_acq_rel,
            std::memory_order_acquire);
    if ( !claimedOwnerThread && expectedOwnerThreadId != currentThreadId )
    {
        wxLogWarning("wxWinUI: initialization requested from thread %lu, "
                     "but the runtime belongs to UI thread %lu",
                     currentThreadId, expectedOwnerThreadId);
        return false;
    }

    wxWinUIRuntimeState expectedState = wxWinUIRuntimeState::Idle;
    if ( !gs_winuiRuntimeState.compare_exchange_strong(
            expectedState,
            wxWinUIRuntimeState::Initializing,
            std::memory_order_acq_rel,
            std::memory_order_acquire) )
    {
        if ( expectedState == wxWinUIRuntimeState::Running &&
             gs_winuiBootstrapInitialized )
        {
            return true;
        }

        wxLogWarning("wxWinUI: re-entrant initialization rejected while "
                     "the runtime is transitioning");
        if ( claimedOwnerThread &&
             expectedState != wxWinUIRuntimeState::Quarantined )
        {
            gs_winuiOwnerThreadId.store(0, std::memory_order_release);
        }
        return false;
    }

    const auto releaseOwnerClaim = [claimedOwnerThread]() noexcept
    {
        gs_winuiRuntimeState.store(
            wxWinUIRuntimeState::Idle, std::memory_order_release);
        if ( claimedOwnerThread )
        {
            gs_winuiOwnerThreadId.store(0, std::memory_order_release);
        }
    };

    if ( gs_winuiBootstrapInitialized || gs_winuiComInitialized )
    {
        // A consistent running runtime is handled before the transition CAS.
        // Reaching this branch means state and process ownership disagreed; fail
        // closed instead of attempting to repair global XAML state in place.
        wxWinUIQuarantineRuntimeEpoch();
        return false;
    }

    try
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
    }
    catch ( const winrt::hresult_error& e )
    {
        // XAML Islands require the UI thread to be an STA. Continuing after
        // RPC_E_CHANGED_MODE would publish a half-initialized global runtime
        // backed by an incompatible MTA.
        wxWinUIBootstrapLogException("winrt::init_apartment", e);
        releaseOwnerClaim();
        return false;
    }

    // Publish ownership as soon as it exists, not only when the runtime
    // reaches Running. A XAML callback can quarantine during an initialization
    // cleanup and must be able to transfer this apartment reference itself.
    gs_winuiComInitialized = true;

    const auto failInitialization = [&]() noexcept
    {
        if ( gs_winuiBootstrapInitialized )
        {
            ::MddBootstrapShutdown();
            gs_winuiBootstrapInitialized = false;
        }
        if ( gs_winuiComInitialized )
        {
            winrt::uninit_apartment();
            gs_winuiComInitialized = false;
        }
        releaseOwnerClaim();
        return false;
    };

    bool xamlActivationAttempted = false;

    // This is the final exception boundary after COM ownership is published.
    // Helpers below normally translate their own failures, but allocations in
    // logging, renderer/policy setup or future initialization work must not
    // strand an Initializing epoch with destructible WinRT globals.
    const auto failUnexpectedInitialization = [&]() noexcept
    {
#if wxUSE_TOOLTIPS
        try
        {
            wxWinUIShutdownToolTipPolicy();
        }
        catch ( ... )
        {
            // Policy globals can themselves retain callback-bearing XAML
            // objects and are not part of the core epoch holder. If their
            // invalidation cannot complete, neither shutdown nor immortalizing
            // just the core globals can prevent a later CRT destructor release.
            wxWinUIFailFastQuarantine();
        }
#endif

        const bool xamlMayBeActive =
            xamlActivationAttempted || gs_winuiApplication ||
            gs_winuiXamlManager ||
            gs_winuiXamlShutdownRetirementHookInstalled;
        if ( xamlMayBeActive )
        {
            if ( !gs_winuiXamlShutdownRetirementHookInstalled )
            {
                wxWinUIQuarantineRuntimeEpoch();
                return false;
            }

            if ( !wxWinUIShutdownOwnedDispatcherQueue(
                     "DispatcherQueueController::ShutdownQueue after "
                     "unexpected initialization exception",
                     true) )
            {
                return false;
            }
        }
        else if ( gs_winuiDispatcherQueueController &&
                  !wxWinUIShutdownOwnedDispatcherQueue(
                      "DispatcherQueueController::ShutdownQueue after "
                      "unexpected pre-XAML initialization exception") )
        {
            return false;
        }

        return failInitialization();
    };

    try
    {
        PACKAGE_VERSION minVersion{};
        minVersion.Version = WINDOWSAPPSDK_RUNTIME_VERSION_UINT64;

        const HRESULT hrBootstrap = ::MddBootstrapInitialize2(
            WINDOWSAPPSDK_RELEASE_MAJORMINOR,
            WINDOWSAPPSDK_RELEASE_VERSION_TAG_W,
            minVersion,
            MddBootstrapInitializeOptions_OnPackageIdentity_NOOP);

        if ( FAILED(hrBootstrap) )
        {
            wxWinUILogHresult("MddBootstrapInitialize2", hrBootstrap);
            return failInitialization();
        }

        // As with COM, partial XAML activation can quarantine before Running.
        // Make the bootstrap reference visible to every callback-driven branch.
        gs_winuiBootstrapInitialized = true;

        if ( !wxWinUIEnsureDispatcherQueue() )
        {
            return failInitialization();
        }

        if ( !wxWinUIPinXamlRuntimeModule() )
        {
            if ( !wxWinUIShutdownOwnedDispatcherQueue(
                     "DispatcherQueueController::ShutdownQueue after XAML "
                     "module pin failure") )
            {
                return false;
            }
            return failInitialization();
        }

        // The application object (which provides the XAML metadata provider)
        // must exist before the XAML manager spins up a default application of
        // its own, otherwise XamlControlsResources cannot be constructed and
        // the controls render without their templates.
        xamlActivationAttempted = true;
        if ( !wxWinUIEnsureApplication() )
        {
            // Application construction/QI is the first XAML activation and may
            // fail after native framework state already exists. Without the
            // XAML terminal hook no same-stack teardown is causally safe.
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }
        if ( runtimeFault ==
                 wxWinUIRuntimeFaultForTesting::AfterApplication )
        {
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }

        if ( !wxWinUIEnsureXamlManager() )
        {
            // InitializeForCurrentThread() can activate XAML before projection
            // construction reports failure. There is no causal XAML terminal
            // hook yet, so neither Application nor the queue may be released.
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }
        if ( runtimeFault ==
                 wxWinUIRuntimeFaultForTesting::AfterXamlManager )
        {
            wxWinUIQuarantineRuntimeEpoch();
            return false;
        }

        // Install all DispatcherQueue phases and the XAML-specific terminal
        // hook before any control can own a peer graph. A runtime without this
        // causal boundary cannot be published as Running.
        if ( !wxWinUIArmFrameworkRetirementRuntime(runtimeFault) )
        {
            if ( !gs_winuiXamlShutdownRetirementHookInstalled )
            {
                // Without the XAML event there is no safe place to release the
                // Application graph. Do not enter ShutdownQueue without a
                // causal deferral: retain the entire epoch unchanged instead.
                wxWinUIQuarantineRuntimeEpoch();
                return false;
            }

            if ( !wxWinUIShutdownOwnedDispatcherQueue(
                     "DispatcherQueueController::ShutdownQueue after shutdown "
                     "hook failure",
                     true) )
            {
                return false;
            }
            return failInitialization();
        }

        if ( runtimeFault ==
                 wxWinUIRuntimeFaultForTesting::ThrowShutdownQueue ||
             runtimeFault ==
                 wxWinUIRuntimeFaultForTesting::SuppressFrameworkCompleted )
        {
            const bool throwBeforeShutdown =
                runtimeFault ==
                    wxWinUIRuntimeFaultForTesting::ThrowShutdownQueue;
            if ( !wxWinUIShutdownOwnedDispatcherQueue(
                     throwBeforeShutdown
                         ? "injected DispatcherQueue::ShutdownQueue throw"
                         : "injected missing FrameworkShutdownCompleted phase",
                     true,
                     throwBeforeShutdown) )
            {
                return false;
            }

            // The suppressed phase must make validation fail closed. If it
            // did not, leave the runtime reusable so the subprocess oracle's
            // next Initialize() call exposes the qualification failure.
            return failInitialization();
        }

        // Theme resources can only be loaded once the framework is initialized
        // by the XAML manager above; do it now that our application is current.
        if ( !wxWinUIEnsureApplicationResources() )
        {
            if ( !wxWinUIShutdownOwnedDispatcherQueue(
                     "DispatcherQueueController::ShutdownQueue after "
                     "resource failure",
                     true) )
            {
                return false;
            }
            return failInitialization();
        }

#if wxUSE_TOOLTIPS
        wxWinUIInitializeToolTipPolicy();
#endif

        // Give generic controls Fluent-looking chrome; this allocation and all
        // following setup remain inside the phase-aware exception boundary.
        extern void wxWinUIInstallRenderer();
        wxWinUIInstallRenderer();

        // Resolve the pre-translate export now and keep every message loop on
        // this thread feeding it -- the native modal ones included.
        wxWinUIGetContentPreTranslateMessage();
        // Non-fatal for bootstrap: the event loop owns the semantic fallback.
        wxWinUIEnsureGetMessageHook();

        gs_winuiRuntimeState.store(
            wxWinUIRuntimeState::Running, std::memory_order_release);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        try
        {
            wxWinUIBootstrapLogException(
                "WinUI runtime initialization", e);
        }
        catch ( ... )
        {
        }
        return failUnexpectedInitialization();
    }
    catch ( ... )
    {
        try
        {
            wxLogWarning("WinUI runtime initialization raised a non-WinRT "
                         "exception");
        }
        catch ( ... )
        {
        }
        return failUnexpectedInitialization();
    }
}

void wxWinUI3Uninitialize()
{
    // Module cleanup always calls OnExit(), including after an
    // initialization failure deliberately retained the epoch in terminal
    // quarantine. Treat that state as an idempotent no-op just like Idle:
    // logging from wx module teardown can fall back to a modal message box
    // after the normal log target has already been destroyed.
    const wxWinUIRuntimeState initialState =
        gs_winuiRuntimeState.load(std::memory_order_acquire);
    if ( initialState == wxWinUIRuntimeState::Idle ||
         initialState == wxWinUIRuntimeState::Quarantined )
    {
        return;
    }

    const DWORD ownerThreadId =
        gs_winuiOwnerThreadId.load(std::memory_order_acquire);
    if ( ownerThreadId && ownerThreadId != ::GetCurrentThreadId() )
    {
        wxLogWarning("wxWinUI: uninitialization requested from a thread "
                     "which does not own the UI runtime");
        return;
    }

    wxWinUIRuntimeState expectedState = wxWinUIRuntimeState::Running;
    if ( !gs_winuiRuntimeState.compare_exchange_strong(
             expectedState,
             wxWinUIRuntimeState::ShuttingDown,
            std::memory_order_acq_rel,
            std::memory_order_acquire) )
    {
        // The state may have become terminal since the initial load above.
        if ( expectedState != wxWinUIRuntimeState::Idle &&
             expectedState != wxWinUIRuntimeState::Quarantined )
        {
            wxLogWarning("wxWinUI: re-entrant uninitialization rejected "
                         "while the runtime is transitioning");
        }
        return;
    }

    // Freeze normal peer-retirement completion at the same synchronous state
    // transition which rejects new runtime work. ShutdownStarting is raised
    // only after entering ShutdownQueue(), which would otherwise leave a gap
    // in which a queued barrier could destroy a XAML graph too early.
    wxWinUIFrameworkRetirementRuntime::Get()
        .BeginRundownBeforeShutdown();

    // Gate the callback before touching the runtime. If unhooking fails, the
    // still-installed hook can only chain onward and never calls XAML again.
    gs_winuiGetMsgHookEnabled = false;
    if ( gs_winuiGetMsgHook )
    {
        if ( ::UnhookWindowsHookEx(gs_winuiGetMsgHook) )
        {
            gs_winuiGetMsgHook = nullptr;
            gs_winuiGetMsgHookState = wxWinUIGetMsgHookState::Absent;
        }
        else
        {
            const DWORD error = ::GetLastError();
            gs_winuiGetMsgHookState =
                wxWinUIGetMsgHookState::Quarantined;

            // Even disabled, the installed Windows hook still targets code
            // in this module and can be entered merely to chain onward. Keep
            // that callback address valid if a shared wx library is unloaded.
            // A pin failure has no safe recoverable continuation.
            if ( !wxWinUIPinOwnerModuleForQuarantine() )
                wxWinUIFailFastQuarantine();

            wxLogWarning("wxWinUI: failed to remove the WH_GETMESSAGE hook "
                         "(error %lu); the callback remains disabled",
                         error);
        }
    }
    else
    {
        gs_winuiGetMsgHookState = wxWinUIGetMsgHookState::Absent;
    }

#if wxUSE_TOOLTIPS
    // Drop every registry-held XAML object and invalidate deferred policy
    // callbacks while the XAML manager and COM apartment are still valid.
    wxWinUIShutdownToolTipPolicy();
#endif

    // Windows App SDK 1.8 owns XAML teardown from DispatcherQueue shutdown.
    // Keep both manager and Application strong through ShutdownQueue(); the
    // XAML terminal callback releases peer graphs and Application under its
    // DispatcherQueue deferral, and the manager is released only afterwards.
    if ( !wxWinUIShutdownOwnedDispatcherQueue(
             "DispatcherQueueController::ShutdownQueue",
             true) )
    {
        return;
    }

    if ( gs_winuiBootstrapInitialized )
    {
        ::MddBootstrapShutdown();
        gs_winuiBootstrapInitialized = false;
    }

    if ( gs_winuiComInitialized )
    {
        winrt::uninit_apartment();
        gs_winuiComInitialized = false;
    }

    gs_winuiContentPreTranslateMessage = nullptr;
    gs_winuiForceEventLoopFallbackForTesting = false;
    gs_winuiKeyboardModifiersOverriddenForTesting = false;
    gs_winuiOwnerThreadId.store(0, std::memory_order_release);
    gs_winuiRuntimeState.store(
        wxWinUIRuntimeState::Idle, std::memory_order_release);
}

bool wxWinUI3PreProcessMessage(WXMSG *msg)
{
    // This is the event-loop fallback for a missing hook. Runtime loading is
    // safe here (unlike from wxWinUIGetMsgHookProc), but a quarantined runtime
    // must never be called after its XAML manager has begun closing.
    const wxWinUIContentPreTranslateMessage fn =
        gs_winuiRuntimeState.load(std::memory_order_acquire) ==
                wxWinUIRuntimeState::Running &&
            gs_winuiBootstrapInitialized
            ? wxWinUIGetContentPreTranslateMessage()
            : nullptr;
    return wxWinUIProcessMessageImpl(reinterpret_cast<MSG *>(msg), fn);
}

bool wxWinUI3PreTranslateMessage(WXMSG *msg)
{
    // Give the focused WinUI island a chance to process the message; this is
    // what routes keyboard input (including character keys) to XAML controls.
    if ( gs_winuiRuntimeState.load(std::memory_order_acquire) !=
             wxWinUIRuntimeState::Running ||
         !gs_winuiBootstrapInitialized )
        return false;

    wxWinUIContentPreTranslateMessage fn = wxWinUIGetContentPreTranslateMessage();
    return fn && fn(reinterpret_cast<const MSG *>(msg)) != FALSE;
}

class wxWinUI3Module : public wxModule
{
public:
    bool OnInit() override
    {
        // Let non-WinUI wxMSW code continue to run if the runtime is missing.
        wxWinUI3Initialize();
        return true;
    }

    void OnExit() override
    {
        wxWinUI3Uninitialize();
    }

    wxDECLARE_DYNAMIC_CLASS(wxWinUI3Module);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUI3Module, wxModule);

#endif // wxUSE_WINUI3
