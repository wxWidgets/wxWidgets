#ifndef __TBARMSWH_BASE__
#define __TBARMSWH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/tbarmsw.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/tbarmsw.h"
#elif defined(__WXGTK__)
#include "wx/gtk/tbarmsw.h"
#endif

#endif
    // __TBARMSWH_BASE__
