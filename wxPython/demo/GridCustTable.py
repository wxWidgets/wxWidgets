from wxPython.wx import *
from wxPython.grid import *

import string

#---------------------------------------------------------------------------

class CustomDataTable(wxPyGridTableBase):
    """
    """

    def __init__(self, log):
        wxPyGridTableBase.__init__(self)
        self.log = log

        self.colLabels = ['ID', 'Description', 'Severity', 'Priority', 'Platform',
                          'Opened?', 'Fixed?', 'Tested?']

        self.dataTypes = [wxGRID_VALUE_NUMBER,
                          wxGRID_VALUE_STRING,
                          wxGRID_VALUE_CHOICE + ':only in a million years!,wish list,minor,normal,major,critical',
                          wxGRID_VALUE_NUMBER + ':1,5',
                          wxGRID_VALUE_CHOICE + ':all,MSW,GTK,other',
                          wxGRID_VALUE_BOOL,
                          wxGRID_VALUE_BOOL,
                          wxGRID_VALUE_BOOL]

        self.data = [
            [1010, "The foo doesn't bar", "major", 1, 'MSW', 1, 1, 1],
            [1011, "I've got a wicket in my wocket", "wish list", 2, 'other', 0, 0, 0],
            [1012, "Rectangle() returns a triangle", "critical", 5, 'all', 0, 0, 0]

            ]


    #--------------------------------------------------
    # required methods for the wxPyGridTableBase interface

    def GetNumberRows(self):
        return len(self.data) + 1

    def GetNumberCols(self):
        return len(self.data[0])

    def IsEmptyCell(self, row, col):
        return not self.data[row][col]

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
            msg = wxGridTableMessage(self,                             # The table
                                     wxGRIDTABLE_NOTIFY_ROWS_APPENDED, # what we did to it
                                     1)                                # how many

            self.GetView().ProcessTableMessage(msg)


    #--------------------------------------------------
    # Some optional methods

    # Called when the grid needs to display labels
    def GetColLabelValue(self, col):
        return self.colLabels[col]

    # Called to determine the kind of editor/renderer to use by
    # default, doesn't necessarily have to be the same type used
    # nativly by the editor/renderer if they know how to convert.
    def GetTypeName(self, row, col):
        return self.dataTypes[col]

    # Called to determine how the data can be fetched and stored by the
    # editor and renderer.  This allows you to enforce some type-safety
    # in the grid.
    def CanGetValueAs(self, row, col, typeName):
        colType = string.split(self.dataTypes[col], ':')[0]
        if typeName == colType:
            return true
        else:
            return false

    def CanSetValueAs(self, row, col, typeName):
        return self.CanGetValueAs(row, col, typeName)





#---------------------------------------------------------------------------



class CustTableGrid(wxGrid):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)

        table = CustomDataTable(log)

        # The second parameter means that the grid is to take ownership of the
        # table and will destroy it when done.  Otherwise you would need to keep
        # a reference to it and call it's Destroy method later.
        self.SetTable(table, true)

        self.SetRowLabelSize(0)
        self.SetMargins(0,0)
        self.AutoSizeColumns(false)

        EVT_GRID_CELL_LEFT_DCLICK(self, self.OnLeftDClick)



    # I do this because I don't like the default behaviour of not starting the
    # cell editor on double clicks, but only a second click.
    def OnLeftDClick(self, evt):
        if self.CanEnableCellControl():
            self.EnableCellEditControl()


#---------------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Custom Table, data driven Grid  Demo", size=(640,480))
        grid = CustTableGrid(self, log)



#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(true)
    app.MainLoop()


#---------------------------------------------------------------------------
