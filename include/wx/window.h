#ifndef __WINDOWH_BASE__
#define __WINDOWH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/window.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/window.h"
#elif defined(__WXGTK__)
#include "wx/gtk/window.h"
#endif

#endif
    // __WINDOWH_BASE__
