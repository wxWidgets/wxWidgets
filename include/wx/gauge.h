#ifndef __GAUGEH_BASE__
#define __GAUGEH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/gauge.h"
#elif defined(__MOTIF__)
#include "wx/xt/gauge.h"
#elif defined(__GTK__)
#include "wx/gtk/gauge.h"
#endif

#endif
    // __GAUGEH_BASE__
