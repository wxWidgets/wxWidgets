#ifndef _WX_DIALOG_H_BASE_
#define _WX_DIALOG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/dialog.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/dialog.h"
#elif defined(__WXGTK__)
#include "wx/gtk/dialog.h"
#elif defined(__WXQT__)
#include "wx/qt/dialog.h"
#elif defined(__WXMAC__)
#include "wx/mac/dialog.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/dialog.h"
#endif

#endif
    // _WX_DIALOG_H_BASE_
