#ifndef _WX_REGION_H_BASE_
#define _WX_REGION_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/region.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/region.h"
#elif defined(__WXGTK__)
#include "wx/gtk/region.h"
#elif defined(__WXQT__)
#include "wx/qt/region.h"
#elif defined(__WXMAC__)
#include "wx/mac/region.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/region.h"
#endif

#endif
    // _WX_REGION_H_BASE_
