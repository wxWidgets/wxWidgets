
from wxPython.wx import *
import string

#---------------------------------------------------------------------------

wildcard = "Python source (*.py)|*.py|" \
           "Compiled Python (*.pyc)|*.pyc|" \
           "All files (*.*)|*.*"

def runTest(frame, nb, log):
    dlg = wxFileDialog(frame, "Choose a file", "", "", wildcard, wxOPEN|wxMULTIPLE)
    if dlg.ShowModal() == wxID_OK:
        for path in dlg.GetPaths():
            log.WriteText('You selected: %s\n' % path)
    dlg.Destroy()

#---------------------------------------------------------------------------














overview = """\
This class represents the file chooser dialog.

"""
