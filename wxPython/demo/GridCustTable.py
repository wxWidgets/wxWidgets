
import  wx
import  wx.grid as gridlib

#---------------------------------------------------------------------------

class CustomDataTable(gridlib.PyGridTableBase):
    def __init__(self, log):
        gridlib.PyGridTableBase.__init__(self)
        self.log = log

        self.colLabels = ['ID', 'Description', 'Severity', 'Priority', 'Platform',
                          'Opened?', 'Fixed?', 'Tested?', 'TestFloat']

        self.dataTypes = [gridlib.GRID_VALUE_NUMBER,
                          gridlib.GRID_VALUE_STRING,
                          gridlib.GRID_VALUE_CHOICE + ':only in a million years!,wish list,minor,normal,major,critical',
                          gridlib.GRID_VALUE_NUMBER + ':1,5',
                          gridlib.GRID_VALUE_CHOICE + ':all,MSW,GTK,other',
                          gridlib.GRID_VALUE_BOOL,
                          gridlib.GRID_VALUE_BOOL,
                          gridlib.GRID_VALUE_BOOL,
                          gridlib.GRID_VALUE_FLOAT + ':6,2',
                          ]

        self.data = [
            [1010, "The foo doesn't bar", "major", 1, 'MSW', 1, 1, 1, 1.12],
            [1011, "I've got a wicket in my wocket", "wish list", 2, 'other', 0, 0, 0, 1.50],
            [1012, "Rectangle() returns a triangle", "critical", 5, 'all', 0, 0, 0, 1.56]

            ]


    #--------------------------------------------------
    # required methods for the wxPyGridTableBase interface

    def GetNumberRows(self):
        return len(self.data) + 1

    def GetNumberCols(self):
        return len(self.data[0])

    def IsEmptyCell(self, row, col):
        try:
            return not self.data[row][col]
        except IndexError:
            return True

    # Get/Set values in the table.  The Python version of these
    # methods can handle any data-type, (as long as the Editor and
    # Renderer understands the type too,) not just strings as in the
    # C++ version.
    def GetValue(self, row, col):
        try:
            return self.data[row][col]
        except IndexError:
            return ''

    def SetValue(self, row, col, value):
        try:
            self.data[row][col] = value
        except IndexError:
            # add a new row
            self.data.append([''] * self.GetNumberCols())
            self.SetValue(row, col, value)

            # tell the grid we've added a row
            msg = gridlib.GridTableMessage(self,            # The table
                    gridlib.GRIDTABLE_NOTIFY_ROWS_APPENDED, # what we did to it
                    1                                       # how many
                    )

            self.GetView().ProcessTableMessage(msg)


    #--------------------------------------------------
    # Some optional methods

    # Called when the grid needs to display labels
    def GetColLabelValue(self, col):
        return self.colLabels[col]

    # Called to determine the kind of editor/renderer to use by
    # default, doesn't necessarily have to be the same type used
    # natively by the editor/renderer if they know how to convert.
    def GetTypeName(self, row, col):
        return self.dataTypes[col]

    # Called to determine how the data can be fetched and stored by the
    # editor and renderer.  This allows you to enforce some type-safety
    # in the grid.
    def CanGetValueAs(self, row, col, typeName):
        colType = self.dataTypes[col].split(':')[0]
        if typeName == colType:
            return True
        else:
            return False

    def CanSetValueAs(self, row, col, typeName):
        return self.CanGetValueAs(row, col, typeName)





#---------------------------------------------------------------------------



class CustTableGrid(gridlib.Grid):
    def __init__(self, parent, log):
        gridlib.Grid.__init__(self, parent, -1)

        table = CustomDataTable(log)

        # The second parameter means that the grid is to take ownership of the
        # table and will destroy it when done.  Otherwise you would need to keep
        # a reference to it and call it's Destroy method later.
        self.SetTable(table, True)

        self.SetRowLabelSize(0)
        self.SetMargins(0,0)
        self.AutoSizeColumns(False)

        gridlib.EVT_GRID_CELL_LEFT_DCLICK(self, self.OnLeftDClick)


    # I do this because I don't like the default behaviour of not starting the
    # cell editor on double clicks, but only a second click.
    def OnLeftDClick(self, evt):
        if self.CanEnableCellControl():
            self.EnableCellEditControl()


#---------------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):

        wx.Frame.__init__(
            self, parent, -1, "Custom Table, data driven Grid  Demo", size=(640,480)
            )

        p = wx.Panel(self, -1, style=0)
        grid = CustTableGrid(p, log)
        b = wx.Button(p, -1, "Another Control...")
        b.SetDefault()
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        b.Bind(wx.EVT_SET_FOCUS, self.OnButtonFocus)
        bs = wx.BoxSizer(wx.VERTICAL)
        bs.Add(grid, 1, wx.GROW|wx.ALL, 5)
        bs.Add(b)
        p.SetSizer(bs)

    def OnButton(self, evt):
        print "button selected"

    def OnButtonFocus(self, evt):
        print "button focus"


#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wx.PySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(True)
    app.MainLoop()


#---------------------------------------------------------------------------
