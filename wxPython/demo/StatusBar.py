
import  time
import  wx

#---------------------------------------------------------------------------

class CustomStatusBar(wx.StatusBar):
    def __init__(self, parent, log):
        wx.StatusBar.__init__(self, parent, -1)

        # This status bar has three fields
        self.SetFieldsCount(3)
        # Sets the three fields to be relative widths to each other.
        self.SetStatusWidths([-2, -1, -2])
        self.log = log
        self.sizeChanged = False
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_IDLE, self.OnIdle)

        # Field 0 ... just text
        self.SetStatusText("A Custom StatusBar...", 0)

        # This will fall into field 1 (the second field)
        self.cb = wx.CheckBox(self, 1001, "toggle clock")
        self.Bind(wx.EVT_CHECKBOX, self.OnToggleClock, self.cb)
        self.cb.SetValue(True)

        # set the initial position of the checkbox
        self.Reposition()

        # We're going to use a timer to drive a 'clock' in the last
        # field.
        self.timer = wx.PyTimer(self.Notify)
        self.timer.Start(1000)
        self.Notify()


    # Handles events from the timer we started in __init__().
    # We're using it to drive a 'clock' in field 2 (the third field).
    def Notify(self):
        t = time.localtime(time.time())
        st = time.strftime("%d-%b-%Y   %I:%M:%S", t)
        self.SetStatusText(st, 2)
        self.log.WriteText("tick...\n")


    # the checkbox was clicked
    def OnToggleClock(self, event):
        if self.cb.GetValue():
            self.timer.Start(1000)
            self.Notify()
        else:
            self.timer.Stop()


    def OnSize(self, evt):
        self.Reposition()  # for normal size events

        # Set a flag so the idle time handler will also do the repositioning.
        # It is done this way to get around a buglet where GetFieldRect is not
        # accurate during the EVT_SIZE resulting from a frame maximize.
        self.sizeChanged = True


    def OnIdle(self, evt):
        if self.sizeChanged:
            self.Reposition()


    # reposition the checkbox
    def Reposition(self):
        rect = self.GetFieldRect(1)
        self.cb.SetPosition((rect.x+2, rect.y+2))
        self.cb.SetSize((rect.width-4, rect.height-4))
        self.sizeChanged = False



class TestCustomStatusBar(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, 'Test Custom StatusBar')

        self.sb = CustomStatusBar(self, log)
        self.SetStatusBar(self.sb)
        tc = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY|wx.TE_MULTILINE)

        self.SetSize((640, 480))
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.sb.timer.Stop()
        del self.sb.timer
        self.Destroy()

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the StatusBar sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = TestCustomStatusBar(self, self.log)
        win.Show(True)

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = """\
A status bar is a narrow window that can be placed along the bottom of
a frame to give small amounts of status information. It can contain
one or more fields, one or more of which can be variable length
according to the size of the window.  

This example demonstrates how to create a custom status bar with actual
gadgets embedded in it. In this case, the first field is just plain text,
The second one has a checkbox that enables the timer, and the third
field has a clock that shows the current time when it is enabled.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
