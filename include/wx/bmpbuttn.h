#ifndef _WX_BMPBUTTON_H_BASE_
#define _WX_BMPBUTTON_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/bmpbuttn.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/bmpbuttn.h"
#elif defined(__WXGTK__)
#include "wx/gtk/bmpbuttn.h"
#elif defined(__WXQT__)
#include "wx/qt/bmpbuttn.h"
#elif defined(__WXMAC__)
#include "wx/mac/bmpbuttn.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/bmpbuttn.h"
#endif

#endif
