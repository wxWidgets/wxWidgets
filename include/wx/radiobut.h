#ifndef __RADIOBUTH_BASE__
#define __RADIOBUTH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/radiobut.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/radiobut.h"
#elif defined(__WXGTK__)
#include "wx/gtk/radiobut.h"
#endif

#endif
    // __RADIOBUTH_BASE__
