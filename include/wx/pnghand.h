#ifndef __PNGHANDH_BASE__
#define __PNGHANDH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/pnghand.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/pnghand.h"
#elif defined(__WXGTK__)
#include "wx/gtk/pnghand.h"
#endif

#endif
    // __PNGHANDH_BASE__
