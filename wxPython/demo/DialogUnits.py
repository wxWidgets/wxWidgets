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
#

import  wx

#---------------------------------------------------------------------------

# Create a new frame class, derived from the wxPython Frame.
class MyFrame(wx.Frame):

    def __init__(self, parent, id, title):
        # First, call the base class' __init__ method to create the frame
        wx.Frame.__init__(self, parent, id, title, (100, 100), (160, 100))

        # Associate some events with methods of this class
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_MOVE, self.OnMove)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        # Add a panel and some controls to display the size and position
        panel = wx.Panel(self, -1)

        wx.StaticText(panel, -1, "Size:", 
            wx.DLG_PNT(panel, (4, 4)),  wx.DefaultSize
            )

        wx.StaticText(panel, -1, "Pos:", 
            wx.DLG_PNT(panel, (4, 16)), wx.DefaultSize
            )

        self.sizeCtrl = wx.TextCtrl(panel, -1, "", 
                            wx.DLG_PNT(panel, (24, 4)),
                            wx.DLG_SZE(panel, (36, -1)),
                            wx.TE_READONLY)

        self.posCtrl = wx.TextCtrl(panel, -1, "", 
                            wx.DLG_PNT(panel, (24, 16)),
                            wx.DLG_SZE(panel, (36, -1)),
                            wx.TE_READONLY)

        #print wx.DLG_PNT(panel, (24, 4)), wx.DLG_SZE(panel, (36, -1))
        #print wx.DLG_PNT(panel, (24, 16)),wx.DLG_SZE(panel, (36, -1))

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
    class MyApp(wx.App):

        # wxWindows calls this method to initialize the application
        def OnInit(self):

            # Create an instance of our customized Frame class
            frame = MyFrame(None, -1, "This is a test")
            frame.Show(True)

            # Tell wxWindows that this is our main window
            self.SetTopWindow(frame)

            # Return a success flag
            return True


    app = MyApp(0)     # Create an instance of the application class
    app.MainLoop()     # Tell it to start processing events


#---------------------------------------------------------------------------
# if running as part of the Demo Framework...

def runTest(frame, nb, log):
    win = MyFrame(frame, -1, "This is a test")
    frame.otherWin = win
    win.Show(True)


overview = """\
A simple example that shows how to use Dialog Units.
"""

#----------------------------------------------------------------------------
#









