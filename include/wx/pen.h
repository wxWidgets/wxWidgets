#ifndef __PENH_BASE__
#define __PENH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/pen.h"
#elif defined(__MOTIF__)
#include "wx/xt/pen.h"
#elif defined(__GTK__)
#include "wx/gtk/pen.h"
#endif

#endif
    // __PENH_BASE__
