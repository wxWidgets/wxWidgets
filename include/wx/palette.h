#ifndef _WX_PALETTE_H_BASE_
#define _WX_PALETTE_H_BASE_

// include it to get wxUSE_PALETTE value
#include "wx/setup.h"

#if wxUSE_PALETTE

#if defined(__WXMSW__)
    #include "wx/msw/palette.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/palette.h"
#elif defined(__WXGTK__) || defined(__WXCOCOA__)
    #include "wx/generic/paletteg.h"
#elif defined(__WXX11__)
    #include "wx/x11/palette.h"
#elif defined(__WXMGL__)
    #include "wx/mgl/palette.h"
#elif defined(__WXMAC__)
    #include "wx/mac/palette.h"
#elif defined(__WXPM__)
    #include "wx/os2/palette.h"
#endif

#endif // wxUSE_PALETTE

#endif
    // _WX_PALETTE_H_BASE_
