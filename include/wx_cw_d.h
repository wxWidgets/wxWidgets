#if __option (profile)    
    #error "profiling is not supported for debug targets"
#endif

#undef WX_PRECOMP

#define WIN32
#define _DEBUG
#define _WINDOWS
#define __WINDOWS__
#define __WXMSW__
#define __WXDEBUG__
#define WXDEBUG            1
#define __WIN95__
#define __WIN32__
#define WINVER            0x0400

#include "wx/wx_cw_cm.h"
#ifdef __WXMSW__
    #include <windows.h>
    #include "wx/msw/winundef.h"
#endif
