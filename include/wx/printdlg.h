#ifndef _WX_PRINTDLG_H_BASE_
#define _WX_PRINTDLG_H_BASE_

#if defined(__WXUNIVERSAL__) && (!defined(__WXMSW__) || wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMSW__)
#include "wx/msw/printdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXX11__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMGL__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/printdlg.h"
#elif defined(__WXPM__)
#include "wx/generic/prntdlgg.h"
#endif

#if (defined(__WXUNIVERSAL__) && (!defined(__WXMSW__) || wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW) ) || (!defined(__WXMSW__) && !defined(__WXMAC__))
#define wxPrintDialog wxGenericPrintDialog

#define wxPrintSetupDialog wxGenericPrintSetupDialog

#define wxPageSetupDialog wxGenericPageSetupDialog
#endif

#endif
    // _WX_PRINTDLG_H_BASE_
