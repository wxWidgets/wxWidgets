#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#if wxUSE_HELP

#include "wx/helpbase.h"

#ifdef __WXWINCE__
    #include "wx/msw/wince/helpwce.h"

    #define wxHelpController wxWinceHelpController
#elif defined(__WXMSW__)
    #include "wx/msw/helpwin.h"

    #define wxHelpController wxWinHelpController
#else // !MSW

#if wxUSE_WXHTML_HELP
    #include "wx/html/helpctrl.h"
    #define wxHelpController wxHtmlHelpController
#else
    #include "wx/generic/helpext.h"
    #define wxHelpController wxExtHelpController
#endif

#endif // MSW/!MSW

#endif // wxUSE_HELP

#endif
    // _WX_HELP_H_BASE_
