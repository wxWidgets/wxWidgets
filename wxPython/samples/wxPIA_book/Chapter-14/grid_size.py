import wx
import wx.grid

class TestFrame(wx.Frame):
    
    
    def __init__(self):
        wx.Frame.__init__(self, None, title="Grid Sizes",
                          size=(600,300))
        grid = wx.grid.Grid(self)
        grid.CreateGrid(5,5)
        for row in range(5):
            for col in range(5):
                grid.SetCellValue(row, col, "(%s,%s)" % (row, col))
         
        grid.SetCellSize(2, 2, 2, 3)        
        grid.SetColSize(1, 125)  
        grid.SetRowSize(1, 100)

app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
