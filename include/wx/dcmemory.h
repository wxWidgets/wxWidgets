#ifndef __DCMEMORYH_BASE__
#define __DCMEMORYH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/dcmemory.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/dcmemory.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcmemory.h"
#endif

#endif
    // __DCMEMORYH_BASE__
