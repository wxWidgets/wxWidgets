#ifndef __BMPBUTTONH_BASE__
#define __BMPBUTTONH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/bmpbuttn.h"
#elif defined(__MOTIF__)
#include "wx/xt/bmpbuttn.h"
#elif defined(__GTK__)
#include "wx/gtk/bmpbuttn.h"
#endif

#endif
