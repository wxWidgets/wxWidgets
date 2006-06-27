#
# Note: this module is not a demo per se, but is used by many of 
# the demo modules for various purposes.
#

import wx

#---------------------------------------------------------------------------


class ColoredPanel(wx.Window):
    def __init__(self, parent, color):
        wx.Window.__init__(self, parent, -1, style = wx.SIMPLE_BORDER)
        self.SetBackgroundColour(color)
        if wx.Platform == '__WXGTK__':
            self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)


#---------------------------------------------------------------------------

