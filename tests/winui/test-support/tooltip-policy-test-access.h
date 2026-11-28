/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI tooltip-policy test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TOOLTIP_POLICY_TEST_ACCESS_H
#define WX_WINUI_TOOLTIP_POLICY_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3 && wxUSE_TOOLTIPS

class WXDLLIMPEXP_CORE wxWinUIToolTipPolicyTestAccess final
{
public:
    enum Fault
    {
        Fault_None = 0,
        Fault_SetBeforeCommit = 1,
        Fault_SetAfterCommit = 2,
        Fault_Policy = 4
    };

    // UINT_MAX is a permanent fault used to prove bounded replay.
    static void SetFault(unsigned faults, unsigned count);
    static void ResetDiagnostics();

    static unsigned GetManagedToolTipCount();
    static unsigned GetFaultAttemptCount();
    static unsigned GetReplayScheduleCount();
    static unsigned GetReplayExecutionCount();
    static unsigned GetQuarantinedEntryCount();
    static bool IsReplayScheduled();
    static bool IsShutdown();
};

#endif // wxUSE_WINUI3 && wxUSE_TOOLTIPS

#endif // WX_WINUI_TOOLTIP_POLICY_TEST_ACCESS_H
