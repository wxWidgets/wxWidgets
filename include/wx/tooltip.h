#ifndef _WX_TOOLTIP_H_BASE_
#define _WX_TOOLTIP_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/tooltip.h"
#elif defined(__WXMOTIF__)
// #include "wx/motif/tooltip.h"
#elif defined(__WXGTK__)
#include "wx/gtk/tooltip.h"
#elif defined(__WXQT__)
#include "wx/qt/tooltip.h"
#elif defined(__WXMAC__)
#include "wx/mac/tooltip.h"
#elif defined(__WXSTUBS__)
// #include "wx/stubs/tooltip.h"
#endif

#endif
    // _WX_TOOLTIP_H_BASE_
