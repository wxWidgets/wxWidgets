#ifndef __COLORDLGH_BASE__
#define __COLORDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/colordlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/colrdlgg.h"
#endif

#endif
    // __COLORDLGH_BASE__
