#ifndef __PALETTEH_BASE__
#define __PALETTEH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/palette.h"
#elif defined(__MOTIF__)
#include "wx/xt/palette.h"
#elif defined(__GTK__)
#include "wx/gtk/palette.h"
#endif

#endif
    // __PALETTEH_BASE__
