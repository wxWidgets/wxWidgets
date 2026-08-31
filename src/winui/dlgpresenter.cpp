/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dlgpresenter.cpp
// Purpose:     wxWinUIDialogPresenter: window/overlay presentation of the
//              WinUI-drawn common dialogs
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/private/windowlifetime.h"

#include "wx/msw/private.h"

#include "private.h"

#include "wx/winui/private/appearance.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <new>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.System.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXD = winrt::Microsoft::UI::Dispatching;

// ----------------------------------------------------------------------------
// the global presentation setting
// ----------------------------------------------------------------------------

namespace
{

// A real dialog window is the default. The in-window ContentDialog path
// remains explicitly opt-in with its distinct geometry and lifetime contract.
wxWinUIDialogPresentation gs_dialogPresentation =
    wxWinUIDialogPresentation::Window;
bool gs_dialogPresentationSet = false;

} // anonymous namespace

void wxWinUISetDialogPresentation(wxWinUIDialogPresentation presentation)
{
    gs_dialogPresentation = presentation;
    gs_dialogPresentationSet = true;
}

wxWinUIDialogPresentation wxWinUIGetDialogPresentation()
{
    // An explicit API choice always wins. Otherwise the environment switch is
    // a convenient way to qualify the optional Overlay path in an existing
    // application without changing its source.
    if ( !gs_dialogPresentationSet )
    {
        wxString overlay;
        if ( wxGetEnv("WX_WINUI_DIALOG_OVERLAY", &overlay) &&
                overlay == "1" )
            return wxWinUIDialogPresentation::Overlay;
    }

    return gs_dialogPresentation;
}

// ----------------------------------------------------------------------------
// per-TLW transient registry
// ----------------------------------------------------------------------------

namespace
{

bool gs_transientAllocationFailureArmed = false;
wxWinUITransientAllocationSite gs_transientAllocationFailureSite =
    wxWinUITransientAllocationSite::Any;

} // anonymous namespace

void wxWinUIFailTransientAllocationAtForTesting(
    wxWinUITransientAllocationSite site)
{
    gs_transientAllocationFailureSite = site;
    gs_transientAllocationFailureArmed = true;
}

void wxWinUIResetTransientAllocationFailureForTesting()
{
    gs_transientAllocationFailureArmed = false;
}

void wxWinUITransientAllocationPointForTesting(
    wxWinUITransientAllocationSite site)
{
    if ( !gs_transientAllocationFailureArmed ||
            (gs_transientAllocationFailureSite !=
                 wxWinUITransientAllocationSite::Any &&
             gs_transientAllocationFailureSite != site) )
    {
        return;
    }

    // Consume before throwing so exception cleanup can allocate/re-enter
    // without recursively failing at the same synthetic boundary.
    gs_transientAllocationFailureArmed = false;
    throw std::bad_alloc();
}

struct wxWinUITransientRegistration::Impl
{
    std::uintptr_t ownerKey = 0;
    std::uint64_t generation = 0;
    wxWinUITransientKind kind = wxWinUITransientKind::Popup;
    bool active = false;
};

struct wxWinUITransientOwnerRetirement::Impl
{
    std::uintptr_t ownerKey = 0;
    wxWeakRef<wxWindow> owner;
    bool active = false;
};

namespace
{

class wxWinUITransientManager final
{
private:
    struct ActiveTransient;
    struct OwnerTransients;
    class OwnerObserver;

public:
    static wxWinUITransientManager& Get()
    {
        static wxWinUITransientManager manager;
        return manager;
    }

    std::shared_ptr<wxWinUITransientRegistration::Impl>
    Register(wxWindow *owner,
             wxWinUITransientKind kind,
             std::function<void ()> cancel)
    {
        wxWindow * const tlw = owner ? wxGetTopLevelParent(owner) : nullptr;
        if ( !tlw || wxWinUITLWHostIsDestroyScheduled(tlw) )
            return {};

        const std::uintptr_t ownerKey =
            reinterpret_cast<std::uintptr_t>(tlw);

        // A recycled wxWindow address must never inherit the previous TLW's
        // modal lease.  The weak owner is authoritative even when the numeric
        // key happens to match.
        auto oldOwner = m_owners.find(ownerKey);
        if ( oldOwner != m_owners.end() )
        {
            if ( !oldOwner->second.owner )
            {
                // The deferred retirement of a destroyed owner may not have
                // run yet.  A new TLW reusing the same address must get its
                // own observer and modal generation, never the dead entry.
                // ownerDestroy=true also invalidates any unexpectedly
                // surviving registrations without deleting the observer
                // from an active destroy callback.
                const std::uint64_t staleEpoch = oldOwner->second.epoch;
                CancelKey(ownerKey, true, staleEpoch);
                oldOwner = m_owners.find(ownerKey);
                if ( oldOwner != m_owners.end() &&
                        oldOwner->second.epoch == staleEpoch &&
                        !oldOwner->second.owner )
                {
                    m_owners.erase(oldOwner);
                }
            }
        }
        oldOwner = m_owners.find(ownerKey);
        if ( oldOwner != m_owners.end() &&
                (oldOwner->second.owner.get() != tlw ||
                 oldOwner->second.shuttingDown ||
                 oldOwner->second.retirementDepth != 0) )
        {
            return {};
        }

        wxWinUITransientModalGate::Lease modalLease;
        std::uint64_t generation = 0;
        if ( kind == wxWinUITransientKind::ModalDialog )
        {
            modalLease = m_modalGate.TryAcquire(ownerKey);
            if ( !modalLease )
                return {};
            generation = modalLease.generation;
        }
        else
        {
            if ( ++m_nextGeneration == 0 )
                ++m_nextGeneration;
            generation = m_nextGeneration;
        }

        try
        {
            wxWinUITransientAllocationPointForTesting(
                wxWinUITransientAllocationSite::RegistrationImpl);
            auto impl =
                std::make_shared<wxWinUITransientRegistration::Impl>();
            impl->ownerKey = ownerKey;
            impl->generation = generation;
            impl->kind = kind;
            impl->active = true;

            ActiveTransient active;
            active.generation = generation;
            active.kind = kind;
            active.registration = impl;
            active.cancel = std::move(cancel);

            wxWinUITransientAllocationPointForTesting(
                wxWinUITransientAllocationSite::RegistrationOwnerEntry);
            auto& ownerEntry = m_owners[ownerKey];
            if ( !ownerEntry.observer )
            {
                ownerEntry.owner = wxWeakRef<wxWindow>(tlw);
                ownerEntry.epoch = NextOwnerEpoch();
                wxWinUITransientAllocationPointForTesting(
                    wxWinUITransientAllocationSite::RegistrationObserver);
                ownerEntry.observer =
                    std::make_unique<OwnerObserver>(
                        this, tlw, ownerKey, ownerEntry.epoch);
            }
            wxWinUITransientAllocationPointForTesting(
                wxWinUITransientAllocationSite::RegistrationEntry);
            ownerEntry.transients.push_back(std::move(active));
            return impl;
        }
        catch ( ... )
        {
            const auto ownerIt = m_owners.find(ownerKey);
            if ( ownerIt != m_owners.end() &&
                    ownerIt->second.transients.empty() )
            {
                m_owners.erase(ownerIt);
            }
            if ( modalLease )
                m_modalGate.Release(modalLease);
            throw;
        }
    }

    std::shared_ptr<wxWinUITransientOwnerRetirement::Impl>
    BeginOwnerRetirement(wxWindow *ownerOrTLW)
    {
        wxWindow * const tlw =
            ownerOrTLW ? wxGetTopLevelParent(ownerOrTLW) : nullptr;
        if ( !tlw || tlw->IsBeingDeleted() )
            return {};

        const std::uintptr_t ownerKey =
            reinterpret_cast<std::uintptr_t>(tlw);
        auto oldOwner = m_owners.find(ownerKey);
        if ( oldOwner != m_owners.end() )
        {
            if ( !oldOwner->second.owner )
            {
                const std::uint64_t staleEpoch = oldOwner->second.epoch;
                CancelKey(ownerKey, true, staleEpoch);
                oldOwner = m_owners.find(ownerKey);
                if ( oldOwner != m_owners.end() &&
                        oldOwner->second.epoch == staleEpoch &&
                        !oldOwner->second.owner )
                {
                    m_owners.erase(oldOwner);
                }
                oldOwner = m_owners.find(ownerKey);
            }
        }
        oldOwner = m_owners.find(ownerKey);
        if ( oldOwner != m_owners.end() &&
                (oldOwner->second.owner.get() != tlw ||
                 oldOwner->second.shuttingDown) )
        {
            return {};
        }

        try
        {
            auto impl =
                std::make_shared<wxWinUITransientOwnerRetirement::Impl>();
            impl->ownerKey = ownerKey;
            impl->owner = wxWeakRef<wxWindow>(tlw);
            impl->active = true;

            auto& ownerEntry = m_owners[ownerKey];
            if ( !ownerEntry.observer )
            {
                ownerEntry.owner = wxWeakRef<wxWindow>(tlw);
                ownerEntry.epoch = NextOwnerEpoch();
                ownerEntry.observer =
                    std::make_unique<OwnerObserver>(
                        this, tlw, ownerKey, ownerEntry.epoch);
            }
            ++ownerEntry.retirementDepth;
            return impl;
        }
        catch ( ... )
        {
            const auto ownerIt = m_owners.find(ownerKey);
            if ( ownerIt != m_owners.end() &&
                    ownerIt->second.transients.empty() &&
                    ownerIt->second.retirementDepth == 0 &&
                    !ownerIt->second.observer )
            {
                m_owners.erase(ownerIt);
            }
            throw;
        }
    }

    void EndOwnerRetirement(
        const std::shared_ptr<wxWinUITransientOwnerRetirement::Impl>& impl)
    {
        if ( !impl || !impl->active )
            return;

        const auto ownerIt = m_owners.find(impl->ownerKey);
        if ( ownerIt != m_owners.end() )
        {
            OwnerTransients& owner = ownerIt->second;
            wxWindow * const leaseOwner = impl->owner.get();
            if ( leaseOwner && owner.owner.get() == leaseOwner &&
                    owner.retirementDepth != 0 )
            {
                --owner.retirementDepth;
                if ( owner.retirementDepth == 0 &&
                        owner.transients.empty() &&
                        !owner.shuttingDown )
                {
                    m_owners.erase(ownerIt);
                }
            }
        }

        impl->active = false;
    }

    void Unregister(
        const std::shared_ptr<wxWinUITransientRegistration::Impl>& impl)
    {
        if ( !impl || !impl->active )
            return;

        const auto ownerIt = m_owners.find(impl->ownerKey);
        if ( ownerIt != m_owners.end() )
        {
            auto& transients = ownerIt->second.transients;
            const auto transientIt = std::find_if(
                transients.begin(),
                transients.end(),
                [impl](const ActiveTransient& active)
                {
                    return active.generation == impl->generation &&
                           active.kind == impl->kind;
                });

            if ( transientIt != transients.end() )
                transients.erase(transientIt);

            if ( transients.empty() && !ownerIt->second.shuttingDown &&
                    ownerIt->second.retirementDepth == 0 )
                m_owners.erase(ownerIt);
        }

        ReleaseModalLease(*impl);
        impl->active = false;
    }

    void Cancel(wxWindow *ownerOrTLW)
    {
        wxWindow * const tlw =
            ownerOrTLW ? wxGetTopLevelParent(ownerOrTLW) : nullptr;
        if ( !tlw )
            return;

        const std::uintptr_t ownerKey =
            reinterpret_cast<std::uintptr_t>(tlw);
        const auto ownerIt = m_owners.find(ownerKey);
        if ( ownerIt != m_owners.end() &&
                ownerIt->second.shuttingDown )
        {
            // The wxEVT_DESTROY observer is still executing. In particular,
            // a cancellation callback is allowed to request another central
            // cancellation, but it must never erase and delete that observer
            // out from under its own member function.
            return;
        }
        CancelKey(ownerKey, false);
    }

    void NotifyOwnerDestroyScheduled(wxWindow *ownerOrTLW)
    {
        wxWindow * const tlw =
            ownerOrTLW ? wxGetTopLevelParent(ownerOrTLW) : nullptr;
        if ( !tlw )
            return;

        const std::uintptr_t ownerKey =
            reinterpret_cast<std::uintptr_t>(tlw);
        auto ownerIt = m_owners.find(ownerKey);
        if ( ownerIt != m_owners.end() &&
                ownerIt->second.owner.get() != tlw )
        {
            // A deferred callback belonging to a destroyed TLW must not make
            // a newly allocated object at the same address inherit its gate.
            const std::uint64_t staleEpoch = ownerIt->second.epoch;
            CancelKey(ownerKey, true, staleEpoch);
            ownerIt = m_owners.find(ownerKey);
            if ( ownerIt != m_owners.end() &&
                    ownerIt->second.epoch == staleEpoch &&
                    ownerIt->second.owner.get() != tlw )
            {
                m_owners.erase(ownerIt);
            }
            ownerIt = m_owners.find(ownerKey);
        }

        if ( ownerIt == m_owners.end() )
        {
            auto inserted = m_owners.emplace(ownerKey, OwnerTransients());
            ownerIt = inserted.first;
            OwnerTransients& owner = ownerIt->second;
            owner.owner = wxWeakRef<wxWindow>(tlw);
            owner.epoch = NextOwnerEpoch();
            try
            {
                owner.observer = std::make_unique<OwnerObserver>(
                    this, tlw, ownerKey, owner.epoch);
            }
            catch ( ... )
            {
                m_owners.erase(ownerIt);
                throw;
            }
        }

        // Destroy() may be requested while a retained WinUI callback is still
        // on the stack, so physical wxEVT_DESTROY can be deferred. Treat the
        // request itself as the lifetime boundary, but keep OwnerObserver
        // alive until that later event has finished dispatching.
        CancelKey(ownerKey, true, ownerIt->second.epoch);
    }

    std::size_t Count(wxWindow *ownerOrTLW) const
    {
        const OwnerTransients * const owner = FindOwner(ownerOrTLW);
        return owner ? owner->transients.size() : 0;
    }

    std::size_t Count(wxWindow *ownerOrTLW,
                      wxWinUITransientKind kind) const
    {
        const OwnerTransients * const owner = FindOwner(ownerOrTLW);
        if ( !owner )
            return 0;

        return static_cast<std::size_t>(std::count_if(
            owner->transients.begin(),
            owner->transients.end(),
            [kind](const ActiveTransient& transient)
            {
                return transient.kind == kind;
            }));
    }

    bool IsOwnerShuttingDown(wxWindow *ownerOrTLW) const
    {
        const OwnerTransients * const owner = FindOwner(ownerOrTLW);
        return owner && owner->shuttingDown;
    }

    wxWinUITransientRegistrySnapshot GetSnapshot() const
    {
        wxWinUITransientRegistrySnapshot snapshot;
        snapshot.ownerCount = m_owners.size();
        snapshot.modalCount = m_modalGate.GetActiveCount();
        snapshot.ownerRetireEnqueueCount = m_ownerRetireEnqueueCount;
        snapshot.ownerRetireCallbackCount = m_ownerRetireCallbackCount;

        for ( const auto& ownerPair : m_owners )
        {
            const OwnerTransients& owner = ownerPair.second;
            if ( owner.shuttingDown )
                ++snapshot.pendingOwnerRetireCount;
            snapshot.activeOwnerRetirementCount += owner.retirementDepth;

            for ( const ActiveTransient& transient : owner.transients )
            {
                ++snapshot.transientCount;
                if ( transient.cancel )
                    ++snapshot.cancelCallbackCount;

                switch ( transient.kind )
                {
                    case wxWinUITransientKind::ModalDialog:
                        break;

                    case wxWinUITransientKind::Popup:
                        ++snapshot.popupCount;
                        break;

                    case wxWinUITransientKind::TeachingTip:
                        ++snapshot.teachingTipCount;
                        break;
                }
            }
        }

        return snapshot;
    }

    wxWinUITransientOwnerIdentityForTesting GetOwnerIdentityForTesting(
        std::uintptr_t ownerKey) const
    {
        const auto ownerIt = m_owners.find(ownerKey);
        if ( ownerIt == m_owners.end() )
            return {};

        return { ownerKey, ownerIt->second.epoch };
    }

    bool HasOwnerIdentityForTesting(
        const wxWinUITransientOwnerIdentityForTesting& identity) const
    {
        if ( !identity )
            return false;

        const auto ownerIt = m_owners.find(identity.ownerKey);
        return ownerIt != m_owners.end() &&
               ownerIt->second.epoch == identity.epoch;
    }

private:
    struct ActiveTransient
    {
        std::uint64_t generation = 0;
        wxWinUITransientKind kind = wxWinUITransientKind::Popup;
        std::weak_ptr<wxWinUITransientRegistration::Impl> registration;
        std::function<void ()> cancel;
    };

    struct OwnerTransients
    {
        wxWeakRef<wxWindow> owner;
        std::vector<ActiveTransient> transients;
        std::unique_ptr<OwnerObserver> observer;
        std::uint64_t epoch = 0;
        std::size_t retirementDepth = 0;
        bool shuttingDown = false;
    };

    class OwnerObserver final : public wxEvtHandler
    {
    public:
        OwnerObserver(wxWinUITransientManager *manager,
                      wxWindow *owner,
                      std::uintptr_t ownerKey,
                      std::uint64_t ownerEpoch)
            : m_manager(manager),
              m_owner(owner),
              m_ownerKey(ownerKey),
              m_ownerEpoch(ownerEpoch)
        {
            owner->Bind(
                wxEVT_DESTROY,
                &OwnerObserver::OnDestroy,
                this);
        }

        ~OwnerObserver() override
        {
            if ( wxWindow * const owner = m_owner.get() )
            {
                if ( !m_inDestroyDispatch )
                {
                    owner->Unbind(
                        wxEVT_DESTROY,
                        &OwnerObserver::OnDestroy,
                        this);
                }
            }
        }

    private:
        void OnDestroy(wxWindowDestroyEvent& event)
        {
            // wxEVT_DESTROY propagates.  The observer is bound to the TLW but
            // must not invalidate every transient merely because one of its
            // descendants is being destroyed.
            if ( reinterpret_cast<std::uintptr_t>(event.GetWindow()) !=
                    m_ownerKey )
            {
                event.Skip();
                return;
            }

            m_inDestroyDispatch = true;
            m_manager->CancelKey(m_ownerKey, true, m_ownerEpoch);
            event.Skip();
            m_inDestroyDispatch = false;
            m_manager->RetireDestroyedOwnerLater(m_ownerKey, m_ownerEpoch);
        }

        wxWinUITransientManager *m_manager;
        wxWeakRef<wxWindow> m_owner;
        std::uintptr_t m_ownerKey;
        std::uint64_t m_ownerEpoch;
        bool m_inDestroyDispatch = false;
    };

    void CancelKey(std::uintptr_t ownerKey,
                   bool ownerDestroy = false,
                   std::uint64_t expectedOwnerEpoch = 0)
    {
        const auto ownerIt = m_owners.find(ownerKey);
        if ( ownerIt == m_owners.end() )
            return;
        if ( expectedOwnerEpoch != 0 &&
                ownerIt->second.epoch != expectedOwnerEpoch )
            return;
        if ( ownerIt->second.shuttingDown && !ownerDestroy )
            return;

        // Retire the current generations before invoking arbitrary code. An
        // ordinary cancellation still allows a replacement generation, but
        // an active owner-retirement lease preserves this entry as a gate and
        // rejects every transient kind until the surrounding transaction is
        // complete.
        std::vector<ActiveTransient> cancelled =
            std::move(ownerIt->second.transients);
        if ( ownerDestroy )
        {
            // Keep the observer alive until wxEVT_DESTROY dispatch returns.
            // The now-empty entry is discarded if this address is ever
            // recycled for a later TLW.
            ownerIt->second.shuttingDown = true;
        }
        else if ( ownerIt->second.retirementDepth == 0 )
        {
            m_owners.erase(ownerIt);
        }

        // First invalidate every generation and release all modal leases.
        // This pass is allocation-free, so a low-memory condition cannot
        // publish shuttingDown while leaving a subset of registrations live.
        for ( auto& active : cancelled )
        {
            if ( active.kind == wxWinUITransientKind::ModalDialog )
            {
                m_modalGate.Release(
                    { ownerKey, active.generation });
            }

            if ( const auto registration = active.registration.lock() )
            {
                if ( registration->active &&
                        registration->generation == active.generation )
                {
                    registration->active = false;
                }
            }
        }

        // Only after the complete registry transaction is visible may
        // arbitrary user cancellation callbacks run.
        for ( const auto& active : cancelled )
        {
            if ( !active.cancel )
                continue;

            wxTRY
            {
                active.cancel();
            }
            wxCATCH_ALL(;)
        }
    }

    void RetireDestroyedOwnerLater(std::uintptr_t ownerKey,
                                   std::uint64_t ownerEpoch)
    {
        if ( !wxTheApp )
            return;

        // Erasing here would destroy OwnerObserver::this before OnDestroy()
        // returns. A posted wx callback runs only after the complete destroy
        // event dispatch, at which point unbinding/deleting the observer is
        // safe. The static manager outlives the application callback.
        ++m_ownerRetireEnqueueCount;
        wxTheApp->CallAfter(
            [this, ownerKey, ownerEpoch]()
            {
                ++m_ownerRetireCallbackCount;
                const auto ownerIt = m_owners.find(ownerKey);
                if ( ownerIt != m_owners.end() &&
                        ownerIt->second.epoch == ownerEpoch &&
                        ownerIt->second.shuttingDown &&
                        ownerIt->second.transients.empty() )
                {
                    m_owners.erase(ownerIt);
                }
            });
    }

    void ReleaseModalLease(
        const wxWinUITransientRegistration::Impl& impl)
    {
        if ( impl.kind == wxWinUITransientKind::ModalDialog )
        {
            m_modalGate.Release(
                { impl.ownerKey, impl.generation });
        }
    }

    std::uint64_t NextOwnerEpoch()
    {
        if ( ++m_nextOwnerEpoch == 0 )
            ++m_nextOwnerEpoch;
        return m_nextOwnerEpoch;
    }

    const OwnerTransients *FindOwner(wxWindow *ownerOrTLW) const
    {
        wxWindow * const tlw =
            ownerOrTLW ? wxGetTopLevelParent(ownerOrTLW) : nullptr;
        if ( !tlw )
            return nullptr;

        const auto it = m_owners.find(
            reinterpret_cast<std::uintptr_t>(tlw));
        return it != m_owners.end() ? &it->second : nullptr;
    }

    wxWinUITransientModalGate m_modalGate;
    std::unordered_map<std::uintptr_t, OwnerTransients> m_owners;
    std::uint64_t m_nextGeneration = 0;
    std::uint64_t m_nextOwnerEpoch = 0;
    std::uint64_t m_ownerRetireEnqueueCount = 0;
    std::uint64_t m_ownerRetireCallbackCount = 0;
};

} // anonymous namespace

wxWinUITransientRegistration::wxWinUITransientRegistration(
    const std::shared_ptr<Impl>& impl)
    : m_impl(impl)
{
}

wxWinUITransientRegistration::~wxWinUITransientRegistration()
{
    Reset();
}

wxWinUITransientRegistration::wxWinUITransientRegistration(
    wxWinUITransientRegistration&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
}

wxWinUITransientRegistration&
wxWinUITransientRegistration::operator=(
    wxWinUITransientRegistration&& other) noexcept
{
    if ( this != &other )
    {
        Reset();
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

wxWinUITransientRegistration::operator bool() const
{
    return m_impl && m_impl->active;
}

std::uint64_t wxWinUITransientRegistration::GetGeneration() const
{
    return m_impl ? m_impl->generation : 0;
}

void wxWinUITransientRegistration::Reset()
{
    if ( m_impl )
    {
        wxWinUITransientManager::Get().Unregister(m_impl);
        m_impl.reset();
    }
}

wxWinUITransientRegistration
wxWinUIRegisterTransient(wxWindow *owner,
                         wxWinUITransientKind kind,
                         std::function<void ()> cancel)
{
    return wxWinUITransientRegistration(
        wxWinUITransientManager::Get().Register(
            owner, kind, std::move(cancel)));
}

wxWinUITransientOwnerRetirement::wxWinUITransientOwnerRetirement(
    const std::shared_ptr<Impl>& impl)
    : m_impl(impl)
{
}

wxWinUITransientOwnerRetirement::~wxWinUITransientOwnerRetirement()
{
    Reset();
}

wxWinUITransientOwnerRetirement::wxWinUITransientOwnerRetirement(
    wxWinUITransientOwnerRetirement&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
}

wxWinUITransientOwnerRetirement&
wxWinUITransientOwnerRetirement::operator=(
    wxWinUITransientOwnerRetirement&& other) noexcept
{
    if ( this != &other )
    {
        Reset();
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

wxWinUITransientOwnerRetirement::operator bool() const
{
    return m_impl && m_impl->active;
}

void wxWinUITransientOwnerRetirement::Reset()
{
    if ( m_impl )
    {
        wxWinUITransientManager::Get().EndOwnerRetirement(m_impl);
        m_impl.reset();
    }
}

wxWinUITransientOwnerRetirement
wxWinUIBeginTransientOwnerRetirement(wxWindow *ownerOrTLW)
{
    return wxWinUITransientOwnerRetirement(
        wxWinUITransientManager::Get().BeginOwnerRetirement(ownerOrTLW));
}

void wxWinUICancelTransientSessions(wxWindow *ownerOrTLW)
{
    wxWinUITransientManager::Get().Cancel(ownerOrTLW);
}

void wxWinUINotifyTransientOwnerDestroyScheduled(wxWindow *ownerOrTLW)
{
    wxWinUITransientManager::Get().NotifyOwnerDestroyScheduled(ownerOrTLW);
}

std::size_t wxWinUITransientCountForTesting(wxWindow *ownerOrTLW)
{
    return wxWinUITransientManager::Get().Count(ownerOrTLW);
}

std::size_t
wxWinUITransientCountForTesting(wxWindow *ownerOrTLW,
                                wxWinUITransientKind kind)
{
    return wxWinUITransientManager::Get().Count(ownerOrTLW, kind);
}

wxWinUITransientRegistrySnapshot
wxWinUIGetTransientRegistrySnapshotForTesting()
{
    return wxWinUITransientManager::Get().GetSnapshot();
}

wxWinUITransientOwnerIdentityForTesting
wxWinUIGetTransientOwnerIdentityForTesting(std::uintptr_t ownerKey)
{
    return wxWinUITransientManager::Get().GetOwnerIdentityForTesting(ownerKey);
}

bool wxWinUIHasTransientOwnerIdentityForTesting(
    const wxWinUITransientOwnerIdentityForTesting& identity)
{
    return wxWinUITransientManager::Get().HasOwnerIdentityForTesting(identity);
}

// ----------------------------------------------------------------------------
// wxWinUIDialogShell: the top-level window used by the Window presentation
// ----------------------------------------------------------------------------

namespace
{

// Padding around the dialog body, in DIPs, matching the ContentDialog metrics.
constexpr int wxWINUI_DIALOG_MARGIN = 24;
constexpr int wxWINUI_BUTTON_HEIGHT = 32;
constexpr int wxWINUI_BUTTON_MIN_WIDTH = 100;
constexpr int wxWINUI_BUTTON_SPACING = 8;

class wxWinUIReentrancyGuard final
{
public:
    explicit wxWinUIReentrancyGuard(bool& flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~wxWinUIReentrancyGuard()
    {
        m_flag = false;
    }

private:
    bool& m_flag;
};

class wxWinUIDialogLifetimeObserver final : public wxEvtHandler
{
public:
    wxWinUIDialogLifetimeObserver() = default;

    ~wxWinUIDialogLifetimeObserver() override
    {
        if ( wxWindow * const owner = m_owner.get() )
        {
            if ( !m_inDestroyDispatch )
            {
                owner->Unbind(
                    wxEVT_DESTROY,
                    &wxWinUIDialogLifetimeObserver::OnDestroy,
                    this);
            }
        }
    }

    bool BindTo(wxWindow *owner, std::function<void ()> cancel)
    {
        if ( !owner || owner->IsBeingDeleted() )
            return false;

        m_owner = owner;
        m_identity = owner;
        m_cancel = std::move(cancel);
        owner->Bind(
            wxEVT_DESTROY,
            &wxWinUIDialogLifetimeObserver::OnDestroy,
            this);
        return true;
    }

private:
    void OnDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();
        if ( event.GetWindow() != m_identity || m_cancelled )
            return;

        m_inDestroyDispatch = true;
        m_cancelled = true;
        const auto cancel = std::move(m_cancel);
        if ( cancel )
            cancel();
        m_inDestroyDispatch = false;
    }

    wxWeakRef<wxWindow> m_owner;
    wxWindow *m_identity = nullptr;
    std::function<void ()> m_cancel;
    bool m_cancelled = false;
    bool m_inDestroyDispatch = false;
};

class wxWinUIDialogShell : public wxDialog
{
public:
    bool Create(wxWindow *parent, const wxString& title)
    {
        // wxRESIZE_BORDER is deliberately not used: the body is sized to its
        // content, exactly like a ContentDialog.
        if ( !wxDialog::Create(parent, wxID_ANY, title, wxDefaultPosition,
                               wxDefaultSize,
                               wxDEFAULT_DIALOG_STYLE & ~wxRESIZE_BORDER) )
            return false;

        Bind(wxEVT_CLOSE_WINDOW, &wxWinUIDialogShell::OnClose, this);
        return m_host.Initialize(this);
    }

    wxWinUIControlHost& GetHost() { return m_host; }
    void SetExternalDismissAllowed(bool allowed)
        { m_canDismissExternally = allowed; }

    int ShowModalWithoutHookWithExternalLifetime()
    {
        // The shell is parented to the public dialog's owner. That owner may
        // be destroyed from inside the nested loop and would normally delete
        // this private shell while WinUIShowModalWithoutHook() is still on its
        // stack. Use the same deferral contract as the public common-dialog
        // object, and reach the object from the exit guard only through a weak
        // reference.
        const wxWeakRef<wxDialog> weakSelf(this);
        WinUIBeginExternalModalLifetime();
        wxScopeGuard externalLifetime = wxMakeGuard(
            [weakSelf]()
            {
                if ( wxDialog * const live = weakSelf.get() )
                    live->WinUIEndExternalModalLifetime();
            });
        wxUnusedVar(externalLifetime);

        WinUIArmExternalModalLifetime();
        if ( IsBeingDeleted() )
            return wxID_CANCEL;

        return WinUIShowModalWithoutHook();
    }

private:
    void OnClose(wxCloseEvent& event)
    {
        if ( !m_canDismissExternally && event.CanVeto() )
        {
            event.Veto();
            return;
        }

        event.Skip();
    }

    wxWinUIControlHost m_host;
    bool m_canDismissExternally = true;
};

// Callback state for the Window presentation.  The XAML delegates and the
// deferred dismissal capture only a weak_ptr to this heap object.  In
// particular neither the stack-local presenter nor the dialog is ever
// captured raw by a retained delegate.
class wxWinUIDialogWindowCallbackState final
    : public std::enable_shared_from_this<
          wxWinUIDialogWindowCallbackState>
{
public:
    wxWinUIDialogWindowCallbackState(
        wxDialog *dialog,
        std::function<bool (int)> onAccept,
        bool canDismissExternally)
        : m_dialog(dialog),
          m_hwnd(dialog->GetHWND()),
          m_hwndGeneration(wxWinUIMSWGetHwndGeneration(dialog, m_hwnd)),
          m_onAccept(std::move(onAccept)),
          m_canDismissExternally(canDismissExternally)
    {
    }

    void RequestDismiss(int id)
    {
        if ( !IsCurrent(m_generation) || m_dismissQueued || m_acceptInProgress )
            return;
        if ( id == wxID_CANCEL &&
             wxWinUIShouldCancelExternalDialogDismiss(
                 m_canDismissExternally, false) )
        {
            return;
        }

        // The validation callback is caller-owned and can run arbitrary wx
        // code, including destroying the owner.  Revalidate the generation
        // and weak window identity after it returns.
        const std::uint64_t generation = m_generation;
        const auto accept = m_onAccept;
        bool accepted = true;
        if ( accept )
        {
            wxWinUIReentrancyGuard acceptGuard(m_acceptInProgress);
            accepted = accept(id);
        }

        if ( !accepted ||
                !IsCurrent(generation) ||
                m_dismissQueued ||
                !GetDialog() )
        {
            return;
        }

        m_dismissQueued = true;
        const std::weak_ptr<wxWinUIDialogWindowCallbackState> weakState =
            shared_from_this();

        if ( wxTheApp )
        {
            // Let the XAML Click stack unwind before ending the nested modal
            // loop.  The queued closure owns no wx object.
            wxTheApp->CallAfter(
                [weakState, generation, id]()
                {
                    if ( const auto state = weakState.lock() )
                        state->DismissNow(generation, id);
                });
        }
        else
        {
            // Application shutdown is already in progress.  EndModal() only
            // marks the loop for exit; no caller-owned object is released
            // until this callback returns.
            DismissNow(generation, id);
        }
    }

    void Invalidate()
    {
        if ( m_active )
        {
            m_active = false;
            ++m_generation;
        }
        m_onAccept = nullptr;
        m_dialog = nullptr;
    }

    void CancelFromLifetimeOwner()
    {
        if ( !m_active )
            return;

        m_active = false;
        ++m_generation;
        m_dismissQueued = true;
        m_onAccept = nullptr;

        wxDialog * const dialog = GetDialog();
        if ( !dialog )
            return;

        if ( dialog->IsModal() )
            dialog->EndModal(wxID_CANCEL);
        else
            dialog->Show(false);
    }

    bool IsActive() const
    {
        return m_active;
    }

private:
    wxDialog *GetDialog() const
    {
        wxWindow * const window = m_dialog.get();
        return window && window->GetHWND() == m_hwnd &&
               m_hwndGeneration &&
               wxWinUIMSWGetHwndGeneration(window, m_hwnd) == m_hwndGeneration
            ? static_cast<wxDialog *>(window)
            : nullptr;
    }

    bool IsCurrent(std::uint64_t generation) const
    {
        wxDialog * const dialog = GetDialog();
        return m_active && m_generation == generation && dialog &&
               !wxWindowIsUnavailableForCallbacks(dialog);
    }

    void DismissNow(std::uint64_t generation, int id)
    {
        if ( !IsCurrent(generation) )
            return;

        wxDialog * const dialog = GetDialog();
        if ( !dialog )
            return;

        if ( dialog->IsModal() )
            dialog->EndModal(id);
        else
            dialog->Show(false);
    }

    wxWeakRef<wxWindow> m_dialog;
    WXHWND m_hwnd = nullptr;
    unsigned long long m_hwndGeneration = 0;
    std::function<bool (int)> m_onAccept;
    std::uint64_t m_generation = 1;
    bool m_active = true;
    bool m_dismissQueued = false;
    bool m_acceptInProgress = false;
    bool m_canDismissExternally = true;
};

// Create a WinUI button for the dialog button bar.
MUXC::Button wxWinUIMakeDialogButton(const wxString& label, bool isDefault)
{
    MUXC::Button button;
    button.Content(winrt::box_value(wxWinUIToHString(
        wxWinUIRemoveMnemonics(label))));
    button.MinWidth(wxWINUI_BUTTON_MIN_WIDTH);
    button.Height(wxWINUI_BUTTON_HEIGHT);
    button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
    const wxString accessKey = wxWinUIExtractDialogAccessKey(label);
    if ( !accessKey.empty() )
        button.AccessKey(wxWinUIToHString(accessKey));

    if ( isDefault )
    {
        // The accent style is what a ContentDialog gives its default button.
        try
        {
            const auto resources = MUX::Application::Current().Resources();
            const auto key = winrt::box_value(winrt::hstring(L"AccentButtonStyle"));
            // Lookup traverses merged dictionaries; HasKey does not and can
            // therefore incorrectly reject the framework's accent resource.
            button.Style(resources.Lookup(key).as<MUX::Style>());
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return button;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWinUIDialogPresenter
// ----------------------------------------------------------------------------

wxWinUIDialogPresenter::wxWinUIDialogPresenter() = default;
wxWinUIDialogPresenter::~wxWinUIDialogPresenter() = default;

bool wxWinUIDialogPresenter::Create(wxWindow *parent, const wxString& title)
{
    if ( !wxWinUI3Initialize() )
        return false;

    m_parent = parent;
    m_title = title;
    m_window = nullptr;
    m_windowHwnd = nullptr;
    m_windowGeneration = 0;
    m_usesExistingWindow = false;
    return true;
}

bool wxWinUIDialogPresenter::CreateForDialog(wxDialog *dialog)
{
    if ( !dialog || dialog->IsBeingDeleted() || !dialog->GetHWND() )
        return false;

    const wxWeakRef<wxWindow> weakDialog(dialog);
    if ( !Create(dialog->GetParentForModalDialog(), dialog->GetTitle()) ||
         !wxWeakWindowIsAvailableForCallbacks(weakDialog, dialog) )
    {
        return false;
    }

    m_window = dialog;
    m_windowHwnd = dialog->GetHWND();
    m_windowGeneration = wxWinUIMSWGetHwndGeneration(dialog, m_windowHwnd);
    m_usesExistingWindow = true;
    SetLifetimeOwner(dialog);
    return GetCurrentWindow() != nullptr;
}

wxDialog *wxWinUIDialogPresenter::GetCurrentWindow() const
{
    wxWindow * const window = m_window.get();
    return wxWeakWindowIsAvailableForCallbacks(m_window, window) &&
           window->GetHWND() == m_windowHwnd && m_windowGeneration &&
           wxWinUIMSWGetHwndGeneration(window, m_windowHwnd) == m_windowGeneration
        ? static_cast<wxDialog *>(window)
        : nullptr;
}

wxSize wxWinUIDialogPresenter::GetWindowClientSize(const wxSize& contentSize,
                                                  size_t buttonCount)
{
    wxSize client = contentSize;
    client.y += wxWINUI_BUTTON_HEIGHT + wxWINUI_DIALOG_MARGIN;
    client.x = wxMax(client.x, static_cast<int>(buttonCount) *
                        (wxWINUI_BUTTON_MIN_WIDTH + wxWINUI_BUTTON_SPACING));
    client.x += 2 * wxWINUI_DIALOG_MARGIN;
    client.y += 2 * wxWINUI_DIALOG_MARGIN;
    return client;
}

void wxWinUIDialogPresenter::SetContent(MUX::UIElement const& content)
{
    m_content = content;
}

void wxWinUIDialogPresenter::AddButton(int id, const wxString& label,
                                       bool isDefault)
{
    Button button;
    button.id = id;
    button.label = label;
    button.isDefault = isDefault;
    m_buttons.push_back(button);
}

int wxWinUIDialogPresenter::ShowModal()
{
    if ( m_buttons.empty() )
        AddButton(wxID_OK, _("OK"), true);

    return wxWinUIGetDialogPresentation() ==
                wxWinUIDialogPresentation::Overlay &&
           m_buttons.size() <= 3
        ? ShowAsOverlay()
        : ShowAsWindow();
}

// ----------------------------------------------------------------------------
// Window presentation: a real top-level dialog hosting a single island
// ----------------------------------------------------------------------------

int wxWinUIDialogPresenter::ShowAsWindow()
{
    if ( !LifetimeOwnerIsAlive() )
        return wxID_CANCEL;

    wxWinUIControlHost borrowedHost;
    wxWinUIDialogShell * const ownedShell =
        m_usesExistingWindow ? nullptr : new wxWinUIDialogShell;
    wxDialog * const window = ownedShell ? ownedShell : GetCurrentWindow();
    if ( !window )
        return wxID_CANCEL;

    const wxWeakRef<wxWindow> weakWindow(window);
    const auto destroyOwnedShell = wxMakeGuard(
        [weakWindow, ownedShell]()
        {
            if ( ownedShell && weakWindow )
                weakWindow->Destroy();
        });

    wxWinUIControlHost * const host = ownedShell
        ? &ownedShell->GetHost() : &borrowedHost;
    if ( ownedShell )
    {
        if ( !ownedShell->Create(m_parent.get(), m_title) ||
             !wxWeakWindowIsAvailableForCallbacks(weakWindow, window) )
        {
            return wxID_CANCEL;
        }
        ownedShell->SetExternalDismissAllowed(m_canDismissExternally);
    }
    else if ( !borrowedHost.Initialize(window) || !GetCurrentWindow() )
    {
        return wxID_CANCEL;
    }

    const WXHWND hwnd = window->GetHWND();
    const auto hwndGeneration = wxWinUIMSWGetHwndGeneration(window, hwnd);
    const auto windowIsCurrent = [weakWindow, window, hwnd, hwndGeneration]()
    {
        return wxWeakWindowIsAvailableForCallbacks(weakWindow, window) &&
               window->GetHWND() == hwnd && hwndGeneration &&
               wxWinUIMSWGetHwndGeneration(window, hwnd) == hwndGeneration;
    };

    int result = wxID_CANCEL;
    MUXC::Border surface{ nullptr };
    MUXC::Grid root{ nullptr };
    auto callbackState =
        std::make_shared<wxWinUIDialogWindowCallbackState>(
            window, m_onAccept, m_canDismissExternally);
    wxWinUIDialogLifetimeObserver lifetimeObserver;
    wxWindow * const lifetimeOwner = m_lifetimeOwner.get();
    if ( m_hasLifetimeOwner &&
            (!lifetimeOwner || lifetimeOwner->IsBeingDeleted()) )
    {
        return wxID_CANCEL;
    }
    if ( lifetimeOwner )
    {
        const std::weak_ptr<wxWinUIDialogWindowCallbackState> weakState =
            callbackState;
        if ( !lifetimeObserver.BindTo(
                 lifetimeOwner,
                 [weakState]()
                 {
                     if ( const auto state = weakState.lock() )
                         state->CancelFromLifetimeOwner();
                 }) )
        {
            return wxID_CANCEL;
        }
    }

    struct ButtonHook
    {
        MUXC::Button button{ nullptr };
        winrt::event_token token{};
    };
    std::vector<ButtonHook> buttonHooks;
    buttonHooks.reserve(m_buttons.size());
    winrt::event_token rootKeyDownToken{};
    wxScopeGuard cleanup = wxMakeGuard(
        [&]()
        {
            // Invalidate before revocation: should WinUI deliver a queued
            // callback while the tree is detached, it can only observe an
            // inactive heap state.
            callbackState->Invalidate();
            for ( const ButtonHook& hook : buttonHooks )
            {
                try
                {
                    if ( hook.button && hook.token.value )
                        hook.button.Click(hook.token);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }
            try
            {
                if ( root && rootKeyDownToken.value )
                    root.KeyDown(rootKeyDownToken);
            }
            catch ( const winrt::hresult_error& )
            {
            }

            try
            {
                // A borrowed host is stack-owned even if its wx window died.
                // Close retires its slot and routes before returning to the
                // caller; the public dialog and its HWND are not destroyed.
                if ( !ownedShell || weakWindow )
                    host->Close();
            }
            catch ( const winrt::hresult_error& )
            {
            }
            try
            {
                if ( root )
                    root.Children().Clear();
            }
            catch ( const winrt::hresult_error& )
            {
            }
            try
            {
                if ( surface )
                    surface.Child(nullptr);
            }
            catch ( const winrt::hresult_error& )
            {
            }

            m_content = nullptr;
        });
    wxUnusedVar(cleanup);

    try
    {
        // Body above, right-aligned button bar below, both inset by the
        // standard dialog margin.
        surface = wxWinUICreateThemeBrushBorder(
            "ContentDialogBackground",
            wxWinUIThemeBrushProperty::Background);
        if ( !surface )
            return result;

        const MUX::ElementTheme theme = wxWinUIGetCurrentElementTheme();
        surface.RequestedTheme(theme);
        root = MUXC::Grid();
        root.RequestedTheme(theme);
        root.Padding(MUX::ThicknessHelper::FromUniformLength(
            wxWINUI_DIALOG_MARGIN));
        root.RowSpacing(wxWINUI_DIALOG_MARGIN);

        MUXC::RowDefinition bodyRow;
        bodyRow.Height(MUX::GridLengthHelper::FromValueAndType(
            1, MUX::GridUnitType::Star));
        MUXC::RowDefinition buttonRow;
        buttonRow.Height(MUX::GridLengthHelper::Auto());
        root.RowDefinitions().Append(bodyRow);
        root.RowDefinitions().Append(buttonRow);

        int defaultButtonId = m_buttons.front().id;
        for ( const Button& definition : m_buttons )
        {
            if ( definition.isDefault )
            {
                defaultButtonId = definition.id;
                break;
            }
        }

        const std::weak_ptr<wxWinUIDialogWindowCallbackState> weakState =
            callbackState;
        rootKeyDownToken = root.KeyDown(
            [weakState, defaultButtonId](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::Input::KeyRoutedEventArgs& args)
            {
                if ( args.Handled() )
                    return;

                wxWinUIDialogWindowKey key =
                    wxWinUIDialogWindowKey::Other;
                switch ( args.Key() )
                {
                    case winrt::Windows::System::VirtualKey::Enter:
                        key = wxWinUIDialogWindowKey::Enter;
                        break;

                    case winrt::Windows::System::VirtualKey::Escape:
                        key = wxWinUIDialogWindowKey::Escape;
                        break;

                    default:
                        break;
                }

                bool sourceAcceptsReturn = false;
                const auto source = args.OriginalSource();
                if ( const auto textBox = source.try_as<MUXC::TextBox>() )
                {
                    sourceAcceptsReturn = textBox.AcceptsReturn();
                }
                else if ( const auto richEdit =
                              source.try_as<MUXC::RichEditBox>() )
                {
                    sourceAcceptsReturn = richEdit.AcceptsReturn();
                }

                const int id = wxWinUIResolveDialogWindowKey(
                    key, sourceAcceptsReturn, defaultButtonId);
                if ( id == wxID_NONE )
                    return;

                if ( const auto state = weakState.lock() )
                {
                    state->RequestDismiss(id);
                    // A validation veto still consumes the dialog key.
                    args.Handled(true);
                }
            });

        if ( m_content )
        {
            MUXC::Grid::SetRow(m_content.as<MUX::FrameworkElement>(), 0);
            root.Children().Append(m_content);
        }

        MUXC::StackPanel buttons;
        buttons.Orientation(MUXC::Orientation::Horizontal);
        buttons.Spacing(wxWINUI_BUTTON_SPACING);
        buttons.HorizontalAlignment(MUX::HorizontalAlignment::Right);
        MUXC::Grid::SetRow(buttons, 1);

        for ( const Button& definition : m_buttons )
        {
            const int id = definition.id;
            MUXC::Button button =
                wxWinUIMakeDialogButton(definition.label, definition.isDefault);

            const std::weak_ptr<wxWinUIDialogWindowCallbackState>
                weakButtonState = callbackState;
            const winrt::event_token token = button.Click(
                [weakButtonState, id](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    if ( const auto state = weakButtonState.lock() )
                        state->RequestDismiss(id);
                });

            buttonHooks.push_back({ button, token });
            buttons.Children().Append(button);
        }

        root.Children().Append(buttons);
        surface.Child(root);
        if ( !windowIsCurrent() || !host->SetContent(surface) ||
             !windowIsCurrent() )
        {
            return result;
        }

        // Only a private shell needs initial sizing here. Public dialogs were
        // sized at Create(), and later application geometry is authoritative.
        if ( ownedShell )
        {
            window->SetClientSize(window->FromDIP(
                GetWindowClientSize(m_contentSize, m_buttons.size())));
            if ( !windowIsCurrent() )
                return result;
            window->CentreOnParent();
        }

        // SetContent(), sizing and centring can dispatch arbitrary wx/XAML
        // callbacks. A lifetime-owner cancellation before the modal loop
        // starts must abort here; showing an inert shell would hang forever.
        if ( LifetimeOwnerIsAlive() &&
                callbackState->IsActive() &&
                windowIsCurrent() )
        {
            result = ownedShell
                ? ownedShell->ShowModalWithoutHookWithExternalLifetime()
                : window->WinUIShowModalWithoutHook();
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI dialog window", e);
    }

    return result;
}

// ----------------------------------------------------------------------------
// Overlay presentation: a ContentDialog over the parent
// ----------------------------------------------------------------------------

namespace
{

class wxWinUIOverlayCallbackState final
    : public std::enable_shared_from_this<wxWinUIOverlayCallbackState>
{
public:
    wxWinUIOverlayCallbackState(
        wxWindow *tlw,
        const MUXD::DispatcherQueue& queue,
        std::vector<int> buttonIds,
        std::function<bool (int)> onAccept,
        bool canDismissExternally)
        : m_tlw(tlw),
          m_queue(queue),
          m_buttonIds(std::move(buttonIds)),
          m_onAccept(std::move(onAccept)),
          m_canDismissExternally(canDismissExternally)
    {
    }

    ~wxWinUIOverlayCallbackState()
    {
        Invalidate();
    }

    void Activate(std::uint64_t generation)
    {
        m_generation = generation;
    }

    bool PrepareLoop(bool dispatcherAccepted,
                     bool shutdownHookInstalled)
    {
        return m_loopState.Prepare(
            dispatcherAccepted, shutdownHookInstalled);
    }

    bool EnterLoop(wxEventLoop& loop)
    {
        if ( !m_loopState.Start() )
            return false;

        m_loop = &loop;
        return true;
    }

    void LeaveLoop()
    {
        m_loop = nullptr;
    }

    bool EnterLifetimeOwnerDestroyLoop(wxEventLoop& loop)
    {
        if ( !m_waitingForLifetimeOwnerDestroy || m_loop )
            return false;

        m_loop = &loop;
        return true;
    }

    bool IsWaitingForLifetimeOwnerDestroy() const
    {
        return m_waitingForLifetimeOwnerDestroy;
    }

    bool AcceptButton(std::size_t index, int id)
    {
        if ( !m_active || m_acceptInProgress )
            return false;

        const std::uint64_t generation = m_generation;
        const auto accept = m_onAccept;
        bool accepted = true;
        if ( accept )
        {
            wxWinUIReentrancyGuard acceptGuard(m_acceptInProgress);
            accepted = accept(id);
        }

        if ( !accepted || !IsCurrent(generation) )
            return false;

        m_resultState.AcceptButton(index);
        return true;
    }

    void Complete(
        const winrt::Windows::Foundation::IAsyncOperation<
            MUXC::ContentDialogResult>& operation,
        winrt::Windows::Foundation::AsyncStatus status)
    {
        if ( !m_active )
            return;

        if ( status ==
             winrt::Windows::Foundation::AsyncStatus::Completed )
        {
            try
            {
                const MUXC::ContentDialogResult result =
                    operation.GetResults();
                if ( m_loopState.Complete() )
                {
                    m_dialogResult = result;
                    m_completedNormally = true;
                }
            }
            catch ( const winrt::hresult_error& )
            {
                m_loopState.Abort();
            }
        }
        else
        {
            m_loopState.Abort();
        }

        ScheduleExitAfterFrameworkCallback();
    }

    void CancelFromRegistry()
    {
        if ( !m_active )
            return;

        // Owner destruction invalidates the registry generation before the
        // owner's wxEVT_DESTROY reaches its child dialogs. Keep processing
        // that same destroy transaction so the public source dialog can send
        // its own wxEVT_DESTROY (and hence the modal-hook Exit notification)
        // before the nested loop unwinds. Explicit central cancellation has
        // no such pending lifetime event and continues to exit immediately.
        wxWindow * const tlw = m_tlw.get();
        m_waitingForLifetimeOwnerDestroy =
            m_hasLifetimeOwner && tlw &&
            wxWinUITransientManager::Get().IsOwnerShuttingDown(tlw);

        m_loopState.Abort();
        m_active = false;
        ++m_generation;
        m_resultState.Reset();
        if ( !m_waitingForLifetimeOwnerDestroy )
            ExitLoop();
        m_onAccept = nullptr;
        m_tlw = nullptr;
        m_queue = nullptr;
    }

    void CancelFromLifetimeOwner()
    {
        if ( m_waitingForLifetimeOwnerDestroy )
        {
            m_waitingForLifetimeOwnerDestroy = false;
            ExitLoop();
            return;
        }

        CancelFromRegistry();
    }

    void CancelAllForTLW()
    {
        if ( wxWindow * const tlw = m_tlw.get() )
            wxWinUICancelTransientSessions(tlw);
        else
            CancelFromRegistry();
    }

    bool CompletedNormally() const
    {
        return m_completedNormally;
    }

    bool IsActive() const
    {
        return m_active;
    }

    bool ShouldCancelClosing() const
    {
        return wxWinUIShouldCancelExternalDialogDismiss(
            m_canDismissExternally,
            m_resultState.HasAcceptedButton());
    }

    int ResolveResult() const
    {
        if ( !m_completedNormally )
            return wxID_CANCEL;

        wxWinUIDialogCompletion completion =
            wxWinUIDialogCompletion::None;
        switch ( m_dialogResult )
        {
            case MUXC::ContentDialogResult::Primary:
                completion = wxWinUIDialogCompletion::Primary;
                break;

            case MUXC::ContentDialogResult::Secondary:
                completion = wxWinUIDialogCompletion::Secondary;
                break;

            case MUXC::ContentDialogResult::None:
                break;
        }

        return m_resultState.Resolve(
            completion,
            m_buttonIds.empty() ? nullptr : m_buttonIds.data(),
            m_buttonIds.size());
    }

    void Invalidate()
    {
        if ( m_active )
        {
            m_active = false;
            ++m_generation;
        }
        m_loopState.Invalidate();
        m_loop = nullptr;
        m_onAccept = nullptr;
        m_tlw = nullptr;
        m_queue = nullptr;
        m_waitingForLifetimeOwnerDestroy = false;
    }

    void SetHasLifetimeOwner(bool hasLifetimeOwner)
    {
        m_hasLifetimeOwner = hasLifetimeOwner;
    }

private:
    bool IsCurrent(std::uint64_t generation) const
    {
        return m_active && m_generation == generation;
    }

    void ScheduleExitAfterFrameworkCallback()
    {
        if ( !m_active || m_exitScheduled )
            return;

        m_exitScheduled = true;
        const std::uint64_t generation = m_generation;
        const std::weak_ptr<wxWinUIOverlayCallbackState> weakState =
            shared_from_this();

        bool accepted = false;
        try
        {
            accepted = m_queue &&
                m_queue.TryEnqueue(
                    [weakState, generation]()
                    {
                        if ( const auto state = weakState.lock() )
                        {
                            if ( state->IsCurrent(generation) )
                                state->ExitLoop();
                        }
                    });
        }
        catch ( const winrt::hresult_error& )
        {
        }

        // Rejection means shutdown has already made the queue unavailable.
        // Exit() merely marks the nested loop; it cannot destroy the shared
        // callback state while this framework callback is still executing.
        if ( !accepted )
            ExitLoop();
    }

    void ExitLoop()
    {
        if ( m_loop && m_loop->IsRunning() )
            m_loop->Exit();
    }

    wxWeakRef<wxWindow> m_tlw;
    MUXD::DispatcherQueue m_queue{ nullptr };
    std::vector<int> m_buttonIds;
    std::function<bool (int)> m_onAccept;
    wxWinUIDialogResultState m_resultState;
    wxWinUIDialogLoopState m_loopState;
    MUXC::ContentDialogResult m_dialogResult =
        MUXC::ContentDialogResult::None;
    wxEventLoop *m_loop = nullptr;
    std::uint64_t m_generation = 0;
    bool m_active = true;
    bool m_exitScheduled = false;
    bool m_completedNormally = false;
    bool m_acceptInProgress = false;
    bool m_canDismissExternally = true;
    bool m_hasLifetimeOwner = false;
    bool m_waitingForLifetimeOwnerDestroy = false;
};

} // anonymous namespace

int wxWinUIDialogPresenter::ShowAsOverlay()
{
    if ( !LifetimeOwnerIsAlive() )
        return wxID_CANCEL;

    // The dialog lives on the parent's shared per-TLW island: its smoke
    // layer covers the whole client area (wx-drawn HWNDs included, since the
    // island band composes above them), and the island hit-test treats an
    // open popup as capturing everything, so the modality is airtight.
    wxWinUITopLevelHost * const host =
        m_parent ? wxWinUITopLevelHost::ForWindow(m_parent.get(), true) : nullptr;
    if ( !LifetimeOwnerIsAlive() )
        return wxID_CANCEL;
    if ( !host || !host->GetXamlRoot() )
        return ShowAsWindow();   // no island: degrade to a real window

    wxWindow * const tlw =
        m_parent ? wxGetTopLevelParent(m_parent.get()) : nullptr;
    const MUXD::DispatcherQueue queue =
        host->Root().DispatcherQueue();
    if ( !tlw || !queue )
        return ShowAsWindow();

    const std::size_t count = std::min<std::size_t>(m_buttons.size(), 3);
    std::vector<int> buttonIds;
    buttonIds.reserve(count);
    for ( std::size_t i = 0; i < count; ++i )
        buttonIds.push_back(m_buttons[i].id);

    auto state = std::make_shared<wxWinUIOverlayCallbackState>(
        tlw, queue, std::move(buttonIds), m_onAccept,
        m_canDismissExternally);
    state->SetHasLifetimeOwner(m_hasLifetimeOwner);
    const std::weak_ptr<wxWinUIOverlayCallbackState> weakState = state;
    wxWinUIDialogLifetimeObserver lifetimeObserver;
    wxWindow * const lifetimeOwner = m_lifetimeOwner.get();
    if ( m_hasLifetimeOwner &&
            (!lifetimeOwner || lifetimeOwner->IsBeingDeleted()) )
        return wxID_CANCEL;
    if ( lifetimeOwner )
    {
        if ( !lifetimeObserver.BindTo(
                 lifetimeOwner,
                 [weakState]()
                 {
                     if ( const auto stateNow = weakState.lock() )
                         stateNow->CancelFromLifetimeOwner();
                 }) )
        {
            return wxID_CANCEL;
        }
    }
    auto registration = wxWinUIRegisterTransient(
        tlw,
        wxWinUITransientKind::ModalDialog,
        [weakState]()
        {
            if ( const auto stateNow = weakState.lock() )
                stateNow->CancelFromRegistry();
        });
    if ( !registration )
    {
        // Reentrant Overlay presentation on the same TLW is rejected rather
        // than entering two competing ContentDialog modal loops.
        return wxID_CANCEL;
    }
    state->Activate(registration.GetGeneration());

    using DialogOperation =
        winrt::Windows::Foundation::IAsyncOperation<
            MUXC::ContentDialogResult>;

    MUXC::ContentDialog dialog{ nullptr };
    DialogOperation operation{ nullptr };
    winrt::event_token primaryToken{};
    winrt::event_token secondaryToken{};
    winrt::event_token closeToken{};
    winrt::event_token closingToken{};
    winrt::event_token shutdownToken{};
    bool shutdownHookInstalled = false;
    bool dispatcherAccepted = false;
    int result = wxID_CANCEL;
    wxScopeGuard cleanup = wxMakeGuard(
        [&]()
        {
            // Central teardown order: invalidate every callback target,
            // unregister the generation, revoke framework callbacks, close
            // any pending operation, then detach caller-owned content.
            const bool completedNormally = state->CompletedNormally();
            state->Invalidate();
            registration.Reset();

            if ( operation )
            {
                try
                {
                    operation.Completed(
                        winrt::Windows::Foundation::
                            AsyncOperationCompletedHandler<
                                MUXC::ContentDialogResult>{ nullptr });
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }

            if ( dialog )
            {
                try
                {
                    if ( primaryToken.value )
                        dialog.PrimaryButtonClick(primaryToken);
                }
                catch ( const winrt::hresult_error& )
                {
                }
                try
                {
                    if ( secondaryToken.value )
                        dialog.SecondaryButtonClick(secondaryToken);
                }
                catch ( const winrt::hresult_error& )
                {
                }
                try
                {
                    if ( closeToken.value )
                        dialog.CloseButtonClick(closeToken);
                }
                catch ( const winrt::hresult_error& )
                {
                }
                try
                {
                    if ( closingToken.value )
                        dialog.Closing(closingToken);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }

            if ( shutdownHookInstalled )
            {
                try
                {
                    queue.ShutdownStarting(shutdownToken);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }

            if ( dialog )
            {
                if ( !completedNormally )
                {
                    try
                    {
                        dialog.Hide();
                    }
                    catch ( const winrt::hresult_error& )
                    {
                    }
                }
                try
                {
                    dialog.Content(nullptr);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }

            m_content = nullptr;
        });
    wxUnusedVar(cleanup);

    try
    {
        dialog = MUXC::ContentDialog();
        dialog.XamlRoot(host->GetXamlRoot());
        dialog.RequestedTheme(wxWinUIGetCurrentElementTheme());
        dialog.Title(winrt::box_value(wxWinUIToHString(m_title)));

        if ( m_content )
            dialog.Content(m_content);

        // ContentDialog exposes exactly three button slots, in this order.
        if ( count > 0 )
        {
            dialog.PrimaryButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[0].label)));
        }
        if ( count > 1 )
        {
            dialog.SecondaryButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[1].label)));
        }
        if ( count > 2 )
        {
            dialog.CloseButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[2].label)));
        }

        for ( size_t i = 0; i < count; ++i )
        {
            if ( !m_buttons[i].isDefault )
                continue;

            switch ( i )
            {
                case 0:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Primary);
                    break;
                case 1:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Secondary);
                    break;
                default:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Close);
                    break;
                }
        }

        if ( count > 0 )
        {
            const int id = m_buttons[0].id;
            primaryToken = dialog.PrimaryButtonClick(
                [weakState, id](
                    MUXC::ContentDialog const&,
                    MUXC::ContentDialogButtonClickEventArgs const& e)
                {
                    const auto stateNow = weakState.lock();
                    if ( !stateNow ||
                            !stateNow->AcceptButton(0, id) )
                    {
                        e.Cancel(true);
                    }
                });
        }
        if ( count > 1 )
        {
            const int id = m_buttons[1].id;
            secondaryToken = dialog.SecondaryButtonClick(
                [weakState, id](
                    MUXC::ContentDialog const&,
                    MUXC::ContentDialogButtonClickEventArgs const& e)
                {
                    const auto stateNow = weakState.lock();
                    if ( !stateNow ||
                            !stateNow->AcceptButton(1, id) )
                    {
                        e.Cancel(true);
                    }
                });
        }
        if ( count > 2 )
        {
            const int id = m_buttons[2].id;
            closeToken = dialog.CloseButtonClick(
                [weakState, id](
                    MUXC::ContentDialog const&,
                    MUXC::ContentDialogButtonClickEventArgs const& e)
                {
                    const auto stateNow = weakState.lock();
                    if ( !stateNow ||
                            !stateNow->AcceptButton(2, id) )
                    {
                        e.Cancel(true);
                    }
                });
        }

        closingToken = dialog.Closing(
            [weakState](
                MUXC::ContentDialog const&,
                MUXC::ContentDialogClosingEventArgs const& e)
            {
                const auto stateNow = weakState.lock();
                if ( !stateNow || stateNow->ShouldCancelClosing() )
                    e.Cancel(true);
            });

        shutdownToken = queue.ShutdownStarting(
            [weakState](
                const MUXD::DispatcherQueue&,
                const MUXD::DispatcherQueueShutdownStartingEventArgs&)
            {
                if ( const auto stateNow = weakState.lock() )
                    stateNow->CancelAllForTLW();
            });
        shutdownHookInstalled = true;

        // A rejected queue must never be followed by a nested event loop.
        dispatcherAccepted = queue.TryEnqueue([]() {});

        if ( state->PrepareLoop(
                 dispatcherAccepted, shutdownHookInstalled) )
        {
            // Behave app-modally: the other top-level windows are disabled
            // while the dialog is up.  The parent remains enabled because it
            // owns the island whose ContentDialog smoke layer captures input.
            wxWindowDisabler disabler(tlw);

            // Enabling/disabling TLWs dispatches application events. If they
            // destroyed the public source, the observer already aborted this
            // generation and no ContentDialog may be shown afterwards.
            if ( LifetimeOwnerIsAlive() &&
                    state->IsActive() )
            {
                operation = dialog.ShowAsync();
                operation.Completed(
                    [weakState](
                        const DialogOperation& async,
                        winrt::Windows::Foundation::AsyncStatus status)
                    {
                        if ( const auto stateNow = weakState.lock() )
                            stateNow->Complete(async, status);
                    });

                wxEventLoop loop;
                if ( state->EnterLoop(loop) )
                {
                    loop.Run();
                    state->LeaveLoop();
                }

                result = state->ResolveResult();
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI dialog overlay", e);
    }

    // Cancellation can occur while XAML setup or wxWindowDisabler is still
    // dispatching callbacks, before the ordinary nested loop has started.
    // In that case keep pumping only until the source lifetime observer sees
    // its real destroy event, preserving the same hook ordering as the normal
    // in-loop owner-cascade path.
    if ( state->IsWaitingForLifetimeOwnerDestroy() )
    {
        wxEventLoop lifetimeLoop;
        if ( state->EnterLifetimeOwnerDestroyLoop(lifetimeLoop) )
        {
            lifetimeLoop.Run();
            state->LeaveLoop();
        }
    }

    return result;
}

#endif // wxUSE_WINUI3
