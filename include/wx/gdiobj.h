#ifndef __GDIOBJH_BASE__
#define __GDIOBJH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/gdiobj.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/gdiobj.h"
#elif defined(__WXGTK__)
#include "wx/gtk/gdiobj.h"
#endif

#endif
    // __GDIOBJH_BASE__
