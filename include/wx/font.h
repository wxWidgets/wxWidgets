#ifndef __FONTH_BASE__
#define __FONTH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/font.h"
#elif defined(__MOTIF__)
#include "wx/xt/font.h"
#elif defined(__GTK__)
#include "wx/gtk/font.h"
#endif

#endif
    // __FONTH_BASE__
