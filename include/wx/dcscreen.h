#ifndef __DCSCREENH_BASE__
#define __DCSCREENH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dcscreen.h"
#elif defined(__MOTIF__)
#include "wx/xt/dcscreen.h"
#elif defined(__GTK__)
#include "wx/gtk/dcscreen.h"
#endif

#endif
    // __DCSCREENH_BASE__
