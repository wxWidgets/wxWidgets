#ifndef __DCH_BASE__
#define __DCH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dc.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dc.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dc.h"
#endif

#endif
    // __DCH_BASE__
