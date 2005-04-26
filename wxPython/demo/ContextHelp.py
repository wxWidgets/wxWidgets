
import  wx

#----------------------------------------------------------------------
# We first have to set an application-wide help provider.  Normally you
# would do this in your app's OnInit or in other startup code...

provider = wx.SimpleHelpProvider()
wx.HelpProvider_Set(provider)

# This panel is chock full of controls about which we can demonstrate the 
# help system.
class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        # This help text, set for the panel itself, will be used if context
        # sensitive help cannot be found for any particular control.
        self.SetHelpText("This is a wx.Panel.")
        
        sizer = wx.BoxSizer(wx.VERTICAL)

        # Init the context help button.
        # And even include help text about the help button :-)
        cBtn = wx.ContextHelpButton(self)
        cBtn.SetHelpText("wx.ContextHelpButton")

        cBtnText = wx.StaticText(self, -1, 
            "This is a wx.ContextHelpButton.  Clicking it puts the\n"
            "app into context sensitive help mode."
            )

        # Yes, even static text can have help text associated with it :-)
        cBtnText.SetHelpText("Some helpful text...")

        s = wx.BoxSizer(wx.HORIZONTAL)
        s.Add(cBtn, 0, wx.ALL, 5)
        s.Add(cBtnText, 0, wx.ALL, 5)
        sizer.Add((20,20))
        sizer.Add(s)

        # A text control with help text.
        text = wx.TextCtrl(self, -1, "Each sub-window can have its own help message",
                          size=(240, 60), style=wx.TE_MULTILINE)
        text.SetHelpText("This is my very own help message.  This is a really long long long long long long long long long long long long long long long long long long long long message!")
        sizer.Add((20,20))
        sizer.Add(text)

        # Same thing, but this time to demonstrate how the help event can be 
        # intercepted.
        text = wx.TextCtrl(self, -1, "You can also intercept the help event if you like.  Watch the log window when you click here...",
                          size=(240, 60), style = wx.TE_MULTILINE)
        text.SetHelpText("Yet another context help message.")
        sizer.Add((20,20))
        sizer.Add(text)
        text.Bind(wx.EVT_HELP, self.OnCtxHelp, text)

        text = wx.TextCtrl(self, -1, "This one displays the tip itself...",
                           size=(240, 60), style = wx.TE_MULTILINE)
        sizer.Add((20,20))
        sizer.Add(text)
        text.Bind(wx.EVT_HELP, self.OnCtxHelp2, text)
        

        border = wx.BoxSizer(wx.VERTICAL)
        border.Add(sizer, 0, wx.ALL, 25)

        self.SetAutoLayout(True)
        self.SetSizer(border)
        self.Layout()


    # On the second text control above, we intercept the help event. This is where
    # we process it. Anything could happen here. In this case we're just printing
    # some stuff about it, then passing it on, at which point we see the help tip.
    def OnCtxHelp(self, evt):
        self.log.write("OnCtxHelp: %s" % evt)
        evt.Skip()

    # On the third text control above, we intercept the help event. 
    # Here, we print a note about it, generate our own tip window, and, 
    # unlike last time, we don't pass it on to the underlying provider.
    def OnCtxHelp2(self, evt):
         self.log.write("OnCtxHelp2: %s\n" % evt)
         tip = wx.TipWindow(self, "This is a wx.TipWindow")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------


overview = """
This demo shows how to incorporate Context Sensitive
help into your application using the wx.SimpleHelpProvider class.

"""

#----------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

