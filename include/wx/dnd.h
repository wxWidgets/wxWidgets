#ifndef __DNDH_BASE__
#define __DNDH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/ole/dropsrc.h"
#include "wx/msw/ole/droptgt.h"
#include "wx/msw/ole/dataobj.h"
#elif defined(__WXMOTIF__)
#elif defined(__WXGTK__)
#include "wx/gtk/dnd.h"
#endif

#endif
    // __DNDH_BASE__
