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

import pycolourbox
import pypalette
import pycolourslider
from intl import _
from wxPython.wx import *

class wxPyColourChooser(wxPanel):
    """A Pure-Python implementation of the colour chooser dialog.

    The PyColourChooser is a pure python implementation of the colour
    chooser dialog. It's useful for embedding the colour choosing functionality
    inside other widgets, when the pop-up dialog is undesirable. It can also
    be used as a drop-in replacement on the GTK platform, as the native
    dialog is kind of ugly.
    """

    colour_names = [
        'ORANGE',
        'GOLDENROD',
        'WHEAT',
        'SPRING GREEN',
        'SKY BLUE',
        'SLATE BLUE',
        'MEDIUM VIOLET RED',
        'PURPLE',

        'RED',
        'YELLOW',
        'MEDIUM SPRING GREEN',
        'PALE GREEN',
        'CYAN',
        'LIGHT STEEL BLUE',
        'ORCHID',
        'LIGHT MAGENTA',

        'BROWN',
        'YELLOW',
        'GREEN',
        'CADET BLUE',
        'MEDIUM BLUE',
        'MAGENTA',
        'MAROON',
        'ORANGE RED',

        'FIREBRICK',
        'CORAL',
        'FOREST GREEN',
        'AQUAMARINE',
        'BLUE',
        'NAVY',
        'THISTLE',
        'MEDIUM VIOLET RED',

        'INDIAN RED',
        'GOLD',
        'MEDIUM SEA GREEN',
        'MEDIUM BLUE',
        'MIDNIGHT BLUE',
        'GREY',
        'PURPLE',
        'KHAKI',

        'BLACK',
        'MEDIUM FOREST GREEN',
        'KHAKI',
        'DARK GREY',
        'SEA GREEN',
        'LIGHT GREY',
        'MEDIUM SLATE BLUE',
        'WHITE',
    ]

    custom_colours = [ (255, 255, 255) ] * 16
    last_custom = 0

    idADD_CUSTOM = wxNewId()
    idSCROLL     = wxNewId()

    def __init__(self, parent, id):
        """Creates an instance of the colour chooser. Note that it is best to
        accept the given size of the colour chooser as it is currently not
        resizeable."""
        wxPanel.__init__(self, parent, id)

        self.basic_label = wxStaticText(self, -1, _("Basic Colours:"))
        self.custom_label = wxStaticText(self, -1, _("Custom Colours:"))
        self.add_button = wxButton(self, self.idADD_CUSTOM, _("Add to Custom Colours"))

        EVT_BUTTON(self, self.idADD_CUSTOM, self.onAddCustom)

        # Since we're going to be constructing widgets that require some serious
        # computation, let's process any events (like redraws) right now
        wxYield()

        # Create the basic colours palette
        self.colour_boxs = [ ]
        colour_grid = wxGridSizer(6, 8)
        for name in self.colour_names:
            new_id = wxNewId()
            box = pycolourbox.PyColourBox(self, new_id)
            EVT_LEFT_DOWN(box.GetColourBox(), lambda x, b=box: self.onBasicClick(x, b))
            self.colour_boxs.append(box)
            colour_grid.Add(box, 0, wxEXPAND)

        # Create the custom colours palette
        self.custom_boxs = [ ]
        custom_grid = wxGridSizer(2, 8)
        for r, g, b in self.custom_colours:
            new_id = wxNewId()
            custom = pycolourbox.PyColourBox(self, new_id)
            EVT_LEFT_DOWN(custom.GetColourBox(), lambda x, b=custom: self.onCustomClick(x, b))
            custom.SetColourTuple((r, g, b))
            custom_grid.Add(custom, 0, wxEXPAND)
            self.custom_boxs.append(custom)

        csizer = wxBoxSizer(wxVERTICAL)
        csizer.Add(1, 25)
        csizer.Add(self.basic_label, 0, wxEXPAND)
        csizer.Add(1, 5)
        csizer.Add(colour_grid, 0, wxEXPAND)
        csizer.Add(1, 25)
        csizer.Add(self.custom_label, 0, wxEXPAND)
        csizer.Add(1, 5)
        csizer.Add(custom_grid, 0, wxEXPAND)
        csizer.Add(1, 5)
        csizer.Add(self.add_button, 0, wxEXPAND)

        self.palette = pypalette.PyPalette(self, -1)
        EVT_LEFT_DOWN(self.palette, self.onPaletteClick)
        self.colour_slider = pycolourslider.PyColourSlider(self, -1)
        self.slider = wxSlider(self, self.idSCROLL, 86, 0, self.colour_slider.HEIGHT - 1,
                                style=wxSL_VERTICAL, size=wxSize(15, self.colour_slider.HEIGHT))
        EVT_COMMAND_SCROLL(self, self.idSCROLL, self.onScroll)
        psizer = wxBoxSizer(wxHORIZONTAL)
        psizer.Add(self.palette, 0, 0)
        psizer.Add(10, 1)
        psizer.Add(self.colour_slider, 0, wxALIGN_CENTER_VERTICAL)
        psizer.Add(self.slider, 0, wxALIGN_CENTER_VERTICAL)

        self.solid = pycolourbox.PyColourBox(self, -1, size=wxSize(75, 50))
        slabel = wxStaticText(self, -1, _("Solid Colour"))
        ssizer = wxBoxSizer(wxVERTICAL)
        ssizer.Add(self.solid, 0, 0)
        ssizer.Add(1, 2)
        ssizer.Add(slabel, 0, wxALIGN_CENTER_HORIZONTAL)

        rlabel = wxStaticText(self, -1, _("Red:"))
        self.rentry = wxTextCtrl(self, -1)
        self.rentry.SetSize((40, -1))
        glabel = wxStaticText(self, -1, _("Green:"))
        self.gentry = wxTextCtrl(self, -1)
        self.gentry.SetSize((40, -1))
        blabel = wxStaticText(self, -1, _("Blue:"))
        self.bentry = wxTextCtrl(self, -1)
        self.bentry.SetSize((40, -1))
        lgrid = wxFlexGridSizer(3, 2, 2, 2)
        lgrid.AddMany([
            (rlabel, 0, wxALIGN_CENTER_VERTICAL), (self.rentry, 0, 0),
            (glabel, 0, wxALIGN_CENTER_VERTICAL), (self.gentry, 0, 0),
            (blabel, 0, wxALIGN_CENTER_VERTICAL), (self.bentry, 0, 0),
        ])

        gsizer = wxGridSizer(1, 2)
        gsizer.Add(ssizer, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL)
        gsizer.Add(lgrid, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL)

        vsizer = wxBoxSizer(wxVERTICAL)
        vsizer.Add(1, 5)
        vsizer.Add(psizer, 0, 0)
        vsizer.Add(1, 15)
        vsizer.Add(gsizer, 0, wxEXPAND)

        sizer = wxBoxSizer(wxHORIZONTAL)
        sizer.Add(5, 1)
        sizer.Add(csizer, 0, wxEXPAND)
        sizer.Add(10, 1)
        sizer.Add(vsizer, 0, wxEXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)

        self.InitColours()
        self.UpdateColour(self.solid.GetColour())

    def InitColours(self):
        """Initializes the pre-set palette colours."""
        for i in range(len(self.colour_names)):
            colour = wxTheColourDatabase.FindColour(self.colour_names[i])
            self.colour_boxs[i].SetColourTuple((colour.Red(),
                                                 colour.Green(),
                                                 colour.Blue()))

    def onBasicClick(self, event, box):
        """Highlights the selected colour box and updates the solid colour
        display and colour slider to reflect the choice."""
        if hasattr(self, '_old_custom_highlight'):
            self._old_custom_highlight.SetHighlight(False)
        if hasattr(self, '_old_colour_highlight'):
            self._old_colour_highlight.SetHighlight(False)
        box.SetHighlight(True)
        self._old_colour_highlight = box
        self.UpdateColour(box.GetColour())

    def onCustomClick(self, event, box):
        """Highlights the selected custom colour box and updates the solid
        colour display and colour slider to reflect the choice."""
        if hasattr(self, '_old_colour_highlight'):
            self._old_colour_highlight.SetHighlight(False)
        if hasattr(self, '_old_custom_highlight'):
            self._old_custom_highlight.SetHighlight(False)
        box.SetHighlight(True)
        self._old_custom_highlight = box
        self.UpdateColour(box.GetColour())

    def onAddCustom(self, event):
        """Adds a custom colour to the custom colour box set. Boxes are
        chosen in a round-robin fashion, eventually overwriting previously
        added colours."""
        colour = self.solid.GetColour()
        r, g, b = colour.Red(), colour.Green(), colour.Blue()
        self.custom_colours[self.last_custom] = (r, g, b)
        self.custom_boxs[self.last_custom].SetColour(colour)
        self.last_custom = (self.last_custom + 1) % 16

    def UpdateColour(self, colour):
        """Performs necessary updates for when the colour selection has
        changed."""
        self.solid.SetColour(colour)
        self.colour_slider.SetBaseColour(colour)
        self.colour_slider.ReDraw()
        self.slider.SetValue(self.colour_slider.HEIGHT / 2)
        self.UpdateEntries(colour)

    def UpdateEntries(self, colour):
        """Updates the rgb text to display the new colour values."""
        self.rentry.SetValue(str(colour.Red()))
        self.gentry.SetValue(str(colour.Green()))
        self.bentry.SetValue(str(colour.Blue()))

    def onPaletteClick(self, event):
        """Retrieves a value from the palette when clicked and updates
        the selected colour values."""
        colour = self.palette.GetValue(event.m_x, event.m_y)
        self.UpdateColour(colour)

    def onScroll(self, event):
        """Updates the solid colour display to reflect the changing slider."""
        value = self.slider.GetValue()
        colour = self.colour_slider.GetValue(value)
        self.solid.SetColour(colour)
        self.UpdateEntries(colour)

    def SetValue(self, colour):
        """Updates the colour chooser to reflect the given wxColour."""
        self.UpdateColour(colour)

    def GetValue(self):
        """Returns a wxColour object indicating the current colour choice."""
        return self.solid.GetColour()

def main():
    """Simple test display."""
    class App(wxApp):
        def OnInit(self):
            frame = wxFrame(NULL, -1, 'PyColourChooser Test')

            chooser = wxPyColourChooser(frame, -1)
            sizer = wxBoxSizer(wxVERTICAL)
            sizer.Add(chooser, 0, 0)
            frame.SetAutoLayout(True)
            frame.SetSizer(sizer)
            sizer.Fit(frame)

            frame.Show(True)
            self.SetTopWindow(frame)
            return True
    app = App()
    app.MainLoop()

if __name__ == '__main__':
    main()
