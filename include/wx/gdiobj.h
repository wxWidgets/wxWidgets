#ifndef _WX_GDIOBJ_H_BASE_
#define _WX_GDIOBJ_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/gdiobj.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/gdiobj.h"
#elif defined(__WXGTK__)
#include "wx/gtk/gdiobj.h"
#elif defined(__WXX11__)
#include "wx/x11/gdiobj.h"
#elif defined(__WXMGL__)
#include "wx/mgl/gdiobj.h"
#elif defined(__WXMAC__)
#include "wx/mac/gdiobj.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/gdiobj.h"
#elif defined(__WXPM__)
#include "wx/os2/gdiobj.h"
#endif

#endif
    // _WX_GDIOBJ_H_BASE_
