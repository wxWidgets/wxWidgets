#ifndef __REGIONH_BASE__
#define __REGIONH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/region.h"
#elif defined(__MOTIF__)
#include "wx/xt/region.h"
#elif defined(__GTK__)
#include "wx/gtk/region.h"
#endif

#endif
    // __REGIONH_BASE__
