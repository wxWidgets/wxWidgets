#ifndef _WX_PRIVATE_OSX_H_
#define _WX_PRIVATE_OSX_H_

#include "wx/osx/core/private.h"

#ifdef __WXOSX_IPHONE__
    #include "wx/osx/iphone/private.h"
#elif defined(__WXOSX_COCOA__)
    #include "wx/osx/cocoa/private.h"
#elif wxUSE_GUI && defined(__WXOSX__)
    #error "Must include wx/defs.h first"
#else
    #include <ApplicationServices/ApplicationServices.h>

    #ifdef __OBJC__
        #import <Cocoa/Cocoa.h>
    #endif
#endif

#endif
