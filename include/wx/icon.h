#ifndef _WX_ICON_H_BASE_
#define _WX_ICON_H_BASE_

// this is for Unix (i.e. now for anything other than MSW)
#undef  wxICON
#define wxICON(icon_name)   wxIcon(icon_name##_xpm)

#if defined(__WXMSW__)
  // under Windows, icon name is the ressource id (string)
  #undef wxICON
  #define wxICON(icon_name)   wxIcon(#icon_name)

  #include "wx/msw/icon.h"
#elif defined(__WXMOTIF__)
  #include "wx/motif/icon.h"
#elif defined(__WXGTK__)
  #include "wx/gtk/icon.h"
#elif defined(__WXQT__)
  #include "wx/qt/icon.h"
#elif defined(__WXMAC__)
  #include "wx/mac/icon.h"
#elif defined(__WXSTUBS__)
  #include "wx/stubs/icon.h"
#endif

#endif
    // _WX_ICON_H_BASE_
