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

#else
#include "wx/generic/printps.h"

#ifndef wxPrinter
#define wxPrinter wxPostScriptPrinter
#endif
#ifndef wxPrintPreview
#define wxPrintPreview wxPostScriptPrintPreview
#endif

#endif


#endif
    // _WX_PRINT_H_BASE_
