#ifndef __DCSCREENH_BASE__
#define __DCSCREENH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dcscreen.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dcscreen.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcscreen.h"
#endif

#endif
    // __DCSCREENH_BASE__
