
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxPrintDialogData()
    data.EnablePrintToFile(True)
    data.EnablePageNumbers(True)
    data.EnableSelection(True)
    data.SetMinPage(1)
    data.SetMaxPage(5)
    dlg = wxPrintDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        data = dlg.GetPrintDialogData()
        log.WriteText('GetAllPages: %d\n' % data.GetAllPages())
    dlg.Destroy()

#---------------------------------------------------------------------------




overview = """\
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
