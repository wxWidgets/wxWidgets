#ifndef __TIMERH_BASE__
#define __TIMERH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/timer.h"
#elif defined(__MOTIF__)
#include "wx/xt/timer.h"
#elif defined(__GTK__)
#include "wx/gtk/timer.h"
#endif

#endif
    // __TIMERH_BASE__
