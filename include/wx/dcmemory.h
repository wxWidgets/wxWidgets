#ifndef __DCMEMORYH_BASE__
#define __DCMEMORYH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/dcmemory.h"
#elif defined(__MOTIF__)
#include "wx/xt/dcmemory.h"
#elif defined(__GTK__)
#include "wx/gtk/dcmemory.h"
#endif

#endif
    // __DCMEMORYH_BASE__
