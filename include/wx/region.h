#ifndef __REGIONH_BASE__
#define __REGIONH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/region.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/region.h"
#elif defined(__WXGTK__)
#include "wx/gtk/region.h"
#endif

#endif
    // __REGIONH_BASE__
