#ifndef __ICONH_BASE__
#define __ICONH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/icon.h"
#elif defined(__MOTIF__)
#include "wx/xt/icon.h"
#elif defined(__GTK__)
#include "wx/gtk/icon.h"
#endif

#endif
    // __ICONH_BASE__
