"""
wxPyColourChooser
Copyright (C) 2002 Michael Gilfix <mgilfix@eecs.tufts.edu>

This file is part of wxPyColourChooser.

This version of wxPyColourChooser is open source; you can redistribute it
and/or modify it under the licensed terms.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

from wxPython.wx import *

class PyColourBox(wxPanel):
    """A Colour Selection Box

    The Colour selection box implements button like behavior but contains
    a solid-filled, coloured sub-box. Placing the colour in a sub-box allows
    for filling in the main panel's background for a high-lighting effect.
    """
    def __init__(self, parent, id, colour=(0, 0, 0), size=(25, 20)):
        """Creates a new colour box instance and initializes the colour
        content."""
        wxPanel.__init__(self, parent, id,
                          size=wxSize(size[0], size[1]))

        self.colour_box = wxPanel(self, -1, style=wxSIMPLE_BORDER)

        sizer = wxGridSizer(1, 1)
        sizer.Add(self.colour_box, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL)
        sizer.SetItemMinSize(self.colour_box, size[0] - 5, size[1] - 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        self.Layout()

        self.real_bg = self.GetBackgroundColour()
        self.SetColourTuple(colour)

    def GetColourBox(self):
        """Returns a reference to the internal box object containing the
        color. This function is useful for setting up event handlers for
        the box."""
        return self.colour_box

    def GetColour(self):
        """Returns a wxColour object indicating the box's current colour."""
        return self.colour_box.GetBackgroundColour()

    def SetColour(self, colour):
        """Accepts a wxColour object and sets the box's current color."""
        self.colour_box.SetBackgroundColour(colour)
        self.colour_box.Refresh()

    def SetColourTuple(self, colour):
        """Sets the box's current couple to the given tuple."""
        self.colour = colour
        self.colour_box.SetBackgroundColour(apply(wxColour, self.colour))

    def Update(self):
        wxPanel.Update(self)
        self.colour_box.Update()

    def SetHighlight(self, val):
        """Accepts a boolean 'val' toggling the box's highlighting."""
        # XXX This code has been disabled for now until I can figure out
        # how to get this to work reliably across all platforms.
        if val:
            #A wxColourPtr is returned in windows, making this difficult
            red =(self.real_bg.Red() - 45) % 255
            green =(self.real_bg.Green() - 45) % 255
            blue =(self.real_bg.Blue() - 45) % 255
            new_colour = wxColour(red, green, blue)
            self.SetBackgroundColour(new_colour)
        else:
            self.SetBackgroundColour(self.real_bg)
        self.Refresh()
