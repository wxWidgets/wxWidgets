#ifndef __BRUSHH_BASE__
#define __BRUSHH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/brush.h"
#elif defined(__MOTIF__)
#include "wx/xt/brush.h"
#elif defined(__GTK__)
#include "wx/gtk/brush.h"
#endif

#endif
    // __BRUSHH_BASE__
