#ifndef __DDEH_BASE__
#define __DDEH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dde.h"
#elif defined(__MOTIF__)
#include "wx/xt/dde.h"
#elif defined(__GTK__)
#include "wx/gtk/dde.h"
#endif

#endif
    // __DDEH_BASE__
