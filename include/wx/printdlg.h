#ifndef __PRINTDLGH_BASE__
#define __PRINTDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/printdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/prntdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/prntdlgg.h"
#endif

#endif
    // __PRINTDLGH_BASE__
