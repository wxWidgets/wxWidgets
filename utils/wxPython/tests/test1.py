#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test1.py
# Purpose:      A minimal wxPython program
#
# Author:       Robin Dunn
#
# Created:      A long time ago, in a galaxy far, far away...
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


## import all of the wxPython GUI package
from wxPython.wx import *


#---------------------------------------------------------------------------

## Create a new frame class, derived from the wxPython Frame.
class MyFrame(wxFrame):

    def __init__(self, parent, id, title):
        # First, call the base class' __init__ method to create the frame
        wxFrame.__init__(self, parent, id, title,
                         wxPoint(100, 100), wxSize(160, 100))

        # Associate some events with methods of this class
        EVT_SIZE(self, self.OnSize)
        EVT_MOVE(self, self.OnMove)


    # This method is called automatically when the CLOSE event is
    # sent to this window
    def OnCloseWindow(self, event):
        # tell the window to kill itself
        self.Destroy()


    # This method is called by the System when the window is resized,
    # because of the association above.
    def OnSize(self, event):
        size = event.GetSize()
        print "size:", size.width, size.height

    # This method is called by the System when the window is moved,
    # because of the association above.
    def OnMove(self, event):
        pos = event.GetPosition()
        print "pos:", pos.x, pos.y



#---------------------------------------------------------------------------

# Every wxWindows application must have a class derived from wxApp
class MyApp(wxApp):

    # wxWindows calls this method to initialize the application
    def OnInit(self):

        # Create an instance of our customized Frame class
        frame = MyFrame(NULL, -1, "This is a test")
        frame.Show(true)

        # Tell wxWindows that this is our main window
        self.SetTopWindow(frame)

        # Return a success flag
        return true

#---------------------------------------------------------------------------


app = MyApp(0)     # Create an instance of the application class
app.MainLoop()     # Tell it to start processing events

print 'done!'

#----------------------------------------------------------------------------
#

