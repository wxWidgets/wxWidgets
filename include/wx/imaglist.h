#ifndef __IMAGLISTH_BASE__
#define __IMAGLISTH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/imaglist.h"
#elif defined(__MOTIF__)
#include "wx/generic/imaglist.h"
#elif defined(__GTK__)
#include "wx/generic/imaglist.h"
#endif

#endif
    // __IMAGLISTH_BASE__
