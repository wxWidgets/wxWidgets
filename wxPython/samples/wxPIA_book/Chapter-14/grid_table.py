import wx
import wx.grid

class TestTable(wx.grid.PyGridTableBase):
    def __init__(self):
        wx.grid.PyGridTableBase.__init__(self)
        self.data = { (1,1) : "Here",
                      (2,2) : "is",
                      (3,3) : "some",
                      (4,4) : "data",
                      }
        
        self.odd=wx.grid.GridCellAttr()
        self.odd.SetBackgroundColour("sky blue")
        self.odd.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD))

        self.even=wx.grid.GridCellAttr()
        self.even.SetBackgroundColour("sea green")
        self.even.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD))


    # these five are the required methods
    def GetNumberRows(self):
        return 50

    def GetNumberCols(self):
        return 50

    def IsEmptyCell(self, row, col):
        return self.data.get((row, col)) is not None

    def GetValue(self, row, col):
        value = self.data.get((row, col))
        if value is not None:
            return value
        else:
            return ''

    def SetValue(self, row, col, value):
        self.data[(row,col)] = value


    # the table can also provide the attribute for each cell
    def GetAttr(self, row, col, kind):
        attr = [self.even, self.odd][row % 2]
        attr.IncRef()
        return attr



class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Grid Table",
                          size=(640,480))

        grid = wx.grid.Grid(self)

        table = TestTable()
        grid.SetTable(table, True)
        

app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
