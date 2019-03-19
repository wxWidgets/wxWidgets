#ifndef _WX_PRIVATE_OSX_H_
#define _WX_PRIVATE_OSX_H_

#include "wx/osx/core/private.h"
#include "wx/osx/private/datatransfer.h"

#if wxOSX_USE_IPHONE
    #include "wx/osx/iphone/private.h"
#elif wxOSX_USE_COCOA
    #include "wx/osx/cocoa/private.h"
#elif wxUSE_GUI
    #error "Must include wx/defs.h first"
#endif

#endif
