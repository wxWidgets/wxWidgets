#ifndef __STATBOXH_BASE__
#define __STATBOXH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/statbox.h"
#elif defined(__MOTIF__)
#include "wx/xt/statbox.h"
#elif defined(__GTK__)
#include "wx/gtk/statbox.h"
#endif

#endif
    // __STATBOXH_BASE__
