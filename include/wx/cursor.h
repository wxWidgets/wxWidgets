#ifndef __CURSORH_BASE__
#define __CURSORH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/cursor.h"
#elif defined(__MOTIF__)
#include "wx/xt/cursor.h"
#elif defined(__GTK__)
#include "wx/gtk/cursor.h"
#endif

#endif
    // __CURSORH_BASE__
