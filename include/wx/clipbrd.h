#ifndef __CLIPBRDH_BASE__
#define __CLIPBRDH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/clipbrd.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/clipbrd.h"
#elif defined(__WXGTK__)
#include "wx/gtk/clipbrd.h"
#elif defined(__WXQT__)
#include "wx/gtk/clipbrd.h"
#endif

#endif
    // __CLIPBRDH_BASE__
