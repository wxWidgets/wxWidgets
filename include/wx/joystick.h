#ifndef __JOYSTICKH_BASE__
#define __JOYSTICKH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/joystick.h"
#elif defined(__WXMOTIF__)
#elif defined(__WXGTK__)
#include "wx/gtk/joystick.h"
#endif

#endif
    // __JOYSTICKH_BASE__
