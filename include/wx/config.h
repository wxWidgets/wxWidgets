#ifndef _WX_CONFIG_H_BASE_
#define _WX_CONFIG_H_BASE_

#include "wx/confbase.h"

#if defined(__WXMSW__) && wxUSE_CONFIG_NATIVE
#  ifdef __WIN32__
#    include "wx/msw/regconf.h"
#else
#    include "wx/msw/iniconf.h"
#  endif
#else
#    include "wx/fileconf.h"
#endif

#endif
    // _WX_CONFIG_H_BASE_
