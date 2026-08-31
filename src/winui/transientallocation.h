/////////////////////////////////////////////////////////////////////////////
// Intracore test-only allocation boundary shared by presenter and MSW popup.
// Not installed and deliberately not DLL-exported. There is one fault authority,
// implemented in dlgpresenter.cpp; actual allocations remain at their old sites.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_TRANSIENT_ALLOCATION_H
#define WX_WINUI_TRANSIENT_ALLOCATION_H
#ifdef WXWINUI_TEST_SUPPORT
#include "transient-test-access.h"
#if wxUSE_WINUI3
void wxWinUITransientAllocationPoint(wxWinUITransientTestAccess::AllocationSite site);
#endif
#endif
#endif // WX_WINUI_TRANSIENT_ALLOCATION_H
