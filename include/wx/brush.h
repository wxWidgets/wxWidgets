#ifndef _WX_BRUSH_H_BASE_
#define _WX_BRUSH_H_BASE_

#if defined(__WXMSW__)
    #include "wx/msw/brush.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
    #include "wx/x11/brush.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/brush.h"
#elif defined(__WXMGL__)
    #include "wx/mgl/brush.h"
#elif defined(__WXMAC__)
    #include "wx/mac/brush.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/brush.h"
#elif defined(__WXPM__)
    #include "wx/os2/brush.h"
#endif

#endif
    // _WX_BRUSH_H_BASE_
