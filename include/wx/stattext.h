#ifndef _WX_STATTEXT_H_BASE_
#define _WX_STATTEXT_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/stattext.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/stattext.h"
#elif defined(__WXGTK__)
#include "wx/gtk/stattext.h"
#elif defined(__WXQT__)
#include "wx/qt/stattext.h"
#elif defined(__WXMAC__)
#include "wx/mac/stattext.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/stattext.h"
#endif

#endif
    // _WX_STATTEXT_H_BASE_
