#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#include "wx/helpbase.h"

#ifdef __WXMSW__
    #include "wx/msw/helpwin.h"

    #undef wxHelpController
    #undef sm_classwxHelpController
    #define wxHelpController wxWinHelpController
    #define sm_classwxHelpController sm_classwxWinHelpController
#else // !MSW
    #include "wx/helpbase.h"
#endif // MSW/!MSW

#endif
    // _WX_HELP_H_BASE_
