#ifndef __TIMERH_BASE__
#define __TIMERH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/timer.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/timer.h"
#elif defined(__WXGTK__)
#include "wx/gtk/timer.h"
#endif

#endif
    // __TIMERH_BASE__
