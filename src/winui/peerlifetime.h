/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/peerlifetime.h
// Purpose:     Private owner/generation state shared by Gauge and ScrollBar
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PEERLIFETIME_H_
#define _WX_WINUI_PEERLIFETIME_H_

#include <atomic>
#include <cstdint>

// Delegates retain this state, not the owner. Invalidate it before revoking
// delegates or destroying the owner: either operation may drain a callback.
// An invalidated state is terminal; a new peer gets a distinct state object.
// This is not an owning lifetime pin; callers keep their UI-thread contract.
// The atomics preserve the existing callback protocol, not thread-safe access
// to the wx control. Mutation depth and peer retirement remain caller policies.
template <typename Owner>
class wxWinUIPeerLifetime
{
public:
    explicit wxWinUIPeerLifetime(Owner *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    Owner *GetOwner(std::uint64_t generation) const
    {
        if ( m_generation.load(std::memory_order_acquire) != generation )
            return nullptr;
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
    }

private:
    std::atomic<Owner *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
};

#endif // _WX_WINUI_PEERLIFETIME_H_
