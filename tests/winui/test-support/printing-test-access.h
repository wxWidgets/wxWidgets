/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI printing test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_PRINTING_TEST_ACCESS_H
#define WX_WINUI_PRINTING_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include "wx/msw/wrapwin.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PRINTING_ARCHITECTURE

// Substitute only the final common-dialog boundaries. Production still
// builds the real PRINTDLGEX/PAGESETUPDLG structures and runs all conversion,
// owner and lifetime code; tests receive the native structure as an opaque
// pointer.
struct wxMSWPrintDialogNativeOpsForTesting
{
    void* context = nullptr;
    long (WINAPI *printDialogEx)(void*, void*) = nullptr;
    int (WINAPI *pageSetupDialog)(void*, void*) = nullptr;
};

WXDLLIMPEXP_CORE void
wxMSWSetPrintDialogNativeOpsForTesting(
    const wxMSWPrintDialogNativeOpsForTesting& ops);
WXDLLIMPEXP_CORE void wxMSWResetPrintDialogNativeOpsForTesting();
WXDLLIMPEXP_CORE bool wxMSWIsPrinterDialogShownForTesting();

// Substitute only the final GDI printing calls. The real printer DC and print
// loop retain ownership of sequencing, errors, cancellation and teardown.
struct wxMSWPrinterDCNativeOpsForTesting
{
    void* context = nullptr;
    int (WINAPI *startDoc)(void*, void*, const void*) = nullptr;
    int (WINAPI *endDoc)(void*, void*) = nullptr;
    int (WINAPI *startPage)(void*, void*) = nullptr;
    int (WINAPI *endPage)(void*, void*) = nullptr;
};

WXDLLIMPEXP_CORE void
wxMSWSetPrinterDCNativeOpsForTesting(
    const wxMSWPrinterDCNativeOpsForTesting& ops);
WXDLLIMPEXP_CORE void wxMSWResetPrinterDCNativeOpsForTesting();
WXDLLIMPEXP_CORE bool wxMSWInvokePrintAbortProcForTesting();

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_PRINTING_ARCHITECTURE

#endif // WX_WINUI_PRINTING_TEST_ACCESS_H
