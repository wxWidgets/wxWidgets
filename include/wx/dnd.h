#ifndef __DNDH_BASE__
#define __DNDH_BASE__

#if defined(__WINDOWS__)
#elif defined(__MOTIF__)
#elif defined(__GTK__)
#include "wx/gtk/dnd.h"
#endif

#endif
    // __DNDH_BASE__
