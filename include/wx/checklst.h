#ifndef _WX_CHECKLST_H_BASE_
#define _WX_CHECKLST_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/checklst.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/checklst.h"
#elif defined(__WXGTK__)
#include "wx/gtk/checklst.h"
#elif defined(__WXQT__)
#include "wx/qt/checklst.h"
#elif defined(__WXMAC__)
#include "wx/mac/checklst.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/checklst.h"
#endif

#endif
    // _WX_CHECKLST_H_BASE_
