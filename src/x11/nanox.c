/*
 * nanox.c
 *
 * Replacements for some comomon Xlib functions
 */

#include "wx/setup.h"

#if wxUSE_NANOX

#include "wx/x11/nanox/X11/Xlib.h"

/* Expands to some compatibility functions (see XtoNX.h) */

STATIC_FUNCTIONS

Colormap DefaultColormapOfScreen(Screen* /* screen */)
{
    static GR_PALETTE* s_globalColormap = 0;
    static bool s_init = FALSE;

    if (!s_init)
    {
        s_globalColormap = (GR_PALETTE*) malloc(sizeof(GR_PALETTE));
       
        GrGetSystemPalette(s_globalColormap);
        s_init = TRUE;
    }

    return s_globalColormap;
}

int XSetGraphicsExposures( Display* /* display */, GC /* gc */, Bool /* graphics_exposures */)
{
    return Success ;
}

int XWarpPointer( Display* /* display */, Window /* srcW */, Window /* srcW */,
                 int /* srcX */, int /* srcY */,
                 unsigned int /* srcWidth */,
                 unsigned int /* srcHeight */,
                 int destX, int destY)
{
    GrMoveCursor(destX, destY);
    return Success;
}

int XSetInputFocus(Display* /* display */, Window focus, int /* revert_to */, Time /* time */)
{
    GrSetFocus(focus);
    return Success;
}

int XGetInputFocus(Display* /* display */, Window* /* focus_return */, int* /* revert_to_return */)
{
    * focus_return = GrGetFocus();
    * revert_to_return = 0;
    return Success;
}

int XGrabPointer(Display* /* display */, Window /* grab_window */,
                 Bool /* owner_events */, unsigned int /* event_mask */,
                 int /* pointer_mode */, int /* keyboard_mode */,
                 Window /* confine_to */, Cursor /* cursor */, Time /* time */)
{
    /* According to comments in srvevent.c in Nano-X, the pointer
     * is implicitly grabbed when a mouse button is down.
     * We may be able to simulate this further in the event loop.
     */
    return Success;
}

int XUngrabPointer(Display /* display */, Time /* time */)
{
    return Success;
}

int XCopyArea(Display* /* display */, Drawable src, Drawable dest, GC gc,
              int src_x, int src_y, unsigned int width, unsigned int height,
              int dest_x, int dest_y)
{
    GrCopyArea(dest, gc, dest_x, dest_y,
			width, height, src,
			src_x, src_y, 0);
    return Success;
}

int XCopyPlane(Display* /* display */, Drawable src, Drawable dest, GC gc,
              int src_x, int src_y, unsigned int width, unsigned int height,
              int dest_x, int dest_y, unsigned long /* plane */)
{
    GrCopyArea(dest, gc, dest_x, dest_y,
			width, height, src,
			src_x, src_y, 0);
    return Success;
}

#if 0
typedef struct {
  GR_WINDOW_ID wid;		/* window id (or 0 if no such window) */
  GR_WINDOW_ID parent;		/* parent window id */
  GR_WINDOW_ID child;		/* first child window id (or 0) */
  GR_WINDOW_ID sibling;		/* next sibling window id (or 0) */
  GR_BOOL inputonly;		/* TRUE if window is input only */
  GR_BOOL mapped;		/* TRUE if window is mapped */
  GR_COUNT unmapcount;		/* reasons why window is unmapped */
  GR_COORD x;			/* absolute x position of window */
  GR_COORD y;			/* absolute y position of window */
  GR_SIZE width;		/* width of window */
  GR_SIZE height;		/* height of window */
  GR_SIZE bordersize;		/* size of border */
  GR_COLOR bordercolor;		/* color of border */
  GR_COLOR background;		/* background color */
  GR_EVENT_MASK eventmask;	/* current event mask for this client */
  GR_WM_PROPS props;		/* window properties */
  GR_CURSOR_ID cursor;		/* cursor id*/
  unsigned long processid;	/* process id of owner*/
} GR_WINDOW_INFO;

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

#endif


Status XGetWindowAttributes(Display* display, Window w,
                            XWindowAttributes* window_attributes_return)
{
    GR_WINDOW_INFO info;
    GrGetWindowInfo(w, & info);

    window_attributes->x = info.x;
    window_attributes->y = info.y;
    window_attributes->width = info.width;
    window_attributes->height = info.height;
    window_attributes->border_width = info.bordersize;
    window_attributes->depth = 0;
    window_attributes->visual = NULL;
    window_attributes->root = 0;
    window_attributes->class = info.inputonly ? InputOnly : InputOutput ;
    window_attributes->bit_gravity = 0;
    window_attributes->win_gravity = 0;
    window_attributes->backing_store = 0;
    window_attributes->backing_planes = 0;
    window_attributes->backing_pixel = 0;
    window_attributes->save_under = FALSE;
    window_attributes->colormap = DefaultColormapOfScreen(0);
    window_attributes->map_installed = FALSE;
    window_attributes->map_state = info.mapped ? IsViewable : IsUnmapped ;
    window_attributes->all_event_masks = 0;
    window_attributes->do_not_propagate_mask = 0;
    window_attributes->override_redirect = FALSE;
    window_attributes->screen = NULL;

    return 1;
}

static XErrorHandler* g_ErrorHandler = NULL;

static void DefaultNanoXErrorHandler(GR_EVENT_ERROR* ep)
{
    if (g_ErrorHandler)
    {
        XErrorEvent errEvent;
        errEvent.type = ep->type;
        errEvent.display = wxGlobalDisplay();
        errEvent.resourceid = ep->id;
        errEvent.serial = 0;
        errEvent.error_code = ep->code;
        errEvent.request_code = 0;
        errEvent.minor_code = 0;
        (*g_ErrorHandler)(wxGlobalDisplay(), & errEvent);
    }
}

XErrorHandler XSetErrorHandler (XErrorHandler handler)
{
    XErrorHandler oldHandler = g_ErrorHandler;
    g_errorHandler = handler;
    return oldHandler;
}

Screen *XScreenOfDisplay(Display*		/* display */,
                         int /* screen_number */)
{
    static Screen s_screen;
    /* TODO: fill in the members. See Xlib.h */
    return & s_screen;
}

int DisplayWidth(Display* /* display */, int /* screen */)
{
    return _display.display_width;
}

int DisplayHeight(Display* /* display */, int /* screen */)
{
    return _display.display_height;
}

int DefaultDepth(Display* /* display */, int /* screen */)
{
    return _display.display_bpp;
}

int XAllocColor(Display* /* display */, Colormap /* cmap */,
                XColor* color)
{
    GR_PIXELVAL pixel;
    GrFindColor(color, & pixel);
    return pixel;
}

int XParseColor(Display* /* display */, Colormap /* cmap */,
                const char* cname, XColor* color)
{
    /* TODO */
    return 0;
}

#endif
  /* wxUSE_NANOX */
