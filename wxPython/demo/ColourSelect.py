#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Selection control display testing on panel for wxPython demo
#
# Author:       Lorne White (email: lorne.white@telusplanet.net)
#
# Version       0.6
# Date:         Nov 14, 2001
# Licence:      wxWindows license
#
# Change Log:  Add Label parameter to accommodate updated library code
#
# Cliff Wells (logiplexsoftware@earthlink.net) 2002/03/11
#              - added code to demonstrate EVT_COLOURSELECT
#              - use sizers
#              - other minor "improvements"
#----------------------------------------------------------------------------

from wxPython.wx import *
from wxPython.lib.colourselect import *

#----------------------------------------------------------------------------

class TestColourSelect(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        self.SetAutoLayout(True)
        mainSizer = wxBoxSizer(wxVERTICAL)
        self.SetSizer(mainSizer)
        t = wxStaticText(self, -1,
                         "This example uses a colour selection control based on the wxButton\n"
                         "and wxColourDialog Classes.  Click Button to get Colour Values")
        mainSizer.Add(t, 0, wxALL, 3)

        b = wxButton(self, -1, "Show All Colours")
        EVT_BUTTON(self, b.GetId(), self.OnShowAll)
        mainSizer.Add(b, 0, wxALL, 3)

        buttonSizer = wxFlexGridSizer(1, 2) # sizer to contain all the example buttons

        # show a button with all default values
        self.colourDefaults = ColourSelect(self, -1)
        EVT_COLOURSELECT(self.colourDefaults, self.colourDefaults.GetId(), self.OnSelectColour)
        buttonSizer.AddMany([
            (wxStaticText(self, -1, "Default Colour/Size"), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL),
            (self.colourDefaults, 0, wxALL, 3),
            ])

        # build several examples of buttons with different colours and sizes
        buttonData = [
            ("Default Size",         (255, 255, 0),   wxDefaultSize, ""),
            ("Another Size",         (255, 0, 255),   (60, 20),      ""),
            ("Another Colour",       (0, 255, 0),     wxDefaultSize, ""),
            ("Larger Size",          (0, 0, 255),     (60, 60),      ""),
            ("With a Label",         (127, 0, 255),   wxDefaultSize, "Colour..."),
            ("Another Colour/Label", (255, 100, 130), (120, -1),     "Choose Colour..."),
            ]

        self.buttonRefs = [] # for saving references to buttons

        # build each button and save a reference to it
        for name, color, size, label in buttonData:
            b = ColourSelect(self, -1, label, color, size = size)
            EVT_COLOURSELECT(b, b.GetId(), self.OnSelectColour)
            self.buttonRefs.append((name, b))  # store reference to button
            buttonSizer.AddMany([
                (wxStaticText(self, -1, name), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL),
                (b, 0, wxALL, 3),
                ])

        mainSizer.Add(buttonSizer, 0, wxALL, 3)
        self.Layout()

    def OnSelectColour(self, event):
        self.log.WriteText("Colour selected: %s" % str(event.GetValue()))

    def OnShowAll(self, event):
        # show the state of each button
        result = []
        colour = self.colourDefaults.GetColour() # default control value
        result.append("Default Colour/Size: " + str(colour))

        for name, button in self.buttonRefs:
            colour = button.GetColour() # get the colour selection button result
            result.append(name + ": " + str(colour))  # create string list for easy viewing of results

        out_result = ',  '.join(result)
        self.log.WriteText("Colour Results: " + out_result + "\n")

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestColourSelect(nb, log)
    return win

#---------------------------------------------------------------------------















overview = """\

"""
