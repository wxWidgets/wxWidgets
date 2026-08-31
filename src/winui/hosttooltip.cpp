/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hosttooltip.cpp
// Purpose:     Host-owned tooltip observation, restoration and policy
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "hosttooltip.h"
#include "wx/winui/private/tlwhost.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/scopeguard.h"
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <utility>

namespace
{

unsigned gs_failToolTipObserverAdds = 0;

void wxWinUIMaybeInjectToolTipObserverFault()
{
    if ( gs_failToolTipObserverAdds )
    {
        --gs_failToolTipObserverAdds;
        throw winrt::hresult_error(
            E_FAIL,
            L"wxWinUI test: injected tooltip observer fault");
    }
}

} // anonymous namespace

class wxWinUIToolTipPropertyState
{
public:
    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    bool active = true;
    bool inCallback = false;
};

#if wxUSE_TOOLTIPS
winrt::Windows::Foundation::IInspectable
wxWinUIReadToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content)
{
    return content.ReadLocalValue(
        winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
            ToolTipProperty());
}

void wxWinUIRestoreToolTipLocalValue(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Windows::Foundation::IInspectable& localValue)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const auto property = MUXC::ToolTipService::ToolTipProperty();
    if ( localValue == MUX::DependencyProperty::UnsetValue() )
        content.ClearValue(property);
    else
        content.SetValue(property, localValue);
}
#endif // wxUSE_TOOLTIPS

bool wxWinUISlot::InstallToolTipPropertyObserver()
{
#if wxUSE_TOOLTIPS
    constexpr unsigned AutomaticRetryLimit = 3;

    const unsigned previousRetryFailures =
        m_toolTipPropertyObserverRetryFailures;
    RevokeToolTipPropertyObserver();
    m_toolTipPropertyObserverRetryFailures = previousRetryFailures;
    if ( !m_content || !m_lifetime )
        return true;

    const auto expectedContent = m_content;
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
               m_content == expectedContent &&
               m_contentGeneration == expectedGeneration &&
               m_contentTransactionEpoch == expectedTransactionEpoch &&
               !m_contentTransactionInProgress;
    };

    try
    {
        auto state = std::make_shared<wxWinUIToolTipPropertyState>();
        state->lifetime = expectedLifetime;
        wxWinUIMaybeInjectToolTipObserverFault();
        wxWinUIMaybeInjectToolTipContentFault();
        const int64_t token = expectedContent.RegisterPropertyChangedCallback(
            winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                ToolTipProperty(),
            [state](
                const winrt::Microsoft::UI::Xaml::DependencyObject& sender,
                const winrt::Microsoft::UI::Xaml::DependencyProperty&)
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
                const auto element =
                    sender.try_as<
                        winrt::Microsoft::UI::Xaml::UIElement>();
                if ( !slot || slot->m_lifetime != lifetime ||
                     slot->m_toolTipPropertyState != state ||
                     slot->m_content != element )
                {
                    return;
                }

                struct CallbackGuard
                {
                    explicit CallbackGuard(
                        const std::shared_ptr<
                            wxWinUIToolTipPropertyState>& value)
                        : state(value)
                    {
                        state->inCallback = true;
                    }
                    ~CallbackGuard() { state->inCallback = false; }
                    std::shared_ptr<wxWinUIToolTipPropertyState> state;
                } guard(state);

                // A direct XAML write has no wx geometry notification. Dirty
                // the slot so ownership/baseline is reconciled. Under global
                // suppression, close a ToolTip object immediately.
                host->MarkDirty(window);
                if ( !wxWinUIAreToolTipsEnabled() )
                {
                    try
                    {
                        const auto actual =
                            winrt::Microsoft::UI::Xaml::Controls::
                                ToolTipService::GetToolTip(element);
                        if ( const auto toolTip =
                                 actual.try_as<
                                     winrt::Microsoft::UI::Xaml::Controls::
                                         ToolTip>() )
                        {
                            toolTip.IsOpen(false);
                        }

                        // ToolTip::Closed is delivered from Popup::Closed and
                        // can therefore publish a replacement after the
                        // original wxToolTip::Enable(false) transaction has
                        // returned. Re-resolve every lifetime boundary crossed
                        // by IsOpen(false), then run the policy-only adapter
                        // immediately so even a frozen host never exposes that
                        // late replacement. Writes made by either host tooltip
                        // transaction are already covered by their own bounded
                        // re-read loop and must not recursively enter it.
                        if ( wxWinUIAreToolTipsEnabled() )
                            return;

                        const auto currentLifetime = state->lifetime.lock();
                        wxWinUITopLevelHost * const currentHost =
                            currentLifetime
                                ? currentLifetime->GetHost()
                                : nullptr;
                        wxWindow * const currentWindow =
                            currentLifetime
                                ? currentLifetime->GetWindow()
                                : nullptr;
                        wxWinUISlot * const currentSlot =
                            currentHost && currentWindow
                                ? currentHost->FindSlot(currentWindow)
                                : nullptr;
                        if ( !state->active ||
                             !currentSlot ||
                             currentSlot->m_lifetime != currentLifetime ||
                             currentSlot->m_toolTipPropertyState != state ||
                             currentSlot->m_content != element ||
                             currentSlot->m_toolTipPolicySyncDepth != 0 )
                        {
                            return;
                        }

                        const auto remaining =
                            winrt::Microsoft::UI::Xaml::Controls::
                                ToolTipService::GetToolTip(element);
                        if ( remaining )
                        {
                            // Last operation: this transaction may invoke
                            // application code which destroys or reparents the
                            // slot.
                            currentHost->SynchronizeSlotToolTipPolicy(
                                *currentSlot);
                        }
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "suppress tooltip under disabled policy", e);
                    }
                }
            });

        // RegisterPropertyChangedCallback() is a WinRT boundary. If it
        // synchronously destroyed/replaced the slot, revoke the just-created
        // callback instead of publishing it onto the obsolete generation.
        if ( !operationIsCurrent() )
        {
            state->active = false;
            try
            {
                expectedContent.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                        ToolTipProperty(),
                    token);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "revoke stale tooltip property observer", e);
            }
            return false;
        }

        m_toolTipObservedContent = expectedContent;
        m_toolTipPropertyCallbackToken = token;
        m_toolTipPropertyState = std::move(state);
        m_toolTipPropertyObserverPending = false;
        m_toolTipPropertyObserverRetryFailures = 0;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("register tooltip property observer", e);
        // A registration HRESULT on the still-exact content is a degraded
        // auxiliary state, not a failed content transaction. A stale result
        // (destroy/replacement/nested swap) belongs to an obsolete operation
        // and must not overwrite the newer observer's fields.
        if ( operationIsCurrent() )
        {
            m_toolTipPropertyObserverPending = true;
            m_toolTipPropertyObserverRetryFailures =
                previousRetryFailures + 1;
            if ( m_toolTipPropertyObserverRetryFailures <
                    AutomaticRetryLimit )
            {
                QueueToolTipPropertyObserverRetry();
            }
        }
        return false;
    }
#else
    return true;
#endif
}

void wxWinUISlot::RevokeToolTipPropertyObserver()
{
#if wxUSE_TOOLTIPS
    const auto state = m_toolTipPropertyState;
    if ( state )
        state->active = false;

    const auto content = m_toolTipObservedContent;
    const int64_t token = m_toolTipPropertyCallbackToken;
    m_toolTipObservedContent = nullptr;
    m_toolTipPropertyCallbackToken = 0;
    m_toolTipPropertyState.reset();
    m_toolTipPropertyObserverPending = false;
    m_toolTipPropertyObserverRetryFailures = 0;

    if ( content && token )
    {
        try
        {
            content.UnregisterPropertyChangedCallback(
                winrt::Microsoft::UI::Xaml::Controls::ToolTipService::
                    ToolTipProperty(),
                token);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("revoke tooltip property observer", e);
        }
    }
#endif
}

void wxWinUISlot::QueueToolTipPropertyObserverRetry()
{
#if wxUSE_TOOLTIPS
    // MarkDirty/CallAfter is already coalesced by the host. The finite retry
    // counter is maintained by InstallToolTipPropertyObserver(); this helper
    // deliberately does not spin or post an independent callback.
    if ( m_toolTipPropertyObserverPending &&
         !m_deletePending &&
         !m_poisoned &&
         m_content &&
         m_lifetime &&
         m_lifetime->GetHost() )
    {
        NudgeDirty();
    }
#endif
}

bool wxWinUISlot::RestoreToolTip()
{
#if wxUSE_TOOLTIPS
    if ( !m_toolTipSynced || !m_content )
        return true;

    // Restoring the application baseline is part of the host's content
    // transaction, not a new application-authored ToolTipProperty write.
    // In particular, cross-TLW migration while global suppression is active
    // must leave the exact baseline attached long enough for the destination
    // slot to adopt it. Prevent the property observer from recursively
    // suppressing this transient host write before the detach completes.
    ++m_toolTipPolicySyncDepth;
    const auto toolTipPolicySyncGuard = wxMakeGuard([this]()
    {
        wxASSERT_MSG(m_toolTipPolicySyncDepth != 0,
                     "unbalanced WinUI tooltip restore transaction");
        --m_toolTipPolicySyncDepth;
    });
    wxUnusedVar(toolTipPolicySyncGuard);

    const auto content = m_content;
    const unsigned long long generation = m_contentGeneration;
    bool ok = true;

    if ( m_toolTipOwned && m_lastAppliedToolTip )
    {
        ok = wxWinUIRestoreToolTip(
            content,
            m_originalToolTipLocalValue,
            m_lastAppliedToolTip);
    }
    else if ( m_toolTipOwned )
    {
        try
        {
            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;
            // Explicit SetToolTip("") owns the attached null. Do not restore
            // an older baseline over a direct application replacement.
            if ( !ToolTipService::GetToolTip(content) &&
                 !wxWinUIReadToolTipLocalValue(content) )
            {
                wxWinUIRestoreToolTipLocalValue(
                    content, m_originalToolTipLocalValue);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("restore slot empty tooltip mask", e);
            ok = false;
        }
    }
    else if ( m_toolTipSuppressed )
    {
        // An application-authored tooltip was detached by the global disable
        // policy. Once the element leaves the host, restore that exact object:
        // wx must not leak a process-policy null onto application-owned XAML.
        try
        {
            using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;
            if ( !ToolTipService::GetToolTip(content) &&
                 !wxWinUIReadToolTipLocalValue(content) )
            {
                wxWinUIRestoreToolTipLocalValue(
                    content, m_originalToolTipLocalValue);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("restore slot authored tooltip", e);
            ok = false;
        }
    }

    if ( !ok || m_content != content || m_contentGeneration != generation )
        return false;

    m_toolTipSynced = false;
    m_toolTipOwned = false;
    m_toolTipSuppressed = false;
    m_lastToolTipPresent = false;
    m_lastToolTip.clear();
    m_lastToolTipMaximumWidth = 0;
    m_lastToolTipDPI = 0;
    m_lastAppliedToolTip = nullptr;
    m_originalToolTip = nullptr;
    m_originalToolTipLocalValue = nullptr;
#endif
    return true;
}

void wxWinUITopLevelHost::SynchronizeSlotToolTipPolicy(wxWinUISlot& slot)
{
#if wxUSE_TOOLTIPS
    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(&slot);
    ++slot.m_toolTipPolicySyncDepth;
    const auto toolTipPolicySyncGuard = wxMakeGuard([&slot]()
    {
        wxASSERT_MSG(slot.m_toolTipPolicySyncDepth != 0,
                     "unbalanced WinUI tooltip policy transaction");
        --slot.m_toolTipPolicySyncDepth;
    });
    wxUnusedVar(toolTipPolicySyncGuard);

    if ( ++slot.m_syncEpoch == 0 )
        ++slot.m_syncEpoch;
    const unsigned long long syncEpoch = slot.m_syncEpoch;

    if ( slot.m_contentTransactionInProgress )
    {
        slot.NudgeDirty();
        return;
    }

    const auto state = slot.m_lifetime;
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    const auto content = slot.m_content;
    const unsigned long long contentGeneration = slot.m_contentGeneration;
    const bool enabled = wxWinUIAreToolTipsEnabled();
    if ( !window || !content || !state || state->GetHost() != this )
        return;

    wxToolTip * const tip = window->GetToolTip();
    const bool tipPresent = tip != nullptr;
    const wxString tipText = tip ? tip->GetTip() : wxString();
    const int maximumWidth =
        tip ? tip->GetWinUIMaxWidthAtCreation() : 0;
    const int toolTipDPI =
        maximumWidth > 0 ? window->GetDPI().GetWidth() : 0;

    const auto getCurrentSlot = [&]() -> wxWinUISlot *
    {
        if ( !state || state->GetHost() != this ||
             state->GetWindow() != window ||
             wxWinUIAreToolTipsEnabled() != enabled )
        {
            return nullptr;
        }
        wxWinUISlot * const current = FindSlot(window);
        if ( !current ||
             current->m_lifetime != state ||
             current->m_content != content ||
             current->m_contentGeneration != contentGeneration ||
             current->m_syncEpoch != syncEpoch ||
             current->m_contentTransactionInProgress )
        {
            return nullptr;
        }

        wxToolTip * const currentTip = window->GetToolTip();
        if ( currentTip != tip )
            return nullptr;
        if ( currentTip )
        {
            const int currentMaximumWidth =
                currentTip->GetWinUIMaxWidthAtCreation();
            const int currentDPI =
                currentMaximumWidth > 0
                    ? window->GetDPI().GetWidth()
                    : 0;
            if ( currentTip->GetTip() != tipText ||
                 currentMaximumWidth != maximumWidth ||
                 currentDPI != toolTipDPI )
            {
                return nullptr;
            }
        }
        return current;
    };

    bool owned = slot.m_toolTipOwned;
    bool suppressed = slot.m_toolTipSuppressed;
    auto applied = slot.m_lastAppliedToolTip;
    auto original = slot.m_originalToolTip;
    auto originalLocal = slot.m_originalToolTipLocalValue;
    const bool wxChanged =
        slot.m_lastToolTipPresent != tipPresent ||
        slot.m_lastToolTip != tipText ||
        slot.m_lastToolTipMaximumWidth != maximumWidth ||
        slot.m_lastToolTipDPI != toolTipDPI;

    using winrt::Microsoft::UI::Xaml::Controls::ToolTipService;

    try
    {
        auto actual = ToolTipService::GetToolTip(content);
        auto actualLocal =
            wxWinUIReadToolTipLocalValue(content);
        if ( !getCurrentSlot() )
            return;

        const auto applyWxOverride = [&]()
        {
            winrt::Windows::Foundation::IInspectable candidate{ nullptr };
            if ( !wxWinUISetToolTip(
                     content, tipText, window, &candidate, maximumWidth) )
            {
                return false;
            }
            if ( !getCurrentSlot() )
                return false;

            const auto post = ToolTipService::GetToolTip(content);
            if ( !getCurrentSlot() )
                return false;

            const bool stillManaged =
                candidate &&
                (post == candidate ||
                 wxWinUIIsManagedToolTipSuppressed(content, candidate) ||
                 wxWinUIIsManagedToolTipCurrent(content, candidate));
            if ( tipText.empty() ? !post : stillManaged )
            {
                owned = true;
                applied = candidate;
            }
            else
            {
                owned = false;
                applied = nullptr;
                if ( candidate &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         candidate) )
                {
                    return false;
                }
                if ( !getCurrentSlot() )
                    return false;
                original = ToolTipService::GetToolTip(content);
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }
            actual = ToolTipService::GetToolTip(content);
            return getCurrentSlot() != nullptr;
        };

        const auto relinquishWxOverride = [&]()
        {
            if ( applied )
            {
                if ( !wxWinUIRestoreToolTip(
                         content, originalLocal, applied) )
                {
                    return false;
                }
            }
            else
            {
                const auto current = ToolTipService::GetToolTip(content);
                if ( !getCurrentSlot() )
                    return false;
                if ( !current &&
                     !wxWinUIReadToolTipLocalValue(content) )
                {
                    wxWinUIRestoreToolTipLocalValue(
                        content, originalLocal);
                }
            }
            if ( !getCurrentSlot() )
                return false;
            actual = ToolTipService::GetToolTip(content);
            actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return false;
            owned = false;
            applied = nullptr;
            original = actual;
            originalLocal =
                wxWinUIReadToolTipLocalValue(content);
            return true;
        };

        if ( !slot.m_toolTipSynced )
        {
            original = actual;
            originalLocal =
                wxWinUIReadToolTipLocalValue(content);
            owned = false;
            applied = nullptr;
            if ( tipPresent && !applyWxOverride() )
                return;
        }
        else
        {
            const bool applicationClearedSuppressedNull =
                suppressed &&
                actualLocal ==
                    winrt::Microsoft::UI::Xaml::
                        DependencyProperty::UnsetValue();
            if ( applicationClearedSuppressedNull )
            {
                // Effective null alone cannot prove that wx still owns the
                // suppression mask. ClearValue() is an application mutation
                // which deliberately removes our local null, even if no
                // effective-value callback fires.
                if ( applied &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         applied) )
                {
                    return;
                }
                if ( !getCurrentSlot() )
                    return;
                owned = false;
                applied = nullptr;
                original = actual;
                originalLocal = actualLocal;
            }

            const bool managedSuppressed =
                owned && applied &&
                wxWinUIIsManagedToolTipSuppressed(content, applied);
            if ( !applicationClearedSuppressedNull &&
                 owned &&
                 actual != applied &&
                 !managedSuppressed )
            {
                // The exact wx object no longer owns the property.
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                if ( applied &&
                     !wxWinUIRestoreToolTip(
                         content,
                         winrt::Microsoft::UI::Xaml::
                             DependencyProperty::UnsetValue(),
                         applied) )
                {
                    return;
                }
                if ( !getCurrentSlot() )
                    return;
                owned = false;
                applied = nullptr;
            }
            else if ( !owned && suppressed && actual )
            {
                // A direct write while disabled becomes the authored
                // baseline. It is suppressed below if the policy remains off.
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }
            else if ( !owned && !suppressed && !wxChanged )
            {
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
            }

            if ( owned )
            {
                if ( !tipPresent )
                {
                    if ( !relinquishWxOverride() )
                        return;
                }
                else if ( wxChanged && !applyWxOverride() )
                    return;
            }
            else if ( tipPresent && wxChanged )
            {
                original = actual;
                originalLocal =
                    wxWinUIReadToolTipLocalValue(content);
                if ( !applyWxOverride() )
                    return;
            }
        }

        if ( !enabled )
        {
            const wxWinUIToolTipSuppressionResult result =
                wxWinUISuppressSlotToolTip(
                    content,
                    getCurrentSlot,
                    owned,
                    applied,
                    original,
                    originalLocal);
            if ( result != wxWinUIToolTipSuppressionResult::Suppressed )
            {
                if ( result ==
                        wxWinUIToolTipSuppressionResult::BudgetExhausted )
                {
                    wxLogWarning(
                        "wxWinUI: synchronous slot tooltip suppression "
                        "replacement budget exhausted");
                    if ( wxWinUISlot * const current = getCurrentSlot() )
                        current->NudgeDirty();
                }
                else if ( result ==
                          wxWinUIToolTipSuppressionResult::Failed )
                {
                    if ( wxWinUISlot * const current = getCurrentSlot() )
                        current->NudgeDirty();
                }
                return;
            }
            suppressed = true;
        }
        else
        {
            actual = ToolTipService::GetToolTip(content);
            actualLocal =
                wxWinUIReadToolTipLocalValue(content);
            if ( !getCurrentSlot() )
                return;
            if ( suppressed )
            {
                if ( owned && tipPresent )
                {
                    if ( actual != applied && !applyWxOverride() )
                        return;
                }
                else if ( !actual && !actualLocal )
                {
                    wxWinUIRestoreToolTipLocalValue(
                        content, originalLocal);
                    if ( !getCurrentSlot() )
                        return;
                }
                else
                {
                    // A non-null effective value or an application-cleared
                    // local mask during suppression is the newer baseline.
                    original = actual;
                    originalLocal = actualLocal;
                    owned = false;
                    applied = nullptr;
                }
            }
            suppressed = false;
        }

        wxWinUISlot * const current = getCurrentSlot();
        if ( !current )
            return;
        current->m_toolTipSynced = true;
        current->m_toolTipOwned = owned;
        current->m_toolTipSuppressed = suppressed;
        current->m_lastToolTipPresent = tipPresent;
        current->m_lastToolTip = tipText;
        current->m_lastToolTipMaximumWidth = maximumWidth;
        current->m_lastToolTipDPI = toolTipDPI;
        current->m_lastAppliedToolTip = applied;
        current->m_originalToolTip = original;
        current->m_originalToolTipLocalValue = originalLocal;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("synchronize slot tooltip policy", e);
        slot.NudgeDirty();
    }
#else
    wxUnusedVar(slot);
#endif
}

void wxWinUITopLevelHost::TestFailToolTipObserverAdds(unsigned count)
{
    gs_failToolTipObserverAdds = count;
}

#endif // wxUSE_WINUI3
