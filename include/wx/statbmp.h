#ifndef __STATBMPH_BASE__
#define __STATBMPH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/statbmp.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/statbmp.h"
#elif defined(__WXGTK__)
#include "wx/gtk/statbmp.h"
#elif defined(__WXQT__)
#include "wx/qt/statbmp.h"
#endif

#endif
    // __STATBMPH_BASE__
