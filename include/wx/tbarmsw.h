#ifndef __TBARMSWH_BASE__
#define __TBARMSWH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/tbarmsw.h"
#elif defined(__MOTIF__)
#include "wx/xt/tbarmsw.h"
#elif defined(__GTK__)
#include "wx/gtk/tbarmsw.h"
#endif

#endif
    // __TBARMSWH_BASE__
