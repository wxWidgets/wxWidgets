# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx
#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wx.PrintDialogData()

    data.EnableSelection(True)
    data.EnablePrintToFile(True)
    data.EnablePageNumbers(True)
    data.SetMinPage(1)
    data.SetMaxPage(5)
    data.SetAllPages(True)

    dlg = wx.PrintDialog(frame, data)

    if dlg.ShowModal() == wx.ID_OK:
        data = dlg.GetPrintDialogData()
        log.WriteText('GetAllPages: %d\n' % data.GetAllPages())

    dlg.Destroy()

#---------------------------------------------------------------------------


overview = """\
This class represents the print and print setup common dialogs. You may obtain 
a wxPrinterDC device context from a successfully dismissed print dialog.

User information is stored in a wxPrintDialogData object that is passed to the
dialog at creation time, and it is filled in by the user. As with other dialogs,
do not use this data once the dialog is dismissed, and do not destroy the dialog
until you have everything you need from it.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
