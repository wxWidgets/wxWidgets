#ifndef _WX_GLCANVAS_H_BASE_
#define _WX_GLCANVAS_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/glcanvas.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/glcanvas.h"
#elif defined(__WXGTK__)
#include "wx/gtk/glcanvas.h"
#elif defined(__WXQT__)
#include "wx/qt/glcanvas.h"
#elif defined(__WXMAC__)
#include "wx/mac/glcanvas.h"
#elif defined(__WXPM__)
#include "wx/os2/glcanvas.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/glcanvas.h"
#endif

#endif
    // _WX_GLCANVAS_H_BASE_
