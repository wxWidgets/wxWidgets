#ifndef __PNGHANDH_BASE__
#define __PNGHANDH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/pnghand.h"
#elif defined(__MOTIF__)
#include "wx/xt/pnghand.h"
#elif defined(__GTK__)
#include "wx/gtk/pnghand.h"
#endif

#endif
    // __PNGHANDH_BASE__
