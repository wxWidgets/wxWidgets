
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxPageSetupDialogData()
    data.SetMarginTopLeft( (15, 15) )
    data.SetMarginBottomRight( (15, 15) )
    #data.SetDefaultMinMargins(True)
    data.SetPaperId(wxPAPER_LETTER)
                               
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






if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
