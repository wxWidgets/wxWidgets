#ifndef __SLIDERH_BASE__
#define __SLIDERH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/slider.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/slider.h"
#elif defined(__WXGTK__)
#include "wx/gtk/slider.h"
#elif defined(__WXQT__)
#include "wx/qt/slider.h"
#endif

#endif
    // __SLIDERH_BASE__
