#ifndef _WX_DIRDLG_H_BASE_
#define _WX_DIRDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dirdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dirdlg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dirdlg.h"
#elif defined(__WXQT__)
#include "wx/qt/dirdlg.h"
#elif defined(__WXMAC__)
#include "wx/mac/dirdlg.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dirdlg.h"
#endif

#endif
    // _WX_DIRDLG_H_BASE_
