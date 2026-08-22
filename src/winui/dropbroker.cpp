/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dropbroker.cpp
// Purpose:     one dual-adapter OLE drop broker per WinUI top-level island
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/dnd.h"
#include "wx/msw/private/dropsession.h"
#include "wx/weakref.h"
#include "wx/winui/private/dropbroker.h"
#include "wx/winui/private/inputrouter.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <algorithm>
#include <atomic>
#include <initguid.h>
#include <mutex>
#include <roerrorapi.h>
#include <set>
#include <vector>

namespace
{

// Keep this local for compatibility with SDKs predating IDropTargetHelper.
struct wxWinUIDropTargetHelper : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE DragEnter(HWND,
                                                IDataObject*,
                                                POINT*,
                                                DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragLeave() = 0;
    virtual HRESULT STDMETHODCALLTYPE DragOver(POINT*, DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject*, POINT*, DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE Show(BOOL) = 0;
};

DEFINE_GUID(wxWinUICLSID_DragDropHelper,
            0x4657278A, 0x411B, 0x11D2, 0x83, 0x9A,
            0x00, 0xC0, 0x4F, 0xD9, 0x18, 0xD0);
DEFINE_GUID(wxWinUIIID_IDropTargetHelper,
            0x4657278B, 0x411B, 0x11D2, 0x83, 0x9A,
            0x00, 0xC0, 0x4F, 0xD9, 0x18, 0xD0);

HRESULT NativeLock(void*, IUnknown* object, BOOL lock, BOOL lastUnlockReleases)
    noexcept
{
    return ::CoLockObjectExternal(object, lock, lastUnlockReleases);
}

HRESULT NativeRegister(void*, HWND hwnd, IDropTarget* target) noexcept
{
    const HRESULT hr = ::RegisterDragDrop(hwnd, target);
    if ( SUCCEEDED(hr) )
    {
        // RegisterDragDrop() can internally originate a recoverable WinRT
        // error while still succeeding. A successful ABI result cannot convey
        // that restricted error, so don't let it poison the next XAML call.
        ::RoClearError();
    }

    return hr;
}

HRESULT NativeRevoke(void*, HWND hwnd) noexcept
{
    return ::RevokeDragDrop(hwnd);
}

wxWinUIDropBrokerNativeOps GetNativeOps()
{
    wxWinUIDropBrokerNativeOps ops;
    ops.lock = NativeLock;
    ops.registerTarget = NativeRegister;
    ops.revokeTarget = NativeRevoke;
    return ops;
}

struct BoundTarget
{
    BoundTarget(const wxMSWOleDropTargetBinding& binding_,
                wxDropTarget* target_,
                wxWindow* owner_,
                HWND hwnd_)
        : binding(binding_),
          target(target_),
          owner(owner_),
          hwnd(hwnd_)
    {
    }

    wxMSWOleDropTargetBinding binding;
    wxDropTarget* target = nullptr;
    wxWindow* owner = nullptr;
    HWND hwnd = nullptr;
};

bool CollectBoundTargets(wxWindow* root, std::vector<BoundTarget>* targets)
{
    if ( !root || !targets )
        return false;

    const wxWeakRef<wxWindow> rootLifetime(root);
    std::set<wxDropTarget*> seen;
    std::vector<wxWeakRef<wxWindow>> pending;
    pending.emplace_back(root);

    while ( !pending.empty() )
    {
        const wxWeakRef<wxWindow> queriedLifetime = pending.back();
        pending.pop_back();
        wxWindow* const queried = queriedLifetime.get();
        if ( !queried || queried->IsBeingDeleted() )
            return false;

        wxMSWOleDropTargetBinding binding;
        const wxMSWOleDropTargetLookup lookup =
            wxMSWOleLookupDropTarget(queried, &binding);
        if ( lookup == wxMSWOleDropTargetLookup::Unstable )
            return false;
        if ( lookup == wxMSWOleDropTargetLookup::Found )
        {
            wxDropTarget* const target = binding.GetTargetIfCurrent();
            wxWindow* const owner = binding.GetOwnerIfCurrent();
            const HWND hwnd =
                reinterpret_cast<HWND>(binding.GetOwnerHwndIfCurrent());
            if ( target && owner && hwnd && seen.insert(target).second )
                targets->push_back(
                    BoundTarget(binding, target, owner, hwnd));
        }

        wxWindow* const current = queriedLifetime.get();
        if ( !current || current != queried || current->IsBeingDeleted() )
            return false;

        for ( wxWindow* const child : queried->GetChildren() )
            pending.emplace_back(child);
    }

    return rootLifetime.get() == root && !root->IsBeingDeleted();
}

} // anonymous namespace

// This state is the only object retained by the COM target. It deliberately
// contains no raw host pointer: every callback re-acquires the invalidatable
// HostLifetime and fails closed after Shutdown().
class wxWinUIDropBrokerState final : public wxMSWOleDropResolver,
                                     public wxMSWOleDropFeedback
{
public:
    wxWinUIDropBrokerState(
        wxWindow* tlw,
        HWND bridge,
        const std::weak_ptr<wxWinUIHostLifetime>& lifetime)
        : m_lifetime(lifetime),
          m_tlw(tlw),
          m_registrationHwnd(tlw ? GetHwndOf(tlw) : nullptr),
          m_registrationGeneration(
              m_registrationHwnd
                  ? wxWinUIMSWGetNativeHwndGeneration(
                        reinterpret_cast<WXHWND>(m_registrationHwnd))
                  : 0),
          m_bridge(bridge),
          m_bridgeGeneration(
              wxWinUIMSWGetNativeHwndGeneration(
                  reinterpret_cast<WXHWND>(bridge))),
          m_uiThread(::GetCurrentThreadId()),
          m_session(*this, *this,
                    wxMSWOleDropSessionMode::DynamicBroker)
    {
        m_snapshot.registrationHwnd = m_registrationHwnd;
        m_snapshot.bridgeRegistration.registrationHwnd = m_bridge;
        m_snapshot.bridgeRegistration.generation = m_bridgeGeneration;
        m_snapshot.tlwRegistration.registrationHwnd = m_registrationHwnd;
        m_snapshot.tlwRegistration.generation = m_registrationGeneration;
        m_snapshot.bridgeRegistration.current = IsBridgeCurrent();
        m_snapshot.tlwRegistration.current = IsRegistrationCurrent();
    }

    ~wxWinUIDropBrokerState() override
    {
        // Normal host shutdown releases this apartment-affine helper on the
        // UI thread. A late external COM reference may release State on
        // another thread; leaking the helper in that exceptional path is
        // safer than invoking its apartment-affine Release there.
        if ( m_helper )
        {
            if ( IsOnUIThread() )
                m_helper->Release();
            else
                wxLogError("wxWinUI: leaking a drag-image helper after "
                           "wrong-thread broker destruction");
        }
    }

    // Helper activation is a COM boundary and may pump a terminal host
    // callback. Keep it separate from publishing m_active so the broker can
    // revalidate its acquisition before COM callbacks become routable.
    void PrepareHelper()
    {
        void* helper = nullptr;
        if ( SUCCEEDED(::CoCreateInstance(
                wxWinUICLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                wxWinUIIID_IDropTargetHelper, &helper)) )
        {
            m_helper = static_cast<wxWinUIDropTargetHelper*>(helper);
        }
    }

    void Activate() noexcept
    {
        m_active.store(true, std::memory_order_release);
    }

    void FailClosed() noexcept
    {
        m_active.store(false, std::memory_order_release);
    }

    void DeactivateOnUIThread() noexcept
    {
        wxASSERT_MSG(IsOnUIThread(),
                     "WinUI OLE broker teardown must run on its UI thread");
        FailClosed();
        m_session.Reset();

        // Reset() has already emitted the matching physical DragLeave, if
        // necessary. Release the apartment-affine helper before State can be
        // retained by a late IDropTarget reference on another thread.
        ++m_feedbackEpoch;
        m_physicalActive = false;
        if ( m_helper )
        {
            m_helper->Release();
            m_helper = nullptr;
        }
    }

    bool IsActive() const noexcept
        { return m_active.load(std::memory_order_acquire); }

    bool IsOnUIThread() const noexcept
        { return ::GetCurrentThreadId() == m_uiThread; }

    bool IsBridgeCurrent() const noexcept
    {
        return m_bridge && ::IsWindow(m_bridge) &&
               wxWinUIMSWGetNativeHwndGeneration(
                   reinterpret_cast<WXHWND>(m_bridge)) ==
                    m_bridgeGeneration &&
               ::GetWindowThreadProcessId(m_bridge, nullptr) == m_uiThread;
    }

    bool IsRegistrationCurrent() const noexcept
    {
        return m_registrationHwnd && m_registrationGeneration != 0 &&
               ::IsWindow(m_registrationHwnd) &&
               wxWinUIMSWGetNativeHwndGeneration(
                   reinterpret_cast<WXHWND>(m_registrationHwnd)) ==
                    m_registrationGeneration &&
               ::GetWindowThreadProcessId(m_registrationHwnd, nullptr) ==
                    m_uiThread;
    }

    bool IsContextCurrent() const noexcept
    {
        return IsRegistrationCurrent() && IsBridgeCurrent();
    }

    wxWindow* GetTLW() const noexcept
        { return m_tlw.get(); }

    HWND GetBridge() const noexcept
        { return m_bridge; }

    std::uint64_t GetBridgeGeneration() const noexcept
        { return m_bridgeGeneration; }

    HWND GetRegistrationHwnd() const noexcept
        { return m_registrationHwnd; }

    std::uint64_t GetRegistrationGeneration() const noexcept
        { return m_registrationGeneration; }

    wxWinUIDropBrokerSnapshot GetSnapshot() const noexcept
    {
        // Query native identity before taking the snapshot mutex. Generation
        // lookup may initialize a HWND cookie and therefore crosses a USER32
        // boundary that can re-enter broker diagnostics.
        const bool bridgeCurrent = IsBridgeCurrent();
        const bool tlwCurrent = IsRegistrationCurrent();
        std::lock_guard<std::mutex> lock(m_snapshotMutex);
        wxWinUIDropBrokerSnapshot snapshot = m_snapshot;
        snapshot.active = IsActive();
        // `current` is evidence about the HWND identities at the instant the
        // snapshot is consumed, not merely when the last native operation
        // happened.  In particular, the physical qualification gate takes a
        // fresh snapshot immediately before input injection and must reject a
        // bridge/TLW generation that churned after initialization.
        snapshot.bridgeRegistration.current = bridgeCurrent;
        snapshot.tlwRegistration.current = tlwCurrent;
        return snapshot;
    }

    template <typename F>
    void MutateSnapshot(F&& fn) noexcept
    {
#if wxUSE_EXCEPTIONS
        try
        {
#endif
            std::lock_guard<std::mutex> lock(m_snapshotMutex);
            fn(m_snapshot);
#if wxUSE_EXCEPTIONS
        }
        catch ( ... )
        {
        }
#endif
    }

    HRESULT COMDragEnter(IDataObject* data,
                         DWORD keyState,
                         POINTL point,
                         DWORD* effect) noexcept
    {
        MutateSnapshot([](wxWinUIDropBrokerSnapshot& snapshot)
        {
            ++snapshot.dragEnterCalls;
        });
        return InvokeEffect(effect, [&]()
        {
            return m_session.DragEnter(data, keyState, point, effect);
        });
    }

    HRESULT COMDragOver(DWORD keyState,
                        POINTL point,
                        DWORD* effect) noexcept
    {
        MutateSnapshot([](wxWinUIDropBrokerSnapshot& snapshot)
        {
            ++snapshot.dragOverCalls;
        });
        return InvokeEffect(effect, [&]()
        {
            return m_session.DragOver(keyState, point, effect);
        });
    }

    HRESULT COMDragLeave() noexcept
    {
        MutateSnapshot([](wxWinUIDropBrokerSnapshot& snapshot)
        {
            ++snapshot.dragLeaveCalls;
        });
        if ( !IsActive() )
            return S_OK;
        if ( !IsOnUIThread() )
            return RPC_E_WRONG_THREAD;

#if wxUSE_EXCEPTIONS
        try
        {
#endif
            return m_session.DragLeave();
#if wxUSE_EXCEPTIONS
        }
        catch ( ... )
        {
            m_session.Reset();
            return E_UNEXPECTED;
        }
#endif
    }

    HRESULT COMDrop(IDataObject* data,
                    DWORD keyState,
                    POINTL point,
                    DWORD* effect) noexcept
    {
        MutateSnapshot([](wxWinUIDropBrokerSnapshot& snapshot)
        {
            ++snapshot.dropCalls;
        });
        return InvokeEffect(effect, [&]()
        {
            return m_session.Drop(data, keyState, point, effect);
        });
    }

    wxMSWOleDropResolveResult
    Resolve(const POINTL& screenPoint, wxMSWOleDropRoute* route) override
    {
        if ( route )
            *route = {};
        if ( !route || !IsActive() || !IsOnUIThread() ||
             !IsContextCurrent() )
        {
            return wxMSWOleDropResolveResult::Unstable;
        }

        const std::shared_ptr<wxWinUIHostLifetime> lifetime =
            m_lifetime.lock();
        wxWinUITopLevelHost* const host =
            lifetime ? lifetime->GetHost() : nullptr;
        wxWindow* const tlw = m_tlw.get();
        if ( !host || !tlw || host->m_shuttingDown ||
             host->m_tlw != tlw || host->m_bridge != m_bridge ||
             GetHwndOf(tlw) != m_registrationHwnd )
        {
            return wxMSWOleDropResolveResult::Unstable;
        }

        wxWinUITopLevelHost::OperationGuard operation(host);
        if ( lifetime->GetHost() != host || host->m_shuttingDown )
            return wxMSWOleDropResolveResult::Unstable;

        const POINT screen = { screenPoint.x, screenPoint.y };
        wxWindow* seed = nullptr;
        const wxWinUITopLevelHost::DropSurfaceResolution surface =
            host->ResolveDropSurface(screen, &seed);
        if ( surface ==
                wxWinUITopLevelHost::DropSurfaceResolution::Unstable )
            return wxMSWOleDropResolveResult::Unstable;
        if ( surface ==
                wxWinUITopLevelHost::DropSurfaceResolution::Blocking )
            return wxMSWOleDropResolveResult::Miss;

        wxWinUINativeHit nativeHit;
        if ( surface ==
                wxWinUITopLevelHost::DropSurfaceResolution::Native )
        {
            const HWND inner = host->m_inner;
            if ( inner && (!::IsWindow(inner) ||
                           ::GetWindowThreadProcessId(inner, nullptr) !=
                               m_uiThread) )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }

            const wxWinUIHitResolution hit = wxWinUIResolveNativeHit(
                tlw, screen,
                reinterpret_cast<WXHWND>(m_bridge),
                reinterpret_cast<WXHWND>(inner),
                &nativeHit);
            if ( hit == wxWinUIHitResolution::Unstable )
                return wxMSWOleDropResolveResult::Unstable;
            if ( hit == wxWinUIHitResolution::Miss )
                return wxMSWOleDropResolveResult::Miss;
            seed = nativeHit.GetTarget().GetWindow();
        }

        if ( !seed || seed->IsBeingDeleted() )
            return wxMSWOleDropResolveResult::Miss;

        const std::uint64_t mutationBefore =
            wxMSWOleGetDropTargetMutationGeneration();
        wxMSWOleDropRoute candidate;
        bool found = false;

        wxWeakRef<wxWindow> queriedLifetime(seed);
        while ( wxWindow* queried = queriedLifetime.get() )
        {
            if ( wxGetTopLevelParent(queried) != tlw )
                return wxMSWOleDropResolveResult::Unstable;

            wxWindow* const parentBefore = queried->GetParent();
            const wxWeakRef<wxWindow> parentLifetime(parentBefore);
            wxMSWOleDropTargetBinding binding;
            const wxMSWOleDropTargetLookup lookup =
                wxMSWOleLookupDropTarget(queried, &binding);
            wxWindow* const current = queriedLifetime.get();
            if ( !current || current != queried ||
                 current->GetParent() != parentBefore )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }
            if ( lookup == wxMSWOleDropTargetLookup::Unstable )
                return wxMSWOleDropResolveResult::Unstable;
            if ( lookup == wxMSWOleDropTargetLookup::None )
            {
                if ( queried == tlw )
                    break;
                queriedLifetime = parentLifetime;
                continue;
            }

            wxWindow* const owner = binding.GetOwnerIfCurrent();
            wxDropTarget* const target = binding.GetTargetIfCurrent();
            const HWND ownerHwnd =
                reinterpret_cast<HWND>(binding.GetOwnerHwndIfCurrent());
            const wxWeakRef<wxWindow> ownerLifetime(owner);
            if ( !owner || !target || !ownerHwnd ||
                  owner->IsBeingDeleted() ||
                  wxGetTopLevelParent(owner) != tlw ||
                  GetHwndOf(owner) != ownerHwnd ||
                  !::IsWindow(ownerHwnd) ||
                  ::GetWindowThreadProcessId(ownerHwnd, nullptr) !=
                     m_uiThread )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }

            const bool shown = owner->IsShownOnScreen();
            if ( ownerLifetime.get() != owner ||
                 !binding.IsCurrent() ||
                 wxGetTopLevelParent(owner) != tlw )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }
            if ( !shown )
                return wxMSWOleDropResolveResult::Miss;

            const bool enabled = owner->IsEnabled();
            if ( ownerLifetime.get() != owner ||
                 !binding.IsCurrent() ||
                 wxGetTopLevelParent(owner) != tlw )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }
            if ( !enabled )
                return wxMSWOleDropResolveResult::Miss;

            POINT client = screen;
            ::SetLastError(ERROR_SUCCESS);
            const int mapped = ::MapWindowPoints(
                HWND_DESKTOP, ownerHwnd, &client, 1);
            if ( mapped == 0 && ::GetLastError() != ERROR_SUCCESS )
                return wxMSWOleDropResolveResult::Unstable;

            candidate.target = binding.GetTargetLease();
            candidate.ownerIdentity =
                reinterpret_cast<std::uintptr_t>(owner);
            candidate.ownerGeneration = binding.GetGeneration();
            candidate.clientPoint = client;
            if ( !candidate.target.IsCurrent() || !binding.IsCurrent() )
                return wxMSWOleDropResolveResult::Unstable;

            found = true;
            break;
        }

        if ( !found )
            return wxMSWOleDropResolveResult::Miss;

        if ( mutationBefore != wxMSWOleGetDropTargetMutationGeneration() )
            return wxMSWOleDropResolveResult::Unstable;

        // Prove the geometric seed has not changed while virtual
        // GetDropTarget() and HWND mapping were allowed to re-enter.
        if ( surface ==
                wxWinUITopLevelHost::DropSurfaceResolution::Native )
        {
            wxWinUINativeHit refreshed;
            const HWND inner = host->m_inner;
            if ( !wxWinUIRefreshNativeHit(
                    tlw, nativeHit,
                    reinterpret_cast<WXHWND>(m_bridge),
                    reinterpret_cast<WXHWND>(inner),
                    &refreshed) )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }
        }
        else
        {
            wxWindow* refreshedSeed = nullptr;
            if ( host->ResolveDropSurface(screen, &refreshedSeed) !=
                    wxWinUITopLevelHost::DropSurfaceResolution::Slot ||
                 refreshedSeed != seed )
            {
                return wxMSWOleDropResolveResult::Unstable;
            }
        }

        if ( lifetime->GetHost() != host || host->m_shuttingDown ||
             !candidate.target.IsCurrent() )
        {
            return wxMSWOleDropResolveResult::Unstable;
        }

        *route = candidate;
        return wxMSWOleDropResolveResult::Hit;
    }

    bool Refresh(const wxMSWOleDropRoute& expected,
                 const POINTL& screenPoint,
                 wxMSWOleDropRoute* refreshed) override
    {
        wxMSWOleDropRoute current;
        if ( Resolve(screenPoint, &current) !=
                wxMSWOleDropResolveResult::Hit ||
             !current.IsSameLogicalTarget(expected) )
        {
            return false;
        }

        if ( refreshed )
            *refreshed = current;
        return refreshed != nullptr;
    }

    void DragEnter(const wxMSWOleDropRoute*,
                   IDataObject* data,
                   const POINTL& screenPoint,
                   DWORD effect) noexcept override
    {
        if ( !m_helper || m_physicalActive || !IsActive() ||
             !IsContextCurrent() )
            return;

        const POINT point = { screenPoint.x, screenPoint.y };
        const HWND pointHwnd = ::WindowFromPoint(point);
        HWND feedbackHwnd = nullptr;
        if ( pointHwnd == m_bridge ||
             (pointHwnd && ::IsChild(m_bridge, pointHwnd)) )
        {
            feedbackHwnd = m_bridge;
        }
        else if ( pointHwnd == m_registrationHwnd ||
                  (pointHwnd &&
                   ::IsChild(m_registrationHwnd, pointHwnd)) )
        {
            feedbackHwnd = m_registrationHwnd;
        }
        if ( !feedbackHwnd )
            return;

        wxWinUIDropTargetHelper* const helper = m_helper;
        helper->AddRef();
        const std::uint64_t epoch = m_feedbackEpoch;
        POINT helperPoint = point;
        const HRESULT hr =
            helper->DragEnter(feedbackHwnd, data, &helperPoint, effect);

        const bool stillCurrent =
            IsActive() && m_helper == helper && m_feedbackEpoch == epoch;
        if ( SUCCEEDED(hr) && stillCurrent )
        {
            m_physicalActive = true;
        }
        else if ( SUCCEEDED(hr) )
        {
            // The helper accepted DragEnter but a nested shutdown/terminal
            // invalidated this feedback epoch before it returned.
            helper->DragLeave();
        }
        helper->Release();
    }

    void DragOver(const wxMSWOleDropRoute*,
                  const POINTL& screenPoint,
                  DWORD effect) noexcept override
    {
        if ( !m_helper || !m_physicalActive )
            return;

        wxWinUIDropTargetHelper* const helper = m_helper;
        helper->AddRef();
        const std::uint64_t epoch = m_feedbackEpoch;
        POINT point = { screenPoint.x, screenPoint.y };
        helper->DragOver(&point, effect);
        // No state is published after the external call. A nested terminal or
        // shutdown owns the new epoch and has already cleared physicalActive.
        wxUnusedVar(epoch);
        helper->Release();
    }

    void DragLeave() noexcept override
    {
        ++m_feedbackEpoch;
        wxWinUIDropTargetHelper* const helper =
            m_helper && m_physicalActive ? m_helper : nullptr;
        m_physicalActive = false;
        if ( helper )
        {
            helper->AddRef();
            helper->DragLeave();
            helper->Release();
        }
    }

    void Drop(const wxMSWOleDropRoute*,
              IDataObject* data,
              const POINTL& screenPoint,
              DWORD effect) noexcept override
    {
        ++m_feedbackEpoch;
        wxWinUIDropTargetHelper* const helper =
            m_helper && m_physicalActive ? m_helper : nullptr;
        m_physicalActive = false;
        if ( helper )
        {
            helper->AddRef();
            POINT point = { screenPoint.x, screenPoint.y };
            helper->Drop(data, &point, effect);
            helper->Release();
        }
    }

private:
    template <typename F>
    HRESULT InvokeEffect(DWORD* effect, F&& fn) noexcept
    {
        if ( !effect )
            return E_INVALIDARG;
        if ( !IsActive() )
        {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        if ( !IsOnUIThread() )
        {
            *effect = DROPEFFECT_NONE;
            return RPC_E_WRONG_THREAD;
        }

#if wxUSE_EXCEPTIONS
        try
        {
#endif
            return fn();
#if wxUSE_EXCEPTIONS
        }
        catch ( ... )
        {
            *effect = DROPEFFECT_NONE;
            m_session.Reset();
            return E_UNEXPECTED;
        }
#endif
    }

    std::weak_ptr<wxWinUIHostLifetime> m_lifetime;
    wxWeakRef<wxWindow> m_tlw;
    HWND m_registrationHwnd = nullptr;
    unsigned long long m_registrationGeneration = 0;
    HWND m_bridge = nullptr;
    unsigned long long m_bridgeGeneration = 0;
    DWORD m_uiThread = 0;
    std::atomic<bool> m_active{ false };

    mutable std::mutex m_snapshotMutex;
    wxWinUIDropBrokerSnapshot m_snapshot;

    wxWinUIDropTargetHelper* m_helper = nullptr;
    bool m_physicalActive = false;
    std::uint64_t m_feedbackEpoch = 0;

    // Declared last so it is destroyed first, while resolver state is alive.
    // The helper itself is released explicitly by DeactivateOnUIThread().
    wxMSWOleDropSession m_session;
};

class wxWinUIDropBrokerCOMTarget final : public IDropTarget
{
public:
    explicit wxWinUIDropBrokerCOMTarget(
        const std::shared_ptr<wxWinUIDropBrokerState>& state)
        : m_state(state)
    {
    }

    STDMETHODIMP QueryInterface(REFIID iid, void** object) override
    {
        if ( !object )
            return E_POINTER;
        *object = nullptr;

        if ( iid == IID_IUnknown || iid == IID_IDropTarget )
        {
            *object = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return static_cast<ULONG>(::InterlockedIncrement(&m_refs));
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        const LONG refs = ::InterlockedDecrement(&m_refs);
        if ( refs == 0 )
            delete this;
        return static_cast<ULONG>(refs);
    }

    STDMETHODIMP DragEnter(IDataObject* data,
                           DWORD keyState,
                           POINTL point,
                           DWORD* effect) override
    {
        if ( !data || !effect )
        {
            if ( effect )
                *effect = DROPEFFECT_NONE;
            return E_INVALIDARG;
        }

        return InvokePinned([&](wxWinUIDropBrokerState& state)
        {
            return state.COMDragEnter(data, keyState, point, effect);
        });
    }

    STDMETHODIMP DragOver(DWORD keyState,
                          POINTL point,
                          DWORD* effect) override
    {
        if ( !effect )
            return E_INVALIDARG;
        return InvokePinned([&](wxWinUIDropBrokerState& state)
        {
            return state.COMDragOver(keyState, point, effect);
        });
    }

    STDMETHODIMP DragLeave() override
    {
        return InvokePinned([](wxWinUIDropBrokerState& state)
        {
            return state.COMDragLeave();
        });
    }

    STDMETHODIMP Drop(IDataObject* data,
                      DWORD keyState,
                      POINTL point,
                      DWORD* effect) override
    {
        if ( !data || !effect )
        {
            if ( effect )
                *effect = DROPEFFECT_NONE;
            return E_INVALIDARG;
        }

        return InvokePinned([&](wxWinUIDropBrokerState& state)
        {
            return state.COMDrop(data, keyState, point, effect);
        });
    }

private:
    template <typename F>
    HRESULT InvokePinned(F&& fn) noexcept
    {
        // User callbacks can synchronously destroy the TLW, which releases
        // the host-owned reference. Pin both the COM object and State until
        // this entry point has completely unwound.
        AddRef();
        const std::shared_ptr<wxWinUIDropBrokerState> state = m_state;
        const HRESULT hr = state ? fn(*state) : E_UNEXPECTED;
        Release();
        return hr;
    }

    ~wxWinUIDropBrokerCOMTarget() = default;

    volatile LONG m_refs = 1;
    std::shared_ptr<wxWinUIDropBrokerState> m_state;
};

std::unique_ptr<wxWinUIDropBroker>
wxWinUIDropBroker::Create(
    wxWindow* tlw,
    HWND bridge,
    const std::weak_ptr<wxWinUIHostLifetime>& lifetime,
    const wxWinUIDropBrokerNativeOps* testOps) noexcept
{
#if wxUSE_EXCEPTIONS
    try
    {
#endif
        if ( !tlw )
            return nullptr;

        const HWND tlwHwnd = GetHwndOf(tlw);
        if ( !tlwHwnd || !bridge || bridge == tlwHwnd )
            return nullptr;

        // Dual ownership requires two distinct exact ledger keys. Reject an
        // untracked or aliased HWND before allocating the two COM adapters;
        // otherwise a duplicate-key second reservation would look like a
        // partial transaction instead of an invalid host topology.
        if ( wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(bridge)) == 0 ||
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(tlwHwnd)) == 0 )
        {
            return nullptr;
        }

        const auto state =
            std::make_shared<wxWinUIDropBrokerState>(tlw, bridge, lifetime);
        auto* bridgeTarget = new wxWinUIDropBrokerCOMTarget(state);
        struct TargetPairGuard
        {
            wxWinUIDropBrokerCOMTarget* bridge;
            wxWinUIDropBrokerCOMTarget* tlw;
            ~TargetPairGuard()
            {
                if ( tlw )
                    tlw->Release();
                if ( bridge )
                    bridge->Release();
            }
        } targetGuard{ bridgeTarget, nullptr };
        auto* tlwTarget = new wxWinUIDropBrokerCOMTarget(state);
        targetGuard.tlw = tlwTarget;
        const wxWinUIDropBrokerNativeOps ops =
            testOps ? *testOps : GetNativeOps();
        if ( !ops.lock || !ops.registerTarget || !ops.revokeTarget )
            return nullptr;

        std::unique_ptr<wxWinUIDropBroker> broker(
            new wxWinUIDropBroker(state, bridgeTarget, tlwTarget, ops));
        targetGuard.bridge = nullptr;
        targetGuard.tlw = nullptr;
        broker->m_testBoundary = testOps != nullptr;
        return broker;
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        return nullptr;
    }
#endif
}

wxWinUIDropBroker::wxWinUIDropBroker(
    const std::shared_ptr<wxWinUIDropBrokerState>& state,
    wxWinUIDropBrokerCOMTarget* bridgeTarget,
    wxWinUIDropBrokerCOMTarget* tlwTarget,
    const wxWinUIDropBrokerNativeOps& nativeOps) noexcept
    : m_state(state),
      m_bridgeTarget(bridgeTarget),
      m_tlwTarget(tlwTarget),
      m_nativeOps(nativeOps)
{
}

wxWinUIDropBroker::~wxWinUIDropBroker()
{
    Shutdown();
    if ( m_tlwTarget )
    {
        m_tlwTarget->Release();
        m_tlwTarget = nullptr;
    }
    if ( m_bridgeTarget )
    {
        m_bridgeTarget->Release();
        m_bridgeTarget = nullptr;
    }
    m_state.reset();
}

HWND wxWinUIDropBroker::GetRegistrationHwnd(
    RegistrationRole role) const noexcept
{
    return role == RegistrationRole::Bridge
        ? m_state->GetBridge()
        : m_state->GetRegistrationHwnd();
}

std::uint64_t wxWinUIDropBroker::GetRegistrationGeneration(
    RegistrationRole role) const noexcept
{
    return role == RegistrationRole::Bridge
        ? m_state->GetBridgeGeneration()
        : m_state->GetRegistrationGeneration();
}

bool wxWinUIDropBroker::IsRegistrationCurrent(
    RegistrationRole role) const noexcept
{
    return role == RegistrationRole::Bridge
        ? m_state->IsBridgeCurrent()
        : m_state->IsRegistrationCurrent();
}

wxWinUIDropBroker::ShellRegistration&
wxWinUIDropBroker::GetRegistration(RegistrationRole role) noexcept
{
    return role == RegistrationRole::Bridge
        ? m_bridgeRegistration
        : m_tlwRegistration;
}

const wxWinUIDropBroker::ShellRegistration&
wxWinUIDropBroker::GetRegistration(RegistrationRole role) const noexcept
{
    return role == RegistrationRole::Bridge
        ? m_bridgeRegistration
        : m_tlwRegistration;
}

IDropTarget* wxWinUIDropBroker::GetRegistrationTarget(
    RegistrationRole role) const noexcept
{
    return role == RegistrationRole::Bridge
        ? static_cast<IDropTarget*>(m_bridgeTarget)
        : static_cast<IDropTarget*>(m_tlwTarget);
}

bool wxWinUIDropBroker::OwnsAnyRegistration() const noexcept
{
    return m_bridgeRegistration.ownsRegistration ||
           m_tlwRegistration.ownsRegistration;
}

bool wxWinUIDropBroker::OwnsCompleteRegistrationSet() const noexcept
{
    return m_bridgeRegistration.ownsRegistration &&
           m_tlwRegistration.ownsRegistration;
}

void wxWinUIDropBroker::RefreshAggregateRegistrationSnapshot() noexcept
{
    if ( !m_state )
        return;

    const bool bridgeLocked = m_bridgeRegistration.locked;
    const bool tlwLocked = m_tlwRegistration.locked;
    const bool bridgeOwned = m_bridgeRegistration.ownsRegistration;
    const bool tlwOwned = m_tlwRegistration.ownsRegistration;
    const bool bridgeCurrent = m_state->IsBridgeCurrent();
    const bool tlwCurrent = m_state->IsRegistrationCurrent();
    m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
    {
        s.lockCalls = s.bridgeRegistration.lockCalls +
                      s.tlwRegistration.lockCalls;
        s.registerCalls = s.bridgeRegistration.registerCalls +
                          s.tlwRegistration.registerCalls;
        s.revokeCalls = s.bridgeRegistration.revokeCalls +
                        s.tlwRegistration.revokeCalls;
        s.unlockCalls = s.bridgeRegistration.unlockCalls +
                        s.tlwRegistration.unlockCalls;
        s.bridgeRegistration.locked = bridgeLocked;
        s.tlwRegistration.locked = tlwLocked;
        s.bridgeRegistration.ownsRegistration = bridgeOwned;
        s.tlwRegistration.ownsRegistration = tlwOwned;
        s.bridgeRegistration.current = bridgeCurrent;
        s.tlwRegistration.current = tlwCurrent;
        s.locked = bridgeLocked || tlwLocked;
        s.ownsRegistration = bridgeOwned || tlwOwned;
        s.ownsCompleteRegistrationSet = bridgeOwned && tlwOwned;
    });
}

bool wxWinUIDropBroker::UpdateShellDropCoverage() noexcept
{
    if ( !m_state )
        return false;

    const bool shouldCover = m_adoptingShellDropTargets ||
                             m_acquiring || m_initializePending ||
                             OwnsAnyRegistration();
    if ( shouldCover == (m_shellDropCoverageToken != 0) )
        return true;

    const HWND hwnd = m_state->GetRegistrationHwnd();
    const std::uint64_t generation =
        m_state->GetRegistrationGeneration();
    if ( !hwnd || generation == 0 )
        return false;

    if ( shouldCover )
    {
        m_shellDropCoverageToken =
            wxWinUITLWHostAcquireOleDropRegistrationCoverage(
                reinterpret_cast<WXHWND>(hwnd), generation);
        return m_shellDropCoverageToken != 0;
    }

    wxWinUITLWHostReleaseOleDropRegistrationCoverage(
        reinterpret_cast<WXHWND>(hwnd), generation,
        m_shellDropCoverageToken);
    m_shellDropCoverageToken = 0;
    return true;
}

HRESULT wxWinUIDropBroker::ReleaseExternalLock(
    RegistrationRole role) noexcept
{
    ShellRegistration& registration = GetRegistration(role);
    if ( !registration.locked )
        return S_FALSE;

    // Claim in the process-global ledger before crossing the callback-bearing
    // COM boundary. Re-entrant cleanup and stale tokens can then never issue a
    // second FALSE for the same successful TRUE.
    if ( !wxMSWOleClaimExternalShellUnlock(&registration.operation) )
    {
        wxLogError("wxWinUI: external OLE lock cleanup had no exact "
                   "one-shot ledger authority; retaining the target "
                   "fail-closed");
        return E_UNEXPECTED;
    }

    const HRESULT unlockHr = m_nativeOps.lock(
        m_nativeOps.context, GetRegistrationTarget(role), FALSE, TRUE);
    if ( SUCCEEDED(unlockHr) )
        registration.locked = false;
    else
        wxLogError("wxWinUI: CoLockObjectExternal unlock failed "
                   "(HRESULT 0x%08lx); retaining the exact inactive target",
                   static_cast<unsigned long>(unlockHr));

    wxMSWOleCompleteExternalShellUnlock(
        &registration.operation, SUCCEEDED(unlockHr));
    m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
    {
        wxWinUIDropBrokerRegistrationSnapshot& registrationSnapshot =
            role == RegistrationRole::Bridge
                ? s.bridgeRegistration
                : s.tlwRegistration;
        ++registrationSnapshot.unlockCalls;
        registrationSnapshot.unlockHr = unlockHr;
        registrationSnapshot.locked = registration.locked;
        s.unlockHr = unlockHr;
    });
    RefreshAggregateRegistrationSnapshot();
    return unlockHr;
}

wxWinUIDropBrokerInitResult wxWinUIDropBroker::PublishInitResult(
    const wxWinUIDropBrokerInitResult& value) noexcept
{
    m_initResult = value;
    if ( m_state )
    {
        m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& snapshot)
        {
            snapshot.status = value.status;
            snapshot.resultHr = value.hresult;
        });
    }
    return value;
}

wxWinUIDropBrokerInitResult wxWinUIDropBroker::Initialize() noexcept
{
    return InitializeNative(false);
}

wxWinUIDropBrokerInitResult wxWinUIDropBroker::InitializeNative(
    bool deferActivation) noexcept
{
    wxWinUIDropBrokerInitResult result;
    if ( !m_state || !m_bridgeTarget || !m_tlwTarget )
    {
        result.status = wxWinUIDropBrokerInitStatus::InvalidContext;
        result.hresult = E_POINTER;
        return result;
    }

    if ( m_shutdown )
    {
        result.status = wxWinUIDropBrokerInitStatus::InvalidContext;
        result.hresult = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
        return result;
    }

    if ( m_initializeAttempted )
        return m_initResult;

    const auto contextIsCurrent = [&]() noexcept
    {
        return !m_shutdown && m_state && m_bridgeTarget && m_tlwTarget &&
               m_state->IsOnUIThread() &&
               (m_testBoundary || m_state->IsContextCurrent());
    };

    const auto finishTerminalAcquisition = [&](
        wxWinUIDropBrokerInitStatus status, HRESULT hr)
    {
        m_acquiring = false;
        UpdateShellDropCoverage();
        // Shutdown() may already have published the terminal latch from a
        // re-entrant native callback. Calling it again now performs the exact
        // rollback using the success flags recorded by this outer frame.
        Shutdown();
        result.status = status;
        result.hresult = hr;
        return PublishInitResult(result);
    };

    const RegistrationRole roles[] =
    {
        RegistrationRole::Bridge,
        RegistrationRole::TLW
    };

    const auto cancelPreparatoryReservations = [&]() noexcept
    {
        for ( const RegistrationRole role : roles )
        {
            ShellRegistration& registration = GetRegistration(role);
            if ( registration.operation.IsPendingFixed() ||
                 registration.operation.IsActive() )
            {
                wxMSWOleCancelExternalShellOperation(
                    &registration.operation);
            }
        }
    };

    if ( !m_state->IsOnUIThread() )
    {
        result.status = wxWinUIDropBrokerInitStatus::WrongThread;
        result.hresult = RPC_E_WRONG_THREAD;
        return PublishInitResult(result);
    }

    if ( !m_testBoundary && !m_state->IsContextCurrent() )
    {
        result.status = wxWinUIDropBrokerInitStatus::InvalidContext;
        result.hresult = E_HANDLE;
        return PublishInitResult(result);
    }

    m_acquiring = true;
    if ( !UpdateShellDropCoverage() )
    {
        m_acquiring = false;
        result.status = wxWinUIDropBrokerInitStatus::RegisterFailed;
        result.hresult = E_OUTOFMEMORY;
        return PublishInitResult(result);
    }

    for ( const RegistrationRole role : roles )
    {
        ShellRegistration& registration = GetRegistration(role);
        if ( !registration.identity.hwnd )
        {
            registration.identity = wxMSWOleEnsureShellHwndIdentity(
                reinterpret_cast<WXHWND>(GetRegistrationHwnd(role)));
        }
        if ( !registration.identity.hwnd ||
             registration.identity.generation !=
                 GetRegistrationGeneration(role) ||
             registration.identity.threadId != ::GetCurrentThreadId() )
        {
            cancelPreparatoryReservations();
            m_acquiring = false;
            UpdateShellDropCoverage();
            result.status = wxWinUIDropBrokerInitStatus::InvalidContext;
            result.hresult = E_HANDLE;
            return PublishInitResult(result);
        }
    }

    enum class ReservationResult
    {
        Active,
        Pending,
        Rejected,
        Invalid
    };

    const auto reserve = [&](RegistrationRole role) noexcept
    {
        ShellRegistration& registration = GetRegistration(role);

        if ( !registration.operation.IsPendingFixed() &&
             !registration.operation.IsActive() )
        {
            registration.operation = wxMSWOleBeginExternalShellOperation(
                registration.identity,
                wxMSWOleShellDropTargetOperationKind::Register,
                GetRegistrationTarget(role));
        }

        if ( registration.operation.IsPendingFixed() )
        {
            if ( !wxMSWOleTryPromoteExternalShellOperation(
                    &registration.operation) )
            {
                if ( !wxMSWOleArmExternalShellContinuation(
                        &registration.operation,
                        reinterpret_cast<WXHWND>(m_state->GetBridge())) &&
                     // The fixed owner can complete between promotion and
                     // continuation arming. Claim the newly free reservation
                     // once before declaring the exact continuation invalid.
                     !wxMSWOleTryPromoteExternalShellOperation(
                         &registration.operation) )
                {
                    const bool stillPending =
                        registration.operation.IsPendingFixed();
                    wxMSWOleCancelExternalShellOperation(
                        &registration.operation);
                    return stillPending
                        ? ReservationResult::Invalid
                        : ReservationResult::Rejected;
                }
            }

            if ( registration.operation.IsPendingFixed() )
                return ReservationResult::Pending;
        }

        return registration.operation.IsActive()
            ? ReservationResult::Active
            : ReservationResult::Rejected;
    };

    // Reserve both exact HWND/generation keys before crossing either native
    // lock/register boundary. If the second key is pending or occupied, the
    // first reservation is withdrawn, so no half-transaction can survive.
    for ( const RegistrationRole role : roles )
    {
        const ReservationResult reservationResult = reserve(role);
        if ( reservationResult == ReservationResult::Active )
            continue;

        if ( reservationResult == ReservationResult::Pending )
        {
            ShellRegistration& pending = GetRegistration(role);
            for ( const RegistrationRole otherRole : roles )
            {
                ShellRegistration& other = GetRegistration(otherRole);
                if ( &other != &pending &&
                     (other.operation.IsPendingFixed() ||
                      other.operation.IsActive()) )
                {
                    wxMSWOleCancelExternalShellOperation(&other.operation);
                }
            }
            m_initializePending = true;
            m_acquiring = false;
            UpdateShellDropCoverage();
            result.status =
                wxWinUIDropBrokerInitStatus::PendingRegistration;
            result.hresult = E_PENDING;
            return PublishInitResult(result);
        }

        cancelPreparatoryReservations();
        m_initializeAttempted =
            reservationResult == ReservationResult::Rejected;
        m_acquiring = false;
        UpdateShellDropCoverage();
        result.status =
            reservationResult == ReservationResult::Rejected
                ? wxWinUIDropBrokerInitStatus::AlreadyRegistered
                : wxWinUIDropBrokerInitStatus::RegisterFailed;
        result.hresult =
            reservationResult == ReservationResult::Rejected
                ? DRAGDROP_E_ALREADYREGISTERED
                : E_HANDLE;
        return PublishInitResult(result);
    }

    m_initializePending = false;
    m_initializeAttempted = true;

    for ( const RegistrationRole role : roles )
    {
        ShellRegistration& registration = GetRegistration(role);
        const HRESULT lockHr = m_nativeOps.lock(
            m_nativeOps.context, GetRegistrationTarget(role),
            TRUE, FALSE);
        m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
        {
            wxWinUIDropBrokerRegistrationSnapshot& registrationSnapshot =
                role == RegistrationRole::Bridge
                    ? s.bridgeRegistration
                    : s.tlwRegistration;
            ++registrationSnapshot.lockCalls;
            registrationSnapshot.lockHr = lockHr;
            s.lockHr = lockHr;
        });
        if ( SUCCEEDED(lockHr) )
        {
            registration.locked = true;
            wxMSWOleNoteExternalShellLock(&registration.operation);
        }
        RefreshAggregateRegistrationSnapshot();

        if ( !contextIsCurrent() )
        {
            return finishTerminalAcquisition(
                wxWinUIDropBrokerInitStatus::InvalidContext,
                m_shutdown
                    ? HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
                    : E_HANDLE);
        }

        if ( FAILED(lockHr) )
        {
            if ( OwnsAnyRegistration() )
            {
                return finishTerminalAcquisition(
                    wxWinUIDropBrokerInitStatus::LockFailed, lockHr);
            }

            cancelPreparatoryReservations();
            m_acquiring = false;
            UpdateShellDropCoverage();
            result.status = wxWinUIDropBrokerInitStatus::LockFailed;
            result.hresult = lockHr;
            return PublishInitResult(result);
        }

        if ( !wxMSWOleArmExternalShellOperation(
                &registration.operation) )
        {
            return finishTerminalAcquisition(
                wxWinUIDropBrokerInitStatus::InvalidContext, E_HANDLE);
        }

        const HWND registrationHwnd = GetRegistrationHwnd(role);
        const HRESULT registerHr = m_nativeOps.registerTarget(
            m_nativeOps.context, registrationHwnd,
            GetRegistrationTarget(role));
        m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
        {
            wxWinUIDropBrokerRegistrationSnapshot& registrationSnapshot =
                role == RegistrationRole::Bridge
                    ? s.bridgeRegistration
                    : s.tlwRegistration;
            ++registrationSnapshot.registerCalls;
            registrationSnapshot.registerHr = registerHr;
            s.registerHr = registerHr;
        });

        const bool ledgerCommitted =
            wxMSWOleCompleteExternalShellOperation(
                &registration.operation, registerHr == S_OK);
        if ( registerHr == S_OK && ledgerCommitted )
            registration.ownsRegistration = true;
        RefreshAggregateRegistrationSnapshot();

        if ( registerHr == S_OK &&
             registration.operation.WasRetireRequested() )
        {
            return finishTerminalAcquisition(
                wxWinUIDropBrokerInitStatus::InvalidContext, E_HANDLE);
        }

        if ( !contextIsCurrent() )
        {
            return finishTerminalAcquisition(
                wxWinUIDropBrokerInitStatus::InvalidContext,
                m_shutdown
                    ? HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
                    : E_HANDLE);
        }

        if ( registerHr == S_OK && !ledgerCommitted )
        {
            return finishTerminalAcquisition(
                wxWinUIDropBrokerInitStatus::InvalidContext,
                E_UNEXPECTED);
        }

        if ( registerHr != S_OK )
        {
            // Balance this slot's TRUE immediately.  It is the final external
            // lock when the first registration failed, but not when the TLW
            // registration failed after the bridge was acquired.
            (void)ReleaseExternalLock(role);
            const wxWinUIDropBrokerInitStatus failureStatus =
                registerHr == DRAGDROP_E_ALREADYREGISTERED
                    ? wxWinUIDropBrokerInitStatus::AlreadyRegistered
                    : wxWinUIDropBrokerInitStatus::RegisterFailed;

            if ( OwnsAnyRegistration() || !contextIsCurrent() )
            {
                return finishTerminalAcquisition(
                    !contextIsCurrent()
                        ? wxWinUIDropBrokerInitStatus::InvalidContext
                        : failureStatus,
                    !contextIsCurrent()
                        ? (m_shutdown
                            ? HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
                            : E_HANDLE)
                        : registerHr);
            }

            cancelPreparatoryReservations();
            m_acquiring = false;
            UpdateShellDropCoverage();
            result.status = failureStatus;
            result.hresult = registerHr;
            return PublishInitResult(result);
        }
    }

    m_state->PrepareHelper();
    RefreshAggregateRegistrationSnapshot();
    if ( !contextIsCurrent() || !OwnsCompleteRegistrationSet() )
    {
        return finishTerminalAcquisition(
            wxWinUIDropBrokerInitStatus::InvalidContext,
            m_shutdown
                ? HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
                : E_HANDLE);
    }

    result.status = wxWinUIDropBrokerInitStatus::Ready;
    result.hresult = S_OK;
    if ( deferActivation )
        return result;

    // No callback-bearing boundary remains between this exact two-key audit
    // and activation. Both native registrations must still denote their
    // captured HWND generations and be owned by their exact adapter
    // identities, both routing through this one logical broker State.
    if ( !contextIsCurrent() || !OwnsCompleteRegistrationSet() )
    {
        return finishTerminalAcquisition(
            wxWinUIDropBrokerInitStatus::InvalidContext, E_HANDLE);
    }
    m_state->Activate();
    m_acquiring = false;
    UpdateShellDropCoverage();
    return PublishInitResult(result);
}

void wxWinUIDropBroker::Shutdown() noexcept
{
    if ( !m_state || !m_bridgeTarget || !m_tlwTarget )
        return;

    if ( !m_shutdown )
    {
        m_shutdown = true;
        m_adoptingShellDropTargets = false;
        UpdateShellDropCoverage();

        // Publish the terminal state before any re-entrant cleanup boundary.
        // Late COM calls fail closed even if native revocation subsequently
        // fails. Acquisition rollback itself is deferred to the outer frame,
        // which alone knows whether each native call eventually succeeded.
        m_state->FailClosed();
        m_state->MutateSnapshot([](wxWinUIDropBrokerSnapshot& s)
        {
            s.shutdown = true;
        });
    }

    if ( m_acquiring || m_shutdownCleanupStarted )
        return;

    if ( !m_state->IsOnUIThread() )
    {
        wxLogError("wxWinUI: OLE drop broker shutdown requested off its UI "
                   "thread; native registration is retained fail-closed");
        return;
    }

    // Every callback boundary below can re-enter Shutdown(). Publish the
    // cleanup owner before crossing the first one so there is exactly one
    // revoke/unlock/rollback sequence.
    m_shutdownCleanupStarted = true;

    const RegistrationRole reverseRoles[] =
    {
        RegistrationRole::TLW,
        RegistrationRole::Bridge
    };

    // PendingFixed claims and preparatory Register reservations own no native
    // registration yet. Withdraw them in reverse transaction order. A TRUE
    // already observed by a partially executed acquisition is balanced under
    // its own exact slot before that reservation is cancelled.
    for ( const RegistrationRole role : reverseRoles )
    {
        ShellRegistration& registration = GetRegistration(role);
        if ( registration.operation.IsActive() && registration.locked )
        {
            (void)ReleaseExternalLock(role);
        }
        if ( registration.operation.IsPendingFixed() ||
             registration.operation.IsActive() )
        {
            wxMSWOleCancelExternalShellOperation(
                &registration.operation);
        }
    }
    m_initializePending = false;
    m_adoptingShellDropTargets = false;
    UpdateShellDropCoverage();

    wxWindow* const rollbackTLW = m_rollbackUncommittedAdoption
        ? m_state->GetTLW()
        : nullptr;
    const wxWeakRef<wxWindow> rollbackLifetime(rollbackTLW);

    m_state->DeactivateOnUIThread();

    // Revoke/unlock in the exact inverse of acquisition. Each HWND owns a
    // distinct COM adapter identity, ledger token, owner serial and external
    // TRUE/FALSE pair; both adapters share the logical State/Session.
    for ( const RegistrationRole role : reverseRoles )
    {
        ShellRegistration& registration = GetRegistration(role);
        const wxChar* const roleName =
            role == RegistrationRole::Bridge ? wxT("bridge") : wxT("TLW");

        // DeactivateOnUIThread() balances a live logical drag through
        // wxDropTarget::OnLeave(), which is arbitrary application code and
        // may destroy/pump/recreate windows. Never pass the old numeric HWND
        // to OLE unless it still denotes this slot's registered generation.
        const bool current = IsRegistrationCurrent(role);
        if ( registration.ownsRegistration && !current )
        {
            wxLogError("wxWinUI: %s was destroyed while balancing OLE "
                       "drop shutdown; retaining the inactive COM target "
                       "instead of revoking a stale HWND", roleName);
            m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
            {
                wxWinUIDropBrokerRegistrationSnapshot&
                    registrationSnapshot =
                        role == RegistrationRole::Bridge
                            ? s.bridgeRegistration
                            : s.tlwRegistration;
                registrationSnapshot.revokeHr = E_HANDLE;
                registrationSnapshot.ownsRegistration =
                    registration.ownsRegistration;
                registrationSnapshot.current = false;
                s.revokeHr = E_HANDLE;
            });
        }
        else if ( registration.ownsRegistration )
        {
            wxMSWOleExternalShellOperation revokeOperation =
                wxMSWOleBeginExternalShellOperation(
                    registration.identity,
                    wxMSWOleShellDropTargetOperationKind::Revoke,
                    GetRegistrationTarget(role));
            HRESULT revokeHr = E_HANDLE;
            bool ledgerCommitted = false;
            bool revokeCalled = false;
            if ( revokeOperation.IsActive() &&
                 wxMSWOleArmExternalShellOperation(&revokeOperation) )
            {
                revokeCalled = true;
                revokeHr = m_nativeOps.revokeTarget(
                    m_nativeOps.context, GetRegistrationHwnd(role));
                const bool revoked = SUCCEEDED(revokeHr) ||
                    revokeHr == DRAGDROP_E_NOTREGISTERED;
                ledgerCommitted = wxMSWOleCompleteExternalShellOperation(
                    &revokeOperation, revoked);
                registration.operation = std::move(revokeOperation);
                if ( revoked && ledgerCommitted )
                {
                    registration.ownsRegistration = false;
                    UpdateShellDropCoverage();
                }
            }

            if ( registration.ownsRegistration )
            {
                wxLogError(
                    "wxWinUI: RevokeDragDrop failed for the %s "
                    "(HRESULT 0x%08lx); retaining a fail-closed occupancy "
                    "tombstone",
                    roleName,
                    static_cast<unsigned long>(revokeHr));
            }

            m_state->MutateSnapshot([&](wxWinUIDropBrokerSnapshot& s)
            {
                wxWinUIDropBrokerRegistrationSnapshot&
                    registrationSnapshot =
                        role == RegistrationRole::Bridge
                            ? s.bridgeRegistration
                            : s.tlwRegistration;
                if ( revokeCalled )
                    ++registrationSnapshot.revokeCalls;
                registrationSnapshot.revokeHr = revokeHr;
                registrationSnapshot.ownsRegistration =
                    registration.ownsRegistration;
                s.revokeHr = revokeHr;
            });
        }

        RefreshAggregateRegistrationSnapshot();

        // This FALSE balances only this adapter's one TRUE even when native
        // Revoke fails. Each role has a distinct COM identity, so its own
        // unlock is necessarily the final external unlock for that identity.
        if ( registration.locked )
        {
            (void)ReleaseExternalLock(role);
        }

        if ( registration.ownsRegistration )
        {
            // RevokeDragDrop() and CoLockObjectExternal(FALSE) are both
            // callback-bearing boundaries. Re-read the exact HWND identity and
            // retire latch after them instead of trusting the pre-revoke
            // snapshot above.
            const bool currentAfterCleanup = IsRegistrationCurrent(role);
            const bool retiringOrStaleAfterCleanup =
                registration.operation.WasRetireRequested() ||
                !currentAfterCleanup;

            // The fake/native context and broker are about to disappear.
            // Preserve this exact physical occupancy without retaining either
            // context in the ledger. Retirement consumes a tombstone
            // immediately when WM_DESTROY already made the key stale.
            (void)wxMSWOleAbandonExternalShellOwner(
                &registration.operation);
            if ( retiringOrStaleAfterCleanup )
                wxMSWOleRetireShellHwnd(registration.identity);

            // Destruction/generation churn already removed the old HWND's OLE
            // registration.  The ledger may still retain a fail-closed COM
            // identity or failed external unlock for that stale key, but it is
            // no longer a routable registration and must not keep suppressing
            // logical drop targets on the still-live TLW.
            if ( retiringOrStaleAfterCleanup )
            {
                registration.ownsRegistration = false;
                UpdateShellDropCoverage();
            }
        }

        RefreshAggregateRegistrationSnapshot();
    }

    if ( m_rollbackUncommittedAdoption )
    {
        m_rollbackUncommittedAdoption = false;
        wxWindow* const currentTLW = rollbackLifetime.get();
        if ( !OwnsAnyRegistration() && currentTLW &&
             currentTLW == rollbackTLW && !currentTLW->IsBeingDeleted() )
        {
            ReconcileShellDropTargets(currentTLW);
        }
        else if ( OwnsAnyRegistration() && currentTLW &&
                  !currentTLW->IsBeingDeleted() )
        {
            wxLogError("wxWinUI: unable to restore shell OLE targets after "
                       "host initialization rollback because the TLW broker "
                       "registration could not be revoked");
        }
    }
}

bool wxWinUIDropBroker::IsReady() const noexcept
{
    return m_state &&
           m_state->IsOnUIThread() &&
           m_state->GetSnapshot().status ==
                wxWinUIDropBrokerInitStatus::Ready &&
           m_state->IsActive() &&
           OwnsCompleteRegistrationSet() &&
           m_state->IsContextCurrent();
}

bool wxWinUIDropBroker::CoversShellDropTargets() const noexcept
{
    return m_shellDropCoverageToken != 0;
}

wxWinUIDropBrokerInitResult
wxWinUIDropBroker::InitializeAndAdoptShellDropTargets() noexcept
{
    wxWinUIDropBrokerInitResult adoptionFailure;
    adoptionFailure.status = wxWinUIDropBrokerInitStatus::AdoptionFailed;
    adoptionFailure.hresult = E_FAIL;

    if ( !m_state || !m_bridgeTarget || !m_tlwTarget ||
         m_initializeAttempted || m_shutdown )
        return adoptionFailure;

    // Never traverse wxWindow state or revoke apartment-affine registrations
    // before the same context checks used by Initialize() have passed.
    if ( !m_state->IsOnUIThread() ||
         (!m_testBoundary && !m_state->IsContextCurrent()) )
    {
        return Initialize();
    }

    wxWindow* const tlw = m_state->GetTLW();
    if ( !tlw )
        return adoptionFailure;
    const wxWeakRef<wxWindow> tlwLifetime(tlw);

    std::vector<BoundTarget> targets;
    std::vector<BoundTarget> detached;
    m_adoptingShellDropTargets = true;
    if ( !UpdateShellDropCoverage() )
    {
        m_adoptingShellDropTargets = false;
        adoptionFailure.hresult = E_OUTOFMEMORY;
        return adoptionFailure;
    }
    bool adoptionCoverageActive = true;
    const auto stopAdoptionCoverage = [&]() noexcept
    {
        if ( adoptionCoverageActive )
        {
            m_adoptingShellDropTargets = false;
            UpdateShellDropCoverage();
            adoptionCoverageActive = false;
        }
    };
    const auto rollback = [&]() noexcept
    {
        // Make the provisional host uncovered before reconciliation so every
        // logical target suppressed or moved in by a re-entrant callback is
        // restored, not just the registrations present in the first snapshot.
        stopAdoptionCoverage();

        // A successful RegisterDragDrop() followed by a terminal callback can
        // reach this path after exact revocation failed (or after the TLW HWND
        // generation became unsafe to revoke). Never recreate per-window
        // shell registrations beside that retained, fail-closed TLW target.
        // Keeping the logical targets detached is the only truthful outcome
        // until the top-level itself is destroyed.
        if ( OwnsAnyRegistration() )
        {
            wxLogError("wxWinUI: unable to restore shell OLE targets after "
                       "broker acquisition rollback because an exact dual "
                       "registration is still retained fail-closed");
            return;
        }
#if wxUSE_EXCEPTIONS
        try
        {
#endif
            bool rollbackOk = true;
            for ( auto it = detached.rbegin(); it != detached.rend(); ++it )
            {
                if ( it->binding.IsCurrent() &&
                     !wxMSWOleIsShellDropTargetRegistered(
                         it->target, it->hwnd) &&
                     !it->target->Register(
                         reinterpret_cast<WXHWND>(it->hwnd)) )
                {
                    rollbackOk = false;
                }
            }
            if ( !rollbackOk )
            {
                wxLogError("wxWinUI: OLE drop-target migration rollback "
                           "was incomplete");
            }
#if wxUSE_EXCEPTIONS
        }
        catch ( ... )
        {
            wxLogError("wxWinUI: exception while restoring shell OLE "
                       "drop-target registrations");
        }
#endif

        wxWindow* const currentTLW = tlwLifetime.get();
        if ( currentTLW == tlw && !currentTLW->IsBeingDeleted() )
            ReconcileShellDropTargets(currentTLW);
    };

#if wxUSE_EXCEPTIONS
    try
    {
#endif
        if ( !CollectBoundTargets(tlw, &targets) ||
             tlwLifetime.get() != tlw || tlw->IsBeingDeleted() )
        {
            wxLogError("wxWinUI: OLE drop-target topology changed while "
                       "the TLW registration was being adopted");
            rollback();
            return adoptionFailure;
        }
        detached.reserve(targets.size());

        for ( const BoundTarget& item : targets )
        {
            wxWindow* const owner = item.binding.GetOwnerIfCurrent();
            if ( tlwLifetime.get() != tlw || !owner ||
                 !item.binding.IsCurrent() ||
                 wxGetTopLevelParent(owner) != tlw )
            {
                rollback();
                return adoptionFailure;
            }

            if ( !wxMSWOleIsShellDropTargetRegistered(
                    item.target, item.hwnd) )
            {
                continue;
            }

            item.target->Revoke(reinterpret_cast<WXHWND>(item.hwnd));
            if ( wxMSWOleIsShellDropTargetRegistered(
                    item.target, item.hwnd) )
            {
                wxLogError("wxWinUI: failed to revoke a shell OLE target "
                           "while adopting the TLW broker");
                rollback();
                return adoptionFailure;
            }
            detached.push_back(item);
        }

        if ( tlwLifetime.get() != tlw || tlw->IsBeingDeleted() )
        {
            rollback();
            return adoptionFailure;
        }

        // Acquire the two physical registrations only after every logical
        // shell target has been detached. In particular, a wxDropTarget set
        // directly on the TLW would otherwise make RegisterDragDrop() report
        // DRAGDROP_E_ALREADYREGISTERED for our own pre-existing target.
        // Keep acquisition and callback routing provisional until a final
        // stable audit proves that no public wxDropTarget::Register() call
        // raced our native RegisterDragDrop() boundary. Shell registration
        // changes advance the global mutation epoch, including manual calls.
        const wxWinUIDropBrokerInitResult init = InitializeNative(true);
        if ( !init.IsReady() )
        {
            if ( init.status ==
                    wxWinUIDropBrokerInitStatus::PendingRegistration )
            {
                // Keep both the exact global pending claim and provisional
                // TLW coverage alive. The host continuation will recollect
                // the current topology after the fixed operation unwinds.
                return init;
            }
            rollback();
            return init;
        }

        bool auditStable = false;
        constexpr unsigned MaxAuditPasses = 8;
        for ( unsigned pass = 0; pass < MaxAuditPasses; ++pass )
        {
            const std::uint64_t mutationBefore =
                wxMSWOleGetDropTargetMutationGeneration();
            std::vector<BoundTarget> auditTargets;
            if ( !CollectBoundTargets(tlw, &auditTargets) ||
                 tlwLifetime.get() != tlw || tlw->IsBeingDeleted() ||
                 m_shutdown || !OwnsCompleteRegistrationSet() ||
                 (!m_testBoundary && !m_state->IsContextCurrent()) )
            {
                break;
            }

            bool detachedDuringPass = false;
            bool passValid = true;
            for ( const BoundTarget& item : auditTargets )
            {
                wxWindow* const owner = item.binding.GetOwnerIfCurrent();
                if ( tlwLifetime.get() != tlw || !owner ||
                     !item.binding.IsCurrent() ||
                     wxGetTopLevelParent(owner) != tlw )
                {
                    passValid = false;
                    break;
                }

                if ( !wxMSWOleIsShellDropTargetRegistered(
                        item.target, item.hwnd) )
                {
                    continue;
                }

                item.target->Revoke(reinterpret_cast<WXHWND>(item.hwnd));
                if ( wxMSWOleIsShellDropTargetRegistered(
                        item.target, item.hwnd) )
                {
                    passValid = false;
                    break;
                }
                detachedDuringPass = true;
            }

            if ( !passValid || tlwLifetime.get() != tlw ||
                 tlw->IsBeingDeleted() || m_shutdown ||
                 !OwnsCompleteRegistrationSet() ||
                 (!m_testBoundary && !m_state->IsContextCurrent()) )
            {
                break;
            }

            const std::uint64_t mutationAfter =
                wxMSWOleGetDropTargetMutationGeneration();
            if ( !detachedDuringPass && mutationBefore == mutationAfter )
            {
                auditStable = true;
                break;
            }
        }

        if ( !auditStable )
        {
            wxLogError("wxWinUI: OLE drop-target topology did not become "
                       "quiescent before broker activation");
            m_acquiring = false;
            UpdateShellDropCoverage();
            Shutdown();
            rollback();
            return PublishInitResult(adoptionFailure);
        }

        // No callback-bearing boundary remains between this exact two-key
        // commit audit and activation. Generation churn on either the bridge
        // or TLW fails closed and rolls both registrations back.
        RefreshAggregateRegistrationSnapshot();
        if ( m_shutdown || !OwnsCompleteRegistrationSet() ||
             (!m_testBoundary && !m_state->IsContextCurrent()) )
        {
            m_acquiring = false;
            UpdateShellDropCoverage();
            Shutdown();
            rollback();
            adoptionFailure.hresult = E_HANDLE;
            return PublishInitResult(adoptionFailure);
        }

        // Only now may the COM target route callbacks.
        m_state->Activate();
        m_acquiring = false;
        UpdateShellDropCoverage();
        m_rollbackUncommittedAdoption = true;
        stopAdoptionCoverage();
        return PublishInitResult(init);
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        // This may be the only frame that knows a deferred native acquisition
        // completed. Release its lease before terminal cleanup so Shutdown()
        // can revoke/unlock exactly; only then may logical targets be restored.
        m_acquiring = false;
        UpdateShellDropCoverage();
        Shutdown();
        rollback();
        wxLogError("wxWinUI: exception while adopting shell OLE "
                   "drop-target registrations");
        return PublishInitResult(adoptionFailure);
    }
#endif
}

void wxWinUIDropBroker::CommitHostInitialization() noexcept
{
    m_rollbackUncommittedAdoption = false;
}

void wxWinUIDropBroker::ReconcileShellDropTargets(
    wxWindow* subtreeRoot) noexcept
{
    if ( !subtreeRoot )
        return;
    const wxWeakRef<wxWindow> subtreeLifetime(subtreeRoot);

#if wxUSE_EXCEPTIONS
    try
    {
#endif
    std::vector<BoundTarget> targets;
    if ( !CollectBoundTargets(subtreeRoot, &targets) ||
         subtreeLifetime.get() != subtreeRoot ||
         subtreeRoot->IsBeingDeleted() )
    {
        wxLogError("wxWinUI: OLE drop-target topology changed during "
                   "reparent reconciliation");
        return;
    }

    for ( const BoundTarget& item : targets )
    {
        if ( !item.binding.IsCurrent() )
            continue;

        wxWindow* const owner = item.binding.GetOwnerIfCurrent();
        wxWindow* const liveSubtree = subtreeLifetime.get();
        if ( !liveSubtree || !owner ||
             (owner != subtreeRoot && !subtreeRoot->IsDescendant(owner)) )
        {
            continue;
        }

        // The public host lookup deliberately hides a provisional host until
        // its complete initialization commits. OLE ownership becomes active
        // earlier, as soon as the provisional broker acquires the TLW, so a
        // reparent from an initialization callback must use the same direct
        // ownership seam as SetDropTarget() and avoid creating a concurrent
        // shell registration during that interval.
        const bool covered =
            wxWinUITLWHostOwnsOleDropRegistration(owner);
        const bool registered =
            wxMSWOleIsShellDropTargetRegistered(item.target, item.hwnd);

        if ( covered && registered )
        {
            item.target->Revoke(reinterpret_cast<WXHWND>(item.hwnd));
            if ( wxMSWOleIsShellDropTargetRegistered(
                    item.target, item.hwnd) )
            {
                wxLogError("wxWinUI: shell OLE drop target remains "
                           "registered after reparent into an island");
            }
        }
        else if ( !covered && !registered )
        {
            if ( !item.target->Register(
                    reinterpret_cast<WXHWND>(item.hwnd)) )
            {
                wxLogError("wxWinUI: failed to restore a shell OLE drop "
                           "target after reparent out of an island");
            }
        }
    }
#if wxUSE_EXCEPTIONS
    }
    catch ( ... )
    {
        wxLogError("wxWinUI: exception while reconciling shell OLE "
                   "drop-target registrations");
    }
#endif
}

wxWinUIDropBrokerSnapshot
wxWinUIDropBroker::GetSnapshotForTest() const noexcept
{
    return m_state ? m_state->GetSnapshot()
                   : wxWinUIDropBrokerSnapshot();
}

IDropTarget* wxWinUIDropBroker::GetCOMTargetForTest() const noexcept
{
    // Preserve the historical direct-routing seam. Either adapter reaches
    // the same State/Session; return the bridge adapter deterministically.
    return static_cast<IDropTarget*>(m_bridgeTarget);
}

#endif // wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP
