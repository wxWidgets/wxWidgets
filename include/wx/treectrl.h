#ifndef __TREECTRLH_BASE__
#define __TREECTRLH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/treectrl.h"
#elif defined(__MOTIF__)
#include "wx/generic/treectrl.h"
#elif defined(__GTK__)
#include "wx/generic/treectrl.h"
#endif

#endif
    // __TREECTRLH_BASE__
