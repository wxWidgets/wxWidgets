#ifndef __PRINTH_BASE__
#define __PRINTH_BASE__

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
    // __PRINTH_BASE__
