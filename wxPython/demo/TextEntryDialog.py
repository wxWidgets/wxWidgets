
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a TextEntryDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        dlg = wx.TextEntryDialog(
                self, 'What is your favorite programming language?',
                'Eh??', 'Python')

        dlg.SetValue("Python is the best!")

        if dlg.ShowModal() == wx.ID_OK:
            self.log.WriteText('You entered: %s\n' % dlg.GetValue())

        dlg.Destroy()




#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\
This class represents a dialog that requests a one-line text string from the user. 
It is implemented as a generic wxWindows dialog. Along with the usual wx.Dialog 
style flags, all of the wx.TextCtrl TE_* style flags are accepted, so, for example,
wx.TE_PASSWORD could be used to create a password dialog.

As with other dialogs of this type, the user input must be retrieved prior to
destroying the dialog.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
