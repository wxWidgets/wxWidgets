/////////////////////////////////////////////////////////////////////////////
// Private, non-installed host geometry test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_HOST_GEOMETRY_TEST_ACCESS_H
#define WX_WINUI_HOST_GEOMETRY_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include <functional>

#if wxUSE_WINUI3

class wxWindow;

class WXDLLIMPEXP_CORE wxWinUIHostGeometryTestAccess final
{
public:
    static unsigned GetFlushRunCount();
    static unsigned GetBridgeRegionChangeCount();
    static unsigned GetFlushCallbackAttemptCount();
    static void ResetImpossibleZOrderCount();

    // Consume-before-call seams on the real geometry publication and
    // Canvas::SetZIndex boundaries; no detached projection is exercised.
    static void SetNextSlotSyncedHook(
        std::function<void (wxWindow *)> callback);
    static void SetNextZOrderSetHook(std::function<void ()> callback);

    // Asks for the full sync of the slot of this window at the next flush.
    // A window which only moves or resizes is repositioned without one.
    static void RequestSlotSync(wxWindow *window);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_HOST_GEOMETRY_TEST_ACCESS_H
