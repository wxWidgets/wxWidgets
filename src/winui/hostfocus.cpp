/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostfocus.cpp
// Purpose:     Host-owned focus arbitration and migration continuations
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "hostfocus.h"
#include "hostnative.h"
#include "visualcoordinates.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif
#include "wx/toplevel.h"
#include "wx/scopeguard.h"
#include "wx/msw/private.h"

#include <algorithm>
#include <utility>

std::map<wxWindow *,
         std::shared_ptr<wxWinUITopLevelHost::FocusMigration>>
    wxWinUITopLevelHost::ms_focusMigrations;

static wxWindow *wxWinUIResolveCurrentNativeFocusWindow()
{
    for ( HWND hwnd = ::GetFocus(); hwnd; hwnd = ::GetParent(hwnd) )
    {
        if ( wxWindow * const window = wxFindWinFromHandle(hwnd) )
            return window;
    }

    return nullptr;
}

// Is the CURRENT XAML focus inside this container's subtree?  Used as the
// stale-guard of the focus arbiter: Got/LostFocus events can arrive one
// dispatch generation late, after the focus was already moved elsewhere,
// and must not overwrite the ownership the newer move established.
bool wxWinUIFocusIsInside(
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
    const winrt::Microsoft::UI::Xaml::XamlRoot& root)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    try
    {
        if ( !container || !root )
            return false;

        const auto containerDO = container.as<MUX::DependencyObject>();
        auto node = MUX::Input::FocusManager::GetFocusedElement(root)
                        .try_as<MUX::DependencyObject>();
        while ( node )
        {
            if ( node == containerDO )
                return true;
            node = MUX::Media::VisualTreeHelper::GetParent(node);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    return false;
}

void wxWinUITopLevelHost::SetPreparedFocusReparentNativeBoundary(
    unsigned long long token,
    bool active)
{
    if ( !token )
        return;

    for ( const auto& pending : ms_focusMigrations )
    {
        const std::shared_ptr<FocusMigration>& migration =
            pending.second;
        if ( migration && !migration->committed &&
             migration->preparedReparentId == token )
        {
            migration->preparedNativeReparentBoundaryActive =
                active;
            return;
        }
    }
}

void wxWinUITopLevelHost::CancelPreparedFocusReparent(
    unsigned long long token)
{
    if ( !token )
        return;

    std::shared_ptr<FocusMigration> migration;
    for ( const auto& pending : ms_focusMigrations )
    {
        if ( pending.second &&
             !pending.second->committed &&
             pending.second->preparedReparentId == token )
        {
            migration = pending.second;
            break;
        }
    }
    if ( !migration )
        return;

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWindow * const window = migration->window.get();
    wxWinUISlot * const slot =
        source && window ? source->FindSlot(window) : nullptr;

    // Consume the caller's lease before any focus call. A nested reparent
    // can now either carry this transport ticket forward or create a new
    // preparation; the stale outer scope guard cannot match either.
    migration->preparedNativeReparentBoundaryActive = false;
    migration->preparedReparentId = 0;

    const bool exactSource =
        source && !source->m_shuttingDown &&
        source->m_focusMigration == migration &&
        window && !window->IsBeingDeleted() &&
        wxGetTopLevelParent(window) == source->m_tlw &&
        slot && slot->m_lifetime == migration->sourceSlot &&
        slot->m_contentGeneration ==
            migration->sourceContentGeneration &&
        migration->sourceSlot->GetHost() == source &&
        migration->sourceSlot->GetWindow() == window &&
        source->m_focusOwner == window &&
        source->m_focusIntentGeneration ==
            migration->sourceFocusIntentGeneration &&
        window->GetHWND() == migration->hwnd &&
        wxWinUIMSWGetHwndGeneration(
            window, migration->hwnd) ==
            migration->hwndGeneration &&
        migration->shellFocusConsumed &&
        !migration->shellFocusParking &&
        ::GetFocus() == static_cast<HWND>(migration->hwnd);
    if ( !exactSource )
    {
        if ( source )
            source->AbortFocusMigration(migration, true);
        return;
    }

    migration->destinationHost = migration->sourceHost;
    migration->destinationSlot = migration->sourceSlot;
    migration->destinationContentGeneration =
        migration->sourceContentGeneration;
    source->FocusSlotImpl(
        window,
        FocusOrigin::MigrationContinuation,
        migration->previous.get(),
        migration->sourceFocusIntentGeneration,
        true);

    if ( !migration->committed )
    {
        const FocusRequest& deferred =
            source->m_deferredFocusRequest;
        const bool exactDeferred =
            source->m_focusMigration == migration &&
            deferred.origin == FocusOrigin::MigrationContinuation &&
            deferred.target == migration->sourceSlot &&
            deferred.intentGeneration ==
                source->m_focusIntentGeneration &&
            source->m_deferredFocusContentGeneration ==
                migration->sourceContentGeneration;
        if ( !exactDeferred )
            source->AbortFocusMigration(migration, true);
    }
}

void wxWinUITopLevelHost::FocusSlot(wxWindow *window)
{
    wxWindow *previous =
        ResolveFocusHwnd(reinterpret_cast<WXHWND>(::GetFocus()));
    if ( !previous )
        previous = wxWinUIResolveCurrentNativeFocusWindow();
    FocusSlotImpl(window, FocusOrigin::LogicalRequest, previous);
}

void wxWinUITopLevelHost::HandleNativeShellFocus(wxWindow *window,
                                                  wxWindow *previous)
{
    if ( m_shuttingDown || !window || !FindSlot(window) )
        return;

    FocusSlotImpl(window, FocusOrigin::WxSetAlreadyDelivered, previous);
}

bool wxWinUITopLevelHost::ConsumeNativeFocusRollback(wxWindow *window)
{
    if ( !m_nativeFocusRollbackEpoch )
        return false;

    const auto rollback = m_nativeFocusRollbackTarget.lock();
    wxWinUISlot * const slot = FindSlot(window);
    const HWND shell = GetHwndOf(window);
    const bool consume =
        !m_shuttingDown && window && slot && rollback &&
        slot->m_lifetime == rollback &&
        rollback->GetHost() == this &&
        rollback->GetWindow() == window &&
        shell && ::GetFocus() == shell &&
        reinterpret_cast<WXHWND>(shell) ==
            m_nativeFocusRollbackShell &&
        wxWinUIMSWGetHwndGeneration(
            window, reinterpret_cast<WXHWND>(shell)) ==
            m_nativeFocusRollbackShellGeneration;

    // Every incoming shell focus supersedes this one-shot marker. A mismatch
    // is not allowed to survive and consume a later recycled HWND.
    m_nativeFocusRollbackTarget.reset();
    m_nativeFocusRollbackShell = nullptr;
    m_nativeFocusRollbackShellGeneration = 0;
    m_nativeFocusRollbackEpoch = 0;
    return consume;
}

bool wxWinUITopLevelHost::ConsumeMigrationShellFocus(wxWindow *window)
{
    const auto pending = ms_focusMigrations.find(window);
    const std::shared_ptr<FocusMigration> migration =
        pending != ms_focusMigrations.end() ? pending->second : nullptr;
    const HWND shell = window ? GetHwndOf(window) : nullptr;
    if ( !migration || migration->committed ||
         migration->sourceHost != m_hostLifetime ||
         m_focusMigration != migration ||
         !migration->shellFocusParking ||
         migration->shellFocusConsumed ||
         migration->window.get() != window ||
         migration->sourceSlot == nullptr ||
         migration->sourceSlot->GetHost() != this ||
         migration->sourceSlot->GetWindow() != window ||
         FindSlot(window) == nullptr ||
         FindSlot(window)->m_lifetime != migration->sourceSlot ||
         shell != static_cast<HWND>(migration->hwnd) ||
         ::GetFocus() != shell ||
         wxWinUIMSWGetHwndGeneration(
             window, reinterpret_cast<WXHWND>(shell)) !=
             migration->hwndGeneration )
    {
        return false;
    }

    migration->shellFocusConsumed = true;
    migration->shellFocusParking = false;
    return true;
}

bool wxWinUITopLevelHost::HasFocusMigrationNativeAuthority(
    const std::shared_ptr<FocusMigration>& migration,
    wxWinUITopLevelHost *destination)
{
    if ( !migration || migration->committed )
        return false;

    wxWindow * const window = migration->window.get();
    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    if ( source &&
         source->m_focusMigration == migration &&
         source->m_focusOwner == window &&
         source->HasNativeFocusAuthority() )
    {
        return true;
    }

    if ( destination &&
         migration->destinationHost == destination->m_hostLifetime &&
         destination->HasNativeFocusAuthority() &&
         (!destination->m_focusOwner ||
          destination->m_focusOwner == window) )
    {
        return true;
    }

    const HWND expected =
        static_cast<HWND>(migration->transportNativeAuthority);
    return expected &&
           migration->transportNativeAuthorityGeneration &&
           ::GetFocus() == expected &&
           wxWinUIMSWGetNativeHwndGeneration(
               reinterpret_cast<WXHWND>(expected)) ==
               migration->transportNativeAuthorityGeneration;
}

bool wxWinUITopLevelHost::NoteMigrationShellFocusDeparture(
    wxWindow *window,
    WXHWND destination)
{
    const auto pending = ms_focusMigrations.find(window);
    const std::shared_ptr<FocusMigration> migration =
        pending != ms_focusMigrations.end() ? pending->second : nullptr;
    if ( !window || !migration || migration->committed ||
         !migration->logicalFocusWasActive ||
         !migration->shellFocusConsumed ||
         migration->shellFocusParking ||
         migration->window.get() != window ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         ::GetFocus() == static_cast<HWND>(migration->hwnd) )
    {
        return false;
    }

    // SetParent() may synchronously activate the exact destination TLW before
    // MigrateSlotToCurrentTLW() has had a chance to create/find its island.
    // This is still the same implementation transport, not a user focus
    // decision. Carry its generation-bound native authority forward and
    // suppress the shell's public KILL_FOCUS; an arbitrary HWND outside this
    // prepared TLW still takes the ordinary terminal-abort path below.
    if ( migration->preparedReparentId &&
         migration->preparedNativeReparentBoundaryActive &&
         destination )
    {
        wxWindow * const sourceTLW =
            migration->preparedSourceTLW.get();
        wxWindow * const destinationTLW =
            migration->preparedDestinationTLW.get();
        const HWND sourceTLWHwnd =
            static_cast<HWND>(
                migration->preparedSourceTLWHwnd);
        const HWND destinationTLWHwnd =
            static_cast<HWND>(
                migration->preparedDestinationTLWHwnd);
        const HWND destinationHwnd =
            static_cast<HWND>(destination);
        const unsigned long long destinationGeneration =
            wxWinUIMSWGetNativeHwndGeneration(destination);
        const auto belongsToPreparedTLW =
            [destinationHwnd](
                wxWindow *tlw,
                WXHWND expectedHwnd,
                unsigned long long expectedGeneration)
            {
                const HWND hwnd = static_cast<HWND>(expectedHwnd);
                return tlw && !tlw->IsBeingDeleted() &&
                       tlw->GetHWND() == expectedHwnd &&
                       wxWinUIMSWGetHwndGeneration(
                           tlw, expectedHwnd) ==
                           expectedGeneration &&
                       hwnd && destinationHwnd &&
                       (destinationHwnd == hwnd ||
                        ::IsChild(hwnd, destinationHwnd));
            };
        const bool belongsToSource =
            belongsToPreparedTLW(
                sourceTLW,
                migration->preparedSourceTLWHwnd,
                migration->preparedSourceTLWHwndGeneration);
        const bool belongsToDestination =
            belongsToPreparedTLW(
                destinationTLW,
                migration->preparedDestinationTLWHwnd,
                migration->
                    preparedDestinationTLWHwndGeneration);
        if ( wxWinUIInputLogEnabled() )
        {
            wxWinUIInputLog(
                "FocusMigration SetParent departure token=%llu "
                "focus=%p root=%p source=%p target=%p "
                "sourceMatch=%d targetMatch=%d targetLive=%d "
                "targetGeneration=%llu/%llu",
                migration->preparedReparentId,
                static_cast<void *>(destinationHwnd),
                static_cast<void *>(
                    destinationHwnd
                        ? ::GetAncestor(destinationHwnd, GA_ROOT)
                        : nullptr),
                static_cast<void *>(sourceTLWHwnd),
                static_cast<void *>(destinationTLWHwnd),
                belongsToSource,
                belongsToDestination,
                destinationTLW &&
                    !destinationTLW->IsBeingDeleted() &&
                    destinationTLW->GetHWND() ==
                        migration->preparedDestinationTLWHwnd,
                destinationTLW
                    ? wxWinUIMSWGetHwndGeneration(
                          destinationTLW,
                          migration->preparedDestinationTLWHwnd)
                    : 0,
                migration->preparedDestinationTLWHwndGeneration);
        }
        if ( (belongsToSource || belongsToDestination) &&
             destinationGeneration &&
             ::GetFocus() == destinationHwnd )
        {
            migration->transportNativeAuthority = destination;
            migration->transportNativeAuthorityGeneration =
                destinationGeneration;
            if ( wxWinUIInputLogEnabled() )
            {
                wxWinUIInputLog(
                    "FocusMigration carried SetParent authority "
                    "token=%llu focus=%p target=%p",
                    migration->preparedReparentId,
                    static_cast<void *>(destinationHwnd),
                    static_cast<void *>(destinationTLWHwnd));
            }
            return true;
        }
    }

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const destinationHost =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const arbiter =
        source ? source : destinationHost;
    if ( !arbiter ||
         (source && source->m_focusMigration != migration) )
    {
        return false;
    }

    // HandleKillFocus() is about to publish the one real wx KILL_FOCUS for
    // this shell departure. Retire all migration state without dispatching a
    // second event; AbortFocusMigration() clears the old logical owner when
    // this marker is present.
    migration->shellLogicalLossDelivered = true;
    arbiter->AbortFocusMigration(migration, false);
    return false;
}

void wxWinUITopLevelHost::ClearDeferredFocusRequest(
    unsigned long long expectedGeneration)
{
    if ( expectedGeneration &&
         m_deferredFocusRequest.intentGeneration != expectedGeneration )
    {
        if ( m_deferredFocusFlushRetryGeneration == expectedGeneration )
            m_deferredFocusFlushRetryGeneration = 0;
        return;
    }

    const FocusRequest retired = m_deferredFocusRequest;
    m_deferredFocusRequest = FocusRequest();
    m_deferredFocusContentGeneration = 0;
    if ( !expectedGeneration ||
         m_deferredFocusFlushRetryGeneration == expectedGeneration )
    {
        m_deferredFocusFlushRetryGeneration = 0;
    }

    const std::shared_ptr<FocusMigration> migration =
        m_focusMigration;
    if ( migration && !migration->committed &&
         retired.origin == FocusOrigin::MigrationContinuation &&
         retired.target &&
         retired.target == migration->destinationSlot )
    {
        if ( wxWinUIInputLogEnabled() )
        {
            wxWinUIInputLog(
                "FocusMigration deferred continuation cleared "
                "expected=%llu retired=%llu current=%llu",
                expectedGeneration,
                retired.intentGeneration,
                m_focusIntentGeneration);
        }
        AbortFocusMigration(migration, true);
    }
}

bool wxWinUITopLevelHost::IsSuppressedMigrationLoss(
    const std::shared_ptr<wxWinUISlotLifetime>& source)
{
    const std::shared_ptr<FocusMigration> migration =
        m_focusMigration;
    if ( !migration || migration->committed ||
         !migration->logicalFocusWasActive ||
         migration->sourceHost != m_hostLifetime ||
         migration->sourceSlot != source ||
         !source || source->GetHost() != this ||
         m_focusOwner != source->GetWindow() )
    {
        return false;
    }

    wxWindow * const window = migration->window.get();
    if ( !window || window != source->GetWindow() ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        AbortFocusMigration(migration, false);
        return false;
    }

    // A real navigation to another source slot wins over the migration
    // suspension. During the expected detach/bridge handoff native authority
    // belongs either to this host or to the ticketed destination host.
    if ( wxWindow * const focused = FindFocusedSlot() )
    {
        if ( focused != window )
        {
            AbortFocusMigration(migration, false);
            return false;
        }
    }

    wxWinUITopLevelHost * const destination =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    const bool ownsTransportAuthority =
        (migration->shellFocusParking ||
         migration->shellFocusConsumed) &&
        HasFocusMigrationNativeAuthority(
            migration, destination);
    if ( !HasNativeFocusAuthority() &&
         !ownsTransportAuthority &&
         (!destination || !destination->HasNativeFocusAuthority()) )
    {
        AbortFocusMigration(migration, false);
        return false;
    }

    migration->sourceLostFocusObserved = true;
    return true;
}

bool wxWinUITopLevelHost::CommitFocusMigration(
    const std::shared_ptr<FocusMigration>& migration,
    const std::shared_ptr<wxWinUISlotLifetime>& destination)
{
    if ( !migration || migration->committed ||
         m_focusMigration != migration ||
         migration->destinationHost != m_hostLifetime ||
         migration->destinationSlot != destination ||
         !destination || destination->GetHost() != this ||
         !HasNativeFocusAuthority() )
    {
        return false;
    }

    wxWindow *window = migration->window.get();
    wxWinUISlot *destinationSlot =
        window ? FindSlot(window) : nullptr;
    if ( !window || destination->GetWindow() != window ||
         !destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration )
    {
        return false;
    }

    const auto destinationContainer = destinationSlot->m_container;
    const auto xamlRoot = GetXamlRoot();
    if ( !wxWinUIFocusIsInside(destinationContainer, xamlRoot) )
        return false;

    // FocusManager/XamlRoot reads are arbitrary callback boundaries. Reacquire
    // every weak identity and generation before publishing either host owner;
    // the focused visual may have been destroyed, replaced or moved again.
    window = migration->window.get();
    destinationSlot = window ? FindSlot(window) : nullptr;
    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    const auto pendingTicket = window
        ? ms_focusMigrations.find(window)
        : ms_focusMigrations.end();
    if ( migration->committed ||
         m_focusMigration != migration ||
         migration->destinationHost != m_hostLifetime ||
         migration->destinationSlot != destination ||
         !destination || destination->GetHost() != this ||
         !window || destination->GetWindow() != window ||
         !destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_container != destinationContainer ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         window->GetHWND() != migration->hwnd ||
         wxWinUIMSWGetHwndGeneration(
             window, migration->hwnd) !=
             migration->hwndGeneration ||
         pendingTicket == ms_focusMigrations.end() ||
         pendingTicket->second != migration ||
         !HasNativeFocusAuthority() ||
         !source ||
         source->m_focusMigration != migration ||
         source->m_focusOwner != window ||
         source->m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        return false;
    }

    OperationGuard sourceOperation(source);
    const auto pendingAfterGuard =
        ms_focusMigrations.find(window);
    if ( migration->sourceHost->GetHost() != source ||
         migration->window.get() != window ||
         m_focusMigration != migration ||
         migration->destinationSlot != destination ||
         destination->GetHost() != this ||
         destination->GetWindow() != window ||
         FindSlot(window) != destinationSlot ||
         destinationSlot->m_lifetime != destination ||
         destinationSlot->m_container != destinationContainer ||
         destinationSlot->m_contentGeneration !=
             migration->destinationContentGeneration ||
         pendingAfterGuard == ms_focusMigrations.end() ||
         pendingAfterGuard->second != migration ||
         !HasNativeFocusAuthority() ||
         source->m_focusMigration != migration ||
         source->m_focusOwner != window ||
         source->m_focusIntentGeneration !=
             migration->sourceFocusIntentGeneration )
    {
        return false;
    }

    // A rollback restores the original carrier and resumes focus through the
    // same continuation path. Its source and destination are deliberately
    // identical: no logical ownership changes and therefore no wx focus
    // event is published.
    if ( source == this )
    {
        if ( destination != migration->sourceSlot )
            return false;

        migration->committedSuccessfully = true;
        migration->committed = true;
        m_focusMigration.reset();
        const auto rollbackPending =
            ms_focusMigrations.find(window);
        if ( rollbackPending != ms_focusMigrations.end() &&
             rollbackPending->second == migration )
        {
            ms_focusMigrations.erase(rollbackPending);
        }
        return true;
    }

    // Commit both logical publications before any cleanup can re-enter focus.
    migration->committedSuccessfully = true;
    migration->committed = true;
    source->m_focusOwner = nullptr;
    m_focusOwner = window;
    source->m_focusMigration.reset();
    m_focusMigration.reset();
    const auto committedPending =
        ms_focusMigrations.find(window);
    if ( committedPending != ms_focusMigrations.end() &&
         committedPending->second == migration )
    {
        ms_focusMigrations.erase(committedPending);
    }
    source->InvalidateFocusIntent(
        migration->sourceFocusIntentGeneration);
    return true;
}

void wxWinUITopLevelHost::AbortFocusMigration(
    const std::shared_ptr<FocusMigration>& migration,
    bool dispatchLogicalLoss,
    wxWindow *replacement)
{
    if ( !migration || migration->committed )
        return;

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog(
            "FocusMigration abort dispatch=%d replacement=%p "
            "shellLoss=%d focus=%p shell=%p",
            dispatchLogicalLoss,
            replacement,
            migration->shellLogicalLossDelivered,
            static_cast<void *>(::GetFocus()),
            static_cast<void *>(static_cast<HWND>(migration->hwnd)));
    }

    wxWinUITopLevelHost * const source =
        migration->sourceHost
            ? migration->sourceHost->GetHost()
            : nullptr;
    wxWinUITopLevelHost * const destination =
        migration->destinationHost
            ? migration->destinationHost->GetHost()
            : nullptr;
    wxWindow * const window = migration->window.get();
    wxWindow * const windowIdentity = migration->windowIdentity;

    const bool sourceOwnsTicket =
        source && source->m_focusMigration == migration;
    const bool sourceWasLogicalOwner =
        sourceOwnsTicket && windowIdentity &&
        source->m_focusOwner == windowIdentity &&
        source->m_focusIntentGeneration ==
            migration->sourceFocusIntentGeneration;
    const HWND shell = static_cast<HWND>(migration->hwnd);
    const bool exactShellOwnsNativeFocus =
        dispatchLogicalLoss && !replacement &&
        !migration->shellLogicalLossDelivered &&
        sourceWasLogicalOwner && window && shell &&
        ::GetFocus() == shell &&
        window->GetHWND() == migration->hwnd &&
        wxWinUIMSWGetHwndGeneration(
            window, migration->hwnd) ==
            migration->hwndGeneration;
    const bool migrationIslandOwnsNativeFocus =
        dispatchLogicalLoss && !replacement &&
        !migration->shellLogicalLossDelivered &&
        sourceWasLogicalOwner && !exactShellOwnsNativeFocus &&
        ((destination && destination->HasNativeFocusAuthority() &&
          (!destination->m_focusOwner ||
           destination->m_focusOwner == window)) ||
         (source && source->HasNativeFocusAuthority() &&
          (!source->m_focusOwner ||
           source->m_focusOwner == window)));

    migration->committed = true;
    if ( sourceOwnsTicket )
        source->m_focusMigration.reset();
    if ( destination &&
         destination->m_focusMigration == migration )
    {
        destination->m_focusMigration.reset();
    }

    // The weak wxWindow can already be dead after a synchronous Destroy().
    // Remove by ticket identity, not by the map key derived from that weak
    // reference, or the ticket would retain both host and slot lifetimes.
    for ( auto pending = ms_focusMigrations.begin();
          pending != ms_focusMigrations.end(); )
    {
        if ( pending->second == migration )
            pending = ms_focusMigrations.erase(pending);
        else
            ++pending;
    }

    if ( migration->shellLogicalLossDelivered )
    {
        // HandleKillFocus() owns the event currently in flight. Publish the
        // host state it must observe, without synthesizing a duplicate KILL.
        if ( sourceWasLogicalOwner &&
             source->m_focusOwner == windowIdentity )
        {
            source->m_focusOwner = nullptr;
            source->InvalidateFocusIntent();
        }
        return;
    }

    if ( exactShellOwnsNativeFocus )
    {
        // Let the real shell WM_KILLFOCUS publish the sole wx event. The
        // ticket is already terminal so the hook cannot recurse into us.
        // Publish the truthful null owner before the event, matching
        // TransitionFocus()'s contract for FindFocus() in KILL handlers.
        if ( source->m_focusOwner == window )
        {
            source->m_focusOwner = nullptr;
            source->InvalidateFocusIntent();
        }
        ::SetFocus(nullptr);
        wxWinUITopLevelHost * const liveSource =
            migration->sourceHost
                ? migration->sourceHost->GetHost()
                : nullptr;
        wxWindow * const liveWindow = migration->window.get();
        if ( ::GetFocus() == shell && liveSource && liveWindow &&
             !liveSource->m_focusOwner )
        {
            // USER32 refused the relinquish and no KILL was sent. Keep native
            // and logical truth aligned; a later explicit request can start a
            // fresh transaction.
            liveSource->m_focusOwner = liveWindow;
        }
        return;
    }

    if ( !window && sourceWasLogicalOwner &&
         migration->sourceHost &&
         migration->sourceHost->GetHost() == source &&
         source->m_focusOwner == windowIdentity )
    {
        // No wx event may target a destroyed object, but the host must still
        // release the exact raw identity it suspended for this ticket.
        source->m_focusOwner = nullptr;
        source->InvalidateFocusIntent();
        return;
    }

    if ( migrationIslandOwnsNativeFocus )
    {
        // A failed continuation must not leave the ownerless bridge focused:
        // FindFocus()/Tab would otherwise report the migrated shell after its
        // logical KILL. Bridge/InputSite HWNDs have no wxWindowMSW handler, so
        // the logical event is still emitted below.
        ::SetFocus(nullptr);
    }

    if ( dispatchLogicalLoss && sourceWasLogicalOwner &&
         migration->sourceHost &&
         migration->sourceHost->GetHost() == source &&
         migration->window.get() == window &&
         source->m_focusOwner == window )
    {
        source->TransitionFocus(
            nullptr,
            window,
            false,
            replacement
                ? replacement
                : wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::NotifySlotContentLoaded(wxWindow *window)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    NotifySlotContentLoaded(
        window, slot->m_content, slot->m_contentGeneration);
}

void wxWinUITopLevelHost::NotifySlotContentLoaded(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& expectedContent,
    unsigned long long expectedGeneration)
{
    OperationGuard operation(this);

    wxWinUISlot * const loadedSlot = FindSlot(window);
    if ( !loadedSlot ||
         loadedSlot->m_content != expectedContent ||
         (expectedGeneration &&
          loadedSlot->m_contentGeneration != expectedGeneration) )
    {
        return;
    }

    // A callback for model A can run after the carrier was detached or even
    // after B was attached, while SetContent() deliberately still publishes
    // A. Record the exact transaction/generation only. Commit discards this
    // latch; rollback may apply it to the restored A generation.
    if ( loadedSlot->m_contentTransactionInProgress )
    {
        loadedSlot->m_contentLoadedDuringTransaction = true;
        loadedSlot->m_contentLoadedTransactionEpoch =
            loadedSlot->m_contentTransactionEpoch;
        loadedSlot->m_contentLoadedTransactionGeneration =
            loadedSlot->m_contentGeneration;
        return;
    }

    loadedSlot->m_contentLoaded = true;

    // UpdateLayout() in an already-running bounded retry may synchronously
    // raise Loaded. The latch above is all that nested callback must do; the
    // outer retry will revalidate and finish the decision.
    if ( m_deferredFocusRetryInProgress )
        return;

    const FocusRequest retry = m_deferredFocusRequest;
    if ( m_shuttingDown || !retry.target ||
         retry.intentGeneration != m_focusIntentGeneration )
    {
        ClearDeferredFocusRequest();
        return;
    }

    // A different slot loading must not consume the pending target.
    if ( retry.target->GetHost() != this ||
         retry.target->GetWindow() != window )
    {
        return;
    }

    if ( loadedSlot->m_lifetime != retry.target ||
         loadedSlot->m_contentGeneration != m_deferredFocusContentGeneration )
    {
        ClearDeferredFocusRequest(retry.intentGeneration);
        return;
    }

    // Loaded before the coalesced flush is only a latch: the unique flush
    // retry is still the next opportunity. This also prevents repeated
    // unload/reload notifications from multiplying attempts for one intent.
    // Loaded performs its one final retry only when the flush edge has
    // already been consumed.
    if ( m_deferredFocusFlushRetryGeneration == retry.intentGeneration )
        return;

    // Focus() is a synchronous arbitrary-callback boundary. Keep the exact
    // request until success and suppress a nested Loaded retry while this one
    // is on the stack.
    m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryGuard = wxMakeGuard([this]()
    {
        m_deferredFocusRetryInProgress = false;
    });
    wxUnusedVar(retryGuard);
    FocusSlotImpl(window, retry.origin, retry.previous.get(),
                  retry.intentGeneration, false);

    // If the flush retry was already consumed, this Loaded callback was the
    // final bounded opportunity. Release the retained slot identity on
    // failure; a future explicit focus request starts a fresh generation.
    if ( m_deferredFocusRequest.intentGeneration == retry.intentGeneration &&
         m_deferredFocusFlushRetryGeneration != retry.intentGeneration )
    {
        ClearDeferredFocusRequest(retry.intentGeneration);
    }
}

void wxWinUITopLevelHost::RetryDeferredFocusAfterFlush()
{
    const unsigned long long intentGeneration =
        m_deferredFocusFlushRetryGeneration;
    if ( !intentGeneration )
        return;

    if ( wxWinUIInputLogEnabled() &&
         m_deferredFocusRequest.origin ==
             FocusOrigin::MigrationContinuation )
    {
        wxWinUIInputLog(
            "FocusMigration retry after flush intent=%llu "
            "request=%llu content=%llu",
            intentGeneration,
            m_deferredFocusRequest.intentGeneration,
            m_deferredFocusContentGeneration);
    }

    // Consume before the callback boundary. This pass is unique even if
    // Focus()/UpdateLayout() pumps another host flush.
    m_deferredFocusFlushRetryGeneration = 0;

    const FocusRequest retry = m_deferredFocusRequest;
    if ( m_shuttingDown || !retry.target ||
         retry.intentGeneration != intentGeneration ||
         intentGeneration != m_focusIntentGeneration )
    {
        ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    wxWindow * const window = retry.target->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( retry.target->GetHost() != this || !slot ||
         slot->m_lifetime != retry.target ||
         slot->m_contentGeneration != m_deferredFocusContentGeneration )
    {
        ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryGuard = wxMakeGuard([this]()
    {
        m_deferredFocusRetryInProgress = false;
    });
    wxUnusedVar(retryGuard);
    FocusSlotImpl(window, retry.origin, retry.previous.get(),
                  intentGeneration, false);

    // A Loaded notification seen before or during this flush means there is
    // no later lifecycle edge to wait for. If focus still failed, terminate
    // the bounded retry now. Otherwise FocusSlotImpl() already cleared it.
    if ( m_deferredFocusRequest.intentGeneration == intentGeneration )
    {
        wxWinUISlot * const current = FindSlot(window);
        if ( current && current->m_lifetime == retry.target &&
             current->m_contentGeneration ==
                 m_deferredFocusContentGeneration &&
             current->m_contentLoaded )
        {
            ClearDeferredFocusRequest(intentGeneration);
        }
    }
}

void wxWinUITopLevelHost::FocusSlotImpl(wxWindow *window,
                                        FocusOrigin origin,
                                        wxWindow *previous,
                                        unsigned long long intentGeneration,
                                        bool allowDeferredRetry)
{
    OperationGuard operation(this);

    // Shutdown publishes the host boundary before restoring any XAML-owned
    // properties. Those restorations can run application code, which must not
    // re-enter virtual focus predicates on a window/TLW being destroyed.
    if ( m_shuttingDown || !window )
        return;

    const wxWeakRef<wxWindow> targetAtEntry(window);
    if ( m_focusMigration && !m_focusMigration->committed &&
         origin != FocusOrigin::MigrationContinuation )
    {
        const std::shared_ptr<FocusMigration> incumbent =
            m_focusMigration;
        if ( incumbent->window.get() == window )
        {
            // The same logical control is already crossing hosts. Its exact
            // continuation owns the single native transfer; another public
            // SetFocus() is satisfied by that pending operation.
            return;
        }

        AbortFocusMigration(incumbent, true);
        window = targetAtEntry.get();
        if ( m_shuttingDown || !window )
            return;
    }

    const std::shared_ptr<FocusMigration> expectedMigration =
        origin == FocusOrigin::MigrationContinuation
            ? m_focusMigration
            : nullptr;
    if ( origin == FocusOrigin::MigrationContinuation &&
         (!expectedMigration || expectedMigration->committed ||
          expectedMigration->window.get() != window ||
          expectedMigration->destinationHost != m_hostLifetime) )
    {
        return;
    }
    wxScopeGuard migrationContinuationGuard = wxMakeGuard(
        [this, expectedMigration]()
        {
            if ( !expectedMigration || expectedMigration->committed )
                return;

            wxWindow * const target =
                expectedMigration->window.get();
            wxWinUITopLevelHost * const source =
                expectedMigration->sourceHost
                    ? expectedMigration->sourceHost->GetHost()
                    : nullptr;
            const auto pending =
                target ? ms_focusMigrations.find(target)
                       : ms_focusMigrations.end();
            const FocusRequest& deferred =
                m_deferredFocusRequest;
            const bool exactDeferredContinuation =
                target && source &&
                m_focusMigration == expectedMigration &&
                source->m_focusMigration == expectedMigration &&
                source->m_focusOwner == target &&
                source->m_focusIntentGeneration ==
                    expectedMigration->sourceFocusIntentGeneration &&
                expectedMigration->destinationHost == m_hostLifetime &&
                expectedMigration->destinationSlot &&
                expectedMigration->destinationSlot->GetHost() == this &&
                expectedMigration->destinationSlot->GetWindow() == target &&
                deferred.origin == FocusOrigin::MigrationContinuation &&
                deferred.target == expectedMigration->destinationSlot &&
                deferred.intentGeneration == m_focusIntentGeneration &&
                m_deferredFocusContentGeneration ==
                    expectedMigration->destinationContentGeneration &&
                pending != ms_focusMigrations.end() &&
                pending->second == expectedMigration &&
                target->GetHWND() == expectedMigration->hwnd &&
                wxWinUIMSWGetHwndGeneration(
                    target, expectedMigration->hwnd) ==
                    expectedMigration->hwndGeneration &&
                expectedMigration->shellFocusConsumed &&
                !expectedMigration->shellFocusParking &&
                HasFocusMigrationNativeAuthority(
                    expectedMigration, this);
            if ( !exactDeferredContinuation )
            {
                if ( wxWinUIInputLogEnabled() )
                {
                    wxWinUIInputLog(
                        "FocusMigration continuation guard abort "
                        "target=%p source=%p local=%d sourceTicket=%d "
                        "sourceOwner=%d sourceIntent=%llu/%llu "
                        "destination=%d slot=%d deferred=%d "
                        "deferredIntent=%llu/%llu generation=%llu/%llu "
                        "pending=%d shell=%d focus=%p expected=%p",
                        target,
                        source,
                        m_focusMigration == expectedMigration,
                        source &&
                            source->m_focusMigration == expectedMigration,
                        source && source->m_focusOwner == target,
                        source ? source->m_focusIntentGeneration : 0,
                        expectedMigration->sourceFocusIntentGeneration,
                        expectedMigration->destinationHost ==
                            m_hostLifetime,
                        expectedMigration->destinationSlot &&
                            expectedMigration->destinationSlot->GetHost() ==
                                this,
                        deferred.origin ==
                                FocusOrigin::MigrationContinuation &&
                            deferred.target ==
                                expectedMigration->destinationSlot,
                        deferred.intentGeneration,
                        m_focusIntentGeneration,
                        m_deferredFocusContentGeneration,
                        expectedMigration->destinationContentGeneration,
                        pending != ms_focusMigrations.end() &&
                            pending->second == expectedMigration,
                        expectedMigration->shellFocusConsumed &&
                            !expectedMigration->shellFocusParking,
                        static_cast<void *>(::GetFocus()),
                        static_cast<void *>(
                            static_cast<HWND>(expectedMigration->hwnd)));
                }
                AbortFocusMigration(expectedMigration, true);
            }
        });
    wxUnusedVar(migrationContinuationGuard);

    // The target's virtual focus predicates and every native/XAML focus call
    // below may synchronously destroy the previous owner. Convert the raw
    // entry hint before crossing the first such boundary.
    const wxWeakRef<wxWindow> previousWeak(previous);

    if ( !intentGeneration )
    {
        if ( ++m_focusIntentGeneration == 0 )
            ++m_focusIntentGeneration;
        intentGeneration = m_focusIntentGeneration;
        ClearDeferredFocusRequest();
    }
    else if ( intentGeneration != m_focusIntentGeneration )
    {
        return;
    }

    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    // Virtual focus predicates and labels are application re-entrancy
    // boundaries too. Snapshot the exact slot before the first one and reject
    // the outer intent after every call: a nested SetFocus() owns its newer
    // generation and its deferred retry must never be overwritten by this
    // older stack frame.
    const std::shared_ptr<wxWinUISlotLifetime> lifetime = slot->m_lifetime;
    const unsigned long long contentGeneration = slot->m_contentGeneration;
    const wxWeakRef<wxWindow> targetWeak(window);
    bool bridgeAcquiredForAttempt = false;
    const auto migrationAuthorityIsCurrent = [&]()
    {
        if ( !expectedMigration )
            return true;
        if ( expectedMigration->committed )
            return expectedMigration->committedSuccessfully;

        wxWindow * const target = targetWeak.get();
        wxWinUITopLevelHost * const source =
            expectedMigration->sourceHost
                ? expectedMigration->sourceHost->GetHost()
                : nullptr;
        const auto pending =
            target ? ms_focusMigrations.find(target)
                   : ms_focusMigrations.end();
        if ( !target || !source ||
             m_focusMigration != expectedMigration ||
             source->m_focusMigration != expectedMigration ||
             source->m_focusOwner != target ||
             source->m_focusIntentGeneration !=
                 expectedMigration->sourceFocusIntentGeneration ||
             expectedMigration->destinationHost != m_hostLifetime ||
             expectedMigration->destinationSlot != lifetime ||
             expectedMigration->destinationContentGeneration !=
                 contentGeneration ||
             pending == ms_focusMigrations.end() ||
             pending->second != expectedMigration ||
             target->GetHWND() != expectedMigration->hwnd ||
             wxWinUIMSWGetHwndGeneration(
                 target, expectedMigration->hwnd) !=
                 expectedMigration->hwndGeneration )
        {
            return false;
        }

        if ( bridgeAcquiredForAttempt )
            return HasNativeFocusAuthority();

        return expectedMigration->shellFocusConsumed &&
               !expectedMigration->shellFocusParking &&
               HasFocusMigrationNativeAuthority(
                   expectedMigration, this);
    };
    const auto getInitialLiveSlot = [&]() -> wxWinUISlot *
    {
        if ( intentGeneration != m_focusIntentGeneration ||
             !lifetime || lifetime->GetHost() != this ||
             !migrationAuthorityIsCurrent() )
        {
            return nullptr;
        }

        wxWindow * const target = targetWeak.get();
        if ( !target || lifetime->GetWindow() != target )
            return nullptr;

        wxWinUISlot * const current = FindSlot(target);
        return current &&
                       current->m_lifetime == lifetime &&
                       current->m_contentGeneration == contentGeneration
                 ? current
                 : nullptr;
    };

    const bool canAcceptFocus = window->CanAcceptFocus();
    window = targetWeak.get();
    if ( !canAcceptFocus || !window || !getInitialLiveSlot() )
        return;

    const wxString label = window->GetLabel();
    window = targetWeak.get();
    if ( !window || !getInitialLiveSlot() )
        return;

    // ::SetFocus() is a reentrant boundary: it synchronously dispatches
    // WM_KILLFOCUS/WM_SETFOCUS, whose handlers may destroy, reparent or
    // unregister the very control being focused. Hold the invalidatable
    // lifetime across every call and re-find the slot afterwards.
    const unsigned long long requestId = ++m_nextFocusRequestId;
    FocusRequest request;
    request.id = requestId;
    request.intentGeneration = intentGeneration;
    request.target = lifetime;
    request.previous = previousWeak;
    request.origin = origin;
    const FocusRequest * const retained =
        intentGeneration == m_deferredFocusRequest.intentGeneration
            ? &m_deferredFocusRequest
            : nullptr;
    const HWND nativeAuthority =
        expectedMigration
            ? static_cast<HWND>(
                  expectedMigration->transportNativeAuthority)
            : retained
            ? static_cast<HWND>(retained->nativeAuthority)
            : ::GetFocus();
    request.nativeAuthority =
        reinterpret_cast<WXHWND>(nativeAuthority);
    request.nativeAuthorityGeneration =
        expectedMigration
            ? expectedMigration->
                  transportNativeAuthorityGeneration
            : retained
            ? retained->nativeAuthorityGeneration
            : wxWinUIMSWGetNativeHwndGeneration(
                  reinterpret_cast<WXHWND>(nativeAuthority));
    m_focusRequests.push_back(std::move(request));

    if ( allowDeferredRetry )
    {
        m_deferredFocusRequest.id = requestId;
        m_deferredFocusRequest.intentGeneration = intentGeneration;
        m_deferredFocusRequest.target = lifetime;
        m_deferredFocusRequest.previous = previousWeak;
        m_deferredFocusRequest.origin = origin;
        m_deferredFocusRequest.nativeAuthority =
            reinterpret_cast<WXHWND>(nativeAuthority);
        m_deferredFocusRequest.nativeAuthorityGeneration =
            expectedMigration
                ? expectedMigration->
                      transportNativeAuthorityGeneration
                : wxWinUIMSWGetNativeHwndGeneration(
                      reinterpret_cast<WXHWND>(nativeAuthority));
        m_deferredFocusContentGeneration = contentGeneration;
        // Publish the flush obligation together with the request, before any
        // SetFocus/Focus callback can synchronously raise Loaded. MarkDirty()
        // is delayed until the immediate attempt actually fails.
        m_deferredFocusFlushRetryGeneration = intentGeneration;
    }

    // Loaded can be raised synchronously from any focus/layout boundary. The
    // current attempt owns the decision; a nested Loaded callback only latches
    // the content state and leaves the bounded retry intact.
    const bool retryWasInProgress = m_deferredFocusRetryInProgress;
    if ( allowDeferredRetry )
        m_deferredFocusRetryInProgress = true;
    wxScopeGuard retryAttemptGuard = wxMakeGuard(
        [this, retryWasInProgress]()
        {
            m_deferredFocusRetryInProgress = retryWasInProgress;
        });
    wxUnusedVar(retryAttemptGuard);

    // Nested focus requests are legal from application focus handlers. Remove
    // exactly this request on every exit, even if a nested vector growth
    // invalidated all references or teardown changed the stack shape.
    wxScopeGuard requestGuard = wxMakeGuard([this, requestId]()
    {
        const auto it = std::find_if(
            m_focusRequests.begin(),
            m_focusRequests.end(),
            [requestId](const FocusRequest& candidate)
            {
                return candidate.id == requestId;
            });
        if ( it != m_focusRequests.end() )
            m_focusRequests.erase(it);
    });
    wxUnusedVar(requestGuard);

    const auto getLiveSlot = [&]() -> wxWinUISlot *
    {
        if ( intentGeneration != m_focusIntentGeneration ||
                 !lifetime || lifetime->GetHost() != this ||
                 lifetime->GetWindow() != window ||
                 !migrationAuthorityIsCurrent() )
            return nullptr;

        wxWinUISlot * const live = FindSlot(window);
        return live &&
                       live->m_lifetime == lifetime &&
                       live->m_contentGeneration == contentGeneration
                 ? live
                 : nullptr;
    };
    const auto abandonCurrentIntent = [&]()
    {
        // A nested focus request owns a newer generation and must never be
        // cleared by the outer request whose callback triggered it.
        if ( intentGeneration != m_focusIntentGeneration )
            return;

        InvalidateFocusIntent(intentGeneration);
        if ( m_focusOwner == window )
        {
            TransitionFocus(
                nullptr, nullptr, false,
                wxWinUIResolveCurrentNativeFocusWindow());
        }
    };

    // Only grab the native focus when it is OUTSIDE the island.  When the
    // island already holds it, it sits on the inner InputSiteWindow, and an
    // unconditional ::SetFocus(bridge) would DOWNGRADE it inner -> bridge:
    // the input site loses the focus, XAML fires stray LostFocus on the
    // currently focused element, and the programmatic Focus() below runs
    // against a focus-less input site.
    const HWND focusNow = ::GetFocus();
    const bool inIsland = HasNativeFocusAuthority();
    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("FocusSlot '%s' focusNow=%p inIsland=%d",
                        label.utf8_str().data(),
                        static_cast<void *>(focusNow), inIsland);
    }
    if ( (!allowDeferredRetry ||
          origin == FocusOrigin::WxSetAlreadyDelivered ||
          origin == FocusOrigin::MigrationContinuation) &&
         !inIsland )
    {
        FocusRequest * const active = FindFocusRequest(window);
        const HWND expectedAuthority =
            active
                ? static_cast<HWND>(active->nativeAuthority)
                : nullptr;
        const bool authorityUnchanged =
            active && focusNow == expectedAuthority &&
            wxWinUIMSWGetNativeHwndGeneration(
                reinterpret_cast<WXHWND>(focusNow)) ==
                active->nativeAuthorityGeneration;
        if ( !authorityUnchanged )
        {
            // This one-shot retry was queued while this TLW owned focus, but
            // another TLW gained it before template/layout became ready.
            // Re-activating the stale TLW here makes two hosts' retries steal
            // focus from each other forever. A later explicit focus request
            // gets a new intent generation and may legitimately activate it.
            abandonCurrentIntent();
            return;
        }
    }
    wxWinUISlot *live = getLiveSlot();
    if ( !live )
        return;

    // Strong XAML references are safe across the following layout/focus
    // boundaries; the invalidatable slot lifetime remains the authority for
    // every wx access.
    const auto container = live->m_container;
    const auto content = live->m_content;
    const auto preferred = live->m_preferredFocus.get();

    // A content swap can detach the focused peer without delivering a usable
    // LostFocus notification. Reconcile the old logical owner before even the
    // Loaded gate; keep this request alive so a new focusable template can
    // still acquire focus on the existing bounded retry.
    if ( origin != FocusOrigin::MigrationContinuation &&
         m_focusOwner == window &&
         (!HasNativeFocusAuthority() ||
          !wxWinUIFocusIsInside(container, GetXamlRoot())) )
    {
        TransitionFocus(
            nullptr, window, false,
            wxWinUIResolveCurrentNativeFocusWindow());
        if ( m_shuttingDown ||
             intentGeneration != m_focusIntentGeneration ||
             !getLiveSlot() )
        {
            return;
        }
    }

    // Until the exact content is Loaded there may be no focusable template
    // part. Retain one bounded attempt, but don't move native focus to the
    // bridge and don't publish a logical owner which XAML has never focused.
    // A native shell path has already delivered its wx events and remains the
    // truthful native focus owner; a direct host request simply completes
    // when Loaded/flush makes the peer focusable.
    if ( !live->m_contentLoaded )
    {
        if ( wxWinUIInputLogEnabled() &&
             origin == FocusOrigin::MigrationContinuation )
        {
            wxWinUIInputLog(
                "FocusMigration continuation deferred until Loaded "
                "intent=%llu content=%llu",
                intentGeneration,
                contentGeneration);
        }
        if ( allowDeferredRetry && wxTheApp )
            MarkDirty(window);
        return;
    }

    if ( !allowDeferredRetry )
    {
        // The only deferred attempt runs after the coalesced host flush.
        // Force pending template/layout work before deciding whether there is
        // any target worth transferring native focus to.
        try
        {
            container.UpdateLayout();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        if ( !getLiveSlot() )
            return;
    }

    // Never move a native HWND's focus to the bridge merely to discover that
    // this loaded subtree has no focusable element. On the immediate pass the
    // template may still settle at the coalesced flush, so retain exactly the
    // existing bounded retry; the final pass simply terminates.
    bool hasFocusableCandidate = false;
    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

        if ( preferred && wxWinUIElementIsInSubtree(preferred, content) )
        {
            if ( const auto control = preferred.try_as<MUXC::Control>() )
            {
                hasFocusableCandidate =
                    control.IsEnabled() &&
                    control.Visibility() == MUX::Visibility::Visible &&
                    control.IsTabStop();
            }
        }

        if ( !hasFocusableCandidate )
        {
            hasFocusableCandidate =
                MUX::Input::FocusManager::FindFirstFocusableElement(container)
                    .try_as<MUXC::Control>() != nullptr;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    if ( !getLiveSlot() )
        return;
    if ( !hasFocusableCandidate )
    {
        if ( allowDeferredRetry && wxTheApp )
            MarkDirty(window);
        else
            ClearDeferredFocusRequest(intentGeneration);
        return;
    }

    bool xamlFocusCommitted = false;
    wxScopeGuard nativeAuthorityRollback = wxMakeGuard(
        [&, this]()
        {
            if ( !bridgeAcquiredForAttempt || xamlFocusCommitted ||
                 m_shuttingDown ||
                 intentGeneration != m_focusIntentGeneration ||
                 !HasNativeFocusAuthority() ||
                 (m_focusOwner && m_focusOwner != window) )
            {
                return;
            }

            FocusRequest *active = FindFocusRequest(window);
            if ( !active || active->intentGeneration != intentGeneration )
                return;

            const HWND original =
                static_cast<HWND>(active->nativeAuthority);
            const bool originalIsCurrent =
                original && ::IsWindow(original) &&
                wxWinUIMSWGetNativeHwndGeneration(
                    reinterpret_cast<WXHWND>(original)) ==
                    active->nativeAuthorityGeneration;

            // This attempt made the bridge authoritative but never acquired
            // XAML focus. Restore only the exact authority it displaced. If
            // that HWND died, drop focus rather than leave an ownerless bridge
            // that FindFocus()/Tab would falsely interpret as the TLW.
            if ( originalIsCurrent )
            {
                const unsigned long long originalGeneration =
                    active->nativeAuthorityGeneration;
                m_nativeFocusDepartureSource = lifetime;
                m_nativeFocusDepartureDestination =
                    reinterpret_cast<WXHWND>(original);
                m_nativeFocusDepartureDestinationGeneration =
                    originalGeneration;

                unsigned long long rollbackEpoch = 0;
                wxWindow * const rollbackWindow = lifetime->GetWindow();
                wxWinUISlot * const rollbackSlot =
                    rollbackWindow ? FindSlot(rollbackWindow) : nullptr;
                const HWND rollbackShell =
                    rollbackWindow ? GetHwndOf(rollbackWindow) : nullptr;
                const unsigned long long rollbackShellGeneration =
                    rollbackWindow && rollbackShell
                        ? wxWinUIMSWGetHwndGeneration(
                              rollbackWindow,
                              reinterpret_cast<WXHWND>(rollbackShell))
                        : 0;
                if ( rollbackSlot &&
                     rollbackSlot->m_lifetime == lifetime &&
                     rollbackShell == original &&
                     rollbackShellGeneration )
                {
                    if ( ++m_nextNativeFocusRollbackEpoch == 0 )
                        ++m_nextNativeFocusRollbackEpoch;
                    rollbackEpoch = m_nextNativeFocusRollbackEpoch;
                    m_nativeFocusRollbackTarget = lifetime;
                    m_nativeFocusRollbackShell =
                        reinterpret_cast<WXHWND>(rollbackShell);
                    m_nativeFocusRollbackShellGeneration =
                        rollbackShellGeneration;
                    m_nativeFocusRollbackEpoch = rollbackEpoch;
                }

                ::SetFocus(original);

                // SetFocus dispatch is synchronous. If the destination's
                // normal wx focus path did not consume this departure latch,
                // it has no remaining authority (including a successful move
                // to another host's bridge). Retire only the exact latch armed
                // by this attempt; a nested transaction may have replaced it.
                if ( m_nativeFocusDepartureSource.lock() == lifetime &&
                     m_nativeFocusDepartureDestination ==
                         reinterpret_cast<WXHWND>(original) &&
                     m_nativeFocusDepartureDestinationGeneration ==
                         originalGeneration )
                {
                    m_nativeFocusDepartureSource.reset();
                    m_nativeFocusDepartureDestination = nullptr;
                    m_nativeFocusDepartureDestinationGeneration = 0;
                }

                // The nested shell WM_SETFOCUS normally consumes the latch.
                // Clear it here only when no newer nested failure superseded
                // this exact epoch.
                if ( rollbackEpoch &&
                     m_nativeFocusRollbackEpoch == rollbackEpoch )
                {
                    m_nativeFocusRollbackTarget.reset();
                    m_nativeFocusRollbackShell = nullptr;
                    m_nativeFocusRollbackShellGeneration = 0;
                    m_nativeFocusRollbackEpoch = 0;
                }
            }
            else if ( HasNativeFocusAuthority() )
                ::SetFocus(nullptr);
        });
    wxUnusedVar(nativeAuthorityRollback);

    if ( !inIsland )
    {
        // A native shell request has already delivered CHILD_FOCUS/SET_FOCUS;
        // a direct logical request publishes the same transition below.
        // Moving shell -> bridge is an implementation detail and must not emit
        // a contradictory KILL_FOCUS for the just-focused logical control.
        wxWinUISlot * const transferSlot = getLiveSlot();
        if ( !transferSlot )
            return;
        const std::weak_ptr<wxWinUISlotLifetime> previousTransfer =
            m_nativeFocusTransferTarget;
        m_nativeFocusTransferTarget = transferSlot->m_lifetime;
        wxScopeGuard transferGuard = wxMakeGuard([this, previousTransfer]()
        {
            m_nativeFocusTransferTarget = previousTransfer;
        });
        wxUnusedVar(transferGuard);
        ::SetFocus(m_bridge);

        // SetFocus synchronously runs arbitrary KILL/SET_FOCUS handlers. They
        // are allowed to redirect native focus to another HWND. XAML can keep
        // a stale focused element for a short time after such a redirect, so
        // never publish that element as wx focus ownership. Supersede this
        // intent as well, making any still-live request frame and its queued
        // retry invisible to Got/LostFocus.
        if ( !HasNativeFocusAuthority() )
        {
            // GotFocus may already have published a transient slot owner while
            // SetFocus() was dispatching. Native focus has since been
            // redirected outside the island, so retire that owner and balance
            // its wx focus notification before returning.
            abandonCurrentIntent();
            return;
        }
        bridgeAcquiredForAttempt = true;
    }

    live = getLiveSlot();
    if ( !live || !HasNativeFocusAuthority() )
        return;

    bool focusInside = false;
    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;

        bool focused = false;

        // The component's preferred target first: a wxRadioBox focuses its
        // SELECTED item, not whatever comes first.
        if ( preferred && wxWinUIElementIsInSubtree(preferred, content) )
        {
            if ( const auto control =
                    preferred.try_as<MUX::Controls::Control>() )
            {
                focused =
                    control.IsEnabled() &&
                    control.Visibility() == MUX::Visibility::Visible &&
                    control.IsTabStop() &&
                    control.Focus(MUX::FocusState::Programmatic);
            }
            if ( !getLiveSlot() )
                return;
        }

        if ( !focused )
        {
            const auto control =
                content.try_as<MUX::Controls::Control>();
            if ( control )
                focused = control.Focus(MUX::FocusState::Programmatic);
            if ( !getLiveSlot() )
                return;
        }

        if ( !focused )
        {
            // The content is not a focusable Control itself: fall back to
            // its first focusable descendant.
            const auto first = MUX::Input::FocusManager::
                FindFirstFocusableElement(container);
            if ( const auto firstControl =
                    first.try_as<MUX::Controls::Control>() )
                focused = firstControl.Focus(MUX::FocusState::Programmatic);
            if ( !getLiveSlot() )
                return;
        }

        // The boolean returned by UIElement::Focus() is not the focus
        // authority: it can be false when focus was already within a
        // templated descendant (notably ComboBox's editable TextBox). Always
        // reconcile from FocusManager's current element and the exact slot
        // content generation. GotFocus normally publishes this transition
        // synchronously; this fallback also covers a projection/template that
        // doesn't bubble it through the container.
        focusInside = wxWinUIFocusIsInside(container, GetXamlRoot());
        if ( focusInside )
        {
            // Control::Focus() is another reentrant boundary. A Got/Kill
            // handler may have moved Win32 focus outside the bridge even
            // though FocusManager still reports this XAML subtree.
            if ( !HasNativeFocusAuthority() )
            {
                // Control::Focus() may have synchronously published the slot
                // before an application callback redirected native focus.
                // Once the island has lost native authority, no logical slot
                // owner may remain visible.
                abandonCurrentIntent();
                return;
            }

            // From here XAML really owns focus in this exact subtree. Any
            // later application redirection is authoritative and must not be
            // undone by the failed-attempt rollback guard.
            xamlFocusCommitted = true;
            FocusRequest *active = FindFocusRequest(window);
            if ( active &&
                 active->origin ==
                     FocusOrigin::MigrationContinuation )
            {
                const std::shared_ptr<FocusMigration> migration =
                    m_focusMigration;
                if ( CommitFocusMigration(migration, lifetime) )
                {
                    ClearDeferredFocusRequest(intentGeneration);
                    return;
                }
                AbortFocusMigration(migration, true);
                if ( !getLiveSlot() ||
                     intentGeneration != m_focusIntentGeneration )
                {
                    return;
                }
                active = FindFocusRequest(window);
            }
            ClearDeferredFocusRequest(intentGeneration);
            const bool alreadyDelivered =
                active &&
                active->origin == FocusOrigin::WxSetAlreadyDelivered;
            wxWindow * const previousHint =
                active ? active->previous.get() : previousWeak.get();
            TransitionFocus(window, previousHint, alreadyDelivered);
            if ( !HasNativeFocusAuthority() )
            {
                abandonCurrentIntent();
                return;
            }
            if ( m_focusOwner != window )
                return;
        }

        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("FocusSlot '%s' XAML focused=%d inside=%d",
                            label.utf8_str().data(), focused, focusInside);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    if ( focusInside || !allowDeferredRetry || !getLiveSlot() || !wxTheApp )
        return;

    // Focus can legitimately be unavailable until the coalesced slot
    // geometry/template pass. Arm one (and only one) post-flush retry for this
    // exact content generation. Loaded is an independent final edge when it
    // occurs after that flush; neither path reposts itself.
    MarkDirty(window);
}

wxWinUITopLevelHost::FocusRequest *
wxWinUITopLevelHost::FindFocusRequest(wxWindow *target)
{
    for ( auto it = m_focusRequests.rbegin();
          it != m_focusRequests.rend();
          ++it )
    {
        if ( it->intentGeneration == m_focusIntentGeneration &&
             it->target &&
             it->target->GetHost() == this &&
             it->target->GetWindow() == target )
        {
            return &*it;
        }
    }

    return nullptr;
}

bool wxWinUITopLevelHost::HasNativeFocusAuthority() const
{
    const HWND focus = ::GetFocus();
    return focus && m_bridge &&
           (focus == m_bridge || ::IsChild(m_bridge, focus));
}

void wxWinUITopLevelHost::InvalidateFocusIntent(
    unsigned long long expectedGeneration)
{
    if ( expectedGeneration &&
         expectedGeneration != m_focusIntentGeneration )
    {
        return;
    }

    if ( ++m_focusIntentGeneration == 0 )
        ++m_focusIntentGeneration;
    ClearDeferredFocusRequest();
}

wxWindow *wxWinUITopLevelHost::FindFocusedSlot() const
{
    const auto root = GetXamlRoot();
    for ( const auto& entry : m_slots )
    {
        const wxWinUISlot * const slot = entry.second;
        if ( slot && slot->m_container &&
             wxWinUIFocusIsInside(slot->m_container, root) )
        {
            return entry.first;
        }
    }

    return nullptr;
}

void wxWinUITopLevelHost::TransitionFocus(wxWindow *newOwner,
                                          wxWindow *previousHint,
                                          bool wxSetAlreadyDelivered,
                                          wxWindow *lossDestination)
{
    OperationGuard operation(this);

    if ( m_shuttingDown )
        return;

    if ( newOwner )
    {
        wxWinUISlot * const newSlot = FindSlot(newOwner);
        if ( !newSlot || !newSlot->m_lifetime ||
             newSlot->m_lifetime->GetHost() != this )
        {
            return;
        }
    }

    wxWindow * const oldOwner = m_focusOwner;
    if ( oldOwner == newOwner )
        return;

    const unsigned long long intentGeneration = m_focusIntentGeneration;
    const std::shared_ptr<wxWinUISlotLifetime> newLifetime =
        newOwner && FindSlot(newOwner)
            ? FindSlot(newOwner)->m_lifetime
            : nullptr;
    const unsigned long long newContentGeneration =
        newOwner && FindSlot(newOwner)
            ? FindSlot(newOwner)->m_contentGeneration
            : 0;
    wxWeakRef<wxWindow> oldWeak(oldOwner ? oldOwner : previousHint);
    wxWeakRef<wxWindow> newWeak(newOwner);
    wxWeakRef<wxWindow> lossWeak(newOwner ? nullptr : lossDestination);
    const auto destinationIsAuthoritative = [&]()
    {
        if ( !newOwner )
            return true;
        if ( intentGeneration != m_focusIntentGeneration ||
             !HasNativeFocusAuthority() || !newLifetime ||
             newLifetime->GetHost() != this ||
             newLifetime->GetWindow() != newWeak.get() )
        {
            return false;
        }

        wxWindow * const current = newWeak.get();
        wxWinUISlot * const slot = current ? FindSlot(current) : nullptr;
        return slot && slot->m_lifetime == newLifetime &&
               slot->m_contentGeneration == newContentGeneration &&
               wxWinUIFocusIsInside(slot->m_container, GetXamlRoot());
    };

    // Publish the truthful logical destination before application callbacks:
    // FindFocus() called from KILL_FOCUS must already see where focus moved.
    m_focusOwner = newOwner;

    const auto dispatch = [](wxWindow *receiver, wxEvent& event)
    {
        receiver->HandleWindowEvent(event);
    };

    if ( oldOwner )
    {
        if ( wxWindow * const oldWindow = oldWeak.get() )
        {
            wxFocusEvent event(wxEVT_KILL_FOCUS, oldWindow->GetId());
            event.SetEventObject(oldWindow);
            event.SetWindow(newOwner ? newWeak.get() : lossWeak.get());
            dispatch(oldWindow, event);
        }

        // KILL_FOCUS handlers may destroy the target or redirect focus. Do
        // not synthesize events for the stale destination in either case.
        if ( m_shuttingDown || m_focusOwner != newOwner )
            return;
        if ( !destinationIsAuthoritative() )
        {
            if ( m_focusOwner == newOwner )
                m_focusOwner = nullptr;
            return;
        }
    }

    if ( !newOwner || wxSetAlreadyDelivered )
        return;

    wxWindow *newWindow = newWeak.get();
    if ( !newWindow || !FindSlot(newWindow) )
    {
        if ( m_focusOwner == newOwner )
            m_focusOwner = nullptr;
        return;
    }

    // Match wxWindowMSW::HandleSetFocus(): parent bookkeeping first, then
    // SET_FOCUS with the previous window as its counterpart.
    wxChildFocusEvent childEvent(newWindow);
    dispatch(newWindow, childEvent);

    newWindow = newWeak.get();
    if ( m_shuttingDown || !newWindow ||
         m_focusOwner != newOwner || !FindSlot(newWindow) ||
         !destinationIsAuthoritative() )
    {
        if ( m_focusOwner == newOwner )
            m_focusOwner = nullptr;
        return;
    }

    wxFocusEvent setEvent(wxEVT_SET_FOCUS, newWindow->GetId());
    setEvent.SetEventObject(newWindow);
    setEvent.SetWindow(oldWeak.get());
    dispatch(newWindow, setEvent);

    // SET handlers may redirect native focus while FocusManager still reports
    // the old XAML leaf. Balance the SET exactly once if no synchronous
    // LostFocus/nested transition already retired this provisional owner.
    if ( m_focusOwner == newOwner && !destinationIsAuthoritative() )
    {
        TransitionFocus(
            nullptr, newOwner, false,
            wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::OnXamlSlotGotFocus(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container)
{
    wxWindow *window = state ? state->GetWindow() : nullptr;
    wxWinUISlot *slot = window ? FindSlot(window) : nullptr;
    if ( !state || state->GetHost() != this || !window || !slot ||
         slot->m_lifetime != state || !container )
    {
        return;
    }

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(slot);

    // GotFocus bubbles once per inner element and stale notifications can
    // arrive after a newer transition. Only the currently focused subtree
    // is authoritative. Win32 focus must still belong to the bridge/InputSite:
    // FocusManager can lag behind a reentrant native redirection.
    if ( !HasNativeFocusAuthority() ||
         !wxWinUIFocusIsInside(container, GetXamlRoot()) )
        return;

    m_nativeFocusDepartureSource.reset();
    m_nativeFocusDepartureDestination = nullptr;
    m_nativeFocusDepartureDestinationGeneration = 0;
    const std::shared_ptr<wxWinUISlotLifetime> entryTarget =
        m_nativeFocusEntryTarget.lock();
    if ( !entryTarget ||
         (entryTarget == state &&
          m_nativeFocusEntryEpoch != 0 &&
          slot->m_nativeFocusEntryEpoch == m_nativeFocusEntryEpoch) )
    {
        m_nativeFocusEntryAuthority = nullptr;
        m_nativeFocusEntryAuthorityGeneration = 0;
        m_nativeFocusEntryTarget.reset();
        m_nativeFocusEntryEpoch = 0;
        slot->m_nativeFocusEntryEpoch = 0;
    }

    wxWeakRef<wxWindow> migrationPrevious;
    const std::shared_ptr<FocusMigration> migration = m_focusMigration;
    if ( migration && !migration->committed &&
         migration->destinationHost == m_hostLifetime )
    {
        migrationPrevious = migration->window;
        if ( migration->destinationSlot == state &&
             CommitFocusMigration(migration, state) )
        {
            FocusRequest * const request = FindFocusRequest(window);
            if ( request &&
                 request->origin ==
                     FocusOrigin::MigrationContinuation )
            {
                ClearDeferredFocusRequest(
                    request->intentGeneration);
            }
            else if ( m_deferredFocusRequest.origin ==
                          FocusOrigin::MigrationContinuation &&
                      m_deferredFocusRequest.target == state )
            {
                ClearDeferredFocusRequest(
                    m_deferredFocusRequest.intentGeneration);
            }
            return;
        }

        // Any other focused destination slot wins over the suspended
        // transaction. Keep its native authority, balance the migrant's one
        // logical owner, then let the ordinary arbiter publish this winner.
        // A failed exact commit is handled by the same fail-closed path.
        AbortFocusMigration(
            migration, true,
            migration->destinationSlot == state ? nullptr : window);

        window = state->GetWindow();
        slot = window ? FindSlot(window) : nullptr;
        if ( !window || !slot || slot->m_lifetime != state ||
             state->GetHost() != this ||
             !HasNativeFocusAuthority() ||
             !wxWinUIFocusIsInside(container, GetXamlRoot()) )
        {
            return;
        }
    }

    FocusRequest *request = FindFocusRequest(window);
    if ( request &&
         request->origin == FocusOrigin::MigrationContinuation )
    {
        // The request can outlive a ticket retired by a native/XAML redirect.
        // It has no authority to revive that migration as an ordinary focus
        // request.
        ClearDeferredFocusRequest(request->intentGeneration);
        request = nullptr;
    }
    if ( !request )
    {
        // A physical/XAML navigation result supersedes any older deferred
        // programmatic intent.
        InvalidateFocusIntent();
    }
    wxWindow * const migratedPrevious = migrationPrevious.get();
    wxWindow * const previous =
        request
            ? request->previous.get()
            : migratedPrevious
                ? migratedPrevious
                : m_focusOwner;
    const bool alreadyDelivered =
        request &&
        request->origin == FocusOrigin::WxSetAlreadyDelivered;
    TransitionFocus(window, previous, alreadyDelivered);
    if ( m_focusOwner == window && !HasNativeFocusAuthority() )
    {
        TransitionFocus(
            nullptr, window, false,
            wxWinUIResolveCurrentNativeFocusWindow());
    }
}

void wxWinUITopLevelHost::OnXamlSlotLostFocus(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container)
{
    wxWindow * const window = state ? state->GetWindow() : nullptr;
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !state || state->GetHost() != this || !window || !slot ||
         slot->m_lifetime != state || !container )
    {
        return;
    }

    OperationGuard hostOperation(this);
    wxWinUISlot::OperationGuard slotOperation(slot);

    const auto xamlRoot = GetXamlRoot();
    if ( wxWinUIFocusIsInside(container, xamlRoot) )
        return;

    // A late LostFocus from a slot superseded by a newer transition is a
    // complete no-op, not another KILL_FOCUS.
    if ( m_focusOwner != window )
        return;

    // Detaching a focused slot for a cross-TLW migration raises LostFocus
    // before the destination carrier can acquire focus. Keep the single
    // logical wx control focused across that implementation handoff; a real
    // redirect to another slot/native HWND cancels the ticket above.
    if ( IsSuppressedMigrationLoss(state) )
        return;

    // Only FocusManager may publish a destination. A pending request is useful
    // as a counterpart hint but is never evidence that XAML focused its slot.
    wxWindow *target = FindFocusedSlot();
    FocusRequest * const request =
        target ? FindFocusRequest(target) : nullptr;
    if ( !request )
    {
        InvalidateFocusIntent();
    }
    const bool alreadyDelivered =
        request &&
        request->origin == FocusOrigin::WxSetAlreadyDelivered;
    // A non-null destination is truthful only while the island owns native
    // focus. If focus has left the bridge, publish a loss instead of reviving
    // a stale XAML target.
    const bool nativeAuthority = HasNativeFocusAuthority();
    wxWindow * const lossDestination =
        nativeAuthority ? nullptr : wxWinUIResolveCurrentNativeFocusWindow();
    TransitionFocus(nativeAuthority ? target : nullptr,
                    window, alreadyDelivered,
                    lossDestination);
}

void wxWinUITopLevelHost::SetSlotPreferredFocus(
    wxWindow *window,
    const winrt::Microsoft::UI::Xaml::UIElement& target)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    if ( !target )
    {
        slot->m_preferredFocus = nullptr;
        return;
    }

    // Never replace a valid target with an element from a detached or
    // speculative tree.  This makes a failed control-host content swap leave
    // the old preferred focus target intact.
    if ( target.try_as<winrt::Microsoft::UI::Xaml::Controls::Control>() &&
            wxWinUIElementIsInSubtree(target, slot->m_content) )
    {
        slot->m_preferredFocus = winrt::make_weak(target);
    }
}

void wxWinUITopLevelHost::OnTakeFocusRequested(bool previous)
{
    OperationGuard operation(this);

    // The island's own Tab navigation ran off its first/last element: hand
    // the focus to the neighbouring wx tab stop -- through the same
    // focusability-aware walk as the Tab pre-translation, so a window that
    // refuses the keyboard focus is never re-entered from this exit either.
    wxWindow * const current = m_focusOwner;
    const HWND hwndCurrent = current ? GetHwndOf(current) : nullptr;
    const HWND hwndTLW = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    if ( !hwndCurrent || !hwndTLW )
        return;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const wxWeakRef<wxWindow> currentWeak(current);
    const wxWeakRef<wxWindow> tlwWeak(m_tlw);
    const unsigned long long currentGeneration =
        wxWinUIMSWGetHwndGeneration(
            current, reinterpret_cast<WXHWND>(hwndCurrent));
    const unsigned long long tlwGeneration =
        wxWinUIMSWGetHwndGeneration(
            m_tlw, reinterpret_cast<WXHWND>(hwndTLW));
    if ( !currentGeneration || !tlwGeneration )
        return;

    const HWND next = wxWinUIFindNextKeyboardFocusable(hwndTLW,
                                                       hwndCurrent, previous);
    wxWindow * const liveCurrent = currentWeak.get();
    wxWindow * const liveTLW = tlwWeak.get();
    if ( !hostState || hostState->GetHost() != this || m_shuttingDown ||
         !liveCurrent || !liveTLW || m_focusOwner != liveCurrent ||
         wxWinUIMSWGetHwndGeneration(
             liveCurrent, reinterpret_cast<WXHWND>(hwndCurrent)) !=
             currentGeneration ||
         wxWinUIMSWGetHwndGeneration(
             liveTLW, reinterpret_cast<WXHWND>(hwndTLW)) != tlwGeneration )
    {
        return;
    }

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("TakeFocus prev=%d from=%p -> %p",
                        previous, static_cast<void *>(hwndCurrent),
                        static_cast<void *>(next));
    }

    if ( !next )
        return;

    const unsigned long long nextGeneration =
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(next));
    if ( !nextGeneration )
        return;

    wxWindow * const nextWin = wxGetWindowFromHWND((WXHWND)next);
    if ( nextWin && FindSlot(nextWin) )
        FocusSlot(nextWin);
    else if ( wxWinUIMSWGetNativeHwndGeneration(
                  reinterpret_cast<WXHWND>(next)) == nextGeneration )
        ::SetFocus(next);
}

bool wxWinUITopLevelHost::ShouldSuppressNativeKillFocus(
    wxWindow *window,
    WXHWND destination)
{
    wxWinUITopLevelHost * const host = FindSlotOwner(window);
    const HWND target = static_cast<HWND>(destination);
    const auto transfer =
        host ? host->m_nativeFocusTransferTarget.lock() : nullptr;
    return host &&
           transfer && transfer->GetHost() == host &&
           transfer->GetWindow() == window &&
           host->FindSlot(window) &&
           host->FindSlot(window)->m_lifetime == transfer &&
           target &&
           (target == host->m_bridge ||
            (host->m_bridge && ::IsChild(host->m_bridge, target)));
}

unsigned wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest()
{
    return static_cast<unsigned>(ms_focusMigrations.size());
}

#endif // wxUSE_WINUI3
