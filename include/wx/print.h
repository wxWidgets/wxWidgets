#ifndef _WX_PRINT_H_BASE_
#define _WX_PRINT_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/printwin.h"

#ifndef wxPrinter
#define wxPrinter wxWindowsPrinter
#endif
#ifndef wxPrintPreview
#define wxPrintPreview wxWindowsPrintPreview
#endif

#elif defined(__WXMAC__)

#include "wx/mac/printmac.h"

#ifndef wxPrinter
#define wxPrinter wxMacPrinter
#endif
#ifndef wxPrintPreview
#define wxPrintPreview wxMacPrintPreview
#endif

#else
#include "wx/generic/printps.h"

#ifndef wxPrinter
#define wxPrinter wxPostScriptPrinter
#define sm_classwxPrinter sm_classwxPostScriptPrinter
#endif
#ifndef wxPrintPreview
#define wxPrintPreview wxPostScriptPrintPreview
#define sm_classwxPrintPreview sm_classwxPostScriptPrintPreview
#endif

#endif


#endif
    // _WX_PRINT_H_BASE_
