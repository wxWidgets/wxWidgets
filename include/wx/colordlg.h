#ifndef _WX_COLORDLG_H_BASE_
#define _WX_COLORDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/colordlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXGTK__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXQT__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXMAC__)
#include "wx/generic/colrdlgg.h"
#elif defined(__WXSTUBS__)
#include "wx/generic/colrdlgg.h"
#endif

#endif
    // _WX_COLORDLG_H_BASE_
