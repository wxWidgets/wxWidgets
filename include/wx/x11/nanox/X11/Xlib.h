/*
 * Xlib compatibility
 */

#ifndef _DUMMY_XLIBH_
#define _DUMMY_XLIBH_

#include <XtoNX.h>

/* Data types */

typedef GR_PALETTE* Colormap;
typedef GR_DRAW_ID Drawable ;
typedef int Status;
typedef unsigned long VisualID;
typedef int Bool;
typedef long XID;
typedef XID KeySym;
/* typedef unsigned long Time; */

#define Success 0
#define GrabSuccess Success
#define GrabNotViewable (Success+1)
#define InputOutput 1
#define InputOnly 2
#define IsUnmapped              0
#define IsUnviewable            1
#define IsViewable              2
/* Is this right? */
#define PropertyChangeMask GR_EVENT_MASK_SELECTION_CHANGED
#define GraphicsExpose GR_EVENT_TYPE_EXPOSURE
#define GraphicsExposeMask GR_EVENT_MASK_EXPOSURE

#define XSynchronize(display,sync)
#define XDefaultRootWindow(d) GR_ROOT_WINDOW_ID
#define XFreePixmap(d, p) GrDestroyWindow(p)
#define XFreeCursor(d, c) GrDestroyCursor(c)
#define DefaultVisual(d, s) ((Visual*) NULL)

/* These defines are wrongly defined in XtoNX.h, IMHO,
 * since they reference a static global.
 * Redefined as functions, below.
 */

#undef DisplayWidth
#undef DisplayHeight
#undef DefaultDepth

/*
 * Data structure used by color operations
 */
typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} XColor;

typedef struct {
	int type;
	Display *display;	/* Display the event was read from */
	XID resourceid;		/* resource id */
	unsigned long serial;	/* serial number of failed request */
	unsigned char error_code;	/* error code of failed request */
	unsigned char request_code;	/* Major op-code of failed request */
	unsigned char minor_code;	/* Minor op-code of failed request */
} XErrorEvent;

/*
 * Visual structure; contains information about colormapping possible.
 */
typedef struct {
	void *ext_data;	/* hook for extension to hang data */
	VisualID visualid;	/* visual id of this visual */
#if defined(__cplusplus) || defined(c_plusplus)
	int c_class;		/* C++ class of screen (monochrome, etc.) */
#else
	int class;		/* class of screen (monochrome, etc.) */
#endif
	unsigned long red_mask, green_mask, blue_mask;	/* mask values */
	int bits_per_rgb;	/* log base 2 of distinct color values */
	int map_entries;	/* color map entries */
} Visual;

/*
 * Depth structure; contains information for each possible depth.
 */	
typedef struct {
	int depth;		/* this depth (Z) of the depth */
	int nvisuals;		/* number of Visual types at this depth */
	Visual *visuals;	/* list of visuals possible at this depth */
} Depth;

/*
 * Information about the screen.  The contents of this structure are
 * implementation dependent.  A Screen should be treated as opaque
 * by application code.
 */

struct _XDisplay;		/* Forward declare before use for C++ */

typedef struct {
	void *ext_data;	/* hook for extension to hang data */
	struct _XDisplay *display;/* back pointer to display structure */
	Window root;		/* Root window id. */
	int width, height;	/* width and height of screen */
	int mwidth, mheight;	/* width and height of  in millimeters */
	int ndepths;		/* number of depths possible */
	Depth *depths;		/* list of allowable depths on the screen */
	int root_depth;		/* bits per pixel */
	Visual *root_visual;	/* root visual */
	GC default_gc;		/* GC for the root root visual */
	Colormap cmap;		/* default color map */
	unsigned long white_pixel;
	unsigned long black_pixel;	/* White and Black pixel values */
	int max_maps, min_maps;	/* max and min color maps */
	int backing_store;	/* Never, WhenMapped, Always */
	Bool save_unders;	
	long root_input_mask;	/* initial root input mask */
} Screen;


typedef struct {
     int x, y;		      /* location of window */
     int width, height;       /* width and height of window */
     int border_width;	      /* border width of window */
     int depth; 	      /* depth of window */
     Visual *visual;	      /* the associated visual structure */
     Window root;	      /* root of screen containing window */
     int _class; 	      /* InputOutput, InputOnly*/
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

typedef int (*XErrorHandler) (	    /* WARNING, this type not in Xlib spec */
    Display*		/* display */,
    XErrorEvent*	/* error_event */
);

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

Colormap DefaultColormapOfScreen(Screen* /* screen */) ;
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

XErrorHandler XSetErrorHandler (XErrorHandler /* handler */);
Display *XOpenDisplay(const char *name);
Screen *XScreenOfDisplay(Display* /* display */,
                         int /* screen_number */);
int DisplayWidth(Display* /* display */, int /* screen */);
int DisplayHeight(Display* /* display */, int /* screen */);
int DefaultDepth(Display* /* display */, int /* screen */);
int XAllocColor(Display* /* display */, Colormap /* cmap */,
                XColor* color);
int XParseColor(Display* display, Colormap cmap,
                const char* cname, XColor* color);

#ifdef __cpluplus
}
#endif

#define XMaxRequestSize(display) 16384

#endif
  /* _DUMMY_XLIBH_ */
