#ifndef _WX_GDIOBJ_H_BASE_
#define _WX_GDIOBJ_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/gdiobj.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/gdiobj.h"
#elif defined(__WXGTK__)
#include "wx/gtk/gdiobj.h"
#elif defined(__WXQT__)
#include "wx/qt/gdiobj.h"
#elif defined(__WXMAC__)
#include "wx/mac/gdiobj.h"
#elif defined(__WXPM__)
#include "wx/os2/gdiobj.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/gdiobj.h"
#endif

#endif
    // _WX_GDIOBJ_H_BASE_
