
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a SingleChoiceDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        dlg = wx.SingleChoiceDialog(
                self, 'Test Single Choice', 'The Caption',
                ['zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight'], 
                wx.CHOICEDLG_STYLE
                )

        if dlg.ShowModal() == wx.ID_OK:
            self.log.WriteText('You selected: %s\n' % dlg.GetStringSelection())

        dlg.Destroy()



#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win
#---------------------------------------------------------------------------




overview = """\
This class represents a dialog that shows a list of strings, and allows the user
to select one. Double-clicking on a list item is equivalent to single-clicking
and then pressing OK.

As with all dialogs, be sure to retrieve the information you need BEFORE you
destroy the dialog.
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

