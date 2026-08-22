/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/dialogsession.h
// Purpose:     Testable state core for WinUI transient dialog sessions
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_DIALOGSESSION_H_
#define _WX_WINUI_PRIVATE_DIALOGSESSION_H_

#include "wx/defs.h"
#include "wx/string.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>

#if wxUSE_WINUI3

class wxWindow;

enum class wxWinUITransientKind
{
    ModalDialog,
    Popup,
    TeachingTip
};

// Deterministic allocation boundaries used only by transient lifecycle tests.
// Any preserves the historical "fail the next popup allocation" seam, while
// named sites prove the rollback contract of each multi-allocation registry
// transaction without replacing the process allocator.
enum class wxWinUITransientAllocationSite
{
    Any,
    PopupCancelToken,
    RegistrationImpl,
    RegistrationOwnerEntry,
    RegistrationObserver,
    RegistrationEntry,
    PopupRegistrationHolder,
    PopupRetirementHolder
};

WXDLLIMPEXP_CORE void wxWinUIFailTransientAllocationAtForTesting(
    wxWinUITransientAllocationSite site);
WXDLLIMPEXP_CORE void wxWinUIResetTransientAllocationFailureForTesting();

// Production allocation sites call this immediately before the corresponding
// allocation. It is a no-op unless a test armed this exact site (or Any), and
// consumes the one-shot request before throwing std::bad_alloc.
WXDLLIMPEXP_CORE void wxWinUITransientAllocationPointForTesting(
    wxWinUITransientAllocationSite site);

class wxWinUITransientRegistration;
class wxWinUITransientOwnerRetirement;

WXDLLIMPEXP_CORE wxWinUITransientRegistration
wxWinUIRegisterTransient(wxWindow *owner,
                         wxWinUITransientKind kind,
                         std::function<void ()> cancel);

WXDLLIMPEXP_CORE wxWinUITransientOwnerRetirement
wxWinUIBeginTransientOwnerRetirement(wxWindow *ownerOrTLW);

enum class wxWinUIDialogWindowKey
{
    Other,
    Enter,
    Escape
};

// Projection-free decision used by the real Window presenter KeyDown handler.
// wxID_NONE means that XAML keeps routing the key.
inline int wxWinUIResolveDialogWindowKey(
    wxWinUIDialogWindowKey key,
    bool sourceAcceptsReturn,
    int defaultButtonId)
{
    switch ( key )
    {
        case wxWinUIDialogWindowKey::Enter:
            return sourceAcceptsReturn ? wxID_NONE : defaultButtonId;

        case wxWinUIDialogWindowKey::Escape:
            return wxID_CANCEL;

        case wxWinUIDialogWindowKey::Other:
            break;
    }

    return wxID_NONE;
}

inline wxString wxWinUIExtractDialogAccessKey(const wxString& label)
{
    for ( std::size_t i = 0; i + 1 < label.length(); ++i )
    {
        if ( label[i] != '&' )
            continue;

        if ( label[i + 1] == '&' )
        {
            ++i;
            continue;
        }

        return label.Mid(i + 1, 1);
    }

    return wxString();
}

// A transient registers an invalidation callback and keeps this move-only
// token for exactly as long as its framework object can call back.  Reset (or
// destruction) removes only the matching generation, so late cleanup cannot
// unregister a newer transient at a recycled address.
class WXDLLIMPEXP_CORE wxWinUITransientRegistration final
{
public:
    wxWinUITransientRegistration() = default;
    ~wxWinUITransientRegistration();

    wxWinUITransientRegistration(wxWinUITransientRegistration&&) noexcept;
    wxWinUITransientRegistration&
        operator=(wxWinUITransientRegistration&&) noexcept;

    wxWinUITransientRegistration(
        const wxWinUITransientRegistration&) = delete;
    wxWinUITransientRegistration&
        operator=(const wxWinUITransientRegistration&) = delete;

    explicit operator bool() const;
    std::uint64_t GetGeneration() const;
    void Reset();

    struct Impl;

private:
    explicit wxWinUITransientRegistration(
        const std::shared_ptr<Impl>& impl);

    std::shared_ptr<Impl> m_impl;

    friend WXDLLIMPEXP_CORE wxWinUITransientRegistration
    wxWinUIRegisterTransient(wxWindow *,
                             wxWinUITransientKind,
                             std::function<void ()>);
};

// Temporarily make an owner unavailable to every transient kind. The lease is
// acquired before cancelling its current generations and remains held across
// their arbitrary callbacks, so none of them can resurrect a Popup,
// TeachingTip or ModalDialog against a surface being hidden or destroyed.
class WXDLLIMPEXP_CORE wxWinUITransientOwnerRetirement final
{
public:
    wxWinUITransientOwnerRetirement() = default;
    ~wxWinUITransientOwnerRetirement();

    wxWinUITransientOwnerRetirement(
        wxWinUITransientOwnerRetirement&&) noexcept;
    wxWinUITransientOwnerRetirement& operator=(
        wxWinUITransientOwnerRetirement&&) noexcept;

    wxWinUITransientOwnerRetirement(
        const wxWinUITransientOwnerRetirement&) = delete;
    wxWinUITransientOwnerRetirement& operator=(
        const wxWinUITransientOwnerRetirement&) = delete;

    explicit operator bool() const;
    void Reset();

    struct Impl;

private:
    explicit wxWinUITransientOwnerRetirement(
        const std::shared_ptr<Impl>& impl);

    std::shared_ptr<Impl> m_impl;

    friend WXDLLIMPEXP_CORE wxWinUITransientOwnerRetirement
    wxWinUIBeginTransientOwnerRetirement(wxWindow *);
};

// Register against the owner's top-level window.  A second ModalDialog for
// the same TLW is rejected; Popup and TeachingTip entries may coexist.  The
// cancellation callback must itself use weak/invalidatable state.

// Invalidate every current transient belonging to this TLW.  Entries are
// removed before callbacks run, which makes reentrant registration safe.
WXDLLIMPEXP_CORE void
wxWinUICancelTransientSessions(wxWindow *ownerOrTLW);

// Mark the owning TLW as logically destroyed before its physical wxWindow
// teardown can run. All current generations are invalidated immediately and
// new registrations are rejected, while the destroy observer is deliberately
// retained until wxEVT_DESTROY completes. This operation is idempotent.
WXDLLIMPEXP_CORE void
wxWinUINotifyTransientOwnerDestroyScheduled(wxWindow *ownerOrTLW);

// Minimal diagnostic seam used by lifecycle tests (and by future TeachingTip
// integration tests).  It observes the production registry without exposing
// its entries or cancellation callbacks.
WXDLLIMPEXP_CORE std::size_t
wxWinUITransientCountForTesting(wxWindow *ownerOrTLW);
WXDLLIMPEXP_CORE std::size_t
wxWinUITransientCountForTesting(wxWindow *ownerOrTLW,
                                 wxWinUITransientKind kind);

// Generation-bearing identity of one exact owner-registry entry. Lifecycle
// tests capture it synchronously after Destroy() has published owner
// retirement, then query it without ever dereferencing the retired wxWindow.
// The epoch prevents a recycled address from satisfying the old identity.
struct WXDLLIMPEXP_CORE wxWinUITransientOwnerIdentityForTesting
{
    std::uintptr_t ownerKey = 0;
    std::uint64_t epoch = 0;

    explicit operator bool() const
    {
        return ownerKey != 0 && epoch != 0;
    }
};

WXDLLIMPEXP_CORE wxWinUITransientOwnerIdentityForTesting
wxWinUIGetTransientOwnerIdentityForTesting(std::uintptr_t ownerKey);
WXDLLIMPEXP_CORE bool
wxWinUIHasTransientOwnerIdentityForTesting(
    const wxWinUITransientOwnerIdentityForTesting& identity);

// Process-wide observation of the real per-TLW registry. Per-owner Count()
// cannot prove that a destroyed owner and its retained cancellation callbacks
// have disappeared because the wxWindow identity is no longer queryable.
// This implementation-only snapshot deliberately exposes counts, never
// callbacks or owner addresses.
struct WXDLLIMPEXP_CORE wxWinUITransientRegistrySnapshot
{
    std::size_t ownerCount = 0;
    std::size_t transientCount = 0;
    std::size_t cancelCallbackCount = 0;
    std::size_t modalCount = 0;
    std::size_t popupCount = 0;
    std::size_t teachingTipCount = 0;
    std::size_t pendingOwnerRetireCount = 0;
    std::size_t activeOwnerRetirementCount = 0;
    std::uint64_t ownerRetireEnqueueCount = 0;
    std::uint64_t ownerRetireCallbackCount = 0;
};

WXDLLIMPEXP_CORE wxWinUITransientRegistrySnapshot
wxWinUIGetTransientRegistrySnapshotForTesting();

// A generation-bearing lease is deliberately independent of XAML.  The
// production overlay presenter and its deterministic tests use this exact
// state machine, so exclusion and stale-cleanup behaviour cannot drift apart.
class wxWinUITransientModalGate final
{
public:
    struct Lease
    {
        std::uintptr_t ownerKey = 0;
        std::uint64_t generation = 0;

        explicit operator bool() const
        {
            return ownerKey != 0 && generation != 0;
        }
    };

    Lease TryAcquire(std::uintptr_t ownerKey)
    {
        if ( ownerKey == 0 || m_active.find(ownerKey) != m_active.end() )
            return {};

        // Zero is reserved for an invalid lease.  Wrapping a 64-bit
        // generation is not a practical runtime event, but skipping zero
        // keeps the state machine correct even if it is forced in a test.
        if ( ++m_nextGeneration == 0 )
            ++m_nextGeneration;

        const Lease lease{ ownerKey, m_nextGeneration };
        m_active.emplace(ownerKey, lease.generation);
        return lease;
    }

    bool IsCurrent(const Lease& lease) const
    {
        if ( !lease )
            return false;

        const auto it = m_active.find(lease.ownerKey);
        return it != m_active.end() && it->second == lease.generation;
    }

    bool Release(const Lease& lease)
    {
        if ( !IsCurrent(lease) )
            return false;

        m_active.erase(lease.ownerKey);
        return true;
    }

    bool Cancel(std::uintptr_t ownerKey)
    {
        return ownerKey != 0 && m_active.erase(ownerKey) != 0;
    }

    void CancelAll()
    {
        m_active.clear();
    }

    std::size_t GetActiveCount() const
    {
        return m_active.size();
    }

private:
    std::map<std::uintptr_t, std::uint64_t> m_active;
    std::uint64_t m_nextGeneration = 0;
};

// ContentDialogResult::None is ambiguous: it covers Escape/external dismissal
// as well as a successful click on the Close button.  Record the accepted
// button independently and resolve None to the third wx id only when the real
// CloseButtonClick path accepted it.  In particular a veto does not call
// AcceptButton(), so a later shutdown can never masquerade as that button.
enum class wxWinUIDialogCompletion
{
    None,
    Primary,
    Secondary
};

class wxWinUIDialogResultState final
{
public:
    void AcceptButton(std::size_t index)
    {
        m_acceptedButton = index < 3 ? static_cast<int>(index) : -1;
    }

    void Reset()
    {
        m_acceptedButton = -1;
    }

    bool HasAcceptedButton() const
    {
        return m_acceptedButton >= 0;
    }

    int Resolve(wxWinUIDialogCompletion completion,
                const int *buttonIds,
                std::size_t buttonCount) const
    {
        if ( !buttonIds )
            buttonCount = 0;

        switch ( completion )
        {
            case wxWinUIDialogCompletion::Primary:
                return buttonCount > 0 ? buttonIds[0] : wxID_CANCEL;

            case wxWinUIDialogCompletion::Secondary:
                return buttonCount > 1 ? buttonIds[1] : wxID_CANCEL;

            case wxWinUIDialogCompletion::None:
                return m_acceptedButton == 2 && buttonCount > 2
                    ? buttonIds[2]
                    : wxID_CANCEL;
        }

        return wxID_CANCEL;
    }

private:
    int m_acceptedButton = -1;
};

inline bool
wxWinUIShouldCancelExternalDialogDismiss(bool canDismissExternally,
                                         bool acceptedButton)
{
    return !canDismissExternally && !acceptedButton;
}

// Decision core for the nested wx loop.  A dialog may remain open for as long
// as the user chooses, but the presenter must never enter that loop unless it
// has both proved that the DispatcherQueue accepts work and installed a
// shutdown notification capable of waking it.  Once completed or aborted,
// no later callback may re-arm the loop.
class wxWinUIDialogLoopState final
{
public:
    bool Prepare(bool dispatcherAccepted, bool shutdownHookInstalled)
    {
        if ( !m_active || m_completed || m_aborted )
            return false;

        if ( !dispatcherAccepted || !shutdownHookInstalled )
        {
            m_aborted = true;
            return false;
        }

        m_prepared = true;
        return true;
    }

    bool Start()
    {
        if ( !m_active || !m_prepared || m_completed || m_aborted )
            return false;

        m_running = true;
        return true;
    }

    bool Complete()
    {
        if ( !m_active || m_completed || m_aborted )
            return false;

        m_completed = true;
        m_running = false;
        return true;
    }

    bool Abort()
    {
        if ( !m_active || m_completed || m_aborted )
            return false;

        m_aborted = true;
        m_running = false;
        return true;
    }

    void Invalidate()
    {
        m_active = false;
        m_prepared = false;
        m_running = false;
    }

    bool IsCompleted() const { return m_completed; }
    bool IsAborted() const { return m_aborted; }
    bool IsRunning() const { return m_running; }
    bool IsActive() const { return m_active; }

private:
    bool m_active = true;
    bool m_prepared = false;
    bool m_running = false;
    bool m_completed = false;
    bool m_aborted = false;
};

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_DIALOGSESSION_H_
