#ifndef __BUTTONH_BASE__
#define __BUTTONH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/button.h"
#elif defined(__MOTIF__)
#include "wx/xt/button.h"
#elif defined(__GTK__)
#include "wx/gtk/button.h"
#endif

#endif
    // __BUTTONH_BASE__
