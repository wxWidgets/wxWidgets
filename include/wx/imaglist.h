#ifndef __IMAGLISTH_BASE__
#define __IMAGLISTH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/imaglist.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/imaglist.h"
#elif defined(__WXGTK__)
#include "wx/generic/imaglist.h"
#elif defined(__WXQT__)
#include "wx/generic/imaglist.h"
#endif

#endif
    // __IMAGLISTH_BASE__
