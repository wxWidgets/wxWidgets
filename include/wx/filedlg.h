#ifndef _WX_FILEDLG_H_BASE_
#define _WX_FILEDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/filedlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/filedlg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/filedlg.h"
#elif defined(__WXQT__)
#include "wx/qt/filedlg.h"
#elif defined(__WXMAC__)
#include "wx/mac/filedlg.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/filedlg.h"
#endif


#endif
    // _WX_FILEDLG_H_BASE_
