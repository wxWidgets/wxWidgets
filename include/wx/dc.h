#ifndef __DCH_BASE__
#define __DCH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dc.h"
#elif defined(__MOTIF__)
#include "wx/xt/dc.h"
#elif defined(__GTK__)
#include "wx/gtk/dc.h"
#endif

#endif
    // __DCH_BASE__
