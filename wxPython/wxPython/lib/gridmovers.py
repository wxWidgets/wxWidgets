## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.gridmovers

__doc__ =  wx.lib.gridmovers.__doc__

ColDragWindow = wx.lib.gridmovers.ColDragWindow
EVT_GRID_COL_MOVE = wx.lib.gridmovers.EVT_GRID_COL_MOVE
EVT_GRID_ROW_MOVE = wx.lib.gridmovers.EVT_GRID_ROW_MOVE
RowDragWindow = wx.lib.gridmovers.RowDragWindow
_ColToRect = wx.lib.gridmovers._ColToRect
_RowToRect = wx.lib.gridmovers._RowToRect
wxGridColMoveEvent = wx.lib.gridmovers.GridColMoveEvent
wxGridColMover = wx.lib.gridmovers.GridColMover
wxGridRowMoveEvent = wx.lib.gridmovers.GridRowMoveEvent
wxGridRowMover = wx.lib.gridmovers.GridRowMover
