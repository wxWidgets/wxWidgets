#ifndef _WX_CHECKBOX_H_BASE_
#define _WX_CHECKBOX_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/checkbox.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/checkbox.h"
#elif defined(__WXGTK__)
#include "wx/gtk/checkbox.h"
#elif defined(__WXQT__)
#include "wx/qt/checkbox.h"
#elif defined(__WXMAC__)
#include "wx/mac/checkbox.h"
#elif defined(__WXPM__)
#include "wx/os2/checkbox.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/checkbox.h"
#endif

#endif
    // _WX_CHECKBOX_H_BASE_
