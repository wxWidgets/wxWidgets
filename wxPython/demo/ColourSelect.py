
#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Selection control display testing on panel for wxPython demo
#
# Author:       Lorne White (email: lorne.white@telusplanet.net)
#
# Version       0.6
# Date:         Nov 14, 2001
# Licence:      wxWindows license

# Change Log:  Add Label parameter to accommodate updated library code

#----------------------------------------------------------------------------

from wxPython.wx import *
from wxPython.lib.colourselect import *
import string

#---------------------------------------------------------------------------

class TestColourSelect(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This example uses a colour selection control based on the wxButton and wxColourDialog Classes.  Click Button to get Colour Values",
                               wxPoint(10, 20), wxSize(400, 60))

        self.x_pos = 30
        self.y_pos = 100
        delta = 40

        mID = NewId()
        wxButton(self, mID, "Get All Colours", wxPoint(self.x_pos, self.y_pos))
        EVT_BUTTON(self, mID, self.OnClick)
        self.y_pos = self.y_pos + delta

        wxStaticText(self, -1, "Default", wxPoint(self.x_pos, self.y_pos), wxSize(-1, -1))   # name
        self.colour_def = ColourSelect(self, -1, pos=wxPoint(self.x_pos+100, self.y_pos))   # default colour selection control

        self.y_pos = self.y_pos + delta
        colours = [[255, 255, 0], [255, 0, 255], [0, 255, 0], [0, 0, 255]]   # list of initial colours for display
        self.names = names = [ "Default Size", "Another Size", "Another Colour", "Larger"]    # display names
        sizes = [ wxDefaultSize, wxSize(60, 20), wxDefaultSize, wxSize(60, 60)]       # button sizes
        self.set_val = []

        for i in range(len(colours)):
            wxStaticText(self, -1, names[i], wxPoint(self.x_pos, self.y_pos), wxSize(-1, -1))   # name

            val = ColourSelect(self, -1, "", colours[i], wxPoint(self.x_pos+100, self.y_pos), sizes[i])     # colour selection button
            self.set_val.append(val)     # store control for reference
            self.y_pos = self.y_pos + delta

        self.y_pos = self.y_pos - delta
        ColourSelect(self, -1, "Color Label", colours[0], wxPoint(self.x_pos+200, self.y_pos), sizes[0])     # colour selection button


    def OnClick(self, event):
        result = []
        colour = self.colour_def.GetColour()        # default control value
        result.append("%s: #%02x%02x%02x" % ("Default", colour[0], colour[1], colour[2]))

        for i in range(len(self.set_val)):
            val = self.set_val[i]
            colour = val.GetColour()        # get the colour selection button result
            name = self.names[i]
            result.append("%s: #%02x%02x%02x" % (name, colour[0], colour[1], colour[2]))
        out_result = string.joinfields(result, ',  ')
        self.log.WriteText("Colour Results :" + out_result + "\n")

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestColourSelect(nb, log)
    return win

#---------------------------------------------------------------------------















overview = """\

"""
