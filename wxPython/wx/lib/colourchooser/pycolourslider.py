"""
PyColourChooser
Copyright (C) 2002 Michael Gilfix

This file is part of PyColourChooser.

You should have received a file COPYING containing license terms
along with this program; if not, write to Michael Gilfix
(mgilfix@eecs.tufts.edu) for a copy.

This version of PyColourChooser is open source; you can redistribute it and/or
modify it under the terms listed in the file COPYING.

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
#

import  wx

import  canvas
import  colorsys

class PyColourSlider(canvas.Canvas):
    """A Pure-Python Colour Slider

    The colour slider displays transitions from value 0 to value 1 in
    HSV, allowing the user to select a colour within the transition
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

        canvas.Canvas.__init__(self, parent, id, size=(self.WIDTH, self.HEIGHT))

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
        """Actual implementation of the widget's drawing. We simply draw
        from value 0.0 to value 1.0 in HSV."""
        if self.base_colour is None:
            return

        target_red = self.base_colour.Red()
        target_green = self.base_colour.Green()
        target_blue = self.base_colour.Blue()

        h,s,v = colorsys.rgb_to_hsv(target_red / 255.0, target_green / 255.0,
                                    target_blue / 255.0)
        v = 1.0
        vstep = 1.0 / self.HEIGHT
        for y_pos in range(0, self.HEIGHT):
            r,g,b = [c * 255.0 for c in colorsys.hsv_to_rgb(h,s,v)]
            colour = wx.Colour(int(r), int(g), int(b))
            self.buffer.SetPen(wx.Pen(colour, 1, wx.SOLID))
            self.buffer.DrawRectangle(0, y_pos, 15, 1)
            v = v - vstep
