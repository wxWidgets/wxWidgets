/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribbontoolkitruntime.cpp
// Purpose:     Private module-local activation of native Toolkit Ribbon peers
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/thread.h"
#include "wx/msw/wrapwin.h"
#include "ribbontoolkitruntime.h"
#include "../winui/xamlmetadata.h"

#include <activation.h>
#include <array>
#include <cstring>
#include <string>
#include <utility>
#include <winrt/Microsoft.UI.Xaml.h>

namespace
{
namespace Xaml = winrt::Microsoft::UI::Xaml;
namespace Markup = Xaml::Markup;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;
namespace Foundation = winrt::Windows::Foundation;

using GetFactory = HRESULT (WINAPI*)(HSTRING, ::IActivationFactory**);
constexpr unsigned ModuleCount = 8;
constexpr unsigned ControlsModule = 6;
constexpr unsigned LabsModule = 7;
const wchar_t* const ModuleNames[ModuleCount] = {
    L"XamlToolkit.WinUI.dll",
    L"XamlToolkit.WinUI.Converters.dll",
    L"XamlToolkit.WinUI.Helpers.dll",
    L"XamlToolkit.WinUI.Animations.dll",
    L"XamlToolkit.WinUI.Interactivity.dll",
    L"Microsoft.Graphics.Canvas.dll",
    L"XamlToolkit.WinUI.Controls.dll",
    L"XamlToolkit.Labs.WinUI.dll"
};

struct Modules final
{
    // Every successful LoadLibrary reference is balanced. OS pins deliberately
    // survive these releases and also protect types/controls escaping providers.
    ~Modules()
    {
        for ( unsigned i = loaded; i != 0; --i )
            ::FreeLibrary(handles[i - 1]);
    }

    std::array<HMODULE, ModuleCount> handles{};
    std::array<GetFactory, ModuleCount> factories{};
    unsigned loaded = 0;
    unsigned pinned = 0;
    const char* stage = "resolve-payload-directory";
    std::wstring detail;
    std::wstring directory;
};

void FailIfFalse(bool success)
{
    if ( !success )
    {
        const DWORD error = ::GetLastError();
        winrt::throw_hresult(error ? HRESULT_FROM_WIN32(error) : E_FAIL);
    }
}

std::wstring ModulePath(HMODULE module)
{
    std::wstring path(32768, L'\0');
    const DWORD size = ::GetModuleFileNameW(
        module, path.data(), static_cast<DWORD>(path.size()));
    FailIfFalse(size != 0);
    if ( size >= path.size() )
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    path.resize(size);
    return path;
}

std::wstring FinalPath(const std::wstring& path, bool directory)
{
    HANDLE file = ::CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, directory ? FILE_FLAG_BACKUP_SEMANTICS : 0, nullptr);
    FailIfFalse(file != INVALID_HANDLE_VALUE);
    struct CloseHandle
    {
        HANDLE value;
        ~CloseHandle() { ::CloseHandle(value); }
    } close{file};
    BY_HANDLE_FILE_INFORMATION info{};
    FailIfFalse(::GetFileInformationByHandle(file, &info) != FALSE);
    if ( ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) != directory )
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
    std::wstring result(32768, L'\0');
    const DWORD size = ::GetFinalPathNameByHandleW(file, result.data(),
        static_cast<DWORD>(result.size()), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    FailIfFalse(size != 0);
    if ( size >= result.size() )
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    result.resize(size);
    return result;
}

bool SamePath(const std::wstring& lhs, const std::wstring& rhs)
{
    return ::CompareStringOrdinal(lhs.c_str(), -1, rhs.c_str(), -1, TRUE) == CSTR_EQUAL;
}

std::wstring PayloadDirectory(const wxString& requested)
{
    std::wstring path;
    if ( requested.empty() )
    {
        path = ModulePath(nullptr);
        const auto separator = path.find_last_of(L"\\/");
        if ( separator == std::wstring::npos )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
        path.resize(separator + 1);
    }
    else
    {
        path = requested.ToStdWstring();
        const bool unc = path.size() > 2 && path[0] == L'\\' && path[1] == L'\\';
        const bool drive = path.size() > 2 && path[1] == L':' &&
            (path[2] == L'\\' || path[2] == L'/');
        if ( (!unc && !drive) || path.find(L'\0') != std::wstring::npos )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
    }
    path = FinalPath(path, true);
    if ( path.back() != L'\\' )
        path.push_back(L'\\');
    return path;
}

void LoadAndPin(Modules& modules)
{
    std::array<std::wstring, ModuleCount> paths;
    modules.stage = "validate-payload-files";
    for ( unsigned i = 0; i != ModuleCount; ++i )
    {
        modules.detail = modules.directory + ModuleNames[i];
        paths[i] = FinalPath(modules.detail, false);
        // A payload file symlink/junction must not silently select a component
        // outside the chosen, already-canonical payload directory.
        if ( !SamePath(paths[i], modules.detail) )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
    }
    for ( unsigned i = 0; i != ModuleCount; ++i )
    {
        modules.stage = "load-module";
        modules.detail = paths[i];
        HMODULE module = ::LoadLibraryExW(paths[i].c_str(), nullptr,
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
        FailIfFalse(module != nullptr);
        modules.handles[i] = module;
        ++modules.loaded;
        modules.stage = "validate-loaded-module-path";
        if ( !SamePath(FinalPath(ModulePath(module), false), paths[i]) )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS));
        modules.stage = "resolve-DllGetActivationFactory";
        modules.factories[i] = reinterpret_cast<GetFactory>(
            ::GetProcAddress(module, "DllGetActivationFactory"));
        if ( !modules.factories[i] )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND));
    }
    // No factory or provider has been called before all eight paths/exports
    // passed. From this point an error may leave a partially pinned closure.
    for ( unsigned i = 0; i != ModuleCount; ++i )
    {
        modules.stage = "pin-module-until-process-exit";
        modules.detail = paths[i];
        HMODULE pinned = nullptr;
        FailIfFalse(::GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(modules.factories[i]), &pinned) != FALSE);
        ++modules.pinned;
        if ( pinned != modules.handles[i] )
            winrt::throw_hresult(E_UNEXPECTED);
    }
}

Foundation::IActivationFactory Factory(const Modules& modules,
                                       unsigned index, const wchar_t* name)
{
    Foundation::IActivationFactory factory{nullptr};
    const winrt::hstring className(name);
    winrt::check_hresult(modules.factories[index](
        static_cast<HSTRING>(winrt::get_abi(className)),
        reinterpret_cast<::IActivationFactory**>(winrt::put_abi(factory))));
    if ( !factory )
        winrt::throw_hresult(E_NOINTERFACE);
    return factory;
}

void Report(wxRibbonToolkitFailure* failure, winrt::hresult code,
            const char* stage, const Modules* modules = nullptr) noexcept
{
    if ( !failure )
        return;
    failure->code = code;
    failure->stage = stage;
    failure->loadedModules = modules ? modules->loaded : 0;
    failure->pinnedModules = modules ? modules->pinned : 0;
    try
    {
        failure->detail = modules ? wxString(modules->detail) : wxString();
    }
    catch ( ... )
    {
        // Reporting an allocation failure must not escape a Try* boundary.
    }
}
} // namespace

struct wxRibbonToolkitRuntimeState final
{
    void CheckThread() const
    {
        if ( ::GetCurrentThreadId() != thread )
            winrt::throw_hresult(RPC_E_WRONG_THREAD);
    }

    void CheckCurrent() const
    {
        CheckThread();
        // This registry boundary rejects rundown/quarantine before any WinRT
        // call. An empty batch is only a liveness check, not provider adoption.
        if ( !wxWinUIRegisterXamlMetadataProviders({}) )
            winrt::throw_hresult(RO_E_CLOSED);
        const auto owner = application.get();
        const auto current = Xaml::Application::Current();
        if ( !owner || !current || winrt::get_abi(owner) != winrt::get_abi(current) )
            winrt::throw_hresult(RO_E_CLOSED);
    }

    Markup::IXamlType RibbonType(const wchar_t* name) const
    {
        const winrt::hstring fullName(name);
        const auto type = providers[1].GetXamlType(fullName);
        if ( !type || type.UnderlyingType().Name != fullName )
            winrt::throw_hresult(E_NOINTERFACE);
        return type;
    }

    // Members retire in reverse order: all provider COM is released before
    // the native leases. Neither this state nor providers own Application.
    std::shared_ptr<Modules> modules;
    DWORD thread = ::GetCurrentThreadId();
    winrt::weak_ref<Xaml::Application> application;
    std::array<Markup::IXamlMetadataProvider, 2> providers{nullptr, nullptr};
};

namespace
{
// Only weak state is process-global. Application registry wrappers retain the
// state while registered, and an adapter may retain it after registry rundown.
std::weak_ptr<wxRibbonToolkitRuntimeState> gs_runtime;
bool gs_acquiring = false;

struct Provider : winrt::implements<Provider,
    Markup::IXamlMetadataProvider, winrt::non_agile>
{
    Provider(std::shared_ptr<wxRibbonToolkitRuntimeState> state, unsigned index)
        : m_state(std::move(state)), m_index(index) {}

    Markup::IXamlType GetXamlType(const winrt::hstring& name)
    {
        m_state->CheckThread();
        return m_state->providers[m_index].GetXamlType(name);
    }
    Markup::IXamlType GetXamlType(
        const winrt::Windows::UI::Xaml::Interop::TypeName& name)
    {
        m_state->CheckThread();
        return m_state->providers[m_index].GetXamlType(name);
    }
    winrt::com_array<Markup::XmlnsDefinition> GetXmlnsDefinitions()
    {
        m_state->CheckThread();
        return m_state->providers[m_index].GetXmlnsDefinitions();
    }

private:
    std::shared_ptr<wxRibbonToolkitRuntimeState> m_state;
    unsigned m_index;
};

template<typename T, typename Activate>
bool TryCreate(const std::shared_ptr<wxRibbonToolkitRuntimeState>& state,
               const wchar_t* name, T& result, wxRibbonToolkitFailure* failure,
               Activate activate) noexcept
{
    const char* stage = "validate-application-epoch";
    try
    {
        state->CheckCurrent();
        stage = "validate-ribbon-metadata";
        state->RibbonType(name);
        stage = "activate-ribbon-peer";
        T candidate = activate(*state->modules, name);
        if ( !candidate || winrt::get_class_name(candidate) != winrt::hstring(name) )
            winrt::throw_hresult(E_NOINTERFACE);
        stage = "validate-application-after-activation";
        state->CheckCurrent();
        result = std::move(candidate);
        Report(failure, S_OK, "created", state->modules.get());
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        Report(failure, e.code(), stage, state->modules.get());
    }
    catch ( const std::bad_alloc& )
    {
        Report(failure, E_OUTOFMEMORY, stage, state->modules.get());
    }
    catch ( ... )
    {
        Report(failure, E_FAIL, stage, state->modules.get());
    }
    return false;
}
} // namespace

bool wxRibbonToolkitRuntime::TryAcquire(
    std::shared_ptr<wxRibbonToolkitRuntime>& result,
    wxRibbonToolkitFailure* failure, const wxString& payloadDirectory) noexcept
{
    // Reject before touching a weak WinRT reference, Application::Current,
    // factory or any process-global UI-thread-only cache.
    if ( !wxIsMainThread() )
    {
        Report(failure, RPC_E_WRONG_THREAD, "require-ui-thread");
        return false;
    }
    if ( gs_acquiring )
    {
        Report(failure, HRESULT_FROM_WIN32(ERROR_BUSY), "reentrant-acquisition");
        return false;
    }
    struct Guard
    {
        Guard() { gs_acquiring = true; }
        ~Guard() { gs_acquiring = false; }
    } guard;

    std::shared_ptr<Modules> modules;
    const char* stage = "validate-running-wx-application";
    try
    {
        if ( !wxWinUIRegisterXamlMetadataProviders({}) )
            winrt::throw_hresult(RO_E_CLOSED);
        const auto application = Xaml::Application::Current();
        if ( !application )
            winrt::throw_hresult(RO_E_CLOSED);
        stage = "resolve-payload-directory";
        const std::wstring directory = PayloadDirectory(payloadDirectory);
        if ( auto existing = gs_runtime.lock() )
        {
            existing->CheckThread();
            const auto previous = existing->application.get();
            if ( previous && winrt::get_abi(previous) == winrt::get_abi(application) )
            {
                modules = existing->modules;
                if ( !SamePath(modules->directory, directory) )
                    winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS));
                existing->CheckCurrent();
                result = std::shared_ptr<wxRibbonToolkitRuntime>(
                    new wxRibbonToolkitRuntime(std::move(existing)));
                Report(failure, S_OK, "reused", modules.get());
                return true;
            }
        }
        modules = std::make_shared<Modules>();
        modules->directory = directory;
        stage = "load-and-pin-dependency-closure";
        LoadAndPin(*modules);
        auto state = std::make_shared<wxRibbonToolkitRuntimeState>();
        state->modules = modules;
        state->application = winrt::make_weak(application);
        stage = "activate-controls-metadata-provider";
        modules->detail = L"XamlToolkit.WinUI.Controls.XamlMetaDataProvider";
        state->providers[0] = Factory(*modules, ControlsModule,
            modules->detail.c_str()).ActivateInstance<Markup::IXamlMetadataProvider>();
        stage = "activate-labs-metadata-provider";
        modules->detail = L"XamlToolkit.Labs.WinUI.XamlMetaDataProvider";
        state->providers[1] = Factory(*modules, LabsModule,
            modules->detail.c_str()).ActivateInstance<Markup::IXamlMetadataProvider>();
        stage = "validate-ribbon-metadata";
        for ( const auto name : {L"XamlToolkit.Labs.WinUI.Ribbon",
                                L"XamlToolkit.Labs.WinUI.RibbonGroup",
                                L"XamlToolkit.Labs.WinUI.RibbonCollapsibleGroup"} )
        {
            modules->detail = name;
            state->RibbonType(name);
        }
        stage = "register-metadata-provider-batch";
        state->CheckCurrent();
        // Construct the adapter before publication: allocation failure must
        // not adopt providers and then falsely report that no context exists.
        auto candidate = std::shared_ptr<wxRibbonToolkitRuntime>(
            new wxRibbonToolkitRuntime(state));
        wxWinUIXamlMetadataRegistry::Providers providers{
            winrt::make<Provider>(state, 0), winrt::make<Provider>(state, 1)};
        if ( !wxWinUIRegisterXamlMetadataProviders(providers) )
            winrt::throw_hresult(RO_E_CLOSED);
        modules->detail.clear();
        gs_runtime = state;
        result = std::move(candidate);
        Report(failure, S_OK, "registered", modules.get());
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        Report(failure, e.code(), modules &&
            std::strcmp(stage, "load-and-pin-dependency-closure") == 0
                ? modules->stage : stage, modules.get());
    }
    catch ( const std::bad_alloc& )
    {
        Report(failure, E_OUTOFMEMORY, stage, modules.get());
    }
    catch ( ... )
    {
        Report(failure, E_FAIL, stage, modules.get());
    }
    return false;
}

bool wxRibbonToolkitRuntime::TryCreateRibbon(
    Labs::Ribbon& result, wxRibbonToolkitFailure* failure) const noexcept
{
    return TryCreate(m_state, L"XamlToolkit.Labs.WinUI.Ribbon", result, failure,
        [](const Modules& modules, const wchar_t* name)
        { return Factory(modules, LabsModule, name).ActivateInstance<Labs::Ribbon>(); });
}

bool wxRibbonToolkitRuntime::TryCreateGroup(
    Labs::RibbonGroup& result, wxRibbonToolkitFailure* failure) const noexcept
{
    return TryCreate(m_state, L"XamlToolkit.Labs.WinUI.RibbonGroup", result, failure,
        [](const Modules& modules, const wchar_t* name)
        {
            // RibbonGroup is composable: its exact pinned WinMD projects the
            // IRibbonGroupFactory constructor, not ActivateInstance().
            Foundation::IInspectable inner{nullptr};
            return Factory(modules, LabsModule, name).as<Labs::IRibbonGroupFactory>()
                .CreateInstance(nullptr, inner);
        });
}

bool wxRibbonToolkitRuntime::TryCreateCollapsibleGroup(
    Labs::RibbonCollapsibleGroup& result, wxRibbonToolkitFailure* failure) const noexcept
{
    return TryCreate(m_state, L"XamlToolkit.Labs.WinUI.RibbonCollapsibleGroup", result, failure,
        [](const Modules& modules, const wchar_t* name)
        { return Factory(modules, LabsModule, name).ActivateInstance<Labs::RibbonCollapsibleGroup>(); });
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RIBBON && WXWINUI_RIBBON_TOOLKIT
