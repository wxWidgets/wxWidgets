
import  wx
import  wx.lib.dialogs

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a ScrolledMessageDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        f = open("Main.py", "r")
        msg = f.read()
        f.close()

        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, msg, "message test")
        dlg.ShowModal()



#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\

<code><b>ScrolledMessageDialog</b>(parent, msg, caption, pos=wx.DefaultPosition, size=(500,300))</code>

This class represents a message dialog that uses a wxTextCtrl to display the
message. This allows more flexible information display without having to be
as much concerned with layout requirements. A text file can simply be used 

This dialog offers no special attributes or methods beyond those supported
by wxDialog.

"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
