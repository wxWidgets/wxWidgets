#ifndef __DIALOGH_BASE__
#define __DIALOGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dialog.h"
#elif defined(__MOTIF__)
#include "wx/xt/dialog.h"
#elif defined(__GTK__)
#include "wx/gtk/dialog.h"
#endif

#endif
    // __DIALOGH_BASE__
