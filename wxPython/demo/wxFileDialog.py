
from wxPython.wx import *
import os

#---------------------------------------------------------------------------

wildcard = "Python source (*.py)|*.py|" \
           "Compiled Python (*.pyc)|*.pyc|" \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    log.WriteText("CWD: %s\n" % os.getcwd())
    dlg = wxFileDialog(frame, "Choose a file", os.getcwd(), "", wildcard,
                       wxOPEN
                       | wxMULTIPLE
                       #| wxCHANGE_DIR
                       )
    if dlg.ShowModal() == wxID_OK:
        paths = dlg.GetPaths()
        log.WriteText('You selected %d files:' % len(paths))
        for path in paths:
            log.WriteText('           %s\n' % path)
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

