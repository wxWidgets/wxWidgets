#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#include "wx/helpbase.h"

#ifdef __WXMSW__
    #include "wx/msw/helpwin.h"

    #define wxHelpController wxWinHelpController
    #define sm_classwxHelpController sm_classwxWinHelpController
#else // !MSW
    #include "wx/generic/helpext.h"

    #define wxHelpController wxExtHelpController
    #define sm_classwxHelpController sm_classwxExtHelpController
#endif // MSW/!MSW

#endif
    // _WX_HELP_H_BASE_
