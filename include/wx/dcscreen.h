#ifndef _WX_DCSCREEN_H_BASE_
#define _WX_DCSCREEN_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dcscreen.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dcscreen.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcscreen.h"
#elif defined(__WXX11__)
#include "wx/x11/dcscreen.h"
#elif defined(__WXMGL__)
#include "wx/mgl/dcscreen.h"
#elif defined(__WXMAC__)
#include "wx/mac/dcscreen.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/dcscreen.h"
#elif defined(__WXPM__)
#include "wx/os2/dcscreen.h"
#endif

#endif
    // _WX_DCSCREEN_H_BASE_
