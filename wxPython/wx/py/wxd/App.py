"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import EvtHandler
import Parameters as wx

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class PyApp(EvtHandler):
    """Python Application base class.

    It is used to:

    - set and get application-wide properties;

    - implement the windowing system message or event loop;

    - initiate application processing via App.OnInit;

    - allow default processing of events not handled by other objects
      in the application."""

    def __init__(self):
        """Create a PyApp instance."""
        pass

    def Dispatch(self):
        """Dispatches the next event in the windowing system event
        queue.

        This can be used for programming event loops."""
        pass

    def ExitMainLoop(self):
        """Call this to explicitly exit the main message (event) loop.

        You should normally exit the main loop (and the application)
        by deleting the top window, which wxPython does automatically."""
        pass

    def GetAppName(self):
        """Return the application name."""
        pass

    def GetAssertMode(self):
        """Return the current assertion mode."""
        pass

    def GetAuto3D(self):
        """Returns True if 3D control mode is on, False otherwise.
        Windows only."""
        pass

    def GetClassName(self):
        """Return the class name of the application."""
        pass

    def GetExitOnFrameDelete(self):
        """Returns True if the application will exit when the
        top-level window is deleted, False otherwise."""
        pass

    def GetPrintMode(self):
        """Deprecated."""
        pass

    def GetTopWindow(self):
        """Return the top window.

        If the top window hasn't been set using App.SetTopWindow,
        this method will find the first top-level window (frame or
        dialog) and return that."""
        pass

    def GetUseBestVisual(self):
        """Return True if the application will use the best visual on
        systems that support different visuals, False otherwise."""
        pass

    def GetVendorName(self):
        """Return the application's vendor name."""
        pass

    def Initialized(self):
        """Return True if the application has been initialized
        (i.e. if App.OnInit has returned successfully). This can be
        useful for error message routines to determine which method of
        output is best for the current state of the program (some
        windowing systems may not like dialogs to pop up before the
        main loop has been entered)."""
        pass

    def MainLoop(self):
        """Called by wxWindows on creation of the application.
        Override this if you wish to provide your own
        (environment-dependent) main loop.

        Return 0 under X, and the wParam of the WM_QUIT message under
        Windows."""
        pass

    def OnAssert(self, file, line, cond, msg):
        """Called when an assert failure occurs, i.e. the condition
        specified in ASSERT macro evaluated to FALSE. It is only
        called in debug mode (when __WXDEBUG__ is defined) as asserts
        are not left in the release code at all.

        The base class version show the default assert failure dialog
        box proposing to the user to stop the program, continue or
        ignore all subsequent asserts.

        file is the name of the source file where the assert occured

        line is the line number in this file where the assert occured

        cond is the condition of the failed assert in string form

        msg is the message specified as argument to ASSERT_MSG or
        FAIL_MSG, will be NULL if just ASSERT or FAIL was used"""
        pass

    def OnExit(self):
        """Provide this member function for any processing which needs
        to be done as the application is about to exit.  OnExit is
        called after destroying all application windows and controls,
        but before wxWindows cleanup."""
        pass

    def OnInit(self):
        """This must be provided by the application, and will usually
        create the application's main window, optionally calling
        App.SetTopWindow.

        Return True to continue processing, False to exit the
        application."""
        pass

    def OnInitGui(self):
        """Called just after the platform's GUI has been initialized,
        but before the App.OnInit() gets called. Rarely needed in
        practice. Unlike App.OnInit(), does not need to return
        True/False."""
        pass
    
    def Pending(self):
        """Return True if unprocessed events are in the window system
        event queue."""
        pass

    def ProcessIdle(self):
        """Sends the EVT_IDLE event and is called inside the MainLoop.
        
        You only need this if you implement your own main loop."""
        pass

    def SetAppName(self, name):
        """Set the name of the application."""
        pass

    def SetAssertMode(self, mode):
        """Lets you control how C++ assertions are processed.

        Valid modes are: PYAPP_ASSERT_SUPPRESS,
        PYAPP_ASSERT_EXCEPTION, and PYAPP_ASSERT_DIALOG.  Using
        _SUPPRESS will give you behavior like the old final builds and
        the assert will be ignored, _EXCEPTION is the new default
        described above, and _DIALOG is like the default in 2.3.3.1
        and prior hybrid builds.  You can also combine _EXCEPTION and
        _DIALOG if you wish, although I don't know why you would."""
        pass

    def SetAuto3D(self, auto3D):
        """Switches automatic 3D controls on or off.  Windows only.

        If auto3D is True, all controls will be created with 3D
        appearances unless overridden for a control or dialog.  The
        default is True."""
        pass

    def SetClassName(self, name):
        """Set the class name of the application."""
        pass

    def SetExitOnFrameDelete(self, flag):
        """If flag is True (the default), the application will exit
        when the top-level frame is deleted.  If False, the
        application will continue to run."""
        pass

    def SetPrintMode(self, mode):
        """Deprecated."""
        pass

    def SetTopWindow(self, window):
        """Set the 'top' window.

        You can call this from within App.OnInit to let wxWindows
        know which is the main window.  You don't have to set the top
        window; it is only a convenience so that (for example) certain
        dialogs without parents can use a specific window as the top
        window.  If no top window is specified by the application,
        wxWindows just uses the first frame or dialog in its top-level
        window list, when it needs to use the top window."""
        pass

    def SetUseBestVisual(self, flag):
        """Allows the programmer to specify whether the application
        will use the best visual on systems that support several
        visual on the same display.  This is typically the case under
        Solaris and IRIX, where the default visual is only 8-bit
        whereas certain applications are supposed to run in TrueColour
        mode.

        Note that this function has to be called in the constructor of
        the App instance and won't have any effect when called later
        on.

        This function currently only has effect under GTK."""
        pass

    def SetVendorName(self, name):
        """Sets the name of application's vendor.  The name will be
        used in registry access."""
        pass

    def Yield(self, onlyIfNeeded=False):
        """Yields control to pending messages in the windowing system.
        This can be useful, for example, when a time-consuming process
        writes to a text window.  Without an occasional yield, the
        text window will not be updated properly, and on systems with
        cooperative multitasking, such as Windows 3.1 other processes
        will not respond.

        Caution should be exercised, however, since yielding may allow
        the user to perform actions which are not compatible with the
        current task.  Disabling menu items or whole menus during
        processing can avoid unwanted reentrance of code: see
        wx.SafeYield for a better function.

        Calling Yield() recursively is normally an error and an assert
        failure is raised in debug build if such situation is
        detected.  However if the the onlyIfNeeded parameter is True,
        the method will just silently return False instead."""
        pass


from wxPython.wx import wxPlatform
_redirect = (wxPlatform == '__WXMSW__' or wxPlatform == '__WXMAC__')
del wxPlatform


class App(PyApp):
    """The main application class.

    Inherit from this class and implement an OnInit method that
    creates a frame and then calls self.SetTopWindow(frame)."""

    def __init__(self, redirect=_redirect, filename=None, useBestVisual=False):
        """Create an App instance.

        redirect defaults to True on Windows and Mac. If redirect is
        True, stdio goes to an output window or a file if filename is
        not None."""
        pass


del _redirect


class PyOnDemandOutputWindow:
    """Used by App to display stdout and stderr messages if app is
    created using App(redirect=True).  Mostly useful on Windows or
    Mac where apps aren't always launched from the command line."""
    pass


class PySimpleApp(App):
    """Use instead of App for simple apps with a simple frame or
    dialog, particularly for testing."""

    def __init__(self, flag=0):
        """Create a PySimpleApp instance.

        flag is the same as App's redirect parameter to redirect stdio."""
        pass

    def OnInit(self):
        """Automatically does a wx.InitAllImageHandlers()."""
        pass


class PyWidgetTester(App):
    """Use instead of App for testing widgets. Provides a frame
    containing an instance of a widget.

    Create a PyWidgetTester instance with the desired size for the
    frame, then create the widget and show the frame using SetWidget."""

    def __init__(self, size=(250, 100)):
        """Create a PyWidgetTester instance, with no stdio redirection.

        size is for the frame to hold the widget."""
        pass

    def OnInit(self):
        """Creates a frame that will hold the widget to be tested."""
        pass

    def SetWidget(self, widgetClass, *args):
        """Create a widgetClass instance using the supplied args and
        with a frame as parent, then show the frame."""
        pass


class SingleInstanceChecker:
    """Allows one to check that only a single instance of a program is
    running. To do it, you should create an object of this class. As
    long as this object is alive, calls to IsAnotherRunning() from
    other processes will return True.

    As the object should have the life span as big as possible, it
    makes sense to create it either as a global or in App.OnInit()."""

    def __init__(self, name, path=wx.EmptyString):
        """Create a SingleInstanceChecker instance.

        name should be as unique as possible. It is used as the mutex
        name under Win32 and the lock file name under Unix.
        App.GetAppName() and wx.GetUserId() are commonly used.

        path is optional and is ignored under Win32 and used as the
        directory to create the lock file in under Unix (default is
        wx.GetHomeDir())."""
        pass

    def Create(self, name, path=wx.EmptyString):
        """Create a SingleInstanceChecker instance."""
        pass

    def IsAnotherRunning(self):
        """Return True if another copy of this program is already running."""
        pass
