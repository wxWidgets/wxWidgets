/*
 * nanox.c
 *
 * Replacements for some comomon Xlib functions
 */

#include "wx/setup.h"

#if wxUSE_NANOX

#include "wx/x11/nanox/X11/Xlib.h"

Colormap DefaultColormapOfScreen(Screen /* screen */)
{
    static Colormap s_globalColormap;
    static bool s_init = FALSE;

    if (!s_init)
    {
        GrGetSystemPalette(& s_globalColormap);
        s_init = TRUE;
    }

    return s_globalColormap;
}

#endif
  /* wxUSE_NANOX */