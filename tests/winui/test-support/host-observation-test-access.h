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
    // Passive acquire loads of the existing real counters: no drain or sweep.
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
