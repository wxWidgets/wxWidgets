/*
 * Xlib compatibility
 */

#include "XtoNX.h"

/* Data types */

typedef GR_PALETTE Colormap;

/* events*/

/* What should this be? */
#if 0
#ifndef ResizeRequest
#define ResizeRequest ??
#endif
#endif

#ifndef MotionNotify
#define MotionNotify GR_EVENT_TYPE_MOUSE_POSITION
#define PointerMotionMask GR_EVENT_MASK_MOUSE_POSITION
#endif

#ifndef FocusIn
#define FocusIn GR_EVENT_TYPE_FOCUS_IN
#define FocusOut GR_EVENT_TYPE_FOCUS_OUT
#define FocusChangeMask GR_EVENT_MASK_FOCUS_IN|GR_EVENT_MASK_FOCUS_OUT
#endif

/* Fuunctions */

#ifdef __cpluplus
extern "C" {
#endif

Colormap DefaultColormapOfScreen(Screen /* screen */);

#ifdef __cpluplus
}
#endif

#define XGetMaxRequestSize(display) 16384

