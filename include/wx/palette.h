#ifndef _WX_PALETTE_H_BASE_
#define _WX_PALETTE_H_BASE_

#if wxUSE_PALETTE

#if defined(__WXMSW__)
    #include "wx/msw/palette.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/palette.h"
#elif defined(__WXGTK__)
    #include "wx/generic/paletteg.h"
#elif defined(__WXMGL__)
    #include "wx/mgl/palette.h"
#elif defined(__WXMAC__)
    #include "wx/mac/palette.h"
#elif defined(__WXPM__)
    #include "wx/os2/palette.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/palette.h"
#endif

#endif // wxUSE_PALETTE

#endif
    // _WX_PALETTE_H_BASE_
