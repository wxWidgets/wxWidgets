
from wxPython.wx          import *
from wxPython.lib.dialogs import wxScrolledMessageDialog

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    f = open("Main.py", "r")
    msg = f.read()
    dlg = wxScrolledMessageDialog(frame, msg, "message test")
    dlg.ShowModal()

#---------------------------------------------------------------------------









overview = """\
"""
