#ifndef __FRAMEH_BASE__
#define __FRAMEH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/frame.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/frame.h"
#elif defined(__WXGTK__)
#include "wx/gtk/frame.h"
#elif defined(__WXQT__)
#include "wx/qt/frame.h"
#endif

#endif
    // __FRAMEH_BASE__
