#ifndef __HELPH_BASE__
#define __HELPH_BASE__

#ifdef __WINDOWS__
#include "wx/msw/helpwin.h"
#elif defined(__GTK__)
#include "wx/generic/helphtml.h"
#else
#include "wx/generic/helpxlp.h"
#endif

#ifdef __WINDOWS__
#define wxHelpController wxWinHelpController
#elif defined(__GTK__)
#define wxHelpController wxHTMLHelpController
#else
#define wxHelpController wxXLPHelpController
#endif

#endif
    // __HELPH_BASE__
