#ifndef __NOTEBOOKH_BASE__
#define __NOTEBOOKH_BASE__

#if defined(__WINDOWS__)
  #include  "wx/msw/notebook.h"
#elif defined(__MOTIF__)
  #error "wxNotebook not implemented under Motif"
#elif defined(__GTK__)
  #include  "wx/gtk/notebook.h"
#endif

#endif
    // __NOTEBOOKH_BASE__
