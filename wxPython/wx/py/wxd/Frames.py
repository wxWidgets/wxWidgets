"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx
from Window import TopLevelWindow, Window


class Frame(TopLevelWindow):
    """A frame is a window whose size and position can (usually) be
    changed by the user. It usually has thick borders and a title bar,
    and can optionally contain a menu bar, toolbar and status bar. A
    frame can contain any window that is not a frame or dialog.

    A frame that has a status bar and toolbar created via the
    CreateStatusBar/CreateToolBar functions manages these windows, and
    adjusts the value returned by GetClientSize to reflect the
    remaining size available to application windows.

    An application should normally define a CloseEvent handler for the
    frame to respond to system close events, for example so that
    related data and subwindows can be cleaned up."""

    def __init__(self, parent, id, title, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
                 name=wx.PyFrameNameStr):
        """Create a Frame instance.

        parent - The window parent.  This may be None.  If it is not
        None, the frame will always be displayed on top of the parent
        window on Windows.

        id - The window identifier. It may take a value of -1 to
        indicate a default value.

        title - The caption to be displayed on the frame's title bar.

        pos - The window position. A value of (-1, -1) indicates a
        default position, chosen by either the windowing system or
        wxWindows, depending on platform.

        size - The window size. A value of (-1, -1) indicates a
        default size, chosen by either the windowing system or
        wxWindows, depending on platform.

        style - The window style.

        name - The name of the window. This parameter is used to
        associate a name with the item, allowing the application user
        to set Motif resource values for individual windows."""
        pass

    def Create(self, parent, id, title, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
               name=wx.PyFrameNameStr):
        """Create a Frame instance."""
        pass

    def Command(self, id):
        """Simulate a menu command; id is a menu item identifier."""
        pass

    def CreateStatusBar(self, number=1, style=wx.ST_SIZEGRIP, id=-1,
                        name=wx.PyStatusLineNameStr):
        """Create a status bar at the bottom of frame.

        number - The number of fields to create. Specify a value
        greater than 1 to create a multi-field status bar.

        style - The status bar style. See wx.StatusBar for a list of
        valid styles.

        id - The status bar window identifier. If -1, an identifier
        will be chosen by wxWindows.

        name - The status bar window name.

        The width of the status bar is the whole width of the frame
        (adjusted automatically when resizing), and the height and
        text size are chosen by the host windowing system.

        By default, the status bar is an instance of wx.StatusBar."""
        pass

    def CreateToolBar(self, style=wx.NO_BORDER | wx.TB_HORIZONTAL,
                      id=-1, name=wx.PyToolBarNameStr):
        """Create a toolbar at the top or left of frame.

        style - The toolbar style. See wxToolBar for a list of valid
        styles.

        id - The toolbar window identifier. If -1, an identifier will
        be chosen by wxWindows.

        name - The toolbar window name.

        By default, the toolbar is an instance of wx.ToolBar (which is
        defined to be a suitable toolbar class on each platform, such
        as wx.ToolBar95).

        When a toolbar has been created with this function, or made
        known to the frame with wx.Frame.SetToolBar, the frame will
        manage the toolbar position and adjust the return value from
        wx.Window.GetClientSize to reflect the available space for
        application windows."""
        pass

    def DoGiveHelp(self, text, show):
        """Show help text (typically in the statusbar).

        show is False if you are hiding the help, True otherwise.

        Meant to be overridden if a derived frame wants to do
        something else with help text from menus and etc.  The default
        implementation simply calls Frame.SetStatusText."""
        pass

    def GetClientAreaOrigin(self):
        """Return origin of frame client area (in client coordinates).

        It may be different from (0, 0) if the frame has a toolbar."""
        pass

    def GetMenuBar(self):
        """Return menubar currently associated with frame (if any)."""
        pass

    def GetStatusBar(self):
        """Return status bar currently associated with frame (if any)."""
        pass

    def GetStatusBarPane(self):
        """Return status bar pane used to display menu/toolbar help."""
        pass

    def GetToolBar(self):
        """Return toolbar currently associated with frame (if any)."""
        pass

    def PopStatusText(self, number=0):
        """Redraw status bar with previous status text.

        number - The status field (starting from zero)."""
        pass

    def ProcessCommand(self, id):
        """Process menu command; return True if processed.

        id is the menu command identifier."""
        pass

    def PushStatusText(self, text, number=0):
        """Set status bar text and redraw status bar, remembering
        previous text.

        text - The text for the status field.

        number - The status field (starting from zero).

        Use an empty string to clear the status bar."""
        pass

    def SendSizeEvent(self):
        """Send a dummy size event to the frame forcing it to
        reevaluate its children positions. It is sometimes useful to
        call this function after adding or deleting a children after
        the frame creation or if a child size changes.

        Note that if the frame is using either sizers or constraints
        for the children layout, it is enough to call Frame.Layout()
        directly and this function should not be used in this case."""
        pass

    def SetMenuBar(self, menubar):
        """Show the menu bar in the frame.

        menuBar - The menu bar to associate with the frame.

        If the frame is destroyed, the menu bar and its menus will be
        destroyed also, so do not delete the menu bar explicitly
        (except by resetting the frame's menu bar to another frame or
        NULL).

        Under Windows, a call to Frame.OnSize is generated, so be sure
        to initialize data members properly before calling SetMenuBar.

        Note that it is not possible to call this function twice for
        the same frame object."""
        pass

    def SetStatusBar(self, statBar):
        """Associate a status bar with the frame."""
        pass

    def SetStatusBarPane(self, n):
        """Set the status bar pane used to display menu and toolbar
        help.  Using -1 disables help display."""
        pass

    def SetStatusText(self, text, number=0):
        """Set status bar text and redraw status bar.

        text - The text for the status field.

        number - The status field (starting from zero).

        Use an empty string to clear the status bar."""
        pass

    def SetStatusWidths(self, choices):
        """Sets the widths of the fields in the status bar.

        choices - a Python list of integers, each of which is a status
        field width in pixels. A value of -1 indicates that the field
        is variable width; at least one field must be -1.

        The widths of the variable fields are calculated from the
        total width of all fields, minus the sum of widths of the
        non-variable fields, divided by the number of variable fields."""
        pass

    def SetToolBar(self, toolbar):
        """Associate a toolbar with the frame."""
        pass


class LayoutAlgorithm(Object):
    """LayoutAlgorithm implements layout of subwindows in MDI or SDI
    frames.  It sends a wx.CalculateLayoutEvent event to children of
    the frame, asking them for information about their size.  For MDI
    parent frames, the algorithm allocates the remaining space to the
    MDI client window (which contains the MDI child frames).  For SDI
    (normal) frames, a 'main' window is specified as taking up the
    remaining space.

    Because the event system is used, this technique can be applied to
    any windows, which are not necessarily 'aware' of the layout
    classes.  However, you may wish to use wx.SashLayoutWindow for
    your subwindows since this class provides handlers for the
    required events, and accessors to specify the desired size of the
    window.  The sash behaviour in the base class can be used,
    optionally, to make the windows user-resizable.

    LayoutAlgorithm is typically used in IDE (integrated development
    environment) applications, where there are several resizable
    windows in addition to the MDI client window, or other primary
    editing window.  Resizable windows might include toolbars, a
    project window, and a window for displaying error and warning
    messages.

    When a window receives an OnCalculateLayout event, it should call
    SetRect in the given event object, to be the old supplied
    rectangle minus whatever space the window takes up.  It should
    also set its own size accordingly.
    SashLayoutWindow.OnCalculateLayout generates an OnQueryLayoutInfo
    event which it sends to itself to determine the orientation,
    alignment and size of the window, which it gets from internal
    member variables set by the application.

    The algorithm works by starting off with a rectangle equal to the
    whole frame client area.  It iterates through the frame children,
    generating OnCalculateLayout events which subtract the window size
    and return the remaining rectangle for the next window to process.
    It is assumed (by SashLayoutWindow.OnCalculateLayout) that a
    window stretches the full dimension of the frame client, according
    to the orientation it specifies.  For example, a horizontal window
    will stretch the full width of the remaining portion of the frame
    client area.  In the other orientation, the window will be fixed
    to whatever size was specified by OnQueryLayoutInfo.  An alignment
    setting will make the window 'stick' to the left, top, right or
    bottom of the remaining client area.  This scheme implies that
    order of window creation is important.  Say you wish to have an
    extra toolbar at the top of the frame, a project window to the
    left of the MDI client window, and an output window above the
    status bar.  You should therefore create the windows in this
    order: toolbar, output window, project window.  This ensures that
    the toolbar and output window take up space at the top and bottom,
    and then the remaining height in-between is used for the project
    window.

    LayoutAlgorithm is quite independent of the way in which
    OnCalculateLayout chooses to interpret a window's size and
    alignment.  Therefore you could implement a different window class
    with a new OnCalculateLayout event handler, that has a more
    sophisticated way of laying out the windows.  It might allow
    specification of whether stretching occurs in the specified
    orientation, for example, rather than always assuming
    stretching. (This could, and probably should, be added to the
    existing implementation).

    The algorithm object does not respond to events, but itself
    generates the following events in order to calculate window sizes:
    EVT_QUERY_LAYOUT_INFO(func), EVT_CALCULATE_LAYOUT(func)."""

    def __init__(self):
        """Create a LayoutAlgorithm instance."""
        pass

    def LayoutFrame(self, frame, mainWindow=wx.NULL):
        """Lay out the children of a normal frame.

        mainWindow is set to occupy the remaining space.  This
        function simply calls LayoutWindow()."""
        pass

    def LayoutMDIFrame(self, frame, rect=wx.NULL):
        """Lay out the children of an MDI parent frame.

        If rect is non-NULL, the given rectangle will be used as a
        starting point instead of the frame's client area.

        The MDI client window is set to occupy the remaining space."""
        pass

    def LayoutWindow(self, parent, mainWindow=wx.NULL):
        """Lay out the children of a normal frame or other window.

        mainWindow is set to occupy the remaining space.  If this is
        not specified, then the last window that responds to a
        calculate layout event in query mode will get the remaining
        space (that is, a non-query OnCalculateLayout event will not
        be sent to this window and the window will be set to the
        remaining size)."""
        pass


class MDIChildFrame(Frame):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Activate(self):
        """"""
        pass

    def Maximize(self):
        """"""
        pass

    def Restore(self):
        """"""
        pass


class MDIClientWindow(Window):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass


class MDIParentFrame(Frame):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def ActivateNext(self):
        """"""
        pass

    def ActivatePrevious(self):
        """"""
        pass

    def ArrangeIcons(self):
        """"""
        pass

    def Cascade(self):
        """"""
        pass

    def GetActiveChild(self):
        """"""
        pass

    def GetClientWindow(self):
        """"""
        pass

    def GetToolBar(self):
        """"""
        pass

    def Tile(self):
        """"""
        pass


class MiniFrame(Frame):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass


class SplashScreen(Frame):
    """"""

    def __init__(self):
        """"""
        pass

    def GetSplashStyle(self):
        """"""
        pass

    def GetSplashWindow(self):
        """"""
        pass

    def GetTimeout(self):
        """"""
        pass


class SplashScreenWindow(Window):
    """"""

    def __init__(self):
        """"""
        pass

    def GetBitmap(self):
        """"""
        pass

    def SetBitmap(self):
        """"""
        pass


class StatusBar(Window):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetBorderX(self):
        """"""
        pass

    def GetBorderY(self):
        """"""
        pass

    def GetFieldRect(self):
        """"""
        pass

    def GetFieldsCount(self):
        """"""
        pass

    def GetStatusText(self):
        """"""
        pass

    def PopStatusText(self):
        """"""
        pass

    def PushStatusText(self):
        """"""
        pass

    def SetFieldsCount(self):
        """"""
        pass

    def SetMinHeight(self):
        """"""
        pass

    def SetStatusText(self):
        """"""
        pass

    def SetStatusWidths(self):
        """"""
        pass
