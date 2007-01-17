#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         run.py
# Purpose:      Simple framework for running individual demos
#
# Author:       Robin Dunn
#
# Created:      6-March-2000
# RCS-ID:       $Id$
# Copyright:    (c) 2000 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

"""
This program will load and run one of the individual demos in this
directory within its own frame window.  Just specify the module name
on the command line.
"""

import wx
import wx.lib.mixins.inspect
import sys, os

# stuff for debugging
print "wx.version:", wx.version()
print "pid:", os.getpid()
##raw_input("Press Enter...")

assertMode = wx.PYAPP_ASSERT_DIALOG
##assertMode = wx.PYAPP_ASSERT_EXCEPTION


#----------------------------------------------------------------------------

class Log:
    def WriteText(self, text):
        if text[-1:] == '\n':
            text = text[:-1]
        wx.LogMessage(text)
    write = WriteText


class RunDemoApp(wx.App, wx.lib.mixins.inspect.InspectionMixin):
    def __init__(self, name, module, useShell):
        self.name = name
        self.demoModule = module
        self.useShell = useShell
        wx.App.__init__(self, redirect=False)


    def OnInit(self):
        wx.Log_SetActiveTarget(wx.LogStderr())

        self.SetAssertMode(assertMode)
        self.Init()  # InspectionMixin

        frame = wx.Frame(None, -1, "RunDemo: " + self.name, pos=(50,50), size=(200,100),
                        style=wx.DEFAULT_FRAME_STYLE, name="run a sample")
        frame.CreateStatusBar()

        menuBar = wx.MenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "E&xit\tCtrl-Q", "Exit demo")
        self.Bind(wx.EVT_MENU, self.OnExitApp, item)
        menuBar.Append(menu, "&File")

        ns = {}
        ns['wx'] = wx
        ns['app'] = self
        ns['module'] = self.demoModule
        ns['frame'] = frame
        
        frame.SetMenuBar(menuBar)
        frame.Show(True)
        frame.Bind(wx.EVT_CLOSE, self.OnCloseFrame)

        win = self.demoModule.runTest(frame, frame, Log())

        # a window will be returned if the demo does not create
        # its own top-level window
        if win:
            # so set the frame to a good size for showing stuff
            frame.SetSize((640, 480))
            win.SetFocus()
            self.window = win
            ns['win'] = win
            frect = frame.GetRect()

        else:
            # It was probably a dialog or something that is already
            # gone, so we're done.
            frame.Destroy()
            return True

        self.SetTopWindow(frame)
        self.frame = frame
        #wx.Log_SetActiveTarget(wx.LogStderr())
        #wx.Log_SetTraceMask(wx.TraceMessages)

        if self.useShell:
            # Make a PyShell window, and position it below our test window
            from wx import py
            shell = py.shell.ShellFrame(None, locals=ns)
            frect.OffsetXY(0, frect.height)
            frect.height = 400
            shell.SetRect(frect)
            shell.Show()

            # Hook the close event of the test window so that we close
            # the shell at the same time
            def CloseShell(evt):
                if shell:
                    shell.Close()
                evt.Skip()
            frame.Bind(wx.EVT_CLOSE, CloseShell)
                    
        return True


    def OnExitApp(self, evt):
        self.frame.Close(True)


    def OnCloseFrame(self, evt):
        if hasattr(self, "window") and hasattr(self.window, "ShutdownDemo"):
            self.window.ShutdownDemo()
        evt.Skip()


#----------------------------------------------------------------------------


def main(argv):
    useShell = False
    for x in range(len(sys.argv)):
        if sys.argv[x] in ['--shell', '-shell', '-s']:
            useShell = True
            del sys.argv[x]
            break
            
    if len(argv) < 2:
        print "Please specify a demo module name on the command-line"
        raise SystemExit

    name, ext  = os.path.splitext(argv[1])
    module = __import__(name)


    app = RunDemoApp(name, module, useShell)
    app.MainLoop()



if __name__ == "__main__":
    main(sys.argv)


