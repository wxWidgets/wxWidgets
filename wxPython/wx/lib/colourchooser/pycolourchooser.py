"""
PyColourChooser
Copyright (C) 2002 Michael Gilfix <mgilfix@eecs.tufts.edu>

This file is part of PyColourChooser.

This version of PyColourChooser is open source; you can redistribute it
and/or modify it under the licensed terms.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxPyColorChooser -> PyColorChooser
# o wxPyColourChooser -> PyColourChooser
# o Added wx.InitAllImageHandlers() to test code since
#   that's where it belongs.
#

import  wx

import  pycolourbox
import  pypalette
import  pycolourslider
import  colorsys
import  intl

from intl import _ # _

class PyColourChooser(wx.Panel):
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

    # Generate the custom colours. These colours are shared across
    # all instances of the colour chooser
    NO_CUSTOM_COLOURS = 16
    custom_colours = [ (wx.Colour(255, 255, 255),
                        pycolourslider.PyColourSlider.HEIGHT / 2)
                     ] * NO_CUSTOM_COLOURS
    last_custom = 0

    idADD_CUSTOM = wx.NewId()
    idSCROLL     = wx.NewId()

    def __init__(self, parent, id):
        """Creates an instance of the colour chooser. Note that it is best to
        accept the given size of the colour chooser as it is currently not
        resizeable."""
        wx.Panel.__init__(self, parent, id)

        self.basic_label = wx.StaticText(self, -1, _("Basic Colours:"))
        self.custom_label = wx.StaticText(self, -1, _("Custom Colours:"))
        self.add_button = wx.Button(self, self.idADD_CUSTOM, _("Add to Custom Colours"))

        self.Bind(wx.EVT_BUTTON, self.onAddCustom, self.add_button)

        # Since we're going to be constructing widgets that require some serious
        # computation, let's process any events (like redraws) right now
        wx.Yield()

        # Create the basic colours palette
        self.colour_boxs = [ ]
        colour_grid = wx.GridSizer(6, 8)
        for name in self.colour_names:
            new_id = wx.NewId()
            box = pycolourbox.PyColourBox(self, new_id)

            box.GetColourBox().Bind(wx.EVT_LEFT_DOWN, lambda x, b=box: self.onBasicClick(x, b))
            
            self.colour_boxs.append(box)
            colour_grid.Add(box, 0, wx.EXPAND)

        # Create the custom colours palette
        self.custom_boxs = [ ]
        custom_grid = wx.GridSizer(2, 8)
        for wxcolour, slidepos in self.custom_colours:
            new_id = wx.NewId()
            custom = pycolourbox.PyColourBox(self, new_id)

            custom.GetColourBox().Bind(wx.EVT_LEFT_DOWN, lambda x, b=custom: self.onCustomClick(x, b))

            custom.SetColour(wxcolour)
            custom_grid.Add(custom, 0, wx.EXPAND)
            self.custom_boxs.append(custom)

        csizer = wx.BoxSizer(wx.VERTICAL)
        csizer.Add((1, 25))
        csizer.Add(self.basic_label, 0, wx.EXPAND)
        csizer.Add((1, 5))
        csizer.Add(colour_grid, 0, wx.EXPAND)
        csizer.Add((1, 25))
        csizer.Add(self.custom_label, 0, wx.EXPAND)
        csizer.Add((1, 5))
        csizer.Add(custom_grid, 0, wx.EXPAND)
        csizer.Add((1, 5))
        csizer.Add(self.add_button, 0, wx.EXPAND)

        self.palette = pypalette.PyPalette(self, -1)
        self.colour_slider = pycolourslider.PyColourSlider(self, -1)
        self.slider = wx.Slider(
                        self, self.idSCROLL, 86, 0, self.colour_slider.HEIGHT - 1,
                        style=wx.SL_VERTICAL, size=(15, self.colour_slider.HEIGHT)
                        )

        self.Bind(wx.EVT_COMMAND_SCROLL, self.onScroll, self.slider)
        psizer = wx.BoxSizer(wx.HORIZONTAL)
        psizer.Add(self.palette, 0, 0)
        psizer.Add((10, 1))
        psizer.Add(self.colour_slider, 0, wx.ALIGN_CENTER_VERTICAL)
        psizer.Add(self.slider, 0, wx.ALIGN_CENTER_VERTICAL)

        # Register mouse events for dragging across the palette
        self.palette.Bind(wx.EVT_LEFT_DOWN, self.onPaletteDown)
        self.palette.Bind(wx.EVT_LEFT_UP, self.onPaletteUp)
        self.palette.Bind(wx.EVT_MOTION, self.onPaletteMotion)
        self.mouse_down = False

        self.solid = pycolourbox.PyColourBox(self, -1, size=(75, 50))
        slabel = wx.StaticText(self, -1, _("Solid Colour"))
        ssizer = wx.BoxSizer(wx.VERTICAL)
        ssizer.Add(self.solid, 0, 0)
        ssizer.Add((1, 2))
        ssizer.Add(slabel, 0, wx.ALIGN_CENTER_HORIZONTAL)

        hlabel = wx.StaticText(self, -1, _("H:"))
        self.hentry = wx.TextCtrl(self, -1)
        self.hentry.SetSize((40, -1))
        slabel = wx.StaticText(self, -1, _("S:"))
        self.sentry = wx.TextCtrl(self, -1)
        self.sentry.SetSize((40, -1))
        vlabel = wx.StaticText(self, -1, _("V:"))
        self.ventry = wx.TextCtrl(self, -1)
        self.ventry.SetSize((40, -1))
        hsvgrid = wx.FlexGridSizer(1, 6, 2, 2)
        hsvgrid.AddMany ([
            (hlabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.hentry, 0, wx.FIXED_MINSIZE),
            (slabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.sentry, 0, wx.FIXED_MINSIZE),
            (vlabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.ventry, 0, wx.FIXED_MINSIZE),
        ])

        rlabel = wx.StaticText(self, -1, _("R:"))
        self.rentry = wx.TextCtrl(self, -1)
        self.rentry.SetSize((40, -1))
        glabel = wx.StaticText(self, -1, _("G:"))
        self.gentry = wx.TextCtrl(self, -1)
        self.gentry.SetSize((40, -1))
        blabel = wx.StaticText(self, -1, _("B:"))
        self.bentry = wx.TextCtrl(self, -1)
        self.bentry.SetSize((40, -1))
        lgrid = wx.FlexGridSizer(1, 6, 2, 2)
        lgrid.AddMany([
            (rlabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.rentry, 0, wx.FIXED_MINSIZE),
            (glabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.gentry, 0, wx.FIXED_MINSIZE),
            (blabel, 0, wx.ALIGN_CENTER_VERTICAL), (self.bentry, 0, wx.FIXED_MINSIZE),
        ])

        gsizer = wx.GridSizer(2, 1)
        gsizer.SetVGap (10)
        gsizer.SetHGap (2)
        gsizer.Add(hsvgrid, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL)
        gsizer.Add(lgrid, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL)

        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        hsizer.Add(ssizer, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL)
        hsizer.Add(gsizer, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL)

        vsizer = wx.BoxSizer(wx.VERTICAL)
        vsizer.Add((1, 5))
        vsizer.Add(psizer, 0, 0)
        vsizer.Add((1, 15))
        vsizer.Add(hsizer, 0, wx.EXPAND)

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add((5, 1))
        sizer.Add(csizer, 0, wx.EXPAND)
        sizer.Add((10, 1))
        sizer.Add(vsizer, 0, wx.EXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)

        self.InitColours()
        self.UpdateColour(self.solid.GetColour())

    def InitColours(self):
        """Initializes the pre-set palette colours."""
        for i in range(len(self.colour_names)):
            colour = wx.TheColourDatabase.FindColour(self.colour_names[i])
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

        # Update the colour panel and then the slider accordingly
        box_index = self.custom_boxs.index(box)
        base_colour, slidepos = self.custom_colours[box_index]
        self.UpdateColour(box.GetColour())
        self.slider.SetValue(slidepos)

    def onAddCustom(self, event):
        """Adds a custom colour to the custom colour box set. Boxes are
        chosen in a round-robin fashion, eventually overwriting previously
        added colours."""
        # Store the colour and slider position so we can restore the
        # custom colours just as they were
        self.setCustomColour(self.last_custom,
                             self.solid.GetColour(),
                             self.colour_slider.GetBaseColour(),
                             self.slider.GetValue())
        self.last_custom = (self.last_custom + 1) % self.NO_CUSTOM_COLOURS

    def setCustomColour (self, index, true_colour, base_colour, slidepos):
        """Sets the custom colour at the given index. true_colour is wxColour
        object containing the actual rgb value of the custom colour.
        base_colour (wxColour) and slidepos (int) are used to configure the
        colour slider and set everything to its original position."""
        self.custom_boxs[index].SetColour(true_colour)
        self.custom_colours[index] = (base_colour, slidepos)

    def UpdateColour(self, colour):
        """Performs necessary updates for when the colour selection has
        changed."""
        # Reset the palette to erase any highlighting
        self.palette.ReDraw()

        # Set the color info
        self.solid.SetColour(colour)
        self.colour_slider.SetBaseColour(colour)
        self.colour_slider.ReDraw()
        self.slider.SetValue(0)
        self.UpdateEntries(colour)

    def UpdateEntries(self, colour):
        """Updates the color levels to display the new values."""
        # Temporary bindings
        r = colour.Red()
        g = colour.Green()
        b = colour.Blue()

        # Update the RGB entries
        self.rentry.SetValue(str(r))
        self.gentry.SetValue(str(g))
        self.bentry.SetValue(str(b))

        # Convert to HSV
        h,s,v = colorsys.rgb_to_hsv(r / 255.0, g / 255.0, b / 255.0)
        self.hentry.SetValue("%.2f" % (h))
        self.sentry.SetValue("%.2f" % (s))
        self.ventry.SetValue("%.2f" % (v))

    def onPaletteDown(self, event):
        """Stores state that the mouse has been pressed and updates
        the selected colour values."""
        self.mouse_down = True
        self.palette.ReDraw()
        self.doPaletteClick(event.m_x, event.m_y)

    def onPaletteUp(self, event):
        """Stores state that the mouse is no longer depressed."""
        self.mouse_down = False

    def onPaletteMotion(self, event):
        """Updates the colour values during mouse motion while the
        mouse button is depressed."""
        if self.mouse_down:
            self.doPaletteClick(event.m_x, event.m_y)

    def doPaletteClick(self, m_x, m_y):
        """Updates the colour values based on the mouse location
        over the palette."""
        # Get the colour value and update
        colour = self.palette.GetValue(m_x, m_y)
        self.UpdateColour(colour)

        # Highlight a fresh selected area
        self.palette.ReDraw()
        self.palette.HighlightPoint(m_x, m_y)

        # Force an onscreen update
        self.solid.Update()
        self.colour_slider.Refresh()

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
    class App(wx.App):
        def OnInit(self):
            frame = wx.Frame(None, -1, 'PyColourChooser Test')

            # Added here because that's where it's supposed to be,
            # not embedded in the library. If it's embedded in the
            # library, debug messages will be generated for duplicate
            # handlers.
            wx.InitAllImageHandlers()

            chooser = PyColourChooser(frame, -1)
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(chooser, 0, 0)
            frame.SetAutoLayout(True)
            frame.SetSizer(sizer)
            sizer.Fit(frame)

            frame.Show(True)
            self.SetTopWindow(frame)
            return True
    app = App(False)
    app.MainLoop()

if __name__ == '__main__':
    main()
