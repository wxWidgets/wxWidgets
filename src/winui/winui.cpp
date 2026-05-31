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

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/module.h"
    #include "wx/string.h"
#endif

#include "wx/msw/wrapwin.h"

#include <objbase.h>

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
bool gs_winuiContentPreTranslateLogShown = false;
winrt::Microsoft::UI::Dispatching::DispatcherQueueController
    gs_winuiDispatcherQueueController{ nullptr };
winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager
    gs_winuiXamlManager{ nullptr };
winrt::Microsoft::UI::Xaml::Application gs_winuiApplication{ nullptr };
bool gs_winuiOwnsXamlManager = false;

// ContentPreTranslateMessage() routes input (including character keys) to the
// focused WinUI island.  It lives in a Windows App SDK runtime DLL that is only
// on the search path after the bootstrapper has run, so it must be resolved
// dynamically rather than statically linked (a static import would make the
// process fail to start because the DLL is absent at load time).
using wxWinUIContentPreTranslateMessage = BOOL (WINAPI *)(const MSG *);
wxWinUIContentPreTranslateMessage gs_winuiContentPreTranslateMessage = nullptr;

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

void wxWinUILogException(const char *what, const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
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
        wxWinUILogException("WinUI loose XAML resources initialization", e);
        return false;
    }
}

bool wxWinUIEnsureDispatcherQueue()
{
    try
    {
        using namespace winrt::Microsoft::UI::Dispatching;

        if ( DispatcherQueue::GetForCurrentThread() )
            return true;

        gs_winuiDispatcherQueueController =
            DispatcherQueueController::CreateOnCurrentThread();

        if ( gs_winuiDispatcherQueueController )
            return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DispatcherQueueController initialization", e);
        return false;
    }

    wxLogWarning("DispatcherQueueController initialization failed.");
    return false;
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
        gs_winuiOwnsXamlManager = true;
        return static_cast<bool>(gs_winuiXamlManager);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WindowsXamlManager initialization", e);
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

        if ( Application::Current() )
            return true;

        gs_winuiApplication =
            winrt::make<wxWinUIApplication>().as<Application>();
        return static_cast<bool>(gs_winuiApplication);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI application creation", e);
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
        auto app = winrt::Microsoft::UI::Xaml::Application::Current();
        auto resources = app ? app.Resources() : nullptr;
        if ( resources && wxWinUIAppendLooseXamlResources(resources) )
            return true;

        wxWinUILogException("WinUI application resources initialization", e);
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

} // namespace

bool wxWinUI3Initialize()
{
    if ( gs_winuiBootstrapInitialized )
        return wxWinUIEnsureDispatcherQueue();

    bool comInitialized = false;
    try
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        comInitialized = true;
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( e.code() != RPC_E_CHANGED_MODE )
        {
            wxWinUILogException("winrt::init_apartment", e);
            return false;
        }
    }

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
        if ( comInitialized )
            winrt::uninit_apartment();
        return false;
    }

    if ( !wxWinUIEnsureDispatcherQueue() )
    {
        ::MddBootstrapShutdown();
        if ( comInitialized )
            winrt::uninit_apartment();
        return false;
    }

    // The application object (which provides the XAML metadata provider) must
    // exist before the XAML manager spins up a default application of its own,
    // otherwise XamlControlsResources cannot be constructed and the controls
    // render without their templates.
    if ( !wxWinUIEnsureApplication() )
    {
        if ( gs_winuiDispatcherQueueController )
        {
            gs_winuiDispatcherQueueController.ShutdownQueue();
            gs_winuiDispatcherQueueController = nullptr;
        }
        ::MddBootstrapShutdown();
        if ( comInitialized )
            winrt::uninit_apartment();
        return false;
    }

    if ( !wxWinUIEnsureXamlManager() )
    {
        gs_winuiApplication = nullptr;
        if ( gs_winuiDispatcherQueueController )
        {
            gs_winuiDispatcherQueueController.ShutdownQueue();
            gs_winuiDispatcherQueueController = nullptr;
        }
        ::MddBootstrapShutdown();
        if ( comInitialized )
            winrt::uninit_apartment();
        return false;
    }

    // Theme resources can only be loaded once the framework is initialized by
    // the XAML manager above; do it now that our application is current.
    if ( !wxWinUIEnsureApplicationResources() )
    {
        if ( gs_winuiXamlManager )
        {
            if ( gs_winuiOwnsXamlManager )
                gs_winuiXamlManager.as<winrt::Windows::Foundation::IClosable>().Close();
            gs_winuiXamlManager = nullptr;
            gs_winuiOwnsXamlManager = false;
        }
        gs_winuiApplication = nullptr;
        if ( gs_winuiDispatcherQueueController )
        {
            gs_winuiDispatcherQueueController.ShutdownQueue();
            gs_winuiDispatcherQueueController = nullptr;
        }
        ::MddBootstrapShutdown();
        if ( comInitialized )
            winrt::uninit_apartment();
        return false;
    }

    gs_winuiBootstrapInitialized = true;
    gs_winuiComInitialized = comInitialized;
    return true;
}

void wxWinUI3Uninitialize()
{
    if ( gs_winuiXamlManager )
    {
        if ( gs_winuiOwnsXamlManager )
        {
            try
            {
                gs_winuiXamlManager.as<winrt::Windows::Foundation::IClosable>().Close();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WindowsXamlManager::Close", e);
            }
        }

        gs_winuiXamlManager = nullptr;
        gs_winuiOwnsXamlManager = false;
    }

    gs_winuiApplication = nullptr;

    if ( gs_winuiDispatcherQueueController )
    {
        try
        {
            gs_winuiDispatcherQueueController.ShutdownQueue();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("DispatcherQueueController::ShutdownQueue", e);
        }

        gs_winuiDispatcherQueueController = nullptr;
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
}

bool wxWinUI3PreTranslateMessage(WXMSG *msg)
{
    // Give the focused WinUI island a chance to process the message; this is
    // what routes keyboard input (including character keys) to XAML controls.
    if ( !gs_winuiBootstrapInitialized )
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
