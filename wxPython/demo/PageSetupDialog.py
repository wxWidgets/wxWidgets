# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wx.PageSetupDialogData()
    data.SetMarginTopLeft( (15, 15) )
    data.SetMarginBottomRight( (15, 15) )
    #data.SetDefaultMinMargins(True)
    data.SetPaperId(wx.PAPER_LETTER)
                               
    dlg = wx.PageSetupDialog(frame, data)

    if dlg.ShowModal() == wx.ID_OK:
        data = dlg.GetPageSetupData()
        tl = data.GetMarginTopLeft()
        br = data.GetMarginBottomRight()
        log.WriteText('Margins are: %s %s\n' % (str(tl), str(br)))

    dlg.Destroy()

#---------------------------------------------------------------------------


overview = """\
This class represents the page setup common dialog. The page setup dialog is standard 
from Windows 95 on, replacing the print setup dialog (which is retained in Windows 
and wxWindows for backward compatibility). On Windows 95 and NT 4.0 and above, 
the page setup dialog is native to the windowing system, otherwise it is emulated.

The page setup dialog contains controls for paper size (A4, A5 etc.), orientation 
(landscape or portrait), and controls for setting left, top, right and bottom margin 
sizes in millimetres.

When the dialog has been closed, you need to query the <code>wxPageSetupDialogData</code> object 
associated with the dialog.

Note that the OK and Cancel buttons do not destroy the dialog; this must be done by 
the application. As with other dialogs, do not destroy the dialog until you are done
with the data, and, conversely, do not use the wxPageSetupDialogData after the
dialog is destroyed.


"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
