#ifndef __BMPBUTTONH_BASE__
#define __BMPBUTTONH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/bmpbuttn.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/bmpbuttn.h"
#elif defined(__WXGTK__)
#include "wx/gtk/bmpbuttn.h"
#elif defined(__WXQT__)
#include "wx/qt/bmpbuttn.h"
#endif

#endif
