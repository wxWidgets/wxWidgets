#ifndef __STATBMPH_BASE__
#define __STATBMPH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/statbmp.h"
#elif defined(__MOTIF__)
#include "wx/xt/statbmp.h"
#elif defined(__GTK__)
#include "wx/gtk/statbmp.h"
#endif

#endif
    // __STATBMPH_BASE__
