#ifndef _WX_DCMEMORY_H_BASE_
#define _WX_DCMEMORY_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dcmemory.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dcmemory.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dcmemory.h"
#elif defined(__WXMGL__)
#include "wx/mgl/dcmemory.h"
#elif defined(__WXQT__)
#include "wx/qt/dcmemory.h"
#elif defined(__WXMAC__)
#include "wx/mac/dcmemory.h"
#elif defined(__WXPM__)
#include "wx/os2/dcmemory.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dcmemory.h"
#endif

#endif
    // _WX_DCMEMORY_H_BASE_
