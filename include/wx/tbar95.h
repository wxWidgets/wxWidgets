#ifndef __TBAR95H_BASE__
#define __TBAR95H_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/tbar95.h"
#elif defined(__MOTIF__)
#include "wx/xt/tbar95.h"
#elif defined(__GTK__)
#include "wx/gtk/tbar95.h"
#endif

#endif
    // __TBAR95H_BASE__
