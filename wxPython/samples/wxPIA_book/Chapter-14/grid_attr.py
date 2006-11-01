import wx
import wx.grid

class TestFrame(wx.Frame):
    
    
    def __init__(self):
        wx.Frame.__init__(self, None, title="Grid Attributes",
                          size=(600,300))
        grid = wx.grid.Grid(self)
        grid.CreateGrid(10,6)
        for row in range(10):
            for col in range(6):
                grid.SetCellValue(row, col, "(%s,%s)" % (row, col))
         
        grid.SetCellTextColour(1, 1, "red")
        grid.SetCellFont(1,1, wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD))
        grid.SetCellBackgroundColour(2, 2, "light blue")
        
        attr = wx.grid.GridCellAttr()
        attr.SetTextColour("navyblue")
        attr.SetBackgroundColour("pink")
        attr.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD))

        grid.SetAttr(4, 0, attr)
        grid.SetAttr(5, 1, attr)
        grid.SetRowAttr(8, attr)


app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
