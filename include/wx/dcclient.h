#ifndef _WX_DCCLIENT_H_BASE_
#define _WX_DCCLIENT_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dcclient.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dcclient.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcclient.h"
#elif defined(__WXMGL__)
#include "wx/mgl/dcclient.h"
#elif defined(__WXQT__)
#include "wx/qt/dcclient.h"
#elif defined(__WXMAC__)
#include "wx/mac/dcclient.h"
#elif defined(__WXPM__)
#include "wx/os2/dcclient.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dcclient.h"
#endif

#endif
    // _WX_DCCLIENT_H_BASE_
