#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#if wxUSE_HELP

#include "wx/helpbase.h"

#ifdef __WXMSW__
    #include "wx/msw/helpwin.h"

    #define wxHelpController wxWinHelpController
    #define sm_classwxHelpController sm_classwxWinHelpController
#else // !MSW

#if wxUSE_WXHTML_HELP
    #include "wx/html/helpctrl.h"
    #define wxHelpController wxHtmlHelpController
    #define sm_classwxHelpController sm_classwxHtmlHelpController
#else
    #include "wx/generic/helpext.h"
    #define wxHelpController wxExtHelpController
    #define sm_classwxHelpController sm_classwxExtHelpController
#endif

#endif // MSW/!MSW

#endif // wxUSE_HELP

#endif
    // _WX_HELP_H_BASE_
