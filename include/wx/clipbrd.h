#ifndef __CLIPBRDH_BASE__
#define __CLIPBRDH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/clipbrd.h"
#elif defined(__MOTIF__)
#include "wx/xt/clipbrd.h"
#elif defined(__GTK__)
#include "wx/gtk/clipbrd.h"
#endif

#endif
    // __CLIPBRDH_BASE__
