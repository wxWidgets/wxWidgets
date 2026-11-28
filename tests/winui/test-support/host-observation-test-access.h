/////////////////////////////////////////////////////////////////////////////
// Private, non-installed observations of the real host retirement state.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_OBSERVATION_TEST_ACCESS_H
#define WX_WINUI_HOST_OBSERVATION_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3
class wxWinUITopLevelHost;
class wxWinUIDropBroker;

class WXDLLIMPEXP_CORE wxWinUIHostObservationTestAccess final
{
public:
    // Counts derived from the live production registries. They do not add
    // any shipping state of their own.
    static unsigned GetLiveHostCount();
    static unsigned GetLiveSlotCount();

    // Test-only lifecycle telemetry. Shipping builds neither store nor
    // update these counters.
    static unsigned GetLiveSlotLifetimeCount();
    static unsigned GetSlotHandlerAddCount();
    static unsigned GetSlotHandlerRevokeCount();
    static unsigned GetLiveHostLifetimeCount();
    static unsigned GetRootHandlerAddCount();
    static unsigned GetRootHandlerRevokeCount();
    static unsigned GetLiveSubclassContextCount();
    static unsigned GetLiveLoadedHookCount();

    // Passive acquire loads of test telemetry: no drain or sweep.
    static unsigned GetPendingPhysicalDisconnectGateCount();
    static unsigned GetPendingTLWTerminalRetirementCount();

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    // Borrowed pointer only. The caller must keep the host alive, and use the
    // broker test accessor's explicit native validation when that is required.
    static wxWinUIDropBroker *GetDropBroker(const wxWinUITopLevelHost& host);
#endif
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_HOST_OBSERVATION_TEST_ACCESS_H
