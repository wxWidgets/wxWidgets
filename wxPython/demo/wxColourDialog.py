
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxColourDialog(frame)
    dlg.GetColourData().SetChooseFull(True)
    if dlg.ShowModal() == wxID_OK:
        data = dlg.GetColourData()
        log.WriteText('You selected: %s\n' % str(data.GetColour().Get()))
    dlg.Destroy()

#---------------------------------------------------------------------------












overview = """\
This class represents the colour chooser dialog.


"""
