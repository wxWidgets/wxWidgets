#ifndef __SCROLBARH_BASE__
#define __SCROLBARH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/scrolbar.h"
#elif defined(__MOTIF__)
#include "wx/xt/scrolbar.h"
#elif defined(__GTK__)
#include "wx/gtk/scrolbar.h"
#endif

#endif
    // __SCROLBARH_BASE__
