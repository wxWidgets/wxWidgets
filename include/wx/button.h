#ifndef __BUTTONH_BASE__
#define __BUTTONH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/button.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/button.h"
#elif defined(__WXGTK__)
#include "wx/gtk/button.h"
#endif

#endif
    // __BUTTONH_BASE__
