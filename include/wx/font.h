#ifndef _WX_FONT_H_BASE_
#define _WX_FONT_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/font.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/font.h"
#elif defined(__WXGTK__)
#include "wx/gtk/font.h"
#elif defined(__WXQT__)
#include "wx/qt/font.h"
#elif defined(__WXMAC__)
#include "wx/mac/font.h"
#elif defined(__WXPM__)
#include "wx/os2/font.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/font.h"
#endif

#endif
    // _WX_FONT_H_BASE_
