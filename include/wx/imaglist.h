#ifndef _WX_IMAGLIST_H_BASE_
#define _WX_IMAGLIST_H_BASE_

#if defined(__WIN32__)
    #include "wx/msw/imaglist.h"
#endif
#include "wx/generic/imaglist.h"
#if !defined(__WIN32__)
    #define wxImageList wxGenericImageList
    #define sm_classwxImageList sm_classwxGenericImageList
#endif

#endif
    // _WX_IMAGLIST_H_BASE_
