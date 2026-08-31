/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/textpeer.h
// Purpose:     Shared private contracts for WinUI text peers
// Author:      wxWidgets development team
// Created:     2026-07-25
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TEXTPEER_H_
#define _WX_WINUI_PRIVATE_TEXTPEER_H_

#include "wx/defs.h"
#include "wx/string.h"

#include <atomic>
#include <cstddef>
#include <cstdint>

// Delegate state shared by the WinUI text controls. Delegates retain this
// object, never the wx control. Close() invalidates it before attempting any
// WinRT revocation, so a queued callback can only observe a null owner.
class WXDLLIMPEXP_CORE wxWinUITextCallbackState final
{
public:
    explicit wxWinUITextCallbackState(void *owner);
    ~wxWinUITextCallbackState();

    template <typename T>
    T *GetOwner() const
    {
        return static_cast<T *>(
            m_owner.load(std::memory_order_acquire));
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
    }

    std::uint64_t GetGeneration() const { return m_generation; }

    static std::size_t GetLiveCountForTesting();

private:
    std::atomic<void *> m_owner;
    std::uint64_t m_generation;
};

// wxMSW exposes CRLF-sized positions for a plain multiline edit while wx
// strings and XAML text are normalized to LF. Keep that translation in one
// pure, testable place. Rich editors and single-line controls use one public
// position per wxString code unit.
class WXDLLIMPEXP_CORE wxWinUITextPositionMap final
{
public:
    wxWinUITextPositionMap(const wxString& value,
                           bool multiline,
                           bool twoCharacterNewlines);

    long GetLastPosition() const;
    bool PublicToLogical(long position, long *logical) const;
    bool PublicRangeToLogical(long from,
                              long to,
                              long *logicalFrom,
                              long *logicalTo) const;
    wxString GetRange(long from, long to) const;
    bool ReplaceRange(long from,
                      long to,
                      const wxString& replacement,
                      wxString *value,
                      long *insertionPoint = nullptr) const;
    long LogicalToPublic(long logical) const;
    bool PositionToXY(long position, long *x, long *y) const;
    long XYToPosition(long x, long y) const;

    static wxString NormalizeNewlines(const wxString& value);

private:
    wxString ExpandPublicValue(const wxString& value) const;

    wxString m_value;
    bool m_multiline;
    bool m_twoCharacterNewlines;
};

#endif // _WX_WINUI_PRIVATE_TEXTPEER_H_
