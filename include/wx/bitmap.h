#ifndef __BITMAPH_BASE__
#define __BITMAPH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/bitmap.h"
#elif defined(__MOTIF__)
#include "wx/xt/bitmap.h"
#elif defined(__GTK__)
#include "wx/gtk/bitmap.h"
#endif

#endif
    // __BITMAPH_BASE__
