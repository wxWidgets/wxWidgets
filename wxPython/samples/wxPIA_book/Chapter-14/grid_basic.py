import wx
import wx.grid

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Simple Grid",
                          size=(640,480))
        grid = wx.grid.Grid(self)
        grid.CreateGrid(50,50)
        for row in range(20):
            for col in range(6):
                grid.SetCellValue(row, col,
                                  "cell (%d,%d)" % (row, col))

app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
