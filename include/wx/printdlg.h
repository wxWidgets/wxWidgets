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
#include "wx/generic/prntdlgg.h"
#elif defined(__WXSRUBS__)
#include "wx/generic/prntdlgg.h"
#endif

#endif
    // _WX_PRINTDLG_H_BASE_
