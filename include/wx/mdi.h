#ifndef __MDIH_BASE__
#define __MDIH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/mdi.h"
#elif defined(__MOTIF__)
#include "wx/xt/mdi.h"
#elif defined(__GTK__)
#include "wx/gtk/mdi.h"
#endif

#endif
    // __MDIH_BASE__
