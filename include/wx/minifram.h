#ifndef __MINIFRAMH_BASE__
#define __MINIFRAMH_BASE_

#if defined(__WINDOWS__)
#include "wx/msw/minifram.h"
#elif defined(__MOTIF__)
#include "wx/xt/minifram.h"
#elif defined(__GTK__)
#include "wx/gtk/minifram.h"
#endif

#endif
    // __MINIFRAMH_BASE__
