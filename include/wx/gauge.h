#ifndef _WX_GAUGE_H_BASE_
#define _WX_GAUGE_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/gauge.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/gauge.h"
#elif defined(__WXGTK__)
#include "wx/gtk/gauge.h"
#elif defined(__WXQT__)
#include "wx/qt/gauge.h"
#elif defined(__WXMAC__)
#include "wx/mac/gauge.h"
#elif defined(__WXPM__)
#include "wx/os2/gauge.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/gauge.h"
#endif

#endif
    // _WX_GAUGE_H_BASE_
