#ifndef __FONTH_BASE__
#define __FONTH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/font.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/font.h"
#elif defined(__WXGTK__)
#include "wx/gtk/font.h"
#endif

#endif
    // __FONTH_BASE__
