
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a PageSetupDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        data = wx.PageSetupDialogData()
        data.SetMarginTopLeft( (15, 15) )
        data.SetMarginBottomRight( (15, 15) )
        #data.SetDefaultMinMargins(True)
        data.SetPaperId(wx.PAPER_LETTER)

        dlg = wx.PageSetupDialog(self, data)

        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetPageSetupData()
            tl = data.GetMarginTopLeft()
            br = data.GetMarginBottomRight()
            self.log.WriteText('Margins are: %s %s\n' % (str(tl), str(br)))

        dlg.Destroy()


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = """\
This class represents the page setup common dialog. The page setup dialog is standard 
from Windows 95 on, replacing the print setup dialog (which is retained in Windows 
and wxWindows for backward compatibility). On Windows 95 and NT 4.0 and above, 
the page setup dialog is native to the windowing system, otherwise it is emulated.

The page setup dialog contains controls for paper size (A4, A5 etc.), orientation 
(landscape or portrait), and controls for setting left, top, right and bottom margin 
sizes in millimetres.

When the dialog has been closed, you need to query the <code>wx.PageSetupDialogData</code> object 
associated with the dialog.

Note that the OK and Cancel buttons do not destroy the dialog; this must be done by 
the application. As with other dialogs, do not destroy the dialog until you are done
with the data, and, conversely, do not use the wx.PageSetupDialogData after the
dialog is destroyed.


"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
