#ifndef __RADIOBUTH_BASE__
#define __RADIOBUTH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/radiobut.h"
#elif defined(__MOTIF__)
#include "wx/xt/radiobut.h"
#elif defined(__GTK__)
#include "wx/gtk/radiobut.h"
#endif

#endif
    // __RADIOBUTH_BASE__
