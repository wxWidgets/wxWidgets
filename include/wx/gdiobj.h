#ifndef __GDIOBJH_BASE__
#define __GDIOBJH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/gdiobj.h"
#elif defined(__MOTIF__)
#include "wx/xt/gdiobj.h"
#elif defined(__GTK__)
#include "wx/gtk/gdiobj.h"
#endif

#endif
    // __GDIOBJH_BASE__
