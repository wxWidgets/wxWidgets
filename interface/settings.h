/////////////////////////////////////////////////////////////////////////////
// Name:        settings.h
// Purpose:     documentation for wxSystemSettings class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSystemSettings
    @wxheader{settings.h}
    
    wxSystemSettings allows the application to ask for details about
    the system. This can include settings such as standard colours, fonts,
    and user interface element sizes.
    
    @library{wxcore}
    @category{misc}
    
    @seealso
    wxFont, wxColour
*/
class wxSystemSettings : public wxObject
{
public:
    /**
        Default constructor. You don't need to create an instance of wxSystemSettings
        since all of its functions are static.
    */
    wxSystemSettings();

    /**
        Returns a system colour.
        
        @e index can be one of:
        
        
        @b wxSYS_COLOUR_SCROLLBAR
        
        
        The scrollbar grey area.
        
        @b wxSYS_COLOUR_BACKGROUND
        
        
        The desktop colour.
        
        @b wxSYS_COLOUR_ACTIVECAPTION
        
        
        Active window caption.
        
        @b wxSYS_COLOUR_INACTIVECAPTION
        
        
        Inactive window caption.
        
        @b wxSYS_COLOUR_MENU
        
        
        Menu background.
        
        @b wxSYS_COLOUR_WINDOW
        
        
        Window background.
        
        @b wxSYS_COLOUR_WINDOWFRAME
        
        
        Window frame.
        
        @b wxSYS_COLOUR_MENUTEXT
        
        
        Menu text.
        
        @b wxSYS_COLOUR_WINDOWTEXT
        
        
        Text in windows.
        
        @b wxSYS_COLOUR_CAPTIONTEXT
        
        
        Text in caption, size box and scrollbar arrow box.
        
        @b wxSYS_COLOUR_ACTIVEBORDER
        
        
        Active window border.
        
        @b wxSYS_COLOUR_INACTIVEBORDER
        
        
        Inactive window border.
        
        @b wxSYS_COLOUR_APPWORKSPACE
        
        
        Background colour MDI applications.
        
        @b wxSYS_COLOUR_HIGHLIGHT
        
        
        Item(s) selected in a control.
        
        @b wxSYS_COLOUR_HIGHLIGHTTEXT
        
        
        Text of item(s) selected in a control.
        
        @b wxSYS_COLOUR_BTNFACE
        
        
        Face shading on push buttons.
        
        @b wxSYS_COLOUR_BTNSHADOW
        
        
        Edge shading on push buttons.
        
        @b wxSYS_COLOUR_GRAYTEXT
        
        
        Greyed (disabled) text.
        
        @b wxSYS_COLOUR_BTNTEXT
        
        
        Text on push buttons.
        
        @b wxSYS_COLOUR_INACTIVECAPTIONTEXT
        
        
        Colour of text in active captions.
        
        @b wxSYS_COLOUR_BTNHIGHLIGHT
        
        
        Highlight colour for buttons (same as wxSYS_COLOUR_3DHILIGHT).
        
        @b wxSYS_COLOUR_3DDKSHADOW
        
        
        Dark shadow for three-dimensional display elements.
        
        @b wxSYS_COLOUR_3DLIGHT
        
        
        Light colour for three-dimensional display elements.
        
        @b wxSYS_COLOUR_INFOTEXT
        
        
        Text colour for tooltip controls.
        
        @b wxSYS_COLOUR_INFOBK
        
        
        Background colour for tooltip controls.
        
        @b wxSYS_COLOUR_DESKTOP
        
        
        Same as wxSYS_COLOUR_BACKGROUND.
        
        @b wxSYS_COLOUR_3DFACE
        
        
        Same as wxSYS_COLOUR_BTNFACE.
        
        @b wxSYS_COLOUR_3DSHADOW
        
        
        Same as wxSYS_COLOUR_BTNSHADOW.
        
        @b wxSYS_COLOUR_3DHIGHLIGHT
        
        
        Same as wxSYS_COLOUR_BTNHIGHLIGHT.
        
        @b wxSYS_COLOUR_3DHILIGHT
        
        
        Same as wxSYS_COLOUR_BTNHIGHLIGHT.
        
        @b wxSYS_COLOUR_BTNHILIGHT
        
        
        Same as wxSYS_COLOUR_BTNHIGHLIGHT.
    */
    static wxColour GetColour(wxSystemColour index);

    /**
        Returns a system font.
        
        @e index can be one of:
        
        
        @b wxSYS_OEM_FIXED_FONT
        
        
        Original equipment manufacturer dependent fixed-pitch font.
        
        @b wxSYS_ANSI_FIXED_FONT
        
        
        Windows fixed-pitch font.
        
        @b wxSYS_ANSI_VAR_FONT
        
        
        Windows variable-pitch (proportional) font.
        
        @b wxSYS_SYSTEM_FONT
        
        
        System font.
        
        @b wxSYS_DEVICE_DEFAULT_FONT
        
        
        Device-dependent font (Windows NT only).
        
        @b wxSYS_DEFAULT_GUI_FONT
        
        
        Default font for user interface
        objects such as menus and dialog boxes. Note that with modern GUIs nothing
        guarantees that the same font is used for all GUI elements, so some controls
        might use a different font by default.
    */
    static wxFont GetFont(wxSystemFont index);

    /**
        Returns the value of a system metric, or -1 if the metric is not supported on
        the current system.
        The value of @e win determines if the metric returned is a global value or
        a wxWindow based value, in which case it might determine the widget, the
        display the window is on, or something similar. The window given should be as
        close to the
        metric as possible (e.g a wxTopLevelWindow in case of the wxSYS_CAPTION_Y
        metric).
        
        @e index can be one of:
        
        
        @b wxSYS_MOUSE_BUTTONS
        
        
        Number of buttons on mouse, or zero if no mouse was installed.
        
        @b wxSYS_BORDER_X
        
        
        Width of single border.
        
        @b wxSYS_BORDER_Y
        
        
        Height of single border.
        
        @b wxSYS_CURSOR_X
        
        
        Width of cursor.
        
        @b wxSYS_CURSOR_Y
        
        
        Height of cursor.
        
        @b wxSYS_DCLICK_X
        
        
        Width in pixels of rectangle within which two successive mouse
        clicks must fall to generate a double-click.
        
        @b wxSYS_DCLICK_Y
        
        
        Height in pixels of rectangle within which two successive mouse
        clicks must fall to generate a double-click.
        
        @b wxSYS_DCLICK_MSEC
        
        
        Maximal time, in milliseconds, which may
        pass between subsequent clicks for a double click to be generated.
        
        @b wxSYS_DRAG_X
        
        
        Width in pixels of a rectangle centered on a drag point
        to allow for limited movement of the mouse pointer before a drag operation
        begins.
        
        @b wxSYS_DRAG_Y
        
        
        Height in pixels of a rectangle centered on a drag point
        to allow for limited movement of the mouse pointer before a drag operation
        begins.
        
        @b wxSYS_EDGE_X
        
        
        Width of a 3D border, in pixels. 
        
        @b wxSYS_EDGE_Y
        
        
        Height of a 3D border, in pixels. 
        
        @b wxSYS_HSCROLL_ARROW_X
        
        
        Width of arrow bitmap on horizontal scrollbar.
        
        @b wxSYS_HSCROLL_ARROW_Y
        
        
        Height of arrow bitmap on horizontal scrollbar.
        
        @b wxSYS_HTHUMB_X
        
        
        Width of horizontal scrollbar thumb.
        
        @b wxSYS_ICON_X
        
        
        The default width of an icon.
        
        @b wxSYS_ICON_Y
        
        
        The default height of an icon.
        
        @b wxSYS_ICONSPACING_X
        
        
        Width of a grid cell for items in large icon view,
        in pixels. Each item fits into a rectangle of this size when arranged.
        
        @b wxSYS_ICONSPACING_Y
        
        
        Height of a grid cell for items in large icon view,
        in pixels. Each item fits into a rectangle of this size when arranged.
        
        @b wxSYS_WINDOWMIN_X
        
        
        Minimum width of a window.
        
        @b wxSYS_WINDOWMIN_Y
        
        
        Minimum height of a window.
        
        @b wxSYS_SCREEN_X
        
        
        Width of the screen in pixels.
        
        @b wxSYS_SCREEN_Y
        
        
        Height of the screen in pixels.
        
        @b wxSYS_FRAMESIZE_X
        
        
        Width of the window frame for a wxTHICK_FRAME window.
        
        @b wxSYS_FRAMESIZE_Y
        
        
        Height of the window frame for a wxTHICK_FRAME window.
        
        @b wxSYS_SMALLICON_X
        
        
        Recommended width of a small icon (in window captions, and small icon view).
        
        @b wxSYS_SMALLICON_Y
        
        
        Recommended height of a small icon (in window captions, and small icon view).
        
        @b wxSYS_HSCROLL_Y
        
        
        Height of horizontal scrollbar in pixels.
        
        @b wxSYS_VSCROLL_X
        
        
        Width of vertical scrollbar in pixels.
        
        @b wxSYS_VSCROLL_ARROW_X
        
        
        Width of arrow bitmap on a vertical scrollbar.
        
        @b wxSYS_VSCROLL_ARROW_Y
        
        
        Height of arrow bitmap on a vertical scrollbar.
        
        @b wxSYS_VTHUMB_Y
        
        
        Height of vertical scrollbar thumb.
        
        @b wxSYS_CAPTION_Y
        
        
        Height of normal caption area.
        
        @b wxSYS_MENU_Y
        
        
        Height of single-line menu bar.
        
        @b wxSYS_NETWORK_PRESENT
        
        
        1 if there is a network present, 0 otherwise.
        
        @b wxSYS_PENWINDOWS_PRESENT
        
        
        1 if PenWindows is installed, 0 otherwise.
        
        @b wxSYS_SHOW_SOUNDS
        
        
        Non-zero if the user requires an application to present information visually in
        situations
        where it would otherwise present the information only in audible form; zero
        otherwise.
        
        @b wxSYS_SWAP_BUTTONS
        
        
        Non-zero if the meanings of the left and right mouse buttons are swapped; zero
        otherwise.
        
        @e win is a pointer to the window for which the metric is requested.
        Specifying the @e win parameter is encouraged, because some metrics on some
        ports are not supported without one,
        or they might be capable of reporting better values if given one. If a window
        does not make sense for a metric,
        one should still be given, as for example it might determine which displays
        cursor width is requested with
        wxSYS_CURSOR_X.
    */
    static int GetMetric(wxSystemMetric index, wxWindow* win = @NULL);

    /**
        Returns the screen type. The return value is one of:
        
        
        @b wxSYS_SCREEN_NONE
        
        
        Undefined screen type
        
        @b wxSYS_SCREEN_TINY
        
        
        Tiny screen, less than 320x240
        
        @b wxSYS_SCREEN_PDA
        
        
        PDA screen, 320x240 or more but less than 640x480
        
        @b wxSYS_SCREEN_SMALL
        
        
        Small screen, 640x480 or more but less than 800x600
        
        @b wxSYS_SCREEN_DESKTOP
        
        
        Desktop screen, 800x600 or more
    */
    static wxSystemScreenType GetScreenType();
};
