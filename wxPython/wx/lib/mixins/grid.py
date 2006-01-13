#----------------------------------------------------------------------------
# Name:        wx.lib.mixins.grid
# Purpose:     Helpful mix-in classes for wx.Grid
#
# Author:      Robin Dunn
#
# Created:     5-June-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2001 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------
# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Untested
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxGridAutoEditMixin -> GridAutoEditMixin
#

import  wx
import  wx.grid

#----------------------------------------------------------------------------


class GridAutoEditMixin:
    """A mix-in class that automatically enables the grid edit control when
    a cell is selected.

    If your class hooks EVT_GRID_SELECT_CELL be sure to call event.Skip so
    this handler will be called too.
    """

    def __init__(self):
        self.Bind(wx.grid.EVT_GRID_SELECT_CELL, self.__OnSelectCell)


    def __DoEnableEdit(self):
        if self.CanEnableCellControl():
            self.EnableCellEditControl()


    def __OnSelectCell(self, evt):
        wx.CallAfter(self.__DoEnableEdit)
        evt.Skip()

