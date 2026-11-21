/////////////////////////////////////////////////////////////////////////////
// Private, non-installed observation of the GDI paint buffer before publication.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_OPAQUE_PAINT_TEST_ACCESS_H
#define WX_WINUI_OPAQUE_PAINT_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/gdicmn.h"

class WXDLLIMPEXP_CORE wxWinUIOpaquePaintTestAccess final
{
public:
    // Observers must not draw, mutate windows, pump messages or throw. The HDC
    // and bounds are borrowed only for this callback, after alpha repair.
    using Observer = void (*)(WXHWND, WXHDC, void *, const wxRect&, void *);
    static void SetObserver(Observer observer, void *context);
};

#endif // WX_WINUI_OPAQUE_PAINT_TEST_ACCESS_H
