
import  wx
import  wx.grid as  gridlib

#---------------------------------------------------------------------------

class HugeTable(gridlib.PyGridTableBase):

    def __init__(self, log):
        gridlib.PyGridTableBase.__init__(self)
        self.log = log

        self.odd=gridlib.GridCellAttr()
        self.odd.SetBackgroundColour("sky blue")
        self.even=gridlib.GridCellAttr()
        self.even.SetBackgroundColour("sea green")

    def GetAttr(self, row, col, kind):
        attr = [self.even, self.odd][row % 2]
        attr.IncRef()
        return attr


    
    # This is all it takes to make a custom data table to plug into a
    # wxGrid.  There are many more methods that can be overridden, but
    # the ones shown below are the required ones.  This table simply
    # provides strings containing the row and column values.
    
    def GetNumberRows(self):
        return 10000

    def GetNumberCols(self):
        return 10000

    def IsEmptyCell(self, row, col):
        return False

    def GetValue(self, row, col):
        return str( (row, col) )

    def SetValue(self, row, col, value):
        self.log.write('SetValue(%d, %d, "%s") ignored.\n' % (row, col, value))


#---------------------------------------------------------------------------



class HugeTableGrid(gridlib.Grid):
    def __init__(self, parent, log):
        gridlib.Grid.__init__(self, parent, -1)

        table = HugeTable(log)

        # The second parameter means that the grid is to take ownership of the
        # table and will destroy it when done.  Otherwise you would need to keep
        # a reference to it and call it's Destroy method later.
        self.SetTable(table, True)

        self.Bind(gridlib.EVT_GRID_CELL_RIGHT_CLICK, self.OnRightDown)  

    def OnRightDown(self, event):
        print "hello"
        print self.GetSelectedRows()


#---------------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, "Huge (virtual) Table Demo", size=(640,480))
        grid = HugeTableGrid(self, log)

        grid.SetReadOnly(5,5, True)

#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wx.PySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(True)
    app.MainLoop()


#---------------------------------------------------------------------------
