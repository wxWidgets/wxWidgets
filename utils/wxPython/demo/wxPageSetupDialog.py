
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxPageSetupDialogData()
    data.SetMarginTopLeft(wxPoint(50,50))
    data.SetMarginBottomRight(wxPoint(50,50))
    dlg = wxPageSetupDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        data = dlg.GetPageSetupData()
        tl = data.GetMarginTopLeft()
        br = data.GetMarginBottomRight()
        log.WriteText('Margins are: %s %s\n' % (str(tl), str(br)))
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
"""
