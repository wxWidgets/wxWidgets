
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    data = wxFontData()
    dlg = wxFontDialog(frame, data)
    if dlg.ShowModal() == wxID_OK:
        data = dlg.GetFontData()
        font = data.GetChosenFont()
        log.WriteText('You selected: "%s", %d points, color %s\n' %
                           (font.GetFaceName(), font.GetPointSize(),
                            data.GetColour().Get()))
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
This class represents the font chooser dialog.

wxFontDialog()
----------------------------

wxFontDialog(wxWindow* parent, wxFontData* data)

Constructor. Pass a parent window and a font data object, which will be copied to the font dialog's font data.
"""
