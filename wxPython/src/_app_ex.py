
#----------------------------------------------------------------------

class PyOnDemandOutputWindow:
    """
    A class that can be used for redirecting Python's stdout and
    stderr streams.  It will do nothing until something is wrriten to
    the stream at which point it will create a Frame with a text area
    and write the text there.
    """
    def __init__(self, title = "wxPython: stdout/stderr"):
        self.frame  = None
        self.title  = title
        self.parent = None

    def SetParent(self, parent):
        """Set the window to be used as the popup Frame's parent."""
        self.parent = parent


    def CreateOutputWindow(self, st):
        self.frame = wx.Frame(self.parent, -1, self.title,
                              style=wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE)
        self.text  = wx.TextCtrl(self.frame, -1, "",
                                 style = wx.TE_MULTILINE | wx.TE_READONLY)
        self.text.AppendText(st)
        self.frame.SetSize((450, 300))
        self.frame.Show(True)
        EVT_CLOSE(self.frame, self.OnCloseWindow)
        

    def OnCloseWindow(self, event):
        if self.frame is not None:
            self.frame.Destroy()
        self.frame = None
        self.text  = None


    # These methods provide the file-like output behaviour.
    def write(self, text):
        """
        Create the output window if needed and write the string to it.
        If not called in the context of the gui thread then uses
        CallAfter to do the work there.
        """        
        if self.frame is None:
            if not wx.Thread_IsMain():
                wx.CallAfter(self.CreateOutputWindow, text)
            else:
                self.CreateOutputWindow(text)
        else:
            if not wx.Thread_IsMain():
                wx.CallAfter(self.text.AppendText, text)
            else:
                self.text.AppendText(text)


    def close(self):
        if self.frame is not None:
            wx.CallAfter(self.frame.Close)



#----------------------------------------------------------------------

_defRedirect = (wx.Platform == '__WXMSW__' or wx.Platform == '__WXMAC__')

class App(wx.PyApp):
    """
    The main application class.  Derive from this and implement an OnInit
    method that creates a frame and then calls self.SetTopWindow(frame)
    """
    outputWindowClass = PyOnDemandOutputWindow

    def __init__(self, redirect=_defRedirect, filename=None, useBestVisual=False):
        wx.PyApp.__init__(self)

        if wx.Platform == "__WXMAC__":
            try:
                import MacOS
                if not MacOS.WMAvailable():
                    print """\
This program needs access to the screen. Please run with 'pythonw',
not 'python', and only when you are logged in on the main display of
your Mac."""
                    _sys.exit(1)
            except:
                pass

        # This has to be done before OnInit
        self.SetUseBestVisual(useBestVisual)

        # Set the default handler for SIGINT.  This fixes a problem
        # where if Ctrl-C is pressed in the console that started this
        # app then it will not appear to do anything, (not even send
        # KeyboardInterrupt???)  but will later segfault on exit.  By
        # setting the default handler then the app will exit, as
        # expected (depending on platform.)
        try:
            import signal
            signal.signal(signal.SIGINT, signal.SIG_DFL)
        except:
            pass

        # Save and redirect the stdio to a window?
        self.stdioWin = None
        self.saveStdio = (_sys.stdout, _sys.stderr)
        if redirect:
            self.RedirectStdio(filename)

        # This finishes the initialization of wxWindows and then calls
        # the OnInit that should be present in the derived class
        self._BootstrapApp()


    def __del__(self):
        try:
            self.RestoreStdio()  # Just in case the MainLoop was overridden
        except:
            pass


    def SetTopWindow(self, frame):
        """Set the \"main\" top level window"""
        if self.stdioWin:
            self.stdioWin.SetParent(frame)
        wx.PyApp.SetTopWindow(self, frame)


    def MainLoop(self):
        """Execute the main GUI event loop"""
        wx.PyApp.MainLoop(self)
        self.RestoreStdio()


    def RedirectStdio(self, filename=None):
        """Redirect sys.stdout and sys.stderr to a file or a popup window."""
        if filename:
            _sys.stdout = _sys.stderr = open(filename, 'a')
        else:
            self.stdioWin = self.outputWindowClass()
            _sys.stdout = _sys.stderr = self.stdioWin


    def RestoreStdio(self):
        _sys.stdout, _sys.stderr = self.saveStdio



# change from wxPyApp_ to wxApp_
App_GetMacSupportPCMenuShortcuts = _core_.PyApp_GetMacSupportPCMenuShortcuts
App_GetMacAboutMenuItemId        = _core_.PyApp_GetMacAboutMenuItemId
App_GetMacPreferencesMenuItemId  = _core_.PyApp_GetMacPreferencesMenuItemId
App_GetMacExitMenuItemId         = _core_.PyApp_GetMacExitMenuItemId
App_GetMacHelpMenuTitleName      = _core_.PyApp_GetMacHelpMenuTitleName
App_SetMacSupportPCMenuShortcuts = _core_.PyApp_SetMacSupportPCMenuShortcuts
App_SetMacAboutMenuItemId        = _core_.PyApp_SetMacAboutMenuItemId
App_SetMacPreferencesMenuItemId  = _core_.PyApp_SetMacPreferencesMenuItemId
App_SetMacExitMenuItemId         = _core_.PyApp_SetMacExitMenuItemId
App_SetMacHelpMenuTitleName      = _core_.PyApp_SetMacHelpMenuTitleName
App_GetComCtl32Version           = _core_.PyApp_GetComCtl32Version

#----------------------------------------------------------------------------

class PySimpleApp(wx.App):
    """
    A simple application class.  You can just create one of these and
    then then make your top level windows later, and not have to worry
    about OnInit."""

    def __init__(self, redirect=False, filename=None, useBestVisual=False):
        wx.App.__init__(self, redirect, filename, useBestVisual)
        
    def OnInit(self):
        wx.InitAllImageHandlers()
        return True


# Is anybody using this one?
class PyWidgetTester(wx.App):
    def __init__(self, size = (250, 100)):
        self.size = size
        wx.App.__init__(self, 0)

    def OnInit(self):
        self.frame = wx.Frame(None, -1, "Widget Tester", pos=(0,0), size=self.size)
        self.SetTopWindow(self.frame)
        return True

    def SetWidget(self, widgetClass, *args):
        w = widgetClass(self.frame, *args)
        self.frame.Show(True)

#----------------------------------------------------------------------------
# DO NOT hold any other references to this object.  This is how we
# know when to cleanup system resources that wxWin is holding.  When
# the sys module is unloaded, the refcount on sys.__wxPythonCleanup
# goes to zero and it calls the wxApp_CleanUp function.

class __wxPyCleanup:
    def __init__(self):
        self.cleanup = _core_.App_CleanUp
    def __del__(self):
        self.cleanup()

_sys.__wxPythonCleanup = __wxPyCleanup()

## # another possible solution, but it gets called too early...
## if sys.version[0] == '2':
##     import atexit
##     atexit.register(_core_.wxApp_CleanUp)
## else:
##     sys.exitfunc = _core_.wxApp_CleanUp


#----------------------------------------------------------------------------
