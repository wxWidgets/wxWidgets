
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxPrintDialogData()
    data.EnablePrintToFile(True)
    data.EnablePageNumbers(True)
    data.EnableSelection(True)
    dlg = wxPrintDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('\n')
    dlg.Destroy()

#---------------------------------------------------------------------------




overview = """\
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
