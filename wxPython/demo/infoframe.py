
from wxPython.wx import *
from wxPython.lib.infoframe import *

#----------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self,output):
        wxFrame.__init__(self,None,-1,"Close me...",size=(300,100))
        menubar = wxMenuBar()
        menu = wxMenu()
        mID = NewId()
        menu.Append(mID,"&Enable output","Display output frame")
        EVT_MENU(self,mID,output.EnableOutput)
        mID = NewId()
        menu.Append(mID,"&Disable output","Close output frame")
        EVT_MENU(self,mID,output.DisableOutput)
        menubar.Append(menu,"&Output")
        self.SetMenuBar(menubar)
        output.SetParent(self)
        output.SetOtherMenuBar(menubar,menuname="Output")
        EVT_CLOSE(self,self.OnClose)
        EVT_TIMER(self, -1, self.OnTimer)

        self.timer = wxTimer(self, -1)
        self.timer.Start(1000)

        self.save_stdout = sys.stdout
        sys.stdout = self.output = output
        print "Hello!"

    def OnClose(self,event):
        sys.stdout = self.save_stdout
        self.output.close()
        self.timer.Stop()
        self.timer = None
        self.Destroy()

    def OnTimer(self, evt):
        print "This was printed with \"print\""


#----------------------------------------------------------------------

from wxPython.lib import infoframe
overview = infoframe.__doc__

def runTest(frame, nb, log):
    """
    This method is used by the wxPython Demo Framework for integrating
    this demo with the rest.
    """
    win = MyFrame(wxPyInformationalMessagesFrame())
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
##             mID = NewId()
##             menu.Append(mID,"&Enable output","Display output frame")
##             EVT_MENU(self,mID,output.EnableOutput)
##             mID = NewId()
##             menu.Append(mID,"&Disable output","Close output frame")
##             EVT_MENU(self,mID,output.DisableOutput)
##             menubar.Append(menu,"&Output")
##             self.SetMenuBar(menubar)
##             output.SetOtherMenuBar(menubar,menuname="Output")

##         def OnClose(self,event):
##             if isinstance(sys.stdout,wxPyInformationalMessagesFrame):
##                 sys.stdout.close()
##             self.Destroy()

    class MyApp(wxApp):
        outputWindowClass = wxPyInformationalMessagesFrame
        def OnInit(self):
            frame = MyFrame(self.stdioWin)
            frame.Show(TRUE)
            self.SetTopWindow(frame)
            if isinstance(sys.stdout,wxPyInformationalMessagesFrame):
                sys.stdout.SetParent(frame)
                #self.redirectStdio(None)# this is done automatically
                # by the MyApp(1) call below
            print "Starting.\n",
            return true

    app = MyApp(1)
    app.MainLoop()
