#ifndef __PRINTDLGH_BASE__
#define __PRINTDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/printdlg.h"
#elif defined(__MOTIF__)
#include "wx/generic/prntdlgg.h"
#elif defined(__GTK__)
#include "wx/generic/prntdlgg.h"
#endif

#endif
    // __PRINTDLGH_BASE__
