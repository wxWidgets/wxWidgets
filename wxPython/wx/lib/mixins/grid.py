#----------------------------------------------------------------------------
# Name:        wxPython.lib.mixins.grid
# Purpose:     Helpful mix-in classes for wxGrid
#
# Author:      Robin Dunn
#
# Created:     5-June-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2001 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------

from wxPython import wx, grid

#----------------------------------------------------------------------------


class wxGridAutoEditMixin:
    """A mix-in class that automatically enables the grid edit control when
    a cell is selected.

    If your class hooks EVT_GRID_SELECT_CELL be sure to call event.Skip so
    this handler will be called too.
    """

    def __init__(self):
        self.__enableEdit = 0
        wx.EVT_IDLE(self, self.__OnIdle)
        grid.EVT_GRID_SELECT_CELL(self, self.__OnSelectCell)


    def __OnIdle(self, evt):
        if self.__enableEdit:
            if self.CanEnableCellControl():
                self.EnableCellEditControl()
            self.__enableEdit = 0
        evt.Skip()


    def __OnSelectCell(self, evt):
        self.__enableEdit = 1
        evt.Skip()

