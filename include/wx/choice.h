#ifndef __CHOICEH_BASE__
#define __CHOICEH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/choice.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/choice.h"
#elif defined(__WXGTK__)
#include "wx/gtk/choice.h"
#elif defined(__WXQT__)
#include "wx/qt/choice.h"
#endif

#endif
    // __CHOICEH_BASE__
