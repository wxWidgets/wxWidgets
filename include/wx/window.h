#ifndef _WX_WINDOW_H_BASE_
#define _WX_WINDOW_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/window.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/window.h"
#elif defined(__WXGTK__)
#include "wx/gtk/window.h"
#elif defined(__WXQT__)
#include "wx/qt/window.h"
#elif defined(__WXMAC__)
#include "wx/mac/window.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/window.h"
#endif

#endif
    // _WX_WINDOW_H_BASE_
