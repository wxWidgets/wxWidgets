///////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/auxwindows.h
// Purpose:     WinUI splash/tip-window lifetime test seams
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_AUXWINDOWS_H_
#define _WX_WINUI_PRIVATE_AUXWINDOWS_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include <cstddef>

#if wxUSE_SPLASH

class wxSplashScreen;

// Callback-rich boundaries in the generic splash constructor. Production
// leaves this hook null. Tests use it to prove that rollback remains complete
// when destruction, owner reparenting or an exception interrupts construction
// at publication. These seams run only after XAML has been initialized by the
// GUI test process; they are not a pre-initialization runtime probe.
enum class wxWinUISplashHookPointForTesting
{
    AfterContent,
    AfterTimerStart,
    AfterFilterRegistration,
    AfterShow,
    AfterUpdate
};

using wxWinUISplashHookForTesting =
    void (*)(wxSplashScreen *, wxWinUISplashHookPointForTesting);

WXDLLIMPEXP_CORE void wxWinUISetSplashHookForTesting(
    wxWinUISplashHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetSplashHookForTesting();

// Fail before the sidecar entry is inserted. This is deliberately narrower
// than replacing the process allocator and makes constructor unwinding
// deterministic. The seam is exercised only when wxUSE_EXCEPTIONS is enabled.
WXDLLIMPEXP_CORE void
wxWinUIFailNextSplashRuntimeAllocationForTesting();

WXDLLIMPEXP_CORE std::size_t
wxWinUIGetSplashRuntimeStateCountForTesting();

#endif // wxUSE_SPLASH


#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_AUXWINDOWS_H_
