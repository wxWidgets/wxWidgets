#----------------------------------------------------------------------------
# Name:         GridColMover.py
# Purpose:      Grid Column Mover Extension
#
# Author:       Gerrit van Dyk (email: gerritvd@decillion.net)
#
# Version       0.1
# Date:         Nov 19, 2002
# RCS-ID:       $Id$
# Licence:      wxWindows license
#----------------------------------------------------------------------------
# 12/07/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 Compatability changes
#
# 12/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxGridColMoveEvent -> GridColMoveEvent
# o wxGridRowMoveEvent -> GridRowMoveEvent
# o wxGridColMover -> GridColMover
# o wxGridRowMover -> GridRowMover
#


import  wx
import  wx.grid

#----------------------------------------------------------------------------
# event class and macros
# 
# New style 12/7/03
#

wxEVT_COMMAND_GRID_COL_MOVE = wx.NewEventType()
wxEVT_COMMAND_GRID_ROW_MOVE = wx.NewEventType()

EVT_GRID_COL_MOVE = wx.PyEventBinder(wxEVT_COMMAND_GRID_COL_MOVE, 1)
EVT_GRID_ROW_MOVE = wx.PyEventBinder(wxEVT_COMMAND_GRID_ROW_MOVE, 1)

#----------------------------------------------------------------------------

class GridColMoveEvent(wx.PyCommandEvent):
    def __init__(self, id, dCol, bCol):
        wx.PyCommandEvent.__init__(self, id = id)
        self.SetEventType(wxEVT_COMMAND_GRID_COL_MOVE)
        self.moveColumn = dCol
        self.beforeColumn = bCol

    def GetMoveColumn(self):
        return self.moveColumn

    def GetBeforeColumn(self):
        return self.beforeColumn


class GridRowMoveEvent(wx.PyCommandEvent):
    def __init__(self, id, dRow, bRow):
        wx.PyCommandEvent.__init__(self,id = id)
        self.SetEventType(wxEVT_COMMAND_GRID_ROW_MOVE)
        self.moveRow = dRow
        self.beforeRow = bRow

    def GetMoveRow(self):
        return self.moveRow

    def GetBeforeRow(self):
        return self.beforeRow


#----------------------------------------------------------------------------
# graft new methods into the wxGrid class

def _ColToRect(self,col):
    if self.GetNumberRows() > 0:
        rect = self.CellToRect(0,col)
    else:
        rect = wxRect()
        rect.height = self.GetColLabelSize()
        rect.width = self.GetColSize(col)

        for cCol in range(0,col):
            rect.x += self.GetColSize(cCol)

    rect.y = self.GetGridColLabelWindow().GetPosition()[1]
    return rect

wx.grid.Grid.ColToRect = _ColToRect


def _RowToRect(self,row):
    if self.GetNumberCols() > 0:
        rect = self.CellToRect(row,0)
    else:
        rect = wxRect()
        rect.width = self.GetRowLabelSize()
        rect.height = self.GetRowSize(row)

        for cRow in range(0,row):
            rect.y += self.GetRowSize(cRow)

    rect.x = self.GetGridRowLabelWindow().GetPosition()[0]
    return rect

wx.grid.Grid.RowToRect = _RowToRect


#----------------------------------------------------------------------------

class ColDragWindow(wx.Window):
    def __init__(self,parent,image,dragCol):
        wx.Window.__init__(self,parent,-1, style=wx.SIMPLE_BORDER)
        self.image = image
        self.SetSize((self.image.GetWidth(),self.image.GetHeight()))
        self.ux = parent.GetScrollPixelsPerUnit()[0]
        self.moveColumn = dragCol

        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def DisplayAt(self,pos,y):
        x = self.GetPositionTuple()[0]
        if x == pos:
            self.Refresh()              # Need to display insertion point
        else:
            self.MoveXY(pos,y)

    def GetMoveColumn(self):
        return self.moveColumn

    def _GetInsertionInfo(self):
        parent = self.GetParent()
        sx = parent.GetViewStart()[0] * self.ux
        sx -= parent._rlSize
        x = self.GetPosition()[0]
        w = self.GetSize()[0]
        sCol = parent.XToCol(x + sx)
        eCol = parent.XToCol(x + w + sx)
        iPos = xPos = xCol = 99999
        centerPos = x + sx + (w / 2)

        for col in range(sCol,eCol + 1):
            cx = parent.ColToRect(col)[0]

            if abs(cx - centerPos) < iPos:
                iPos = abs(cx - centerPos)
                xCol = col
                xPos = cx

        if xCol < 0 or xCol > parent.GetNumberCols():
            xCol = parent.GetNumberCols()

        return (xPos - sx - x,xCol)

    def GetInsertionColumn(self):
        return self._GetInsertionInfo()[1]

    def GetInsertionPos(self):
        return self._GetInsertionInfo()[0]

    def OnPaint(self,evt):
        dc = wx.PaintDC(self)
        w,h = self.GetSize()
        dc.DrawBitmap(self.image, 0,0)
        dc.SetPen(wx.Pen(wx.BLACK,1,wx.SOLID))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle(0,0, w,h)
        iPos = self.GetInsertionPos()
        dc.DrawLine(iPos,h - 10, iPos,h)




class RowDragWindow(wx.Window):
    def __init__(self,parent,image,dragRow):
        wx.Window.__init__(self,parent,-1, style=wx.SIMPLE_BORDER)
        self.image = image
        self.SetSize((self.image.GetWidth(),self.image.GetHeight()))
        self.uy = parent.GetScrollPixelsPerUnit()[1]
        self.moveRow = dragRow

        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def DisplayAt(self,x,pos):
        y = self.GetPosition()[1]
        if y == pos:
            self.Refresh()              # Need to display insertion point
        else:
            self.MoveXY(x,pos)

    def GetMoveRow(self):
        return self.moveRow

    def _GetInsertionInfo(self):
        parent = self.GetParent()
        sy = parent.GetViewStart()[1] * self.uy
        sy -= parent._clSize
        y = self.GetPosition()[1]
        h = self.GetSize()[1]
        sRow = parent.YToRow(y + sy)
        eRow = parent.YToRow(y + h + sy)
        iPos = yPos = yRow = 99999
        centerPos = y + sy + (h / 2)

        for row in range(sRow,eRow + 1):
            cy = parent.RowToRect(row)[1]

            if abs(cy - centerPos) < iPos:
                iPos = abs(cy - centerPos)
                yRow = row
                yPos = cy

        if yRow < 0 or yRow > parent.GetNumberRows():
            yRow = parent.GetNumberRows()

        return (yPos - sy - y,yRow)

    def GetInsertionRow(self):
        return self._GetInsertionInfo()[1]

    def GetInsertionPos(self):
        return self._GetInsertionInfo()[0]

    def OnPaint(self,evt):
        dc = wx.PaintDC(self)
        w,h = self.GetSize()
        dc.DrawBitmap(self.image, 0,0)
        dc.SetPen(wx.Pen(wx.BLACK,1,wx.SOLID))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle(0,0, w,h)
        iPos = self.GetInsertionPos()
        dc.DrawLine(w - 10,iPos, w,iPos)

#----------------------------------------------------------------------------

class GridColMover(wx.EvtHandler):
    def __init__(self,grid):
        wx.EvtHandler.__init__(self)

        self.grid = grid
        self.grid._rlSize = self.grid.GetRowLabelSize()
        self.lwin = grid.GetGridColLabelWindow()
        self.lwin.PushEventHandler(self)
        self.colWin = None
        self.ux = self.grid.GetScrollPixelsPerUnit()[0]
        self.startX = -10
        self.cellX = 0
        self.didMove = False
        self.isDragging = False

        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnPress)
        self.Bind(wx.EVT_LEFT_UP, self.OnRelease)

    def OnMouseMove(self,evt):
        if self.isDragging:
            if abs(self.startX - evt.m_x) >= 3:
                self.didMove = True
                sx,y = self.grid.GetViewStart()
                w,h = self.lwin.GetClientSize()
                x = sx * self.ux

                if (evt.m_x + x) < x:
                    x = evt.m_x + x
                elif evt.m_x > w:
                    x += evt.m_x - w

                if x < 1: x = 0
                else: x /= self.ux

                if x != sx:
                    if wx.Platform == '__WXMSW__':
                        self.colWin.Show(False)

                    self.grid.Scroll(x,y)

                x,y = self.lwin.ClientToScreenXY(evt.m_x,0)
                x,y = self.grid.ScreenToClientXY(x,y)

                if not self.colWin.IsShown():
                    self.colWin.Show(True)

                px = x - self.cellX

                if px < 0 + self.grid._rlSize: px = 0 + self.grid._rlSize

                if px > w - self.colWin.GetSize()[0] + self.grid._rlSize:
                    px = w - self.colWin.GetSize()[0] + self.grid._rlSize

                self.colWin.DisplayAt(px,y)
                return

        evt.Skip()

    def OnPress(self,evt):
        self.startX = evt.m_x
        sx = self.grid.GetViewStart()[0] * self.ux
        sx -= self.grid._rlSize
        px,py = self.lwin.ClientToScreenXY(evt.m_x,evt.m_y)
        px,py = self.grid.ScreenToClientXY(px,py)

        if self.grid.XToEdgeOfCol(px + sx) != wx.NOT_FOUND:
            evt.Skip()
            return

        self.isDragging = True
        self.didMove = False
        col = self.grid.XToCol(px + sx)
        rect = self.grid.ColToRect(col)
        self.cellX = px + sx - rect.x
        size = self.lwin.GetSize()
        rect.y = 0
        rect.x -= sx + self.grid._rlSize
        rect.height = size[1]
        colImg = self._CaptureImage(rect)
        self.colWin = ColDragWindow(self.grid,colImg,col)
        self.colWin.Show(False)
        self.lwin.CaptureMouse()
        evt.Skip()

    def OnRelease(self,evt):
        if self.isDragging:
            self.lwin.ReleaseMouse()
            self.colWin.Show(False)
            self.isDragging = False

            if not self.didMove:
                px = self.lwin.ClientToScreenXY(self.startX,0)[0]
                px = self.grid.ScreenToClientXY(px,0)[0]
                sx = self.grid.GetViewStart()[0] * self.ux
                sx -= self.grid._rlSize
                col = self.grid.XToCol(px+sx)

                if col != wx.NOT_FOUND:
                    self.grid.SelectCol(col,evt.m_controlDown)

                return
            else:
                bCol = self.colWin.GetInsertionColumn()
                dCol = self.colWin.GetMoveColumn()
                wx.PostEvent(self,
                             GridColMoveEvent(self.grid.GetId(), dCol, bCol))

            self.colWin.Destroy()
        evt.Skip()

    def _CaptureImage(self,rect):
        bmp = wx.EmptyBitmap(rect.width,rect.height)
        memdc = wx.MemoryDC()
        memdc.SelectObject(bmp)
        dc = wx.WindowDC(self.lwin)
        memdc.Blit(0,0, rect.width, rect.height, dc, rect.x, rect.y)
        memdc.SelectObject(wx.NullBitmap)
        return bmp


class GridRowMover(wx.EvtHandler):
    def __init__(self,grid):
        wx.EvtHandler.__init__(self)

        self.grid = grid
        self.grid._clSize = self.grid.GetColLabelSize()
        self.lwin = grid.GetGridRowLabelWindow()
        self.lwin.PushEventHandler(self)
        self.rowWin = None
        self.uy = self.grid.GetScrollPixelsPerUnit()[1]
        self.startY = -10
        self.cellY = 0
        self.didMove = False
        self.isDragging = False

        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnPress)
        self.Bind(wx.EVT_LEFT_UP, self.OnRelease)

    def OnMouseMove(self,evt):
        if self.isDragging:
            if abs(self.startY - evt.m_y) >= 3:
                self.didMove = True
                x,sy = self.grid.GetViewStart()
                w,h = self.lwin.GetClientSizeTuple()
                y = sy * self.uy

                if (evt.m_y + y) < y:
                    y = evt.m_y + y
                elif evt.m_y > h:
                    y += evt.m_y - h

                if y < 1: 
                    y = 0
                else: 
                    y /= self.uy

                if y != sy:
                    if wx.Platform == '__WXMSW__':
                        self.rowWin.Show(False)

                    self.grid.Scroll(x,y)

                x,y = self.lwin.ClientToScreenXY(0,evt.m_y)
                x,y = self.grid.ScreenToClientXY(x,y)

                if not self.rowWin.IsShown():
                    self.rowWin.Show(True)

                py = y - self.cellY

                if py < 0 + self.grid._clSize: 
                    py = 0 + self.grid._clSize

                if py > h - self.rowWin.GetSize()[1] + self.grid._clSize:
                    py = h - self.rowWin.GetSize()[1] + self.grid._clSize

                self.rowWin.DisplayAt(x,py)
                return

        evt.Skip()

    def OnPress(self,evt):
        self.startY = evt.m_y
        sy = self.grid.GetViewStart()[1] * self.uy
        sy -= self.grid._clSize
        px,py = self.lwin.ClientToScreenXY(evt.m_x,evt.m_y)
        px,py = self.grid.ScreenToClientXY(px,py)

        if self.grid.YToEdgeOfRow(py + sy) != wx.NOT_FOUND:
            evt.Skip()
            return

        self.isDragging = True
        self.didMove = False
        row = self.grid.YToRow(py + sy)
        rect = self.grid.RowToRect(row)
        self.cellY = py + sy - rect.y
        size = self.lwin.GetSize()
        rect.x = 0
        rect.y -= sy + self.grid._clSize
        rect.width = size[0]
        rowImg = self._CaptureImage(rect)
        self.rowWin = RowDragWindow(self.grid,rowImg,row)
        self.rowWin.Show(False)
        self.lwin.CaptureMouse()
        evt.Skip()

    def OnRelease(self,evt):
        if self.isDragging:
            self.lwin.ReleaseMouse()
            self.rowWin.Show(False)
            self.isDragging = False

            if not self.didMove:
                py = self.lwin.ClientToScreenXY(0,self.startY)[1]
                py = self.grid.ScreenToClientXY(0,py)[1]
                sy = self.grid.GetViewStart()[1] * self.uy
                sy -= self.grid._clSize
                row = self.grid.YToRow(py + sy)

                if row != wx.NOT_FOUND:
                    self.grid.SelectRow(row,evt.m_controlDown)
                return
            else:
                bRow = self.rowWin.GetInsertionRow()
                dRow = self.rowWin.GetMoveRow()

                wx.PostEvent(self,
                             GridRowMoveEvent(self.grid.GetId(), dRow, bRow))

            self.rowWin.Destroy()
        evt.Skip()

    def _CaptureImage(self,rect):
        bmp = wx.EmptyBitmap(rect.width,rect.height)
        memdc = wx.MemoryDC()
        memdc.SelectObject(bmp)
        dc = wx.WindowDC(self.lwin)
        memdc.Blit(0,0, rect.width, rect.height, dc, rect.x, rect.y)
        memdc.SelectObject(wx.NullBitmap)
        return bmp


#----------------------------------------------------------------------------
