/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#include "wx/cursor.h"
#include "wx/window.h" // for wxTopLevelWindows

#include <ctype.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
// #include <netdb.h>
#include <signal.h>

#if (defined(__SUNCC__) || defined(__CLCC__))
    #include <sysent.h>
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif

#include "wx/unix/execute.h"

#include "wx/x11/private.h"

#include "X11/Xutil.h"

#ifdef __VMS__
#pragma message enable nosimpint
#endif

// ----------------------------------------------------------------------------
// async event processing
// ----------------------------------------------------------------------------

// Consume all events until no more left
void wxFlushEvents()
{
    Display *display = (Display*) wxGetDisplay();

    XSync (display, FALSE);

    // TODO for X11
    // ??
}

bool wxCheckForInterrupt(wxWindow *wnd)
{
    wxASSERT_MSG(FALSE, "wxCheckForInterrupt not yet implemented.");
    return FALSE;
}

// ----------------------------------------------------------------------------
// wxExecute stuff
// ----------------------------------------------------------------------------

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    // TODO
    return 0;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// Emit a beeeeeep
void wxBell()
{
    // Use current setting for the bell
    XBell ((Display*) wxGetDisplay(), 0);
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    if (majorVsn)
        *majorVsn = 0;
        
    if (minorVsn)
        *minorVsn = 0;
        
    return wxX11;
}

// ----------------------------------------------------------------------------
// display info
// ----------------------------------------------------------------------------

void wxGetMousePosition( int* x, int* y )
{
#if wxUSE_NANOX
    // TODO
    *x = 0;
    *y = 0;
#else
    XMotionEvent xev;
    Window root, child;
    XQueryPointer((Display*) wxGetDisplay(),
                  DefaultRootWindow((Display*) wxGetDisplay()),
                  &root, &child,
                  &(xev.x_root), &(xev.y_root),
                  &(xev.x),      &(xev.y),
                  &(xev.state));
    *x = xev.x_root;
    *y = xev.y_root;
#endif
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    return wxDisplayDepth() > 1;
}

// Returns depth of screen
int wxDisplayDepth()
{
    Display *dpy = (Display*) wxGetDisplay();

    return DefaultDepth (dpy, DefaultScreen (dpy));
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    Display *dpy = (Display*) wxGetDisplay();

    if ( width )
        *width = DisplayWidth (dpy, DefaultScreen (dpy));
    if ( height )
        *height = DisplayHeight (dpy, DefaultScreen (dpy));
}

void wxDisplaySizeMM(int *width, int *height)
{
    Display *dpy = (Display*) wxGetDisplay();

    if ( width )
        *width = DisplayWidthMM(dpy, DefaultScreen (dpy));
    if ( height )
        *height = DisplayHeightMM(dpy, DefaultScreen (dpy));
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // This is supposed to return desktop dimensions minus any window
    // manager panels, menus, taskbars, etc.  If there is a way to do that
    // for this platform please fix this function, otherwise it defaults
    // to the entire desktop.
    if (x) *x = 0;
    if (y) *y = 0;
    wxDisplaySize(width, height);
}


// Configurable display in wxX11 and wxMotif
static WXDisplay *gs_currentDisplay = NULL;
static wxString gs_displayName;

WXDisplay *wxGetDisplay()
{
    if (gs_currentDisplay)
        return gs_currentDisplay;
    return wxApp::GetDisplay();
}

bool wxSetDisplay(const wxString& display_name)
{
    gs_displayName = display_name;

    if ( display_name.IsEmpty() )
    {
        gs_currentDisplay = NULL;

        return TRUE;
    }
    else
    {
        Display* display = XOpenDisplay((char*) display_name.c_str());

        if (display)
        {
            gs_currentDisplay = (WXDisplay*) display;
            return TRUE;
        }
        else
            return FALSE;
    }
}

wxString wxGetDisplayName()
{
    return gs_displayName;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

// ----------------------------------------------------------------------------
// keycode translations
// ----------------------------------------------------------------------------

#include <X11/keysym.h>

// FIXME what about tables??

int wxCharCodeXToWX(KeySym keySym)
{
    int id;
    switch (keySym)
    {
        case XK_Shift_L:
        case XK_Shift_R:
            id = WXK_SHIFT; break;
        case XK_Control_L:
        case XK_Control_R:
            id = WXK_CONTROL; break;
        case XK_BackSpace:
            id = WXK_BACK; break;
        case XK_Delete:
            id = WXK_DELETE; break;
        case XK_Clear:
            id = WXK_CLEAR; break;
        case XK_Tab:
            id = WXK_TAB; break;
        case XK_numbersign:
            id = '#'; break;
        case XK_Return:
            id = WXK_RETURN; break;
        case XK_Escape:
            id = WXK_ESCAPE; break;
        case XK_Pause:
        case XK_Break:
            id = WXK_PAUSE; break;
        case XK_Num_Lock:
            id = WXK_NUMLOCK; break;
        case XK_Scroll_Lock:
            id = WXK_SCROLL; break;

        case XK_Home:
            id = WXK_HOME; break;
        case XK_End:
            id = WXK_END; break;
        case XK_Left:
            id = WXK_LEFT; break;
        case XK_Right:
            id = WXK_RIGHT; break;
        case XK_Up:
            id = WXK_UP; break;
        case XK_Down:
            id = WXK_DOWN; break;
        case XK_Next:
            id = WXK_NEXT; break;
        case XK_Prior:
            id = WXK_PRIOR; break;
        case XK_Menu:
            id = WXK_MENU; break;
        case XK_Select:
            id = WXK_SELECT; break;
        case XK_Cancel:
            id = WXK_CANCEL; break;
        case XK_Print:
            id = WXK_PRINT; break;
        case XK_Execute:
            id = WXK_EXECUTE; break;
        case XK_Insert:
            id = WXK_INSERT; break;
        case XK_Help:
            id = WXK_HELP; break;

        case XK_KP_Multiply:
            id = WXK_MULTIPLY; break;
        case XK_KP_Add:
            id = WXK_ADD; break;
        case XK_KP_Subtract:
            id = WXK_SUBTRACT; break;
        case XK_KP_Divide:
            id = WXK_DIVIDE; break;
        case XK_KP_Decimal:
            id = WXK_DECIMAL; break;
        case XK_KP_Equal:
            id = '='; break;
        case XK_KP_Space:
            id = ' '; break;
        case XK_KP_Tab:
            id = WXK_TAB; break;
        case XK_KP_Enter:
            id = WXK_RETURN; break;
        case XK_KP_0:
            id = WXK_NUMPAD0; break;
        case XK_KP_1:
            id = WXK_NUMPAD1; break;
        case XK_KP_2:
            id = WXK_NUMPAD2; break;
        case XK_KP_3:
            id = WXK_NUMPAD3; break;
        case XK_KP_4:
            id = WXK_NUMPAD4; break;
        case XK_KP_5:
            id = WXK_NUMPAD5; break;
        case XK_KP_6:
            id = WXK_NUMPAD6; break;
        case XK_KP_7:
            id = WXK_NUMPAD7; break;
        case XK_KP_8:
            id = WXK_NUMPAD8; break;
        case XK_KP_9:
            id = WXK_NUMPAD9; break;
        case XK_F1:
            id = WXK_F1; break;
        case XK_F2:
            id = WXK_F2; break;
        case XK_F3:
            id = WXK_F3; break;
        case XK_F4:
            id = WXK_F4; break;
        case XK_F5:
            id = WXK_F5; break;
        case XK_F6:
            id = WXK_F6; break;
        case XK_F7:
            id = WXK_F7; break;
        case XK_F8:
            id = WXK_F8; break;
        case XK_F9:
            id = WXK_F9; break;
        case XK_F10:
            id = WXK_F10; break;
        case XK_F11:
            id = WXK_F11; break;
        case XK_F12:
            id = WXK_F12; break;
        case XK_F13:
            id = WXK_F13; break;
        case XK_F14:
            id = WXK_F14; break;
        case XK_F15:
            id = WXK_F15; break;
        case XK_F16:
            id = WXK_F16; break;
        case XK_F17:
            id = WXK_F17; break;
        case XK_F18:
            id = WXK_F18; break;
        case XK_F19:
            id = WXK_F19; break;
        case XK_F20:
            id = WXK_F20; break;
        case XK_F21:
            id = WXK_F21; break;
        case XK_F22:
            id = WXK_F22; break;
        case XK_F23:
            id = WXK_F23; break;
        case XK_F24:
            id = WXK_F24; break;
        default:
            id = (keySym <= 255) ? (int)keySym : -1;
    }

    return id;
}

KeySym wxCharCodeWXToX(int id)
{
    KeySym keySym;

    switch (id)
    {
        case WXK_CANCEL:            keySym = XK_Cancel; break;
        case WXK_BACK:              keySym = XK_BackSpace; break;
        case WXK_TAB:            keySym = XK_Tab; break;
        case WXK_CLEAR:        keySym = XK_Clear; break;
        case WXK_RETURN:        keySym = XK_Return; break;
        case WXK_SHIFT:        keySym = XK_Shift_L; break;
        case WXK_CONTROL:        keySym = XK_Control_L; break;
        case WXK_MENU :        keySym = XK_Menu; break;
        case WXK_PAUSE:        keySym = XK_Pause; break;
        case WXK_ESCAPE:        keySym = XK_Escape; break;
        case WXK_SPACE:        keySym = ' '; break;
        case WXK_PRIOR:        keySym = XK_Prior; break;
        case WXK_NEXT :        keySym = XK_Next; break;
        case WXK_END:        keySym = XK_End; break;
        case WXK_HOME :        keySym = XK_Home; break;
        case WXK_LEFT :        keySym = XK_Left; break;
        case WXK_UP:        keySym = XK_Up; break;
        case WXK_RIGHT:        keySym = XK_Right; break;
        case WXK_DOWN :        keySym = XK_Down; break;
        case WXK_SELECT:        keySym = XK_Select; break;
        case WXK_PRINT:        keySym = XK_Print; break;
        case WXK_EXECUTE:        keySym = XK_Execute; break;
        case WXK_INSERT:        keySym = XK_Insert; break;
        case WXK_DELETE:        keySym = XK_Delete; break;
        case WXK_HELP :        keySym = XK_Help; break;
        case WXK_NUMPAD0:        keySym = XK_KP_0; break;
        case WXK_NUMPAD1:        keySym = XK_KP_1; break;
        case WXK_NUMPAD2:        keySym = XK_KP_2; break;
        case WXK_NUMPAD3:        keySym = XK_KP_3; break;
        case WXK_NUMPAD4:        keySym = XK_KP_4; break;
        case WXK_NUMPAD5:        keySym = XK_KP_5; break;
        case WXK_NUMPAD6:        keySym = XK_KP_6; break;
        case WXK_NUMPAD7:        keySym = XK_KP_7; break;
        case WXK_NUMPAD8:        keySym = XK_KP_8; break;
        case WXK_NUMPAD9:        keySym = XK_KP_9; break;
        case WXK_MULTIPLY:        keySym = XK_KP_Multiply; break;
        case WXK_ADD:        keySym = XK_KP_Add; break;
        case WXK_SUBTRACT:        keySym = XK_KP_Subtract; break;
        case WXK_DECIMAL:        keySym = XK_KP_Decimal; break;
        case WXK_DIVIDE:        keySym = XK_KP_Divide; break;
        case WXK_F1:        keySym = XK_F1; break;
        case WXK_F2:        keySym = XK_F2; break;
        case WXK_F3:        keySym = XK_F3; break;
        case WXK_F4:        keySym = XK_F4; break;
        case WXK_F5:        keySym = XK_F5; break;
        case WXK_F6:        keySym = XK_F6; break;
        case WXK_F7:        keySym = XK_F7; break;
        case WXK_F8:        keySym = XK_F8; break;
        case WXK_F9:        keySym = XK_F9; break;
        case WXK_F10:        keySym = XK_F10; break;
        case WXK_F11:        keySym = XK_F11; break;
        case WXK_F12:        keySym = XK_F12; break;
        case WXK_F13:        keySym = XK_F13; break;
        case WXK_F14:        keySym = XK_F14; break;
        case WXK_F15:        keySym = XK_F15; break;
        case WXK_F16:        keySym = XK_F16; break;
        case WXK_F17:        keySym = XK_F17; break;
        case WXK_F18:        keySym = XK_F18; break;
        case WXK_F19:        keySym = XK_F19; break;
        case WXK_F20:        keySym = XK_F20; break;
        case WXK_F21:        keySym = XK_F21; break;
        case WXK_F22:        keySym = XK_F22; break;
        case WXK_F23:        keySym = XK_F23; break;
        case WXK_F24:        keySym = XK_F24; break;
        case WXK_NUMLOCK:    keySym = XK_Num_Lock; break;
        case WXK_SCROLL:     keySym = XK_Scroll_Lock; break;
        default:             keySym = id <= 255 ? (KeySym)id : 0;
    }

    return keySym;
}

// ----------------------------------------------------------------------------
// Some colour manipulation routines
// ----------------------------------------------------------------------------

void wxHSVToXColor(wxHSV *hsv,XColor *rgb)
{
    int h = hsv->h;
    int s = hsv->s;
    int v = hsv->v;
    int r = 0, g = 0, b = 0;
    int i, f;
    int p, q, t;
    s = (s * wxMAX_RGB) / wxMAX_SV;
    v = (v * wxMAX_RGB) / wxMAX_SV;
    if (h == 360) h = 0;
    if (s == 0) { h = 0; r = g = b = v; }
    i = h / 60;
    f = h % 60;
    p = v * (wxMAX_RGB - s) / wxMAX_RGB;
    q = v * (wxMAX_RGB - s * f / 60) / wxMAX_RGB;
    t = v * (wxMAX_RGB - s * (60 - f) / 60) / wxMAX_RGB;
    switch (i)
    {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }
    rgb->red = r << 8;
    rgb->green = g << 8;
    rgb->blue = b << 8;
}

void wxXColorToHSV(wxHSV *hsv,XColor *rgb)
{
    int r = rgb->red >> 8;
    int g = rgb->green >> 8;
    int b = rgb->blue >> 8;
    int maxv = wxMax3(r, g, b);
    int minv = wxMin3(r, g, b);
    int h = 0, s, v;
    v = maxv;
    if (maxv) s = (maxv - minv) * wxMAX_RGB / maxv;
    else s = 0;
    if (s == 0) h = 0;
    else
    {
        int rc, gc, bc, hex = 0;
        rc = (maxv - r) * wxMAX_RGB / (maxv - minv);
        gc = (maxv - g) * wxMAX_RGB / (maxv - minv);
        bc = (maxv - b) * wxMAX_RGB / (maxv - minv);
        if (r == maxv) { h = bc - gc, hex = 0; }
        else if (g == maxv) { h = rc - bc, hex = 2; }
        else if (b == maxv) { h = gc - rc, hex = 4; }
        h = hex * 60 + (h * 60 / wxMAX_RGB);
        if (h < 0) h += 360;
    }
    hsv->h = h;
    hsv->s = (s * wxMAX_SV) / wxMAX_RGB;
    hsv->v = (v * wxMAX_SV) / wxMAX_RGB;
}

void wxAllocNearestColor(Display *d,Colormap cmp,XColor *xc)
{
#if !wxUSE_NANOX
    int llp;

    int screen = DefaultScreen(d);
    int num_colors = DisplayCells(d,screen);

    XColor *color_defs = new XColor[num_colors];
    for(llp = 0;llp < num_colors;llp++) color_defs[llp].pixel = llp;
    XQueryColors(d,cmp,color_defs,num_colors);

    wxHSV hsv_defs, hsv;
    wxXColorToHSV(&hsv,xc);

    int diff, min_diff = 0, pixel = 0;

    for(llp = 0;llp < num_colors;llp++)
    {
        wxXColorToHSV(&hsv_defs,&color_defs[llp]);
        diff = wxSIGN(wxH_WEIGHT * (hsv.h - hsv_defs.h)) +
            wxSIGN(wxS_WEIGHT * (hsv.s - hsv_defs.s)) +
            wxSIGN(wxV_WEIGHT * (hsv.v - hsv_defs.v));
        if (llp == 0) min_diff = diff;
        if (min_diff > diff) { min_diff = diff; pixel = llp; }
        if (min_diff == 0) break;
    }

    xc -> red = color_defs[pixel].red;
    xc -> green = color_defs[pixel].green;
    xc -> blue = color_defs[pixel].blue;
    xc -> flags = DoRed | DoGreen | DoBlue;

/*  FIXME, TODO
    if (!XAllocColor(d,cmp,xc))
        cout << "wxAllocNearestColor : Warning : Cannot find nearest color !\n";
*/

    delete[] color_defs;
#endif
}

void wxAllocColor(Display *d,Colormap cmp,XColor *xc)
{
    if (!XAllocColor(d,cmp,xc))
    {
        //          cout << "wxAllocColor : Warning : Can not allocate color, attempt find nearest !\n";
        wxAllocNearestColor(d,cmp,xc);
    }
}

#ifdef __WXDEBUG__
wxString wxGetXEventName(XEvent& event)
{
#if wxUSE_NANOX
    wxString str(wxT("(some event)"));
    return str;
#else
    int type = event.xany.type;
	    static char* event_name[] = {
		"", "unknown(-)",                                         // 0-1
		"KeyPress", "KeyRelease", "ButtonPress", "ButtonRelease", // 2-5
		"MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn",  // 6-9
		"FocusOut", "KeymapNotify", "Expose", "GraphicsExpose",   // 10-13
		"NoExpose", "VisibilityNotify", "CreateNotify",           // 14-16
		"DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",// 17-20
		"ReparentNotify", "ConfigureNotify", "ConfigureRequest",  // 21-23
		"GravityNotify", "ResizeRequest", "CirculateNotify",      // 24-26
		"CirculateRequest", "PropertyNotify", "SelectionClear",   // 27-29
		"SelectionRequest", "SelectionNotify", "ColormapNotify",  // 30-32
		"ClientMessage", "MappingNotify",                         // 33-34
		"unknown(+)"};                                            // 35
	    type = wxMin(35, type); type = wxMax(1, type);
        wxString str(event_name[type]);
        return str;
#endif
}
#endif

bool wxWindowIsVisible(Window win)
{
    XWindowAttributes wa;
    XGetWindowAttributes(wxGlobalDisplay(), win, &wa);

    return (wa.map_state == IsViewable);
}
