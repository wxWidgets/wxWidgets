#ifndef _WX_ACCEL_H_BASE_
#define _WX_ACCEL_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/accel.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/accel.h"
#elif defined(__WXGTK__)
#include "wx/gtk/accel.h"
#elif defined(__WXQT__)
#include "wx/qt/accel.h"
#elif defined(__WXMAC__)
#include "wx/mac/accel.h"
#elif defined(__WXPM__)
#include "wx/os2/accel.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/accel.h"
#endif

#endif
    // _WX_ACCEL_H_BASE_
