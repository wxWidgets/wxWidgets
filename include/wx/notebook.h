#ifndef __NOTEBOOKH_BASE__
#define __NOTEBOOKH_BASE__

#if defined(__WINDOWS__)
#elif defined(__MOTIF__)
#elif defined(__GTK__)
  #include  "wx/gtk/notebook.h"
#endif

#endif
    // __NOTEBOOKH_BASE__
