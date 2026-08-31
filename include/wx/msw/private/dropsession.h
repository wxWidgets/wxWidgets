/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/dropsession.h
// Purpose:     Shared logical dispatch for native MSW OLE drop targets
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_DROPSESSION_H_
#define _WX_MSW_PRIVATE_DROPSESSION_H_

#include "wx/defs.h"

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP

#include "wx/msw/wrapwin.h"

#include <cstdint>
#include <memory>

class wxDropTarget;
class wxMSWOleDropTargetState;
class wxWindow;
struct IDataObject;
struct IDropTarget;

// Private, layout-neutral access used by the fixed-wrapper lifetime tests.
// wxDropTarget grants this class friendship without exposing the COM pointer
// in its public API or changing its vtable/instance layout.
class WXDLLIMPEXP_CORE wxMSWOleDropTargetAccess
{
public:
    static IDropTarget* GetCOMInterface(wxDropTarget* target) noexcept;
};

// A drop target is owned by its wxWindow and can therefore disappear from any
// user callback.  This private lease is the identity of one exact target
// lifetime.  It never owns the target: invalidation makes every outstanding
// lease fail before the target is deleted.
class WXDLLIMPEXP_CORE wxMSWOleDropTargetLease final
{
public:
    wxMSWOleDropTargetLease() = default;

    bool IsCurrent() const noexcept;
    wxDropTarget* GetIfCurrent() const noexcept;
    bool IsSameLifetime(const wxMSWOleDropTargetLease& other) const noexcept;

private:
    friend WXDLLIMPEXP_CORE wxMSWOleDropTargetLease
    wxMSWOleAcquireDropTarget(wxDropTarget*) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleInvalidateDropTarget(wxDropTarget*) noexcept;

    wxMSWOleDropTargetLease(
                            const std::shared_ptr<wxMSWOleDropTargetState>& state,
                            std::uint64_t generation) noexcept;

    std::shared_ptr<wxMSWOleDropTargetState> m_state;
    std::uint64_t m_generation = 0;
};

// Acquire/invalidate do not alter wxDropTarget's layout or vtable.  The latter
// must be called before an owning window deletes or replaces its target, with
// wxDropTarget's destructor providing the final safety net.
WXDLLIMPEXP_CORE wxMSWOleDropTargetLease
wxMSWOleAcquireDropTarget(wxDropTarget* target) noexcept;

WXDLLIMPEXP_CORE void
wxMSWOleInvalidateDropTarget(wxDropTarget* target) noexcept;

// Exact association created by wxWindowMSW::SetDropTarget(). This is distinct
// from a target lifetime because composite controls can expose another
// window's target through their virtual GetDropTarget().
enum class wxMSWOleDropTargetLookup;

class WXDLLIMPEXP_CORE wxMSWOleDropTargetBinding final
{
public:
    wxMSWOleDropTargetBinding() = default;

    bool IsCurrent() const noexcept;
    wxDropTarget* GetTargetIfCurrent() const noexcept;
    wxWindow* GetOwnerIfCurrent() const noexcept;
    WXHWND GetOwnerHwndIfCurrent() const noexcept;
    wxMSWOleDropTargetLease GetTargetLease() const noexcept;
    bool IsSameBinding(const wxMSWOleDropTargetBinding& other) const noexcept;
    std::uint64_t GetGeneration() const noexcept
        { return m_bindingGeneration; }

private:
    friend WXDLLIMPEXP_CORE void
    wxMSWOleBindDropTarget(wxWindow*, wxDropTarget*, WXHWND) noexcept;
    friend WXDLLIMPEXP_CORE wxMSWOleDropTargetLookup
    wxMSWOleLookupDropTarget(wxWindow*, wxMSWOleDropTargetBinding*) noexcept;

    wxMSWOleDropTargetBinding(
        const std::shared_ptr<wxMSWOleDropTargetState>& state,
        std::uint64_t targetGeneration,
        std::uint64_t bindingGeneration) noexcept;

    std::shared_ptr<wxMSWOleDropTargetState> m_state;
    std::uint64_t m_targetGeneration = 0;
    std::uint64_t m_bindingGeneration = 0;
};

enum class wxMSWOleDropTargetLookup
{
    None,
    Found,
    Unstable
};

WXDLLIMPEXP_CORE void
wxMSWOleBindDropTarget(wxWindow* owner,
                       wxDropTarget* target,
                       WXHWND ownerHwnd) noexcept;

WXDLLIMPEXP_CORE void
wxMSWOleUnbindDropTarget(wxWindow* owner,
                         wxDropTarget* target) noexcept;

// Calls queried->GetDropTarget() virtually. A non-null pointer not present in
// the registry is Unstable (not a dereferenceable target), covering native
// sentinels such as the rich-edit default drop target.
WXDLLIMPEXP_CORE wxMSWOleDropTargetLookup
wxMSWOleLookupDropTarget(wxWindow* queried,
                         wxMSWOleDropTargetBinding* binding) noexcept;

WXDLLIMPEXP_CORE std::uint64_t
wxMSWOleGetDropTargetMutationGeneration() noexcept;

// Physical shell registration bookkeeping. Begin() publishes an in-flight
// guard before the first COM boundary. Arm() then assigns the mutation order
// immediately before RegisterDragDrop/RevokeDragDrop. The token pins the exact
// private state and target generation, preventing pointer-reuse (ABA) from an
// old completion corrupting a newly allocated wxDropTarget at the same address.
enum class wxMSWOleShellDropTargetOperationKind
{
    Register,
    Revoke
};

enum class wxMSWOleShellDropTargetProjection
{
    Unchanged,
    Zero,
    Unique,
    Ambiguous
};

class WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation final
{
public:
    wxMSWOleShellDropTargetOperation() = default;
    ~wxMSWOleShellDropTargetOperation();

    wxMSWOleShellDropTargetOperation(
        wxMSWOleShellDropTargetOperation&& other) noexcept;
    wxMSWOleShellDropTargetOperation& operator=(
        wxMSWOleShellDropTargetOperation&& other) noexcept;

    bool IsActive() const noexcept { return m_active; }
    bool IsArmed() const noexcept
        { return m_active && m_operationGeneration != 0; }
    wxMSWOleShellDropTargetProjection GetProjection() const noexcept
        { return m_projection; }
    WXHWND GetProjectedHwnd() const noexcept
        { return m_projectedHwnd; }

private:
    friend WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
    wxMSWOleBeginShellDropTargetOperation(wxDropTarget*, WXHWND) noexcept;
    friend WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
    wxMSWOleBeginShellDropTargetOperation(
        wxDropTarget*, WXHWND,
        wxMSWOleShellDropTargetOperationKind) noexcept;
    friend WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
    wxMSWOleBeginShellDropTargetOperation(
        wxDropTarget*, WXHWND,
        wxMSWOleShellDropTargetOperationKind, IDropTarget*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleArmShellDropTargetOperation(
        wxMSWOleShellDropTargetOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleCompleteShellDropTargetOperation(
        wxMSWOleShellDropTargetOperation*, bool) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleCancelShellDropTargetOperation(
        wxMSWOleShellDropTargetOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleIsLatestShellDropTargetOperation(
        const wxMSWOleShellDropTargetOperation&) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleReserveShellDropTargetCompensation(
        wxMSWOleShellDropTargetOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleCompleteShellDropTargetCompensation(
        wxMSWOleShellDropTargetOperation*, bool) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleNoteShellDropTargetExternalLock(
        wxMSWOleShellDropTargetOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleClaimShellDropTargetExternalUnlock(
        wxMSWOleShellDropTargetOperation*) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleNoteShellDropTargetExternalUnlock(
        wxMSWOleShellDropTargetOperation*, bool) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleCleanupShellDropTargetOwner(
        wxMSWOleShellDropTargetOperation*) noexcept;

    wxMSWOleShellDropTargetOperation(
        const std::shared_ptr<wxMSWOleDropTargetState>& state,
        std::uint64_t targetGeneration,
        WXHWND hwnd,
        unsigned long long hwndGeneration,
        unsigned long threadId,
        std::uint64_t reservationId,
        std::uint64_t baselineProjectionEpoch,
        wxMSWOleShellDropTargetOperationKind kind,
        IDropTarget* comIdentity,
        bool globalTracking) noexcept;

    std::shared_ptr<wxMSWOleDropTargetState> m_state;
    std::uint64_t m_targetGeneration = 0;
    std::uint64_t m_operationGeneration = 0;
    std::uint64_t m_reservationId = 0;
    std::uint64_t m_baselineProjectionEpoch = 0;
    std::uint64_t m_projectionEpoch = 0;
    std::uint64_t m_globalOwnerSerial = 0;
    std::uint64_t m_compensationReservationId = 0;
    WXHWND m_hwnd = nullptr;
    WXHWND m_projectedHwnd = nullptr;
    unsigned long long m_hwndGeneration = 0;
    unsigned long m_threadId = 0;
    wxMSWOleShellDropTargetOperationKind m_kind =
        wxMSWOleShellDropTargetOperationKind::Register;
    wxMSWOleShellDropTargetProjection m_projection =
        wxMSWOleShellDropTargetProjection::Unchanged;
    IDropTarget* m_comIdentity = nullptr;
    IDropTarget* m_deferredRelease = nullptr;
    bool m_globalTracking = false;
    bool m_externalLockAcquired = false;
    bool m_externalUnlockClaimed = false;
    bool m_active = false;

    wxDECLARE_NO_COPY_CLASS(wxMSWOleShellDropTargetOperation);
};

WXDLLIMPEXP_CORE bool
wxMSWOleHasDropTargetState(wxDropTarget* target) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleIsShellDropTargetRegistered(wxDropTarget* target,
                                    WXHWND hwnd) noexcept;

// Diagnostic/stability predicate only. An in-flight Register is not ownership
// and must never authorize RevokeDragDrop(hwnd): another target can still own
// that HWND until this operation succeeds.
WXDLLIMPEXP_CORE bool
wxMSWOleIsShellDropTargetOperationInFlight(wxDropTarget* target,
                                           WXHWND hwnd) noexcept;

WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(wxDropTarget* target,
                                      WXHWND hwnd) noexcept;

// The kind-only overload is a deterministic ledger seam: it exercises exact
// target/HWND reservations without claiming that a native OLE call occurred.
WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(
    wxDropTarget* target,
    WXHWND hwnd,
    wxMSWOleShellDropTargetOperationKind kind) noexcept;

// Production overload. The fixed COM identity is retained by the global
// exact-HWND owner ledger after a successful RegisterDragDrop().
WXDLLIMPEXP_CORE wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(
    wxDropTarget* target,
    WXHWND hwnd,
    wxMSWOleShellDropTargetOperationKind kind,
    IDropTarget* comIdentity) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleArmShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

// Always records an exact successful native result per HWND generation.
// Returns true only when it may also publish the wrapper's derived unique
// HWND/helper and the post-boundary HWND is still current. Independent HWND
// keys commit independently; the projection is recomputed from all live keys.
// Call exactly once after success; call Cancel() after native failure.
WXDLLIMPEXP_CORE bool
wxMSWOleCompleteShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation,
    bool registered) noexcept;

// Returns true if no newer successful operation committed while this failed
// operation was in flight.
WXDLLIMPEXP_CORE bool
wxMSWOleCancelShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleIsLatestShellDropTargetOperation(
    const wxMSWOleShellDropTargetOperation& operation) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleReserveShellDropTargetCompensation(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleCompleteShellDropTargetCompensation(
    wxMSWOleShellDropTargetOperation* operation,
    bool revoked) noexcept;

WXDLLIMPEXP_CORE void
wxMSWOleNoteShellDropTargetExternalLock(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleClaimShellDropTargetExternalUnlock(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

WXDLLIMPEXP_CORE void
wxMSWOleNoteShellDropTargetExternalUnlock(
    wxMSWOleShellDropTargetOperation* operation,
    bool unlocked) noexcept;

struct WXDLLIMPEXP_CORE wxMSWOleShellHwndIdentity
{
    WXHWND hwnd = nullptr;
    unsigned long long generation = 0;
    unsigned long threadId = 0;
};

// Exact process-global bookkeeping for a registration whose native calls and
// external COM lock are owned by another component (the WinUI TLW broker).
// PendingFixed is a non-terminal reservation: no native call may be made until
// TryPromote() succeeds after the in-flight fixed-wrapper operation unwinds.
enum class wxMSWOleExternalShellOperationStatus
{
    Rejected,
    PendingFixed,
    Active
};

class WXDLLIMPEXP_CORE wxMSWOleExternalShellOperation final
{
public:
    wxMSWOleExternalShellOperation() = default;
    ~wxMSWOleExternalShellOperation();

    wxMSWOleExternalShellOperation(
        wxMSWOleExternalShellOperation&& other) noexcept;
    wxMSWOleExternalShellOperation& operator=(
        wxMSWOleExternalShellOperation&& other) noexcept;

    wxMSWOleExternalShellOperationStatus GetStatus() const noexcept
        { return m_status; }
    bool IsActive() const noexcept
        { return m_status == wxMSWOleExternalShellOperationStatus::Active; }
    bool IsPendingFixed() const noexcept
        { return m_status ==
            wxMSWOleExternalShellOperationStatus::PendingFixed; }
    bool IsArmed() const noexcept
        { return IsActive() && m_operationGeneration != 0; }
    bool WasRetireRequested() const noexcept
        { return m_retireRequested; }
    std::uint64_t GetPendingId() const noexcept
        { return IsPendingFixed() ? m_pendingId : 0; }

private:
    friend WXDLLIMPEXP_CORE wxMSWOleExternalShellOperation
    wxMSWOleBeginExternalShellOperation(
        const wxMSWOleShellHwndIdentity&,
        wxMSWOleShellDropTargetOperationKind,
        IDropTarget*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleTryPromoteExternalShellOperation(
        wxMSWOleExternalShellOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleArmExternalShellOperation(
        wxMSWOleExternalShellOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleArmExternalShellContinuation(
        wxMSWOleExternalShellOperation*, WXHWND) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleCompleteExternalShellOperation(
        wxMSWOleExternalShellOperation*, bool) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleNoteExternalShellLock(
        wxMSWOleExternalShellOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleClaimExternalShellUnlock(
        wxMSWOleExternalShellOperation*) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleCompleteExternalShellUnlock(
        wxMSWOleExternalShellOperation*, bool) noexcept;
    friend WXDLLIMPEXP_CORE void
    wxMSWOleCancelExternalShellOperation(
        wxMSWOleExternalShellOperation*) noexcept;
    friend WXDLLIMPEXP_CORE bool
    wxMSWOleAbandonExternalShellOwner(
        wxMSWOleExternalShellOperation*) noexcept;

    wxMSWOleShellHwndIdentity m_identity;
    wxMSWOleShellDropTargetOperationKind m_kind =
        wxMSWOleShellDropTargetOperationKind::Register;
    IDropTarget* m_comIdentity = nullptr;
    IDropTarget* m_deferredRelease = nullptr;
    std::uint64_t m_reservationId = 0;
    std::uint64_t m_pendingId = 0;
    std::uint64_t m_operationGeneration = 0;
    std::uint64_t m_ownerSerial = 0;
    wxMSWOleExternalShellOperationStatus m_status =
        wxMSWOleExternalShellOperationStatus::Rejected;
    bool m_retireRequested = false;
    bool m_identityPinned = false;
    bool m_externalLockAcquired = false;
    bool m_externalUnlockClaimed = false;

    wxDECLARE_NO_COPY_CLASS(wxMSWOleExternalShellOperation);
};

WXDLLIMPEXP_CORE wxMSWOleExternalShellOperation
wxMSWOleBeginExternalShellOperation(
    const wxMSWOleShellHwndIdentity& identity,
    wxMSWOleShellDropTargetOperationKind kind,
    IDropTarget* comIdentity) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleTryPromoteExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept;

WXDLLIMPEXP_CORE UINT
wxMSWOleGetExternalShellContinuationMessage() noexcept;

// The registered continuation message can itself be pumped by a later OLE
// boundary. Never resume the broker while any fixed-shell public call remains
// on this thread; defer the exact pending id back to that call's outer tail.
WXDLLIMPEXP_CORE bool
wxMSWOleIsFixedShellCallInProgress() noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleDeferExternalShellContinuation(
    std::uint64_t pendingId,
    WXHWND wakeHwnd) noexcept;

// Consume exactly one successfully posted continuation. Duplicate, stale or
// redirected registered messages fail closed without touching the broker.
WXDLLIMPEXP_CORE bool
wxMSWOleClaimExternalShellContinuation(
    std::uint64_t pendingId,
    WXHWND wakeHwnd) noexcept;

// Retry one previously failed PostMessage delivery for this exact bridge.
// This is called from ordinary bridge message activity, never as a spin loop.
WXDLLIMPEXP_CORE bool
wxMSWOleRetryFailedExternalShellContinuation(
    WXHWND wakeHwnd) noexcept;

// Associate the pending token with a same-thread wake HWND. The fixed
// operation tail posts (never sends) the registered message after releasing
// every ledger mutex; no broker callback occurs on the fixed stack.
WXDLLIMPEXP_CORE bool
wxMSWOleArmExternalShellContinuation(
    wxMSWOleExternalShellOperation* operation,
    WXHWND wakeHwnd) noexcept;

WXDLLIMPEXP_CORE bool
wxMSWOleArmExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept;

// Complete after the external component's native call. For Register, S_OK
// publishes ExternalManaged ownership. For Revoke, false preserves physical
// ownership until the component explicitly abandons its now fail-closed
// context with wxMSWOleAbandonExternalShellOwner().
WXDLLIMPEXP_CORE bool
wxMSWOleCompleteExternalShellOperation(
    wxMSWOleExternalShellOperation* operation,
    bool succeeded) noexcept;

// Record the successful TRUE owned by the external component without ever
// calling its native/fake lock boundary from the core ledger.
WXDLLIMPEXP_CORE void
wxMSWOleNoteExternalShellLock(
    wxMSWOleExternalShellOperation* operation) noexcept;

// Atomically claim the external component's single balancing FALSE. A failed
// FALSE remains claimed: its side effects are not assumed and it is never
// issued a second time merely because the HRESULT failed.
WXDLLIMPEXP_CORE bool
wxMSWOleClaimExternalShellUnlock(
    wxMSWOleExternalShellOperation* operation) noexcept;

// Publish the external component's FALSE result after its native/fake call.
// Failure retains the exact COM identity and held-lock proof fail-closed.
WXDLLIMPEXP_CORE void
wxMSWOleCompleteExternalShellUnlock(
    wxMSWOleExternalShellOperation* operation,
    bool unlocked) noexcept;

WXDLLIMPEXP_CORE void
wxMSWOleCancelExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept;

// Convert an ExternalManaged owner to an occupied, context-free tombstone.
// This is used only after the broker has failed closed and balanced its own
// external lock. It releases the ledger's retained COM identity outside the
// registry mutex while continuing to block competing fixed registrations.
// Returns false and retains the managed owner if the lock is still held.
WXDLLIMPEXP_CORE bool
wxMSWOleAbandonExternalShellOwner(
    wxMSWOleExternalShellOperation* operation) noexcept;

// Read-only exact identity query. It never creates a classic-MSW HWND cookie.
WXDLLIMPEXP_CORE unsigned long long
wxMSWOleGetShellHwndGeneration(WXHWND hwnd) noexcept;

WXDLLIMPEXP_CORE wxMSWOleShellHwndIdentity
wxMSWOleCaptureShellHwndIdentity(WXHWND hwnd) noexcept;

// Same capture for an owner about to cross its first native registration
// boundary. On classic MSW it installs the non-zero HWND cookie exactly once.
WXDLLIMPEXP_CORE wxMSWOleShellHwndIdentity
wxMSWOleEnsureShellHwndIdentity(WXHWND hwnd) noexcept;

// Called from WM_DESTROY while the exact HWND/cookie is still observable.
// It makes a bounded attempt to revoke an orphaned owner and balance its
// external lock before releasing the retained COM identity.
WXDLLIMPEXP_CORE void
wxMSWOleRetireShellHwnd(
    const wxMSWOleShellHwndIdentity& identity) noexcept;

// Bounded exact-owner cleanup used when a live HWND is detached/rebound. It
// never retires the HWND cookie and never revokes unless the global ledger can
// still prove the retained COM identity owning this exact generation.
WXDLLIMPEXP_CORE bool
wxMSWOleCleanupShellHwndOwner(
    const wxMSWOleShellHwndIdentity& identity) noexcept;

// Exact post-Register cleanup. Unlike the HWND-wide detach helper, this can
// revoke only the physical owner serial committed by this operation and can
// never remove a newer broker/fixed target on the same live HWND generation.
WXDLLIMPEXP_CORE bool
wxMSWOleCleanupShellDropTargetOwner(
    wxMSWOleShellDropTargetOperation* operation) noexcept;

struct WXDLLIMPEXP_CORE wxMSWOleDropRoute
{
    wxMSWOleDropTargetLease target;

    // Identify the logical owner independently from the target pointer: the
    // same target attached to another window is a different route.
    std::uintptr_t ownerIdentity = 0;
    std::uint64_t ownerGeneration = 0;

    // Already mapped to the HWND logically owning the wxDropTarget.
    POINT clientPoint{ 0, 0 };

    bool IsSameLogicalTarget(const wxMSWOleDropRoute& other) const noexcept;
};

enum class wxMSWOleDropResolveResult
{
    Miss,
    Hit,
    Unstable
};

// The resolver owns all hit-testing and coordinate conversion. Refresh() must
// resolve again at the same screen point and only succeed for the same exact
// route. This is the re-entrancy boundary used after every external callback.
class WXDLLIMPEXP_CORE wxMSWOleDropResolver
{
public:
    virtual ~wxMSWOleDropResolver() = default;

    virtual wxMSWOleDropResolveResult
    Resolve(const POINTL& screenPoint, wxMSWOleDropRoute* route) = 0;

    virtual bool
    Refresh(const wxMSWOleDropRoute& expected,
            const POINTL& screenPoint,
            wxMSWOleDropRoute* refreshed) = 0;
};

// Physical drag-image feedback belongs to the registered COM target (the
// shell HWND or the active member of the WinUI bridge/TLW registration pair),
// not to each logical wxDropTarget traversed below it. Coordinates here are
// always the original screen coordinates.
class WXDLLIMPEXP_CORE wxMSWOleDropFeedback
{
public:
    virtual ~wxMSWOleDropFeedback() = default;

    virtual void DragEnter(const wxMSWOleDropRoute* logicalRoute,
                           IDataObject* data,
                           const POINTL& screenPoint,
                           DWORD effect) noexcept = 0;
    virtual void DragOver(const wxMSWOleDropRoute* logicalRoute,
                          const POINTL& screenPoint,
                          DWORD effect) noexcept = 0;
    virtual void DragLeave() noexcept = 0;
    virtual void Drop(const wxMSWOleDropRoute* logicalRoute,
                      IDataObject* data,
                      const POINTL& screenPoint,
                      DWORD effect) noexcept = 0;
};

enum class wxMSWOleDropSessionMode
{
    // Preserve the historical one-target wrapper's externally observable
    // handling of rejected formats and DragLeave().
    FixedTargetCompatibility,

    // Keep the data object across target-free regions so a later DragOver can
    // enter another logical target below the same physical registration.
    DynamicBroker
};

enum class wxMSWOleDropExceptionPolicy
{
    ReportToApplication,
    SuppressForTest
};

// Shared logical state machine used by the historical one-HWND wrapper and by
// the WinUI per-TLW broker. No exception is allowed to escape these COM
// entry points.
class WXDLLIMPEXP_CORE wxMSWOleDropSession final
{
public:
    wxMSWOleDropSession(wxMSWOleDropResolver& resolver,
                        wxMSWOleDropFeedback& feedback,
                        wxMSWOleDropSessionMode mode =
                            wxMSWOleDropSessionMode::DynamicBroker,
                        wxMSWOleDropExceptionPolicy exceptionPolicy =
                            wxMSWOleDropExceptionPolicy::ReportToApplication);
    ~wxMSWOleDropSession();

    HRESULT DragEnter(IDataObject* data,
                      DWORD keyState,
                      POINTL screenPoint,
                      DWORD* effect);
    HRESULT DragOver(DWORD keyState,
                     POINTL screenPoint,
                     DWORD* effect);
    HRESULT DragLeave();
    HRESULT Drop(IDataObject* data,
                 DWORD keyState,
                 POINTL screenPoint,
                 DWORD* effect);

    // Teardown without a user callback. In particular this is safe from the
    // wxDropTarget destructor and from host shutdown.
    void Reset() noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    wxDECLARE_NO_COPY_CLASS(wxMSWOleDropSession);
};

#endif // wxUSE_OLE && wxUSE_DRAG_AND_DROP

#endif // _WX_MSW_PRIVATE_DROPSESSION_H_
