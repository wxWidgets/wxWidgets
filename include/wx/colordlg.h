#ifndef __COLORDLGH_BASE__
#define __COLORDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/colordlg.h"
#elif defined(__MOTIF__)
#include "wx/generic/colrdlgg.h"
#elif defined(__GTK__)
#include "wx/generic/colrdlgg.h"
#endif

#endif
    // __COLORDLGH_BASE__
