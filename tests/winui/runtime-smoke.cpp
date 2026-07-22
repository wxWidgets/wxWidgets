/////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/runtime-smoke.cpp
// Purpose:     Standalone probe of the unpackaged wxWinUI runtime payload
// Author:      wxWidgets development team
// Created:     2026-07-21
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This program verifies that the directory it runs from contains everything a
// wxWinUI application needs to start on a machine without any manually copied
// artefacts: the Windows App SDK bootstrapper, and the PRI/asset payload that
// XAML needs to resolve its theme and control template resources.
//
// It deliberately does not link against wxWidgets: it must be runnable from
// its own output directory to prove that the payload deployed next to *any*
// executable is self-sufficient, and it must stay buildable while the wx test
// suite itself is broken for unrelated reasons.  It replicates the exact
// initialization sequence of src/winui/winui.cpp (bootstrap, dispatcher
// queue, application object providing the XAML metadata, XAML manager, then
// XamlControlsResources) and then realizes Button, TreeView and TabView in a
// real (off-screen) XAML island.
//
// Exit codes:
//   0  success
//   1  Win32 setup failed (window class/window creation)
//   2  MddBootstrapInitialize2 failed: the Windows App Runtime framework
//      package is not installed on this machine
//   3  XamlControlsResources could not be created: the WinUI resource payload
//      (PRI files / asset trees) is missing next to the executable
//   4  the probe controls did not load or did not get their templates
//   5  unexpected WinRT error
//   6  dispatcher queue / XAML manager initialization failed
//   7  watchdog timeout (a stage hung instead of failing)

#include <windows.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>

namespace
{

// What the watchdog reports if a stage hangs instead of returning an error.
std::atomic<const char*> gs_stage{ "startup" };

void ReportStage(const char* stage)
{
    gs_stage = stage;
    std::printf("wx_winui_runtime_smoke: %s\n", stage);
    std::fflush(stdout);
}

int FailWinRT(int code, const char* what, const winrt::hresult_error& e)
{
    std::fprintf(stderr,
                 "wx_winui_runtime_smoke: FAILED: %s: HRESULT 0x%08lx: %ls\n",
                 what,
                 static_cast<unsigned long>(e.code().value),
                 e.message().c_str());
    return code;
}

void PrintPayloadHint()
{
    wchar_t path[MAX_PATH]{};
    ::GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::fprintf(stderr,
        "wx_winui_runtime_smoke: the unpackaged WinUI resource payload is "
        "expected next to the executable:\n"
        "  %ls\n"
        "  required: resources.pri (the Windows App Runtime framework "
        "resource index) and the Microsoft.UI.Xaml asset tree\n"
        "  (deployed by the wx_winui3_deploy_runtime() CMake helper)\n",
        path);
}

// Same shape as wxWinUIApplication in src/winui/winui.cpp: the application
// object must provide the XAML metadata for the WinUI controls, otherwise
// XamlControlsResources cannot be constructed at all, payload or not.
struct SmokeApplication :
    winrt::Microsoft::UI::Xaml::ApplicationT
    <
        SmokeApplication,
        winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider
    >
{
    SmokeApplication() = default;

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

LRESULT CALLBACK SmokeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Pump the message loop until the condition holds or the deadline passes.
template <typename Pred>
bool PumpUntil(Pred pred, DWORD timeoutMs)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    for ( ;; )
    {
        MSG msg;
        while ( ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }

        if ( pred() )
            return true;
        if ( ::GetTickCount64() >= deadline )
            return false;

        ::MsgWaitForMultipleObjectsEx(0, nullptr, 50, QS_ALLINPUT,
                                      MWMO_INPUTAVAILABLE);
    }
}

} // anonymous namespace

int main()
{
    // The probe must terminate with a diagnostic even if some stage hangs
    // instead of failing: CI would otherwise only see a timeout.
    std::thread([]()
    {
        ::Sleep(120000);
        std::fprintf(stderr,
                     "wx_winui_runtime_smoke: FAILED: watchdog timeout in "
                     "stage '%s'\n", gs_stage.load());
        std::fflush(stderr);
        ::ExitProcess(7);
    }).detach();

    ReportStage("initializing COM apartment");
    try
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( e.code() != RPC_E_CHANGED_MODE )
            return FailWinRT(5, "winrt::init_apartment", e);
    }

    ReportStage("initializing Windows App SDK bootstrap");
    PACKAGE_VERSION minVersion{};
    minVersion.Version = WINDOWSAPPSDK_RUNTIME_VERSION_UINT64;
    const HRESULT hrBootstrap = ::MddBootstrapInitialize2(
        WINDOWSAPPSDK_RELEASE_MAJORMINOR,
        WINDOWSAPPSDK_RELEASE_VERSION_TAG_W,
        minVersion,
        MddBootstrapInitializeOptions_OnPackageIdentity_NOOP);
    if ( FAILED(hrBootstrap) )
    {
        std::fprintf(stderr,
            "wx_winui_runtime_smoke: FAILED: MddBootstrapInitialize2: "
            "HRESULT 0x%08lx\n"
            "  the Windows App Runtime %u.%u framework package (version >= "
            "%u.%u.%u.%u) is not installed; install it from the "
            "Microsoft.WindowsAppSDK.Runtime NuGet package (tools/MSIX) or "
            "the Windows App SDK runtime installer\n",
            static_cast<unsigned long>(hrBootstrap),
            WINDOWSAPPSDK_RELEASE_MAJOR,
            WINDOWSAPPSDK_RELEASE_MINOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_MAJOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_MINOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_BUILD,
            WINDOWSAPPSDK_RUNTIME_VERSION_REVISION);
        return 2;
    }

    int rc = 5;
    try
    {
        using namespace winrt::Microsoft::UI::Dispatching;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        ReportStage("creating dispatcher queue");
        auto dispatcherController =
            DispatcherQueueController::CreateOnCurrentThread();
        if ( !dispatcherController )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "DispatcherQueueController is null\n");
            ::MddBootstrapShutdown();
            return 6;
        }

        // Application first, XAML manager second: same ordering constraint
        // as wxWinUI3Initialize().
        ReportStage("creating application object");
        auto application = winrt::make<SmokeApplication>().as<Application>();

        ReportStage("initializing XAML manager");
        auto xamlManager = WindowsXamlManager::InitializeForCurrentThread();
        if ( !xamlManager )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "WindowsXamlManager is null\n");
            ::MddBootstrapShutdown();
            return 6;
        }

        ReportStage("loading XamlControlsResources");
        try
        {
            auto resources = Application::Current().Resources();
            resources.MergedDictionaries().Append(XamlControlsResources());
        }
        catch ( const winrt::hresult_error& e )
        {
            FailWinRT(3, "XamlControlsResources", e);
            PrintPayloadHint();
            return 3;
        }

        ReportStage("creating host window");
        WNDCLASSW wc{};
        wc.lpfnWndProc = SmokeWndProc;
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.lpszClassName = L"wxWinUIRuntimeSmokeWindow";
        if ( !::RegisterClassW(&wc) )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "RegisterClass: %lu\n", ::GetLastError());
            return 1;
        }
        // Off-screen and never activated so that running the probe does not
        // disturb whoever is using the machine.
        HWND hwnd = ::CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            wc.lpszClassName, L"wx_winui_runtime_smoke",
            WS_OVERLAPPEDWINDOW,
            -32000, -32000, 800, 600,
            nullptr, nullptr, wc.hInstance, nullptr);
        if ( !hwnd )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "CreateWindowEx: %lu\n", ::GetLastError());
            return 1;
        }
        ::ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        ReportStage("creating XAML island");
        auto source = DesktopWindowXamlSource();
        source.Initialize(winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));
        source.SiteBridge().ResizePolicy(
            winrt::Microsoft::UI::Content::ContentSizePolicy::
                ResizeContentToParentWindow);

        ReportStage("realizing Button, TreeView and TabView");
        auto panel = StackPanel();

        auto button = Button();
        button.Content(winrt::box_value(L"wx"));
        panel.Children().Append(button);

        auto tree = TreeView();
        auto node = TreeViewNode();
        node.Content(winrt::box_value(L"node"));
        tree.RootNodes().Append(node);
        panel.Children().Append(tree);

        auto tabs = TabView();
        auto tab = TabViewItem();
        tab.Header(winrt::box_value(L"tab"));
        tabs.TabItems().Append(tab);
        panel.Children().Append(tabs);

        source.Content(panel);

        const bool loaded = PumpUntil(
            [&]()
            {
                return button.IsLoaded() && tree.IsLoaded() && tabs.IsLoaded();
            },
            15000);

        panel.UpdateLayout();

        if ( !loaded )
        {
            std::fprintf(stderr,
                "wx_winui_runtime_smoke: FAILED: controls did not load within "
                "15s (Button=%d TreeView=%d TabView=%d)\n",
                int(button.IsLoaded()), int(tree.IsLoaded()),
                int(tabs.IsLoaded()));
            PrintPayloadHint();
            rc = 4;
        }
        else if ( button.DesiredSize().Height <= 0 ||
                  tree.DesiredSize().Height <= 0 ||
                  tabs.DesiredSize().Height <= 0 )
        {
            // A control without its template measures to nothing.
            std::fprintf(stderr,
                "wx_winui_runtime_smoke: FAILED: a control measured to an "
                "empty size, its template is missing (Button=%.0fx%.0f "
                "TreeView=%.0fx%.0f TabView=%.0fx%.0f)\n",
                button.DesiredSize().Width, button.DesiredSize().Height,
                tree.DesiredSize().Width, tree.DesiredSize().Height,
                tabs.DesiredSize().Width, tabs.DesiredSize().Height);
            PrintPayloadHint();
            rc = 4;
        }
        else
        {
            std::printf(
                "wx_winui_runtime_smoke: OK: Button %.0fx%.0f, TreeView "
                "%.0fx%.0f, TabView %.0fx%.0f\n",
                button.DesiredSize().Width, button.DesiredSize().Height,
                tree.DesiredSize().Width, tree.DesiredSize().Height,
                tabs.DesiredSize().Width, tabs.DesiredSize().Height);
            rc = 0;
        }

        ReportStage("shutting down");
        source.Close();
        xamlManager.as<winrt::Windows::Foundation::IClosable>().Close();

        // Let XAML process the posted cleanup before tearing the queue down.
        PumpUntil([]() { return false; }, 200);

        ::DestroyWindow(hwnd);
        dispatcherController.ShutdownQueue();
    }
    catch ( const winrt::hresult_error& e )
    {
        ::MddBootstrapShutdown();
        return FailWinRT(5, "runtime smoke", e);
    }

    ::MddBootstrapShutdown();

    if ( rc == 0 )
        std::printf("wx_winui_runtime_smoke: PASSED\n");
    return rc;
}
