
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





if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
