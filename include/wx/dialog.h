#ifndef __DIALOGH_BASE__
#define __DIALOGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dialog.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dialog.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dialog.h"
#endif

#endif
    // __DIALOGH_BASE__
