/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/popupretirement.cpp
// Purpose:     Exact popup and causal focus retirement for native controls
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"
#include <algorithm>
#include <exception>
#include <utility>

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
#include "choice-test-access.h"
#include "combo-popup-test-access.h"
#include "physical-disconnect-test-access.h"
#endif

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;

template <typename T>
WF::IUnknown wxWinUIPopupObjectIdentity(const T& object) noexcept
{
    return object ? object.template try_as<WF::IUnknown>() : nullptr;
}

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
using wxWinUIComboPopupResolutionOrigin =
    wxWinUIComboPopupTestAccess::Origin;
using wxWinUIComboPopupFailCloseReason =
    wxWinUIComboPopupTestAccess::FailCloseReason;
wxWinUIComboPopupTestAccess::Diagnostic gs_comboPopupResolutionDiagnostic;
#else
enum class wxWinUIComboPopupResolutionOrigin : unsigned
{
    None,
    DropDownOpenedNewSession,
    DropDownOpenedExistingSession,
    AfterNativeSetter
};

enum class wxWinUIComboPopupFailCloseReason : unsigned
{
    None,
    HostShutdownSealed,
    PublicationPoisoned,
    ReopenBeforeRetirement,
    BeginSessionRejected,
    ObserveOpenedException,
    StrictResolutionRejected,
    RequestCloseException
};
#endif

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

    const WF::IUnknown rootIdentity = wxWinUIPopupObjectIdentity(root);
    if ( !rootIdentity )
        return wxWinUIElementRelationship::Unavailable;

    try
    {
        MUX::DependencyObject current = element;
        for ( unsigned depth = 0; current && depth != 512; ++depth )
        {
            if ( wxWinUIPopupObjectIdentity(current) == rootIdentity )
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

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
    static wxWinUIChoiceTestAccess::PopupRetirementCoreProbe ProbeForTesting()
    {
        wxWinUIChoiceTestAccess::PopupRetirementCoreProbe result;
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
            wxWinUIPhysicalDisconnectTestAccess::ResetPublicationPoison();
        return result;
    }
#endif

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
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
    bool HasCompletionForTesting() const noexcept
        { return static_cast<bool>(m_completion); }
#endif

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
            m_gettingFocusToken = MUXI::FocusManager::GettingFocus(
                [weak, generation](const WF::IInspectable&,
                                   const MUXI::GettingFocusEventArgs& args)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveGettingFocus(generation, args);
                });
            if ( !m_gettingFocusToken.value )
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

    void ObserveGettingFocus(
        std::uint64_t generation,
        const MUXI::GettingFocusEventArgs& args) noexcept
    {
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }
        try
        {
            const auto correlation = args.CorrelationId();
            // Only the transfer admitted by LosingFocus of this exact popup
            // can be retired here. An unrelated destination cancellation is
            // not evidence about our pending focus graph.
            if ( !wxWinUIContainsGuid(m_pendingCorrelations, correlation) &&
                 std::none_of(m_unresolvedCandidates.begin(),
                              m_unresolvedCandidates.end(),
                     [&correlation](const PendingFocusCandidate& candidate)
                     { return candidate.correlation == correlation; }) )
            {
                return;
            }
            ScheduleFocusCancellationObservation(
                generation, correlation, args, true);
        }
        catch ( ... ) { Degrade(); }
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
        ScheduleFocusCancellationObservation(
            generation, correlation, args, false);
    }

    template<typename FocusArgs>
    void ScheduleFocusCancellationObservation(
        std::uint64_t generation,
        const winrt::guid& correlation,
        const FocusArgs& args, bool getting) noexcept
    {
        auto& observations = getting
            ? m_gettingCancelObservations : m_cancelObservations;
        if ( wxWinUIContainsGuid(observations, correlation) )
            return;

        try
        {
            observations.push_back(correlation);
            const std::weak_ptr<wxWinUIExactPopupRetirementGate> weak =
                shared_from_this();
            const bool accepted = m_queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Normal,
                [weak, generation, correlation, focusArgs = args, getting]()
                {
                    const auto state = weak.lock();
                    if ( !state )
                        return;
                    state->ObserveFinalFocusCancellation(
                        generation, correlation, focusArgs, getting);
                });
            if ( !accepted )
            {
                wxWinUIEraseGuid(observations, correlation);
                Degrade();
            }
        }
        catch ( ... )
        {
            wxWinUIEraseGuid(observations, correlation);
            Degrade();
        }
    }

    template<typename FocusArgs>
    void ObserveFinalFocusCancellation(
        std::uint64_t generation,
        const winrt::guid& correlation,
        const FocusArgs& args, bool getting) noexcept
    {
        wxWinUIEraseGuid(getting ? m_gettingCancelObservations
                                : m_cancelObservations, correlation);
        if ( generation != m_generation || m_terminal || m_degraded ||
             !m_sessionActive )
        {
            return;
        }

        try
        {
            // This queue turn is solely an observation of the final Cancel
            // value after every LosingFocus/GettingFocus subscriber returned.
            // GettingFocus can cancel a transfer without changing the source
            // LosingFocus args or generating LostFocus (e.g. a retired slot).
            // This is not
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
        if ( m_gettingFocusToken.value )
        {
            try
            {
                MUXI::FocusManager::GettingFocus(m_gettingFocusToken);
                m_gettingFocusToken = {};
            }
            catch ( ... ) { ok = false; }
        }

        if ( ok || terminal )
        {
            m_popupClosedToken = {};
            m_losingFocusToken = {};
            m_lostFocusToken = {};
            m_gettingFocusToken = {};
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
            m_gettingCancelObservations.clear();
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
    winrt::event_token m_gettingFocusToken{};
    OwnerClosedProbe m_ownerClosedProbe;
    std::function<void ()> m_completion;
    std::vector<winrt::guid> m_pendingCorrelations;
    std::vector<PendingFocusCandidate> m_unresolvedCandidates;
    std::vector<winrt::guid> m_completedBeforeResolve;
    std::vector<winrt::guid> m_cancelObservations;
    std::vector<winrt::guid> m_gettingCancelObservations;
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
    explicit wxWinUICompositePhysicalDisconnectGate(
        const MUXD::DispatcherQueue& queue)
        : m_queue(queue)
    {
    }

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
            // A fan-in belongs to its creating UI queue, including when it
            // has never opened a popup or its last transient child was pruned.
            // Never admit a receipt from another dispatcher generation.
            const auto childQueue = child->GetDispatcherQueue();
            if ( childQueue && childQueue != m_queue )
                return false;

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
        return m_queue;
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

    // Shutdown seals even an empty gate until the real Source.Close boundary.
    // Deriving this identity from m_entries loses it precisely in that state.
    const MUXD::DispatcherQueue m_queue;
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

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
void wxWinUIComboPopupTestAccess::ResetDiagnostic() noexcept
{
    gs_comboPopupResolutionDiagnostic = {};
}

wxWinUIComboPopupTestAccess::Diagnostic
wxWinUIComboPopupTestAccess::GetDiagnostic() noexcept
{
    return gs_comboPopupResolutionDiagnostic;
}
#endif

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
                    wxWinUIPopupObjectIdentity(xamlRoot)));
            const std::uintptr_t popupIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(popup)));
            const std::uintptr_t childIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(child)));
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
                    wxWinUIComboPopupFailCloseReason::
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
                    wxWinUIPopupObjectIdentity(xamlRoot))) ==
                    m_preparedXamlRootIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(popup))) ==
                    m_preparedPopupIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(child))) ==
                    m_preparedPopupChildIdentity;
            if ( !identitiesMatch )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReason::
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
                    wxWinUIComboPopupFailCloseReason::
                        StrictResolutionRejected);
                return false;
            }

            return ObserveOpened() && ResolveAfterOpen();
        }
        catch ( ... )
        {
            FailClose(
                wxWinUIComboPopupFailCloseReason::
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
            wxWinUIComboPopupFailCloseReason::
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
                wxWinUIComboPopupFailCloseReason::
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
                    wxWinUIComboPopupFailCloseReason::
                        ReopenBeforeRetirement);
                return false;
            }
            return ResolveOpenedPopupStrict(
                wxWinUIComboPopupResolutionOrigin::
                    DropDownOpenedExistingSession);
        }

        try
        {
            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
            const MUXD::DispatcherQueue queue = m_combo.DispatcherQueue();
            StartOpenResolutionGeneration(xamlRoot);
            BeginResolutionDiagnostic(
                wxWinUIComboPopupResolutionOrigin::
                    DropDownOpenedNewSession);
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.rootAvailable =
                xamlRoot != nullptr;
            gs_comboPopupResolutionDiagnostic.queueAvailable =
                queue != nullptr;
            gs_comboPopupResolutionDiagnostic.sessionActive =
                m_gate->IsSessionActive();
            gs_comboPopupResolutionDiagnostic.completionArmed =
                m_gate->HasCompletionForTesting();
#endif
            if ( !m_gate->BeginUnresolvedSession(
                     xamlRoot, queue,
                     [combo = m_combo]()
                     {
                         return !combo || !combo.IsDropDownOpen();
                     }) )
            {
                FailClose(
                    wxWinUIComboPopupFailCloseReason::
                        BeginSessionRejected);
                return false;
            }
            const bool resolved = ResolveOpenedPopupStrict(
                wxWinUIComboPopupResolutionOrigin::
                    DropDownOpenedNewSession);
            ClearPreparedOpen();
            return resolved;
        }
        catch ( ... )
        {
            FailClose(
                wxWinUIComboPopupFailCloseReason::
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
                wxWinUIComboPopupResolutionOrigin::
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
            wxWinUIComboPopupResolutionOrigin::AfterNativeSetter);
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
                wxWinUIComboPopupFailCloseReason::
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
            winrt::get_abi(wxWinUIPopupObjectIdentity(xamlRoot)));
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

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
    static wxWinUIComboPopupTestAccess::Result
    DiagnosticResult(OpenResolution result) noexcept
    {
        switch ( result )
        {
            case OpenResolution::Resolved:
                return wxWinUIComboPopupTestAccess::Result::Resolved;
            case OpenResolution::TemplatePending:
                return wxWinUIComboPopupTestAccess::Result::
                    TemplatePending;
            case OpenResolution::PublicationPending:
                return wxWinUIComboPopupTestAccess::Result::
                    PublicationPending;
            case OpenResolution::Closing:
                return wxWinUIComboPopupTestAccess::Result::Closing;
            case OpenResolution::Failed:
                return wxWinUIComboPopupTestAccess::Result::Failed;
        }

        return wxWinUIComboPopupTestAccess::Result::Failed;
    }
#endif

    void BeginResolutionDiagnostic(
        wxWinUIComboPopupResolutionOrigin origin) noexcept
    {
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
        const auto previousReason =
            gs_comboPopupResolutionDiagnostic.failCloseReason;
        const unsigned previousCount =
            gs_comboPopupResolutionDiagnostic.failCloseCount;
        gs_comboPopupResolutionDiagnostic = {};
        gs_comboPopupResolutionDiagnostic.origin = origin;
        gs_comboPopupResolutionDiagnostic.failCloseReason =
            previousReason;
        gs_comboPopupResolutionDiagnostic.failCloseCount =
            previousCount;
        gs_comboPopupResolutionDiagnostic.hooksArmed =
            m_openPopupHooksArmed;
        gs_comboPopupResolutionDiagnostic.sessionActive =
            m_gate->IsSessionActive();
        gs_comboPopupResolutionDiagnostic.completionArmed =
            m_gate->HasCompletionForTesting();
#else
        (void)origin;
#endif
    }

    OpenResolution FinishResolutionDiagnostic(OpenResolution result) noexcept
    {
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
        gs_comboPopupResolutionDiagnostic.result =
            DiagnosticResult(result);
        gs_comboPopupResolutionDiagnostic.hooksArmed =
            m_openPopupHooksArmed;
#endif
        return result;
    }

    void RecordUnavailableResolution(
        wxWinUIComboPopupResolutionOrigin origin) noexcept
    {
        BeginResolutionDiagnostic(origin);
        (void)FinishResolutionDiagnostic(OpenResolution::Failed);
    }

    OpenResolution TryResolveOpenedPopup(
        wxWinUIComboPopupResolutionOrigin origin) noexcept
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
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.propertyOpen =
                propertyOpen;
#endif
            if ( !propertyOpen )
                return FinishResolutionDiagnostic(OpenResolution::Closing);

            const MUX::XamlRoot xamlRoot = m_combo.XamlRoot();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.rootAvailable =
                xamlRoot != nullptr;
#endif
            const bool rootMatches =
                xamlRoot && m_openXamlRootIdentity &&
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(xamlRoot))) ==
                    m_openXamlRootIdentity;
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.rootMatches = rootMatches;
#endif
            if ( !rootMatches )
            {
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            }
            const std::uintptr_t rootIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(xamlRoot)));
            if ( m_preparedForOpen &&
                 rootIdentity != m_preparedXamlRootIdentity )
            {
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            }

            const MUXC::IControlProtected control =
                m_combo.try_as<MUXC::IControlProtected>();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.controlAvailable =
                control != nullptr;
#endif
            if ( !control )
                return FinishResolutionDiagnostic(OpenResolution::Failed);
            const MUXCP::Popup popup =
                control.GetTemplateChild(L"Popup").try_as<MUXCP::Popup>();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.popupAvailable =
                popup != nullptr;
#endif
            if ( !popup )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::TemplatePending);
            }
            const MUX::UIElement child = popup.Child();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.childAvailable =
                child != nullptr;
#endif
            if ( !child )
            {
                return FinishResolutionDiagnostic(
                    OpenResolution::TemplatePending);
            }

            const std::uintptr_t popupIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(popup)));
            const std::uintptr_t childIdentity =
                reinterpret_cast<std::uintptr_t>(winrt::get_abi(
                    wxWinUIPopupObjectIdentity(child)));
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
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
                gs_comboPopupResolutionDiagnostic.hooksArmed = true;
#endif
            }

            // A synchronous Dismiss() can win after the exact Closed hook was
            // armed but before open-list publication was observed. Leave the
            // unsatisfied gate to consume Popup.Closed/owner close and its one
            // causal tail; this is a normal close, not a resolution failure.
            const bool stillOpen = m_combo.IsDropDownOpen();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.propertyOpen =
                stillOpen;
#endif
            if ( !stillOpen )
                return FinishResolutionDiagnostic(OpenResolution::Closing);
            const bool popupIsOpen = popup.IsOpen();
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.popupIsOpen =
                popupIsOpen;
#endif
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
                         wxWinUIPopupObjectIdentity(openPopup))) ==
                     popupIdentity )
                {
                    if ( exactOpenPopup )
                    {
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
                        gs_comboPopupResolutionDiagnostic.
                            duplicateOpenList = true;
#endif
                        return FinishResolutionDiagnostic(
                            OpenResolution::Failed);
                    }
                    exactOpenPopup = true;
                }
            }
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
            gs_comboPopupResolutionDiagnostic.exactOpenList =
                exactOpenPopup;
#endif
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
        wxWinUIComboPopupResolutionOrigin origin) noexcept
    {
        const OpenResolution result = TryResolveOpenedPopup(origin);
        if ( result == OpenResolution::Resolved )
            return true;
        // The first DropDownOpened subscriber runs after Popup::Open has
        // attached the exact child and inserted the popup in PopupRoot, but
        // before any wx callback. Missing/ambiguous facts here are therefore
        // a protocol fault, not a reason to install Closed asynchronously.
        FailClose(
            wxWinUIComboPopupFailCloseReason::
                StrictResolutionRejected);
        return false;
    }

    void FailClose(
        wxWinUIComboPopupFailCloseReason reason) noexcept
    {
#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
        gs_comboPopupResolutionDiagnostic.failCloseReason = reason;
        ++gs_comboPopupResolutionDiagnostic.failCloseCount;
#else
        (void)reason;
#endif
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
            wxWinUIComboPopupFailCloseReason::
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

std::shared_ptr<wxWinUIComboPopupRetirementState>
wxWinUICreateComboPopupRetirementState(const MUXC::ComboBox& combo)
{
    return std::make_shared<wxWinUIComboPopupRetirementState>(combo);
}

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetComboPopupRetirementGate(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    return state;
}

void wxWinUIRequestComboPopupClose(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    if ( state ) state->RequestClose();
}

void wxWinUIObserveComboPopupClosed(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept
{
    if ( state ) state->ObserveClosed();
}

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
        const MUXCP::FlyoutBase& flyout,
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
            m_openedToken = m_flyout.Opened(
                [weak](const WF::IInspectable&,
                       const WF::IInspectable&)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveOpened();
                });
            if ( !m_openedToken.value )
            {
                m_gate->MarkDegraded();
                return false;
            }
            m_closingToken = m_flyout.Closing(
                [weak](const MUXCP::FlyoutBase&,
                       const MUXCP::FlyoutBaseClosingEventArgs& args)
                {
                    if ( const auto state = weak.lock() )
                        state->ObserveClosing(args);
                });
            if ( !m_closingToken.value )
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
        ResolvePopup(m_openedObserved);
    }

    void ResolvePopup(bool strict) noexcept
    {
        if ( !m_flyout || !m_gate->IsSessionActive() || m_gate->IsDegraded() )
            return;
        if ( m_closedObserved )
        {
            FailClose();
            return;
        }
        if ( m_popupResolved )
            return;
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
            if ( !exactPopup )
            {
                // ShowAt may return before its real Popup is published. Keep
                // the unresolved session and its focus debt until Opened;
                // only that native event makes absence a strict failure.
                if ( strict )
                    FailClose();
                return;
            }
            if ( !m_gate->ResolvePopup(exactPopup, exactChild) )
                FailClose();
            else
                m_popupResolved = true;
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

    void ObserveOpened() noexcept
    {
        if ( !m_flyout || !m_gate->IsSessionActive() || m_gate->IsDegraded() )
            return;
        // A sealed host owns late native events through its terminal ledger.
        // They must not publish a new popup identity or replay a normal Hide.
        if ( m_gate->IsSealedForHostShutdown() )
            return;
        if ( m_openedObserved || m_closedObserved )
        {
            FailClose();
            return;
        }
        m_openedObserved = true;
        ResolvePopup(true);
        if ( m_closePending && m_popupResolved &&
             m_gate->IsSessionActive() && !m_gate->IsDegraded() )
        {
            m_closePending = false;
            try
            {
                m_flyout.Hide();
            }
            catch ( ... )
            {
                FailClose();
            }
        }
    }

    void ObserveClosed() noexcept
    {
        m_closedObserved = true;
        if ( !m_popupResolved )
        {
            // Source.Close may cancel publication before Opened. The host
            // already owns this sealed, unresolved debt until XAML rundown;
            // Closed alone proves neither an exact popup nor safe retirement.
            if ( m_gate->IsSealedForHostShutdown() )
                return;
            FailClose();
            return;
        }
        m_gate->ObserveOwnerClosed();
    }

    void ObserveClosing(const MUXCP::FlyoutBaseClosingEventArgs& args) noexcept
    {
        if ( !m_flyout || !m_gate->IsSessionActive() || m_gate->IsDegraded() )
            return;
        // Never cancel terminal source teardown just to finish publication.
        if ( m_gate->IsSealedForHostShutdown() )
            return;
        if ( !m_popupResolved )
            ResolvePopup(m_openedObserved);
        if ( !m_popupResolved && !m_openedObserved &&
             m_gate->IsSessionActive() && !m_gate->IsDegraded() )
        {
            // Hide can precede native publication. Cancelling Closing leaves
            // WinUI's pending opening intact; Opened will resolve the exact
            // Popup and replay Hide. This is still debt, never a close receipt.
            try
            {
                args.Cancel(true);
                m_closePending = true;
            }
            catch ( ... )
            {
                FailClose();
            }
        }
    }

    bool CleanupFlyout(bool terminal) noexcept
    {
        bool ok = true;
        if ( m_flyout && m_openedToken.value )
        {
            try
            {
                m_flyout.Opened(m_openedToken);
                m_openedToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }
        if ( m_flyout && m_closingToken.value )
        {
            try
            {
                m_flyout.Closing(m_closingToken);
                m_closingToken = {};
            }
            catch ( ... )
            {
                ok = false;
            }
        }
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
            m_openedToken = {};
            m_closingToken = {};
            m_closedToken = {};
            m_flyout = nullptr;
            m_xamlRoot = nullptr;
            m_exactItem = nullptr;
        }
        return ok;
    }

    MUXCP::FlyoutBase m_flyout{ nullptr };
    MUX::XamlRoot m_xamlRoot{ nullptr };
    MUX::DependencyObject m_exactItem{ nullptr };
    winrt::event_token m_openedToken{};
    winrt::event_token m_closingToken{};
    winrt::event_token m_closedToken{};
    std::shared_ptr<wxWinUIExactPopupRetirementGate> m_gate;
    std::function<void ()> m_observer;
    bool m_openedObserved = false;
    bool m_closedObserved = false;
    bool m_popupResolved = false;
    bool m_closePending = false;
};

// The short-lived DataView date editor uses the real CalendarDatePicker
// template, not a second calendar/flyout. Its Opening hook precedes all focus
// movement; Opened resolves only the Popup containing that exact CalendarView.
class wxWinUICalendarPopupRetirementState final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<wxWinUICalendarPopupRetirementState>
{
public:
    explicit wxWinUICalendarPopupRetirementState(
        const MUXC::CalendarDatePicker& picker)
        : m_picker(picker),
          m_gate(std::make_shared<wxWinUIExactPopupRetirementGate>())
    {
        if ( picker )
            m_gate->PreserveDispatcherQueue(picker.DispatcherQueue());
    }

    ~wxWinUICalendarPopupRetirementState() override { Revoke(); }

    bool Arm()
    {
        if ( !m_picker )
            return false;
        const std::weak_ptr<wxWinUICalendarPopupRetirementState> weak =
            shared_from_this();
        m_loaded = m_picker.Loaded(
            [weak](const WF::IInspectable&, const MUX::RoutedEventArgs&)
            {
                if ( const auto state = weak.lock() )
                    state->BindTemplate();
            });
        // CalendarDatePicker forwards its template Flyout.Opened before later
        // flyout subscribers run. Observe the control event before application
        // delegates can finish editing or destroy its logical model.
        m_opened = m_picker.Opened(
            [weak](const WF::IInspectable&, const WF::IInspectable&)
            {
                if ( const auto state = weak.lock() )
                    state->Opened();
            });
        m_closed = m_picker.Closed(
            [weak](const WF::IInspectable&, const WF::IInspectable&)
            {
                if ( const auto state = weak.lock() )
                {
                    state->m_gate->ObserveOwnerClosed();
                }
            });
        if ( m_picker.IsLoaded() )
            BindTemplate();
        return !m_gate->IsDegraded();
    }

    void RequestClose() noexcept
    {
        try
        {
            if ( !m_picker )
                return;
            if ( m_picker.IsCalendarOpen() && !m_openedObserved &&
                 !m_gate->IsSealedForHostShutdown() )
            {
                // IsCalendarOpen becomes true before the native Flyout has
                // necessarily delivered Opening/Opened. Keep its pending
                // publication alive, arm focus observation now, and close at
                // Opened after resolving the exact Popup. Setting false here
                // can cancel publication before there is a Closed receipt.
                if ( !m_gate->IsSessionActive() )
                {
                    BindTemplate();
                    if ( !TemplateIsCurrent() ||
                         !m_gate->BeginUnresolvedSession(
                             m_picker.XamlRoot(), m_picker.DispatcherQueue(),
                             [picker = m_picker]()
                             {
                                 return !picker.IsCalendarOpen();
                             }) )
                    {
                        FailClose();
                        return;
                    }
                    m_resolved = false;
                    m_preparedClose = true;
                }
                m_closePending = true;
                m_retiring = true;
                return;
            }
            m_retiring = true;
            m_picker.IsCalendarOpen(false);
            if ( !m_picker.IsCalendarOpen() )
                m_gate->ObserveOwnerClosed();
        }
        catch ( ... ) { FailClose(); }
    }

    bool IsSatisfied() const noexcept override
        { return m_gate->IsSatisfied(); }
    bool IsDegraded() const noexcept override
        { return m_gate->IsDegraded(); }
    bool IsSealedForHostShutdown() const noexcept override
        { return m_gate->IsSealedForHostShutdown(); }
    MUXD::DispatcherQueue GetDispatcherQueue() const noexcept override
        { return m_gate->GetDispatcherQueue(); }
    bool SetCompletion(std::function<void()> complete) noexcept override
        { return m_gate->SetCompletion(std::move(complete)); }
    void SealForHostShutdown() noexcept override
    {
        m_retiring = true;
        LatchPublishedPopupDebtBeforeSourceClose();
        m_gate->SealForHostShutdown();
    }
    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        if ( m_gate->IsSessionActive() )
            return;
        try
        {
            if ( m_picker && m_picker.IsCalendarOpen() )
                m_gate->MarkDegraded();
        }
        catch ( ... ) { m_gate->MarkDegraded(); }
    }
    bool CompleteSealedWithoutPopup() noexcept override
    {
        LatchPublishedPopupDebtBeforeSourceClose();
        return m_gate->CompleteSealedWithoutPopup();
    }
    void CompleteAtXamlBoundary() noexcept override
    {
        m_retiring = true;
        m_gate->CompleteAtXamlBoundary();
        Revoke();
        m_picker = nullptr;
        m_flyout = nullptr;
        m_calendar = nullptr;
        m_root = nullptr;
    }

private:
    void BindTemplate() noexcept
    {
        if ( m_retiring || !m_picker || m_gate->IsDegraded() )
            return;
        try
        {
            const auto control = m_picker.as<MUXC::IControlProtected>();
            const auto root = control.GetTemplateChild(L"Root")
                .try_as<MUX::FrameworkElement>();
            const auto calendar = control.GetTemplateChild(L"CalendarView")
                .try_as<MUXC::CalendarView>();
            const auto flyout = root
                ? MUXCP::FlyoutBase::GetAttachedFlyout(root)
                      .try_as<MUXC::Flyout>()
                : nullptr;
            if ( !root || !calendar || !flyout ||
                 flyout.Content() != calendar )
            {
                FailClose();
                return;
            }
            if ( flyout == m_flyout && calendar == m_calendar &&
                 root == m_root )
            {
                return;
            }
            if ( m_gate->IsSessionActive() )
            {
                FailClose();
                return;
            }
            RevokeFlyout();
            m_root = root;
            m_calendar = calendar;
            m_flyout = flyout;
            const std::weak_ptr<wxWinUICalendarPopupRetirementState> weak =
                shared_from_this();
            m_opening = flyout.Opening(
                [weak](const WF::IInspectable&, const WF::IInspectable&)
                {
                    if ( const auto state = weak.lock() )
                        state->Opening();
                });
            m_closing = flyout.Closing(
                [weak](const MUXCP::FlyoutBase&,
                       const MUXCP::FlyoutBaseClosingEventArgs& args)
                {
                    if ( const auto state = weak.lock() )
                        state->Closing(args);
                });
        }
        catch ( ... ) { FailClose(); }
    }

    bool TemplateIsCurrent() const
    {
        if ( !m_picker || !m_root || !m_flyout || !m_calendar )
            return false;
        const auto control = m_picker.as<MUXC::IControlProtected>();
        return control.GetTemplateChild(L"Root") == m_root &&
               control.GetTemplateChild(L"CalendarView") == m_calendar &&
               MUXCP::FlyoutBase::GetAttachedFlyout(m_root) == m_flyout &&
               m_flyout.Content() == m_calendar;
    }

    void Opening() noexcept
    {
        try
        {
            if ( m_preparedClose && m_gate->IsSessionActive() &&
                 !m_gate->IsDegraded() &&
                 !m_gate->IsSealedForHostShutdown() && TemplateIsCurrent() )
            {
                // RequestClose already owns this exact pending opening.
                // Do not begin another generation or discard its close intent.
                m_preparedClose = false;
                return;
            }
            if ( m_retiring || m_gate->IsSealedForHostShutdown() ||
                 wxWinUIIsPhysicalDisconnectPublicationPoisoned() ||
                 !TemplateIsCurrent() || !m_gate->BeginUnresolvedSession(
                     m_picker.XamlRoot(), m_picker.DispatcherQueue(),
                     [picker = m_picker]()
                     {
                         return !picker.IsCalendarOpen();
                     }) )
            {
                FailClose();
                return;
            }
            m_resolved = false;
            m_openedObserved = false;
            m_closePending = false;
        }
        catch ( ... ) { FailClose(); }
    }

    bool Resolve(bool strict) noexcept
    {
        if ( m_resolved )
            return true;
        try
        {
            if ( !m_gate->IsSessionActive() || !TemplateIsCurrent() )
            {
                FailClose();
                return false;
            }
            MUXCP::Popup exact{nullptr};
            MUX::UIElement child{nullptr};
            for ( const auto& popup :
                  MUXM::VisualTreeHelper::GetOpenPopupsForXamlRoot(
                      m_picker.XamlRoot()) )
            {
                if ( !popup.IsOpen() || !popup.Child() )
                    continue;
                const auto relationship =
                    wxWinUIIsElementWithin(m_calendar, popup.Child());
                if ( relationship == wxWinUIElementRelationship::Unavailable ||
                     (relationship == wxWinUIElementRelationship::Inside &&
                      exact) )
                {
                    FailClose();
                    return false;
                }
                if ( relationship == wxWinUIElementRelationship::Inside )
                {
                    exact = popup;
                    child = popup.Child();
                }
            }
            if ( !exact )
            {
                if ( strict )
                    FailClose();
                return false;
            }
            m_resolved = m_gate->ResolvePopup(exact, child);
            if ( !m_resolved )
                FailClose();
            return m_resolved;
        }
        catch ( ... ) { FailClose(); return false; }
    }

    void Opened() noexcept
    {
        if ( m_gate->IsSealedForHostShutdown() )
            return;
        m_openedObserved = true;
        m_preparedClose = false;
        if ( Resolve(true) && m_closePending )
        {
            m_closePending = false;
            RequestClose();
        }
    }

    void Closing(const MUXCP::FlyoutBaseClosingEventArgs& args) noexcept
    {
        if ( m_gate->IsSealedForHostShutdown() || m_gate->IsDegraded() ||
             !m_gate->IsSessionActive() )
        {
            return;
        }
        if ( !Resolve(m_openedObserved) && !m_openedObserved &&
             !m_gate->IsDegraded() )
        {
            try
            {
                args.Cancel(true);
                m_closePending = true;
            }
            catch ( ... ) { FailClose(); }
        }
    }

    void FailClose() noexcept
    {
        m_gate->MarkDegraded();
        try { if ( m_picker ) m_picker.IsCalendarOpen(false); }
        catch ( ... ) { }
    }

    void RevokeFlyout() noexcept
    {
        try
        {
            if ( m_flyout && m_opening.value ) m_flyout.Opening(m_opening);
            if ( m_flyout && m_closing.value ) m_flyout.Closing(m_closing);
        }
        catch ( ... ) { m_gate->MarkDegraded(); }
        m_opening = {};
        m_closing = {};
    }

    void Revoke() noexcept
    {
        RevokeFlyout();
        try
        {
            if ( m_picker && m_loaded.value ) m_picker.Loaded(m_loaded);
            if ( m_picker && m_opened.value ) m_picker.Opened(m_opened);
            if ( m_picker && m_closed.value ) m_picker.Closed(m_closed);
        }
        catch ( ... ) { m_gate->MarkDegraded(); }
        m_loaded = {};
        m_opened = {};
        m_closed = {};
    }

    MUXC::CalendarDatePicker m_picker{nullptr};
    MUX::FrameworkElement m_root{nullptr};
    MUXC::CalendarView m_calendar{nullptr};
    MUXC::Flyout m_flyout{nullptr};
    std::shared_ptr<wxWinUIExactPopupRetirementGate> m_gate;
    winrt::event_token m_loaded{}, m_opened{}, m_closed{};
    winrt::event_token m_opening{}, m_closing{};
    bool m_resolved = false;
    bool m_openedObserved = false;
    bool m_closePending = false;
    bool m_preparedClose = false;
    bool m_retiring = false;
};

std::shared_ptr<wxWinUICalendarPopupRetirementState>
wxWinUICreateCalendarPopupRetirementState(
    const MUXC::CalendarDatePicker& picker)
{
    auto state = std::make_shared<wxWinUICalendarPopupRetirementState>(picker);
    return state->Arm() ? state : nullptr;
}

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetCalendarPopupRetirementGate(
    const std::shared_ptr<wxWinUICalendarPopupRetirementState>& state) noexcept
{
    return state;
}

void wxWinUIRequestCalendarPopupClose(
    const std::shared_ptr<wxWinUICalendarPopupRetirementState>& state) noexcept
{
    if ( state ) state->RequestClose();
}

std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreateCompositePhysicalDisconnectGate()
{
    const auto queue = MUXD::DispatcherQueue::GetForCurrentThread();
    if ( !queue )
        return {};
    return std::make_shared<wxWinUICompositePhysicalDisconnectGate>(queue);
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
    const MUXCP::FlyoutBase& flyout,
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

#if defined(WXWINUI_TEST_SUPPORT) && wxUSE_CHOICE
wxWinUIChoiceTestAccess::PopupRetirementCoreProbe wxWinUIChoiceTestAccess::ProbePopupRetirementCore()
{
    PopupRetirementCoreProbe result =
        wxWinUIExactPopupRetirementGate::ProbeForTesting();
    const bool poisonWasAlreadySet =
        wxWinUIIsPhysicalDisconnectPublicationPoisoned();

    auto composite =
        std::make_shared<wxWinUICompositePhysicalDisconnectGate>(
            MUXD::DispatcherQueue::GetForCurrentThread());
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
        wxWinUIPhysicalDisconnectTestAccess::ResetPublicationPoison();
    return result;
}
#endif // WXWINUI_TEST_SUPPORT && wxUSE_CHOICE

#endif // wxUSE_WINUI3
