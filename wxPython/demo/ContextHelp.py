
from wxPython.wx import *
from wxPython.help import *

#----------------------------------------------------------------------
# We first have to set an application-wide help provider.  Normally you
# would do this in your app's OnInit or in other startup code...

provider = wxSimpleHelpProvider()
wxHelpProvider_Set(provider)


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        self.SetHelpText("This is a wxPanel.")
        sizer = wxBoxSizer(wxVERTICAL)

        cBtn = wxContextHelpButton(self)
        cBtn.SetHelpText("wxContextHelpButton")
        cBtnText = wxStaticText(self, -1, "This is a wxContextHelpButton.  Clicking it puts the\n"
                                          "app into context sensitive help mode.")
        cBtnText.SetHelpText("Some helpful text...")

        s = wxBoxSizer(wxHORIZONTAL)
        s.Add(cBtn, 0, wxALL, 5)
        s.Add(cBtnText, 0, wxALL, 5)
        sizer.Add(20,20)
        sizer.Add(s)

        text = wxTextCtrl(self, -1, "Each sub-window can have its own help message",
                          size=(240, 60), style = wxTE_MULTILINE)
        text.SetHelpText("This is my very own help message.  This is a really long long long long long long long long long long long long long long long long long long long long message!")
        sizer.Add(20,20)
        sizer.Add(text)

        text = wxTextCtrl(self, -1, "You can also intercept the help event if you like.  Watch the log window when you click here...",
                          size=(240, 60), style = wxTE_MULTILINE)
        text.SetHelpText("Yet another context help message.")
        sizer.Add(20,20)
        sizer.Add(text)
        EVT_HELP(text, text.GetId(), self.OnCtxHelp)

        text = wxTextCtrl(self, -1, "This one displays the tip itself...",
                          size=(240, 60), style = wxTE_MULTILINE)
        sizer.Add(20,20)
        sizer.Add(text)
        EVT_HELP(text, text.GetId(), self.OnCtxHelp2)


        border = wxBoxSizer(wxVERTICAL)
        border.Add(sizer, 0, wxALL, 25)

        self.SetAutoLayout(True)
        self.SetSizer(border)
        self.Layout()


    def OnCtxHelp(self, evt):
        self.log.write("OnCtxHelp: %s" % evt)
        evt.Skip()


    def OnCtxHelp2(self, evt):
         self.log.write("OnCtxHelp: %s\n" % evt)
         tip = wxTipWindow(self, "This is a wxTipWindow")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------




overview = """
This demo shows how to encorporate Context Sensitive
help into your applicaiton using the wxSimpleHelpProvider class.

"""



#----------------------------------------------------------------------

