#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#include "wx/helpbase.h"

#ifdef __WXMSW__
    #include "wx/msw/helpwin.h"

    #define wxHelpController wxWinHelpController
    #define sm_classwxHelpController sm_classwxWinHelpController
#else // !MSW

/* By default, if wxHTML is compiled in, use the
   wxHelpControllerHtml. If not, use the external help controller.
   (of course, we shouldn't do it for wxMSW)
*/

#if wxUSE_HTML
#   include "wx/generic/helpwxht.h"
#   define wxHelpController wxHelpControllerHtml
#   define sm_classwxHelpController sm_classwxHelpControllerHtml
#else
#   include "wx/generic/helpext.h"
#   define wxHelpController wxExtHelpController
#   define sm_classwxHelpController sm_classwxExtHelpController
#endif

#endif // MSW/!MSW

#endif
    // _WX_HELP_H_BASE_
