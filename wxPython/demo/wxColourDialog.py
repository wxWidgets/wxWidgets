
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxColourData()
    data.SetChooseFull(true)
    dlg = wxColourDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        data = dlg.GetColourData()
        log.WriteText('You selected: %s\n' % str(data.GetColour().Get()))
    dlg.Destroy()

#---------------------------------------------------------------------------












overview = """\
This class represents the colour chooser dialog.

wxColourDialog()
------------------------------

wxColourDialog(wxWindow* parent, wxColourData* data = NULL)

Constructor. Pass a parent window, and optionally a pointer to a block of colour data, which will be copied to the colour dialog's colour data.

"""
