#ifndef _WX_MMEDIA_DEFS_H_
#define _WX_MMEDIA_DEFS_H_

#ifdef WXMAKINGDLL_MMEDIA
    #define WXDLLIMPEXP_MMEDIA WXEXPORT
    #define WXDLLIMPEXP_DATA_MMEDIA(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MMEDIA WXIMPORT
    #define WXDLLIMPEXP_DATA_MMEDIA(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_MMEDIA
    #define WXDLLIMPEXP_DATA_MMEDIA(type) type
#endif

#endif // _WX_MMEDIA_DEFS_H_
