
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxDirDialog(frame, "Choose a directory:",
                      style=wxDD_DEFAULT_STYLE|wxDD_NEW_DIR_BUTTON)
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetPath())
    dlg.Destroy()

#---------------------------------------------------------------------------





overview = """\
This class represents the directory chooser dialog.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

