/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostuia.cpp
// Purpose:     Accessibility ownership, style observation and HWND shell providers
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"
#include "hostuia.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/xamlhost.h"
#include "wx/scopeguard.h"
#include "wx/msw/private.h"

#include <UIAutomation.h>
#include <UIAutomationCoreApi.h>
#include <wrl/implements.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <utility>
#include <vector>

namespace
{
unsigned gs_accessibilityShellProviderRetires = 0;
std::atomic<unsigned> gs_liveInvisibleShellProviders{ 0 };
unsigned gs_liveAutomationNameStylePropertyStates = 0;
} // anonymous namespace

wxWinUIAutomationNameStylePropertyState::
wxWinUIAutomationNameStylePropertyState()
{
    ++gs_liveAutomationNameStylePropertyStates;
}

wxWinUIAutomationNameStylePropertyState::
~wxWinUIAutomationNameStylePropertyState()
{
    wxASSERT(gs_liveAutomationNameStylePropertyStates > 0);
    --gs_liveAutomationNameStylePropertyStates;
}

// Derive the UIA name wx may contribute when the XAML application didn't
// author one. wxWindow::GetName() is a technical/resource identifier (used by
// XRC, persistence and FindWindowByName), not the accessible name: the common
// wxWindowAccessible contract exposes only the stripped label/title. Keeping
// the same rule here also avoids leaking defaults such as "button" or
// "activityindicator" into narration.
wxString wxWinUIGetAutomationName(wxWindow *window)
{
    if ( !window )
        return wxString();

    return wxStripMenuCodes(window->GetLabel(), wxStrip_Mnemonics);
}

namespace
{

// Read whether a Style (or any BasedOn ancestor) authors
// AutomationProperties.Name. ReadLocalValue() cannot see Style setters, and an
// empty setter is otherwise indistinguishable from the dependency property's
// empty default. A malformed/cyclic chain is treated conservatively as
// authored: accessibility state must never be overwritten because inspection
// could not prove it was absent.
bool wxWinUIStyleChainAuthorsAutomationName(
    winrt::Microsoft::UI::Xaml::Style style)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

    std::vector<MUX::Style> visited;
    while ( style )
    {
        if ( std::find(visited.begin(), visited.end(), style) !=
                visited.end() )
        {
            return true;
        }
        visited.push_back(style);

        for ( const auto& setterBase : style.Setters() )
        {
            const auto setter = setterBase.try_as<MUX::Setter>();
            if ( setter &&
                 setter.Property() == MUXA::AutomationProperties::NameProperty() )
            {
                return true;
            }
        }

        style = style.BasedOn();
    }

    return false;
}

struct wxWinUIImplicitStyleLookup
{
    bool found = false;
    bool authorsAutomationName = false;
};

// Read only the entry owned by this dictionary. HasKey(), unlike Lookup(),
// doesn't walk MergedDictionaries, which lets the caller reproduce XAML's
// reverse merged-dictionary precedence and omit wx-installed framework
// dictionaries from the Application scope.
wxWinUIImplicitStyleLookup wxWinUIReadImplicitStyleEntry(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key)
{
    if ( !resources )
        return {};

    try
    {
        if ( !resources.HasKey(key) )
            return {};

        const auto value = resources.Lookup(key);
        // ResourceDictionary lookup is permitted to report a miss as null
        // instead of E_BOUNDS. A valid implicit Style can never be null.
        if ( !value )
            return {};

        const auto style =
            value.try_as<winrt::Microsoft::UI::Xaml::Style>();
        if ( !style )
        {
            // A resource occupying the implicit-style key but having the
            // wrong type is malformed application state. Fail closed for UIA
            // ownership instead of publishing narration over it.
            return { true, true };
        }

        return { true,
                 wxWinUIStyleChainAuthorsAutomationName(style) };
    }
    catch ( const winrt::hresult_out_of_bounds& )
    {
        return {};
    }
    catch ( const winrt::hresult_error& e )
    {
        // A deferred resource may fail while being materialized. Accessibility
        // ownership is fail-closed: log it and don't publish a wx fallback
        // over application state which couldn't be inspected.
        wxWinUILogException("resolve implicit automation Style", e);
        return { true, true };
    }
}

bool wxWinUIIsFrameworkResourceDictionary(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    if ( !resources )
        return false;

    try
    {
        if ( resources.try_as<MUXC::XamlControlsResources>() )
            return true;

        // wxWinUIEnsureApplicationResources() has a loose-XAML fallback for
        // machines where XamlControlsResources construction fails. Those
        // dictionaries are ordinary ResourceDictionary instances, but their
        // source still identifies the WinUI framework payload unambiguously.
        const auto source = resources.Source();
        if ( !source )
            return false;

        const wxString uri(source.AbsoluteUri().c_str());
        return uri.Lower().Find("/microsoft.ui.xaml/themes/") != wxNOT_FOUND;
    }
    catch ( const winrt::hresult_error& )
    {
        // Unknown provenance is application-owned for accessibility purposes:
        // fail closed instead of skipping a dictionary we couldn't identify.
        return false;
    }
}

wxWinUIImplicitStyleLookup wxWinUILookupImplicitStyleImpl(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key,
    bool skipFrameworkResources,
    std::vector<winrt::Microsoft::UI::Xaml::ResourceDictionary>& stack)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    if ( !resources )
        return {};

    if ( std::find(stack.begin(), stack.end(), resources) != stack.end() )
    {
        // A merged-dictionary cycle is malformed and makes ownership
        // unknowable. Preserve application accessibility state.
        return { true, true };
    }

    stack.push_back(resources);
    wxScopeGuard popStack = wxMakeGuard([&stack]()
    {
        stack.pop_back();
    });
    wxUnusedVar(popStack);

    const wxWinUIImplicitStyleLookup local =
        wxWinUIReadImplicitStyleEntry(resources, key);
    if ( local.found )
        return local;

    try
    {
        const auto merged = resources.MergedDictionaries();
        for ( uint32_t i = merged.Size(); i > 0; --i )
        {
            const MUX::ResourceDictionary dictionary = merged.GetAt(i - 1);
            if ( skipFrameworkResources &&
                 wxWinUIIsFrameworkResourceDictionary(dictionary) )
            {
                continue;
            }

            const wxWinUIImplicitStyleLookup nested =
                wxWinUILookupImplicitStyleImpl(
                    dictionary,
                    key,
                    skipFrameworkResources,
                    stack);
            if ( nested.found )
                return nested;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("walk implicit automation Style resources", e);
        return { true, true };
    }

    return {};
}

// WinUI's implicit-style resolver uses the element's TypeName as the resource
// key. Returning "found" independently from the Style result is important:
// the nearest implicit Style wins even if it doesn't set Name, so a farther
// application resource must not be inspected in that case.
wxWinUIImplicitStyleLookup wxWinUILookupImplicitStyle(
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& resources,
    const winrt::Windows::Foundation::IInspectable& key,
    bool skipFrameworkResources = false)
{
    std::vector<winrt::Microsoft::UI::Xaml::ResourceDictionary> stack;
    return wxWinUILookupImplicitStyleImpl(
        resources, key, skipFrameworkResources, stack);
}

} // anonymous namespace

// FrameworkElement::Style() deliberately exposes only an explicitly assigned
// Style. Implicit styles have to be resolved through the same resource scopes
// as XAML: the element/parent chain first, then application resources. This
// matters especially for an authored empty Name, whose effective value is
// otherwise identical to the dependency property's default.
bool wxWinUIStyleAuthorsAutomationName(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    bool inspectImplicitStyle,
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& slotResources)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    auto framework = content.try_as<MUX::FrameworkElement>();
    if ( !framework )
        return false;

    if ( const MUX::Style explicitStyle = framework.Style() )
    {
        return wxWinUIStyleChainAuthorsAutomationName(explicitStyle);
    }

    const auto localStyle =
        framework.ReadLocalValue(MUX::FrameworkElement::StyleProperty());
    if ( localStyle != MUX::DependencyProperty::UnsetValue() )
    {
        // Style={x:Null} is a deliberate opt-out from implicit styling. A
        // non-null local value with a null effective Style is inconsistent,
        // so keep ownership fail-closed in that case.
        return localStyle != nullptr;
    }

    // ResourceDictionary mutations don't necessarily invalidate an implicit
    // Style already resolved by WinUI. Native peers therefore inspect only at
    // creation and known re-resolution boundaries. Arbitrary wxWinUIXamlHost
    // content can also be re-inspected conservatively during an ordinary dirty
    // pass: application-owned XAML may have performed a silent x:Null ->
    // UnsetValue transition, and preserving application accessibility state is
    // safer than recreating a wx local Name over it.
    if ( !inspectImplicitStyle )
        return false;

    const winrt::Windows::UI::Xaml::Interop::TypeName contentType
    {
        winrt::get_class_name(content),
        winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
    };
    const auto implicitStyleKey =
        winrt::box_value(contentType);
    MUX::DependencyObject current = framework;
    std::vector<MUX::DependencyObject> visited;
    while ( current )
    {
        if ( std::find(visited.begin(), visited.end(), current) !=
                visited.end() )
        {
            return true;
        }
        visited.push_back(current);

        if ( const auto currentFramework =
                 current.try_as<MUX::FrameworkElement>() )
        {
            const wxWinUIImplicitStyleLookup lookup =
                wxWinUILookupImplicitStyle(
                    currentFramework.Resources(), implicitStyleKey);
            if ( lookup.found )
                return lookup.authorsAutomationName;
        }

        current = MUX::Media::VisualTreeHelper::GetParent(current);
    }

    // ContentPresenter can keep an arbitrary content root outside the visual
    // parent chain exposed by VisualTreeHelper. The slot container is still
    // an authoritative resource scope, so inspect its dictionary explicitly
    // before falling back to application resources.
    if ( slotResources )
    {
        const wxWinUIImplicitStyleLookup lookup =
            wxWinUILookupImplicitStyle(slotResources, implicitStyleKey);
        if ( lookup.found )
            return lookup.authorsAutomationName;
    }

    const MUX::Application application = MUX::Application::Current();
    if ( application )
    {
        const wxWinUIImplicitStyleLookup lookup =
            wxWinUILookupImplicitStyle(
                application.Resources(), implicitStyleKey, true);
        if ( lookup.found )
            return lookup.authorsAutomationName;
    }

    return false;
}

bool wxWinUITryGetLocalStringValue(
    const winrt::Windows::Foundation::IInspectable& value,
    wxString *result)
{
    if ( !value || !result )
        return false;

    const auto propertyValue =
        value.try_as<winrt::Windows::Foundation::IPropertyValue>();
    if ( !propertyValue ||
         propertyValue.Type() !=
             winrt::Windows::Foundation::PropertyType::String )
    {
        return false;
    }

    *result = wxString(propertyValue.GetString().c_str());
    return true;
}

bool wxWinUIHasBinding(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Microsoft::UI::Xaml::DependencyProperty& property)
{
    const auto framework =
        content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    return framework && framework.GetBindingExpression(property);
}

bool wxWinUIUpdateLocalStyleSourceSnapshot(
    const std::shared_ptr<wxWinUIAutomationNameStylePropertyState>& state,
    const winrt::Microsoft::UI::Xaml::FrameworkElement& framework)
{
    const auto localStyle =
        framework.ReadLocalValue(
            winrt::Microsoft::UI::Xaml::FrameworkElement::StyleProperty());
    const bool isSet =
        localStyle !=
            winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue();
    const bool changed =
        state->localStyleSourceInitialized &&
        (state->localStyleSourceIsSet != isSet ||
         (isSet && state->localStyleSource != localStyle));
    state->localStyleSource = isSet ? localStyle : nullptr;
    state->localStyleSourceIsSet = isSet;
    state->localStyleSourceInitialized = true;
    return changed;
}

void wxWinUISlot::RetireExposedAccessibilityShellProvider(bool terminal)
{
    if ( m_accessibilityShellRetirementTerminal )
        return;

    // Publish terminal state before the external COM boundary. Logical slot
    // retirement can be followed by a delayed physical disconnect after its
    // HWND value has been recycled; that later phase must never purge the new
    // native lifetime. Non-terminal content/authority changes remain
    // repeatable while this slot is live.
    if ( terminal )
        m_accessibilityShellRetirementTerminal = true;

    wxWinUITLWHostRetireAccessibilityShellProvider(m_hwnd);
}

bool wxWinUISlot::InstallAutomationNameStylePropertyObserver()
{
    constexpr unsigned AutomaticRetryLimit = 3;

    const unsigned previousRetryFailures =
        m_automationNameStylePropertyObserverRetryFailures;
    RevokeAutomationNameStylePropertyObserver();
    m_automationNameStylePropertyObserverRetryFailures =
        previousRetryFailures;

    const auto expectedContent =
        m_semanticTarget.try_as<
            winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if ( !expectedContent || !m_lifetime )
        return true;

    const auto expectedLifetime = m_lifetime;
    const unsigned long long expectedGeneration = m_contentGeneration;
    const unsigned long long expectedTransactionEpoch =
        m_contentTransactionEpoch;
    const auto operationIsCurrent = [&]()
    {
        return !m_deletePending &&
               !m_poisoned &&
               m_lifetime == expectedLifetime &&
               expectedLifetime->GetHost() != nullptr &&
               m_semanticTarget == expectedContent &&
               m_contentGeneration == expectedGeneration &&
               m_contentTransactionEpoch == expectedTransactionEpoch &&
               !m_contentTransactionInProgress;
    };

    auto state =
        std::make_shared<wxWinUIAutomationNameStylePropertyState>();
    state->lifetime = expectedLifetime;
    int64_t styleToken = 0;
    int64_t nameToken = 0;
    int64_t labeledByToken = 0;
    winrt::event_token themeToken{};
    winrt::event_token layoutUpdatedToken{};

    const auto notifyStyleContextChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::FrameworkElement& framework,
            bool implicitStyleWasReresolved)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_automationNameStyleObservedContent != framework ||
                 slot->m_semanticTarget != framework )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // Record every explicit Style mutation for reentrancy validation.
            // Style changes and ActualThemeChanged are both boundaries where
            // WinUI can resolve a different implicit Style.
            if ( ++slot->m_automationNameStyleRevision == 0 )
                ++slot->m_automationNameStyleRevision;
            if ( implicitStyleWasReresolved )
            {
                if ( ++slot->m_automationNameImplicitStyleRevision == 0 )
                    ++slot->m_automationNameImplicitStyleRevision;
            }
            host->MarkDirty(window);
        };

    const auto notifyAutomationNameChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::DependencyObject& content)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_semanticTarget != content )
            {
                return;
            }

            if ( slot->m_contentTransactionInProgress )
            {
                // RestoreAutomationName() has already removed wx's exact
                // scalar before the carrier transaction starts. Any later
                // Name edge in this interval therefore comes from application
                // code (including SetName followed by ClearValue) and must
                // survive a migration even though its final DP value is unset.
                slot->m_contentTransactionAutomationNameRelinquished = true;
                return;
            }

            if ( !slot->m_uiaNameSynced ||
                 slot->m_uiaNameRelinquished )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // Name mutations don't re-resolve Style and must not advance its
            // revision. They only ask the common ownership pass to compare
            // local, binding, animated/effective and wx-published values.
            host->MarkDirty(window);
        };

    const auto notifyAutomationLabeledByChanged =
        [state](
            const winrt::Microsoft::UI::Xaml::DependencyObject& content)
        {
            if ( !state->active || state->inCallback )
                return;

            const auto lifetime = state->lifetime.lock();
            wxWinUITopLevelHost * const host =
                lifetime ? lifetime->GetHost() : nullptr;
            wxWindow * const window =
                lifetime ? lifetime->GetWindow() : nullptr;
            wxWinUISlot * const slot =
                host && window ? host->FindSlot(window) : nullptr;
            if ( !slot || slot->m_lifetime != lifetime ||
                 slot->m_automationNameStylePropertyState != state ||
                 slot->m_semanticTarget != content )
            {
                return;
            }

            struct CallbackGuard
            {
                explicit CallbackGuard(
                    const std::shared_ptr<
                        wxWinUIAutomationNameStylePropertyState>& value)
                    : state(value)
                {
                    state->inCallback = true;
                }
                ~CallbackGuard() { state->inCallback = false; }
                std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
                    state;
            } guard(state);

            // LabeledBy takes semantic precedence over wx's fallback Name,
            // but it is reversible. Advance the revision so a relation
            // mutation occurring inside another XAML setter invalidates that
            // outer SyncSlot pass instead of publishing stale ownership.
            if ( ++slot->m_automationNameStyleRevision == 0 )
                ++slot->m_automationNameStyleRevision;
            host->MarkDirty(window);
        };

    const auto updateLocalStyleSourceSnapshot =
        [state](
            const winrt::Microsoft::UI::Xaml::FrameworkElement& framework)
        {
            return wxWinUIUpdateLocalStyleSourceSnapshot(state, framework);
        };

    const auto revokeProvisional = [&]()
    {
        state->active = false;
        if ( layoutUpdatedToken.value )
            expectedContent.LayoutUpdated(layoutUpdatedToken);
        if ( themeToken.value )
            expectedContent.ActualThemeChanged(themeToken);
        if ( styleToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::FrameworkElement::
                        StyleProperty(),
                    styleToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation Style observer", e);
            }
        }
        if ( nameToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Automation::
                        AutomationProperties::NameProperty(),
                    nameToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation Name observer", e);
            }
        }
        if ( labeledByToken )
        {
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Automation::
                        AutomationProperties::LabeledByProperty(),
                    labeledByToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke provisional automation LabeledBy observer", e);
            }
        }
    };

    try
    {
        const auto styleProperty =
            winrt::Microsoft::UI::Xaml::FrameworkElement::StyleProperty();
        updateLocalStyleSourceSnapshot(expectedContent);

        styleToken =
            expectedContent.RegisterPropertyChangedCallback(
                styleProperty,
                [state,
                 updateLocalStyleSourceSnapshot,
                 notifyStyleContextChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    const auto framework =
                        sender.try_as<
                            winrt::Microsoft::UI::Xaml::FrameworkElement>();
                    if ( framework )
                    {
                        try
                        {
                            updateLocalStyleSourceSnapshot(framework);
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "inspect changed Style source", e);
                        }
                    }
                    notifyStyleContextChanged(framework, true);
                });
        nameToken =
            expectedContent.RegisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::NameProperty(),
                [notifyAutomationNameChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    notifyAutomationNameChanged(sender);
                });
        labeledByToken =
            expectedContent.RegisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::LabeledByProperty(),
                [notifyAutomationLabeledByChanged](
                    const winrt::Microsoft::UI::Xaml::DependencyObject&
                        sender,
                    const winrt::Microsoft::UI::Xaml::DependencyProperty&)
                {
                    notifyAutomationLabeledByChanged(sender);
                });
        themeToken = expectedContent.ActualThemeChanged(
            [notifyStyleContextChanged](
                const winrt::Microsoft::UI::Xaml::FrameworkElement& sender,
                const winrt::Windows::Foundation::IInspectable&)
            {
                notifyStyleContextChanged(sender, true);
            });

        if ( wxDynamicCast(m_window, wxWinUIXamlHost) )
        {
            const winrt::weak_ref<
                winrt::Microsoft::UI::Xaml::FrameworkElement> weakContent =
                    expectedContent;
            layoutUpdatedToken = expectedContent.LayoutUpdated(
                [state,
                 weakContent,
                 updateLocalStyleSourceSnapshot,
                 notifyStyleContextChanged](
                    const winrt::Windows::Foundation::IInspectable&,
                    const winrt::Windows::Foundation::IInspectable&)
                {
                    if ( !state->active || state->inCallback )
                    {
                        return;
                    }

                    const auto framework = weakContent.get();
                    if ( !framework )
                        return;

                    try
                    {
                        if ( !updateLocalStyleSourceSnapshot(framework) )
                            return;

                        // RegisterPropertyChangedCallback reports effective
                        // value changes. x:Null <-> UnsetValue can therefore
                        // be silent while the effective Style stays null.
                        // Arbitrary XAML hosts keep this source snapshot for
                        // their lifetime so layout-affecting late application
                        // mutations converge without imposing an observer on
                        // every native wx peer.
                        notifyStyleContextChanged(framework, true);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "inspect local Style source", e);
                    }
                });
        }

        if ( !operationIsCurrent() )
        {
            revokeProvisional();
            return false;
        }

        m_automationNameStyleObservedContent = expectedContent;
        m_automationNameStylePropertyCallbackToken = styleToken;
        m_automationNamePropertyCallbackToken = nameToken;
        m_automationLabeledByPropertyCallbackToken = labeledByToken;
        m_automationNameActualThemeChangedToken = themeToken;
        m_automationNameStyleLayoutUpdatedToken = layoutUpdatedToken;
        m_automationNameStylePropertyState = std::move(state);
        m_automationNameStylePropertyObserverPending = false;
        if ( previousRetryFailures )
        {
            // A Style/RequestedTheme/relationship mutation may have occurred
            // while no observer was installed. Force the catch-up pass to
            // re-inspect the complete naming context instead of treating the
            // last successful snapshot as current.
            if ( ++m_automationNameStyleRevision == 0 )
                ++m_automationNameStyleRevision;
            if ( ++m_automationNameImplicitStyleRevision == 0 )
                ++m_automationNameImplicitStyleRevision;
        }
        m_automationNameStylePropertyObserverRetryFailures = 0;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        revokeProvisional();
        wxWinUILogException(
            "register automation Name/style/relation observer", e);
        if ( operationIsCurrent() )
        {
            m_automationNameStylePropertyObserverPending = true;
            m_automationNameStylePropertyObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_automationNameStylePropertyObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueAutomationNameStylePropertyObserverRetry();
            }
        }
        return false;
    }
}

void wxWinUISlot::RevokeAutomationNameStylePropertyObserver()
{
    const auto state = m_automationNameStylePropertyState;
    if ( state )
        state->active = false;

    const auto content = m_automationNameStyleObservedContent;
    const int64_t token =
        m_automationNameStylePropertyCallbackToken;
    const int64_t nameToken =
        m_automationNamePropertyCallbackToken;
    const int64_t labeledByToken =
        m_automationLabeledByPropertyCallbackToken;
    const winrt::event_token themeToken =
        m_automationNameActualThemeChangedToken;
    const winrt::event_token layoutUpdatedToken =
        m_automationNameStyleLayoutUpdatedToken;
    m_automationNameStyleObservedContent = nullptr;
    m_automationNameStylePropertyCallbackToken = 0;
    m_automationNamePropertyCallbackToken = 0;
    m_automationLabeledByPropertyCallbackToken = 0;
    m_automationNameActualThemeChangedToken = {};
    m_automationNameStyleLayoutUpdatedToken = {};
    m_automationNameStylePropertyState.reset();
    m_automationNameStylePropertyObserverPending = false;
    m_automationNameStylePropertyObserverRetryFailures = 0;

    if ( content && token )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::FrameworkElement::
                    StyleProperty(),
                token);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation Style observer", e);
        }
    }

    if ( content && nameToken )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::NameProperty(),
                nameToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation Name observer", e);
        }
    }

    if ( content && labeledByToken )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties::LabeledByProperty(),
                labeledByToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "revoke automation LabeledBy observer", e);
        }
    }

    // C++/WinRT projects event removal as noexcept; state was invalidated
    // above first, so even an ABI-side removal failure cannot call live wx
    // objects through the retained delegate.
    if ( content && themeToken.value )
        content.ActualThemeChanged(themeToken);
    if ( content && layoutUpdatedToken.value )
        content.LayoutUpdated(layoutUpdatedToken);
}

void wxWinUISlot::QueueAutomationNameStylePropertyObserverRetry()
{
    if ( m_automationNameStylePropertyObserverPending &&
         !m_deletePending &&
         !m_poisoned &&
         m_semanticTarget &&
         m_lifetime &&
         m_lifetime->GetHost() )
    {
        NudgeDirty();
    }
}

void wxWinUISlot::PublishAutomationNameState(
    bool synced,
    bool owned,
    bool appOwned,
    bool relinquished,
    bool suppressedByLabeledBy,
    const wxString& lastDesired,
    const wxString& lastApplied,
    const winrt::Windows::Foundation::IInspectable& originalLocalValue,
    unsigned long long implicitStyleInspectedRevision)
{
    m_uiaNameSynced = synced;
    m_uiaNameOwned = owned;
    m_uiaNameAppOwned = appOwned;
    m_uiaNameRelinquished = relinquished;
    m_uiaNameSuppressedByLabeledBy = suppressedByLabeledBy;
    m_lastUIAName = lastDesired;
    m_lastAppliedUIAName = lastApplied;
    m_originalUIANameLocalValue = originalLocalValue;
    m_automationNameImplicitStyleInspectedRevision =
        implicitStyleInspectedRevision;
    if ( ++m_uiaNameStateRevision == 0 )
        ++m_uiaNameStateRevision;
}

bool wxWinUISlot::RestoreHelpText()
{
    if ( !m_helpTextSynced || !m_semanticTarget )
        return true;

    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace MUX = winrt::Microsoft::UI::Xaml;

    const auto content = m_semanticTarget;
    const unsigned long long generation = m_contentGeneration;
    try
    {
        if ( m_helpTextOwned )
        {
            const auto property =
                Automation::AutomationProperties::HelpTextProperty();
            const auto actualLocal = content.ReadLocalValue(property);
            const bool actualHasLocal =
                actualLocal != MUX::DependencyProperty::UnsetValue();
            const wxString actual(
                Automation::AutomationProperties::GetHelpText(content).c_str());

            // Only the exact value last published by wx is ours to restore.
            if ( actualHasLocal && actual == m_lastAppliedHelpText )
            {
                if ( !m_originalHelpTextLocalValue ||
                     m_originalHelpTextLocalValue ==
                         MUX::DependencyProperty::UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(
                        property, m_originalHelpTextLocalValue);
                }
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot HelpText", e);
        return false;
    }

    if ( m_semanticTarget != content ||
         m_contentGeneration != generation )
        return false;

    m_helpTextSynced = false;
    m_helpTextOwned = false;
    m_lastHelpTextPresent = false;
    m_lastHelpText.clear();
    m_lastAppliedHelpText.clear();
    m_originalHelpTextLocalValue = nullptr;
    return true;
}

bool wxWinUISlot::RestoreAutomationName()
{
    if ( !m_uiaNameSynced || !m_semanticTarget )
        return true;

    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace MUX = winrt::Microsoft::UI::Xaml;

    const auto content = m_semanticTarget;
    const auto lifetime = m_lifetime;
    const unsigned long long generation = m_contentGeneration;
    unsigned long long expectedStateRevision = m_uiaNameStateRevision;
    const bool previousSynced = m_uiaNameSynced;
    const bool previousOwned = m_uiaNameOwned;
    const bool previousAppOwned = m_uiaNameAppOwned;
    const bool previousRelinquished = m_uiaNameRelinquished;
    const bool previousSuppressedByLabeledBy =
        m_uiaNameSuppressedByLabeledBy;
    const wxString previousDesired = m_lastUIAName;
    const wxString previousApplied = m_lastAppliedUIAName;
    const auto previousOriginalLocalValue =
        m_originalUIANameLocalValue;
    const unsigned long long previousInspectedRevision =
        m_automationNameImplicitStyleInspectedRevision;
    winrt::Windows::Foundation::IInspectable preWriteLocal{ nullptr };
    bool setterAttempted = false;

    const auto operationIsCurrent = [&]()
    {
        return !m_poisoned &&
               m_lifetime == lifetime &&
               lifetime &&
               m_semanticTarget == content &&
               m_contentGeneration == generation &&
               m_uiaNameStateRevision == expectedStateRevision;
    };

    try
    {
        if ( m_uiaNameOwned )
        {
            const auto property =
                Automation::AutomationProperties::NameProperty();
            wxWinUIMaybeInjectAutomationNameContentFault(
                wxWinUITopLevelHost::
                    TestContentFault_AutomationNameRestoreRead);
            const auto actualLocal =
                content.ReadLocalValue(property);
            wxString actualLocalString;
            const bool hasBinding =
                wxWinUIHasBinding(content, property);
            if ( !operationIsCurrent() )
                return false;

            // Compare the LOCAL scalar, not GetName()'s effective value: an
            // animation may temporarily override the effective name while the
            // local value is still exactly the one supplied by wx. Leaving
            // that local behind would make it leak back after detach when the
            // animation ends. A direct application replacement (including a
            // local empty string or Binding) is preserved.
            if ( !hasBinding &&
                 actualLocal != MUX::DependencyProperty::UnsetValue() &&
                 wxWinUITryGetLocalStringValue(
                     actualLocal, &actualLocalString) &&
                 actualLocalString == m_lastAppliedUIAName )
            {
                preWriteLocal = actualLocal;
                setterAttempted = true;
                // Publish the logical result before crossing the setter. A
                // property callback may call FlushSync() synchronously: it
                // must see that the old scalar is already relinquished while
                // retaining the current desired-name cursor, otherwise it can
                // either classify wx's transient value as application-owned
                // or immediately reclaim the value being restored.
                PublishAutomationNameState(
                    true, false, false, false, false,
                    previousDesired, wxString(), nullptr,
                    previousInspectedRevision);
                expectedStateRevision = m_uiaNameStateRevision;

                if ( !previousOriginalLocalValue ||
                     previousOriginalLocalValue ==
                         MUX::DependencyProperty::UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(
                        property, previousOriginalLocalValue);
                }

                // A nested pass is the newer writer. Leave its complete tuple
                // intact and let the surrounding content transaction retry
                // from that state instead of publishing an obsolete reset.
                if ( !operationIsCurrent() )
                    return false;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot automation Name", e);

        // WinRT setters are allowed to throw before or after mutation. Restore
        // the old provenance only when the exact slot and exact pre-write
        // local object are still present; otherwise the last writer wins.
        if ( operationIsCurrent() )
        {
            try
            {
                const auto property =
                    Automation::AutomationProperties::NameProperty();
                const auto actualLocal = content.ReadLocalValue(property);
                if ( operationIsCurrent() &&
                     setterAttempted &&
                     actualLocal == preWriteLocal &&
                     !wxWinUIHasBinding(content, property) )
                {
                    wxString actualLocalString;
                    if ( wxWinUITryGetLocalStringValue(
                             actualLocal, &actualLocalString) &&
                         actualLocalString == previousApplied )
                    {
                        PublishAutomationNameState(
                            previousSynced,
                            previousOwned,
                            previousAppOwned,
                            previousRelinquished,
                            previousSuppressedByLabeledBy,
                            previousDesired,
                            previousApplied,
                            previousOriginalLocalValue,
                            previousInspectedRevision);
                        expectedStateRevision = m_uiaNameStateRevision;
                    }
                }
            }
            catch ( const winrt::hresult_error& nested )
            {
                wxWinUILogException(
                    "inspect failed automation Name restore", nested);
            }
        }
        if ( operationIsCurrent() )
            NudgeDirty();
        return false;
    }

    if ( !operationIsCurrent() )
        return false;

    PublishAutomationNameState(
        false, false, false, false, false,
        wxString(), wxString(), nullptr,
        m_automationNameImplicitStyleInspectedRevision);
    return true;
}

bool wxWinUISlot::ShouldPreserveAutomationNameRelinquishment() const
{
    if ( m_uiaNameRelinquished )
        return true;

    // A direct ClearValue() is intentionally indistinguishable from the
    // dependency property's empty default once the slot is rebuilt. Detect
    // it while the old ownership record still tells us which exact local
    // scalar wx supplied. This also covers a direct replacement performed
    // immediately before a rollback or migration, without requiring a prior
    // coalesced SyncSlot pass.
    if ( !m_uiaNameSynced || !m_uiaNameOwned || !m_semanticTarget )
        return false;

    try
    {
        namespace Automation =
            winrt::Microsoft::UI::Xaml::Automation;
        namespace MUX = winrt::Microsoft::UI::Xaml;

        wxWinUIMaybeInjectAutomationNameContentFault(
            wxWinUITopLevelHost::
                TestContentFault_AutomationNameTransferRead);
        const auto local = m_semanticTarget.ReadLocalValue(
            Automation::AutomationProperties::NameProperty());
        wxString localString;
        const bool hasBinding = wxWinUIHasBinding(
            m_semanticTarget,
            Automation::AutomationProperties::NameProperty());
        const bool localStillExact =
            !hasBinding &&
            local != MUX::DependencyProperty::UnsetValue() &&
            wxWinUITryGetLocalStringValue(local, &localString) &&
            localString == m_lastAppliedUIAName;
        const wxString effective(
            Automation::AutomationProperties::GetName(
                m_semanticTarget).c_str());
        const bool implicitStyleNeedsInspection =
            m_automationNameImplicitStyleRevision !=
                m_automationNameImplicitStyleInspectedRevision;
        return !localStillExact ||
               effective != m_lastAppliedUIAName ||
               wxWinUIStyleAuthorsAutomationName(
                   m_semanticTarget,
                   implicitStyleNeedsInspection,
                   m_container.Resources());
    }
    catch ( const winrt::hresult_error& e )
    {
        // Accessibility suppression must fail closed: if the current
        // application/wx ownership cannot be distinguished, never recreate
        // narration merely because the slot is being moved or rolled back.
        wxWinUILogException(
            "read automation Name transfer state", e);
        return true;
    }
}

void wxWinUISlot::SeedAutomationNameRelinquishment(
    const wxString& lastDesiredName)
{
    PublishAutomationNameState(
        true, false, true, true, false,
        lastDesiredName, wxString(), nullptr,
        m_automationNameImplicitStyleInspectedRevision);
}

bool wxWinUISlot::RestoreAccessibilityView()
{
    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace Peers =
        winrt::Microsoft::UI::Xaml::Automation::Peers;

    if ( !m_accessibilityViewSynced || !m_accessibilityViewOwned ||
            !m_semanticTarget )
    {
        m_accessibilityViewSynced = false;
        m_accessibilityViewOwned = false;
        m_originalAccessibilityViewLocalValue = nullptr;
        return true;
    }

    const auto content = m_semanticTarget;
    const unsigned long long generation = m_contentGeneration;
    try
    {
        const auto actual =
            Automation::AutomationProperties::GetAccessibilityView(content);

        // A non-Raw value was written directly by the application while the
        // native tree was authoritative. It already is the value to preserve,
        // so do not overwrite it with our older snapshot.
        if ( actual == Peers::AccessibilityView::Raw )
        {
            const auto property =
                Automation::AutomationProperties::AccessibilityViewProperty();
            if ( m_originalAccessibilityViewLocalValue ==
                    winrt::Microsoft::UI::Xaml::DependencyProperty::
                        UnsetValue() )
            {
                content.ClearValue(property);
            }
            else
            {
                content.SetValue(
                    property, m_originalAccessibilityViewLocalValue);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("restore slot AccessibilityView", e);
        return false;
    }

    // Any XAML property callback is arbitrary application code. Only retire
    // the cache if it still describes the same attached element.
    if ( m_semanticTarget != content ||
         m_contentGeneration != generation )
        return false;

    m_accessibilityViewSynced = false;
    m_accessibilityViewOwned = false;
    m_originalAccessibilityViewLocalValue = nullptr;
    return true;
}

unsigned
wxWinUITopLevelHost::GetLiveAutomationNameStyleObserverCountForTest()
{
    return gs_liveAutomationNameStylePropertyStates;
}

bool wxWinUITopLevelHost::TryGetAccessibilityShellState(
    const wxWindow *window,
    std::shared_ptr<wxWinUISlotLifetime> *lifetime,
    WXHWND *hwnd,
    unsigned long long *hwndGeneration,
    unsigned long long *contentGeneration,
    bool *hasWxAccessible,
    unsigned long long *authorityGeneration) const
{
    if ( !window || !lifetime || !hwnd || !hwndGeneration ||
            !contentGeneration || !hasWxAccessible ||
            !authorityGeneration ||
            window == m_tlw || window->IsTopLevel() ||
            window->IsBeingDeleted() || m_shuttingDown )
    {
        return false;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_poisoned || slot->m_deletePending ||
            !slot->m_lifetime ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return false;
    }

    const WXHWND currentHwnd = window->GetHWND();
    if ( !currentHwnd || slot->m_hwnd != currentHwnd )
        return false;

    const unsigned long long currentContentGeneration =
        slot->m_contentGeneration;
    const unsigned long long generation =
        wxWinUIMSWGetHwndGeneration(
            const_cast<wxWindow *>(window), currentHwnd);
    if ( !generation )
        return false;

    // Revalidate after the generation query: it crosses into USER32 and a
    // subclass may synchronously destroy or rebind the window.
    if ( FindSlot(window) != slot ||
            slot->m_deletePending || slot->m_poisoned ||
            slot->m_hwnd != window->GetHWND() ||
            slot->m_contentGeneration != currentContentGeneration ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return false;
    }

    *lifetime = slot->m_lifetime;
    *hwnd = currentHwnd;
    *hwndGeneration = generation;
    *contentGeneration = currentContentGeneration;
    *hasWxAccessible = slot->m_hasWxAccessible;
    *authorityGeneration = slot->m_accessibilityAuthorityGeneration;
    return true;
}

void wxWinUITopLevelHost::SetSlotAutomationNameOverride(
    wxWindow *window,
    const wxString& name)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const bool hasOverride = !name.empty();
    if ( slot->m_hasAutomationNameOverride == hasOverride &&
         slot->m_automationNameOverride == name )
    {
        return;
    }

    slot->m_hasAutomationNameOverride = hasOverride;
    slot->m_automationNameOverride = hasOverride ? name : wxString();
    if ( ++slot->m_automationNameOverrideRevision == 0 )
        ++slot->m_automationNameOverrideRevision;
    MarkDirty(window);
}

namespace
{

constexpr LONG wxWINUI_OBJID_CLIENT = -4;
constexpr LONG wxWINUI_UIA_ROOT_OBJECT_ID = -25;
constexpr wchar_t wxWINUI_UIA_SHELL_PROVIDER_MARKER[] =
    L"wxWidgets.WinUI.InvisibleShellProviderExposed";

class wxWinUIInvisibleShellProvider final
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::ClassicCom>,
          IRawElementProviderSimple>
{
public:
    wxWinUIInvisibleShellProvider(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        HWND hwnd,
        unsigned long long hwndGeneration,
        unsigned long long contentGeneration,
        unsigned long long authorityGeneration)
        : m_state(state),
          m_hwnd(hwnd),
          m_hwndGeneration(hwndGeneration),
          m_contentGeneration(contentGeneration),
          m_authorityGeneration(authorityGeneration)
    {
        gs_liveInvisibleShellProviders.fetch_add(
            1, std::memory_order_relaxed);
    }

    ~wxWinUIInvisibleShellProvider() override
    {
        gs_liveInvisibleShellProviders.fetch_sub(
            1, std::memory_order_relaxed);
    }

    IFACEMETHODIMP get_ProviderOptions(
        ProviderOptions *options) noexcept override
    {
        if ( !options )
            return E_POINTER;

        *options = static_cast<ProviderOptions>(
            ProviderOptions_ServerSideProvider |
            ProviderOptions_UseComThreading);
        return S_OK;
    }

    IFACEMETHODIMP GetPatternProvider(
        PATTERNID,
        IUnknown **provider) noexcept override
    {
        if ( !provider )
            return E_POINTER;
        *provider = nullptr;
        return IsCurrent() ? S_OK : UIA_E_ELEMENTNOTAVAILABLE;
    }

    IFACEMETHODIMP GetPropertyValue(
        PROPERTYID propertyId,
        VARIANT *propertyValue) noexcept override
    {
        if ( !propertyValue )
            return E_POINTER;
        ::VariantInit(propertyValue);

        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;

        switch ( propertyId )
        {
            case UIA_IsControlElementPropertyId:
            case UIA_IsContentElementPropertyId:
            case UIA_IsKeyboardFocusablePropertyId:
            case UIA_HasKeyboardFocusPropertyId:
                propertyValue->vt = VT_BOOL;
                propertyValue->boolVal = VARIANT_FALSE;
                break;

            case UIA_NamePropertyId:
                propertyValue->vt = VT_BSTR;
                propertyValue->bstrVal = ::SysAllocString(L"");
                if ( !propertyValue->bstrVal )
                {
                    propertyValue->vt = VT_EMPTY;
                    return E_OUTOFMEMORY;
                }
                break;

            default:
                // VT_EMPTY means that this provider does not supply the
                // property. The explicit Control/Content/Name values above
                // prevent UIA from promoting the HWND proxy into narration.
                break;
        }

        return S_OK;
    }

    IFACEMETHODIMP get_HostRawElementProvider(
        IRawElementProviderSimple **provider) noexcept override
    {
        if ( !provider )
            return E_POINTER;
        *provider = nullptr;

        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;

        Microsoft::WRL::ComPtr<IRawElementProviderSimple> hostProvider;
        const HRESULT hr = ::UiaHostProviderFromHwnd(
            m_hwnd, hostProvider.GetAddressOf());

        // UIA can synchronously consult provider state while resolving the
        // HWND proxy. Publish only if this exact shell/content/accessibility
        // generation still owns the result after that boundary.
        if ( !IsCurrent() )
            return UIA_E_ELEMENTNOTAVAILABLE;
        if ( FAILED(hr) )
            return hr;

        return hostProvider.CopyTo(provider);
    }

private:
    bool IsCurrent() const noexcept
    {
        const auto state = m_state.lock();
        if ( !state )
            return false;

        wxWindow * const window = state->GetWindow();
        wxWinUITopLevelHost * const host = state->GetHost();
        if ( !window || !host )
            return false;

        std::shared_ptr<wxWinUISlotLifetime> currentState;
        WXHWND currentHwnd = nullptr;
        unsigned long long currentGeneration = 0;
        unsigned long long contentGeneration = 0;
        bool hasWxAccessible = false;
        unsigned long long authorityGeneration = 0;
        return host->TryGetAccessibilityShellState(
                   window,
                   &currentState,
                   &currentHwnd,
                   &currentGeneration,
                   &contentGeneration,
                   &hasWxAccessible,
                   &authorityGeneration) &&
               currentState.get() == state.get() &&
               static_cast<HWND>(currentHwnd) == m_hwnd &&
               currentGeneration == m_hwndGeneration &&
               contentGeneration == m_contentGeneration &&
               !hasWxAccessible &&
               authorityGeneration == m_authorityGeneration;
    }

    std::weak_ptr<wxWinUISlotLifetime> m_state;
    HWND m_hwnd = nullptr;
    unsigned long long m_hwndGeneration = 0;
    unsigned long long m_contentGeneration = 0;
    unsigned long long m_authorityGeneration = 0;
};

bool wxWinUITryGetInvisibleShellState(
    wxWindow *window,
    std::shared_ptr<wxWinUISlotLifetime> *state,
    WXHWND *hwnd,
    unsigned long long *generation,
    unsigned long long *contentGeneration,
    bool *hasWxAccessible,
    unsigned long long *authorityGeneration)
{
    if ( !window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    return host &&
           host->TryGetAccessibilityShellState(
               window, state, hwnd, generation,
               contentGeneration, hasWxAccessible,
               authorityGeneration);
}

} // anonymous namespace

WXDLLIMPEXP_CORE HRESULT
wxWinUITestCreateInvisibleShellProvider(
    wxWindow *window,
    IRawElementProviderSimple **provider)
{
    if ( !provider )
        return E_POINTER;
    *provider = nullptr;

    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwndValue = nullptr;
    unsigned long long hwndGeneration = 0;
    unsigned long long contentGeneration = 0;
    bool hasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    if ( !wxWinUITryGetInvisibleShellState(
             window, &state, &hwndValue, &hwndGeneration,
             &contentGeneration, &hasWxAccessible,
             &authorityGeneration) ||
         hasWxAccessible )
    {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }

    auto shell = Microsoft::WRL::Make<
        wxWinUIInvisibleShellProvider>(
            state,
            static_cast<HWND>(hwndValue),
            hwndGeneration,
            contentGeneration,
            authorityGeneration);
    return shell ? shell.CopyTo(provider) : E_OUTOFMEMORY;
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostIsInvisibleAccessibilityShell(wxWindow *window)
{
    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwnd = nullptr;
    unsigned long long generation = 0;
    unsigned long long contentGeneration = 0;
    bool hasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    return wxWinUITryGetInvisibleShellState(
        window, &state, &hwnd, &generation,
        &contentGeneration, &hasWxAccessible,
        &authorityGeneration);
}

WXDLLIMPEXP_CORE bool
wxWinUITLWHostHandleShellGetObject(wxWindow *window,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam,
                                   bool hasWxAccessible,
                                   WXLRESULT *result)
{
    if ( !result || hasWxAccessible )
        return false;

    const LONG objectId = static_cast<LONG>(lParam);
    if ( objectId != wxWINUI_OBJID_CLIENT &&
            objectId != wxWINUI_UIA_ROOT_OBJECT_ID )
    {
        return false;
    }

    std::shared_ptr<wxWinUISlotLifetime> state;
    WXHWND hwndValue = nullptr;
    unsigned long long generation = 0;
    unsigned long long contentGeneration = 0;
    bool stateHasWxAccessible = false;
    unsigned long long authorityGeneration = 0;
    if ( !wxWinUITryGetInvisibleShellState(
             window, &state, &hwndValue, &generation,
             &contentGeneration, &stateHasWxAccessible,
             &authorityGeneration) )
    {
        return false;
    }
    if ( stateHasWxAccessible )
        return false;

    if ( objectId == wxWINUI_OBJID_CLIENT )
    {
        // The authoritative accessible object is the XAML peer. Returning a
        // native MSAA proxy here would add the covered HWND shell as a second
        // control; the UiaRoot path below supplies the explicit Raw-only host.
        *result = 0;
        return true;
    }

    const HWND hwnd = static_cast<HWND>(hwndValue);
    auto provider = Microsoft::WRL::Make<
        wxWinUIInvisibleShellProvider>(
            state, hwnd, generation, contentGeneration,
            authorityGeneration);
    if ( !provider )
    {
        *result = 0;
        return true;
    }

    // UI Automation retains per-HWND server-provider state after the return
    // below. Mark only exact shells that reached this path so WM_DESTROY can
    // explicitly retire that state before USER32 recycles the handle.
    if ( !::SetPropW(
             hwnd,
             wxWINUI_UIA_SHELL_PROVIDER_MARKER,
             reinterpret_cast<HANDLE>(static_cast<uintptr_t>(1))) )
    {
        *result = 0;
        return true;
    }

    *result = static_cast<WXLRESULT>(
        ::UiaReturnRawElementProvider(
            hwnd,
            static_cast<WPARAM>(wParam),
            static_cast<LPARAM>(lParam),
            provider.Get()));
    if ( !*result )
        ::RemovePropW(hwnd, wxWINUI_UIA_SHELL_PROVIDER_MARKER);
    return true;
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostRetireAccessibilityShellProvider(WXHWND hwndValue)
{
    const HWND hwnd = static_cast<HWND>(hwndValue);
    if ( !hwnd ||
         !::RemovePropW(hwnd, wxWINUI_UIA_SHELL_PROVIDER_MARKER) )
    {
        return;
    }

    ++gs_accessibilityShellProviderRetires;
    (void)::UiaReturnRawElementProvider(hwnd, 0, 0, nullptr);
}

WXDLLIMPEXP_CORE unsigned
wxWinUITestGetAccessibilityShellProviderRetireCount()
{
    return gs_accessibilityShellProviderRetires;
}

WXDLLIMPEXP_CORE unsigned
wxWinUITestGetLiveInvisibleShellProviderCount()
{
    return gs_liveInvisibleShellProviders.load(
        std::memory_order_relaxed);
}

WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyAccessibilityAuthority(wxWindow *window,
                                           bool hasWxAccessible)
{
    if ( !window )
        return;

    // FindSlotOwner() is intentionally the only lookup here. In particular,
    // do not call ForWindow(): SetAccessible() on an ordinary/native window
    // must not instantiate an island as a side effect.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( host )
        host->AccessibilityAuthorityChanged(window, hasWxAccessible);
}

void wxWinUITopLevelHost::AccessibilityAuthorityChanged(
    wxWindow *window,
    bool hasWxAccessible)
{
    OperationGuard hostOperation(this);

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot || slot->m_deletePending || slot->m_poisoned ||
            !slot->m_lifetime ||
            slot->m_lifetime->GetHost() != this ||
            slot->m_lifetime->GetWindow() != window )
    {
        return;
    }

    SyncSlotAccessibilityAuthority(*slot, hasWxAccessible);
}

void wxWinUITopLevelHost::SyncSlotAccessibilityAuthority(
    wxWinUISlot& slot,
    bool hasWxAccessible)
{
    namespace Automation =
        winrt::Microsoft::UI::Xaml::Automation;
    namespace Peers =
        winrt::Microsoft::UI::Xaml::Automation::Peers;

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(&slot);

    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    const auto content = slot.m_semanticTarget;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    if ( !window || slot.m_deletePending || slot.m_poisoned ||
            !state || state->GetHost() != this )
    {
        return;
    }

    if ( slot.m_hasWxAccessible != hasWxAccessible )
    {
        slot.m_hasWxAccessible = hasWxAccessible;
        if ( ++slot.m_accessibilityAuthorityGeneration == 0 )
            ++slot.m_accessibilityAuthorityGeneration;

        // UIA caches the provider by HWND, independently of our provider
        // object's generation check. Make the newly-selected native/XAML
        // authority observable on the next WM_GETOBJECT for this same shell.
        slot.RetireExposedAccessibilityShellProvider();
    }
    const unsigned long long authorityGeneration =
        slot.m_accessibilityAuthorityGeneration;

    if ( !content )
    {
        slot.m_accessibilityViewSynced = false;
        slot.m_accessibilityViewOwned = false;
        slot.m_originalAccessibilityViewLocalValue = nullptr;
        return;
    }

    const bool previousSynced = slot.m_accessibilityViewSynced;
    const bool previousOwned = slot.m_accessibilityViewOwned;
    auto nextOriginal = slot.m_originalAccessibilityViewLocalValue;
    bool nextOwned = previousOwned;

    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        if ( !state || state->GetHost() != this ||
                state->GetWindow() != window )
        {
            return nullptr;
        }

        wxWinUISlot * const current = FindSlot(window);
        return current &&
                   current == &slot &&
                   current->m_lifetime == state &&
                   current->m_semanticTarget == content &&
                   current->m_contentGeneration == contentGeneration &&
                   current->m_hasWxAccessible == hasWxAccessible &&
                   current->m_accessibilityAuthorityGeneration ==
                       authorityGeneration
             ? current
             : nullptr;
    };

    try
    {
        const auto actual =
            Automation::AutomationProperties::GetAccessibilityView(content);
        if ( !getCurrentSlot() )
            return;

        if ( hasWxAccessible )
        {
            if ( actual != Peers::AccessibilityView::Raw )
            {
                // This is either the initial authored value or a newer direct
                // application mutation. Remember it, then keep the native
                // wxAccessible tree singular by hiding the XAML peer.
                nextOriginal = content.ReadLocalValue(
                    Automation::AutomationProperties::
                        AccessibilityViewProperty());

                // Publish the restoration snapshot before the setter. Its
                // property-changed callback may synchronously remove
                // wxAccessible; that nested authority transition must see
                // enough state to restore this exact authored value.
                wxWinUISlot * const current = getCurrentSlot();
                if ( !current )
                    return;
                current->m_accessibilityViewSynced = true;
                current->m_accessibilityViewOwned = true;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;

                Automation::AutomationProperties::SetAccessibilityView(
                    content, Peers::AccessibilityView::Raw);
                nextOwned = true;
            }
            else if ( !previousSynced )
            {
                // Raw was already authored by the application; there is
                // nothing for the host to restore later.
                nextOriginal = content.ReadLocalValue(
                    Automation::AutomationProperties::
                        AccessibilityViewProperty());
                nextOwned = false;
            }
        }
        else
        {
            if ( previousOwned &&
                 actual == Peers::AccessibilityView::Raw )
            {
                const auto property =
                    Automation::AutomationProperties::
                        AccessibilityViewProperty();

                // Conversely, retire ownership before restoring. A callback
                // which re-adds wxAccessible then treats the just-restored
                // application value as the new baseline instead of trying to
                // restore it a second time.
                wxWinUISlot * const current = getCurrentSlot();
                if ( !current )
                    return;
                current->m_accessibilityViewSynced = true;
                current->m_accessibilityViewOwned = false;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;

                if ( nextOriginal ==
                        winrt::Microsoft::UI::Xaml::DependencyProperty::
                            UnsetValue() )
                {
                    content.ClearValue(property);
                }
                else
                {
                    content.SetValue(property, nextOriginal);
                }
            }

            // If `actual` was already non-Raw, the application changed it
            // while native authority was active. Leave that newer authored
            // value untouched rather than restoring the older snapshot.
            nextOwned = false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        // If restoration itself failed, retain ownership so a later sync can
        // retry. This is safe even for a setter that mutated before throwing:
        // the no-authority path observes a non-Raw value and simply retires
        // ownership without overwriting it.
        if ( wxWinUISlot * const current = getCurrentSlot() )
        {
            if ( !hasWxAccessible && previousOwned )
            {
                current->m_accessibilityViewSynced = previousSynced;
                current->m_accessibilityViewOwned = true;
                current->m_originalAccessibilityViewLocalValue =
                    nextOriginal;
            }
        }
        wxWinUILogException("sync slot accessibility authority", e);
        return;
    }

    // Every attached-property setter can synchronously run application code.
    // Commit only onto the same live slot/content/authority generation.
    if ( wxWinUISlot * const current = getCurrentSlot() )
    {
        current->m_accessibilityViewSynced = true;
        current->m_accessibilityViewOwned = nextOwned;
        current->m_originalAccessibilityViewLocalValue = nextOriginal;
    }
}

#endif // wxUSE_WINUI3
