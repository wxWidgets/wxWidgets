
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxFontDialog(frame)
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

wxFontDialog(wxWindow* parent, wxFontData* data = NULL)

Constructor. Pass a parent window, and optionally a pointer to a block of font data, which will be copied to the font dialog's font data.
"""
