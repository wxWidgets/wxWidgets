///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ole/droptgt.cpp
// Purpose:     wxDropTarget implementation
// Author:      Vadim Zeitlin
// Created:
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_OLE && wxUSE_DRAG_AND_DROP

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/private/dropsession.h"

#include "wx/msw/wrapshl.h"            // for DROPFILES structure

#include "wx/dnd.h"
#include "wx/scopeguard.h"
#include "wx/thread.h"
#include "wx/weakref.h"
#include "wx/window.h"

#include "wx/private/safecall.h"

#include "wx/msw/ole/oleutils.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#include <initguid.h>
#include <algorithm>
#include <atomic>
#include <limits>
#include <map>
#include <mutex>
#include <utility>
#include <vector>

// Some (very) old SDKs don't define IDropTargetHelper, so define our own
// version of it here.
struct wxIDropTargetHelper : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE DragEnter(HWND hwndTarget,
                                                IDataObject *pDataObject,
                                                POINT *ppt,
                                                DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragLeave() = 0;
    virtual HRESULT STDMETHODCALLTYPE DragOver(POINT *ppt, DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObject,
                                           POINT *ppt,
                                           DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE Show(BOOL fShow) = 0;
};

namespace
{
    DEFINE_GUID(wxCLSID_DragDropHelper,
                0x4657278A,0x411B,0x11D2,0x83,0x9A,0x00,0xC0,0x4F,0xD9,0x18,0xD0);
    DEFINE_GUID(wxIID_IDropTargetHelper,
                0x4657278B,0x411B,0x11D2,0x83,0x9A,0x00,0xC0,0x4F,0xD9,0x18,0xD0);
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDragResult ConvertDragEffectToResult(DWORD dwEffect);
static DWORD ConvertDragResultToEffect(wxDragResult result);
static DWORD GetDropEffect(DWORD flags,
                           wxDragResult defaultAction,
                           DWORD sourceEffects);
static void SetFixedDropTargetHwnd(IDropTarget* target, HWND hwnd) noexcept;

// ============================================================================
// Shared target identity
// ============================================================================

using wxMSWOleShellHwndKey =
    std::pair<HWND, unsigned long long>;

struct wxMSWOleShellHwndState
{
    std::uint64_t lastCommittedOperation = 0;
    std::uint64_t registerReservation = 0;
    unsigned int activeOperations = 0;
    bool registered = false;
    bool globalTracking = false;
};

class wxMSWOleDropTargetState
{
public:
    explicit wxMSWOleDropTargetState(wxDropTarget* target_)
        : target(target_)
    {
    }

    std::mutex mutex;
    wxDropTarget* target;
    std::uint64_t generation = 1;
    wxWeakRef<wxWindow> owner;
    HWND ownerHwnd = nullptr;
    DWORD ownerThread = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    unsigned long long ownerHwndGeneration = 0;
#endif
    std::uint64_t bindingGeneration = 0;
    std::uint64_t shellProjectionEpoch = 0;
    wxMSWOleShellHwndKey shellWrapperKey{nullptr, 0};
    wxMSWOleShellHwndKey shellRegisterReservationKey{nullptr, 0};
    std::uint64_t shellRegisterReservation = 0;
    unsigned int shellOperationDepth = 0;
    std::map<wxMSWOleShellHwndKey, wxMSWOleShellHwndState> shellHwndStates;
};

namespace
{

struct wxMSWOleGlobalShellReservation
{
    std::uint64_t id = 0;
    std::uint64_t operationSerial = 0;
    wxMSWOleShellDropTargetOperationKind kind =
        wxMSWOleShellDropTargetOperationKind::Register;
    IDropTarget* identity = nullptr;
    bool externalLockHeld = false;
    bool externalUnlockClaimed = false;
    bool retireRequested = false;
    bool externalManaged = false;
};

struct wxMSWOleGlobalShellOwner
{
    enum class Kind
    {
        None,
        Fixed,
        ExternalManaged,
        ExternalTombstone
    };

    Kind kind = Kind::None;
    IDropTarget* identity = nullptr;
    std::shared_ptr<wxMSWOleDropTargetState> state;
    std::uint64_t targetGeneration = 0;
    std::uint64_t operationSerial = 0;
    bool registered = false;
    bool externalLockHeld = false;
    bool externalUnlockClaimed = false;
    bool identityRetained = false;
};

enum class wxMSWOlePendingContinuationState
{
    Unarmed,
    Armed,
    Ready,
    Queued,
    DeliveryFailed
};

struct wxMSWOleGlobalShellHwndState
{
    wxMSWOleGlobalShellReservation reservation;
    wxMSWOleGlobalShellOwner owner;
    std::uint64_t pendingExternalId = 0;
    IDropTarget* pendingExternalIdentity = nullptr;
    HWND pendingContinuationHwnd = nullptr;
    unsigned long long pendingContinuationGeneration = 0;
    DWORD pendingContinuationThread = 0;
    UINT_PTR pendingContinuationTimerId = 0;
    wxMSWOlePendingContinuationState pendingContinuationState =
        wxMSWOlePendingContinuationState::Unarmed;
    bool retiring = false;
};

struct wxMSWOlePendingExternalWake
{
    HWND hwnd = nullptr;
    unsigned long long generation = 0;
    DWORD threadId = 0;
    std::uint64_t pendingId = 0;
};

struct DropTargetRegistry
{
    std::mutex mutex;
    std::map<wxDropTarget*, std::weak_ptr<wxMSWOleDropTargetState>> states;
    std::map<wxMSWOleShellHwndKey, wxMSWOleGlobalShellHwndState>
        shellHwndStates;
    std::uint64_t mutationGeneration = 0;
    std::uint64_t shellReservationGeneration = 0;
    std::uint64_t shellOperationGeneration = 0;
};

DropTargetRegistry& GetDropTargetRegistry()
{
    // wxDropTarget instances may belong to static wx objects destroyed after
    // this translation unit's ordinary statics. Keep the private registry
    // alive until process teardown rather than risking destruction-order UAF.
    static DropTargetRegistry* const registry = new DropTargetRegistry;
    return *registry;
}

std::uint64_t NextNonZero(std::uint64_t *value) noexcept
{
    if ( ++*value == 0 )
        ++*value;
    return *value;
}

#if !defined(__WXWINUI__) || !wxUSE_WINUI3

constexpr wchar_t wxMSW_OLE_HWND_GENERATION_PROP[] =
    L"wxWidgets.MSW.OleHwndGeneration."
    L"{F180BD3F-358C-4B5B-8934-53BC5400EA5A}";
std::atomic<ULONG_PTR> gs_nextOleHwndGeneration{0};

#endif

unsigned long long QueryShellHwndGeneration(WXHWND hwnd) noexcept
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    return hwnd ? wxWinUIMSWGetNativeHwndGeneration(hwnd) : 0;
#else
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    return nativeHwnd
        ? static_cast<unsigned long long>(reinterpret_cast<ULONG_PTR>(
              ::GetPropW(nativeHwnd, wxMSW_OLE_HWND_GENERATION_PROP)))
        : 0;
#endif
}

unsigned long long EnsureShellHwndGeneration(WXHWND hwnd) noexcept
{
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    if ( !nativeHwnd || !::IsWindow(nativeHwnd) )
        return 0;
    const DWORD threadId =
        ::GetWindowThreadProcessId(nativeHwnd, nullptr);
    if ( !threadId || threadId != ::GetCurrentThreadId() )
        return 0;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    return wxWinUIMSWGetNativeHwndGeneration(hwnd);
#else
    unsigned long long generation = QueryShellHwndGeneration(hwnd);
    if ( generation )
        return generation;

    do
    {
        generation = static_cast<unsigned long long>(
            ++gs_nextOleHwndGeneration);
    }
    while ( generation == 0 );

    if ( !::SetPropW(
            nativeHwnd,
            wxMSW_OLE_HWND_GENERATION_PROP,
            reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(generation))) )
    {
        return 0;
    }

    return ::IsWindow(nativeHwnd) &&
                   ::GetWindowThreadProcessId(nativeHwnd, nullptr) ==
                       threadId &&
                   QueryShellHwndGeneration(hwnd) == generation
        ? generation
        : 0;
#endif
}

bool IsShellHwndKeyCurrent(const wxMSWOleShellHwndKey& key,
                           DWORD threadId = 0) noexcept
{
    const HWND hwnd = key.first;
    if ( !hwnd || !key.second || !::IsWindow(hwnd) ||
         QueryShellHwndGeneration(reinterpret_cast<WXHWND>(hwnd)) !=
             key.second )
    {
        return false;
    }

    return !threadId ||
           ::GetWindowThreadProcessId(hwnd, nullptr) == threadId;
}

bool IsBindingOwnerCurrentLocked(const wxMSWOleDropTargetState& state)
{
    wxWindow* const owner = state.owner.get();
    if ( !owner || !state.ownerHwnd || !::IsWindow(state.ownerHwnd) ||
         GetHwndOf(owner) != state.ownerHwnd ||
         ::GetWindowThreadProcessId(state.ownerHwnd, nullptr) !=
            state.ownerThread )
    {
        return false;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( wxWinUIMSWGetHwndGeneration(owner, state.ownerHwnd) !=
            state.ownerHwndGeneration )
    {
        return false;
    }
#endif

    return true;
}

std::shared_ptr<wxMSWOleDropTargetState>
GetOrCreateDropTargetStateLocked(wxDropTarget* target)
{
    DropTargetRegistry& registry = GetDropTargetRegistry();
    const auto it = registry.states.find(target);
    if ( it != registry.states.end() )
    {
        if ( const auto state = it->second.lock() )
        {
            std::lock_guard<std::mutex> stateLock(state->mutex);
            if ( state->target == target )
                return state;
        }
    }

    const auto state = std::make_shared<wxMSWOleDropTargetState>(target);
    registry.states[target] = state;
    return state;
}

wxMSWOleShellDropTargetProjection
DeriveShellProjectionLocked(const wxMSWOleDropTargetState& state,
                            WXHWND *projectedHwnd) noexcept
{
    *projectedHwnd = nullptr;
    unsigned int registeredCount = 0;
    for ( const auto& entry : state.shellHwndStates )
    {
        if ( !entry.second.registered ||
             !IsShellHwndKeyCurrent(entry.first) )
        {
            continue;
        }

        ++registeredCount;
        if ( registeredCount == 1 )
        {
            *projectedHwnd =
                reinterpret_cast<WXHWND>(entry.first.first);
        }
        else
        {
            *projectedHwnd = nullptr;
            return wxMSWOleShellDropTargetProjection::Ambiguous;
        }
    }

    return registeredCount == 0
        ? wxMSWOleShellDropTargetProjection::Zero
        : wxMSWOleShellDropTargetProjection::Unique;
}

bool HasOtherLiveRegistrationOrReservationLocked(
    const wxMSWOleDropTargetState& state,
    const wxMSWOleShellHwndKey& requested) noexcept
{
    for ( const auto& entry : state.shellHwndStates )
    {
        if ( entry.first == requested ||
             !IsShellHwndKeyCurrent(entry.first) )
        {
            continue;
        }

        if ( entry.second.registered || entry.second.registerReservation )
            return true;
    }
    return false;
}

bool DepublishFixedOwnerLocked(
    wxMSWOleGlobalShellOwner& owner,
    const wxMSWOleShellHwndKey& key,
    HWND *projectedHwnd) noexcept
{
    *projectedHwnd = nullptr;
    if ( !owner.state )
        return false;

    std::lock_guard<std::mutex> stateLock(owner.state->mutex);
    const auto shell = owner.state->shellHwndStates.find(key);
    if ( shell == owner.state->shellHwndStates.end() )
        return false;

    shell->second.registered = false;
    ++owner.state->shellProjectionEpoch;
    if ( owner.state->shellProjectionEpoch == 0 )
        ++owner.state->shellProjectionEpoch;
    WXHWND projection = nullptr;
    const wxMSWOleShellDropTargetProjection result =
        DeriveShellProjectionLocked(*owner.state, &projection);
    owner.state->shellWrapperKey =
        result == wxMSWOleShellDropTargetProjection::Unique
            ? wxMSWOleShellHwndKey(
                  reinterpret_cast<HWND>(projection),
                  QueryShellHwndGeneration(projection))
            : wxMSWOleShellHwndKey(nullptr, 0);
    *projectedHwnd = result == wxMSWOleShellDropTargetProjection::Unique
        ? reinterpret_cast<HWND>(projection)
        : nullptr;
    return true;
}

struct wxMSWOlePendingTimerCancel
{
    wxMSWOlePendingTimerCancel(UINT_PTR timerId = 0,
                               DWORD ownerThreadId = 0) noexcept
        : id(timerId), threadId(ownerThreadId)
    {
    }

    UINT_PTR id;
    DWORD threadId;
};

wxMSWOlePendingTimerCancel ClearPendingExternalWakeLocked(
    wxMSWOleGlobalShellHwndState& global) noexcept
{
    const wxMSWOlePendingTimerCancel cancel = {
        global.pendingContinuationTimerId,
        global.pendingContinuationThread
    };
    global.pendingContinuationHwnd = nullptr;
    global.pendingContinuationGeneration = 0;
    global.pendingContinuationThread = 0;
    global.pendingContinuationTimerId = 0;
    global.pendingContinuationState =
        wxMSWOlePendingContinuationState::Unarmed;
    return cancel;
}

void CancelPendingExternalTimer(
    const wxMSWOlePendingTimerCancel& cancel) noexcept
{
    // Thread timers are owned by the creating queue. A token destroyed on an
    // invalid foreign thread must not kill an unrelated timer that happens to
    // reuse the same numeric id there; the original callback is one-shot and
    // will self-kill before observing that its exact ledger row is gone.
    if ( cancel.id && cancel.threadId == ::GetCurrentThreadId() )
        ::KillTimer(nullptr, cancel.id);
}

void MarkPendingExternalWakeReadyLocked(
    wxMSWOleGlobalShellHwndState& global) noexcept
{
    if ( global.pendingExternalId && global.pendingContinuationHwnd &&
         global.pendingContinuationGeneration &&
         global.pendingContinuationThread &&
         global.pendingContinuationState ==
             wxMSWOlePendingContinuationState::Armed )
    {
        global.pendingContinuationState =
            wxMSWOlePendingContinuationState::Ready;
    }
}

wxMSWOlePendingExternalWake GetPendingExternalWakeLocked(
    const wxMSWOleGlobalShellHwndState& global) noexcept
{
    wxMSWOlePendingExternalWake wake;
    if ( global.pendingExternalId )
    {
        wake.hwnd = global.pendingContinuationHwnd;
        wake.generation = global.pendingContinuationGeneration;
        wake.threadId = global.pendingContinuationThread;
        wake.pendingId = global.pendingExternalId;
    }
    return wake;
}

bool PostPendingExternalWake(
    const wxMSWOlePendingExternalWake& wake) noexcept
{
    if ( !wake.hwnd || !wake.generation || !wake.threadId ||
         !wake.pendingId ||
         ::GetCurrentThreadId() != wake.threadId ||
         !IsShellHwndKeyCurrent(
             wxMSWOleShellHwndKey(wake.hwnd, wake.generation),
             wake.threadId) )
    {
        return false;
    }

    const UINT message = wxMSWOleGetExternalShellContinuationMessage();
    return message &&
        ::PostMessageW(
            wake.hwnd,
            message,
            static_cast<WPARAM>(
                static_cast<std::uint32_t>(wake.pendingId)),
            static_cast<LPARAM>(
                static_cast<std::uint32_t>(wake.pendingId >> 32))) != FALSE;
}

struct wxMSWOleFixedTailState
{
    unsigned int depth = 0;
};

wxMSWOleFixedTailState& GetFixedTailState() noexcept
{
    static thread_local wxMSWOleFixedTailState state;
    return state;
}

void CALLBACK PendingExternalWakeTimerProc(
    HWND, UINT, UINT_PTR timerId, DWORD);

void SchedulePendingExternalWakeRetry(
    const wxMSWOlePendingExternalWake& wake) noexcept
{
    if ( !wake.pendingId || !wake.hwnd )
        return;

    // A thread timer with a zero id receives a system-allocated identity. It
    // is the bounded idle-desktop fallback for a saturated queue: its callback
    // is dispatched only after this stack returns to the same UI message loop.
    const UINT_PTR timerId =
        ::SetTimer(nullptr, 0, USER_TIMER_MINIMUM,
                   PendingExternalWakeTimerProc);
    bool retainedTimer = false;

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        for ( auto& item : registry.shellHwndStates )
        {
            wxMSWOleGlobalShellHwndState& global = item.second;
            if ( global.pendingExternalId == wake.pendingId &&
                 global.pendingContinuationHwnd == wake.hwnd &&
                 global.pendingContinuationGeneration == wake.generation &&
                 global.pendingContinuationThread == wake.threadId &&
                 global.pendingContinuationState ==
                     wxMSWOlePendingContinuationState::Queued )
            {
                global.pendingContinuationTimerId = timerId;
                global.pendingContinuationState =
                    wxMSWOlePendingContinuationState::DeliveryFailed;
                ++registry.mutationGeneration;
                retainedTimer = timerId != 0;
                break;
            }
        }
    }
    catch ( ... )
    {
    }

    if ( timerId && !retainedTimer )
        ::KillTimer(nullptr, timerId);
}

void CALLBACK PendingExternalWakeTimerProc(
    HWND, UINT, UINT_PTR timerId, DWORD)
{
    if ( !timerId )
        return;

    // Make the periodic Win32 timer one-shot before consulting the ledger.
    ::KillTimer(nullptr, timerId);

    const DWORD currentThread = ::GetCurrentThreadId();
    const bool fixedCallInProgress =
        wxMSWOleIsFixedShellCallInProgress();
    wxMSWOlePendingExternalWake wake;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        for ( auto& item : registry.shellHwndStates )
        {
            wxMSWOleGlobalShellHwndState& global = item.second;
            if ( global.pendingContinuationTimerId == timerId &&
                 global.pendingContinuationThread == currentThread &&
                 global.pendingContinuationState ==
                     wxMSWOlePendingContinuationState::DeliveryFailed )
            {
                global.pendingContinuationTimerId = 0;
                if ( fixedCallInProgress )
                {
                    global.pendingContinuationState =
                        wxMSWOlePendingContinuationState::Ready;
                }
                else
                {
                    wake = GetPendingExternalWakeLocked(global);
                    global.pendingContinuationState =
                        wxMSWOlePendingContinuationState::Queued;
                }
                ++registry.mutationGeneration;
                break;
            }
        }
    }
    catch ( ... )
    {
        return;
    }

    // A timer pumped by an OLE call only marks the exact row Ready. The outer
    // fixed tail owns delivery after every shell frame and mutex has unwound.
    if ( fixedCallInProgress || !wake.pendingId )
        return;

    if ( !PostPendingExternalWake(wake) )
        SchedulePendingExternalWakeRetry(wake);
}

void DrainReadyExternalWakes() noexcept
{
    const DWORD currentThread = ::GetCurrentThreadId();
    for ( ;; )
    {
        wxMSWOlePendingExternalWake wake;
        try
        {
            DropTargetRegistry& registry = GetDropTargetRegistry();
            std::lock_guard<std::mutex> lock(registry.mutex);
            for ( auto& item : registry.shellHwndStates )
            {
                wxMSWOleGlobalShellHwndState& global = item.second;
                if ( global.pendingContinuationState !=
                         wxMSWOlePendingContinuationState::Ready ||
                     global.pendingContinuationThread != currentThread )
                {
                    continue;
                }

                wake = GetPendingExternalWakeLocked(global);
                global.pendingContinuationState =
                    wxMSWOlePendingContinuationState::Queued;
                ++registry.mutationGeneration;
                break;
            }
        }
        catch ( ... )
        {
            return;
        }

        if ( !wake.pendingId )
            return;

        if ( !PostPendingExternalWake(wake) )
            SchedulePendingExternalWakeRetry(wake);
    }
}

void EnterFixedOperationTail() noexcept
{
    ++GetFixedTailState().depth;
}

void LeaveFixedOperationTail() noexcept
{
    wxMSWOleFixedTailState& tail = GetFixedTailState();
    if ( tail.depth == 0 )
        return;

    --tail.depth;
    if ( tail.depth != 0 )
        return;

    DrainReadyExternalWakes();
}

class wxMSWOleFixedPublicCallScope final
{
public:
    wxMSWOleFixedPublicCallScope() noexcept
    {
        EnterFixedOperationTail();
    }

    ~wxMSWOleFixedPublicCallScope()
    {
        LeaveFixedOperationTail();
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxMSWOleFixedPublicCallScope);
};

} // anonymous namespace

UINT wxMSWOleGetExternalShellContinuationMessage() noexcept
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.MSW.OleExternalShellContinuation."
        L"{EA62F718-CC39-4E31-905B-C27B5A245B43}");
    return message;
}

bool wxMSWOleIsFixedShellCallInProgress() noexcept
{
    return GetFixedTailState().depth != 0;
}

bool wxMSWOleDeferExternalShellContinuation(
    std::uint64_t pendingId,
    WXHWND wakeHwnd) noexcept
{
    if ( !pendingId || !wakeHwnd )
        return false;

    const wxMSWOleShellHwndIdentity wakeIdentity =
        wxMSWOleCaptureShellHwndIdentity(wakeHwnd);
    if ( !wakeIdentity.hwnd || !wakeIdentity.generation ||
         !wakeIdentity.threadId ||
         ::GetCurrentThreadId() != wakeIdentity.threadId )
    {
        return false;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        for ( auto& item : registry.shellHwndStates )
        {
            wxMSWOleGlobalShellHwndState& global = item.second;
            if ( global.pendingExternalId == pendingId &&
                 global.pendingContinuationHwnd ==
                     reinterpret_cast<HWND>(wakeIdentity.hwnd) &&
                 global.pendingContinuationGeneration ==
                     wakeIdentity.generation &&
                 global.pendingContinuationThread == wakeIdentity.threadId &&
                 global.pendingContinuationState ==
                     wxMSWOlePendingContinuationState::Queued )
            {
                global.pendingContinuationState =
                    wxMSWOlePendingContinuationState::Ready;
                ++registry.mutationGeneration;
                return true;
            }
        }
    }
    catch ( ... )
    {
        return false;
    }
    return false;
}

bool wxMSWOleClaimExternalShellContinuation(
    std::uint64_t pendingId,
    WXHWND wakeHwnd) noexcept
{
    if ( !pendingId || !wakeHwnd ||
         wxMSWOleIsFixedShellCallInProgress() )
    {
        return false;
    }

    const wxMSWOleShellHwndIdentity wakeIdentity =
        wxMSWOleCaptureShellHwndIdentity(wakeHwnd);
    if ( !wakeIdentity.hwnd || !wakeIdentity.generation ||
         !wakeIdentity.threadId ||
         ::GetCurrentThreadId() != wakeIdentity.threadId )
    {
        return false;
    }

    wxMSWOlePendingTimerCancel timerCancel;
    bool claimed = false;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        for ( auto& item : registry.shellHwndStates )
        {
            wxMSWOleGlobalShellHwndState& global = item.second;
            if ( global.pendingExternalId == pendingId &&
                 global.pendingContinuationHwnd ==
                     reinterpret_cast<HWND>(wakeIdentity.hwnd) &&
                 global.pendingContinuationGeneration ==
                     wakeIdentity.generation &&
                 global.pendingContinuationThread == wakeIdentity.threadId &&
                 global.pendingContinuationState ==
                     wxMSWOlePendingContinuationState::Queued )
            {
                timerCancel = ClearPendingExternalWakeLocked(global);
                ++registry.mutationGeneration;
                claimed = true;
                break;
            }
        }
    }
    catch ( ... )
    {
        return false;
    }
    CancelPendingExternalTimer(timerCancel);
    return claimed;
}

bool wxMSWOleRetryFailedExternalShellContinuation(
    WXHWND wakeHwnd) noexcept
{
    if ( !wakeHwnd )
        return false;
    const bool fixedCallInProgress =
        wxMSWOleIsFixedShellCallInProgress();

    const wxMSWOleShellHwndIdentity wakeIdentity =
        wxMSWOleCaptureShellHwndIdentity(wakeHwnd);
    if ( !wakeIdentity.hwnd || !wakeIdentity.generation ||
         !wakeIdentity.threadId ||
         ::GetCurrentThreadId() != wakeIdentity.threadId )
    {
        return false;
    }

    wxMSWOlePendingExternalWake wake;
    UINT_PTR timerId = 0;
    bool madeReady = false;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        for ( auto& item : registry.shellHwndStates )
        {
            wxMSWOleGlobalShellHwndState& global = item.second;
            if ( global.pendingContinuationHwnd ==
                     reinterpret_cast<HWND>(wakeIdentity.hwnd) &&
                 global.pendingContinuationGeneration ==
                     wakeIdentity.generation &&
                 global.pendingContinuationThread == wakeIdentity.threadId &&
                 global.pendingContinuationState ==
                     wxMSWOlePendingContinuationState::DeliveryFailed )
            {
                timerId = global.pendingContinuationTimerId;
                global.pendingContinuationTimerId = 0;
                if ( fixedCallInProgress )
                {
                    global.pendingContinuationState =
                        wxMSWOlePendingContinuationState::Ready;
                    ++registry.mutationGeneration;
                    madeReady = true;
                    break;
                }
                wake = GetPendingExternalWakeLocked(global);
                global.pendingContinuationState =
                    wxMSWOlePendingContinuationState::Queued;
                ++registry.mutationGeneration;
                break;
            }
        }
    }
    catch ( ... )
    {
        return false;
    }

    if ( timerId )
        ::KillTimer(nullptr, timerId);
    if ( madeReady )
        return true;

    if ( !wake.pendingId )
        return false;
    if ( PostPendingExternalWake(wake) )
        return true;

    SchedulePendingExternalWakeRetry(wake);
    return false;
}

unsigned long long
wxMSWOleGetShellHwndGeneration(WXHWND hwnd) noexcept
{
    return QueryShellHwndGeneration(hwnd);
}

wxMSWOleShellHwndIdentity
wxMSWOleCaptureShellHwndIdentity(WXHWND hwnd) noexcept
{
    wxMSWOleShellHwndIdentity identity;
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    if ( !nativeHwnd || !::IsWindow(nativeHwnd) )
        return identity;

    identity.hwnd = hwnd;
    identity.generation = QueryShellHwndGeneration(hwnd);
    identity.threadId =
        ::GetWindowThreadProcessId(nativeHwnd, nullptr);
    if ( !identity.generation || !identity.threadId )
        return {};

    return identity;
}

wxMSWOleShellHwndIdentity
wxMSWOleEnsureShellHwndIdentity(WXHWND hwnd) noexcept
{
    wxMSWOleShellHwndIdentity identity;
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    if ( !nativeHwnd || !::IsWindow(nativeHwnd) )
        return identity;
    identity.threadId =
        ::GetWindowThreadProcessId(nativeHwnd, nullptr);
    if ( !identity.threadId || identity.threadId != ::GetCurrentThreadId() )
        return {};
    identity.hwnd = hwnd;
    identity.generation = EnsureShellHwndGeneration(hwnd);
    if ( !identity.generation )
        return {};
    return identity;
}

wxMSWOleDropTargetLease::wxMSWOleDropTargetLease(
    const std::shared_ptr<wxMSWOleDropTargetState>& state,
    std::uint64_t generation) noexcept
    : m_state(state),
      m_generation(generation)
{
}

bool wxMSWOleDropTargetLease::IsCurrent() const noexcept
{
    return GetIfCurrent() != nullptr;
}

wxDropTarget* wxMSWOleDropTargetLease::GetIfCurrent() const noexcept
{
    if ( !m_state )
        return nullptr;

    std::lock_guard<std::mutex> lock(m_state->mutex);
    if ( m_state->generation != m_generation )
        return nullptr;

    return m_state->target;
}

bool wxMSWOleDropTargetLease::IsSameLifetime(
    const wxMSWOleDropTargetLease& other) const noexcept
{
    return m_state == other.m_state && m_generation == other.m_generation;
}

wxMSWOleDropTargetLease
wxMSWOleAcquireDropTarget(wxDropTarget* target) noexcept
{
    if ( !target )
        return {};

#if wxUSE_EXCEPTIONS
    try
    {
#endif
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> registryLock(registry.mutex);

        const auto state = GetOrCreateDropTargetStateLocked(target);
        std::lock_guard<std::mutex> stateLock(state->mutex);
        return wxMSWOleDropTargetLease(state, state->generation);
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return {};
    }
#endif
}

void wxMSWOleInvalidateDropTarget(wxDropTarget* target) noexcept
{
    if ( !target )
        return;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return;

    if ( const auto state = it->second.lock() )
    {
        std::lock_guard<std::mutex> stateLock(state->mutex);
        if ( state->target == target )
        {
            state->owner.Release();
            state->ownerHwnd = nullptr;
            state->ownerThread = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            state->ownerHwndGeneration = 0;
#endif
            ++state->bindingGeneration;
            state->target = nullptr;
            ++state->generation;
            ++registry.mutationGeneration;
        }
    }

    registry.states.erase(it);
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        // All leases fail closed if invalidation cannot take the registry
        // lock. This can only happen during catastrophic process teardown.
    }
#endif
}

wxMSWOleDropTargetBinding::wxMSWOleDropTargetBinding(
    const std::shared_ptr<wxMSWOleDropTargetState>& state,
    std::uint64_t targetGeneration,
    std::uint64_t bindingGeneration) noexcept
    : m_state(state),
      m_targetGeneration(targetGeneration),
      m_bindingGeneration(bindingGeneration)
{
}

bool wxMSWOleDropTargetBinding::IsCurrent() const noexcept
{
    if ( !m_state )
        return false;

    std::lock_guard<std::mutex> lock(m_state->mutex);
    return m_state->target != nullptr &&
           m_state->generation == m_targetGeneration &&
           m_state->bindingGeneration == m_bindingGeneration &&
           IsBindingOwnerCurrentLocked(*m_state);
}

wxDropTarget*
wxMSWOleDropTargetBinding::GetTargetIfCurrent() const noexcept
{
    if ( !m_state )
        return nullptr;

    std::lock_guard<std::mutex> lock(m_state->mutex);
    if ( m_state->generation != m_targetGeneration ||
         m_state->bindingGeneration != m_bindingGeneration ||
         !IsBindingOwnerCurrentLocked(*m_state) )
    {
        return nullptr;
    }

    return m_state->target;
}

wxWindow* wxMSWOleDropTargetBinding::GetOwnerIfCurrent() const noexcept
{
    if ( !m_state )
        return nullptr;

    std::lock_guard<std::mutex> lock(m_state->mutex);
    if ( m_state->generation != m_targetGeneration ||
         m_state->bindingGeneration != m_bindingGeneration ||
         !m_state->target ||
         !IsBindingOwnerCurrentLocked(*m_state) )
    {
        return nullptr;
    }

    return m_state->owner.get();
}

WXHWND
wxMSWOleDropTargetBinding::GetOwnerHwndIfCurrent() const noexcept
{
    if ( !m_state )
        return nullptr;

    std::lock_guard<std::mutex> lock(m_state->mutex);
    if ( m_state->generation != m_targetGeneration ||
         m_state->bindingGeneration != m_bindingGeneration ||
         !m_state->target ||
         !IsBindingOwnerCurrentLocked(*m_state) )
    {
        return nullptr;
    }

    return reinterpret_cast<WXHWND>(m_state->ownerHwnd);
}

wxMSWOleDropTargetLease
wxMSWOleDropTargetBinding::GetTargetLease() const noexcept
{
    return wxMSWOleAcquireDropTarget(GetTargetIfCurrent());
}

bool wxMSWOleDropTargetBinding::IsSameBinding(
    const wxMSWOleDropTargetBinding& other) const noexcept
{
    return m_state == other.m_state &&
           m_targetGeneration == other.m_targetGeneration &&
           m_bindingGeneration == other.m_bindingGeneration;
}

void wxMSWOleBindDropTarget(wxWindow* owner,
                            wxDropTarget* target,
                            WXHWND ownerHwnd) noexcept
{
    if ( !owner || !target || !ownerHwnd )
        return;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> registryLock(registry.mutex);
        const auto state = GetOrCreateDropTargetStateLocked(target);
        std::lock_guard<std::mutex> stateLock(state->mutex);
        state->owner = owner;
        state->ownerHwnd = reinterpret_cast<HWND>(ownerHwnd);
        state->ownerThread =
            ::GetWindowThreadProcessId(state->ownerHwnd, nullptr);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        state->ownerHwndGeneration =
            wxWinUIMSWGetHwndGeneration(owner, ownerHwnd);
#endif
        ++state->bindingGeneration;
        ++registry.mutationGeneration;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        // Lookup will fail closed if the registry couldn't be populated.
    }
#endif
}

void wxMSWOleUnbindDropTarget(wxWindow* owner,
                              wxDropTarget* target) noexcept
{
    if ( !owner || !target )
        return;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return;

    if ( const auto state = it->second.lock() )
    {
        std::lock_guard<std::mutex> stateLock(state->mutex);
        if ( state->target == target && state->owner.get() == owner )
        {
            state->owner.Release();
            state->ownerHwnd = nullptr;
            state->ownerThread = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            state->ownerHwndGeneration = 0;
#endif
            ++state->bindingGeneration;
            ++registry.mutationGeneration;
        }
    }
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
    }
#endif
}

wxMSWOleDropTargetLookup
wxMSWOleLookupDropTarget(wxWindow* queried,
                         wxMSWOleDropTargetBinding* binding) noexcept
{
    if ( binding )
        *binding = {};
    if ( !queried || !binding )
        return wxMSWOleDropTargetLookup::Unstable;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
        const wxWeakRef<wxWindow> queriedLifetime(queried);
        wxWindow* const parentBefore = queried->GetParent();
        const std::uint64_t before =
            wxMSWOleGetDropTargetMutationGeneration();
        wxDropTarget* const target = queried->GetDropTarget();
        wxWindow* const stillAlive = queriedLifetime.get();
        if ( !stillAlive || stillAlive != queried ||
             stillAlive->GetParent() != parentBefore ||
             before != wxMSWOleGetDropTargetMutationGeneration() )
        {
            return wxMSWOleDropTargetLookup::Unstable;
        }

        if ( !target )
            return wxMSWOleDropTargetLookup::None;

        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> registryLock(registry.mutex);
        const auto it = registry.states.find(target);
        if ( it == registry.states.end() )
            return wxMSWOleDropTargetLookup::Unstable;

        const auto state = it->second.lock();
        if ( !state )
            return wxMSWOleDropTargetLookup::Unstable;

        std::lock_guard<std::mutex> stateLock(state->mutex);
        if ( state->target != target ||
             state->shellOperationDepth != 0 ||
             !IsBindingOwnerCurrentLocked(*state) )
        {
            return wxMSWOleDropTargetLookup::Unstable;
        }

        *binding = wxMSWOleDropTargetBinding(
            state, state->generation, state->bindingGeneration);
        return wxMSWOleDropTargetLookup::Found;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return wxMSWOleDropTargetLookup::Unstable;
    }
#endif
}

std::uint64_t wxMSWOleGetDropTargetMutationGeneration() noexcept
{
#if wxUSE_EXCEPTIONS
    try
    {
#endif
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        return registry.mutationGeneration;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        // A changing generation is the fail-closed answer for callers that
        // cannot inspect the registry.
        return std::numeric_limits<std::uint64_t>::max();
    }
#endif
}

bool wxMSWOleHasDropTargetState(wxDropTarget* target) noexcept
{
    if ( !target )
        return false;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return false;

    const auto state = it->second.lock();
    if ( !state )
        return false;

    std::lock_guard<std::mutex> stateLock(state->mutex);
    return state->target == target;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

bool wxMSWOleIsShellDropTargetRegistered(wxDropTarget* target,
                                         WXHWND hwnd) noexcept
{
    if ( !target || !hwnd )
        return false;
    const unsigned long long hwndGeneration =
        QueryShellHwndGeneration(hwnd);
    if ( !hwndGeneration )
        return false;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return false;

    const auto state = it->second.lock();
    if ( !state )
        return false;

    std::lock_guard<std::mutex> stateLock(state->mutex);
    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(hwnd), hwndGeneration);
    const auto shell = state->shellHwndStates.find(key);
    if ( state->target != target ||
         shell == state->shellHwndStates.end() ||
         !shell->second.registered )
    {
        return false;
    }

    if ( !shell->second.globalTracking )
        return true;

    const auto global = registry.shellHwndStates.find(key);
    return global != registry.shellHwndStates.end() &&
           global->second.owner.registered &&
           global->second.owner.state == state &&
           global->second.owner.targetGeneration == state->generation;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

bool wxMSWOleIsShellDropTargetOperationInFlight(wxDropTarget* target,
                                                WXHWND hwnd) noexcept
{
    if ( !target || !hwnd )
        return false;
    const unsigned long long hwndGeneration =
        QueryShellHwndGeneration(hwnd);
    if ( !hwndGeneration )
        return false;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return false;

    const auto state = it->second.lock();
    if ( !state )
        return false;

    std::lock_guard<std::mutex> stateLock(state->mutex);
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    const auto operation = state->shellHwndStates.find(
        std::make_pair(nativeHwnd, hwndGeneration));
    return state->target == target &&
           operation != state->shellHwndStates.end() &&
           operation->second.activeOperations != 0;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

wxMSWOleShellDropTargetOperation::wxMSWOleShellDropTargetOperation(
    const std::shared_ptr<wxMSWOleDropTargetState>& state,
    std::uint64_t targetGeneration,
    WXHWND hwnd,
    unsigned long long hwndGeneration,
    unsigned long threadId,
    std::uint64_t reservationId,
    std::uint64_t baselineProjectionEpoch,
    wxMSWOleShellDropTargetOperationKind kind,
    IDropTarget* comIdentity,
    bool globalTracking) noexcept
    : m_state(state),
      m_targetGeneration(targetGeneration),
      m_reservationId(reservationId),
      m_baselineProjectionEpoch(baselineProjectionEpoch),
      m_hwnd(hwnd),
      m_hwndGeneration(hwndGeneration),
      m_threadId(threadId),
      m_kind(kind),
      m_comIdentity(comIdentity),
      m_globalTracking(globalTracking),
      m_active(true)
{
    if ( m_globalTracking )
        EnterFixedOperationTail();
}

wxMSWOleShellDropTargetOperation::~wxMSWOleShellDropTargetOperation()
{
    if ( m_active )
        (void)wxMSWOleCancelShellDropTargetOperation(this);

    IDropTarget * const deferredRelease = m_deferredRelease;
    const bool fixedTailLease = m_globalTracking;

    // No member may be observed after the COM Release() below: it can pump a
    // callback which destroys the object containing this token.
    m_deferredRelease = nullptr;
    m_globalTracking = false;

    if ( deferredRelease )
        deferredRelease->Release();
    if ( fixedTailLease )
        LeaveFixedOperationTail();
}

wxMSWOleShellDropTargetOperation::wxMSWOleShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation&& other) noexcept
    : m_state(std::move(other.m_state)),
      m_targetGeneration(other.m_targetGeneration),
      m_operationGeneration(other.m_operationGeneration),
      m_reservationId(other.m_reservationId),
      m_baselineProjectionEpoch(other.m_baselineProjectionEpoch),
      m_projectionEpoch(other.m_projectionEpoch),
      m_globalOwnerSerial(other.m_globalOwnerSerial),
      m_compensationReservationId(other.m_compensationReservationId),
      m_hwnd(other.m_hwnd),
      m_projectedHwnd(other.m_projectedHwnd),
      m_hwndGeneration(other.m_hwndGeneration),
      m_threadId(other.m_threadId),
      m_kind(other.m_kind),
      m_projection(other.m_projection),
      m_comIdentity(other.m_comIdentity),
      m_deferredRelease(other.m_deferredRelease),
      m_globalTracking(other.m_globalTracking),
      m_externalLockAcquired(other.m_externalLockAcquired),
      m_externalUnlockClaimed(other.m_externalUnlockClaimed),
      m_active(other.m_active)
{
    other.m_targetGeneration = 0;
    other.m_operationGeneration = 0;
    other.m_reservationId = 0;
    other.m_baselineProjectionEpoch = 0;
    other.m_projectionEpoch = 0;
    other.m_globalOwnerSerial = 0;
    other.m_compensationReservationId = 0;
    other.m_hwnd = nullptr;
    other.m_projectedHwnd = nullptr;
    other.m_hwndGeneration = 0;
    other.m_threadId = 0;
    other.m_projection = wxMSWOleShellDropTargetProjection::Unchanged;
    other.m_comIdentity = nullptr;
    other.m_deferredRelease = nullptr;
    other.m_globalTracking = false;
    other.m_externalLockAcquired = false;
    other.m_externalUnlockClaimed = false;
    other.m_active = false;
}

wxMSWOleShellDropTargetOperation&
wxMSWOleShellDropTargetOperation::operator=(
    wxMSWOleShellDropTargetOperation&& other) noexcept
{
    if ( this == &other )
        return *this;

    // Swapping is deliberately callback-free. The old state moves into the
    // source token and is finalized by its own destructor outside this
    // assignment, so a COM Release can never invalidate `this` mid-write.
    using std::swap;
    swap(m_state, other.m_state);
    swap(m_targetGeneration, other.m_targetGeneration);
    swap(m_operationGeneration, other.m_operationGeneration);
    swap(m_reservationId, other.m_reservationId);
    swap(m_baselineProjectionEpoch, other.m_baselineProjectionEpoch);
    swap(m_projectionEpoch, other.m_projectionEpoch);
    swap(m_globalOwnerSerial, other.m_globalOwnerSerial);
    swap(m_compensationReservationId, other.m_compensationReservationId);
    swap(m_hwnd, other.m_hwnd);
    swap(m_projectedHwnd, other.m_projectedHwnd);
    swap(m_hwndGeneration, other.m_hwndGeneration);
    swap(m_threadId, other.m_threadId);
    swap(m_kind, other.m_kind);
    swap(m_projection, other.m_projection);
    swap(m_comIdentity, other.m_comIdentity);
    swap(m_deferredRelease, other.m_deferredRelease);
    swap(m_globalTracking, other.m_globalTracking);
    swap(m_externalLockAcquired, other.m_externalLockAcquired);
    swap(m_externalUnlockClaimed, other.m_externalUnlockClaimed);
    swap(m_active, other.m_active);
    return *this;
}

wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(wxDropTarget* target,
                                      WXHWND hwnd) noexcept
{
    return wxMSWOleBeginShellDropTargetOperation(
        target, hwnd, wxMSWOleShellDropTargetOperationKind::Register);
}

wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(
    wxDropTarget* target,
    WXHWND hwnd,
    wxMSWOleShellDropTargetOperationKind kind) noexcept
{
    return wxMSWOleBeginShellDropTargetOperation(
        target, hwnd, kind, nullptr);
}

wxMSWOleShellDropTargetOperation
wxMSWOleBeginShellDropTargetOperation(
    wxDropTarget* target,
    WXHWND hwnd,
    wxMSWOleShellDropTargetOperationKind kind,
    IDropTarget* comIdentity) noexcept
{
    if ( !target || !hwnd )
        return {};
    const HWND nativeHwnd = reinterpret_cast<HWND>(hwnd);
    const DWORD threadId = ::GetCurrentThreadId();
    if ( !::IsWindow(nativeHwnd) ||
         ::GetWindowThreadProcessId(nativeHwnd, nullptr) != threadId )
    {
        return {};
    }
    const unsigned long long hwndGeneration =
        EnsureShellHwndGeneration(hwnd);
    if ( hwndGeneration == 0 )
        return {};
    const wxMSWOleShellHwndKey key(nativeHwnd, hwndGeneration);
    const bool globalTracking = comIdentity != nullptr;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    const auto it = registry.states.find(target);
    if ( it == registry.states.end() )
        return {};

    const auto state = it->second.lock();
    if ( !state )
        return {};

    std::lock_guard<std::mutex> stateLock(state->mutex);
    if ( state->target != target )
        return {};

    wxMSWOleShellHwndState& shell =
        state->shellHwndStates[key];
    if ( kind == wxMSWOleShellDropTargetOperationKind::Register )
    {
        if ( shell.registered || shell.registerReservation ||
             state->shellRegisterReservation ||
             HasOtherLiveRegistrationOrReservationLocked(*state, key) )
        {
            return {};
        }
    }
    else if ( !shell.registered || state->shellRegisterReservation )
    {
        return {};
    }

    wxMSWOleGlobalShellHwndState *global = nullptr;
    if ( globalTracking )
    {
        global = &registry.shellHwndStates[key];
        if ( global->retiring || global->reservation.id ||
             (kind == wxMSWOleShellDropTargetOperationKind::Register &&
              global->pendingExternalId) )
            return {};

        if ( kind == wxMSWOleShellDropTargetOperationKind::Register )
        {
            if ( global->owner.registered || global->owner.identity )
                return {};
        }
        else if ( !global->owner.registered ||
                  global->owner.kind !=
                      wxMSWOleGlobalShellOwner::Kind::Fixed ||
                  global->owner.identity != comIdentity ||
                  global->owner.state != state ||
                  global->owner.targetGeneration != state->generation )
        {
            return {};
        }
    }

    const std::uint64_t reservationId =
        NextNonZero(&registry.shellReservationGeneration);
    ++state->shellOperationDepth;
    ++shell.activeOperations;
    if ( kind == wxMSWOleShellDropTargetOperationKind::Register )
    {
        shell.registerReservation = reservationId;
        state->shellRegisterReservation = reservationId;
        state->shellRegisterReservationKey = key;
    }
    if ( global )
    {
        global->reservation.id = reservationId;
        global->reservation.kind = kind;
        global->reservation.identity = comIdentity;
        global->reservation.externalManaged = false;
        if ( kind == wxMSWOleShellDropTargetOperationKind::Revoke )
        {
            global->reservation.externalLockHeld =
                global->owner.externalLockHeld;
            global->reservation.externalUnlockClaimed =
                global->owner.externalUnlockClaimed;
        }
    }
    ++registry.mutationGeneration;
    wxMSWOleShellDropTargetOperation operation(
        state, state->generation, hwnd, hwndGeneration, threadId,
        reservationId, state->shellProjectionEpoch, kind, comIdentity,
        globalTracking);
    if ( global &&
         kind == wxMSWOleShellDropTargetOperationKind::Revoke )
    {
        operation.m_globalOwnerSerial =
            global->owner.operationSerial;
        operation.m_externalLockAcquired =
            global->reservation.externalLockHeld;
        operation.m_externalUnlockClaimed =
            global->reservation.externalUnlockClaimed;
    }
    return operation;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return {};
    }
#endif
}

bool wxMSWOleArmShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || !operation->m_active ||
         operation->m_operationGeneration != 0 || !operation->m_state )
    {
        return false;
    }

    const HWND hwnd = reinterpret_cast<HWND>(operation->m_hwnd);
    if ( ::GetCurrentThreadId() != operation->m_threadId ||
         !::IsWindow(hwnd) ||
         ::GetWindowThreadProcessId(hwnd, nullptr) !=
            operation->m_threadId ||
         QueryShellHwndGeneration(operation->m_hwnd) !=
            operation->m_hwndGeneration )
    {
        return false;
    }

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    std::lock_guard<std::mutex> stateLock(operation->m_state->mutex);
    if ( !operation->m_state->target ||
         operation->m_state->generation != operation->m_targetGeneration )
    {
        return false;
    }
    const auto shell = operation->m_state->shellHwndStates.find(
        std::make_pair(hwnd, operation->m_hwndGeneration));
    if ( shell == operation->m_state->shellHwndStates.end() ||
         shell->second.activeOperations == 0 ||
         (operation->m_kind ==
              wxMSWOleShellDropTargetOperationKind::Register &&
          (shell->second.registerReservation != operation->m_reservationId ||
           operation->m_state->shellRegisterReservation !=
               operation->m_reservationId)) )
    {
        return false;
    }

    operation->m_operationGeneration =
        NextNonZero(&registry.shellOperationGeneration);
    if ( operation->m_globalTracking )
    {
        const auto global = registry.shellHwndStates.find(
            std::make_pair(hwnd, operation->m_hwndGeneration));
        if ( global == registry.shellHwndStates.end() ||
             global->second.reservation.id != operation->m_reservationId ||
             global->second.reservation.identity != operation->m_comIdentity ||
             global->second.reservation.kind != operation->m_kind ||
             (operation->m_kind ==
                  wxMSWOleShellDropTargetOperationKind::Register &&
              (global->second.retiring ||
               global->second.reservation.retireRequested)) )
        {
            operation->m_operationGeneration = 0;
            return false;
        }
        global->second.reservation.operationSerial =
            operation->m_operationGeneration;
    }
    ++registry.mutationGeneration;
    return true;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

bool wxMSWOleCompleteShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation,
    bool registered) noexcept
{
    if ( !operation || !operation->m_active || !operation->m_state ||
         !operation->m_hwnd || operation->m_operationGeneration == 0 )
    {
        return false;
    }

    if ( registered !=
            (operation->m_kind ==
                wxMSWOleShellDropTargetOperationKind::Register) )
    {
        return false;
    }

    IDropTarget *retainedCandidate = nullptr;
    if ( operation->m_globalTracking && registered )
    {
        retainedCandidate = operation->m_comIdentity;
        retainedCandidate->AddRef();
    }

    const HWND boundaryHwnd =
        reinterpret_cast<HWND>(operation->m_hwnd);
    const bool boundaryCurrent =
        ::GetCurrentThreadId() == operation->m_threadId &&
        ::IsWindow(boundaryHwnd) &&
        ::GetWindowThreadProcessId(boundaryHwnd, nullptr) ==
            operation->m_threadId &&
        QueryShellHwndGeneration(operation->m_hwnd) ==
            operation->m_hwndGeneration;

    bool publishProjection = false;
#if wxUSE_EXCEPTIONS
    try
    {
#endif
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> registryLock(registry.mutex);
        std::lock_guard<std::mutex> stateLock(operation->m_state->mutex);

        if ( operation->m_state->shellOperationDepth != 0 )
            --operation->m_state->shellOperationDepth;
        const wxMSWOleShellHwndKey key(
            boundaryHwnd, operation->m_hwndGeneration);
        const auto shell = operation->m_state->shellHwndStates.find(key);
        if ( shell != operation->m_state->shellHwndStates.end() &&
             shell->second.activeOperations != 0 )
        {
            --shell->second.activeOperations;
        }

        bool reservationCurrent =
            shell != operation->m_state->shellHwndStates.end();
        if ( operation->m_kind ==
                wxMSWOleShellDropTargetOperationKind::Register )
        {
            reservationCurrent = reservationCurrent &&
                shell->second.registerReservation ==
                    operation->m_reservationId &&
                operation->m_state->shellRegisterReservation ==
                    operation->m_reservationId;
            if ( reservationCurrent )
            {
                shell->second.registerReservation = 0;
                operation->m_state->shellRegisterReservation = 0;
                operation->m_state->shellRegisterReservationKey =
                    wxMSWOleShellHwndKey(nullptr, 0);
            }
        }

        bool globalCommit = !operation->m_globalTracking;
        if ( operation->m_globalTracking )
        {
            const auto global = registry.shellHwndStates.find(key);
            globalCommit = global != registry.shellHwndStates.end() &&
                global->second.reservation.id ==
                    operation->m_reservationId &&
                global->second.reservation.identity ==
                    operation->m_comIdentity &&
                global->second.reservation.kind == operation->m_kind;
            if ( globalCommit )
            {
                const bool retireRequested =
                    global->second.reservation.retireRequested;
                if ( registered )
                {
                    globalCommit = !global->second.owner.identity;
                    if ( globalCommit )
                    {
                        global->second.owner.identity = retainedCandidate;
                        retainedCandidate = nullptr;
                        global->second.owner.kind =
                            wxMSWOleGlobalShellOwner::Kind::Fixed;
                        global->second.owner.state = operation->m_state;
                        global->second.owner.targetGeneration =
                            operation->m_targetGeneration;
                        global->second.owner.operationSerial =
                            operation->m_operationGeneration;
                        global->second.owner.registered = true;
                        global->second.owner.externalLockHeld =
                            global->second.reservation.externalLockHeld;
                        global->second.owner.externalUnlockClaimed =
                            global->second.reservation.externalUnlockClaimed;
                        global->second.owner.identityRetained = true;
                        operation->m_globalOwnerSerial =
                            operation->m_operationGeneration;
                    }
                }
                else
                {
                    globalCommit = global->second.owner.registered &&
                        global->second.owner.kind ==
                            wxMSWOleGlobalShellOwner::Kind::Fixed &&
                        global->second.owner.identity ==
                            operation->m_comIdentity &&
                        global->second.owner.state == operation->m_state &&
                        global->second.owner.targetGeneration ==
                            operation->m_targetGeneration;
                    if ( globalCommit )
                    {
                        global->second.owner.registered = false;
                        global->second.owner.operationSerial =
                            operation->m_operationGeneration;
                        operation->m_globalOwnerSerial =
                            operation->m_operationGeneration;
                        if ( !global->second.owner.externalLockHeld )
                        {
                            operation->m_deferredRelease =
                                global->second.owner.identity;
                            global->second.owner =
                                wxMSWOleGlobalShellOwner();
                        }
                    }
                }
                global->second.reservation =
                    wxMSWOleGlobalShellReservation();
                if ( retireRequested )
                    global->second.retiring = true;
                MarkPendingExternalWakeReadyLocked(global->second);
            }
        }

        const bool exactLifetime =
            operation->m_state->target != nullptr &&
            operation->m_state->generation ==
                operation->m_targetGeneration;
        const bool shellCommit = exactLifetime && reservationCurrent &&
            globalCommit &&
            shell != operation->m_state->shellHwndStates.end() &&
            operation->m_operationGeneration >
                shell->second.lastCommittedOperation;
        if ( shellCommit )
        {
            shell->second.registered = registered;
            shell->second.globalTracking = operation->m_globalTracking;
            shell->second.lastCommittedOperation =
                operation->m_operationGeneration;
            ++operation->m_state->shellProjectionEpoch;
            if ( operation->m_state->shellProjectionEpoch == 0 )
                ++operation->m_state->shellProjectionEpoch;
            operation->m_projectionEpoch =
                operation->m_state->shellProjectionEpoch;
            operation->m_projection = DeriveShellProjectionLocked(
                *operation->m_state, &operation->m_projectedHwnd);
            operation->m_state->shellWrapperKey =
                operation->m_projection ==
                    wxMSWOleShellDropTargetProjection::Unique
                    ? wxMSWOleShellHwndKey(
                          reinterpret_cast<HWND>(operation->m_projectedHwnd),
                          QueryShellHwndGeneration(
                              operation->m_projectedHwnd))
                    : wxMSWOleShellHwndKey(nullptr, 0);
            publishProjection = boundaryCurrent;
        }

        operation->m_active = false;
        ++registry.mutationGeneration;
    }
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        publishProjection = false;
    }
#endif
    if ( retainedCandidate )
        retainedCandidate->Release();
    return publishProjection;
}

bool wxMSWOleCancelShellDropTargetOperation(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || !operation->m_active || !operation->m_state )
        return false;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    DropTargetRegistry& registry = GetDropTargetRegistry();
    std::lock_guard<std::mutex> registryLock(registry.mutex);
    std::lock_guard<std::mutex> stateLock(operation->m_state->mutex);
    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(operation->m_hwnd),
        operation->m_hwndGeneration);
    const auto shell = operation->m_state->shellHwndStates.find(key);
    const bool exactLifetime = operation->m_state->target != nullptr &&
        operation->m_state->generation == operation->m_targetGeneration;
    const bool projectionUnchanged = exactLifetime &&
        operation->m_state->shellProjectionEpoch ==
            operation->m_baselineProjectionEpoch;
    if ( operation->m_state->shellOperationDepth != 0 )
        --operation->m_state->shellOperationDepth;
    if ( shell != operation->m_state->shellHwndStates.end() )
    {
        if ( shell->second.activeOperations != 0 )
            --shell->second.activeOperations;
        if ( shell->second.registerReservation == operation->m_reservationId )
            shell->second.registerReservation = 0;
    }
    if ( operation->m_state->shellRegisterReservation ==
            operation->m_reservationId )
    {
        operation->m_state->shellRegisterReservation = 0;
        operation->m_state->shellRegisterReservationKey =
            wxMSWOleShellHwndKey(nullptr, 0);
    }
    if ( operation->m_globalTracking )
    {
        const auto global = registry.shellHwndStates.find(key);
        if ( global != registry.shellHwndStates.end() &&
             global->second.reservation.id == operation->m_reservationId )
        {
            if ( operation->m_kind ==
                     wxMSWOleShellDropTargetOperationKind::Revoke &&
                 global->second.owner.kind ==
                     wxMSWOleGlobalShellOwner::Kind::Fixed &&
                 global->second.owner.identity == operation->m_comIdentity )
            {
                global->second.owner.externalLockHeld =
                    global->second.reservation.externalLockHeld;
                global->second.owner.externalUnlockClaimed =
                    global->second.reservation.externalUnlockClaimed;
            }
            const bool keepLockTombstone =
                global->second.reservation.externalLockHeld &&
                !global->second.reservation.externalUnlockClaimed;
            if ( keepLockTombstone )
            {
                global->second.retiring = true;
            }
            else
            {
                global->second.reservation =
                    wxMSWOleGlobalShellReservation();
                MarkPendingExternalWakeReadyLocked(global->second);
                if ( !global->second.owner.identity &&
                     !global->second.retiring &&
                     !global->second.pendingExternalId )
                {
                    registry.shellHwndStates.erase(global);
                }
            }
        }
    }
    operation->m_active = false;
    ++registry.mutationGeneration;
    return projectionUnchanged;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

bool wxMSWOleIsLatestShellDropTargetOperation(
    const wxMSWOleShellDropTargetOperation& operation) noexcept
{
    if ( !operation.m_state || operation.m_operationGeneration == 0 )
        return false;

    const HWND hwnd = reinterpret_cast<HWND>(operation.m_hwnd);
    if ( ::GetCurrentThreadId() != operation.m_threadId ||
         !::IsWindow(hwnd) ||
         ::GetWindowThreadProcessId(hwnd, nullptr) !=
            operation.m_threadId ||
         QueryShellHwndGeneration(operation.m_hwnd) !=
            operation.m_hwndGeneration )
    {
        return false;
    }

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    std::lock_guard<std::mutex> stateLock(operation.m_state->mutex);
    return operation.m_projection ==
               wxMSWOleShellDropTargetProjection::Unique &&
           operation.m_projectedHwnd == operation.m_hwnd &&
           operation.m_state->target != nullptr &&
           operation.m_state->generation == operation.m_targetGeneration &&
           operation.m_state->shellProjectionEpoch ==
               operation.m_projectionEpoch &&
           operation.m_state->shellWrapperKey ==
               wxMSWOleShellHwndKey(hwnd, operation.m_hwndGeneration);
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return false;
    }
#endif
}

void wxMSWOleNoteShellDropTargetExternalLock(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || !operation->m_active ||
         operation->m_externalLockAcquired )
    {
        return;
    }

    operation->m_externalLockAcquired = true;
    if ( !operation->m_globalTracking )
        return;

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_hwnd),
            operation->m_hwndGeneration);
        const auto global = registry.shellHwndStates.find(key);
        if ( global != registry.shellHwndStates.end() &&
             global->second.reservation.id == operation->m_reservationId &&
             global->second.reservation.identity == operation->m_comIdentity )
        {
            global->second.reservation.externalLockHeld = true;
        }
    }
    catch ( ... )
    {
        // The local token still remembers the acquired lock and guarantees a
        // single balancing FALSE attempt even if bookkeeping allocation fails.
    }
}

bool wxMSWOleClaimShellDropTargetExternalUnlock(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || !operation->m_externalLockAcquired ||
         operation->m_externalUnlockClaimed )
    {
        return false;
    }

    if ( !operation->m_globalTracking )
    {
        operation->m_externalUnlockClaimed = true;
        return true;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_hwnd),
            operation->m_hwndGeneration);
        const auto global = registry.shellHwndStates.find(key);
        if ( global == registry.shellHwndStates.end() )
        {
            operation->m_externalUnlockClaimed = true;
            return true;
        }

        bool claimed = false;
        if ( global->second.reservation.id == operation->m_reservationId &&
             global->second.reservation.identity == operation->m_comIdentity &&
             global->second.reservation.externalLockHeld &&
             !global->second.reservation.externalUnlockClaimed )
        {
            global->second.reservation.externalUnlockClaimed = true;
            claimed = true;
        }
        else if ( operation->m_globalOwnerSerial &&
                  global->second.owner.kind ==
                      wxMSWOleGlobalShellOwner::Kind::Fixed &&
                  global->second.owner.identity == operation->m_comIdentity &&
                  global->second.owner.operationSerial ==
                      operation->m_globalOwnerSerial &&
                  global->second.owner.externalLockHeld &&
                  !global->second.owner.externalUnlockClaimed )
        {
            global->second.owner.externalUnlockClaimed = true;
            claimed = true;
        }
        if ( claimed )
            operation->m_externalUnlockClaimed = true;
        if ( !claimed )
        {
            const bool alreadyClaimedByExactRecord =
                (global->second.reservation.id ==
                     operation->m_reservationId &&
                 global->second.reservation.identity ==
                     operation->m_comIdentity &&
                 global->second.reservation.externalUnlockClaimed) ||
                (operation->m_globalOwnerSerial &&
                 global->second.owner.identity ==
                     operation->m_comIdentity &&
                 global->second.owner.operationSerial ==
                     operation->m_globalOwnerSerial &&
                 global->second.owner.externalUnlockClaimed);
            if ( alreadyClaimedByExactRecord )
                return false;

            // Even if a re-entrant mutation already retired the reservation,
            // this stack token still uniquely owns the TRUE it observed.
            operation->m_externalUnlockClaimed = true;
            return true;
        }
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

void wxMSWOleNoteShellDropTargetExternalUnlock(
    wxMSWOleShellDropTargetOperation* operation,
    bool unlocked) noexcept
{
    if ( !operation || !operation->m_externalUnlockClaimed )
        return;

    if ( unlocked )
        operation->m_externalLockAcquired = false;

    if ( !operation->m_globalTracking )
    {
        if ( unlocked )
            operation->m_externalLockAcquired = false;
        return;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_hwnd),
            operation->m_hwndGeneration);
        const auto global = registry.shellHwndStates.find(key);
        if ( global == registry.shellHwndStates.end() )
            return;

        if ( global->second.reservation.id == operation->m_reservationId &&
             global->second.reservation.identity == operation->m_comIdentity &&
             global->second.reservation.externalUnlockClaimed )
        {
            global->second.reservation.externalLockHeld = !unlocked;
        }

        if ( operation->m_globalOwnerSerial &&
             global->second.owner.kind ==
                 wxMSWOleGlobalShellOwner::Kind::Fixed &&
             global->second.owner.identity == operation->m_comIdentity &&
             global->second.owner.operationSerial ==
                 operation->m_globalOwnerSerial &&
             global->second.owner.externalUnlockClaimed )
        {
            global->second.owner.externalLockHeld = !unlocked;
            if ( unlocked && !global->second.owner.registered )
            {
                operation->m_deferredRelease =
                    global->second.owner.identity;
                global->second.owner = wxMSWOleGlobalShellOwner();
                if ( !global->second.reservation.id &&
                     !global->second.pendingExternalId )
                    registry.shellHwndStates.erase(global);
            }
        }
        ++registry.mutationGeneration;
    }
    catch ( ... )
    {
        // The exact record remains fail-closed; never issue a second FALSE for
        // this TRUE merely because post-call bookkeeping failed.
    }
}

wxMSWOleExternalShellOperation::~wxMSWOleExternalShellOperation()
{
    if ( m_status != wxMSWOleExternalShellOperationStatus::Rejected )
        wxMSWOleCancelExternalShellOperation(this);

    IDropTarget * const deferredRelease = m_deferredRelease;
    IDropTarget * const pinnedIdentity =
        m_identityPinned ? m_comIdentity : nullptr;
    m_deferredRelease = nullptr;
    m_identityPinned = false;
    m_comIdentity = nullptr;

    // Release only after the last member access: either COM callback can
    // synchronously destroy the broker which owns this token.
    if ( deferredRelease )
        deferredRelease->Release();
    if ( pinnedIdentity )
        pinnedIdentity->Release();
}

wxMSWOleExternalShellOperation::wxMSWOleExternalShellOperation(
    wxMSWOleExternalShellOperation&& other) noexcept
    : m_identity(other.m_identity),
      m_kind(other.m_kind),
      m_comIdentity(other.m_comIdentity),
      m_deferredRelease(other.m_deferredRelease),
      m_reservationId(other.m_reservationId),
      m_pendingId(other.m_pendingId),
      m_operationGeneration(other.m_operationGeneration),
      m_ownerSerial(other.m_ownerSerial),
      m_status(other.m_status),
      m_retireRequested(other.m_retireRequested),
      m_identityPinned(other.m_identityPinned),
      m_externalLockAcquired(other.m_externalLockAcquired),
      m_externalUnlockClaimed(other.m_externalUnlockClaimed)
{
    other.m_identity = {};
    other.m_comIdentity = nullptr;
    other.m_deferredRelease = nullptr;
    other.m_reservationId = 0;
    other.m_pendingId = 0;
    other.m_operationGeneration = 0;
    other.m_ownerSerial = 0;
    other.m_status = wxMSWOleExternalShellOperationStatus::Rejected;
    other.m_retireRequested = false;
    other.m_identityPinned = false;
    other.m_externalLockAcquired = false;
    other.m_externalUnlockClaimed = false;
}

wxMSWOleExternalShellOperation&
wxMSWOleExternalShellOperation::operator=(
    wxMSWOleExternalShellOperation&& other) noexcept
{
    if ( this == &other )
        return *this;

    // Keep assignment callback-free. The source receives our previous state
    // and its destructor performs cancellation/releases after this operation
    // has returned, so re-entrant broker destruction cannot cause a UAF here.
    using std::swap;
    swap(m_identity, other.m_identity);
    swap(m_kind, other.m_kind);
    swap(m_comIdentity, other.m_comIdentity);
    swap(m_deferredRelease, other.m_deferredRelease);
    swap(m_reservationId, other.m_reservationId);
    swap(m_pendingId, other.m_pendingId);
    swap(m_operationGeneration, other.m_operationGeneration);
    swap(m_ownerSerial, other.m_ownerSerial);
    swap(m_status, other.m_status);
    swap(m_retireRequested, other.m_retireRequested);
    swap(m_identityPinned, other.m_identityPinned);
    swap(m_externalLockAcquired, other.m_externalLockAcquired);
    swap(m_externalUnlockClaimed, other.m_externalUnlockClaimed);
    return *this;
}

wxMSWOleExternalShellOperation wxMSWOleBeginExternalShellOperation(
    const wxMSWOleShellHwndIdentity& identity,
    wxMSWOleShellDropTargetOperationKind kind,
    IDropTarget* comIdentity) noexcept
{
    wxMSWOleExternalShellOperation operation;
    if ( !identity.hwnd || !identity.generation || !identity.threadId ||
         !comIdentity || ::GetCurrentThreadId() != identity.threadId )
    {
        return operation;
    }

    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(identity.hwnd), identity.generation);
    if ( !IsShellHwndKeyCurrent(key, identity.threadId) )
        return operation;

    wxMSWOlePendingTimerCancel timerCancel;

#if wxUSE_EXCEPTIONS
    try
    {
#endif
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        wxMSWOleGlobalShellHwndState& global = registry.shellHwndStates[key];

        operation.m_identity = identity;
        operation.m_kind = kind;
        operation.m_comIdentity = comIdentity;

        if ( kind == wxMSWOleShellDropTargetOperationKind::Register )
        {
            if ( global.retiring || global.pendingExternalId )
            {
                return {};
            }

            if ( global.reservation.id )
            {
                if ( global.reservation.externalManaged )
                    return {};

                const std::uint64_t pendingId =
                    NextNonZero(&registry.shellReservationGeneration);
                timerCancel = ClearPendingExternalWakeLocked(global);
                global.pendingExternalId = pendingId;
                global.pendingExternalIdentity = comIdentity;
                operation.m_pendingId = pendingId;
                operation.m_status =
                    wxMSWOleExternalShellOperationStatus::PendingFixed;
            }
            else if ( global.owner.registered || global.owner.identity ||
                      global.owner.kind !=
                          wxMSWOleGlobalShellOwner::Kind::None )
            {
                return {};
            }
            else
            {
                const std::uint64_t reservationId =
                    NextNonZero(&registry.shellReservationGeneration);
                global.reservation.id = reservationId;
                global.reservation.kind = kind;
                global.reservation.identity = comIdentity;
                global.reservation.externalManaged = true;
                operation.m_reservationId = reservationId;
                operation.m_status =
                    wxMSWOleExternalShellOperationStatus::Active;
            }
        }
        else
        {
            if ( global.reservation.id || global.pendingExternalId ||
                 global.owner.kind !=
                     wxMSWOleGlobalShellOwner::Kind::ExternalManaged ||
                 !global.owner.registered ||
                 global.owner.identity != comIdentity )
            {
                return {};
            }

            const std::uint64_t reservationId =
                NextNonZero(&registry.shellReservationGeneration);
            global.reservation.id = reservationId;
            global.reservation.kind = kind;
            global.reservation.identity = comIdentity;
            global.reservation.externalManaged = true;
            global.reservation.retireRequested = global.retiring;
            global.reservation.externalLockHeld =
                global.owner.externalLockHeld;
            global.reservation.externalUnlockClaimed =
                global.owner.externalUnlockClaimed;
            operation.m_reservationId = reservationId;
            operation.m_ownerSerial = global.owner.operationSerial;
            operation.m_externalLockAcquired =
                global.reservation.externalLockHeld;
            operation.m_externalUnlockClaimed =
                global.reservation.externalUnlockClaimed;
            operation.m_status =
                wxMSWOleExternalShellOperationStatus::Active;
        }
        ++registry.mutationGeneration;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return {};
    }
#endif

    CancelPendingExternalTimer(timerCancel);

    // The broker owns this identity too, but the token must independently pin
    // it across native calls and across a deferred PendingFixed continuation.
    comIdentity->AddRef();
    operation.m_identityPinned = true;
    return operation;
}

bool wxMSWOleArmExternalShellContinuation(
    wxMSWOleExternalShellOperation* operation,
    WXHWND wakeHwnd) noexcept
{
    if ( !operation || !operation->IsPendingFixed() ||
         !operation->m_pendingId || !wakeHwnd )
    {
        return false;
    }

    const wxMSWOleShellHwndIdentity wakeIdentity =
        wxMSWOleEnsureShellHwndIdentity(wakeHwnd);
    if ( !wakeIdentity.hwnd ||
         wakeIdentity.threadId != operation->m_identity.threadId )
    {
        return false;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() ||
             found->second.pendingExternalId != operation->m_pendingId ||
             found->second.pendingExternalIdentity !=
                 operation->m_comIdentity ||
             !found->second.reservation.id ||
             found->second.reservation.externalManaged ||
             found->second.retiring ||
             found->second.reservation.retireRequested )
        {
            return false;
        }

        found->second.pendingContinuationHwnd =
            reinterpret_cast<HWND>(wakeIdentity.hwnd);
        found->second.pendingContinuationGeneration = wakeIdentity.generation;
        found->second.pendingContinuationThread = wakeIdentity.threadId;
        found->second.pendingContinuationState =
            wxMSWOlePendingContinuationState::Armed;
        ++registry.mutationGeneration;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxMSWOleTryPromoteExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || !operation->IsPendingFixed() ||
         !operation->m_pendingId || !operation->m_comIdentity ||
         ::GetCurrentThreadId() != operation->m_identity.threadId )
    {
        return false;
    }

    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(operation->m_identity.hwnd),
        operation->m_identity.generation);
    if ( !IsShellHwndKeyCurrent(key, operation->m_identity.threadId) )
        return false;

    wxMSWOlePendingTimerCancel timerCancel;
    bool promoted = false;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() )
            return false;
        wxMSWOleGlobalShellHwndState& global = found->second;
        if ( global.pendingExternalId != operation->m_pendingId ||
             global.pendingExternalIdentity != operation->m_comIdentity ||
             global.reservation.id || global.owner.registered ||
             global.owner.identity || global.retiring )
        {
            return false;
        }

        global.reservation.id = operation->m_pendingId;
        global.reservation.kind =
            wxMSWOleShellDropTargetOperationKind::Register;
        global.reservation.identity = operation->m_comIdentity;
        global.reservation.externalManaged = true;
        global.pendingExternalId = 0;
        global.pendingExternalIdentity = nullptr;
        timerCancel = ClearPendingExternalWakeLocked(global);
        operation->m_reservationId = operation->m_pendingId;
        operation->m_pendingId = 0;
        operation->m_status =
            wxMSWOleExternalShellOperationStatus::Active;
        ++registry.mutationGeneration;
        promoted = true;
    }
    catch ( ... )
    {
        return false;
    }
    CancelPendingExternalTimer(timerCancel);
    return promoted;
}

bool wxMSWOleArmExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || !operation->IsActive() ||
         operation->m_operationGeneration || !operation->m_reservationId ||
         ::GetCurrentThreadId() != operation->m_identity.threadId )
    {
        return false;
    }

    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(operation->m_identity.hwnd),
        operation->m_identity.generation);
    if ( !IsShellHwndKeyCurrent(key, operation->m_identity.threadId) )
        return false;

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() ||
             found->second.reservation.id != operation->m_reservationId ||
             found->second.reservation.identity != operation->m_comIdentity ||
             !found->second.reservation.externalManaged ||
             found->second.reservation.kind != operation->m_kind ||
             (operation->m_kind ==
                  wxMSWOleShellDropTargetOperationKind::Register &&
              (found->second.retiring ||
               found->second.reservation.retireRequested)) )
        {
            return false;
        }
        if ( operation->m_kind ==
                 wxMSWOleShellDropTargetOperationKind::Revoke &&
             (found->second.owner.kind !=
                  wxMSWOleGlobalShellOwner::Kind::ExternalManaged ||
              found->second.owner.identity != operation->m_comIdentity ||
              found->second.owner.operationSerial !=
                  operation->m_ownerSerial) )
        {
            return false;
        }

        operation->m_operationGeneration =
            NextNonZero(&registry.shellOperationGeneration);
        found->second.reservation.operationSerial =
            operation->m_operationGeneration;
        ++registry.mutationGeneration;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxMSWOleCompleteExternalShellOperation(
    wxMSWOleExternalShellOperation* operation,
    bool succeeded) noexcept
{
    if ( !operation || !operation->IsArmed() ||
         !operation->m_comIdentity )
    {
        return false;
    }

    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(operation->m_identity.hwnd),
        operation->m_identity.generation);
    const bool boundaryCurrent =
        ::GetCurrentThreadId() == operation->m_identity.threadId &&
        IsShellHwndKeyCurrent(key, operation->m_identity.threadId);

    bool committed = false;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() ||
             found->second.reservation.id != operation->m_reservationId ||
             found->second.reservation.identity != operation->m_comIdentity ||
             !found->second.reservation.externalManaged ||
             found->second.reservation.kind != operation->m_kind )
        {
            operation->m_status =
                wxMSWOleExternalShellOperationStatus::Rejected;
            return false;
        }

        wxMSWOleGlobalShellHwndState& global = found->second;
        operation->m_retireRequested = global.retiring ||
            global.reservation.retireRequested || !boundaryCurrent;
        if ( operation->m_retireRequested )
            global.retiring = true;

        // The local token is the allocation-free fallback if bookkeeping was
        // interrupted after the external TRUE. Under the exact reservation
        // both views describe the same lock and must be transferred together.
        const bool externalLockHeld =
            global.reservation.externalLockHeld ||
            operation->m_externalLockAcquired;
        const bool externalUnlockClaimed =
            global.reservation.externalUnlockClaimed ||
            operation->m_externalUnlockClaimed;
        global.reservation.externalLockHeld = externalLockHeld;
        global.reservation.externalUnlockClaimed =
            externalUnlockClaimed;

        if ( operation->m_kind ==
                 wxMSWOleShellDropTargetOperationKind::Register )
        {
            committed = !succeeded ||
                (!global.owner.registered && !global.owner.identity);
            if ( committed && (succeeded || externalLockHeld) )
            {
                global.owner.kind =
                    wxMSWOleGlobalShellOwner::Kind::ExternalManaged;
                global.owner.identity = operation->m_comIdentity;
                global.owner.operationSerial =
                    operation->m_operationGeneration;
                global.owner.registered = succeeded;
                global.owner.externalLockHeld = externalLockHeld;
                global.owner.externalUnlockClaimed =
                    externalUnlockClaimed;
                global.owner.identityRetained = true;
                operation->m_ownerSerial =
                    operation->m_operationGeneration;
                // Transfer the token's independent pin to the owner ledger.
                operation->m_identityPinned = false;
            }
        }
        else
        {
            committed = global.owner.kind ==
                    wxMSWOleGlobalShellOwner::Kind::ExternalManaged &&
                global.owner.identity == operation->m_comIdentity &&
                global.owner.operationSerial == operation->m_ownerSerial;
            if ( committed )
            {
                global.owner.externalLockHeld = externalLockHeld;
                global.owner.externalUnlockClaimed =
                    externalUnlockClaimed;
                if ( succeeded )
                {
                    global.owner.registered = false;
                    global.owner.operationSerial =
                        operation->m_operationGeneration;
                    operation->m_ownerSerial =
                        operation->m_operationGeneration;

                    // RegisterDragDrop ownership is gone, but a failed or not
                    // yet attempted external FALSE still needs the retained
                    // identity and exact owner row. CompleteExternalUnlock()
                    // removes it only after a successful balance.
                    if ( !global.owner.externalLockHeld )
                    {
                        if ( global.owner.identityRetained )
                        {
                            operation->m_deferredRelease =
                                global.owner.identity;
                        }
                        global.owner = wxMSWOleGlobalShellOwner();
                    }
                }
            }
        }

        global.reservation = wxMSWOleGlobalShellReservation();
        operation->m_reservationId = 0;
        operation->m_status =
            wxMSWOleExternalShellOperationStatus::Rejected;
        if ( !global.owner.registered && !global.owner.identity &&
             !global.pendingExternalId )
        {
            registry.shellHwndStates.erase(found);
        }
        ++registry.mutationGeneration;
    }
    catch ( ... )
    {
        return false;
    }
    return committed;
}

void wxMSWOleNoteExternalShellLock(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || !operation->IsActive() ||
         !operation->m_comIdentity || operation->m_externalLockAcquired )
    {
        return;
    }

    // Set the token first: even if registry bookkeeping unexpectedly fails,
    // this native-call owner still issues at most one balancing FALSE.
    operation->m_externalLockAcquired = true;

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found != registry.shellHwndStates.end() &&
             found->second.reservation.id == operation->m_reservationId &&
             found->second.reservation.identity ==
                 operation->m_comIdentity &&
             found->second.reservation.externalManaged )
        {
            found->second.reservation.externalLockHeld = true;
            ++registry.mutationGeneration;
        }
    }
    catch ( ... )
    {
        // The token remains the exact local proof for rollback.
    }
}

bool wxMSWOleClaimExternalShellUnlock(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || !operation->m_comIdentity ||
         !operation->m_externalLockAcquired ||
         operation->m_externalUnlockClaimed )
    {
        return false;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() )
        {
            operation->m_externalUnlockClaimed = true;
            return true;
        }

        wxMSWOleGlobalShellHwndState& global = found->second;
        bool claimed = false;
        if ( global.reservation.id == operation->m_reservationId &&
             global.reservation.identity == operation->m_comIdentity &&
             global.reservation.externalManaged &&
             global.reservation.externalLockHeld &&
             !global.reservation.externalUnlockClaimed )
        {
            global.reservation.externalUnlockClaimed = true;
            claimed = true;
        }
        else if ( operation->m_ownerSerial &&
                  global.owner.kind ==
                      wxMSWOleGlobalShellOwner::Kind::ExternalManaged &&
                  global.owner.identity == operation->m_comIdentity &&
                  global.owner.operationSerial == operation->m_ownerSerial &&
                  global.owner.externalLockHeld &&
                  !global.owner.externalUnlockClaimed )
        {
            global.owner.externalUnlockClaimed = true;
            claimed = true;
        }

        if ( claimed )
        {
            operation->m_externalUnlockClaimed = true;
            ++registry.mutationGeneration;
            return true;
        }

        const bool exactRecordAlreadyClaimed =
            (global.reservation.id == operation->m_reservationId &&
             global.reservation.identity == operation->m_comIdentity &&
             global.reservation.externalManaged &&
             global.reservation.externalUnlockClaimed) ||
            (operation->m_ownerSerial &&
             global.owner.kind ==
                 wxMSWOleGlobalShellOwner::Kind::ExternalManaged &&
             global.owner.identity == operation->m_comIdentity &&
             global.owner.operationSerial == operation->m_ownerSerial &&
             global.owner.externalUnlockClaimed);
        if ( exactRecordAlreadyClaimed )
            return false;

        // A re-entrant retirement can remove the reservation, but it cannot
        // transfer this token's independently observed TRUE. Preserve the
        // one-shot local cleanup authority without touching an unrelated row.
        operation->m_externalUnlockClaimed = true;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

void wxMSWOleCompleteExternalShellUnlock(
    wxMSWOleExternalShellOperation* operation,
    bool unlocked) noexcept
{
    if ( !operation || !operation->m_comIdentity ||
         !operation->m_externalUnlockClaimed )
    {
        return;
    }

    if ( unlocked )
        operation->m_externalLockAcquired = false;

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() )
            return;

        wxMSWOleGlobalShellHwndState& global = found->second;
        if ( global.reservation.id == operation->m_reservationId &&
             global.reservation.identity == operation->m_comIdentity &&
             global.reservation.externalManaged &&
             global.reservation.externalUnlockClaimed )
        {
            global.reservation.externalLockHeld = !unlocked;
        }

        if ( operation->m_ownerSerial &&
             global.owner.kind ==
                 wxMSWOleGlobalShellOwner::Kind::ExternalManaged &&
             global.owner.identity == operation->m_comIdentity &&
             global.owner.operationSerial == operation->m_ownerSerial &&
             global.owner.externalUnlockClaimed )
        {
            global.owner.externalLockHeld = !unlocked;
            if ( unlocked && !global.owner.registered )
            {
                if ( global.owner.identityRetained )
                    operation->m_deferredRelease = global.owner.identity;
                global.owner = wxMSWOleGlobalShellOwner();
                if ( !global.reservation.id && !global.pendingExternalId )
                    registry.shellHwndStates.erase(found);
            }
        }
        ++registry.mutationGeneration;
    }
    catch ( ... )
    {
        // Keep the one-shot claim and local held bit. A failed HRESULT (or
        // failed bookkeeping after it) never authorizes another FALSE.
    }
}

void wxMSWOleCancelExternalShellOperation(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || operation->m_status ==
            wxMSWOleExternalShellOperationStatus::Rejected )
    {
        return;
    }

    wxMSWOlePendingTimerCancel timerCancel;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found != registry.shellHwndStates.end() )
        {
            wxMSWOleGlobalShellHwndState& global = found->second;
            if ( operation->IsPendingFixed() &&
                 global.pendingExternalId == operation->m_pendingId &&
                 global.pendingExternalIdentity == operation->m_comIdentity )
            {
                global.pendingExternalId = 0;
                global.pendingExternalIdentity = nullptr;
                timerCancel = ClearPendingExternalWakeLocked(global);
            }
            else if ( operation->IsActive() &&
                      global.reservation.id == operation->m_reservationId &&
                      global.reservation.identity == operation->m_comIdentity &&
                      global.reservation.externalManaged )
            {
                if ( global.reservation.retireRequested )
                    global.retiring = true;

                const bool externalLockHeld =
                    global.reservation.externalLockHeld ||
                    operation->m_externalLockAcquired;
                const bool externalUnlockClaimed =
                    global.reservation.externalUnlockClaimed ||
                    operation->m_externalUnlockClaimed;
                if ( operation->m_kind ==
                         wxMSWOleShellDropTargetOperationKind::Register &&
                     externalLockHeld && !global.owner.identity )
                {
                    // No RegisterDragDrop call owns this target, but a TRUE
                    // already succeeded. Preserve the exact identity until
                    // the external component reports its one FALSE result.
                    global.owner.kind =
                        wxMSWOleGlobalShellOwner::Kind::ExternalManaged;
                    global.owner.identity = operation->m_comIdentity;
                    global.owner.operationSerial =
                        operation->m_operationGeneration
                            ? operation->m_operationGeneration
                            : NextNonZero(&registry.shellOperationGeneration);
                    global.owner.registered = false;
                    global.owner.externalLockHeld = true;
                    global.owner.externalUnlockClaimed =
                        externalUnlockClaimed;
                    global.owner.identityRetained = true;
                    operation->m_ownerSerial =
                        global.owner.operationSerial;
                    operation->m_identityPinned = false;
                }
                else if ( operation->m_kind ==
                              wxMSWOleShellDropTargetOperationKind::Revoke &&
                          global.owner.kind ==
                              wxMSWOleGlobalShellOwner::Kind::ExternalManaged &&
                          global.owner.identity == operation->m_comIdentity &&
                          global.owner.operationSerial ==
                              operation->m_ownerSerial )
                {
                    global.owner.externalLockHeld = externalLockHeld;
                    global.owner.externalUnlockClaimed =
                        externalUnlockClaimed;
                }
                global.reservation = wxMSWOleGlobalShellReservation();
            }

            if ( !global.owner.registered && !global.owner.identity &&
                 !global.pendingExternalId && !global.reservation.id )
            {
                registry.shellHwndStates.erase(found);
            }
            ++registry.mutationGeneration;
        }
    }
    catch ( ... )
    {
    }

    CancelPendingExternalTimer(timerCancel);

    operation->m_reservationId = 0;
    operation->m_pendingId = 0;
    operation->m_operationGeneration = 0;
    operation->m_status =
        wxMSWOleExternalShellOperationStatus::Rejected;
}

bool wxMSWOleAbandonExternalShellOwner(
    wxMSWOleExternalShellOperation* operation) noexcept
{
    if ( !operation || !operation->m_comIdentity ||
         !operation->m_ownerSerial )
    {
        return false;
    }

    IDropTarget *releaseIdentity = nullptr;
    bool abandoned = false;
    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_identity.hwnd),
            operation->m_identity.generation);
        const auto found = registry.shellHwndStates.find(key);
        if ( found == registry.shellHwndStates.end() ||
             found->second.owner.kind !=
                 wxMSWOleGlobalShellOwner::Kind::ExternalManaged ||
             found->second.owner.identity != operation->m_comIdentity ||
             found->second.owner.operationSerial != operation->m_ownerSerial ||
             found->second.reservation.id )
        {
            return false;
        }

        wxMSWOleGlobalShellOwner& owner = found->second.owner;
        if ( owner.externalLockHeld )
            return false;

        if ( owner.identityRetained )
            releaseIdentity = owner.identity;
        if ( owner.registered )
        {
            const std::uint64_t ownerSerial = owner.operationSerial;
            owner = wxMSWOleGlobalShellOwner();
            owner.kind =
                wxMSWOleGlobalShellOwner::Kind::ExternalTombstone;
            owner.operationSerial = ownerSerial;
            owner.registered = true;
            owner.externalUnlockClaimed = true;
        }
        else
        {
            owner = wxMSWOleGlobalShellOwner();
            if ( !found->second.pendingExternalId )
                registry.shellHwndStates.erase(found);
        }
        ++registry.mutationGeneration;
        abandoned = true;
    }
    catch ( ... )
    {
        return false;
    }

    if ( releaseIdentity )
        releaseIdentity->Release();
    return abandoned;
}

bool wxMSWOleReserveShellDropTargetCompensation(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || operation->m_active ||
         !operation->m_globalTracking || !operation->m_globalOwnerSerial ||
         operation->m_compensationReservationId )
    {
        return false;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_hwnd),
            operation->m_hwndGeneration);
        const auto global = registry.shellHwndStates.find(key);
        if ( global == registry.shellHwndStates.end() ||
             global->second.reservation.id ||
             global->second.owner.kind !=
                 wxMSWOleGlobalShellOwner::Kind::Fixed ||
             !global->second.owner.registered ||
             global->second.owner.identity != operation->m_comIdentity ||
             global->second.owner.operationSerial !=
                 operation->m_globalOwnerSerial ||
             !IsShellHwndKeyCurrent(key, operation->m_threadId) )
        {
            return false;
        }

        const std::uint64_t reservationId =
            NextNonZero(&registry.shellReservationGeneration);
        global->second.reservation.id = reservationId;
        global->second.reservation.kind =
            wxMSWOleShellDropTargetOperationKind::Revoke;
        global->second.reservation.identity = operation->m_comIdentity;
        global->second.reservation.operationSerial =
            NextNonZero(&registry.shellOperationGeneration);
        global->second.reservation.externalLockHeld =
            global->second.owner.externalLockHeld;
        global->second.reservation.externalUnlockClaimed =
            global->second.owner.externalUnlockClaimed;
        operation->m_compensationReservationId = reservationId;
        ++registry.mutationGeneration;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxMSWOleCompleteShellDropTargetCompensation(
    wxMSWOleShellDropTargetOperation* operation,
    bool revoked) noexcept
{
    if ( !operation || !operation->m_compensationReservationId ||
         !operation->m_globalTracking )
    {
        return false;
    }

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> registryLock(registry.mutex);
        const wxMSWOleShellHwndKey key(
            reinterpret_cast<HWND>(operation->m_hwnd),
            operation->m_hwndGeneration);
        const auto global = registry.shellHwndStates.find(key);
        if ( global == registry.shellHwndStates.end() ||
             global->second.reservation.id !=
                 operation->m_compensationReservationId ||
             global->second.reservation.identity != operation->m_comIdentity ||
             global->second.owner.kind !=
                 wxMSWOleGlobalShellOwner::Kind::Fixed ||
             global->second.owner.identity != operation->m_comIdentity ||
             global->second.owner.operationSerial !=
                 operation->m_globalOwnerSerial )
        {
            operation->m_compensationReservationId = 0;
            return false;
        }

        if ( revoked )
        {
            global->second.owner.registered = false;
            global->second.owner.operationSerial =
                global->second.reservation.operationSerial;
            operation->m_globalOwnerSerial =
                global->second.owner.operationSerial;

            if ( global->second.owner.state )
            {
                std::lock_guard<std::mutex> stateLock(
                    global->second.owner.state->mutex);
                const auto shell =
                    global->second.owner.state->shellHwndStates.find(key);
                if ( shell !=
                     global->second.owner.state->shellHwndStates.end() )
                {
                    shell->second.registered = false;
                    ++global->second.owner.state->shellProjectionEpoch;
                    if ( global->second.owner.state->shellProjectionEpoch == 0 )
                        ++global->second.owner.state->shellProjectionEpoch;
                    if ( global->second.owner.state->target != nullptr &&
                         global->second.owner.state->generation ==
                             operation->m_targetGeneration )
                    {
                        operation->m_projectionEpoch =
                            global->second.owner.state->shellProjectionEpoch;
                        operation->m_projection =
                            DeriveShellProjectionLocked(
                                *global->second.owner.state,
                                &operation->m_projectedHwnd);
                        global->second.owner.state->shellWrapperKey =
                            operation->m_projection ==
                                wxMSWOleShellDropTargetProjection::Unique
                                ? wxMSWOleShellHwndKey(
                                      reinterpret_cast<HWND>(
                                          operation->m_projectedHwnd),
                                      QueryShellHwndGeneration(
                                          operation->m_projectedHwnd))
                                : wxMSWOleShellHwndKey(nullptr, 0);
                    }
                }
            }
            if ( !global->second.owner.externalLockHeld )
            {
                if ( global->second.owner.identityRetained )
                    operation->m_deferredRelease =
                        global->second.owner.identity;
                global->second.owner = wxMSWOleGlobalShellOwner();
            }
        }
        global->second.reservation = wxMSWOleGlobalShellReservation();
        MarkPendingExternalWakeReadyLocked(global->second);
        operation->m_compensationReservationId = 0;
        ++registry.mutationGeneration;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool wxMSWOleDropRoute::IsSameLogicalTarget(
    const wxMSWOleDropRoute& other) const noexcept
{
    return target.IsSameLifetime(other.target) &&
           ownerIdentity == other.ownerIdentity &&
           ownerGeneration == other.ownerGeneration;
}

// ============================================================================
// Shared logical OLE drop session
// ============================================================================

class wxMSWOleDropSession::Impl
{
public:
    Impl(wxMSWOleDropResolver& resolver,
         wxMSWOleDropFeedback& feedback,
         wxMSWOleDropSessionMode mode,
         wxMSWOleDropExceptionPolicy exceptionPolicy)
        : m_resolver(resolver),
          m_feedback(feedback),
          m_mode(mode),
          m_exceptionPolicy(exceptionPolicy)
    {
    }

    HRESULT DragEnter(IDataObject* data,
                      DWORD keyState,
                      POINTL screenPoint,
                      DWORD* effect)
    {
        if ( !effect || !data )
            return E_INVALIDARG;

        if ( !wxIsMainThread() )
        {
            *effect = DROPEFFECT_NONE;
            return RPC_E_WRONG_THREAD;
        }

        const std::uint64_t token = BeginOperation();
        AbortState(true);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        m_dataObject = data;

        if ( m_mode == wxMSWOleDropSessionMode::DynamicBroker )
        {
            // Arm physical feedback before any logical callback. If OnEnter()
            // re-enters DragOver/DragLeave, that nested operation then owns a
            // complete physical session instead of an orphan logical route.
            m_physicalFeedbackActive = true;
            m_feedback.DragEnter(nullptr, data, screenPoint, DROPEFFECT_NONE);
            if ( !IsCurrent(token) )
            {
                *effect = DROPEFFECT_NONE;
                return S_OK;
            }
        }

        wxMSWOleDropRoute route;
        const wxMSWOleDropResolveResult resolution =
            m_resolver.Resolve(screenPoint, &route);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }

        if ( resolution == wxMSWOleDropResolveResult::Hit &&
             route.target.IsCurrent() )
        {
            if ( m_mode ==
                    wxMSWOleDropSessionMode::FixedTargetCompatibility )
            {
                m_compatLeaveRoute =
                    std::unique_ptr<wxMSWOleDropRoute>(
                        new wxMSWOleDropRoute(route));
            }

            EnterRoute(route, data, keyState, screenPoint, effect, token);
        }
        else
        {
            *effect = DROPEFFECT_NONE;
        }

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }

        // The historical fixed wrapper didn't retain rejected data or start
        // its helper. The bridge must do both even over a target-free region.
        if ( m_mode == wxMSWOleDropSessionMode::DynamicBroker )
        {
            m_feedback.DragOver(m_activeRoute.get(), screenPoint, *effect);
        }
        else if ( m_activeRoute )
        {
            m_physicalFeedbackActive = true;
            m_feedback.DragEnter(m_activeRoute.get(), data, screenPoint,
                                 *effect);
        }

        if ( m_mode ==
                wxMSWOleDropSessionMode::FixedTargetCompatibility &&
             !m_activeRoute )
        {
            m_dataObject.reset();
        }

        if ( !IsCurrent(token) )
            *effect = DROPEFFECT_NONE;

        return S_OK;
    }

    HRESULT DragOver(DWORD keyState, POINTL screenPoint, DWORD* effect)
    {
        if ( !effect )
            return E_INVALIDARG;

        if ( !wxIsMainThread() )
        {
            *effect = DROPEFFECT_NONE;
            return RPC_E_WRONG_THREAD;
        }

        const std::uint64_t token = BeginOperation();

        if ( m_dataObject )
            UpdateRoute(keyState, screenPoint, effect, token);
        else
            *effect = DROPEFFECT_NONE;

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }

        if ( m_physicalFeedbackActive ||
             m_mode ==
                wxMSWOleDropSessionMode::FixedTargetCompatibility )
        {
            m_feedback.DragOver(m_activeRoute.get(), screenPoint, *effect);
        }
        if ( !IsCurrent(token) )
            *effect = DROPEFFECT_NONE;

        return S_OK;
    }

    HRESULT DragLeave()
    {
        if ( !wxIsMainThread() )
            return RPC_E_WRONG_THREAD;

        const std::uint64_t token = BeginOperation();

        if ( m_terminalRoute )
        {
            // Drop already retired the logical route before invoking
            // OnDrop/OnData. A nested DragLeave terminates the physical
            // session but must not invent an OnLeave after Drop began. The
            // outer Drop owns an in-flight callback guard and keeps its data
            // source alive until OnDrop/OnData has actually returned.
            m_terminalRoute.reset();
        }
        else if ( m_activeRoute )
        {
            LeaveActive(token);
        }
        else if ( m_mode ==
                    wxMSWOleDropSessionMode::FixedTargetCompatibility &&
                  m_compatLeaveRoute )
        {
            // Preserve the old wrapper's unusual OnLeave() after a rejected
            // DragEnter. Dynamic routing never invents such a callback.
            LeaveRoute(*m_compatLeaveRoute, token);
        }

        if ( !IsCurrent(token) )
            return S_OK;

        m_activeRoute.reset();
        m_compatLeaveRoute.reset();
        m_terminalRoute.reset();
        m_dataObject.reset();

        const bool endFeedback =
            m_physicalFeedbackActive ||
            m_mode == wxMSWOleDropSessionMode::FixedTargetCompatibility;
        m_physicalFeedbackActive = false;
        if ( endFeedback )
            m_feedback.DragLeave();

        return S_OK;
    }

    HRESULT Drop(IDataObject* data,
                 DWORD keyState,
                 POINTL screenPoint,
                 DWORD* effect)
    {
        if ( !effect || !data )
            return E_INVALIDARG;

        if ( !wxIsMainThread() )
        {
            *effect = DROPEFFECT_NONE;
            return RPC_E_WRONG_THREAD;
        }

        const std::uint64_t token = BeginOperation();
        wxCOMPtr<IDataObject> operationData = m_dataObject;
        if ( !operationData )
            operationData = data;
        else
            wxASSERT_MSG(m_dataObject == data,
                         "Drop data object differs from DragEnter");
        IDataObject* const dispatchData = operationData;

        std::unique_ptr<wxMSWOleDropRoute> dropRoute;
        const auto clearInFlightData = wxMakeGuard(
            [&dropRoute]()
            {
                if ( dropRoute )
                    ClearRouteDataSource(*dropRoute);
            });
        if ( m_activeRoute )
        {
            wxMSWOleDropRoute current;
            const wxMSWOleDropResolveResult resolution =
                m_resolver.Resolve(screenPoint, &current);

            if ( !IsCurrent(token) )
            {
                *effect = DROPEFFECT_NONE;
                return S_OK;
            }

            if ( resolution == wxMSWOleDropResolveResult::Hit &&
                 current.IsSameLogicalTarget(*m_activeRoute) )
            {
                m_terminalRoute =
                    std::unique_ptr<wxMSWOleDropRoute>(
                        new wxMSWOleDropRoute(std::move(current)));
                dropRoute =
                    std::unique_ptr<wxMSWOleDropRoute>(
                        new wxMSWOleDropRoute(*m_terminalRoute));
                m_activeRoute.reset();
                DispatchDrop(*dropRoute, dispatchData,
                             keyState, screenPoint, effect,
                             token);
            }
            else
            {
                // OLE normally sends DragOver before Drop. If the logical hit
                // changed without one, fail closed instead of inventing an
                // Enter/Drop pair for a target that never saw the drag.
                LeaveActive(token);
                *effect = DROPEFFECT_NONE;
            }
        }
        else
        {
            *effect = DROPEFFECT_NONE;
        }

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }

        const bool endFeedback =
            m_physicalFeedbackActive ||
            m_mode == wxMSWOleDropSessionMode::FixedTargetCompatibility;
        m_physicalFeedbackActive = false;
        if ( endFeedback )
            m_feedback.Drop(dropRoute.get(), dispatchData,
                            screenPoint, *effect);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }

        m_activeRoute.reset();
        m_compatLeaveRoute.reset();
        m_terminalRoute.reset();
        m_dataObject.reset();
        m_physicalFeedbackActive = false;
        return S_OK;
    }

    void Reset() noexcept
    {
        ++m_epoch;
        AbortState(true);
    }

    void AbortDrop(IDataObject* data, const POINTL& screenPoint) noexcept
    {
        const std::uint64_t token = ++m_epoch;
        wxCOMPtr<IDataObject> operationData = m_dataObject;
        if ( !operationData )
            operationData = data;
        std::unique_ptr<wxMSWOleDropRoute> route =
            std::move(m_terminalRoute);
        if ( !route )
            route = std::move(m_activeRoute);
        if ( !route )
            route = std::move(m_compatLeaveRoute);

        m_compatLeaveRoute.reset();
        m_dataObject.reset();

        const bool endFeedback =
            m_physicalFeedbackActive ||
            m_mode == wxMSWOleDropSessionMode::FixedTargetCompatibility;
        m_physicalFeedbackActive = false;
        if ( endFeedback )
        {
            m_feedback.Drop(route.get(), operationData, screenPoint,
                            DROPEFFECT_NONE);
        }

        if ( IsCurrent(token) && route )
            ClearRouteDataSource(*route);
    }

    bool ShouldReportExceptions() const noexcept
    {
        return m_exceptionPolicy ==
               wxMSWOleDropExceptionPolicy::ReportToApplication;
    }

private:
    std::uint64_t BeginOperation()
    {
        return ++m_epoch;
    }

    bool IsCurrent(std::uint64_t token) const
    {
        return m_epoch == token;
    }

    static void ClearRouteDataSource(
        const wxMSWOleDropRoute& route) noexcept
    {
        if ( wxDropTarget* const target = route.target.GetIfCurrent() )
            target->MSWSetDataSource(nullptr);
    }

    void AbortState(bool endFeedback) noexcept
    {
        std::unique_ptr<wxMSWOleDropRoute> route =
            std::move(m_terminalRoute);
        if ( !route )
            route = std::move(m_activeRoute);
        if ( !route )
            route = std::move(m_compatLeaveRoute);

        if ( route )
            ClearRouteDataSource(*route);

        m_activeRoute.reset();
        m_compatLeaveRoute.reset();
        m_terminalRoute.reset();
        m_dataObject.reset();
        const bool hadFeedback = m_physicalFeedbackActive;
        m_physicalFeedbackActive = false;
        if ( endFeedback && hadFeedback )
            m_feedback.DragLeave();
    }

    bool RefreshRoute(const wxMSWOleDropRoute& expected,
                      const POINTL& screenPoint,
                      wxMSWOleDropRoute* refreshed,
                      std::uint64_t token)
    {
        if ( !IsCurrent(token) || !expected.target.IsCurrent() )
            return false;

        if ( !m_resolver.Refresh(expected, screenPoint, refreshed) )
            return false;

        return IsCurrent(token) &&
               refreshed->target.IsCurrent() &&
               refreshed->IsSameLogicalTarget(expected);
    }

    bool LeaveRoute(const wxMSWOleDropRoute& route, std::uint64_t token)
    {
        wxDropTarget* const target = route.target.GetIfCurrent();
        if ( !target )
            return IsCurrent(token);

        target->OnLeave();
        if ( !IsCurrent(token) )
            return false;

        if ( wxDropTarget* const current = route.target.GetIfCurrent() )
            current->MSWSetDataSource(nullptr);

        return true;
    }

    bool LeaveActive(std::uint64_t token)
    {
        if ( !m_activeRoute )
            return IsCurrent(token);

        // Retire it before invoking user code. A nested terminal operation can
        // no longer send a duplicate Leave to this logical target.
        const wxMSWOleDropRoute route = *m_activeRoute;
        m_activeRoute.reset();
        return LeaveRoute(route, token);
    }

    bool EnterRoute(const wxMSWOleDropRoute& candidate,
                    IDataObject* data,
                    DWORD keyState,
                    const POINTL& screenPoint,
                    DWORD* effect,
                    std::uint64_t token)
    {
        wxDropTarget* target = candidate.target.GetIfCurrent();
        if ( !target )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        const bool accepted = target->MSWIsAcceptedData(data);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        wxMSWOleDropRoute refreshed;
        if ( !RefreshRoute(candidate, screenPoint, &refreshed, token) )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        if ( !accepted )
        {
            *effect = DROPEFFECT_NONE;
            return true;
        }

        target = refreshed.target.GetIfCurrent();
        if ( !target )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        target->MSWSetDataSource(data);

        const DWORD proposedEffect =
            GetDropEffect(keyState, target->GetDefaultAction(), *effect);

        // Publish before OnEnter(): a nested DragLeave must see and retire the
        // target exactly once.
        m_activeRoute =
            std::unique_ptr<wxMSWOleDropRoute>(
                new wxMSWOleDropRoute(std::move(refreshed)));
        const wxDragResult result =
            target->OnEnter(m_activeRoute->clientPoint.x,
                            m_activeRoute->clientPoint.y,
                            ConvertDragEffectToResult(proposedEffect));

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        wxMSWOleDropRoute afterCallback;
        if ( !m_activeRoute ||
             !RefreshRoute(*m_activeRoute, screenPoint, &afterCallback, token) )
        {
            if ( m_activeRoute )
                ClearRouteDataSource(*m_activeRoute);
            m_activeRoute.reset();
            *effect = DROPEFFECT_NONE;
            return false;
        }

        m_activeRoute =
            std::unique_ptr<wxMSWOleDropRoute>(
                new wxMSWOleDropRoute(std::move(afterCallback)));
        *effect = ConvertDragResultToEffect(result);
        return true;
    }

    bool OverActive(DWORD keyState,
                    const POINTL& screenPoint,
                    DWORD* effect,
                    std::uint64_t token)
    {
        if ( !m_activeRoute )
        {
            *effect = DROPEFFECT_NONE;
            return true;
        }

        wxDropTarget* const target =
            m_activeRoute->target.GetIfCurrent();
        if ( !target )
        {
            m_activeRoute.reset();
            *effect = DROPEFFECT_NONE;
            return false;
        }

        const DWORD proposedEffect =
            GetDropEffect(keyState, target->GetDefaultAction(), *effect);
        const wxDragResult result =
            target->OnDragOver(m_activeRoute->clientPoint.x,
                               m_activeRoute->clientPoint.y,
                               ConvertDragEffectToResult(proposedEffect));

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        wxMSWOleDropRoute afterCallback;
        if ( !RefreshRoute(*m_activeRoute, screenPoint, &afterCallback, token) )
        {
            ClearRouteDataSource(*m_activeRoute);
            m_activeRoute.reset();
            *effect = DROPEFFECT_NONE;
            return false;
        }

        m_activeRoute =
            std::unique_ptr<wxMSWOleDropRoute>(
                new wxMSWOleDropRoute(std::move(afterCallback)));
        *effect = ConvertDragResultToEffect(result);
        return true;
    }

    void UpdateRoute(DWORD keyState,
                     const POINTL& screenPoint,
                     DWORD* effect,
                     std::uint64_t token)
    {
        wxMSWOleDropRoute route;
        wxMSWOleDropResolveResult resolution =
            m_resolver.Resolve(screenPoint, &route);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        if ( resolution != wxMSWOleDropResolveResult::Hit ||
             !route.target.IsCurrent() )
        {
            LeaveActive(token);
            *effect = DROPEFFECT_NONE;
            return;
        }

        if ( m_activeRoute &&
             route.IsSameLogicalTarget(*m_activeRoute) )
        {
            *m_activeRoute = std::move(route);
            OverActive(keyState, screenPoint, effect, token);
            return;
        }

        if ( !LeaveActive(token) || !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        // OnLeave is user code and can change the entire tree. Never enter the
        // route resolved before it.
        resolution = m_resolver.Resolve(screenPoint, &route);
        if ( !IsCurrent(token) ||
             resolution != wxMSWOleDropResolveResult::Hit ||
             !route.target.IsCurrent() )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        EnterRoute(route, m_dataObject, keyState, screenPoint, effect,
                   token);
    }

    void DispatchDrop(const wxMSWOleDropRoute& route,
                      IDataObject* data,
                      DWORD keyState,
                      const POINTL& screenPoint,
                      DWORD* effect,
                      std::uint64_t token)
    {
        wxDropTarget* target = route.target.GetIfCurrent();
        if ( !target )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        const bool wantsData =
            target->OnDrop(route.clientPoint.x, route.clientPoint.y);
        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        wxMSWOleDropRoute refreshed;
        if ( !RefreshRoute(route, screenPoint, &refreshed, token) ||
             !wantsData )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        target = refreshed.target.GetIfCurrent();
        if ( !target )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        target->MSWSetDataSource(data);
        const DWORD proposedEffect =
            GetDropEffect(keyState, target->GetDefaultAction(), *effect);
        const wxDragResult result =
            target->OnData(refreshed.clientPoint.x,
                           refreshed.clientPoint.y,
                           ConvertDragEffectToResult(proposedEffect));

        if ( !IsCurrent(token) )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        wxMSWOleDropRoute afterCallback;
        if ( !RefreshRoute(refreshed, screenPoint, &afterCallback, token) )
        {
            *effect = DROPEFFECT_NONE;
            return;
        }

        *effect = wxIsDragResultOk(result)
                    ? ConvertDragResultToEffect(result)
                    : DROPEFFECT_NONE;
    }

    wxMSWOleDropResolver& m_resolver;
    wxMSWOleDropFeedback& m_feedback;
    wxMSWOleDropSessionMode m_mode;
    wxMSWOleDropExceptionPolicy m_exceptionPolicy;
    wxCOMPtr<IDataObject> m_dataObject;
    std::unique_ptr<wxMSWOleDropRoute> m_activeRoute;
    std::unique_ptr<wxMSWOleDropRoute> m_compatLeaveRoute;
    std::unique_ptr<wxMSWOleDropRoute> m_terminalRoute;
    std::uint64_t m_epoch = 0;
    bool m_physicalFeedbackActive = false;
};

wxMSWOleDropSession::wxMSWOleDropSession(
    wxMSWOleDropResolver& resolver,
    wxMSWOleDropFeedback& feedback,
    wxMSWOleDropSessionMode mode,
    wxMSWOleDropExceptionPolicy exceptionPolicy)
    : m_impl(new Impl(resolver, feedback, mode, exceptionPolicy))
{
}

wxMSWOleDropSession::~wxMSWOleDropSession() = default;

HRESULT wxMSWOleDropSession::DragEnter(IDataObject* data,
                                       DWORD keyState,
                                       POINTL screenPoint,
                                       DWORD* effect)
{
#if wxUSE_EXCEPTIONS
    return wxSafeCall<HRESULT>(
        [&]() { return m_impl->DragEnter(data, keyState, screenPoint, effect); },
        [&]()
        {
            if ( m_impl->ShouldReportExceptions() )
                wxEvtHandler::WXConsumeException();
            m_impl->Reset();
            if ( effect )
                *effect = DROPEFFECT_NONE;
            return E_UNEXPECTED;
        });
#else
    return m_impl->DragEnter(data, keyState, screenPoint, effect);
#endif
}

namespace
{

bool CleanupShellHwndOwner(
    const wxMSWOleShellHwndIdentity& identity,
    bool retire,
    IDropTarget* expectedOwner = nullptr,
    std::uint64_t expectedOwnerSerial = 0) noexcept
{
    wxMSWOleFixedPublicCallScope fixedTail;
    if ( !identity.hwnd || !identity.generation || !identity.threadId )
        return true;
    if ( ::GetCurrentThreadId() != identity.threadId )
        return false;

    const wxMSWOleShellHwndKey key(
        reinterpret_cast<HWND>(identity.hwnd), identity.generation);
    constexpr unsigned MaxRevokeAttempts = 2;

    for ( unsigned attempt = 0; attempt < MaxRevokeAttempts; ++attempt )
    {
        IDropTarget *ownerIdentity = nullptr;
        std::uint64_t ownerSerial = 0;
        std::uint64_t reservationId = 0;
        bool callRevoke = false;
        bool staleIdentity = false;
        bool publishWrapperHwnd = false;
        HWND projectedWrapperHwnd = nullptr;
        bool forceContextFreeTombstone = false;

        try
        {
            DropTargetRegistry& registry = GetDropTargetRegistry();
            std::lock_guard<std::mutex> lock(registry.mutex);
            const auto global = registry.shellHwndStates.find(key);
            if ( global == registry.shellHwndStates.end() )
                break;

            if ( expectedOwner &&
                 (global->second.owner.kind !=
                      wxMSWOleGlobalShellOwner::Kind::Fixed ||
                  global->second.owner.identity != expectedOwner ||
                  global->second.owner.operationSerial !=
                      expectedOwnerSerial) )
            {
                return false;
            }

            if ( retire )
                global->second.retiring = true;

            if ( global->second.reservation.id )
            {
                if ( retire )
                    global->second.reservation.retireRequested = true;
                return false;
            }

            staleIdentity =
                !IsShellHwndKeyCurrent(key, identity.threadId);

            // External-managed owners must consume the retire latch in their
            // own native-ops context. Calling USER32 here would bypass their
            // snapshot, test boundary and exact lock ownership.
            if ( global->second.owner.kind ==
                    wxMSWOleGlobalShellOwner::Kind::ExternalManaged )
            {
                return false;
            }

            if ( global->second.owner.kind ==
                    wxMSWOleGlobalShellOwner::Kind::ExternalTombstone )
            {
                // A live tombstone is the physical occupancy proof left by a
                // failed broker revoke and must continue blocking Fixed. Once
                // this exact HWND retires (or is already stale), OLE can no
                // longer route through it and the context-free record is done.
                if ( !retire && !staleIdentity )
                    return false;
                registry.shellHwndStates.erase(global);
                ++registry.mutationGeneration;
                break;
            }

            if ( !global->second.owner.identity )
            {
                if ( !global->second.pendingExternalId )
                    registry.shellHwndStates.erase(global);
                break;
            }

            ownerIdentity = global->second.owner.identity;
            ownerSerial = global->second.owner.operationSerial;
            if ( staleIdentity )
            {
                // A recycled numeric HWND is never a revoke authority. The old
                // native window has already lost its OLE registration; only
                // the retained owner/lock tombstone remains to be balanced.
                global->second.owner.registered = false;
                publishWrapperHwnd = DepublishFixedOwnerLocked(
                    global->second.owner, key, &projectedWrapperHwnd);
            }
            else if ( global->second.owner.registered )
            {
                reservationId =
                    NextNonZero(&registry.shellReservationGeneration);
                global->second.reservation.id = reservationId;
                global->second.reservation.kind =
                    wxMSWOleShellDropTargetOperationKind::Revoke;
                global->second.reservation.identity = ownerIdentity;
                global->second.reservation.operationSerial =
                    NextNonZero(&registry.shellOperationGeneration);
                callRevoke = true;
            }
        }
        catch ( ... )
        {
            return false;
        }

        if ( publishWrapperHwnd && ownerIdentity )
            SetFixedDropTargetHwnd(ownerIdentity, projectedWrapperHwnd);

        HRESULT revokeHr = S_OK;
        if ( callRevoke )
        {
            revokeHr = ::RevokeDragDrop(key.first);
            const bool revoked = SUCCEEDED(revokeHr) ||
                revokeHr == DRAGDROP_E_NOTREGISTERED;

            try
            {
                DropTargetRegistry& registry = GetDropTargetRegistry();
                std::lock_guard<std::mutex> lock(registry.mutex);
                const auto global = registry.shellHwndStates.find(key);
                if ( global == registry.shellHwndStates.end() ||
                     global->second.reservation.id != reservationId ||
                     global->second.reservation.identity != ownerIdentity ||
                     global->second.owner.kind !=
                         wxMSWOleGlobalShellOwner::Kind::Fixed ||
                     global->second.owner.identity != ownerIdentity ||
                     global->second.owner.operationSerial != ownerSerial )
                {
                    return false;
                }

                if ( revoked )
                {
                    global->second.owner.registered = false;
                    global->second.owner.operationSerial =
                        global->second.reservation.operationSerial;
                    publishWrapperHwnd = DepublishFixedOwnerLocked(
                        global->second.owner, key, &projectedWrapperHwnd);
                }
                global->second.reservation =
                    wxMSWOleGlobalShellReservation();
                MarkPendingExternalWakeReadyLocked(global->second);
                ++registry.mutationGeneration;
            }
            catch ( ... )
            {
                return false;
            }


            if ( publishWrapperHwnd && ownerIdentity )
                SetFixedDropTargetHwnd(ownerIdentity, projectedWrapperHwnd);

            if ( !revoked )
            {
                if ( attempt + 1 != MaxRevokeAttempts )
                    continue;
                wxLogError("wxMSW: exact OLE shell-owner retirement failed "
                           "(HRESULT 0x%08lx); retaining fail-closed "
                           "occupancy",
                           static_cast<unsigned long>(revokeHr));
                forceContextFreeTombstone = retire;
            }
        }

        bool callUnlock = false;
        try
        {
            DropTargetRegistry& registry = GetDropTargetRegistry();
            std::lock_guard<std::mutex> lock(registry.mutex);
            const auto global = registry.shellHwndStates.find(key);
            if ( global == registry.shellHwndStates.end() )
                break;
            if ( global->second.owner.kind !=
                    wxMSWOleGlobalShellOwner::Kind::Fixed ||
                 global->second.owner.identity != ownerIdentity )
            {
                return false;
            }
            if ( global->second.owner.externalLockHeld &&
                 !global->second.owner.externalUnlockClaimed )
            {
                global->second.owner.externalUnlockClaimed = true;
                callUnlock = true;
            }
        }
        catch ( ... )
        {
            return false;
        }

        bool unlocked = false;
        if ( callUnlock )
        {
            const HRESULT unlockHr = ::CoLockObjectExternal(
                ownerIdentity, FALSE, TRUE);
            unlocked = SUCCEEDED(unlockHr);
            if ( !unlocked )
            {
                wxLogError("wxMSW: exact OLE shell-owner unlock failed "
                           "(HRESULT 0x%08lx); retaining fail-closed owner",
                           static_cast<unsigned long>(unlockHr));
            }
        }

        IDropTarget *releaseIdentity = nullptr;
        bool clean = false;
        try
        {
            DropTargetRegistry& registry = GetDropTargetRegistry();
            std::lock_guard<std::mutex> lock(registry.mutex);
            const auto global = registry.shellHwndStates.find(key);
            if ( global == registry.shellHwndStates.end() )
                break;
            if ( global->second.owner.kind !=
                    wxMSWOleGlobalShellOwner::Kind::Fixed ||
                 global->second.owner.identity != ownerIdentity )
            {
                return false;
            }
            if ( callUnlock )
                global->second.owner.externalLockHeld = !unlocked;

            if ( forceContextFreeTombstone )
            {
                publishWrapperHwnd = DepublishFixedOwnerLocked(
                    global->second.owner, key, &projectedWrapperHwnd);
                if ( !global->second.owner.externalLockHeld )
                {
                    if ( global->second.owner.identityRetained )
                        releaseIdentity = global->second.owner.identity;
                    const std::uint64_t tombstoneSerial =
                        global->second.owner.operationSerial;
                    global->second.owner = wxMSWOleGlobalShellOwner();
                    global->second.owner.kind =
                        wxMSWOleGlobalShellOwner::Kind::ExternalTombstone;
                    global->second.owner.operationSerial = tombstoneSerial;
                    global->second.owner.registered = true;
                    global->second.owner.externalUnlockClaimed = true;
                }
                // A failed FALSE is an ambiguous strong-lock boundary. Keep
                // the exact Fixed owner and retained identity instead of
                // manufacturing a context-free tombstone or trying FALSE a
                // second time. The retired generation cannot block its ABA
                // successor, but the outstanding lock remains truthful.
            }
            else
            {
                clean = !global->second.owner.registered &&
                        !global->second.owner.externalLockHeld;
                if ( clean )
                {
                    releaseIdentity = global->second.owner.identity;
                    global->second.owner = wxMSWOleGlobalShellOwner();
                    if ( !global->second.reservation.id &&
                         !global->second.pendingExternalId )
                        registry.shellHwndStates.erase(global);
                }
            }
            ++registry.mutationGeneration;
        }
        catch ( ... )
        {
            return false;
        }

        if ( publishWrapperHwnd && ownerIdentity )
            SetFixedDropTargetHwnd(ownerIdentity, projectedWrapperHwnd);
        if ( releaseIdentity )
            releaseIdentity->Release();
        if ( forceContextFreeTombstone )
            return false;
        if ( clean )
            break;
        return false;
    }

#if !defined(__WXWINUI__) || !wxUSE_WINUI3
    if ( retire &&
         QueryShellHwndGeneration(identity.hwnd) == identity.generation )
    {
        ::RemovePropW(reinterpret_cast<HWND>(identity.hwnd),
                      wxMSW_OLE_HWND_GENERATION_PROP);
    }
#else
    wxUnusedVar(retire);
#endif

    try
    {
        DropTargetRegistry& registry = GetDropTargetRegistry();
        std::lock_guard<std::mutex> lock(registry.mutex);
        const auto global = registry.shellHwndStates.find(key);
        return global == registry.shellHwndStates.end() ||
            (global->second.owner.kind ==
                 wxMSWOleGlobalShellOwner::Kind::None &&
             !global->second.owner.registered &&
             !global->second.owner.identity);
    }
    catch ( ... )
    {
        return false;
    }
}

} // anonymous namespace

bool wxMSWOleCleanupShellHwndOwner(
    const wxMSWOleShellHwndIdentity& identity) noexcept
{
    return CleanupShellHwndOwner(identity, false);
}

bool wxMSWOleCleanupShellDropTargetOwner(
    wxMSWOleShellDropTargetOperation* operation) noexcept
{
    if ( !operation || !operation->m_globalTracking ||
         !operation->m_comIdentity || !operation->m_globalOwnerSerial )
    {
        return false;
    }

    wxMSWOleShellHwndIdentity identity;
    identity.hwnd = operation->m_hwnd;
    identity.generation = operation->m_hwndGeneration;
    identity.threadId = operation->m_threadId;
    return CleanupShellHwndOwner(
        identity, false, operation->m_comIdentity,
        operation->m_globalOwnerSerial);
}

void wxMSWOleRetireShellHwnd(
    const wxMSWOleShellHwndIdentity& identity) noexcept
{
    (void)CleanupShellHwndOwner(identity, true);
}

HRESULT wxMSWOleDropSession::DragOver(DWORD keyState,
                                      POINTL screenPoint,
                                      DWORD* effect)
{
#if wxUSE_EXCEPTIONS
    return wxSafeCall<HRESULT>(
        [&]() { return m_impl->DragOver(keyState, screenPoint, effect); },
        [&]()
        {
            if ( m_impl->ShouldReportExceptions() )
                wxEvtHandler::WXConsumeException();
            m_impl->Reset();
            if ( effect )
                *effect = DROPEFFECT_NONE;
            return E_UNEXPECTED;
        });
#else
    return m_impl->DragOver(keyState, screenPoint, effect);
#endif
}

HRESULT wxMSWOleDropSession::DragLeave()
{
#if wxUSE_EXCEPTIONS
    return wxSafeCall<HRESULT>(
        [&]() { return m_impl->DragLeave(); },
        [&]()
        {
            if ( m_impl->ShouldReportExceptions() )
                wxEvtHandler::WXConsumeException();
            m_impl->Reset();
            return E_UNEXPECTED;
        });
#else
    return m_impl->DragLeave();
#endif
}

HRESULT wxMSWOleDropSession::Drop(IDataObject* data,
                                  DWORD keyState,
                                  POINTL screenPoint,
                                  DWORD* effect)
{
#if wxUSE_EXCEPTIONS
    return wxSafeCall<HRESULT>(
        [&]() { return m_impl->Drop(data, keyState, screenPoint, effect); },
        [&]()
        {
            if ( m_impl->ShouldReportExceptions() )
                wxEvtHandler::WXConsumeException();
            m_impl->AbortDrop(data, screenPoint);
            if ( effect )
                *effect = DROPEFFECT_NONE;
            return E_UNEXPECTED;
        });
#else
    return m_impl->Drop(data, keyState, screenPoint, effect);
#endif
}

void wxMSWOleDropSession::Reset() noexcept
{
    if ( m_impl )
        m_impl->Reset();
}

// ----------------------------------------------------------------------------
// Fixed-HWND resolver and feedback used by wxDropTarget's historical wrapper.
// ----------------------------------------------------------------------------

class wxMSWFixedDropResolver final : public wxMSWOleDropResolver
{
public:
    explicit wxMSWFixedDropResolver(wxDropTarget* target)
        : m_target(wxMSWOleAcquireDropTarget(target))
    {
    }

    HWND GetRegisteredHwnd() const { return m_hwnd; }

    void SetRegisteredHwnd(HWND hwnd)
    {
        m_hwnd = hwnd;
        ++m_registrationGeneration;
    }

    wxMSWOleDropResolveResult
    Resolve(const POINTL& screenPoint, wxMSWOleDropRoute* route) override
    {
        if ( !route || !m_hwnd || !m_target.IsCurrent() )
            return wxMSWOleDropResolveResult::Miss;

        POINT point = { screenPoint.x, screenPoint.y };
        if ( !::ScreenToClient(m_hwnd, &point) )
            wxLogLastError(wxT("ScreenToClient"));

        route->target = m_target;
        route->ownerIdentity =
            reinterpret_cast<std::uintptr_t>(m_hwnd);
        route->ownerGeneration = m_registrationGeneration;
        route->clientPoint = point;
        return wxMSWOleDropResolveResult::Hit;
    }

    bool Refresh(const wxMSWOleDropRoute& expected,
                 const POINTL& screenPoint,
                 wxMSWOleDropRoute* refreshed) override
    {
        return Resolve(screenPoint, refreshed) ==
                    wxMSWOleDropResolveResult::Hit &&
               refreshed->IsSameLogicalTarget(expected);
    }

private:
    wxMSWOleDropTargetLease m_target;
    HWND m_hwnd = nullptr;
    std::uint64_t m_registrationGeneration = 0;
};

class wxMSWFixedDropFeedback final : public wxMSWOleDropFeedback
{
public:
    explicit wxMSWFixedDropFeedback(wxDropTarget* target)
        : m_target(wxMSWOleAcquireDropTarget(target))
    {
    }

    void DragEnter(const wxMSWOleDropRoute* route,
                   IDataObject* WXUNUSED(data),
                   const POINTL& WXUNUSED(screenPoint),
                   DWORD effect) noexcept override
    {
        wxDropTarget* target = m_target.GetIfCurrent();
        if ( !target || !route )
            return;

        const wxDragResult result = ConvertDragEffectToResult(effect);
        target->MSWUpdateDragImageOnEnter(route->clientPoint.x,
                                          route->clientPoint.y,
                                          result);
        target = m_target.GetIfCurrent();
        if ( !target )
            return;

        target->MSWUpdateDragImageOnDragOver(route->clientPoint.x,
                                             route->clientPoint.y,
                                             result);
    }

    void DragOver(const wxMSWOleDropRoute* route,
                  const POINTL& screenPoint,
                  DWORD effect) noexcept override
    {
        wxDropTarget* const target = m_target.GetIfCurrent();
        if ( !target )
            return;

        const LONG x = route ? route->clientPoint.x : screenPoint.x;
        const LONG y = route ? route->clientPoint.y : screenPoint.y;
        target->MSWUpdateDragImageOnDragOver(
            x, y, ConvertDragEffectToResult(effect));
    }

    void DragLeave() noexcept override
    {
        if ( wxDropTarget* const target = m_target.GetIfCurrent() )
            target->MSWUpdateDragImageOnLeave();
    }

    void Drop(const wxMSWOleDropRoute* route,
              IDataObject* WXUNUSED(data),
              const POINTL& screenPoint,
              DWORD effect) noexcept override
    {
        wxDropTarget* const target = m_target.GetIfCurrent();
        if ( !target )
            return;

        const LONG x = route ? route->clientPoint.x : screenPoint.x;
        const LONG y = route ? route->clientPoint.y : screenPoint.y;
        target->MSWUpdateDragImageOnData(
            x, y, ConvertDragEffectToResult(effect));
    }

private:
    wxMSWOleDropTargetLease m_target;
};

// ----------------------------------------------------------------------------
// IDropTarget interface: forward all interesting things through the shared
// session to the one fixed wxDropTarget used outside an island host.
// ----------------------------------------------------------------------------

class wxIDropTarget final : public IDropTarget
{
public:
    explicit wxIDropTarget(wxDropTarget* target)
        : m_resolver(target),
          m_feedback(target),
          m_session(m_resolver, m_feedback,
                    wxMSWOleDropSessionMode::FixedTargetCompatibility)
    {
    }

    ~wxIDropTarget() = default;

    HWND GetHWND() const { return m_resolver.GetRegisteredHwnd(); }
    void SetHwnd(HWND hwnd) { m_resolver.SetRegisteredHwnd(hwnd); }
    void InvalidateTarget() noexcept { m_session.Reset(); }

    STDMETHODIMP DragEnter(LPDATAOBJECT data,
                           DWORD keyState,
                           POINTL point,
                           LPDWORD effect) override
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::DragEnter"));
        return InvokePinned([&]()
        {
            return m_session.DragEnter(data, keyState, point, effect);
        });
    }

    STDMETHODIMP DragOver(DWORD keyState,
                          POINTL point,
                          LPDWORD effect) override
    {
        return InvokePinned([&]()
        {
            return m_session.DragOver(keyState, point, effect);
        });
    }

    STDMETHODIMP DragLeave() override
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::DragLeave"));
        return InvokePinned([&]()
        {
            return m_session.DragLeave();
        });
    }

    STDMETHODIMP Drop(LPDATAOBJECT data,
                      DWORD keyState,
                      POINTL point,
                      LPDWORD effect) override
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::Drop"));
        return InvokePinned([&]()
        {
            return m_session.Drop(data, keyState, point, effect);
        });
    }

    DECLARE_IUNKNOWN_METHODS;

private:
    template <typename F>
    HRESULT InvokePinned(F&& fn) noexcept
    {
        // Revoke/delete of the owning wxDropTarget is legal from a callback.
        // Keep this COM wrapper and its session alive until the outer entry
        // point has observed the re-entrant invalidation and unwound.
        AddRef();
        const HRESULT hr = fn();
        Release();
        return hr;
    }

    wxMSWFixedDropResolver m_resolver;
    wxMSWFixedDropFeedback m_feedback;
    wxMSWOleDropSession m_session;

    wxDECLARE_NO_COPY_CLASS(wxIDropTarget);
};

static void SetFixedDropTargetHwnd(IDropTarget* target, HWND hwnd) noexcept
{
    if ( target )
        static_cast<wxIDropTarget*>(target)->SetHwnd(hwnd);
}

// ============================================================================
// wxIDropTarget implementation
// ============================================================================

// Name    : static wxIDropTarget::GetDropEffect
// Purpose : determine the drop operation from keyboard/mouse state.
// Returns : DWORD combined from DROPEFFECT_xxx constants
// Params  : [in] DWORD flags                 kbd & mouse flags as passed to
//                                            IDropTarget methods
//           [in] wxDragResult defaultAction  the default action of the drop target
//           [in] DWORD pdwEffect             the supported actions of the drop
//                                            source passed to IDropTarget methods
// Notes   : We do "move" normally and "copy" if <Ctrl> is pressed,
//           which is the standard behaviour (currently there is no
//           way to redefine it)
static DWORD GetDropEffect(DWORD flags,
                           wxDragResult defaultAction,
                           DWORD pdwEffect)
{
    DWORD effectiveAction;
    if ( defaultAction == wxDragCopy )
        effectiveAction = flags & MK_SHIFT ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
    else
        effectiveAction = flags & MK_CONTROL ? DROPEFFECT_COPY : DROPEFFECT_MOVE;

    if ( !(effectiveAction & pdwEffect) )
    {
        // the action is not supported by drag source, fall back to something
        // that it does support
        if ( pdwEffect & DROPEFFECT_MOVE )
            effectiveAction = DROPEFFECT_MOVE;
        else if ( pdwEffect & DROPEFFECT_COPY )
            effectiveAction = DROPEFFECT_COPY;
        else if ( pdwEffect & DROPEFFECT_LINK )
            effectiveAction = DROPEFFECT_LINK;
        else
            effectiveAction = DROPEFFECT_NONE;
    }

    return effectiveAction;
}

BEGIN_IID_TABLE(wxIDropTarget)
  ADD_IID(Unknown)
  ADD_IID(DropTarget)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIDropTarget)

// ============================================================================
// wxDropTarget implementation
// ============================================================================

IDropTarget*
wxMSWOleDropTargetAccess::GetCOMInterface(wxDropTarget* target) noexcept
{
    return target
        ? reinterpret_cast<IDropTarget*>(target->m_pIDropTarget)
        : nullptr;
}

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget(wxDataObject *dataObj)
            : wxDropTargetBase(dataObj),
              m_pIDropTarget(nullptr),
              m_pIDataSource(nullptr),
              m_dropTargetHelper(nullptr)
{
    // create an IDropTarget implementation which will notify us about d&d
    // operations.
    m_pIDropTarget = new wxIDropTarget(this);
    m_pIDropTarget->AddRef();
}

wxDropTarget::~wxDropTarget()
{
    // Depublish the callback-bearing helper before invalidating any other
    // member. Its Release() is arbitrary COM code, so keep it in a local and
    // cross that boundary only after the wrapper has been touched for the
    // final time.
    wxIDropTargetHelper * const dragImageHelper = m_dropTargetHelper;
    m_dropTargetHelper = nullptr;
    // A callback already on the stack may outlive the owning wxWindow. Make
    // its lease fail before any derived state is released.
    wxMSWOleInvalidateDropTarget(this);
    m_pIDropTarget->InvalidateTarget();
    ReleaseInterface(m_pIDropTarget);
    if ( dragImageHelper )
        dragImageHelper->Release();
}

// ----------------------------------------------------------------------------
// [un]register drop handler
// ----------------------------------------------------------------------------

bool wxDropTarget::Register(WXHWND hwnd)
{
    wxMSWOleFixedPublicCallScope fixedTail;
    wxDropTarget * const identity = this;
    const wxMSWOleDropTargetLease lifetime =
        wxMSWOleAcquireDropTarget(identity);
    wxIDropTarget * const comTarget = m_pIDropTarget;
    if ( !comTarget || !lifetime.IsCurrent() )
        return false;

    wxMSWOleShellDropTargetOperation operation =
        wxMSWOleBeginShellDropTargetOperation(
            identity, hwnd,
            wxMSWOleShellDropTargetOperationKind::Register,
            comTarget);
    if ( !operation.IsActive() )
        return false;

    // Keep the fixed COM wrapper alive even if CoLockObjectExternal() or
    // RegisterDragDrop() pumps a callback which destroys its wx owner.
    comTarget->AddRef();
    if ( lifetime.GetIfCurrent() != identity )
    {
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        comTarget->Release();
        return false;
    }

    HRESULT hr = ::CoLockObjectExternal(comTarget, TRUE, FALSE);
    if ( FAILED(hr) ) {
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        wxLogApiError(wxT("CoLockObjectExternal"), hr);
        comTarget->Release();
        return false;
    }
    wxMSWOleNoteShellDropTargetExternalLock(&operation);

    const auto balanceExternalLock = [&](BOOL lastUnlockReleases)
    {
        if ( !wxMSWOleClaimShellDropTargetExternalUnlock(&operation) )
            return;
        const HRESULT unlockHr = ::CoLockObjectExternal(
            comTarget, FALSE, lastUnlockReleases);
        wxMSWOleNoteShellDropTargetExternalUnlock(
            &operation, SUCCEEDED(unlockHr));
        if ( FAILED(unlockHr) )
            wxLogApiError(wxT("CoLockObjectExternal(FALSE)"), unlockHr);
    };

    // CoLockObjectExternal may dispatch messages. Never create a native
    // registration for a wrapper whose exact wx lifetime disappeared there.
    if ( lifetime.GetIfCurrent() != identity )
    {
        balanceExternalLock(FALSE);
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        comTarget->Release();
        return false;
    }

    // Assign ordering only after every preparatory COM boundary. A successful
    // nested operation pumped by CoLock therefore precedes this native write.
    if ( !wxMSWOleArmShellDropTargetOperation(&operation) )
    {
        balanceExternalLock(FALSE);
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        comTarget->Release();
        return false;
    }

    hr = ::RegisterDragDrop((HWND) hwnd, comTarget);
    if ( FAILED(hr) ) {
        balanceExternalLock(FALSE);
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        wxLogApiError(wxT("RegisterDragDrop"), hr);
        comTarget->Release();
        return false;
    }

    // Record physical ownership before inspecting wx lifetime. The global
    // exact-HWND ledger retains this COM identity independently, allowing a
    // compensating revoke only while this serial is still the owner.
    bool committed =
        wxMSWOleCompleteShellDropTargetOperation(&operation, true);
    const bool lifetimeCurrent = lifetime.GetIfCurrent() == identity;
    if ( !lifetimeCurrent || !committed )
    {
        bool revoked = false;
        if ( wxMSWOleReserveShellDropTargetCompensation(&operation) )
        {
            const HRESULT revokeHr = ::RevokeDragDrop((HWND)hwnd);
            revoked = SUCCEEDED(revokeHr) ||
                revokeHr == DRAGDROP_E_NOTREGISTERED;
            (void)wxMSWOleCompleteShellDropTargetCompensation(
                &operation, revoked);
            if ( !revoked )
            {
                wxLogApiError(wxT("RevokeDragDrop(compensation)"), revokeHr);
            }
        }

        // This TRUE belongs to this Register() even when exact compensation
        // was no longer safe or failed. The native/global retained refs, not
        // an unbalanced external lock, keep a fail-closed tombstone alive.
        balanceExternalLock(TRUE);
        // A retire request or lost HWND generation can make compensation
        // ineligible even though this exact RegisterDragDrop() succeeded.
        // Reconcile only the owner serial committed by this token; a nested
        // broker/fixed replacement on the same key is never revoke authority.
        (void)wxMSWOleCleanupShellDropTargetOwner(&operation);
        committed = false;
    }

    wxDropTarget * const liveTarget = lifetime.GetIfCurrent();
    if ( committed && liveTarget == identity )
    {
        // Zero and Ambiguous both project fail-closed HWND 0. A valid
        // registration publishes only the one derived Unique key.
        comTarget->SetHwnd(
            operation.GetProjection() ==
                    wxMSWOleShellDropTargetProjection::Unique
                ? reinterpret_cast<HWND>(operation.GetProjectedHwnd())
                : nullptr);

        // CoCreateInstance is another pumping boundary. Build the helper into
        // a local and publish it only if this registration is still the newest
        // successful operation afterwards.
        wxIDropTargetHelper *dragImageHelper = nullptr;
        ::CoCreateInstance(wxCLSID_DragDropHelper,
                           nullptr,
                           CLSCTX_INPROC_SERVER,
                           wxIID_IDropTargetHelper,
                           reinterpret_cast<void**>(&dragImageHelper));
        wxDropTarget * const current = lifetime.GetIfCurrent();
        if ( dragImageHelper && current == identity &&
             wxMSWOleIsLatestShellDropTargetOperation(operation) &&
             operation.GetProjectedHwnd() == hwnd &&
             wxMSWOleIsShellDropTargetRegistered(identity, hwnd) )
        {
            // Publish without crossing a COM boundary. Release() of the old
            // in-proc helper is arbitrary code and may synchronously destroy
            // or replace this target, so move it to a local and never touch
            // the wx wrapper again after calling it.
            wxIDropTargetHelper * const previousHelper =
                current->m_dropTargetHelper;
            current->m_dropTargetHelper = dragImageHelper;
            dragImageHelper = nullptr;
            if ( previousHelper )
                previousHelper->Release();
        }
        if ( dragImageHelper )
            dragImageHelper->Release();
    }

    const bool registered =
        lifetime.GetIfCurrent() == identity &&
        wxMSWOleIsShellDropTargetRegistered(identity, hwnd);
    comTarget->Release();

    return registered;
}

void wxDropTarget::Revoke(WXHWND hwnd)
{
    wxMSWOleFixedPublicCallScope fixedTail;
    wxDropTarget * const identity = this;
    const wxMSWOleDropTargetLease lifetime =
        wxMSWOleAcquireDropTarget(identity);
    wxIDropTarget * const comTarget = m_pIDropTarget;
    if ( !comTarget || !lifetime.IsCurrent() )
        return;

    wxMSWOleShellDropTargetOperation operation =
        wxMSWOleBeginShellDropTargetOperation(
            identity, hwnd,
            wxMSWOleShellDropTargetOperationKind::Revoke,
            comTarget);
    if ( !operation.IsActive() )
        return;
    comTarget->AddRef();
    if ( lifetime.GetIfCurrent() != identity )
    {
        if ( wxMSWOleClaimShellDropTargetExternalUnlock(&operation) )
        {
            const HRESULT unlockHr =
                ::CoLockObjectExternal(comTarget, FALSE, TRUE);
            wxMSWOleNoteShellDropTargetExternalUnlock(
                &operation, SUCCEEDED(unlockHr));
            if ( FAILED(unlockHr) )
                wxLogApiError(wxT("CoLockObjectExternal(FALSE)"), unlockHr);
        }
        (void)wxMSWOleCancelShellDropTargetOperation(&operation);
        comTarget->Release();
        return;
    }

    // The helper is part of this registration. AddRef() is already a COM
    // boundary: reacquire the exact wrapper before touching another member,
    // move the helper out before RevokeDragDrop(), and continue from the COM
    // pin alone if the wrapper disappeared during AddRef().
    wxIDropTargetHelper *dragImageHelper = nullptr;
    if ( wxDropTarget * const liveTarget = lifetime.GetIfCurrent() )
    {
        if ( liveTarget == identity )
        {
            dragImageHelper = liveTarget->m_dropTargetHelper;
            liveTarget->m_dropTargetHelper = nullptr;
        }
    }

    if ( !wxMSWOleArmShellDropTargetOperation(&operation) )
    {
        bool unlockSucceeded = true;
        if ( wxMSWOleClaimShellDropTargetExternalUnlock(&operation) )
        {
            const HRESULT unlockHr =
                ::CoLockObjectExternal(comTarget, FALSE, TRUE);
            unlockSucceeded = SUCCEEDED(unlockHr);
            wxMSWOleNoteShellDropTargetExternalUnlock(
                &operation, unlockSucceeded);
            if ( !unlockSucceeded )
                wxLogApiError(wxT("CoLockObjectExternal(FALSE)"), unlockHr);
        }
        const bool restoreRegistrationState =
            wxMSWOleCancelShellDropTargetOperation(&operation);
        wxDropTarget * const liveTarget = lifetime.GetIfCurrent();
        if ( restoreRegistrationState && liveTarget == identity )
            liveTarget->m_dropTargetHelper = dragImageHelper;
        else if ( dragImageHelper )
            dragImageHelper->Release();
        comTarget->Release();
        return;
    }

    HRESULT hr = ::RevokeDragDrop((HWND) hwnd);
    const bool physicallyRevoked = SUCCEEDED(hr) ||
        hr == DRAGDROP_E_NOTREGISTERED;

    if ( !physicallyRevoked ) {
        if ( wxMSWOleClaimShellDropTargetExternalUnlock(&operation) )
        {
            const HRESULT unlockHr =
                ::CoLockObjectExternal(comTarget, FALSE, TRUE);
            wxMSWOleNoteShellDropTargetExternalUnlock(
                &operation, SUCCEEDED(unlockHr));
            if ( FAILED(unlockHr) )
                wxLogApiError(wxT("CoLockObjectExternal(FALSE)"), unlockHr);
        }
        const bool restoreRegistrationState =
            wxMSWOleCancelShellDropTargetOperation(&operation);
        // RegisterDragDrop and the exact global owner retain the inactive COM
        // identity. Restore the helper only if the unlock boundary did not
        // publish a newer projection; the TRUE has already been balanced.
        wxDropTarget * const liveTarget = lifetime.GetIfCurrent();
        if ( restoreRegistrationState && liveTarget == identity )
            liveTarget->m_dropTargetHelper = dragImageHelper;
        else if ( dragImageHelper )
        {
            dragImageHelper->Release();
        }
        wxLogApiError(wxT("RevokeDragDrop"), hr);
        comTarget->Release();
        return;
    }

    const bool committed =
        wxMSWOleCompleteShellDropTargetOperation(&operation, false);

    // Everything below is owned by locals. Neither CoLockObjectExternal nor
    // logging/helper Release is allowed to lead back to the wx wrapper.
    if ( committed )
    {
        comTarget->SetHwnd(
            operation.GetProjection() ==
                    wxMSWOleShellDropTargetProjection::Unique
                ? reinterpret_cast<HWND>(operation.GetProjectedHwnd())
                : nullptr);
    }

    if ( wxMSWOleClaimShellDropTargetExternalUnlock(&operation) )
    {
        const HRESULT unlockHr =
            ::CoLockObjectExternal(comTarget, FALSE, TRUE);
        wxMSWOleNoteShellDropTargetExternalUnlock(
            &operation, SUCCEEDED(unlockHr));
        if ( FAILED(unlockHr) )
            wxLogApiError(wxT("CoLockObjectExternal(FALSE)"), unlockHr);
    }

    if ( dragImageHelper )
        dragImageHelper->Release();
    comTarget->Release();
}

// ----------------------------------------------------------------------------
// base class pure virtuals
// ----------------------------------------------------------------------------

// OnDrop() is called only if we previously returned true from
// IsAcceptedData(), so no need to check anything here
bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return true;
}

// copy the data from the data source to the target data object
bool wxDropTarget::GetData()
{
    wxDataFormat format = MSWGetSupportedFormat(m_pIDataSource);
    if ( format == wxDF_INVALID ) {
        return false;
    }

    STGMEDIUM stm;
    FORMATETC fmtMemory;
    fmtMemory.cfFormat  = format;
    fmtMemory.ptd       = nullptr;
    fmtMemory.dwAspect  = DVASPECT_CONTENT;
    fmtMemory.lindex    = -1;
    fmtMemory.tymed     = TYMED_HGLOBAL;  // TODO to add other media

    bool rc = false;

    HRESULT hr = m_pIDataSource->GetData(&fmtMemory, &stm);
    if ( SUCCEEDED(hr) ) {
        IDataObject *dataObject = m_dataObject->GetInterface();

        hr = dataObject->SetData(&fmtMemory, &stm, TRUE);
        if ( SUCCEEDED(hr) ) {
            rc = true;
        }
        else {
            wxLogApiError(wxT("IDataObject::SetData()"), hr);
        }
    }
    else {
        wxLogApiError(wxT("IDataObject::GetData()"), hr);
    }

    return rc;
}

// ----------------------------------------------------------------------------
// callbacks used by wxIDropTarget
// ----------------------------------------------------------------------------

// we need a data source, so wxIDropTarget gives it to us using this function
void wxDropTarget::MSWSetDataSource(IDataObject *pIDataSource)
{
    m_pIDataSource = pIDataSource;
}

// determine if we accept data of this type
bool wxDropTarget::MSWIsAcceptedData(IDataObject *pIDataSource) const
{
    return MSWGetSupportedFormat(pIDataSource) != wxDF_INVALID;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

wxDataFormat wxDropTarget::GetMatchingPair()
{
    return MSWGetSupportedFormat( m_pIDataSource );
}

wxDataFormat wxDropTarget::MSWGetSupportedFormat(IDataObject *pIDataSource) const
{
    // this structure describes a data of any type (first field will be
    // changing) being passed through global memory block.
    static FORMATETC s_fmtMemory = {
        0,
        nullptr,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL       // TODO is it worth supporting other tymeds here?
    };

    // get the list of supported formats
    size_t nFormats = m_dataObject->GetFormatCount(wxDataObject::Set);
    wxDataFormat format;
    wxDataFormat *formats;
    formats = nFormats == 1 ? &format :  new wxDataFormat[nFormats];

    m_dataObject->GetAllFormats(formats, wxDataObject::Set);

    // cycle through all supported formats
    size_t n;
    for ( n = 0; n < nFormats; n++ ) {
        s_fmtMemory.cfFormat = formats[n];

        // NB: don't use SUCCEEDED macro here: QueryGetData returns S_FALSE
        //     for file drag and drop (format == CF_HDROP)
        if ( pIDataSource->QueryGetData(&s_fmtMemory) == S_OK ) {
            format = formats[n];

            break;
        }
    }

    if ( formats != &format ) {
        // free memory if we allocated it
        delete [] formats;
    }

    return n < nFormats ? format : wxFormatInvalid;
}

// ----------------------------------------------------------------------------
// drag image functions
// ----------------------------------------------------------------------------

void
wxDropTarget::MSWEndDragImageSupport()
{
    // release drop target helper
    if ( m_dropTargetHelper != nullptr )
    {
        m_dropTargetHelper->Release();
        m_dropTargetHelper = nullptr;
    }
}

void
wxDropTarget::MSWInitDragImageSupport()
{
    // Use the default drop target helper to show shell drag images
    CoCreateInstance(wxCLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                     wxIID_IDropTargetHelper, (LPVOID*)&m_dropTargetHelper);
}

void
wxDropTarget::MSWUpdateDragImageOnData(wxCoord x,
                                       wxCoord y,
                                       wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != nullptr )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->Drop(m_pIDataSource, &pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnDragOver(wxCoord x,
                                           wxCoord y,
                                           wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != nullptr )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->DragOver(&pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnEnter(wxCoord x,
                                        wxCoord y,
                                        wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != nullptr )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->DragEnter(m_pIDropTarget->GetHWND(), m_pIDataSource, &pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnLeave()
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != nullptr )
    {
        m_dropTargetHelper->DragLeave();
    }
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDragResult ConvertDragEffectToResult(DWORD dwEffect)
{
    switch ( dwEffect ) {
        case DROPEFFECT_COPY:
            return wxDragCopy;

        case DROPEFFECT_LINK:
            return wxDragLink;

        case DROPEFFECT_MOVE:
            return wxDragMove;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragEffectToResult"));
            wxFALLTHROUGH;

        case DROPEFFECT_NONE:
            return wxDragNone;
    }
}

static DWORD ConvertDragResultToEffect(wxDragResult result)
{
    switch ( result ) {
        case wxDragCopy:
            return DROPEFFECT_COPY;

        case wxDragLink:
            return DROPEFFECT_LINK;

        case wxDragMove:
            return DROPEFFECT_MOVE;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragResultToEffect"));
            wxFALLTHROUGH;

        case wxDragNone:
            return DROPEFFECT_NONE;
    }
}

#endif // wxUSE_OLE && wxUSE_DRAG_AND_DROP
