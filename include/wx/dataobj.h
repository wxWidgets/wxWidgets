#ifndef _WX_DATAOBJ_H_BASE_
#define _WX_DATAOBJ_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/ole/dataobj.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dataobj.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dataobj.h"
#elif defined(__WXQT__)
#include "wx/qt/dnd.h"
#elif defined(__WXMAC__)
#include "wx/mac/dnd.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dnd.h"
#endif

#endif
    // _WX_DATAOBJ_H_BASE_
