#ifndef _WX_JOYSTICK_H_BASE_
#define _WX_JOYSTICK_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/joystick.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/joystick.h"
#elif defined(__WXGTK__)
#include "wx/gtk/joystick.h"
#elif defined(__WXMAC__)
#include "wx/mac/joystick.h"
#elif defined(__WXPM__)
#include "wx/os2/joystick.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/joystick.h"
#endif

#endif
    // _WX_JOYSTICK_H_BASE_
