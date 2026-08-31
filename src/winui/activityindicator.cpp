/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/activityindicator.cpp
// Purpose:     wxWinUI wxActivityIndicator implementation (WinUI ProgressRing)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_ACTIVITYINDICATOR

#include "wx/activityindicator.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/weakref.h"

#include "private.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "feedback-test-access.h"
#endif
#include "wx/winui/private/appearance.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>

namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

class wxWinUIActivityCallbackState final
{
public:
    static constexpr unsigned MaxApplyPassBudget = 16;

    explicit wxWinUIActivityCallbackState(wxActivityIndicator *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t GetGeneration() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxActivityIndicator *GetOwner(std::uint64_t generation) const
    {
        if ( !generation || generation != GetGeneration() )
            return nullptr;

        wxActivityIndicator * const owner =
            m_owner.load(std::memory_order_acquire);
        return generation == GetGeneration() ? owner : nullptr;
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
        BumpModelRevision();
        RequestApply();
        CancelDeferredApply();
        QuarantineApply();
    }

    std::uint64_t BumpModelRevision()
    {
        // A model mutation which starts outside the projection transaction is
        // a fresh request and is allowed a new bounded budget. Mutations made
        // synchronously by a peer callback are part of the current request:
        // letting each of them re-arm the budget would turn the deferred replay
        // into an unbounded event-loop pump.
        if ( !m_applyActive.load(std::memory_order_acquire) )
        {
            m_applyPassesRemaining.store(
                MaxApplyPassBudget, std::memory_order_release);
            m_applyQuarantined.store(false, std::memory_order_release);
            ClearApplyBudgetWarning();
        }

        std::uint64_t revision =
            m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        if ( !revision )
        {
            revision =
                m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        return revision;
    }

    std::uint64_t GetModelRevision() const
    {
        return m_modelRevision.load(std::memory_order_acquire);
    }

    bool TryBeginApply()
    {
        bool expected = false;
        if ( !m_applyActive.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            RequestApply();
            return false;
        }
        return true;
    }

    void EndApply()
    {
        m_applyActive.store(false, std::memory_order_release);
    }

    void RequestApply()
    {
        m_applyRequested.store(true, std::memory_order_release);
    }

    bool ConsumeApplyRequest()
    {
        return m_applyRequested.exchange(false, std::memory_order_acq_rel);
    }

    bool TryScheduleDeferredApply()
    {
        bool expected = false;
        return m_deferredApplyScheduled.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    bool ConsumeDeferredApply()
    {
        return m_deferredApplyScheduled.exchange(
            false, std::memory_order_acq_rel);
    }

    void CancelDeferredApply()
    {
        m_deferredApplyScheduled.store(false, std::memory_order_release);
    }

    bool HasDeferredApply() const
    {
        return m_deferredApplyScheduled.load(std::memory_order_acquire);
    }

    bool TryConsumeApplyPass()
    {
        unsigned remaining =
            m_applyPassesRemaining.load(std::memory_order_acquire);
        while ( remaining )
        {
            if ( m_applyPassesRemaining.compare_exchange_weak(
                     remaining, remaining - 1,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire) )
            {
                return true;
            }
        }
        return false;
    }

    bool HasApplyPassesRemaining() const
    {
        return m_applyPassesRemaining.load(std::memory_order_acquire) != 0;
    }

    void QuarantineApply()
    {
        m_applyPassesRemaining.store(0, std::memory_order_release);
        m_applyRequested.store(false, std::memory_order_release);
        m_applyQuarantined.store(true, std::memory_order_release);
        CancelDeferredApply();
    }

    bool IsApplyQuarantined() const
    {
        return m_applyQuarantined.load(std::memory_order_acquire);
    }

    bool MarkApplyBudgetWarning()
    {
        return !m_applyBudgetWarning.exchange(
            true, std::memory_order_acq_rel);
    }

    void ClearApplyBudgetWarning()
    {
        m_applyBudgetWarning.store(false, std::memory_order_release);
    }

private:
    std::atomic<wxActivityIndicator *> m_owner;
    std::atomic<std::uint64_t> m_generation{ 1 };
    std::atomic<std::uint64_t> m_modelRevision{ 1 };
    std::atomic<bool> m_applyActive{ false };
    std::atomic<bool> m_applyRequested{ false };
    std::atomic<bool> m_deferredApplyScheduled{ false };
    std::atomic<bool> m_applyBudgetWarning{ false };
    std::atomic<unsigned> m_applyPassesRemaining{ MaxApplyPassBudget };
    std::atomic<bool> m_applyQuarantined{ false };
};

class wxWinUIActivityApplyGuard final
{
public:
    explicit wxWinUIActivityApplyGuard(
        std::shared_ptr<wxWinUIActivityCallbackState> state)
        : m_state(std::move(state)),
          m_acquired(m_state->TryBeginApply())
    {
    }

    ~wxWinUIActivityApplyGuard()
    {
        if ( m_acquired )
            m_state->EndApply();
    }

    explicit operator bool() const
    {
        return m_acquired;
    }

private:
    std::shared_ptr<wxWinUIActivityCallbackState> m_state;
    bool m_acquired;
};

} // namespace

class wxWinUIActivityIndicatorImpl
{
public:
    ~wxWinUIActivityIndicatorImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();
#ifdef WXWINUI_TEST_SUPPORT
        nextPeerWriteHookForTesting = nullptr;
        nextPeerWriteContextForTesting = nullptr;
#endif
        host.Close();
        ring = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    MUXC::ProgressRing ring{ nullptr };
    std::shared_ptr<wxWinUIActivityCallbackState> callbackState;
#ifdef WXWINUI_TEST_SUPPORT
    using PeerWriteHook = void (*)(void *);
    PeerWriteHook nextPeerWriteHookForTesting = nullptr;
    void *nextPeerWriteContextForTesting = nullptr;
#endif
};

wxIMPLEMENT_DYNAMIC_CLASS(wxActivityIndicator, wxActivityIndicatorBase);

wxActivityIndicator::wxActivityIndicator()
{
}

wxActivityIndicator::wxActivityIndicator(wxWindow* parent,
                                         wxWindowID winid,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style,
                                         const wxString& name)
{
    Create(parent, winid, pos, size, style, name);
}

wxActivityIndicator::~wxActivityIndicator()
{
    if ( m_winui )
        m_winui->Close();
}

bool wxActivityIndicator::Create(wxWindow* parent,
                                 wxWindowID winid,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(
             parent, winid, pos, size, style, wxDefaultValidator, name) )
    {
        return false;
    }

    m_running = false;
    m_winui.reset(new wxWinUIActivityIndicatorImpl);
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    wxWinUIActivityIndicatorImpl * const impl = m_winui.get();
    wxWeakRef<wxActivityIndicator> alive(this);
    const auto abandonCreate = [&]()
    {
        if ( alive && alive->m_winui.get() == impl )
            alive->m_winui.reset();
        return false;
    };
    try
    {
        impl->callbackState =
            std::make_shared<wxWinUIActivityCallbackState>(this);
        impl->ring = MUXC::ProgressRing();
        impl->ring.IsTabStop(false);
        impl->ring.IsHitTestVisible(false);
        MUXA::AutomationProperties::SetLocalizedControlType(
            impl->ring, wxWinUIToHString(_("activity indicator")));
        MUXA::AutomationProperties::SetLiveSetting(
            impl->ring, MUXAP::AutomationLiveSetting::Polite);
        MUXA::AutomationProperties::SetAccessibilityView(
            impl->ring, MUXAP::AccessibilityView::Content);
        if ( !ApplyWinUIModel() )
            return abandonCreate();
        if ( !alive || !alive->m_winui ||
             alive->m_winui.get() != impl )
        {
            return false;
        }
        if ( !impl->host.SetContent(impl->ring) )
            return abandonCreate();
        if ( !alive || !alive->m_winui ||
             alive->m_winui.get() != impl )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ProgressRing creation", e);
        return abandonCreate();
    }

    wxActivityIndicator * const owner = alive.get();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl )
    {
        return false;
    }
    owner->SetInitialSize(size);
    return alive && alive->m_winui &&
           alive->m_winui.get() == impl;
}

void wxActivityIndicator::Start()
{
    if ( m_running )
        return;
    m_running = true;
    BumpWinUIModelRevision();
    ApplyWinUIModel();
}

void wxActivityIndicator::Stop()
{
    if ( !m_running )
        return;
    m_running = false;
    BumpWinUIModelRevision();
    ApplyWinUIModel();
}

bool wxActivityIndicator::IsRunning() const
{
    return m_running;
}

bool wxActivityIndicator::SetForegroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetForegroundColour(colour);
    BumpWinUIModelRevision();
    ApplyWinUIModel();
    return changed;
}

bool wxActivityIndicator::SetBackgroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetBackgroundColour(colour);
    BumpWinUIModelRevision();
    ApplyWinUIModel();
    return changed;
}

void wxActivityIndicator::BumpWinUIModelRevision()
{
    if ( m_winui && m_winui->callbackState )
        m_winui->callbackState->BumpModelRevision();
}

bool wxActivityIndicator::ApplyWinUIModel()
{
    if ( !m_winui || !m_winui->ring || !m_winui->callbackState )
        return false;

    wxWinUIActivityIndicatorImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIActivityCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const wxWinUIActivityApplyGuard applyGuard(callbackState);
    if ( !applyGuard )
        return true;

    const auto ring = impl->ring;

    enum class ApplyState
    {
        Current,
        Restart,
        Dead
    };

    constexpr unsigned MaxSynchronousApplyPasses = 8;
    const auto quarantineLatestRevision =
        [&]() -> bool
        {
            callbackState->QuarantineApply();
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI ProgressRing model kept changing during "
                    "projection; quarantining this request until the next "
                    "external model mutation.");
            }
            return false;
        };
    const auto deferLatestRevision =
        [&]() -> bool
        {
            if ( !callbackState->HasApplyPassesRemaining() )
                return quarantineLatestRevision();

            callbackState->RequestApply();
            if ( callbackState->HasDeferredApply() )
                return true;

            if ( !wxTheApp )
                return quarantineLatestRevision();

            // Logging can invoke an application log target which yields the
            // event loop. Do it before making the replay ticket consumable.
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI ProgressRing model kept changing during "
                    "projection; using its one bounded asynchronous replay.");
            }

            if ( !callbackState->TryScheduleDeferredApply() )
            {
                return callbackState->HasDeferredApply()
                    ? true
                    : quarantineLatestRevision();
            }

            const std::weak_ptr<wxWinUIActivityCallbackState>
                weakState(callbackState);
            const std::uint64_t scheduledRevision =
                callbackState->GetModelRevision();

            // Keep posting as the final operation before the active projection
            // guard unwinds.
            wxTheApp->CallAfter(
                [weakState, lifetimeGeneration, scheduledRevision, impl]()
                {
                    const std::shared_ptr<
                        wxWinUIActivityCallbackState> state =
                            weakState.lock();
                    if ( !state || !state->ConsumeDeferredApply() )
                        return;

                    wxActivityIndicator * const owner =
                        state->GetOwner(lifetimeGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state )
                    {
                        return;
                    }

                    if ( state->GetModelRevision() != scheduledRevision )
                        state->RequestApply();
                    owner->ApplyWinUIModel();
                });
            return true;
        };

    for ( unsigned pass = 0;
          pass < MaxSynchronousApplyPasses;
          ++pass )
    {
        if ( !callbackState->TryConsumeApplyPass() )
            return quarantineLatestRevision();

        callbackState->ConsumeApplyRequest();
        wxActivityIndicator * const live =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !live || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != callbackState )
        {
            return false;
        }

        const std::uint64_t revision =
            callbackState->GetModelRevision();
        const bool running = live->m_running;
        const wxColour foreground =
            live->m_hasFgCol
                ? live->GetForegroundColour()
                : wxNullColour;
        const wxColour background =
            live->m_hasBgCol
                ? live->GetBackgroundColour()
                : wxNullColour;

        const auto checkState =
            [callbackState, lifetimeGeneration, impl, revision]()
            {
                wxActivityIndicator * const current =
                    callbackState->GetOwner(lifetimeGeneration);
                if ( !current || !current->m_winui ||
                     current->m_winui.get() != impl ||
                     current->m_winui->callbackState != callbackState )
                {
                    return ApplyState::Dead;
                }
                return callbackState->GetModelRevision() == revision
                    ? ApplyState::Current
                    : ApplyState::Restart;
            };

#ifdef WXWINUI_TEST_SUPPORT
        const wxWinUIActivityIndicatorImpl::PeerWriteHook hook =
            impl->nextPeerWriteHookForTesting;
        void * const hookContext =
            impl->nextPeerWriteContextForTesting;
        impl->nextPeerWriteHookForTesting = nullptr;
        impl->nextPeerWriteContextForTesting = nullptr;
#endif

        try
        {
            MUXA::AutomationProperties::SetItemStatus(
                ring,
                wxWinUIToHString(running ? _("Busy") : _("Idle")));
            ApplyState state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

#ifdef WXWINUI_TEST_SUPPORT
            if ( hook )
            {
                hook(hookContext);
                state = checkState();
                if ( state == ApplyState::Dead )
                    return false;
                if ( state == ApplyState::Restart )
                    continue;
            }
#endif

            ring.IsActive(running);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxWinUIApplyForeground(ring, foreground);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxWinUIApplyBackground(ring, background);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            impl->host.ForceRender();
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            if ( callbackState->ConsumeApplyRequest() )
                continue;

            callbackState->CancelDeferredApply();
            callbackState->ClearApplyBudgetWarning();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ProgressRing model", e);
            return false;
        }
    }

    return deferLatestRevision();
}

wxSize wxActivityIndicator::DoGetBestSize() const
{
    return wxWindow::FromDIP(
        wxSize(40, 40), const_cast<wxActivityIndicator *>(this));
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIActivityIndicatorTestAccess::GetState(
    const wxActivityIndicator& activity,
    bool *peerActive,
    bool *isTabStop,
    wxString *itemStatus,
    wxWinUIAppearanceSnapshot *appearance)
{
    if ( !activity.m_winui || !activity.m_winui->ring )
        return false;

    try
    {
        if ( peerActive )
            *peerActive = activity.m_winui->ring.IsActive();
        if ( isTabStop )
            *isTabStop = activity.m_winui->ring.IsTabStop();
        if ( itemStatus )
        {
            *itemStatus = wxString(
                MUXA::AutomationProperties::GetItemStatus(
                    activity.m_winui->ring).c_str());
        }
        if ( appearance )
        {
            *appearance = wxWinUICaptureAppearance(
                activity.m_winui->ring, activity.m_winui->ring);
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxWinUIActivityIndicatorTestAccess::SetNextPeerWriteHook(
    wxActivityIndicator& activity,
    PeerWriteHook hook,
    void *context)
{
    if ( !activity.m_winui )
        return;

    activity.m_winui->nextPeerWriteHookForTesting = hook;
    activity.m_winui->nextPeerWriteContextForTesting =
        hook ? context : nullptr;
}

bool wxWinUIActivityIndicatorTestAccess::HasDeferredPeerWrite(
    const wxActivityIndicator& activity)
{
    return activity.m_winui && activity.m_winui->callbackState &&
           activity.m_winui->callbackState->HasDeferredApply();
}

bool wxWinUIActivityIndicatorTestAccess::IsPeerProjectionQuarantined(
    const wxActivityIndicator& activity)
{
    return activity.m_winui && activity.m_winui->callbackState &&
           activity.m_winui->callbackState->IsApplyQuarantined();
}

unsigned long long wxWinUIActivityIndicatorTestAccess::GetModelRevision(
    const wxActivityIndicator& activity)
{
    return activity.m_winui && activity.m_winui->callbackState
        ? activity.m_winui->callbackState->GetModelRevision()
        : 0;
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_ACTIVITYINDICATOR
