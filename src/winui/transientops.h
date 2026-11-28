/////////////////////////////////////////////////////////////////////////////
// Intracore native contracts for the existing owner/backdrop transactions.
// This header is not installed and declares no exported test interface.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_TRANSIENTOPS_H
#define WX_WINUI_TRANSIENTOPS_H
#include "wx/defs.h"
#if wxUSE_WINUI3
// Deliberately independent of RECT so the state-machine tests don't need to
// include the complete Windows headers.
struct wxWinUITransientRect
{
    long left = 0;
    long top = 0;
    long right = 0;
    long bottom = 0;

    bool operator==(const wxWinUITransientRect& other) const
    {
        return left == other.left && top == other.top &&
               right == other.right && bottom == other.bottom;
    }

    bool operator!=(const wxWinUITransientRect& other) const
    {
        return !(*this == other);
    }
};

// Every native boundary used by the two state machines is injectable. The
// production table verifies USER32 calls and their postconditions; tests use
// a deterministic fake to exercise failures, HWND reuse and reentrancy.
struct wxWinUIOwnerNativeOps
{
    void *context = nullptr;

    unsigned long long (*getGeneration)(void *, WXHWND) = nullptr;
    bool (*isWindow)(void *, WXHWND) = nullptr;

    bool (*getOwner)(void *, WXHWND, WXHWND *) = nullptr;
    bool (*setOwner)(void *, WXHWND, WXHWND, WXHWND *) = nullptr;
    bool (*setOwnerMarker)(void *, WXHWND, WXHWND) = nullptr;
    bool (*getOwnerMarker)(void *, WXHWND, WXHWND *) = nullptr;
    bool (*clearOwnerMarker)(void *, WXHWND) = nullptr;
};

struct wxWinUIBackdropNativeOps
{
    void *context = nullptr;

    unsigned long long (*getGeneration)(void *, WXHWND) = nullptr;
    bool (*isWindow)(void *, WXHWND) = nullptr;
    bool (*getRect)(void *, WXHWND, wxWinUITransientRect *) = nullptr;
    bool (*setSize)(void *, WXHWND, int, int) = nullptr;
    bool (*setPrimeMarker)(void *, WXHWND) = nullptr;
    bool (*hasPrimeMarker)(void *, WXHWND) = nullptr;
    bool (*clearPrimeMarker)(void *, WXHWND) = nullptr;

    // Returns false without taking ownership of callbackData. On success it
    // owns the data, invokes callback at most once, and always invokes destroy
    // exactly once even when application shutdown discards the queued work.
    bool (*post)(void *,
                 void (*callback)(void *),
                 void (*destroy)(void *),
                 void *callbackData) = nullptr;
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_TRANSIENTOPS_H
