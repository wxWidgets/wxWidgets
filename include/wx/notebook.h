#ifndef _WX_NOTEBOOK_H_BASE_
#define _WX_NOTEBOOK_H_BASE_

#if defined(__WXMSW__)
#ifdef __WIN16__
  #include  "wx/generic/notebook.h"
#else
  #include  "wx/msw/notebook.h"
#endif
#elif defined(__WXMOTIF__)
  #include  "wx/generic/notebook.h"
#elif defined(__WXGTK__)
  #include  "wx/gtk/notebook.h"
#elif defined(__WXQT__)
  #include  "wx/qt/notebook.h"
#elif defined(__WXMAC__)
  #include  "wx/mac/notebook.h"
#elif defined(__WXSTUBS__)
  #include  "wx/stubs/notebook.h"
#endif

#endif
    // _WX_NOTEBOOK_H_BASE_
