/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/nativeresize.h
// Purpose:     private diagnostics for the host-owned native resize handoff
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_NATIVERESIZE_H_
#define _WX_WINUI_PRIVATE_NATIVERESIZE_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/msw/wrapwin.h"
#include <cstdint>

class wxWindow;

enum class wxWinUINativeResizePhase
{
    Idle,
    Pending,
    Entered
};

struct wxWinUINativeResizeSnapshot
{
    wxWinUINativeResizePhase phase = wxWinUINativeResizePhase::Idle;
    std::uint64_t ticket = 0;
    unsigned scheduled = 0;
    unsigned entered = 0;
    unsigned exited = 0;
    unsigned cancelled = 0;
    int hitTest = HTNOWHERE;
    POINT screenPoint = { 0, 0 };
};

// These private seams never replace the native dispatch or its wndproc. The
// contact reader permits bounded begin/cancel integration without injecting
// mouse input; it does not establish that a physical drag works.
WXDLLIMPEXP_CORE bool wxWinUIGetNativeResizeSnapshotForTesting(
    wxWindow *window, wxWinUINativeResizeSnapshot *snapshot);
WXDLLIMPEXP_CORE void wxWinUISetNativeResizeContactReaderForTesting(
    bool (*reader)(void *), void *context = nullptr);
WXDLLIMPEXP_CORE void wxWinUIFailNextNativeResizePostForTesting();

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_NATIVERESIZE_H_
