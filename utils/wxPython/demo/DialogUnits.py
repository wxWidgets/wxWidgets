#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         DialogUnits.py
# Purpose:      A minimal wxPython program that is a bit smarter than test1.
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

        # Add a panel and some controls to display the size and position
        panel = wxPanel(self, -1)
        wxStaticText(panel, -1, "Size:",
                     wxDLG_PNT(panel, wxPoint(4, 4)),  wxDefaultSize)
        wxStaticText(panel, -1, "Pos:",
                     wxDLG_PNT(panel, wxPoint(4, 16)), wxDefaultSize)
        self.sizeCtrl = wxTextCtrl(panel, -1, "",
                                   wxDLG_PNT(panel, wxPoint(24, 4)),
                                   wxDLG_SZE(panel, wxSize(36, -1)),
                                   wxTE_READONLY)

        self.posCtrl = wxTextCtrl(panel, -1, "",
                                  wxDLG_PNT(panel, wxPoint(24, 16)),
                                  wxDLG_SZE(panel, wxSize(36, -1)),
                                  wxTE_READONLY)

        #print wxDLG_PNT(panel, wxPoint(24, 4)), wxDLG_SZE(panel, wxSize(36, -1))
        #print wxDLG_PNT(panel, wxPoint(24, 16)),wxDLG_SZE(panel, wxSize(36, -1))


    # This method is called automatically when the CLOSE event is
    # sent to this window
    def OnCloseWindow(self, event):
        # tell the window to kill itself
        self.Destroy()


    # This method is called by the System when the window is resized,
    # because of the association above.
    def OnSize(self, event):
        size = event.GetSize()
        self.sizeCtrl.SetValue("%s, %s" % (size.width, size.height))

        # tell the event system to continue looking for an event handler,
        # so the default handler will get called.
        event.Skip()

    # This method is called by the System when the window is moved,
    # because of the association above.
    def OnMove(self, event):
        pos = event.GetPosition()
        self.posCtrl.SetValue("%s, %s" % (pos.x, pos.y))



#---------------------------------------------------------------------------
# if running standalone

if __name__ == "__main__":
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


    app = MyApp(0)     # Create an instance of the application class
    app.MainLoop()     # Tell it to start processing events


#---------------------------------------------------------------------------
# if running as part of the Demo Framework...

def runTest(frame, nb, log):
    win = MyFrame(frame, -1, "This is a test")
    frame.otherWin = win
    win.Show(true)


overview = """\
A simple example that shows how to use Dialog Units.
"""

#----------------------------------------------------------------------------
#









