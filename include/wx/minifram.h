#ifndef __MINIFRAMH_BASE__
#define __MINIFRAMH_BASE_

#if defined(__WXMSW__)
#include "wx/msw/minifram.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/minifram.h"
#elif defined(__WXGTK__)
#include "wx/gtk/minifram.h"
#endif

#endif
    // __MINIFRAMH_BASE__
