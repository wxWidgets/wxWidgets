/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuixamlmetadata.cpp
// Purpose:     Native component metadata resolution and runtime ownership
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "../../src/winui/private.h"
#include "../../src/winui/xamlmetadata.h"
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>

#include <map>
#include <thread>

namespace
{
using Registry = wxWinUIXamlMetadataRegistry;
using MUXApplication = winrt::Microsoft::UI::Xaml::Application;

struct MetadataState
{
    std::map<std::wstring, std::wstring> names;
    std::vector<Registry::Xmlns> xmlns;
    std::function<void()> onLookup;
    std::function<void()> onXmlns;
    unsigned stringQueries = 0;
    unsigned typeQueries = 0;
    bool fail = false;
};

struct MetadataProvider : winrt::implements<MetadataProvider, Registry::Provider>
{
    explicit MetadataProvider(std::shared_ptr<MetadataState> state)
        : m_state(std::move(state))
    {
    }

    Registry::Type GetXamlType(const winrt::hstring& name)
    {
        ++m_state->stringQueries;
        return Lookup(name);
    }

    Registry::Type GetXamlType(const Registry::TypeName& name)
    {
        ++m_state->typeQueries;
        return Lookup(name.Name);
    }

    winrt::com_array<Registry::Xmlns> GetXmlnsDefinitions()
    {
        if ( m_state->onXmlns )
            m_state->onXmlns();
        return {m_state->xmlns.begin(), m_state->xmlns.end()};
    }

private:
    Registry::Type Lookup(const winrt::hstring& name)
    {
        if ( m_state->onLookup )
            m_state->onLookup();
        if ( m_state->fail )
            winrt::throw_hresult(E_FAIL);
        const auto found = m_state->names.find(std::wstring(name));
        return found == m_state->names.end()
            ? nullptr : m_builtin.GetXamlType(winrt::hstring(found->second));
    }

    std::shared_ptr<MetadataState> m_state;
    winrt::Microsoft::UI::Xaml::XamlTypeInfo::XamlControlsXamlMetaDataProvider m_builtin;
};

Registry::Provider MakeMetadata(const std::shared_ptr<MetadataState>& state)
{
    return winrt::make<MetadataProvider>(state);
}
}

TEST_CASE("WinUIComponentMetadata::PriorityAtomicBatchAndOverloads",
          "[winui-component-metadata]")
{
    auto base = std::make_shared<MetadataState>();
    base->names[L"Shared"] = L"Microsoft.UI.Xaml.Controls.Button";
    base->xmlns = {{L"using:base", L"Base"}};
    Registry registry(MakeMetadata(base));
    auto component = std::make_shared<MetadataState>();
    // Use component-owned metadata. Intrinsic XAML types such as TextBlock
    // are resolved by the parser and need not appear in this provider.
    component->names[L"Shared"] = L"Microsoft.UI.Xaml.Controls.TabView";
    component->names[L"Extra"] = L"Microsoft.UI.Xaml.Controls.TabView";
    component->xmlns = {{L"using:base", L"Base"}, {L"using:extra", L"Extra"}};
    const auto provider = MakeMetadata(component);
    REQUIRE(registry.Register({provider, provider}));
    REQUIRE(registry.Register({provider}));

    REQUIRE(registry.GetXamlType(L"Shared"));
    CHECK(registry.GetXamlType(L"Shared").FullName() ==
          L"Microsoft.UI.Xaml.Controls.Button");
    CHECK(component->stringQueries == 0);
    REQUIRE(registry.GetXamlType(L"Extra"));
    CHECK(registry.GetXamlType(L"Extra").FullName() ==
          L"Microsoft.UI.Xaml.Controls.TabView");
    const Registry::TypeName extra{
        L"Extra", winrt::Windows::UI::Xaml::Interop::TypeKind::Custom};
    REQUIRE(registry.GetXamlType(extra));
    CHECK(registry.GetXamlType(extra).FullName() ==
          L"Microsoft.UI.Xaml.Controls.TabView");
    CHECK(base->typeQueries == 2);
    CHECK(component->typeQueries == 2);
    const unsigned before = component->stringQueries;
    CHECK_FALSE(registry.GetXamlType(L"Absent"));
    CHECK(component->stringQueries == before + 1);

    auto rejected = std::make_shared<MetadataState>();
    rejected->names[L"Rejected"] = L"Microsoft.UI.Xaml.Controls.Button";
    CHECK_FALSE(registry.Register({MakeMetadata(rejected), nullptr}));
    CHECK_FALSE(registry.GetXamlType(L"Rejected"));
    CHECK(rejected->stringQueries == 0);
    const auto xmlns = registry.GetXmlnsDefinitions();
    REQUIRE(xmlns.size() == 2);
    CHECK(xmlns[0].XmlNamespace == L"using:base");
    CHECK(xmlns[1].Namespace == L"Extra");

    component->fail = true;
    CHECK_THROWS_AS(registry.GetXamlType(L"Absent"), winrt::hresult_error);
    component->fail = false;
    CHECK(registry.GetXamlType(L"Extra"));
}

TEST_CASE("WinUIComponentMetadata::ReentrantLookupsUseStableSnapshot",
          "[winui-component-metadata]")
{
    auto base = std::make_shared<MetadataState>();
    Registry registry(MakeMetadata(base));
    auto late = std::make_shared<MetadataState>();
    late->names[L"Late"] = L"Microsoft.UI.Xaml.Controls.Button";
    late->xmlns = {{L"using:late", L"Late"}};
    const auto lateProvider = MakeMetadata(late);
    bool registered = false;
    base->onLookup = [&] { registered = registry.Register({lateProvider}); };
    CHECK_FALSE(registry.GetXamlType(L"Late"));
    CHECK(registered);
    CHECK(late->stringQueries == 0);
    REQUIRE(registry.GetXamlType(L"Late"));
    CHECK(late->stringQueries == 1);
    base->onLookup = {};

    auto last = std::make_shared<MetadataState>();
    last->xmlns = {{L"using:last", L"Last"}};
    const auto lastProvider = MakeMetadata(last);
    base->onXmlns = [&] { registered = registry.Register({lastProvider}); };
    const auto firstXmlns = registry.GetXmlnsDefinitions();
    CHECK(registered);
    REQUIRE(firstXmlns.size() == 1);
    CHECK(firstXmlns[0].Namespace == L"Late");
    const auto secondXmlns = registry.GetXmlnsDefinitions();
    REQUIRE(secondXmlns.size() == 2);
    CHECK(secondXmlns[1].Namespace == L"Last");
    base->onXmlns = {};
}

TEST_CASE("WinUIComponentMetadata::ApplicationRegistrationIsOwnerThreadOnly",
          "[winui-component-metadata]")
{
    const auto application = MUXApplication::Current();
    REQUIRE(application);
    const auto metadata = application.as<Registry::Provider>();
    REQUIRE(metadata.GetXamlType(L"Microsoft.UI.Xaml.Controls.Button"));
    const winrt::hstring name = L"wxWinUI.Test.Component.RegisteredButton";
    CHECK_FALSE(metadata.GetXamlType(name));
    auto state = std::make_shared<MetadataState>();
    state->names[std::wstring(name)] = L"Microsoft.UI.Xaml.Controls.Button";
    const auto provider = MakeMetadata(state);
    bool workerAccepted = true;
    std::thread worker([&]
    {
        workerAccepted = wxWinUIRegisterXamlMetadataProviders({provider});
    });
    worker.join();
    CHECK_FALSE(workerAccepted);
    CHECK(state->stringQueries == 0);
    CHECK_FALSE(metadata.GetXamlType(name));
    REQUIRE(wxWinUIRegisterXamlMetadataProviders({provider}));
    REQUIRE(metadata.GetXamlType(name));
    CHECK(metadata.GetXamlType(name).FullName() ==
          L"Microsoft.UI.Xaml.Controls.Button");
    CHECK_FALSE(wxWinUIRegisterXamlMetadataProviders({nullptr}));
    REQUIRE(metadata.GetXamlType(L"Microsoft.UI.Xaml.Controls.Button"));
}

#endif // __WXWINUI__ && wxUSE_WINUI3
