#ifndef __CHOICEH_BASE__
#define __CHOICEH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/choice.h"
#elif defined(__MOTIF__)
#include "wx/xt/choice.h"
#elif defined(__GTK__)
#include "wx/gtk/choice.h"
#endif

#endif
    // __CHOICEH_BASE__
