#if __option (profile)    
    #error "profiling is not supported for debug targets"
#endif

#define WX_PRECOMP
//#define __WXDEBUG__         1
//#define wxUSE_GUI             1
//#define wxUSE_SOCKETS         1

#define WIN32
#define    _DEBUG
#define    _WINDOWS
#define    __WINDOWS__
#define    __WXMSW__
#define    __WXDEBUG__
#define    WXDEBUG            1
#define    __WIN95__
#define    __WIN32__
#define    WINVER            0x0400

#include "wx/wx_cw_cm.h"
#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#endif

