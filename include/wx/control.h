#ifndef __CONTROLH_BASE__
#define __CONTROLH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/control.h"
#elif defined(__MOTIF__)
#include "wx/xt/control.h"
#elif defined(__GTK__)
#include "wx/gtk/control.h"
#endif

#endif
    // __CONTROLH_BASE__
