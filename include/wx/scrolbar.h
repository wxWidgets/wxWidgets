#ifndef __SCROLBARH_BASE__
#define __SCROLBARH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/scrolbar.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/scrolbar.h"
#elif defined(__WXGTK__)
#include "wx/gtk/scrolbar.h"
#endif

#endif
    // __SCROLBARH_BASE__
