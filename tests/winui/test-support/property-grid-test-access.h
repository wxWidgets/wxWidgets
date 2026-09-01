///////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/test-support/property-grid-test-access.h
// Purpose:     Private, non-installed wxPropertyGrid test adapter
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_PROPERTY_GRID_TEST_ACCESS_H
#define WX_PROPERTY_GRID_TEST_ACCESS_H

#ifndef WXPROPGRID_TEST_SUPPORT
#error Enable the private wxPropertyGrid test support to use this header
#endif

#include "wx/propgrid/manager.h"

#if wxUSE_HEADERCTRL
class WXDLLIMPEXP_FWD_CORE wxHeaderCtrlEvent;
#endif

class WXDLLIMPEXP_PROPGRID wxPropertyGridTestAccess final
{
public:
#if wxUSE_TOOLBAR
    static void FailToolbarRemoval(unsigned int ordinal);
    static void ResetToolbarRemovalFailures();
#endif

#if wxUSE_HEADERCTRL
    static bool ProcessHeaderResizeEvent(wxPropertyGridManager* manager,
                                         wxHeaderCtrlEvent& event);
#endif

#ifdef __WXMSW__
    static void FailNextDirectEditorParking();
    static void FailNextFallbackEditorParking();
    static void ResetEditorParkingFailures();
    static unsigned int GetEditorParkingHostCount();
    static unsigned int GetDeferredEditorBatchCount();
    static bool IsEditorParkingHost(WXWidget hwnd);
#endif
};

#endif // WX_PROPERTY_GRID_TEST_ACCESS_H
