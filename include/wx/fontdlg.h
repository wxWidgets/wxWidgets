#ifndef __FONTDLGH_BASE__
#define __FONTDLGH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/fontdlg.h"
#elif defined(__MOTIF__)
#include "wx/generic/fontdlgg.h"
#elif defined(__GTK__)
#include "wx/generic/fontdlgg.h"
#endif

#endif
    // __FONTDLGH_BASE__
