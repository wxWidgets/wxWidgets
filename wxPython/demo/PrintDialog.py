
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a PrintDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        data = wx.PrintDialogData()

        data.EnableSelection(True)
        data.EnablePrintToFile(True)
        data.EnablePageNumbers(True)
        data.SetMinPage(1)
        data.SetMaxPage(5)
        data.SetAllPages(True)

        dlg = wx.PrintDialog(self, data)

        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetPrintDialogData()
            self.log.WriteText('GetAllPages: %d\n' % data.GetAllPages())

        dlg.Destroy()



#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
This class represents the print and print setup common dialogs. You may obtain 
a wx.PrinterDC device context from a successfully dismissed print dialog.

User information is stored in a wx.PrintDialogData object that is passed to the
dialog at creation time, and it is filled in by the user. As with other dialogs,
do not use this data once the dialog is dismissed, and do not destroy the dialog
until you have everything you need from it.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
