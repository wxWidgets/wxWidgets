#ifndef __FRAMEH_BASE__
#define __FRAMEH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/frame.h"
#elif defined(__MOTIF__)
#include "wx/xt/frame.h"
#elif defined(__GTK__)
#include "wx/gtk/frame.h"
#endif

#endif
    // __FRAMEH_BASE__
