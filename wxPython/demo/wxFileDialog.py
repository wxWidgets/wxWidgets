
from wxPython.wx import *

#---------------------------------------------------------------------------

wildcard = "Python source (*.py)|*.py|" \
           "Compiled Python (*.pyc)|*.pyc|" \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    dlg = wxFileDialog(frame, "Choose a file", "", "", wildcard, wxOPEN|wxMULTIPLE)
    if dlg.ShowModal() == wxID_OK:
        paths = dlg.GetPaths()
        log.WriteText('You selected %d files:' % len(paths))
        for path in paths:
            log.WriteText('           %s\n' % path)
    dlg.Destroy()

#---------------------------------------------------------------------------














overview = """\
This class represents the file chooser dialog.

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

