#ifndef __NOTEBOOKH_BASE__
#define __NOTEBOOKH_BASE__

#if defined(__WXMSW__)
  #include  "wx/msw/notebook.h"
#elif defined(__WXMOTIF__)
  #error "wxNotebook not implemented under Motif"
#elif defined(__WXGTK__)
  #include  "wx/gtk/notebook.h"
#elif defined(__WXQT__)
  #include  "wx/qt/notebook.h"
#endif

#endif
    // __NOTEBOOKH_BASE__
