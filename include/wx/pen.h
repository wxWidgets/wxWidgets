#ifndef _WX_PEN_H_BASE_
#define _WX_PEN_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/pen.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/pen.h"
#elif defined(__WXGTK__)
#include "wx/gtk/pen.h"
#elif defined(__WXQT__)
#include "wx/qt/pen.h"
#elif defined(__WXMAC__)
#include "wx/mac/pen.h"
#elif defined(__WXPM__)
#include "wx/os2/pen.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/pen.h"
#endif

#endif
    // _WX_PEN_H_BASE_
