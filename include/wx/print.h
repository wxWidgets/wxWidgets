#ifndef _WX_PRINT_H_BASE_
#define _WX_PRINT_H_BASE_

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

#include "wx/msw/printwin.h"

#elif defined(__WXMAC__)

#include "wx/mac/printmac.h"

#else

#include "wx/generic/printps.h"

#endif


#endif
    // _WX_PRINT_H_BASE_
