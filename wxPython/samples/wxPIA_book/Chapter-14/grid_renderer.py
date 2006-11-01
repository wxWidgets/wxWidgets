import wx
import wx.grid
import random

class RandomBackgroundRenderer(wx.grid.PyGridCellRenderer):
    def __init__(self):
        wx.grid.PyGridCellRenderer.__init__(self)


    def Draw(self, grid, attr, dc, rect, row, col, isSelected):
        text = grid.GetCellValue(row, col)
        hAlign, vAlign = attr.GetAlignment()
        dc.SetFont( attr.GetFont() )
        if isSelected:
            bg = grid.GetSelectionBackground()
            fg = grid.GetSelectionForeground()
        else:
            bg = random.choice(["pink", "sky blue", "cyan", "yellow", "plum"])
            fg = attr.GetTextColour()

        dc.SetTextBackground(bg)
        dc.SetTextForeground(fg)
        dc.SetBrush(wx.Brush(bg, wx.SOLID))
        dc.SetPen(wx.TRANSPARENT_PEN)
        dc.DrawRectangleRect(rect)           
        grid.DrawTextRectangle(dc, text, rect, hAlign, vAlign)


    def GetBestSize(self, grid, attr, dc, row, col):
        text = grid.GetCellValue(row, col)
        dc.SetFont(attr.GetFont())
        w, h = dc.GetTextExtent(text)
        return wx.Size(w, h)
  
    def Clone(self):
        return RandomBackgroundRenderer()

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Grid Renderer",
                          size=(640,480))

        grid = wx.grid.Grid(self)
        grid.CreateGrid(50,50)

        # Set this custom renderer just for row 4
        attr = wx.grid.GridCellAttr()
        attr.SetRenderer(RandomBackgroundRenderer())
        grid.SetRowAttr(4, attr)
        
        for row in range(10):
            for col in range(10):
                grid.SetCellValue(row, col,
                                  "cell (%d,%d)" % (row, col))

app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
