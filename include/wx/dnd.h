#ifndef _WX_DND_H_BASE_
#define _WX_DND_H_BASE_

#if defined(__WXMSW__)
#include "wx/dataobj.h"
#include "wx/msw/ole/dropsrc.h"
#include "wx/msw/ole/droptgt.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dnd.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dnd.h"
#elif defined(__WXQT__)
#include "wx/qt/dnd.h"
#elif defined(__WXMAC__)
#include "wx/mac/dnd.h"
#elif defined(__WXPM__)
#include "wx/os2/dnd.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dnd.h"
#endif

#endif
    // _WX_DND_H_BASE_
