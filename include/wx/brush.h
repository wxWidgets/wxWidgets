#ifndef __BRUSHH_BASE__
#define __BRUSHH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/brush.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/brush.h"
#elif defined(__WXGTK__)
#include "wx/gtk/brush.h"
#endif

#endif
    // __BRUSHH_BASE__
