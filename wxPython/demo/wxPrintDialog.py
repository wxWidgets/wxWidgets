
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
