#ifndef __WINDOWH_BASE__
#define __WINDOWH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/window.h"
#elif defined(__MOTIF__)
#include "wx/xt/window.h"
#elif defined(__GTK__)
#include "wx/gtk/window.h"
#endif

#endif
    // __WINDOWH_BASE__
