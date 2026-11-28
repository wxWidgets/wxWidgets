/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/dropbroker.h
// Purpose:     one dual-adapter OLE drop broker per WinUI top-level island
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_DROPBROKER_H_
#define _WX_WINUI_PRIVATE_DROPBROKER_H_

#include "wx/defs.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

#include "wx/msw/wrapwin.h"
#include "wx/msw/private/dropsession.h"

#include <memory>

struct IDropTarget;
struct IUnknown;
class wxWindow;
class wxWinUIHostLifetime;
class wxWinUIDropBrokerState;
class wxWinUIDropBrokerCOMTarget;

enum class wxWinUIDropBrokerInitStatus
{
    NotInitialized,
    Ready,
    InvalidContext,
    WrongThread,
    LockFailed,
    AlreadyRegistered,
    PendingRegistration,
    RegisterFailed,
    AdoptionFailed
};

struct WXDLLIMPEXP_CORE wxWinUIDropBrokerInitResult
{
    wxWinUIDropBrokerInitStatus status =
        wxWinUIDropBrokerInitStatus::NotInitialized;
    HRESULT hresult = E_UNEXPECTED;

    bool IsReady() const noexcept
        { return status == wxWinUIDropBrokerInitStatus::Ready; }
};

// Host-owned handle. The actual COM target has independent ref-counted state,
// so an OLE reference surviving a failed RevokeDragDrop() can only call an
// inactive, fail-closed object and can never retain or dereference the host.
class WXDLLIMPEXP_CORE wxWinUIDropBroker final
{
public:
    static std::unique_ptr<wxWinUIDropBroker>
    Create(wxWindow* tlw,
           HWND bridge,
           const std::weak_ptr<wxWinUIHostLifetime>& lifetime) noexcept;

    ~wxWinUIDropBroker();

    wxWinUIDropBrokerInitResult Initialize() noexcept;
    void Shutdown() noexcept;
    bool IsReady() const noexcept;

    // UI-thread ownership query, including a retained registration or an
    // ambiguous external unlock after shutdown. Does not inspect HWNDs,
    // invoke COM, refresh diagnostics, or schedule a retry.
    bool HasNativeOwnership() const noexcept;

    bool IsInitializationPending() const noexcept
        { return m_initializePending; }
    bool IsPendingContinuation(std::uint64_t pendingId) const noexcept
    {
        return m_initializePending && pendingId != 0 &&
               (m_bridgeRegistration.operation.GetPendingId() == pendingId ||
                m_tlwRegistration.operation.GetPendingId() == pendingId);
    }

    // True while this broker owns either physical registration or is in the
    // transaction that acquires the complete pair. The provisional state
    // prevents a
    // re-entrant SetDropTarget()/Reparent() from installing a competing shell
    // registration between collection and RegisterDragDrop().
    bool CoversShellDropTargets() const noexcept;

    // Transactionally detach every already-registered logical target below
    // this TLW, then acquire the bridge + TLW physical registrations.
    // Detaching first is required when the TLW itself already owns a
    // wxDropTarget.
    // Failure reconciles every current logical target, including targets
    // added re-entrantly while acquisition was in progress.
    wxWinUIDropBrokerInitResult
    InitializeAndAdoptShellDropTargets() noexcept;

    // Seal the acquisition only after wxWinUITopLevelHost itself commits.
    // Before this point Shutdown() restores logical shell registrations if
    // later host initialization rolls back.
    void CommitHostInitialization() noexcept;

    // Reconcile a reparented subtree against its current TLW host.
    static void ReconcileShellDropTargets(wxWindow* subtreeRoot) noexcept;

private:
    friend class wxWinUIDropBrokerState;
    friend class wxWinUIDropBrokerTestAccess;

    wxWinUIDropBroker(
        const std::shared_ptr<wxWinUIDropBrokerState>& state,
        wxWinUIDropBrokerCOMTarget* bridgeTarget,
        wxWinUIDropBrokerCOMTarget* tlwTarget) noexcept;

    wxWinUIDropBrokerInitResult InitializeNative(
        bool deferActivation) noexcept;
    wxWinUIDropBrokerInitResult PublishInitResult(
        const wxWinUIDropBrokerInitResult& value) noexcept;
    bool UpdateShellDropCoverage() noexcept;

    enum class RegistrationRole
    {
        Bridge,
        TLW
    };

    struct ShellRegistration
    {
        wxMSWOleShellHwndIdentity identity;
        wxMSWOleExternalShellOperation operation;
        bool locked = false;
        bool ownsRegistration = false;
    };

    HWND GetRegistrationHwnd(RegistrationRole role) const noexcept;
    std::uint64_t GetRegistrationGeneration(
        RegistrationRole role) const noexcept;
    bool IsRegistrationCurrent(RegistrationRole role) const noexcept;
    ShellRegistration& GetRegistration(RegistrationRole role) noexcept;
    const ShellRegistration& GetRegistration(
        RegistrationRole role) const noexcept;
    IDropTarget* GetRegistrationTarget(
        RegistrationRole role) const noexcept;
    HRESULT ReleaseExternalLock(RegistrationRole role) noexcept;
    bool OwnsAnyRegistration() const noexcept;
    bool OwnsCompleteRegistrationSet() const noexcept;

    std::shared_ptr<wxWinUIDropBrokerState> m_state;
    // OLE associates an IDropTarget identity with an HWND. Keep one adapter
    // identity per physical registration while both adapters route through
    // the same logical State/Session.
    wxWinUIDropBrokerCOMTarget* m_bridgeTarget = nullptr;
    wxWinUIDropBrokerCOMTarget* m_tlwTarget = nullptr;
    wxWinUIDropBrokerInitResult m_initResult;
    bool m_initializeAttempted = false;
    bool m_initializePending = false;
    bool m_shutdown = false;
    // Native OLE/COM calls may pump messages and synchronously request host
    // shutdown. While acquisition is on the stack Shutdown() publishes the
    // terminal, fail-closed state immediately but leaves exact rollback to
    // the outer acquisition frame, which knows which calls succeeded.
    bool m_acquiring = false;
    bool m_shutdownCleanupStarted = false;
    bool m_adoptingShellDropTargets = false;
    unsigned long long m_shellDropCoverageToken = 0;
    bool m_rollbackUncommittedAdoption = false;
    ShellRegistration m_bridgeRegistration;
    ShellRegistration m_tlwRegistration;

    wxDECLARE_NO_COPY_CLASS(wxWinUIDropBroker);
};

#endif // wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

#endif // _WX_WINUI_PRIVATE_DROPBROKER_H_
