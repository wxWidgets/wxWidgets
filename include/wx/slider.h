#ifndef __SLIDERH_BASE__
#define __SLIDERH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/slider.h"
#elif defined(__MOTIF__)
#include "wx/xt/slider.h"
#elif defined(__GTK__)
#include "wx/gtk/slider.h"
#endif

#endif
    // __SLIDERH_BASE__
