/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/xamlmetadata.h
// Purpose:     Application-owned metadata for native XAML component libraries
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_XAMLMETADATA_H_
#define _WX_WINUI_XAMLMETADATA_H_

#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <algorithm>
#include <memory>
#include <vector>

// UI-thread only. The Application owns this registry and therefore also owns
// component providers through the existing XAML shutdown/quarantine boundary.
// Immutable snapshots allow a provider to register another component during a
// lookup without invalidating the current traversal or changing its result.
class wxWinUIXamlMetadataRegistry final
{
public:
    using Provider = winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider;
    using Type = winrt::Microsoft::UI::Xaml::Markup::IXamlType;
    using TypeName = winrt::Windows::UI::Xaml::Interop::TypeName;
    using Xmlns = winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition;
    using Providers = std::vector<Provider>;

    explicit wxWinUIXamlMetadataRegistry(const Provider& primary)
        : m_providers(std::make_shared<const Providers>(Providers{primary}))
    {
        if ( !primary )
            winrt::throw_hresult(E_INVALIDARG);
    }

    bool Register(const Providers& providers)
    {
        if ( m_registering ||
             std::any_of(providers.begin(), providers.end(),
                         [](const Provider& provider) { return !provider; }) )
            return false;

        if ( providers.empty() )
            return true;

        struct Guard
        {
            explicit Guard(bool& flag) : active(flag) { active = true; }
            ~Guard() { active = false; }
            bool& active;
        } guard(m_registering);

        auto candidate = std::make_shared<Providers>(*m_providers);
        for ( const auto& provider : providers )
        {
            const auto present = std::find_if(candidate->begin(), candidate->end(),
                [&provider](const Provider& existing)
                { return winrt::get_abi(existing) == winrt::get_abi(provider); });
            if ( present == candidate->end() )
                candidate->push_back(provider);
        }
        m_providers = std::move(candidate);
        return true;
    }

    Type GetXamlType(const winrt::hstring& name) const
    {
        const auto snapshot = m_providers;
        for ( const auto& provider : *snapshot )
            if ( auto type = provider.GetXamlType(name) )
                return type;
        return nullptr;
    }

    Type GetXamlType(const TypeName& name) const
    {
        const auto snapshot = m_providers;
        for ( const auto& provider : *snapshot )
            if ( auto type = provider.GetXamlType(name) )
                return type;
        return nullptr;
    }

    winrt::com_array<Xmlns> GetXmlnsDefinitions() const
    {
        const auto snapshot = m_providers;
        std::vector<Xmlns> result;
        for ( const auto& provider : *snapshot )
        {
            for ( const auto& entry : provider.GetXmlnsDefinitions() )
            {
                const auto present = std::find_if(result.begin(), result.end(),
                    [&entry](const Xmlns& existing)
                    {
                        return existing.XmlNamespace == entry.XmlNamespace &&
                               existing.Namespace == entry.Namespace;
                    });
                if ( present == result.end() )
                    result.push_back(entry);
            }
        }
        return {result.begin(), result.end()};
    }

private:
    std::shared_ptr<const Providers> m_providers;
    bool m_registering = false;
};

// Non-installed component boundary, not an application extension API. Reject
// foreign Application instances, wrong threads and runtime rundown. Providers
// must keep their implementing modules alive for the entire XAML epoch.
WXDLLIMPEXP_CORE bool wxWinUIRegisterXamlMetadataProviders(
    const wxWinUIXamlMetadataRegistry::Providers& providers) noexcept;

#endif // _WX_WINUI_XAMLMETADATA_H_
