/////////////////////////////////////////////////////////////////////////////
// Private, non-installed input-state observations.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_INPUT_STATE_TEST_ACCESS_H
#define WX_WINUI_INPUT_STATE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/inputstate.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIInputStateTestAccess final
{
public:
    static std::size_t CountActions(
        const wxWinUIInputTransition& transition,
        wxWinUIInputActionKind kind);
    static bool IsStormGuardArmed(const wxWinUIInputState& state);
    static unsigned GetStormInterventionCount(
        const wxWinUIInputState& state);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_INPUT_STATE_TEST_ACCESS_H
