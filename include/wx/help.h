#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#ifdef __WXMSW__
#include "wx/msw/helpwin.h"
#else
#include "wx/generic/helpext.h"
#endif

#ifdef __WXMSW__
#define wxHelpController wxWinHelpController
#define sm_classwxHelpController sm_classwxWinHelpController
#else
#define wxHelpController wxExtHelpController
#define sm_classwxHelpController sm_classwxExtHelpController
#endif

#endif
    // _WX_HELP_H_BASE_
