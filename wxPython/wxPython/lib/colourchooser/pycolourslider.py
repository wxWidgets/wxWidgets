"""
wxPyColourChooser
Copyright (C) 2002 Michael Gilfix

This file is part of wxPyColourChooser.

You should have received a file COPYING containing license terms
along with this program; if not, write to Michael Gilfix
(mgilfix@eecs.tufts.edu) for a copy.

This version of wxPyColourChooser is open source; you can redistribute it and/or
modify it under the terms listed in the file COPYING.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

import canvas
from wxPython.wx import *

class PyColourSlider(canvas.Canvas):
    """A Pure-Python Colour Slider

    The colour slider displays transitions between white to target-colour
    to black, allowing the user to select a colour within the transition
    spectrum.

    This class is best accompanying by a wxSlider that allows the user
    to select a particular colour shade.
    """

    HEIGHT = 172
    WIDTH = 12

    def __init__(self, parent, id, colour=None):
        """Creates a blank slider instance. A colour must be set before the
        slider will be filled in."""
        # Set the base colour first since our base class calls the buffer
        # drawing function
        self.SetBaseColour(colour)

        canvas.Canvas.__init__(self, parent, id,
                                size=wxSize(self.WIDTH, self.HEIGHT))

    def SetBaseColour(self, colour):
        """Sets the base, or target colour, to use as the central colour
        when calculating colour transitions."""
        self.base_colour = colour

    def GetBaseColour(self):
        """Return the current colour used as a colour base for filling out
        the slider."""
        return self.base_colour

    def GetValue(self, pos):
        """Returns the colour value for a position on the slider. The position
        must be within the valid height of the slider, or results can be
        unpredictable."""
        return self.buffer.GetPixel(0, pos)

    def DrawBuffer(self):
        """Actual implementation of the widget's drawing. Interpolation
        is calculated between white and the target colour, and black and
        the target colour to draw both halves of the slider."""
        if self.base_colour is None:
            return

        target_red = self.base_colour.Red()
        target_green = self.base_colour.Green()
        target_blue = self.base_colour.Blue()

        half = self.HEIGHT / 2

        wxYield()

        # Interpolate between white and target in middle of slider
        r_step = float(255 - target_red) / float(half)
        g_step = float(255 - target_green) / float(half)
        b_step = float(255 - target_blue) / float(half)
        r, g, b = 255.0, 255.0, 255.0
        for y_pos in range(0, half):
            colour = wxColour(int(r), int(g),(b))
            self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
            self.buffer.DrawRectangle(0, y_pos, 15, 1)
            r, g, b = r - r_step, g - g_step, b - b_step

        wxYield()

        # Interpolate between target and black
        r_step = float(target_red) / float(half)
        g_step = float(target_green) / float(half)
        b_step = float(target_blue) / float(half)
        r, g, b = float(target_red), float(target_green), float(target_blue)
        for y_pos in range(half, self.HEIGHT):
            colour = wxColour(int(r), int(g), int(b))
            self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
            self.buffer.DrawRectangle(0, y_pos, 15, 1)
            r, g, b = r - r_step, g - g_step, b - b_step

        wxYield()

        # Correct the very last pixel from round-off error
        self.buffer.SetPen(wxPen(wxColour(0, 0, 0), 1, wxSOLID))
        self.buffer.DrawRectangle(0, self.HEIGHT - 1, 15, 1)
