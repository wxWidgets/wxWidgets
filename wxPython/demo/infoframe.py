
import  sys

import  wx
import  wx.lib.infoframe

#----------------------------------------------------------------------

class MyFrame(wx.Frame):
    def __init__(self, output):
        wx.Frame.__init__(self, None, -1, "Close me...", size=(300,100))

        menubar = wx.MenuBar()

        # Output menu
        menu = wx.Menu()

        # Enable output menu item
        mID = wx.NewId()
        menu.Append(mID, "&Enable output", "Display output frame")
        self.Bind(wx.EVT_MENU, output.EnableOutput, id=mID)

        # Disable output menu item
        mID = wx.NewId()
        menu.Append(mID, "&Disable output", "Close output frame")
        self.Bind(wx.EVT_MENU, output.DisableOutput, id=mID)

        # Attach the menu to our menu bar
        menubar.Append(menu, "&Output")
        
        # Attach menu bar to frame
        self.SetMenuBar(menubar)
        
        # Point to ourselves as the output object's parent.
        output.SetParent(self)
        
        # Associate menu bar with output object
        output.SetOtherMenuBar(menubar, menuname="Output")

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        # We're going to set up a timer; set up an event handler for it.
        self.Bind(wx.EVT_TIMER, self.OnTimer)

        # Set up a timer for demo purposes
        self.timer = wx.Timer(self, -1)
        self.timer.Start(1000)

        # Get a copy of stdout and set it aside. We'll use it later.
        self.save_stdout = sys.stdout

        # Now point to the output object for stdout
        sys.stdout = self.output = output
        # ... and use it.
        print "Hello!"

    def OnClose(self,event):
        # We stored a pointer to the original stdout above in .__init__(), and
        # here we restore it before closing the window.
        sys.stdout = self.save_stdout

        # Clean up
        self.output.close()
        self.timer.Stop()
        self.timer = None

        self.Destroy()

    # Event handler for timer events.
    def OnTimer(self, evt):
        print "This was printed with \"print\""


#----------------------------------------------------------------------

overview = wx.lib.infoframe.__doc__

def runTest(frame, nb, log):
    """
    This method is used by the wxPython Demo Framework for integrating
    this demo with the rest.
    """
    win = MyFrame(wx.lib.infoframe.PyInformationalMessagesFrame())
    frame.otherWin = win
    win.Show(1)

#----------------------------------------------------------------------

if __name__ == "__main__":
##     class MyFrame(wxFrame):
##         def __init__(self,output):
##             wxFrame.__init__(self,None,-1,"Close me...",size=(300,100))
##             EVT_CLOSE(self,self.OnClose)
##             menubar = wxMenuBar()
##             menu = wxMenu()
##             mID = wxNewId()
##             menu.Append(mID,"&Enable output","Display output frame")
##             EVT_MENU(self,mID,output.EnableOutput)
##             mID = wxNewId()
##             menu.Append(mID,"&Disable output","Close output frame")
##             EVT_MENU(self,mID,output.DisableOutput)
##             menubar.Append(menu,"&Output")
##             self.SetMenuBar(menubar)
##             output.SetOtherMenuBar(menubar,menuname="Output")

##         def OnClose(self,event):
##             if isinstance(sys.stdout,wx.lib.infoframe.PyInformationalMessagesFrame):
##                 sys.stdout.close()
##             self.Destroy()

    class MyApp(wx.App):
        
        # Override the default output window and point it to the
        # custom class.
        outputWindowClass = wx.lib.infoframe.PyInformationalMessagesFrame
        
        def OnInit(self):
            
            # At this point, we should probably check to see if self.stdioWin
            # is actually pointed to something. By default, wx.App() sets this
            # attribute to None. This causes problems when setting up the menus
            # in MyFrame() above. On the other hand, since there's little that
            # can be done at this point, you might be better served putting
            # an error handler directly into MyFrame().
            #
            # That's in practice. In the case of this demo, the whole point
            # of the exercise is to demonstrate the window, so we're being 
            # just a little lazy for clarity's sake. But do be careful in
            # a 'real world' implementation :-)
            
            frame = MyFrame(self.stdioWin)
            frame.Show(True)
            self.SetTopWindow(frame)
            
            # Associate the frame with stdout.
            if isinstance(sys.stdout, wx.lib.infoframe.PyInformationalMessagesFrame):
                sys.stdout.SetParent(frame)

            print "Starting.\n",
            return True

    # *extremely important*
    # 
    # In this demo, if the redirect flag is set to False, the infoframe will not
    # be created or used. All output will go to the default stdout, which in this
    # case will cause the app to throw an exception. In a real app, you should
    # probably plan ahead and add a check before forging ahead. See suggestion above.
    app = MyApp(True)
    app.MainLoop()
