#ifndef __HELPH_BASE__
#define __HELPH_BASE__

#ifdef __WINDOWS__
#include "wx/msw/helpwin.h"
#elif defined(__GTK__)
#include "wx/generic/helphtml.h"
#else
#include "wx/generic/helpxlp.h"
#endif

#endif
    // __HELPH_BASE__
