#ifndef _WX_SLIDER_H_BASE_
#define _WX_SLIDER_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/slider.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/slider.h"
#elif defined(__WXGTK__)
#include "wx/gtk/slider.h"
#elif defined(__WXQT__)
#include "wx/qt/slider.h"
#elif defined(__WXMAC__)
#include "wx/mac/slider.h"
#elif defined(__WXPM__)
#include "wx/os2/slider.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/slider.h"
#endif

#endif
    // _WX_SLIDER_H_BASE_
