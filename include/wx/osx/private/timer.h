/* common warning snippet for all osx direct includes */

#ifndef wxOSX_USE_CARBON
#error "this files should only be included after platform.h was included
#endif

#if wxOSX_USE_COCOA_OR_IPHONE
    // this should become the future version also for carbon
    #include "wx/osx/core/private/timer.h"
#elif wxOSX_USE_CARBON
    #include "wx/osx/carbon/private/timer.h"
#endif
