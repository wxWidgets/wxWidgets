#ifndef _WX_REGION_H_BASE_
#define _WX_REGION_H_BASE_

#include "wx/colour.h"

#if defined(__PALMOS__)
    #include "wx/palmos/region.h"
#elif defined(__WXMSW__)
    #include "wx/msw/region.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/region.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
    #include "wx/x11/region.h"
#elif defined(__WXMGL__)
    #include "wx/mgl/region.h"
#elif defined(__WXMAC__)
    #include "wx/mac/region.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/region.h"
#elif defined(__WXPM__)
    #include "wx/os2/region.h"
#endif

#endif
    // _WX_REGION_H_BASE_
