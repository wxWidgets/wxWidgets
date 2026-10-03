/////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/toolkit-activation-probe.h
// Purpose:     Opt-in, standalone native Toolkit consumption experiment
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_TESTS_WINUI_TOOLKIT_ACTIVATION_PROBE_H
#define WX_TESTS_WINUI_TOOLKIT_ACTIVATION_PROBE_H

#include <cmath>
#include <string>
#include <type_traits>
#include <vector>
#include <activation.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/XamlToolkit.Labs.WinUI.h>

// This header is compiled only into the opt-in standalone probe. It must not
// install an activation hook, register COM classes, or change wxcore state.
namespace wxWinUIToolkitProbe
{
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Markup;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;

class State final
{
public:
    void Initialize()
    {
        std::wstring executable(32768, L'\0');
        const DWORD length = ::GetModuleFileNameW(
            nullptr, executable.data(), static_cast<DWORD>(executable.size()));
        if ( !length || length >= executable.size() )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
        executable.resize(length);
        const auto separator = executable.find_last_of(L"\\/");
        if ( separator == std::wstring::npos )
            winrt::throw_hresult(E_UNEXPECTED);
        const std::wstring directory = executable.substr(0, separator + 1);

        // Keep these reference counts until process exit: XAML and C++/WinRT
        // may retain factories past the last local control. This test does not
        // claim hot-unload safety, and never calls FreeLibrary on live code.
        const wchar_t* const names[] = {
            L"XamlToolkit.WinUI.dll",
            L"XamlToolkit.WinUI.Converters.dll",
            L"XamlToolkit.WinUI.Helpers.dll",
            L"XamlToolkit.WinUI.Animations.dll",
            L"XamlToolkit.WinUI.Interactivity.dll",
            L"Microsoft.Graphics.Canvas.dll",
            L"XamlToolkit.WinUI.Controls.dll",
            L"XamlToolkit.Labs.WinUI.dll"
        };
        for ( const auto name : names )
        {
            const std::wstring path = directory + name;
            HMODULE module = ::LoadLibraryExW(
                path.c_str(), nullptr,
                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
            if ( !module )
            {
                const DWORD error = ::GetLastError();
                std::fprintf(stderr,
                    "wx_winui_toolkit_probe: LoadLibraryEx(%ls): %lu\n",
                    path.c_str(), error);
                winrt::throw_hresult(HRESULT_FROM_WIN32(error));
            }
            m_modules.push_back(module);
            std::printf("wx_winui_toolkit_probe: {\"event\":\"module\","
                        "\"name\":\"%ls\",\"result\":\"loaded\"}\n", name);
        }

        AddProvider(m_modules[6],
                    L"XamlToolkit.WinUI.Controls.XamlMetaDataProvider");
        AddProvider(m_modules[7],
                    L"XamlToolkit.Labs.WinUI.XamlMetaDataProvider");
        std::printf("wx_winui_toolkit_probe: {\"event\":\"dependency\","
                    "\"toolkit_version\":\"0.8.0-alpha\","
                    "\"win2d_version\":\"1.4.0\","
                    "\"activation\":\"direct-dll-factory\"}\n");
    }

    MUXM::IXamlType GetXamlType(winrt::hstring const& name) const
    {
        for ( const auto& provider : m_providers )
        {
            if ( auto type = provider.GetXamlType(name) )
                return type;
        }
        return nullptr;
    }

    MUXM::IXamlType GetXamlType(
        winrt::Windows::UI::Xaml::Interop::TypeName const& name) const
    {
        for ( const auto& provider : m_providers )
        {
            if ( auto type = provider.GetXamlType(name) )
                return type;
        }
        return nullptr;
    }

    void AppendXmlnsDefinitions(std::vector<MUXM::XmlnsDefinition>& result) const
    {
        for ( const auto& provider : m_providers )
        {
            const auto entries = provider.GetXmlnsDefinitions();
            result.insert(result.end(), entries.begin(), entries.end());
        }
    }

    void Attach(MUXC::StackPanel const& panel)
    {
        if ( GetXamlType(L"wxWinUI.ToolkitProbe.NonexistentClass") )
            winrt::throw_hresult(E_UNEXPECTED);

        auto ribbon = Create<Labs::Ribbon>(L"XamlToolkit.Labs.WinUI.Ribbon");
        ribbon.Width(600);
        auto group = Create<Labs::RibbonGroup>(
            L"XamlToolkit.Labs.WinUI.RibbonGroup");
        group.Label(L"Native commands");
        MUXC::Button command;
        command.Content(winrt::box_value(L"Toolkit probe"));
        group.Content(command);
        ribbon.Items().Append(group);

        auto collapsible = Create<Labs::RibbonCollapsibleGroup>(
            L"XamlToolkit.Labs.WinUI.RibbonCollapsibleGroup");
        collapsible.Label(L"Adaptive group");
        MUXC::Button second;
        second.Content(winrt::box_value(L"Second command"));
        collapsible.Content(second);
        ribbon.Items().Append(collapsible);

        // Attach the independently constructed Ribbon before exercising the
        // table. A table activation failure must not erase evidence about the
        // Ribbon's actual templates, Loaded state and geometry.
        panel.Children().Append(ribbon);
        m_controls = { ribbon, group, collapsible };
        m_ribbonAttached = true;

        auto header = Create<Labs::DataTable>(
            L"XamlToolkit.Labs.WinUI.DataTable");
        header.ColumnSpacing(8);
        const double spacing = header.ColumnSpacing();
        m_propertyContractPassed = spacing == 8;
        std::printf("wx_winui_toolkit_probe: {\"event\":\"property-contract\","
                    "\"class\":\"XamlToolkit.Labs.WinUI.DataTable\","
                    "\"property\":\"ColumnSpacing\",\"expected\":8,"
                    "\"actual\":%.17g,\"passed\":%s}\n", spacing,
                    m_propertyContractPassed ? "true" : "false");
        // Preserve a bad property round-trip as a failed consumption result,
        // but continue to gather independent native presentation evidence.
        for ( const auto label : { L"Name", L"Value" } )
        {
            auto column = Create<Labs::DataColumn>(
                L"XamlToolkit.Labs.WinUI.DataColumn");
            MUXC::TextBlock text;
            text.Text(label);
            column.Content(text);
            column.CanResize(true);
            column.DesiredWidth(MUX::GridLengthHelper::FromPixels(220));
            header.Children().Append(column);
        }
        auto row = Create<Labs::DataRow>(L"XamlToolkit.Labs.WinUI.DataRow");
        for ( const auto value : { L"Native data row", L"42" } )
        {
            MUXC::TextBlock text;
            text.Text(value);
            row.Children().Append(text);
        }
        MUXC::ListView list;
        list.Width(600);
        list.Height(120);
        list.Header(header);
        list.Items().Append(row);
        panel.Children().Append(list);
        m_dataTableAttached = true;
        for ( const auto& child : header.Children() )
            m_controls.push_back(child.as<MUXC::Control>());
    }

    bool IsLoaded() const
    {
        if ( m_peers.empty() )
            return false;
        for ( const auto& peer : m_peers )
        {
            if ( !peer.element.IsLoaded() )
                return false;
        }
        return true;
    }

    bool HasPeers() const
    {
        return !m_peers.empty();
    }

    bool MetadataComplete() const
    {
        return m_metadataComplete;
    }

    bool VerifyPresentation() const
    {
        bool passed = IsLoaded();
        bool ribbonPassed = m_ribbonAttached;
        bool tablePassed = m_dataTableAttached;
        bool templatesPassed = !m_controls.empty();
        for ( const auto& peer : m_peers )
        {
            const double width = peer.element.ActualWidth();
            const double height = peer.element.ActualHeight();
            const bool valid = peer.element.IsLoaded() &&
                std::isfinite(width) && std::isfinite(height) &&
                width > 0 && height > 0;
            passed = passed && valid;
            if ( std::wstring(peer.name).find(L".Ribbon") != std::wstring::npos )
                ribbonPassed = ribbonPassed && valid;
            else
                tablePassed = tablePassed && valid;
            std::printf("wx_winui_toolkit_probe: {\"event\":\"presentation\","
                        "\"class\":\"%ls\",\"loaded\":%s,"
                        "\"width\":%.3f,\"height\":%.3f,\"passed\":%s}\n",
                        peer.name, peer.element.IsLoaded() ? "true" : "false",
                        width, height, valid ? "true" : "false");
        }
        for ( const auto& control : m_controls )
        {
            const bool hasTemplate = control.Template() &&
                winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                    GetChildrenCount(control) > 0;
            passed = passed && hasTemplate;
            templatesPassed = templatesPassed && hasTemplate;
            const auto name = winrt::get_class_name(control);
            if ( std::wstring(name).find(L".Ribbon") != std::wstring::npos )
                ribbonPassed = ribbonPassed && hasTemplate;
            else
                tablePassed = tablePassed && hasTemplate;
            std::printf("wx_winui_toolkit_probe: {\"event\":\"template\","
                        "\"class\":\"%ls\",\"passed\":%s}\n", name.c_str(),
                        hasTemplate ? "true" : "false");
        }
        std::printf("wx_winui_toolkit_probe: {\"event\":\"family-presentation\","
                    "\"family\":\"ribbon\",\"passed\":%s}\n",
                    ribbonPassed ? "true" : "false");
        std::printf("wx_winui_toolkit_probe: {\"event\":\"family-presentation\","
                    "\"family\":\"data-table\",\"passed\":%s}\n",
                    tablePassed ? "true" : "false");
        std::printf("wx_winui_toolkit_probe: {\"event\":\"templates\","
                    "\"result\":\"%s\"}\n",
                    templatesPassed ? "passed" : "failed");
        return passed && ribbonPassed && tablePassed && m_propertyContractPassed;
    }

private:
    static winrt::Windows::Foundation::IInspectable ActivateDirect(
        HMODULE module, const wchar_t* name)
    {
        using GetFactory = HRESULT (WINAPI*)(HSTRING, ::IActivationFactory**);
        const auto getFactory = reinterpret_cast<GetFactory>(
            ::GetProcAddress(module, "DllGetActivationFactory"));
        if ( !getFactory )
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND));
        winrt::com_ptr<::IActivationFactory> factory;
        const winrt::hstring className(name);
        winrt::check_hresult(getFactory(
            static_cast<HSTRING>(winrt::get_abi(className)), factory.put()));
        winrt::Windows::Foundation::IInspectable instance;
        winrt::check_hresult(factory->ActivateInstance(
            reinterpret_cast<::IInspectable**>(winrt::put_abi(instance))));
        return instance;
    }

    void AddProvider(HMODULE module, const wchar_t* name)
    {
        const auto instance = ActivateDirect(module, name);
        m_providers.push_back(instance.as<MUXM::IXamlMetadataProvider>());
        std::printf("wx_winui_toolkit_probe: {\"event\":\"provider\","
                    "\"class\":\"%ls\",\"result\":\"activated\"}\n", name);
    }

    template <typename T>
    T Create(const wchar_t* name)
    {
        const winrt::hstring fullName(name);
        const auto byName = GetXamlType(fullName);
        const winrt::Windows::UI::Xaml::Interop::TypeName typeName{
            fullName, winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata };
        const auto byType = GetXamlType(typeName);
        const auto nameUnderlying = byName ? byName.UnderlyingType() :
            winrt::Windows::UI::Xaml::Interop::TypeName{};
        const auto typeUnderlying = byType ? byType.UnderlyingType() :
            winrt::Windows::UI::Xaml::Interop::TypeName{};
        std::printf("wx_winui_toolkit_probe: {\"event\":\"metadata\","
                    "\"class\":\"%ls\",\"by_name\":%s,\"by_type\":%s,"
                    "\"by_name_underlying\":\"%ls\",\"by_type_underlying\":\"%ls\","
                    "\"by_name_kind\":%d,\"by_type_kind\":%d}\n", name,
                    byName ? "true" : "false", byType ? "true" : "false",
                    nameUnderlying.Name.c_str(), typeUnderlying.Name.c_str(),
                    static_cast<int>(nameUnderlying.Kind),
                    static_cast<int>(typeUnderlying.Kind));
        const bool metadataComplete = byName && byType &&
             nameUnderlying.Name == fullName &&
             typeUnderlying.Name == fullName;
        T peer{ nullptr };
        if ( !metadataComplete )
        {
            m_metadataComplete = false;
            std::printf("wx_winui_toolkit_probe: {\"event\":\"metadata-gate\","
                        "\"class\":\"%ls\",\"result\":\"failed\"}\n", name);
            // These two classes are Panel subclasses, with no control
            // template. The exact pinned winmd projects their constructor as
            // IActivationFactory::ActivateInstance, independently of XAML
            // metadata. Do not extend this path to templated controls.
            if constexpr ( std::is_same_v<T, Labs::DataTable> ||
                           std::is_same_v<T, Labs::DataRow> )
            {
                const auto instance = ActivateDirect(m_modules[7], name);
                const auto runtimeName = winrt::get_class_name(instance);
                peer = instance.as<T>();
                std::printf("wx_winui_toolkit_probe: {\"event\":\"direct-panel\","
                            "\"class\":\"%ls\",\"runtime_class\":\"%ls\","
                            "\"interface_matched\":true,\"class_matched\":%s}\n",
                            name, runtimeName.c_str(),
                            runtimeName == fullName ? "true" : "false");
                if ( runtimeName != fullName )
                    winrt::throw_hresult(E_NOINTERFACE);
            }
            else
            {
                std::fprintf(stderr,
                    "wx_winui_toolkit_probe: incomplete metadata for %ls\n", name);
                winrt::throw_hresult(E_NOINTERFACE);
            }
        }
        else
        {
            peer = byName.ActivateInstance().as<T>();
        }
        m_peers.push_back({ name, peer.template as<MUX::FrameworkElement>() });
        std::printf("wx_winui_toolkit_probe: {\"event\":\"peer\","
                    "\"class\":\"%ls\",\"result\":\"created\"}\n", name);
        return peer;
    }

    struct Peer
    {
        const wchar_t* name;
        MUX::FrameworkElement element;
    };
    std::vector<HMODULE> m_modules;
    std::vector<MUXM::IXamlMetadataProvider> m_providers;
    std::vector<Peer> m_peers;
    std::vector<MUXC::Control> m_controls;
    bool m_ribbonAttached = false;
    bool m_dataTableAttached = false;
    bool m_metadataComplete = true;
    bool m_propertyContractPassed = false;
};
} // namespace wxWinUIToolkitProbe

#endif // WX_TESTS_WINUI_TOOLKIT_ACTIVATION_PROBE_H
