#ifndef _WX_CONTROL_H_BASE_
#define _WX_CONTROL_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/control.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/control.h"
#elif defined(__WXGTK__)
#include "wx/gtk/control.h"
#elif defined(__WXQT__)
#include "wx/qt/control.h"
#elif defined(__WXMAC__)
#include "wx/mac/control.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/control.h"
#endif

#endif
    // _WX_CONTROL_H_BASE_
