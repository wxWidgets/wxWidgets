# 11/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/28/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Changed the event binding slightly.


import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        self.fbtn = wx.Button(self, -1, "Show Find Dialog", (25, 50))
        self.Bind(wx.EVT_BUTTON, self.OnShowFind, self.fbtn)

        self.frbtn = wx.Button(self, -1, "Show Find && Replace Dialog", (25, 90))
        self.Bind(wx.EVT_BUTTON, self.OnShowFindReplace, self.frbtn)

        self.Bind(wx.EVT_FIND, self.OnFind)
        self.Bind(wx.EVT_FIND_NEXT, self.OnFind)
        self.Bind(wx.EVT_FIND_REPLACE, self.OnFind)
        self.Bind(wx.EVT_FIND_REPLACE_ALL, self.OnFind)
        self.Bind(wx.EVT_FIND_CLOSE, self.OnFindClose)

    def EnableButtons(self):
        self.fbtn.Enable()
        self.frbtn.Enable()

    def DisableButtons(self):
        self.fbtn.Disable()
        self.frbtn.Disable()

    def OnShowFind(self, evt):
        self.DisableButtons()
        data = wx.FindReplaceData()
        dlg = wx.FindReplaceDialog(self, data, "Find")
        dlg.data = data  # save a reference to it...
        dlg.Show(True)


    def OnShowFindReplace(self, evt):
        self.DisableButtons()
        data = wx.FindReplaceData()
        dlg = wx.FindReplaceDialog(self, data, "Find & Replace", wx.FR_REPLACEDIALOG)
        dlg.data = data  # save a reference to it...
        dlg.Show(True)


    def OnFind(self, evt):
        map = {
            wx.wxEVT_COMMAND_FIND : "FIND",
            wx.wxEVT_COMMAND_FIND_NEXT : "FIND_NEXT",
            wx.wxEVT_COMMAND_FIND_REPLACE : "REPLACE",
            wx.wxEVT_COMMAND_FIND_REPLACE_ALL : "REPLACE_ALL",
            }

        et = evt.GetEventType()
        
        if et in map:
            evtType = map[et]
        else:
            evtType = "**Unknown Event Type**"

        if et in [wx.wxEVT_COMMAND_FIND_REPLACE, wx.wxEVT_COMMAND_FIND_REPLACE_ALL]:
            replaceTxt = "Replace text: %s" % evt.GetReplaceString()
        else:
            replaceTxt = ""

        self.log.write("%s -- Find text: %s   Replace text: %s  Flags: %d  \n" %
                       (evtType, evt.GetFindString(), replaceTxt, evt.GetFlags()))


    def OnFindClose(self, evt):
        self.log.write("FindReplaceDialog closing...\n")
        evt.GetDialog().Destroy()
        self.EnableButtons()
        

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
FindReplaceDialog is a standard modeless dialog which is used to allow the user 
to search for some text (and possibly replace it with something else). The actual 
searching is supposed to be done in the owner window which is the parent of this 
dialog. Note that it means that unlike for the other standard dialogs this one 
<u>must have a parent window</u>. Also note that there is no way to use this 
dialog in a modal way; <b>it is always, by design and implementation, modeless</b>.

FileReplaceDialog requires the use of <b>FindReplaceData</b>. This holds the 
data for the dialog. It is used to initialize the dialog with the default values 
and will keep the last values from the dialog when it is closed. It is also 
updated each time a FindDialogEvent is generated so instead of using the 
FindDialogEvent methods you can also directly query this object. <b>Care must be
taken not to use this object after the dialog is destroyed.</b> The data within
will be invalid after the parent dialog is destroyed.
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

