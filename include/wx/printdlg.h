#ifndef _WX_PRINTDLG_H_BASE_
#define _WX_PRINTDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/printdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXQT__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/printdlg.h"
#elif defined(__WXPM__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/prntdlgg.h"
#endif

#if !defined(__WXMSW__) && !defined(__WXMAC__)
#define wxPrintDialog wxGenericPrintDialog
#define sm_classwxPrintDialog sm_classwxGenericPrintDialog

#define wxPrintSetupDialog wxGenericPrintSetupDialog
#define sm_classwxPrintSetupDialog sm_classwxGenericPrintSetupDialog

#define wxPageSetupDialog wxGenericPageSetupDialog
#define sm_classwxPageSetupDialog sm_classwxGenericPageSetupDialog
#endif

#endif
    // _WX_PRINTDLG_H_BASE_
