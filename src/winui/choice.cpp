/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/choice.cpp
// Purpose:     wxWinUI wxChoice implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/validate.h"
#endif

#include "private.h"
#include "wx/weakref.h"
#include "wx/winui/private/inputtest.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <utility>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace
{

template <typename T>
winrt::Windows::Foundation::IUnknown
wxWinUIChoiceObjectIdentity(const T& object) noexcept
{
    return object
        ? object.template try_as<winrt::Windows::Foundation::IUnknown>()
        : nullptr;
}

class wxWinUIPeerMutationGuard final
{
public:
    explicit wxWinUIPeerMutationGuard(
        const std::shared_ptr<wxWinUIChoiceCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUIPeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

private:
    std::shared_ptr<wxWinUIChoiceCallbackState> m_state;
};

bool wxWinUIChoiceHasModifiers(WXMSG *msg)
{
    wxWinUIKeyboardModifiers modifiers;
    if ( wxWinUI3GetKeyboardModifiersOverrideForTesting(&modifiers) )
    {
        return modifiers.shiftDown || modifiers.controlDown ||
               modifiers.leftAltDown || modifiers.rightAltDown;
    }

    return wxIsShiftDown() || wxIsCtrlDown() ||
           (msg && (HIWORD(msg->lParam) & KF_ALTDOWN));
}

bool wxWinUIChoiceHasItemPeer(const wxWinUIChoiceImpl *impl)
{
    return impl && (impl->simpleListBox || impl->comboBox);
}

winrt::Microsoft::UI::Xaml::Controls::ItemCollection
wxWinUIChoiceGetPeerItems(const wxWinUIChoiceImpl *impl)
{
    if ( impl->simpleListBox )
        return impl->simpleListBox.Items();

    return impl->comboBox.Items();
}

int wxWinUIChoiceGetPeerSelection(const wxWinUIChoiceImpl *impl)
{
    return impl->simpleListBox ? impl->simpleListBox.SelectedIndex()
                               : impl->comboBox.SelectedIndex();
}

void wxWinUIChoiceSetPeerSelection(wxWinUIChoiceImpl *impl, int selection)
{
    if ( impl->simpleListBox )
        impl->simpleListBox.SelectedIndex(selection);
    else
        impl->comboBox.SelectedIndex(selection);
}

winrt::Windows::Foundation::IInspectable
wxWinUICreateChoicePeerItem(const wxWinUIChoiceImpl *impl)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    if ( impl->simpleListBox )
        return MUXC::ListBoxItem();

    return MUXC::ComboBoxItem();
}

} // anonymous namespace

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;

wxWinUIComboPopupResolutionDiagnosticForTesting
    gs_comboPopupResolutionDiagnosticForTesting;

enum class wxWinUIElementRelationship
{
    Outside,
    Inside,
    Unavailable
};

wxWinUIElementRelationship wxWinUIIsElementWithin(
    const MUX::DependencyObject& element,
    const MUX::DependencyObject& root) noexcept
{
    if ( !element || !root )
        return wxWinUIElementRelationship::Outside;

    const WF::IUnknown rootIdentity = wxWinUIChoiceObjectIdentity(root);
    if ( !rootIdentity )
        return wxWinUIElementRelationship::Unavailable;

    try
    {
        MUX::DependencyObject current = element;
        for ( unsigned depth = 0; current && depth != 512; ++depth )
        {
            if ( wxWinUIChoiceObjectIdentity(current) == rootIdentity )
                return wxWinUIElementRelationship::Inside;
            current = MUXM::VisualTreeHelper::GetParent(current);
        }

        return current ? wxWinUIElementRelationship::Unavailable
                       : wxWinUIElementRelationship::Outside;
    }
    catch ( ... )
    {
        return wxWinUIElementRelationship::Unavailable;
    }
}

bool wxWinUIContainsGuid(const std::vector<winrt::guid>& values,
                         const winrt::guid& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

void wxWinUIEraseGuid(std::vector<winrt::guid>& values,
                      const winrt::guid& value)
{
    values.erase(std::remove(values.begin(), values.end(), value),
                 values.end());
}

class wxWinUIExactPopupRetirementGate final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<wxWinUIExactPopupRetirementGate>
{
public:
    using OwnerClosedProbe = std::function<bool ()>;

    static wxWinUIPopupRetirementCoreProbeForTesting ProbeForTesting()
    {
        wxWinUIPopupRetirementCoreProbeForTesting result;
        const bool poisonWasAlreadySet =
            wxWinUIIsPhysicalDisconnectPublicationPoisoned();

        auto detached =
            std::make_shared<wxWinUIExactPopupRetirementGate>();
        winrt::guid correlation{};
        correlation.Data1 = 1;
        detached->m_sessionActive = true;
        detached->m_resolved = true;
        detached->m_pendingCorrelations.push_back(correlation);
        // Deliberately leave m_child null: LostFocus must retire an admitted
        // ID without walking ancestry after Popup.Child was detached.
        detached->RetirePendingCorrelation(correlation);
        result.detachedCorrelationRetired =
            detached->m_pendingCorrelations.empty();
        detached->CompleteAtXamlBoundary();

        auto reopening =
            std::make_shared<wxWinUIExactPopupRetirementGate>();
        reopening->m_sessionActive = true;
        result.reopenRejected = !reopening->BeginUnresolvedSession(
            nullptr, nullptr);
        result.reopenDegraded = reopening->IsDegraded();
        reopening->CompleteAtXamlBoundary();

        if ( !poisonWasAlreadySet )
            wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
        return result;
    }

    bool BeginResolvedSession(
        const MUX::XamlRoot& xamlRoot,
        const MUXD::DispatcherQueue& queue,
        const MUXCP::Popup& popup,
        const MUX::UIElement& child,
        OwnerClosedProbe ownerClosedProbe) noexcept
    {
        if ( !BeginSession(xamlRoot, queue,
                           std::move(ownerClosedProbe)) )
        {
            return false;
        }
        return ResolvePopup(popup, child);
    }

    bool BeginUnresolvedSession(
        const MUX::XamlRoot& xamlRoot,
        const MUXD::DispatcherQueue& queue,
        OwnerClosedProbe ownerClosedProbe = {}) noexcept
    {
        return BeginSession(xamlRoot, queue,
                            std::move(ownerClosedProbe));
    }

    bool ResolvePopup(const MUXCP::Popup& popup,
                      const MUX::UIElement& child) noexcept
    {
        if ( m_terminal || m_degraded || !m_sessionActive ||
             m_resolved || !popup || !child )
        {
            Degrade();
            return false;
        }

        try
        {
            m_popup = popup;
            m_child = child;
            m_resolved = true;
            const std::uint64_t generation = m_generation;
            const std::weak_ptr<wxWinUIExactPopupRetirementGate> weak =
                shared_from_this();
            m_popupClosedToken = popup.Closed(
                [weak, generation](const WF::IInspectable&,
                                   const WF::IInspectable&)
                {
                    if ( const auto state = weak.lock() )
                        state->ObservePopupClosed(generation);
                });
            if ( !m_popupClosedToken.value )
            {
                Degrade();
                return false;
            }

            for ( const PendingFocusCandidate& candidate :
                  m_unresolvedCandidates )
            {
                const wxWinUIElementRelationship relationship =
                    wxWinUIIsElementWithin(candidate.oldElement, m_child);
                if ( relationship ==
                         wxWinUIElementRelationship::Unavailable )
                {
                    Degrade();
                    return false;
                }
                if ( relationship == wxWinUIElementRelationship::Inside &&
                     !wxWinUIContainsGuid(m_completedBeforeResolve,
                                          candidate.correlation) &&
                     !wxWinUIContainsGuid(m_pendingCorrelations,
                                          candidate.correlation) )
                {
                    m_pendingCorrelations.push_back(
                        candidate.correlation);
                }
            }
            m_unresolvedCandidates.clear();
            m_completedBeforeResolve.clear();
            TryScheduleTail();
            return !m_degraded;
        }
        catch ( ... )
        {
            Degrade();
            return false;
        }
    }

    void ObserveOwnerClosed() noexcept
    {
        if ( m_terminal || !m_sessionActive )
            return;
        m_ownerClosedSeen = true;
        TryScheduleTail();
    }

    void MarkDegraded() noexcept { Degrade(); }

    void PreserveDispatcherQueue(
        const MUXD::DispatcherQueue& queue) noexcept
    {
        if ( m_queue || !queue )
            return;
        try
        {
            m_queue = queue;
        }
        catch ( ... )
        {
        }
    }

    bool IsSatisfied() const noexcept override
    {
        return m_satisfied && !m_degraded && !m_hostShutdownSealed;
    }

    bool IsDegraded() const noexcept override { return m_degraded; }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_hostShutdownSealed; }
    bool IsSessionActive() const noexcept { return m_sessionActive; }
    bool HasCompletionForTesting() const noexcept
        { return static_cast<bool>(m_completion); }

    MUXD::DispatcherQueue GetDispatcherQueue() const noexcept override
    {
        try
        {
            return m_queue;
        }
        catch ( ... )
        {
            return nullptr;
        }
    }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        if ( m_terminal || !complete )
            return false;

        try
        {
            m_completion = std::move(complete);
        }
        catch ( ... )
        {
            Degrade();
            return false;
        }

        if ( IsSatisfied() )
            InvokeCompletion();
        // A degraded gate intentionally never calls this destination during
        // normal dispatch, but it must still accept retargeting so the TLW
        // terminal owner can retain and clean it at the XAML boundary.
        return true;
    }

    void SealForHostShutdown() noexcept override
    {
        m_hostShutdownSealed = true;
    }

    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        // Exact popup state has no owner property to sample. Its wrapper
        // performs this observation; an already armed session is itself debt.
    }

    bool CompleteSealedWithoutPopup() noexcept override
    {
        if ( m_sealedNoDebtCompleted )
            return true;
        if ( !m_hostShutdownSealed || m_degraded || m_sessionActive )
            return false;
        // Sealing permanently revokes publication authority. This consumes
        // only the provisional host ticket for a peer with no popup debt.
        m_sealedNoDebtCompleted = true;
        m_satisfied = true;
        InvokeCompletion();
        return true;
    }

    void CompleteAtXamlBoundary() noexcept override
    {
        if ( m_terminal )
            return;
        m_terminal = true;
        m_completion = {};
        CleanupSession(true);
        m_satisfied = true;
    }

private:
    struct PendingFocusCandidate
    {
        winrt::guid correlation{};
        MUX::DependencyObject oldElement{ nullptr };
    };

    bool BeginSession(const MUX::XamlRoot& xamlRoot,
                      const MUXD::DispatcherQueue& queue,
                      OwnerClosedProbe ownerClosedProbe) noexcept
    {
        // A reopen before the previous generation's exact LostFocus/tail has
        // completed cannot reuse this observer: revoking its static handlers
        // here would erase correlations which are still causally pending.
        // Fail closed and keep that one previous graph for terminal cleanup;
        // the process poison blocks any later popup publication.
        if ( m_terminal || m_hostShutdownSealed || m_completion ||
             m_sessionActive )
        {
            Degrade();
            return false;
        }
        if ( !CleanupSession(false) )
        {
            Degrade();
            return false;
        }
        if ( !queue )
        {
            Degrade();
            return false;
        }

        try
        {
            // Preserve the real queue even if the XamlRoot/exact-popup proof
            // below fails. A degraded gate still needs a queue-bound terminal
            // retirement ticket; null-queue quarantine is not acceptable.
            m_queue = queue;
            if ( !xamlRoot )
            {
                Degrade();
                return false;
            }
            if ( ++m_generation == 0 )
                ++m_generation;
            m_xamlRoot = xamlRoot;
            m_ownerClosedProbe = std::move(ownerClosedProbe);
            m_sessionActive = true;
            m_satisfied = false;
            m_resolved = false;
            m_ownerClosedSeen = false;
            m_popupClosedSeen = false;
            m_tailScheduled = false;
            m_pendingCorrelations.clear();
            m_unresolvedCandidates.clear();
            m_completedBeforeResolve.clear();

            const std::uint64_t generation = m_generation;
            const std::weak_ptr<wxWinUIExactPopupRetirementGate> weak =
                shared_from_this();
            m_losingFocusToken = MUXI::FocusManager::LosingFocus(
                [weak, generation](const WF::IInspectable&,
                                   const MUXI::LosingFocusEventArgs& args)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveLosingFocus(generation, args);
                });
            if ( !m_losingFocusToken.value )
            {
                Degrade();
                return false;
            }
            m_lostFocusToken = MUXI::FocusManager::LostFocus(
                [weak, generation](const WF::IInspectable&,
                    const MUXI::FocusManagerLostFocusEventArgs& args)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveLostFocus(generation, args);
                });
            if ( !m_lostFocusToken.value )
            {
                Degrade();
                return false;
            }
            return true;
        }
        catch ( ... )
        {
            Degrade();
            return false;
        }
    }

    void ObserveLosingFocus(
        std::uint64_t generation,
        const MUXI::LosingFocusEventArgs& args) noexcept
    {
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }

        try
        {
            const MUX::DependencyObject oldElement =
                args.OldFocusedElement();
            const winrt::guid correlation = args.CorrelationId();
            if ( !m_resolved )
            {
                m_unresolvedCandidates.push_back(
                    PendingFocusCandidate{correlation, oldElement});
                ScheduleLosingFocusCancellationObservation(
                    generation, correlation, args);
                return;
            }

            const wxWinUIElementRelationship relationship =
                wxWinUIIsElementWithin(oldElement, m_child);
            if ( relationship == wxWinUIElementRelationship::Unavailable )
            {
                Degrade();
                return;
            }
            if ( relationship == wxWinUIElementRelationship::Inside &&
                 !wxWinUIContainsGuid(m_pendingCorrelations, correlation) )
            {
                m_pendingCorrelations.push_back(correlation);
                ScheduleLosingFocusCancellationObservation(
                    generation, correlation, args);
            }
        }
        catch ( ... )
        {
            Degrade();
        }
    }

    void ObserveLostFocus(
        std::uint64_t generation,
        const MUXI::FocusManagerLostFocusEventArgs& args) noexcept
    {
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }

        try
        {
            const winrt::guid correlation = args.CorrelationId();
            if ( !m_resolved )
            {
                if ( !wxWinUIContainsGuid(m_completedBeforeResolve,
                                          correlation) )
                {
                    m_completedBeforeResolve.push_back(correlation);
                }
                return;
            }

            // Exact ancestry was proved when this ID was admitted by
            // LosingFocus. By LostFocus the popup child may already be
            // detached, so walking its ancestry again would turn the normal
            // causal completion into a false degraded fault.
            RetirePendingCorrelation(correlation);
            TryScheduleTail();
        }
        catch ( ... )
        {
            Degrade();
        }
    }

    void ObservePopupClosed(std::uint64_t generation) noexcept
    {
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }

        m_popupClosedSeen = true;
        if ( m_ownerClosedProbe )
        {
            try
            {
                if ( m_ownerClosedProbe() )
                    m_ownerClosedSeen = true;
            }
            catch ( ... )
            {
                Degrade();
                return;
            }
        }
        TryScheduleTail();
    }

    void RetirePendingCorrelation(const winrt::guid& correlation) noexcept
    {
        if ( wxWinUIContainsGuid(m_pendingCorrelations, correlation) )
            wxWinUIEraseGuid(m_pendingCorrelations, correlation);
    }

    void ScheduleLosingFocusCancellationObservation(
        std::uint64_t generation,
        const winrt::guid& correlation,
        const MUXI::LosingFocusEventArgs& args) noexcept
    {
        if ( wxWinUIContainsGuid(m_cancelObservations, correlation) )
            return;

        try
        {
            m_cancelObservations.push_back(correlation);
            const std::weak_ptr<wxWinUIExactPopupRetirementGate> weak =
                shared_from_this();
            const bool accepted = m_queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Normal,
                [weak, generation, correlation, losingArgs = args]()
                {
                    const auto state = weak.lock();
                    if ( !state )
                        return;
                    state->ObserveFinalLosingFocusCancellation(
                        generation, correlation, losingArgs);
                });
            if ( !accepted )
            {
                wxWinUIEraseGuid(m_cancelObservations, correlation);
                Degrade();
            }
        }
        catch ( ... )
        {
            wxWinUIEraseGuid(m_cancelObservations, correlation);
            Degrade();
        }
    }

    void ObserveFinalLosingFocusCancellation(
        std::uint64_t generation,
        const winrt::guid& correlation,
        const MUXI::LosingFocusEventArgs& args) noexcept
    {
        wxWinUIEraseGuid(m_cancelObservations, correlation);
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }

        try
        {
            // This queue turn is solely an observation of the final Cancel
            // value after every LosingFocus subscriber returned. It is not
            // the causal retirement tail and cannot complete a popup by
            // itself. LostFocus may already have won and retired the same ID.
            if ( !args.Cancel() )
                return;

            RetirePendingCorrelation(correlation);
            m_unresolvedCandidates.erase(
                std::remove_if(
                    m_unresolvedCandidates.begin(),
                    m_unresolvedCandidates.end(),
                    [&correlation](const PendingFocusCandidate& candidate)
                    {
                        return candidate.correlation == correlation;
                    }),
                m_unresolvedCandidates.end());
            wxWinUIEraseGuid(m_completedBeforeResolve, correlation);
            TryScheduleTail();
        }
        catch ( ... )
        {
            Degrade();
        }
    }

    bool CausalFactsReady(bool *unavailable) noexcept
    {
        *unavailable = false;
        if ( m_terminal || m_degraded || !m_sessionActive || !m_resolved ||
             !m_ownerClosedSeen || !m_popupClosedSeen ||
             !m_pendingCorrelations.empty() || !m_popup || !m_child ||
             !m_xamlRoot )
        {
            return false;
        }

        try
        {
            if ( m_popup.IsOpen() )
                return false;
            if ( m_ownerClosedProbe && !m_ownerClosedProbe() )
                return false;

            const WF::IInspectable focusedInspectable =
                MUXI::FocusManager::GetFocusedElement(m_xamlRoot);
            const MUX::DependencyObject focused =
                focusedInspectable.try_as<MUX::DependencyObject>();
            if ( focusedInspectable && !focused )
            {
                *unavailable = true;
                return false;
            }
            const wxWinUIElementRelationship relationship =
                wxWinUIIsElementWithin(focused, m_child);
            if ( relationship == wxWinUIElementRelationship::Unavailable )
            {
                *unavailable = true;
                return false;
            }
            return relationship == wxWinUIElementRelationship::Outside;
        }
        catch ( ... )
        {
            *unavailable = true;
            return false;
        }
    }

    void TryScheduleTail() noexcept
    {
        if ( m_tailScheduled || m_terminal || m_degraded )
            return;

        bool unavailable = false;
        if ( !CausalFactsReady(&unavailable) )
        {
            if ( unavailable )
                Degrade();
            return;
        }

        m_tailScheduled = true;
        const std::uint64_t generation = m_generation;
        const std::weak_ptr<wxWinUIExactPopupRetirementGate> weak =
            shared_from_this();
        bool accepted = false;
        try
        {
            accepted = m_queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Normal,
                [weak, generation]()
                {
                    if ( const auto state = weak.lock() )
                        state->RunTail(generation);
                });
        }
        catch ( ... )
        {
        }
        if ( !accepted )
        {
            m_tailScheduled = false;
            Degrade();
        }
    }

    void RunTail(std::uint64_t generation) noexcept
    {
        if ( generation != m_generation || m_terminal || m_degraded )
            return;
        m_tailScheduled = false;

        bool unavailable = false;
        if ( !CausalFactsReady(&unavailable) )
        {
            if ( unavailable )
                Degrade();
            return;
        }

        if ( !CleanupSession(false) )
        {
            Degrade();
            return;
        }
        m_satisfied = true;
        InvokeCompletion();
    }

    bool CleanupSession(bool terminal) noexcept
    {
        bool ok = true;
        if ( m_popup && m_popupClosedToken.value )
        {
            try
            {
                m_popup.Closed(m_popupClosedToken);
                m_popupClosedToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }
        if ( m_losingFocusToken.value )
        {
            try
            {
                MUXI::FocusManager::LosingFocus(m_losingFocusToken);
                m_losingFocusToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }
        if ( m_lostFocusToken.value )
        {
            try
            {
                MUXI::FocusManager::LostFocus(m_lostFocusToken);
                m_lostFocusToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }

        if ( ok || terminal )
        {
            m_popupClosedToken = {};
            m_losingFocusToken = {};
            m_lostFocusToken = {};
            m_popup = nullptr;
            m_child = nullptr;
            m_xamlRoot = nullptr;
            if ( terminal )
                m_queue = nullptr;
            m_ownerClosedProbe = {};
            m_pendingCorrelations.clear();
            m_unresolvedCandidates.clear();
            m_completedBeforeResolve.clear();
            m_cancelObservations.clear();
            m_sessionActive = false;
            m_resolved = false;
            m_tailScheduled = false;
        }
        return ok;
    }

    void InvokeCompletion() noexcept
    {
        if ( !m_completion )
            return;
        std::function<void ()> complete = std::move(m_completion);
        try
        {
            complete();
        }
        catch ( ... )
        {
            Degrade();
        }
    }

    void Degrade() noexcept
    {
        m_degraded = true;
        m_satisfied = false;
        m_tailScheduled = false;
        wxWinUINotePhysicalDisconnectGateDegraded();
    }

    MUX::XamlRoot m_xamlRoot{ nullptr };
    MUXD::DispatcherQueue m_queue{ nullptr };
    MUXCP::Popup m_popup{ nullptr };
    MUX::UIElement m_child{ nullptr };
    winrt::event_token m_popupClosedToken{};
    winrt::event_token m_losingFocusToken{};
    winrt::event_token m_lostFocusToken{};
    OwnerClosedProbe m_ownerClosedProbe;
    std::function<void ()> m_completion;
    std::vector<winrt::guid> m_pendingCorrelations;
    std::vector<PendingFocusCandidate> m_unresolvedCandidates;
    std::vector<winrt::guid> m_completedBeforeResolve;
    std::vector<winrt::guid> m_cancelObservations;
    std::uint64_t m_generation = 0;
    bool m_sessionActive = false;
    bool m_resolved = false;
    bool m_ownerClosedSeen = false;
    bool m_popupClosedSeen = false;
    bool m_tailScheduled = false;
    bool m_satisfied = true;
    bool m_degraded = false;
    bool m_terminal = false;
    bool m_hostShutdownSealed = false;
    bool m_sealedNoDebtCompleted = false;
};

class wxWinUICompositePhysicalDisconnectGate final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<
        wxWinUICompositePhysicalDisconnectGate>
{
public:
    bool Add(const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& child,
             bool reusable) noexcept
    {
        if ( !child || m_closing || m_hostShutdownSealed || m_terminal )
            return false;

        // IsDegraded() deliberately inspects every child dynamically: an
        // exact-popup gate can lose a hook after it was admitted. Latch that
        // observation before refusing this publication so the composite
        // itself remains poisoned even if terminal cleanup later drops the
        // child.
        if ( IsDegraded() )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return false;
        }

        // The child which reports the first fault must still be admitted so
        // its partially installed projection hooks have an owner. Once the
        // process poison predates a healthy child, however, no new popup
        // publication may join a live peer.
        if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() &&
             !child->IsDegraded() )
        {
            return false;
        }

        if ( !reusable &&
             std::any_of(
                 m_entries.begin(), m_entries.end(),
                 [](const Entry& entry)
                 {
                     return !entry.reusable &&
                            !entry.gate->IsSatisfied();
                 }) )
        {
            // At most one transient child may be current/retiring. Its owner
            // coalesces a replay on the child's causal completion.
            return false;
        }

        try
        {
            Entry entry;
            entry.id = ++m_nextId;
            if ( !entry.id )
                entry.id = ++m_nextId;
            entry.gate = child;
            entry.reusable = reusable;
            const std::uint64_t id = entry.id;
            m_entries.push_back(std::move(entry));

            if ( child->IsDegraded() )
            {
                m_degraded = true;
                wxWinUINotePhysicalDisconnectGateDegraded();
            }

            if ( !reusable )
            {
                const std::weak_ptr<
                    wxWinUICompositePhysicalDisconnectGate> weak =
                        shared_from_this();
                if ( !child->SetCompletion(
                         [weak, id]()
                         {
                             if ( const auto composite = weak.lock() )
                                 composite->ChildCompleted(id);
                         }) )
                {
                    m_degraded = true;
                    wxWinUINotePhysicalDisconnectGateDegraded();
                    return false;
                }
            }
            return true;
        }
        catch ( ... )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return false;
        }
    }

    bool IsSatisfied() const noexcept override
    {
        if ( m_degraded || m_hostShutdownSealed )
            return false;
        return std::all_of(
            m_entries.begin(), m_entries.end(),
            [](const Entry& entry) { return entry.gate->IsSatisfied(); });
    }

    bool IsDegraded() const noexcept override
    {
        if ( m_degraded )
            return true;
        return std::any_of(
            m_entries.begin(), m_entries.end(),
            [](const Entry& entry) { return entry.gate->IsDegraded(); });
    }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_hostShutdownSealed; }

    MUXD::DispatcherQueue GetDispatcherQueue() const noexcept override
    {
        MUXD::DispatcherQueue fallback{ nullptr };
        for ( const Entry& entry : m_entries )
        {
            const MUXD::DispatcherQueue queue =
                entry.gate->GetDispatcherQueue();
            if ( !fallback && queue )
                fallback = queue;
            if ( !entry.gate->IsSatisfied() && queue )
                return queue;
        }
        return fallback;
    }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        if ( !complete || m_terminal )
            return false;

        try
        {
            m_completion = std::move(complete);
            if ( m_closing )
            {
                // TLW shutdown retargets the already-installed fan-in. Child
                // callbacks continue to address this composite; only its
                // final destination changes from the invalidated host to the
                // terminal retirement ticket.
                MaybeComplete();
                return true;
            }
            m_closing = true;
            const std::weak_ptr<
                wxWinUICompositePhysicalDisconnectGate> weak =
                    shared_from_this();
            std::vector<std::pair<
                std::uint64_t,
                std::shared_ptr<wxWinUIPhysicalDisconnectGate>>> reusable;
            reusable.reserve(m_entries.size());
            for ( const Entry& entry : m_entries )
            {
                if ( entry.reusable )
                    reusable.emplace_back(entry.id, entry.gate);
            }
            for ( const auto& entry : reusable )
            {
                const std::uint64_t id = entry.first;
                if ( !entry.second->SetCompletion(
                         [weak, id]()
                         {
                             if ( const auto composite = weak.lock() )
                                 composite->ChildCompleted(id);
                         }) )
                {
                    m_degraded = true;
                    wxWinUINotePhysicalDisconnectGateDegraded();
                    return false;
                }
            }
            MaybeComplete();
            return true;
        }
        catch ( ... )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return false;
        }
    }

    void SealForHostShutdown() noexcept override
    {
        if ( m_hostShutdownSealed )
            return;
        m_hostShutdownSealed = true;
        for ( const Entry& entry : m_entries )
            entry.gate->SealForHostShutdown();
    }

    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> snapshot;
        try
        {
            snapshot.reserve(m_entries.size());
            for ( const Entry& entry : m_entries )
                snapshot.push_back(entry.gate);
        }
        catch ( ... )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return;
        }
        for ( const auto& child : snapshot )
        {
            if ( child )
                child->LatchPublishedPopupDebtBeforeSourceClose();
        }
    }

    bool CompleteSealedWithoutPopup() noexcept override
    {
        if ( m_sealedNoDebtCompleted )
            return true;
        if ( !m_hostShutdownSealed || m_degraded || IsDegraded() )
            return false;

        // A child may synchronously erase itself through ChildCompleted().
        // Never retain an iterator/reference into m_entries across that call.
        std::vector<std::pair<
            std::uint64_t,
            std::shared_ptr<wxWinUIPhysicalDisconnectGate>>> snapshot;
        try
        {
            snapshot.reserve(m_entries.size());
            for ( const Entry& entry : m_entries )
                snapshot.emplace_back(entry.id, entry.gate);
        }
        catch ( ... )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
            return false;
        }

        bool allCompleted = true;
        for ( const auto& candidate : snapshot )
        {
            const auto live = std::find_if(
                m_entries.begin(), m_entries.end(),
                [&candidate](const Entry& entry)
                {
                    return entry.id == candidate.first;
                });
            if ( live == m_entries.end() )
                continue;

            const std::shared_ptr<wxWinUIPhysicalDisconnectGate> child =
                candidate.second;
            if ( !child || !child->CompleteSealedWithoutPopup() )
                allCompleted = false;
        }

        m_sealedNoDebtCompleted = allCompleted && m_entries.empty();
        MaybeComplete();
        return m_sealedNoDebtCompleted;
    }

    void CompleteAtXamlBoundary() noexcept override
    {
        if ( m_terminal )
            return;
        m_terminal = true;
        m_completion = {};
        std::vector<Entry> entries = std::move(m_entries);
        m_entries.clear();
        for ( const Entry& entry : entries )
            entry.gate->CompleteAtXamlBoundary();
    }

private:
    struct Entry
    {
        std::uint64_t id = 0;
        std::shared_ptr<wxWinUIPhysicalDisconnectGate> gate;
        bool reusable = false;
    };

    void ChildCompleted(std::uint64_t id) noexcept
    {
        const auto it = std::find_if(
            m_entries.begin(), m_entries.end(),
            [id](const Entry& entry) { return entry.id == id; });
        if ( it != m_entries.end() )
            m_entries.erase(it);
        MaybeComplete();
    }

    void MaybeComplete() noexcept
    {
        if ( !m_closing || m_terminal || m_degraded ||
             IsDegraded() || !m_entries.empty() || !m_completion )
        {
            return;
        }

        std::function<void ()> complete = std::move(m_completion);
        try
        {
            complete();
        }
        catch ( ... )
        {
            m_degraded = true;
            wxWinUINotePhysicalDisconnectGateDegraded();
        }
    }

    std::vector<Entry> m_entries;
    std::function<void ()> m_completion;
    std::uint64_t m_nextId = 0;
    bool m_closing = false;
    bool m_hostShutdownSealed = false;
    bool m_sealedNoDebtCompleted = false;
    bool m_degraded = false;
    bool m_terminal = false;
};

} // anonymous namespace

void wxWinUIResetComboPopupResolutionDiagnosticForTesting() noexcept
{
    gs_comboPopupResolutionDiagnosticForTesting = {};
}

wxWinUIComboPopupResolutionDiagnosticForTesting
wxWinUIGetComboPopupResolutionDiagnosticForTesting() noexcept
{
    return gs_comboPopupResolutionDiagnosticForTesting;
}

class wxWinUIComboPopupRetirementState final :
    public wxWinUIPhysicalDisconnectGate
{
public:
    explicit wxWinUIComboPopupRetirementState(const MUXC::ComboBox& combo)
        : m_combo(combo),
          m_gate(std::make_shared<wxWinUIExactPopupRetirementGate>())
    {
        if ( m_combo )
            m_gate->PreserveDispatcherQueue(m_combo.DispatcherQueue());
    }

    bool PrepareForOpen() noexcept
    {
        if ( !m_combo || m_hostShutdownSealed || m_gate->IsDegraded() ||
             m_gate->IsSessionActive() || m_preparedForOpen ||
             wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
        {
            return false;
        }

        try
        {
            // This preparation is deliberately debt-free. A public Popup()
            // issued in the same stack as Show() can reach us before the peer
            // has joined an island. In that case opening is a normal no-op:
            // there is no XamlRoot/PopupRoot which could own a physical popup.
            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
            const MUXD::DispatcherQueue queue = m_combo.DispatcherQueue();
            if ( !xamlRoot || !queue )
                return false;

            const MUXC::IControlProtected control =
                m_combo.try_as<MUXC::IControlProtected>();
            if ( !control )
                return false;
            const MUXCP::Popup popup =
                control.GetTemplateChild(L"Popup").try_as<MUXCP::Popup>();
            if ( !popup )
                return false;
            const MUX::UIElement child = popup.Child();
            if ( !child )
                return false;

            const std::uintptr_t rootIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(xamlRoot)));
            const std::uintptr_t popupIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(popup)));
            const std::uintptr_t childIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(child)));
            if ( !rootIdentity || !popupIdentity || !childIdentity )
                return false;

            // A still-open peer belongs either to the current logical
            // generation (handled by WinUICoalescePopupReopen) or to a native
            // transition already in flight. Never start a second generation
            // from this preparation path.
            if ( m_combo.IsDropDownOpen() || popup.IsOpen() )
                return false;

            m_gate->PreserveDispatcherQueue(queue);
            m_preparedXamlRootIdentity = rootIdentity;
            m_preparedPopupIdentity = popupIdentity;
            m_preparedPopupChildIdentity = childIdentity;
            m_preparedForOpen = true;
            return true;
        }
        catch ( ... )
        {
            ClearPreparedOpen();
            return false;
        }
    }

    void CancelPreparedOpen() noexcept
    {
        ClearPreparedOpen();
    }

    bool FinishPreparedOpen() noexcept
    {
        if ( m_hostShutdownSealed || m_gate->IsDegraded() || !m_combo )
            return false;

        // The source event normally consumes the preparation synchronously.
        // Revalidate that already-owned generation once more after the setter.
        if ( m_gate->IsSessionActive() )
        {
            try
            {
                // A wx DROPDOWN handler may synchronously call Dismiss(). The
                // exact Closed hook already owns that graph; post-setter
                // reconciliation must not reinterpret this normal causal
                // close as a failed opening.
                if ( m_ownerClosedObserved || !m_combo.IsDropDownOpen() )
                    return !m_gate->IsDegraded();
            }
            catch ( ... )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReasonForTesting::
                        ObserveOpenedException);
                return false;
            }
            return ResolveAfterOpen();
        }
        if ( !m_preparedForOpen )
            return false;

        try
        {
            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
            const MUXC::IControlProtected control =
                m_combo.try_as<MUXC::IControlProtected>();
            const MUXCP::Popup popup = control
                ? control.GetTemplateChild(L"Popup").try_as<MUXCP::Popup>()
                : nullptr;
            const MUX::UIElement child = popup ? popup.Child() : nullptr;
            const bool identitiesMatch = xamlRoot && popup && child &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(xamlRoot))) ==
                    m_preparedXamlRootIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(popup))) ==
                    m_preparedPopupIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(child))) ==
                    m_preparedPopupChildIdentity;
            if ( !identitiesMatch )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReasonForTesting::
                        StrictResolutionRejected);
                return false;
            }

            const bool propertyOpen = m_combo.IsDropDownOpen();
            const bool popupOpen = popup.IsOpen();
            if ( !propertyOpen && !popupOpen )
            {
                // The setter was refused before publishing anything. No
                // physical graph exists and the debt-free preparation can be
                // discarded normally.
                ClearPreparedOpen();
                return false;
            }
            if ( !propertyOpen || !popupOpen )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReasonForTesting::
                        StrictResolutionRejected);
                return false;
            }

            return ObserveOpened() && ResolveAfterOpen();
        }
        catch ( ... )
        {
            FailClose(
                wxWinUIComboPopupFailCloseReasonForTesting::
                    ObserveOpenedException);
            return false;
        }
    }

    void AbortPreparedOpen() noexcept
    {
        if ( !m_preparedForOpen && !m_gate->IsSessionActive() )
            return;

        // An exception after touching IsDropDownOpen leaves publication
        // ambiguous. Retain the real queue/root through the degraded terminal
        // owner and close best-effort; never erase the preparation blindly.
        FailClose(
            wxWinUIComboPopupFailCloseReasonForTesting::
                ObserveOpenedException);
    }

    bool ObserveOpened() noexcept
    {
        // A fault in any exact-popup gate closes the process-wide publication
        // door. Existing controls remain usable while closed, but must not
        // create another popup graph which could later join the exceptional
        // retirement set.
        if ( m_hostShutdownSealed )
        {
            // A native/user open can race after the host sealed this peer.
            // Its queue and complete graph are already owned by the TLW
            // ledger, so close/degrade and let that terminal owner retire it;
            // this is not the pre-existing-poison fail-fast path.
            FailClose(
                wxWinUIComboPopupFailCloseReasonForTesting::
                    HostShutdownSealed);
            return false;
        }
        if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
        {
            RefusePoisonedPublication();
            return false;
        }

        // A public Popup() performs this observation again after the native
        // setter returns. The first DropDownOpened subscriber must already
        // have proved the exact popup; this second call is only an idempotent
        // revalidation, not permission to install a late Closed handler. A
        // real programmatic reopen is coalesced before the peer property is
        // set while the previous session remains active.
        if ( m_gate->IsSessionActive() )
        {
            // A duplicate notification for the same still-open generation is
            // harmless and is fully revalidated below. Once DropDownClosed
            // was observed, however, a native/user reopen before the causal
            // tail must not reuse the old Popup.Closed/focus session.
            if ( m_ownerClosedObserved )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReasonForTesting::
                        ReopenBeforeRetirement);
                return false;
            }
            return ResolveOpenedPopupStrict(
                wxWinUIComboPopupResolutionOriginForTesting::
                    DropDownOpenedExistingSession);
        }

        try
        {
            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
            const MUXD::DispatcherQueue queue = m_combo.DispatcherQueue();
            StartOpenResolutionGeneration(xamlRoot);
            BeginResolutionDiagnostic(
                wxWinUIComboPopupResolutionOriginForTesting::
                    DropDownOpenedNewSession);
            gs_comboPopupResolutionDiagnosticForTesting.rootAvailable =
                xamlRoot != nullptr;
            gs_comboPopupResolutionDiagnosticForTesting.queueAvailable =
                queue != nullptr;
            gs_comboPopupResolutionDiagnosticForTesting.sessionActive =
                m_gate->IsSessionActive();
            gs_comboPopupResolutionDiagnosticForTesting.completionArmed =
                m_gate->HasCompletionForTesting();
            if ( !m_gate->BeginUnresolvedSession(
                     xamlRoot, queue,
                     [combo = m_combo]()
                     {
                         return !combo || !combo.IsDropDownOpen();
                     }) )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReasonForTesting::
                        BeginSessionRejected);
                return false;
            }
            const bool resolved = ResolveOpenedPopupStrict(
                wxWinUIComboPopupResolutionOriginForTesting::
                    DropDownOpenedNewSession);
            ClearPreparedOpen();
            return resolved;
        }
        catch ( ... )
        {
            FailClose(
                wxWinUIComboPopupFailCloseReasonForTesting::
                    ObserveOpenedException);
            return false;
        }
    }

    bool ResolveAfterOpen() noexcept
    {
        if ( m_hostShutdownSealed || m_gate->IsDegraded() ||
             !m_gate->IsSessionActive() )
        {
            RecordUnavailableResolution(
                wxWinUIComboPopupResolutionOriginForTesting::
                    AfterNativeSetter);
            return false;
        }

        // DropDownOpened is the only safe publication boundary: WinUI has
        // already opened and registered the Popup, while no wx callback has
        // run yet. A programmatic caller reaches this method after the setter
        // only to revalidate the identity armed by that first subscriber (or
        // to cover a platform path which deferred the source event). Never
        // install Popup.Closed from a later dispatcher turn.
        return ResolveOpenedPopupStrict(
            wxWinUIComboPopupResolutionOriginForTesting::AfterNativeSetter);
    }

    void ObserveClosed() noexcept
    {
        m_ownerClosedObserved = true;
        m_gate->ObserveOwnerClosed();
    }

    void RequestClose() noexcept
    {
        try
        {
            // A programmatic IsDropDownOpen(true) can be accepted before its
            // native DropDownOpened delegate is delivered. Resolve and arm
            // the exact popup before issuing the close in that case.
            if ( IsSatisfied() && m_combo && m_combo.IsDropDownOpen() )
                ObserveOpened();
            if ( IsSatisfied() )
                return;
            if ( m_combo && m_combo.IsDropDownOpen() )
                m_combo.IsDropDownOpen(false);
            if ( m_combo && !m_combo.IsDropDownOpen() )
                m_gate->ObserveOwnerClosed();
        }
        catch ( ... )
        {
            FailClose(
                wxWinUIComboPopupFailCloseReasonForTesting::
                    RequestCloseException);
        }
    }

    bool IsSatisfied() const noexcept override
        { return m_gate->IsSatisfied(); }
    bool IsDegraded() const noexcept override
        { return m_gate->IsDegraded(); }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_hostShutdownSealed; }
    MUXD::DispatcherQueue GetDispatcherQueue() const noexcept override
        { return m_gate->GetDispatcherQueue(); }
    bool SetCompletion(std::function<void ()> complete) noexcept override
        { return m_gate->SetCompletion(std::move(complete)); }
    void SealForHostShutdown() noexcept override
    {
        InvalidateOpenResolution();
        m_hostShutdownSealed = true;
        try
        {
            if ( m_combo )
                m_gate->PreserveDispatcherQueue(m_combo.DispatcherQueue());
        }
        catch ( ... )
        {
            m_gate->MarkDegraded();
        }
        m_gate->SealForHostShutdown();
        LatchPublishedPopupDebtBeforeSourceClose();
    }
    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        if ( m_publishedDebtLatched )
            return;
        if ( m_gate->IsSessionActive() )
        {
            m_publishedDebtLatched = true;
            return;
        }
        try
        {
            if ( m_combo && !m_combo.IsDropDownOpen() )
                return;
        }
        catch ( ... )
        {
            // Unknown is debt: Source.Close can erase the only later probe.
        }
        m_publishedDebtLatched = true;
        m_gate->MarkDegraded();
    }
    bool CompleteSealedWithoutPopup() noexcept override
    {
        if ( m_publishedDebtLatched )
            return false;
        try
        {
            // IsDropDownOpen(true) may be accepted before DropDownOpened is
            // delivered. Such a published-but-unobserved graph is debt even
            // though the exact child has no active session yet. The ledger
            // already owns source/root here; keep it until XAML terminal.
            if ( m_combo && m_combo.IsDropDownOpen() )
            {
                LatchPublishedPopupDebtBeforeSourceClose();
                return false;
            }
        }
        catch ( ... )
        {
            LatchPublishedPopupDebtBeforeSourceClose();
            return false;
        }
        return m_gate->CompleteSealedWithoutPopup();
    }
    void CompleteAtXamlBoundary() noexcept override
    {
        InvalidateOpenResolution();
        m_gate->CompleteAtXamlBoundary();
        m_combo = nullptr;
    }

private:
    enum class OpenResolution
    {
        Resolved,
        TemplatePending,
        PublicationPending,
        Closing,
        Failed
    };

    void StartOpenResolutionGeneration(const MUX::XamlRoot& xamlRoot) noexcept
    {
        m_openPopupHooksArmed = false;
        m_ownerClosedObserved = false;
        m_openPopupIdentity = 0;
        m_openPopupChildIdentity = 0;
        m_openXamlRootIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(wxWinUIChoiceObjectIdentity(xamlRoot)));
    }

    void InvalidateOpenResolution() noexcept
    {
        m_openXamlRootIdentity = 0;
        ClearPreparedOpen();
    }

    void ClearPreparedOpen() noexcept
    {
        m_preparedXamlRootIdentity = 0;
        m_preparedPopupIdentity = 0;
        m_preparedPopupChildIdentity = 0;
        m_preparedForOpen = false;
    }

    static wxWinUIComboPopupResolutionResultForTesting
    DiagnosticResult(OpenResolution result) noexcept
    {
        switch ( result )
        {
            case OpenResolution::Resolved:
                return wxWinUIComboPopupResolutionResultForTesting::Resolved;
            case OpenResolution::TemplatePending:
                return wxWinUIComboPopupResolutionResultForTesting::
                    TemplatePending;
            case OpenResolution::PublicationPending:
                return wxWinUIComboPopupResolutionResultForTesting::
                    PublicationPending;
            case OpenResolution::Closing:
                return wxWinUIComboPopupResolutionResultForTesting::Closing;
            case OpenResolution::Failed:
                return wxWinUIComboPopupResolutionResultForTesting::Failed;
        }

        return wxWinUIComboPopupResolutionResultForTesting::Failed;
    }

    void BeginResolutionDiagnostic(
        wxWinUIComboPopupResolutionOriginForTesting origin) noexcept
    {
        const auto previousReason =
            gs_comboPopupResolutionDiagnosticForTesting.failCloseReason;
        const unsigned previousCount =
            gs_comboPopupResolutionDiagnosticForTesting.failCloseCount;
        gs_comboPopupResolutionDiagnosticForTesting = {};
        gs_comboPopupResolutionDiagnosticForTesting.origin = origin;
        gs_comboPopupResolutionDiagnosticForTesting.failCloseReason =
            previousReason;
        gs_comboPopupResolutionDiagnosticForTesting.failCloseCount =
            previousCount;
        gs_comboPopupResolutionDiagnosticForTesting.hooksArmed =
            m_openPopupHooksArmed;
        gs_comboPopupResolutionDiagnosticForTesting.sessionActive =
            m_gate->IsSessionActive();
        gs_comboPopupResolutionDiagnosticForTesting.completionArmed =
            m_gate->HasCompletionForTesting();
    }

    OpenResolution FinishResolutionDiagnostic(OpenResolution result) noexcept
    {
        gs_comboPopupResolutionDiagnosticForTesting.result =
            DiagnosticResult(result);
        gs_comboPopupResolutionDiagnosticForTesting.hooksArmed =
            m_openPopupHooksArmed;
        return result;
    }

    void RecordUnavailableResolution(
        wxWinUIComboPopupResolutionOriginForTesting origin) noexcept
    {
        BeginResolutionDiagnostic(origin);
        (void)FinishResolutionDiagnostic(OpenResolution::Failed);
    }

    OpenResolution TryResolveOpenedPopup(
        wxWinUIComboPopupResolutionOriginForTesting origin) noexcept
    {
        BeginResolutionDiagnostic(origin);
        if ( m_hostShutdownSealed || m_gate->IsDegraded() ||
             !m_gate->IsSessionActive() || !m_combo )
        {
            return FinishResolutionDiagnostic(OpenResolution::Failed);
        }

        try
        {
            // A close which wins before strict validation is not allowed to
            // masquerade as a successfully observed opening. The caller will
            // fail-close the queue-backed gate without publishing wx state.
            const bool propertyOpen = m_combo.IsDropDownOpen();
            gs_comboPopupResolutionDiagnosticForTesting.propertyOpen =
                propertyOpen;
            if ( !propertyOpen )
                return FinishResolutionDiagnostic(OpenResolution::Closing);

            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
            gs_comboPopupResolutionDiagnosticForTesting.rootAvailable =
                xamlRoot != nullptr;
            const bool rootMatches =
                xamlRoot && m_openXamlRootIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(xamlRoot))) ==
                    m_openXamlRootIdentity;
            gs_comboPopupResolutionDiagnosticForTesting.rootMatches =
                rootMatches;
            if ( !rootMatches )
            {
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            }
            const std::uintptr_t rootIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(xamlRoot)));
            if ( m_preparedForOpen &&
                 rootIdentity != m_preparedXamlRootIdentity )
            {
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            }

            const MUXC::IControlProtected control =
                m_combo.try_as<MUXC::IControlProtected>();
            gs_comboPopupResolutionDiagnosticForTesting.controlAvailable =
                control != nullptr;
            if ( !control )
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            const MUXCP::Popup popup =
                control.GetTemplateChild(L"Popup").try_as<MUXCP::Popup>();
            gs_comboPopupResolutionDiagnosticForTesting.popupAvailable =
                popup != nullptr;
            if ( !popup )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::TemplatePending);
            }
            const MUX::UIElement child = popup.Child();
            gs_comboPopupResolutionDiagnosticForTesting.childAvailable =
                child != nullptr;
            if ( !child )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::TemplatePending);
            }

            const std::uintptr_t popupIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(popup)));
            const std::uintptr_t childIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIChoiceObjectIdentity(child)));
            if ( !popupIdentity || !childIdentity )
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            if ( m_preparedForOpen &&
                 (popupIdentity != m_preparedPopupIdentity ||
                  childIdentity != m_preparedPopupChildIdentity) )
            {
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            }

            // DropDownOpened is raised only after Popup::Open has attached the
            // child and inserted this exact popup in PopupRoot. Bind Closed
            // before checking the remaining facts so a same-stack Dismiss()
            // from the later wx callback cannot escape observation.
            if ( m_openPopupHooksArmed )
            {
                if ( popupIdentity != m_openPopupIdentity ||
                     childIdentity != m_openPopupChildIdentity )
                {
                    return FinishResolutionDiagnostic(OpenResolution::Failed);
                }
            }
            else
            {
                if ( !m_gate->ResolvePopup(popup, child) )
                    return FinishResolutionDiagnostic(OpenResolution::Failed);
                // The exact gate owns both COM references until its causal
                // tail. Keep only non-owning identities here so a closed
                // popup graph is not retained until the next generation.
                m_openPopupIdentity = popupIdentity;
                m_openPopupChildIdentity = childIdentity;
                m_openPopupHooksArmed = true;
                gs_comboPopupResolutionDiagnosticForTesting.hooksArmed = true;
            }

            // A synchronous Dismiss() can win after the exact Closed hook was
            // armed but before open-list publication was observed. Leave the
            // unsatisfied gate to consume Popup.Closed/owner close and its one
            // causal tail; this is a normal close, not a resolution failure.
            const bool stillOpen = m_combo.IsDropDownOpen();
            gs_comboPopupResolutionDiagnosticForTesting.propertyOpen =
                stillOpen;
            if ( !stillOpen )
                return FinishResolutionDiagnostic(OpenResolution::Closing);
            const bool popupIsOpen = popup.IsOpen();
            gs_comboPopupResolutionDiagnosticForTesting.popupIsOpen =
                popupIsOpen;
            if ( !popupIsOpen )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::PublicationPending);
            }

            bool exactOpenPopup = false;
            for ( const MUXCP::Popup& openPopup :
                  MUXM::VisualTreeHelper::
                      GetOpenPopupsForXamlRoot(xamlRoot) )
            {
                if ( reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                         wxWinUIChoiceObjectIdentity(openPopup))) ==
                     popupIdentity )
                {
                    if ( exactOpenPopup )
                    {
                        gs_comboPopupResolutionDiagnosticForTesting.
                            duplicateOpenList = true;
                        return FinishResolutionDiagnostic(
                            OpenResolution::Failed);
                    }
                    exactOpenPopup = true;
                }
            }
            gs_comboPopupResolutionDiagnosticForTesting.exactOpenList =
                exactOpenPopup;
            if ( !exactOpenPopup )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::PublicationPending);
            }
            return FinishResolutionDiagnostic(OpenResolution::Resolved);
        }
        catch ( ... )
        {
            return FinishResolutionDiagnostic(OpenResolution::Failed);
        }
    }

    bool ResolveOpenedPopupStrict(
        wxWinUIComboPopupResolutionOriginForTesting origin) noexcept
    {
        const OpenResolution result = TryResolveOpenedPopup(origin);
        if ( result == OpenResolution::Resolved )
            return true;
        // The first DropDownOpened subscriber runs after Popup::Open has
        // attached the exact child and inserted the popup in PopupRoot, but
        // before any wx callback. Missing/ambiguous facts here are therefore
        // a protocol fault, not a reason to install Closed asynchronously.
        FailClose(
            wxWinUIComboPopupFailCloseReasonForTesting::
                StrictResolutionRejected);
        return false;
    }

    void FailClose(
        wxWinUIComboPopupFailCloseReasonForTesting reason) noexcept
    {
        gs_comboPopupResolutionDiagnosticForTesting.failCloseReason = reason;
        ++gs_comboPopupResolutionDiagnosticForTesting.failCloseCount;
        InvalidateOpenResolution();
        try
        {
            if ( m_combo )
            {
                m_gate->PreserveDispatcherQueue(
                    m_combo.DispatcherQueue());
            }
        }
        catch ( ... )
        {
        }
        m_gate->MarkDegraded();
        try
        {
            if ( m_combo && m_combo.IsDropDownOpen() )
                m_combo.IsDropDownOpen(false);
        }
        catch ( ... )
        {
            // The degraded gate and its real dispatcher queue now retain the
            // exact graph until the TLW/XAML terminal boundary.
        }
    }

    void RefusePoisonedPublication() noexcept
    {
        // This control was already published when another gate poisoned the
        // process. Give its now-visible popup a real queue-backed degraded
        // owner before asking WinUI to close it; otherwise the still-
        // satisfied reusable child would permit synchronous slot detach.
        FailClose(
            wxWinUIComboPopupFailCloseReasonForTesting::
                PublicationPoisoned);
        try
        {
            // Continuing after a rejected synchronous fail-close would add
            // an unobserved popup to an already poisoned runtime. There is no
            // bounded recovery owner for that second fault.
            if ( m_combo && m_combo.IsDropDownOpen() )
                std::terminate();
        }
        catch ( ... )
        {
            std::terminate();
        }
    }

    MUXC::ComboBox m_combo{ nullptr };
    std::shared_ptr<wxWinUIExactPopupRetirementGate> m_gate;
    std::uintptr_t m_openPopupIdentity = 0;
    std::uintptr_t m_openPopupChildIdentity = 0;
    std::uintptr_t m_openXamlRootIdentity = 0;
    std::uintptr_t m_preparedPopupIdentity = 0;
    std::uintptr_t m_preparedPopupChildIdentity = 0;
    std::uintptr_t m_preparedXamlRootIdentity = 0;
    bool m_openPopupHooksArmed = false;
    bool m_ownerClosedObserved = false;
    bool m_preparedForOpen = false;
    bool m_hostShutdownSealed = false;
    bool m_publishedDebtLatched = false;
};

bool wxWinUIIsComboPopupRetirementSatisfied(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return !state || state->IsSatisfied();
}

bool wxWinUIIsComboPopupRetirementSealed(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state && state->IsSealedForHostShutdown();
}

bool wxWinUIArmComboPopupRetirementCompletion(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state,
    std::function<void ()> completion) noexcept
{
    return state && !state->IsDegraded() &&
           state->SetCompletion(std::move(completion));
}

bool wxWinUIObserveComboPopupOpened(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state && state->ObserveOpened();
}

bool wxWinUIResolveComboPopupAfterOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state && state->ResolveAfterOpen();
}

bool wxWinUIPrepareComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state && state->PrepareForOpen();
}

void wxWinUICancelPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    if ( state )
        state->CancelPreparedOpen();
}

bool wxWinUIFinishPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state && state->FinishPreparedOpen();
}

void wxWinUIAbortPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    if ( state )
        state->AbortPreparedOpen();
}

class wxWinUIFlyoutPopupRetirementState final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<
        wxWinUIFlyoutPopupRetirementState>
{
public:
    wxWinUIFlyoutPopupRetirementState(
        const MUXC::MenuFlyout& flyout,
        const MUX::XamlRoot& xamlRoot,
        const MUX::DependencyObject& exactItem)
        : m_flyout(flyout),
          m_xamlRoot(xamlRoot),
          m_exactItem(exactItem),
          m_gate(std::make_shared<wxWinUIExactPopupRetirementGate>())
    {
    }

    bool Arm() noexcept
    {
        try
        {
            if ( !m_flyout || !m_xamlRoot || !m_exactItem ||
                 !m_gate->BeginUnresolvedSession(
                     m_xamlRoot, m_exactItem.DispatcherQueue()) )
            {
                m_gate->MarkDegraded();
                return false;
            }
            const std::weak_ptr<wxWinUIFlyoutPopupRetirementState> weak =
                shared_from_this();
            m_closedToken = m_flyout.Closed(
                [weak](const WF::IInspectable&,
                       const WF::IInspectable&)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveClosed();
                });
            if ( !m_closedToken.value )
            {
                m_gate->MarkDegraded();
                return false;
            }
            return true;
        }
        catch ( ... )
        {
            m_gate->MarkDegraded();
            return false;
        }
    }

    void ResolveAfterShow() noexcept
    {
        try
        {
            MUXCP::Popup exactPopup{ nullptr };
            MUX::UIElement exactChild{ nullptr };
            for ( const MUXCP::Popup& popup :
                  MUXM::VisualTreeHelper::
                      GetOpenPopupsForXamlRoot(m_xamlRoot) )
            {
                const MUX::UIElement child = popup.Child();
                if ( !popup.IsOpen() || !child )
                    continue;
                const wxWinUIElementRelationship relationship =
                    wxWinUIIsElementWithin(m_exactItem, child);
                if ( relationship ==
                         wxWinUIElementRelationship::Unavailable ||
                     (relationship == wxWinUIElementRelationship::Inside &&
                      exactPopup) )
                {
                    FailClose();
                    return;
                }
                if ( relationship == wxWinUIElementRelationship::Inside )
                {
                    exactPopup = popup;
                    exactChild = child;
                }
            }
            if ( !exactPopup || !m_gate->ResolvePopup(exactPopup, exactChild) )
                FailClose();
        }
        catch ( ... )
        {
            FailClose();
        }
    }

    void FailShow() noexcept { FailClose(); }

    bool SetObserver(std::function<void ()> observer) noexcept
    {
        try
        {
            m_observer = std::move(observer);
            return true;
        }
        catch ( ... )
        {
            m_gate->MarkDegraded();
            return false;
        }
    }

    bool IsSatisfied() const noexcept override
        { return m_gate->IsSatisfied(); }
    bool IsDegraded() const noexcept override
        { return m_gate->IsDegraded(); }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_gate->IsSealedForHostShutdown(); }
    MUXD::DispatcherQueue GetDispatcherQueue() const noexcept override
        { return m_gate->GetDispatcherQueue(); }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        const std::weak_ptr<wxWinUIFlyoutPopupRetirementState> weak =
            shared_from_this();
        try
        {
            return m_gate->SetCompletion(
                [weak, complete = std::move(complete)]() mutable
                {
                    const auto state = weak.lock();
                    if ( !state )
                        return;
                    if ( !state->CleanupFlyout(false) )
                    {
                        state->m_gate->MarkDegraded();
                        return;
                    }
                    complete();
                    std::function<void ()> observer =
                        std::move(state->m_observer);
                    if ( observer )
                    {
                        try
                        {
                            observer();
                        }
                        catch ( ... )
                        {
                            // The exact child is already causally retired,
                            // but an observer failure must still prevent any
                            // later popup publication in this process.
                            wxWinUINotePhysicalDisconnectGateDegraded();
                        }
                    }
                });
        }
        catch ( ... )
        {
            m_gate->MarkDegraded();
            return false;
        }
    }

    void SealForHostShutdown() noexcept override
    {
        m_gate->SealForHostShutdown();
    }
    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        m_gate->LatchPublishedPopupDebtBeforeSourceClose();
    }
    bool CompleteSealedWithoutPopup() noexcept override
        { return m_gate->CompleteSealedWithoutPopup(); }

    void CompleteAtXamlBoundary() noexcept override
    {
        m_gate->CompleteAtXamlBoundary();
        m_observer = {};
        CleanupFlyout(true);
    }

private:
    void FailClose() noexcept
    {
        try
        {
            if ( m_exactItem )
            {
                m_gate->PreserveDispatcherQueue(
                    m_exactItem.DispatcherQueue());
            }
        }
        catch ( ... )
        {
        }
        m_gate->MarkDegraded();
        try
        {
            if ( m_flyout )
                m_flyout.Hide();
        }
        catch ( ... )
        {
            // Terminal retirement owns the still-live exact flyout graph.
        }
    }

    void ObserveClosed() noexcept
    {
        m_gate->ObserveOwnerClosed();
    }

    bool CleanupFlyout(bool terminal) noexcept
    {
        bool ok = true;
        if ( m_flyout && m_closedToken.value )
        {
            try
            {
                m_flyout.Closed(m_closedToken);
                m_closedToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }
        if ( ok || terminal )
        {
            m_closedToken = {};
            m_flyout = nullptr;
            m_xamlRoot = nullptr;
            m_exactItem = nullptr;
        }
        return ok;
    }

    MUXC::MenuFlyout m_flyout{ nullptr };
    MUX::XamlRoot m_xamlRoot{ nullptr };
    MUX::DependencyObject m_exactItem{ nullptr };
    winrt::event_token m_closedToken{};
    std::shared_ptr<wxWinUIExactPopupRetirementGate> m_gate;
    std::function<void ()> m_observer;
};

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreateCompositePhysicalDisconnectGate()
{
    return std::make_shared<wxWinUICompositePhysicalDisconnectGate>();
}

bool wxWinUIAddCompositePhysicalDisconnectGate(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& composite,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& child,
    bool reusable) noexcept
{
    const auto concrete =
        std::dynamic_pointer_cast<wxWinUICompositePhysicalDisconnectGate>(
            composite);
    return concrete && concrete->Add(child, reusable);
}

std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
wxWinUICreateFlyoutPopupRetirementState(
    const MUXC::MenuFlyout& flyout,
    const MUX::XamlRoot& xamlRoot,
    const MUX::DependencyObject& exactItem) noexcept
{
    try
    {
        auto state = std::make_shared<wxWinUIFlyoutPopupRetirementState>(
            flyout, xamlRoot, exactItem);
        if ( !state->Arm() )
            return state;
        return state;
    }
    catch ( ... )
    {
        wxWinUINotePhysicalDisconnectGateDegraded();
        return {};
    }
}

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetFlyoutPopupRetirementGate(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept
{
    return state;
}

void wxWinUIResolveFlyoutPopupAfterShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept
{
    if ( state )
        state->ResolveAfterShow();
}

void wxWinUIFailFlyoutPopupShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept
{
    if ( state )
        state->FailShow();
}

bool wxWinUISetFlyoutPopupRetirementObserver(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state,
    std::function<void ()> observer) noexcept
{
    return state && state->SetObserver(std::move(observer));
}

wxWinUIChoiceImpl::~wxWinUIChoiceImpl()
{
    Close();
}

void wxWinUIChoiceImpl::Close()
{
    // Invalidate before the first revocation attempt. Revocation can throw,
    // and detaching a focused editable ComboBox can synchronously (or on the
    // dispatcher) commit pending text. Every delegate owns only this shared
    // state, so either path observes a null owner instead of a dying control.
    if ( callbackState )
        callbackState->Invalidate();

    // Keep the exact popup/focus observation live while requesting closure.
    // The ComboBox can raise DropDownClosed inline; that delegate records the
    // logical close even though its wx owner was invalidated above.
    if ( popupRetirementState )
        popupRetirementState->RequestClose();

    if ( comboBox )
    {
        if ( textSubmittedToken.value )
        {
            try
            {
                comboBox.TextSubmitted(textSubmittedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI ComboBox TextSubmitted removal", e);
            }
        }
        textSubmittedToken = {};

        if ( textChangedCallbackToken )
        {
            try
            {
                comboBox.UnregisterPropertyChangedCallback(
                    winrt::Microsoft::UI::Xaml::Controls::ComboBox::
                        TextProperty(),
                    textChangedCallbackToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox text callback removal", e);
            }
        }
        textChangedCallbackToken = 0;

        if ( selectionChangedToken.value )
        {
            try
            {
                comboBox.SelectionChanged(selectionChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI ComboBox SelectionChanged removal", e);
            }
        }
        selectionChangedToken = {};

        if ( choiceKeyDownToken.value )
        {
            try
            {
                comboBox.KeyDown(choiceKeyDownToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice KeyDown removal", e);
            }
        }
        choiceKeyDownToken = {};

        if ( choiceDropDownOpenedToken.value )
        {
            try
            {
                comboBox.DropDownOpened(choiceDropDownOpenedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice DropDownOpened removal", e);
            }
        }
        choiceDropDownOpenedToken = {};

        if ( choiceDropDownClosedToken.value )
        {
            try
            {
                comboBox.DropDownClosed(choiceDropDownClosedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Choice DropDownClosed removal", e);
            }
        }
        choiceDropDownClosedToken = {};
    }

    if ( simpleListBox && simpleSelectionChangedToken.value )
    {
        try
        {
            simpleListBox.SelectionChanged(simpleSelectionChangedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI simple ComboBox SelectionChanged removal", e);
        }
    }
    simpleSelectionChangedToken = {};

    // Unregistering the slot invalidates its shared callback state before the
    // XAML element is detached. Only then release our final peer references.
    // WinUI can dispatch its popup-child LostFocus after this Close() returns.
    // Only an actually open/closing popup carries a gate: a never-opened or
    // already causally retired ComboBox disconnects synchronously.
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate> retirementGate =
        physicalDisconnectGate && !physicalDisconnectGate->IsSatisfied()
            ? physicalDisconnectGate
            : nullptr;
    host.Close(retirementGate);
    editBox = nullptr;
    simpleListBox = nullptr;
    simpleRoot = nullptr;
    editableRoot = nullptr;
    comboBox = nullptr;
    popupRetirementState.reset();
    physicalDisconnectGate.reset();
    pendingItemId = 0;
    hasPendingSelection = false;
    choiceDropDownOpen = false;
    callbackState.reset();
}

wxChoice::wxChoice()
{
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   int n,
                   const wxString choices[],
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, n, choices, style, validator, name);
}

wxChoice::wxChoice(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxChoice()
{
    Create(parent, id, pos, size, choices, style, validator, name);
}

wxChoice::~wxChoice()
{
    // wxComboBox's derived members (notably m_value) have already been
    // destroyed when this base destructor starts. A delayed TextSubmitted
    // callback must therefore be disconnected before Clear() mutates the
    // peer, and the impl must be destroyed before m_items is torn down.
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }

    Clear();
}

bool wxChoice::MSWShouldPreProcessMessage(WXMSG *msg)
{
    if ( msg && msg->message == WM_KEYDOWN &&
            !wxWinUIChoiceHasModifiers(msg) )
    {
        switch ( msg->wParam )
        {
            case VK_F4:
                // F4 belongs to the XAML ComboBox even while it is closed.
                // wxCB_SIMPLE has no popup and leaves F4 to ordinary dialog
                // preprocessing, matching the native CBS_SIMPLE control.
                if ( !HasFlag(wxCB_SIMPLE) || HasFlag(wxCB_READONLY) )
                    return false;
                break;

            case VK_ESCAPE:
            case VK_RETURN:
                // Escape/Enter belong to an open dropdown, not to a parent
                // accelerator or dialog default button.
                try
                {
                    if ( m_winui && m_winui->comboBox &&
                            m_winui->comboBox.IsDropDownOpen() )
                    {
                        return false;
                    }
                }
                catch ( const winrt::hresult_error& )
                {
                    // During teardown, fall through to normal preprocessing:
                    // there is no usable peer left to own this key.
                }
                break;
        }
    }

    return wxControl::MSWShouldPreProcessMessage(msg);
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n,
                      const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxASSERT_MSG( !(style & wxCB_DROPDOWN) &&
                  !(style & wxCB_READONLY) &&
                  !(style & wxCB_SIMPLE),
                  wxT("this style flag is ignored by wxChoice") );

    // The WinUI ComboBox draws its own border, so suppress the native control
    // border to avoid an extra grey frame around the island.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUIChoiceImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIChoiceCallbackState>(this);
    wxWinUIChoiceImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->comboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox();
        m_winui->popupRetirementState =
            std::make_shared<wxWinUIComboPopupRetirementState>(
                m_winui->comboBox);
        m_winui->physicalDisconnectGate =
            wxWinUICreateCompositePhysicalDisconnectGate();
        if ( !m_winui->popupRetirementState ||
             !m_winui->physicalDisconnectGate ||
             !wxWinUIAddCompositePhysicalDisconnectGate(
                 m_winui->physicalDisconnectGate,
                 m_winui->popupRetirementState,
                 true) )
        {
            return false;
        }
        m_winui->host.SetPhysicalDisconnectGate(
            m_winui->physicalDisconnectGate);
        // An editable derived ComboBox must install its text/template
        // delegates before the one and only host publication. Publishing it
        // here would consume the initial Loaded/template transition in the
        // wxChoice half of construction; a later same-content SetContent()
        // deliberately doesn't touch the carrier and cannot replay it.
        const bool deferEditablePeerPublication =
            WinUIWantsEditablePeerDuringCreate();
        m_winui->comboBox.IsEditable(deferEditablePeerPublication);
        m_winui->comboBox.MaxDropDownHeight(240);
        // wxChoice::GetCurrentSelection() must expose the highlighted item
        // while the popup is open, before GetSelection() is committed.  The
        // default Committed trigger doesn't publish keyboard navigation.
        m_winui->comboBox.SelectionChangedTrigger(
            winrt::Microsoft::UI::Xaml::Controls::
                ComboBoxSelectionChangedTrigger::Always);

        m_winui->selectionChangedToken = m_winui->comboBox.SelectionChanged(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Microsoft::UI::Xaml::Controls::
                    SelectionChangedEventArgs const&)
            {
                wxChoice * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     callbackState->IsPeerMutationInProgress() )
                {
                    return;
                }

                owner->WinUIOnPeerSelectionChanged();
            });

        m_winui->choiceKeyDownToken = m_winui->comboBox.KeyDown(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const&
                    args)
            {
                if ( args.Handled() ||
                     args.Key() !=
                         winrt::Windows::System::VirtualKey::Escape )
                {
                    return;
                }

                wxChoice * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     !owner->m_winui->choiceDropDownOpen )
                {
                    return;
                }

                // Leave Escape unhandled so the real ComboBox closes its
                // popup, but discard its transient highlight first.  The
                // ensuing DropDownClosed callback then has nothing to commit.
                owner->WinUICancelPendingPeerSelection();
            });

        const std::shared_ptr<wxWinUIComboPopupRetirementState>
            popupRetirementState = m_winui->popupRetirementState;
        m_winui->choiceDropDownOpenedToken =
            m_winui->comboBox.DropDownOpened(
                [callbackState, popupRetirementState](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    // WinUI raises this event before moving focus into the
                    // popup. Arm exact-popup and FocusManager observation
                    // before any wx callback can run or destroy the owner.
                    if ( !popupRetirementState->ObserveOpened() )
                        return;
                    wxChoice * const owner = callbackState->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->WinUIOnDropDownChanged(true);
                });

        m_winui->choiceDropDownClosedToken =
            m_winui->comboBox.DropDownClosed(
                [callbackState, popupRetirementState](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    popupRetirementState->ObserveClosed();
                    wxChoice * const owner = callbackState->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->WinUIOnDropDownChanged(false);
                });

        const bool contentSet = deferEditablePeerPublication ||
            createImpl->host.SetContent(createImpl->comboBox);
        wxChoice * const owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox creation", e);
        wxChoice * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxChoice *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    if ( n && choices )
    {
        owner->Append(n, choices);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner();
    return owner && owner->m_winui &&
           owner->m_winui.get() == createImpl;
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

void wxChoice::SetLabel(const wxString& label)
{
    const int sel = FindString(label);
    const wxWeakRef<wxWindow> self(this);
    SetSelection(sel);
    if ( self )
        wxControl::SetLabel(label);
}

unsigned int wxChoice::GetCount() const
{
    return static_cast<unsigned int>(m_itemModel.GetCount());
}

int wxChoice::GetSelection() const
{
    if ( !m_selectedItemId )
        return wxNOT_FOUND;

    const std::size_t index = m_itemModel.IndexOf(m_selectedItemId);
    return index == wxWinUIItemModel::npos
               ? wxNOT_FOUND
               : static_cast<int>(index);
}

int wxChoice::GetCurrentSelection() const
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return GetSelection();

    // The selection delegate translates the native index to a stable item ID
    // synchronously. Keep that transaction authoritative instead of reading
    // ComboBox.SelectedIndex again: an editable ComboBox can transiently
    // clear SelectedIndex while projecting its TextBox document even though
    // both the accepted model and the popup highlight remain valid.
    if ( !m_winui->choiceDropDownOpen ||
         !m_winui->hasPendingSelection )
    {
        return GetSelection();
    }

    if ( !m_winui->pendingItemId )
        return wxNOT_FOUND;

    const std::size_t index =
        m_itemModel.IndexOf(m_winui->pendingItemId);
    return index == wxWinUIItemModel::npos
               ? GetSelection()
               : static_cast<int>(index);
}

void wxChoice::WinUIOnPeerSelectionChanged()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
         !m_winui->callbackState ||
         m_winui->callbackState->IsPeerMutationInProgress() )
    {
        return;
    }

    int selection = wxNOT_FOUND;
    try
    {
        selection = wxWinUIChoiceGetPeerSelection(m_winui.get());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI item peer selection read", e);
        return;
    }
    wxWinUIItemModel::Id selectedId = 0;
    if ( selection >= 0 )
    {
        const std::size_t peerIndex =
            static_cast<std::size_t>(selection);
        if ( !m_winui->peerItemsValid ||
             peerIndex >= m_winui->peerItemIds.size() )
        {
            return;
        }

        selectedId = m_winui->peerItemIds[peerIndex];
        if ( m_itemModel.IndexOf(selectedId) ==
                wxWinUIItemModel::npos )
        {
            return;
        }
    }

    // A simple list has no transient popup state: keyboard and pointer
    // selection are accepted immediately. Normal Choice/ComboBox peers retain
    // the highlighted-vs-accepted split while their dropdown is open.
    if ( !m_winui->simpleListBox && m_winui->choiceDropDownOpen )
    {
        m_winui->pendingItemId = selectedId;
        m_winui->hasPendingSelection = true;
        return;
    }

    WinUICommitPeerSelection(selectedId);
}

void wxChoice::WinUICommitPeerSelection(
    wxWinUIItemModel::Id selectedId)
{
    if ( selectedId &&
         m_itemModel.IndexOf(selectedId) == wxWinUIItemModel::npos )
    {
        return;
    }

    const bool changed = m_selectedItemId != selectedId;
    m_selectedItemId = selectedId;
    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        m_itemModel.At(n).selected =
            selectedId && m_itemModel.At(n).id == selectedId;
    }
    if ( changed )
        SendSelectionEvent();
}

void wxChoice::WinUIOnDropDownChanged(bool open)
{
    if ( !m_winui || m_winui->choiceDropDownOpen == open )
        return;

    m_winui->choiceDropDownOpen = open;
    if ( open )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
        return;
    }

    const bool commit = m_winui->hasPendingSelection;
    const wxWinUIItemModel::Id pending = m_winui->pendingItemId;
    m_winui->pendingItemId = 0;
    m_winui->hasPendingSelection = false;
    if ( commit )
        WinUICommitPeerSelection(pending);
}

bool wxChoice::WinUICoalescePopupReopen(
    std::function<void (wxChoice *)> replay) noexcept
{
    if ( !m_winui || !m_winui->popupRetirementState )
        return false;

    // The first degraded gate closes publication process-wide. Existing
    // controls must reject public Popup() before setting the peer property;
    // DropDownOpened remains the fail-close backstop for a native/user open.
    if ( wxWinUIIsPhysicalDisconnectPublicationPoisoned() )
        return true;

    // Shutdown sealing is a permanent publication barrier. Refuse a public
    // reopen before touching IsDropDownOpen, without poisoning a peer which
    // never actually published a second popup graph.
    if ( wxWinUIIsComboPopupRetirementSealed(
             m_winui->popupRetirementState) )
    {
        return true;
    }

    // Repeating Popup() while this generation is still logically open is an
    // idempotent public operation, not a request to replay after retirement.
    // Dismiss() clears this bit before delivering CLOSEUP, so a Popup() from
    // that synchronous handler still enters the coalesced-reopen path below.
    if ( m_winui->choiceDropDownOpen )
        return true;

    if ( wxWinUIIsComboPopupRetirementSatisfied(
             m_winui->popupRetirementState) )
    {
        return false;
    }

    if ( m_winui->popupReopenPending )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const auto state = impl->popupRetirementState;
    const std::weak_ptr<wxWinUIComboPopupRetirementState> weakState = state;
    const std::shared_ptr<wxWinUIChoiceCallbackState> callbackState =
        impl->callbackState;
    if ( ++impl->popupReopenGeneration == 0 )
        ++impl->popupReopenGeneration;
    const std::uint64_t generation = impl->popupReopenGeneration;
    impl->popupReopenPending = true;
    ++impl->popupReopenSchedulesForTesting;

    try
    {
        if ( !wxWinUIArmComboPopupRetirementCompletion(
                 state,
                 [weakState, callbackState, generation,
                  replay = std::move(replay)]() mutable
                 {
                     const auto exactState = weakState.lock();
                     wxChoice * const owner = callbackState->GetOwner();
                     if ( !exactState || !owner || !owner->m_winui ||
                          owner->m_winui->callbackState != callbackState ||
                          owner->m_winui->popupRetirementState !=
                              exactState ||
                          !owner->m_winui->popupReopenPending ||
                          owner->m_winui->popupReopenGeneration !=
                              generation )
                     {
                         return;
                     }

                     owner->m_winui->popupReopenPending = false;
                     ++owner->m_winui->popupReopenRunsForTesting;
                     replay(owner);
                 }) )
        {
            impl->popupReopenPending = false;
        }
    }
    catch ( ... )
    {
        impl->popupReopenPending = false;
        wxWinUINotePhysicalDisconnectGateDegraded();
    }
    return true;
}

void wxChoice::WinUICancelPendingPeerSelection()
{
    if ( !m_winui || !m_winui->comboBox ||
         !m_winui->choiceDropDownOpen )
    {
        return;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;
    const int accepted = GetSelection();
    impl->pendingItemId = 0;
    impl->hasPendingSelection = false;

    wxWinUIPeerMutationGuard guard(state);
    try
    {
        impl->comboBox.SelectedIndex(accepted);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Choice cancelled selection restore", e);
    }
}

void wxChoice::SetSelection(int n)
{
    if ( n < 0 || static_cast<std::size_t>(n) >= m_itemModel.GetCount() )
        n = wxNOT_FOUND;

    m_selectedItemId =
        n == wxNOT_FOUND ? 0 : m_itemModel.At(n).id;
    for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
        m_itemModel.At(i).selected = static_cast<int>(i) == n;

    // A direct selection is authoritative even while the dropdown is open.
    if ( m_winui )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

int wxChoice::FindString(const wxString& s, bool bCase) const
{
    for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
    {
        if ( m_itemModel.At(i).text.IsSameAs(s, bCase) )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), wxString(),
                 wxT("invalid choice index") );
    return m_itemModel.At(n).text;
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );

    if ( !WinUIEnsurePeerConsistent() )
        return;
    const auto change = m_itemModel.Rename(n, s, IsSorted());
    if ( change.kind == wxWinUIItemModel::Change::Kind::Move )
    {
        WinUIOnItemMoved(
            static_cast<unsigned int>(change.oldIndex),
            static_cast<unsigned int>(change.newIndex));
        WinUIMovePeerItem(
            static_cast<unsigned int>(change.oldIndex),
            static_cast<unsigned int>(change.newIndex));
        WinUIUpdatePeerItem(static_cast<unsigned int>(change.newIndex));
    }
    else
    {
        WinUIUpdatePeerItem(n);
    }

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

void wxChoice::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );

    if ( !WinUIEnsurePeerConsistent() )
        return;
    const wxWinUIItemModel::Id erasedId = m_itemModel.At(n).id;
    if ( m_winui && m_winui->hasPendingSelection &&
         m_winui->pendingItemId == erasedId )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }
    WinUIOnItemErased(n);
    m_itemModel.Erase(n);
    if ( m_selectedItemId == erasedId )
        m_selectedItemId = 0;
    WinUIErasePeerItem(n);

    if ( !ApplySelectionToPeer() )
        return;
    InvalidateBestSize();
}

void wxChoice::DoClear()
{
    WinUIOnItemsCleared();
    m_itemModel.Clear();
    m_selectedItemId = 0;
    if ( m_winui )
    {
        m_winui->pendingItemId = 0;
        m_winui->hasPendingSelection = false;
    }

    if ( wxWinUIChoiceHasItemPeer(m_winui.get()) )
    {
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        m_winui->peerItemsValid = false;
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            while ( items.Size() )
                items.RemoveAtEnd();
            m_winui->peerItemIds.clear();
            m_winui->peerItemsValid = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item clear", e);
            m_winui->peerItemIds.clear();
            m_winui->peerItemsValid = false;
        }
    }

    InvalidateBestSize();
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter& items,
                            unsigned int pos,
                            void **clientData,
                            wxClientDataType type)
{
    wxCHECK_MSG( pos <= m_itemModel.GetCount(), wxNOT_FOUND,
                 wxT("invalid choice index") );

    int last = wxNOT_FOUND;
    for ( unsigned int i = 0; i < items.GetCount(); ++i )
    {
        if ( !WinUIEnsurePeerConsistent() )
            return wxNOT_FOUND;
        const auto change = m_itemModel.Insert(
            items[i],
            IsSorted() ? m_itemModel.GetCount() : pos + i,
            IsSorted());
        const unsigned int insertPos =
            static_cast<unsigned int>(change.newIndex);

        AssignNewItemClientData(insertPos, clientData, i, type);
        WinUIOnItemInserted(insertPos);
        WinUIInsertPeerItem(insertPos);
        last = static_cast<int>(change.newIndex);
    }

    if ( !ApplySelectionToPeer() )
        return wxNOT_FOUND;
    InvalidateBestSize();
    return last;
}

void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid choice index") );
    m_itemModel.At(n).clientData = clientData;
}

void* wxChoice::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), nullptr,
                 wxT("invalid choice index") );
    return m_itemModel.At(n).clientData;
}

wxSize wxChoice::DoGetBestSize() const
{
    wxCoord width = 0;
    wxCoord height = GetCharHeight();
    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        wxCoord itemWidth = 0;
        wxCoord itemHeight = 0;
        GetTextExtent(m_itemModel.At(n).text, &itemWidth, &itemHeight);
        width = wxMax(width, itemWidth);
        height = wxMax(height, itemHeight);
    }

    const wxSize chrome = FromDIP(wxSize(48, 14));
    const wxSize minimum = FromDIP(wxSize(80, 32));
    return wxSize(wxMax(width + chrome.x, minimum.x),
                  wxMax(height + chrome.y, minimum.y));
}

wxBitmap wxChoice::WinUIGetItemBitmap(
    unsigned int WXUNUSED(n),
    double WXUNUSED(requestedScale)) const
{
    return wxBitmap();
}

void wxChoice::WinUIRefreshItems(double requestedBitmapScale)
{
    if ( !WinUIEnsurePeerConsistent() )
        return;
    for ( unsigned int n = 0; n < GetCount(); ++n )
        WinUIUpdatePeerItem(n, requestedBitmapScale);
}

void wxChoice::WinUIOnItemInserted(unsigned int WXUNUSED(n))
{
}

void wxChoice::WinUIOnItemErased(unsigned int WXUNUSED(n))
{
}

void wxChoice::WinUIOnItemMoved(unsigned int WXUNUSED(oldIndex),
                                unsigned int WXUNUSED(newIndex))
{
}

void wxChoice::WinUIOnItemsCleared()
{
}

bool wxChoice::WinUIEnsurePeerConsistent()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    bool valid = m_winui->peerItemsValid;
    try
    {
        const std::size_t count = m_itemModel.GetCount();
        valid = valid &&
                wxWinUIChoiceGetPeerItems(m_winui.get()).Size() == count &&
                m_winui->peerItemIds.size() == count;
        for ( std::size_t n = 0; valid && n < count; ++n )
        {
            valid = m_winui->peerItemIds[n] ==
                    m_itemModel.At(n).id;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        valid = false;
    }

    if ( !valid )
        return ApplyItemsToPeer();

    return true;
}

bool wxChoice::ApplyItemsToPeer()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    wxWinUIPeerMutationGuard guard(m_winui->callbackState);
    m_winui->peerItemsValid = false;
    try
    {
        auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        while ( items.Size() )
            items.RemoveAtEnd();
        m_winui->peerItemIds.clear();

        for ( unsigned int n = 0; n < GetCount(); ++n )
        {
            items.Append(wxWinUICreateChoicePeerItem(m_winui.get()));
            m_winui->peerItemIds.push_back(m_itemModel.At(n).id);
        }

        m_winui->peerItemsValid =
            items.Size() == m_itemModel.GetCount() &&
            m_winui->peerItemIds.size() ==
                m_itemModel.GetCount();
        if ( m_winui->peerItemsValid )
        {
            for ( unsigned int n = 0; n < GetCount(); ++n )
            {
                WinUIUpdatePeerItem(n);
                if ( !m_winui->peerItemsValid )
                    break;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox item update", e);
        m_winui->peerItemIds.clear();
        m_winui->peerItemsValid = false;
    }

    return ApplySelectionToPeer();
}

bool wxChoice::ApplySelectionToPeer()
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return true;

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;

    if ( !impl->peerItemsValid )
        return true;

    int peerSelection = GetSelection();
    if ( impl->choiceDropDownOpen && impl->hasPendingSelection )
    {
        if ( impl->pendingItemId )
        {
            const std::size_t pendingIndex =
                m_itemModel.IndexOf(impl->pendingItemId);
            peerSelection = pendingIndex == wxWinUIItemModel::npos
                ? wxNOT_FOUND
                : static_cast<int>(pendingIndex);
        }
        else
        {
            peerSelection = wxNOT_FOUND;
        }
    }

    wxWinUIPeerMutationGuard guard(state);
    try
    {
        // While wxComboBox is replacing the construction-time ComboBox with
        // its wxCB_SIMPLE composite, the new item peer is deliberately built
        // off-tree. Rendering the old hosted peer here could re-enter wx and
        // destroy the control before the transaction publishes simpleRoot.
        wxWinUIChoiceSetPeerSelection(impl, peerSelection);

        // SelectedIndex is a re-entrant XAML boundary. It can start an
        // editable-template transition (or destroy this control), so no
        // precomputed render decision and no unvalidated impl access may cross
        // it. Re-evaluate the exact live generation before each further host
        // boundary and never force layout while a natural template replay owns
        // the subtree.
        wxChoice *live = state ? state->GetOwner() : nullptr;
        if ( live != this || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != state )
        {
            return false;
        }

        bool shouldRender = !impl->comboTemplateTransition &&
                            !impl->comboTemplateReplayPending;
        if ( shouldRender && impl->simpleRoot )
        {
            const auto simpleRoot = impl->simpleRoot;
            const auto hostedContent = impl->host.GetContentForTesting();
            live = state ? state->GetOwner() : nullptr;
            if ( live != this || !live->m_winui ||
                 live->m_winui.get() != impl ||
                 live->m_winui->callbackState != state ||
                 live->m_winui->simpleRoot != simpleRoot )
            {
                return false;
            }
            shouldRender = !impl->comboTemplateTransition &&
                           !impl->comboTemplateReplayPending &&
                           wxWinUIChoiceObjectIdentity(hostedContent) ==
                               wxWinUIChoiceObjectIdentity(simpleRoot);
        }
        if ( shouldRender )
            impl->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox selection update", e);
    }

    wxChoice * const live = state ? state->GetOwner() : nullptr;
    return live == this && live->m_winui &&
           live->m_winui.get() == impl &&
           live->m_winui->callbackState == state;
}

void wxChoice::SendSelectionEvent()
{
    const int selection = GetSelection();
    wxCommandEvent event(wxEVT_CHOICE, GetId());
    InitCommandEventWithItems(event, selection);
    event.SetInt(selection);
    if ( selection != wxNOT_FOUND )
        event.SetString(m_itemModel.At(selection).text);
    ProcessCommand(event);
}

void wxChoice::WinUIInsertPeerItem(unsigned int n)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
            n >= m_itemModel.GetCount() )
    {
        return;
    }

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() + 1 ==
            m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            items.InsertAt(n, wxWinUICreateChoicePeerItem(m_winui.get()));
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + n,
                m_itemModel.At(n).id);
            m_winui->peerItemsValid =
                items.Size() ==
                    m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            WinUIUpdatePeerItem(n);
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item insertion", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIErasePeerItem(unsigned int n)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return;

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() ==
            m_itemModel.GetCount() + 1;
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            items.RemoveAt(n);
            m_winui->peerItemIds.erase(
                m_winui->peerItemIds.begin() + n);
            m_winui->peerItemsValid =
                items.Size() == m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item removal", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIMovePeerItem(unsigned int oldIndex,
                                 unsigned int newIndex)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
         oldIndex == newIndex )
        return;

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() == m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    wxWinUIChoiceGetPeerItems(m_winui.get()).Size() ==
                        m_itemModel.GetCount() &&
                    oldIndex < m_winui->peerItemIds.size() &&
                    newIndex < m_itemModel.GetCount() &&
                    m_winui->peerItemIds[oldIndex] ==
                        m_itemModel.At(newIndex).id;
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_winui->callbackState);
        try
        {
            const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
            const auto item = items.GetAt(oldIndex);
            items.RemoveAt(oldIndex);
            items.InsertAt(newIndex, item);

            const std::uint64_t id =
                m_winui->peerItemIds.at(oldIndex);
            m_winui->peerItemIds.erase(
                m_winui->peerItemIds.begin() + oldIndex);
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + newIndex, id);
            m_winui->peerItemsValid = true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ComboBox item move", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxChoice::WinUIUpdatePeerItem(unsigned int n,
                                   double requestedBitmapScale)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
            n >= m_itemModel.GetCount() )
    {
        return;
    }

    if ( !m_winui->peerItemsValid ||
         n >= m_winui->peerItemIds.size() ||
         m_winui->peerItemIds[n] != m_itemModel.At(n).id )
    {
        ApplyItemsToPeer();
        return;
    }

    wxWinUIPeerMutationGuard guard(m_winui->callbackState);
    try
    {
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
        const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        if ( n >= items.Size() )
            return;

        const MUXC::ContentControl item =
            items.GetAt(n).as<MUXC::ContentControl>();
        const wxBitmap bmp =
            WinUIGetItemBitmap(n, requestedBitmapScale);
        if ( bmp.IsOk() )
        {
            MUXC::StackPanel panel;
            panel.Orientation(MUXC::Orientation::Horizontal);
            panel.Spacing(8);

            if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
            {
                MUXC::Image image;
                image.Source(source);
                // wxMSW's historical GetLogicalWidth()/Height() aliases
                // deliberately return physical pixels. XAML layout is in
                // DIPs, and wxBitmapBundle::GetBitmap() records the selected
                // representation's scale factor, so GetDIPSize() is the
                // portable, non-double-scaled size here.
                const wxSize dipSize = bmp.GetDIPSize();
                image.Width(dipSize.x);
                image.Height(dipSize.y);
                panel.Children().Append(image);
            }

            MUXC::TextBlock text;
            text.Text(wxWinUIToHString(m_itemModel.At(n).text));
            text.VerticalAlignment(
                winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);
            panel.Children().Append(text);
            item.Content(panel);
        }
        else
        {
            item.Content(winrt::box_value(
                wxWinUIToHString(m_itemModel.At(n).text)));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ComboBox item content update", e);
        m_winui->peerItemsValid = false;
    }
}

std::uint64_t wxChoice::WinUIGetItemIdForTesting(unsigned int n) const
{
    return n < m_itemModel.GetCount() ? m_itemModel.At(n).id : 0;
}

std::uintptr_t
wxChoice::WinUIGetItemPeerIdentityForTesting(unsigned int n) const
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return 0;

    try
    {
        const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        if ( n >= items.Size() )
            return 0;
        return reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(
                wxWinUIChoiceObjectIdentity(items.GetAt(n))));
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

bool wxChoice::WinUIGetItemPeerBitmapStateForTesting(
    unsigned int n,
    wxSize *pixelSize,
    wxSize *dipSize) const
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) ||
            !m_winui->peerItemsValid ||
            n >= m_winui->peerItemIds.size() )
    {
        return false;
    }

    try
    {
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
        namespace MUXMI =
            winrt::Microsoft::UI::Xaml::Media::Imaging;

        const auto items = wxWinUIChoiceGetPeerItems(m_winui.get());
        if ( n >= items.Size() )
            return false;

        const MUXC::ContentControl item =
            items.GetAt(n).try_as<MUXC::ContentControl>();
        const MUXC::StackPanel panel =
            item ? item.Content().try_as<MUXC::StackPanel>() : nullptr;
        if ( !panel )
            return false;

        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            const MUXC::Image image =
                children.GetAt(i).try_as<MUXC::Image>();
            if ( !image )
                continue;

            const MUXMI::WriteableBitmap bitmap =
                image.Source().try_as<MUXMI::WriteableBitmap>();
            if ( !bitmap )
                return false;

            if ( pixelSize )
            {
                *pixelSize =
                    wxSize(bitmap.PixelWidth(), bitmap.PixelHeight());
            }
            if ( dipSize )
            {
                *dipSize =
                    wxSize(static_cast<int>(std::lround(image.Width())),
                           static_cast<int>(std::lround(image.Height())));
            }
            return true;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

bool wxChoice::WinUISelectPeerItemForTesting(int selection)
{
    if ( !wxWinUIChoiceHasItemPeer(m_winui.get()) )
        return false;

    if ( !WinUIEnsurePeerConsistent() ||
         !m_winui || !m_winui->peerItemsValid )
        return false;

    try
    {
        wxWinUIChoiceSetPeerSelection(m_winui.get(), selection);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Choice peer selection test mutation", e);
        return false;
    }
}

bool wxChoice::WinUISetDropDownForTesting(bool open)
{
    if ( !m_winui || !m_winui->comboBox )
        return false;

    if ( open && WinUICoalescePopupReopen(
             [](wxChoice *owner)
             {
                 owner->WinUISetDropDownForTesting(true);
             }) )
    {
        return true;
    }

    wxWinUIChoiceImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIChoiceCallbackState> state =
        impl->callbackState;
    const MUXC::ComboBox combo = impl->comboBox;
    const std::shared_ptr<wxWinUIComboPopupRetirementState>
        popupRetirementState = impl->popupRetirementState;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxChoice *
    {
        wxChoice * const owner = wxDynamicCast(self.get(), wxChoice);
        return owner && owner->m_winui && owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == state &&
                       owner->m_winui->comboBox == combo
                 ? owner
                 : nullptr;
    };
    bool setterAttempted = false;
    try
    {
        if ( impl->comboBox.SelectionChangedTrigger() !=
             winrt::Microsoft::UI::Xaml::Controls::
                 ComboBoxSelectionChangedTrigger::Always )
        {
            return false;
        }

        if ( open )
        {
            // Match wxComboBox::Popup(): apply the template while the peer is
            // closed, then render that exact tree before opening it. Reversing
            // this order leaves the named Popup part unrealized; applying a
            // template after the setter closes the dropdown inside WinUI.
            combo.ApplyTemplate();
            wxChoice *live = getLiveOwner();
            if ( !live )
                return false;
            impl->host.ForceRender();
            live = getLiveOwner();
            if ( !live )
                return false;
            if ( !wxWinUIPrepareComboPopupOpen(popupRetirementState) )
                return false;
            live = getLiveOwner();
            if ( !live )
            {
                wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
                return false;
            }
            if ( live->WinUICoalescePopupReopen(
                     [](wxChoice *owner)
                     {
                         owner->WinUISetDropDownForTesting(true);
                     }) )
            {
                wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
                return true;
            }
        }

        setterAttempted = open;
        combo.IsDropDownOpen(open);
        wxChoice *live = getLiveOwner();
        if ( !live )
        {
            return false;
        }
        if ( open &&
             !wxWinUIFinishPreparedComboPopupOpen(popupRetirementState) )
        {
            return false;
        }

        live = getLiveOwner();
        if ( !live )
        {
            return false;
        }

        // The peer normally raises this transition inline, but a retained
        // popup can defer it. Keep the deterministic seam synchronous while
        // leaving the later native event idempotent.
        live->WinUIOnDropDownChanged(open);
        live = wxDynamicCast(self.get(), wxChoice);
        return live && live->m_winui && live->m_winui.get() == impl;
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
        wxWinUILogException("WinUI Choice dropdown test mutation", e);
        return false;
    }
    catch ( ... )
    {
        if ( setterAttempted )
            wxWinUIAbortPreparedComboPopupOpen(popupRetirementState);
        else
            wxWinUICancelPreparedComboPopupOpen(popupRetirementState);
        return false;
    }
}

bool wxChoice::WinUIIsPeerDropDownOpenForTesting() const
{
    if ( !m_winui || !m_winui->comboBox )
        return false;
    try
    {
        return m_winui->comboBox.IsDropDownOpen();
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxChoice::WinUIGetPopupReopenSnapshotForTesting(
    bool *pending,
    std::uint64_t *generation,
    unsigned *schedules,
    unsigned *runs) const
{
    if ( !m_winui || !pending || !generation || !schedules || !runs )
        return false;
    *pending = m_winui->popupReopenPending;
    *generation = m_winui->popupReopenGeneration;
    *schedules = m_winui->popupReopenSchedulesForTesting;
    *runs = m_winui->popupReopenRunsForTesting;
    return true;
}

wxWinUIPopupRetirementCoreProbeForTesting
wxChoice::WinUIProbePopupRetirementCoreForTesting()
{
    wxWinUIPopupRetirementCoreProbeForTesting result =
        wxWinUIExactPopupRetirementGate::ProbeForTesting();
    const bool poisonWasAlreadySet =
        wxWinUIIsPhysicalDisconnectPublicationPoisoned();

    auto composite =
        std::make_shared<wxWinUICompositePhysicalDisconnectGate>();
    auto first = std::make_shared<wxWinUIExactPopupRetirementGate>();
    auto second = std::make_shared<wxWinUIExactPopupRetirementGate>();
    bool completed = false;
    const bool added = composite->Add(first, true) &&
                       composite->Add(second, true);
    composite->SealForHostShutdown();
    const bool armed = composite->SetCompletion(
        [&completed]() { completed = true; });
    const bool noDebt = composite->CompleteSealedWithoutPopup();
    result.compositeReentrantCompletion =
        added && armed && noDebt && completed;
    result.sealRemainsClosed = !composite->IsSatisfied();
    const auto rejectedChild =
        std::make_shared<wxWinUIExactPopupRetirementGate>();
    result.sealedAddRejectedWithoutPoison =
        !composite->Add(rejectedChild, true) &&
        wxWinUIIsPhysicalDisconnectPublicationPoisoned() ==
            poisonWasAlreadySet;
    composite->CompleteAtXamlBoundary();

    // Exercise the source-event backstop directly: unlike public Popup(),
    // this represents an already-published native open racing host sealing.
    auto sealedNative =
        std::make_shared<wxWinUIComboPopupRetirementState>(nullptr);
    sealedNative->SealForHostShutdown();
    result.sealedNativeOpenFailsClosed =
        !sealedNative->ObserveOpened() && sealedNative->IsDegraded();
    sealedNative->CompleteAtXamlBoundary();

    if ( !poisonWasAlreadySet )
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    return result;
}

#endif // wxUSE_CHOICE
