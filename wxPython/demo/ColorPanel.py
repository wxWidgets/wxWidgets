
from wxPython.wx import *

#---------------------------------------------------------------------------


class ColoredPanel(wxWindow):
    def __init__(self, parent, color):
        wxWindow.__init__(self, parent, -1,
                          wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
        self.SetBackgroundColour(color)

#---------------------------------------------------------------------------
