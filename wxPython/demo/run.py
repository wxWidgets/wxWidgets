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


import sys, os
from wxPython.wx import *

#----------------------------------------------------------------------------

class Log:
    def WriteText(self, text):
        if text[-1:] == '\n':
            text = text[:-1]
        wxLogMessage(text)
    write = WriteText


class RunDemoApp(wxApp):
    def __init__(self, name, module):
        self.name = name
        self.demoModule = module
        wxApp.__init__(self, 0) ##wxPlatform == "__WXMAC__")


    def OnInit(self):
        wxInitAllImageHandlers()
        wxLog_SetActiveTarget(wxLogStderr())

        frame = wxFrame(None, -1, "RunDemo: " + self.name, pos=(50,50), size=(0,0),
                        style=wxNO_FULL_REPAINT_ON_RESIZE|wxDEFAULT_FRAME_STYLE)
        frame.CreateStatusBar()
        menuBar = wxMenuBar()
        menu = wxMenu()
        menu.Append(101, "E&xit\tAlt-X", "Exit demo")
        EVT_MENU(self, 101, self.OnButton)
        menuBar.Append(menu, "&File")
        frame.SetMenuBar(menuBar)
        frame.Show(True)
        EVT_CLOSE(frame, self.OnCloseFrame)

        win = self.demoModule.runTest(frame, frame, Log())

        # a window will be returned if the demo does not create
        # its own top-level window
        if win:
            # so set the frame to a good size for showing stuff
            frame.SetSize((640, 480))
            win.SetFocus()
            self.window = win

        else:
            # otherwise the demo made its own frame, so just put a
            # button in this one
            if hasattr(frame, 'otherWin'):
                b = wxButton(frame, -1, " Exit ")
                frame.SetSize((200, 100))
                EVT_BUTTON(frame, b.GetId(), self.OnButton)
            else:
                # It was probably a dialog or something that is already
                # gone, so we're done.
                frame.Destroy()
                return True

        self.SetTopWindow(frame)
        self.frame = frame
        #wxLog_SetActiveTarget(wxLogStderr())
        #wxLog_SetTraceMask(wxTraceMessages)
        return True


    def OnButton(self, evt):
        self.frame.Close(True)


    def OnCloseFrame(self, evt):
        if hasattr(self, "window") and hasattr(self.window, "ShutdownDemo"):
            self.window.ShutdownDemo()
        evt.Skip()


#----------------------------------------------------------------------------


def main(argv):
    if len(argv) < 2:
        print "Please specify a demo module name on the command-line"
        raise SystemExit

    name = argv[1]
    if name[-3:] == '.py':
        name = name[:-3]
    module = __import__(name)


    app = RunDemoApp(name, module)
    app.MainLoop()



if __name__ == "__main__":
    main(sys.argv)


