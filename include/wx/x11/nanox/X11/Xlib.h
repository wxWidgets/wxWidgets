/*
 * Xlib compatibility
 */

#include "XtoNX.h"

/* Data types */

typedef GR_PALETTE Colormap;
typedef GR_DRAW_ID Drawable ;
typedef int Status;

#define Success 0
#define GrabSuccess Success
#define GrabNotViewable (Success+1)
#define InputOutput 1
#define InputOnly 2
#define IsUnmapped              0
#define IsUnviewable            1
#define IsViewable              2

typedef struct {
     int x, y;		      /* location of window */
     int width, height;       /* width and height of window */
     int border_width;	      /* border width of window */
     int depth; 	      /* depth of window */
     Visual *visual;	      /* the associated visual structure */
     Window root;	      /* root of screen containing window */
     int class; 	      /* InputOutput, InputOnly*/
     int bit_gravity;	      /* one of the bit gravity values */
     int win_gravity;	      /* one of the window gravity values */
     int backing_store;       /* NotUseful, WhenMapped, Always */
     unsigned long backing_planes;/* planes to be preserved if possible */
     unsigned long backing_pixel;/* value to be used when restoring planes */
     Bool save_under;	      /* boolean, should bits under be saved? */
     Colormap colormap;       /* color map to be associated with window */
     Bool map_installed;      /* boolean, is color map currently installed*/
     int map_state;	      /* IsUnmapped, IsUnviewable, IsViewable */
     long all_event_masks;    /* set of events all people have interest in*/
     long your_event_mask;    /* my event mask */
     long do_not_propagate_mask;/* set of events that should not propagate */
     Bool override_redirect;  /* boolean value for override-redirect */
     Screen *screen;	      /* back pointer to correct screen */
} XWindowAttributes;

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

Colormap DefaultColormapOfScreen(Screen /* screen */) ;
int XSetGraphicsExposures( Display* /* display */, GC /* gc */, Bool /* graphics_exposures */) ;
int XWarpPointer( Display* /* display */, Window /* srcW */, Window /* srcW */,
                 int /* srcX */, int /* srcY */,
                 unsigned int /* srcWidth */,
                 unsigned int /* srcHeight */,
                 int destX, int destY);
int XSetInputFocus(Display* /* display */, Window focus, int /* revert_to */, Time /* time */) ;
int XGetInputFocus(Display* /* display */, Window* /* focus_return */, int* /* revert_to_return */) ;
int XGrabPointer(Display* /* display */, Window /* grab_window */,
                 Bool /* owner_events */, unsigned int /* event_mask */,
                 int /* pointer_mode */, int /* keyboard_mode */,
                 Window /* confine_to */, Cursor /* cursor */, Time /* time */) ;
int XUngrabPointer(Display /* display */, Time /* time */) ;
int XCopyArea(Display* /* display */, Drawable src, Drawable dest, GC gc,
              int src_x, int src_y, unsigned int width, unsigned int height,
              int dest_x, int dest_y) ;
int XCopyPlane(Display* /* display */, Drawable src, Drawable dest, GC gc,
              int src_x, int src_y, unsigned int width, unsigned int height,
              int dest_x, int dest_y, unsigned long /* plane */) ;

#ifdef __cpluplus
}
#endif

#define XGetMaxRequestSize(display) 16384

