#ifndef __SPINBUTTH_BASE__
#define __SPINBUTTH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/spinbutt.h"
#elif defined(__MOTIF__)
#include "wx/xt/spinbutt.h"
#elif defined(__GTK__)
#include "wx/gtk/spinbutt.h"
#endif

#endif
    // __SPINBUTTH_BASE__
