/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/compositeretirement.h
// Purpose:     Causal physical retirement of nested shared-island slots
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_COMPOSITERETIREMENT_H_
#define _WX_WINUI_COMPOSITERETIREMENT_H_

#include "private.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

// UI-thread only. Unlike the per-control popup fan-in, this gate does not own
// or retarget any child gate: each child keeps its existing host destination.
// The host sends a receipt only after removing that exact child's physical
// carrier. Neither popup completion nor logical deletion is such a receipt.
class wxWinUICompositeSlotRetirementGate final :
    public wxWinUIPhysicalDisconnectGate,
    public std::enable_shared_from_this<wxWinUICompositeSlotRetirementGate>
{
public:
    using ChildId = std::uint64_t;
    using DispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue;

    static std::shared_ptr<wxWinUICompositeSlotRetirementGate> Make(
        std::shared_ptr<wxWinUIPhysicalDisconnectGate> parentGate,
        const DispatcherQueue& queue) noexcept
    {
        try
        {
            return std::shared_ptr<wxWinUICompositeSlotRetirementGate>(
                new wxWinUICompositeSlotRetirementGate(
                    std::move(parentGate), queue));
        }
        catch ( ... )
        {
            wxWinUINotePhysicalDisconnectGateDegraded();
            return {};
        }
    }

    // Register before logical removal can run child callbacks. A zero id
    // refuses the operation: the caller must not remove an unaccounted child.
    ChildId AddChild() noexcept
    {
        if ( m_closing || m_sealed || m_terminal || IsDegraded() )
            return 0;
        if ( m_nextId == std::numeric_limits<ChildId>::max() )
        {
            Degrade();
            return 0;
        }
        try
        {
            const ChildId id = m_nextId + 1;
            m_pending.push_back(id);
            m_nextId = id;
            return id;
        }
        catch ( ... )
        {
            Degrade();
            return 0;
        }
    }

    // False identifies an unknown, duplicate, or terminal receipt. In
    // particular, replaying one id cannot consume another child's debt.
    bool ChildDisconnected(ChildId id) noexcept
    {
        if ( !id || m_terminal )
            return false;
        const auto found = std::find(m_pending.begin(), m_pending.end(), id);
        if ( found == m_pending.end() )
            return false;
        const auto keepAlive = weak_from_this().lock();
        m_pending.erase(found);
        MaybeComplete();
        return true;
    }

    bool IsSatisfied() const noexcept override
    {
        if ( m_terminal || m_sealed || IsDegraded() || !m_pending.empty() )
            return false;
        return m_closing ? m_parentCompleted
                         : !m_parentGate || m_parentGate->IsSatisfied();
    }

    bool IsDegraded() const noexcept override
    {
        return m_degraded || (m_parentGate && m_parentGate->IsDegraded());
    }

    bool IsSealedForHostShutdown() const noexcept override { return m_sealed; }

    DispatcherQueue GetDispatcherQueue() const noexcept override
    {
        return m_queue;
    }

    bool SetCompletion(std::function<void ()> complete) noexcept override
    {
        if ( !complete || m_terminal )
            return false;
        const auto keepAlive = weak_from_this().lock();
        m_completion = std::move(complete);
        m_closing = true;
        if ( m_parentGate && !m_parentArmed )
        {
            // Arm only once: retargeting at TLW shutdown changes our final
            // destination, not the causal destination installed on this gate.
            m_parentArmed = true;
            try
            {
                const std::weak_ptr<wxWinUICompositeSlotRetirementGate> weak =
                    keepAlive;
                std::function<void ()> parentCompleted = [weak]()
                {
                    if ( const auto gate = weak.lock() )
                        gate->ParentCompleted();
                };
                const auto parent = m_parentGate;
                ++m_parentCallDepth;
                const bool accepted =
                    parent->SetCompletion(std::move(parentCompleted));
                --m_parentCallDepth;
                if ( !accepted )
                {
                    Degrade();
                    return false;
                }
            }
            catch ( ... )
            {
                Degrade();
                return false;
            }
        }
        MaybeComplete();
        return true;
    }

    void SealForHostShutdown() noexcept override
    {
        if ( m_terminal || m_sealed )
            return;
        const auto keepAlive = weak_from_this().lock();
        m_sealed = true;
        // An earlier normal completion is not proof that the last live
        // pre-Source.Close observation found no newly published popup debt.
        m_parentCompleted = !m_parentGate;
        if ( const auto parent = m_parentGate )
        {
            ++m_parentCallDepth;
            parent->SealForHostShutdown();
            --m_parentCallDepth;
        }
    }

    void LatchPublishedPopupDebtBeforeSourceClose() noexcept override
    {
        if ( m_terminal )
            return;
        const auto keepAlive = weak_from_this().lock();
        if ( const auto parent = m_parentGate )
        {
            ++m_parentCallDepth;
            parent->LatchPublishedPopupDebtBeforeSourceClose();
            --m_parentCallDepth;
        }
        // Child popup debt remains owned by each child's own gate. Pending
        // receipts are deliberately not cleared by Source.Close/Unloaded.
        MaybeComplete();
    }

    bool CompleteSealedWithoutPopup() noexcept override
    {
        if ( m_terminal || !m_sealed || IsDegraded() )
            return false;
        const auto keepAlive = weak_from_this().lock();
        // The host calls this only after its terminal ledger owns the whole
        // graph. Until then even an empty fan-in must remain observable.
        m_shutdownCompletionRequested = true;
        if ( const auto parent = m_parentGate )
        {
            ++m_parentCallDepth;
            const bool completed = parent->CompleteSealedWithoutPopup();
            --m_parentCallDepth;
            if ( !m_terminal && completed )
                m_parentCompleted = true;
        }
        const bool completed = !m_terminal && !IsDegraded() &&
                               m_parentCompleted && m_pending.empty();
        MaybeComplete();
        return completed;
    }

    // Shutdown-only handoff, before sealing the gates for Source.Close().
    // The caller must already own the entire attached graph and account for
    // every physically present child gate in its TLW retirement ledger. That
    // ledger then retires the graph as a unit: waiting for receipts from slot
    // wrappers which shutdown is about to delete would be a circular wait.
    // Transfer only this parent's gate; do not consume any child receipt or
    // revoke the parent's popup observations before the ledger adopts them.
    std::shared_ptr<wxWinUIPhysicalDisconnectGate>
        TakeParentGateForGraphRetirement() noexcept
    {
        if ( m_terminal )
            return {};
        const auto keepAlive = weak_from_this().lock();
        m_degraded = IsDegraded();
        m_terminal = true;
        auto parent = std::move(m_parentGate);
        m_pending.clear();
        m_completion = {};
        m_queue = nullptr;
        return parent;
    }

    void CompleteAtXamlBoundary() noexcept override
    {
        if ( m_terminal )
            return;
        const auto keepAlive = weak_from_this().lock();
        m_degraded = IsDegraded();
        m_terminal = true;
        // Hold the final callback's captures while the parent's projection
        // callbacks are revoked. Never invoke normal slot removal here.
        auto discardedCompletion = std::move(m_completion);
        const auto parent = std::move(m_parentGate);
        if ( parent )
            parent->CompleteAtXamlBoundary();
        m_pending.clear();
        discardedCompletion = {};
        m_queue = nullptr;
    }

private:
    wxWinUICompositeSlotRetirementGate(
        std::shared_ptr<wxWinUIPhysicalDisconnectGate> parentGate,
        const DispatcherQueue& queue)
        : m_parentGate(std::move(parentGate)),
          m_queue(queue),
          m_parentCompleted(!m_parentGate)
    {
        if ( !m_queue && m_parentGate )
            m_queue = m_parentGate->GetDispatcherQueue();
    }

    void ParentCompleted() noexcept
    {
        if ( m_terminal )
            return;
        m_parentCompleted = true;
        MaybeComplete();
    }

    void MaybeComplete() noexcept
    {
        if ( m_terminal || !m_closing || m_parentCallDepth || IsDegraded() ||
             !m_parentCompleted || !m_pending.empty() || !m_completion ||
             (m_sealed && !m_shutdownCompletionRequested) )
        {
            return;
        }
        const auto keepAlive = weak_from_this().lock();
        auto complete = std::move(m_completion);
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
        wxWinUINotePhysicalDisconnectGateDegraded();
    }

    std::shared_ptr<wxWinUIPhysicalDisconnectGate> m_parentGate;
    DispatcherQueue m_queue{nullptr};
    std::vector<ChildId> m_pending;
    std::function<void ()> m_completion;
    ChildId m_nextId = 0;
    unsigned int m_parentCallDepth = 0;
    bool m_parentCompleted = false;
    bool m_parentArmed = false;
    bool m_closing = false;
    bool m_sealed = false;
    bool m_shutdownCompletionRequested = false;
    bool m_degraded = false;
    bool m_terminal = false;
};

#endif // _WX_WINUI_COMPOSITERETIREMENT_H_
