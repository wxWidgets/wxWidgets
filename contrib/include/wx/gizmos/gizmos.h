#ifndef _WX_GIZMOS_H_
#define _WX_GIZMOS_H_

#ifdef WXMAKINGDLL_GIZMOS
    #define WXDLLIMPEXP_GIZMOS WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_GIZMOS WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_GIZMOS
#endif

#endif
