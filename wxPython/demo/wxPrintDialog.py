
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxPrintDialogData()
    data.EnablePrintToFile(true)
    data.EnablePageNumbers(true)
    data.EnableSelection(true)
    dlg = wxPrintDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('\n')
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
"""
