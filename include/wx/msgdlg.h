#ifndef _WX_MSGDLG_H_BASE_
#define _WX_MSGDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/msgdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/msgdlg.h"
#elif defined(__WXQT__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/msgdlg.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/msgdlgg.h"
#endif

#endif
    // _WX_MSGDLG_H_BASE_
