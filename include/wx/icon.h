#ifndef _WX_ICON_H_BASE_
#define _WX_ICON_H_BASE_

#include "wx/iconloc.h"

#if defined(__WXMSW__)
  #include "wx/msw/icon.h"
#elif defined(__WXMOTIF__)
  #include "wx/motif/icon.h"
#elif defined(__WXGTK__)
  #include "wx/gtk/icon.h"
#elif defined(__WXX11__)
  #include "wx/x11/icon.h"
#elif defined(__WXMGL__)
  #include "wx/mgl/icon.h"
#elif defined(__WXMAC__)
  #include "wx/mac/icon.h"
#elif defined(__WXCOCOA__)
  #include "wx/cocoa/icon.h"
#elif defined(__WXPM__)
  #include "wx/os2/icon.h"
#endif

#endif
    // _WX_ICON_H_BASE_
