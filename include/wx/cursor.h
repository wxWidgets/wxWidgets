#ifndef _WX_CURSOR_H_BASE_
#define _WX_CURSOR_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/cursor.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/cursor.h"
#elif defined(__WXGTK__)
#include "wx/gtk/cursor.h"
#elif defined(__WXQT__)
#include "wx/qt/cursor.h"
#elif defined(__WXMAC__)
#include "wx/mac/cursor.h"
#elif defined(__WXPM__)
#include "wx/os2/cursor.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/cursor.h"
#endif

#endif
    // _WX_CURSOR_H_BASE_
