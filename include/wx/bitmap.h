#ifndef __BITMAPH_BASE__
#define __BITMAPH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/bitmap.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/bitmap.h"
#elif defined(__WXGTK__)
#include "wx/gtk/bitmap.h"
#endif

#endif
    // __BITMAPH_BASE__
