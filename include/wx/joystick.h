#ifndef __JOYSTICKH_BASE__
#define __JOYSTICKH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/joystick.h"
#elif defined(__MOTIF__)
#elif defined(__GTK__)
#include "wx/gtk/joystick.h"
#endif

#endif
    // __JOYSTICKH_BASE__
