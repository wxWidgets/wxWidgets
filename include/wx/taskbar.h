#ifndef _WX_TASKBAR_H_BASE_
#define _WX_TASKBAR_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/taskbar.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/taskbar.h"
#elif defined(__WXGTK__)
#elif defined(__WXQT__)
#elif defined(__WXMAC__)
#elif defined(__WXSTUBS__)
#include "wx/stubs/taskbar.h"
#endif

#endif
    // _WX_TASKBAR_H_BASE_
