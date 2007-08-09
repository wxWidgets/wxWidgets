import wx
import wx.grid

class TestTable(wx.grid.PyGridTableBase):
    def __init__(self):
        wx.grid.PyGridTableBase.__init__(self)
        self.rowLabels = ["uno", "dos", "tres", "quatro", "cinco"]
        self.colLabels = ["homer", "marge", "bart", "lisa", "maggie"]            
        
    def GetNumberRows(self):
        return 5

    def GetNumberCols(self):
        return 5

    def IsEmptyCell(self, row, col):
        return False

    def GetValue(self, row, col):
        return "(%s,%s)" % (self.rowLabels[row], self.colLabels[col])

    def SetValue(self, row, col, value):
        pass
        
    def GetColLabelValue(self, col):
        return self.colLabels[col]
       
    def GetRowLabelValue(self, row):
        return self.rowLabels[row]

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Grid Table",
                          size=(500,200))
        grid = wx.grid.Grid(self)
        table = TestTable()
        grid.SetTable(table, True)
        
app = wx.PySimpleApp()
frame = TestFrame()
frame.Show()
app.MainLoop()
