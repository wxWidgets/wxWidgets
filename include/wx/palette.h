#ifndef __PALETTEH_BASE__
#define __PALETTEH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/palette.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/palette.h"
#elif defined(__WXGTK__)
#include "wx/gtk/palette.h"
#endif

#endif
    // __PALETTEH_BASE__
