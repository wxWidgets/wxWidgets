#!/bin/env python
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


import sys
from wxPython.wx import *

#----------------------------------------------------------------------------

class Log:
    def WriteText(self, text):
        sys.stdout.write(text)
    write = WriteText


class RunDemoApp(wxApp):
    def __init__(self, name, module):
        self.name = name
        self.demoModule = module
        wxApp.__init__(self, 0)

    def OnInit(self):
        wxInitAllImageHandlers()
        frame = wxFrame(None, -1, "RunDemo: " + self.name, size=(0,0))
        frame.CreateStatusBar()
        frame.Show(true)
        win = self.demoModule.runTest(frame, frame, Log())

        # a window will be returned if the demo does not create
        # its own top-level window
        if win:
            # so set the frame to a good size for showing stuff
            frame.SetSize((600, 450))

        else:
            # otherwise the demo made its own frame, so just put a
            # button in this one
            if hasattr(frame, 'otherWin'):
                wxButton(frame, 1101, " Exit ")
                frame.SetSize((200, 100))
                EVT_BUTTON(frame, 1101, self.OnButton)
            else:
                # It was probably a dialog or something that is already
                # gone, so we're done.
                frame.Destroy()
                return true

        self.SetTopWindow(frame)
        self.frame = frame
        return true


    def OnButton(self, evt):
        self.frame.Close(true)

#----------------------------------------------------------------------------


def main():
    if len(sys.argv) != 2:
        print "Please specify a demo module name on the command-line"
        raise SystemExit

    name = sys.argv[1]
    module = __import__(name)


    app = RunDemoApp(name, module)
    app.MainLoop()



if __name__ == "__main__":
    main()


