
from wxPython.wx import *
import os

#---------------------------------------------------------------------------

wildcard = "Python source (*.py)|*.py|"     \
           "Compiled Python (*.pyc)|*.pyc|" \
           "SPAM files (*.spam)|*.spam|"    \
           "Egg file (*.egg)|*.egg|"        \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    log.WriteText("CWD: %s\n" % os.getcwd())
    dlg = wxFileDialog(frame, "Save file as...", os.getcwd(), "", wildcard,
                       wxSAVE
                       #| wxCHANGE_DIR
                       )
    dlg.SetFilterIndex(2)
    if dlg.ShowModal() == wxID_OK:
        path = dlg.GetPath()
        log.WriteText('You selected "%s"' % path)
    log.WriteText("CWD: %s\n" % os.getcwd())
    dlg.Destroy()

#---------------------------------------------------------------------------







overview = """\
This class provides the file chooser dialog.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

