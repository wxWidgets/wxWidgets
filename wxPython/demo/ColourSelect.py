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
# 

import  wx
import  wx.lib.colourselect as  csel

#----------------------------------------------------------------------------

class TestColourSelect(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.SetAutoLayout(True)
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(mainSizer)
        t = wx.StaticText(self, -1,
                         "This example uses a colour selection control based on the wxButton\n"
                         "and wxColourDialog Classes.  Click Button to get Colour Values")
        mainSizer.Add(t, 0, wx.ALL, 3)

        b = wx.Button(self, -1, "Show All Colours")
        self.Bind(wx.EVT_BUTTON, self.OnShowAll, id=b.GetId())
        mainSizer.Add(b, 0, wx.ALL, 3)

        buttonSizer = wx.FlexGridSizer(1, 2) # sizer to contain all the example buttons

        # show a button with all default values
        self.colourDefaults = csel.ColourSelect(self, -1)

        self.Bind(csel.EVT_COLOURSELECT, self.OnSelectColour, id=self.colourDefaults.GetId())
        
        buttonSizer.AddMany([
            (wx.StaticText(self, -1, "Default Colour/Size"), 0, wx.ALIGN_RIGHT | wx.ALIGN_CENTER_VERTICAL),
            (self.colourDefaults, 0, wx.ALL, 3),
            ])

        # build several examples of buttons with different colours and sizes
        buttonData = [
            ("Default Size",         (255, 255, 0),   wx.DefaultSize, ""),
            ("Another Size",         (255, 0, 255),   (60, 20),      ""),
            ("Another Colour",       (0, 255, 0),     wx.DefaultSize, ""),
            ("Larger Size",          (0, 0, 255),     (60, 60),      ""),
            ("With a Label",         (127, 0, 255),   wx.DefaultSize, "Colour..."),
            ("Another Colour/Label", (255, 100, 130), (120, -1),     "Choose Colour..."),
            ]

        self.buttonRefs = [] # for saving references to buttons

        # build each button and save a reference to it
        for name, color, size, label in buttonData:
            b = csel.ColourSelect(self, -1, label, color, size = size)

            b.Bind(csel.EVT_COLOURSELECT, self.OnSelectColour)
            self.buttonRefs.append((name, b))  # store reference to button

            buttonSizer.AddMany([
                (wx.StaticText(self, -1, name), 0, wx.ALIGN_RIGHT | wx.ALIGN_CENTER_VERTICAL),
                (b, 0, wx.ALL, 3),
                ])

        mainSizer.Add(buttonSizer, 0, wx.ALL, 3)
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
A coloured button that when clicked allows the user to select a colour from the wxColourDialog.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

