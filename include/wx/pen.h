#ifndef __PENH_BASE__
#define __PENH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/pen.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/pen.h"
#elif defined(__WXGTK__)
#include "wx/gtk/pen.h"
#endif

#endif
    // __PENH_BASE__
