
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxFileDialog(frame, "Choose a file", ".", "", "*.*", wxOPEN|wxMULTIPLE)
    if dlg.ShowModal() == wxID_OK:
        for path in dlg.GetPaths():
            log.WriteText('You selected: %s\n' % path)
    dlg.Destroy()

#---------------------------------------------------------------------------














overview = """\
This class represents the file chooser dialog.

"""
