#ifndef __LISTCTRLH_BASE__
#define __LISTCTRLH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/listctrl.h"
#elif defined(__MOTIF__)
#include "wx/generic/listctrl.h"
#elif defined(__GTK__)
#include "wx/generic/listctrl.h"
#endif

#endif
    // __LISTCTRLH_BASE__
